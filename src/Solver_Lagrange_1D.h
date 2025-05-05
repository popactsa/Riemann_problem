#ifndef SOLVER_LAGRANGE_1D_H
#define SOLVER_LAGRANGE_1D_H

#include "Parser.h"
#include "auxiliary_functions.h"
#include "iSolver.h"
#include <cstddef>
#include <tuple>
#include <variant>

template <>
struct Wall<Solver_Lagrange_1D> : std::true_type {
    using PolyParsers = dash::VariantWrapper<Parser<double>>;
    double P;
    double v;
    dash::TinyMap<std::string, PolyParsers, 2> parsing_table{
        {{{"P", PolyParsers{dash::qUniqueID<Parser<double>>, &P}},
          {"v", PolyParsers{dash::qUniqueID<Parser<double>>, &v}}}}};
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    enum class Tests { qTest1, qTest2, qTest3, qTest4 };
    using PolyParsers =
        dash::VariantWrapper<Parser<double>,
                             Parser<std::size_t>,
                             Parser<std::string>,
                             Parser<std::array<Wall<Solver_Lagrange_1D>, 2>>>;

    Solver_Lagrange_1D() = default;
    void Start() noexcept;
    void ParseLine(const ScenParsingLine& line) noexcept;
    void print() { std::cout << walls[0].P << ' ' << walls[0].v << std::endl; }
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

    dash::TinyMap<std::string, PolyParsers, 9> parsing_table{
        {{{"x_start", PolyParsers{dash::qUniqueID<Parser<double>>, &x_start}},
          {"nx", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nx}},
          {"nt", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nt}},
          {"gamma", PolyParsers{dash::qUniqueID<Parser<double>>, &gamma}},
          {"CFL", PolyParsers{dash::qUniqueID<Parser<double>>, &CFL}},
          {"write_file",
           PolyParsers{dash::qUniqueID<Parser<std::string>>, &write_file}},
          {"walls",
           PolyParsers{
               dash::qUniqueID<Parser<std::array<Wall<Solver_Lagrange_1D>, 2>>>,
               &walls}},
          {"nt_write",
           PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nt_write}},
          {"x_end", PolyParsers{dash::qUniqueID<Parser<double>>, &x_end}}}}};
};

#endif // SOLVER_LAGRANGE_1D_H
