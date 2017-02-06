/* slop.hpp: exposes a selection interface
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

#include "window.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "rectangle.hpp"
#include "slopstates.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "resource.hpp"

class SlopState;

class SlopOptions {
public:
    SlopOptions();
    float borderSize;
    float padding;
    bool highlight;
    std::string shader;
    float r;
    float g;
    float b;
    float a;
};

class SlopSelection {
public:
    SlopSelection( float x, float y, float w, float h );
    float x;
    float y;
    float w;
    float h;
};

class SlopMemory {
private:
    SlopState* state;
    SlopState* nextState;
public:
    SlopMemory( SlopOptions* options );
    ~SlopMemory();
    bool running;
    Rectangle* rectangle;
    void setState( SlopState* state );
    void update( double dt );
    void draw( glm::mat4& matrix );
};

SlopSelection SlopSelect( SlopOptions* options = NULL, bool* cancelled = NULL );

#endif // N_SLOP_H_
