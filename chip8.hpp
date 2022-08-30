#pragma once

#include <array>
#include <iostream>

using BYTE = unsigned char;
using WORD = unsigned short;

class chip8
{
    WORD opcode; // Current opcode
    /* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
     * 0x200-0xFFF - Program ROM and work RAM */
    std::array<BYTE, 4096> memory;
    std::array<BYTE, 16> V;     // Registers. V0 to VF
    WORD I;                     // Index register
    WORD pc;                    // Program counter
    BYTE delay_timer;           // Delay timer
    BYTE sound_timer;           // Sound timer
    std::array<WORD, 16> stack; // Stack
    WORD sp;                    // Stack pointer
    std::array<BYTE, 16> key;   // Keyboard Matrix

  public:
    std::array<std::array<BYTE, 64>, 32> gfx; // Graphics

    bool draw_flag;

    void initialize();
    void load_game(std::string const &name);
    void emulate_cycle();
    void set_keys(BYTE key, BYTE status);

    void push_stack(WORD ret);
    WORD pop_stack();

    BYTE rand();
};

/* Prototypes */
/* error.c */
void exit_with_errmsg();
