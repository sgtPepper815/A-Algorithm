#pragma once
#include <queue>
#include <iostream>

struct Cell
{
	int x, y;
	bool wall = false;

	double g = std::numeric_limits<double>::infinity();
	double f = std::numeric_limits<double>::infinity();

	bool visited = false;
	bool inOpen = false;
	Cell* parent = nullptr;

	void reset()
	{
		g = std::numeric_limits<double>::infinity();
		f = std::numeric_limits<double>::infinity();

		visited = false;
		inOpen = false;
		parent = nullptr;
	}



};
