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

		for (int y = 0; y < rows; ++y)
			for (int x = 0; x < cols; ++x)
				at(x, y) = Cell{ x, y };
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

	std::vector<Cell*> neighbors(Cell* c)
	{
		std::vector<Cell*> result;
		static const int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
		static const int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};
		for (int i = 0; i < 8; i++)
		{
			int cx = c->x + dx[i];
			int cy = c->y + dy[i];
			if (!inBounds(cx, cy))
				continue;
			Cell& cb = at(cx, cy);
			if (cb.wall)
				continue;

			// don't cut walls diagonally
			if (dx[i] != 0 && dy[i] != 0)
			{
				if (at(cx, c->y).wall && at(c->x, cy).wall)
					continue;
			}

			result.push_back(&cb);

		}

		return result;
	}

	void resetPathfindingData()
	{
		for (auto& c : cells) c.reset();
	}

	void clearWalls()
	{
		for (auto& c : cells) c.wall = false;
	}


};
