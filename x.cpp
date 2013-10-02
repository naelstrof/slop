#include "x.hpp"

slop::XEngine* xengine = new slop::XEngine();

slop::XEngine::XEngine() {
    m_keypressed = false;
    m_display = NULL;
    m_visual = NULL;
    m_screen = NULL;
    m_good = false;
    m_mousex = -1;
    m_mousey = -1;
    m_hoverXWindow = None;
}

slop::XEngine::~XEngine() {
    if ( !m_good ) {
        return;
    }
    for ( unsigned int i=0; i<m_cursors.size(); i++ ) {
        if ( m_cursors.at( i ) ) {
            XFreeCursor( m_display, m_cursors[i] );
        }
    }
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        delete m_rects.at( i );
    }
    XCloseDisplay( m_display );
}

// We need to keep track of the rectangle windows, so that they don't override our "focus"d windows.
void slop::XEngine::addRect( Rectangle* rect ) {
    m_rects.push_back( rect );
}

void slop::XEngine::removeRect( Rectangle* rect ) {
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        if ( m_rects.at( i ) == rect ) {
            m_rects.erase( m_rects.begin() + i );
            i--;
            delete rect;
            return;
        }
    }
}

bool slop::XEngine::mouseDown( unsigned int button ) {
    if ( button >= m_mouse.size() ) {
        return false;
    }
    return m_mouse.at( button );
}

int slop::XEngine::init( std::string display ) {
    // Initialize display
    m_display = XOpenDisplay( display.c_str() );
    if ( !m_display ) {
        fprintf( stderr, "Error: Failed to open X display %s\n", display.c_str() );
        return 1;
    }
    m_screen    = ScreenOfDisplay( m_display, DefaultScreen( m_display ) );
    m_visual    = DefaultVisual  ( m_display, XScreenNumberOfScreen( m_screen ) );
    m_colormap  = DefaultColormap( m_display, XScreenNumberOfScreen( m_screen ) );
    //m_root      = RootWindow     ( m_display, XScreenNumberOfScreen( m_screen ) );
    m_root      = DefaultRootWindow( m_display );

    m_good = true;
    return 0;
}

int slop::XEngine::grabKeyboard() {
    if ( !m_good ) {
        return 1;
    }
    XGrabKey( m_display, AnyKey, AnyModifier, m_root, False, GrabModeAsync, GrabModeAsync );
    // For whatever we fail to grab the keyboard 100% of the time if slop is launched in the background.
    /*int err = XGrabKeyboard( m_display, m_root, False,
                             GrabModeAsync, GrabModeAsync, CurrentTime );
    if ( err != GrabSuccess ) {
        fprintf( stderr, "Error: Failed to grab X keyboard.\n" );
        fprintf( stderr, "This can be caused by launching slop incorrectly.\n" );
        fprintf( stderr, "gnome-session launches it fine from keyboard binds.\n" );
        return 1;
    }*/
    return 0;
}

int slop::XEngine::releaseKeyboard() {
    if ( !m_good ) {
        return 1;
    }
    XUngrabKeyboard( m_display, CurrentTime );
    return 0;
}

// Grabs the cursor, be wary that setCursor changes the mouse masks.
int slop::XEngine::grabCursor( slop::CursorType type ) {
    if ( !m_good ) {
        return 1;
    }
    int xfontcursor = getCursor( type );
    int err = XGrabPointer( m_display, m_root, False,
                            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                            GrabModeAsync, GrabModeAsync, m_root, xfontcursor, CurrentTime );
    if ( err != GrabSuccess ) {
        fprintf( stderr, "Error: Failed to grab X cursor.\n" );
        fprintf( stderr, "This can be caused by launching slop incorrectly.\n" );
        fprintf( stderr, "gnome-session launches it fine from keyboard binds.\n" );
        return 1;
    }

    // Quickly set the mouse position so we don't have to worry about x11 generating an event.
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    XQueryPointer( m_display, m_root, &root, &child, &mx, &my, &wx, &wy, &mask );
    m_mousex = mx;
    m_mousey = my;

    // Oh and while we're at it, make sure we set the window we're hoving over as well.
    updateHoverWindow( child );
    return 0;
}

int slop::XEngine::releaseCursor() {
    if ( !m_good ) {
        return 1;
    }
    XUngrabPointer( m_display, CurrentTime );
    return 0;
}

