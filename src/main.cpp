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
    if (!chip8.load_rom(argv[0]))
    {
        std::cerr << "Invalid Path to ROM\n";
        exit(1);
    }

    // Init window and sound
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error in initialising SDL" << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1048, 960, NULL);
    if (window == nullptr)
    {
        std::cerr << "Error in initialising the Window" << SDL_GetError() << std::endl;
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
    }

    // Cleaning up
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}