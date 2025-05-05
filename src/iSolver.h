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
class Wall {};

template <typename Solver>
class InitCond {};

template <typename Solver>
class iSolver {
public:
    void Start() noexcept { static_cast<Solver*>(this)->Start(); }
    void ParseLine(const ScenParsingLine& line) noexcept
    {
        static_cast<Solver*>(this)->ParseLine(line);
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
    }
private:
    friend Solver;
    iSolver() {};
};

// template <typename T>
//     requires dash::IsCRTPBaseOf_v<iSolver, T>
// struct SolverType : dash::Type<T> {};
//
// template <typename T>
//     requires dash::IsCRTPBaseOf_v<iSolver, T>
// inline constexpr const void* qSolverUniqueID = &SolverType<T>::dummy_;

class Solver_Lagrange_1D;
using PolySolver = dash::VariantWrapper<Solver_Lagrange_1D>;

#endif // ISOLVER_H
