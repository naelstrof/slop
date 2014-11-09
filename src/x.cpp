/* x.cpp: Handles starting and managing X.
 *
 * Copyright (C) 2014: Dalton Nell, Slop Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Slop.
 *
 * Slop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Slop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Slop.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "x.hpp"

slop::XEngine* xengine = new slop::XEngine();

int slop::XEngineErrorHandler( Display* dpy, XErrorEvent* event ) {
    // Ignore XGrabKeyboard BadAccess errors, we can work without it.
    // 31 = XGrabKeyboard's request code
    if ( event->request_code == 31 && event->error_code == BadAccess ) {
        fprintf( stderr, "_X Error \"BadAccess\" for XGrabKeyboard ignored...\n" );
        return EXIT_SUCCESS;
    }
    // Everything else should be fatal as I don't like undefined behavior.
    char buffer[1024];
    XGetErrorText( dpy, event->error_code, buffer, 1024 );
    fprintf( stderr,
             "_X Error of failed request: %s\n_  Error code of failed request: %3d\n_  Major opcode of failed request: %3d\n_  Minor opcode of failed request: %3d\n_  Serial number of failed request: %5li\n_  Current serial number in output stream:?????\n",
             buffer,
             event->error_code,
             event->request_code,
             event->minor_code,
             event->serial );
    exit(1);
}

unsigned int slop::XEngine::getWidth() {
    if ( !m_good ) {
        return -1;
    }
    return WidthOfScreen( m_screen );
}

unsigned int slop::XEngine::getHeight() {
    if ( !m_good ) {
        return -1;
    }
    return HeightOfScreen( m_screen );
}

slop::XEngine::XEngine() {
    m_keypressed = false;
    m_display = NULL;
    m_visual = NULL;
    m_screen = NULL;
    m_good = false;
    m_mousex = -1;
    m_mousey = -1;
    m_hoverWindow = None;
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
    XCloseDisplay( m_display );
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
        return EXIT_FAILURE;
    }
    m_screen    = ScreenOfDisplay( m_display, DefaultScreen( m_display ) );
    m_visual    = DefaultVisual  ( m_display, XScreenNumberOfScreen( m_screen ) );
    m_colormap  = DefaultColormap( m_display, XScreenNumberOfScreen( m_screen ) );
    //m_root      = RootWindow     ( m_display, XScreenNumberOfScreen( m_screen ) );
    m_root      = DefaultRootWindow( m_display );

    m_good = true;
    XSetErrorHandler( slop::XEngineErrorHandler );
    selectAllInputs( m_root, EnterWindowMask );
    return EXIT_SUCCESS;
}

bool slop::XEngine::keyPressed( KeySym key ) {
    KeyCode keycode = XKeysymToKeycode( m_display, key );
    if ( keycode != 0 ) {
        // Get the whole keyboard state
        char keys[32];
        XQueryKeymap( m_display, keys );
        // Check our keycode
        return ( keys[ keycode / 8 ] & ( 1 << ( keycode % 8 ) ) ) != 0;
    } else {
        return false;
    }
}

bool slop::XEngine::anyKeyPressed() {
    if ( !m_good ) {
        return false;
    }
    // Thanks to SFML for some reliable key state grabbing.
    // Get the whole keyboard state
    char keys[ 32 ];
    XQueryKeymap( m_display, keys );
    // Each bit indicates a different key, 1 for pressed, 0 otherwise.
    // Every bit should be 0 if nothing is pressed.
    for ( unsigned int i = 0; i < 32; i++ ) {
        if ( keys[ i ] != 0 ) {
            return true;
        }
    }
    return false;
}

int slop::XEngine::grabKeyboard() {
    if ( !m_good ) {
        return EXIT_FAILURE;
    }
    int err = XGrabKeyboard( m_display, m_root, False, GrabModeAsync, GrabModeAsync, CurrentTime );
    if ( err != GrabSuccess ) {
        fprintf( stderr, "Warning: Failed to grab X keyboard.\n" );
        fprintf( stderr, "         This happens when something has already grabbed your keybaord.\n" );
        fprintf( stderr, "         slop should still run properly though.\n" );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int slop::XEngine::releaseKeyboard() {
    if ( !m_good ) {
        return EXIT_FAILURE;
    }
    XUngrabKeyboard( m_display, CurrentTime );
    return EXIT_SUCCESS;
}

void slop::XEngine::selectAllInputs( Window win, long event_mask) {
    Window root, parent;
    Window* children;
    unsigned int nchildren;
    XQueryTree( m_display, win, &root, &parent, &children, &nchildren );
    for ( unsigned int i=0;i<nchildren;i++ ) {
        XSelectInput( m_display, children[ i ], event_mask );
        selectAllInputs( children[ i ], event_mask );
    }
    free( children );
}

// Grabs the cursor, be wary that setCursor changes the mouse masks.
int slop::XEngine::grabCursor( slop::CursorType type ) {
    if ( !m_good ) {
        return EXIT_FAILURE;
    }
    int xfontcursor = getCursor( type );
    int err = XGrabPointer( m_display, m_root, True,
                            PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask,
                            GrabModeAsync, GrabModeAsync, None, xfontcursor, CurrentTime );
    if ( err != GrabSuccess ) {
        fprintf( stderr, "Error: Failed to grab X cursor.\n" );
        fprintf( stderr, "       This can be caused by launching slop weirdly.\n" );
        return EXIT_FAILURE;
    }
    // Quickly set the mouse position so we don't have to worry about x11 generating an event.
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    XQueryPointer( m_display, m_root, &root, &child, &mx, &my, &wx, &wy, &mask );
    m_mousex = mx;
    m_mousey = my;

    // Get the deepest available window.
    Window test = child;
    while( test ) {
        child = test;
        XQueryPointer( m_display, child, &root, &test, &mx, &my, &wx, &wy, &mask );
    }
    m_hoverWindow = child;
    return EXIT_SUCCESS;
}

int slop::XEngine::releaseCursor() {
    if ( !m_good ) {
        return EXIT_FAILURE;
    }
    XUngrabPointer( m_display, CurrentTime );
    return EXIT_SUCCESS;
}

void slop::XEngine::tick() {
    if ( !m_good ) {
        return;
    }
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
            case EnterNotify: {
                if ( event.xcrossing.subwindow != None ) {
                    m_hoverWindow = event.xcrossing.subwindow;
                } else {
                    m_hoverWindow = event.xcrossing.window;
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
        case Dot:                   xfontcursor = XC_dot; break;
        case Box:                   xfontcursor = 40; break;
    }
    Cursor newcursor = 0;
    if ( m_cursors.size() > (unsigned int)xfontcursor ) {
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

void slop::WindowRectangle::applyPadding( int padding ) {
    if ( (int)m_width + padding*2 >= 0 ) {
        m_x -= padding;
        m_width += padding*2;
    }
    if ( (int)m_height + padding*2 >= 0 ) {
        m_y -= padding;
        m_height += padding*2;
    }
}

Window slop::WindowRectangle::getWindow() {
    return m_window;
}

void slop::WindowRectangle::applyMinMaxSize( unsigned int minimumsize, unsigned int maximumsize ) {
    if ( minimumsize > maximumsize && maximumsize > 0 ) {
        fprintf( stderr, "Error: minimumsize is greater than maximumsize.\n" );
        exit( 1 );
    }
    if ( m_width < minimumsize ) {
        int diff = minimumsize - m_width;
        m_width = minimumsize;
        m_x -= diff/2;
    }
    if ( m_height < minimumsize ) {
        int diff = minimumsize - m_height;
        m_height = minimumsize;
        m_y -= diff/2;
    }
    if ( maximumsize > 0 ) {
        if ( m_width > maximumsize ) {
            int diff = m_width;
            m_width = maximumsize;
            // Center in the center of the window
            m_x += diff/2 - maximumsize/2;
        }
        if ( m_height > maximumsize ) {
            int diff = m_height;
            m_height = maximumsize;
            // Center in the center of the window
            m_y += diff/2 - maximumsize/2;
        }
    }
}

void slop::WindowRectangle::setGeometry( Window win, bool decorations ) {
    if ( decorations ) {
        Window root, parent, test, junk;
        Window* childlist;
        unsigned int ujunk;
        // Try to find the actual decorations.
        test = win;
        int status = XQueryTree( xengine->m_display, test, &root, &parent, &childlist, &ujunk);
        free( childlist );
        while( parent != root ) {
            if ( !parent || !status ) {
                break;
            }
            test = parent;
            status = XQueryTree( xengine->m_display, test, &root, &parent, &childlist, &ujunk);
            free( childlist );
        }
        // test contains the window we're screenshotting.
        m_window = test;
        // Once found, proceed normally.
        if ( test && parent == root && status ) {
            XWindowAttributes attr;
            XGetWindowAttributes( xengine->m_display, test, &attr );
            m_width = attr.width;
            m_height = attr.height;
            m_border = attr.border_width;
            XTranslateCoordinates( xengine->m_display, test, attr.root, -attr.border_width, -attr.border_width, &(m_x), &(m_y), &junk );
            // We make sure we include borders, since we want decorations.
            m_width += m_border * 2;
            m_height += m_border * 2;
        }
        return;
    }
    Window junk;
    // Now here we should be able to just use whatever we get.
    XWindowAttributes attr;
    // We use XGetWindowAttributes to know our root window.
    XGetWindowAttributes( xengine->m_display, win, &attr );
    //m_x = attr.x;
    //m_y = attr.y;
    m_width = attr.width;
    m_height = attr.height;
    m_border = attr.border_width;
    XTranslateCoordinates( xengine->m_display, win, attr.root, -attr.border_width, -attr.border_width, &(m_x), &(m_y), &junk );
    m_window = win;
}
