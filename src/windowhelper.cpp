#include "windowhelper.hpp"

glm::vec4 getWindowGeometry( Window win, bool removeDecoration) {
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
