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

// Here we make some C-styled structs and function definitions, 
// allows other people to have a pure C interface to slop.

struct slop_options {
    //SlopOptions();
    int quiet;
    float border;
    float padding;
    float tolerance;
    int highlight;
    int noopengl;
    int nokeyboard;
    int nodecorations;
    char* shaders;
    float x_ratio;
    float y_ratio;
    float r;
    float g;
    float b;
    float a;
    char* xdisplay;
};

struct slop_selection {
    //SlopSelection( float x, float y, float w, float h, int id, bool cancelled );
    int cancelled;
    float x;
    float y;
    float w;
    float h;
// This is an X11 Window ID
    int id;
};

#ifdef __cplusplus
namespace slop {

class SlopOptions {
public:
    SlopOptions();
    bool quiet;
    float border;
    float padding;
    float tolerance;
    bool nodrag;
    bool highlight;
    bool noopengl;
    bool nokeyboard;
    bool nodecorations;
    char* shaders;
    float x_ratio;
    float y_ratio;
    float r;
    float g;
    float b;
    float a;
    char* xdisplay;
};

class SlopSelection {
public:
    SlopSelection( float x, float y, float w, float h, int id, bool cancelled );
    bool cancelled;
    float x;
    float y;
    float w;
    float h;
// This is an X11 Window ID
    int id;
};

SlopSelection SlopSelect( SlopOptions* options = NULL );

}

extern "C" struct slop_options slop_options_default();
extern "C" struct slop_selection slop_select( struct slop_options* options );

#else // __cplusplus

struct slop_options slop_options_default();
struct slop_selection slop_select( struct slop_options* options );

#endif

#endif // N_SLOP_H_
