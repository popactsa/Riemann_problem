#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <type_traits>

namespace custom_exceptions {
class parameters_exception : public std::exception {
    // A common class for exceptions thrown when checking parameters validity
protected:
    std::string what_message;
public:
    explicit parameters_exception(const std::string& msg) : what_message(msg) {}
    const char* what() const noexcept override { return what_message.c_str(); }
};
class invalid_parameter_value : public parameters_exception {
public:
    explicit invalid_parameter_value(const std::string& msg) :
        parameters_exception(msg)
    {
    }
};

class parser_exception : public std::exception {
    // A common class for exceptions thrown when parsing a file
protected:
    std::string what_message;
public:
    explicit parser_exception(const std::string& msg) : what_message(msg) {}
    const char* what() const noexcept override { return what_message.c_str(); }
};
class multiple_read_definitions : public parser_exception {
public:
    explicit multiple_read_definitions(const std::string& msg) :
        parser_exception(msg)
    {
    }
};
class invalid_read_name : public parser_exception {
public:
    explicit invalid_read_name(const std::string& msg) : parser_exception(msg)
    {
    }
};
class invalid_group_control : public parser_exception {
public:
    explicit invalid_group_control(const std::string& msg) :
        parser_exception(msg)
    {
    }
};
} // namespace custom_exceptions

enum class ErrorAction { qIgnore, qThrowing, qTerminating, qLogging };

constexpr ErrorAction qDefaultErrorAction = ErrorAction::qThrowing;

template <ErrorAction action, typename exc, typename C>
constexpr void expect(const C cond, const std::string& msg)
////////Tour of C++ 2022 B.Stroustrup p.49/////////
// @does: Facilitates an error diagnostics
{
    if constexpr (action == ErrorAction::qThrowing)
        if (!cond()) {
            throw exc(msg);
        }
    if constexpr (action == ErrorAction::qTerminating)
        if (!cond()) {
            std::cerr << msg << std::endl;
            std::terminate();
        }
    if constexpr (action == ErrorAction::qLogging)
        if (!cond()) {
            std::cerr << "expect() failure: " << msg << std::endl;
        }
    // Error_action::qIgnore --> nothing happens
}

#endif // ERROR_HANDLING_H
