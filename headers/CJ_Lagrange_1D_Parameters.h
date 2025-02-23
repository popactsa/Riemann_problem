#include "CJ_Lagrange_1D.h"
#ifndef CJ_LAGRANGE_1D_PARAMETERS_H
#define CJ_LAGRANGE_1D_PARAMETERS_H

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <exception>
#include <array>
#include <utility>
#include <set>
#include <algorithm>

#include <concepts>
#include <iterator>
#include <type_traits>

#include "io_auxiliary.h"
#include "error_handling.h"
#include "custom_concepts.h"


class CJ_Lagrange_1D_Parameters
{
	// std::unordered_map w/ string_hash for heterogenious search
	template<typename key, typename value>
	using ums_w_hs = std::unordered_map<key, value, string_hash, std::equal_to<>>;
	public:
		// General variables
		int nx_all; // calculated implicitly
		double dx;
		double x_start, x_end;
		int nx;
		double CFL;
		int nt;
		double gamma, Q, P_0, v_0, rho_0, V_0, V_CJ; // material properties

		enum class viscosity
		{
			none,
			PIC,
		} visc;

		enum class ic_preset
		{
			test1,
		} ic;

		double mu_0 = 2.0;
		int nt_write;
		std::string write_file;

		// Wall-related variables
		struct wall
		{
			enum class w_type
			{
				noslip,
				flux,
				piston
			} type;
			int n_fict = 1; // set implicitly depending on type(will make it later, depending on solver)
		};
		
		static constexpr int number_of_walls = 2;
		std::array<wall, number_of_walls> walls;

		CJ_Lagrange_1D_Parameters(){};
		CJ_Lagrange_1D_Parameters(std::ifstream);
	
	protected:
		viscosity interp_viscosity(std::string_view str) const;
		ic_preset interp_ic_preset(std::string_view str) const;
		wall::w_type interp_wall_type(std::string_view str) const;
	
		void assign_read_value(const std::string&, std::string_view);
		void assign_read_wall_value(const std::string&, std::string_view, const ums_w_hs<std::string, std::pair<std::string, void*>>&, const int);
		std::string set_wall_properties(std::ifstream&, const int);

	private:
		ums_w_hs<std::string, std::pair<std::string, void*>> var_table
		{
			{"x_start", {"double", &x_start}},
			{"x_end", {"double", &x_end}},
			{"nx", {"int", &nx}},
			{"CFL", {"double", &CFL}},
			{"gamma", {"double", &gamma}},
			{"Q", {"double", &Q}},
			{"P_0", {"double", &P_0}},
			{"rho_0", {"double", &rho_0}},
			{"nt", {"int", &nt}},
			{"nt_write", {"int", &nt_write}},
			{"write_file", {"string", &write_file}},
			{"viscosity", {"viscosity", &visc}},
			{"initial_conditions", {"ic_preset", &ic}},
		};

		std::set<void*> initialized_variables
		{
			&mu_0 // adding default-initialized variables
		};

		bool does_initialized_values_contain_all_w_vars(const int, const ums_w_hs<std::string, std::pair<std::string, void*>>&) const noexcept;
		bool are_all_general_variables_initialized() const noexcept;	
		bool are_all_wall_variables_initialized(const std::vector<int>&) const noexcept;
};


#endif
