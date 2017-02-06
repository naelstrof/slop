#include "window.hpp"

static void shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
	wl_shell_surface_pong (shell_surface, serial);
}
static void shell_surface_configure (void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
	Window* window = (Window*)data;
	wl_egl_window_resize (window->egl_window, width, height, 0, 0);
    window->camera = glm::ortho( 0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    window->width = width;
    window->height = height;
    glViewport(0,0,width,height);
    window->framebuffer->resize( width, height );
}
static void shell_surface_popup_done (void *data, struct wl_shell_surface *shell_surface) {
	
}
static struct wl_shell_surface_listener shell_surface_listener = {&shell_surface_ping, &shell_surface_configure, &shell_surface_popup_done};

Window::Window( unsigned int w, unsigned int h ) {
    eglBindAPI (EGL_OPENGL_API);
	EGLint attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
	EGL_NONE};
    camera = glm::ortho( 0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
	EGLConfig config;
	EGLint num_config;
	eglChooseConfig (wayland->egl_display, attributes, &config, 1, &num_config);
	egl_context = eglCreateContext (wayland->egl_display, config, EGL_NO_CONTEXT, NULL);
	
	surface = wl_compositor_create_surface (wayland->compositor);
	shell_surface = wl_shell_get_shell_surface (wayland->shell, surface);
	wl_shell_surface_add_listener (shell_surface, &shell_surface_listener, this);
	wl_shell_surface_set_toplevel (shell_surface);
	egl_window = wl_egl_window_create (surface, w, h);
	egl_surface = eglCreateWindowSurface (wayland->egl_display, config, egl_window, NULL);
    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        throw new std::runtime_error("Failed to load function pointers for OpenGL.");
    }
    setCurrent();
    framebuffer = new Framebuffer( w, h );
}

Window::~Window() {
    delete framebuffer;
	eglDestroySurface (wayland->egl_display, egl_surface);
	wl_egl_window_destroy (egl_window);
	wl_surface_destroy (surface);
	wl_shell_surface_destroy (shell_surface);
	eglDestroyContext (wayland->egl_display, egl_context);
}

void Window::display() {
	if (!eglSwapBuffers(wayland->egl_display, egl_surface)) {
        throw new std::runtime_error("Failed to swap buffers.");
    }
}

void Window::setFullScreen() {
	wl_shell_surface_set_fullscreen(shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, NULL);
}

void Window::setCurrent() {
	if (!eglMakeCurrent (wayland->egl_display, egl_surface, egl_surface, egl_context)) {
        throw new std::runtime_error("Failed set EGL's current surface for rendering.");
    }
}

void Window::setTitle(std::string title) {
	wl_shell_surface_set_title(shell_surface, title.c_str());
}

void Window::setClass(std::string title) {
	wl_shell_surface_set_class(shell_surface, title.c_str());
}
