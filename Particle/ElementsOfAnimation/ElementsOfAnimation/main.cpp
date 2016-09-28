#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\gtx\transform.hpp" 
#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\gtc\matrix_transform.hpp"

using namespace glm;

#include "shader.hpp"
#include "controls.hpp"
#include "Box.hpp"
#include "SolidSphere.cpp"
#include "Particle.h"
#include "Geometry.h"

int main(void)
{
	int fps = 60;
	float dt = 1 / (float)fps; //simulation time-step
	int start_time = GetTickCount();
	float passed_time = 0.0;

	//Create Sphere
	SolidSphere sphere(1, 12, 24);

	//Create box
	Box boxen(5, 4, 10);

	//-------------- Create particles -----------------------

	const int n_particles = 1000;
	std::vector<Particle> all_particles;
	for (int i = 0; i < n_particles; i++)
	{
		float randx = (((double)rand() / (RAND_MAX))) - 0.5f;
		float randz = (((double)rand() / (RAND_MAX))) - 0.5f;
		all_particles.push_back(Particle(randx, 2.0f, randz));
		//Particle p(1.0f, 2.0f, 1.5f); //initial position of the particle
		all_particles[i].setLifetime(500.0f);
		all_particles[i].setBouncing(0.8f);
		all_particles[i].addForce(0.0f, -9.8f, 0.0f);
		all_particles[i].setVelocity(0.0f, 0.0f, 0.0f);
	}


	// Define a box made out of planes, for collision
	Plane box_restrictions[5] = {
		Plane(glm::vec3(0, -boxen.getMeasurements()[1] / 2,0), glm::vec3(0,1,0)),
		Plane(glm::vec3(boxen.getMeasurements()[0] / 2,0,0), glm::vec3(-1,0,0)),
		Plane(glm::vec3(-boxen.getMeasurements()[0] / 2,0,0), glm::vec3(1,0,0)),
		Plane(glm::vec3(0,0,boxen.getMeasurements()[2] / 2), glm::vec3(0,0,-1)),
		Plane(glm::vec3(0,0,-boxen.getMeasurements()[2] / 2), glm::vec3(0,0,1))
	};


	//-------------------------------------------------------

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Particle", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Set background color
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.glsl", "SimpleFragmentShader.glsl");

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint TimeID = glGetUniformLocation(programID, "TIME");

	//-------------- Sphere buffers start ------------------------

	// Load it into a VBO
	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sphere.getVertices().size() * sizeof(glm::vec3), &sphere.getVertices()[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sphere.getTextcoords().size() * sizeof(glm::vec2), &sphere.getTextcoords()[0], GL_STATIC_DRAW);

	GLuint normalbuffer1;
	glGenBuffers(1, &normalbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sphere.getNormals().size() * sizeof(glm::vec3), &sphere.getNormals()[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer1;
	glGenBuffers(1, &elementbuffer1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndices().size() * sizeof(GLushort), &sphere.getIndices()[0], GL_STATIC_DRAW);
	
	//-------------- Sphere buffers end ------------------------

	//-------------- Box buffers start -------------------------

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, boxen.getVertices().size() * sizeof(GLfloat), &boxen.getVertices()[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer2;
	glGenBuffers(1, &elementbuffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, boxen.getIndices().size() * sizeof(GLushort), &boxen.getIndices()[0], GL_STATIC_DRAW);


	//-------------- Box buffers end ---------------------------

	//-------------- Triangle buffers start --------------------

	GLfloat triangle_vertices[9] =
	{
		 2.0, 2.0, 2.0,
		 2.0,-2.0,-2.0,
		 -2.0,-2.0, 2.0
	};
	GLushort triangle_indices[3] =
	{
		0,1,2
	};

	GLuint vertexbuffer3;
	glGenBuffers(1, &vertexbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), &triangle_vertices[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer3;
	glGenBuffers(1, &elementbuffer3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLushort), &triangle_indices[0], GL_STATIC_DRAW);


	//-------------- Triangle buffers end ----------------------

	//-------------- Particle buffers start --------------------

	// Set point size, points used for particle rendering
	glPointSize(10);

	std::vector<GLfloat> particle_vertices;
	std::vector<GLushort> particle_indices;

	for (int i = 0; i < n_particles; i++)
	{
		particle_vertices.push_back(all_particles[i].getCurrentPosition()[0]);
		particle_vertices.push_back(all_particles[i].getCurrentPosition()[1]);
		particle_vertices.push_back(all_particles[i].getCurrentPosition()[2]);
		particle_indices.push_back((GLushort)i);
	}

	/*GLfloat particle_vertices[n_particles * 3] =
	{
		p.getCurrentPosition()[0], p.getCurrentPosition()[1], p.getCurrentPosition()[2]
	};
	GLushort particle_indices[n_particles] =
	{
		0
	};*/

	GLuint vertexbuffer4;
	glGenBuffers(1, &vertexbuffer4);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer4);
	glBufferData(GL_ARRAY_BUFFER, n_particles * 3 * sizeof(GLfloat), &particle_vertices[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer4;
	glGenBuffers(1, &elementbuffer4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_particles * sizeof(GLushort), &particle_indices[0], GL_STATIC_DRAW);

	//-------------- Particle buffers end ----------------------


	//-------------- Create random colors ----------------------

	GLuint colorbuffer1;
	glGenBuffers(1, &colorbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);

	//Create a vector with random colors
	std::vector<GLfloat> colors;
	for(int i = 0; i < boxen.getVertices().size() + sphere.getVertices().size() /*+ particle_vertices.size()*/; i++)
		colors.push_back((((double) rand() / (RAND_MAX))));

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	
	//----------------------------------------------------------


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		//-------------- Start rendering sphere ------------------------------

		glm::mat4 ModelMatrix1 = glm::mat4(3.0);
		glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);

		//GL_LINE_LOOP

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			sphere.getIndices().size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		//-------------- End of rendering sphere -----------------------------

		//-------------- Start rendering box ---------------------------------

		glm::mat4 ModelMatrix2 = glm::mat4(1.0);
		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			boxen.getIndices().size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		//-------------- End of rendering box --------------------------------

		//-------------- Start rendering triangle ----------------------------

		glm::mat4 ModelMatrix3 = glm::mat4(1.0);
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix3[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer3);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			3,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		//-------------- End of rendering triangle ---------------------------

		//-------------- Start rendering particles ---------------------------

		glm::mat4 ModelMatrix4 = glm::mat4(1.0);
		glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * ModelMatrix4;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix4[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer4);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer4);

		// Draw the triangles !
		glDrawElements(
			GL_POINTS,      // mode
			particle_indices.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		//-------------- End of rendering particles --------------------------


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));

		//Pass time variable to uniform variable in fragmentshader
		glUniform1f(TimeID, passed_time);
		passed_time = (GetTickCount() - start_time) * 0.001f;

		//Here starts the loop for calulating new positions and velocities for particles
		for (int i = 0; i < n_particles; i++)
		{
			Particle *p = &all_particles[i];

			float disact[5], disant[5];
			for (int i = 0; i < 5; i++)
				disact[i] = box_restrictions[i].distPoint2Plane(p->getCurrentPosition());

			p->updateParticle(dt, Particle::UpdateMethod::EulerOrig);
			p->setLifetime(p->getLifetime() - dt);

			//Check for plane collisions
			for (int i = 0; i < 5; i++)
			{
				disant[i] = disact[i];
				disact[i] = box_restrictions[i].distPoint2Plane(p->getCurrentPosition());
				if (disant[i] * disact[i] < 0.0f) {
					glm::vec3 new_pos = p->getCurrentPosition() - (1.0f + p->getBouncing()) * (box_restrictions[i].normal * p->getCurrentPosition() + box_restrictions[i].dconst) * box_restrictions[i].normal;
					p->setPosition(new_pos);
					glm::vec3 new_vel = p->getVelocity() - (1.0f + p->getBouncing()) * (box_restrictions[i].normal * p->getVelocity()) * box_restrictions[i].normal;
					p->setVelocity(new_vel);
					disact[i] = -disact[i];
				}
			}

			//Check for triangle collisions
			glm::vec3 A = glm::vec3(triangle_vertices[0], triangle_vertices[1], triangle_vertices[2]);
			glm::vec3 B = glm::vec3(triangle_vertices[3], triangle_vertices[4], triangle_vertices[5]);
			glm::vec3 C = glm::vec3(triangle_vertices[6], triangle_vertices[7], triangle_vertices[8]);
			glm::vec3 triangle_normal = glm::normalize(glm::cross((B - A), (C - A)));
			glm::vec3 particle_projection = p->getCurrentPosition() - glm::dot(p->getCurrentPosition() - A, triangle_normal) * triangle_normal;

			float A1 = (1.0f / 2.0f) * glm::length(glm::cross(B - particle_projection, C - particle_projection));
			float A2 = (1.0f / 2.0f) * glm::length(glm::cross(particle_projection - A, C - A));
			float A3 = (1.0f / 2.0f) * glm::length(glm::cross(B - A, particle_projection - A));
			float A4 = (1.0f / 2.0f) * glm::length(glm::cross(B - A, C - A));

			if ((A1 + A2 + A3 - A4) >= 0)
			{
				float d = -1.0f * dot(triangle_normal, glm::vec3(triangle_vertices[0], triangle_vertices[1], triangle_vertices[2]));
				float dist_to_triangle = (dot(triangle_normal, p->getCurrentPosition()) + d) * (dot(triangle_normal, p->getPreviousPosition()) + d);
				if (dist_to_triangle <= 0.0f)
				{
					glm::vec3 new_pos = p->getCurrentPosition() - 2.0f * (glm::dot(p->getCurrentPosition(), triangle_normal) + d) * triangle_normal;
					p->setPosition(new_pos);

					glm::vec3 new_vel = p->getVelocity() - (1.0f + p->getBouncing()) * glm::dot(p->getVelocity(), triangle_normal) * triangle_normal;
					p->setVelocity(new_vel);
				}
			}


			//Check for sphere collisions
			if (glm::length(p->getCurrentPosition()) <= sphere.getRadius())
			{
				//glm::vec3 new_pos = p.getCurrentPosition()

				glm::vec3 new_vel = p->getVelocity() - (1.0f + p->getBouncing()) * glm::dot(p->getVelocity(), glm::normalize(p->getCurrentPosition())) * glm::normalize(p->getCurrentPosition());
				p->setVelocity(new_vel);
			}

			//Update the vertix-array with the new particle positions
			particle_vertices[i * 3] = p->getCurrentPosition()[0];
			particle_vertices[(i * 3) + 1] = p->getCurrentPosition()[1];
			particle_vertices[(i * 3) + 2] = p->getCurrentPosition()[2];

		} //End of particle loop

		//Send the updated data to the buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer4);
		glBufferData(GL_ARRAY_BUFFER, n_particles * 3 * sizeof(GLfloat), &particle_vertices[0], GL_STATIC_DRAW);

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &vertexbuffer3);
	glDeleteBuffers(1, &uvbuffer1);
	glDeleteBuffers(1, &normalbuffer1);
	glDeleteBuffers(1, &elementbuffer1);
	glDeleteBuffers(1, &elementbuffer2);
	glDeleteBuffers(1, &elementbuffer3);
	glDeleteBuffers(1, &colorbuffer1);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

