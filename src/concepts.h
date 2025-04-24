#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <iterator>

namespace dash {
template <class C>
concept Container = requires(C a, const C b) {
    // Containers with iterators, copy/move constructors, destructors
    // i.e std::array, std::vector
    requires std::regular<C>;
    requires std::swappable<C>;
    requires std::destructible<typename C::value_type>;
    requires std::same_as<typename C::reference, typename C::value_type&>;
    requires std::same_as<typename C::const_reference,
                          const typename C::value_type&>;
    requires std::forward_iterator<typename C::iterator>;
    requires std::forward_iterator<typename C::const_iterator>;
    requires std::signed_integral<typename C::difference_type>;
    requires std::same_as<
        typename C::difference_type,
        typename std::iterator_traits<typename C::iterator>::difference_type>;
    requires std::same_as<typename C::difference_type,
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

template <class C>
concept HasBackInserterContainer = requires(C a, const C b) {
    requires Container<C>;
    { std::back_inserter(a) };
};

// Convenient structs
template <class T>
struct RemoveCVRef : std::remove_cv<std::remove_reference_t<T>> {};

template <class T>
using RemoveCVRefT = typename RemoveCVRef<T>::type;

template <class T, class U>
struct IsBaseOf : std::is_base_of<RemoveCVRefT<T>, RemoveCVRefT<U>> {};

// CRTP support; allows to use comparison etc.
template <template <typename T> class CRTP_Base, class CRTP_Derived>
struct IsCRTPBaseOf
    : std::is_base_of<RemoveCVRefT<CRTP_Base<RemoveCVRefT<CRTP_Derived>>>,
                      RemoveCVRefT<CRTP_Derived>> {};
template <template <typename T> class CRTP_Base, class CRTP_Derived>
concept IsCRTPBaseOfValue = IsCRTPBaseOf<CRTP_Base, CRTP_Derived>::value;

template <class T, class U>
concept IsBaseOfValue = IsBaseOf<T, U>::value;

// Hash-function to allow heterogenious search with std::string_view
// in unordered containers
struct StringHash {
    using is_transparent = void;
    [[nodiscard]] size_t operator()(const char* txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(std::string_view txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(const std::string& txt) const
    {
        return std::hash<std::string>{}(txt);
    }
};

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace dash

#endif // CONCEPTS_H
