#ifndef N_WAYLAND_H_
#define N_WAYLAND_H_

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

//#include <mutter/meta/compositor-mutter.h>

#ifndef NULL
#define NULL 0
#endif

class Wayland {
public:
    Wayland();
    ~Wayland();
    void init();
    wl_display* display;
    wl_compositor* compositor;
    wl_seat* seat;
    wl_shell* shell;
    wl_shm* shm;
    wl_output* output;
    EGLDisplay egl_display;
    glm::vec2 mousepos;
    std::vector<glm::vec4> getWindows();
};

extern Wayland* wayland;

#endif // N_WAYLAND_H_
