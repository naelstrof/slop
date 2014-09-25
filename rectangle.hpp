// rectangle.hpp: handles creating rectangles on screen.

#ifndef IS_RECTANGLE_H_
#define IS_RECTANGLE_H_

#include "x.hpp"

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace slop {

class Rectangle {
public:
            Rectangle( int x, int y, int width, int height, int border, int padding, float r, float g, float b );
            ~Rectangle();
    void    setPos( int x, int y );
    void    setDim( int w, int h );
    void    setGeo( int x, int y, int w, int h );
    Window  m_window;
    XColor  m_color;
    int     m_x;
    int     m_y;
    int     m_xoffset;
    int     m_yoffset;
    int     m_width;
    int     m_height;
    int     m_border;
    int     m_padding;
    bool    m_flippedx;
    bool    m_flippedy;
private:
    int     convertColor( float r, float g, float b );
    void    constrain( int w, int h );
};

}

#endif // IS_RECTANGLE_H_
