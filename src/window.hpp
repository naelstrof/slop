#ifndef N_WINDOW_H_
#define N_WINDOW_H_

#include <string>
#include <exception>

#include "gl_core_3_3.h"
#include "wayland.hpp"
//#include <GL/glew.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "framebuffer.hpp"

class Window {
public:
    Framebuffer* framebuffer;
    int width, height;
    glm::mat4 camera;
    Window( unsigned int w, unsigned int h );
    ~Window();
    void setFullScreen();
    void setCurrent();
    void setTitle( std::string title );
    void setClass( std::string c );
    void display();
    EGLContext egl_context;
    struct wl_surface *surface;
    struct wl_shell_surface *shell_surface;
    struct wl_egl_window *egl_window;
    EGLSurface egl_surface;
};

#endif // N_WINDOW_H_
