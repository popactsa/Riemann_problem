#ifndef ISOLVER_H
#define ISOLVER_H

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
private:
    friend Solver;
    iSolver() {};
};

class Solver_Lagrange_1D;
using PolySolver = std::variant<std::monostate, Solver_Lagrange_1D>;

#endif // ISOLVER_H
