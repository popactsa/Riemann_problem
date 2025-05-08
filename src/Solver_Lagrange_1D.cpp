#include "Solver_Lagrange_1D.h"
#include "auxiliary_functions.h"
#include <cstdlib>

void Solver_Lagrange_1D::Start_impl() noexcept
{
    CheckParameters();
    {
        auto alloc_timer = dash::SetScopedTimer("Allocating memory");
        P.resize(nx_all);
        rho.resize(nx_all);
        U.resize(nx_all);
        m.resize(nx_all + 1);
        v.resize(nx_all + 1);
        x.resize(nx_all + 1);
        omega.resize(nx_all);
    }
    SetInitialConditions();
    {
        auto solve_timer = dash::SetScopedTimer("Solving");
        for (step = 1; step < nt; ++step) {
            ApplyBoundaryConditions();
            GetTimeStep();
            SolveStep();
            t += dt;
            if (step % nt_write == 0) {
                WriteData();
            }
        }
    }
}

void Solver_Lagrange_1D::CheckParameters_impl() const noexcept
{
    using exc = dash::InvalidParameterValue;
    try {
        // A little bit of overhead
        dash::Expect<dash::ErrorAction::qThrowing, exc>(
            [this] {
                bool status = true;
                status &= x_start < x_end;
                status &= nx > 1;
                for (const auto& wall : walls) {
                    status &= wall.P > 0.0;
                }
                status &= gamma > 0.0;
                status &= nt_write > 0;
                status &= nt >= nt_write;
                status &= CFL > 0.0;
                status &= mu0 > 0.0;
                status &= !write_dir.empty();
                return status;
            },
            "Bad Solver_Lagrange_1D parameter read");

    } catch (const exc& err) {
        std::terminate();
    }
}

void Solver_Lagrange_1D::SetInitialConditions_impl() noexcept
{
    double middle_plain = 0.5 * (x_start + x_end);
    for (std::size_t i = 0; i < nx_all + 1; ++i) {
        x(i) = x_start + (i - 1) * dx;
        switch (IC_preset.value_) {
            using enum InitCond<Solver_Lagrange_1D>::E;
        case qTest2: {
            v(i) = (i * dx <= middle_plain) ? -2.0 : 2.0;
            break;
        }
        case qTest1:
        case qTest3:
        case qTest4: {
            v(i) = 0.0;
            break;
        }
        }
    }
    for (std::size_t i = 0; i < nx_all; ++i) {
        switch (IC_preset.value_) {
            using enum InitCond<Solver_Lagrange_1D>::E;
        case qTest1: {
            if (i * dx <= middle_plain) {
                P(i) = 1.0;
                rho(i) = 1.0;
            } else {
                P(i) = 0.1;
                rho(i) = 0.125;
            }
            break;
        }
        case qTest2: {
            P(i) = 0.4;
            rho(i) = 1.0;
            break;
        }
        case qTest3: {
            if (i * dx <= middle_plain) {
                P(i) = 1000.0;
                rho(i) = 1.0;
            } else {
                P(i) = 0.01;
                rho(i) = 1.0;
            }
            break;
        }
        case qTest4: {
            if (i * dx <= middle_plain) {
                P(i) = 0.01;
                rho(i) = 1.0;
            } else {
                P(i) = 100.0;
                rho(i) = 1.0;
            }
        }
        }
        U(i) = P(i) / (gamma - 1.0) / rho(i);
        m(i) = rho(i) * (x(i + 1) - x(i));
    }
}

void Solver_Lagrange_1D::GetTimeStep_impl() noexcept
{
    double min_dt = 1.0e6;
    double dx, V, c, dt_temp;
    for (std::size_t i = 1; i < nx_all; ++i) {
        dx = x(i + 1) - x(i);
        V = 0.5 * (v(i + 1) + v(i));
        c = std::sqrt(gamma * P(i) / rho(i));
        dt_temp = CFL * dx / (c + std::fabs(V));
        if (dt_temp < min_dt) {
            min_dt = dt_temp;
        }
    }
    dt = min_dt;
}

void Solver_Lagrange_1D::ApplyBoundaryConditions_impl() noexcept
{
    for (std::size_t i = 0; i < 2; ++i) {
        std::size_t fict = i == 0 ? 0 : nx_all;
        switch (walls[i].type.value_) {
            using enum WallType<Solver_Lagrange_1D>::E;
        case qNoSlip: {
            v(fict) = i == 0 ? -v(1) : v(nx_all - 1);
            break;
        }
        case qFreeFlux: {
            v(fict) = i == 0 ? v(1) : v(nx_all - 1);
            break;
        }
        }
        fict = i == 0 ? 0 : nx_all - 1;
        rho(fict) = i == 0 ? rho(1) : rho(nx_all - 2);
        U(fict) = rho(fict);
    }
}

