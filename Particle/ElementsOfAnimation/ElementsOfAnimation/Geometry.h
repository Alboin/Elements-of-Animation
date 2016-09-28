#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "C:\Users\Albin\Documents\GitHub\Elements-of-Animation\Particle\deps\include\glm-0.9.7.1\glm\glm.hpp"

struct Geometry{
	virtual void setPosition(const glm::vec3& newPos) = 0;
	virtual bool isInside(const glm::vec3& point) = 0;
};

struct Plane : public Geometry {
	glm::vec3 normal;
	float dconst;
	Plane(){};
	~Plane() {};
	Plane(const glm::vec3& point, const glm::vec3& normalVect);
	Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2);

	void setPosition(const glm::vec3& newPos);
	bool isInside(const glm::vec3& point);
	float distPoint2Plane(const glm::vec3& point);
	glm::vec3 closestPointInPlane(const glm::vec3& point);
	bool intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall);
};	

struct Triangle : public Plane {
	//glm::vec3 vertex1, vertex2, vertex3;
	//Triangle(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2);
	//~Triangle() {};
	//void setPosition(const glm::vec3& newPos);
	//bool isInside(const glm::vec3& point);
	//bool intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall);
};

struct Sphere : public Geometry {
	//glm::vec3 center;
	//float radi;
	//Sphere(const glm::vec3& point, const float& radious);
	//~Sphere() {};
	//void setPosition(const glm::vec3& newPos);
	//bool isInside(const glm::vec3& point);
	//bool intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall);
};

#endif
