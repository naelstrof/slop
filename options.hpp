#ifndef IS_OPTIONS_H_

#include <string>
#include <cstdio>

namespace slrn {

class Options {
public:
                Options();
    int         parseOptions( int argc, char** argv );
    void        printHelp();

    int         m_borderSize;
    int         m_padding;
    int         m_tolerance;
    std::string m_xdisplay;
private:
    int         parseInt( std::string arg, int* returnInt );
    int         parseString( std::string arg, std::string* returnString );
    bool        matches( std::string arg, std::string shorthand, std::string longhand );
};

}

extern slrn::Options* options;

#endif // IS_OPTIONS_H_
