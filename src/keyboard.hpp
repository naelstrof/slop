#ifndef N_KEYBOARD_H_
#define N_KEYBOARD_H_


#include <glm/glm.hpp>
#include <wayland-client.h>
#include <vector>
#include "wayland.hpp"

class Keyboard {
private:
    std::vector<glm::ivec2> keys;
public:
    Keyboard( Wayland* wayland );
    void setKey( int key, int state );
    int getKey( int key );
    bool anyKeyDown();
};

extern Keyboard* keyboard;

#endif // N_KEYBOARD_H_
