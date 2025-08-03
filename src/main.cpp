#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>
#include "auxiliary_functions.hpp"
#include "solver_lagrange1d.hpp"

int main() {
    Solver_Lagrange1d solver;
    solver.load_parameters_from_file_impl(
        dash::cmake_dir() / "scenarios" / "scenario4.yaml");
    solver.run("results/latest");
    return 0;
}
