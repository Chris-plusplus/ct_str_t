#pragma once

#include <string>
#include <tuple>
#include "ct_str.hpp"
#include "ct_c.hpp"

namespace __ct_str_t {
/// @brief Helper struct concatenating string from variadic usage of ct_c struct.
/// @tparam ...Str - ct_c structs.
template<class Char, class... Str>
struct v_str {
	/// @brief Helper struct dividing Str... into first char and rest.
	/// @tparam C - first char.
	/// @tparam ...Str2 - rest of variadic string.
	template<class C, class... Str2>
	struct __next {
		/// @brief First char.
		///
		using char_t = C;
		/// @brief Rest of variadic string.
		///
		using type = v_str<Char, Str2...>;
	};

	/// @brief Type of first char.
	///
	using char_t = __next<Str...>::char_t;
	/// @brief Value of first char.
	///
	static inline constexpr Char front = char_t::c;

	/// @brief Rest of Str...
	///
	using next = __next<Str...>::type;

	/// @brief Concatenating function.
	///
	static inline constexpr std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>> str();

	/// @brief Length of string.
	///
	static inline constexpr size_t length = sizeof...(Str) - 1;
	/// @brief Concatenated string.
	///
	static inline const std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>> value = str();
};

template<class Char, class... Str>
constexpr std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>> v_str<Char, Str...>::str() {
	if constexpr (std::same_as<next, v_str<Char>>) { // recursion end
		return std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>>();
	}
	else { // recursive concatenation
		return front + next::str();
	}
}

/// @brief Specialization for empty string.
///
template<class Char>
struct v_str<Char> {};

/// @brief Helper function.
/// @tparam ...Str - variadic string.
/// @return Return type is the only usable thing.
template<class Char, class... Str>
constexpr auto __tuple_to_v_str(const std::tuple<Str...>&) {
	return v_str<Char, Str...>{};
}

/// @brief Helper function converting Str to equivalent tuple type.
/// @tparam Str - ct_str to converting.
/// @return Return type is the only usable thing.
template<class Char, ct_str Str>
constexpr auto __ct_str_to_tuple() {
	if constexpr (sizeof(Str.value)/sizeof(Char) <= 1) {
		return std::tuple<ct_c<Char, 0>>{};
	}
	else {
		return std::tuple_cat(std::tuple<ct_c<Char, Str.value[0]>>{}, std::invoke_result_t<decltype(__ct_str_to_tuple<Char, ct_str<Char, sizeof(Str.value)/sizeof(Char) - 1>(Str.value + 1)>)>{});
	}
}

/// @brief Helper function converting Str to equivalent v_str type.
/// @tparam Str - ct_str to convert. 
/// @return Return type is the only usable thing.
template<class Char, ct_str Str>
constexpr auto __ct_str_to_v_str() {
	return __tuple_to_v_str<Char>(__ct_str_to_tuple<Char, Str>());
}

/// @brief Helper alias converting Str to equivalent v_str type.
/// @tparam Str - ct_str to convert.
template<class Char, ct_str Str>
using __ct_str_to_v_str_t = std::invoke_result_t<decltype(__ct_str_to_v_str<Char, Str>)>;

/// @brief Helper class.
///
template<class Char, ct_str Str, class VStr = __ct_str_to_v_str_t<Char, Str>>
struct __ct_str_to_t {
	static inline constexpr const char* name = Str.value;
	static inline constexpr const size_t length = VStr::length;
};
}