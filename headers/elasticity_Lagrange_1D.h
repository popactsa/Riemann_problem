#ifndef ELASTICITY_LAGRANGE_1D_H
#define ELASTICITY_LAGRANGE_1D_H

#include "elasticity_Lagrange_1D_Parameters.h"
#include "iSolver.h"
#include "error_handling.h"
#include "io_auxiliary.h"
#include <vector>

class elasticity_Lagrange_1D: public iSolver {
	public:
		elasticity_Lagrange_1D(const elasticity_Lagrange_1D_Parameters& _par): par{_par} {}
		bool start() override;
		void apply_boundary_conditions() override;
		void solve_step() override;
		void set_initial_conditions() override;
		void get_time_step() override;
		void write_data() override;
		void check_parameters() override;
	private:
		const elasticity_Lagrange_1D_Parameters par;
		std::vector<double> P, rho, U, m, S, P_hydr;
		std::vector<double> v, x;
		std::vector<double> omega; //viscosity
		double t, dt;
	 	int step;
};
#endif
