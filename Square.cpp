#include "Square.h"

Square::Square()
{
}

Square::Square(const std::vector<glm::vec3> f_vertices, const std::vector<glm::vec3> f_colors, const std::vector<GLushort> f_indices, GLuint programId) {

    vertices = f_vertices;
    colors = f_colors;
    indices = f_indices;

    GLuint pos;

    // Step 0: Create vertex array object.
    glGenVertexArrays(1, &shape.vao);
    glBindVertexArray(shape.vao);

    // Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
    glGenBuffers(1, &shape.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shape.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Bind it to position.
    pos = glGetAttribLocation(programId, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Step 2: Create vertex buffer object for color attribute and bind it to...
    glGenBuffers(1, &shape.colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shape.colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

    // Bind it to color.
    pos = glGetAttribLocation(programId, "color");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Step 3: Create vertex buffer object for indices. No binding needed here.
    glGenBuffers(1, &shape.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

    // Unbind vertex array object (back to default).
    glBindVertexArray(0);

    // Modify model matrix.
    shape.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f));
}

void Square::render(glm::mat4x4& view, glm::mat4x4& projection, cg::GLSLProgram& program)
{
    mvp = projection * view * shape.model;

    // Bind the shader program and set uniform(s).
    program.use();
    program.setUniform("mvp", mvp);

    // Bind vertex array object so we can render the 2 triangles.
    glBindVertexArray(shape.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
