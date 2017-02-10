/* slopstates.hpp: State machine stuffs.
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

#ifndef N_SLOPSTATES_H_
#define N_SLOPSTATES_H_

#include "mouse.hpp"
#include "windowhelper.hpp"
#include "slop.hpp"

#ifdef SLOP_LEGACY_MODE
#include "xshaperectangle.hpp"
#else
#include "rectangle.hpp"
#endif

class SlopMemory;

class SlopState {
public:
    virtual ~SlopState();
    virtual void onEnter( SlopMemory& memory );
    virtual void onExit( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopStart : SlopState {
private:
    bool setStartPos;
    glm::vec2 startPos;
public:
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopStartDrag : SlopState {
private:
    glm::vec2 startPoint;
public:
    SlopStartDrag( glm::vec2 point );
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopEndDrag : SlopState {
public:
    virtual void onEnter( SlopMemory& memory );
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


#endif // N_SLOPSTATES_H_
