#include "glrectangle.hpp"

GLRectangle::GLRectangle( glm::vec2 p1, glm::vec2 p2, float border, float padding, glm::vec4 color, bool highlight ) {
    this->color = color;
    this->border = border;
    this->padding = padding;
    this->highlight = highlight;
    // Find each corner of the rectangle
    ul = glm::vec2( glm::min( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    bl = glm::vec2( glm::min( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    ur = glm::vec2( glm::max( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    br = glm::vec2( glm::max( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    // Offset the inner corners by the padding.
    ul = ul + glm::vec2(-padding,padding);
    bl = bl + glm::vec2(-padding,-padding);
    ur = ur + glm::vec2(padding,padding);
    br = br + glm::vec2(padding,-padding);
    // Create the outer corners by offsetting the inner by the bordersize
    oul = ul + glm::vec2(-border,border);
    obl = bl + glm::vec2(-border,-border);
    our = ur + glm::vec2(border,border);
    obr = br + glm::vec2(border,-border);
    generateBuffers();
    std::string vert = "#version 130\nin vec2 position;\nuniform mat4 projection;\nvoid main() {\ngl_Position = projection*vec4(position,0,1);\n }";
    std::string frag = "#version 130\nuniform vec4 color;\nout vec4 outColor;\nvoid main() {\noutColor = color;\n}";

    shader = new Shader( vert, frag, false );
}

void GLRectangle::setPoints( glm::vec2 p1, glm::vec2 p2 ) {
    // Find each corner of the rectangle
    ul = glm::vec2( glm::min( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    bl = glm::vec2( glm::min( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    ur = glm::vec2( glm::max( p1.x, p2.x ), glm::max( p1.y, p2.y ) ) ;
    br = glm::vec2( glm::max( p1.x, p2.x ), glm::min( p1.y, p2.y ) ) ;
    // Offset the inner corners by the padding.
    ul = ul + glm::vec2(-padding,padding);
    bl = bl + glm::vec2(-padding,-padding);
    ur = ur + glm::vec2(padding,padding);
    br = br + glm::vec2(padding,-padding);
    // Create the outer corners by offsetting the inner by the bordersize
    oul = ul + glm::vec2(-border,border);
    obl = bl + glm::vec2(-border,-border);
    our = ur + glm::vec2(border,border);
    obr = br + glm::vec2(border,-border);
    generateBuffers();
}

void GLRectangle::generateBuffers() {
    std::vector<glm::vec2> rectangle_verts;
    std::vector<glm::vec2> rectangle_uvs;
    std::vector<glm::vec2> corner_verts;
    std::vector<glm::vec2> corner_uvs;
    std::vector<glm::vec2> center_verts;
    std::vector<glm::vec2> center_uvs;

    // Top left corner
    corner_verts.push_back( oul );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( ul );
    corner_uvs.push_back( glm::vec2(1, 0) );
    corner_verts.push_back( ul + glm::vec2( -border, 0 ) );
    corner_uvs.push_back( glm::vec2(0, 0) );

    corner_verts.push_back( oul );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( ul + glm::vec2( 0, border ) );
    corner_uvs.push_back( glm::vec2(1, 1) );
    corner_verts.push_back( ul );
    corner_uvs.push_back( glm::vec2(1, 0) );

    // Top right corner
    corner_verts.push_back( ur + glm::vec2( 0, border ) );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( ur + glm::vec2( border, 0 ) );
    corner_uvs.push_back( glm::vec2(1, 0) );
    corner_verts.push_back( ur );
    corner_uvs.push_back( glm::vec2(0, 0) );

    corner_verts.push_back( ur + glm::vec2( 0, border ) );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( our );
    corner_uvs.push_back( glm::vec2(1, 1) );
    corner_verts.push_back( ur + glm::vec2( border, 0 ) );
    corner_uvs.push_back( glm::vec2(1, 0) );

    // Bottom left corner
    corner_verts.push_back( bl + glm::vec2( -border, 0 ) );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( obl + glm::vec2( border, 0 ) );
    corner_uvs.push_back( glm::vec2(1, 0) );
    corner_verts.push_back( obl );
    corner_uvs.push_back( glm::vec2(0, 0) );

    corner_verts.push_back( bl + glm::vec2( -border, 0 ) );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( bl );
    corner_uvs.push_back( glm::vec2(1, 1) );
    corner_verts.push_back( bl + glm::vec2( 0, -border ) );
    corner_uvs.push_back( glm::vec2(1, 0) );

    // Bottom right corner
    corner_verts.push_back( br );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( obr );
    corner_uvs.push_back( glm::vec2(1, 0) );
    corner_verts.push_back( br + glm::vec2( 0, -border ) );
    corner_uvs.push_back( glm::vec2(0, 0) );

    corner_verts.push_back( br );
    corner_uvs.push_back( glm::vec2(0, 1) );
    corner_verts.push_back( br + glm::vec2( border, 0 ) );
    corner_uvs.push_back( glm::vec2(1, 1) );
    corner_verts.push_back( obr );
    corner_uvs.push_back( glm::vec2(1, 0) );

    // Left GLRectangle
    rectangle_verts.push_back( ul + glm::vec2( -border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( bl );
    rectangle_uvs.push_back( glm::vec2(1, 0) );
    rectangle_verts.push_back( bl + glm::vec2( -border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(0, 0) );

    rectangle_verts.push_back( ul + glm::vec2( -border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( ul );
    rectangle_uvs.push_back( glm::vec2(1, 1) );
    rectangle_verts.push_back( bl );
    rectangle_uvs.push_back( glm::vec2(1, 0) );

    // Right GLRectangle
    rectangle_verts.push_back( ur );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( br + glm::vec2( border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(1, 0) );
    rectangle_verts.push_back( br );
    rectangle_uvs.push_back( glm::vec2(0, 0) );

    rectangle_verts.push_back( ur );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( ur + glm::vec2( border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(1, 1) );
    rectangle_verts.push_back( br + glm::vec2( border, 0 ) );
    rectangle_uvs.push_back( glm::vec2(1, 0) );

    // Top GLRectangle
    rectangle_verts.push_back( ul + glm::vec2( 0, border ) );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( ur );
    rectangle_uvs.push_back( glm::vec2(1, 0) );
    rectangle_verts.push_back( ul );
    rectangle_uvs.push_back( glm::vec2(0, 0) );

    rectangle_verts.push_back( ul + glm::vec2( 0, border ) );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( ur + glm::vec2( 0, border ) );
    rectangle_uvs.push_back( glm::vec2(1, 1) );
    rectangle_verts.push_back( ur );
    rectangle_uvs.push_back( glm::vec2(1, 0) );

    // Bot GLRectangle
    rectangle_verts.push_back( bl );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( br + glm::vec2( 0, -border ) );
    rectangle_uvs.push_back( glm::vec2(1, 0) );
    rectangle_verts.push_back( bl + glm::vec2( 0, -border ) );
    rectangle_uvs.push_back( glm::vec2(0, 0) );

    rectangle_verts.push_back( bl );
    rectangle_uvs.push_back( glm::vec2(0, 1) );
    rectangle_verts.push_back( br );
    rectangle_uvs.push_back( glm::vec2(1, 1) );
    rectangle_verts.push_back( br + glm::vec2( 0, -border ) );
    rectangle_uvs.push_back( glm::vec2(1, 0) );

    center_verts.push_back( ul );
    center_uvs.push_back( glm::vec2(0, 1) );
    center_verts.push_back( br );
    center_uvs.push_back( glm::vec2(1, 0) );
    center_verts.push_back( bl );
    center_uvs.push_back( glm::vec2(0, 0) );

    center_verts.push_back( ul );
    center_uvs.push_back( glm::vec2(0, 1) );
    center_verts.push_back( ur );
    center_uvs.push_back( glm::vec2(1, 1) );
    center_verts.push_back( br );
    center_uvs.push_back( glm::vec2(1, 0) );

    glGenBuffers( 6, (GLuint*)&buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer.corner_verts );
    glBufferData( GL_ARRAY_BUFFER, corner_verts.size() * sizeof( glm::vec2 ), &corner_verts[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, buffer.corner_uvs );
    glBufferData( GL_ARRAY_BUFFER, corner_uvs.size() * sizeof( glm::vec2 ), &corner_uvs[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, buffer.rectangle_verts );
    glBufferData( GL_ARRAY_BUFFER, rectangle_verts.size() * sizeof( glm::vec2 ), &rectangle_verts[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, buffer.rectangle_uvs );
    glBufferData( GL_ARRAY_BUFFER, rectangle_uvs.size() * sizeof( glm::vec2 ), &rectangle_uvs[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, buffer.center_verts );
    glBufferData( GL_ARRAY_BUFFER, center_verts.size() * sizeof( glm::vec2 ), &center_verts[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, buffer.rectangle_uvs );
    glBufferData( GL_ARRAY_BUFFER, center_uvs.size() * sizeof( glm::vec2 ), &center_uvs[0], GL_STATIC_DRAW );
    corner_vertCount = corner_verts.size();
    rectangle_vertCount = rectangle_verts.size();
    center_vertCount = center_verts.size();
}

GLRectangle::~GLRectangle() {
    delete shader;
    glDeleteBuffers( 6, (GLuint*)&buffer );
}

void GLRectangle::draw( glm::mat4& matrix ) {
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader->bind();
    shader->setParameter( "projection", matrix );
    if ( highlight ) {
        // Draw the center of the highlight
        shader->setParameter( "color", color );
        shader->setAttribute( "position", buffer.center_verts, 2 );
        glDrawArrays(GL_TRIANGLES, 0, center_vertCount );
        // Set the color to have no alpha, then draw the borders.
        glm::vec4 fullAlpha = glm::vec4(color.r,color.g,color.b,1);
        shader->setParameter( "color", fullAlpha );
        shader->setAttribute( "position", buffer.corner_verts, 2 );
        glDrawArrays(GL_TRIANGLES, 0, corner_vertCount );
        shader->setAttribute( "position", buffer.rectangle_verts, 2 );
        glDrawArrays(GL_TRIANGLES, 0, rectangle_vertCount );
    } else {
        shader->setParameter( "color", color );
        shader->setAttribute( "position", buffer.corner_verts, 2 );
        glDrawArrays(GL_TRIANGLES, 0, corner_vertCount );
        shader->setAttribute( "position", buffer.rectangle_verts, 2 );
        glDrawArrays(GL_TRIANGLES, 0, rectangle_vertCount );
    }
    shader->unbind();
    glDisable( GL_BLEND );
}

glm::vec4 GLRectangle::getRect() {
    return glm::vec4( bl.x, bl.y, ur.x-ul.x, ul.y-bl.y );
}
