#ifndef SOLVER_RIEMANN1D_HPP
#define SOLVER_RIEMANN1D_HPP
#include "solver.hpp"

class Solver_Riemann1d: public Solver<Solver_Riemann1d> {
public:
    Solver_Riemann1d(Io& io);
    void run_impl();
    void load_parameters_from_file_impl(const std::filesystem::path& path);

private:
    Io& io_;
    Io::parsing_table_t get_parsing_table_impl();
    int    nx;
    int    nt;
    double gamma;
};

#endif    // SOLVER_RIEMANN1D_HPP
