#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <yaml-cpp/yaml.h>
#include <charconv>
#include <concepts>
#include <string_view>
#include "concepts.hpp"

// This constant is used when subscription index is fictional
inline constexpr std::size_t qNotAnArray = 0;

template<typename T>
void unbound_parser(
    T&               variable,
    std::string_view source,
    std::size_t      i) = delete;

template<typename T>
void unbound_parser(
    T&               variable,
    std::string_view source,
    std::size_t)
    requires std::floating_point<T> || std::integral<T>
{
    std::from_chars(source.begin(), source.end(), variable);
}

template<typename T>
void unbound_parser(
    T&               variable,
    std::string_view source,
    std::size_t      i)
    requires dash::concepts::Container<T>
{
    unbound_parser(variable[i], source, qNotAnArray);
}

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

template<typename T>
auto parser(T& variable) {
    return [&](std::string_view source, std::size_t i) {
        return unbound_parser(variable, source, i);
    };
}

#endif    // PARSERS_HPP
