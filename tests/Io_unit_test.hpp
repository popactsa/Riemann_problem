#ifndef IO_UNIT_TEST_HPP
#define IO_UNIT_TEST_HPP

#include <gtest/gtest.h>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include "auxiliary_functions.hpp"
#include "parser.hpp"

inline const std::filesystem::path test_samples_dir =
    dash::cmake_dir() / "tests" / "samples";

// inline const std::filesystem::path test_samples_dir =
//     std::filesystem::current_path().parent_path().parent_path()
//     / "tests"
//     / "samples";

struct ConfigSample {
    ConfigSample(std::string_view filename) {
        sample_path  = test_samples_dir;
        sample_path /= filename;
    }

    const std::filesystem::path& get_sample_path() const { return sample_path; }

    std::filesystem::path sample_path;
};

enum class FooEnum : int {
    qRed   = 0,
    qGreen = 123,
    qBlue  = 234
};

template<>
inline void Parser::unbound_parser<FooEnum>(
    FooEnum&         variable,
    std::string_view source,
    std::size_t) {
    using enum FooEnum;
    static std::unordered_map<std::string_view, FooEnum> tbl{
        {"Red",   qRed  },
        {"Green", qGreen},
        {"Blue",  qBlue },
    };
    auto found = tbl.find(source);
    if (found == tbl.end()) {
        throw std::runtime_error("No such enum value");
    }
    variable = found->second;
}

struct ConfigSample_SimpleTypes: public ConfigSample {
    using ConfigSample::ConfigSample;

    bool correct_read() const {
        bool status  = true;
        status      &= foo_int == 5;
        status      &= foo_double == 7.7;
        status      &= foo_char == 'a';
        status      &= foo_bool == true;
        status      &= foo_enum == FooEnum::qGreen;
        status      &= foo_string.compare("hello, world") == 0;
        return status;
    }

    Parser::parsing_table_t get_parsing_table() {
        return Parser::parsing_table_t{
            {"foo_int",    Parser::parser(foo_int)   },
            {"foo_double", Parser::parser(foo_double)},
            {"foo_char",   Parser::parser(foo_char)  },
            {"foo_bool",   Parser::parser(foo_bool)  },
            {"foo_enum",   Parser::parser(foo_enum)  },
            {"foo_string", Parser::parser(foo_string)},
        };
    }

    int         foo_int;
    double      foo_double;
    char        foo_char;
    std::string foo_string;
    FooEnum     foo_enum;
    bool        foo_bool;
};

struct ConfigSample_ArrayTypes: public ConfigSample {
    using ConfigSample::ConfigSample;

    bool correct_read() const {
        bool status  = true;
        status      &= names[0].compare("Dasha") == 0;
        status      &= names[1].compare("Jeka") == 0;
        status      &= names[2].compare("Theodor") == 0;
        return status;
    }

    Parser::parsing_table_t get_parsing_table() {
        return Parser::parsing_table_t{
            {"names", Parser::parser(names)},
        };
    }

    std::array<std::string, 3> names;
};

#endif    // IO_UNIT_TEST_HPP
