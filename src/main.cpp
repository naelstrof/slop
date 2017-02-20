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

using namespace slop;

SlopOptions* getOptions( Options& options ) {
    SlopOptions* foo = new SlopOptions();
    options.getFloat("bordersize", 'b', foo->borderSize);
    options.getFloat("padding", 'p', foo->padding);
    options.getFloat("tolerance", 't', foo->tolerance);
    glm::vec4 color = glm::vec4( foo->r, foo->g, foo->b, foo->a );
    options.getColor("color", 'c', color);
    options.getBool("nokeyboard", 'k', foo->nokeyboard);
    options.getString( "xdisplay", 'x', foo->xdisplay );
    options.getString( "shader", 's', foo->shader );
    foo->r = color.r;
    foo->g = color.g;
    foo->b = color.b;
    foo->a = color.a;
    options.getBool("highlight", 'l', foo->highlight);
    options.getInt("nodecorations", 'n', foo->nodecorations);
    return foo;
}

std::string formatOutput( std::string input, SlopSelection selection, bool cancelled ) {
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
                case 'c':
                case 'C': output << cancelled; break;
                case 'h':
                case 'H': output << round(selection.h); break;
                case 'g':
                case 'G': output << round(selection.w) << "x" << round(selection.h)
                          << "+" << round(selection.x) << "+" << round(selection.y); break;
                case 'i':
                case 'I': output << selection.id; break;
                case '%': output << "%"; break;
                default: throw new std::invalid_argument( std::string()+"Expected x, y, w, h, g, i, c, or % after % in format. Got `" + input[i+1] + "`." );
             }
            i++;
            continue;
        }
        output << input[i];
    }
    return output.str();
}

