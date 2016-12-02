#include "Plane.h"

#include <iostream>

Plane::Plane(int n_segments, float segment_size)
	: n_seg(n_segments), seg_size(segment_size)
{
	for (int i = 0; i < (n_seg+1)*(n_seg+1); i++)
	{
		v.push_back(0.0f);
		u.push_back(0.0f);
	}
	createVAO();
	resetSimulation(5.0);

}

void Plane::createVertices()
{
	//Create vertices and vertice's color
	for (int i = 0; i < n_seg+1; i++)
		for (int j = 0; j < n_seg+1; j++)
		{
			//Vertex position
			vertices.push_back(vec3(j*seg_size - (float)(n_seg*seg_size) / 2, 0.0f, (float)(n_seg*seg_size) / 2 - i*seg_size));
			//Vertex normal
			vertices.push_back(vec3(0, 1, 0));
			//Vertex color			
			vertices.push_back(vec3(0.1f, 0.3f, 0.6f)); //blue
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

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), &vertices[0] , GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);

	//Vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (GLvoid*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
	//Vertex color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (GLvoid*)(2*sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

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

void Plane::updateNormals()
{
	for (int i = 0; i < indices.size(); i += 3)
	{
		vertices[indices[i] * 3 + 1] = vec3(0,0,0);
		vertices[indices[i + 1] * 3 + 1] = vec3(0,0,0);
		vertices[indices[i + 2] * 3 + 1] = vec3(0,0,0);
	}
	for (int i = 0; i < indices.size(); i += 3)
	{
		vec3 A = vertices[indices[i] * 3];
		vec3 B = vertices[indices[i + 1] * 3];
		vec3 C =vertices[indices[i + 2] * 3];

		vec3 normal = normalize(cross(B - A, C - A));

		vertices[indices[i] * 3 + 1] = normalize(normal + vertices[indices[i] * 3 + 1]);
		vertices[indices[i + 1] * 3 + 1] = normalize(normal + vertices[indices[i + 1] * 3 + 1]);
		vertices[indices[i + 2] * 3 + 1] = normalize(normal + vertices[indices[i + 2] * 3 + 1]);
	}
}

void Plane::updateVertexPos(double timestep, double wavespeed_factor, double damp)
{
	int segs = n_seg + 1;
	float h = seg_size;
	double c = (seg_size / timestep)/wavespeed_factor;
	vector<float> forces;

	for(int i = 0; i < segs; i++)
		for (int j = 0; j < segs; j++)
		{
			float f = v[i*segs + j];

				if (i - 1 >= 0)
				f += (c*c) * u[(i - 1)*segs + j]/ (h*h);
				else
				f += (c*c) * u[i*segs + j]/(h*h);

				if (i + 1 < segs)
				f += (c*c) * u[(i + 1)*segs + j]/(h*h);
				else
				f += (c*c) * u[i*segs + j] / (h*h);

				if (j - 1 >= 0)
				f += (c*c) * u[i*segs + j - 1] / (h*h);
				else
				f += (c*c) * u[i*segs + j] / (h*h);

				if (j + 1 < segs)
				f += (c*c) * u[i*segs + j + 1] / (h*h);
				else
				f += (c*c) * u[i*segs + j] / (h*h);

			f -= (c*c) * 4*u[i*segs + j] /(h*h);
			//f *= 0.5; //does affect the simulation?
			forces.push_back(f);
		}

	for (int i = 0; i < u.size(); i++)
	{
		v[i] += forces[i] * timestep;
		v[i] *= damp; //instead of damping the forces
		u[i] += v[i] * timestep;
	}

	for(int i = 0; i < segs; i++)
		for (int j = 0; j < segs; j++)
		{
			vertices[3 * (i*segs + j)].y = u[i*segs + j];
		}

}

void Plane::resetSimulation(float amplitude)
{
	for (int i = 0; i < v.size(); i++)
	{
		v[i] = 0;
		u[i] = 0;
	}

	for (int i = 0; i < n_seg+1; i++)
		for (int j = 0; j < n_seg+1; j++)
		{
			vertices[3 * (i*(n_seg+1) + j)].y = u[i*(n_seg+1) + j];
		}

	//Create som initial waves
	for (int i = 0; i < u.size(); i++)
	{
		if (i % (n_seg+1) < n_seg / 50)
			u[i] = amplitude / 5.0;

	}
	u[((n_seg + 1) / 2)*(n_seg + 1) + (n_seg + 1) / 2] = amplitude;
	u[u.size() - 1] = amplitude;
}

void Plane::changeColor()
{
	if (color == 0)
	{
		//Golden
		for (int i = 0; i < vertices.size(); i += 3)
			vertices[i + 2] = vec3(0.9, 0.7, 0.3);
		color++;
	}
	/*else if (color == 2)
	{
		//Random
		for (int i = 0; i < vertices.size(); i += 3)
		{
			float r1 = (float)(rand() % 255) / 255.0;
			float r2 = (float)(rand() % 255) / 255.0;
			float r3 = (float)(rand() % 255) / 255.0;
			vertices[i + 2] = vec3(r1, r2, r3);
		}
		color++;
	}*/
	else if (color == 1)
	{
		//Silver
		for (int i = 0; i < vertices.size(); i += 3)
			vertices[i + 2] = vec3(0.6f, 0.6f, 0.65f);
		color++;
	}
	else if (color == 2)
	{
		//Blood
		for (int i = 0; i < vertices.size(); i += 3)
			vertices[i + 2] = vec3(0.7f, 0.2f, 0.2f);
		color++;
	}
	else
	{
		//Blue
		for (int i = 0; i < vertices.size(); i += 3)
			vertices[i + 2] = vec3(0.1f, 0.3f, 0.6f);
		color = 0;
	}
}

void Plane::spawnRaindrop(float amplitude)
{
	u[rand() % u.size()] = amplitude;
}