#include "shader.hpp"

slop::Shader::Shader( std::string vert, std::string frag, bool file ) {
    std::string vert_contents;
    std::string frag_contents;
    if ( file ) {
        vert = resource->getRealPath(vert);
        frag = resource->getRealPath(frag);
        std::ifstream v( vert.c_str() );
        if (!v.is_open()) {
            throw new std::runtime_error( "Failed to open " + vert );
        }
        vert_contents = std::string((std::istreambuf_iterator<char>(v)),
                                   std::istreambuf_iterator<char>());
        std::ifstream f( frag.c_str() );
        if (!f.is_open()) {
            throw new std::runtime_error( "Failed to open " + frag );
        }
        frag_contents = std::string((std::istreambuf_iterator<char>(f)),
                                  std::istreambuf_iterator<char>());
    } else {
        vert_contents = vert;
        frag_contents = frag;
    }

    const char* vertsrc = vert_contents.c_str();
    const char* fragsrc = frag_contents.c_str();

    // Create the program to link to.
    program = gl::CreateProgram();
    
    if ( vert_contents.length() <= 0 ) {
        std::string errstring = "Failed to open file (or is empty) `" + vert + "`.\n";
        throw new std::runtime_error(errstring);
    }

    if ( frag_contents.length() <= 0 ) {
        std::string errstring = "Failed to open file (or is empty) `" + frag + "`.\n";
        throw new std::runtime_error(errstring);
    }

    // Compile both shaders.
    unsigned int vertShader = gl::CreateShader( gl::VERTEX_SHADER );
    gl::ShaderSource( vertShader, 1, &vertsrc , NULL );
    std::string errortxt;
    int err = compile( vertShader, errortxt );

    if ( err ) {
        std::string errstring = "Failed to compile shader `" + vert + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        gl::DeleteShader( vertShader );
        return;
    }

    unsigned int fragShader = gl::CreateShader( gl::FRAGMENT_SHADER );
    gl::ShaderSource( fragShader, 1, &fragsrc, NULL );
    err = compile( fragShader, errortxt );
    if ( err ) {
        std::string errstring = "Failed to compile shader `" + frag + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        gl::DeleteShader( vertShader );
        gl::DeleteShader( fragShader );
        return;
    }

    // Then attempt to link them to this shader.
    err = link( vertShader, fragShader, errortxt );
    if ( err ) {
        std::string errstring = "Failed to link shader `" + vert + "` and  `" + frag + "`:\n" + errortxt;
        throw new std::runtime_error(errstring);
        gl::DeleteShader( vertShader );
        gl::DeleteShader( fragShader );
        return;
    }

    // Clean up :)
    gl::DeleteShader( vertShader );
    gl::DeleteShader( fragShader );
    gl::UseProgram( 0 );
}

slop::Shader::~Shader() {
    gl::DeleteProgram( program );
}

unsigned int slop::Shader::getProgram() {
    return program;
}

void slop::Shader::bind() {
    gl::UseProgram( program );
}

int slop::Shader::compile( unsigned int shader, std::string& error ) {
    gl::CompileShader( shader );

    // Compiling the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result;
    int logLength;
    gl::GetShaderiv( shader, gl::COMPILE_STATUS, &result );
    gl::GetShaderiv( shader, gl::INFO_LOG_LENGTH, &logLength );
    if ( result == false ) {
        char* errormsg = new char[ logLength ];
        gl::GetShaderInfoLog( shader, logLength, NULL, errormsg );
        error = errormsg;
        delete[] errormsg;
        return 1;
    }
    return 0;
}

int slop::Shader::link( unsigned int vertshader, unsigned int fragshader, std::string& error ) {
    gl::AttachShader( program, vertshader );
    gl::AttachShader( program, fragshader );
    gl::LinkProgram( program );

    // Linking the shader is the easy part, all this junk down here is for printing the error it might generate.
    int result = false;
    int logLength;
    gl::GetProgramiv( program, gl::LINK_STATUS, &result);
    gl::GetProgramiv( program, gl::INFO_LOG_LENGTH, &logLength);
    if ( result == false ) {
        char* errormsg = new char[ logLength ];
        gl::GetProgramInfoLog( program, logLength, NULL, errormsg );
        error = errormsg;
        delete[] errormsg;
        return 1;
    }
    return 0;
}

unsigned int slop::Shader::getUniformLocation( std::string name ) {
    gl::UseProgram( program );
    return gl::GetUniformLocation( program, name.c_str() );
}

bool slop::Shader::hasParameter( std::string name ) {
    gl::UseProgram( program );
    return gl::GetUniformLocation( program, name.c_str() ) != -1;
}

void slop::Shader::setParameter( std::string name, int foo ) {
    gl::Uniform1i( getUniformLocation( name ), foo );
}

void slop::Shader::setParameter( std::string name, float foo ) {
    gl::Uniform1f( getUniformLocation( name ), foo );
}

void slop::Shader::setParameter( std::string name, glm::mat4& foo ) {
    gl::UniformMatrix4fv( getUniformLocation( name ), 1, false, glm::value_ptr( foo ) );
}

void slop::Shader::setParameter( std::string name, glm::vec4 foo ) {
    gl::Uniform4f( getUniformLocation( name ), foo.x, foo.y, foo.z, foo.w );
}

void slop::Shader::setParameter( std::string name, glm::vec2 foo ) {
    gl::Uniform2f( getUniformLocation( name ), foo.x, foo.y );
}

void slop::Shader::setAttribute( std::string name, unsigned int buffer, unsigned int stepsize ) {
    unsigned int a = gl::GetAttribLocation( program, name.c_str() );
    gl::EnableVertexAttribArray( a );
    gl::BindBuffer( gl::ARRAY_BUFFER, buffer );
    gl::VertexAttribPointer( a, stepsize, gl::FLOAT, false, 0, NULL );
    activeAttributes.push_back( a );
}

void slop::Shader::unbind() {
    for ( unsigned int i=0; i<activeAttributes.size(); i++ ) {
        gl::DisableVertexAttribArray( activeAttributes[i] );
    }
    activeAttributes.clear();
    gl::UseProgram( 0 );
}
