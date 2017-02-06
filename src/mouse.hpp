#ifndef N_MOUSE_H_
#define N_MOUSE_H_

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <vector>
#include <glm/glm.hpp>
#include "linux/input-event-codes.h"
#include "wayland.hpp"

class Mouse {
private:
    wl_cursor_theme* theme;
    float x,y;
    std::vector<glm::ivec2> buttons;
    std::string currentMouseCursor;
public:
    wl_surface* surface;
    wl_cursor* cursor;
    wl_pointer* pointer;
    int serial;
    Mouse( Wayland* wayland );
    void setMousePos( float x, float y );
    void setCursor( std::string name );
    glm::vec2 getMousePos();
    void setButton( int button, int state );
    int getButton( int button );
};

extern Mouse* mouse;

#endif // N_MOUSE_H_
