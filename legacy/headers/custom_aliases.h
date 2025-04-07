#ifndef CUSTOM_ALIASES_H
#define CUSTOM_ALIASES_H

#include <string>
#include <string_view>
#include <unordered_map>

namespace custom {
namespace types {
// Convenient structs
template <class T>
struct RemoveCVRef : std::remove_cv<std::remove_reference_t<T>> {};

template <class T>
using RemoveCVRefT = typename RemoveCVRef<T>::type;

template <class T, class U>
struct IsBaseOf : std::is_base_of<RemoveCVRefT<T>, RemoveCVRefT<U>> {};

// CRTP-support(search concept)
template <template <typename T> class CRTP_Base, class CRTP_Derived>
struct IsCRTPBaseOf
    : std::is_base_of<RemoveCVRefT<CRTP_Base<RemoveCVRefT<CRTP_Derived>>>,
                      RemoveCVRefT<CRTP_Derived>> {};

// Hash-function to allow heterogenious search with std::string_view
// in unordered containers
struct StringHash {
    using is_transparent = void;
    [[nodiscard]] size_t operator()(const char *txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(std::string_view txt) const
    {
        return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(const std::string &txt) const
    {
        return std::hash<std::string>{}(txt);
    }
};
} // namespace types
namespace concepts {
// Containers with iterators, copy/move constructors, destructors
// i.e std::array, std::vector
template <class ContainerType>
concept Container = requires(ContainerType a, const ContainerType b) {
    requires std::regular<ContainerType>;
    requires std::swappable<ContainerType>;
    requires std::destructible<typename ContainerType::value_type>;
    requires std::same_as<typename ContainerType::reference,
                          typename ContainerType::value_type &>;
    requires std::same_as<typename ContainerType::const_reference,
                          const typename ContainerType::value_type &>;
    requires std::forward_iterator<typename ContainerType::iterator>;
    requires std::forward_iterator<typename ContainerType::const_iterator>;
    requires std::signed_integral<typename ContainerType::difference_type>;
    requires std::same_as<
        typename ContainerType::difference_type,
        typename std::iterator_traits<
            typename ContainerType::iterator>::difference_type>;
    requires std::same_as<
        typename ContainerType::difference_type,
        typename std::iterator_traits<
            typename ContainerType::const_iterator>::difference_type>;
    { a.begin() } -> std::same_as<typename ContainerType::iterator>;
    { a.end() } -> std::same_as<typename ContainerType::iterator>;
    { b.begin() } -> std::same_as<typename ContainerType::const_iterator>;
    { b.end() } -> std::same_as<typename ContainerType::const_iterator>;
    { a.cbegin() } -> std::same_as<typename ContainerType::const_iterator>;
    { a.cend() } -> std::same_as<typename ContainerType::const_iterator>;
    { a.size() } -> std::same_as<typename ContainerType::size_type>;
    { a.max_size() } -> std::same_as<typename ContainerType::size_type>;
    { a.empty() } -> std::same_as<bool>;
};

template <class T, std::size_t N>
concept HasTupleElement = requires(T t) {
    typename std::tuple_element_t<N, std::remove_const_t<T>>;
    { get<N>(t) } -> std::convertible_to<const std::tuple_element_t<N, T> &>;
};

template <class T>
concept TupleLike = !std::is_reference_v<T> && requires(T t) {
    typename std::tuple_size<T>::type;
    requires std::derived_from<
        std::tuple_size<T>,
        std::integral_constant<std::size_t, std::tuple_size_v<T>>>;
} && []<std::size_t... N>(std::index_sequence<N...>) {
    return (HasTupleElement<T, N> && ...);
}(std::make_index_sequence<std::tuple_size_v<T>>());

// CRTP support; allows to use comparison etc.
template <template <typename T> class CRTP_Base, class CRTP_Derived>
concept IsCRTPBaseOf = types::IsCRTPBaseOf<CRTP_Base, CRTP_Derived>::value;

template <class T, class U>
concept IsCRTPBaseOfValue = types::IsBaseOf<T, U>::value;
} // namespace concepts
} // namespace custom

// std::unordered_map w/ string_hash for heterogenious search
template <typename Key, typename Value>
using UnorderedMapStringHash =
    std::unordered_map<Key, Value, custom::types::StringHash, std::equal_to<>>;

#endif
