#ifndef BOX_HPP
#define BOX_HPP

#include <vector>

using namespace std;

class Box {
public:
	Box(float height, float width, float length);
private:
	float h, w, l;
	vector<int> indices;
	vector<float> vertices;
};

#endif