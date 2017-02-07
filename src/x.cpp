#include "x.hpp"

X11::X11( std::string displayName ) {
    // Initialize display
    display = XOpenDisplay( displayName.c_str() );
    if ( !display ) {
        throw new std::runtime_error(std::string("Error: Failed to open X display: ") + displayName );
    }
    screen = ScreenOfDisplay( display, DefaultScreen( display ) );
    visual = DefaultVisual( display, XScreenNumberOfScreen( screen ) );
    root = DefaultRootWindow( display );
    selectAllInputs( root );
}

X11::~X11() {
    XCloseDisplay( display );
}

// This cheesy function makes sure we get all EnterNotify events on ALL the windows.
void X11::selectAllInputs( Window win ) {
    Window root, parent;
    Window* children;
    unsigned int nchildren;
    XQueryTree( display, win, &root, &parent, &children, &nchildren );
    for ( unsigned int i=0;i<nchildren;i++ ) {
        XSelectInput( display, children[ i ], EnterWindowMask );
        selectAllInputs( children[i] );
    }
    free( children );
}
