#include "Plane.h"

#include <iostream>
#include <GL/glew.h>

Plane::Plane(int n_segments, float segment_size, float pos_x, float pos_z)
	: n_seg(n_segments), seg_size(segment_size), posX(pos_x), posZ(pos_z)
{
	obstacle = false;
	path = false;
	path_bi = false;
	end = false;
	start = false;
	selected = false;
	explored = false;
	explored_bi = false;
	createVAO();
}

void Plane::makeObstacle()
{
	obstacle = true;
	for (int i = 1; i < vertices.size(); i += 2)
		vertices[i] = vec3(0.0f, 0.25f, 0.25f);
	for (int i = 0; i < vertices.size(); i += 2)
		vertices[i][1] = 0.3f;
}

void Plane::makePath()
{
	if (start || end)
		return;

	path = true;
	if (!path_bi)
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.2f, 0.7f, 0.2f);
	else
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.7f, 0.2f, 0.2f);
}

void Plane::makePath_bi()
{
	path_bi = true;
}

void Plane::makeStart()
{
	start = true;
	for (int i = 1; i < vertices.size(); i += 2)
		vertices[i] = vec3(1.0f, 1.0f, 1.0f);
}

void Plane::makeEnd()
{
	end = true;
	for (int i = 1; i < vertices.size(); i += 2)
		vertices[i] = vec3(0.0f, 0.0f, 0.0f);
}

void Plane::toggleSelected()
{
	if (!selected)
	{
		selected = true;
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.8f, 1.0f, 1.0f);
	}
	else
	{
		selected = false;
		if (end)
			makeEnd();
		else if (start)
			makeStart();
		else if (path)
			makePath();
		else if (obstacle)
			makeObstacle();
		else if (explored)
			setExplored();
		else
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.7f, 0.9f, 1.0f);
	}
}

void Plane::setExplored()
{
	explored = true;
	if (!(end || start || path || obstacle || selected))
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.70f, 1.0f, 0.95f);
}

void Plane::setExplored_bi()
{
	explored = true;
	if (!(end || start || path || obstacle || selected))
		for (int i = 1; i < vertices.size(); i += 2)
			vertices[i] = vec3(0.98f, 0.73f, 0.97f);
}

void Plane::resetStatus()
{
	end = false;
	start = false;
	obstacle = false;
	path = false;
	path_bi = false;
	explored = false;
	explored_bi = false;
	for (int i = 1; i < vertices.size(); i += 2)
		vertices[i] = vec3(0.7f, 0.9f, 1.0f);
	for (int i = 0; i < vertices.size(); i += 2)
		vertices[i][1] = 0.0f;
}

bool Plane::isObstacle()
{
	return obstacle;
}

void Plane::createVertices()
{
	//Create vertices and vertice's color
	for (int i = 0; i < n_seg + 1; i++)
		for (int j = 0; j < n_seg + 1; j++)
		{
			//Vertex position
			vec4 temp = vec4(j*seg_size - (float)(n_seg*seg_size) / 2, 0.0f, (float)(n_seg*seg_size) / 2 - i*seg_size, 1.0f);
			temp = translate(mat4(1.0), vec3(posX, 0.0f, posZ)) * temp;
			vertices.push_back(vec3(temp));
			//Vertex color
			vertices.push_back(vec3(0.7f, 0.9f, 1.0f));
		}

	//Indexing
	for (int i = 0; i < n_seg; i++)
		for (int j = 0; j < n_seg; j++)
		{
			indices.push_back(j + i*(n_seg + 1));
			indices.push_back(j + 1 + i*(n_seg + 1));
			indices.push_back(j + (i + 1)*(n_seg + 1));

			indices.push_back(j + (i + 1)*(n_seg + 1));
			indices.push_back(j + 1 + i*(n_seg + 1));
			indices.push_back(j + 1 + (i + 1)*(n_seg + 1));
		}
}

void Plane::createVAO()
{
	if (vertices.size() == 0)
		createVertices();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

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

}

GLuint Plane::getVAO()
{
	return VAO;
}

void Plane::draw(GLuint shaderProgramID)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

vec3 Plane::getPosition()
{
	return vec3(posX, 0, posZ);
}