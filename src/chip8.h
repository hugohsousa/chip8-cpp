#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <string>

class Chip8
{
private:
    // Memory
    uint8_t memory[4096];

    // Stack
    uint16_t stack[16];

    // 16 8 bit registers
    uint8_t V[16];

    uint16_t I;         // Store memory addresses
    uint16_t PC;        // Program counter
    uint8_t SP;         // Stack pointer
    uint8_t delayt_reg; // Delay timer register
    uint8_t soundt_reg; // Sound timer register

    // Display framebuffer
    bool framebuffer[64 * 32];

    // Keypad
    bool keypad[16];

public:
    Chip8();                    // Constructor
    bool load_rom(std::string); // Load rom to memory
};

#endif // CHIP8_H