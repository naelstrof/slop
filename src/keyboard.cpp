#include <chrono>
#include <thread>
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

void slop::Keyboard::zeroKey( char keys[32], KeySym key ) {
    KeyCode keycode = XKeysymToKeycode( x11->display, key );
    keys[ keycode / 8 ] = keys[ keycode / 8 ] & ~( 1 << ( keycode % 8 ) );
}

void slop::Keyboard::update() {
    char keys[32];
    XQueryKeymap( x11->display, keys );
    // We first delete the arrow key buttons from the mapping.
    // This allows the user to press the arrow keys without triggering anyKeyDown
    zeroKey(keys, XK_Left);
    zeroKey(keys, XK_Right);
    zeroKey(keys, XK_Up);
    zeroKey(keys, XK_Down);
    // Also deleting Space for move operation
    zeroKey(keys, XK_space);

    if ( mouseKeys ) {
        zeroKey(keys, XK_KP_0);
        zeroKey(keys, XK_KP_1);
        zeroKey(keys, XK_KP_2);
        zeroKey(keys, XK_KP_3);
        zeroKey(keys, XK_KP_4);
        zeroKey(keys, XK_KP_5);
        zeroKey(keys, XK_KP_6);
        zeroKey(keys, XK_KP_7);
        zeroKey(keys, XK_KP_8);
        zeroKey(keys, XK_KP_9);
        zeroKey(keys, XK_KP_Add);
        zeroKey(keys, XK_KP_Subtract);
        zeroKey(keys, XK_KP_Multiply);
        zeroKey(keys, XK_KP_Divide);
        zeroKey(keys, XK_KP_Insert);
        zeroKey(keys, XK_KP_Delete);
        zeroKey(keys, XK_KP_Decimal);
    }

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

    // Ignore any failures while checking for Mouse Keys
    XkbDescPtr xkb = XkbGetKeyboard( x11->display, XkbControlsMask, XkbUseCoreKbd );
    if ( xkb != 0 ) {
        Status status = XkbGetControls( x11->display, XkbAllControlsMask, xkb );
        if ( status == Success ) {
            mouseKeys = xkb->ctrls->enabled_ctrls & XkbMouseKeysMask;
            XkbFreeControls( xkb, XkbAllControlsMask, true );
        }
        XkbFreeKeyboard( xkb, XkbControlsMask, true );
    }

    XQueryKeymap( x11->display, deltaState );
    keyDown = false;
}

slop::Keyboard::~Keyboard() {
    XUngrabKeyboard( x11->display, CurrentTime );
}
