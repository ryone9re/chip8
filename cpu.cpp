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
    opcode = 0;
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(V.begin(), V.end(), 0);
    I = 0;
    pc = 0;
    std::fill(gfx.begin(), gfx.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);
    sp = 0;
    std::fill(key.begin(), key.end(), 0);

    // Load fontset
    for (int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];

    delay_timer = 0;
    sound_timer = 0;

    draw_flag = 0;
}

void chip8::load_game(std::string const &name)
{
    std::ifstream in(name.c_str(), std::ios::in | std::ios::binary);
    for (int i = 0; !in.eof() && i < (4096 - 512); i++)
        memory[i + 512] = in.get();
    in.close();
}

void chip8::emulate_cycle()
{
    // Fetch
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode
    switch (opcode & 0xF000)
    {

    case 0x0000: {
        switch (opcode & 0x000F)
        {
        case 0x0000: {
            disp_clear();
            pc += 2;
            break;
        }
        case 0x000E: {
            sp -= 1;
            pc = stack[sp];
            pc += 2;
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
        }
        break;
    }

    case 0x1000: {
        pc = opcode & 0x0FFF;
        break;
    }

    case 0x2000: {
        stack[sp++] = pc;
        pc = (opcode & 0x0FFF);
        break;
    }

    case 0x3000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        if (V[x] == (opcode & 0x00FF))
            pc += 2;
        pc += 2;
        break;
    }

    case 0x4000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        if (V[x] != (opcode & 0x00FF))
            pc += 2;
        pc += 2;
        break;
    }

    case 0x5000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        BYTE y = (opcode & 0x00F0) >> 4;
        if (V[x] == V[y])
            pc += 2;
        pc += 2;
        break;
    }

    case 0x6000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        V[x] = (opcode & 0x00FF);
        pc += 2;
        break;
    }

    case 0x7000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        V[x] += (opcode & 0x00FF);
        pc += 2;
        break;
    }

    case 0x8000: {
        switch (opcode & 0x000F)
        {
        case 0x0000: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            V[x] = V[y];
            pc += 2;
            break;
        }
        case 0x0001: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            V[x] |= V[y];
            pc += 2;
            break;
        }
        case 0x0002: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            V[x] &= V[y];
            pc += 2;
            break;
        }
        case 0x0003: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            V[x] ^= V[y];
            pc += 2;
            break;
        }
        case 0x0004: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            if (V[y] > (0xFF - V[x]))
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[x] += V[y];
            pc += 2;
            break;
        }
        case 0x0005: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            if (V[y] > V[x])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[x] -= V[y];
            pc += 2;
            break;
        }
        case 0x0006: {
            BYTE x = (opcode & 0x0F00) >> 8;
            V[x] >>= 1;
            pc += 2;
            break;
        }
        case 0x0007: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE y = (opcode & 0x00F0) >> 4;
            if (V[x] > V[y])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[x] = V[y] - V[x];
            pc += 2;
            break;
        }
        case 0x000E: {
            BYTE x = (opcode & 0x0F00) >> 8;
            V[x] <<= 1;
            pc += 2;
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
        }
        break;
    }

    case 0x9000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        BYTE y = (opcode & 0x00F0) >> 4;
        if (V[x] != V[y])
            pc += 2;
        pc += 2;
        break;
    }

    case 0xA000: {
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    }

    case 0xB000: {
        pc = V[0] + (opcode & 0x0FFF);
        break;
    }

    case 0xC000: {
        BYTE x = (opcode & 0x0F00) >> 8;
        V[x] = rand() & (opcode & 0x00FF);
        pc += 2;
        break;
    }

    case 0xD000: {
        // TODO
        break;
    }

    case 0xE000: {
        switch (opcode & 0x00FF)
        {
        case 0x009E: {
            BYTE x = (opcode & 0x0F00) >> 8;
            if (get_keys() == V[x])
                pc += 2;
            pc += 2;
            break;
        }
        case 0x00A1: {
            BYTE x = (opcode & 0x0F00) >> 8;
            if (get_keys() != V[x])
                pc += 2;
            pc += 2;
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
        }
        break;
    }

    case 0xF000: {
        switch (opcode & 0x00F0)
        {

        case 0x0000: {
            switch (opcode & 0x000F)
            {
            case 0x0007: {
                BYTE x = (opcode & 0x0F00) >> 8;
                V[x] = delay_timer;
                pc += 2;
                break;
            }
            case 0x000A: {
                BYTE x = (opcode & 0x0F00) >> 8;
                V[x] = get_keys();
                pc += 2;
                break;
            }
            default:
                std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
            }
            break;
        }

        case 0x0010: {
            switch (opcode & 0x000F)
            {
            case 0x0005: {
                BYTE x = (opcode & 0x0F00) >> 8;
                delay_timer = V[x];
                pc += 2;
                break;
            }
            case 0x0008: {
                BYTE x = (opcode & 0x0F00) >> 8;
                sound_timer = V[x];
                pc += 2;
                break;
            }
            case 0x000E: {
                BYTE x = (opcode & 0x0F00) >> 8;
                I += V[x];
                pc += 2;
                break;
            }
            default:
                std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
            }
        }

        case 0x0020: {
            BYTE x = (opcode & 0x0F00) >> 8;
            I = V[x] * 0x5;
            pc += 2;
            break;
        }

        case 0x0030: {
            BYTE x = (opcode & 0x0F00) >> 8;
            BYTE top = V[x] / 100;
            BYTE mid = (V[x] % 100) / 10;
            BYTE bot = (V[x] % 100) % 10;
            memory[I] = top;
            memory[I + 1] = mid;
            memory[I + 2] = bot;
            pc += 2;
            break;
        }

        case 0x0050: {
            BYTE x = (opcode & 0x0F00) >> 8;
            for (int i = 0; i <= x; i++)
                V[i] = memory[i + I];
            pc += 2;
            break;
        }

        case 0x0060: {
            BYTE x = (opcode & 0x0F00) >> 8;
            for (int i = 0; i <= x; i++)
                memory[i + I] = V[i];
            pc += 2;
            break;
        }

        default:
            std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
        }
        break;
    }

    default:
        std::cout << "Unknown opcode: 0x%X" << opcode << std::endl;
    }

    // Update timers
    if (delay_timer > 0)
        delay_timer -= 1;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            std::cout << "BEEP!" << std::endl;
        sound_timer -= 1;
    }
}

void chip8::set_keys()
{
}

BYTE chip8::get_keys()
{
    return 1;
}

void chip8::disp_clear()
{
    std::fill(gfx.begin(), gfx.end(), 0);
}

BYTE chip8::rand()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    return dist(rng);
}
