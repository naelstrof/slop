/* xshaperectangle.hpp: A rectangle that doesn't use OpenGL to function.
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

#ifndef N_XSHAPERECTANGLE_H_
#define N_XSHAPERECTANGLE_H_

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include "x.hpp"

class Rectangle {
private:
    glm::vec2 ul, oul;
    glm::vec2 bl, obl;
    glm::vec2 ur, our;
    glm::vec2 br, obr;
    bool createdWindow;
    bool highlight;
    float border;
    float padding;
    XColor color;
    float alpha;
    XColor convertColor( glm::vec4 color );
    void generateHoles();
    void createWindow();
public:
    Window window;
    glm::vec4 getRect();
    Rectangle(glm::vec2 p1, glm::vec2 p2, float border = 1, float padding = 0, glm::vec4 color = glm::vec4(1,1,1,1), bool highlight = false );
    ~Rectangle();
    void setPoints( glm::vec2 p1, glm::vec2 p2 );
    void draw(glm::mat4& matrix);
};

#endif // N_XSHAPERECTANGLE_H_
