#include "Solver_Lagrange_1D.h"
#include "auxiliary_functions.h"

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
        for (std::size_t step = 1; step < nt; ++step) {
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
                status &= !write_file.empty();
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

void Solver_Lagrange_1D::ApplyBoundaryConditions_impl() noexcept {}

void Solver_Lagrange_1D::SolveStep_impl() noexcept {}

void Solver_Lagrange_1D::WriteData_impl() const noexcept {}

void Parse(InitCond<Solver_Lagrange_1D>* target, std::string_view str_value)
{
    // Add Enum-type parser!
    *target = {InitCond<Solver_Lagrange_1D>::parsing_table.at(str_value)};
}

void Parse(Viscosity<Solver_Lagrange_1D>* target, std::string_view str_value)
{
    *target = {Viscosity<Solver_Lagrange_1D>::parsing_table.at(str_value)};
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
