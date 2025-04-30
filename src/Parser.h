#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"
#include <charconv>
#include <type_traits>
#include <variant>

template <typename T>
class Parser {
    // Common `Parser` for all types which have `parsing_table`
    // TODO : Add constrains
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qNamedType};
    constexpr Parser(T* target) : target_(target) {}
    void Parse(const ScenParsingLine::NamedArg& named_arg)
    {
        auto& found_ptr = target_->parsing_table.at(named_arg.name_);
        std::visit(dash::overloaded{[]([[maybe_unused]] std::monostate& arg) {
                                        std::terminate();
                                    },
                                    [named_arg](auto& arg) {
                                        arg.Parse(named_arg.value_);
                                    }},
                   found_ptr);
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
    void Parse(const ScenParsingLine& line,
               [[maybe_unused]] std::size_t pos = 0)
    {
        auto var_type = line.get_type();
        dash::Expect<dash::ErrorAction::qTerminating, std::range_error>(
            [&var_type, this]() {
                return var_type
                       & dash::Flag{ScenParsingLine::VariableType::qArrayType};
            },
            "Parsed line is not of qArrayType");
        auto index = line.get_index();
        dash::Expect<dash::ErrorAction::qTerminating, std::range_error>(
            [&index, this]() { return target_->size() < index; },
            "Incorrect array element index read");
        // Iterating through items
        using elementT = T::value_type;
        if constexpr (Parser<elementT>::qParsedVariableType
                      == dash::Flag{
                          ScenParsingLine::VariableType::qCommonType}) {
            dash::Expect<dash::ErrorAction::qTerminating,
                         dash::IncorrectArgAccess>(
                [&var_type, this]() {
                    return var_type
                           & dash::Flag{
                               ScenParsingLine::VariableType::qCommonType};
                },
                "Parsed line is not of qArrayType");
            auto& args = line.get_common_args();
            std::size_t i = 0;
            for (const auto& it : args) {
                Parser<elementT>(target_->data() + i++).Parse(it);
            }
        } else if constexpr (Parser<elementT>::qParsedVariableType
                             == dash::Flag{
                                 ScenParsingLine::VariableType::qNamedType}) {
            dash::Expect<dash::ErrorAction::qTerminating,
                         dash::IncorrectArgAccess>(
                [&var_type, this]() {
                    return var_type
                           & dash::Flag{
                               ScenParsingLine::VariableType::qNamedType};
                },
                "Parsed line is not of qArrayType");
            auto& args = line.get_named_args();
            std::size_t i = 0;
            for (const auto& it : args) {
                Parser<elementT>(target_->data() + i++).Parse(it);
            }
        }
    }
private:
    T* target_;
};

template <>
class Parser<int> {
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qCommonType};
    constexpr Parser(int* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        std::from_chars(str_value.cbegin(), str_value.cend(), *target_);
    }
private:
    int* target_;
};

template <>
class Parser<std::size_t> {
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qCommonType};
    constexpr Parser(std::size_t* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        std::from_chars(str_value.cbegin(), str_value.cend(), *target_);
    }
private:
    std::size_t* target_;
};

template <>
class Parser<double> {
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qCommonType};
    constexpr Parser(double* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        std::from_chars(str_value.cbegin(), str_value.cend(), *target_);
    }
private:
    double* target_;
};

template <>
class Parser<std::string> {
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qCommonType};
    constexpr Parser(std::string* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str) { *target_ = str; }
private:
    std::string* target_;
};

#endif // PARSER_H
