/* main.cpp: parses options, runs slop, prints results.
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

#include <iostream>
#include <sstream>
#include "slop.hpp"
#include "options.hpp"

SlopOptions* getOptions( Options& options ) {
    SlopOptions* foo = new SlopOptions();
    options.getFloat("bordersize", 'b', foo->borderSize);
    options.getFloat("padding", 'p', foo->padding);
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    options.getColor("color", 'c', color);
    foo->r = color.r;
    foo->g = color.g;
    foo->b = color.b;
    foo->a = color.a;
    options.getBool("highlight", 'h', foo->highlight);
    return foo;
}

std::string formatOutput( std::string input, SlopSelection selection ) {
    std::stringstream output;
    for( unsigned int i=0;i<input.length();i++) {
        if ( input[i] == '%' ) {
            if ( input.length() <= i+1 ) {
                throw new std::invalid_argument( "Expected character after `%`, got END." );
            }
            switch( input[i+1] ) {
                case 'x':
                case 'X': output << round(selection.x); break;
                case 'y':
                case 'Y': output << round(selection.y); break;
                case 'w':
                case 'W': output << round(selection.w); break;
                case 'h':
                case 'H': output << round(selection.h); break;
                case 'g':
                case 'G': output << round(selection.w) << "x" << round(selection.h)
                          << "+" << round(selection.x) << "+" << round(selection.y); break;
                case '%': output << "%"; break;
                default: throw new std::invalid_argument( std::string()+"Expected x, y, w, h, g, or % after % in format. Got `" + input[i+1] + "`." );
             }
            i++;
            continue;
        }
        output << input[i];
    }
    return output.str();
}

int app( int argc, char** argv ) {
    // Options just validates all of our input from argv
    Options options( argc, argv );
    // We then parse the options into something slop can understand.
    SlopOptions* parsedOptions = getOptions( options );

    // We want to validate our format option if we got one, we do that by just doing a dry run
    // on a fake selection.
    SlopSelection selection(0,0,0,0);
    std::string format;
    bool gotFormat = options.getString("format", 'f', format);
    if ( gotFormat ) {
        formatOutput( format, selection );
    }

    // Finally we do the real selection.
    bool cancelled = false;
    selection = SlopSelect(parsedOptions, &cancelled);
    
    // Here we're done with the parsed option data.
    delete parsedOptions;
    // We know if we cancelled or not
    if ( cancelled ) {
        std::cerr << "Selection was cancelled by keystroke or right-click.\n";
        return 1;
    }
    // If we recieved a format option, we output the specified output.
    if ( gotFormat ) {
        std::cout << formatOutput( format, selection );
        return 0;
    }
    // Otherwise we default to an `eval` compatible format.
    std::cout << formatOutput( "X=%x\nY=%y\nW=%w\nH=%h\nG=%g\n", selection );
    return 0;
}

int main( int argc, char** argv ) {
    try {
        return app( argc, argv );
    } catch( std::exception* e ) {
        std::cerr << "Slop encountered an error:\n" << e->what() << "\n";
        return 1;
    } // let the operating system handle any other kind of exception.
    return 1;
}
