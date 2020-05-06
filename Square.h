#pragma once
#include "Shape.h"
class Square : Shape
{
public:
	Square();
	Square(const std::vector<glm::vec3> f_vertices, const std::vector<glm::vec3> f_colors, const std::vector<GLushort> f_indices, GLuint programId);
	void render(glm::mat4x4& view, glm::mat4x4& projection, cg::GLSLProgram& program);
};

