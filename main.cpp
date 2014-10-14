#include <unistd.h>
#include <cstdio>
#include "x.hpp"
#include "rectangle.hpp"
#include "options.hpp"

void printSelection( bool cancelled, int x, int y, int w, int h, int window ) {
    printf( "X=%i\n", x );
    printf( "Y=%i\n", y );
    printf( "W=%i\n", w );
    printf( "H=%i\n", h );
    printf( "G=%ix%i", w, h );
    if ( x >= 0 ) {
        printf( "+%i", x );
    } else {
        // Negative is already included
        printf( "%i", x );
    }
    if ( y >= 0 ) {
        printf( "+%i", y );
    } else {
        // Negative is already included
        printf( "%i", y );
    }
    printf( "\n" );
    printf( "ID=%i\n", window );
    if ( cancelled ) {
        printf( "Cancel=true\n" );
    } else {
        printf( "Cancel=false\n" );
    }
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
    int err = options->parseOptions( argc, argv );
    if ( err ) {
        return err;
    }
    int state = 0;
    bool running = true;
    slop::Rectangle* selection = NULL;
    Window window = None;
    Window windowmemory = None;
    std::string xdisplay = options->m_xdisplay;
    int padding = options->m_padding;
    int borderSize = options->m_borderSize;
    int tolerance = options->m_tolerance;
    float r = options->m_red;
    float g = options->m_green;
    float b = options->m_blue;
    float a = options->m_alpha;
    bool highlight = options->m_highlight;
    bool keyboard = options->m_keyboard;
    bool decorations = options->m_decorations;
    timespec start, time;
    int cx = 0;
    int cy = 0;
    int xmem = 0;
    int ymem = 0;
    int wmem = 0;
    int hmem = 0;
    int minimumsize = options->m_minimumsize;
    int maximumsize = options->m_maximumsize;

    // First we set up the x interface and grab the mouse,
    // if we fail for either we exit immediately.
    err = xengine->init( xdisplay.c_str() );
    if ( err ) {
        printSelection( true, 0, 0, 0, 0, None );
        return err;
    }
    err = xengine->grabCursor( slop::Cross );
    if ( err ) {
        printSelection( true, 0, 0, 0, 0, None );
        return err;
    }
    if ( keyboard ) {
        err = xengine->grabKeyboard();
        // We shouldn't error out from failing to grab the keyboard.
        //if ( err ) {
            //printSelection( true, 0, 0, 0, 0 );
            //return err;
        //}
    }
    clock_gettime( CLOCK_REALTIME, &start );
    while ( running ) {
        clock_gettime( CLOCK_REALTIME, &time );
        // "ticking" the xengine makes it process all queued events.
        xengine->tick();
        // If the user presses any key on the keyboard, exit the application.
        // Make sure at least options->m_gracetime has passed before allowing canceling
        double timei = double( time.tv_sec*1000000000L + time.tv_nsec )/1000000000.f;
        double starti = double( start.tv_sec*1000000000L + start.tv_nsec )/1000000000.f;
        if ( timei - starti > options->m_gracetime ) {
            if ( ( xengine->anyKeyPressed() && keyboard ) || xengine->mouseDown( 3 ) ) {
                printSelection( true, 0, 0, 0, 0, None );
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
                    window = xengine->m_hoverWindow;
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
                printSelection( false, x, y, w, h, window );
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
    delete options;
    // Sleep for 0.05 seconds to ensure everything was cleaned up. (Without this, slop's window often shows up in screenshots.)
    usleep( 50000 );
    // If we canceled the selection, return error.
    if ( state == -1 ) {
        return 1;
    }
    return 0;
}
