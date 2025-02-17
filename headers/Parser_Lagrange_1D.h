#ifndef PARSER_LAGRANGE_1D_H
#define PARSER_LAGRANGE_1D_H

#include <memory>
#include <iostream>
#include <tuple>
#include <set>
#include "Parameters_parser.h"
#include "Parameters_Lagrange_1D.h"

class Parser_Lagrange_1D: public Parameters_parser<Parser_Lagrange_1D>
{
private:
	std::shared_ptr<Parameters_Lagrange_1D> pars_ptr;
	Parameters_Lagrange_1D::viscosity interp_viscosity(std::string_view) const;
	Parameters_Lagrange_1D::ic_preset interp_ic_preset(std::string_view) const;
	Parameters_Lagrange_1D::wall::w_type interp_wall_type(std::string_view) const;
	std::set<void*> initialized_variables
	{
		&pars_ptr->mu0 // adding default-initialized variables
	};
protected:
	Parser_Lagrange_1D(std::filesystem::path _path, std::shared_ptr<Parameters_Lagrange_1D> _pars_ptr)
		try: 
			Parameters_parser<Parser_Lagrange_1D>(_path),
			pars_ptr(_pars_ptr)
		{
		}
		catch(const std::bad_weak_ptr& error)
		{
			std::cerr << error.what();
		}
		catch(const std::ifstream::failure& error)
		{
			std::cerr << error.what() << " " << error.code() << std::endl;
		} // Needs to be handled in caller!
	bool check_initialization_impl() const;
	std::tuple<Parameters_parser_common::var_group, std::string_view, void*> get_var_properties_impl(std::string_view, Parameters_parser_common::var_group, void*) const;
	bool assign_read_value_impl(const std::vector<std::string>&, std::string_view, void*) const;
};

#endif
