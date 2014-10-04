// rectangle.hpp: handles creating rectangles on screen.

#ifndef IS_RECTANGLE_H_
#define IS_RECTANGLE_H_

#include "x.hpp"

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace slop {

class Rectangle {
public:
            Rectangle( int sx, int sy, int ex, int ey, int border, bool highlight, float r, float g, float b, float a );
            ~Rectangle();
    void    setPos( int x, int y );
    void    setDim( int w, int h );
    void    setGeo( int x, int y, int w, int h );
    Window  m_window;
    XColor  m_color;
    int     m_x;
    int     m_y;
    int     m_width;
    int     m_height;
    int     m_border;
    bool    m_highlight;
private:
    int     convertColor( float r, float g, float b );
    void    constrain( int w, int h );
};

}

#endif // IS_RECTANGLE_H_
