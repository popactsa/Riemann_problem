#include "Solver_Lagrange_1D.h"

void Solver_Lagrange_1D::Start() noexcept {}

void Solver_Lagrange_1D::ParseLine(const ScenParsingLine& line) noexcept
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
