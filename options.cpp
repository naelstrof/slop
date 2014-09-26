#include "options.hpp"

slop::Options* options = new slop::Options();

slop::Options::Options() {
    m_version = "v2.0.2";
    m_borderSize = 10;
    m_padding = 0;
    m_xdisplay = ":0";
    m_tolerance = 4;
    m_red = 0;
    m_green = 0;
    m_blue = 0;
    m_gracetime = 0.4;
    m_keyboard = true;
    m_decorations = true;
}

void slop::Options::printHelp() {
    printf( "Usage: slop [options]\n" );
    printf( "Print user selected region to stdout. Pressing keys or right-clicking cancels selection.\n" );
    printf( "\n" );
    printf( "Options\n" );
    printf( "    -h, --help                     Show this message.\n" );
    printf( "    -nkb, --nokeyboard             Disables the ability to cancel selections with the keyboard.\n" );
    printf( "    -b=INT, --bordersize=INT       Set selection rectangle border size.\n" );
    printf( "    -p=INT, --padding=INT          Set padding size for selection.\n" );
    printf( "    -t=INT, --tolerance=INT        How far in pixels the mouse can move after clicking and still be detected\n" );
    printf( "                                   as a normal click. Setting to zero will disable window selections.\n" );
    printf( "    -x=STRING, --xdisplay=STRING   Set x display (STRING must be hostname:number.screen_number format)\n" );
    printf( "    -c=COLOR, --color=COLOR        Set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT\n" );
    printf( "    -g=FLOAT, --gracetime=FLOAT    Set the amount of time before slop will check for keyboard cancellations\n" );
    printf( "                                   in seconds.\n" );
    printf( "    -nd, --nodecorations           attempts to remove decorations from window selections.\n" );
    printf( "    -v, --version                  prints version.\n" );
    printf( "\n" );
    printf( "Examples\n" );
    printf( "    $ # Gray, thick border for maximum visiblity.\n" );
    printf( "    $ slop -b=20 -c=0.5,0.5,0.5\n" );
    printf( "\n" );
    printf( "    $ # Remove window decorations.\n" );
    printf( "    $ slop -nd\n" );
    printf( "\n" );
    printf( "    $ # Disable window selections. Useful for selecting individual pixels.\n" );
    printf( "    $ slop -t=0\n" );
}

int slop::Options::parseOptions( int argc, char** argv ) {
    // Simple command parsing. Just uses sscanf to read each argument.
    // It looks complicated because you have to have spaces for delimiters for sscanf.
    for ( int i=0; i<argc; i++ ) {
        std::string arg = argv[i];
        if ( matches( arg, "-b=", "--bordersize=" ) ) {
            int err = parseInt( arg, &m_borderSize );
            if ( err ) {
                return 1;
            }
            if ( m_borderSize < 0 ) {
                m_borderSize = 0;
            }
        } else if ( matches( arg, "-p=", "--padding=" ) ) {
            int err = parseInt( arg, &m_padding );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-c=", "--color=" ) ) {
            int err = parseColor( arg, &m_red, &m_green, &m_blue );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-t=", "--tolerance=" ) ) {
            int err = parseInt( arg, &m_tolerance );
            if ( err ) {
                return 1;
            }
            if ( m_tolerance < 0 ) {
                m_tolerance = 0;
            }
        } else if ( matches( arg, "-g=", "--gracetime=" ) ) {
            int err = parseFloat( arg, &m_gracetime );
            if ( err ) {
                return 1;
            }
            if ( m_gracetime < 0 ) {
                m_gracetime = 0;
            }
        } else if ( matches( arg, "-x=", "--xdisplay=" ) ) {
            int err = parseString( arg, &m_xdisplay );
            if ( err ) {
                return 1;
            }
        } else if ( matches( arg, "-nkb", "--nokeyboard" ) ) {
            m_keyboard = false;
        } else if ( matches( arg, "-nd", "--nodecorations" ) ) {
            m_decorations = false;
        } else if ( matches( arg, "-h", "--help" ) ) {
            printHelp();
            return 2;
        } else if ( matches( arg, "-v", "--version" ) ) {
            printf( "slop %s\n", m_version.c_str() );
            return 2;
        } else {
            if ( i == 0 ) {
                continue;
            }
            fprintf( stderr, "Error: Unknown argument %s\n", argv[i] );
            fprintf( stderr, "Try -h or --help for help.\n" );
            return 1;
        }
    }
    return 0;
}

int slop::Options::parseInt( std::string arg, int* returnInt ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %i", x, returnInt );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=INT\n", x );
        fprintf( stderr, "Example: %s=10 or %s=-12\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

int slop::Options::parseFloat( std::string arg, float* returnFloat ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %f", x, returnFloat );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=FLOAT\n", x );
        fprintf( stderr, "Example: %s=3.14 or %s=-99\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

bool slop::Options::matches( std::string arg, std::string shorthand, std::string longhand ) {
    if ( arg.substr( 0, shorthand.size() ) == shorthand ) {
        if ( arg == shorthand || shorthand[shorthand.length()-1] == '=' ) {
            return true;
        }
    }
    if ( longhand.size() && arg.substr( 0, longhand.size() ) == longhand ) {
        if ( arg == longhand || longhand[longhand.length()-1] == '=' ) {
            return true;
        }
    }
    return false;
}

int slop::Options::parseString( std::string arg, std::string* returnString ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    if ( find != copy.npos ) {
        copy.at( find ) = ' ';
    }
    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    char* y = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %s", x, y );
    if ( num != 2 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=STRING\n", x );
        fprintf( stderr, "Example: %s=:0 or %s=hostname:0.1\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        delete[] y;
        return 1;
    }
    *returnString = y;
    delete[] x;
    delete[] y;
    return 0;
}

int slop::Options::parseColor( std::string arg, float* r, float* g, float* b ) {
    std::string copy = arg;
    int find = copy.find( "=" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "," );
    }

    // Just in case we error out, grab the actual argument name into x.
    char* x = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %f %f %f", x, r, g, b );
    if ( num != 4 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=COLOR\n", x );
        fprintf( stderr, "Example: %s=0,0,0 or %s=0.7,0.2,1\n", x, x );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] x;
        return 1;
    }
    delete[] x;
    return 0;
}

int slop::Options::parseGeometry( std::string arg, int* x, int* y, int* w, int* h ) {
    std::string copy = arg;
    // Replace the first =, all x's and +'s with spaces.
    int find = copy.find( "=" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "x" );
    }
    find = copy.find( "+" );
    while( find != copy.npos ) {
        copy.at( find ) = ' ';
        find = copy.find( "+" );
    }

    // Just in case we error out, grab the actual argument name into x.
    char* foo = new char[ arg.size() ];
    int num = sscanf( copy.c_str(), "%s %d %d %d %d", foo, w, h, x, y );
    if ( num != 5 ) {
        fprintf( stderr, "Error parsing command arguments near %s\n", arg.c_str() );
        fprintf( stderr, "Usage: %s=GEOMETRY\n", foo );
        fprintf( stderr, "Example: %s=1920x1080+0+0 or %s=256x256+100+-200\n", foo, foo );
        fprintf( stderr, "Try -h or --help for help.\n" );
        delete[] foo;
        return 1;
    }
    delete[] foo;
    return 0;
}
