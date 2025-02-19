#ifndef LAGRANGE_1D_H
#define LAGRANGE_1D_H

#include "Parameters_Lagrange_1D.h"
#include "Parser_Lagrange_1D.h"
#include "iSolver.h"
#include "error_handling.h"
#include "io_auxiliary.h"
#include <memory>
#include <vector>

class Lagrange_1D: public iSolver {
	public:
		Lagrange_1D(std::filesystem::path);
		bool start() override;
		void apply_boundary_conditions() override;
		void solve_step() override;
		void set_initial_conditions() override;
		void get_time_step() override;
		void write_data() override;
		void check_parameters() override;
		Parameters_Lagrange_1D* get_ptr_to_par()
		{
			return &par;
		}
	protected:
		Parameters_Lagrange_1D par;
		Parser_Lagrange_1D parser;
		std::vector<double> P, rho, U, m;
		std::vector<double> v, x;
		std::vector<double> omega; //viscosity
		double t, dt;
		int step;
};
#endif
