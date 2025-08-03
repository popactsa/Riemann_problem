#include "parser.hpp"
#include <cassert>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>

void Parser::load_parameters_from_yaml(
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
        auto found_key = par_tbl.find(key);
        if (found_key == par_tbl.end()) {
            throw std::runtime_error(std::format("Key `{}` not found", key));
        }
        if (pair.second.IsScalar()) {
            ParseScalar(found_key->second, pair.second.as<std::string_view>());
        } else if (pair.second.IsSequence()) {
            if (pair.second[0].IsScalar()) {
                ParseVector(found_key->second, pair.second);
            } else if (pair.second[0].IsMap()) {
                ParseCompoundVector(found_key->second, pair.second);
            } else {
                throw std::runtime_error("Unknown parsing type");
            }
        } else if (pair.second.IsMap()) {
            ParseCompound(found_key->second, pair.second);
        } else {
            // Ignore
        }
    }
}

void Parser::ParseScalar(
    const parser_t&  parser,
    std::string_view source) {
    // 0 is fictional
    std::invoke(parser, source, qNotAnArray);
}

void Parser::ParseCompound(
    const parser_t&   parser,
    const YAML::Node& source) {
    // 0 is fictional
    assert(source.isMap());
    std::invoke(parser, YAML::Dump(source), qNotAnArray);
}

void Parser::ParseVector(
    const parser_t&   parser,
    const YAML::Node& source_array) {
    assert(source_array.isSequence());
    assert(source_array[0].isScalar());
    std::size_t i{0};
    for (const auto& value : source_array) {
        std::invoke(parser, value.as<std::string_view>(), i++);
    }
}

void Parser::ParseCompoundVector(
    const parser_t&   parser,
    const YAML::Node& source_array) {
    assert(source_array.isSequence());
    assert(source_array[0].isMap());
    std::size_t i{0};
    for (const auto& pair : source_array) {
        std::invoke(parser, YAML::Dump(pair), i++);
    }
}

bool Parser::is_file_readable(const std::filesystem::path& path) {
    namespace fs = std::filesystem;
    using enum fs::perms;
    if (!fs::exists(path)) {
        return false;
    }
    auto file_perms = fs::status(path).permissions();
    std::cout << path.native() << std::endl;
    return fs::is_regular_file(path) && ((file_perms & owner_read) != none);
}

bool Parser::is_dir_writeable(const std::filesystem::path& path) {
    namespace fs = std::filesystem;
    using enum fs::perms;
    std::error_code ignored_ec;
    if (!fs::exists(path) && !fs::create_directory(path, ignored_ec)) {
        return false;
    }
    auto file_perms = fs::status(path).permissions();
    return fs::is_directory(path) && ((file_perms & owner_write) != none);
}

template<>
void Parser::unbound_parser<std::string>(
    std::string&     variable,
    std::string_view source,
    std::size_t) {
    variable.clear();
    variable.reserve(source.size());
    std::ranges::copy(source, std::back_inserter(variable));
}

template<>
void Parser::unbound_parser<char>(
    char&            variable,
    std::string_view source,
    std::size_t) {
    assert(source.size() == 1);
    variable = source.front();
}

template<>
void Parser::unbound_parser<bool>(
    bool&            variable,
    std::string_view source,
    std::size_t) {
    if (!source.compare("true") || !source.compare("1")) {
        variable = true;
    } else if (!source.compare("false") || !source.compare("0")) {
        variable = false;
    } else {
        throw std::runtime_error("Incorrect bool value");
    }
}
