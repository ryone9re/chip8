#include "chip8.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

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

    case 0x0000: {
        switch (this->opcode & 0x000F)
        {
        case 0x0000: {
            this->disp_clear();
            this->pc += 2;
            break;
        }
        case 0x000E: {
            // TODO Returns from subroutine
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
        }
        break;
    }

    case 0x1000: {
        this->pc = this->opcode & 0x0FFF;
        break;
    }

    case 0x2000: {
        this->stack[this->sp++] = this->pc;
        this->pc = (this->opcode & 0x0FFF);
        break;
    }

    case 0x3000: {
        int x = (this->opcode & 0x0F00) >> 8;
        if (this->V[x] == (this->opcode & 0x00FF))
            this->pc += 2;
        this->pc += 2;
        break;
    }

    case 0x4000: {
        int x = (this->opcode & 0x0F00) >> 8;
        if (this->V[x] != (this->opcode & 0x00FF))
            this->pc += 2;
        this->pc += 2;
        break;
    }

    case 0x5000: {
        int x = (this->opcode & 0x0F00) >> 8;
        int y = (this->opcode & 0x00F0) >> 4;
        if (this->V[x] == this->V[y])
            this->pc += 2;
        this->pc += 2;
        break;
    }

    case 0x6000: {
        int x = (this->opcode & 0x0F00) >> 8;
        this->V[x] = (this->opcode & 0x00FF);
        this->pc += 2;
        break;
    }

    case 0x7000: {
        int x = (this->opcode & 0x0F00) >> 8;
        this->V[x] += (this->opcode & 0x00FF);
        this->pc += 2;
        break;
    }

    case 0x8000: {
        // TODO
        break;
    }

    case 0x9000: {
        int x = (this->opcode & 0x0F00) >> 8;
        int y = (this->opcode & 0x00F0) >> 4;
        if (this->V[x] != this->V[y])
            this->pc += 2;
        this->pc += 2;
        break;
    }

    case 0xA000: {
        this->I = this->opcode & 0x0FFF;
        this->pc += 2;
        break;
    }

    case 0xB000: {
        this->pc = this->V[0] + (this->opcode & 0x0FFF);
        break;
    }

    case 0xC000: {
        int x = (this->opcode & 0x0F00) >> 8;
        this->V[x] = this->rand() & (this->opcode & 0x00FF);
        this->pc += 2;
        break;
    }

    case 0xD000: {
        // TODO
        break;
    }

    case 0xE000: {
        // TODO
        switch (this->opcode & 0x00FF)
        {
        case 0x009E: {
            break;
        }
        case 0x00A1: {
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
        }
        break;
    }

    case 0xF000: {
        // TODO
        switch (this->opcode & 0x00F0)
        {

        case 0x0000: {
            switch (this->opcode & 0x000F)
            {
            case 0x0007: {
                break;
            }
            case 0x000A: {
                break;
            }
            default:
                std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
            }
            break;
        }

        case 0x0010: {
            switch (this->opcode & 0x000F)
            {
            case 0x0005: {
                break;
            }
            case 0x0008: {
                break;
            }
            case 0x000E: {
                break;
            }
            default:
                std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
            }
        }

        case 0x0020: {
            break;
        }

        case 0x0030: {
            break;
        }

        case 0x0050: {
            break;
        }

        case 0x0060: {
            break;
        }

        default:
            std::cout << "Unknown opcode: 0x%X" << this->opcode << std::endl;
        }
        break;
    }

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

void chip8::disp_clear()
{
    std::fill(this->gfx.begin(), this->gfx.end(), 0);
}

BYTE chip8::rand()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    return dist(rng);
}
