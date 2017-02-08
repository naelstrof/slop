#include "slopstates.hpp"

SlopState::~SlopState() {
}
void SlopState::onEnter( SlopMemory& memory ) {
}
void SlopState::onExit( SlopMemory& memory ) {
}
void SlopState::update( SlopMemory& memory, double dt ) {
}
void SlopState::draw( SlopMemory& memory, glm::mat4 matrix ) {
}

void SlopStart::onEnter( SlopMemory& memory ) {
    setStartPos = false;
}
void SlopStart::update( SlopMemory& memory, double dt ) {
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

void SlopStart::draw( SlopMemory& memory, glm::mat4 matrix ) {
    if ( memory.tolerance > 0 ) {
        memory.rectangle->draw( matrix );
    }
}

SlopStartDrag::SlopStartDrag( glm::vec2 point ) {
    startPoint = point;
}

void SlopStartDrag::onEnter( SlopMemory& memory ) {
    memory.rectangle->setPoints(startPoint, startPoint);
}

void SlopStartDrag::update( SlopMemory& memory, double dt ) {
    memory.rectangle->setPoints(startPoint, mouse->getMousePos());
    char a = startPoint.y > mouse->getMousePos().y;
    char b = startPoint.x > mouse->getMousePos().x;
    char c = (a << 1) | b;
    switch ( c ) {
        case 0: mouse->setCursor( XC_lr_angle ); break;
        case 1: mouse->setCursor( XC_ll_angle ); break;
        case 2: mouse->setCursor( XC_ur_angle ); break;
        case 3: mouse->setCursor( XC_ul_angle ); break;
    }
    if ( !mouse->getButton( 1 ) ) {
        memory.setState( (SlopState*)new SlopEndDrag() );
    }
}

void SlopStartDrag::draw( SlopMemory& memory, glm::mat4 matrix ) {
    memory.rectangle->draw( matrix );
}

void SlopEndDrag::onEnter( SlopMemory& memory ) {
    memory.running = false;
}
