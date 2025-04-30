#include "auxiliary_functions.h"
#include <cstdint>
#include <iostream>

enum class colors : std::uint8_t {
    red = 1 << 0,
    blue = 1 << 1,
    green = 1 << 2
};

int main()
{
    dash::Flag flag1{colors::red};
    flag1.reset();
    dash::Flag flag2{colors::red};
    std::cout << (flag1 & flag2).any() << std::endl;

    return 0;
}