void printHelp() {
    std::cout << "slop v5.3.21\n";
    std::cout << "\n";
    std::cout << "Copyright (C) 2017 Dalton Nell, Slop Contributors\n";
    std::cout << "(https://github.com/naelstrof/slop/graphs/contributors)\n";
    std::cout << "Usage: slop [options]\n";
    std::cout << "\n";
    std::cout << "slop (Select Operation) is an application that queries for a selection from the\n";
    std::cout << "user and prints the region to stdout.\n";
    std::cout << "\n";
    std::cout << "-h, --help                    Print help and exit\n";
    std::cout << "-v, --version                 Print version and exit\n";
    std::cout << "Options\n";
    std::cout << "  -x, --xdisplay=hostname:number.screen_number\n";
    std::cout << "                                Sets the x display.\n";
	std::cout << "  -k, --nokeyboard              Disables the ability to cancel selections with\n";
	std::cout << "                                  the keyboard.  (default=off)\n";
	std::cout << "  -b, --bordersize=FLOAT        Set the selection rectangle's thickness.\n";
	std::cout << "                                  (default=`1')\n";
	std::cout << "  -p, --padding=FLOAT           Set the padding size of the selection. Can be\n";
	std::cout << "                                  negative.  (default=`0')\n";
	std::cout << "  -t, --tolerance=FLOAT         How far in pixels the mouse can move after\n";
	std::cout << "                                  clicking and still be detected as a normal\n";
	std::cout << "                                  click instead of a click and drag. Setting\n";
	std::cout << "                                  this to 0 will disable window selections.\n";
	std::cout << "                                  Alternatively setting it to 999999 would.\n";
	std::cout << "                                  only allow for window selections.\n";
	std::cout << "                                  (default=`2')\n";
	std::cout << "  -c, --color=FLOAT,FLOAT,FLOAT,FLOAT\n";
	std::cout << "                                Set the selection rectangle's color. Supports\n";
	std::cout << "                                  RGB or RGBA values.\n";
	std::cout << "                                  (default=`0.5,0.5,0.5,1')\n";
	std::cout << "  -n, --nodecorations=INT       Attempt to select child windows in order to\n";
	std::cout << "                                  avoid window decorations. Setting this to\n";
    std::cout << "                                  1 will enable a light attempt to\n";
    std::cout << "                                  remove decorations. Setting this to 2 will\n";
    std::cout << "                                  enable aggressive decoration removal.\n";
    std::cout << "                                  (default=`0')\n";
	std::cout << "  -q, --quiet                   Disable any unnecessary cerr output. Any\n";
	std::cout << "                                  warnings simply won't print.\n";
	std::cout << "  -l, --highlight               Instead of outlining selections, slop\n";
	std::cout << "                                  highlights it. This is only useful when\n";
	std::cout << "                                  --color is set to a transparent color.\n";
	std::cout << "                                  (default=off)\n";
	std::cout << "  -r, --shader=STRING           Sets the shader to load and use from\n";
	std::cout << "                                  ~/.config/slop/\n";
	std::cout << "  -f, --format=STRING           Set the output format string. Format specifiers\n";
	std::cout << "                                  are %x, %y, %w, %h, %i, %g, and %c.\n";
	std::cout << "                                  (default=`%g\n')\n";
	std::cout << "Examples\n";
	std::cout << "    $ # Gray, thick, transparent border for maximum visiblity.\n";
	std::cout << "    $ slop -b 20 -c 0.5,0.5,0.5,0.8\n";
	std::cout << "\n";
	std::cout << "    $ # Remove window decorations.\n";
	std::cout << "    $ slop --nodecorations\n";
	std::cout << "\n";
	std::cout << "    $ # Disable window selections. Useful for selecting individual pixels.\n";
	std::cout << "    $ slop -t 0\n";
	std::cout << "\n";
	std::cout << "    $ # Classic Windows XP selection.\n";
	std::cout << "    $ slop -l -c 0.3,0.4,0.6,0.4\n";
	std::cout << "\n";
	std::cout << "    $ # Read slop output for use in scripts.\n";
	std::cout << "    $ read -r X Y W H G ID < <(slop -f '%x %y %w %h %g %i')\n";
	std::cout << "\n";
	std::cout << "Tips\n";
	std::cout << "    * If you don't like a selection: you can cancel it by right-clicking\n";
	std::cout << "regardless of which options are enabled or disabled for slop.\n";
	std::cout << "    * If slop doesn't seem to select a window accurately, the problem could be\n";
	std::cout << "because of decorations getting in the way. Try enabling the --nodecorations\n";
	std::cout << "flag.\n";
}

int app( int argc, char** argv ) {
    // Options just validates all of our input from argv
    Options options( argc, argv );
    bool quiet = false;
    options.getBool( "quiet", 'q', quiet );
    bool help = false;
    if ( options.getBool( "help", 'h', help ) ) {
        printHelp();
        return 0;
    }
    if ( options.getBool( "version", 'v', help ) ) {
        std::cout << SLOP_VERSION << "\n";
        return 0;
    }
    // We then parse the options into something slop can understand.
    SlopOptions* parsedOptions = getOptions( options );

    // We want to validate our format option if we got one, we do that by just doing a dry run
    // on a fake selection.
    SlopSelection selection(0,0,0,0,0);
    std::string format;
    bool gotFormat = options.getString("format", 'f', format);
    if ( gotFormat ) {
        formatOutput( format, selection, false );
    }

    // Finally we do the real selection.
    bool cancelled = false;
    selection = SlopSelect(parsedOptions, &cancelled, quiet);
    
    // Here we're done with the parsed option data.
    delete parsedOptions;
    // We know if we cancelled or not
    if ( cancelled ) {
        if ( !quiet ) {
            std::cerr << "Selection was cancelled by keystroke or right-click.\n";
        }
        return 1;
    }
    // If we recieved a format option, we output the specified output.
    if ( gotFormat ) {
        std::cout << formatOutput( format, selection, cancelled );
        return 0;
    }
    std::cout << formatOutput( "%g\n", selection, cancelled );
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
