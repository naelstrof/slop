#include "slopstates.hpp"

using namespace slop;

slop::SlopMemory::SlopMemory( SlopOptions* options, Rectangle* rect ) {
    running = true;
    state = (SlopState*)new SlopStart();
    nextState = NULL;
    tolerance = options->tolerance;
    nodecorations = options->nodecorations;
    rectangle = rect;
    selectedWindow = x11->root;
    state->onEnter( *this );
}

slop::SlopMemory::~SlopMemory() {
    delete state;
    if ( nextState ) {
        delete nextState;
    }
    delete rectangle;
}

void slop::SlopMemory::update( double dt ) {
    state->update( *this, dt );
    if ( nextState ) {
        state->onExit( *this );
        delete state;
        state = nextState;
        state->onEnter( *this );
        nextState = NULL;
    }
}

void slop::SlopMemory::setState( SlopState* state ) {
    if ( nextState ) {
        delete nextState;
    }
    nextState = state;
}

void slop::SlopMemory::draw( glm::mat4& matrix ) {
    state->draw( *this, matrix );
}


slop::SlopState::~SlopState() {
}
void slop::SlopState::onEnter( SlopMemory& memory ) {
}
void slop::SlopState::onExit( SlopMemory& memory ) {
}
void slop::SlopState::update( SlopMemory& memory, double dt ) {
}
void slop::SlopState::draw( SlopMemory& memory, glm::mat4 matrix ) {
}

void slop::SlopStart::onEnter( SlopMemory& memory ) {
    setStartPos = false;
}
void slop::SlopStart::update( SlopMemory& memory, double dt ) {
    if ( mouse->getButton( 1 ) && !setStartPos ) {
        startPos = mouse->getMousePos();
        setStartPos = true;
    }
    if ( setStartPos && glm::distance( startPos, mouse->getMousePos() ) >= memory.tolerance ) {
        memory.setState( (SlopState*)new SlopStartDrag( startPos ) );
    }
    if ( mouse->hoverWindow != None ) {
        glm::vec4 rect = getWindowGeometry( mouse->hoverWindow, memory.nodecorations );
        memory.rectangle->setPoints( glm::vec2( (float)rect.x, (float)rect.y ), glm::vec2( (float)rect.x+rect.z, (float)rect.y+rect.w ) );
    }
    if ( setStartPos && !mouse->getButton( 1 ) ) {
        memory.selectedWindow = mouse->hoverWindow;
        memory.setState( (SlopState*)new SlopEndDrag() );
    }
}

void slop::SlopStart::draw( SlopMemory& memory, glm::mat4 matrix ) {
    if ( memory.tolerance > 0 ) {
        memory.rectangle->draw( matrix );
    }
}

slop::SlopStartDrag::SlopStartDrag( glm::vec2 point ) {
    startPoint = point;
}

void slop::SlopStartDrag::onEnter( SlopMemory& memory ) {
    memory.rectangle->setPoints(startPoint, startPoint);
}

void slop::SlopStartDrag::update( SlopMemory& memory, double dt ) {
    
    //these are true when on the edges of the screen
    int xm = (mouse->getMousePos().x == 0 || mouse->getMousePos().x == WidthOfScreen(x11->screen)-1);
    int ym = (mouse->getMousePos().y == 0 || mouse->getMousePos().y == HeightOfScreen(x11->screen)-1);
    
    // set cursor and compensate edges somehow
    bool curserAbove = startPoint.y > mouse->getMousePos().y;
    bool cursorLeft = startPoint.x > mouse->getMousePos().x;

    if ( curserAbove ) {
        if ( cursorLeft ) {
            mouse->setCursor( XC_ul_angle );
            memory.rectangle->setPoints(startPoint+glm::vec2(1*xm,1*ym), mouse->getMousePos()+glm::vec2(0,0));        }
        else {
            mouse->setCursor( XC_ur_angle );
            memory.rectangle->setPoints(startPoint+glm::vec2(0,1*ym), mouse->getMousePos()+glm::vec2(1*xm,0));
        }
    }
    else {
        if ( cursorLeft ) {
            mouse->setCursor( XC_ll_angle );
            memory.rectangle->setPoints(startPoint+glm::vec2(0,0), mouse->getMousePos()+glm::vec2(1*xm,1*ym));
        }
        else {
            mouse->setCursor( XC_lr_angle );
            memory.rectangle->setPoints(startPoint+glm::vec2(0,0), mouse->getMousePos()+glm::vec2(1*xm,1*ym));
       }
    }

    if ( !mouse->getButton( 1 ) ) {
        memory.setState( (SlopState*)new SlopEndDrag() );
    }
    if ( keyboard ) {
        int arrows[2];
        arrows[0] = keyboard->getKey(XK_Down)-keyboard->getKey(XK_Up);
        arrows[1] = keyboard->getKey(XK_Right)-keyboard->getKey(XK_Left);
        if ( arrows[0] || arrows[1] ) {
            if ( repeatTimer == 0 || repeatTimer > .4 ) {
                startPoint.y += arrows[0]*multiplier;
                startPoint.x += arrows[1]*multiplier;
            }
            if ( repeatTimer > 1 ) {
                multiplier += dt*2;
            }
            repeatTimer += dt;
        } else {
            repeatTimer = 0;
            multiplier = 1;
        }
    }
}

void slop::SlopStartDrag::draw( SlopMemory& memory, glm::mat4 matrix ) {
    memory.rectangle->draw( matrix );
}

void slop::SlopEndDrag::onEnter( SlopMemory& memory ) {
    memory.running = false;
}
