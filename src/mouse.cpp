#include "mouse.hpp"

void Mouse::setButton( int button, int state ) {
    for (unsigned int i=0;i<buttons.size();i++ ) {
        if ( buttons[i].x == button ) {
            buttons[i].y = state;
            return;
        }
    }
    buttons.push_back(glm::ivec2(button,state));
}

int Mouse::getButton( int button ) {
    for (unsigned int i=0;i<buttons.size();i++ ) {
        if ( buttons[i].x == button ) {
            return buttons[i].y;
        }
    }
    return 0;
}

glm::vec2 Mouse::getMousePos() {
    Window root, child;
    int mx, my;
    int wx, wy;
    unsigned int mask;
    XQueryPointer( x11->display, x11->root, &root, &child, &mx, &my, &wx, &wy, &mask );
    return glm::vec2( mx, my );
}

void Mouse::setCursor( int cursor ) {
    if ( currentCursor == cursor ) {
        return;
    }
    XFreeCursor( x11->display, xcursor );
    xcursor = XCreateFontCursor( x11->display, cursor );
    XChangeActivePointerGrab( x11->display,
                              PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask,
                              xcursor, CurrentTime );
}

Mouse::Mouse(X11* x11) {
    this->x11 = x11;
    currentCursor = XC_cross;
    xcursor = XCreateFontCursor( x11->display, XC_cross );
    XGrabPointer( x11->display, x11->root, True,
                  PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask,
                  GrabModeAsync, GrabModeAsync, None, xcursor, CurrentTime );
}

Mouse::~Mouse() {
	XUngrabPointer( x11->display, CurrentTime );
}

void Mouse::update() {
    XEvent event;
    while ( XCheckTypedEvent( x11->display, ButtonPress, &event ) ) {
		setButton( event.xbutton.button, 1 );
	}
    while ( XCheckTypedEvent( x11->display, ButtonRelease, &event ) ) {
		setButton( event.xbutton.button, 0 );
	}
    while ( XCheckTypedEvent( x11->display, EnterNotify, &event ) ) {
        subWindow = event.xcrossing.subwindow;
        hoverWindow = event.xcrossing.window;
        std::cout << hoverWindow << "\n";
	}
}

