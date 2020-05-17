#include "slopstates.hpp"

using namespace slop;

slop::SlopMemory::SlopMemory( SlopOptions* options, Rectangle* rect ) {
    up = false;
    running = true;
    state = (SlopState*)new SlopStart();
    nextState = NULL;
    tolerance = options->tolerance;
    aspectRatio = glm::vec2(options->x_ratio, options->y_ratio);
    nodrag = options->nodrag;
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
    if ( (setStartPos && !mouse->getButton( 1 ) && !memory.nodrag) ||
         (setStartPos && memory.nodrag && memory.up && mouse->getButton( 1 ) )
       ) {
        memory.selectedWindow = mouse->hoverWindow;
        memory.setState( (SlopState*)new SlopEndDrag() );
    }
    if ( setStartPos && memory.nodrag && !mouse->getButton( 1 )) {
        memory.up = true;
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
    if ( memory.nodrag && !memory.up && !mouse->getButton( 1 ) ) {
        memory.up = true;
    }
    if ( memory.nodrag && memory.up && mouse->getButton( 1 ) ) {
        memory.setState( (SlopState*)new SlopEndDrag() );
    }

    // Query X server once only during update function
    glm::vec2 mousePos = mouse->getMousePos();

    // Determine which cursor to use
    char a = startPoint.y > mousePos.y;
    char b = startPoint.x > mousePos.x;
    char c = (a << 1) | b;

    // -1 to invert, 1 to leave as is
    glm::vec2 invert;
    switch ( c ) {
        case 0: mouse->setCursor( XC_lr_angle );
                invert = glm::vec2(1, 1);
                break;
        case 1: mouse->setCursor( XC_ll_angle );
                invert = glm::vec2(-1, 1);
                break;
        case 2: mouse->setCursor( XC_ur_angle );
                invert = glm::vec2(1, -1);
                break;
        case 3: mouse->setCursor( XC_ul_angle );
                invert = glm::vec2(-1, -1);
                break;
    }

    glm::vec2 widthHeight = mousePos - startPoint;
    float scale = glm::max(glm::abs(widthHeight.x) / memory.aspectRatio.x,
                           glm::abs(widthHeight.y) / memory.aspectRatio.y);

    glm::vec2 rectEndPoint;
    if (memory.aspectRatio == glm::vec2(0, 0)) {
        rectEndPoint = glm::vec2(mousePos);
    } else {
        rectEndPoint = glm::vec2(startPoint + memory.aspectRatio*scale*invert);
    }

    // Compensate for edges of screen, depending on the mouse position in relation to the start point.
    int lx = mousePos.x < startPoint.x;
    int ly = mousePos.y < startPoint.y;

    memory.rectangle->setPoints(startPoint+glm::vec2(1*lx,1*ly), rectEndPoint+glm::vec2(1*(!lx), 1*(!ly)));

    if ( !memory.nodrag && !mouse->getButton( 1 ) ) {
        memory.setState( (SlopState*)new SlopEndDrag() );
        return;
    }

    if ( keyboard ) {
        if ( keyboard->getKey(XK_space) ) {
            memory.setState( (SlopState*)new SlopStartMove(startPoint, rectEndPoint) );
            return;
        }
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

void slop::SlopEndDrag::onEnter( SlopMemory& memory ) {
        // Clip rectangle on edges of screen.
        // p1(x,y), p2(z,w) with default format as p2_x x p2_y+p1_x+p1_y
        glm::vec4 rect = memory.rectangle->getRect();
        glm::vec2 p1, p2;
        p1.x = glm::min(int(rect.x), WidthOfScreen(x11->screen));
        p1.x = glm::max(int(rect.x), 0);
        p1.y = glm::min(int(rect.y), HeightOfScreen(x11->screen));
        p1.y = glm::max(int(rect.y), 0);
        p2.x = glm::min(int(rect.x + rect.z), WidthOfScreen(x11->screen));
        p2.y = glm::min(int(rect.y + rect.w), HeightOfScreen(x11->screen));
        memory.rectangle->setPoints(p1, p2);

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


    int lx = mouse->getMousePos().x < startPoint.x;
    int ly = mouse->getMousePos().y < startPoint.y;
    memory.rectangle->setPoints(startPoint+glm::vec2(1*lx,1*ly), startPoint+diagonal+glm::vec2(1*(!lx), 1*(!ly)));

    // space or mouse1 released, return to drag
    // if mouse1 is released then drag will end also
    if ( !keyboard->getKey(XK_space) or (!mouse->getButton( 1 ) && !memory.nodrag) ) {
        memory.setState( (SlopState*) new SlopStartDrag(startPoint) );
    }
}
