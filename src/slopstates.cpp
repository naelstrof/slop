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
    memory.rectangle->draw( matrix );
}
void slop::SlopState::setPoints( SlopMemory& memory, compensation comp ) {
    memory.rectangle->setPoints( startPoint + comp.Start,
                                 mouse->getMousePos() + comp.Mouse );
}
slop::SlopState::compensation slop::SlopState::determineCompensation(){
    /*
     Determines compensation for the rectangle according to
     the following visualization. S is the starting point
     and each quadrant represents the 4 different positions
     of the cursor respective to the starting point.

        +--------------+--------------+
        |              |              |
        | Start.x = xm | Start.x = 0  |
        | Start.y = ym | Start.y = ym |
        | Mouse.x = 0  | Mouse.x = xm |
        | Mouse.y = 0  | Mouse.y = 0  |
        |              |              |
        +--------------S--------------+
        |              |              |
        | Start.x = 0  | Start.x = 0  |
        | Start.y = 0  | Start.y = 0  |
        | Mouse.x = xm | Mouse.x = xm |
        | Mouse.y = ym | Mouse.y = ym |
        |              |              |
        +--------------+--------------+

     xm and ym are 1 if the cursor is on either
     edge of the screen for each dimension
    */

    //these are true when on the edges of the screen
    bool xm = (mouse->getMousePos().x == 0 || mouse->getMousePos().x == WidthOfScreen(x11->screen)-1);
    bool ym = (mouse->getMousePos().y == 0 || mouse->getMousePos().y == HeightOfScreen(x11->screen)-1);
    
    // determine relative cursor position
    bool cursorAbove = startPoint.y > mouse->getMousePos().y;
    bool cursorLeft = startPoint.x > mouse->getMousePos().x;

    // intialize compensation vectors
    glm::vec2 cStart = glm::vec2(0, 0);
    glm::vec2 cMouse = glm::vec2(0, 0);

    // apply accordingly
    if ( cursorAbove ) {
        cStart.y = ym ? 0 : 1 ;
        if ( cursorLeft ) {
            cStart.x = xm ? 0 : 1 ;
        }
        else {
            cMouse.x = xm ? 0 : 1 ;
        }
    }
    else { // cursor below
        cMouse.x = xm ? 0 : 1 ;
        cMouse.y = ym ? 0 : 1 ;
    }
    compensation comp = { cStart, cMouse };
    return comp;
}

void slop::SlopStart::onEnter( SlopMemory& memory ) {
    setStartPos = false;
}
void slop::SlopStart::update( SlopMemory& memory, double dt ) {
    if ( mouse->getButton( 1 ) && !setStartPos ) {
        startPoint = mouse->getMousePos();
        setStartPos = true;
    }
    if ( setStartPos && glm::distance( startPoint, mouse->getMousePos() ) >= memory.tolerance ) {
        memory.setState( (SlopState*)new SlopStartDrag( startPoint ) );
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
    // redundant because it happens in upate() anyway
    //compensation comp = determineCompensation();
    //setPoints( memory, comp );
}
void slop::SlopStartDrag::update( SlopMemory& memory, double dt ) {

    compensation comp = determineCompensation();
    setPoints( memory, comp );
    setAngleCursor();

    if ( !mouse->getButton( 1 ) ) {
        memory.setState( (SlopState*)new SlopEndDrag() );
        return;
    }

    if ( keyboard->getKey(XK_space) ) {
        memory.setState( (SlopState*)new SlopStartMove( startPoint, mouse->getMousePos() ) );
        return;
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
void slop::SlopStartDrag::setAngleCursor() {
    // determine relative cursor position
    bool cursorAbove = startPoint.y > mouse->getMousePos().y;
    bool cursorLeft = startPoint.x > mouse->getMousePos().x;

    if ( cursorAbove ) {
        if ( cursorLeft ) {
            mouse->setCursor( XC_ul_angle );
        }
        else {
            mouse->setCursor( XC_ur_angle );
        }
    }
    else {
        if ( cursorLeft ) {
            mouse->setCursor( XC_ll_angle );
        }
        else {
            mouse->setCursor( XC_lr_angle );
       }
    }
}

void slop::SlopEndDrag::onEnter( SlopMemory& memory ) {
    memory.running = false;
}

slop::SlopStartMove::SlopStartMove( glm::vec2 oldPoint, glm::vec2 newPoint ) {
    // oldPoint is where drag was started and newPoint where move was
    startPoint = oldPoint;
    // This vector is the diagonal of the rectangle
    // it will be used to move the startPoint along with mousePos
    diagonal = newPoint - oldPoint;
}
void slop::SlopStartMove::onEnter( SlopMemory& memory ) {
    // redundant because of update()
    //compensation comp = determineCompensation();
    //setPoints( memory, comp );

    mouse->setCursor( XC_fleur );
}
void slop::SlopStartMove::update( SlopMemory& memory, double dt ) {
    // Unclear why it has to be - and not +
    startPoint = mouse->getMousePos() - diagonal;

    compensation comp = determineCompensation();
    setPoints( memory, comp );

    // space or mouse1 released, return to drag
    // if mouse1 is released then drag will end also
    if ( !keyboard->getKey(XK_space) or !mouse->getButton( 1) ) {
        memory.setState( (SlopState*) new SlopStartDrag(startPoint) );
    }
}