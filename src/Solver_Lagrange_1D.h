#ifndef SOLVER_LAGRANGE_1D
#define SOLVER_LAGRANGE_1D

#include "iSolver.h"
#include <array>

class Solver_Lagrange_1D;

template <>
class Wall<Solver_Lagrange_1D> : std::true_type {
public:
    double P;
    double v;

    inline std::unordered_map<
        std::string_view,
        const iSolver<Solver_Lagrange_1D>::ParameterInfo>::iterator
    FindInParsingTable(std::string_view key) noexcept
    {
        return parsing_table.find(key);
    };
    std::unordered_map<std::string_view,
                       const iSolver<Solver_Lagrange_1D>::ParameterInfo>
        parsing_table{{"P", {"double", &P}}, {"v", {"double", &v}}};
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    enum class Tests { qTest1, qTest2, qTest3, qTest4 };

    Solver_Lagrange_1D() = default;
    inline std::unordered_map<std::string_view, const ParameterInfo>::iterator
    FindInParsingTable(std::string_view key) noexcept
    {
        return parsing_table.find(key);
    };
    inline std::unordered_set<std::string_view>::iterator
    FindInGroupNames(std::string_view item) noexcept
    {
        return group_names.find(item);
    }
    void AssignEnumValue(std::string_view type,
                         const std::vector<std::string>& args,
                         void* ptr) noexcept;
    Wall<Solver_Lagrange_1D> wall;
private:
    std::size_t nx;
    std::size_t nt;
    std::size_t nt_write;
    double x_end;
    double x_start;
    double CFL;
    double gamma;
    std::string write_file;
    Tests test;

    std::unordered_set<std::string_view> group_names{"wall"};
    std::unordered_map<std::string_view,
                       const iSolver<Solver_Lagrange_1D>::ParameterInfo>
        parsing_table{{"x_start", {"double", &x_start}},
                      {"x_end", {"double", &x_end}},
                      {"CFL", {"double", &CFL}},
                      {"gamma", {"double", &gamma}},
                      {"nt_write", {"uint", &nt_write}},
                      {"write_file", {"string", &write_file}},
                      {"nt", {"uint", &nt}},
                      {"test", {"Tests", &test}},
                      {"wall", {"Wall", &wall}},
                      {"nx", {"uint", &nx}}};
};

#endif // SOLVER_LAGRANGE_1D
