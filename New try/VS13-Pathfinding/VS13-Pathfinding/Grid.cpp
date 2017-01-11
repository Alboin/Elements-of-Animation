#include <vector>
#include <queue>
#include <iostream>
#include "Node.cpp"
#include "Plane.h"

using namespace std;

class Grid
{
public:
	vector<vector<Node> > nodes;
	vector<Node*> open_list;
	vector<Node*> open_list_bi;
	vector<Node*> closed_list;
	vector<Node*> closed_list_bi;
	vector<vector<Plane> > * area_pointer;

	int gridsize;
	pair<int, int> starting_point;
	pair<int, int> goal;
	Node *goal_node;
	Node *start_node;
	bool goal_exist;

	int calculateBidirectionalPath(int start_x, int start_y);

	Grid(int size, vector<vector<Plane> > * p)
		:gridsize(size), goal_exist(false), area_pointer(p)
	{
		for (int i = 0; i < size; i++)
		{
			vector<Node> temp;
			for (int j = 0; j < size; j++)
				temp.push_back(Node(i, j));
			nodes.push_back(temp);
		}
		if (p->size() != 0)
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					if ((*area_pointer)[i][j].isObstacle())
						nodes[i][j].closed = true;
		//cout << endl << "Grid created of size: " << nodes.size() << "x" << nodes[0].size() << endl << endl;
	}

	void setGoal(int x, int y)
	{
		if (goal_exist)
			nodes[goal.first][goal.second].is_goal = false;
		nodes[x][y].is_goal = true;
		goal = make_pair(x, y);
		goal_node = &nodes[x][y];
		goal_exist = true;
		updateNodes();
	}

	void setToClosed(int x, int y)
	{
		nodes[x][y].closed = true;
	}

	int calculatePath(int start_x, int start_y)
	{
		int explored_nodes = 0;
		starting_point = make_pair(start_x, start_y);

		start_node = &nodes[start_x][start_y];
		Node *current_node = start_node;

		while (!(*current_node).is_goal)
		{
			closed_list.push_back(&(*current_node));

			//Loop through all node positions
			for (int i = 0; i < gridsize; i++)
				for (int j = 0; j < gridsize; j++)
				{
					//If the position is one position away from our current node
					if (!(i == (*current_node).x && j == (*current_node).y))
						if (i >= (*current_node).x - 1 && i <= (*current_node).x + 1)
							if (j >= (*current_node).y - 1 && j <= (*current_node).y + 1)
							{
								Node *compare_node = &nodes[i][j];
								(*area_pointer)[i][j].setExplored();

								//Have we reached our goal?
								if (compare_node == goal_node && !(abs(abs(i - (*current_node).x) + abs(j - (*current_node).y) - 2) < 0.01))
								{
									(*goal_node).parent = current_node;
									current_node = goal_node;
									return explored_nodes;
								}
								else if (!(*compare_node).closed) //closed here means "blocked"
								{
									//Has the node already been added to the closedlist?
									bool compare_is_closed = false;
									for (int a = 0; a < closed_list.size(); a++)
										if (closed_list[a] == compare_node)
										{
											compare_is_closed = true;
											break;
										}

									if (!compare_is_closed)
									{
										//Calculate move cost to node from current
										if (abs(abs(i - (*current_node).x) + abs(j - (*current_node).y) - 2) < 0.01)
											(*compare_node).g = 14000; //Diagonal
										else
											(*compare_node).g = 10; //Horizontal or vertical

										if ((*compare_node).g + (*compare_node).h < (*compare_node).f)
										{
											(*compare_node).update_f();
											(*compare_node).parent = &(*current_node);
										}
										//Add the compared node to the openlist if it does not already exist.
										bool exist = false;
										for (int a = 0; a < open_list.size(); a++)
											if (open_list[a] == compare_node)
											{
												exist = true;
												break;
											}
										if (!exist)
										{
											explored_nodes++;
											open_list.push_back(&(*compare_node));
										}
									}
									

								}
							}
				} //Finished with current_node

			//Choose the node with lowest "cost" as the next current_node
			current_node = open_list[0];
			for (int a = 1; a < open_list.size(); a++)
			{
				if ((*open_list[a]).f < (*current_node).f)
				{
					current_node = open_list[a];
					open_list.erase(open_list.begin() + a);
				}
			}
			if (current_node == open_list[0])
				open_list.erase(open_list.begin());

		}
		return explored_nodes;
	}


private:
	void updateNodes()
	{
		for (int i = 0; i < nodes.size(); i++)
			for (int j = 0; j < nodes[i].size(); j++)
				nodes[i][j].h = abs(nodes[i][j].x - goal.first) + abs(nodes[i][j].y - goal.second);
	}
	void bi_updateNodes()
	{
		for (int i = 0; i < nodes.size(); i++)
			for (int j = 0; j < nodes[i].size(); j++)
				nodes[i][j].h2 = abs(nodes[i][j].x - starting_point.first) + abs(nodes[i][j].y - starting_point.second);
	}

};