#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <variant>

#include "Solver_Lagrange_1D.h"
#include "error_handling.h"
#include "io.h"

struct winsize w;
int main()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (IsReadable(qScenDir)) {
        std::cout << "Choose scenario from " << qScenDir << " : " << std::endl;
        std::size_t n_items = PrintFilenames(qScenDir, qPostfix);
        auto [scenario_solver_type, scenario_file] =
            ChooseFileInDir(qScenDir, 0, n_items, qPostfix);
        PolySolver Solver;
        switch (scenario_solver_type) {
            using enum Solvers;
        case qLagrange1D: {
            Solver = Solver_Lagrange_1D();
            break;
        }
        default: {
            throw dash::ParserException("Incorrect solver name");
            break;
        }
        }
    }
    return 0;
}
