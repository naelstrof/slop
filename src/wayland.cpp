#include "wayland.hpp"

static void global_registry_handler(void *data, wl_registry* registry,
                                    uint32_t id, const char* interface, uint32_t version) {
    //std::cout << "Got register " << interface << " with id " << id << "\n";
    if (std::string(interface) == "wl_compositor") {
        wayland->compositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if(std::string(interface) == "wl_shell") {
        wayland->shell = (wl_shell*)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    } else if(std::string(interface) == "wl_shm") {
        wayland->shm = (wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
    } else if(std::string(interface) == "wl_seat") {
        wayland->seat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 1);
    } else if(std::string(interface) == "wl_output") {
        wayland->output = (wl_output*)wl_registry_bind(registry, id, &wl_output_interface, 1);
        //wl_list_insert(&output_list, &output->link);
        //wl_output_add_listener(output->output, &output_listener, output);
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};

Wayland::Wayland() {
    display = wl_display_connect(NULL);
    if (!display) {
        throw new std::runtime_error("Failed to connect to display.");
    }
}

Wayland::~Wayland() {
    wl_display_disconnect(display);
}

void Wayland::init() {
    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);
    if (!compositor) {
        throw new std::runtime_error("Can't find compositor.");
    }

    egl_display = eglGetDisplay( display );
    eglInitialize( egl_display, NULL, NULL );
    if ( !egl_display ) {
        throw new std::runtime_error("Failed to initialize EGL! (Thats the graphics stuff.)");
    }
}
