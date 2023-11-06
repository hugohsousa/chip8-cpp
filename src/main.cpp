#include <iostream>

#include "chip8.h"

int main(int argc, char **argv) {
    if(argc < 1) {
        std::cerr << "Missing Path to ROM\n";
    }

    Chip8 chip8;
    if(!chip8.load_rom(argv[0])) {
        std::cerr << "Invalid Path to ROM\n";
        exit(1);
    } 
}