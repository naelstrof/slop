#ifndef N_RECTANGLE_H_
#define N_RECTANGLE_H_

#include "gl_core_3_3.h"
#include <iostream>
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>

#include "shader.hpp"

struct RectangleBuffer {
    unsigned int corner_verts;
    unsigned int corner_uvs;
    unsigned int rectangle_verts;
    unsigned int rectangle_uvs;
    unsigned int center_verts;
    unsigned int center_uvs;
};

class Rectangle {
private:
    glm::vec2 ul, oul;
    glm::vec2 bl, obl;
    glm::vec2 ur, our;
    glm::vec2 br, obr;
    bool highlight;
    void generateBuffers();
    RectangleBuffer buffer;
    unsigned int corner_vertCount;
    unsigned int rectangle_vertCount;
    unsigned int center_vertCount;
    float border;
    float padding;
    Shader* shader;
    glm::vec4 color;
public:
    glm::vec4 getRect();
    Rectangle(glm::vec2 p1, glm::vec2 p2, float border = 1, float padding = 0, glm::vec4 color = glm::vec4(1,1,1,1), bool highlight = false );
    ~Rectangle();
    void setPoints( glm::vec2 p1, glm::vec2 p2 );
    void draw(glm::mat4& matrix);
};

#endif // N_RECTANGLE_H_
