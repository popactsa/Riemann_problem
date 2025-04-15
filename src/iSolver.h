#ifndef ISOLVER_H
#define ISOLVER_H

#include <filesystem>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    void AssignEnumValue(std::string_view type,
                         const std::vector<std::string>& args,
                         void* ptr) noexcept
    {
        static_cast<Derived*>(this)->AssignEnumValue(args, ptr);
    }
private:
    friend Derived;
    iSolver() {};
};

#endif // ISOLVER_H
