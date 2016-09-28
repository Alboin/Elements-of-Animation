#include "Box.hpp"

Box::Box(float w, float h, float l)
	:width(w), height(h), length(l)
{

	GLfloat vertxs[24] =
	{
		 1.0, 1.0,-1.0,
		 1.0,-1.0,-1.0,
		 1.0,-1.0, 1.0,
		 1.0, 1.0, 1.0,
		-1.0, 1.0,-1.0,
		-1.0, 1.0, 1.0,
		-1.0,-1.0, 1.0,
		-1.0,-1.0,-1.0
	};

	for (int i = 0; i < 24; i += 3)
	{
		vertices.push_back(vertxs[i] * width / 2);
		vertices.push_back(vertxs[i+1] * height / 2);
		vertices.push_back(vertxs[i+2] * length / 2);
	}

	GLushort indxs[30] =
	{
		0,1,2,
		0,2,3,
		1,6,2,
		1,7,6,
		4,5,6,
		4,6,7,
		2,6,3,
		5,3,6,
		0,4,7,
		0,7,1
	};

	for (int i = 0; i < 30; i++)
		indices.push_back(indxs[i]);
}

std::vector<GLfloat> Box::getVertices()
{
	return vertices;
}

std::vector<GLushort> Box::getIndices()
{
	return indices;
}

glm::vec3 Box::getMeasurements()
{
	return glm::vec3(width, height, length);
}

