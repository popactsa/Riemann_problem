#ifndef ISOLVER_H
#define ISOLVER_H

#include <filesystem>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

enum class Solvers { qUnknown, qLagrange1D, qWENO3_1D, qGodunov1D };
// Don't forget to add a solver name in io.cpp `SolversNameTypeTable`

template <typename S>
class Wall : std::false_type {};

template <typename Derived>
class iSolver {
    // CRTP-interface for a solvers
public:
    class ParameterInfo {
        // Container for info about parameter of `Derived` solver
    public:
        ParameterInfo() noexcept : type_("void"), ptr_(nullptr) {}
        ParameterInfo(std::string_view type, void* ptr) noexcept :
            type_(type), ptr_(ptr)
        {
        }
        ParameterInfo& operator=(const ParameterInfo& rhs) noexcept
        {
            if (&rhs == this) {
                return *this;
            }
            type_ = rhs.type_;
            ptr_ = rhs.ptr_;
            return *this;
        }
        void Reset() noexcept
        {
            type_ = "void";
            ptr_ = nullptr;
        }
        bool IsSet() const noexcept
        {
            if (type_ == "void" && ptr_ == nullptr) {
                return false;
            }
            return true;
        }
        void* ptr() const noexcept { return ptr_; }
        std::string_view type() const noexcept { return type_; }
    protected:
        std::string_view type_;
        void* ptr_;
    };

    inline std::unordered_map<std::string_view, const ParameterInfo>::iterator
    FindInParsingTable(std::string_view key) noexcept
    // @returns : A found const_iterator in `parsing_table`
    {
        return static_cast<Derived*>(this)->FindInParsingTable(key);
    };

    inline std::unordered_set<std::string_view>::iterator
    FindInGroupNames(std::string_view item) noexcept
    // @returns : A found const_iterator in `group_names`
    {
        return static_cast<Derived*>(this)->FindInGroupNames(item);
    };
private:
    friend Derived;
    iSolver() {};
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
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
private:
    std::size_t nx;
    std::size_t nt;
    std::size_t nt_write;
    double x_end;
    double x_start;
    double CFL;
    double gamma;
    std::string write_file;

    std::unordered_set<std::string_view> group_names{"Wall"};
    std::unordered_map<std::string_view,
                       const iSolver<Solver_Lagrange_1D>::ParameterInfo>
        parsing_table{{"x_start", {"double", &x_start}},
                      {"x_end", {"double", &x_end}},
                      {"CFL", {"double", &CFL}},
                      {"gamma", {"double", &gamma}},
                      {"nt_write", {"uint", &nt_write}},
                      {"write_file", {"string", &write_file}},
                      {"nt", {"uint", &nt}},
                      {"nx", {"uint", &nx}}};
};

template <>
class Wall<Solver_Lagrange_1D> : std::true_type {
public:
    double P;
    double v;

    std::unordered_map<std::string_view,
                       const iSolver<Solver_Lagrange_1D>::ParameterInfo>
        parsing_table{{"P", {"double", &P}}, {"v", {"double", &v}}};
};

#endif // ISOLVER_H
