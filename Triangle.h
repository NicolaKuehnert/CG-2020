#pragma once
#include "Shape.h"
class Triangle : Shape
{
public:
	Triangle();
	Triangle(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<GLushort>  indices, GLuint programId);
	void render(glm::mat4x4& view, glm::mat4x4& projection, cg::GLSLProgram& program);
};

