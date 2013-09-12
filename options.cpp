#include "options.hpp"

slop::Options* options = new slop::Options();

slop::Options::Options() {
    m_borderSize = 10;
    m_padding = 0;
    m_xdisplay = ":0";
    m_tolerance = 4;
    m_red = 0;
    m_green = 0;
    m_blue = 0;
}

void slop::Options::printHelp() {
    printf( "Usage: slop [options]\n" );
    printf( "Print user selected region to stdout.\n" );
    printf( "\n" );
    printf( "options\n" );
    printf( "    -h, --help                     show this message.\n" );
    printf( "    -b=INT, --bordersize=INT       set selection rectangle border size.\n" );
    printf( "    -p=INT, --m_padding=INT        set m_padding size for selection.\n" );
    printf( "    -t=INT, --tolerance=INT        if you have a shaky mouse, increasing this value will make slop detect single clicks better. Rather than interpreting your shaky clicks as region selections.\n" );
    printf( "    -x=STRING, --xdisplay=STRING   set x display (STRING must be hostname:number.screen_number format)\n" );
    printf( "    -c=COLOR, --color=COLOR        set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT\n" );
    printf( "examples\n" );
    printf( "    slop -b=10 -x=:0 -p=-30 -t=4 -c=0.5,0.5,0.5\n" );
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
        } else if ( matches( arg, "-x=", "--xdisplay=" ) ) {
            int err = parseString( arg, &m_xdisplay );
            if ( err ) {
                return 1;
            }
        } else if ( arg == "-h" || arg == "--help" ) {
            printHelp();
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

bool slop::Options::matches( std::string arg, std::string shorthand, std::string longhand ) {
    if ( arg.substr( 0, shorthand.size() ) == shorthand ||
         arg.substr( 0, longhand.size() ) == longhand ) {
        return true;
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
