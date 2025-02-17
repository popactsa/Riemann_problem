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

	template <class T> 
	struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {}; 
	 
	template <class T> 
	using remove_cvref_t = typename remove_cvref<T>::type; 
	 
	template <class T, class U> 
	struct is_base_of : std::is_base_of<remove_cvref_t<T>, remove_cvref_t<U>> {}; 
	 
	template <class T, class U> 
	concept is_base_of_v = is_base_of<T, U>::value; 
	 
	template <template<typename T> class CRTP_Base, class CRTP_Derived> 
	struct is_crtp_base_of : std::is_base_of<remove_cvref_t<CRTP_Base<remove_cvref_t<CRTP_Derived>>>, remove_cvref_t<CRTP_Derived>> {}; 
	 
	template <template<typename T> class CRTP_Base, class CRTP_Derived> 
	concept is_crtp_base_of_v = is_crtp_base_of<CRTP_Base, CRTP_Derived>::value;
}


// std::unordered_map w/ string_hash for heterogenious search
namespace custom_types
{
	struct string_hash { // to allow heterogenious search for unordered containers
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
}

#endif
