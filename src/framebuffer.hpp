#ifndef IS_FRAMEBUFFER_H_
#define IS_FRAMEBUFFER_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include "x.hpp"
#include "shader.hpp"
#include "resource.hpp"

namespace slop {

class Framebuffer
{
public:
    enum buffers
    {
        color = 0x01,
        depth = 0x02,
        stencil = 0x04
    };
    Framebuffer();
    Framebuffer( unsigned int width, unsigned int height, unsigned char flags = color, std::string shader = "simple" );
    ~Framebuffer();
    void create( unsigned int width, unsigned int height, unsigned char flags = color, std::string shader = "simple" );
    void createFromTexture( unsigned int texture, std::string shader = "simple" );
    void setShader( std::string shader = "simple" );
    void clear( glm::vec4 color = glm::vec4( 0, 0, 0, 1 ), unsigned char flags = color | depth | stencil );
    void bind();
    void unbind();
    void draw( float time, unsigned int image );
    unsigned int m_texture;
    unsigned int m_depth;
    unsigned int m_stencil;
    slop::Shader* m_shader;
private:
    unsigned int m_width;
    unsigned int m_height;
    void check();
    void generateBuffers();
    unsigned char m_flags;
    bool generatedBuffers;
    unsigned int m_frame;
    unsigned int m_buffers[2];
};

}

#endif //IS_FRAMEBUFFER_H_
