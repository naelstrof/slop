#include "mouse.hpp"

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
		                         uint32_t serial, struct wl_surface *surface,
		                         wl_fixed_t sx_w, wl_fixed_t sy_w) {
    Mouse* m = (Mouse*)data;
	float sx = wl_fixed_to_double(sx_w);
	float sy = wl_fixed_to_double(sy_w);
    m->setMousePos( sx, sy );
    m->serial = serial;
    m->pointer = pointer;
    struct wl_cursor_image *image = m->cursor->images[0];
    wl_pointer_set_cursor(pointer, serial, m->surface, image->hotspot_x, image->hotspot_y);
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
		                         uint32_t serial, struct wl_surface *surface) {
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
		                          uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w)
{
    Mouse* m = (Mouse*)data;
	float sx = wl_fixed_to_double(sx_w);
	float sy = wl_fixed_to_double(sy_w);
    m->pointer = pointer;
    m->setMousePos( sx, sy );
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial,
                                  uint32_t time, uint32_t button, uint32_t state_w) {
    Mouse* m = (Mouse*)data;
    m->pointer = pointer;
    m->setButton( button, state_w );
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer,
		                        uint32_t time, uint32_t axis, wl_fixed_t value) {
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

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

void Mouse::setMousePos(float x, float y) {
    this->x = x;
    this->y = y;
}

glm::vec2 Mouse::getMousePos() {
    return glm::vec2( x, y );
}

void Mouse::setCursor( std::string name ) {
    if ( name == currentMouseCursor ) {
        return;
    }
    // First we gotta delete the old mouse surface.
    wl_surface_destroy( surface );

    // Then we get the new cursor image.
    cursor = wl_cursor_theme_get_cursor(theme, name.c_str());
    struct wl_cursor_image *image = cursor->images[0];
    struct wl_buffer *cursor_buf = wl_cursor_image_get_buffer(image);
    
    // Then we create the new surface
    surface = wl_compositor_create_surface(wayland->compositor);
    wl_surface_attach(surface, cursor_buf, 0, 0);
    wl_surface_damage(surface, 0, 0, image->width, image->height);
    wl_surface_commit(surface);

    // Finally assign the surface as a pointer
    wl_pointer_set_cursor(pointer, serial, surface, image->hotspot_x, image->hotspot_y);
}

Mouse::Mouse(Wayland* wayland) {
    wl_pointer_add_listener(wl_seat_get_pointer(wayland->seat), &pointer_listener, this);
    theme = wl_cursor_theme_load("default", 16, wayland->shm );
    cursor = wl_cursor_theme_get_cursor(theme, "cross");
    std::string currentMouseCursor = "cross";

    surface = wl_compositor_create_surface(wayland->compositor);
    struct wl_cursor_image *image = cursor->images[0];
    struct wl_buffer *cursor_buf = wl_cursor_image_get_buffer(image);
    wl_surface_attach(surface, cursor_buf, 0, 0);
    wl_surface_damage(surface, 0, 0, image->width, image->height);
    wl_surface_commit(surface);
}
