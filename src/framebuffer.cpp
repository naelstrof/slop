#include "framebuffer.hpp"

slop::Framebuffer::Framebuffer( int w, int h ) {
    gl::GenFramebuffers( 1, &fbuffer );
    gl::BindFramebuffer( gl::FRAMEBUFFER, fbuffer );
    gl::GenTextures( 1, &image );
    gl::BindTexture( gl::TEXTURE_2D, image );
    gl::TexImage2D( gl::TEXTURE_2D, 0, gl::RGBA, w, h, 0, gl::RGBA, gl::UNSIGNED_BYTE, NULL );
    gl::TexParameteri( gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST );
    gl::TexParameteri( gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST );
    gl::TexParameteri( gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE );
    gl::TexParameteri( gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE );
    gl::FramebufferTexture2D( gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, image, 0 );
    gl::BindFramebuffer( gl::FRAMEBUFFER, 0 );
    // generate our vertex and uv buffers
    std::vector<glm::vec2> verts;
    std::vector<glm::vec2> uvs;

    verts.push_back( glm::vec2(1,1) );
    uvs.push_back( glm::vec2( 1, 1 ) );
    verts.push_back( glm::vec2(-1,-1) );
    uvs.push_back( glm::vec2( 0, 0 ) );
    verts.push_back( glm::vec2(-1,1) );
    uvs.push_back( glm::vec2( 0, 1 ) );
    verts.push_back( glm::vec2(-1,-1) );
    uvs.push_back( glm::vec2( 0, 0 ) );
    verts.push_back( glm::vec2(1,1) );
    uvs.push_back( glm::vec2( 1, 1 ) );
    verts.push_back( glm::vec2(1,-1) );
    uvs.push_back( glm::vec2( 1, 0 ) );

    gl::GenBuffers( 2, buffers );
    gl::BindBuffer( gl::ARRAY_BUFFER, buffers[0] );
    gl::BufferData( gl::ARRAY_BUFFER, verts.size() * sizeof( glm::vec2 ), &verts[0], gl::STATIC_DRAW );
    gl::BindBuffer( gl::ARRAY_BUFFER, buffers[1] );
    gl::BufferData( gl::ARRAY_BUFFER, uvs.size() * sizeof( glm::vec2 ), &uvs[0], gl::STATIC_DRAW );
    vertCount = verts.size();
    generatedDesktopImage = false;
}

void slop::Framebuffer::setShader( slop::Shader* shader ) {
    this->shader = shader;
    if ( shader->hasParameter( "desktop" ) && !generatedDesktopImage ) {
        // Try to keep the image from being changed under our feet.
        XGrabServer(x11->display);
        XImage* image = XGetImage( x11->display, x11->root, 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ), 0xffffffff, ZPixmap );
        XUngrabServer(x11->display);
        gl::Enable(gl::TEXTURE_2D);
        gl::GenTextures(1, &desktopImage);
        gl::BindTexture(gl::TEXTURE_2D, desktopImage);
        gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
        gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
        //gl::TexEnvf(gl::TEXTURE_ENV, gl::TEXTURE_ENV_MODE, gl::MODULATE);
        gl::TexImage2D( gl::TEXTURE_2D, 0, gl::RGB, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ), 0, gl::BGRA, gl::UNSIGNED_BYTE, (void*)(&(image->data[0])));
        XDestroyImage( image );
        generatedDesktopImage = true;
    }
}

slop::Framebuffer::~Framebuffer() {
    gl::DeleteTextures(1, &image);
    gl::DeleteFramebuffers(1,&fbuffer);
    gl::DeleteBuffers(2,buffers);
}

void slop::Framebuffer::resize( int w, int h ) {
    // Regenerate the image
    gl::DeleteTextures(1, &image);
    gl::BindTexture(gl::TEXTURE_2D, image);
    gl::TexImage2D( gl::TEXTURE_2D, 0, gl::RGBA, w, h, 0, gl::RGBA, gl::UNSIGNED_BYTE, NULL);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);

    // Re-bind it to the framebuffer
    gl::BindFramebuffer( gl::FRAMEBUFFER, fbuffer );
    gl::FramebufferTexture2D( gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, image, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
}

void slop::Framebuffer::bind() {
    gl::BindFramebuffer( gl::FRAMEBUFFER, fbuffer );
}

void slop::Framebuffer::unbind() {
    gl::BindFramebuffer( gl::FRAMEBUFFER, 0 );
}

void slop::Framebuffer::draw(glm::vec2 mouse, float time, glm::vec4 color){
    shader->bind();
    shader->setParameter( "texture", 0 );
    shader->setAttribute( "position", buffers[0], 2 );
    shader->setAttribute( "uv", buffers[1], 2 );
    if ( shader->hasParameter( "mouse" ) ) {
        shader->setParameter( "mouse", mouse );
    }
    if ( shader->hasParameter( "color" ) ) {
        shader->setParameter( "color", color );
    }
    if ( shader->hasParameter( "screenSize" ) ) {
        shader->setParameter( "screenSize", glm::vec2( WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ) ));
    }
    if ( shader->hasParameter( "time" ) ) {
        shader->setParameter( "time", time );
    }
    gl::ActiveTexture(gl::TEXTURE0);
    gl::BindTexture( gl::TEXTURE_2D, image );
    if ( shader->hasParameter( "desktop" ) ) {
		shader->setParameter( "desktop", 1 );
        gl::ActiveTexture(gl::TEXTURE0 + 1);
        gl::BindTexture( gl::TEXTURE_2D, desktopImage );
    }
    gl::Enable( gl::TEXTURE_2D );
    gl::DrawArrays( gl::TRIANGLES, 0, vertCount );
    gl::Disable( gl::TEXTURE_2D );
    shader->unbind();
}
