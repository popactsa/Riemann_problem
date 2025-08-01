#ifndef IO_UNIT_TEST_HPP
#define IO_UNIT_TEST_HPP

#include <gtest/gtest.h>
#include <filesystem>
#include <functional>
#include "io.hpp"
inline const std::filesystem::path test_samples_dir(
    std::filesystem::current_path().parent_path().parent_path()
    / "tests/samples");

struct ConfigSample_SimpleTypes {
    ConfigSample_SimpleTypes(std::string_view filename) {
        sample_path  = test_samples_dir;
        sample_path /= filename;
    }

    const std::filesystem::path& get_sample_path() const { return sample_path; }

    bool correct_read() const {
        bool status = true;
        std::cout
            << foo_int
            << ' '
            << foo_double
            << ' '
            << foo_char
            << ' '
            << foo_string
            << std::endl;
        status &= foo_int == 5;
        status &= foo_double == 7.7;
        status &= foo_char == 'a';
        status &= foo_string.compare("hello, world") == 0;
        return status;
    }

    Io::parsing_table_t get_parsing_table() {
        return Io::parsing_table_t{
            {   "foo_int",    parser(foo_int)},
            {"foo_double", parser(foo_double)},
            {  "foo_char",   parser(foo_char)},
            {"foo_string", parser(foo_string)},
        };
    }

    int         foo_int;
    double      foo_double;
    char        foo_char;
    std::string foo_string;

    std::filesystem::path sample_path;
};

#endif    // IO_UNIT_TEST_HPP
