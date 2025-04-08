#include <cstdio>
#include <fstream>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include "io.h"
#include "parser.h"

struct winsize w;
int main()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (IsReadable(qScenDir)) {
        std::cout << "Choose scenario from " << qScenDir << " : " << std::endl;
        std::size_t n_items = PrintFilenames(qScenDir, qPostfix);
        auto [scenario_solver_type, scenario_file] =
            GetPathToScenInDir(qScenDir, 0, n_items, qPostfix);

        Solver_Lagrange_1D solver;
        std::ifstream fin(scenario_file);
        std::string string_line;
        ScenParsingLine line;
        Parser<Solver_Lagrange_1D> parser(solver);
        while (std::getline(fin, string_line)) {
            line = string_line;
            parser.ParseLine(line);
        }
    }
    return 0;
}
