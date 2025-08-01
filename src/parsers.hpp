#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <charconv>
#include <concepts>
#include <string_view>

template<typename T>
void unbound_parser(
    T&               variable,
    std::string_view source) = delete;

template<typename T>
void unbound_parser(
    T&               variable,
    std::string_view source)
    requires std::floating_point<T> || std::integral<T>
{
    std::from_chars(source.begin(), source.end(), variable);
}

template<>
void unbound_parser<std::string>(
    std::string&     variable,
    std::string_view source);

template<>
void unbound_parser<char>(
    char&            variable,
    std::string_view source);

template<typename T>
auto parser(T& variable) {
    return [&](std::string_view source) {
        return unbound_parser(variable, source);
    };
}

#endif    // PARSERS_HPP
