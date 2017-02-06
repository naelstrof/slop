#ifndef N_SLOP_H_
#define N_SLOP_H_

#include <string>

#include "window.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "rectangle.hpp"
#include "slopstates.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "resource.hpp"

class SlopState;

class SlopOptions {
public:
    SlopOptions();
    float borderSize;
    float padding;
    bool highlight;
    std::string shader;
    float r;
    float g;
    float b;
    float a;
};

class SlopSelection {
public:
    SlopSelection( float x, float y, float w, float h );
    float x;
    float y;
    float w;
    float h;
};

class SlopMemory {
private:
    SlopState* state;
    SlopState* nextState;
public:
    SlopMemory( SlopOptions* options );
    ~SlopMemory();
    bool running;
    Rectangle* rectangle;
    void setState( SlopState* state );
    void update( double dt );
    void draw( glm::mat4& matrix );
};

SlopSelection SlopSelect( SlopOptions* options = NULL, bool* cancelled = NULL );

#endif // N_SLOP_H_
