#pragma once

#include <string_view>

namespace Plugin
{
	using namespace std::literals;

	inline constexpr REL::Version VERSION
	{
		// clang-format off
		1u,
		0u,
		0u,
		// clang-format on
	};

	inline constexpr auto NAME = "Bound Weapons Dual Cast Dual Wield"sv;
	inline constexpr auto AUTHOR = "Zebrina"sv;
}
