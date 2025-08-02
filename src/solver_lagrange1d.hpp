#ifndef SOLVER_LAGRANGE1D_HPP
#define SOLVER_LAGRANGE1D_HPP
#include <armadillo>
#include "solver.hpp"

class Solver_Lagrange1d: public Solver<Solver_Lagrange1d> {
public:
    Solver_Lagrange1d(Io& io);
    void run_impl();
    void load_parameters_from_file_impl(const std::filesystem::path& path);

private:
    bool check_parameters() const noexcept;
    void set_initial_conditions();
    void apply_boundary_conditions();
    void solve_step();
    void write_data() const;

    void update_time_step() noexcept;

    Io::parsing_table_t get_parsing_table();
    double lx;
    int    nx;
    int    nt;
    int    nt_write;
    double CFL;
    double gamma;
    double mu0;
    double u;
    bool   is_conservative;
    enum class WallType {
        qNoSlip,
        qFreeFlux
    };
    auto enum_parser(WallType& variable);
    WallType wall_type;
    enum class ViscosityType {
        qNone,
        qNeuman,
        qLatter,
        qLinear,
        qSum
    };
    auto enum_parser(ViscosityType& variable);
    ViscosityType viscosity_type;
    int           initial_conditions_preset;

    arma::vec P;
    arma::vec rho;
    arma::vec U;
    arma::vec m;
    arma::vec v;
    arma::vec x;
    arma::vec omega;
    int       step;
    double    t{0.0};

    static constexpr int nx_fict = 1;
    double               dx;
    double               dt;
};

#endif    // SOLVER_LAGRANGE1D_HPP
