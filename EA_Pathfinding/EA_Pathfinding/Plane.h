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
		Plane(int n_segments, float segment_size, float damping);
		void createVertices();
		void createVAO();
		void draw(GLuint shaderProgramID);
		void updateNormals();
		void updateVertexPos(double timestep);
		void resetSimulation();

		vector<vec3> vertices;
		vector<float> u;
		vector<float> v;

	private:
		int n_seg;
		float seg_size;
		float damp;

		GLuint VBO, VAO, EBO;

		vector<GLuint> indices;

};

#endif
