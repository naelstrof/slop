#include "window.hpp"

SlopWindow::SlopWindow() {
    XVisualInfo visual;
    XMatchVisualInfo(x11->display, DefaultScreen(x11->display), 32, TrueColor, &visual);

    XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap( x11->display, x11->root, visual.visual, AllocNone );
    attributes.background_pixmap = None;
    attributes.border_pixel = 0;
    // Disable window decorations.
    attributes.override_redirect = True;
    // Make sure we know when we've been successfully destroyed later!
    //attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWOverrideRedirect | CWColormap | CWBackPixmap | CWBorderPixel;


    // Create the window
    window = XCreateWindow( x11->display, x11->root, 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ),
                            0, visual.depth, InputOutput,
                            visual.visual, valueMask, &attributes );

    if ( !window ) {
        throw new std::runtime_error( "Couldn't create a GL window!" );
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

    // Load up a opengl context
    context = glXCreateContext( x11->display, &visual, 0, True );
    if ( !context ) {
		throw new std::runtime_error( "Failed to create an OpenGL context." );
    }
    setCurrent();
    // Finally we grab some OpenGL 3.3 stuffs.
    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        throw new std::runtime_error("Failed to load function pointers for OpenGL.");
    }
    framebuffer = new Framebuffer( WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) );

    glViewport( 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) );
    camera = glm::ortho( 0.0f, (float)WidthOfScreen( x11->screen ), (float)HeightOfScreen( x11->screen ), 0.0f, -1.0f, 1.0f);

    // Last, we actually display the window <:o)
	XMapWindow( x11->display, window );
}

SlopWindow::~SlopWindow() {
    delete framebuffer;
    // Try to erase the window before destroying it.
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    display();
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    display();
    glXDestroyContext( x11->display, context );
    XDestroyWindow( x11->display, window );
}

void SlopWindow::display() {
    glXSwapBuffers( x11->display, window );
    glXWaitGL();
}

void SlopWindow::setCurrent() {
    glXMakeCurrent( x11->display, window, context ) ;
}
