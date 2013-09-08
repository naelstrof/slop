#include <unistd.h>
#include "x.hpp"

int main( int argc, char** argv ) {
    int state = 0;
    bool running = true;
    is::Rectangle* selection;

    xengine->init( ":0" );
    xengine->grabCursor( is::Cross );
    while ( running ) {
        xengine->tick();
        switch ( state ) {
            case 0: {
                if ( xengine->mouseDown( 1 ) ) {
                    state++;
                }
                break;
            }
            case 1: {
                // Check to make sure we actually have a cursor position before creating the selection rectangle.
                if ( xengine->m_mousex == -1 ) {
                    break;
                }
                selection = new is::Rectangle( xengine->m_mousex, xengine->m_mousey, 1, 1, 10, 0 );
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
                printf( "X: %i\n", selection->m_x+selection->m_xoffset );
                printf( "Y: %i\n", selection->m_y+selection->m_yoffset );
                printf( "W: %i\n", selection->m_width + 1 );
                printf( "H: %i\n", selection->m_height + 1 );
                xengine->removeRect( selection );
                running = false;
                break;
            }
        }
        // No need to max out CPU--
        usleep( 1000 );
    }
    delete xengine;
    return 0;
}
