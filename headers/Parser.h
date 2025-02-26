#ifndef PARSER_H
#define PARSER_H

#include "Parameters.h"
#include "custom.h"
#include "io_auxiliary.h"
#include <filesystem>
#include <set>
#include <fstream>
#include <vector>

class Parser
{
protected:
	template<typename P>
	friend void start_Parser(P&, const std::filesystem::path&);
	Parser() = default;

	using OBJECT_GROUP = Parameters_common::OBJECT_GROUP;
	using variable_info_type = std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>;
	using object_info_type = std::pair<Parameters_common::OBJECT_GROUP, void*>;
	using parsing_tbl_type = unordered_map_w_shs<std::string_view, std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>>;
	using get_object_variable_info_function_type = std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>
		(std::string_view, const std::pair<Parameters_common::OBJECT_GROUP, void*>&);
	using assign_enum_variable_function_type = void (std::string_view, std::string_view, void*);
	using initialized_variables_type = std::set<void*>;

	static void assign_read_variable(variable_info_type&, std::string_view,
			get_object_variable_info_function_type*, assign_enum_variable_function_type*);
	static variable_info_type get_read_variable_info(const std::vector<std::string_view>&, const object_info_type&, 
			parsing_tbl_type*, get_object_variable_info_function_type*);
	static void* find_object_ptr(void*, const std::vector<std::string_view>&);
public:
	static void parse_line(std::ifstream&, object_info_type&, 
			parsing_tbl_type*, initialized_variables_type*, 
			get_object_variable_info_function_type*, assign_enum_variable_function_type*);
};

template<typename Parameters_spec>
	requires custom::concepts::is_crtp_base_of_v<Parameters, Parameters_spec>
void start_Parser(Parameters_spec& target, const std::filesystem::path& path)
{
	std::ifstream fin(path);
	using object_info_type = std::pair<Parameters_common::OBJECT_GROUP, void*>;
	object_info_type object_info;
	while (fin.peek() != EOF)
		Parser::parse_line(fin, object_info, 
			target.get_parsing_tbl(), target.get_initialized_variables(), 
			&Parameters_spec::get_object_variable_info, &Parameters_spec::assign_enum_variable);
	target.initialize_dependent_variables();
}

#endif
