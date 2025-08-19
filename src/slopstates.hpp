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
#include "keyboard.hpp"
#include "slop.hpp"

#include "rectangle.hpp"

#include <vector>
#include <X11/keysymdef.h>
#include "x.hpp"

namespace slop {

class SlopMemory;
class SlopOptions;

class SlopState {
protected:
    glm::vec2 startPoint;
public:
    virtual void onEnter( SlopMemory& memory );
    virtual void onExit( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
    virtual ~SlopState();
};

class SlopStart : SlopState {
private:
    bool setStartPos;
public:
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopStartDrag : SlopState {
private:
    float repeatTimer;
    float multiplier;
public:
    SlopStartDrag( glm::vec2 point);
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
};

class SlopEndDrag : SlopState {
public:
    virtual void onEnter( SlopMemory& memory );
};

class SlopStartMove : SlopState {
private:
    glm::vec2 diagonal;
public:
    SlopStartMove( glm::vec2 oldPoint, glm::vec2 newPoint );
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
};

class SlopMemory {
private:
    SlopState* state;
    SlopState* nextState;
public:
    SlopMemory( SlopOptions* options, Rectangle* rect );
    ~SlopMemory();
    Window selectedWindow;
    bool running;
    float tolerance;
    bool nodecorations;
    bool nodrag;
    bool up;
    Rectangle* rectangle;
    KeySym keyMove;
    std::vector<KeySym> keyAdjust;
    void setState( SlopState* state );
    void update( double dt );
    void draw( glm::mat4& matrix );
};

}

#endif // N_SLOPSTATES_H_
