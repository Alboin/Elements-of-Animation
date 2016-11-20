#include <iostream>
#include <vector>
#include <string>
#include <math.h>

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

int main()
{
	int windowWidth = 800;
	int windowHeight = 600;
	int fps = 60;
	//Starting position of camera
	view = lookAt(vec3(1, 1, 1), vec3(0, 0, 0), vec3(0, 1, 0));

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



	Plane plane1 = Plane(20, 0.1f);


	//Run the application until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Checks if any events are triggered (like keyboard or mouse events)
		glfwPollEvents();
		//Update mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Rendering commands here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		plane1.draw(shaderProgramID);


		//Create tranformation matrix
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
		//trans = glm::rotate(trans, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 model;
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = mat4(1.0);

		glm::mat4 projection;
		projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

		mat4 MVP = projection * view * model;

		//Send the transformation matrix to the vertex shader
		GLuint transformLoc = glGetUniformLocation(shaderProgramID, "MVP");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		//Swap the buffers
		glfwSwapBuffers(window);

		Sleep(1000 / (float)fps);


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
		
		if (camera_pos.y < 0.1)
		{
			if (ypos - mouseY > 0)
			{
				camera_pos = vec3(camera_pos.x, 0.0f, camera_pos.z);
				vec3 temp = normalize(cross(vec3(0.0f, 1.0f, 0.0f), camera_pos));
				view = rotate(view, (float)(ypos - mouseY) / 150, temp);
			}
		}
		else if (sqrt(camera_pos.x * camera_pos.x + camera_pos.z * camera_pos.z) > 0.5f)
		{
			camera_pos = vec3(camera_pos.x, 0.0f, camera_pos.z);
			vec3 temp = normalize(cross(vec3(0.0f, 1.0f, 0.0f), camera_pos));
			view = rotate(view, (float)(ypos - mouseY) / 100, temp);
		}
		else if (ypos - mouseY < 0)
		{
			camera_pos = vec3(camera_pos.x, 0.0f, camera_pos.z);
			vec3 temp = normalize(cross(vec3(0.0f, 1.0f, 0.0f), camera_pos));
			view = rotate(view, (float)(ypos - mouseY) / 100, temp);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	view = scale(view, vec3(1.0 + 0.1*yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMousePressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftMousePressed = false;
}