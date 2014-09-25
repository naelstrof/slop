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
    int                 m_mousex;
    int                 m_mousey;
    Display*            m_display;
    Visual*             m_visual;
    Screen*             m_screen;
    Colormap            m_colormap;
    Window              m_root;
    Window              m_hoverWindow;
    std::vector<bool>   m_mouse;
    bool                mouseDown( unsigned int button );
    bool                m_keypressed;
private:
    bool                m_good;
    std::vector<Cursor> m_cursors;
    Cursor              getCursor( slop::CursorType type );
    void                selectAllInputs( Window win, long event_mask);
};

int XEngineErrorHandler( Display* dpy, XErrorEvent* event );

}

extern slop::XEngine* xengine;

#endif // IS_X_H_
