#ifndef LAGRANGE_1D_H
#define LAGRANGE_1D_H

#include "Parameters_Lagrange_1D.h"
#include "Solver_base.h"
#include <vector>

class Lagrange_1D final: public Solver_base<Lagrange_1D> 
{
	public:
		Lagrange_1D(const std::filesystem::path&);
		bool start_impl();
		void apply_boundary_conditions_impl();
		void solve_step_impl();
		void set_initial_conditions_impl();
		void get_time_step_impl();
		void write_data_impl() const;
		void check_parameters_impl() const;
	private:
		Parameters_Lagrange_1D par;
		std::vector<double> P, rho, U, m;
		std::vector<double> v, x;
		std::vector<double> omega; //viscosity
		double t, dt;
		int step;
};
#endif
