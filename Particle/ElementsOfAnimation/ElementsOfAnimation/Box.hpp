#ifndef BOX_HPP
#define BOX_HPP

#include <GL/glew.h>
#include <new>
#include <vector>

class Box {
public:
	Box(float w, float h, float l);
	std::vector<GLfloat> getVertices();
	std::vector<GLushort> getIndices();
private:
	float width, height, length;
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
};

#endif
