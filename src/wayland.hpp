/* wayland.hpp: Interfaces with wayland to get keyboard, mouse, windows...
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

#ifndef N_WAYLAND_H_
#define N_WAYLAND_H_

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

//#include <mutter/meta/compositor-mutter.h>

#ifndef NULL
#define NULL 0
#endif

class Wayland {
public:
    Wayland();
    ~Wayland();
    void init();
    wl_display* display;
    wl_compositor* compositor;
    wl_seat* seat;
    wl_shell* shell;
    wl_shm* shm;
    wl_output* output;
    EGLDisplay egl_display;
    glm::vec2 mousepos;
    std::vector<glm::vec4> getWindows();
};

extern Wayland* wayland;

#endif // N_WAYLAND_H_
