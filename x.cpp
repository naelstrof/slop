#include "x.hpp"

is::XEngine* xengine = new is::XEngine();

is::XEngine::XEngine() {
    m_display = NULL;
    m_visual = NULL;
    m_screen = NULL;
    m_good = false;
    m_mousex = -1;
    m_mousey = -1;
    m_hoverXWindow = None;
}

is::XEngine::~XEngine() {
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

void is::XEngine::addRect( Rectangle* rect ) {
    m_rects.push_back( rect );
}

void is::XEngine::removeRect( Rectangle* rect ) {
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        if ( m_rects.at( i ) == rect ) {
            m_rects.erase( m_rects.begin() + i );
            i--;
            delete rect;
            return;
        }
    }
}

bool is::XEngine::mouseDown( unsigned int button ) {
    if ( button >= m_mouse.size() ) {
        return false;
    }
    return m_mouse.at( button );
}

int is::XEngine::init( std::string display ) {
    // Initialize display
    m_display = XOpenDisplay( display.c_str() );
    if ( !m_display ) {
        printf( "Failed to open X display %s\n", display.c_str() );
        return 1;
    }
    m_screen    = ScreenOfDisplay( m_display, DefaultScreen( m_display ) );
    m_visual    = DefaultVisual  ( m_display, XScreenNumberOfScreen( m_screen ) );
    m_colormap  = DefaultColormap( m_display, XScreenNumberOfScreen( m_screen ) );
    m_root      = RootWindow     ( m_display, XScreenNumberOfScreen( m_screen ) );

    m_good = true;
    return 0;
}

int is::XEngine::grabCursor( is::CursorType type ) {
    if ( !m_good ) {
        return 1;
    }
    int xfontcursor = getCursor( type );
    int err = XGrabPointer( m_display, m_root, False,
                            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                            GrabModeAsync, GrabModeAsync, m_root, xfontcursor, CurrentTime );
    if ( err != GrabSuccess ) {
        printf( "Failed to grab X cursor\n" );
        return 1;
    }

    // Quickly set the mouse position so we don't have to worry about x11 generating an event.
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    XQueryPointer( m_display, m_root, &root, &child, &mx, &my, &wx, &wy, &mask );
    m_mousex = mx;
    m_mousex = my;
    updateHoverWindow( child );
    return 0;
}

int is::XEngine::releaseCursor() {
    if ( !m_good ) {
        return 1;
    }
    XUngrabPointer( m_display, CurrentTime );
    return 0;
}

void is::XEngine::tick() {
    if ( !m_good ) {
        return;
    }
    XSync( m_display, false );
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
            default: break;
        }
    }

    // Since I couldn't get Xlib to send a EnterNotify or LeaveNotify events, we need to query the underlying window every frame.
    updateHoverWindow();
}

Cursor is::XEngine::getCursor( is::CursorType type ) {
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

void is::XEngine::setCursor( is::CursorType type ) {
    if ( !m_good ) {
        return;
    }
    Cursor xfontcursor = getCursor( type );
    XChangeActivePointerGrab( m_display,
                              PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                              xfontcursor, CurrentTime );
}

is::Rectangle::~Rectangle() {
    //XFreeGC( xengine->m_display, m_gc );
    XUnmapWindow( xengine->m_display, m_window );
    XDestroyWindow( xengine->m_display, m_window );
}

is::Rectangle::Rectangle( int x, int y, int width, int height, int border, int padding ) {
    m_xoffset = 0;
    m_yoffset = 0;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_border = border;
    m_padding = padding;

    if ( m_width < 0 ) {
        m_xoffset += m_width;
        m_width = -m_width;
    }
    if ( m_height < 0 ) {
        m_yoffset += m_height;
        m_height = -m_height;
    }

    XAllocNamedColor( xengine->m_display, xengine->m_colormap, "black", &m_forground, &m_forgroundExact );
    XAllocNamedColor( xengine->m_display, xengine->m_colormap, "white", &m_background, &m_backgroundExact );
    XSetWindowAttributes attributes;
    attributes.background_pixmap = None;
    attributes.background_pixel = m_forground.pixel;
    attributes.save_under = True;
    attributes.override_redirect = True;
    attributes.colormap = xengine->m_colormap;
    unsigned long valueMask = CWBackPixmap | CWBackPixel |
                              CWSaveUnder | CWOverrideRedirect |
                              CWColormap;

    m_window = XCreateWindow( xengine->m_display, xengine->m_root, m_x-m_border+m_xoffset, m_y-m_border+m_yoffset, m_width+m_border*2, m_height+m_border*2,
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );
    XRectangle rect;
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;

    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    XMapWindow( xengine->m_display, m_window );

    draw();
}

void is::Rectangle::setPos( int x, int y ) {
    if ( m_x == x && m_y == y ) {
        return;
    }
    m_x = x;
    m_y = y;
    XMoveWindow( xengine->m_display, m_window, m_x-m_border+m_xoffset, m_y-m_border+m_yoffset );
}

void is::Rectangle::setDim( int w, int h ) {
    if ( m_width == w && m_height == h ) {
        return;
    }

    m_xoffset = 0;
    m_yoffset = 0;
    m_width = w;
    m_height = h;
    if ( w < 0 ) {
        m_xoffset += w;
        m_width = -w;
    }
    if ( h < 0 ) {
        m_yoffset += h;
        m_height = -h;
    }
    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    XMoveWindow( xengine->m_display, m_window, m_x-m_border+m_xoffset, m_y-m_border+m_yoffset );
    // Now punch another hole in it.
    XRectangle rect;
    rect.x = rect.y = 0;
    rect.width = m_width+m_border*2;
    rect.height = m_height+m_border*2;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSet, 0);
    rect;
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    draw();
}

void is::Rectangle::draw() {
}

void is::XEngine::updateHoverWindow() {
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    XQueryPointer( m_display, m_root, &root, &child, &mx, &my, &wx, &wy, &mask );
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
    XGetGeometry( m_display, child, &root,
                  &(m_hoverWindow.m_x), &(m_hoverWindow.m_y),
                  &(m_hoverWindow.m_width), &(m_hoverWindow.m_height),
                  &(m_hoverWindow.m_border), &depth );
}

void is::XEngine::updateHoverWindow( Window child ) {
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
