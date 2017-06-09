/* window.hpp: Spawns and manages windows. By default it creates an OpenGL context with them.
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

#ifndef N_WINDOW_H_
#define N_WINDOW_H_

#include <string>
#include <exception>

#include <iostream>
#include "gl_core_3_0.h"
#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>
#include <glm/gtc/matrix_transform.hpp>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>

#include "x.hpp"
#include "framebuffer.hpp"

namespace slop {

class SlopWindow {
public:
    Framebuffer* framebuffer;
    int width, height;
    glm::mat4 camera;
    GLXWindow window;
    GLXContext context;
    SlopWindow();
    ~SlopWindow();
    void setCurrent();
    void display();
};

}

#endif // N_WINDOW_H_
