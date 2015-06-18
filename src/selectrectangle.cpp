/* selectrectangle.cpp: Handles creating rectangles on the screen.
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
#include "selectrectangle.hpp"

bool slop::isSelectRectangleSupported() {
    int event_base;
    int error_base;
    return XShapeQueryExtension( xengine->m_display, &event_base, &error_base );
}

slop::SelectRectangle::~SelectRectangle() {
}

void slop::SelectRectangle::update( double dt ) {
}

void slop::SelectRectangle::setGeo( int sx, int sy, int ex, int ey ) {
    fprintf( stderr, "Tried to use a class function that's meant to be overridden!\n");
}
