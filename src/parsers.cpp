#include "parsers.hpp"
#include <algorithm>
#include <cassert>
#include <string>

////////////////// Parser specializations //////////////////
template<>
void unbound_parser<std::string>(
    std::string&     variable,
    std::string_view source,
    std::size_t) {
    variable.clear();
    variable.reserve(source.size());
    std::ranges::copy(source, std::back_inserter(variable));
}

template<>
void unbound_parser<char>(
    char&            variable,
    std::string_view source,
    std::size_t) {
    assert(source.size() == 1);
    variable = source.front();
}

template<>
void unbound_parser<bool>(
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
