#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>

#include "shaderLoader.h"
#include "Box.h"
#include "Plane.h"


using namespace glm;
using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// GLOBAL VARIABLES
//View-matrix needs to be global for the key_callback function to have access to it
mat4 view;
bool leftMousePressed = false;
double mouseX, mouseY;
double timestep = 0.001;
bool wireframe = false;
bool lock_fps = false;
int procedural = 1;
float procedural_scale_length = 0.5;
float procedural_scale_height = 1;

Plane * plane1;

//Some global variables for the fps-counter
double t0 = 0.0;
int frames = 0;
char titlestring[200];

int main()
{
	int windowWidth = 1200;
	int windowHeight = 800;
	int force_fps = 60;
	//Starting position of camera
	view = lookAt(vec3(1.2f, 3.0f, -4), vec3(0, 0, 0), vec3(0, 1, 0));
	vec3 lightPos(0.0f, 0.7f, 2.5f);

	//Plane properties
	int n_segments = 200;
	float segment_size = 0.1f;//2.0f / n_segments;
	float damping = 0.995;

	//INITIATE STUFF ======================================================

	//Initiate glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Try to create a window
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Pathfinding", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Initiate glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//Tell OpenGL the size of the viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);

	// Create and compile our GLSL program from the shaders
	GLuint shaderProgramID = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");


	//Register external intpu in GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Save the initial cursor position
	glfwGetCursorPos(window, &mouseX, &mouseY);



	//END OF INITIATION ====================================================

	plane1 = new Plane(n_segments, segment_size, damping);

	vector<float> randomness;
	for (int i = 0; i < plane1->vertices.size() / 3; i++)
		randomness.push_back((float)(rand() % 5) / 300.0f);

	//Run the application until the user closes the window
	while (!glfwWindowShouldClose(window))
	{

		//Checks if any events are triggered (like keyboard or mouse events)
		glfwPollEvents();
		//Update mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Rendering commands here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (!procedural)
		{
			plane1->updateVertexPos(timestep);
			plane1->updateNormals();
		}
		plane1->draw(shaderProgramID);

		//Create tranformation matrix
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
		//trans = glm::rotate(trans, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 model;
		model = glm::rotate(model, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = mat4(1.0);

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

		mat4 MVP = projection * view * model;

		//Send the transformation matrix to the vertex shader
		GLuint transformLoc = glGetUniformLocation(shaderProgramID, "MVP");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		GLuint modeltransLoc = glGetUniformLocation(shaderProgramID, "model");
		glUniformMatrix4fv(modeltransLoc, 1, GL_FALSE, glm::value_ptr(model));

		//UNIFORMS
		//Set lightpos uniform
		GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		//Set light color uniform
		GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Set light's color (white)
		//Set camera position uniform
		GLint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
		mat3 rotMat(view);
		vec3 transl(view[3]);
		vec3 camera_pos = -transl * rotMat;
		glUniform3f(viewPosLoc, camera_pos.x, camera_pos.y, camera_pos.z);
		GLint shaderTimeLoc = glGetUniformLocation(shaderProgramID, "glfw_time");
		glUniform1f(shaderTimeLoc, (float)glfwGetTime());
		GLint proceduralLoc = glGetUniformLocation(shaderProgramID, "procedural");
		glUniform1i(proceduralLoc, procedural);
		GLint proceduralScaleLengthLoc = glGetUniformLocation(shaderProgramID, "scale_length");
		glUniform1f(proceduralScaleLengthLoc, procedural_scale_length);
		GLint proceduralScaleHeightLoc = glGetUniformLocation(shaderProgramID, "scale_height");
		glUniform1f(proceduralScaleHeightLoc, procedural_scale_height);


		//Swap the buffers
		glfwSwapBuffers(window);

		if(lock_fps)
			Sleep(1000 / (float)force_fps);

		//Show fps in window title
		// Get current time
		double t = glfwGetTime();  // Gets number of seconds since glfwInit()
		// If one second has passed, or if this is the very first frame
		if ((t - t0) > 1.0 || frames == 0)
		{
			double fps = (double)frames / (t - t0);
			if (lock_fps)
				sprintf(titlestring, "Water simulation (%.1f FPS, locked fps)", fps);
			else
				sprintf(titlestring, "Water simulation (%.1f FPS, unlocked fps)", fps);
			glfwSetWindowTitle(window, titlestring);
			t0 = t;
			frames = 0;
		}
		frames++;
	}

	// Properly de-allocate all resources once they've outlived their purpose
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	//Clean/delete all resources that we allocated
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		if (wireframe)
			wireframe = false;
		else
			wireframe = true;

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		if (procedural == 0)
		{
			procedural = 1;
			plane1->resetSimulation();
		}
		else
		{
			procedural = 0;
		}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		if (lock_fps)
			lock_fps = false;
		else
			lock_fps = true;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		procedural_scale_length /= 1.1;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		procedural_scale_length *= 1.1;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		procedural_scale_height /= 1.1;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		procedural_scale_height *= 1.1;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (leftMousePressed)
	{
		//Rotate around Y-axis
		view = rotate(view, (float)(xpos - mouseX) / 100, vec3(0.0f, 1.0f, 0.0f));

		//Rotate around up/down
		mat3 rotMat(view);
		vec3 transl(view[3]);
		vec3 camera_pos = -transl * rotMat;

		camera_pos = vec3(camera_pos.x, 0.0f, camera_pos.z);
		vec3 temp = normalize(cross(vec3(0.0f, 1.0f, 0.0f), camera_pos));
		view = rotate(view, (float)(ypos - mouseY) / 150, temp);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Zoom in and out through scrolling
	view = scale(view, vec3(1.0 + 0.1*yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMousePressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftMousePressed = false;
}

void showFPS() {


}