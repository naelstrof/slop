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
}

slop::Rectangle::Rectangle( int x, int y, int width, int height, int border, int padding, float r, float g, float b ) {
    m_xoffset = 0;
    m_yoffset = 0;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_border = border;
    m_padding = padding;
    m_window = None;

    // Convert the width, height, x, and y to coordinates that don't have negative values.
    // (also adjust for padding and border size.)
    constrain( width, height );
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
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
    // Not actually sure what this does, but it keeps the window from bugging out :u.
    attributes.override_redirect = True;
    // We must use our color map, because that's where our color is allocated.
    attributes.colormap = xengine->m_colormap;
    // Make sure we know when we've been successfully destroyed later!
    attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWBackPixmap | CWBackPixel | CWOverrideRedirect | CWColormap | CWEventMask;

    // Create the window offset by our generated offsets (see constrain( float, float ))
    m_window = XCreateWindow( xengine->m_display, xengine->m_root, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border, m_width+m_border*2, m_height+m_border*2,
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );

    // Now punch a hole into it so it looks like a selection rectangle!
    XRectangle rect;
    rect.x = rect.y = m_border;
    rect.width = m_width;
    rect.height = m_height;

    XClassHint classhints;
    char name[] = "slop";
    classhints.res_name = name;
    classhints.res_class = name;
    XSetClassHint( xengine->m_display, m_window, &classhints );

    XShapeCombineRectangles( xengine->m_display, m_window, ShapeBounding, 0, 0, &rect, 1, ShapeSubtract, 0);
    XMapWindow( xengine->m_display, m_window );
}

void slop::Rectangle::setPos( int x, int y ) {
    if ( m_x == x && m_y == y ) {
        return;
    }
    m_x = x;
    m_y = y;
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border );
}

void slop::Rectangle::setDim( int w, int h ) {
    if ( m_width == w && m_height == h ) {
        return;
    }

    constrain( w, h );
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }

    // Change the window size and location to our generated offsets (see constrain( float, float ))
    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border );
    // Regenerate our hole
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
}

void slop::Rectangle::setGeo( int x, int y, int w, int h ) {
    if ( m_x == x && m_y == y && m_width == w && m_height == h ) {
        return;
    }

    m_x = x;
    m_y = y;
    constrain( w, h );
    // If we don't have a border, we don't exist, so just die.
    if ( m_border == 0 ) {
        return;
    }

    // Change the window size and location to our generated offsets (see constrain( float, float ))
    XResizeWindow( xengine->m_display, m_window, m_width+m_border*2, m_height+m_border*2 );
    XMoveWindow( xengine->m_display, m_window, m_x+m_xoffset-m_border, m_y+m_yoffset-m_border );
    // Regenerate our hole
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
}

// Keeps our rectangle's sizes all positive, so Xlib doesn't throw an exception.
// It also keeps our values in absolute coordinates which is nice.
void slop::Rectangle::constrain( int w, int h ) {
    int pad = m_padding;
    if ( pad < 0 && std::abs( w ) < std::abs( pad ) * 2 ) {
        pad = 0;
    }
    if ( w < 0 ) {
        m_flippedx = true;
        m_xoffset = w - pad;
        m_width = -w + pad * 2;
    } else {
        m_flippedx = false;
        m_xoffset = -pad;
        m_width = w + pad * 2;
    }

    pad = m_padding;
    if ( pad < 0 && std::abs( h ) < std::abs( pad ) * 2 ) {
        pad = 0;
    }
    if ( h < 0 ) {
        m_flippedy = true;
        m_yoffset = h - pad;
        m_height = -h + pad * 2;
    } else {
        m_flippedy = false;
        m_yoffset = -pad;
        m_height = h + pad * 2;
    }
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
