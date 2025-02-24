#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string_view>
#include "Parser.h"

struct Parameters_common: public Parser
{
	enum class variable_groups
	{
		none,
		common,
		wall
	};
};

template <typename Parameters_spec>
class Parameters
{
private:
	friend Parameters_spec;
	Parameters() = default;
public:
	void* get_wall_property(std::string_view, void*) const;
	void* get_parsing_tbl();
};
#endif
