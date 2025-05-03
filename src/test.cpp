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
    using Object = dash::VariantWrapper<Object_int, Object_double>;
    dash::TinyMap map{
        std::pair{"int", Object{dash::qUniqueID<Object_int>, 5}},
        std::pair{"double", Object{dash::qUniqueID<Object_double>, 5.0}},
    };
    return 0;
}
