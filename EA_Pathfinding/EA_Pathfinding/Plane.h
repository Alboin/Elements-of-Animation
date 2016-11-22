#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

class Plane
{
	public:
		Plane(int n_segments, float segment_size);
		void createVertices();
		void createVAO();
		void draw(GLuint shaderProgramID);
		void updateNormals();

		vector<vec3> vertices;

	private:
		int n_seg;
		float seg_size;

		GLuint VBO, VAO, EBO;

		vector<GLuint> indices;
		vector<pair <int,int> > obstacles;
};

#endif
