#include "solver_riemann1d.hpp"

Solver_Riemann1d::Solver_Riemann1d(Io& io): io_(io) {}

void Solver_Riemann1d::load_parameters_from_file_impl(
    const std::filesystem::path& path) {
    if (std::string_view(path.c_str()).ends_with(".yaml")) {
        io_.load_parameters_from_yaml(path, get_parsing_table_impl());
    } else {
        throw std::runtime_error("Given file extension is not supported");
    }
}

Io::parsing_table_t Solver_Riemann1d::get_parsing_table_impl() {
    return Io::parsing_table_t{
        {   "nx",    parser(nx)},
        {   "nt",    parser(nt)},
        {"gamma", parser(gamma)},
    };
}
