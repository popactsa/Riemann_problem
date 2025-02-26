#include "Parser.h"
#include "Parameters.h"

void Parser::parse_line(std::ifstream& fin, object_info_type& object, 
	parsing_tbl_type* parsing_tbl, initialized_variables_type* initialized_variables, get_object_variable_info_function_type* get_object_variable_info,
	assign_enum_variable_function_type* assign_enum_variable_function)
{
	using enum Parameters_common::OBJECT_GROUP;
	std::string read;
	std::getline(fin, read);
	while (read[0] == '\t') read.erase(0, 1);
	if (read[0] == '#' 		// commentary
		|| read[0] == '!') 	// info is not meant to be parsed into a variable
	{
		return;
	}
	if (read[0] == '}') 		// end parsing object
	{
		object = {NONE, nullptr};
		return;
	}
	
	std::vector<std::string_view> read_split = split_string_view_to_v(read);
	variable_info_type variable = get_read_variable_info(read_split, object, parsing_tbl, get_object_variable_info);
	void* variable_ptr;
	std::string_view variable_type;
	Parameters_common::OBJECT_GROUP variable_supposed_object_group;
	std::tie(variable_type, variable_supposed_object_group, variable_ptr) = variable;

	if (variable_supposed_object_group != NONE
			&& std::get<Parameters_common::OBJECT_GROUP>(object) == NONE) // i.e. its a group section beginning
	{
		object = {variable_supposed_object_group, find_object_ptr(variable_ptr, read_split)};
		return;
	}
	assign_read_variable(variable, read_split[1], get_object_variable_info, assign_enum_variable_function);
	initialized_variables->emplace(variable_ptr);
}

void* Parser::find_object_ptr(void* object_ptr_array_ptr, const std::vector<std::string_view>& read_split)
{
	int offset;
	std::from_chars(read_split[1].data(), read_split[1].data() + read_split[1].size(), offset);
	return static_cast<char*>(object_ptr_array_ptr) + offset;
}

void Parser::assign_read_variable(variable_info_type& variable, std::string_view read_value,
		get_object_variable_info_function_type* get_object_variable_info_function,
		assign_enum_variable_function_type* assign_enum_variable_function)
{
	using enum Parameters_common::OBJECT_GROUP;
	auto [variable_type, object_group, variable_ptr] = variable;
	if (!variable_type.compare("double"))
	{
		double value;
		std::from_chars(read_value.data(), read_value.data() + read_value.size(), value);
		*(static_cast<double*>(variable_ptr)) = value;
	}
	else if (!variable_type.compare("int"))
	{
		int value;
		std::from_chars(read_value.data(), read_value.data() + read_value.size(), value);
		*(static_cast<int*>(variable_ptr)) = value;
	}
	else if (!variable_type.compare("bool"))
	{
		if (!read_value.compare("false"))
			*(static_cast<bool*>(variable_ptr)) = false;
		else if (!read_value.compare("true"))
			*(static_cast<bool*>(variable_ptr)) = true;
		else
			throw std::invalid_argument("Incorrect bool value");
	}
	else // enums
	{
		assign_enum_variable_function(variable_type, read_value, variable_ptr);
	}
}

std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*> 
Parser::get_read_variable_info(const std::vector<std::string_view>& read_split, const object_info_type& object, 
		parsing_tbl_type* parsing_tbl, get_object_variable_info_function_type* get_object_variable_info)
{
	std::string_view read_name = read_split[0];
	using enum OBJECT_GROUP;
	switch(std::get<OBJECT_GROUP>(object))
	{
		case NONE:
			{
				auto found = parsing_tbl->find(read_name);
				if (found == parsing_tbl->end())
					std::terminate();
				return found->second;
				break;
			}
		case WALL:
			{
				return get_object_variable_info(read_name, object);
			}
	}
}
