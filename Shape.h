#pragma once
#include "Object.h"

class Shape
{
public:
	Shape();
	Shape( std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<GLushort>  indices);
protected:
	Object shape;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 mvp;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<GLushort>  indices;
};


