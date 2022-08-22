#pragma once

#include <array>
#include <iostream>

typedef unsigned char BYTE;
typedef unsigned short WORD;

class chip8
{
    WORD opcode; // Current opcode
    /* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
     * 0x200-0xFFF - Program ROM and work RAM */
    std::array<BYTE, 4096> memory;
    std::array<BYTE, 16> V;        // Registers. V0 to VF
    WORD I;                        // Index register
    WORD pc;                       // Program counter
    std::array<BYTE, 64 * 32> gfx; // Graphics
    BYTE delay_timer;              // Delay timer
    BYTE sound_timer;              // Sound timer
    std::array<WORD, 16> stack;    // Stack
    WORD sp;                       // Stack pointer
    std::array<BYTE, 16> key;      // Keyboard Matrix

  public:
    bool drawFlag;

    void initialize();
    void load_game(std::string const &name);
    void emulate_cycle();
    void set_keys();
};

/* Prototypes */
/* error.c */
void exit_with_errmsg();
