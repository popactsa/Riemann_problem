#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"
#include <charconv>
#include <type_traits>
#include <variant>

template <ScenParsingLine::VariableType F>
struct ParserTypeLabel {
    static constexpr dash::Flag qParsedVariableType{F};
};

template <typename T>
class Parser : ParserTypeLabel<ScenParsingLine::VariableType::qNamedType> {
public:
    constexpr static auto qParsedVariableType =
        dash::Flag{ScenParsingLine::VariableType::qNamedType};
    constexpr Parser(T* target) : target_(target) {}
    void Parse(const ScenParsingLine::NamedArg& named_arg)
    {
        auto& found_ptr = target_->parsing_table.at(named_arg.name_);
        if (found_ptr.stored) {
            std::visit(dash::overloaded{[named_arg](auto& arg) {
                           arg.Parse(named_arg.value_);
                       }},
                       *found_ptr.stored);
        }
    }
private:
    T* target_;
};

template <typename T>
    requires dash::Container<T>
class Parser<T> : ParserTypeLabel<ScenParsingLine::VariableType::qArrayType> {
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
            [&index, this]() { return index < target_->size(); },
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
                "Parsed line is not of qCommonType");
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
                "Parsed line is not of qNamedType");
            auto& args = line.get_named_args();
            for (const auto& it : args) {
                Parser<elementT>(target_->data() + index).Parse(it);
            }
        }
    }
private:
    T* target_;
};

template <>
class Parser<int>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
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
class Parser<std::size_t>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
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
class Parser<double>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
    constexpr Parser(double* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        std::from_chars(str_value.cbegin(), str_value.cend(), *target_);
    }
    // private:
    double* target_;
};

template <>
class Parser<std::string>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
    constexpr Parser(std::string* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str) { *target_ = str; }
private:
    std::string* target_;
};

template <>
class Parser<bool>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
    constexpr Parser(bool* target) noexcept : target_(target) {}
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str)
    {
        if (str.compare("true") || str.compare("1")) {
            *target_ = true;
        } else if (str.compare("false") || str.compare("0")) {
            *target_ = false;
        }
    }
private:
    bool* target_;
};

#endif // PARSER_H
