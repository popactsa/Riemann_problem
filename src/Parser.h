#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"
#include <charconv>
#include <type_traits>
#include <variant>

void Parse(int* target, std::string_view str_value);

void Parse(std::size_t* target, std::string_view str_value);

void Parse(double* target, std::string_view str_value);

void Parse(std::string* target, std::string_view str_value);

void Parse(bool* target, std::string_view str);

template <typename T>
concept NamedType =
    dash::LabeledAs<T, VariableTypeTag<VariableType::qNamedType>>
    && requires(T t) {
           { t.parsing_table };
           // dash::IsInstanceOf_v<dash::RemoveCVRefT<decltype(t.parsing_table)>,
           //                      dash::TinyMap>;
           // todo: harden a requirement
       };

template <typename T>
concept EnumType = dash::LabeledAs<T, VariableTypeTag<VariableType::qEnumType>>
                   && requires(T t) {
                          { t.parsing_table };
                          // dash::IsInstanceOf_v<dash::RemoveCVRefT<decltype(t.parsing_table)>,
                          //                      dash::TinyMap>;
                          // todo: harden a requirement
                      };

template <typename T>
    requires EnumType<T>
void Parse(T* target, std::string_view str_value)
{
    *target = {T::parsing_table.at(str_value)};
}

template <typename T>
void Parse(T* target, const ScenParsingLine& line)
{
    auto line_type = line.get_type();
    dash::Expect<dash::ErrorAction::qTerminating, dash::IncorrectArgAccess>(
        [&line_type]() {
            return line_type & dash::Flag{VariableType::qCommonType};
        },
        "Parsed line is not of qCommonType");

    Parse(target, line.get_common_arg_at(0));
}

template <typename T>
    requires NamedType<T>
void Parse(T* target, const ScenParsingLine& line)
{
    auto line_type = line.get_type();
    dash::Expect<dash::ErrorAction::qTerminating, dash::IncorrectArgAccess>(
        [&line_type]() {
            return line_type & dash::Flag{VariableType::qNamedType};
        },
        "Parsed line is not of qNamedType");

    auto& named_args = line.get_named_args();
    for (auto named_arg : named_args) {
        auto& found_ptr = target->parsing_table.at(named_arg.name_);
        if (found_ptr.stored) {
            std::visit(dash::overloaded{[&named_arg](auto& arg) {
                           Parse(arg, named_arg.value_);
                       }},
                       *found_ptr.stored);
        }
    }
}

template <typename T>
    requires dash::Container<T>
             && (!std::is_same_v<T, std::string>)
             && (!NamedType<typename T::value_type>)
void Parse(T* target, const ScenParsingLine& line)
{
    auto var_type = line.get_type();
    dash::Expect<dash::ErrorAction::qTerminating, std::range_error>(
        [&var_type]() {
            auto mask = (dash::Flag{VariableType::qArrayType}
                         | dash::Flag{VariableType::qCommonType});
            return ((var_type & mask) == mask);
        },
        "Parsed line is not of qArrayType & qCommonType");
    // Iterating through items
    auto& args = line.get_common_args();
    std::size_t i = 0;
    for (const auto& it : args) {
        Parse(target->data() + i++, it);
    }
}

template <typename T>
    requires NamedType<typename T::value_type> && dash::Container<T>
void Parse(T* target, const ScenParsingLine& line)
{
    auto var_type = line.get_type();
    dash::Expect<dash::ErrorAction::qTerminating, std::range_error>(
        [&var_type]() {
            auto mask = (dash::Flag{VariableType::qArrayType}
                         | dash::Flag{VariableType::qNamedType});
            return ((var_type & mask) == mask);
        },
        "Parsed line is not of qArrayType & qNamedType");
    auto index = line.get_index();
    dash::Expect<dash::ErrorAction::qTerminating, std::range_error>(
        [&index, &target]() { return index < target->size(); },
        "Incorrect array element index read");
    Parse(target->data() + index, line);
}

#endif // PARSER_H
