#include "keyboard.hpp"

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size) {
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    Keyboard* kb = (Keyboard*)data;
    kb->setKey( key, state );
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
}

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

Keyboard::Keyboard(Wayland* wayland) {
    wl_keyboard_add_listener(wl_seat_get_keyboard(wayland->seat), &keyboard_listener, this);
}

void Keyboard::setKey( int key, int state ) {
    for (unsigned int i=0;i<keys.size();i++ ) {
        if ( keys[i].x == key ) {
            keys[i].y = state;
            return;
        }
    }
    keys.push_back(glm::ivec2(key,state));
}

int Keyboard::getKey( int button ) {
    for (unsigned int i=0;i<keys.size();i++ ) {
        if ( keys[i].x == button ) {
            return keys[i].y;
        }
    }
    return 0;
}

bool Keyboard::anyKeyDown() {
    for ( glm::ivec2 combo : keys ) {
        if ( combo.y == 1 ) {
            return true;
        }
    }
    return false;
}
