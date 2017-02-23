#include "x.hpp"

using namespace slop;

glm::vec4 slop::getWindowGeometry( Window win, bool removeDecoration) {
    XWindowAttributes attr;         
    XGetWindowAttributes( x11->display, win, &attr );
    unsigned int width = attr.width;           
    unsigned int height = attr.height;         
    unsigned int border = attr.border_width;   
    int x, y;
    Window junk;
    XTranslateCoordinates( x11->display, win, attr.root, -attr.border_width, -attr.border_width, &x, &y, &junk );
    if ( !removeDecoration ) {
        width += border*2;
        height += border*2;
        return glm::vec4( x, y, width, height );
    }
    // This is required to be defined by the window manager, so we can assume it exists.
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *data;
    int result = XGetWindowProperty(x11->display, win,
                                    XInternAtom(x11->display, "_NET_FRAME_EXTENTS", true),
                                    0, 4, False, AnyPropertyType, 
                                    &actual_type, &actual_format, &nitems, &bytes_after, &data);
    if ( result == Success ) {
        // Make sure we got the data we expect...
        if ((nitems == 4) && (bytes_after == 0)) {
            x += (int)data[0];
            width -= (int)data[1];
            y += (int)data[2];
            height -= (int)data[3];
        }
    }
    return glm::vec4( x, y, width, height );
}

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
