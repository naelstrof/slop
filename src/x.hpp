/* x.hpp: Handles starting and managing X.
 *
 * Copyright (C) 2014: Dalton Nell, Slop Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Slop.
 *
 * Slop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Slop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Slop.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IS_X_H_
#define IS_X_H_

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

#include <stdlib.h>
#include <cstring>
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
    LowerLeftCorner,
    Dot,
    Box
};

class WindowRectangle {
public:
    int          m_x;
    int          m_y;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_border;
    Window       m_window;
    bool         m_decorations;
    Window       getWindow();
    void         setGeometry( Window win, bool decorations );
    void         applyPadding( int padding );
    void         applyMinMaxSize( unsigned int minimumsize, unsigned int maximumsize );
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
    unsigned int        getWidth();
    unsigned int        getHeight();
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
