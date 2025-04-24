#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <type_traits>

namespace dash {
class ParametersException : public std::exception {
    // A common class for exceptions thrown when checking parameters validity
protected:
    std::string what_message;
public:
    explicit ParametersException(const std::string& msg) : what_message(msg) {}
    const char* what() const noexcept override { return what_message.c_str(); }
};
class InvalidParameterValue : public ParametersException {
public:
    explicit InvalidParameterValue(const std::string& msg) :
        ParametersException(msg)
    {
    }
};

class ParserException : public std::exception {
    // A common class for exceptions thrown when parsing a file
protected:
    std::string what_message;
public:
    explicit ParserException(const std::string& msg) : what_message(msg) {}
    const char* what() const noexcept override { return what_message.c_str(); }
};
class MultipleReadDefinitions : public ParserException {
public:
    explicit MultipleReadDefinitions(const std::string& msg) :
        ParserException(msg)
    {
    }
};
class InvalidReadName : public ParserException {
public:
    explicit InvalidReadName(const std::string& msg) : ParserException(msg) {}
};

enum class ErrorAction { qIgnore, qThrowing, qTerminating, qLogging };

constexpr ErrorAction qDefaultErrorAction = ErrorAction::qThrowing;

template <ErrorAction action = qDefaultErrorAction, typename exc, typename C>
constexpr void Expect(const C cond, const std::string& msg)
////////Tour of C++ 2022 B.Stroustrup p.49/////////
// @does: Facilitates error diagnostics
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
} // namespace dash

#endif // ERROR_HANDLING_H
