#ifndef PARAMETERS_LAGRANGE_1D_H
#define PARAMETERS_LAGRANGE_1D_H

#include "custom_aliases.h"
#include "Parameters.h"
#include "Parser.h"

#include <string>
#include <cstring>
#include <array>
#include <set>

class Parameters_Lagrange_1D: public Parameters<Parameters_Lagrange_1D>
{
private:
	friend class Lagrange_1D;

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
	} Initial_condition;

	struct Wall
	{
		enum class w_type
		{
			noslip,
			flux
		} type;
		int n_fict = 1; // set implicitly depending on type(will make it later, depending on solver)
		double P, v;
	};
	enum
	{
		number_of_walls = 2
	};
	std::array<Wall, number_of_walls> walls;

	using enum Object_group;
	using Parser_variant = std::variant<Parser_int, Parser_double, Parser_string, Parser_Wall, Parser_Initial_condition>;
	unordered_map_w_shs<std::string_view, Parser_variant> parsing_tbl
	{
		{"x_start", Parser<double>(&x_start)},
		{"x_end", Parser<double>(&x_end)},
		{"nx", Parser<int>(&nx)},
		{"CFL", Parser<double>(&CFL)},
		{"nt", Parser<int>(&nx)},
		{"nt_write", Parser<int>(&nt_write)},
		{"write_file", Parser<std::string>(&write_file)},
		{"gamma", Parser<double>(&gamma)},
		{"mu0", Parser<double>(&mu0)},
		{"viscosity", Parser<double>(&CFL)},
		{"Initial_condition", Parser<},
		{"is_conservative", },
		{"Wall", }
	};
	std::set<void*> initialized_variables
	{
		&mu0 // adding default-initialized variables
	};

	static std::tuple<std::string_view, OBJECT_GROUP, void*> get_WALL_object_variable_info(std::string_view, void*);
	static viscosity interp_viscosity(std::string_view);
	static ic_preset interp_ic_preset(std::string_view);
	static wall::w_type interp_w_type(std::string_view);
public:
	Parameters_Lagrange_1D(const std::filesystem::path& path)
	{
	};
};


#endif
