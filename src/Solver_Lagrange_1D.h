#ifndef SOLVER_LAGRANGE_1D_H
#define SOLVER_LAGRANGE_1D_H

#include "Parser.h"
#include "auxiliary_functions.h"
#include "iSolver.h"
#include <cstddef>
#include <tuple>
#include <variant>

template <>
struct Wall<Solver_Lagrange_1D> {
    using PolyParsers = dash::VariantWrapper<Parser<double>>;
    double P;
    double v;
    dash::TinyMap<std::string_view, PolyParsers, 2> parsing_table{
        {{{"P", PolyParsers{dash::qUniqueID<Parser<double>>, &P}},
          {"v", PolyParsers{dash::qUniqueID<Parser<double>>, &v}}}}};
};

template <>
struct InitCond<Solver_Lagrange_1D> {
    enum class E { qTest1, qTest2, qTest3, qTest4 };
    E value_;
};

template <>
class Parser<InitCond<Solver_Lagrange_1D>>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
    constexpr Parser(InitCond<Solver_Lagrange_1D>* target) noexcept :
        target_(target)
    {
    }
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        std::cout << str_value << std::endl;
        *target_ = parsing_table.at(str_value);
    }
private:
    using enum InitCond<Solver_Lagrange_1D>::E;
    static constexpr dash::
        TinyMap<std::string_view, InitCond<Solver_Lagrange_1D>, 4>
            parsing_table{{{{"test1", {qTest1}},
                            {"test2", {qTest2}},
                            {"test3", {qTest3}},
                            {"test4", {qTest4}}}}};
    InitCond<Solver_Lagrange_1D>* target_;
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    using PolyParsers =
        dash::VariantWrapper<Parser<double>,
                             Parser<std::size_t>,
                             Parser<std::string>,
                             Parser<InitCond<Solver_Lagrange_1D>>,
                             Parser<std::array<Wall<Solver_Lagrange_1D>, 2>>>;

    Solver_Lagrange_1D() = default;
    void Start() noexcept;
    void ParseLine(const ScenParsingLine& line) noexcept;
    void print() {}
private:
    std::size_t nx;
    std::size_t nt;
    std::size_t nt_write;
    double x_end;
    double x_start;
    double CFL;
    double gamma;
    std::string write_file;
    InitCond<Solver_Lagrange_1D> IC_preset;
    std::array<Wall<Solver_Lagrange_1D>, 2> walls;

    dash::TinyMap<std::string_view, PolyParsers, 10> parsing_table{
        {{{"x_start", PolyParsers{dash::qUniqueID<Parser<double>>, &x_start}},
          {"nx", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nx}},
          {"nt", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nt}},
          {"InitCond",
           PolyParsers{dash::qUniqueID<Parser<InitCond<Solver_Lagrange_1D>>>,
                       &IC_preset}},
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
