#include "Lagrange_1D.h"
#include "Parameters_Lagrange_1D.h"

Lagrange_1D::Lagrange_1D(const std::filesystem::path& _path):
	par(_path)
{}

bool Lagrange_1D::start_impl()
{
	try
	{
		using t_format = std::chrono::milliseconds;
		const auto solve_start_tick_time = std::chrono::system_clock::now();
		auto print_solve_time = print_time_between_on_exit([&]()
			{
				std::cout << "Lagrange_1D : done!" << std::endl;
				std::cout << "Processing time : " << count_time_between_const<t_format>(solve_start_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl << std::endl;
			}
		);
		auto prev_tick_time = std::chrono::system_clock::now();
		check_parameters();
		P.resize(par.nx_all);
		rho.resize(par.nx_all);
		U.resize(par.nx_all);
		m.resize(par.nx_all + 1);
		v.resize(par.nx_all + 1);
		x.resize(par.nx_all + 1);
		omega.resize(par.nx_all);
		std::cout << "Allocation : done!" << std::endl;
		std::cout << "Processing time : " << count_time_between<t_format>(prev_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl << std::endl;

		set_initial_conditions(); // only nx_fict = 1
		for (step = 1; step < par.nt; ++step)
		{
			solve_step();
			t += dt;
			if (step % par.nt_write == 0)	write_data();
		}
		std::cout << "Lagrange 1D calculations : done!" << std::endl;
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

void Lagrange_1D::check_parameters_impl() const
{
	try
	{
		typedef custom_exceptions::invalid_parameter_value exc;
		expect<Error_action::throwing, exc>([this]{return par.x_start < par.x_end; }, "par.x_start < par.x_end");
		for (auto it : par.walls)
		{
			expect<Error_action::throwing, exc>([&it]{return (it.n_fict == 1); }, "par.nx_fict > 1");
			expect<Error_action::throwing, exc>([&it]{return !(it.n_fict <= 0); }, "par.nx_fict <= 0");
		}
		expect<Error_action::throwing, exc>([this]{return par.gamma > 0.0; }, "par.gamma > 0");
		expect<Error_action::throwing, exc>([this]{return par.nt > 0; }, "par.nt > 0");
		expect<Error_action::throwing, exc>([this]{return par.nt_write > 0; }, "par.nt_write > 0");
		expect<Error_action::throwing, exc>([this]{return par.CFL > 0.0; }, "par.CFL > 0");
		expect<Error_action::throwing, exc>([this]{return par.mu0 > 0; }, "par.mu0 > 1");

	}
	catch (const custom_exceptions::parameters_exception& err)
	{
		std::cerr << "failure : must satisfy " << err.what() << std::endl;
		std::terminate();
	}
}

void Lagrange_1D::set_initial_conditions_impl()
{
	double middle_plain = par.x_start + 0.5 * (par.x_end - par.x_start);
	for (auto it : par.walls)
		middle_plain += it.n_fict * par.dx;
	using enum Parameters_Lagrange_1D::ic_preset;
	for (int i = 0; i < par.nx_all + 1; ++i)
	{
		x[i] = par.x_start - (par.walls[0].n_fict - i) * par.dx;	
		switch(par.ic)
		{
			case test2:
				v[i] = (i * par.dx <= middle_plain) 
					? -2.0 
					: 2.0;
				break;
			case test1:
			case test3:
			case test4:
				v[i] = 0.0;
				break;
		}
	}
	for (int i = 0; i < par.nx_all; ++i)
	{
		switch(par.ic)
		{
			case test1:
				if (i * par.dx <= middle_plain)
				{
					P[i] = 1.0;
					rho[i] = 1.0;
				}
				else
				{
					P[i] = 0.1;
					rho[i] = 0.125;
				}
				break;
			case test2:
				P[i] = 0.4;
				rho[i] = 1.0;
				break;
			case test3:
				if (i * par.dx <= middle_plain)
				{
					P[i] = 1000.0;
					rho[i] = 1.0;
				}
				else
				{
					P[i] = 0.01;
					rho[i] = 1.0;
				}
				break;
			case test4:
				if (i * par.dx <= middle_plain)
				{
					P[i] = 0.01;
					rho[i] = 1.0;
				}
				else
				{
					P[i] = 100.0;
					rho[i] = 1.0;
				}
				break;
		}
	  	U[i] = P[i] / (par.gamma - 1.0) / rho[i];
		m[i] = rho[i] * (x[i + 1] - x[i]);
	}
}

void Lagrange_1D::apply_boundary_conditions_impl()
{
	using enum Parameters_Lagrange_1D::wall::w_type;
	for (int i = 0; i < par.number_of_walls; ++i)
	{
		int fict = (i == 1) ? par.nx_all : 0; // right/left wall cases
		switch(par.walls[i].type)
		{
			case noslip:
				v[fict] = i == 1 ? -v[par.nx_all - 1] : -v[1];
				break;
			case flux:
				v[fict] = i == 1 ? v[par.nx_all - 1] : v[1];
			break;
			rho[fict] = i == 1 ? rho[par.nx_all - 1] : rho[1]; // both cases
			U[fict] = i == 1 ? rho[par.nx_all - 1] : rho[1];
		}
	}
}

void Lagrange_1D::get_time_step_impl()
{
	double min_dt = 1.0e6;
	for (int i = 1; i < par.nx_all; ++i)
	{
		double dx = x[i + 1] - x[i];
		double V = 0.5 * (v[i + 1] + v[i]);
		double c = std::sqrt(par.gamma * P[i] / rho[i]);
		double dt_temp = par.CFL * dx / (c + fabs(V));
		if (dt_temp < min_dt) min_dt = dt_temp;
	}
	dt = min_dt;
}

void Lagrange_1D::solve_step_impl()
{
	apply_boundary_conditions();
	get_time_step();

	double v_last[par.nx_all + 1];
	for (int i = 0; i < par.nx_all + 1; ++i) v_last[i] = v[i]; // Last solved layer of v

	for (int i = 0; i < par.nx_all; ++i) // Artificial viscosity blurs head of a shock wave
	{
		using enum Parameters_Lagrange_1D::viscosity;
		switch(par.visc)
		{
			case none:
				omega[i] = 0.0;
				break;
			case Neuman:
				omega[i] = -par.mu0 * rho[i] * fabs(v[i + 1] - v[i]) * (v[i + 1] - v[i]);
				break;
			case Latter:
				omega[i] = ((v[i + 1] - v[i]) < 0.0) ? par.mu0 * rho[i] * (v[i + 1] - v[i]) * (v[i + 1] - v[i]) : 0.0;
				break;
			case linear:
				omega[i] = par.mu0 * rho[i] * (v[i + 1] - v[i]) * m[i];
				break;
			case sum:
				omega[i] = par.mu0 * rho[i] * (v[i + 1] - v[i]) * m[i] - par.mu0 * rho[i]*fabs(v[i + 1] - v[i]) * (v[i + 1] - v[i]);
				break;
		}
	}

	// calculating v
	for (int i = par.walls[0].n_fict + 1; i < par.nx_all - par.walls[1].n_fict; ++i) 
		v[i] = v[i] - ((P[i] + omega[i]) - (P[i - 1] + omega[i - 1])) * dt / (0.5 * (m[i] + m[i - 1]));

	// recalculating x
	for (int i = 0; i < par.nx_all + 1; ++i) x[i] += v[i] * dt;

	// calculating P at boundaries of cells
	double Pb[par.nx_all];
	for (int i = 1; i < par.nx_all; ++i) Pb[i] = 0.5 * (P[i] + omega[i] + P[i-1] + omega[i-1]);

	for (int i = par.walls[0].n_fict; i < par.nx_all - par.walls[1].n_fict; ++i)
	{
		// calculating rho
		rho[i] = rho[i] / (rho[i] * (v[i + 1] - v[i]) * dt / m[i] + 1.0);
		// calculating U
		if (par.is_conservative) 
		{
			double U_temp = U[i];
			U[i] = U[i] - (v[i + 1] * Pb[i + 1] - v[i] * Pb[i]) * dt / m[i] 
				+ (v_last[i + 1] + v_last[i]) * (v_last[i + 1] + v_last[i]) / 8.0 
				- (v[i + 1] + v[i]) * (v[i + 1] + v[i]) / 8.0;
			// Return to unconservative scheme on this step
			if (U[i] < 0.0) U[i] = U_temp / (rho[i] * (v[i + 1] - v[i]) * (par.gamma - 1.0) * dt / m[i] + 1.0);
		}
		else U[i] = U[i] / (rho[i] * (v[i + 1] - v[i]) * (par.gamma - 1.0) * dt / m[i] + 1.0);
	}
	for (int i = 0; i < par.nx_all; ++i) P[i] = rho[i] * (par.gamma - 1.0) * U[i];	
}

void Lagrange_1D::write_data_impl() const
{
	std::string file_name = "../data/" + std::to_string(step) + ".csv";
	std::ofstream file(file_name);

	double x_grid[par.nx_all];
	double v_grid[par.nx_all];
	for (int i = 0; i < par.nx_all; ++i)
	{
		x_grid[i] = 0.5 * (x[i + 1] + x[i]);
		v_grid[i] = 0.5 * (v[i + 1] + v[i]);
	}

	for (int i = par.walls[0].n_fict; i < par.nx_all - par.walls[1].n_fict; ++i) 
		file << x_grid[i] << " " 
			<< rho[i]  << " " 
			<< v_grid[i] << " " 
			<< P[i] << std::endl;
}
