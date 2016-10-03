#include "Cloth.h"
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"


Cloth::Cloth(int cloth_width, int cloth_height, float cloth_spring_constant, glm::vec3 left_corner, glm::vec3 right_corner)
	:width(cloth_width), height(cloth_height), spring_constant(cloth_spring_constant)
{

	for(int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			particles.push_back(Particle(i, -j, 0));
			particle_vertices.push_back(glm::vec3(i, -j, 0));

			//Indexing
			particle_indices.push_back((GLushort)(j + i*width));

			particles.back().addForce(glm::vec3(0, -9.8f, 0));
			if ((i == 0 && j == 0) || (i == width-1 && j == 0))
				particles.back().setFixed(true);
		}


	//particles;
	//particle_vertices;
	//particle_indices;
}