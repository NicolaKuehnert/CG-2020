#include "Shape.h"

Shape::Shape()
{
}

/*
* Init the Shape with given vertices, colors and indices
*/
Shape::Shape(const std::vector<glm::vec3> f_vertices, const std::vector<glm::vec3> f_colors, const std::vector<GLushort>  f_indices)
{
    vertices = f_vertices;
    colors = f_colors;
    indices = f_indices;

}

