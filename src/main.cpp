#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>
#include "io.hpp"

int main() {
    std::filesystem::path path =
        std::filesystem::current_path().parent_path().parent_path()
        / "tests/samples/array_types.yaml";
    YAML::Node config = YAML::LoadFile(path);
    for (const auto& pair : config) {
        auto str = YAML::Dump(pair.second);
        std::cout << str << std::endl;
    }
}
