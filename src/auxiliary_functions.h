#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace dash {
template <typename F>
struct FinalAction {
    // Performs an action `act` when destructor called
    explicit FinalAction(F f) : act(f) {}
    ~FinalAction() { act(); }
    F act;
};

template <typename F>
[[nodiscard]] auto Finally(F f)
// Wrapper for FinalAction
{
    return FinalAction{f};
}

template <typename T1, typename T2, std::size_t Size>
struct TinyMap {
    constexpr TinyMap(std::initializer_list<std::pair<T1, T2>> init_list)
    {
        if (init_list.size() != Size) {
            throw std::range_error("Initializer list has incorrect size");
        }
        std::copy(init_list.begin(), init_list.end(), values_.begin());
    }
    [[nodiscard]] constexpr T2& at(const T1& key) const
    {
        const auto found =
            std::find_if(values_.cbegin(), values_.cend(),
                         [&key](const auto& it) { return it.first == key; });
        if (found != values_.cend()) {
            return found->second;
        } else {
            throw std::range_error("No such key");
        }
    }
    [[nodiscard]] constexpr T2& at(const T1& key)
    {
        const auto found =
            std::find_if(values_.begin(), values_.end(),
                         [&key](const auto& it) { return it.first == key; });
        if (found != values_.end()) {
            return found->second;
        } else {
            throw std::range_error("No such key");
        }
    }
    std::array<std::pair<T1, T2>, Size> values_;
};

} // namespace dash

#endif // AUXILIARY_FUNCTIONS_H
