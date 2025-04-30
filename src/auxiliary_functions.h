#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

#include "error_handling.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <climits>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ranges>
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
        dash::Expect<dash::ErrorAction::qThrowing, std::range_error>(
            [&found, this] { return found != values_.cend(); }, "No such key");
        return found->second;
    }
    [[nodiscard]] constexpr T2& at(const T1& key)
    {
        const auto found =
            std::find_if(values_.begin(), values_.end(),
                         [&key](const auto& it) { return it.first == key; });
        dash::Expect<dash::ErrorAction::qThrowing, std::range_error>(
            [&found, this] { return found != values_.end(); }, "No such key");
        return found->second;
    }
    std::array<std::pair<T1, T2>, Size> values_;
};

template <typename T>
class ConstexprBitsetFromT {
public:
    using UT =
        typename std::make_unsigned_t<typename std::underlying_type_t<T>>;
    constexpr ConstexprBitsetFromT(T init) noexcept
    {
        std::ranges::generate(bits_ | std::views::reverse,
                              [init_bits = static_cast<UT>(init)]() mutable {
                                  bool bit = init_bits & 1;
                                  init_bits >>= 1;
                                  return bit;
                              });
    }
    constexpr ConstexprBitsetFromT() = default;
    constexpr ConstexprBitsetFromT(const ConstexprBitsetFromT&) = default;
    constexpr ConstexprBitsetFromT&
    operator=(const ConstexprBitsetFromT&) = default;

    constexpr void reset() noexcept
    {
        std::ranges::generate(bits_, []() { return false; });
    }
    constexpr ConstexprBitsetFromT&
    operator|=(const ConstexprBitsetFromT& rhs) noexcept
    {
        for (std::size_t i = 0; i < bits_.size(); ++i) {
            bits_[i] |= rhs.bits_[i];
        }
        return *this;
    }
    [[nodiscard]]
    constexpr ConstexprBitsetFromT
    operator|(const ConstexprBitsetFromT& rhs) const noexcept
    {
        auto result = *this;
        result |= rhs;
        return result;
    }
    constexpr ConstexprBitsetFromT&
    operator&=(const ConstexprBitsetFromT& rhs) noexcept
    {
        for (std::size_t i = 0; i < bits_.size(); ++i) {
            bits_[i] &= rhs.bits_[i];
        }
        return *this;
    }
    [[nodiscard]]
    constexpr ConstexprBitsetFromT
    operator&(const ConstexprBitsetFromT& rhs) const noexcept
    {
        auto result = *this;
        result &= rhs;
        return result;
    }
    [[nodiscard]]
    constexpr bool operator==(const ConstexprBitsetFromT& rhs) const noexcept
    {
        return bits_ == rhs.bits_;
    }
    [[nodiscard]]
    constexpr bool operator!=(const ConstexprBitsetFromT& rhs) const noexcept
    {
        return !(*this == rhs);
    }
    [[nodiscard]]
    constexpr bool any() const noexcept
    {
        return std::ranges::find(bits_, true) != bits_.cend() ? true : false;
    }
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return any();
    }
private:
    std::array<bool, sizeof(T) * CHAR_BIT> bits_;
};

template <typename E>
    requires std::is_enum_v<E>
class Flag {
public:
    explicit constexpr Flag(E rhs) noexcept : bits_(rhs) {}
    constexpr Flag() = default;
    constexpr Flag(const Flag&) = default;
    constexpr Flag& operator=(const Flag&) = default;

    constexpr void reset() noexcept { bits_.reset(); }
    constexpr Flag& operator|=(const Flag& rhs) noexcept
    {
        bits_ |= rhs.bits_;
        return *this;
    }
    [[nodiscard]]
    constexpr Flag operator|(const Flag& rhs) const noexcept
    {
        Flag result = *this;
        result |= rhs;
        return result;
    }
    constexpr Flag& operator&=(const Flag& rhs) noexcept
    {
        bits_ &= rhs.bits_;
        return *this;
    }
    [[nodiscard]]
    constexpr Flag operator&(const Flag& rhs) const noexcept
    {
        Flag result = *this;
        result &= rhs;
        return result;
    }

    [[nodiscard]]
    constexpr bool operator==(Flag rhs) const noexcept
    {
        return bits_ == rhs.bits_;
    }

    [[nodiscard]]
    constexpr bool operator!=(Flag rhs) const noexcept
    {
        return bits_ != rhs.bits_;
    }
    [[nodiscard]]
    constexpr bool any() const noexcept
    {
        return bits_.any();
    }
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return any();
    }
private:
    ConstexprBitsetFromT<E> bits_;
};

inline bool IsUnsignedInt(std::string_view str)
{
    static auto isDigit = [](auto c) {
        return c >= '0' && c <= '9';
    };
    return std::ranges::find_if_not(str, isDigit) == str.cend() ? true : false;
}
} // namespace dash

#endif // AUXILIARY_FUNCTIONS_H
