#include <unistd.h>
#include <cstdio>
#include "x.hpp"
#include "rectangle.hpp"
#include "options.hpp"

void printSelection( bool cancelled, int x, int y, int w, int h ) {
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
    if ( cancelled ) {
        printf( "Cancel=true\n" );
    } else {
        printf( "Cancel=false\n" );
    }
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
    std::string xdisplay = options->m_xdisplay;
    int padding = options->m_padding;
    int borderSize = options->m_borderSize;
    int tolerance = options->m_tolerance;
    float r = options->m_red;
    float g = options->m_green;
    float b = options->m_blue;
    bool keyboard = options->m_keyboard;
    bool decorations = options->m_decorations;
    timespec start, time;
    int cx = 0;
    int cy = 0;
    int xmem = 0;
    int ymem = 0;
    int wmem = 0;
    int hmem = 0;

    // First we set up the x interface and grab the mouse,
    // if we fail for either we exit immediately.
    err = xengine->init( xdisplay.c_str() );
    if ( err ) {
        printSelection( true, 0, 0, 0, 0 );
        return err;
    }
    err = xengine->grabCursor( slop::Cross );
    if ( err ) {
        printSelection( true, 0, 0, 0, 0 );
        return err;
    }
    if ( keyboard ) {
        err = xengine->grabKeyboard();
        if ( err ) {
            printSelection( true, 0, 0, 0, 0 );
            return err;
        }
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
                printSelection( true, 0, 0, 0, 0 );
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
                    // Make sure we only apply offsets to windows that we've forcibly removed decorations on.
                    if ( !selection ) {
                        selection = new slop::Rectangle( t.m_x,
                                                         t.m_y,
                                                         t.m_width,
                                                         t.m_height,
                                                         borderSize, padding,
                                                         r, g, b );
                    } else {
                        selection->setGeo( t.m_x, t.m_y, t.m_width, t.m_height );
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
                    selection = new slop::Rectangle( cx,
                                                     cy,
                                                     xengine->m_mousex - cx,
                                                     xengine->m_mousey - cy,
                                                     borderSize, padding,
                                                     r, g, b );
                }
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
                    selection->setGeo( xmem, ymem, wmem, hmem );
                    xengine->setCursor( slop::Left );
                    continue;
                }
                // We also detect which way the user is pulling and set the mouse icon accordingly.
                bool x = selection->m_flippedx;
                bool y = selection->m_flippedy;
                if ( selection->m_width == 0 && selection->m_height == 0 ) {
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
                // Set the selection rectangle's dimensions to mouse movement.
                // We use the function setDim since rectangles can't have negative widths,
                // and because the rectangles have borders and padding to worry about.
                selection->setGeo( cx, cy, w, h );
                break;
            }
            case 3: {
                int x, y, w, h;
                // Exit the utility after this state runs once.
                running = false;
                // We pull the dimensions and positions from the selection rectangle.
                // The selection rectangle automatically converts the positions and
                // dimensions to absolute coordinates when we set them earilier.
                x = selection->m_x+selection->m_xoffset;
                y = selection->m_y+selection->m_yoffset;
                w = selection->m_width;
                h = selection->m_height;
                // Delete the rectangle, which will remove it from the screen.
                delete selection;
                // Print the selection :)
                printSelection( false, x, y, w, h );
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
    // If we canceled the selection, return error.
    if ( state == -1 ) {
        return 1;
    }
    return 0;
}
