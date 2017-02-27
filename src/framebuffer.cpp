#include "framebuffer.hpp"

slop::Framebuffer::Framebuffer( int w, int h ) {
    glGenFramebuffers( 1, &fbuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, fbuffer );
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    glGenBuffers( 2, buffers );
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( glm::vec2 ), &verts[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
    glBufferData( GL_ARRAY_BUFFER, uvs.size() * sizeof( glm::vec2 ), &uvs[0], GL_STATIC_DRAW );
    vertCount = verts.size();
    generatedDesktopImage = false;
}

void slop::Framebuffer::setShader( slop::Shader* shader ) {
    this->shader = shader;
    if ( shader->hasParameter( "desktop" ) && !generatedDesktopImage ) {
        XImage* image = XGetImage( x11->display, x11->root, 0, 0, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ), 0xffffffff, ZPixmap );
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &desktopImage);
        glBindTexture(GL_TEXTURE_2D, desktopImage);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, WidthOfScreen( x11->screen ), HeightOfScreen( x11->screen ), 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)(&(image->data[0])));
        XDestroyImage( image );
        generatedDesktopImage = true;
    }
}

slop::Framebuffer::~Framebuffer() {
    glDeleteTextures(1, &image);
    glDeleteFramebuffers(1,&fbuffer);
    glDeleteBuffers(2,buffers);
    delete shader;
}

void slop::Framebuffer::resize( int w, int h ) {
    // Regenerate the image
    glDeleteTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Re-bind it to the framebuffer
    glBindFramebuffer( GL_FRAMEBUFFER, fbuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void slop::Framebuffer::bind() {
    glBindFramebuffer( GL_FRAMEBUFFER, fbuffer );
}

void slop::Framebuffer::unbind() {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, image );
    if ( shader->hasParameter( "desktop" ) ) {
		shader->setParameter( "desktop", 1 );
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture( GL_TEXTURE_2D, desktopImage );
    }
    glEnable( GL_TEXTURE_2D );
    glDrawArrays( GL_TRIANGLES, 0, vertCount );
    glDisable( GL_TEXTURE_2D );
    shader->unbind();
}
