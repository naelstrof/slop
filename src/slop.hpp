/* glslop.hpp: exposes an opengl selection interface
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

#ifndef N_SLOP_H_
#define N_SLOP_H_

#include <string>
#include <chrono>
#include <thread>

#include "slopstates.hpp"
#include "mouse.hpp"
#include "resource.hpp"
#include "keyboard.hpp"

#include "window.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "glrectangle.hpp"
#include "xshaperectangle.hpp"

class SlopOptions {
public:
    SlopOptions();
    float borderSize;
    float padding;
    float tolerance;
    bool highlight;
    bool nokeyboard;
    int nodecorations;
    std::string shader;
    float r;
    float g;
    float b;
    float a;
    std::string xdisplay;
};

class SlopSelection {
public:
    SlopSelection( float x, float y, float w, float h, int id );
    float x;
    float y;
    float w;
    float h;
// This is an X11 Window ID
    int id;
};

SlopSelection SlopSelect( SlopOptions* options = NULL, bool* cancelled = NULL, bool quiet = false );

#endif // N_SLOP_H_
