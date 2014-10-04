#include "rectangle.hpp"

static Bool isDestroyNotify( Display* dpy, XEvent* ev, XPointer win ) {
    return ev->type == DestroyNotify && ev->xdestroywindow.window == *((Window*)win);
}

slop::Rectangle::~Rectangle() {
    if ( m_window == None ) {
        return;
    }
    // Free up our color.
    XFreeColors( xengine->m_display, xengine->m_colormap, &m_color.pixel, 1, 0 );
    XDestroyWindow( xengine->m_display, m_window );
    XEvent event;
    // Block until the window is actually completely removed.
    XIfEvent( xengine->m_display, &event, &isDestroyNotify, (XPointer)&m_window );
    // Sleep for 0.1 seconds in hope that the screen actually cleared the window.
    usleep( 10000 );
}

slop::Rectangle::Rectangle( int sx, int sy, int ex, int ey, int border, bool highlight, float r, float g, float b, float a ) {
    m_x = std::min( sx, ex );
    m_y = std::min( sy, ey );
    m_width = std::max( sx, ex ) - m_x;
    m_height = std::max( sy, ey ) - m_y;
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

    // This sets up m_color
    int err = convertColor( r, g, b );
    if ( err ) {
        fprintf( stderr, "Couldn't allocate color of value %f,%f,%f!\n", r, g, b );
    }
    XSetWindowAttributes attributes;
    // Set up the window so it's our color 
    attributes.background_pixmap = None;
    attributes.background_pixel = m_color.pixel;
    attributes.border_pixel = m_color.pixel;
    // Not actually sure what this does, but it keeps the window from bugging out :u.
    attributes.override_redirect = True;
    // We must use our color map, because that's where our color is allocated.
    attributes.colormap = xengine->m_colormap;
    // Make sure we know when we've been successfully destroyed later!
    attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWBackPixmap | CWBackPixel | CWOverrideRedirect | CWColormap | CWEventMask;

    // Create the window
    m_window = XCreateWindow( xengine->m_display, xengine->m_root, m_x-m_border, m_y-m_border, m_width+m_border*2, m_height+m_border*2,
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );

    if ( a < 1 ) {
        unsigned int cardinal_alpha = (unsigned int) (a * (unsigned int)-1) ;
        XChangeProperty( xengine->m_display, m_window, XInternAtom( xengine->m_display, "_NET_WM_WINDOW_OPACITY", 0),
                         XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&cardinal_alpha, 1 );
    }

    XClassHint classhints;
    char name[] = "slop";
    classhints.res_name = name;
    classhints.res_class = name;
    XSetClassHint( xengine->m_display, m_window, &classhints );

    // Now punch a hole into it so it looks like a selection rectangle, but only if we're not highlighting.
    if ( !m_highlight ) {
        XRectangle rect;
        rect.x = rect.y = m_border;
        rect.width = m_width;
        rect.height = m_height;

        XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    }
    // Make it so all input falls through
    XRectangle rect;
    rect.x = rect.y = rect.width = rect.height = 0;
    XShapeCombineRectangles( xengine->m_display, m_window, ShapeInput, 0, 0, &rect, 1, ShapeSet, 0);
    XMapWindow( xengine->m_display, m_window );
}

void slop::Rectangle::setGeo( int sx, int sy, int ex, int ey ) {
    int x = std::min( sx, ex );
    int y = std::min( sy, ey );
    int w = std::max( sx, ex ) - x;
    int h = std::max( sy, ey ) - y;
    if ( m_x == x && m_y == y && m_width == w && m_height == h ) {
        return;
    }

    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;

    // Change the window size
    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    if ( m_border > 0 ) {
        // Fill up our old hole
        XRectangle rect;
        rect.x = rect.y = 0;
        rect.width = m_width+m_border*2;
        rect.height = m_height+m_border*2;
        XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSet, 0);
        // Then punch out another.
        rect.x = rect.y = m_border;
        rect.width = m_width;
        rect.height = m_height;
        XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
        // Then make it so all input falls through.
        rect.x = rect.y = rect.width = rect.height = 0;
        XShapeCombineRectangles( xengine->m_display, m_window, ShapeInput, 0, 0, &rect, 1, ShapeSet, 0);
    }
    XMoveWindow( xengine->m_display, m_window, m_x-m_border, m_y-m_border );
}

int slop::Rectangle::convertColor( float r, float g, float b ) {
    // Convert float colors to shorts.
    short red   = short( floor( r * 65535.f ) );
    short green = short( floor( g * 65535.f ) );
    short blue  = short( floor( b * 65535.f ) );
    XColor color;
    color.red = red;
    color.green = green;
    color.blue = blue;
    int err = XAllocColor( xengine->m_display, xengine->m_colormap, &color );
    if ( err == BadColor ) {
        return err;
    }
    m_color = color;
    return 0;
}
