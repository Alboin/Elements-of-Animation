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
	Cloth(int cloth_width, int cloth_height, float real_width, float starting_height, float starting_angle);
	
	void setSpringConstant(float k);
	void setSpringDamping(float d);
	void calcSpringForces();

	std::vector<Particle> particles;
	std::vector<glm::vec3> particle_vertices;
	std::vector<GLushort> particle_indices;
	int width, height;
	float spring_constant, spring_length, spring_damping;
};

#endif