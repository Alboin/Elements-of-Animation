#include "OrthogonalPlane.hpp"

OrthogonalPlane::OrthogonalPlane(glm::vec3 norm, glm::vec3 pos, float w, float l)
	:normal(norm), position(pos), width(w), length(l)
{

	//Fill the vector with vertices so that we create a plane parallell tothe xz-plane which
	//will be rotated and translated to its right position later on.
	for (int i = 0; i < 6; i++)
	{
		vertices.push_back(1 * width/2);
		vertices.push_back(0);
		vertices.push_back(1 * length/2);
	}
	for (int i = 0; i < vertices.size(); i++)
		if (i == 5 || i == 6 || i == 9 || i == 14 || i == 15 || i == 17)
			vertices.at(i) = vertices.at(i) * (-1);
	
	//If normal is y-axis
	glm::mat3 rotation(1);
	if (normal[0] != 0) //If normal is x-axis
		rotation = glm::mat3(0, -1, 0, 1, 0, 0, 0, 0, 1);
	else if (normal[2] != 0) //If normal is z-axis
		rotation = glm::mat3(1, 0, 0, 0, 0, -1, 0, 1, 0);
	

	for (int i = 0; i < vertices.size(); i += 3)
	{
		//Create 3d vector and perform rotation.
		glm::vec3 pos(vertices.at(i), vertices.at(i + 1), vertices.at(i + 2));
		glm::vec3 rotated_pos = pos * rotation;
		
		//Make it a 4d vector and perform translation.
		glm::vec4 translate_pos(rotated_pos, 1);
		glm::mat4 translation(1, 0, 0, position[0],
							  0, 1, 0, position[1],
							  0, 0, 1, position[2],
							  0, 0, 0, 1);
		glm::vec4 new_pos =  translate_pos * translation;

		//Put the modified values back into the vector.
		vertices.at(i) = new_pos[0];
		vertices.at(i+1) = new_pos[1];
		vertices.at(i+2) = new_pos[2];
	}

}

std::vector<GLfloat> OrthogonalPlane::getVertices()
{
	return vertices;
}

std::vector<GLuint> OrthogonalPlane::getIndices()
{
	return indices;
}