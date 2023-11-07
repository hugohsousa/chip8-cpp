#include <iostream>
#include <fstream>

#include "chip8.h"

Chip8::Chip8()
{
    // Init PC
    PC = 0x200;

    // Init other registers
    I = 0;
    SP = 0;
    delayt_reg = 0;
    soundt_reg = 0;
}

bool Chip8::load_rom(std::string path_rom)
{
    // Open rom
    std::ifstream rom(path_rom, std::ios::binary);
    if (!rom)
    {
        std::cerr << " Failed to open\n";
        return false;
    }

    char c;
    for (int i = 0x200; rom.get(c); i++)
    {
        if (i > 0xFFF)
        {
            std::cerr << " The ROM is to big\n";
            return false;
        }
        memory[i] = u_int8_t(c);
    }

    return true;
}