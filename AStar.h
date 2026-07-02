#pragma once
#include <unordered_map>

struct AStar
{
    Grid& grid;
    Cell* start;
    Cell* end;
    std::vector<Cell*> openSet;
    bool running = false;
    bool found = false;

    AStar(Grid& _grid, Cell* _start, Cell* _end) : grid(_grid), start(_start), end(_end)
    {
        grid.resetPathfindingData();
        start->g = 0;
        start->f = heuristic(start, end);
        openSet.push_back(start);
        start->inOpen = true;
        running = true;
        found = false;

    }

    static double heuristic(const Cell* a, const Cell* b)
    {   // distance - 8 direction movement (hoz, vert, diag)
        double dx = std::abs(a->x - b->x);
        double dy = std::abs(a->y - b->y);
        return (dx + dy) + (std::sqrt(2.0) - 2.0) * std::min(dx, dy);
    }


    bool step()
    {
        if (!running)
            return false;

        if (openSet.empty())
        {
            running = false;
            return false;
        }

        // get cell with smallest f value
        auto it = std::min_element(openSet.begin(), openSet.end(),
            [](Cell* a, Cell* b) { return a->f < b->f; });

        Cell* current = *it;
        openSet.erase(it);
        current->inOpen = false;
        current->visited = true;

        if (current == end)
        {
            running = false;
            found = true;
            return false;
        }

        for (Cell* neighbor : grid.neighbors(current))
        {
            if (neighbor->visited)
                continue;

            double moveCost = (neighbor->x != current->x && neighbor->y != current->y) ? std::sqrt(2.0) : 1.0;
            double tentativeG = current->g + moveCost;

            if (tentativeG < neighbor->g)
            {
                neighbor->parent = current;
                neighbor->g = tentativeG;
                neighbor->f = tentativeG + heuristic(neighbor, end);
                if (!neighbor->inOpen)
                {
                    openSet.push_back(neighbor);
                    neighbor->inOpen = true;
                }
            }
        }

        return true;

    }

    // build final path
    std::vector<Cell*> reconstructPath()
    {
        std::vector<Cell*> path;
        if (!found)
            return path;
        Cell* cur = end;
        while (cur != nullptr)
        {
            path.push_back(cur);
            cur = cur->parent;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    bool isRunning() { return running;  }
    bool isFound() { return found;  }
};
