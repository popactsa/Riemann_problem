#ifndef IO_AUXILIARY_H
#define IO_AUXILIARY_H

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <string_view>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <array>
#include <cmath>
#include <initializer_list>
#include <fstream>
#include <unordered_map>
#include <concepts>

#if __has_include(<format>)
	#include <format> // cluster4 doesn't provide it
#endif


#include "custom.h"
#include "error_handling.h"

extern struct winsize w;
extern const std::time_t start_time;
extern const std::chrono::time_point<std::chrono::system_clock> start_tick_time;

namespace io_constants
{
	 inline constexpr int asc_time_len = 25;
	 inline constexpr int max_file_name_size = 35;
	 inline constexpr int max_solver_name_size = 15;
}

enum class solver_types
{
	unknown,
	solver_Lagrange_1D,
};

static unordered_map_w_shs<std::string, solver_types> solver_types_table
{
	{"unknown", solver_types::unknown},
	{"Lagrange 1D", solver_types::solver_Lagrange_1D}
};

std::string time_to_string(const std::filesystem::file_time_type&) noexcept;

template<typename timeformat>
	requires std::common_with<timeformat, std::chrono::seconds>
inline int count_time_between(std::chrono::time_point<std::chrono::system_clock>& from, const std::chrono::time_point<std::chrono::system_clock>& to)
{
	int result = std::chrono::duration_cast<timeformat>(to - from).count();
	from = to;
	return result;
}

template<typename timeformat>
	requires std::common_with<timeformat, std::chrono::seconds>
inline int count_time_between_const(const std::chrono::time_point<std::chrono::system_clock>& from, const std::chrono::time_point<std::chrono::system_clock>& to)
{
	int result = std::chrono::duration_cast<timeformat>(to - from).count();
	return result;
}

template<typename F>
struct final_action
{
	explicit final_action(F f): act(f){}
	~final_action() {act(); }
	F act;
};

template<typename F>
[[nodiscard]] auto print_time_between_on_exit(F f)
{
	return final_action{f};
}

std::tuple<solver_types, std::filesystem::path> get_path_to_file_in_dir(const std::filesystem::path&, int, std::string_view);

inline std::tuple<solver_types, std::filesystem::path> get_path_to_file_in_dir(const std::filesystem::path &dir, int pos)
{
	return get_path_to_file_in_dir(dir, pos, "");
}

int print_filenames(const std::filesystem::path&, std::string_view) noexcept;

template<typename C>
	requires custom::concepts::Container<C> && (!std::assignable_from<typename C::value_type, std::string_view>)
constexpr void split_string(std::string_view init, C& result, const char sep) // if get rid of expect-assertions, function can be declared constexpr
{
	auto it = result.begin();
	std::size_t prev{0};
	for (std::size_t current(init.find(sep, prev)); current != init.npos; prev = current + 1, current = init.find(sep, prev))
	{
		expect<Error_action::throwing, std::length_error>(
			[&](){return (std::distance(it, const_cast<C::iterator>(result.end())) > 0); }, 
			(std::string("Size of init string is too small, must be = ") + std::to_string(std::size(result))).c_str()
		);
		*it++ = init.substr(prev, current - prev);
	}
	*it++ = init.substr(prev);
	expect<Error_action::throwing, std::length_error>(
		[&](){return (std::distance(it, const_cast<C::iterator>(result.end())) == 0); }, 
		(std::string("Size of init string is too large, must be = ") + std::to_string(std::size(result))).c_str()
	);
}

template<typename C>
	requires custom::concepts::Container<C> && (!std::assignable_from<typename C::value_type, std::string_view>)
constexpr void split_string(std::string_view init, C& result) // if get rid of expect-assertions, function can be declared constexpr
{
	split_string(init, result, ' ');
}

std::vector<std::string_view> split_string_view_to_v(std::string_view init, const char sep);

std::vector<std::string_view> split_string_view_to_v(std::string_view init); // if get rid of expect-assertions, function can be declared constexpr

inline int print_filenames(const std::filesystem::path& dir) noexcept
{
	return print_filenames(dir, "");
}

int choose_in_range(const int, const int);

bool check_rmod(const std::filesystem::path&) noexcept;

#ifdef __cpp_lib_format
inline void fmt_add_align(std::string& fmt, const std::string& align, const std::vector<int>& sizes) noexcept
{
	for (auto it : sizes)
	{
		fmt += "{:";
		fmt += align;
		fmt += std::to_string(it);
		fmt += "}";
	}
}

inline void fmt_add_align(std::string& fmt, const std::vector<std::pair<std::string, int>>& align_n_sizes) noexcept
{
	for (auto [align, size] : align_n_sizes) fmt_add_align(fmt, align, {size});
}

std::string get_format_by_left_side_impl(std::initializer_list<std::string_view> args) noexcept; // deprecate?
#endif

#endif
