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
    double lx;
    int    nx;
    int    nt;
    double CFL;
    double gamma;
    double u;
    bool   is_conservative;

    static constexpr int nx_fict = 1;
    double               dx;
    double               dt;
};

#endif    // SOLVER_RIEMANN1D_HPP