void slop::XEngine::tick() {
    if ( !m_good ) {
        return;
    }
    XFlush( m_display );
    XEvent event;
    while ( XPending( m_display ) ) {
        XNextEvent( m_display, &event );
        switch ( event.type ) {
            case MotionNotify: {
                m_mousex = event.xmotion.x;
                m_mousey = event.xmotion.y;
                break;
            }
            case ButtonPress: {
                // Our pitiful mouse manager--
                if ( m_mouse.size() > event.xbutton.button ) {
                    m_mouse.at( event.xbutton.button ) = true;
                } else {
                    m_mouse.resize( event.xbutton.button+2, false );
                    m_mouse.at( event.xbutton.button ) = true;
                }
                break;
            }
            case ButtonRelease: {
                if ( m_mouse.size() > event.xbutton.button ) {
                    m_mouse.at( event.xbutton.button ) = false;
                } else {
                    m_mouse.resize( event.xbutton.button+2, false );
                    m_mouse.at( event.xbutton.button ) = false;
                }
                break;
            }
            // For this particular utility, we only care if a key is pressed.
            // I'm too lazy to implement an actual keyhandler for that.
            case KeyPress: {
                m_keypressed = true;
                break;
            }
            // Since we also don't care if it's released, do nothing! yay
            case KeyRelease: {
                //m_keypressed = false;
                break;
            }
            default: break;
        }
    }

    // Since I couldn't get Xlib to send EnterNotify or LeaveNotify events, we need to query the underlying window every frame.
    updateHoverWindow();
}

// This converts an enum into a preallocated cursor, the cursor will automatically deallocate itself on ~XEngine
Cursor slop::XEngine::getCursor( slop::CursorType type ) {
    int xfontcursor;
    switch ( type ) {
        default:
        case Left:                  xfontcursor = XC_left_ptr; break;
        case Crosshair:             xfontcursor = XC_crosshair; break;
        case Cross:                 xfontcursor = XC_cross; break;
        case UpperLeftCorner:       xfontcursor = XC_ul_angle; break;
        case UpperRightCorner:      xfontcursor = XC_ur_angle; break;
        case LowerLeftCorner:       xfontcursor = XC_ll_angle; break;
        case LowerRightCorner:      xfontcursor = XC_lr_angle; break;
    }
    Cursor newcursor = 0;
    if ( m_cursors.size() > xfontcursor ) {
        newcursor = m_cursors.at( xfontcursor );
    }
    if ( !newcursor ) {
        newcursor = XCreateFontCursor( m_display, xfontcursor );
        m_cursors.resize( xfontcursor+2, 0 );
        m_cursors.at( xfontcursor ) = newcursor;
    }
    return newcursor;
}

// Swaps out the current cursor, bewary that XChangeActivePointerGrab also resets masks, so if you change the mouse masks on grab you need to change them here too.
void slop::XEngine::setCursor( slop::CursorType type ) {
    if ( !m_good ) {
        return;
    }
    Cursor xfontcursor = getCursor( type );
    XChangeActivePointerGrab( m_display,
                              PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                              xfontcursor, CurrentTime );
}

slop::Rectangle::~Rectangle() {
    //XFreeGC( xengine->m_display, m_gc );
    if ( m_window == None ) {
        return;
    }
    //XFreeColors( xengine->m_display, xengine->m_colormap, m_color.pixel, 1,
    // Attempt to move window offscreen before trying to remove it.
    XResizeWindow( xengine->m_display, m_window, 1, 1 );
    XMoveWindow( xengine->m_display, m_window, 0, 0 );
    XUnmapWindow( xengine->m_display, m_window );
    XDestroyWindow( xengine->m_display, m_window );
}

slop::Rectangle::Rectangle( int x, int y, int width, int height, int border, int padding, float r, float g, float b ) {
    m_xoffset = 0;
    m_yoffset = 0;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_border = border;
    m_padding = padding;
    m_window = None;

    // Convert the width, height, x, and y to coordinates that don't have negative values.
    // (also adjust for padding and border size.)
    constrain( width, height );
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }

    // This sets up m_color
    int err = convertColor( r, g, b );
    if ( err ) {
        fprintf( stderr, "Couldn't allocate color of value %f,%f,%f!\n", r, g, b );
    }
    XSetWindowAttributes attributes;
    // Set up the window so it's our color 
    attributes.background_pixmap = None;
    attributes.background_pixel = m_color.pixel;
    // Not actually sure what this does, but it keeps the window from bugging out :u.
    attributes.override_redirect = True;
    // We must use our color map, because that's where our color is allocated.
    attributes.colormap = xengine->m_colormap;
    unsigned long valueMask = CWBackPixmap | CWBackPixel | CWOverrideRedirect | CWColormap;

    // Create the window offset by our generated offsets (see constrain( float, float ))
    m_window = XCreateWindow( xengine->m_display, xengine->m_root, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border, m_width+m_border*2, m_height+m_border*2,
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );

    // Now punch a hole into it so it looks like a selection rectangle!
    XRectangle rect;
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;

    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    XMapWindow( xengine->m_display, m_window );
}

