#pragma once
#include <iostream>
#include "Cell.h"

struct Grid
{

	int cols;
	int rows;
	std::vector<Cell> cells;

	Grid(int _cols, int _rows)
	{
		cols = _cols;
		rows = _rows;
		cells.resize(_cols * _rows);
	}

	// get cell at [x, y]
	Cell& at(int x, int y)
	{
		return cells[y * cols + x];
	}

	// check if coordinates inside the grid
	bool inBounds(int x, int y)
	{
		return x >= 0 && x < cols && y >= 0 && y < rows;
	}


};
