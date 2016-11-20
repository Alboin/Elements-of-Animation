#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <GL/glew.h>

using namespace std;

class Plane
{
	public:
		Plane(int n_segments, float segment_size);
		void addObstacle(int coordx, int coordy);
		void createVertices();
		void createVAO();
		void draw(GLuint shaderProgramID);
		GLuint getVAO();

	private:
		int n_seg;
		float seg_size;
		GLuint plane_vao;
		vector<GLfloat> vertices;
		vector<GLuint> indices;
		vector<pair <int,int> > obstacles;
};

#endif
