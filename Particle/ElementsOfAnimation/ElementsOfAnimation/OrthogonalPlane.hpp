#ifndef ORTHOGONALPLANE_HPP
#define ORTHOGONALPLANE_HPP

//Include glew
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include "C:\Users\Albin\Desktop\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"
#include "C:\Users\Albin\Desktop\Particle\deps\include\glm-0.9.7.1\glm\gtc\matrix_transform.hpp"

#include <new>
#include <vector>

class OrthogonalPlane {
public:
	OrthogonalPlane(glm::vec3 norm, glm::vec3 pos, float w, float l);
	std::vector<GLfloat> getVertices();
	std::vector<GLuint> getIndices();
private:
	glm::vec3 normal;
	glm::vec3 position;
	float width, length;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

#endif
