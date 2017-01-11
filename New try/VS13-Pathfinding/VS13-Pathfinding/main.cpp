#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stack>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <cal3d/cal3d.h>

#include <windows.h>

#include "shaderLoader.h"
#include "Plane.h"
#include "Particle.h"
#include "SolidSphere.cpp"
#include "Grid.cpp"
//#include "global.h"
//#include "Model.h"


using namespace glm;
using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void renderCal3dModel(Model * m);


// GLOBAL VARIABLES
//View-matrix needs to be global for the key_callback function to have access to it
mat4 view;
bool leftMousePressed = false;
double mouseX, mouseY;
bool wireframe = false;
//Plane variables and constants
int selected_x = 0, selected_y = 0;
const int segments = 20;
vector< vector <Plane> > Area;
//Create navigation mesh
Grid nav_grid(segments, &Area);
//Particle variables
float radius = 0.25;
vector<Particle> particles;
vector<SolidSphere> spheres;
vector<stack<vec3> > waypoints;
vector<stack<vec3> > bi_waypoints;
stack<vec3> all_waypoints;
stack<vec3> all_bi_waypoints;

const float plate_size = 1;
GLuint shaderProgramID;

//void renderModelMesh() {}

int main()
{
	int windowWidth = 1000;
	int windowHeight = 800;
	int fps = 120;
	float dt = 1 / (0.2*(float)fps);
	//Starting position of camera
	view = lookAt(vec3(10, 10, 0), vec3(0, 0, 0), vec3(0, 1, 0));

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
	shaderProgramID = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");

	//Register external intpu in GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Save the initial cursor position
	glfwGetCursorPos(window, &mouseX, &mouseY);


	//END OF INITIATION ====================================================

	//Create Area out of planes
	Area.resize(segments);
	for (int i = 0; i < segments; i++)
		for (int j = 0; j < segments; j++)
		{
			Area[i].push_back(Plane(1, plate_size, plate_size / 2 + j*plate_size - (float)segments*plate_size / 2, (float)segments*plate_size / 2 - plate_size / 2 - i*plate_size));// , ((float)j)*plate_size, ((float)i)*plate_size));
			if (rand() % 4 < 1)
			{
				Area[i][j].makeObstacle();
				nav_grid.setToClosed(i, j);
			}
		}

	Area[selected_x][selected_y].toggleSelected();


	/*Model *pModel;
	pModel = new Model();
	pModel->setPath("C:/Users/Albin/Documents/GitHub/Elements-of-Animation/EA_Pathfinding/data/cally/");
	if (!pModel->onInit("C:/Users/Albin/Documents/GitHub/Elements-of-Animation/EA_Pathfinding/data/cally.cfg"))
	{
		delete pModel;
		std::cout << endl << "Model initialization failed! (cally)" << std::endl;
		//return false;
	}
	float temp[3] = { 0.3f, 0.3f, 0.4f };
	pModel->setMotionBlend(temp, 0.1f);
	pModel->setState(Model::STATE_IDLE, 0.3f);*/
	//pModel->onInit("C:/Users/Albin/Documents/GitHub/Elements-of-Animation/EA_Pathfinding/data/cally.cfg");

	//CalCoreModel * myCoreModel = new CalCoreModel("cally");

	bool first_run = true;


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

		for (int i = 0; i < Area.size(); i++)
			for (int j = 0; j < Area[0].size(); j++)
				Area[i][j].draw(shaderProgramID);

		glPointSize(10);
		glLineWidth(5);

		//Loop through particles and update forces and so forth..
		for (int i = 0; i < particles.size(); i++)
		{
			Particle *p = &particles[i];

			//Outer wall collision
			if (p->getCurrentPosition()[0] > segments / 2 * plate_size) {
				p->setVelocity(-p->getVelocity()[0], 0, p->getVelocity()[2]);
				p->setPosition(p->getCurrentPosition() + glm::vec3(-0.01,0,0));
			}
			if (p->getCurrentPosition()[0] < -segments / 2 * plate_size) {
				p->setVelocity(-p->getVelocity()[0], 0, p->getVelocity()[2]);
				p->setPosition(p->getCurrentPosition() + glm::vec3(0.01, 0, 0));
			}
			if (p->getCurrentPosition()[2] > segments / 2 * plate_size) {
				p->setPosition(p->getCurrentPosition() + glm::vec3(0, 0,-0.01));
				p->setVelocity(p->getVelocity()[0], 0, -p->getVelocity()[2]);
			}
			if (p->getCurrentPosition()[2] < -segments / 2 * plate_size) {
				p->setPosition(p->getCurrentPosition() + glm::vec3(0, 0, 0.01));
				p->setVelocity(p->getVelocity()[0], 0, -p->getVelocity()[2]);
			}

			//Particle collision with other particles
			for (int j = i+1; j < particles.size(); j++)
				if (j != i)
				{
					glm::vec3 dist = p->getCurrentPosition() - particles[j].getCurrentPosition();
					if (glm::length(dist) < radius + 0.4)
					{
						dist = glm::normalize(dist);
						glm::vec3 reflection1 = p->getVelocity() - 2 * glm::dot(dist, p->getVelocity()) * dist;
						glm::vec3 reflection2 = particles[j].getVelocity() - 2 * glm::dot(dist, particles[j].getVelocity()) * dist;

						p->setVelocity(reflection1);
						particles[j].setVelocity(reflection2);

						p->setPosition(p->getCurrentPosition() + 0.01f * dist);
					}
				}
			
			//Avoiding of other particles heading this way
			for (int j = 0; j < particles.size(); j++)
				if (j != i)
				{
					vec3 p1 = p->getCurrentPosition() + p->getVelocity()*1.2f;
					vec3 p2 = particles[j].getCurrentPosition();
					if (glm::length(p1 - p2) < radius)
						p->addForce(normalize(cross(p->getVelocity(), vec3(0, 1, 0)))*75.0f);
				}

			vec3 render_wayp = vec3(0, 1000, 0);
			if (bi_waypoints.size() == 0 && !waypoints[i].empty())
			{
				//Create force towards next waypoint if only A* is used.
				vec3 wayp = waypoints[i].top();
				vec3 temp = glm::normalize(wayp - p->getCurrentPosition());
				p->addForce(glm::normalize(wayp - p->getCurrentPosition()) * 1.0f);

				if (glm::length(wayp - p->getCurrentPosition()) < 0.3)
					waypoints[i].pop();
				render_wayp = wayp;
			}
			else if (!(p->bi_particle) && !waypoints[i/2].empty())
			{
				//Create force towards next waypoint if bidirectional A* is used.
				vec3 wayp = waypoints[i/2].top();
				vec3 temp = glm::normalize(wayp - p->getCurrentPosition());
				p->addForce(glm::normalize(wayp - p->getCurrentPosition()) * 1.0f);
				
				if (glm::length(wayp - p->getCurrentPosition()) < 0.3)
					waypoints[i/2].pop();
				render_wayp = wayp;
			}
			else if ((p->bi_particle) && !bi_waypoints[i/2].empty())
			{
				//Create force towards next waypoint if bidirectional A* is used.
				vec3 wayp = bi_waypoints[i/2].top();
				vec3 temp = glm::normalize(wayp - p->getCurrentPosition());
				p->addForce(glm::normalize(wayp - p->getCurrentPosition()) * 1.0f);

				if (glm::length(wayp - p->getCurrentPosition()) < 0.3)
					bi_waypoints[i/2].pop();
				render_wayp = wayp;
			}

			//Restrict the velocity
			if (glm::length(p->getVelocity()) > 0.7)
				p->setVelocity(glm::normalize(p->getVelocity()) * 0.5f);

			vec3 friction = -(p->getVelocity() * 0.5f);
			p->addForce(friction);

			//Check for collisions with closed areas.
			for (int a = 0; a < segments; a++)
				for (int b = 0; b < segments; b++)
				{
					if (Area[a][b].isObstacle())
					{
						vec3 pos = Area[a][b].getPosition();
						vec3 dist_vec = p->getCurrentPosition() - pos;
						dist_vec[1] = 0;

						if (abs(dist_vec[0]) < (plate_size / 2.0 + radius) && abs(dist_vec[2]) < (plate_size / 2.0 + radius))
						{
							glm::vec3 reflection = p->getVelocity() - 2 * glm::dot(dist_vec, p->getVelocity()) * dist_vec;
							p->setVelocity(reflection);
							p->setPosition(p->getCurrentPosition() + 0.01f * dist_vec);
						}
					}
				}
			p->updateParticle(dt, Particle::UpdateMethod::EulerSemi);
			p->setForce(glm::vec3(0, 0, 0));
			spheres[i].updateVertices(particles[i].getCurrentPosition());
			spheres[i].draw(shaderProgramID);
			p->draw(shaderProgramID, render_wayp);
		}

		//CAL3D STUFF
		/*pModel->onUpdate(0.1);
		pModel->setMotionBlend(temp, 0.1f);
		pModel->setState(Model::STATE_IDLE, 0.3f);
		pModel->onRender();
		renderCal3dModel(pModel);*/

		// get the renderer of the model
		//CalRenderer *pCalRenderer;
		//pCalRenderer = pModel->getRenderer();

		glm::mat4 model;
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

		if (first_run)
		{
			first_run = false;
			int index1 = 1;
			std::string instructions = "The scene can be rotated by dragging with the mouse. \n";
			instructions.append("Zoom in and out through scrolling. \n\n");
			instructions.append("Move the marked cell with the arrow-keys. \n");
			instructions.append("Key: S -> Set A* starting point. \n");
			instructions.append("Key: E -> Set A* ending point. \n\n");
			instructions.append("Key: Enter -> Run the regular A* algorithm. \n");
			instructions.append("Key: B -> Run the bidirectional A* algorithm. \n\n");
			instructions.append("Key: P -> Spawn one 'particle' to follow the path, more can \n be added by pressing again. \n\n");
			instructions.append("Key: R -> Reset the area: remove paths and particles. \n");
			instructions.append("Key: C -> Clear the area: remove everything, including closed cells. \n");
			instructions.append("Key: Space -> Close the marked cell. \n");
			instructions.append("Key: M -> Generate a new map. \n");
			MessageBoxA(NULL, instructions.c_str(), "Instructions", MB_OK);
		}



	}

	glfwTerminate();
	//delete myCoreModel;
	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS && !wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframe = true;
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS && wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframe = false;
	}
	bool selection = false;
	int prev_x = selected_x, prev_y = selected_y;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		selection = true;
		selected_y++;
		if (selected_y >= segments)
			selected_y = 0;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		selection = true;
		selected_y--;
		if (selected_y < 0)
			selected_y = segments - 1;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		selection = true;
		selected_x--;
		if (selected_x < 0)
			selected_x = segments - 1;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		selection = true;
		selected_x++;
		if (selected_x >= segments)
			selected_x = 0;
	}
	if (selection)
	{
		Area[prev_x][prev_y].toggleSelected();
		Area[selected_x][selected_y].toggleSelected();
		selection = false;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		Area[selected_x][selected_y].makeObstacle();
		nav_grid.setToClosed(selected_x, selected_y);
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		Area[selected_x][selected_y].makeStart();
		nav_grid.starting_point.first = selected_x;
		nav_grid.starting_point.second = selected_y;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		Area[selected_x][selected_y].makeEnd();
		nav_grid.setGoal(selected_x, selected_y);
	}
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		int explored_nodes = nav_grid.calculatePath(nav_grid.starting_point.first, nav_grid.starting_point.second);
		int path = 0;
		Node *p = nav_grid.goal_node;
		stack<vec3> temp;
		while (p != nav_grid.start_node)
		{
			temp.push(vec3((p->y) - segments / 2 + plate_size / 2.0, radius, -(p->x) + segments / 2 - plate_size / 2.0));
			Area[(*p).x][(*p).y].makePath();
			//cout << (*(*p).parent).x << ", " << (*(*p).parent).y << endl;
			p = (*p).parent;
			path++;
		}
		waypoints.push_back(temp);
		all_waypoints = temp;

		cout << endl << "Regular A*";
		cout << endl << "Nodes explored: " << explored_nodes << endl;
		cout << "Number of nodes in path: " << path - 1 << endl;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		int explored_nodes = nav_grid.calculateBidirectionalPath(nav_grid.starting_point.first, nav_grid.starting_point.second);
		int path = 0;
		Node *p = nav_grid.goal_node;
		stack<vec3> temp;

		while (p != nav_grid.start_node)
		{
			temp.push(vec3(p->y - segments / 2 + plate_size / 2.0, radius, -(p->x) + segments / 2 - plate_size / 2.0));
			Area[(*p).x][(*p).y].makePath();
			p = (*p).parent;
			path++;
		}
		waypoints.push_back(temp);
		all_waypoints = temp;

		stack<vec3> temp_reverse;
		temp_reverse.push(vec3(p->y - segments / 2 + plate_size / 2.0, radius, -(p->x) + segments / 2 - plate_size / 2.0));
		while (!temp.empty())
		{
			temp_reverse.push(temp.top());
			temp.pop();
		}
		temp_reverse.pop();
		bi_waypoints.push_back(temp_reverse);
		all_bi_waypoints = temp_reverse;

		cout << endl << "Bidirectional A*";
		cout << endl << "Nodes explored: " << explored_nodes << endl;
		cout << "Number of nodes in path: " << path - 1 << endl;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		for (int i = 0; i < segments; i++)
			for (int j = 0; j < segments; j++)
				Area[i][j].resetStatus();
		particles.clear();
		spheres.clear();
		waypoints.clear();
		bi_waypoints.clear();
		nav_grid = Grid(segments, &Area);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		for (int i = 0; i < segments; i++)
			for (int j = 0; j < segments; j++)
			{
				if (!Area[i][j].isObstacle())
				{
					Area[i][j].resetStatus();
				}
				else
				{
					Area[i][j].resetStatus();
					Area[i][j].makeObstacle();
				}
			}
		particles.clear();
		spheres.clear();
		waypoints.clear();
		bi_waypoints.clear();
		nav_grid = Grid(segments, &Area);
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		for (int i = 0; i < segments; i++)
			for (int j = 0; j < segments; j++)
			{
				Area[i][j].resetStatus();
				if (rand() % 4 < 1)
				{
					Area[i][j].makeObstacle();
					nav_grid.setToClosed(i, j);
				}
			}
		nav_grid = Grid(segments, &Area);

	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{		
		if (particles.size() % 2 != 0 && bi_waypoints.size() > 0)
		{
			if (particles.size() > 1)
				bi_waypoints.push_back(all_bi_waypoints);

			particles.push_back(Particle(nav_grid.goal.second - segments / 2 + plate_size / 2.0,
				0.25,
				-nav_grid.goal.first + segments / 2 - plate_size / 2.0));
			particles.back().bi_particle = true;
		}
		else
		{
			if (particles.size() > 0)
				waypoints.push_back(all_waypoints);

			particles.push_back(Particle(nav_grid.starting_point.second - segments / 2 + plate_size / 2.0,
				0.25,
				-nav_grid.starting_point.first + segments / 2 - plate_size / 2.0));
		}
		spheres.push_back(SolidSphere(radius, 16, 16));
	}
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
	view = scale(view, vec3(1.0 + 0.1*yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftMousePressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftMousePressed = false;
}

/*void renderCal3dModel(Model * m)
{
	//for (int i = 0; i < m->n_vertices; i++)
		//cout << m->vertices[i][0] << ", " << m->vertices[i][1] << ", " << m->vertices[i][2] << endl;
	
	//vector<GLuint> indices;
	//for (int i = 0; i < m->n_vertices*3; i++)
		//indices.push_back(i);
	
	
	//cout << m->n_vertices << endl;

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m->vert.size(), &(m->vert[0]), GL_STATIC_DRAW);
	glUseProgram(shaderProgramID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->face.size(), &(m->face[0]), GL_STATIC_DRAW);

	//Vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glDrawElements(GL_POINTS, m->face.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

}*/

