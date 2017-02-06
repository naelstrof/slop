#ifndef N_SHADER_H_
#define N_SHADER_H_

#include <fstream>
#include <vector>
#include <string>
#include <exception>

#include "gl_core_3_3.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <EGL/egl.h>
#include <GL/gl.h>

#include "resource.hpp"

class Shader {
public:
    Shader( std::string vert, std::string frag );
    ~Shader();
    unsigned int    getProgram();
    void            bind();
    void            unbind();
    void            setParameter( std::string name, int foo );
    void            setParameter( std::string name, float foo );
    void            setParameter( std::string name, glm::mat4& foo );
    void            setParameter( std::string name, glm::vec4 foo );
    void            setParameter( std::string name, glm::vec2 foo );
    void            setAttribute( std::string name, unsigned int buffer, unsigned int stepsize );
private:
    std::vector<unsigned int>   m_activeattribs;
    bool                        m_good;
    unsigned int                getUniformLocation( std::string );
    int                         compile( unsigned int shader, std::string& error );
    int                         link( unsigned int vert, unsigned int frag, std::string& error );
    unsigned int                m_program;
};

#endif // N_SHADER_H_
