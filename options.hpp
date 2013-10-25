#ifndef IS_OPTIONS_H_

#include <string>
#include <cstdio>

namespace slop {

class Options {
public:
                Options();
    int         parseOptions( int argc, char** argv );
    void        printHelp();

    int         m_borderSize;
    int         m_padding;
    int         m_tolerance;
    float       m_red;
    float       m_green;
    float       m_blue;
    std::string m_xdisplay;
    float       m_gracetime;
    bool        m_keyboard;
    bool        m_window;
private:
    int         parseInt( std::string arg, int* returnInt );
    int         parseFloat( std::string arg, float* returnFloat );
    int         parseString( std::string arg, std::string* returnString );
    int         parseColor( std::string arg, float* r, float* g, float* b );
    bool        matches( std::string arg, std::string shorthand, std::string longhand );
};

}

extern slop::Options* options;

#endif // IS_OPTIONS_H_
