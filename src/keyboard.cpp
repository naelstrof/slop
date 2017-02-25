#include "keyboard.hpp"

bool slop::Keyboard::getKey( KeySym key ) {
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

bool slop::Keyboard::anyKeyDown() {
    return keyDown;
}

void slop::Keyboard::update() {
    char keys[32];
    XQueryKeymap( x11->display, keys );
    keyDown = false;
    for ( int i=0;i<32;i++ ) {
        if ( deltaState[i] == keys[i] ) {
            continue;
        }
        // Found a key in a group of 4 that's different
        char a = deltaState[i];
        char b = keys[i];
        // Find the "different" bits
        char c = a^b;
        // A new key was pressed since the last update.
        if ( c && b&c ) {
            keyDown = true;
        }
        deltaState[i] = keys[i];
    }
}

slop::Keyboard::Keyboard( X11* x11 ) {
    this->x11 = x11;
    int err = XGrabKeyboard( x11->display, x11->root, False, GrabModeAsync, GrabModeAsync, CurrentTime );
    if ( err != GrabSuccess ) {
        throw new std::runtime_error( "Failed to grab keyboard.\n" );
    }
    XQueryKeymap( x11->display, deltaState );
    keyDown = false;
}

slop::Keyboard::~Keyboard() {
    XUngrabKeyboard( x11->display, CurrentTime );
}
