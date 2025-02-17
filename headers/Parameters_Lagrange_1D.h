#ifndef PARAMETERS_H
#define PARAMETERS_H

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
#include "custom_concepts.h"


class Parameters_Lagrange_1D: public std::enable_shared_from_this<Parameters_Lagrange_1D>
{
	// std::unordered_map w/ string_hash for heterogenious search
	template<typename key, typename value>
	using ums_w_hs = std::unordered_map<key, value, custom_types::string_hash, std::equal_to<>>;

	private:
		struct Private
		{
			explicit Private() = default;
		};

		bool does_initialized_values_contain_all_w_vars(const int, const ums_w_hs<std::string, std::pair<std::string, void*>>&) const noexcept;
		bool are_all_general_variables_initialized() const noexcept;	
		bool are_all_wall_variables_initialized(const std::vector<int>&) const noexcept;
	public:
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

		ums_w_hs<std::string, std::pair<std::string, void*>> general_vars_table
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
		static constexpr int number_of_walls = 2;
		std::array<wall, number_of_walls> walls;
		ums_w_hs<std::string, std::pair<std::string, void*>> wall_vars_table
		{
			{"wall0", {"wall", &walls[0]}},
			{"wall1", {"wall", &walls[1]}},
		};

		std::set<void*> initialized_variables
		{
			&mu0 // adding default-initialized variables
		};
		std::tuple<std::string_view, void*> get_wall_property(std::string_view);
		


		Parameters_Lagrange_1D(Private){};
		static std::shared_ptr<Parameters_Lagrange_1D> create_shared_ptr()
		{
			return std::make_shared<Parameters_Lagrange_1D>(Private());
		}
		std::shared_ptr<Parameters_Lagrange_1D> get_ptr()
		{
			return shared_from_this();
		}
	
	protected:
		viscosity interp_viscosity(std::string_view str) const;
		ic_preset interp_ic_preset(std::string_view str) const;
		wall::w_type interp_wall_type(std::string_view str) const;
	
		void assign_read_value(std::string_view, std::string_view);
		void assign_read_wall_value(std::string_view, std::string_view, const ums_w_hs<std::string, std::pair<std::string, void*>>&, const int);
		std::string set_wall_properties(std::ifstream&, const int);
};


#endif
