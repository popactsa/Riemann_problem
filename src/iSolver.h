#ifndef ISOLVER_H
#define ISOLVER_H

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

enum class Solvers { qUnknown, qLagrange1D, qWENO3_1D, qGodunov1D };
// Don't forget to add a solver name in io.cpp `SolversNameTypeTable`

template <typename Derived>
class iSolver {
    // CRTP-interface for a solvers
public:
    class GroupOwner {
        // Container for info about parameter of `Derived` solver
    public:
        GroupOwner() noexcept : type_("void"), ptr_(nullptr) {}
        GroupOwner(std::string_view type, void* ptr) noexcept :
            type_(type), ptr_(ptr)
        {
        }
        GroupOwner& operator=(const GroupOwner& rhs) noexcept
        {
            if (&rhs == this) {
                return *this;
            }
            type_ = rhs.type_;
            ptr_ = rhs.ptr_;
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
    protected:
        std::string_view type_;
        void* ptr_;
    };

    inline std::unordered_map<std::string_view,
                              const GroupOwner>::const_iterator
    FindInParsingTable(std::string_view key) const noexcept
    // @returns : A found const_iterator in `parsing_table`
    {
        return static_cast<Derived*>(this)->FindInParsingTable(key);
    };

    inline std::unordered_set<std::string_view>::const_iterator
    FindInGroupNames(std::string_view item) const noexcept
    // @returns : A found const_iterator in `group_names`
    {
        return static_cast<Derived*>(this)->FindInGroupNames(item);
    };
private:
    friend Derived;
    iSolver() = delete;
};

class Solver_Lagrange_1D : public iSolver<Solver_Lagrange_1D> {
public:
    inline std::unordered_map<std::string_view,
                              const GroupOwner>::const_iterator
    FindInParsingTable(std::string_view key) const noexcept
    {
        return parsing_table.find(key);
    };
    inline std::unordered_set<std::string_view>::const_iterator
    FindInGroupNames(std::string_view item) const noexcept
    {
        return group_names.find(item);
    }
private:
    double P;
    std::size_t x_steps;
    std::size_t t_steps;
    std::unordered_set<std::string_view> group_names{"Wall"};
    std::unordered_map<std::string_view,
                       const iSolver<Solver_Lagrange_1D>::GroupOwner>
        parsing_table{{"P", {"double", &P}},
                      {"x_steps", {"uint", &x_steps}},
                      {"t_steps", {"uint", &t_steps}}};
};

#endif // ISOLVER_H
