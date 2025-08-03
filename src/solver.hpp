#ifndef SOLVER_HPP
#define SOLVER_HPP
#include <filesystem>
#include "auxiliary_functions.hpp"
#include "parser.hpp"

// CRTP-interface for solvers
// Redirection to _impl functions is used when solver-custom logic is used
template<typename Spec>
class Solver {
public:
    inline void load_parameters_from_file(const std::filesystem::path& path);
    inline void run(
        const std::filesystem::path& write_dir,
        std::size_t                  num_threads = 1);

private:
    Solver() {}

    inline static auto scenarios_dir = dash::cmake_dir() / "scenarios";

    friend Spec;
    bool                  parameters_loaded_{false};
    std::filesystem::path write_dir_;
    std::size_t           num_threads_;
};

////////////////////////////////////////////////////////

template<typename Spec>
void Solver<Spec>::load_parameters_from_file(
    const std::filesystem::path& path) {
    static_cast<Spec&>(*this).load_parameters_from_file_impl(path);
}

template<typename Spec>
void Solver<Spec>::run(
    const std::filesystem::path& write_dir,
    std::size_t                  num_threads) {
    num_threads_ = num_threads;
    if (write_dir_.is_relative()) {
        write_dir_ = dash::cmake_dir() / write_dir;
    } else {
        write_dir_ = write_dir;
    }
    if (!std::filesystem::exists(write_dir_)) {
        std::filesystem::create_directory(write_dir_);
    }
    static_cast<Spec&>(*this).run_impl();
}

#endif    // SOLVER_HPP
