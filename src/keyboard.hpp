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

#define XK_MISCELLANY
#define XK_LATIN1 // for XK_space
#include <X11/keysymdef.h>
#include <X11/XKBlib.h>

#include "x.hpp"

namespace slop {

class Keyboard {
private:
    char deltaState[32];
    X11* x11;
    bool keyDown;
    bool mouseKeys;
    void zeroKey( char keys[32], KeySym key );
public:
    Keyboard( X11* x11 );
    ~Keyboard();
    void update();
    bool getKey( KeySym key );
    bool anyKeyDown();
};

extern Keyboard* keyboard;

}

#endif // N_KEYBOARD_H_
