#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "io_auxiliary.h"
#include "Parameters_Lagrange_1D.h"
#include "Parser_base.h"
#include "Parser_Lagrange_1D.h"
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
		bool task_status = false;
		using enum solver_types;
		int nt, nt_write;
		switch(scenario_solver_type)
		{
			case solver_Lagrange_1D:
				{
					Lagrange_1D task(scenario_file);
					task_status = task.start();
					auto par_ptr = task.get_ptr_to_par();
					nt = par_ptr->nt;
					nt_write = par_ptr->nt_write;
				}
				break;
			case unknown:
				break;
		}
		if (task_status)
		{
			std::string post_start(static_cast<std::string>("python ../source/post.py ") + std::to_string(nt / nt_write - 1));
			system(post_start.c_str());
#ifdef WIN32
#else
    			system("sxiv graph.png");
#endif // WIN32
		}
	}
	return 0;
}
