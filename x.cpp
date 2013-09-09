#include "x.hpp"

slrn::XEngine* xengine = new slrn::XEngine();

slrn::XEngine::XEngine() {
    m_display = NULL;
    m_visual = NULL;
    m_screen = NULL;
    m_good = false;
    m_mousex = -1;
    m_mousey = -1;
    m_hoverXWindow = None;
}

slrn::XEngine::~XEngine() {
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

void slrn::XEngine::addRect( Rectangle* rect ) {
    m_rects.push_back( rect );
}

void slrn::XEngine::removeRect( Rectangle* rect ) {
    for ( unsigned int i=0; i<m_rects.size(); i++ ) {
        if ( m_rects.at( i ) == rect ) {
            m_rects.erase( m_rects.begin() + i );
            i--;
            delete rect;
            return;
        }
    }
}

bool slrn::XEngine::mouseDown( unsigned int button ) {
    if ( button >= m_mouse.size() ) {
        return false;
    }
    return m_mouse.at( button );
}

int slrn::XEngine::init( std::string display ) {
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

int slrn::XEngine::grabCursor( slrn::CursorType type ) {
    if ( !m_good ) {
        return 1;
    }
    int xfontcursor = getCursor( type );
    int err = XGrabPointer( m_display, m_root, False,
                            PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                            GrabModeAsync, GrabModeAsync, m_root, xfontcursor, CurrentTime );
    if ( err != GrabSuccess ) {
        printf( "Failed to grab X cursor.\n" );
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
    updateHoverWindow( child );
    return 0;
}

int slrn::XEngine::releaseCursor() {
    if ( !m_good ) {
        return 1;
    }
    XUngrabPointer( m_display, CurrentTime );
    return 0;
}

void slrn::XEngine::tick() {
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

Cursor slrn::XEngine::getCursor( slrn::CursorType type ) {
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

void slrn::XEngine::setCursor( slrn::CursorType type ) {
    if ( !m_good ) {
        return;
    }
    Cursor xfontcursor = getCursor( type );
    XChangeActivePointerGrab( m_display,
                              PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                              xfontcursor, CurrentTime );
}

slrn::Rectangle::~Rectangle() {
    //XFreeGC( xengine->m_display, m_gc );
    if ( m_window == None ) {
        return;
    }
    XUnmapWindow( xengine->m_display, m_window );
    XDestroyWindow( xengine->m_display, m_window );
}

slrn::Rectangle::Rectangle( int x, int y, int width, int height, int border, int padding ) {
    m_xoffset = 0;
    m_yoffset = 0;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_border = border;
    m_padding = padding;
    m_window = None;

    constrain( width, height );
    if ( m_border == 0 ) {
        return;
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

    m_window = XCreateWindow( xengine->m_display, xengine->m_root, m_x+m_xoffset, m_y+m_yoffset, m_width+m_border*2, m_height+m_border*2,
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );
    XRectangle rect;
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;

    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    XMapWindow( xengine->m_display, m_window );
}

void slrn::Rectangle::setPos( int x, int y ) {
    if ( m_x == x && m_y == y ) {
        return;
    }
    m_x = x;
    m_y = y;
    if ( m_border == 0 ) {
        return;
    }
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset, m_y+m_yoffset );
}

void slrn::Rectangle::setDim( int w, int h ) {
    if ( m_width == w && m_height == h ) {
        return;
    }

    constrain( w, h );
    if ( m_border == 0 ) {
        return;
    }

    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset, m_y+m_yoffset );
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
}

void slrn::XEngine::updateHoverWindow() {
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

void slrn::XEngine::updateHoverWindow( Window child ) {
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
void slrn::Rectangle::constrain( int w, int h ) {
    int pad = m_padding;
    if ( pad < 0 && std::abs( w ) < std::abs( pad )*2 ) {
        pad = 0;
    }
    if ( w < 0 ) {
        m_flippedx = true;
        m_xoffset = w - pad - m_border;
        m_width = -w + pad*2;
    } else {
        m_flippedx = false;
        m_xoffset = -pad - m_border;
        m_width = w + pad*2;
    }

    pad = m_padding;
    if ( pad < 0 && std::abs( h ) < std::abs( pad )*2 ) {
        pad = 0;
    }
    if ( h < 0 ) {
        m_flippedy = true;
        m_yoffset = h - pad - m_border;
        m_height = -h + pad*2;
    } else {
        m_flippedy = false;
        m_yoffset = -pad - m_border;
        m_height = h + pad*2;
    }
}
