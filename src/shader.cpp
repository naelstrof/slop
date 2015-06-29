#include "shader.hpp"

slop::Shader::Shader( std::string name, std::string vert, std::string frag, std::string type ) {
    m_name = name;
    // Set up the type of the shader, this is read by render to know what kind of matricies it needs.
    if ( type == "perspective" ) {
        m_type = 0;
    } else if ( type == "orthographic" ) {
        m_type = 1;
    } else {
        m_type = -1;
    }
    m_good = false;
    // Create the program to link to.
    m_program = glCreateProgram();

    std::ifstream v( vert.c_str() );
    std::string vert_contents((std::istreambuf_iterator<char>(v)),
                               std::istreambuf_iterator<char>());
    std::ifstream f( frag.c_str() );
    std::string frag_contents((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());

    const char* vertsrc = vert_contents.c_str();
    const char* fragsrc = frag_contents.c_str();

    // Compile both shaders.
    unsigned int vertShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertShader, 1, &vertsrc , NULL );
    int err = compile( vertShader );

    if ( err ) {
        fprintf( stderr, "Failed to compile shader %s.\n", vert.c_str() );
        glDeleteShader( vertShader );
        return;
    }

    unsigned int fragShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragShader, 1, &fragsrc, NULL );
    err = compile( fragShader );
    if ( err ) {
        fprintf( stderr, "Failed to compile shader %s.\n", frag.c_str() );
        glDeleteShader( vertShader );
        glDeleteShader( fragShader );
        return;
    }


    // Then attempt to link them to this shader.
    err = link( vertShader, fragShader );
    if ( err ) {
        fprintf( stderr, "Failed to link shader %s.\n", name.c_str() );
        glDeleteShader( vertShader );
        glDeleteShader( fragShader );
        return;
    }

    // Clean up :)
    glDeleteShader( vertShader );
    glDeleteShader( fragShader );
    m_good = true;
    glUseProgram( 0 );
}

slop::Shader::~Shader() {
    glDeleteProgram( m_program );
}

unsigned int slop::Shader::getProgram() {
    return m_program;
}

void slop::Shader::bind() {
    glUseProgram( m_program );
}

int slop::Shader::compile( unsigned int shader ) {
    glCompileShader( shader );

    // Compiling the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result;
    int logLength;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
    if ( result == GL_FALSE ) {
        char* errormsg = new char[ logLength ];
        glGetShaderInfoLog( shader, logLength, NULL, errormsg );
        fprintf( stderr, "%s\n", errormsg );
        delete[] errormsg;
        return 1;
    }
    return 0;
}

int slop::Shader::link( unsigned int vertshader, unsigned int fragshader ) {
    glAttachShader( m_program, vertshader );
    glAttachShader( m_program, fragshader );
    glLinkProgram( m_program );

    // Linking the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result;
    int logLength;
    glGetProgramiv( m_program, GL_LINK_STATUS, &result);
    glGetProgramiv( m_program, GL_INFO_LOG_LENGTH, &logLength);
    if ( result == GL_FALSE ) {
        char* errormsg = new char[ logLength ];
        glGetProgramInfoLog( m_program, logLength, NULL, errormsg );
        fprintf( stderr, "%s\n", errormsg );
        delete[] errormsg;
        return 1;
    }
    return 0;
}

unsigned int slop::Shader::getUniformLocation( std::string name ) {
    if ( !m_good ) {
        return 0;
    }
    glUseProgram( m_program );
    return glGetUniformLocation( m_program, name.c_str() );
}

void slop::Shader::setParameter( std::string name, int foo ) {
    if ( !m_good ) {
        return;
    }
    glUniform1i( getUniformLocation( name ), foo );
}

void slop::Shader::setParameter( std::string name, float foo ) {
    if ( !m_good ) {
        return;
    }
    glUniform1f( getUniformLocation( name ), foo );
}

void slop::Shader::setParameter( std::string name, glm::mat4 foo ) {
    if ( !m_good ) {
        return;
    }
    glUniformMatrix4fv( getUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( foo ) );
}

void slop::Shader::setParameter( std::string name, glm::vec4 foo ) {
    if ( !m_good ) {
        return;
    }
    glUniform4f( getUniformLocation( name ), foo.x, foo.y, foo.z, foo.w );
}

void slop::Shader::setParameter( std::string name, glm::vec2 foo ) {
    if ( !m_good ) {
        return;
    }
    glUniform2f( getUniformLocation( name ), foo.x, foo.y );
}

void slop::Shader::setAttribute( std::string name, unsigned int buffer, unsigned int stepsize ) {
    unsigned int a = glGetAttribLocation( m_program, name.c_str() );
    glEnableVertexAttribArray( a );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glVertexAttribPointer( a, stepsize, GL_FLOAT, GL_FALSE, 0, NULL );
    m_activeattribs.push_back( a );
}

void slop::Shader::unbind() {
    for ( unsigned int i=0; i<m_activeattribs.size(); i++ ) {
        glDisableVertexAttribArray( m_activeattribs[i] );
    }
    m_activeattribs.clear();
    glUseProgram( 0 );
}
