#ifndef SOLVER_LAGRANGE_1D_H
#define SOLVER_LAGRANGE_1D_H

#include "Parser.h"
#include "VariableType.h"
#include "auxiliary_functions.h"
#include "iSolver.h"
#define ARMA_USE_SUPERLU 1
#include <armadillo>
#include <cstddef>
#include <tuple>
#include <variant>

template <>
struct Wall<Solver_Lagrange_1D> : VariableTypeTag<VariableType::qNamedType> {
    using PolyParsers = dash::VariantWrapper<double*>;
    double P;
    double v;
    dash::TinyMap<std::string, PolyParsers, 2> parsing_table{
        {{{"P", PolyParsers{dash::qUniqueID<double*>, &P}},
          {"v", PolyParsers{dash::qUniqueID<double*>, &v}}}}};
};

template <>
struct InitCond<Solver_Lagrange_1D>
    : VariableTypeTag<VariableType::qCommonType> {
    enum class E { qTest1, qTest2, qTest3, qTest4 };
    static constexpr dash::TinyMap<std::string_view, E, 4> parsing_table{
        {{{"test1", E::qTest1},
          {"test2", E::qTest2},
          {"test3", E::qTest3},
          {"test4", E::qTest4}}}};
    InitCond() = default;
    InitCond(E value) : value_{value} {}
    E value_;
};

void Parse(InitCond<Solver_Lagrange_1D>* target, std::string_view str_value);

template <>
struct Viscosity<Solver_Lagrange_1D>
    : VariableTypeTag<VariableType::qCommonType> {
    enum class E { qNone, qNeuman, qLatter, qLinear, qSum };
    static constexpr dash::TinyMap<std::string_view, E, 5> parsing_table{
        {{{"None", E::qNone},
          {"Neuman", E::qNeuman},
          {"Latter", E::qLatter},
          {"Sum", E::qSum},
          {"Linear", E::qLinear}}}};
    Viscosity() = default;
    Viscosity(E value) : value_{value} {}
    E value_;
};

void Parse(Viscosity<Solver_Lagrange_1D>* target, std::string_view str_value);

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    using PolyParsers =
        dash::VariantWrapper<double*,
                             std::size_t*,
                             std::string*,
                             bool*,
                             InitCond<Solver_Lagrange_1D>*,
                             Viscosity<Solver_Lagrange_1D>*,
                             std::array<Wall<Solver_Lagrange_1D>, 2>*>;

    Solver_Lagrange_1D() = default;
    void Start_impl() noexcept;
    void ParseLine_impl(const ScenParsingLine& line) noexcept;
    void InitializeDependent_impl() noexcept;
    void ApplyBoundaryConditions_impl() noexcept;
    void SolveStep_impl() noexcept;
    void SetInitialConditions_impl() noexcept;
    void GetTimeStep_impl() noexcept;
    void WriteData_impl() const noexcept;
    void CheckParameters_impl() const noexcept;
private:
    double dx;
    std::size_t nx_all;

    std::size_t nx;
    std::size_t nt;
    std::size_t nt_write;
    bool is_conservative;
    double x_start;
    double x_end;
    double CFL;
    double gamma;
    double mu0{2.0};
    std::string write_file;
    InitCond<Solver_Lagrange_1D> IC_preset;
    Viscosity<Solver_Lagrange_1D> viscosity{
        Viscosity<Solver_Lagrange_1D>::E::qNone};
    std::array<Wall<Solver_Lagrange_1D>, 2> walls;

    dash::TinyMap<std::string, PolyParsers, 13> parsing_table{
        {{{"nx", PolyParsers{dash::qUniqueID<std::size_t*>, &nx}},
          {"nt", PolyParsers{dash::qUniqueID<std::size_t*>, &nt}},
          {"nt_write", PolyParsers{dash::qUniqueID<std::size_t*>, &nt_write}},

          {"is_conservative",
           PolyParsers{dash::qUniqueID<bool*>, &is_conservative}},

          {"x_start", PolyParsers{dash::qUniqueID<double*>, &x_start}},
          {"x_end", PolyParsers{dash::qUniqueID<double*>, &x_end}},
          {"CFL", PolyParsers{dash::qUniqueID<double*>, &CFL}},
          {"gamma", PolyParsers{dash::qUniqueID<double*>, &gamma}},
          {"mu0", PolyParsers{dash::qUniqueID<double*>, &mu0}},

          {"write_file",
           PolyParsers{dash::qUniqueID<std::string*>, &write_file}},

          {"InitCond",
           PolyParsers{dash::qUniqueID<InitCond<Solver_Lagrange_1D>*>,
                       &IC_preset}},

          {"viscosity",
           PolyParsers{dash::qUniqueID<Viscosity<Solver_Lagrange_1D>*>,
                       &viscosity}},

          {"walls",
           PolyParsers{
               dash::qUniqueID<std::array<Wall<Solver_Lagrange_1D>, 2>*>,
               &walls}}}}};

    arma::vec P, rho, U, m;
    arma::vec v, x;
    arma::vec omega;
    double t = 0.0;
    double dt;
};

#endif // SOLVER_LAGRANGE_1D_H
