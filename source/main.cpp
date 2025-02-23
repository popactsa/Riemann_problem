#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "io_auxiliary.h"
//#include "Lagrange_1D.h"
//#include "elasticity_Lagrange_1D.h"
#include "CJ_Lagrange_1D.h"
//#include "Parameters.h"
//#include "elasticity_Lagrange_1D_Parameters.h"
#include "CJ_Lagrange_1D_Parameters.h"

struct winsize w;
const std::chrono::time_point<std::chrono::system_clock> start_tick_time = std::chrono::system_clock::now();
const std::time_t start_time = std::chrono::system_clock::to_time_t(start_tick_time);

int main()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	namespace fs = std::filesystem;
	const fs::path scenario_dir{"../scenarios"};
	std::string_view postfix = ".scen";
	if (check_rmod(scenario_dir))
	{
		fs::path scenario_file;
		std::cout << "Choose scenario from " << scenario_dir << " : " << std::endl;
		int n_items = print_filenames(scenario_dir, postfix);
		int choose_item = choose_in_range(1, n_items);
		scenario_file = get_path_to_file_in_dir(scenario_dir, choose_item, postfix);

		const CJ_Lagrange_1D_Parameters pars(std::ifstream{scenario_file});
		CJ_Lagrange_1D task(pars);
		if (task.start())
		{
			std::string post_start(static_cast<std::string>("python ../source/post.py ") + std::to_string(pars.nt / pars.nt_write - 1));
			system(post_start.c_str());
#ifdef WIN32
#else
    		system("sxiv graph.png");
#endif // WIN32
		}
	}
	return 0;
}
