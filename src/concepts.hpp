#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP
#include <concepts>
#include <iterator>

namespace dash {
namespace concepts {
template<typename C>
concept Container = requires(C a, const C b) {
    requires std::regular<C>;
    requires std::swappable<C>;
    requires std::destructible<typename C::value_type>;
    requires std::same_as<typename C::reference, typename C::value_type&>;
    requires std::
        same_as<typename C::const_reference, const typename C::value_type&>;
    requires std::forward_iterator<typename C::iterator>;
    requires std::forward_iterator<typename C::const_iterator>;
    requires std::signed_integral<typename C::difference_type>;
    requires std::same_as<
        typename C::difference_type,
        typename std::iterator_traits<typename C::iterator>::difference_type>;
    requires std::same_as<
        typename C::difference_type,
        typename std::iterator_traits<
            typename C::const_iterator>::difference_type>;
    { a.begin() } -> std::same_as<typename C::iterator>;
    { a.end() } -> std::same_as<typename C::iterator>;
    { b.begin() } -> std::same_as<typename C::const_iterator>;
    { b.end() } -> std::same_as<typename C::const_iterator>;
    { a.cbegin() } -> std::same_as<typename C::const_iterator>;
    { a.cend() } -> std::same_as<typename C::const_iterator>;
    { a.size() } -> std::same_as<typename C::size_type>;
    { a.max_size() } -> std::same_as<typename C::size_type>;
    { a.empty() } -> std::same_as<bool>;
};

template<typename T, template<typename...> typename U>
inline constexpr bool is_instance_of_v = std::false_type{};
template<template<typename...> typename U, typename... Vs>
inline constexpr bool is_instance_of_v<U<Vs...>, U> = std::true_type{};
template<typename T, template<typename...> typename U>
concept IsInstanceOf = is_instance_of_v<T, U>;

// Convenient types
template<typename T>
struct RemoveCVRef: std::remove_cv<std::remove_reference_t<T>> {};

template<typename T>
using RemoveCVRefT = typename RemoveCVRef<T>::type;

template<typename T, typename U>
struct IsBaseOf: std::is_base_of<RemoveCVRefT<T>, RemoveCVRefT<U>> {};

// Hash-function to allow heterogeneous search with std::string_view
// in unordered containers
struct StringHash {
    using is_transparent = void;

    [[nodiscard]]
    size_t operator()(const char* txt) const {
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]]
    size_t operator()(std::string_view txt) const {
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]]
    size_t operator()(const std::string& txt) const {
        return std::hash<std::string>{}(txt);
    }
};

// CRTP support; allows using of comparison etc.
template<template<typename T> typename CRTP_Base, typename CRTP_Derived>
struct IsCRTPBaseOf: std::is_base_of<
                         RemoveCVRefT<CRTP_Base<RemoveCVRefT<CRTP_Derived>>>,
                         RemoveCVRefT<CRTP_Derived>> {};

template<template<typename T> typename CRTP_Base, typename CRTP_Derived>
concept IsCRTPBaseOf_v = IsCRTPBaseOf<CRTP_Base, CRTP_Derived>::value;
template<typename T, typename U>
concept IsBaseOf_v = IsBaseOf<T, U>::value;

// Overloaded lambda trick(CRTP technique)
template<typename... Ts>
struct overloaded: Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}    // namespace concepts
}    // namespace dash
#endif    // CONCEPTS_HPP
