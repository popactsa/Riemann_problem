#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Solver_Lagrange_1D.h"
#include "error_handling.h"
#include "iSolver.h"
#include "io.h"

struct winsize w;
int main()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    PolySolver Solver{};
    if (IsReadable(qScenDir)) {
        std::cout << "Choose scenario from " << qScenDir << " : " << std::endl;
        std::size_t n_items = PrintFilenames(qScenDir, qPostfix);
        auto [scenario_solver_type, scenario_file] =
            ChooseFileInDir(qScenDir, 0, n_items, qPostfix);
        switch (scenario_solver_type) {
            using enum Solvers;
        case qLagrange1D: {
            Solver.emplace<Solver_Lagrange_1D>();
            break;
        }
        default: {
            throw dash::ParserException("Incorrect solver name");
            break;
        }
        }
        if (Solver.stored) {
            std::visit(dash::overloaded{[&scenario_file](auto& arg) {
                           arg.ReadParameters(scenario_file);
                           arg.Start();
                           // {} - stands for 'the last step'
                           arg.ShowResultAtStep({});
                       }},
                       *Solver.stored);
        }
    }

    return 0;
}
