#ifndef PARAMETERS_PARSER_H
#define PARAMETERS_PARSER_H

#include "error_handling.h"
#include "io_auxiliary.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <tuple>

class Parameters_parser_common
{
protected:
	enum class var_group
	{
		none, // commentaries, etc.
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
	Parameters_parser(std::filesystem::path _path):
		fin(_path)
	{
		fin.exceptions(fin.failbit);
	}
	bool check_initialization() const
	{
		return static_cast<const Parser_spec*>(this)->check_initialization_impl();
	}
	std::tuple<Parameters_parser_common::var_group, std::string_view, void*> get_var_properties(std::string_view var_name, std::string_view object) const
	{
		return static_cast<const Parser_spec*>(this)->get_var_properties_impl(var_name, object);
	}
	bool assign_read_value(const std::vector<std::string> &read_v, std::string_view type, void* ptr) const
	{
		return static_cast<const Parser_spec*>(this)->assign_read_value_impl(read_v, type, ptr);
	}
	bool parse_line(var_group &current_group, void* &current_object_ptr)
	{
		std::string read;
		if (fin.eof())
			return false;
		std::getline(fin, read);
		using enum var_group;
		if (read[0] == '#')
			return true;
		else if (read[0] == '{') // begin parsing group
			return true;
		else if (read[0] == '}') // end parsing group
		{
			current_object_ptr = nullptr;
			current_group = none;
			return true;
		}
		else if (read[0] == '\t') // pop aligning character
		{
			read.erase(0, 1);
		}
		std::vector<std::string> read_split;
		split_string_to_v(read, read_split);
		auto [group, type, var_ptr] = get_var_properties(read_split, current_group, current_object_ptr);
		switch(group)
		{
			case none:
				return true;
			case general:
				current_group = none;
				current_object_ptr = nullptr;
				return assign_read_value(read_split, type, var_ptr);
			case wall:
				return assign_read_value(read_split, type, var_ptr);
		}
	}
public:
	bool start()
	{
		var_group current_group = var_group::none;
		void* current_object_ptr = nullptr;
		while(parse_line(current_group, current_object_ptr)); //parsing data from file
		return check_initialization();
	}
	virtual ~Parameters_parser()
	{
		fin.close();
	}
};


#endif
