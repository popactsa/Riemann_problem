#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string_view>
#include <set>
#include <functional>
#include "custom.h"

struct Parameters_common
{
	enum class OBJECT_GROUP
	{
		NONE,
		WALL
	};

	std::function<std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>
		(std::string_view, const std::pair<Parameters_common::OBJECT_GROUP, void*>&)> get_object_variable_info;
	std::function<void (std::string_view, std::string_view, void*)> assign_enum_variable;
	unordered_map_w_shs<std::string_view, std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>> parsing_tbl;
	std::set<void*> initialized_variables;
};

template <typename Parameters_spec>
class Parameters: public Parameters_common
{
private:
	friend Parameters_spec;
	Parameters() = default;
public:
	static std::tuple<std::string_view, OBJECT_GROUP, void*> get_object_variable_info(std::string_view object_variable_name, const std::pair<Parameters_common::OBJECT_GROUP, void*>& object_info)
	{
		return Parameters_spec::get_object_variable_info_impl(object_variable_name, object_info);
	}
	std::set<void*>* get_initialized_variables()
	{
		return static_cast<Parameters_spec*>(this)->get_initialized_variables_impl();
	}
	unordered_map_w_shs<std::string_view, std::tuple<std::string_view, OBJECT_GROUP, void*>>* get_parsing_tbl()
	{
		return static_cast<Parameters_spec*>(this)->get_parsing_tbl_impl();
	}
	static void assign_enum_variable(std::string_view variable_type, std::string_view read_value, void* variable_ptr)
	{
		Parameters_spec::assign_enum_variable_impl(variable_type, read_value, variable_ptr);
	}
	void initialize_dependent_variables()
	{
		static_cast<Parameters_spec*>(this)->initialize_dependent_variables_impl();	
	}
};
#endif
