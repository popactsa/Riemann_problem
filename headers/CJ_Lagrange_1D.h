#ifndef CJ_LAGRANGE_1D_H
#define CJ_LAGRANGE_1D_H

#include "CJ_Lagrange_1D.h"
#include "CJ_Lagrange_1D_Parameters.h"
#include "iSolver.h"
#include "error_handling.h"
#include "io_auxiliary.h"
#include <vector>

class CJ_Lagrange_1D: public iSolver {
	public:
		CJ_Lagrange_1D(const CJ_Lagrange_1D_Parameters& _par): par{_par} {}
		bool start() override;
		void apply_boundary_conditions() override;
		void solve_step() override;
		void set_initial_conditions() override;
		void get_time_step() override;
		void write_data() override;
		void check_parameters() override;
	private:
		const CJ_Lagrange_1D_Parameters par;
		std::vector<double> P, rho, U, V, W, T;
		std::vector<double> v, x, m;
		std::vector<double> omega; //viscosity
		double t, dt;
		int step;
};
#endif
