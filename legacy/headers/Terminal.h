#ifndef TERMINAL_H
#define TERMINAL_H

#include "custom_aliases.h"
#include <chrono>
#include <cmath>
#include <concepts>
#include <ctime>
#include <filesystem>
#include <string>
#include <string_view>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#if __has_include(<format>)
	#include <format> // cluster4 doesn't provide it
#endif // __has_include(<format>)

extern struct winsize w;

class Terminal
{
private:
	Terminal(){}
public:
};

/*class Terminal*/
/*{*/
/*private:*/
/*	UnorderedMapStringHash<std::string_view, std::filesystem::path> directories_*/
/*	{*/
/*		{std::string_view("scenarios"), "../scenarios"},*/
/*		{std::string_view("data"), "../data"},*/
/*		{std::string_view("debug"), "../debug"}*/
/*	};*/
/*	std::filesystem::path get_path_to_file_in_dir(const std::filesystem::path& dir, int item, std::string_view postfix) const noexcept;*/
/*	inline std::filesystem::path get_path_to_file_in_dir(const std::filesystem::path &dir, int pos) const noexcept;*/
/*	bool check_rmod(const std::filesystem::path&) const noexcept;*/
/*public:*/
/*	enum*/
/*	{*/
/*		time_string_length = 25,*/
/*		max_file_name_length = 35,*/
/*		max_solver_name_length = 15*/
/*	};*/
/*	Terminal() = default;	*/
/*	std::filesystem::path& choose_file_in_directory(std::string_view directory_pseudonym, std::string_view postfix) const noexcept;*/
/*	inline std::filesystem::path& choose_file_in_directory(std::string_view directory_pseudonym) const noexcept;*/
/*	template<typename Time_unit = std::chrono::milliseconds>*/
/*		requires std::common_with<Time_unit, std::chrono::seconds>*/
/*	void print_time_passed(std::chrono::time_point<std::chrono::system_clock>& previous_time_point) const noexcept;*/
/**/
/*#ifdef __cpp_lib_format*/
/*	static inline void fmt_add_align(std::string& fmt, const std::string& align, const std::vector<int>& sizes) noexcept;*/
/*	static inline void fmt_add_align(std::string& fmt, const std::vector<std::pair<std::string, int>>& align_n_sizes) noexcept;*/
/*#endif //__cpp_lib_format*/
/*};*/

#endif // TERMINAL_H
