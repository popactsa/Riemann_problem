#include <cstdio>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include "io.h"

struct winsize w;
int main()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (IsReadable(qScenDir)) {
        std::cout << "Choose scenario from " << qScenDir << " : " << std::endl;
        std::size_t n_items = PrintFilenames(qScenDir, qPostfix);
        auto [scenario_solver_type, scenario_file] =
            GetPathToScenInDir(qScenDir, 0, n_items, qPostfix);
    }
    return 0;
}
