#include <unistd.h>
#include "x.hpp"

int main( int argc, char** argv ) {
    int state = 0;
    bool running = true;
    is::Rectangle* selection;
    is::Rectangle* windowselection = NULL;
    Window window = None;

    xengine->init( ":0" );
    xengine->grabCursor( is::Cross );
    while ( running ) {
        xengine->tick();
        if ( xengine->mouseDown( 3 ) ) {
            printf( "X: 0\n" );
            printf( "Y: 0\n" );
            printf( "W: 0\n" );
            printf( "H: 0\n" );
            printf( "User right-clicked. Canceled selection.\n" );
            state = -1;
            running = false;
        }
        switch ( state ) {
            default: {
                break;
            }
            case 0: {
                if ( window != xengine->m_hoverXWindow ) {
                    if ( windowselection ) {
                        xengine->removeRect( windowselection );
                    }
                    is::WindowRectangle t = xengine->m_hoverWindow;
                    windowselection = new is::Rectangle( t.m_x,
                                                         t.m_y,
                                                         t.m_width,
                                                         t.m_height,
                                                         10, 0 );
                    xengine->addRect( windowselection );
                    window = xengine->m_hoverXWindow;
                }
                if ( xengine->mouseDown( 1 ) ) {
                    if ( windowselection ) {
                        xengine->removeRect( windowselection );
                    }
                    state++;
                }
                break;
            }
            case 1: {
                selection = new is::Rectangle( xengine->m_mousex, xengine->m_mousey, 0, 0, 10, 0 );
                selection->setPos( xengine->m_mousex, xengine->m_mousey );
                xengine->addRect( selection );
                state++;
                break;
            }
            case 2: {
                if ( !xengine->mouseDown( 1 ) ) {
                    state++;
                    break;
                }
                selection->setDim( xengine->m_mousex - selection->m_x, xengine->m_mousey - selection->m_y );
                // x and y offsets can indicate if the selection is inside-out, which lets us know which kind of cursor we need.
                int x = selection->m_xoffset;
                int y = selection->m_yoffset;
                if ( x == 0 && y == 0) {
                    xengine->setCursor( is::LowerRightCorner );
                } else if ( x && y == 0 ) {
                    xengine->setCursor( is::LowerLeftCorner );
                } else if ( x == 0 && y ) {
                    xengine->setCursor( is::UpperRightCorner );
                } else {
                    xengine->setCursor( is::UpperLeftCorner );
                }

                break;
            }
            case 3: {
                int x = selection->m_x+selection->m_xoffset;
                int y = selection->m_y+selection->m_yoffset;
                int w = selection->m_width;
                int h = selection->m_height;
                xengine->removeRect( selection );
                running = false;
                if ( w || h || xengine->m_hoverXWindow == None ) {
                    printf( "X: %i\n", x );
                    printf( "Y: %i\n", y );
                    printf( "W: %i\n", w + 1 );
                    printf( "H: %i\n", h + 1 );
                    break;
                }
                is::WindowRectangle t = xengine->m_hoverWindow;
                x = t.m_x;
                y = t.m_y;
                w = t.m_width + t.m_border;
                h = t.m_height + t.m_border;
                printf( "X: %i\n", x );
                printf( "Y: %i\n", y );
                printf( "W: %i\n", w );
                printf( "H: %i\n", h );
                break;
            }
        }
        // No need to max out CPU--
        usleep( 1000 );
    }
    delete xengine;
    return 0;
}
