#include <iostream>
#include <SDL.h>
#include <unistd.h>

#include "chip8.h"

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        std::cerr << "Missing Path to ROM\n";
    }

    // Start chip8 and load ROM
    Chip8 chip8;
    if (!chip8.load_rom(argv[1]))
    {
        std::cerr << "Error while loading ROM\nPlease provide a valid ROM!\n";
        exit(1);
    }

    // Init window
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error in initialising SDL" << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    int w = 960;
    int h = 480;
    SDL_Window *window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, w, h, NULL);
    if (window == nullptr)
    {
        std::cerr << "Could not create a window" << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Could not create renderer" << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    // Main loop
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                // Break out of the loop on quit
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        // Render the pixels
        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 0);
        for (int i = 0; i <= 64; i++)
        {
            for (int j = 0; j <= 32; j++)
            {
                if (chip8.getPixelData(j * 64 + i))
                {
                    SDL_Rect pixel = {w / 64 * i, h / 32 * j, w / 64, h / 32};
                    SDL_RenderDrawRect(renderer, &pixel);
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        // Show the renderer contents
        SDL_RenderPresent(renderer);
        chip8.Tick();
    }

    // Cleaning up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}