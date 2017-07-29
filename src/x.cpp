#include "x.hpp"

using namespace slop;

glm::vec4 slop::getWindowGeometry( Window win, bool removeDecoration ) {
    // First lets check for if we're a window manager frame.
    Window root, parent;
    Window* children;
    unsigned int num_children;
    XQueryTree( x11->display, win, &root, &parent, &children, &num_children);

    // To do that, we check if our top level child happens to have the _NET_FRAME_EXTENTS atom.
    unsigned char *data;
    Atom type_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    int format_return;
    bool window_frame = false;
    Window actualWindow = win;
    if ( num_children > 0 && XGetWindowProperty( x11->display, children[num_children-1], XInternAtom( x11->display, "_NET_FRAME_EXTENTS", False),
                        0, LONG_MAX, False, XA_CARDINAL, &type_return,
                        &format_return, &nitems_return, &bytes_after_return,
                        &data) == Success ) {
        if ((type_return == XA_CARDINAL) && (format_return == 32) && (nitems_return == 4) && (data)) {
            actualWindow = children[num_children-1];
            window_frame = true;
        }
    }

    // If we actually don't want the window frame (decoration), we just actually grab the decoration's innards.
    // And ignore all of the _NET_FRAME_EXTENTS information.
    if ( actualWindow != win && removeDecoration ) {
        win = actualWindow;
        window_frame = false;
    }

    // If we're a window frame, we actually get the dimensions of the child window, then add the _NET_FRAME_EXTENTS to it.
    // (then add the border width of the window frame after that.)
    if ( window_frame ) {
        // First lets grab the border width.
        XWindowAttributes frameattr;
        XGetWindowAttributes( x11->display, win, &frameattr );
        // Then lets grab the dims of the child window.
        XWindowAttributes attr;
        XGetWindowAttributes( x11->display, actualWindow, &attr );
        unsigned int width = attr.width;           
        unsigned int height = attr.height;         
        // We combine both border widths.
        unsigned int border = attr.border_width+frameattr.border_width;   
        int x, y;
        // Gotta translate them into root coords, we can adjust for the border width here.
        Window junk;
        XTranslateCoordinates( x11->display, actualWindow, attr.root, -border, -border, &x, &y, &junk );
        width += border*2;
        height += border*2;
        // Now uh, remember that _NET_FRAME_EXTENTS stuff? That's the window frame information.
        // We HAVE to do this because mutter likes to mess with window sizes with shadows and stuff.
        unsigned long* ldata = (unsigned long*)data;
        width += ldata[0] + ldata[1];
        height += ldata[2] + ldata[3];
        x -= ldata[0];
        y -= ldata[2];
        XFree( data );
        return glm::vec4( x, y, width, height );
    } else {
        // Either the WM is malfunctioning, or the window specified isn't a window manager frame.
        // so we just rely on X to give a decent geometry.
        XWindowAttributes attr;
        XGetWindowAttributes( x11->display, win, &attr );
        unsigned int width = attr.width;           
        unsigned int height = attr.height;         
        // We combine both border widths.
        unsigned int border = attr.border_width;   
        int x, y;
        // Gotta translate them into root coords, we can adjust for the border width here.
        Window junk;
        XTranslateCoordinates( x11->display, win, attr.root, -border, -border, &x, &y, &junk );
        width += border*2;
        height += border*2;
        return glm::vec4( x, y, width, height );
    }
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
