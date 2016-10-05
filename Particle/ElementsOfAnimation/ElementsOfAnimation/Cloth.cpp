#include "Cloth.h"
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"
#include <math.h>
#include <iostream>

Cloth::Cloth(int cloth_width, int cloth_height, float real_width, float starting_height, float starting_angle)
	:width(cloth_width), height(cloth_height)
{
	//Default values
	spring_damping = 20;
	spring_constant = 200;

	spring_length = real_width / width;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			glm::vec4 pos = glm::vec4(j*spring_length - real_width / 2, -i*spring_length, 0, 1);
			glm::mat4 rotX = glm::mat4(1, 0, 0, 0, 0, cos(starting_angle), -sin(starting_angle), 0, 0, sin(starting_angle), cos(starting_angle), 0, 0, 0, 0, 1);
			glm::mat4 transY = glm::mat4(1, 0, 0, 0, 0, 1, 0, starting_height, 0, 0, 1, 0, 0, 0, 0, 1);
			pos = pos * rotX;
			pos = pos * transY;

			particles.push_back(Particle(pos[0], pos[1], pos[2]));
			particles.back().setBouncing(0.1);
			particle_vertices.push_back(glm::vec3(pos[0], pos[1], pos[2]));

			//Indexing
			particle_indices.push_back((GLushort)(i + j*height));

			//Set top left and right corner of cloth to fixed
			if ((i == 0 && j == 0) || (j == width-1 && i == 0))
				particles.back().setFixed(true);
		}
}

void Cloth::setSpringConstant(float k)
{
	spring_constant = k;
}

void Cloth::setSpringDamping(float d)
{
	spring_damping = d;
}

void Cloth::calcSpringForces()
{
	int j = -1;

	//Reset forces
	for (int i = 0; i < particles.size(); i++)
		particles[i].setForce(0, -0.1, 0);

	for (int i = 0; i < particles.size(); i++)
	{
		//Calculate these forces for all particles except right side.
		if ((i+1) % width != 0)
		{
			//Calculate forces to particle to the right.
			j = i + 1;
		}
		if (j != -1)
		{
			glm::vec3 diff_pos = particles[j].getCurrentPosition() - particles[i].getCurrentPosition();
			glm::vec3 diff_vel = particles[j].getVelocity() - particles[i].getVelocity();
			glm::vec3 f = (spring_constant * (glm::length(diff_pos) - spring_length) + spring_damping * diff_vel * (diff_pos / glm::length(diff_pos))) * (diff_pos / glm::length(diff_pos));
			particles[i].addForce(f);
			particles[j].addForce(-f);
			j = -1;
		}
		//Calculate these forces for all particles except bottom row.
		if (i < particles.size() - width)
		{
			//Calculate forces to particle below.
			j = i + width;
		}
		if (j != -1)
		{
			glm::vec3 diff_pos = particles[j].getCurrentPosition() - particles[i].getCurrentPosition();
			glm::vec3 diff_vel = particles[j].getVelocity() - particles[i].getVelocity();
			glm::vec3 f = (spring_constant * (glm::length(diff_pos) - spring_length) + spring_damping * diff_vel * (diff_pos / glm::length(diff_pos))) * (diff_pos / glm::length(diff_pos));
			particles[i].addForce(f);
			particles[j].addForce(-f);
			j = -1;
		}

	}

}