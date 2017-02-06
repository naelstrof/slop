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

#include "gl_core_3_3.h"
#include "wayland.hpp"
//#include <GL/glew.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "framebuffer.hpp"

class Window {
public:
    Framebuffer* framebuffer;
    int width, height;
    glm::mat4 camera;
    Window( unsigned int w, unsigned int h );
    ~Window();
    void setFullScreen();
    void setCurrent();
    void setTitle( std::string title );
    void setClass( std::string c );
    void display();
    EGLContext egl_context;
    struct wl_surface *surface;
    struct wl_shell_surface *shell_surface;
    struct wl_egl_window *egl_window;
    EGLSurface egl_surface;
};

#endif // N_WINDOW_H_
