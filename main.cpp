#include "chip8.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        exit_with_errmsg();

    chip8 machine;

    machine.initialize();
    machine.load_game(argv[1]);

    while (true)
    {
        machine.emulate_cycle();

        if (machine.draw_flag)
            ; // draw

        machine.set_keys();
    }

    return 0;
}
