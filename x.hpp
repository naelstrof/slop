// x.hpp: handles starting and managing X

#ifndef IS_X_H_
#define IS_X_H_

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace slrn {

enum CursorType {
    Left,
    Crosshair,
    Cross,
    UpperLeftCorner,
    UpperRightCorner,
    LowerRightCorner,
    LowerLeftCorner
};

class WindowRectangle {
public:
    int m_x;
    int m_y;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_border;
};

class Rectangle {
public:
            Rectangle( int x, int y, int width, int height, int border, int padding );
            ~Rectangle();
    void    setPos( int x, int y );
    void    setDim( int w, int h );
    Window  m_window;
    int     m_x;
    int     m_y;
    int     m_xoffset;
    int     m_yoffset;
    int     m_width;
    int     m_height;
    int     m_border;
    int     m_padding;
    XColor  m_forground, m_forgroundExact;
    XColor  m_background, m_backgroundExact;
    bool    m_flippedx;
    bool    m_flippedy;
private:
    void    constrain( int w, int h );
};

class XEngine {
public:
                        XEngine();
                        ~XEngine();
    int                 init( std::string display );
    void                tick();
    int                 grabCursor( slrn::CursorType type );
    int                 releaseCursor();
    void                setCursor( slrn::CursorType type );
    void                drawRect( int x, int y, unsigned int w, unsigned int h );
    void                addRect( Rectangle* rect );
    void                removeRect( Rectangle* rect );
    Display*            m_display;
    Visual*             m_visual;
    Screen*             m_screen;
    Colormap            m_colormap;
    Window              m_root;
    int                 m_mousex;
    int                 m_mousey;
    std::vector<bool>   m_mouse;
    bool                mouseDown( unsigned int button );
    WindowRectangle     m_hoverWindow;
    Window              m_hoverXWindow;
private:
    void                updateHoverWindow();
    void                updateHoverWindow( Window child );
    bool                m_good;
    std::vector<Cursor> m_cursors;
    std::vector<Rectangle*> m_rects;
    Cursor              getCursor( slrn::CursorType type );
};

}

extern slrn::XEngine* xengine;

#endif // IS_X_H_
