#include "keyboard.hpp"

bool Keyboard::getKey( KeySym key ) {
    KeyCode keycode = XKeysymToKeycode( x11->display, key );
    if ( keycode != 0 ) {
        // Get the whole keyboard state
        char keys[32];
        XQueryKeymap( x11->display, keys );
        // Check our keycode
        return ( keys[ keycode / 8 ] & ( 1 << ( keycode % 8 ) ) ) != 0;
    } else {
        return false;
    }
}

bool Keyboard::anyKeyDown() {
    // Thanks to SFML for some reliable key state grabbing.
    // Get the whole keyboard state
    char keys[ 32 ];
    XQueryKeymap( x11->display, keys );
    // Each bit indicates a different key, 1 for pressed, 0 otherwise.
    // Every bit should be 0 if nothing is pressed.
    for ( unsigned int i = 0; i < 32; i++ ) {
        if ( keys[ i ] != 0 ) {
            return true;
        }
    }
    return false;
}

Keyboard::Keyboard( X11* x11 ) {
    this->x11 = x11;
    int err = XGrabKeyboard( x11->display, x11->root, False, GrabModeAsync, GrabModeAsync, CurrentTime );
    if ( err != GrabSuccess ) {
        throw new std::runtime_error( "Failed to grab keyboard.\n" );
    }
}

Keyboard::~Keyboard() {
    XUngrabKeyboard( x11->display, CurrentTime );
}
