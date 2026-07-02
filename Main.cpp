
#include <SDL.h>
#include <iostream>
#include <optional>
#include "Grid.h"
#include "Cell.h"
#include "AStar.h"

constexpr int CELL_SIZE = 24;
constexpr int COLS = 45;
constexpr int ROWS = 25;
constexpr int WINDOW_WIDTH = COLS * CELL_SIZE;
constexpr int WINDOW_HEIGHT = ROWS * CELL_SIZE;
constexpr int STEPS_PER_FRAME = 2;

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

    // draw cells
    for (int y = 0; y < grid.rows; ++y) {
        for (int x = 0; x < grid.cols; ++x) {
            Cell& c = grid.at(x, y);
            SDL_Rect rect{ x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };

            if (c.wall) {
                draw_color(renderer, COL_WALL);
                SDL_RenderFillRect(renderer, &rect);
            }
            else if (c.visited) {
                draw_color(renderer, COL_CLOSED);
                SDL_RenderFillRect(renderer, &rect);
            }
            else if (c.inOpen) {
                draw_color(renderer, COL_OPEN);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // create path
    draw_color(renderer, COL_PATH);
    for (Cell* c : path) {
        SDL_Rect rect{ c->x * CELL_SIZE, c->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    // start - finish
    if (start) {
        draw_color(renderer, COL_START);
        SDL_Rect rect{ start->x * CELL_SIZE, start->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }
    if (end) {
        draw_color(renderer, COL_END);
        SDL_Rect rect{ end->x * CELL_SIZE, end->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    // cell lines
    draw_color(renderer, COL_GRID);
    for (int x = 0; x <= grid.cols; ++x)
        SDL_RenderDrawLine(renderer, x * CELL_SIZE, 0, x * CELL_SIZE, WINDOW_HEIGHT);
    for (int y = 0; y <= grid.rows; ++y)
        SDL_RenderDrawLine(renderer, 0, y * CELL_SIZE, WINDOW_WIDTH, y * CELL_SIZE);

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
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION: {
                int mx, my;
                Uint32 buttons = SDL_GetMouseState(&mx, &my);
                int gx = mx / CELL_SIZE;
                int gy = my / CELL_SIZE;
                if (!grid.inBounds(gx, gy)) break;

                Cell& c = grid.at(gx, gy);

                if (setting_start && (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                    start = &c;
                    c.wall = false;
                }
                else if (setting_end && (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                    end = &c;
                    c.wall = false;
                }
                else if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    if (&c != start && &c != end) c.wall = true;
                }
                else if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                    c.wall = false;
                }
                break;
            }

                break;
            }
        }

        // A*-Schritte ausführen (animiert, mehrere Schritte pro Frame)
        if (a_star.has_value() && a_star->isRunning()) {
            for (int i = 0; i < STEPS_PER_FRAME && a_star->isRunning(); ++i) {
                a_star->step();
            }
            if (!a_star->isRunning() && a_star->isFound()) {
                path = a_star->reconstructPath();
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

