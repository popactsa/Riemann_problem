#ifndef SOLVER_HPP
#define SOLVER_HPP
#include <filesystem>
#include "auxiliary_functions.hpp"
#include "io.hpp"

// CRTP-interface for solvers
// Redirection to _impl functions is used when solver-custom logic is used
template<typename Spec>
class Solver {
public:
    inline void load_parameters_from_file(const std::filesystem::path& path);
    inline void run(std::size_t num_threads = 1);

private:
    Solver(Io& io): io_(io) {}

    inline static auto scenarios_dir = dash::cmake_dir() / "scenarios";

    friend Spec;
    Io&         io_;
    bool        parameters_loaded_{false};
    std::size_t num_threads_;
};

////////////////////////////////////////////////////////

template<typename Spec>
void Solver<Spec>::load_parameters_from_file(
    const std::filesystem::path& path) {
    static_cast<Spec&>(*this).load_parameters_from_file_impl(path);
}

template<typename Spec>
void Solver<Spec>::run(std::size_t num_threads) {
    num_threads_ = num_threads;
    static_cast<Spec&>(*this).run_impl();
}

#endif    // SOLVER_HPP
