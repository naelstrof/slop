#include "framebuffer.hpp"

slop::Framebuffer::Framebuffer( int w, int h ) {
    std::string vert = "#version 130\nattribute vec2 position;\nattribute vec2 uv;\nvarying vec2 uvCoord;\nvoid main()\n{\nuvCoord = uv;\ngl_Position = vec4(position,0,1);\n}\n";
    std::string frag = "#version 130\nuniform sampler2D texture;\nvarying vec2 uvCoord;\nout vec4 outColor;void main()\n{\noutColor = texture2D( texture, uvCoord );\n}\n";
    shader = new Shader( vert, frag, false );
    glGenFramebuffers( 1, &fbuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, fbuffer );
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
}

void slop::Framebuffer::setShader( std::string name ) {
    delete shader;
    shader = new Shader( name + ".vert", name + ".frag" );
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

void slop::Framebuffer::draw(){
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader->bind();
    shader->setParameter( "texture", 0 );
    shader->setAttribute( "position", buffers[0], 2 );
    shader->setAttribute( "uv", buffers[1], 2 );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, image );
    glEnable( GL_TEXTURE_2D );
    glDrawArrays( GL_TRIANGLES, 0, vertCount );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    shader->unbind();
}
