#ifndef IO_H
#define IO_H

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <format>
#include <iostream>
#include <regex>
#include <string_view>
#include <utility>

#include "concepts.h"
#include "iSolver.h"

constexpr inline std::string_view qPostfix{".scen"};
constexpr inline std::string_view qScenDir{"../scenarios"};
namespace fs = std::filesystem;

template <typename C>
    requires Container<C>
             && (!std::assignable_from<typename C::value_type, std::string>)
void SplitString(std::string init, const char sep, C &result) noexcept
// @does : Splits a string into a container of strings
// If std::back_inserter constructed from `result` doesn't compile : pushes to
// the back of `result`, else : if (size(result) < size(init)) excessive
// elements are not inserted
{
    std::regex sep_rx;
    if (std::isspace(sep)) {
        sep_rx = "\\s+";
    } else {
        sep_rx = std::format("{}+", sep);
    }
    if constexpr (HasBackInserterContainer<C>) {
        std::copy(
            std::sregex_token_iterator(init.cbegin(), init.cend(), sep_rx, -1),
            std::sregex_token_iterator(), std::back_inserter(result));
    } else {
        std::copy(
            std::sregex_token_iterator(init.cbegin(), init.cend(), sep_rx, -1),
            std::sregex_token_iterator(), result.begin());
    }
}

bool IsReadable(const fs::path &path) noexcept;

std::size_t PrintFilenames(const fs::path &dir,
                           std::string_view postfix) noexcept;
// @does : Prints all readable filenames in `dir` with `postfix`
// @returns : count of found files
// if __cpp_lib_format defined AND horizontal size of a screen is big
// enough calls GetScenSolverTypeName for every <file>`postfix` and outputs a
// found solver's name, else outputs just a name of a file

std::pair<Solvers, fs::path>
GetPathToScenInDir(const fs::path &dir,
                   std::size_t min,
                   std::size_t max,
                   std::string_view postfix) noexcept;
// @does : Asks for a number of a file in given range [min, max]
// @returns : A path to a chosen file with `pos` in `dir`

#endif // IO_H
