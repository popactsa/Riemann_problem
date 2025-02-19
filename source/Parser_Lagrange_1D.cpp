#include "Parser_Lagrange_1D.h"
#include "Parameters_Lagrange_1D.h"
#include "Parameters_parser.h"
#include <map>

template<typename key, typename value>
using ums_w_hs = std::unordered_map<key, value, custom_types::string_hash, std::equal_to<>>;

void Parser_Lagrange_1D::consider_default_initialized_variables_impl()
{
	initialized_variables.emplace(&pars_ptr->mu0);
}

void* Parser_Lagrange_1D::get_wall_ptr_impl(std::string_view read_number) const
{
	int result;
	std::from_chars(read_number.data(), read_number.data() + read_number.size(), result);
	return &(pars_ptr->walls)[result];
}

std::tuple<Parameters_parser_common::var_group, std::string_view, void*> Parser_Lagrange_1D::get_var_properties_impl(std::string_view var_name, Parameters_parser_common::var_group object_group, void* object_ptr) const
{
	using enum Parameters_parser_common::var_group;
	switch(object_group)
	{
		case general: // not being inside of any group
			{
				if (!var_name.compare("wall"))
					return {wall, "wall", nullptr};
				std::map<var_group, ums_w_hs<std::string, std::pair<std::string, void*>>* > maps_ptrs
				{
					{general, &(pars_ptr->general_vars_table)}
				};
				for (auto [group, ptr] : maps_ptrs)
				{
					auto found_ptr = ptr->find(var_name);
					if (found_ptr != ptr->end())
					{
						std::tuple<std::string_view, void*> temp{found_ptr->second};
						return {group, std::get<0>(temp), std::get<1>(temp)};
					}
				}
				return {general, "general", nullptr};
			}
			break;
		case wall:
			{
				auto found = pars_ptr->wall_properties_map.find(var_name);
				if (found != pars_ptr->wall_properties_map.end())
				{
					void* property_ptr = pars_ptr->get_wall_property(found->first, object_ptr);		
					return {wall, found->second, property_ptr};
				}
			}
			break;
	}
}

Parameters_Lagrange_1D::viscosity Parser_Lagrange_1D::interp_viscosity(std::string_view str) const
{
	using enum Parameters_Lagrange_1D::viscosity;
	ums_w_hs<std::string, Parameters_Lagrange_1D::viscosity> tbl
	{
		{"none", none},
		{"Neuman", Neuman},
		{"Latter", Latter},
		{"linear", linear},
		{"sum", sum}
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect viscosity value passed");
	}
}


Parameters_Lagrange_1D::ic_preset Parser_Lagrange_1D::interp_ic_preset(std::string_view str) const
{
	using enum Parameters_Lagrange_1D::ic_preset;
	ums_w_hs<std::string, Parameters_Lagrange_1D::ic_preset> tbl
	{
		{"test1", test1},
		{"test2", test2},
		{"test3", test3},
		{"test4", test4}
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect ic_preset value passed");
	}
}

Parameters_Lagrange_1D::wall::w_type Parser_Lagrange_1D::interp_w_type(std::string_view str) const
{
	using enum Parameters_Lagrange_1D::wall::w_type;
	ums_w_hs<std::string, Parameters_Lagrange_1D::wall::w_type> tbl
	{
		{"noslip", noslip},
		{"flux", flux}
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect wall type value passed");
	}
}

bool Parser_Lagrange_1D::assign_read_value_impl(const std::vector<std::string> &read_v, std::string_view type, void* ptr)
{
	std::string_view value = read_v[1];
	if (!type.compare("double"))
	{
		double result;
		std::from_chars(value.data(), value.data() + value.size(), result);
		*(static_cast<double*>(ptr)) = result;
	}
	else if (!type.compare("int"))
	{
		int result;
		std::from_chars(value.data(), value.data() + value.size(), result);
		*(static_cast<int*>(ptr)) = result;
	}
	else if (!type.compare("bool"))
	{
		if (!value.compare("false"))
			*(static_cast<bool*>(ptr)) = false;
		else if (!value.compare("true"))
			*(static_cast<bool*>(ptr)) = true;
		else
			throw std::invalid_argument("Incorrect bool value");
	}
	else if (!type.compare("ic_preset"))
		*(static_cast<Parameters_Lagrange_1D::ic_preset*>(ptr)) = interp_ic_preset(value);
	else if (!type.compare("w_type"))
		*(static_cast<Parameters_Lagrange_1D::wall::w_type*>(ptr)) = interp_w_type(value);
	else if (!type.compare("viscosity"))
		*(static_cast<Parameters_Lagrange_1D::viscosity*>(ptr)) = interp_viscosity(value);
	expect<Error_action::logging, custom_exceptions::multiple_read_definitions>(
		[ptr, this]() {auto [iter, status] = initialized_variables.emplace(ptr); return status; }, 
		static_cast<std::string>("Variable ") + read_v[0] + static_cast<std::string>(" is already defined")
	); //maybe that's too expensive to have that must static_cast's
	return true;
}

bool Parser_Lagrange_1D::check_initialization_impl() const
{
	bool result = true;
	for (const auto &it : pars_ptr->general_vars_table)
	{
		if (!initialized_variables.contains(it.second.second))
		{
			if (result == true) std::cerr << "Variables are not initialized : " << std::endl;
			result = false;
			std::cerr << '\t' << it.first << std::endl;
		}
	}
	/*for (const auto &it : pars_ptr->wall_vars_table)*/
	/*{*/
	/*	if (!initialized_variables.contains(it.second.second))*/
	/*	{*/
	/*		if (result == true) std::cerr << "Variables are not initialized : " << std::endl;*/
	/*		result = false;*/
	/*		std::cerr << '\t' << it.first << std::endl;*/
	/*	}*/
	/*}*/
	return result;
}

void Parser_Lagrange_1D::initialize_dependent_variables_impl()
{
	pars_ptr->nx_all = pars_ptr->nx;
	for (auto it : pars_ptr->walls)
		pars_ptr->nx_all += it.n_fict;
	pars_ptr->dx = (pars_ptr->x_end - pars_ptr->x_start) / pars_ptr->nx;
}
