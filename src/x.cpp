#include "x.hpp"

bool X11::hasCompositor() {
    std::stringstream prop_name;
    prop_name << "_NET_WM_CM_S" << XScreenNumberOfScreen( screen );
    Atom prop_atom = XInternAtom(display, prop_name.str().c_str(), False);
    return XGetSelectionOwner(display, prop_atom) != None;
}

X11::X11( std::string displayName ) {
    // Initialize display
    display = XOpenDisplay( displayName.c_str() );
    if ( !display ) {
        throw new std::runtime_error(std::string("Error: Failed to open X display: ") + displayName );
    }
    screen = ScreenOfDisplay( display, DefaultScreen( display ) );
    visual = DefaultVisual( display, XScreenNumberOfScreen( screen ) );
    root = DefaultRootWindow( display );
}

X11::~X11() {
    XCloseDisplay( display );
}
