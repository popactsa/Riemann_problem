#include "Solver_Lagrange_1D.h"

void Solver_Lagrange_1D::Start_impl() noexcept {}

void Solver_Lagrange_1D::ParseLine_impl(const ScenParsingLine& line) noexcept
{
    try {
        auto found = parsing_table.at(line.get_name());
        if (found.stored) {
            std::visit(dash::overloaded{[&line](auto& arg) {
                           arg.Parse(line);
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
}
