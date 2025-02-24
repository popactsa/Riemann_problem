#ifndef PARSER_LAGRANGE_1D_H
#define PARSER_LAGRANGE_1D_H

#include <memory>
#include <iostream>
#include <tuple>
#include "Parser_base.h"
#include "Parameters_Lagrange_1D.h"

class Parser_Lagrange_1D final: public Parser_base<Parser_Lagrange_1D>
{
private:
	std::unique_ptr<Parameters_Lagrange_1D> pars_ptr;
protected:

	Parameters_Lagrange_1D::viscosity interp_viscosity(std::string_view) const;
	Parameters_Lagrange_1D::ic_preset interp_ic_preset(std::string_view) const;
	Parameters_Lagrange_1D::wall::w_type interp_w_type(std::string_view) const;
public:
	bool check_initialization_impl() const;
	std::tuple<Parser_base_common::var_group, std::string_view, void*> get_var_properties_impl(std::string_view, Parser_base_common::var_group, void*) const;
	bool assign_read_value_impl(const std::vector<std::string>&, std::string_view, void*);
	void consider_default_initialized_variables_impl();
	void* get_wall_ptr_impl(std::string_view) const;
	bool initialize_dependent_variables_impl();
	Parser_Lagrange_1D(std::filesystem::path _path, Parameters_Lagrange_1D* _pars_ptr):
		Parser_base<Parser_Lagrange_1D>(_path),
		pars_ptr(_pars_ptr)
	{
	}
};

#endif
