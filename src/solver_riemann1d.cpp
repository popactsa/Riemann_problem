#include "solver_riemann1d.hpp"
#include "auxiliary_functions.hpp"

Solver_Riemann1d::Solver_Riemann1d(Io& io): io_(io) {}

void Solver_Riemann1d::run_impl() {}

void Solver_Riemann1d::load_parameters_from_file_impl(
    const std::filesystem::path& path) {
    if (std::string_view(path.c_str()).ends_with(".yaml")) {
        if (path.is_relative()) {
            io_.load_parameters_from_yaml(
                scenarios_dir / path, get_parsing_table_impl());
        } else {
            io_.load_parameters_from_yaml(path, get_parsing_table_impl());
        }
    } else {
        throw std::runtime_error("Given file extension is not supported");
    }
    nx += 2 * nx_fict;
    dx  = static_cast<double>(lx) / nx;
    dt  = CFL * dx / u;
}

Io::parsing_table_t Solver_Riemann1d::get_parsing_table_impl() {
    return Io::parsing_table_t{
        {             "lx",              parser(lx)},
        {             "nx",              parser(nx)},
        {             "nt",              parser(nt)},
        {            "CFL",             parser(CFL)},
        {          "gamma",           parser(gamma)},
        {              "u",               parser(u)},
        {"is_conservative", parser(is_conservative)}
    };
}
