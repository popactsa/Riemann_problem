#ifndef IO_HPP
#define IO_HPP
#include <filesystem>
#include <functional>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include "parsers.hpp"

class Io {
public:
    using parser_t        = std::function<void(std::string_view)>;
    using parsing_table_t = std::unordered_map<std::string_view, parser_t>;

    Io():
        Io(std::cin,
           std::cout) {}

    Io(std::istream& in,
       std::ostream& out);
    void load_parameters_from_yaml(
        const std::filesystem::path& path,
        const parsing_table_t&       par_tbl);

private:
    [[maybe_unused]]
    std::istream& in_;
    [[maybe_unused]]
    std::ostream& out_;
    bool is_file_readable(const std::filesystem::path& path);
};

#endif    // IO_HPP
