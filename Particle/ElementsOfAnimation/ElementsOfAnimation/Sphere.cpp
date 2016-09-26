#include "Sphere.hpp"

Sphere::Sphere(float radius, unsigned int sectors, unsigned int rings)
	:rad(radius), s(sectors), r(rings)
{
	const float M_PI = 3.14159265359;
	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);

	for (int r = 0; r < rings; ++r) {
		for (int s = 0; s < sectors; ++s) {
			float const y = sin(-M_PI/2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			//texcoords.push_back(glm::vec2(s*S, r*R));
			//vertices.push_back(glm::vec3(x, y, z) * radius);
			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z * radius);

		}
	}
}

std::vector<GLfloat> Sphere::getVertices()
{
	return vertices;
}