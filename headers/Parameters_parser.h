#ifndef PARAMETERS_PARSER_H
#define PARAMETERS_PARSER_H

#include "error_handling.h"
#include "io_auxiliary.h"
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>
#include <set>

class Parameters_parser_common
{
protected:
	enum class var_group
	{
		general, // single simple variables 
		wall // wall is a struct, containing several simple variables
	};
};

template<typename Parser_spec>
struct Parameters_parser: public Parameters_parser_common
// Base class for parsers
{
protected:
	std::ifstream fin;
	std::set<void*> initialized_variables
	{
	};
	Parameters_parser(std::filesystem::path _path):
		fin(_path)
	{
		fin.exceptions(fin.failbit);
	}
	bool check_initialization() const
	{
		return static_cast<const Parser_spec*>(this)->check_initialization_impl();
	}
	std::tuple<Parameters_parser_common::var_group, std::string_view, void*> get_var_properties(std::string_view var_name, Parameters_parser_common::var_group object_group, void* object_ptr) const
	{
		return static_cast<const Parser_spec*>(this)->get_var_properties_impl(var_name, object_group, object_ptr);
	}
	bool assign_read_value(const std::vector<std::string> &read_v, std::string_view type, void* ptr)
	{
		return static_cast<Parser_spec*>(this)->assign_read_value_impl(read_v, type, ptr);
	}
	void* get_wall_ptr(std::string_view read_number) const
	{
		return static_cast<const Parser_spec*>(this)->get_wall_ptr_impl(read_number);
	}
	void initialize_dependent_variables()
	{
		return static_cast<Parser_spec*>(this)->initialize_dependent_variables_impl();
	}
	bool parse_line(var_group &current_group, void* &current_object_ptr)
	{
		std::string read;
		if (fin.eof())
			return false;
		std::getline(fin, read);
		std::cout << read << std::endl;
		using enum var_group;
		if (read[0] == '#')
			return true;
		else if (read[0] == '!') // variables, that won't be assigned(solver name, etc.)
			return true;
		else if (read[0] == '{') // begin parsing group
			return true;
		else if (read[0] == '}') // end parsing group
		{
			current_object_ptr = nullptr;
			current_group = general;
			return true;
		}
		else if (read[0] == '\t') // pop aligning character
		{
			read.erase(0, 1);
		}
		std::vector<std::string> read_split;
		split_string_to_v(read, read_split);
		auto [group, type, var_ptr] = get_var_properties(read_split[0], current_group, current_object_ptr);
		switch(group)
		{
			case general:
				if (var_ptr == nullptr)
				{
					std::cout << read_split[0] << " : not found!" << std::endl;
					std::terminate();
				}
				else
					return assign_read_value(read_split, type, var_ptr);
			case wall:
				{
					if (var_ptr == nullptr) // e.g. it's a wall declaration string read, now get ptr to wall
					{
						current_object_ptr = get_wall_ptr(read_split[1]);
						current_group = wall;
						return true;
					}
					else
					{
						return assign_read_value(read_split, type, var_ptr);
					}
				}
		}
	}
	void consider_default_initialized_variables()
	{
		return static_cast<Parser_spec*>(this)->consider_default_initialized_variables_impl();
	}
public:
	bool start()
	{
		var_group current_group = var_group::general;
		void* current_object_ptr = nullptr;
		consider_default_initialized_variables();
		while(fin.peek() != EOF)
			parse_line(current_group, current_object_ptr);//parsing data from file
		check_initialization();
		initialize_dependent_variables();
		return true;
	}
	virtual ~Parameters_parser()
	{
		fin.close();
	}
};


#endif
