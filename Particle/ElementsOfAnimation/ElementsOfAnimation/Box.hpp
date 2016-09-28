#ifndef BOX_HPP
#define BOX_HPP

#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"

#include <GL/glew.h>
#include <new>
#include <vector>

class Box {
public:
	Box(float w, float h, float l);
	std::vector<GLfloat> getVertices();
	std::vector<GLushort> getIndices();
	glm::vec3 getMeasurements();
private:
	float width, height, length;
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
};

#endif