void Solver_Lagrange_1D::SolveStep_impl() noexcept
{
    arma::vec v_last = v;
    for (std::size_t i = 0; i < nx_all; ++i) {
        switch (viscosity.value_) {
            using enum Viscosity<Solver_Lagrange_1D>::E;
        case qNone: {
            omega(i) = 0.0;
            break;
        }
        case qNeuman: {
            omega(i) = -mu0 * rho(i) * std::pow(v(i + 1) - v(i), 2);
            omega(i) *= (v(i + 1) - v(i)) >= 0.0 ? 1.0 : -1.0;
            break;
        }
        case qLatter: {
            omega(i) = (v(i + 1) - v(i) < 0.0)
                           ? mu0 * rho(i) * std::pow((v(i + 1) - v(i)), 2)
                           : 0.0;
            break;
        }
        case qLinear: {
            omega(i) = mu0 * rho(i) * (v(i + 1) - v(i)) * m(i);
            break;
        }
        case qSum: {
            omega(i) = mu0
                       * rho(i)
                       * (v(i + 1) - v(i))
                       * m(i)
                       - mu0
                       * rho(i)
                       * std::pow(v(i + 1) - v(i), 2);
            omega(i) *= (v(i + 1) - v(i)) >= 0.0 ? 1.0 : -1.0;
            break;
        }
        }
    }
    for (std::size_t i = 2; i < nx_all - 1; ++i) {
        v(i) -= ((P(i) + omega(i)) - (P(i - 1) + omega(i - 1)))
                * dt
                / (0.5 * (m(i) + m(i - 1)));
    }
    // Recalculating grid
    for (std::size_t i = 0; i < nx_all + 1; ++i) {
        x(i) += v(i) * dt;
    }
    for (std::size_t i = 1; i < nx_all - 1; ++i) {
        double Pb_i = 0.5 * (P(i) + omega(i) + P(i - 1) + omega(i - 1));
        double Pb_ip1 = 0.5 * (P(i + 1) + omega(i + 1) + P(i) + omega(i));
        rho(i) /= 1.0 + rho(i) * (v(i + 1) - v(i)) * dt / m(i);
        double U_temp = U(i);
        if (is_conservative) {
            U(i) += -(v(i + 1) * Pb_ip1 - v(i) * Pb_i)
                    * dt
                    / m(i)
                    + std::pow(v_last(i + 1) + v_last(i), 2)
                    / 8.0
                    - std::pow(v(i + 1) + v(i), 2)
                    / 8.0;
        }
        if (!is_conservative || U(i) < 0.0) {
            U(i) = U_temp
                   / (rho(i)
                      * (v(i + 1) - v(i))
                      * (gamma - 1.0)
                      * dt
                      / m(i)
                      + 1.0);
        }
    }
    for (std::size_t i = 0; i < nx_all; ++i) {
        P(i) = rho(i) * (gamma - 1.0) * U(i);
    }
}

void Solver_Lagrange_1D::WriteData_impl() const noexcept
{
    namespace fs = std::filesystem;
    fs::path data_dir{"data"};
    if (!fs::exists(data_dir)) {
        fs::create_directory(data_dir);
    }
    if ((fs::status(data_dir).permissions() & fs::perms::owner_write)
        != fs::perms::owner_write) {
        std::terminate();
    }
    data_dir += '/';
    data_dir += write_dir;
    if (!fs::exists(data_dir)) {
        fs::create_directory(data_dir);
    }
    fs::path file_name = data_dir;
    file_name += '/';
    file_name += std::to_string(step + 1);
    file_name += ".csv";
    std::ofstream fout(file_name);
    char sep = '\t';
    fout << "x" << sep << "rho" << sep << "v" << sep << "P" << std::endl;
    for (std::size_t i = 1; i < nx_all - 1; ++i) {
        fout
            << (0.5 * (x(i + 1) + x(i)))
            << sep
            << rho(i)
            << sep
            << (0.5 * (v(i + 1) + v(i)))
            << sep
            << P(i)
            << std::endl;
    }
}

void Solver_Lagrange_1D::ShowResultAtStep(
    std::optional<std::size_t> show_step) const noexcept
{
    if (!show_step) {
        show_step = step;
    }
    if (*show_step > step || *show_step > nt) {
        return;
    }
    std::string path = "data/";
    path += write_dir;
    std::string sys_call = "python ";
    sys_call +=
        path + "/post.py " + path + '/' + std::to_string(*show_step) + ".csv";
    // std::cout << sys_call << std::endl;
    std::system(sys_call.c_str());
}

void Solver_Lagrange_1D::ParseLine_impl(const ScenParsingLine& line) noexcept
{
    try {
        auto found = parsing_table.at(line.get_name());
        if (found.stored) {
            std::visit(dash::overloaded{[&line](auto& arg) {
                           Parse(arg, line);
                       }},
                       *found.stored);
        }
    } catch (const std::range_error& exc) {
        // Skipping qCommentary and others..
    }
}

void Solver_Lagrange_1D::InitializeDependent_impl() noexcept
{
    nx_all = nx + 2;
    dx = static_cast<double>(x_end - x_start) / nx;
}
