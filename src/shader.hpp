/* shader.hpp: Handles compiling shaders.
 *
 * Copyright (C) 2014: Dalton Nell, Slop Contributors (https://github.com/naelstrof/slop/graphs/contributors).
 *
 * This file is part of Slop.
 *
 * Slop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Slop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Slop.  If not, see <http://www.gnu.org/licenses/>.
 */

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
