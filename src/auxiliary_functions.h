#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

#include "error_handling.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <climits>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <variant>

namespace dash {

/**
 * @brief A helper struct, which performs action when destroyed
 */
template <typename F>
struct FinalAction {
    explicit FinalAction(F f) noexcept : act(f) {}
    ~FinalAction() noexcept { act(); }
    F act;
};

/**
 * @brief A helper factory-function for FinalAction
 *
 * @tparam F Type of functor executed when FinalAction destroyed
 * @param f Executed functor
 * @return Loaded FinalAction
 */
template <typename F>
[[nodiscard]] auto Finally(F f) noexcept
{
    return FinalAction{f};
}

/**
 * @brief Automatic compile-time created struct for tagging of types
 */
template <typename T>
struct Type {
    static constexpr char dummy_ = 0;
};

/**
 * @brief Compile-time tagging for type T
 *
 * @tparam T Tagged type
 */
template <typename T>
inline constexpr const void* qUniqueID = &Type<T>::dummy_;

/**
 * @brief Run-time tagging
 *
 * @tparam T Tagged type
 * @return A tag for type T
 */
template <typename T>
static std::size_t UniqueID() noexcept
{
    return reinterpret_cast<std::size_t>(qUniqueID<T>);
}

/**
 * @brief Factory-functor for variants. ONLY COMPILE-TIME TYPES, idc about other
 cases yet
 *
 * @tparam T Types in std::variant template
 */
template <typename... T>
class VariantFactory {
public:
    static constexpr std::size_t qTCount = sizeof...(T);
    static constexpr std::array<const void*, qTCount> qLookupTbl{
        dash::qUniqueID<T>...};
    using VariantT = std::variant<T...>;
    using ResultT = std::optional<VariantT>;
    template <std::size_t I>
    using AlternativeT = std::variant_alternative_t<I, VariantT>;

    /**
     * @brief Creates a std::optional w/ std::variant. Forwards a call to
    hidden emplace_variant func. Uses RVO
     * @tparam Args Type of arguments for an alternative constructor
     * @param type_idx UniqueID for alternative
     * @param args Arguments for an alternative constructor
     * @return Constructed std::optional
     */
    template <typename... Args>
    constexpr ResultT operator()(const void* type_idx, Args&&... args) noexcept
    {
        return emplace_variant(type_idx, std::forward<Args>(args)...);
    }
private:
    /**
     * @brief Recursive function which lookups for a given alternative to
     construct with passed arguments. Uses RVO
     *
     * @tparam Args Type of arguments for an alternative constructor
     * @tparam I A Call counter.
     * @param type_idx UniqueID for alternative
     * @param args Arguments for an alternative constructor
     * @return Constructed std::optional
     */
    template <std::size_t I = 0, typename... Args>
    static constexpr ResultT emplace_variant(const void* type_idx,
                                             Args&&... args) noexcept
    {
        if constexpr (I >= qTCount) {
            std::cerr << "I >= qTCount" << std::endl;
            return {};
        } else {
            if (qLookupTbl[I] == type_idx) {
                if constexpr (std::is_constructible_v<AlternativeT<I>,
                                                      Args...>) {
                    return ResultT(std::in_place, std::in_place_index<I>,
                                   std::forward<Args>(args)...);
                } else {
                    std::cerr
                        << "Can't construct a variant in VariantFactory"
                        << std::endl;
                    return {};
                }
            } else {
                return emplace_variant<I + 1>(type_idx,
                                              std::forward<Args>(args)...);
            }
        }
    }
};

/**
 * @brief Wrapper for a std::variant with given types T
 */
template <typename... T>
struct VariantWrapper {
    using UsedFactory = VariantFactory<T...>;
    constexpr VariantWrapper() = default;
    template <typename... Args>
    constexpr VariantWrapper(const void* type_idx, Args&&... args) noexcept :
        stored(UsedFactory{}(type_idx, std::forward<Args>(args)...))
    {
    }
    template <typename... Args>
    constexpr void emplace(const void* type_idx, Args&&... args) noexcept
    {
        stored = UsedFactory{}(type_idx, std::forward<Args>(args)...);
    }
    template <typename T_alt, typename... Args>
    constexpr void emplace(Args&&... args) noexcept
    {
        stored.emplace(std::in_place_type<T_alt>, std::forward<Args>(args)...);
    }
    UsedFactory::ResultT stored;
};

/**
 * @brief Compile-time map with linear search. If used with std::string as a key
 shows a better performance(at least with a reasonable amount of buckets) than
 std::unordered_map/std::map because of linear search optimizations(separates
 all keys into groups with their lengths thus eliminating unnecessary
 comparisons).
    Can be used with types with no-default constructors.
    Can be constructed with automaticaly deduced size
    Relies on aggregate initialization
 */

template <typename T1, typename T2, std::size_t Size>
struct TinyMap {
    /**
     * @brief Safely accesses stored values by key
     *
     * @param key A key for a table
     * @return A value if found, otherwise throws an exception
     */
    [[nodiscard]] constexpr const T2& at(const T1& key) const
    {
        const auto found =
            std::find_if(values_.cbegin(), values_.cend(),
                         [&key](const auto& it) { return it.first == key; });
        dash::Expect<dash::ErrorAction::qThrowing, std::range_error>(
            [&found, this] { return found != values_.cend(); }, "No such key");
        return found->second;
    }
    /**
     * @brief Accesses stored values by key
     *
     * @param key A key for a table
     * @return A value if found, otherwise throws an exception
     */
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

/**
 * @brief A compile-time constructed from a enum(idc about other cases)
 bitset(since std::bitset is not compile-time constructible in c++20(but it is
 in c++23)).
 */
template <typename E>
    requires(std::is_enum_v<E>)
class ConstexprBitsetFromE {
public:
    using UT =
        typename std::make_unsigned_t<typename std::underlying_type_t<E>>;
    constexpr ConstexprBitsetFromE(E init) noexcept
    {
        std::ranges::generate(bits_ | std::views::reverse,
                              [init_bits = static_cast<UT>(init)]() mutable {
                                  bool bit = init_bits & 1;
                                  init_bits >>= 1;
                                  return bit;
                              });
    }
    constexpr ConstexprBitsetFromE() = default;
    constexpr ConstexprBitsetFromE(const ConstexprBitsetFromE&) = default;
    constexpr ConstexprBitsetFromE&
    operator=(const ConstexprBitsetFromE&) = default;

