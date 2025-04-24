#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "iSolver.h"
#include "parsing_line.h"
#include <variant>

template <typename T>
// TODO: add requires-clause check for `parsing_table`
class Parser {
    // Common `Parser` for all types which have `parsing_table`
public:
    constexpr Parser(T* target) : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        Parser(target_->parsing_table.at(line.get_name())).Parse(line);
    }
private:
    T* target_;
};

// Excessive data in `line.args_` is ignored, no warning emitted
// Range check of `line.args_` is performed(access through `at`)

template <typename T>
    requires dash::Container<T>
// Only contigious containers
class Parser<T> {
public:
    constexpr Parser(T* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        Parser(target_.data() + line.get_index()).Parse(line);
    }
private:
    T* target_;
};

template <>
class Parser<int> {
public:
    constexpr Parser(int* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        *target_ = std::stoi(line.get_args().at(0));
    }
private:
    int* target_;
};

template <>
class Parser<std::size_t> {
public:
    constexpr Parser(std::size_t* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        *target_ = std::stoul(line.get_args().at(0));
    }
private:
    std::size_t* target_;
};

template <>
class Parser<double> {
public:
    constexpr Parser(double* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        *target_ = std::stod(line.get_args().at(0));
    }
private:
    double* target_;
};

template <>
class Parser<std::string> {
public:
    constexpr Parser(std::string* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line)
    {
        *target_ = line.get_args().at(0);
    }
private:
    std::string* target_;
};

#endif // PARSER_H
