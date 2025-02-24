#ifndef PARSER_H
#define PARSER_H

#include "Parameters.h"
#include "Parameters_Lagrange_1D.h"
#include "custom.h"
#include "io_auxiliary.h"
#include <filesystem>
#include <fstream>
#include <string>

class Parser
{
protected:
	using obj_info = std::tuple<std::string_view, Parameters_common::variable_groups, void*>;
	using group_info = std::pair<Parameters_common::variable_groups, void*>;
	using parsing_tbl_type = unordered_map_w_shs<std::string_view, std::tuple<std::string_view, Parameters_common::variable_groups, void*>>;
	static void parse_line(std::ifstream&, obj_info&, group_info&, parsing_tbl_type&, std::set<void*>&);
	static void* find_current_structure_ptr(const std::vector<std::string_view>&);
	static void assign_read_variable(const std::vector<std::string_view>&, const group_info&);
	static obj_info get_read_variable_info(const std::vector<std::string_view>&, const group_info&, parsing_tbl_type&);
	static void read(parsing_tbl_type&, std::set<void*>&, const std::filesystem::path&);
};

void Parser::read(parsing_tbl_type& parsing_tbl, std::set<void*>& initialized_variables, const std::filesystem::path& path)
{
	std::ifstream fin(path);
	obj_info current_obj_info;
	group_info current_structure;
	while (fin.peek() != EOF)
		parse_line(fin, current_obj_info, current_structure, parsing_tbl, initialized_variables);
}

void Parser::parse_line(std::ifstream& fin, obj_info& obj, group_info& current_structure, parsing_tbl_type& parsing_tbl, std::set<void*>& initialized_variables)
{
	using enum Parameters_common::variable_groups;
	std::string read;
	std::getline(fin, read);
	while (read[0] == '\t') read.erase(0, 1);
	if (read[0] == '#' // commentary
	|| read[0] == '!') // info is not meant to be parsed into parameters
	{
		return;
	}
	if (read[0] == '}')
	{
		current_structure = {none, nullptr};
	}
	std::vector<std::string_view> read_split = split_string_view_to_v(read);
	obj = get_read_variable_info(read_split, current_structure, parsing_tbl);

	void* obj_ptr;
	std::string_view obj_name;
	Parameters_common::variable_groups obj_supposed_group;
	std::tie(obj_name, obj_supposed_group, obj_ptr) = obj;

	if (obj_supposed_group != none
			&& std::get<Parameters_common::variable_groups>(current_structure) == none) // i.e. its a group section beginning
	{
		current_structure = {obj_supposed_group, find_current_structure_ptr(read_split)};
		return;
	}
	assign_read_variable(read_split, current_structure);
	initialized_variables.emplace(obj_ptr);
}


#endif
