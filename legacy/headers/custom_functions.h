#ifndef CUSTOM_FUNCTIONS_H
#define CUSTOM_FUNCTIONS_H

#include <filesystem>
#include <string_view>

#include "custom_aliases.h"

namespace custom
{
	template<typename C>
		requires concepts::Container<C> && (!std::assignable_from<typename C::value_type, std::string_view>)
	static constexpr void SplitStringView(std::string_view init, C& result, const char sep);

	template<typename C>
		requires concepts::Container<C> && (!std::assignable_from<typename C::value_type, std::string_view>)
	static constexpr void SplitStringView(std::string_view init, C& result);

	static constexpr std::string ConvertFileTimeToString(const std::filesystem::file_time_type& time) noexcept;

	// Action on scope exit
	template<typename F>
	struct FinalAction
	{
		explicit FinalAction(F f): act(f){}
		~FinalAction() {act(); }
		F act;
	};

	template<typename F>
	[[nodiscard]] auto PerformAtScopeExit(F f)
	{
		return FinalAction{f};
	}
}

#endif // CUSTOM_FUNCTIONS_H
