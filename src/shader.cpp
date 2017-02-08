#include "shader.hpp"

Shader::Shader( std::string vert, std::string frag) {
    vert = resource->getRealPath(vert);
    frag = resource->getRealPath(frag);
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
    
    if ( vert_contents.length() <= 0 ) {
        std::string errstring = "Failed to open file (or is empty) `" + vert + "`.\n";
        throw new std::runtime_error(errstring);
    }

    if ( frag_contents.length() <= 0 ) {
        std::string errstring = "Failed to open file (or is empty) `" + frag + "`.\n";
        throw new std::runtime_error(errstring);
    }

    // Compile both shaders.
    unsigned int vertShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertShader, 1, &vertsrc , NULL );
    std::string errortxt;
    int err = compile( vertShader, errortxt );

    if ( err ) {
        std::string errstring = "Failed to compile shader `" + vert + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        glDeleteShader( vertShader );
        return;
    }

    unsigned int fragShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragShader, 1, &fragsrc, NULL );
    err = compile( fragShader, errortxt );
    if ( err ) {
        std::string errstring = "Failed to compile shader `" + frag + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        glDeleteShader( vertShader );
        glDeleteShader( fragShader );
        return;
    }

    // Then attempt to link them to this shader.
    err = link( vertShader, fragShader, errortxt );
    if ( err ) {
        std::string errstring = "Failed to link shader `" + vert + "` and  `" + frag + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        glDeleteShader( vertShader );
        glDeleteShader( fragShader );
        return;
    }

    // Clean up :)
    glDeleteShader( vertShader );
    glDeleteShader( fragShader );
    glUseProgram( 0 );
}

Shader::~Shader() {
    glDeleteProgram( m_program );
}

unsigned int Shader::getProgram() {
    return m_program;
}

void Shader::bind() {
    glUseProgram( m_program );
}

int Shader::compile( unsigned int shader, std::string& error ) {
    glCompileShader( shader );

    // Compiling the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result = GL_FALSE;
    int logLength;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
    if ( result == GL_FALSE ) {
        char* errormsg = new char[ logLength ];
        glGetShaderInfoLog( shader, logLength, NULL, errormsg );
        error = errormsg;
        delete[] errormsg;
        return 1;
    }
    return 0;
}

int Shader::link( unsigned int vertshader, unsigned int fragshader, std::string& error ) {
    glAttachShader( m_program, vertshader );
    glAttachShader( m_program, fragshader );
    glLinkProgram( m_program );

    // Linking the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result = GL_FALSE;
    int logLength;
    glGetProgramiv( m_program, GL_LINK_STATUS, &result);
    glGetProgramiv( m_program, GL_INFO_LOG_LENGTH, &logLength);
    if ( result == GL_FALSE ) {
        char* errormsg = new char[ logLength ];
        glGetProgramInfoLog( m_program, logLength, NULL, errormsg );
        error = errormsg;
        delete[] errormsg;
        return 1;
    }
    return 0;
}

unsigned int Shader::getUniformLocation( std::string name ) {
    glUseProgram( m_program );
    return glGetUniformLocation( m_program, name.c_str() );
}

void Shader::setParameter( std::string name, int foo ) {
    glUniform1i( getUniformLocation( name ), foo );
}

void Shader::setParameter( std::string name, float foo ) {
    glUniform1f( getUniformLocation( name ), foo );
}

void Shader::setParameter( std::string name, glm::mat4& foo ) {
    glUniformMatrix4fv( getUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( foo ) );
}

void Shader::setParameter( std::string name, glm::vec4 foo ) {
    glUniform4f( getUniformLocation( name ), foo.x, foo.y, foo.z, foo.w );
}

void Shader::setParameter( std::string name, glm::vec2 foo ) {
    glUniform2f( getUniformLocation( name ), foo.x, foo.y );
}

void Shader::setAttribute( std::string name, unsigned int buffer, unsigned int stepsize ) {
    unsigned int a = glGetAttribLocation( m_program, name.c_str() );
    glEnableVertexAttribArray( a );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glVertexAttribPointer( a, stepsize, GL_FLOAT, GL_FALSE, 0, NULL );
    m_activeattribs.push_back( a );
}

void Shader::unbind() {
    for ( unsigned int i=0; i<m_activeattribs.size(); i++ ) {
        glDisableVertexAttribArray( m_activeattribs[i] );
    }
    m_activeattribs.clear();
    glUseProgram( 0 );
}
