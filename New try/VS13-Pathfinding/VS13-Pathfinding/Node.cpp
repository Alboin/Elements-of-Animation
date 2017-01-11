

class Node
{
public:
	Node(int xpos, int ypos)
		:x(xpos), y(ypos), closed(false), f(100000), f2(100000), parent(nullptr), bi_parent(nullptr), is_goal(false)
	{
	}
	/*bool operator<(const Node &n) const
	{
		return f > n.f;
	}*/
	void update_f()
	{
		f = g + h;
	}
	void update_f2()
	{
		f2 = g + h2;
	}
	int x;
	int y;
	float h;
	float h2;
	float g;
	float f;
	float f2;
	bool closed;
	bool is_goal;
	Node * parent;
	Node * bi_parent;
};