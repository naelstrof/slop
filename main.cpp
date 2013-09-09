#include <unistd.h>
#include <cstdio>
#include "x.hpp"

int borderSize = 10;
int padding = 0;
std::string xserver = ":0";

void printHelp() {
    printf( "Usage: slrn [options]\n" );
    printf( "Print user selected region to stdout.\n" );
    printf( "\n" );
    printf( "options\n" );
    printf( "    -h, --help                     show this message.\n" );
    printf( "    -b=INT, --bordersize=INT       set selection rectangle border size.\n" );
    printf( "    -p=INT, --padding=INT          set padding size for selection.\n" );
    printf( "    -x=STRING, --xdisplay=STRING   set x display (STRING must be hostname:number.screen_number format)\n" );
    printf( "examples\n" );
    printf( "    slrn -b=10 -x=:0 -p=-30\n" );
}

int parseOptions( int argc, char** argv ) {
    for ( int i=0; i<argc; i++ ) {
        std::string arg = argv[i];
        if ( arg.substr( 0, 3 ) == "-b=" || arg.substr( 0, 13 ) == "--bordersize=" ) {
            int find = arg.find( "=" );
            if ( find != arg.npos ) {
                arg.at( find ) = ' ';
            }
            int num = sscanf( arg.c_str(), "%*s %i", &borderSize );
            if ( borderSize < 0 ) {
                borderSize = 0;
            }
            if ( num != 1 ) {
                printf( "Error parsing command arguments near %s\n", argv[i] );
                printf( "Usage: -b=INT or --bordersize=INT\n" );
                printf( "Example: -b=10 or --bordersize=12\n" );
                return 1;
            }
        } else if ( arg.substr( 0, 3 ) == "-p=" || arg.substr( 0, 10 ) == "--padding=" ) {
            int find = arg.find( "=" );
            if ( find != arg.npos ) {
                arg.at( find ) = ' ';
            }
            int num = sscanf( arg.c_str(), "%*s %i", &padding );
            if ( num != 1 ) {
                printf( "Error parsing command arguments near %s\n", argv[i] );
                printf( "Usage: -p=INT or --padding=INT\n" );
                printf( "Example: -p=0 or --padding=-12\n" );
                return 1;
            }
        } else if ( arg.substr( 0, 3 ) == "-x=" || arg.substr( 0, 11 ) == "--xdisplay=" ) {
            int find = arg.find( "=" );
            if ( find != arg.npos ) {
                arg.at( find ) = ' ';
            }
            char* x = new char[ arg.size() ];
            int num = sscanf( arg.c_str(), "%*s %s", x );
            if ( num != 1 ) {
                printf( "Error parsing command arguments near %s\n", argv[i] );
                printf( "Usage: -x=STRING or --xserver=STRING.\n" );
                printf( "Example: -x=:0 or --xserver=winston:1.3\n" );
                delete[] x;
                return 1;
            }
            xserver = x;
            delete[] x;
        } else if ( arg == "-h" || arg == "--help" ) {
            printHelp();
            return 2;
        } else {
            if ( i == 0 ) {
                continue;
            }
            printf( "Error: Unknown argument %s\n", argv[i] );
            printf( "Try -h or --help for help.\n" );
            return 1;
        }
    }
    return 0;
}

int main( int argc, char** argv ) {
    int err = parseOptions( argc, argv );
    if ( err ) {
        return err;
    }
    int state = 0;
    bool running = true;
    is::Rectangle* selection;
    is::Rectangle* windowselection = NULL;
    Window window = None;

    err = xengine->init( xserver.c_str() );
    if ( err ) {
        return err;
    }
    err = xengine->grabCursor( is::Cross );
    if ( err ) {
        return err;
    }
    while ( running ) {
        xengine->tick();
        if ( xengine->mouseDown( 3 ) ) {
            printf( "X=0\n" );
            printf( "Y=0\n" );
            printf( "W=0\n" );
            printf( "H=0\n" );
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
                    windowselection = new is::Rectangle( t.m_x - t.m_border,
                                                         t.m_y - t.m_border,
                                                         t.m_width + t.m_border,
                                                         t.m_height + t.m_border,
                                                         borderSize, padding );
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
                selection = new is::Rectangle( xengine->m_mousex, xengine->m_mousey, 0, 0, borderSize, padding );
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
                bool x = selection->m_flippedx;
                bool y = selection->m_flippedy;
                if ( !x && !y ) {
                    xengine->setCursor( is::LowerRightCorner );
                } else if ( x && !y ) {
                    xengine->setCursor( is::LowerLeftCorner );
                } else if ( !x && y ) {
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
                    printf( "X=%i\n", x );
                    printf( "Y=%i\n", y );
                    printf( "W=%i\n", w + 1 );
                    printf( "H=%i\n", h + 1 );
                    break;
                }
                is::WindowRectangle t = xengine->m_hoverWindow;
                x = t.m_x - padding - t.m_border;
                y = t.m_y - padding - t.m_border;
                w = t.m_width + t.m_border + padding*2;
                h = t.m_height + t.m_border + padding*2;
                printf( "X=%i\n", x );
                printf( "Y=%i\n", y );
                printf( "W=%i\n", w );
                printf( "H=%i\n", h );
                break;
            }
        }
        // No need to max out CPU--
        usleep( 1000 );
    }
    delete xengine;
    return 0;
}
