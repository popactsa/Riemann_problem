#ifndef PARAMETERS_LAGRANGE_1D_H
#define PARAMETERS_LAGRANGE_1D_H

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <iomanip>
#include <exception>
#include <array>
#include <utility>
#include <set>
#include <algorithm>
#include <tuple>

#include <charconv>

#include <concepts>
#include <iterator>
#include <type_traits>

#include "Parameters.h"
#include "Parser.h"
#include "io_auxiliary.h"
#include "error_handling.h"
#include "custom.h"

class Parameters_Lagrange_1D: public Parameters<Parameters_Lagrange_1D>
{
private:
	friend Parameters_Lagrange_1D;

	void* get_wall_property_impl(std::string_view, void*) const;
	int nx_all, nx, nt; 
	double CFL, dx, x_start, x_end;
	double gamma, mu0 = 2;
	bool is_conservative;

	int nt_write;
	std::string write_file;

	enum class viscosity
	{
		none,
		Neuman,
		Latter,
		linear,
		sum
	} visc;

	enum class ic_preset
	{
		test1,
		test2,
		test3,
		test4
	} ic;

	struct wall
	{
		enum class w_type
		{
			noslip,
			flux
		} type;
		int n_fict = 1; // set implicitly depending on type(will make it later, depending on solver)
		double P, v;
	};
	unordered_map_w_shs<std::string_view, std::string_view> wall_properties_tbl
	{
		{"type", "w_type"},
		{"n_fict", "int"},
		{"P", "double"},
		{"v", "double"}
	};
	enum
	{
		number_of_walls = 2
	};
	std::array<wall, number_of_walls> walls;

	using enum variable_groups;
	unordered_map_w_shs<std::string_view, std::tuple<std::string_view, variable_groups, void*>> parsing_tbl
	{
		{"x_start", {"double", none, &x_start}},
		{"x_end", {"double", none, &x_end}},
		{"nx", {"int", none, &nx}},
		{"CFL", {"double", none, &CFL}},
		{"nt", {"int", none, &nt}},
		{"nt_write", {"int", none, &nt_write}},
		{"write_file", {"string", none, &write_file}},
		{"gamma", {"double", none, &gamma}},
		{"mu0", {"double", none, &mu0}},
		{"viscosity", {"viscosity", none, &visc}},
		{"initial_conditions", {"ic_preset", none, &ic}},
		{"is_conservative", {"bool", none, &is_conservative}},
		{"wall", {"wall", wall, &walls}}
	};
	std::set<void*> initialized_variables
	{
		&mu0 // adding default-initialized variables
	};
public:
	Parameters_Lagrange_1D(const std::filesystem::path& path)
	{
		read(parsing_tbl, initialized_variables, path);
	};
};


#endif
