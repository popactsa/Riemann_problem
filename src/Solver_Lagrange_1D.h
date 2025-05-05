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
    static constexpr dash::TinyMap<std::string_view, E, 4> parsing_table{
        {{{"test1", E::qTest1},
          {"test2", E::qTest2},
          {"test3", E::qTest3},
          {"test4", E::qTest4}}}};
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
        *target_ = InitCond<Solver_Lagrange_1D>{
            InitCond<Solver_Lagrange_1D>::parsing_table.at(str_value)};
    }
private:
    InitCond<Solver_Lagrange_1D>* target_;
};

template <>
struct Viscosity<Solver_Lagrange_1D> {
    enum class E { qNone, qNeuman, qLatter, qLinear, qSum };
    static constexpr dash::TinyMap<std::string_view, E, 5> parsing_table{
        {{{"None", E::qNone},
          {"Neuman", E::qNeuman},
          {"Latter", E::qLatter},
          {"Sum", E::qSum},
          {"Linear", E::qLinear}}}};
    E value_;
};

template <>
class Parser<Viscosity<Solver_Lagrange_1D>>
    : ParserTypeLabel<ScenParsingLine::VariableType::qCommonType> {
public:
    constexpr Parser(Viscosity<Solver_Lagrange_1D>* target) noexcept :
        target_(target)
    {
    }
    void Parse(const ScenParsingLine& line, const std::size_t pos = 0)
    {
        Parse(line.get_common_arg_at(pos));
    }
    void Parse(std::string_view str_value)
    {
        *target_ = Viscosity<Solver_Lagrange_1D>{
            Viscosity<Solver_Lagrange_1D>::parsing_table.at(str_value)};
    }
private:
    using enum Viscosity<Solver_Lagrange_1D>::E;
    Viscosity<Solver_Lagrange_1D>* target_;
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    using PolyParsers =
        dash::VariantWrapper<Parser<double>,
                             Parser<std::size_t>,
                             Parser<std::string>,
                             Parser<bool>,
                             Parser<InitCond<Solver_Lagrange_1D>>,
                             Parser<Viscosity<Solver_Lagrange_1D>>,
                             Parser<std::array<Wall<Solver_Lagrange_1D>, 2>>>;

    Solver_Lagrange_1D() = default;
    void Start_impl() noexcept;
    void ParseLine_impl(const ScenParsingLine& line) noexcept;
    void InitializeDependent_impl() noexcept;
private:
    std::size_t nx;
    std::size_t nx_all;
    std::size_t nt;
    std::size_t nt_write;
    bool is_conservative;
    double x_end;
    double x_start;
    double CFL;
    double gamma;
    double mu0{2.0};
    std::string write_file;
    InitCond<Solver_Lagrange_1D> IC_preset;
    Viscosity<Solver_Lagrange_1D> viscosity{
        Viscosity<Solver_Lagrange_1D>::E::qNone};
    std::array<Wall<Solver_Lagrange_1D>, 2> walls;

    dash::TinyMap<std::string_view, PolyParsers, 13> parsing_table{
        {{{"x_start", PolyParsers{dash::qUniqueID<Parser<double>>, &x_start}},
          {"nx", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nx}},
          {"nt", PolyParsers{dash::qUniqueID<Parser<std::size_t>>, &nt}},
          {"InitCond",
           PolyParsers{dash::qUniqueID<Parser<InitCond<Solver_Lagrange_1D>>>,
                       &IC_preset}},
          {"gamma", PolyParsers{dash::qUniqueID<Parser<double>>, &gamma}},
          {"mu0", PolyParsers{dash::qUniqueID<Parser<double>>, &mu0}},
          {"viscosity",
           PolyParsers{dash::qUniqueID<Parser<Viscosity<Solver_Lagrange_1D>>>,
                       &viscosity}},
          {"is_conservative",
           PolyParsers{dash::qUniqueID<Parser<bool>>, &is_conservative}},
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
