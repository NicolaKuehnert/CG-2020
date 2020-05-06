#pragma once
#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"
class Object
{
public:
    inline Object()
        : vao(0),
        positionBuffer(0),
        colorBuffer(0),
        indexBuffer(0)
    {}

    inline ~Object() { // GL context must exist on destruction
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &positionBuffer);
    }

    GLuint vao;        // vertex-array-object ID

    GLuint positionBuffer; // ID of vertex-buffer: position
    GLuint colorBuffer;    // ID of vertex-buffer: color

    GLuint indexBuffer;    // ID of index-buffer

    glm::mat4x4 model; // model matrix
};

