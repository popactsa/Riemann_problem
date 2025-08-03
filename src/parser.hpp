#ifndef IO_HPP
#define IO_HPP
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <charconv>
#include <concepts>
#include <filesystem>
#include <functional>
#include <string_view>
#include <unordered_map>
#include "concepts.hpp"

class Parser {
public:
    using parser_t        = std::function<void(std::string_view, std::size_t)>;
    using parsing_table_t = std::unordered_map<std::string_view, parser_t>;

    inline constexpr static std::size_t qNotAnArray = 0;
    Parser()                                        = default;

    template<typename T>
    static auto parser(T& variable);

    template<typename T>
        requires std::is_enum_v<T>
    static auto parser(
        const std::unordered_map<
            std::string_view,
            T>& table,
        T&      variable);

    static void load_parameters_from_yaml(
        const std::filesystem::path& path,
        const parsing_table_t&       par_tbl);

private:
    static bool is_file_readable(const std::filesystem::path& path);
    static bool is_dir_writeable(const std::filesystem::path& path);

    template<typename T>
    static void unbound_parser(
        T&               variable,
        std::string_view source,
        std::size_t      i) = delete;

    template<typename T>
    static void unbound_parser(
        T&               variable,
        std::string_view source,
        std::size_t)
        requires std::floating_point<T> || std::integral<T>;

    template<typename T>
    static void unbound_parser(
        T&               variable,
        std::string_view source,
        std::size_t      i)
        requires dash::concepts::Container<T>;

    template<>
    void unbound_parser<std::string>(
        std::string&     variable,
        std::string_view source,
        std::size_t);

    template<>
    void unbound_parser<char>(
        char&            variable,
        std::string_view source,
        std::size_t);

    template<>
    void unbound_parser<bool>(
        bool&            variable,
        std::string_view source,
        std::size_t);

    static void ParseScalar(
        const parser_t&  parser,
        std::string_view source);
    static void ParseCompound(
        const parser_t&   parser,
        const YAML::Node& source);
    static void ParseVector(
        const parser_t&   parser,
        const YAML::Node& source_array);
    static void ParseCompoundVector(
        const parser_t&   parser,
        const YAML::Node& source_array);
};

template<typename T>
auto Parser::parser(T& variable) {
    return [&](std::string_view source, std::size_t i) {
        return unbound_parser(variable, source, i);
    };
}

template<typename T>
    requires std::is_enum_v<T>
auto Parser::parser(
    const std::unordered_map<
        std::string_view,
        T>& table,
    T&      variable) {
    return [&](std::string_view source, std::size_t) {
        auto found = table.find(source);
        if (found == table.end()) {
            throw std::runtime_error("No such enum value");
        }
        variable = found->second;
    };
}

template<typename T>
void Parser::unbound_parser(
    T&               variable,
    std::string_view source,
    std::size_t)
    requires std::floating_point<T> || std::integral<T>
{
    std::from_chars(source.begin(), source.end(), variable);
}

template<typename T>
void Parser::unbound_parser(
    T&               variable,
    std::string_view source,
    std::size_t      i)
    requires dash::concepts::Container<T>
{
    unbound_parser(variable[i], source, qNotAnArray);
}

#endif    // IO_HPP