void slop::Rectangle::setPos( int x, int y ) {
    if ( m_x == x && m_y == y ) {
        return;
    }
    m_x = x;
    m_y = y;
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border );
}

void slop::Rectangle::setDim( int w, int h ) {
    if ( m_width == w && m_height == h ) {
        return;
    }

    constrain( w, h );
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }

    // Change the window size and location to our generated offsets (see constrain( float, float ))
    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border );
    // Regenerate our hole
    XRectangle rect;
    rect.x = rect.y = 0;
    rect.width = m_width+m_border*2;
    rect.height = m_height+m_border*2;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSet, 0);
    // Then punch out another.
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
}

void slop::XEngine::updateHoverWindow() {
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    // Query the pointer for the child window, the child window is basically the window we're hovering over.
    XQueryPointer( m_display, m_root, &root, &child, &mx, &my, &wx, &wy, &mask );
    // If we already know that we're hovering over it, do nothing.
    if ( m_hoverXWindow == child ) {
        return;
    }
    // Make sure we can't select one of our selection rectangles, that's just weird.
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        if ( m_rects.at( i )->m_window == child ) {
            return;
        }
    }
    m_hoverXWindow = child;
    if ( child == None ) {
        return;
    }
    // Generate the geometry values so we can use them if needed.
    unsigned int depth;
    XGetGeometry( m_display, child, &root,
                  &(m_hoverWindow.m_x), &(m_hoverWindow.m_y),
                  &(m_hoverWindow.m_width), &(m_hoverWindow.m_height),
                  &(m_hoverWindow.m_border), &depth );
}

void slop::XEngine::updateHoverWindow( Window child ) {
    // Same thing as updateHoverWindow but it uses the specified child.
    // It's used when we first grab the cursor so it's slightly more effecient
    // than calling XQueryPointer twice.
    if ( m_hoverXWindow == child ) {
        return;
    }
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        if ( m_rects.at( i )->m_window == child ) {
            return;
        }
    }
    m_hoverXWindow = child;
    if ( child == None ) {
        return;
    }
    unsigned int depth;
    Window root;
    XGetGeometry( m_display, child, &root,
                  &(m_hoverWindow.m_x), &(m_hoverWindow.m_y),
                  &(m_hoverWindow.m_width), &(m_hoverWindow.m_height),
                  &(m_hoverWindow.m_border), &depth );
}

// Keeps our rectangle's sizes all positive, so Xlib doesn't throw an exception.
// It also keeps our values in absolute coordinates which is nice.
void slop::Rectangle::constrain( int w, int h ) {
    int pad = m_padding;
    if ( pad < 0 && std::abs( w ) < std::abs( pad )*2 ) {
        pad = 0;
    }
    if ( w < 0 ) {
        m_flippedx = true;
        m_xoffset = w - pad;
        m_width = -w + pad*2;
    } else {
        m_flippedx = false;
        m_xoffset = -pad;
        m_width = w + pad*2;
    }

    pad = m_padding;
    if ( pad < 0 && std::abs( h ) < std::abs( pad )*2 ) {
        pad = 0;
    }
    if ( h < 0 ) {
        m_flippedy = true;
        m_yoffset = h - pad;
        m_height = -h + pad*2;
    } else {
        m_flippedy = false;
        m_yoffset = -pad;
        m_height = h + pad*2;
    }
}

int slop::Rectangle::convertColor( float r, float g, float b ) {
    // Convert float colors to shorts.
    short red   = short( floor( r * 65535.f ) );
    short green = short( floor( g * 65535.f ) );
    short blue  = short( floor( b * 65535.f ) );
    XColor color;
    color.red = red;
    color.green = green;
    color.blue = blue;
    // I don't deallocate this anywhere, I think X handles it ???
    int err = XAllocColor( xengine->m_display, xengine->m_colormap, &color );
    if ( err == BadColor ) {
        return err;
    }
    m_color = color;
    return 0;
}
