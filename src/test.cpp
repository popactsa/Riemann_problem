#include "auxiliary_functions.h"
#include "parsing_line.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> data{"Wall 1.0 2.0 3.0"};
    ScenParsingLine parsed;
    for (auto& it : data) {
        parsed.Load(it);
    }
    return 0;
}
