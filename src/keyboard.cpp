#include <X11/Xlib.h>
#include <chrono>
#include <thread>
#include <vector>
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

// This returns if a key is currently pressed.
// Ignores arrow key presses specifically so users can
// adjust their selection.
bool slop::Keyboard::anyKeyDown() {
    return keyDown;
}

void slop::Keyboard::update() {
    char keys[32];
    KeyCode keycode;
    XQueryKeymap( x11->display, keys );

    // We first delete the arrow key buttons from the mapping.
    // This allows the user to press the arrow keys without triggering anyKeyDown
    for ( size_t i = 0; i < 4; i++) {
        keycode = XKeysymToKeycode( x11->display, keyAdjust.at(i));
        keys[ keycode / 8 ] = keys[ keycode / 8 ] & ~( 1 << ( keycode % 8 ) );
    }
    // Also deleting Space for move operation
    keycode = XKeysymToKeycode( x11->display, keyMove );
    keys[ keycode / 8 ] = keys[ keycode / 8 ] & ~( 1 << ( keycode % 8 ) );

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

slop::Keyboard::Keyboard( X11* x11 , KeySym keyMove, std::vector<KeySym> keyAdjust) {
    this->x11 = x11;
    this->keyMove = keyMove;
    this->keyAdjust = keyAdjust;
    int err = XGrabKeyboard( x11->display, x11->root, False, GrabModeAsync, GrabModeAsync, CurrentTime );
    int tries = 0;
    while( err != GrabSuccess && tries < 5 ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        err = XGrabKeyboard( x11->display, x11->root, False, GrabModeAsync, GrabModeAsync, CurrentTime );
        tries++;
    }
    // Non-fatal.
    if ( err != GrabSuccess ) {
        //throw std::runtime_error( "Couldn't grab the keyboard after 10 tries." );
    }
    XQueryKeymap( x11->display, deltaState );
    keyDown = false;
}

slop::Keyboard::~Keyboard() {
    XUngrabKeyboard( x11->display, CurrentTime );
}
