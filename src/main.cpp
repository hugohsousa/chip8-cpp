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

    Chip8 chip8;
    if (!chip8.load_rom(argv[1]))
    {
        std::cerr << "Invalid Path to ROM\n";
        exit(1);
    }

    // Init window and sound
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error in initialising SDL" << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1048, 960, NULL);
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

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (renderer == nullptr)
    {
        std::cerr << "Could not create texture" << SDL_GetError() << "\n";
        SDL_Quit();
        exit(1);
    }

    while (true)
    {
        // Get the next event
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                // Break out of the loop on quit
                break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        // Show the renderer contents
        SDL_RenderPresent(renderer);
    }

    // Cleaning up
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}