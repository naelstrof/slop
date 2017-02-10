/* rectangle.hpp: Rectangle generic
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

#ifndef N_RECTANGLE_H_
#define N_RECTANGLE_H_

#include <glm/glm.hpp>

class Rectangle {
public:
    Rectangle();
    virtual glm::vec4 getRect();
    virtual ~Rectangle();
    virtual void setPoints( glm::vec2 p1, glm::vec2 p2 );
    virtual void draw(glm::mat4& matrix);
};

#endif // N_RECTANGLE_H_
