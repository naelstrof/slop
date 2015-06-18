/* glrectangle.hpp: Handles creating hardware accelerated rectangles on the screen using X11 and OpenGL.
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
#include "glselectrectangle.hpp"

static Bool isDestroyNotify( Display* dpy, XEvent* ev, XPointer win ) {
    return ev->type == DestroyNotify && ev->xdestroywindow.window == *((Window*)win);
}

slop::GLSelectRectangle::~GLSelectRectangle() {
    if ( m_window == None ) {
        return;
    }
    // Try to erase the window before destroying it.
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glXSwapBuffers( xengine->m_display, m_glxWindow );
    // Sleep for 0.1 seconds in hope that the rectangle was erased.
    usleep( 10000 );
    XDestroyWindow( xengine->m_display, m_window );
    XEvent event;
    // Block until the window is actually completely removed.
    XIfEvent( xengine->m_display, &event, &isDestroyNotify, (XPointer)&m_window );
    // Sleep for 0.1 seconds in hope that the screen actually cleared the window.
    usleep( 10000 );
}

slop::GLSelectRectangle::GLSelectRectangle( int sx, int sy, int ex, int ey, int border, bool highlight, float r, float g, float b, float a ) {
    m_x = std::min( sx, ex );
    m_y = std::min( sy, ey );
    m_width = std::max( sx, ex ) - m_x;
    m_height = std::max( sy, ey ) - m_y;
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
    m_border = border;
    m_window = None;
    m_highlight = highlight;

    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }

    if ( m_highlight ) {
        m_border = 0;
    }

    static int visdata[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 16,
        None
    };

    int numfbconfigs = 0;
    GLXFBConfig* fbconfigs = glXChooseFBConfig( xengine->m_display,  DefaultScreen( xengine->m_display ), visdata, &numfbconfigs );
    m_fbconfig = 0;
    for ( int i=0; i<numfbconfigs; i++ ) {
        m_visual = (XVisualInfo*)glXGetVisualFromFBConfig( xengine->m_display, fbconfigs[i] );
        if ( !m_visual ) {
            continue;
        }
        m_pictFormat = XRenderFindVisualFormat( xengine->m_display, m_visual->visual );
        if ( !m_pictFormat ) {
            continue;
        }
        m_fbconfig = fbconfigs[i];
        if ( m_pictFormat->direct.alphaMask > 0 ) {
            break;
        }
    }

    if ( !m_fbconfig ) {
        fprintf( stderr, "Couldn't find a matching FB config for a transparent OpenGL window!\n");
    }

    m_cmap = XCreateColormap( xengine->m_display, xengine->m_root, m_visual->visual, AllocNone );

    XSetWindowAttributes attributes;
    attributes.colormap = m_cmap;
    attributes.background_pixmap = None;
    attributes.border_pixmap = None;
    attributes.border_pixel = 0;
    // Disable window decorations.
    attributes.override_redirect = True;
    // Make sure we know when we've been successfully destroyed later!
    attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWOverrideRedirect | CWEventMask | CWBackPixmap | CWColormap | CWBorderPixel;


    // Create the window
    m_window = XCreateWindow( xengine->m_display, xengine->m_root, 0, 0, xengine->getWidth(), xengine->getHeight(),
                              0, m_visual->depth, InputOutput,
                              m_visual->visual, valueMask, &attributes );

    if ( !m_window ) {
        fprintf( stderr, "Couldn't create a GL window!\n");
    }

    m_glxWindow = m_window;

    static char title[] = "OpenGL Slop";
    XWMHints* startup_state = XAllocWMHints();
    startup_state->initial_state = NormalState;
    startup_state->flags = StateHint;
    XTextProperty textprop;
    textprop.value = (unsigned char*)title;
    textprop.encoding = XA_STRING;
    textprop.format = 8;
    textprop.nitems = strlen( title );
    XSizeHints sizehints;
    sizehints.x = 0;
    sizehints.y = 0;
    sizehints.width = xengine->getWidth();
    sizehints.height = xengine->getHeight();
    sizehints.flags = USPosition | USSize;
    XClassHint classhints;
    char name[] = "slop";
    classhints.res_name = name;
    classhints.res_class = name;
    XSetClassHint( xengine->m_display, m_window, &classhints );
    XSetWMProperties( xengine->m_display, m_window, &textprop, &textprop, NULL, 0, &sizehints, startup_state, NULL );
    XFree( startup_state );

    // Make it so all input falls through
    XRectangle rect;
    rect.x = rect.y = rect.width = rect.height = 0;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeInput, 0, 0, &rect, 1, ShapeSet, 0);

    XMapWindow( xengine->m_display, m_window );

    int dummy;
    if ( !glXQueryExtension( xengine->m_display, &dummy, &dummy ) ) {
        fprintf( stderr, "OpenGL is not supported!\n" );
    }
    m_renderContext = glXCreateNewContext( xengine->m_display, m_fbconfig, GLX_RGBA_TYPE, 0, True );
    if ( !m_renderContext ) {
        fprintf( stderr, "Failed to create a GL context.\n" );
    }
    if ( !glXMakeContextCurrent( xengine->m_display, m_glxWindow, m_glxWindow, m_renderContext ) ) {
        fprintf( stderr, "Failed to attach GL context to window!\n" );
    }
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glXSwapBuffers( xengine->m_display, m_glxWindow );
}

void slop::GLSelectRectangle::setGeo( int sx, int sy, int ex, int ey ) {
    int x = std::min( sx, ex );
    int y = std::min( sy, ey );
    int w = std::max( sx, ex ) - x;
    int h = std::max( sy, ey ) - y;

    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
    glDrawBuffer( GL_BACK );
    glViewport( 0, 0, xengine->getWidth(), xengine->getHeight() );

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, xengine->getWidth(), xengine->getHeight(), 0, 1, -1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f( m_r, m_g, m_b, m_a );
    glRecti( m_x-m_border, m_y, m_x+m_width+m_border, m_y-m_border );
    glRecti( m_x-m_border, m_y+m_height, m_x+m_width+m_border, m_y+m_height+m_border );
    glRecti( m_x-m_border, m_y, m_x, m_y+m_height );
    glRecti( m_x+m_width, m_y, m_x+m_width+m_border, m_y+m_height );

    glXSwapBuffers( xengine->m_display, m_glxWindow );
}
