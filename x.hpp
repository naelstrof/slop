// x.hpp: handles starting and managing X

#ifndef IS_X_H_
#define IS_X_H_

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace slop {

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
    int          m_x;
    int          m_y;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_border;
    bool         m_decorations;
    void         setGeometry( Window win, bool decorations );
};

class Rectangle {
public:
            Rectangle( int x, int y, int width, int height, int border, int padding, float r, float g, float b );
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
    XColor  m_color;
    bool    m_flippedx;
    bool    m_flippedy;
private:
    int     convertColor( float r, float g, float b );
    void    constrain( int w, int h );
};

class XEngine {
public:
                        XEngine();
                        ~XEngine();
    int                 init( std::string display );
    void                tick();
    int                 grabCursor( slop::CursorType type );
    int                 grabKeyboard();
    bool                anyKeyPressed();
    int                 releaseCursor();
    int                 releaseKeyboard();
    void                setCursor( slop::CursorType type );
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
    Window              m_hoverWindow;
    bool                m_keypressed;
private:
    void                updateHoverWindow();
    void                updateHoverWindow( Window child );
    bool                m_good;
    std::vector<Cursor> m_cursors;
    std::vector<Rectangle*> m_rects;
    Cursor              getCursor( slop::CursorType type );
};

int XEngineErrorHandler( Display* dpy, XErrorEvent* event );

}

extern slop::XEngine* xengine;

#endif // IS_X_H_
