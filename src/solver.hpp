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
    inline void run();

private:
    Solver() {}

    friend Spec;

    inline static auto scenarios_dir = dash::cmake_dir() / "scenarios";
    Io::parsing_table_t get_parsing_table();
    bool parameters_loaded{false};
};

////////////////////////////////////////////////////////

template<typename Spec>
void Solver<Spec>::load_parameters_from_file(
    const std::filesystem::path& path) {
    static_cast<Spec&>(*this).load_parameters_from_file_impl(path);
}

template<typename Spec>
void Solver<Spec>::run() {
    static_cast<Spec&>(*this).run_impl();
}

template<typename Spec>
Io::parsing_table_t Solver<Spec>::get_parsing_table() {
    return static_cast<Spec&>(*this).get_parsing_table_impl();
}

#endif    // SOLVER_HPP
