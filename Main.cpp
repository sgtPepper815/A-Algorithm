
#include <SDL.h>
#include <iostream>
#include <optional>
#include "Grid.h"
#include "Cell.h"
#include "AStar.h"

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 600;
constexpr int COLS = 5;
constexpr int ROWS = 5;

struct Color { Uint8 r, g, b, a; };
constexpr Color COL_BG{ 30, 30, 30, 255 };
constexpr Color COL_GRID{ 55, 55, 55, 255 };
constexpr Color COL_WALL{ 20, 20, 20, 255 };
constexpr Color COL_START{ 46, 204, 113, 255 };
constexpr Color COL_END{ 231, 76, 60, 255 };
constexpr Color COL_OPEN{ 52, 152, 219, 255 };
constexpr Color COL_CLOSED{ 41, 128, 185, 120 };
constexpr Color COL_PATH{ 241, 196, 15, 255 };


void draw_color(SDL_Renderer* r, Color c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}


void draw_grid(SDL_Renderer* renderer, Grid& grid, Cell* start, Cell* end, std::vector<Cell*> path)
{
    draw_color(renderer, COL_BG);
    SDL_RenderClear(renderer);



    SDL_RenderPresent(renderer);

}

int main(int argc, char* argv[])
{
    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Could not init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* m_window = SDL_CreateWindow("A* Algorithm", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!m_window)
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

    if (!m_renderer)
    {
        std::cout << "Could not create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    Grid grid(COLS, ROWS);

    Cell cell;

    Cell* start = &grid.at(2, ROWS / 2);
    Cell* end = &grid.at(COLS - 3, ROWS / 2);

    std::optional<AStar> a_star;
    std::vector<Cell*> path;

    bool setting_start = false;
    bool setting_end = false;

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_SPACE:
                    if (start && end)
                    {
                        path.clear();
                        a_star.emplace(grid, start, end);
                    }
                    break;

                case SDLK_r:
                    // reset
                    grid =  Grid(COLS, ROWS);
                    start = &grid.at(2, ROWS / 2);
                    end = &grid.at(COLS - 3, ROWS / 2);
                    path.clear();
                    a_star.reset();
                    break;

                case SDLK_s:
                    setting_start = true;
                    break;

                case SDLK_z:
                    setting_end = true;
                    break;
                }
                break;
            }
        }


        draw_grid(m_renderer, grid, start, end, path);
        SDL_Delay(16); // ~ 60 FPS

    }

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

    return 0;
}

