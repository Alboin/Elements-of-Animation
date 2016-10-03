#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
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
	const int window_w = 1200;
	const int window_h = 800;

	//Create Sphere
	SolidSphere sphere(1, 12, 24);

	//-------------- Create particles -----------------------

	const int n_particles = 500;
	std::vector<Particle> all_particles;
	all_particles.reserve(n_particles);

	for (int i = 0; i < n_particles; i++)
		all_particles.push_back(Particle(0.0f, 0.0f, 0.0f));

	//Create a floor for the particles
	Plane floor(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0));


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
	window = glfwCreateWindow(window_w, window_h, "Particle", NULL, NULL);

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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

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

	//-------------- Plane buffers start -----------------------

	std::vector<GLfloat> plane_vertices;
	std::vector<GLushort> plane_indices;

	int width = 25;

	for (int i = 0; i < width; i++)
		for (int j = 0; j < width; j++)
		{
			plane_vertices.push_back((GLfloat)i - (GLfloat)(width-1) / 2);
			plane_vertices.push_back(-2.0f);
			plane_vertices.push_back((GLfloat)j - (GLfloat)(width-1) / 2);
		}

	for (int i = 0; i < width-1; i++)
		for (int j = 0; j < width-1; j++)
		{
			plane_indices.push_back(j + i*width);
			plane_indices.push_back(j + 1 + i*width);
			plane_indices.push_back(j + width + i*width);
			plane_indices.push_back(j + 1 + i*width);
			plane_indices.push_back(j + 1 + width + i*width);
			plane_indices.push_back(j + width + i*width);
		}

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, plane_vertices.size() * sizeof(GLfloat), &plane_vertices[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer2;
	glGenBuffers(1, &elementbuffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane_indices.size() * sizeof(GLushort), &plane_indices[0], GL_STATIC_DRAW);

	//-------------- Plane buffers end -------------------------

	//-------------- Create random colors ----------------------

	GLuint colorbuffer1;
	glGenBuffers(1, &colorbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);

	//Create a vector with random colors
	std::vector<GLfloat> colors;
	for(int i = 0; i < sphere.getVertices().size() + particle_vertices.size() + plane_vertices.size(); i++)
		colors.push_back((((double) rand() / (RAND_MAX))));

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	
	//----------------------------------------------------------


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, window_w / 2, window_h / 2);

	int loop_n = 0;

	do {


		//"Spawn" the particle as the loop iterates
		if (loop_n < n_particles)
		{
			float randx = (((double)rand() / (RAND_MAX)) - 0.5f) * 0.3f;
			float randz = (((double)rand() / (RAND_MAX)) - 0.5f) * 0.3f;
			all_particles[loop_n] = Particle(randx, 3.5f, randz);
			all_particles[loop_n].setForce(0.0f, -9.8f, 0.0f);
			all_particles[loop_n].setBouncing(0.9f);
			loop_n++;
			//if (loop_n == n_particles)
				//loop_n = 0;
		}


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs(window_w, window_h);
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


		//-------------- Start rendering plane -------------------------------

		glm::mat4 ModelMatrix2 = glm::mat4(3.0);
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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);

		//GL_LINE_LOOP

		// Draw the triangles !
		glDrawElements(
			GL_LINE_LOOP,      // mode
			plane_indices.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		//-------------- End of rendering plane ------------------------------


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

			float disact, disant;
			disact = floor.distPoint2Plane(p->getCurrentPosition());

			p->updateParticle(dt, Particle::UpdateMethod::Verlet);
			p->setLifetime(p->getLifetime() - dt);



			//Check for floor collisions
				disant = disact;
				disact = floor.distPoint2Plane(p->getCurrentPosition());
				if (disant * disact < 0.0f) {
					glm::vec3 new_pos = p->getCurrentPosition() - (1.0f + p->getBouncing()) * (floor.normal * p->getCurrentPosition() + floor.dconst) * floor.normal;
					p->setPosition(new_pos);
					glm::vec3 new_vel = p->getVelocity() - (1.0f + p->getBouncing()) * (floor.normal * p->getVelocity()) * floor.normal;
					new_vel = new_vel - 0.3f * (new_vel - (floor.normal * new_vel) * floor.normal);
					p->setVelocity(new_vel);
					disact = -disact;
				}


			//Check for sphere collisions
			if (glm::length(p->getCurrentPosition()) <= sphere.getRadius())
			{
				float dist = sphere.getRadius() - glm::length(p->getCurrentPosition());
				glm::vec3 new_pos = glm::normalize(p->getCurrentPosition()) * (sphere.getRadius() + dist);
				p->setPosition(new_pos);

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
	glDeleteBuffers(1, &uvbuffer1);
	glDeleteBuffers(1, &normalbuffer1);
	glDeleteBuffers(1, &elementbuffer1);
	glDeleteBuffers(1, &colorbuffer1);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

