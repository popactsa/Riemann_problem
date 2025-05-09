#ifndef SOLVER_GODUNOV_1D_H
#define SOLVER_GODUNOV_1D_H

#include "Parser.h"
#include "VariableType.h"
#include "auxiliary_functions.h"
#include "iSolver.h"
#define ARMA_USE_SUPERLU 1
#include <armadillo>
#include <cstddef>

struct ReconstructionType {
    enum class E { qGodunov, qKolgan72, qKolgan75, qOsher84 };
    static constexpr dash::TinyMap<std::string_view, E, 4> parsing_table{
        {{{"Godunov", E::qGodunov},
          {"Kolgan72", E::qKolgan72},
          {"Kolgan72", E::qKolgan75},
          {"Kolgan72", E::qOsher84}}}};
    ReconstructionType() = default;
    ReconstructionType(E value) : value_{value} {}
    E value_;
};

template <>
struct WallType<Solver_Godunov_1D> : VariableTypeTag<VariableType::qEnumType> {
    enum class E { qNoSlip, qFreeFlux };
    static constexpr dash::TinyMap<std::string_view, E, 2> parsing_table{
        {{{"NoSlip", E::qNoSlip}, {"FreeFlux", E::qFreeFlux}}}};
    WallType() = default;
    WallType(E value) : value_{value} {}
    E value_;
};

template <>
struct Wall<Solver_Godunov_1D> : VariableTypeTag<VariableType::qNamedType> {
    using PolyParsers =
        dash::VariantWrapper<double*, WallType<Solver_Godunov_1D>*>;
    WallType<Solver_Godunov_1D> type;
    double v;
    dash::TinyMap<std::string, PolyParsers, 2> parsing_table{{{

        {"type",
         PolyParsers{dash::qUniqueID<WallType<Solver_Godunov_1D>*>, &type}},
        {"v", PolyParsers{dash::qUniqueID<double*>, &v}}}}};
};

template <>
struct InitCond<Solver_Godunov_1D> : VariableTypeTag<VariableType::qEnumType> {
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

class Solver_Godunov_1D : public iSolver<Solver_Godunov_1D> {
public:
    using PolyParsers =
        dash::VariantWrapper<double*,
                             std::size_t*,
                             std::string*,
                             bool*,
                             InitCond<Solver_Godunov_1D>*,
                             std::array<Wall<Solver_Godunov_1D>, 2>*>;

    Solver_Godunov_1D() = default;
    void Start_impl() noexcept;
    void ParseLine_impl(const ScenParsingLine& line) noexcept;
    void InitializeDependent_impl() noexcept;
    void ApplyBoundaryConditions_impl() noexcept;
    void SolveStep_impl() noexcept;
    void SetInitialConditions_impl() noexcept;
    void GetTimeStep_impl() noexcept;
    void WriteData_impl() const noexcept;
    void CheckParameters_impl() const noexcept;
    void ShowResultAtStep(std::optional<std::size_t> show_step) const noexcept;
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
    std::string write_dir = "Solver_Godunov_1D";
    InitCond<Solver_Godunov_1D> IC_preset;
    std::array<Wall<Solver_Godunov_1D>, 2> walls;

    dash::TinyMap<std::string, PolyParsers, 12> parsing_table{
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
           PolyParsers{dash::qUniqueID<std::string*>, &write_dir}},

          {"InitCond",
           PolyParsers{dash::qUniqueID<InitCond<Solver_Godunov_1D>*>,
                       &IC_preset}},

          {"walls",
           PolyParsers{dash::qUniqueID<std::array<Wall<Solver_Godunov_1D>, 2>*>,
                       &walls}}}}};

    arma::vec P, rho, rho_u, rho_e;
    arma::vec x;
    arma::vec omega;
    double t = 0.0;
    double dt;
    std::size_t step = 0;
    arma::vec F_m, F_imp, F_e;
};

#endif // SOLVER_GODUNOV_1D_H
