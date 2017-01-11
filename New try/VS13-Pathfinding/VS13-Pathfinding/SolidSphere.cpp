//Include glew
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
//extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <new>
#include <vector>
#include <cmath>
#include <iostream>

class SolidSphere
{
protected:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<GLuint> indices;
	float r;
	std::vector<glm::vec3> sphere_VBO;
	GLuint VBO, VAO, EBO;

public:
	SolidSphere(float radius, unsigned int rings, unsigned int sectors)
		:r(radius)
	{
		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);
		int r, s;

		const float M_PI = 3.14159265359;

		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<glm::vec3>::iterator v = vertices.begin();
		std::vector<glm::vec3>::iterator n = normals.begin();
		std::vector<glm::vec2>::iterator t = texcoords.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = sin((-1)*(M_PI / 2) + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*t++ = glm::vec2(s*S, r*R);

			*v++ = glm::vec3(x * radius, y * radius, z * radius);

			*n++ = glm::vec3(x, y, z);
		}

		indices.resize(rings * sectors * 6);
		std::vector<GLuint>::iterator i = indices.begin();
		for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
			*i++ = r * sectors + s;
			*i++ = (r + 1) * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);

			*i++ = r * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = r * sectors + (s + 1);

		}
		//calculateNormals();
		createVAO();

	}

	void updateVertices(glm::vec3 new_pos)
	{
		for (int i = 0; i < sphere_VBO.size(); i+=2)
		{
			sphere_VBO[i] = vertices[i/2] + new_pos;
		}
	}

	void draw(GLuint shaderProgramID)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sphere_VBO.size() * sizeof(glm::vec3), &sphere_VBO[0], GL_STATIC_DRAW);
		glUseProgram(shaderProgramID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void calculateNormals()
	{
		for (int i = 0; i < indices.size(); i+=3)
		{
			glm::vec3 normal = glm::normalize(glm::cross(vertices[indices[i]], vertices[indices[i+1]]));
			normals[indices[i]] = normal;
			normals[indices[i+1]] = normal;
			normals[indices[i+2]] = normal;
		}
	}

	void createVAO()
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			sphere_VBO.push_back(vertices[i]);
			if (i % 5 > 0)
			{
				sphere_VBO.push_back(glm::vec3(1.0, 0.5, 0.5));
			}
			else
			{
				sphere_VBO.push_back(glm::vec3(0.8, 0.3, 0.3));
				//sphere_VBO.push_back(glm::vec3(0.5, 1.0, 0.5));
			}
			//sphere_VBO.push_back(normals[i]);
		}
		for (int i = 0; i < indices.size(); i++)
		{
			indices[i] = indices[i] * 2;
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sphere_VBO.size() * sizeof(glm::vec3), &sphere_VBO[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);

		//Vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//Vertex normal attribute
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
		//glEnableVertexAttribArray(0);
		//Vertex color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	}

	std::vector<glm::vec3> getVertices()
	{
		return vertices;
	}
	std::vector<glm::vec3> getNormals()
	{
		return normals;
	}
	std::vector<glm::vec2> getTextcoords()
	{
		return texcoords;
	}
	std::vector<GLuint> getIndices()
	{
		return indices;
	}
	float getRadius()
	{
		return r;
	}
};