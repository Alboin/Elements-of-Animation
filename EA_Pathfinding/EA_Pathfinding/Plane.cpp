#include "Plane.h"

#include <iostream>

Plane::Plane(int n_segments, float segment_size)
	: n_seg(n_segments), seg_size(segment_size)
{
	createVAO();
}

void Plane::addObstacle(int coordx, int coordy)
{
	obstacles.push_back(make_pair(coordx, coordy));

}

void Plane::createVertices()
{
	//Create vertices and vertice's color
	for (int i = 0; i < n_seg+1; i++)
		for (int j = 0; j < n_seg+1; j++)
		{
			//Vertex position
			vertices.push_back(j*seg_size - (float)(n_seg*seg_size) / 2);
			vertices.push_back(0.0f);
			vertices.push_back((float)(n_seg*seg_size) / 2 - i*seg_size);
			//Vertex color
			vertices.push_back(1.0f * ((float)i)/(float)n_seg * ((float)j)/(float)n_seg);
			vertices.push_back(0.5f);
			vertices.push_back(0.0f);
		}

	//Indexing
	for (int i = 0; i < n_seg; i++)
		for (int j = 0; j < n_seg; j++)
		{
			indices.push_back(j + i*(n_seg+1));
			indices.push_back(j + 1 + i*(n_seg+1));
			indices.push_back(j + (i + 1)*(n_seg+1));

			indices.push_back(j + (i + 1)*(n_seg + 1));
			indices.push_back(j + 1 + i*(n_seg + 1));
			indices.push_back(j + 1 + (i+1)*(n_seg + 1));
		}
}

void Plane::createVAO()
{
	createVertices();

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0] , GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);

	//Vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	plane_vao = VAO;
}

GLuint Plane::getVAO()
{
	return plane_vao;
}

void Plane::draw(GLuint shaderProgramID)
{
	// Draw our first triangle
	glUseProgram(shaderProgramID);
	glBindVertexArray(plane_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}