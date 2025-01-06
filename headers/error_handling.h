#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <exception>
#include <string>

////////Tour of C++ 2022 B.Stroustrup p.49/////////

enum class Error_action
{
	ignore,
	throwing,
	terminating,
	logging
};

constexpr Error_action default_Error_action = Error_action::throwing;

enum class Error_code
{
	not_specified,
	variable_not_found,
	range_error,
	TBD
};

std::string Error_code_name[]
{
	"not specified"
	"variable not found",
	"range error",
	"TBD"
};

struct Error
{
	Error_code ec;
	std::string msg;

	Error(Error_code _ec, const char* _msg): ec{_ec}, msg{_msg} {}
	Error(Error_code _ec): Error(_ec, "<blank>") {}
	Error(): Error(Error_code::not_specified, "<blank>") {}
	
	const char* what() const
	{
		return msg.c_str();
	}
	inline const char* name()
	{
		return Error_code_name[int(ec)].c_str();
	}
};

template<Error_action action = default_Error_action, class C>
constexpr void expect(C cond, Error x)
{
	if constexpr (action == Error_action::throwing)
		if (!cond()) throw x;
	if constexpr (action == Error_action::terminating)
		if (!cond()) std::terminate();
	if constexpr (action == Error_action::logging)
		if (!cond()) std::cerr << "expect() failure: " << x.name() << ' ' << x.what() << std::endl;
	// ignore --> nothing happens
}

#endif
