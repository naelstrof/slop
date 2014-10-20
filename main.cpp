/* main.cpp
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
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include "x.hpp"
#include "rectangle.hpp"
#include "cmdline.h"

int printSelection( std::string format, bool cancelled, int x, int y, int w, int h, int window ) {
    size_t pos = 0;
    while ( ( pos = format.find( "%", pos ) ) != std::string::npos ) {
        if ( pos + 1 > format.size() ) {
            fprintf( stderr, "Format error: %% found at the end of format string.\n", format[ pos + 1 ] );
            return 1;
        }
        std::stringstream foo;
        switch( format[ pos + 1 ] ) {
            case '%':
                format.replace( pos, 2, "%" );
                pos += 1;
                break;
            case 'x':
            case 'X':
                foo << x;
                format.replace( pos, 2, foo.str() );
                break;
            case 'y':
            case 'Y':
                foo << y;
                format.replace( pos, 2, foo.str() );
                break;
            case 'w':
            case 'W':
                foo << w;
                format.replace( pos, 2, foo.str() );
                break;
            case 'h':
            case 'H':
                foo << h;
                format.replace( pos, 2, foo.str() );
                break;
            case 'g':
            case 'G':
                foo << w << 'x' << h << '+' << x << '+' << y;
                format.replace( pos, 2, foo.str() );
                break;
            case 'i':
            case 'I':
                foo << window;
                format.replace( pos, 2, foo.str() );
                break;
            case 'c':
            case 'C':
                format.replace( pos, 2, cancelled ? "true" : "false" );
                break;
            default:
                fprintf( stderr, "Format error: %%%c is an unknown replacement identifier.\n", format[ pos + 1 ] );
                fprintf( stderr, "Valid replacements: %%x, %%y, %%w, %%h, %%i, %%c, %%.\n" );
                return 1;
                break;
        }
    }
    pos = 0;
    while ( ( pos = format.find( "\\", pos ) ) != std::string::npos ) {
        if ( pos + 1 > format.size() ) {
            break;
        }
        if ( format[ pos + 1 ] == 'n' ) {
            format.replace( pos, 2, "\n" );
        }
        pos = pos + 1;
    }
    printf( "%s", format.c_str() );
    return 0;
}

int parseColor( std::string arg, float* r, float* g, float* b, float* a ) {
    std::string copy = arg;
    int find = copy.find( "," );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "," );
    }

    // Just in case we didn't include an alpha value
    *a = 1;
    int num = sscanf( copy.c_str(), "%f %f %f %f", r, g, b, a );
    if ( num != 3 && num != 4 ) {
        fprintf( stderr, "Error parsing color %s\n", arg.c_str() );
        return 1;
    }
    return 0;
}

void constrain( int sx, int sy, int ex, int ey, int padding, int minimumsize, int maximumsize, int* rsx, int* rsy, int* rex, int* rey ) {
    if ( minimumsize > maximumsize && maximumsize > 0 ) {
        fprintf( stderr, "Error: minimumsize is greater than maximumsize.\n" );
        exit( 1 );
    }
    int x = std::min( sx, ex );
    int y = std::min( sy, ey );
    // We add one to make sure we select the pixel under the mouse.
    int w = std::max( sx, ex ) - x + 1;
    int h = std::max( sy, ey ) - y + 1;
    // Make sure we don't turn inside out...
    if ( w + padding*2 >= 0 ) {
        x -= padding;
        w += padding*2;
    }
    if ( h + padding*2 >= 0 ) {
        y -= padding;
        h += padding*2;
    }
    if ( w < minimumsize ) {
        int diff = minimumsize - w;
        w = minimumsize;
        x -= diff/2;
    }
    if ( h < minimumsize ) {
        int diff = minimumsize - h;
        h = minimumsize;
        y -= diff/2;
    }
    if ( maximumsize > 0 ) {
        if ( w > maximumsize ) {
            int diff = w;
            w = maximumsize;
            x += diff/2 - maximumsize/2;
        }
        if ( h > maximumsize ) {
            int diff = h;
            h = maximumsize;
            y += diff/2 - maximumsize/2;
        }
    }
    // Center around mouse if we have a fixed size.
    if ( maximumsize == minimumsize && w == maximumsize && h == maximumsize ) {
        x = ex - maximumsize/2;
        y = ey - maximumsize/2;
    }
    *rsx = x;
    *rsy = y;
    *rex = x + w;
    *rey = y + h;
}

int main( int argc, char** argv ) {
    gengetopt_args_info options;
    int err = cmdline_parser( argc, argv, &options );
    if ( err ) {
        return err;
    }
    int state = 0;
    bool running = true;
    slop::Rectangle* selection = NULL;
    Window window = None;
    Window windowmemory = None;
    std::string xdisplay = options.xdisplay_arg;
    int padding = options.padding_arg;
    int borderSize = options.bordersize_arg;
    int tolerance = options.tolerance_arg;
    float r, g, b, a;
    parseColor( options.color_arg, &r, &g, &b, &a );
    float gracetime = atof( options.gracetime_arg );
    bool highlight = options.highlight_flag;
    bool keyboard = !options.nokeyboard_flag;
    bool decorations = !options.nodecorations_flag;
    timespec start, time;
    int cx = 0;
    int cy = 0;
    int xmem = 0;
    int ymem = 0;
    int wmem = 0;
    int hmem = 0;
    int minimumsize = options.min_arg;
    int maximumsize = options.max_arg;
    std::string format = options.format_arg;
    cmdline_parser_free( &options );

    // First we set up the x interface and grab the mouse,
    // if we fail for either we exit immediately.
    err = xengine->init( xdisplay.c_str() );
    if ( err ) {
        printSelection( format, true, 0, 0, 0, 0, None );
        return err;
    }
    err = xengine->grabCursor( slop::Cross );
    if ( err ) {
        printSelection( format, true, 0, 0, 0, 0, None );
        return err;
    }
    if ( keyboard ) {
        err = xengine->grabKeyboard();
        if ( err ) {
            fprintf( stderr, "Warning: Failed to grab the keyboard. This is non-fatal, keyboard presses might fall through to other applications.\n" );
        }
    }
    clock_gettime( CLOCK_REALTIME, &start );
    while ( running ) {
        clock_gettime( CLOCK_REALTIME, &time );
        // "ticking" the xengine makes it process all queued events.
        xengine->tick();
        // If the user presses any key on the keyboard, exit the application.
        // Make sure at least gracetime has passed before allowing canceling
        double timei = double( time.tv_sec*1000000000L + time.tv_nsec )/1000000000.f;
        double starti = double( start.tv_sec*1000000000L + start.tv_nsec )/1000000000.f;
        if ( timei - starti > gracetime ) {
            if ( ( xengine->anyKeyPressed() && keyboard ) || xengine->mouseDown( 3 ) ) {
                printSelection( format, true, 0, 0, 0, 0, None );
                fprintf( stderr, "User pressed key. Canceled selection.\n" );
                state = -1;
                running = false;
            }
        }
        // Our adorable little state manager will handle what state we're in.
        switch ( state ) {
            default: {
                break;
            }
            case 0: {
                // If xengine has found a window we're hovering over (or if it changed)
                // create a rectangle around it so the user knows he/she can click on it.
                // --but only if the user wants us to
                if ( window != xengine->m_hoverWindow && tolerance > 0 ) {
                    slop::WindowRectangle t;
                    t.setGeometry( xengine->m_hoverWindow, decorations );
                    t.applyPadding( padding );
                    t.applyMinMaxSize( minimumsize, maximumsize );
                    // Make sure we only apply offsets to windows that we've forcibly removed decorations on.
                    if ( !selection ) {
                        selection = new slop::Rectangle( t.m_x,
                                                         t.m_y,
                                                         t.m_x + t.m_width,
                                                         t.m_y + t.m_height,
                                                         borderSize,
                                                         highlight,
                                                         r, g, b, a );
                    } else {
                        selection->setGeo( t.m_x, t.m_y, t.m_x + t.m_width, t.m_y + t.m_height );
                    }
                    //window = xengine->m_hoverWindow;
                    // Since WindowRectangle can select different windows depending on click location...
                    window = t.getWindow();
                }
                // If the user clicked we move on to the next state.
                if ( xengine->mouseDown( 1 ) ) {
                    state++;
                }
                break;
            }
            case 1: {
                // Set the mouse position of where we clicked, used so that click tolerance doesn't affect the rectangle's position.
                cx = xengine->m_mousex;
                cy = xengine->m_mousey;
                // Also remember where the original selection was
                if ( selection ) {
                    xmem = selection->m_x;
                    ymem = selection->m_y;
                    wmem = selection->m_width;
                    hmem = selection->m_height;
                } else {
                    xmem = cx;
                    ymem = cy;
                }
                state++;
                break;
            }
            case 2: {
                // It's possible that our selection doesn't exist still, lets make sure it actually gets created here.
                if ( !selection ) {
                    int sx, sy, ex, ey;
                    constrain( cx, cy, xengine->m_mousex, xengine->m_mousey, padding, minimumsize, maximumsize, &sx, &sy, &ex, &ey );
                    selection = new slop::Rectangle( sx,
                                                     sy,
                                                     ex,
                                                     ey,
                                                     borderSize,
                                                     highlight,
                                                     r, g, b, a );
                }
                windowmemory = window;
                // If the user has let go of the mouse button, we'll just
                // continue to the next state.
                if ( !xengine->mouseDown( 1 ) ) {
                    state++;
                    break;
                }
                // Check to make sure the user actually wants to drag for a selection before moving things around.
                int w = xengine->m_mousex - cx;
                int h = xengine->m_mousey - cy;
                if ( ( std::abs( w ) < tolerance && std::abs( h ) < tolerance ) ) {
                    // We make sure the selection rectangle stays on the window we had selected
                    selection->setGeo( xmem, ymem, xmem + wmem, ymem + hmem );
                    xengine->setCursor( slop::Left );
                    // Make sure
                    window = windowmemory;
                    continue;
                }
                // If we're not selecting a window.
                windowmemory = window;
                window = None;
                // We also detect which way the user is pulling and set the mouse icon accordingly.
                bool x = cx > xengine->m_mousex;
                bool y = cy > xengine->m_mousey;
                if ( selection->m_width <= 1 && selection->m_height <= 1 || ( minimumsize == maximumsize && minimumsize != 0 && maximumsize != 0 ) ) {
                    xengine->setCursor( slop::Cross );
                } else if ( !x && !y ) {
                    xengine->setCursor( slop::LowerRightCorner );
                } else if ( x && !y ) {
                    xengine->setCursor( slop::LowerLeftCorner );
                } else if ( !x && y ) {
                    xengine->setCursor( slop::UpperRightCorner );
                } else if ( x && y ) {
                    xengine->setCursor( slop::UpperLeftCorner );
                }
                // Apply padding and minimum size adjustments.
                int sx, sy, ex, ey;
                constrain( cx, cy, xengine->m_mousex, xengine->m_mousey, padding, minimumsize, maximumsize, &sx, &sy, &ex, &ey );
                // Set the selection rectangle's dimensions to mouse movement.
                selection->setGeo( sx, sy, ex, ey );
                break;
            }
            case 3: {
                int x, y, w, h;
                // Exit the utility after this state runs once.
                running = false;
                // We pull the dimensions and positions from the selection rectangle.
                // The selection rectangle automatically converts the positions and
                // dimensions to absolute coordinates when we set them earilier.
                x = selection->m_x;
                y = selection->m_y;
                w = selection->m_width;
                h = selection->m_height;
                // Delete the rectangle, which will remove it from the screen.
                delete selection;
                // Print the selection :)
                printSelection( format, false, x, y, w, h, window );
                break;
            }
        }
        // This sleep is required because drawing the rectangles is a very expensive task that acts really weird with Xorg when called as fast as possible.
        // 0.01 seconds
        usleep( 10000 );
    }
    xengine->releaseCursor();
    xengine->releaseKeyboard();
    // Try to process any last-second requests.
    //xengine->tick();
    // Clean up global classes.
    delete xengine;
    // Sleep for 0.05 seconds to ensure everything was cleaned up. (Without this, slop's window often shows up in screenshots.)
    usleep( 50000 );
    // If we canceled the selection, return error.
    if ( state == -1 ) {
        return 1;
    }
    return 0;
}
