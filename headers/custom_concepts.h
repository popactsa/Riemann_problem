#ifndef CUSTOM_CONCEPTS_H
#define CUSTOM_CONCEPTS_H

namespace custom_concepts
{
	template <class ContainerType> 
	concept Container = requires(ContainerType a, const ContainerType b) 
	{
	    requires std::regular<ContainerType>;
	    requires std::swappable<ContainerType>;
	    requires std::destructible<typename ContainerType::value_type>;
	    requires std::same_as<typename ContainerType::reference, typename ContainerType::value_type &>;
	    requires std::same_as<typename ContainerType::const_reference, const typename ContainerType::value_type &>;
	    requires std::forward_iterator<typename ContainerType::iterator>;
	    requires std::forward_iterator<typename ContainerType::const_iterator>;
	    requires std::signed_integral<typename ContainerType::difference_type>;
	    requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename
	ContainerType::iterator>::difference_type>;
	    requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename
	ContainerType::const_iterator>::difference_type>;
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
	template<class T, std::size_t N>
	concept has_tuple_element =
		requires(T t) {
			typename std::tuple_element_t<N, std::remove_const_t<T>>;
			{ get<N>(t) } -> std::convertible_to<const std::tuple_element_t<N, T>&>;
		};

	template<class T>
	concept tuple_like = !std::is_reference_v<T> 
		&& requires(T t) { 
	    		typename std::tuple_size<T>::type; 
			requires std::derived_from<
				std::tuple_size<T>, 
				std::integral_constant<std::size_t, std::tuple_size_v<T>>
	    		>;
	  		} 
		&& []<std::size_t... N>(std::index_sequence<N...>) { 
			return (has_tuple_element<T, N> && ...); 
			}(std::make_index_sequence<std::tuple_size_v<T>>());
}

struct string_hash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const char *txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(std::string_view txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(const std::string &txt) const {
    return std::hash<std::string>{}(txt);
  }
};

#endif
