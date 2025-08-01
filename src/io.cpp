#include "io.hpp"
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <cassert>
#include <format>
#include <functional>

Io::Io(
    std::istream& in,
    std::ostream& out):
    in_(in),
    out_(out) {}

void Io::load_parameters_from_yaml(
    const std::filesystem::path& path,
    const parsing_table_t&       par_tbl) {
    if (!std::string_view(path.c_str()).ends_with(".yaml")) {
        throw std::runtime_error("Not a .yaml file");
    }
    if (!is_file_readable(path)) {
        throw std::runtime_error("Can't open parameters file");
    }
    YAML::Node config = YAML::LoadFile(path);
    for (const auto& pair : config) {
        auto key       = pair.first.as<std::string_view>();
        auto value     = pair.second.as<std::string_view>();
        auto found_key = par_tbl.find(key);
        if (found_key == par_tbl.end()) {
            throw std::runtime_error(std::format("Key `{}` not found", key));
        }
        std::invoke(found_key->second, value);
    }
}

bool Io::is_file_readable(const std::filesystem::path& path) {
    namespace fs = std::filesystem;
    using enum fs::perms;
    if (!fs::exists(path)) {
        return false;
    }
    auto file_perms = fs::status(path).permissions();
    std::cout << path.native() << std::endl;
    return fs::is_regular_file(path) && ((file_perms & owner_read) != none);
}
