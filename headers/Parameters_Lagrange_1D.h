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

#include <charconv>

#include <concepts>
#include <iterator>
#include <type_traits>

#include "io_auxiliary.h"
#include "error_handling.h"
#include "custom.h"

class Parameters_Lagrange_1D
{
	public:
		void* get_wall_property(std::string_view, void*) const;
		// General variables
		int nx_all; // calculated implicitly
		double dx;

		double x_start, x_end;
		int nx;
		double CFL;
		int nt;

		double gamma;
		double mu0 = 2;
		bool is_conservative;

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

		int nt_write;
		std::string write_file;
		unordered_map_w_shs<std::string, std::pair<std::string, void*>> general_vars_table
		{
			{"x_start", {"double", &x_start}},
			{"x_end", {"double", &x_end}},
			{"nx", {"int", &nx}},
			{"CFL", {"double", &CFL}},
			{"nt", {"int", &nt}},
			{"nt_write", {"int", &nt_write}},
			{"write_file", {"string", &write_file}},
			{"gamma", {"double", &gamma}},
			{"mu0", {"double", &mu0}},
			{"viscosity", {"viscosity", &visc}},
			{"initial_conditions", {"ic_preset", &ic}},
			{"is_conservative", {"bool", &is_conservative}}
		};

		// Wall-related variables
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
		unordered_map_w_shs<std::string, std::string> wall_properties_map
		{
			{"type", "w_type"},
			{"n_fict", "int"},
			{"P", "double"},
			{"v", "double"}
		};
		static constexpr int number_of_walls = 2;
		std::array<wall, number_of_walls> walls;

		std::set<void*> initialized_variables
		{
			&mu0 // adding default-initialized variables
		};

		Parameters_Lagrange_1D(){};
};


#endif
