#include <iostream>
#include <fstream>
#include <cstring>
#include <bitset>

#include "chip8.h"

Chip8::Chip8()
{
    // Declare font
    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    // Init memory, framebuffers, stack and registers
    std::memset(memory, 0, sizeof(memory));
    std::memset(framebuffer, false, sizeof(framebuffer));
    std::memset(stack, 0, sizeof(stack));
    std::memset(V, 0, sizeof(V));
    I = 0;
    SP = -1;
    delayt_reg = 0;
    soundt_reg = 0;

    // Init PC
    PC = 0x200;

    // Copy font to memory
    std::memmove(&memory[0], font, sizeof(font));
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

bool Chip8::getPixelData(int pixel)
{
    return framebuffer[pixel];
}

void Chip8::Tick()
{
    // Debug
    // Shift first byte to left and copy second
    uint16_t opcode = (memory[PC] << 8) | (memory[PC + 1]);
    printf("%.4X %.4X %.2X\n", PC, opcode, SP);

    switch (opcode >> 12)
    {
    case 0:
        switch (opcode)
        {
        case 0x00E0:
            std::memset(framebuffer, false, sizeof(framebuffer));
            PC += 2;
            break;
        case 0x00EE:
            PC = stack[SP];
            SP--;
            break;
        default:
            std::cerr << "Invalid opcode- " << opcode;
            break;
        }
        break;
    case 1:
        PC = opcode & 0x0FFF;
        break;
    default:
        std::cerr << "Invalid opcode- " << opcode;
        exit(1);
        break;
    }

}
