#ifndef IS_OPTIONS_H_

#include <string>
#include <cstdio>

namespace slop {

class Options {
public:
                Options();
    int         parseOptions( int argc, char** argv );
    void        printHelp();

    std::string m_version;
    int         m_borderSize;
    int         m_padding;
    int         m_tolerance;
    int         m_minimumsize;
    int         m_maximumsize;
    int         m_maxsize;
    float       m_red;
    float       m_green;
    float       m_blue;
    float       m_alpha;
    std::string m_xdisplay;
    float       m_gracetime;
    bool        m_keyboard;
    bool        m_decorations;
    bool        m_highlight;
private:
    int         parseInt( std::string arg, int* returnInt );
    int         parseFloat( std::string arg, float* returnFloat );
    int         parseString( std::string arg, std::string* returnString );
    int         parseGeometry( std::string arg, int* x, int* y, int* w, int* h );
    int         parseColor( std::string arg, float* r, float* g, float* b, float* a );
    bool        matches( std::string arg, std::string shorthand, std::string longhand );
};

}

extern slop::Options* options;

#endif // IS_OPTIONS_H_
