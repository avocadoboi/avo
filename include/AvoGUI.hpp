/*
MIT License

Copyright (c) 2020 Björn Sundin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

//------------------------------

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

#if __has_include(<source_location>)
#	include <source_location>
#elif __has_include(<experimental/source_location>)
#	include <experimental/source_location>
	namespace std {
		using source_location = std::experimental::source_location;
	}
#endif

//------------------------------
// This is recommended. 
// Remove it if it causes you any problems.

using namespace std::chrono_literals;
using namespace std::string_view_literals;

//------------------------------

namespace avo {

// Physical screen pixels
using Pixels = std::int32_t;

// Density independent pixels
using Dip = float;

// Represents a proportion of something. Something that will be multiplied with a value.
using Factor = float;

namespace utils {

template<typename T>
concept IsTrivial = std::is_trivial_v<T>;

template<typename T, typename ... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template<typename T>
concept IsNumber = std::integral<T> || std::floating_point<T>;

//------------------------------

/*
	Represents a range of integers that can be iterated to produce the integers within the range.
*/
template<std::integral _Value, bool is_reverse = false>
class Range {
public:
	using value_type = _Value;

	class Iterator {
	public:
		using value_type = std::remove_cv_t<_Value>;
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = value_type;

	private:
		value_type _current_value;

