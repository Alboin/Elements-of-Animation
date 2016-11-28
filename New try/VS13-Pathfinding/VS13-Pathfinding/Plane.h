#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Plane
{
public:
	Plane(int n_segments, float segment_size, float pos_x = 0, float pos_y = 0);
	void makeObstacle();
	bool isObstacle();
	void createVertices();
	void createVAO();
	void draw(GLuint shaderProgramID);
	GLuint getVAO();

private:
	int n_seg;
	float seg_size;
	float posX;
	float posZ;

	bool obstacle;

	GLuint VBO, VAO, EBO;
	vector<vec3> vertices;
	vector<GLuint> indices;
};

#endif
