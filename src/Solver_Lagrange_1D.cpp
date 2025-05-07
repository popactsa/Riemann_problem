#include "Solver_Lagrange_1D.h"

void Parse(InitCond<Solver_Lagrange_1D>* target, std::string_view str_value)
{
    using enum InitCond<Solver_Lagrange_1D>::E;
    *target = InitCond<Solver_Lagrange_1D>{
        InitCond<Solver_Lagrange_1D>::parsing_table.at(str_value)};
}

void Parse(Viscosity<Solver_Lagrange_1D>* target, std::string_view str_value)
{
    using enum Viscosity<Solver_Lagrange_1D>::E;
    *target = Viscosity<Solver_Lagrange_1D>{
        Viscosity<Solver_Lagrange_1D>::parsing_table.at(str_value)};
}

void Solver_Lagrange_1D::Start_impl() noexcept {}

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
}
