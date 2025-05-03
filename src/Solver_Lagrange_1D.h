#ifndef SOLVER_LAGRANGE_1D_H
#define SOLVER_LAGRANGE_1D_H

#include "Parser.h"
#include "auxiliary_functions.h"
#include "iSolver.h"
#include <tuple>
#include <variant>

template <>
struct Wall<Solver_Lagrange_1D> : std::true_type {
    using PolyParsers = std::variant<std::monostate, Parser<double>>;
    double P;
    double v;
    dash::TinyMap<std::string_view, PolyParsers, 2> parsing_table{{"P", &P},
                                                                  {"v", &v}};
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    enum class Tests { qTest1, qTest2, qTest3, qTest4 };
    using PolyParsers =
        std::variant<std::monostate,
                     Parser<int>,
                     Parser<double>,
                     Parser<std::size_t>,
                     Parser<std::string>,
                     Parser<std::array<Wall<Solver_Lagrange_1D>, 2>>>;

    Solver_Lagrange_1D() = default;
    void Start() noexcept;
    void ParseLine(const ScenParsingLine& line) noexcept;
    void print()
    {
        std::cout
            << nx
            << ' '
            << nt
            << ' '
            << write_file
            << ' '
            << CFL
            << std::endl;
    }
private:
    std::size_t nx;
    std::size_t nt;
    std::size_t nt_write;
    double x_end;
    double x_start;
    double CFL;
    double gamma;
    std::string write_file;
    std::array<Wall<Solver_Lagrange_1D>, 2> walls;

    dash::TinyMap<std::string_view, PolyParsers, 9> parsing_table{
        {"x_start", &x_start},
        {"x_end", &x_end},
        {"CFL", &CFL},
        {"gamma", &gamma},
        {"nt_write", &nt_write},
        {"write_file", &write_file},
        {"nt", &nt},
        {"wall", &walls},
        {"nx", &nx}};
};

#endif // SOLVER_LAGRANGE_1D_H
