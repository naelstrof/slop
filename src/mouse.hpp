/* mouse.hpp: Interfaces with x11 to grab mouse information.
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

#ifndef N_MOUSE_H_
#define N_MOUSE_H_

#include <vector>
#include <glm/glm.hpp>
#include <X11/cursorfont.h>
#include <iostream>

#include "x.hpp"

class Mouse {
private:
    X11* x11;
    std::vector<glm::ivec2> buttons;
    Cursor xcursor;
    int currentCursor;
public:
	Window hoverWindow;
	Window subWindow;
	void update();
    Mouse( X11* x11 );
    ~Mouse();
    void setCursor( int cursor );
    glm::vec2 getMousePos();
    void setButton( int button, int state );
    int getButton( int button );
};

extern Mouse* mouse;

#endif // N_MOUSE_H_
