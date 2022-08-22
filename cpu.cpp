#include "chip8.hpp"
#include <algorithm>
#include <fstream>

const BYTE chip8_fontset[0x50] = {
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

void chip8::initialize()
{
    this->opcode = 0;
    std::fill(this->memory.begin(), this->memory.end(), 0);
    std::fill(this->V.begin(), this->V.end(), 0);
    this->I = 0;
    this->pc = 0;
    std::fill(this->gfx.begin(), this->gfx.end(), 0);
    std::fill(this->stack.begin(), this->stack.end(), 0);
    this->sp = 0;
    std::fill(this->key.begin(), this->key.end(), 0);

    // Load fontset
    for (int i = 0; i < 80; i++)
        this->memory[i] = chip8_fontset[i];

    this->delay_timer = 0;
    this->sound_timer = 0;

    this->drawFlag = 0;
}

void chip8::load_game(std::string const &name)
{
    std::ifstream in(name.c_str(), std::ios::in | std::ios::binary);
    for (int i = 0; !in.eof() && i < (4096 - 512); i++)
        this->memory[i + 512] = in.get();
    in.close();
}

void chip8::emulate_cycle()
{
    // Fetch
    this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc + 1];

    // Decode
    switch (this->opcode & 0xF000)
    {

    case 0xA000: // ANNN: Sets I to address NNN
        this->I = this->opcode & 0x0FFF;
        this->pc += 2;
        break;

    default:
        std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
    }

    // Update timers
    if (this->delay_timer > 0)
        this->delay_timer -= 1;

    if (this->sound_timer > 0)
    {
        if (this->sound_timer == 1)
            std::cout << "BEEP!" << std::endl;
        this->sound_timer -= 1;
    }
}

void chip8::set_keys()
{
}
