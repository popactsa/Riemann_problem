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

constexpr std::vector<std::string> SplitString(std::string_view init,
                                               const char sep) noexcept
// @does: Splits a string_view into a vector of strings
// @returns: A vector of split partitions
{
    std::vector<std::string> result;
    for (auto it = init.cbegin(), prev = it; it != init.cend();) {
        it = std::find(prev, init.cend(), sep);
        if (prev != it) {
            result.emplace_back(prev, it);
        }
        prev = it + 1;
    }
    return result;
}

constexpr std::vector<std::string> SplitString(const std::string& init,
                                               const char sep) noexcept
{
    return SplitString(static_cast<std::string_view>(init), sep);
}

bool IsReadable(const fs::path& path) noexcept;

std::size_t PrintFilenames(const fs::path& dir,
                           std::string_view postfix) noexcept;
// @does : Prints all readable filenames in `dir` with `postfix`
// @returns : count of found files
// if __cpp_lib_format defined and horizontal size of a screen is big
// enough calls GetScenSolverTypeName for every <file>`postfix` and outputs a
// found solver's name, else outputs just a name of a file

std::pair<Solvers, fs::path> ChooseFileInDir(const fs::path& dir,
                                             std::size_t min,
                                             std::size_t max,
                                             std::string_view postfix) noexcept;
// @does : Calls `ChooseFileNumberInRange` for a number of a file in given range
// [min, max]
// @returns : A path to a chosen file with name containing `postfix` and `pos`
// in `dir`

#endif // IO_H
