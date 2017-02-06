#ifndef N_FRAMEBUFFER_H_
#define N_FRAMEBUFFER_H_

#include "gl_core_3_3.h"
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>

#include "shader.hpp"

class Framebuffer {
private:
    unsigned int fbuffer;
    unsigned int image;
    unsigned int buffers[2];
    unsigned int vertCount;
    Shader* shader;
public:
    Framebuffer( int w, int h );
    ~Framebuffer();
    void setShader( std::string );
    void draw();
    void resize( int w, int h );
    void bind();
    void unbind();
};

#endif
