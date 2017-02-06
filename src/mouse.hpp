/* mouse.hpp: Interfaces with wayland to grab mouse information.
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

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <vector>
#include <glm/glm.hpp>
#include "linux/input-event-codes.h"
#include "wayland.hpp"

class Mouse {
private:
    wl_cursor_theme* theme;
    float x,y;
    std::vector<glm::ivec2> buttons;
    std::string currentMouseCursor;
public:
    wl_surface* surface;
    wl_cursor* cursor;
    wl_pointer* pointer;
    int serial;
    Mouse( Wayland* wayland );
    void setMousePos( float x, float y );
    void setCursor( std::string name );
    glm::vec2 getMousePos();
    void setButton( int button, int state );
    int getButton( int button );
};

extern Mouse* mouse;

#endif // N_MOUSE_H_
