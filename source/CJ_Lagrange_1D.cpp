#include "CJ_Lagrange_1D.h"
#include "custom_concepts.h"
#include "io_auxiliary.h"

bool CJ_Lagrange_1D::start()
{
	try
	{
		using t_format = std::chrono::milliseconds;
		const auto solve_start_tick_time = std::chrono::system_clock::now();
		auto print_solve_time = print_time_between_on_exit([&]()
			{
				std::cout << "Chapman-Jouguet Lagrange 1D : done!" << std::endl;
				std::cout << "Processing time : " << count_time_between_const<t_format>(solve_start_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl << std::endl;
			}
		);
		auto prev_tick_time = std::chrono::system_clock::now();
		check_parameters();
		P.resize(par.nx_all);
		rho.resize(par.nx_all);
		U.resize(par.nx_all);
		m.resize(par.nx_all);
		V.resize(par.nx_all);
		W.resize(par.nx_all);
		T.resize(par.nx_all);
		omega.resize(par.nx_all);

		v.resize(par.nx_all + 1);
		x.resize(par.nx_all + 1);

		std::cout << "Allocation : done!" << std::endl;
		std::cout << "Processing time : " << count_time_between<t_format>(prev_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl << std::endl;

		set_initial_conditions();// only nx_fict = 1
	
		for (step = 1; step <= par.nt; ++step)
		{
			solve_step();
			t += dt;
			if (step % par.nt_write == 0)
				write_data();
		}

		std::cout << "Chapman-Jouguet Lagrange 1D calculations : done!" << std::endl;
		std::cout << "Processing time : " << count_time_between<t_format>(prev_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl;
		std::cout << "=================" << std::endl;
		return true;	
	}
	catch(const std::exception& exc)
	{
		std::cerr << "Something bad happened!" << std::endl;
		return false;
	}
}

void CJ_Lagrange_1D::check_parameters()
{
	try
	{
		typedef custom_exceptions::invalid_parameter_value exc;
		typedef custom_exceptions::tbd tbd;
		expect<Error_action::throwing, exc>([this]{return par.x_start < par.x_end; }, "par.x_start < par.x_end");
		for (auto it : par.walls)
		{
			expect<Error_action::throwing, tbd>([&it]{return !(it.n_fict > 1); }, "par.nx_fict > 1");
			expect<Error_action::throwing, exc>([&it]{return !(it.n_fict <= 0); }, "par.nx_fict <= 0");
		}
		expect<Error_action::throwing, exc>([this]{return par.nt > 0; }, "par.nt > 0");
		expect<Error_action::throwing, exc>([this]{return par.nt_write > 0; }, "par.nt_write > 0");
		expect<Error_action::throwing, exc>([this]{return par.CFL > 0.0; }, "par.CFL > 0");
		expect<Error_action::throwing, exc>([this]{return par.mu_0 > 0; }, "par.mu_0 > 0");

		expect<Error_action::throwing, exc>([this]{return par.v_0 > 0; }, "par.v_0 > 0");
		expect<Error_action::throwing, exc>([this]{return par.rho_0 > 0; }, "par.rho_0 > 0");

	}
	catch (const custom_exceptions::parameters_exception& err)
	{
		std::cerr << "failure : must satisfy " << err.what() << std::endl;
		std::terminate();
	}
}

void CJ_Lagrange_1D::set_initial_conditions()
{
	for (int i = 0; i < par.nx_all + 1; ++i)
	{
		x[i] = par.x_start - (par.walls[0].n_fict - i) * par.dx;	
		v[i] = 0.0;
	}
	for (int i = 0; i < par.nx_all; ++i)
	{
		rho[i] = par.rho_0;
		V[i] = 1.0 / rho[i];
		U[i] = par.P_0 / (par.gamma - 1.0) / rho[i];
		if (i <= 10)
			W[i] = 0.0;
		else
			W[i] = 1.0;
		P[i] = (par.gamma - 1.0) * rho[i] * (U[i] + par.Q);
		T[i] = P[i] / rho[i] / par.R;
		m[i] = rho[i] * (x[i + 1] - x[i]);
		omega[i] = 0.0;
	}
}

void CJ_Lagrange_1D::apply_boundary_conditions()
{
	using enum CJ_Lagrange_1D_Parameters::wall::w_type;
	for (int i = 0; i < par.number_of_walls; ++i)
	{
		int edge = (i == 1) ? par.nx_all : 0; // right/left wall cases
		int corrected_edge = (i == 1) ? edge - 1  : edge;
		int first_shift = (i == 1) ? -par.walls[i].n_fict : par.walls[i].n_fict;
		int second_shift = (i == 1) ? first_shift - 1 : first_shift + 1;
		
		switch(par.walls[i].type)
		{
			case noslip:
				v[edge + first_shift] = 0.0;
				v[edge] = -v[edge + second_shift] + 2 * v[edge + first_shift];
				break;
			case flux:
				v[edge + first_shift] = v[edge + second_shift];
				v[edge] = v[edge + second_shift];
				break;
			case piston:
				v[edge + first_shift] = par.v_0;
				v[edge] = -v[edge + second_shift] + 2 * v[edge + first_shift];
				break;
			
		}
		rho[corrected_edge] = rho[corrected_edge + first_shift];
		U[corrected_edge] = U[corrected_edge + first_shift];
	}
}

void CJ_Lagrange_1D::get_time_step()
{
	double min_dt = 1e6;
	for (int i = 1; i < par.nx_all; ++i)
	{
		double dx = x[i + 1] - x[i];
		double v_mean = 0.5 * (v[i + 1] + v[i]);
		double c = std::sqrt(par.gamma * P[i] / rho[i]);
		double dt_temp = par.CFL * dx / (c + fabs(v_mean));
		if (dt_temp < min_dt)
			min_dt = dt_temp;
	}
	dt = min_dt;
}

void CJ_Lagrange_1D::solve_step()
{
	get_time_step();

	double v_last[par.nx_all + 1];
	for (int i = 0; i < par.nx_all + 1; ++i)
		v_last[i] = v[i]; // Last solved layer of v

	for (int i = 0; i < par.nx_all; ++i) // Artificial viscosity blurs head of a shock wave
	{
		using enum CJ_Lagrange_1D_Parameters::viscosity;
		switch(par.visc)
		{
			case none:
			case PIC:
				// if (i == 0)
				// 	std::cout << v[i] << std::endl;
				omega[i] = (v[i] - v[i+1] > 0.0) ? 0.5 * par.mu_0 * rho[i] * (v[i] + v[i+1]) * (v[i] - v[i+1]) : 0.0;
				break;
		}
	}

	// calculating v
	for (int i = par.walls[0].n_fict + 1; i < par.nx_all - par.walls[1].n_fict; ++i) 
		v[i] = v[i] - ((P[i] + omega[i]) - (P[i - 1] + omega[i - 1])) * dt / (0.5 * (m[i] + m[i - 1]));
	
	
	apply_boundary_conditions();
	
	// recalculating x
	for (int i = 0; i < par.nx_all + 1; ++i)
	{
		x[i] = x[i] + v[i] * dt;
	}
	for (int i = par.walls[0].n_fict; i < par.nx_all - par.walls[1].n_fict; ++i)
	{
		// calculating density
		rho[i] = m[i] / (x[i+1] - x[i]);

		// calculating U
		U[i] = U[i] + dt / m[i] 
			* ( v[i] * ( (m[i] * P[i-1] + m[i-1] * P[i]) / (m[i-1] + m[i]) + 0.5 * (omega[i] + omega[i-1])) 
			-  v[i+1] * ( (m[i+1] * P[i] + m[i] * P[i+1]) / (m[i] + m[i+1]) + 0.5 * (omega[i+1] + omega[i])) ) 
			+ (v_last[i + 1] + v_last[i]) * (v_last[i + 1] + v_last[i]) / 8.0 
			- (v[i + 1] + v[i]) * (v[i + 1] + v[i]) / 8.0;
	}
	apply_boundary_conditions();
	for (int i = 0; i < par.nx_all; ++i)
	{
		// calculating volume
		V[i] = 1 / rho[i];
		
		// calculating mass fraction
		double temp = 1.0 - (par.V_0 - V[i]) / (par.V_0 - par.V_CJ);
		if (temp <= W[i])
			W[i] = temp;
		else if ((temp > W[i]) and (temp < 0.9))
			W[i] = 0.0;
		else
			W[i] = 1.0;
		
	
		// calculating pressure
		double P_product = (par.gamma - 1.0) * rho[i] * (U[i] + par.Q);
		P[i] = (W[i] < 0.99) ? (1 - W[i]) * P_product : par.P_0;

		// calculating temperature
		T[i] = P[i] / rho[i] / par.R;
	}
}

void CJ_Lagrange_1D::write_data()
{
	std::string file_name = "../data/" + std::to_string(step) + ".csv";
	std::ofstream file(file_name);

	if (file.is_open())
	{
		double x_grid[par.nx_all];
		//double v_grid[par.nx_all];
		for (int i = 0; i < par.nx_all; ++i)
		{
			x_grid[i] = 0.5 * (x[i + 1] + x[i]);
			//v_grid[i] = 0.5 * (v[i + 1] + v[i]);
		}

		for (int i = par.walls[0].n_fict; i < par.nx_all - par.walls[1].n_fict; ++i) 
			file << x_grid[i] << " " 
				<< rho[i]  << " " 
				<< W[i] << " " 
				<< T[i] << std::endl;
	}
	else
	{
		std::cout << file_name << std::endl;
	}
}
