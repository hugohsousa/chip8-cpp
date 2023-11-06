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

bool Chip8::load_rom(std::string)
{

    return false;
}
