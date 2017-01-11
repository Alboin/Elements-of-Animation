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
	void makePath();
	void makePath_bi();
	void makeEnd();
	void makeStart();
	void toggleSelected();
	void setExplored();
	void setExplored_bi();
	void resetStatus();
	bool isObstacle();
	void createVertices();
	void createVAO();
	void draw(GLuint shaderProgramID);
	GLuint getVAO();
	vec3 getPosition();

private:
	int n_seg;
	float seg_size;
	float posX;
	float posZ;

	bool obstacle;
	bool path;
	bool path_bi;
	bool end;
	bool start;
	bool selected;
	bool explored;
	bool explored_bi;

	GLuint VBO, VAO, EBO;
	vector<vec3> vertices;
	vector<GLuint> indices;
};

#endif
