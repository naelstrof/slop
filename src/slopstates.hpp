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
#include "rectangle.hpp"
#include "slop.hpp"

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
public:
    virtual void update( SlopMemory& memory, double dt );
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

#endif // N_SLOPSTATES_H_
