#include "window.hpp"

using namespace slop;

slop::SlopWindow::SlopWindow() {
    // Load up a opengl context
    static int attributeList[] = { GLX_RENDER_TYPE, GLX_RGBA_BIT,
                                   GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                                   GLX_DOUBLEBUFFER, True,
                                   GLX_RED_SIZE, 1,
                                   GLX_GREEN_SIZE, 1,
                                   GLX_BLUE_SIZE, 1,
                                   GLX_ALPHA_SIZE, 1,
                                   None };
    int nelements;
    int render_event_base, render_error_base;
    if(!XRenderQueryExtension(x11->display, &render_event_base, &render_error_base)) {
        throw std::runtime_error("No XRENDER extension found\n");
    }

    GLXFBConfig* fbc = glXChooseFBConfig(x11->display, DefaultScreen(x11->display), attributeList, &nelements);
    GLXFBConfig fbconfig;
    if ( !fbc ) {
        throw std::runtime_error("No matching visuals available.\n");
    }
    XVisualInfo* vi ;
    XRenderPictFormat *pictFormat;
    int i;
    for (i=0; i<nelements; i++) {
        vi = glXGetVisualFromFBConfig(x11->display, fbc[i]);
        if (!vi) { continue; }
        pictFormat = XRenderFindVisualFormat(x11->display, vi->visual);
        if (!pictFormat) {
          XFree( vi );
          continue;
        }
        if(pictFormat->direct.alphaMask > 0) {
            fbconfig = fbc[i];
            break;
        } else { 
          XFree( vi );
        }
    }
    if (i == nelements ) {
        throw std::runtime_error( "No matching visuals available" );
    }
    XFree( fbc );

    XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap(x11->display, RootWindow(x11->display, vi->screen), vi->visual, AllocNone);
    attributes.background_pixmap = None;
    attributes.border_pixel = 0;
    // Disable window decorations.
    attributes.override_redirect = True;
    // Make sure we know when we've been successfully destroyed later!
    attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWOverrideRedirect | CWColormap | CWBackPixmap | CWBorderPixel | CWEventMask;


    // Create the window
    window = XCreateWindow( x11->display, x11->root, 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ),
                            0, vi->depth, InputOutput,
                            vi->visual, valueMask, &attributes );
    XFree( vi );

    if ( !window ) {
        throw std::runtime_error( "Couldn't create a GL window!" );
    }

    // Prep some hints for the window
    static char title[] = "slop";
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
    sizehints.width = WidthOfScreen( x11->screen );
    sizehints.height = HeightOfScreen( x11->screen );
    sizehints.flags = USPosition | USSize;
    XClassHint classhints;
    char name[] = "slop";
    classhints.res_name = name;
    classhints.res_class = name;
    // Finally send it all over...
    XSetClassHint( x11->display, window, &classhints );
    XSetWMProperties( x11->display, window, &textprop, &textprop, NULL, 0, &sizehints, startup_state, NULL );
    XFree( startup_state );
    // Keep the window on top of all other windows.
    Atom stateAbove = XInternAtom(x11->display, "_NET_WM_STATE_ABOVE", False);
    XChangeProperty(x11->display, window, XInternAtom(x11->display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) &stateAbove, 1);

    context = glXCreateNewContext( x11->display, fbconfig, GLX_RGBA_TYPE, 0, True );
    if ( !context ) {
        throw std::runtime_error( "Failed to create an OpenGL context." );
    }
    setCurrent();
    // Finally we grab some OpenGL 3.3 stuffs.
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      throw std::runtime_error((char*)glewGetErrorString(err));
    }
    framebuffer = new Framebuffer( WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) );

    glViewport( 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) );
    camera = glm::ortho( 0.0f, (float)WidthOfScreen( x11->screen ), (float)HeightOfScreen( x11->screen ), 0.0f, -1.0f, 1.0f);

    // Last, we actually display the window <:o)
    XMapWindow( x11->display, window );
}

slop::SlopWindow::~SlopWindow() {
    delete framebuffer;
    // Try to erase the window before destroying it.
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    display();
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    display();
    glXDestroyContext( x11->display, context );
    XUnmapWindow( x11->display, window );
    XDestroyWindow( x11->display, window );
    glXMakeCurrent( x11->display, None, NULL );
}

void slop::SlopWindow::display() {
    glXSwapBuffers( x11->display, window );
    glXWaitGL();
}

void slop::SlopWindow::setCurrent() {
    glXMakeCurrent( x11->display, window, context );
}
