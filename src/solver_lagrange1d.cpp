#include "solver_lagrange1d.hpp"
#include "auxiliary_functions.hpp"
#include "solver.hpp"

Solver_Lagrange1d::Solver_Lagrange1d(Io& io): Solver(io) {}

void Solver_Lagrange1d::load_parameters_from_file_impl(
    const std::filesystem::path& path) {
    if (std::string_view(path.c_str()).ends_with(".yaml")) {
        if (path.is_relative()) {
            io_.load_parameters_from_yaml(
                scenarios_dir / path, get_parsing_table());
        } else {
            io_.load_parameters_from_yaml(path, get_parsing_table());
        }
    } else {
        throw std::runtime_error("Given file extension is not supported");
    }
    nx += 2 * nx_fict;
    dx  = static_cast<double>(lx) / nx;
    dt  = CFL * dx / u;
}

auto Solver_Lagrange1d::enum_parser(ViscosityType& variable) {
    using enum ViscosityType;
    static const std::unordered_map<std::string_view, ViscosityType> tbl{
        {"None",   qNone  },
        {"Neuman", qNeuman},
        {"Latter", qLatter},
        {"Sum",    qSum   }
    };
    return parser(tbl, variable);
}

auto Solver_Lagrange1d::enum_parser(WallType& variable) {
    using enum WallType;
    static const std::unordered_map<std::string_view, WallType> tbl{
        {"NoSlip",   qNoSlip  },
        {"FreeFlux", qFreeFlux}
    };
    return parser(tbl, variable);
}

Io::parsing_table_t Solver_Lagrange1d::get_parsing_table() {
    return Io::parsing_table_t{
        {"lx",                        parser(lx)                       },
        {"nx",                        parser(nx)                       },
        {"nt",                        parser(nt)                       },
        {"nt write",                  parser(nt_write)                 },
        {"mu0",                       parser(mu0)                      },
        {"CFL",                       parser(CFL)                      },
        {"viscosity type",            enum_parser(viscosity_type)      },
        {"wall type",                 enum_parser(wall_type)           },
        {"gamma",                     parser(gamma)                    },
        {"u",                         parser(u)                        },
        {"initial conditions preset", parser(initial_conditions_preset)},
        {"is conservative",           parser(is_conservative)          }
    };
}

void Solver_Lagrange1d::run_impl() {
    if (!check_parameters()) {
        throw std::runtime_error("Incorrect parameters given");
    }
    auto solving_timer = dash::SetScopedTimer("Solved in");
    P.resize(nx);
    rho.resize(nx);
    U.resize(nx);
    m.resize(nx + 1);
    v.resize(nx + 1);
    x.resize(nx + 1);
    omega.resize(nx);
    set_initial_conditions();
    for (step = 1; step < nt; ++step) {
        apply_boundary_conditions();
        update_time_step();
        solve_step();
        t += dt;
        if (step % nt_write == 0) {
            write_data();
        }
    }
}

bool Solver_Lagrange1d::check_parameters() const noexcept {
    bool status{true};
    status &= lx > 0.0;
    status &= nx > 1 + 2 * nx_fict;
    status &= gamma > 0.0;
    status &= nt_write > 0;
    status &= nt >= nt_write;
    status &= CFL > 0.0;
    status &= mu0 > 0.0;
    status &= initial_conditions_preset >= 0;
    status &= initial_conditions_preset < 4;
    return status;
}

void Solver_Lagrange1d::update_time_step() noexcept {
    double min_dt = 1.0e6;
    double dx, V, c, dt_temp;
    for (int i = 1; i < nx; ++i) {
        dx      = x(i + 1) - x(i);
        V       = 0.5 * (v(i + 1) + v(i));
        c       = std::sqrt(gamma * P(i) / rho(i));
        dt_temp = CFL * dx / (c + std::fabs(V));
        if (dt_temp < min_dt) {
            min_dt = dt_temp;
        }
    }
    dt = min_dt;
}

