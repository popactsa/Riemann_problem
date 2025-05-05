#include "auxiliary_functions.h"
#include "concepts.h"
#include "parsing_line.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

struct Object_int {
    int a;
};

struct Object_double {
    double d;
};

int main()
{
    // using Object = dash::VariantWrapper<Object_int, Object_double>;
    dash::TinyMap<std::string, int, 2> map;
    map.construct_at(0, "int", 5);
    std::cout << map.at("int");
    return 0;
}
