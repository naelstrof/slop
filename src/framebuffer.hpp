/* framebuffer.hpp: Creates and manages an off-screen framebuffer. Uses supplied shader to draw it to the screen.
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

#ifndef N_FRAMEBUFFER_H_
#define N_FRAMEBUFFER_H_

#include "gl_core_3_3.h"
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>

#include "shader.hpp"

namespace slop {

class Framebuffer {
private:
    unsigned int fbuffer;
    unsigned int image;
    unsigned int buffers[2];
    unsigned int vertCount;
    Shader* shader;
public:
    Framebuffer( int w, int h );
    ~Framebuffer();
    void setShader( std::string );
    void draw();
    void resize( int w, int h );
    void bind();
    void unbind();
};

}

#endif
