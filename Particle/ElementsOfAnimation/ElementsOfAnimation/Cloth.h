#ifndef CLOTH_HPP
#define CLOTH_HPP

//Include glew
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <vector>
#include "Particle.h"
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"


class Cloth {
public:
	Cloth(int cloth_width, int cloth_height, float cloth_spring_constant, glm::vec3 left_corner, glm::vec3 right_corner);
	std::vector<Particle> particles;
	std::vector<glm::vec3> particle_vertices;
	std::vector<GLushort> particle_indices;
	int width, height;
	float spring_constant;
};

#endif