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
#include <chrono>
#include <thread>

#include "xshaperectangle.hpp"
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
    float tolerance;
    bool highlight;
    bool nodecorations;
    float r;
    float g;
    float b;
    float a;
    std::string xdisplay;
};

class SlopSelection {
public:
    SlopSelection( float x, float y, float w, float h, Window id );
    float x;
    float y;
    float w;
    float h;
    Window id;
};

class SlopMemory {
private:
    SlopState* state;
    SlopState* nextState;
public:
    SlopMemory( SlopOptions* options );
    ~SlopMemory();
    Window selectedWindow;
    bool running;
    float tolerance;
    bool nodecorations;
    Rectangle* rectangle;
    void setState( SlopState* state );
    void update( double dt );
    void draw( glm::mat4& matrix );
};

SlopSelection SlopSelect( SlopOptions* options = NULL, bool* cancelled = NULL );

#endif // N_SLOP_H_
