/* keyboard.hpp: Interfaces with x11 to grab key press information.
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

#ifndef N_KEYBOARD_H_
#define N_KEYBOARD_H_

#include "x.hpp"

class Keyboard {
private:
    X11* x11;
public:
    Keyboard( X11* x11 );
    ~Keyboard();
    bool getKey( KeySym key );
    bool anyKeyDown();
};

extern Keyboard* keyboard;

#endif // N_KEYBOARD_H_
