#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <variant>

#include "io_auxiliary.h"
#include "Parameters_Lagrange_1D.h"
#include "Parser.h"
#include "Lagrange_1D.h"
#include "custom.h"
struct winsize w;
const std::chrono::time_point<std::chrono::system_clock> start_tick_time = std::chrono::system_clock::now();
const std::time_t start_time = std::chrono::system_clock::to_time_t(start_tick_time);

enum class solver_type
{
	solver_Lagrange_1D,
	solver_elasticity_Lagrange_1D
};

int main()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	namespace fs = std::filesystem;
	const fs::path scenario_dir{"../scenarios"};
	std::string_view postfix = ".scen";
	if (check_rmod(scenario_dir))
	{
		std::cout << "Choose scenario from " << scenario_dir << " : " << std::endl;
		int n_items = print_filenames(scenario_dir, postfix);
		int choose_item = choose_in_range(1, n_items);
		auto [scenario_solver_type, scenario_file] = get_path_to_file_in_dir(scenario_dir, choose_item, postfix);
		using enum solver_types;
		poly_t<std::monostate, Lagrange_1D> solver;
		switch(scenario_solver_type)
		{
			case solver_Lagrange_1D:
				solver = Lagrange_1D(scenario_file);
				break;
			case unknown:
				break;
		}
		std::visit(overloaded
			{
				[](std::monostate& sol){std::cout << "nothing!" << std::endl;},
				[](auto& sol){sol.start();}
			}, solver
		);
	}
	return 0;
}
