#include "chip8.hpp"

chip8 machine;

int main(int argc, char **argv)
{
    if (argc != 2)
        exit_with_errmsg();

    machine.initialize();
    machine.load_game(argv[1]);

    while (true)
    {
        machine.emulate_cycle();

        if (machine.drawFlag)
            ; // draw

        machine.set_keys();
    }

    return 0;
}
