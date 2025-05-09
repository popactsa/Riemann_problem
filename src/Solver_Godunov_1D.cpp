#include "Solver_Godunov_1D.h"

#include "auxiliary_functions.h"
#include <cstdlib>

void Solver_Godunov_1D::Start_impl() noexcept
{
    CheckParameters();
    {
        auto alloc_timer = dash::SetScopedTimer("Allocating memory");
        P.resize(nx_all);
        rho.resize(nx_all);
        rho_u.resize(nx_all);
        rho_e.resize(nx_all);
        x.resize(nx_all + 1);
        F_m.resize(nx_all + 1);
        F_imp.resize(nx_all + 1);
        F_e.resize(nx_all + 1);
    }
    SetInitialConditions();
    {
        namespace fs = std::filesystem;
        std::string path = "data/";
        path += write_dir;
        for (auto const& dir_entry : fs::directory_iterator{path}) {
            if (dir_entry.path().string().ends_with(".csv")) {
                fs::remove(dir_entry.path());
            }
        }
    }
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

void Solver_Godunov_1D::CheckParameters_impl() const noexcept
{
    using exc = dash::InvalidParameterValue;
    try {
        // A little bit of overhead
        dash::Expect<dash::ErrorAction::qThrowing, exc>(
            [this] {
                bool status = true;
                status &= x_start < x_end;
                status &= nx > 1;
                status &= gamma > 0.0;
                status &= nt_write > 0;
                status &= nt >= nt_write;
                status &= CFL > 0.0;
                status &= mu0 > 0.0;
                status &= !write_dir.empty();
                return status;
            },
            "Bad Solver_Godunov_1D parameter read");

    } catch (const exc& err) {
        std::terminate();
    }
}

void Solver_Godunov_1D::SetInitialConditions_impl() noexcept
{
    double middle_plain = 0.5 * (x_start + x_end);
    for (std::size_t i = 0; i < nx_all; ++i) {
        double v;
        bool is_left_side = i * dx <= middle_plain;
        switch (IC_preset.value_) {
            using enum InitCond<Solver_Godunov_1D>::E;
        case qTest1: {
            if (is_left_side) {
                rho(i) = 1.0;
                P(i) = 1.0;
            } else {
                rho(i) = 0.125;
                P(i) = 0.1;
            }
            v = 0.0;
            break;
        }
        case qTest2: {
            rho(i) = 1.0;
            P(i) = 0.4;
            v = is_left_side ? -2.0 : 2.0;
            break;
        }
        case qTest3: {
            rho(i) = 1.0;
            P(i) = is_left_side ? 1000.0 : 0.01;
            v = 0.0;
            break;
        }
        case qTest4: {
            rho(i) = 3.0 + std::sin((i - 0.5) * dx * 10.0);
            P(i) = 0.1;
            v = 1.0;
            break;
        }
        }
        x(i) = (i - 0.5) * dx;
        rho_e(i) = rho(i) * (std::pow(v, 2) / 2.0) + P(i) / (gamma - 1.0);
        rho_u(i) = rho(i) * v;
    }
}

void Solver_Godunov_1D::GetTimeStep_impl() noexcept
{
    double min_dt = 1.0e6;
    double u, c, dt_temp;
    for (std::size_t i = 1; i < nx_all; ++i) {
        u = rho_u(i) / rho(i);
        c = rho(i) != 0.0 ? std::sqrt(gamma * P(i) / rho(i)) : 0.0;
        dt_temp = CFL * dx / (c + std::fabs(u));
        if (dt_temp < min_dt) {
            min_dt = dt_temp;
        }
    }
    dt = min_dt;
}

void Solver_Godunov_1D::ApplyBoundaryConditions_impl() noexcept
{
    rho(0) = rho(1);
    rho(nx_all - 1) = rho(nx_all - 2);

    rho_e(0) = rho_e(1);
    rho_e(nx_all - 1) = rho(nx_all - 2);

    P(0) = P(1);
    P(nx_all - 1) = P(nx_all - 2);

    for (std::size_t i = 0; i < 2; ++i) {
        std::size_t i_temp = i == 0 ? nx_all : 0;
        int sign = i == 0 ? -1 : 1;
        std::size_t f_i_temp = i == 0 ? nx : 0;
        switch (walls[i].type.value_) {
            using enum WallType<Solver_Godunov_1D>::E;
        case qNoSlip: {
            rho_u(i_temp + sign) = -rho_u(i_temp);
            F_m(f_i_temp) = walls[i].v * rho(i_temp);
            F_imp(f_i_temp) =
                std::pow(walls[i].v, 2.0) * rho(i_temp) + P(i_temp);
            F_e(f_i_temp) =
                (rho_e(i_temp) + P(i_temp)) * walls[i].v * rho(i_temp);
            break;
        }
        case qFreeFlux: {
            rho_u(i_temp + sign) = -rho_u(i_temp);
            F_m(f_i_temp) = rho_u(i_temp);
            F_imp(f_i_temp) =
                std::pow(rho_u(i_temp), 2.0) / rho(i_temp) + P(i_temp);
            F_e(f_i_temp) =
                (rho_e(i_temp) + P(i_temp)) * rho_u(i_temp) / rho(i_temp);
            break;
        }
        }
    }
}

void Solver_Godunov_1D::SolveStep_impl() noexcept
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

void Solver_Godunov_1D::WriteData_impl() const noexcept
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

void Solver_Godunov_1D::ShowResultAtStep(
    std::optional<std::size_t> show_step) const noexcept
{
    if (!show_step) {
        show_step = step;
    }
    if (*show_step > step || *show_step > nt) {
        return;
    }
    std::string sys_call = "python ";
    std::string path = "data/post_Solver_Lagrange_1D.py ";
    sys_call +=
        path + "data/" + write_dir + '/' + std::to_string(*show_step) + ".csv";
    // std::cout << sys_call << std::endl;
    std::system(sys_call.c_str());
}

void Solver_Godunov_1D::ParseLine_impl(const ScenParsingLine& line) noexcept
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

void Solver_Godunov_1D::InitializeDependent_impl() noexcept
{
    nx_all = nx + 2;
    dx = static_cast<double>(x_end - x_start) / nx;
}