void Solver_Lagrange1d::set_initial_conditions() {
    double middle_plain = 0.5 * lx;
    for (int i{0}; i < nx + 1; ++i) {
        x(i) = (i - 1) * dx;
        switch (initial_conditions_preset) {
        case 1:
            v(i) = (i * dx <= middle_plain) ? -2.0 : 2.0;
            break;
        case 0:
        case 2:
        case 4:
            v(i) = 0.0;
            break;
        default:
            assert(false);
        }
    }
    for (int i{0}; i < nx; ++i) {
        switch (initial_conditions_preset) {
        case 0:
            if (i * dx <= middle_plain) {
                P(i)   = 1.0;
                rho(i) = 1.0;
            } else {
                P(i)   = 0.1;
                rho(i) = 0.125;
            }
            break;
        case 1:
            P(i)   = 0.4;
            rho(i) = 1.0;
            break;
        case 2:
            if (i * dx <= middle_plain) {
                P(i)   = 1000.0;
                rho(i) = 1.0;
            } else {
                P(i)   = 0.01;
                rho(i) = 1.0;
            }
            break;
        case 3:
            if (i * dx <= middle_plain) {
                P(i)   = 0.01;
                rho(i) = 1.0;
            } else {
                P(i)   = 100.0;
                rho(i) = 1.0;
            }
            break;
        default:
            assert(false);
        }
        U(i) = P(i) / (gamma - 1.0) / rho(i);
        m(i) = rho(i) * (x(i + 1) - x(i));
    }
}

void Solver_Lagrange1d::apply_boundary_conditions() {
    switch (wall_type) {
        using enum WallType;
    case qNoSlip:
        v(0)  = -v(1);
        v(nx) = -v(nx - 1);
        break;
    case qFreeFlux:
        v(0)  = v(1);
        v(nx) = v(nx - 1);
        break;
    }
    rho(0)      = rho(1);
    rho(nx - 1) = rho(nx - 2);
    U(0)        = rho(0);
    U(nx - 1)   = rho(nx - 1);
}

void Solver_Lagrange1d::solve_step() {
    arma::vec v_last = v;
    for (int i{0}; i < nx; ++i) {
        double vdiff     = v(i + 1) - v(i);
        double sqr_vdiff = std::pow(vdiff, 2);
        switch (viscosity_type) {
            using enum ViscosityType;
        case qNone:
            omega(i) = 0.0;
            break;
        case qNeuman:
            omega(i)  = -mu0 * rho(i) * sqr_vdiff;
            omega(i) *= vdiff >= 0 ? 1.0 : -1.0;
            break;
        case qLatter:
            omega(i) = vdiff < 0.0 ? mu0 * rho(i) * sqr_vdiff : 0.0;
            break;
        case qLinear:
            omega(i) = mu0 * rho(i) * vdiff * m(i);
            break;
        case qSum:
            omega(i)  = mu0 * rho(i) * (vdiff * m(i) - sqr_vdiff);
            omega(i) *= vdiff >= 0.0 ? 1.0 : -1.0;
        }
    }
    for (int i{2}; i < nx - 1; ++i) {
        v(i) -= ((P(i) + omega(i)) - (P(i - 1) + omega(i - 1)))
              * dt
              / (0.5 * (m(i) + m(i - 1)));
    }

    // Recalculating grid
    for (int i = 0; i < nx + 1; ++i) {
        x(i) += v(i) * dt;
    }
    for (int i{1}; i < nx - 1; ++i) {
        double Pb_i    = 0.5 * (P(i) + omega(i) + P(i - 1) + omega(i - 1));
        double Pb_ip1  = 0.5 * (P(i + 1) + omega(i + 1) + P(i) + omega(i));
        rho(i)        /= 1.0 + rho(i) * (v(i + 1) - v(i)) * dt / m(i);
        double U_temp  = U(i);
        if (is_conservative) {
            U(i) += -(v(i + 1) * Pb_ip1 - v(i) * Pb_i) * dt / m(i)
                  + std::pow(v_last(i + 1) + v_last(i), 2) / 8.0
                  - std::pow(v(i + 1) + v(i), 2) / 8.0;
        }
        if (!is_conservative || U(i) < 0.0) {
            U(i) = U_temp
                 / (rho(i) * (v(i + 1) - v(i)) * (gamma - 1.0) * dt / m(i)
                    + 1.0);
        }
    }
}

void Solver_Lagrange1d::write_data() const {
    const std::filesystem::path& write_dir = io_.get_write_dir();
    std::ofstream fout(write_dir / (std::to_string(step) + ".csv"));
    fout << std::format("x;rho;v;P\n");
    for (int i{0}; i < nx; ++i) {
        fout << std::format(
            "{};{};{};{}\n",
            0.5 * (x(i + 1) + x(i)),
            rho(i),
            0.5 * (v(i + 1) + v(i)),
            P(i));
        fout << std::flush;
    }
}
