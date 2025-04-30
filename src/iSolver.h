#ifndef ISOLVER_H
#define ISOLVER_H

#include "parsing_line.h"
#include <string_view>
#include <tuple>
#include <type_traits>
#include <variant>

enum class Solvers { qUnknown, qLagrange1D, qWENO3_1D, qGodunov1D };

template <typename Solver>
class Wall : std::false_type {};

template <typename Solver>
class iSolver {
public:
    void Start() noexcept { static_cast<Solver*>(this)->Start(); }
    void ParseLine(ScenParsingLine&& line) noexcept
    {
        static_cast<Solver*>(this)->ParseLine(line);
    }
private:
    friend Solver;
    iSolver() {};
};

class Solver_Lagrange_1D;
using PolySolver = std::variant<std::monostate, Solver_Lagrange_1D>;

#endif // ISOLVER_H