	public:
		constexpr auto operator++(int) noexcept -> Iterator {
			if constexpr (is_reverse) {
				return Iterator{_current_value--};
			}
			else {
				return Iterator{_current_value++};
			}
		}
		constexpr auto operator++() noexcept -> Iterator& {
			if constexpr (is_reverse) {
				--_current_value;
			}
			else {
				++_current_value;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr auto operator+(difference_type const offset) const noexcept -> Iterator {
			if constexpr (is_reverse) {
				return _current_value - offset;
			}
			else {
				return _current_value + offset;
			}
		}
		constexpr auto operator+=(difference_type const offset) noexcept -> Iterator& {
			if constexpr (is_reverse) {
				_current_value -= offset;
			}
			else {
				_current_value += offset;
			}
			return *this;
		}

		constexpr auto operator--(int) noexcept -> Iterator {
			if constexpr (is_reverse) {
				return Iterator{_current_value++};
			}
			else {
				return Iterator{_current_value--};
			}
		}
		constexpr auto operator--() noexcept -> Iterator& {
			if constexpr (is_reverse) {
				++_current_value;
			}
			else {
				--_current_value;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr auto operator-(difference_type const offset) const noexcept -> Iterator {
			if constexpr (is_reverse) {
				return _current_value + offset;
			}
			else {
				return _current_value - offset;
			}        
		}
		constexpr auto operator-=(difference_type const offset) noexcept -> Iterator& {
			if constexpr (is_reverse) {
				_current_value += offset;
			}
			else {
				_current_value -= offset;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr auto operator*() const noexcept -> value_type const& {
			return _current_value;
		}
		[[nodiscard]]
		constexpr auto operator*() noexcept -> value_type& {
			return _current_value;
		}

		[[nodiscard]]
		constexpr auto operator<=>(Iterator const& other) const noexcept = default;

		constexpr Iterator() noexcept = default;
		constexpr Iterator(_Value const value) noexcept :
			_current_value{value}
		{}
	};

private:
	Iterator _start;
	Iterator _end;

public:
	[[nodiscard]]
	constexpr auto reverse() const noexcept -> Range<_Value, !is_reverse> {
		return {*(_end - 1), *_start};
	}

	[[nodiscard]]
	constexpr auto begin() const noexcept -> Iterator {
		return _start;
	}

	[[nodiscard]]
	constexpr auto end() const noexcept -> Iterator {
		return _end;
	}

	constexpr auto operator==(Range const&) const noexcept -> bool = default;

	/*
		Creates a range of integers starting with start and ending with inclusive_end.
	*/
	constexpr Range(_Value const start, _Value const inclusive_end) noexcept requires (!is_reverse) :
		_start{start},
		_end{inclusive_end + 1}
	{}
	constexpr Range(_Value const start, _Value const inclusive_end) noexcept requires is_reverse :
		_start{start},
		_end{inclusive_end - 1}
	{}
	/*
		Creates a range of integers starting with 0 and ending with count - 1.
	*/
	constexpr Range(_Value const count) noexcept :
		_start{0},
		_end{count}
	{}

	constexpr Range() noexcept = default;
};

template<typename T, typename _Value = typename T::value_type>
concept IsRange = requires(T range) {
	{ Range{range} } -> std::same_as<T>;
	requires std::same_as<_Value, typename T::value_type>;
};

#ifdef BUILD_TESTING
static_assert(
	[] {
		constexpr auto check_with = [](IsRange<int> auto const range) {
			return range.reverse().reverse() == range;
		};
		return check_with(Range{31415}) && check_with(Range{-6283, 31415});
	}(), 
	"Inverse property of avo::utils::Range::reverse() is not held"
);
static_assert(
	[] {
		constexpr auto number_of_iterations = std::size_t{10};

		auto a = std::size_t{};
		for (auto const b : Range{number_of_iterations}) {
			if (a != b) {
				return false;
			}
			++a;
		}
		return a == number_of_iterations;
	}(),
	"avo::utils::Range with single constructor argument works incorrectly."
);
static_assert(
	[] {
		constexpr auto number_of_iterations = std::size_t{10};

		auto a = number_of_iterations;
		for (auto const b : Range{number_of_iterations}.reverse()) {
			--a;
			if (a != b) {
				return false;
			}
		}
		return a == 0;
	}(),
	"Reversed avo::utils::Range with single constructor argument works incorrectly."
);
static_assert(
	[] {
		constexpr auto min = -5;
		constexpr auto max = 8;

		auto a = min;
		for (auto const b : Range{min, max}) {
			if (a != b) {
				return false;
			}
			++a;
		}
		return a == max + 1;
	}(),
	"avo::utils::Range with two constructor arguments works incorrectly."
);
static_assert(
	[] {
		constexpr auto min = -5;
		constexpr auto max = 8;

		auto a = max;
		for (auto const b : Range{min, max}.reverse()) {
			if (a != b) {
				return false;
			}
			--a;
		}
		return a == min - 1;
	}(),
	"Reversed avo::utils::Range with two constructor arguments works incorrectly."
);
#endif // BUILD_TESTING

//------------------------------

/*
	Takes any range and returns a range containing the indices of the elements of the original range.
*/
[[nodiscard]]
constexpr auto indices(std::ranges::sized_range auto&& range) -> Range<std::size_t> {
	return std::size(range);
}

#ifdef BUILD_TESTING
static_assert(
	[] {
		constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
		return indices(container) == Range{container.size()};
	}(),
	"avo::utils::indices with lvalue reference failed."
);
static_assert(
	indices(std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6}) == Range{std::size_t{11}},
	"avo::utils::indices with rvalue reference failed."
);
#endif // BUILD_TESTING

//------------------------------

template<typename T>
struct EnumeratedElement {
	std::size_t index;
	T& element;
};

/*
	Takes any range and returns a new range of (index, element) pairs referring to the original range.
	The original range isn't moved anywhere.
*/
[[nodiscard]]
constexpr auto enumerate(std::ranges::range auto& range) 
	-> std::ranges::view auto 
{
	return range | std::views::transform([i = std::size_t{}](auto& element) mutable {
		return EnumeratedElement{i++, element};
	});
}

/*
	Takes an rvalue range and returns a range of (index, element) pairs referring to 
	the original range. The returned range owns the original range since it is moved into it.
*/
[[nodiscard]]
constexpr auto enumerate(std::ranges::range auto&& range) -> std::ranges::range auto;

/*
	A range of (index, element) pairs referring to the elements of an owned range.
	The original range is moved into and owned by the enumerated range.
*/
template<std::ranges::range T> requires std::movable<T>
class EnumeratedRange {
private:
	using _BaseIterator = std::ranges::iterator_t<T const>;
	T _range;

public:
	class Iterator {
	public:
		using value_type = EnumeratedElement<std::ranges::range_value_t<T> const>;
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::input_iterator_tag;
		using iterator_concept = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;

	private:
		_BaseIterator _base_iterator;
		std::size_t _index;
		
	public:
		constexpr auto operator++(int) -> Iterator {
			return Iterator{++_base_iterator, ++_index};
		}
		constexpr auto operator++() -> Iterator& {
			++_base_iterator;
			++_index;
			return *this;
		}

		[[nodiscard]]
		constexpr auto operator==(Iterator const& other) const noexcept -> bool {
			return _base_iterator == other._base_iterator;
		}

		[[nodiscard]]
		constexpr auto operator*() const -> value_type {
			return EnumeratedElement{_index, *_base_iterator};
		}

		constexpr Iterator() = default;
		constexpr Iterator(_BaseIterator const base_iterator, std::size_t const index) :
			_base_iterator{base_iterator},
			_index{index}
		{}
	};

	[[nodiscard]]
	constexpr auto begin() const -> Iterator {
		return Iterator{std::begin(_range), std::size_t{}};
	}
	[[nodiscard]]
	constexpr auto end() const -> Iterator {
		return Iterator{std::end(_range), std::size_t{}};
	}

	friend constexpr auto enumerate<>(T&& range) -> std::ranges::range auto;

private:
	constexpr EnumeratedRange(T&& range) noexcept :
		_range{std::forward<T>(range)}
	{}
	constexpr EnumeratedRange() = default;
};

constexpr auto enumerate(std::ranges::range auto&& range) 
	-> std::ranges::range auto
{
	return EnumeratedRange{std::move(range)};
}

#ifdef BUILD_TESTING
static_assert(
	[] {
		constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
		auto correct_index = std::size_t{};
		for (auto const [index, element] : enumerate(container)) {
			if (index != correct_index || element != container[correct_index]) {
				return false;
			}
			++correct_index;
		}
		return correct_index == container.size();
	}(),
	"avo::utils::enumerate with lvalue reference failed."
);
static_assert(
	[] {
		constexpr auto original_container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
		auto correct_index = std::size_t{};
		for (auto const [index, element] : enumerate(std::array{original_container})) {
			if (index != correct_index || element != original_container[correct_index]) {
				return false;
			}
			++correct_index;
		}
		return correct_index == original_container.size();
	}(),
	"avo::utils::enumerate with rvalue reference failed."
);
#endif

//------------------------------

/*
	This can be called when the program reaches a path that should never be reachable.
	It prints error output and exits the program.
*/
#ifdef __cpp_lib_source_location
[[noreturn]]
inline auto unreachable(std::source_location const& source_location = std::source_location::current()) -> void {
	// TODO: use std::format when supported
	// std::cerr << std::format("Reached an unreachable code path in file {}, in function {}, on line {}.", 
	// 	source_location.file_name(), source_location.function_name(), source_location.line());
	std::cerr << "Reached an unreachable code path in file " << source_location.file_name() << 
		", in function " << source_location.function_name() << ", on line " << source_location.line() << ".\n";
	std::exit(1);
}
#else
[[noreturn]]
inline auto unreachable() -> void {
	std::cerr << "Reached an unreachable code path, exiting.\n";
	std::exit(1);
}
#endif

//------------------------------

/*
	Used to invoke a lambda at the end of a scope.
*/
template<std::invocable T>
class [[nodiscard]] Cleanup {
private:
	T _callable;
	
public:
	Cleanup(Cleanup&&) noexcept = delete;
	auto operator=(Cleanup&&) noexcept -> Cleanup& = delete;

	Cleanup(Cleanup const&) = delete;
	auto operator=(Cleanup const&) -> Cleanup& = delete;

	[[nodiscard]] 
	Cleanup(T&& callable) :
		_callable{std::forward<T>(callable)}
	{}

	Cleanup() = delete;
	~Cleanup() {
		_callable();
	}
};

//------------------------------

/*
	Similar to std::unique_ptr except that non-pointer types can be held
	and that a custom deleter must be specified. 

	This is useful for OS handles that are integer types, for example a native socket handle.
	Use C++20 lambdas in unevaluated contexts to specify a deleter, or use an already defined 
	functor type. 
	
	Example:
	using DllHandle = avo::utils::UniqueHandle<HMODULE, decltype([](auto& h){FreeLibrary(h);})>;
*/
template<IsTrivial _Type, std::invocable<_Type> _Deleter, _Type invalid_handle = _Type{}>
class UniqueHandle {
	_Type _handle{invalid_handle};

	auto close() -> void {
		if (_handle != invalid_handle) {
			_Deleter{}(_handle);
			_handle = invalid_handle;
		}
	}
public:
	[[nodiscard]]
	explicit operator _Type() const {
		return _handle;
	}
	[[nodiscard]]
	auto get() const -> _Type {
		return _handle;
	}
	[[nodiscard]]
	auto get() -> _Type& {
		return _handle;
	}

	[[nodiscard]]
	auto operator->() const -> _Type const* {
		return &_handle;
	}
	[[nodiscard]]
	auto operator->() -> _Type* {
		return &_handle;
	}

	[[nodiscard]]
	auto operator&() const -> _Type const* {
		return &_handle;
	}
	[[nodiscard]]
	auto operator&() -> _Type* {
		return &_handle;
	}

	[[nodiscard]]
	explicit operator bool() const {
		return _handle != invalid_handle;
	}
	[[nodiscard]]
	auto operator!() const -> bool {
		return _handle == invalid_handle;
	}

	explicit UniqueHandle(_Type handle) :
		_handle{handle}
	{}
	auto operator=(_Type handle) -> UniqueHandle& {
		close();
		_handle = handle;
		return *this;
	}

	UniqueHandle() = default;
	~UniqueHandle() {
		close();
	}

	UniqueHandle(UniqueHandle&& handle) noexcept :
		_handle{handle._handle}
	{
		handle._handle = invalid_handle;
	}
	auto operator=(UniqueHandle&& handle) noexcept -> UniqueHandle& {
		_handle = handle._handle;
		handle._handle = invalid_handle;
		return *this;
	}

	UniqueHandle(UniqueHandle const&) = delete;
	auto operator=(UniqueHandle const&) -> UniqueHandle& = delete;
};

//------------------------------

using DataVector = std::vector<std::byte>;
using DataView = std::span<std::byte const>;
using DataRange = std::span<std::byte>;

[[nodiscard]] 
inline auto read_file(std::string const path) -> DataVector {
	auto file = std::ifstream(path.data(), std::ios::ate | std::ios::binary);

	if (!file) {
		return {};
	}

	auto result = DataVector(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(result.data()), result.size());

	return result;
}

//------------------------------

/*
	Binds a const object to a member method of its class, so that the returned object can be invoked without providing the instance.
*/
template<typename _ReturnType, typename _Class, typename ... _Arguments>
[[nodiscard]] 
constexpr auto bind(auto (_Class::* const function)(_Arguments...) const -> _ReturnType, _Class const* const instance)
{
	return [instance, function](_Arguments&& ... arguments) { 
		return (instance->*function)(std::forward<_Arguments>(arguments)...); 
	};
}

/*
	Binds a mutable object to a member method of its class, so that the returned object can be invoked without providing the instance.
*/
template<typename _ReturnType, typename _Class, typename ... _Arguments>
[[nodiscard]] 
constexpr auto bind(auto (_Class::* const function)(_Arguments...) -> _ReturnType, _Class* const instance)
{
	return [instance, function](_Arguments&& ... arguments) { 
		return (instance->*function)(std::forward<_Arguments>(arguments)...); 
	};
}

#ifdef BUILD_TESTING
static_assert(
	[]{
		struct Test {
			bool b{};

			constexpr auto get() const -> bool {
				return b;
			}
		};
		auto const instance = Test{true};
		auto const get_function = bind(&Test::get, &instance);
		return get_function() == true;
	}(),
	"avo::utils::bind does not work with const objects."
);
static_assert(
	[]{
		struct Test {
			bool b{};

			constexpr auto get() -> bool {
				return b;
			}
		};
		auto instance = Test{true};
		auto const get_function = bind(&Test::get, &instance);
		return get_function() == true;
	}(),
	"avo::utils::bind does not work with mutable objects."
);
#endif

} // namespace utils

//------------------------------

/*
	Unicode support library.
*/
namespace unicode {

/*
	Enables UTF-8 encoded console output on Windows.
	Pretty much all other platforms use UTF-8 by default.
*/
auto enable_utf8_console() -> void;

//------------------------------

/*
	Converts a UTF-8 encoded char string to a UTF-16 encoded char16 string.
	Returns the length of the converted string, in code point units (char16_t).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
auto utf8_to_utf16(std::string_view input, std::span<char16_t> output) -> std::optional<std::size_t>;
/*
	Converts a UTF-8 encoded string to a UTF-16 encoded std::u16string.
*/
[[nodiscard]]
auto utf8_to_utf16(std::string_view input) -> std::u16string;

/*
	Converts a UTF-16 encoded char16 string to a UTF-8 encoded char string.
	Returns the length of the converted string, in code point units (char).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
auto utf16_to_utf8(std::u16string_view input, std::span<char> output) -> std::optional<std::size_t>;
/*
	Converts a UTF-16 char16 string to a UTF-8 encoded std::string.
*/
[[nodiscard]]
auto utf16_to_utf8(std::u16string_view input) -> std::string;

//------------------------------

/*
	Returns the number of UTF-8 code points a character starting with 
	first_code_point_in_character consists of in total.
	Returns 0 if the code point is not the first one in a character.
	Returns -1 if the code point is an invalid UTF-8 code point.
*/
[[nodiscard]]
constexpr auto code_point_count(char const first_code_point_in_character) noexcept -> int {
	// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 126
	if (!(first_code_point_in_character & 0x80)) // 0xxxxxxx
		return 1;
	if ((first_code_point_in_character & 0xc0) == 0x80) // 10??????
		return 0;
	if ((first_code_point_in_character & 0xe0) == 0xc0) // 110yyyyy
		return 2;
	if ((first_code_point_in_character & 0xf0) == 0xe0) // 1110zzzz
		return 3;
	if ((first_code_point_in_character & 0xf8) == 0xf0) // 11110uuu
		return 4;
	return -1;
}

/*
	Returns the number of UTF-16 code points a character starting with 
	first_code_point_in_character consists of in total.
	Returns 0 if the code point is not the first one in a character.
	Returns -1 if the code point is an invalid UTF-16 code point.
*/
[[nodiscard]]
constexpr auto code_point_count(char16_t const first_code_point_in_character) noexcept -> int {
	// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 125
	if ((first_code_point_in_character & 0xfc00) == 0xd800) // 110110wwwwxxxxxx
		return 2;
	if ((first_code_point_in_character & 0xfc00) == 0xdc00) // 110111xxxxxxxxxx
		return 0;
	return 1; // xxxxxxxxxxxxxxxx
}

/*
	Returns whether the passed code point is the start of a UTF-8 encoded character.
*/
[[nodiscard]]
constexpr auto is_first_code_point(char const code_point) noexcept -> bool {
	return (code_point & 0xc0) != 0x80;
}

/*
	Returns whether p_unit is the start of a UTF-16 encoded character
*/
[[nodiscard]]
constexpr auto is_first_code_point(char16_t const code_point) noexcept -> bool {
	return (code_point & 0xfc00) != 0xdc00;
}

template<typename T>
concept IsCodePoint = utils::IsAnyOf<T, char, char16_t>;

/*
	Returns the index of the code point at a certain character index in a UTF-8 or UTF-16 encoded string.
	If character_index is outside of the string, the size of the string is returned.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr auto code_point_index(std::basic_string_view<T> const string, std::size_t const character_index) 
	-> std::size_t 
{
	if (!character_index) {
		return {};
	}
	if (character_index >= string.size()) {
		return string.size();
	}

	auto const position = std::ranges::find_if(
		string, [character_index, char_count = std::size_t{}](T const code_point) mutable {
			return is_first_code_point(code_point) && char_count++ == character_index;
		}
	);
	return position - string.begin();
}

/*
	Returns the index of the character that the code point at code_point_index in the UTF-8 or UTF-16 encoded string belongs to.
	If code_point_index is outside of the string, the last character index is returned.
	If code_point_index is past the start of a character but before the next one, it returns the index of the character it is part of.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr auto character_index(std::basic_string_view<T> const string, std::size_t const code_point_index) 
	-> std::size_t 
{
	if (!code_point_index) {
		return {};
	}
	if (code_point_index >= string.size()) {
		return string.size();
	}

	return std::ranges::count_if(
		string.begin() + 1, string.begin() + code_point_index + 1,
		[](T const code_point) { return is_first_code_point(code_point); }
	);
}

/*
	Returns the number of unicode characters that a UTF-8 or UTF-16 string consists of.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr auto character_count(std::basic_string_view<T> const string) -> std::size_t {
	return character_index(string, string.size()) + 1;
}

#ifdef BUILD_TESTING
static_assert(
	code_point_count('a') == 1 &&
	code_point_count("å"[0]) == 2 &&
	code_point_count("√"[0]) == 3 &&
	code_point_count("🪢"[0]) == 4 &&
	code_point_count(static_cast<char>(0b10101010)) == 0 &&
	code_point_count(static_cast<char>(0b11111111)) == -1,
	"avo::unicode::code_point_count does not work correctly with UTF-8."
);
static_assert(
	code_point_count(u'a') == 1 &&
	code_point_count(u"å"[0]) == 1 &&
	code_point_count(u"√"[0]) == 1 &&
	code_point_count(u"🪢"[0]) == 2 &&
	code_point_count(static_cast<char16_t>(0b1101111010000011)) == 0,
	"avo::unicode::code_point_count does not work correctly with UTF-16."
);
static_assert(
	code_point_index("🪢 här √ är knut"sv, 10) == 17 &&
	code_point_index("🪢 här 🪢 är knut"sv, 10) == 18, 
	"code_point_index does not work correctly with UTF-8."
);
static_assert(
	code_point_index(u"🪢 här √ är knut"sv, 10) == 11 &&
	code_point_index(u"🪢 här 🪢 är knut"sv, 10) == 12, 
	"code_point_index does not work correctly with UTF-16."
);
static_assert(
	character_index("🪢 här √ är knut"sv, 17) == 10 &&
	character_index("🪢 här 🪢 är knut"sv, 18) == 10, 
	"character_index does not work correctly with UTF-8."
);
static_assert(
	character_index(u"🪢 här √ är knut"sv, 11) == 10 &&
	character_index(u"🪢 här 🪢 är knut"sv, 12) == 10, 
	"character_index does not work correctly with UTF-16."
);
#endif

} // namespace unicode

//------------------------------

namespace math {

template<utils::IsNumber T>
[[nodiscard]]
constexpr auto sign(T const number) -> T {
	return std::copysign(T{1}, number);
}

/*
	Returns a number multiplied by itself.
	Can be useful if you want to quickly square a longer expression.
*/
template<utils::IsNumber T>
[[nodiscard]]
constexpr auto square(T const x) noexcept -> T {
	return x*x;
}

/*
	Returns the inverse square root of a float using a faster but less accurate algorithm.
	It is about 8% to 15% faster than 1.f/std::sqrt(x) with gcc -O3 on my computer.
*/
[[nodiscard]]
inline auto fast_inverse_sqrt(float const input) noexcept -> float
{
	static_assert(std::numeric_limits<float>::is_iec559, "fast_inverse_sqrt error: float type must follow IEEE 754-1985 standard.");
	static_assert(sizeof(float) == 4, "fast_inverse_sqrt error: sizeof(float) must be 4.");

	// Only way to do type punning without undefined behavior it seems.
	std::uint32_t bits;
	std::memcpy(&bits, &input, 4);

	bits = 0x5f3759df - bits/2;

	float approximation;
	std::memcpy(&approximation, &bits, 4);

	return approximation*(1.5f - 0.5f*input*approximation*approximation);
}

template<typename T>
[[nodiscard]]
constexpr auto max(T&& value) -> decltype(auto) {
	return std::forward<T>(value);
}
/*
	Returns the parameter that is compared largest.
	All types must be totally ordered with each other.
	All types must be convertible to each other, but this is 
	best checked by the actual code than by a constraint.
*/
template<typename T0, typename T1, typename ... T2> requires std::totally_ordered_with<T0, T1>
[[nodiscard]]
constexpr auto max(T0&& first, T1&& second, T2&& ... arguments) -> decltype(auto) 
{
	return (first > second) ? 
		max(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		max(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

template<typename T>
[[nodiscard]]
constexpr auto min(T&& value) -> decltype(auto) {
	return std::forward<T>(value);
}
/*
	Returns the parameter that is compared smallest.
	All types must be totally ordered with each other.
	All types must be convertible to each other, but this is 
	best checked by the actual code than by a constraint.
*/
template<typename T0, typename T1, typename ... T2> requires std::totally_ordered_with<T0, T1>
[[nodiscard]]
constexpr auto min(T0&& first, T1&& second, T2&& ... arguments) -> decltype(auto) 
{
	return (first < second) ? 
		min(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		min(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

#ifdef BUILD_TESTING
static_assert(
	min(1, 9.89, 3, 6.1, -6, 0., 1845, 14) == -6 &&
	min(-1, 2) == -1 &&
	min(-1) == -1,
	"avo::math::min works incorrectly."
);
static_assert(
	max(1, 9.89, 3, 6.1, -6, 0., 1845, 14) == 1845 &&
	max(-1, 2) == 2 &&
	max(-1) == -1,
	"avo::math::max works incorrectly."
);
#endif

//------------------------------

/*
	A random number generator, a small abstraction on top of a subset 
	of the standard library random utilities.
*/
class Random {
private:
	std::default_random_engine _engine{std::random_device{}()};
public:
	/*
		Generates a new uniformly distributed random floating point number in the range [min, max).
	*/
	template<std::floating_point T>
	[[nodiscard]]
	auto next(T const min = T{}, T const max = T{1}) -> T {
		return std::uniform_real_distribution<T>{min, max}(_engine);
	}
	/*
		Generates a new uniformly distributed random integer in the range [min, max].
	*/
	template<std::integral T>
	[[nodiscard]]
	auto next(T const min = T{}, T const max = T{1}) -> T {
		return std::uniform_int_distribution<T>{min, max}(_engine);
	}
	/*
		Generates a new random floating point number distributed according to a gaussian distribution
		with a mean and a standard deviation.
	*/
	template<std::floating_point T>
	[[nodiscard]]
	auto next_normal(T const mean, T const standard_deviation) -> T {
		return std::normal_distribution<T>{mean, standard_deviation};
	}

	Random(std::uint_fast32_t const seed) :
		_engine{seed}
	{}
	Random() = default;
};

} // namespace math

//------------------------------

/*
	Represents an ID.
	To generate a new unique ID, use the default constructor like this:
		auto const id = Id{};
	To create an ID with a specific value (not guaranteed to be unique), just assign:
		auto const id = Id{1234};
	An ID which converts to 0 is considered invalid, and can be created like this:
		auto const id = Id{0};
*/
class Id {
public:
	using ValueType = std::uint64_t;
	
private:
	static ValueType s_counter;
	ValueType _count;

public:
	[[nodiscard]]
	constexpr operator ValueType() const noexcept {
		return _count;
	}
	[[nodiscard]]
	constexpr auto operator==(Id const& id) const noexcept -> bool = default;

	constexpr explicit Id(ValueType const id) noexcept :
		_count{id}
	{}
	Id() noexcept :
		_count{++s_counter}
	{}
};

//------------------------------

template<typename T>
class EventListeners;

/*
	This is a class used to easily manage event listeners. Any type of callable can be a listener.
	The return type and arguments have to be the same for all listeners added to one instance of EventListeners.
*/
template<typename _Return, typename ... _Arguments>
class EventListeners<_Return(_Arguments...)> {
public:
	using FunctionType = _Return(_Arguments...);
	
private:
	std::recursive_mutex _mutex;
	std::vector<std::function<FunctionType>> _listeners;

public:
	[[nodiscard]]
	auto begin() noexcept -> decltype(_listeners)::iterator {
		return _listeners.begin();
	}
	[[nodiscard]]
	auto end() noexcept -> decltype(_listeners)::iterator {
		return _listeners.end();
	}

	auto add(std::function<FunctionType> listener) -> void {
		auto const lock = std::scoped_lock{_mutex};    
		_listeners.emplace_back(std::move(listener));
	}
	auto operator+=(std::function<FunctionType> listener) -> EventListeners& {
		add(std::move(listener));
		return *this;
	}

	auto remove(std::function<FunctionType> const& listener) -> void {
		auto const lock = std::scoped_lock{_mutex};
		auto const& listener_type = listener.target_type();
		auto const found_position = std::ranges::find_if(_listeners, [&](auto const& listener_element) {
			// template keyword is used to expicitly tell the compiler that target is a template method for
			// std::function<FunctionalType> and < shouldn't be parsed as the less-than operator
			return listener_type == listener_element.target_type() &&
				*(listener.template target<FunctionType>()) == *(listener_element.template target<FunctionType>());
		});
		if (found_position != _listeners.end()) {
			*found_position = std::move(_listeners.back());
			_listeners.pop_back();
		}
	}
	auto operator-=(std::function<FunctionType> const& listener) -> EventListeners& {
		remove(listener);
		return *this;
	}

	/*
		Calls all of the listeners with event_arguments as arguments.
	*/
	auto notify_all(_Arguments&& ... event_arguments) -> void {
		auto const lock = std::scoped_lock{_mutex};
		for (auto& listener : _listeners) {
			listener(std::forward<_Arguments>(event_arguments)...);
		}
	}
	auto operator()(_Arguments&& ... event_arguments) -> void {
		notify_all(std::forward<_Arguments>(event_arguments)...);
	}

	EventListeners() = default;

	EventListeners(EventListeners&& other) :
		_listeners{std::move(other._listeners)}
	{}
	EventListeners(EventListeners const&) = delete;

	auto operator=(EventListeners&& other) -> EventListeners& {
		_listeners = std::move(other._listeners);
		return *this;
	}
	auto operator=(EventListeners const&) -> EventListeners& = delete;
};

} // namespace avo
