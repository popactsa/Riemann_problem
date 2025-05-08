#ifndef ISOLVER_H
#define ISOLVER_H

#include "parsing_line.h"
#include <filesystem>
#include <fstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <variant>

enum class Solvers { qUnknown, qLagrange1D };

template <typename Solver>
struct WallType {};

template <typename Solver>
struct Wall {};

template <typename Solver>
struct InitCond {};

template <typename Solver>
struct Viscosity {};

template <typename Solver>
class iSolver {
public:
    void Start() noexcept { static_cast<Solver*>(this)->Start_impl(); }
    void ApplyBoundaryConditions() noexcept
    {
        return static_cast<Solver*>(this)->ApplyBoundaryConditions_impl();
    }
    void SolveStep() noexcept
    {
        return static_cast<Solver*>(this)->SolveStep_impl();
    }
    void SetInitialConditions() noexcept
    {
        return static_cast<Solver*>(this)->SetInitialConditions_impl();
    }
    void GetTimeStep() noexcept
    {
        return static_cast<Solver*>(this)->GetTimeStep_impl();
    }
    void WriteData() const noexcept
    {
        return static_cast<const Solver*>(this)->WriteData_impl();
    }
    void CheckParameters() const noexcept
    {
        return static_cast<const Solver*>(this)->CheckParameters_impl();
    }
    void ParseLine(const ScenParsingLine& line) noexcept
    {
        static_cast<Solver*>(this)->ParseLine_impl(line);
    }
    void InitializeDependent() noexcept
    {
        static_cast<Solver*>(this)->InitializeDependent_impl();
    }
    void ReadParameters(const std::filesystem::path& path) noexcept
    {
        std::ifstream fin(path);
        dash::Expect<dash::ErrorAction::qTerminating, std::exception>(
            [&fin]() { return fin.is_open(); }, "Can't open a file");
        ScenParsingLine line;
        std::string read;
        while (std::getline(fin, read)) {
            line.Load(read);
            if (line.get_head_spec_char()
                == ScenParsingLine::HeadSpecialChars::qCommentary) {
                continue;
            }
            ParseLine(line);
        }
        InitializeDependent();
    }
private:
    friend Solver;
    iSolver() {};
};

class Solver_Lagrange_1D;
using PolySolver = dash::VariantWrapper<Solver_Lagrange_1D>;

#endif // ISOLVER_H
