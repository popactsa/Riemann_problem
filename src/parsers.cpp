#include "parsers.hpp"
#include <algorithm>
#include <cassert>
#include <string>

////////////////// Parser specializations //////////////////
template<>
void unbound_parser<std::string>(
    std::string&     variable,
    std::string_view source) {
    variable.clear();
    variable.reserve(source.size());
    std::ranges::copy(source, std::back_inserter(variable));
}

template<>
void unbound_parser<char>(
    char&            variable,
    std::string_view source) {
    assert(source.size() == 1);
    variable = source.front();
}
