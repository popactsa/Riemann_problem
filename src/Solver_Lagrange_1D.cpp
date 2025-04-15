#include "Solver_Lagrange_1D.h"
#include "concepts.h"
#include <iostream>
#include <unordered_map>

// std::unordered_map w/ string_hash for heterogenious search
template <typename Key, typename Value>
using UnorderedMapStringHash =
    std::unordered_map<Key, Value, StringHash, std::equal_to<>>;

void Solver_Lagrange_1D::AssignEnumValue(std::string_view type,
                                         const std::vector<std::string>& args,
                                         void* ptr) noexcept
{
    if (type == "Tests") {
        using enum Tests;
        UnorderedMapStringHash<std::string_view, Tests> tbl{{"test1", qTest1},
                                                            {"test2", qTest2},
                                                            {"test3", qTest3},
                                                            {"test4", qTest4}};
        auto found = tbl.find(args[0]);
        if (found != tbl.end()) {
            *static_cast<Tests*>(ptr) = found->second;
        } else {
            std::cerr << "passed incorrect value : " << args[0] << std::endl;
            std::cerr << "possible values : " << std::endl;
            for (auto it : tbl) {
                std::cerr << '\t' << it.first << std::endl;
            }
            std::terminate();
            // throw std::invalid_argument("incorrect ic_preset value passed");
        }
    }
}
