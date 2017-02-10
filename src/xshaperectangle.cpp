#include "xshaperectangle.hpp"

XShapeRectangle::XShapeRectangle( glm::vec2 p1, glm::vec2 p2, float border, float padding, glm::vec4 color, bool highlight ) {
    this->color = convertColor( color );
    this->border = border;
    this->padding = padding;
    this->highlight = highlight;
    this->alpha = color.a;
    // Find each corner of the rectangle
    ul = glm::vec2( glm::min( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    bl = glm::vec2( glm::min( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    ur = glm::vec2( glm::max( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    br = glm::vec2( glm::max( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    // Offset the inner corners by the padding.
    ul = ul + glm::vec2(-padding,padding);
    bl = bl + glm::vec2(-padding,-padding);
    ur = ur + glm::vec2(padding,padding);
    br = br + glm::vec2(padding,-padding);
    // Create the outer corners by offsetting the inner by the bordersize
    oul = ul + glm::vec2(-border,border);
    obl = bl + glm::vec2(-border,-border);
    our = ur + glm::vec2(border,border);
    obr = br + glm::vec2(border,-border);

    XSetWindowAttributes attributes;
    // Set up the window so it's our color 
    attributes.background_pixel = this->color.pixel;
    // Disable window decorations.
    attributes.override_redirect = True;
    // Make sure we know when we've been successfully destroyed later!
    attributes.event_mask = StructureNotifyMask;
    unsigned long valueMask = CWBackPixel | CWOverrideRedirect | CWEventMask;
    // Create the window
    window = XCreateWindow( x11->display, x11->root, 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ),
                              0, CopyFromParent, InputOutput,
                              CopyFromParent, valueMask, &attributes );
    if ( alpha < 1 ) {
        // Change the window opacity
        unsigned int cardinal_alpha = (unsigned int) (alpha * (unsigned int)-1) ;
        XChangeProperty( x11->display, window, XInternAtom( x11->display, "_NET_WM_WINDOW_OPACITY", 0),
                         XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&cardinal_alpha, 1 );
    }
    // Set the class hint, and title to "slop"
    XClassHint classhints;
    char name[] = "slop";
    classhints.res_name = name;
    classhints.res_class = name;
    XSetClassHint( x11->display, window, &classhints );
    // Now punch a hole into it so it looks like a selection rectangle, but only if we're not highlighting.
    generateHoles();
    createdWindow = false;
}

void XShapeRectangle::createWindow() {
    if ( createdWindow ) {
        return;
    }
	XMapWindow( x11->display, window );
    createdWindow = true;
}

void XShapeRectangle::generateHoles() {
    if ( !highlight ) {
        XRectangle rects[4];
        // Left
        rects[0].x = oul.x;
        rects[0].y = obl.y;
        rects[0].width = border;
        rects[0].height = oul.y-obl.y;
        // Top
        rects[1].x = ul.x;
        rects[1].y = obl.y;
        rects[1].width = ur.x-ul.x;
        rects[1].height = border;
        // Right
        rects[2].x = ur.x;
        rects[2].y = obr.y;
        rects[2].width = border;
        rects[2].height = our.y - obr.y;
        // Bottom
        rects[3].x = bl.x;
        rects[3].y = ul.y;
        rects[3].width = br.x-bl.x;
        rects[3].height = border;
        XShapeCombineRectangles( x11->display, window, ShapeBounding, 0, 0, rects, 4, ShapeSet, 0);
        return;
    }
    XRectangle rect;
    rect.x = oul.x;
    rect.y = obl.y;
    rect.width = our.x-oul.x;
    rect.height = oul.y-obl.y;
    XShapeCombineRectangles( x11->display, window, ShapeBounding, 0, 0, &rect, 1, ShapeSet, 0);
}

void XShapeRectangle::setPoints( glm::vec2 p1, glm::vec2 p2 ) {
    // Find each corner of the rectangle
    ul = glm::vec2( glm::min( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    bl = glm::vec2( glm::min( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    ur = glm::vec2( glm::max( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    br = glm::vec2( glm::max( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    // Offset the inner corners by the padding.
    ul = ul + glm::vec2(-padding,padding);
    bl = bl + glm::vec2(-padding,-padding);
    ur = ur + glm::vec2(padding,padding);
    br = br + glm::vec2(padding,-padding);
    // Create the outer corners by offsetting the inner by the bordersize
    oul = ul + glm::vec2(-border,border);
    obl = bl + glm::vec2(-border,-border);
    our = ur + glm::vec2(border,border);
    obr = br + glm::vec2(border,-border);
    generateHoles();
}

XShapeRectangle::~XShapeRectangle() {
}

void XShapeRectangle::draw( glm::mat4& matrix ) {
    // We don't want to be visible until we're asked to draw.
    createWindow();
}

glm::vec4 XShapeRectangle::getRect() {
    return glm::vec4( bl.x, bl.y, ur.x-ul.x, ul.y-bl.y );
}

XColor XShapeRectangle::convertColor( glm::vec4 color ) {
    // Convert float colors to shorts.
    short red   = short( floor( color.r * 65535.f ) );
    short green = short( floor( color.g * 65535.f ) );
    short blue  = short( floor( color.b * 65535.f ) );
    XColor xc;
    xc.red = red;
    xc.green = green;
    xc.blue = blue;
    int err = XAllocColor( x11->display, DefaultColormap( x11->display, XScreenNumberOfScreen( x11->screen ) ), &xc );
    if ( err == BadColor ) {
        throw new std::runtime_error(std::string("Couldn't allocate a color"));
    }
    return xc;
}
