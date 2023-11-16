#include <iostream>
#include <fstream>
#include <cstring>
#include <bitset>

#include "chip8.h"
#include <unistd.h>

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
            // 00E0 - Clears the screen.
            std::memset(framebuffer, false, sizeof(framebuffer));
            PC += 2;
            break;
        case 0x00EE:
            // 00EE - return from a subroutine
            PC = stack[SP];
            SP--;
            break;
        default:
            // 0nnn - Does nothing
            PC += 2;
            break;
        }
        break;
    case 1:
        // 1nnn - Jumps to address NNN.
        PC = opcode & 0x0FFF;
        break;
    case 2:
        // 2nnn - Calls subroutine at nnn.
        SP++;
        stack[SP] = PC;
        PC = opcode & 0x0FFF;
        break;
    case 3:
        // 3xnn - Skips the next instruction if Vx equals NN.
        if (V[parseOpCode(opcode, 8, 0x0F00)] == (opcode & 0x00FF))
        {
            PC += 2;
        }
        PC += 2;
        break;
    case 4:
        // 4xkk - Skip next instruction if Vx != kk.
        if (V[parseOpCode(opcode, 8, 0x0F00)] != (opcode & 0x00FF))
        {
            PC += 2;
        }
        PC += 2;
        break;
    case 5:
        // 5xy0 - Skip next instruction if Vx = Vy.
        if (V[parseOpCode(opcode, 8, 0x0F00)] == V[parseOpCode(opcode, 4, 0x00F0)])
        {
            PC += 2;
        }
        PC += 2;
        break;
    case 6:
        // 6xkk - Set Vx = kk.
        V[parseOpCode(opcode, 8, 0x0F00)] = (opcode & 0x00FF);
        PC += 2;
        break;
    case 7:
        // 7xkk - Set Vx = Vx + kk.
        V[parseOpCode(opcode, 8, 0x0F00)] += (opcode & 0x00FF);
        PC += 2;
        break;
    case 8:
        switch (opcode & 0x000F)
        {
        case 0:
            // 8xy0 - Set Vx = Vy.
            V[parseOpCode(opcode, 8, 0x0F00)] = V[parseOpCode(opcode, 4, 0x00F0)];
            break;
        case 1:
            // 8xy1 - Set Vx = Vx OR Vy.
            V[parseOpCode(opcode, 8, 0x0F00)] |= V[parseOpCode(opcode, 4, 0x00F0)];
            break;
        case 2:
            // 8xy2 - Set Vx = Vx AND Vy.
            V[parseOpCode(opcode, 8, 0x0F00)] &= V[parseOpCode(opcode, 4, 0x00F0)];
            break;
        case 3:
            // 8xy3 - Set Vx = Vx XOR Vy.
            V[parseOpCode(opcode, 8, 0x0F00)] ^= V[parseOpCode(opcode, 4, 0x00F0)];
            break;
        case 4:
            // 8xy4 - Set Vx = Vx + Vy, set VF = carry.
            if (V[parseOpCode(opcode, 8, 0x0F00)] + V[parseOpCode(opcode, 4, 0x00F0)] > 0xFF)
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[parseOpCode(opcode, 8, 0x0F00)] += V[parseOpCode(opcode, 4, 0x00F0)];
            V[parseOpCode(opcode, 8, 0x0F00)] = (uint8_t)V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 5:
            // 8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
            if (V[parseOpCode(opcode, 8, 0x0F00)] > V[parseOpCode(opcode, 4, 0x00F0)])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }

            V[parseOpCode(opcode, 8, 0x0F00)] = (uint8_t)V[parseOpCode(opcode, 8, 0x0F00)] - (uint8_t)V[parseOpCode(opcode, 4, 0x00F0)];
            break;
        case 6:
            // 8xy6 - Set Vx = Vx SHR 1.
            V[0xF] = V[parseOpCode(opcode, 8, 0x0F00)] & 0x1;
            V[parseOpCode(opcode, 8, 0x0F00)] >>= 1;
            V[parseOpCode(opcode, 8, 0x0F00)] = (uint8_t)V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 7:
            // 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
            if (V[parseOpCode(opcode, 8, 0x0F00)] < V[parseOpCode(opcode, 4, 0x00F0)])
            {
                V[0xF] = 1;
            }
            else
            {
                V[0xF] = 0;
            }
            V[parseOpCode(opcode, 8, 0x0F00)] = (uint8_t)V[parseOpCode(opcode, 4, 0x00F0)] - (uint8_t)V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 0xE:
            // 8xyE - Set Vx = Vx SHL 1.
            V[0xF] = V[parseOpCode(opcode, 8, 0x0F00)] >> 7;
            V[0xF] = (uint8_t)V[0xF];
            V[parseOpCode(opcode, 8, 0x0F00)] <<= 1;
            V[parseOpCode(opcode, 8, 0x0F00)] = (uint8_t)V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        default:
            std::cerr << "Invalid opcode- " << opcode;
            exit(1);
            break;
        }
        PC += 2;
        break;
    case 9:
        // 9xy0 - Skip next instruction if Vx != Vy.
        if (V[parseOpCode(opcode, 8, 0x0F00)] != V[parseOpCode(opcode, 4, 0x00F0)])
        {
            PC += 2;
        }
        PC += 2;
        break;
    case 0xA:
        // Annn - Set I = nnn.
        I = (opcode & 0x0FFF);
        PC += 2;
        break;
    case 0xB:
        // Bnnn - Jump to location nnn + V0.
        PC = (opcode & 0x0FFF) + V[0];
        break;
    case 0xC:
        // Cxkk - Set Vx = random byte AND kk.
        V[parseOpCode(opcode, 8, 0x0F00)] = (rand() % 256) & (opcode & 0x00FF);
        PC += 2;
        break;
    case 0xD:
    {
        // Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        uint8_t Vx = V[parseOpCode(opcode, 8, 0x0F00)];
        uint8_t Vy = V[parseOpCode(opcode, 4, 0x00F0)];
        uint8_t n = parseOpCode(opcode, 0, 0x00F);
        // Start at I ; Read n bytes
        for (uint32_t i = 0; i < n; i++)
        {
            uint8_t line = memory[I + i];
            for (uint32_t x = 0; x < 8; x++)
            {
                if ((Vy + i >= 32) || (Vx + x >= 64))
                    continue;
                if (line & (0x80 >> x) && framebuffer[((Vy + i) * 64) + Vx + x])
                    V[0xF] = 1;

                framebuffer[((Vy + i) * 64) + Vx + x] ^= line & (0x80 >> x);
            }
        }
        PC += 2;
        break;
    }
    case 0xE:
        // To do - Keyboard integration.
        if ((opcode & 0x00FF) == 0x009E)
        {
            // Ex9E - Skip next instruction if key with the value of Vx is pressed.
        }
        else if ((opcode & 0x00FF) == 0x00A1)
        {
            // ExA1 - Skip next instruction if key with the value of Vx is not pressed.
        }
        else
        {
            std::cerr << "Invalid opcode- " << opcode;
            exit(1);
        }
        break;
    case 0xF:
        switch (opcode & 0x00FF)
        {
        case 0x07:
            // Fx07 - Set Vx = delay timer value.
            V[parseOpCode(opcode, 8, 0x0F00)] = delayt_reg;
            break;
        case 0x0A:
            // Fx0A - Wait for a key press, store the value of the key in Vx.
            std::cout << "Waiting to be implemented\n";
            break;
        case 0x15:
            // Fx15 - Set delay timer = Vx.
            delayt_reg = V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 0x18:
            // Fx18 - Set sound timer = Vx.
            soundt_reg = V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 0x1E:
            // Fx1E - Set I = I + Vx.
            I += V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 0x29:
            // Fx29 - Set I = location of sprite for digit Vx.
            I = 5 * V[parseOpCode(opcode, 8, 0x0F00)];
            break;
        case 0x33:
        {
            // Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
            uint8_t n = V[parseOpCode(opcode, 8, 0x0F00)];
            memory[I] = n / 100;
            memory[I + 1] = (n / 10) % 10;
            memory[I + 2] = n % 10;
            break;
        }
        case 0x55:
            // Fx55 - Store registers V0 through Vx in memory starting at location I.
            for (int i = 0; i <= (parseOpCode(opcode, 8, 0x0F00)); i++)
            {
                memory[I++] = V[i];
            }
            break;
        case 0x65:
            // Fx65 - Read registers V0 through Vx from memory starting at location I.
            for (int i = 0; i <= ((parseOpCode(opcode, 8, 0x0F00)) >> 8); i++)
            {
                std::cout << (parseOpCode(opcode, 8, 0x0F00)) << "\n";
                V[i] = memory[I++];
            }
            break;
        default:
            std::cerr << "Invalid opcode- " << opcode;
            exit(1);
        }
        PC += 2;
        break;
    default:
        std::cerr << "Invalid opcode- " << opcode;
        exit(1);
    }
}

u_int8_t Chip8::parseOpCode(uint16_t opcode, uint8_t bits, uint16_t val)
{
    return ((opcode & val) >> bits);
}
