/* rectangle.hpp: generates a vertex mesh and draws it.
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

#ifndef N_GLRECTANGLE_H_
#define N_GLRECTANGLE_H_

#include "gl_core_3_0.h"
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>

#include "shader.hpp"
#include "rectangle.hpp"

namespace slop {

struct RectangleBuffer {
    unsigned int corner_verts;
    unsigned int corner_uvs;
    unsigned int rectangle_verts;
    unsigned int rectangle_uvs;
    unsigned int center_verts;
    unsigned int center_uvs;
};

class GLRectangle : public Rectangle {
private:
    glm::vec2 ul, oul;
    glm::vec2 bl, obl;
    glm::vec2 ur, our;
    glm::vec2 br, obr;
    bool highlight;
    void generateBuffers();
    RectangleBuffer buffer;
    unsigned int corner_vertCount;
    unsigned int rectangle_vertCount;
    unsigned int center_vertCount;
    float border;
    float padding;
    Shader* shader;
    glm::vec4 color;
public:
    glm::vec4 getRect();
    GLRectangle(glm::vec2 p1, glm::vec2 p2, float border = 1, float padding = 0, glm::vec4 color = glm::vec4(1,1,1,1), bool highlight = false );
    ~GLRectangle();
    void setPoints( glm::vec2 p1, glm::vec2 p2 );
    void draw(glm::mat4& matrix);
};

}

#endif // N_RECTANGLE_H_
