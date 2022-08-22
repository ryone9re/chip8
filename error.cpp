#include "chip8.hpp"
#include <cstdlib>
#include <iostream>

void exit_with_errmsg()
{
    std::cout << "Error occurred" << std::endl;
    std::exit(1);
}
