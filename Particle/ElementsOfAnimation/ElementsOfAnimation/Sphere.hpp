#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <GL/glew.h>
#include <vector>

#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\gtx\transform.hpp" 
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\gtc\matrix_transform.hpp"

class Sphere
{
public:
	Sphere(float radius, unsigned int sectors, unsigned int rings);
	std::vector<GLfloat> getVertices();
private:
	float rad;
	unsigned int s, r;
	std::vector<GLfloat> vertices;
};

#endif
