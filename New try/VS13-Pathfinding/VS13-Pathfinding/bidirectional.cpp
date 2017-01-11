#include "Grid.cpp"

int Grid::calculateBidirectionalPath(int start_x, int start_y)
{
	int explored_nodes = 0;
	starting_point = make_pair(start_x, start_y);

	start_node = &nodes[start_x][start_y];
	Node *current_node = start_node;
	Node *bi1_current_node = start_node;
	Node *bi2_current_node = goal_node;

	bi_updateNodes();
	bool bi_side = false;

	while (true)
	{
		if (!bi_side)
		{
			current_node = bi1_current_node;
			closed_list.push_back(&(*current_node));
		}
		else
		{
			current_node = bi2_current_node;
			closed_list_bi.push_back(&(*current_node));
		}


		//Loop through all node positions
		for (int i = 0; i < gridsize; i++)
			for (int j = 0; j < gridsize; j++)
			{
				//If the position is one position away from our current node
				if (!(i == (*current_node).x && j == (*current_node).y))
					if (i >= (*current_node).x - 1 && i <= (*current_node).x + 1)
						if (j >= (*current_node).y - 1 && j <= (*current_node).y + 1 && !(abs(abs(i - (*current_node).x) + abs(j - (*current_node).y) - 2) < 0.01))
						{
							Node *compare_node = &nodes[i][j];
							if (!bi_side)
								(*area_pointer)[i][j].setExplored();
							else
								(*area_pointer)[i][j].setExplored_bi();

							bool reached_goal = false;
							bool found_on_bi_side = false;
							if (!bi_side)
							{
								for (int a = 0; a < open_list_bi.size(); a++)
									if (compare_node == open_list_bi[a])
									{
										found_on_bi_side = true;
										reached_goal = true;
										break;
									}
								for (int a = 0; a < closed_list_bi.size(); a++)
									if (compare_node == closed_list_bi[a])
									{
										found_on_bi_side = true;
										reached_goal = true;
										break;
									}
							}
							else
							{
								for (int a = 0; a < open_list.size(); a++)
									if (compare_node == open_list[a])
									{
										reached_goal = true;
										break;
									}
								for (int a = 0; a < closed_list.size(); a++)
									if (compare_node == closed_list[a])
									{
										reached_goal = true;
										break;
									}
							}
								
							//Have we reached our goal?
							if (reached_goal)
							{
								if (!(*compare_node).bi_parent)
									(*compare_node).bi_parent = bi2_current_node;

								Node *p = compare_node;
								Node *p_next = (*compare_node).bi_parent;
								Node *p_next_next = (*(*compare_node).bi_parent).bi_parent;


								while (p_next && p && p != goal_node)
								{
									(*area_pointer)[p->x][p->y].makePath_bi();
									if (p_next_next)
									{
										(*p_next).parent = p;
										p = p_next;
										p_next = p_next_next;
										p_next_next = (*p_next_next).bi_parent;
									}
									else
									{
										(*p_next).parent = p;
										break;
									}

								}
								if (found_on_bi_side)
									(*compare_node).parent = bi1_current_node;
								return explored_nodes;
							}
							else if (!(*compare_node).closed) //closed here means "blocked"
							{
								//Has the node already been added to the closedlist?
								bool compare_is_closed = false;
								if (!bi_side)
								{
									for (int a = 0; a < closed_list.size(); a++)
										if (closed_list[a] == compare_node)
										{
											compare_is_closed = true;
											break;
										}
								}
								else
								{
									for (int a = 0; a < closed_list_bi.size(); a++)
										if (closed_list_bi[a] == compare_node)
										{
											compare_is_closed = true;
											break;
										}
								}


								if (!compare_is_closed)
								{
									//Calculate move cost to node from current
									if (abs(abs(i - (*current_node).x) + abs(j - (*current_node).y) - 2) < 0.01)
										(*compare_node).g = 14000; //Diagonal
									else
										(*compare_node).g = 10; //Horizontal or vertical

									if (!bi_side && (*compare_node).g + (*compare_node).h < (*compare_node).f)
									{
										(*compare_node).update_f();
										(*compare_node).parent = &(*current_node);
									}
									else if (bi_side && (*compare_node).g + (*compare_node).h2 < (*compare_node).f2)
									{
										(*compare_node).update_f2();
										(*compare_node).bi_parent = &(*current_node);
									}
									//Add the compared node to the openlist if it does not already exist.
									bool exist = false;
									if (!bi_side)
									{
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
									else
									{
										for (int a = 0; a < open_list_bi.size(); a++)
											if (open_list_bi[a] == compare_node)
											{
												exist = true;
												break;
											}
										if (!exist)
										{
											explored_nodes++;
											open_list_bi.push_back(&(*compare_node));
										}
									}
								}
							}
						}
			} //Finished with current_node

			//Choose the node with lowest "cost" as the next current_node
			if (!bi_side)
			{
				bi1_current_node = open_list[0];
				for (int a = 1; a < open_list.size(); a++)
				{
					if ((*open_list[a]).f < (*bi1_current_node).f)
					{
						bi1_current_node = open_list[a];
						open_list.erase(open_list.begin() + a);
					}
				}
				if (bi1_current_node == open_list[0])
					open_list.erase(open_list.begin());
				bi_side = true;
			}
			else
			{
				bi2_current_node = open_list_bi[0];
				for (int a = 1; a < open_list_bi.size(); a++)
				{
					if ((*open_list_bi[a]).f2 < (*bi2_current_node).f2)
					{
						bi2_current_node = open_list_bi[a];
						open_list_bi.erase(open_list_bi.begin() + a);
					}
				}
				if (bi2_current_node == open_list_bi[0])
					open_list_bi.erase(open_list_bi.begin());
				bi_side = false;
			}


	}
	return explored_nodes;
}