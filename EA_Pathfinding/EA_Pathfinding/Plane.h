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
		void updateVertexPos(double timestep, double wavespeed_factor, double damp);
		void resetSimulation(float amplitude);
		void changeColor();

		vector<vec3> vertices;
		vector<float> u;
		vector<float> v;

	private:
		int n_seg;
		float seg_size;
		float damp;
		int color = 0;

		GLuint VBO, VAO, EBO;

		vector<GLuint> indices;

};

#endif
