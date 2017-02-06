#ifndef N_SLOPSTATES_H_
#define N_SLOPSTATES_H_

#include "mouse.hpp"
#include "rectangle.hpp"
#include "slop.hpp"

class SlopMemory;

class SlopState {
public:
    virtual ~SlopState();
    virtual void onEnter( SlopMemory& memory );
    virtual void onExit( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopStart : SlopState {
public:
    virtual void update( SlopMemory& memory, double dt );
};

class SlopStartDrag : SlopState {
private:
    glm::vec2 startPoint;
public:
    SlopStartDrag( glm::vec2 point );
    virtual void onEnter( SlopMemory& memory );
    virtual void update( SlopMemory& memory, double dt );
    virtual void draw( SlopMemory& memory, glm::mat4 matrix );
};

class SlopEndDrag : SlopState {
public:
    virtual void onEnter( SlopMemory& memory );
};

#endif // N_SLOPSTATES_H_