    constexpr void reset() noexcept
    {
        std::ranges::generate(bits_, []() { return false; });
    }
    constexpr ConstexprBitsetFromE&
    operator|=(const ConstexprBitsetFromE& rhs) noexcept
    {
        for (std::size_t i = 0; i < bits_.size(); ++i) {
            bits_[i] |= rhs.bits_[i];
        }
        return *this;
    }
    [[nodiscard]]
    constexpr ConstexprBitsetFromE
    operator|(const ConstexprBitsetFromE& rhs) const noexcept
    {
        auto result = *this;
        result |= rhs;
        return result;
    }
    constexpr ConstexprBitsetFromE&
    operator&=(const ConstexprBitsetFromE& rhs) noexcept
    {
        for (std::size_t i = 0; i < bits_.size(); ++i) {
            bits_[i] &= rhs.bits_[i];
        }
        return *this;
    }
    [[nodiscard]]
    constexpr ConstexprBitsetFromE
    operator&(const ConstexprBitsetFromE& rhs) const noexcept
    {
        auto result = *this;
        result &= rhs;
        return result;
    }
    [[nodiscard]]
    constexpr bool operator==(const ConstexprBitsetFromE& rhs) const noexcept
    {
        return bits_ == rhs.bits_;
    }
    [[nodiscard]]
    constexpr bool operator!=(const ConstexprBitsetFromE& rhs) const noexcept
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
    std::array<bool, sizeof(E) * CHAR_BIT> bits_;
};

/**
 * @brief A class for flags based on enums. Provides support for binary
 operations.
 */
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
    ConstexprBitsetFromE<E> bits_;
};

/**
 * @brief A helper function to check if given std::string_view is an unsigned
 int explicit representation
 *
 * @param str Checked std::string_view
 * @return Is it?
 */
inline bool IsUnsignedInt(std::string_view str)
{
    static auto isDigit = [](auto c) {
        return c >= '0' && c <= '9';
    };
    return std::ranges::find_if_not(str, isDigit) == str.cend() ? true : false;
}
} // namespace dash

#endif // AUXILIARY_FUNCTIONS_H
