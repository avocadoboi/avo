/*
MIT License

Copyright (c) 2021 Björn Sundin

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
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numbers>
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

/*
	At the moment, trying to instantiate this concept crashes GCC...
	TODO: Take advantage of this in the code when this GCC bug is fixed:
	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98611
*/
// template<typename T, template<typename...> typename _Class>
// concept IsInstantiationOf = requires(T object) {
// 	{ _Class{object} } -> std::same_as<T>;
// };

// template<typename T, template<typename...> typename ... U>
// concept IsInstantiationOfAny = (IsInstantiationOf<T, U> || ...);

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

/*
	This can be used to define a strong type that wraps a primitive arithmetic type.
	Arithmetic operations can be made on types that derive from this struct.
*/
template<utils::IsNumber _Value>
struct ArithmeticBase {
	using value_type = _Value;
	
	_Value value;

	[[nodiscard]]
	constexpr explicit operator _Value() const noexcept {
		return value;
	}
};

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator==(_Class<A> const first, _Class<B> const second) noexcept -> bool {
    return first.value == second.value;
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator<=>(_Class<A> const first, _Class<B> const second) noexcept {
    return first.value <=> second.value;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator+(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.value + second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
constexpr auto operator+=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.value += second.value;
	return first;
}

template<utils::IsNumber A, template<utils::IsNumber> typename _Class>
	requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(_Class<A> const value) noexcept {
	return _Class{-value.value};
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.value - second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
constexpr auto operator-=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.value -= second.value;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(_Class<A> const first, B const second) noexcept {
	return _Class{first.value*second};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(B first, _Class<A> const second) noexcept {
	return _Class{first*second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
constexpr auto operator*=(_Class<A>& first, B const second) noexcept -> _Class<A>& {
	first.value *= second;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator/(_Class<A> const first, B const second) noexcept {
	return _Class{first.value/second};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> typename _Class>
    requires std::derived_from<_Class<A>, ArithmeticBase<A>>
constexpr auto operator/=(_Class<A>& first, B const second) noexcept -> _Class<A>& {
	first.value /= second;
	return first;
}

//------------------------------

template<std::floating_point T>
struct Radians : ArithmeticBase<T> {};

template<typename T>
Radians(T) -> Radians<T>;

template<utils::IsNumber T>
struct Degrees : ArithmeticBase<T> {};

template<typename T>
Degrees(T) -> Degrees<T>;

// template<typename T>
// concept IsAngle = utils::IsInstantiationOfAny<T, Radians, Degrees>;

template<typename T>
concept IsDegrees = requires(T x) { { Degrees{x} } -> std::same_as<T>; };

template<typename T>
concept IsRadians = requires(T x) { { Radians{x} } -> std::same_as<T>; };

template<typename T>
concept IsAngle = IsRadians<T> || IsDegrees<T>;

/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<utils::IsNumber _To, std::floating_point _From>
[[nodiscard]]
constexpr auto to_degrees(Radians<_From> const radians) noexcept -> Degrees<_To> {
	if constexpr (std::integral<_To>) {
		return Degrees{static_cast<_To>(std::round(radians.value / std::numbers::pi_v<_From> * static_cast<_From>(180)))};
	}
	return Degrees{static_cast<_To>(radians.value / std::numbers::pi_v<_From> * static_cast<_From>(180))};
}
/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<utils::IsNumber _To, utils::IsNumber _From>
[[nodiscard]]
constexpr auto to_degrees(Degrees<_From> const degrees) noexcept -> Degrees<_To> {
	if constexpr (std::integral<_To> && std::floating_point<_From>) {
		return Degrees{static_cast<_To>(std::round(degrees.value))};
	}
	return Degrees{static_cast<_To>(degrees.value)};
}

/*
	Converts an angle to radians.
*/
template<std::floating_point _To, utils::IsNumber _From>
[[nodiscard]]
constexpr auto to_radians(Degrees<_From> const degrees) noexcept -> Radians<_To> {
	return Radians{static_cast<_To>(degrees.value / static_cast<_To>(180) * std::numbers::pi_v<_To>)};
}
/*
	Converts an angle to radians.
*/
template<std::floating_point _To, std::floating_point _From>
[[nodiscard]]
constexpr auto to_radians(Radians<_From> const radians) noexcept -> Radians<_To> {
	return Radians{static_cast<_To>(radians.value)};
}

/*
	Converts an angle to another angle type.
*/
template<IsRadians _To>
[[nodiscard]]
constexpr auto angle_as(IsAngle auto const angle) noexcept -> _To {
	return to_radians<typename _To::value_type>(angle);
}

/*
	Converts an angle to another angle type.
*/
template<IsDegrees _To>
[[nodiscard]]
constexpr auto angle_as(IsAngle auto const angle) noexcept -> _To {
	return to_degrees<typename _To::value_type>(angle);
}

inline namespace angle_literals {

constexpr auto operator"" _deg(long double const value) noexcept -> Degrees<double> {
	return Degrees{static_cast<double>(value)};
}
constexpr auto operator"" _degf(long double const value) noexcept -> Degrees<float> {
	return Degrees{static_cast<float>(value)};
}
constexpr auto operator"" _deg(unsigned long long const value) noexcept -> Degrees<int> {
	return Degrees{static_cast<int>(value)};
}
constexpr auto operator"" _rad(long double const value) noexcept -> Radians<double> {
	return Radians{static_cast<double>(value)};
}
constexpr auto operator"" _radf(long double const value) noexcept -> Radians<float> {
	return Radians{static_cast<float>(value)};
}

} // namespace angle_literals

namespace literals {

using namespace angle_literals;

} // namespace literals

#ifdef BUILD_TESTING

static_assert(utils::IsTrivial<Degrees<int>>);
static_assert(std::same_as<decltype(Degrees{5} + Degrees{3.1}), Degrees<double>>);
static_assert(std::same_as<decltype(Degrees{5}*3.1), Degrees<double>>);
static_assert(3.f*(Radians{5.f} + Radians{3.f}*3.f)/2.f - Radians{3.f} == Radians{18.f}, "Radian arithmetic does not work");
static_assert([]{
	auto angle = Radians{5.f};
	angle += Radians{2.f};
	if (angle != Radians{7.f}) return false;
	angle -= Radians{2.f};
	if (angle != Radians{5.f}) return false;
	angle /= 2.f;
	if (angle != Radians{2.5f}) return false;
	angle *= 4.f;
	if (angle != Radians{10.f}) return false;
	return true;
}(), "Radian arithmetic does not work.");
static_assert(80_deg == Degrees{80} && -80_deg == Degrees{-80});
static_assert(80._deg == Degrees{80.} && 80._degf == Degrees{80.f});
static_assert(3.14_rad == Radians{3.14} && 3.14_radf == Radians{3.14f});
static_assert(Degrees{80} > Degrees{-30} && Degrees{-30} < Degrees{80});
static_assert(Degrees{50} == Degrees{50} && Degrees{50} != Degrees{51});
static_assert(to_radians<float>(Degrees{180.f}) == Radians{std::numbers::pi_v<float>});
static_assert(to_degrees<int>(Radians{std::numbers::pi_v<float>}) == Degrees{180});
static_assert(to_degrees<float>(Degrees{50}) == Degrees{50.f});

#endif

//------------------------------

template<std::floating_point T>
[[nodiscard]]
constexpr auto approximately_equal(T const a, T const b, T const max_difference = static_cast<T>(1e-6)) -> bool 
{
	return std::abs(a - b) <= max_difference;
}

/*
	Returns 1 if the number is positive, 0 if it is 0 and -1 if it is negative.
*/
template<utils::IsNumber T>
[[nodiscard]]
constexpr auto sign(T const number) -> T {
	return std::copysign(T{1}, number);
}

template<utils::IsNumber T>
[[nodiscard]]
constexpr auto abs(T const number) -> T {
	return number >= T{} ? number ? -number;
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

/*
	Returns the pair of cosine and sine values for any angle.
*/
template<std::floating_point _Return>
auto cos_sin(IsAngle auto angle) -> std::pair<_Return, _Return> {
	auto const radians = to_radians<_Return>(angle);
	return std::pair{std::cos(radians.value), std::sin(radians.value)};
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

//------------------------------

template<utils::IsNumber>
struct Vector2dBase;

/*
	Evaluates to whether a class template is a 2d vector or not.
*/
template<template<typename> typename T>
concept Is2dVectorTemplate = std::derived_from<T<float>, Vector2dBase<float>>;
// Assuming all other instantiations also derive from Vector2dBase<T>.

/*
	Evaluates to whether a type is a 2d vector or not.
*/
template<typename T>
concept Is2dVector = requires {
	typename T::value_type;
	requires std::derived_from<T, Vector2dBase<typename T::value_type>>;
}; 

auto operator<<(std::ostream& stream, Is2dVector auto const vector) -> std::ostream& {
	stream << '(' << vector.x << ", " << vector.y << ')';
	return stream;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator==(_Class<A> const first, _Class<B> const second) noexcept -> bool {
	return first.x == second.x && first.y == second.y;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator<=>(_Class<A> const first, _Class<B> const second) noexcept -> std::partial_ordering {
	if (first.x < second.x && first.y < second.y) {
		return std::partial_ordering::less;
	}
	if (first.x > second.x && first.y > second.y) {
		return std::partial_ordering::greater;
	}
	if (first == second) {
		return std::partial_ordering::equivalent;
	}
	return std::partial_ordering::unordered;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator+(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.x + second.x, first.y + second.y};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator+=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.x += second.x;
	first.y += second.y;
	return first;
}

template<utils::IsNumber A, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator-(_Class<A> const vector) noexcept {
	return _Class{vector.x, vector.y};
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator-(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.x - second.x, first.y - second.y};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator-=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.x -= second.x;
	first.y -= second.y;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator*(_Class<A> const first, B const second) noexcept {
	return _Class{first.x*second, first.y*second};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator*(B const first, _Class<A> const second) noexcept {
	return _Class{second.x*first, second.y*first};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator*=(_Class<A>& first, B const second) noexcept -> _Class<A>& {
	first.x *= second;
	first.y *= second;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator*(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.x*second.x, first.y*second.y};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator*=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.x *= second.x;
	first.y *= second.y;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto scaled(_Class<A> const first, B const second) noexcept {
	return first * second;
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto scaled(_Class<A> const first, _Class<B> const second) noexcept {
	return first * second;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator/(_Class<A> const first, B const second) noexcept {
	return _Class{first.x/second, first.y/second};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator/(B const first, _Class<A> const second) noexcept {
	return _Class{first/second.x, first/second.y};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator/=(_Class<A>& first, B const second) noexcept -> _Class<A>& {
	first.x /= second;
	first.y /= second;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr auto operator/(_Class<A> const first, _Class<B> const second) noexcept {
	return _Class{first.x/second.x, first.y/second.y};
}
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
constexpr auto operator/=(_Class<A>& first, _Class<B> const second) noexcept -> _Class<A>& {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

/*
	Creates a square 2d vector, that is a vector with both coordinates equal.
*/
template<template<typename> typename _Vector, utils::IsNumber _Value> requires Is2dVectorTemplate<_Vector>
constexpr auto square(_Value const side_length) -> _Vector<_Value> {
	return _Vector{side_length, side_length};
}

template<std::floating_point T, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
constexpr auto interpolate(_Vector<T> const a, _Vector<T> const b, T const c) -> _Vector<T> {
	return _Vector{
		std::lerp(a.x, b.x, c),
		std::lerp(a.y, b.y, c),
	};
}

/*
	Creates a 2d vector from polar coordinates.
	The angle goes anticlockwise.
*/
template<template<typename> typename _Vector, std::floating_point _Length> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
auto polar(IsAngle auto const angle, _Length const length) {
	auto const [x, y] = cos_sin(angle);
	return _Vector{x*length, y*length};
}
/*
	Creates a unit 2d vector from an angle that goes anticlockwise.
*/
template<template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
auto polar(IsAngle auto const angle) {
	auto const [x, y] = cos_sin(angle);
	return _Vector{x, y};
}

template<utils::IsNumber T, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
constexpr auto with_negative_space_clipped(_Vector<T> vector) noexcept -> _Vector<T> {
	vector.clip_negative_space();
	return vector;
}

/*
	Returns the dot product of two 2d vectors.
*/
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
constexpr auto dot(_Vector<A> const first, _Vector<B> const second) {
	return first.x*second.x + first.y*second.y;
}

/*
	Returns the cross product of two 2d vectors.
*/
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
constexpr auto cross(_Vector<A> const first, _Vector<B> const second) {
	return first.x*second.y - first.y*second.x;
}

/*
	Returns the euclidean distance between two 2d vectors.
*/
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
auto distance(_Vector<A> const first, _Vector<B> const second) {
	return std::hypot(second.x - first.x, second.y - first.y);
}
/*
	Returns the squared euclidean distance between two 2d vectors.
*/
template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
[[nodiscard]]
constexpr auto distance_squared(_Vector<A> const first, _Vector<B> const second) noexcept {
	return square(second.x - first.x) + square(second.y - first.y);
}

/*
	Returns a vector that has the same angle as the argument but with a length of 1.
*/
template<Is2dVector T>
[[nodiscard]]
auto normalized(T vector) -> T {
	vector.normalize();
	return vector;
}
/*
	Returns a vector that has the same angle as the argument but with a length of 1, using a slightly faster algorithm.
*/
template<Is2dVector T>
[[nodiscard]]
auto normalized_fast(T vector) -> T {
	vector.normalize_fast();
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise.
*/
template<Is2dVector T>
[[nodiscard]]
auto rotated(T vector, IsAngle auto const angle) -> T {
	vector.rotate(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
auto rotated(T vector, IsAngle auto const angle, Is2dVector auto const origin) -> T {
	vector.rotate(angle, origin);
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise.
*/
template<Is2dVector T>
[[nodiscard]]
auto with_angle(T vector, IsAngle auto const angle) -> T {
	vector.set_angle(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise
	relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
auto with_angle(T vector, IsAngle auto const angle, Is2dVector auto const origin) -> T {
	vector.set_angle(angle, origin);
	return vector;
}

/*
	All 2d vector types derive from this type.
	It provides a bunch of 2d vector operations.
*/
template<utils::IsNumber _Value>
struct Vector2dBase {
	using value_type = _Value;

	_Value x, y;

	/*
		Returns the magnitude of the vector, or the hypotenuse of the triangle.
	*/
	[[nodiscard]]
	auto get_length() const {
		return std::hypot(x, y);
	}
	/*
		Returns the squared magnitude of the vector, or the squared hypotenuse of the triangle.
	*/
	[[nodiscard]]
	constexpr auto get_length_squared() const noexcept -> _Value {
		return x*x + y*y;
	}

	/*
		Sets any negative coordinates to 0.
	*/
	constexpr auto clip_negative_space() noexcept -> void {
		x = max(_Value{}, x);
		y = max(_Value{}, y);
	}

	/*
		Rotates the vector by an angle clockwise.
	*/
	auto rotate(IsAngle auto const angle) -> void {
		// A very small change in angle could result in a very big change in cartesian coordinates.
		// Therefore we use long double for these calculations and not _Value.
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const x_before = x;
		x = static_cast<_Value>(x*cos - y*sin);
		y = static_cast<_Value>(y*cos + x_before*sin);
	}
	/*
		Rotates the vector by an angle clockwise relative to an origin.
	*/
	auto rotate(IsAngle auto const angle, Is2dVector auto const origin) -> void {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const x_before = x;
		x = static_cast<_Value>((x - origin.x)*cos - (y - origin.y)*sin + origin.x);
		y = static_cast<_Value>((y - origin.y)*cos + (x_before - origin.x)*sin + origin.y);
	}

	/*
		Sets the angle of the vector measured anticlockwise from the right side.
	*/
	auto set_angle(IsAngle auto const angle) -> void {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const length = get_length();
		x = static_cast<_Value>(cos*length);
		y = static_cast<_Value>(sin*length);
	}
	/*
		Sets the angle of the vector measured anticlockwise from the right side relative to an origin.
	*/
	auto set_angle(IsAngle auto const angle, Is2dVector auto const origin) -> void {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const length = distance(*this, origin);
		x = static_cast<_Value>(cos*length + origin.x);
		y = static_cast<_Value>(sin*length + origin.y);
	}

	/*
		Returns the angle of the vector measured anticlockwise from the right side.
	*/
	template<IsAngle _Angle>
	[[nodiscard]]
	auto get_angle() const -> _Angle {
		if (!x && !y) {
			return _Angle{};
		}
		auto const atan2 = std::atan2(static_cast<long double>(y), static_cast<long double>(x));
		if (atan2 < 0) {
			return angle_as<_Angle>(Radians{atan2 + 2*std::numbers::pi_v<long double>});
		}
		return angle_as<_Angle>(Radians{atan2});
	}
	/*
		Returns the angle of the vector measured anticlockwise from the right side relative to an origin.
	*/
	template<IsAngle _Angle>
	[[nodiscard]]
	auto get_angle(Is2dVector auto const origin) const -> _Angle {
		if (x == origin.x && y == origin.y) {
			return _Angle{};
		}
		auto const atan2 = std::atan2(
			static_cast<long double>(y - origin.y), 
			static_cast<long double>(x - origin.x)
		);
		if (atan2 < 0) {
			return angle_as<_Angle>(Radians{atan2 + 2*std::numbers::pi_v<long double>});
		}
		return angle_as<_Angle>(Radians{atan2});
	}

	/*
		Keeps the angle of the vector but sets its length to 1.
	*/
	auto normalize() -> void {
		auto const length = get_length();
		x /= length;
		y /= length;
	}
	/*
		Keeps the angle of the vector but sets its length to 1 using a slightly faster algorithm.
	*/
	auto normalize_fast() -> void {
		auto const inverse_length = fast_inverse_sqrt(get_length_squared());
		x *= inverse_length;
		y *= inverse_length;
	}

	template<Is2dVector _Vector>
	[[nodiscard]]
	constexpr auto to() const noexcept -> _Vector {
		return _Vector{
			static_cast<typename _Vector::value_type>(x),
			static_cast<typename _Vector::value_type>(y)
		};
	}
};

template<utils::IsNumber _Value = float>
struct Vector2d : Vector2dBase<_Value> {};

template<typename T>
Vector2d(T, T) -> Vector2d<T>;

template<utils::IsNumber _Value = float>
struct Point : Vector2dBase<_Value> {};

template<typename T>
Point(T, T) -> Point<T>;

template<utils::IsNumber _Value = float>
struct Size : Vector2dBase<_Value> {};

template<typename T>
Size(T, T) -> Size<T>;

#ifdef BUILD_TESTING
static_assert(utils::IsTrivial<Vector2d<float>>);
static_assert(Vector2d{5.f, 3.f} != Vector2d{5.f, 3.1f});
static_assert(Vector2d{5.f, 3.f} == Vector2d{5.f, 3.f});
static_assert(Vector2d{-2.f, -3.f} < Vector2d{5.f, 3.f} && Vector2d{5.f, 3.f} > Vector2d{-2.f, -3.f});
static_assert(Vector2d{-2.f, 1.f}*2.f == Vector2d{16.f, -8.f}/-4.f);
static_assert(Vector2d{-2.f, 1.f}*2.f == 2.f*Vector2d{-2.f, 1.f});
static_assert(2.f/Vector2d{2.f, -4.f} == Vector2d{1.f, -0.5f});
static_assert(100/(3*Vector2d{4, 3} + Vector2d{2, 1}*2) == Vector2d{100/16, 100/11});

static_assert(interpolate(Vector2d{0.f, 0.f}, Vector2d{1.f, 1.f}, 0.5f) == Vector2d{0.5f, 0.5f});

static_assert(with_negative_space_clipped(Size{-4.f, 8.f}) == Size{0.f, 8.f});
static_assert(with_negative_space_clipped(Size{-4.f, -8.f}) == Size{});

static_assert(square<Vector2d>(5.f) == Vector2d{5.f, 5.f});
static_assert(square(5.f) == 25.f);

static_assert(dot(Vector2d{4, 2}, Vector2d{-2, -3}) == -14);
static_assert(cross(Vector2d{4, 2}, Vector2d{-2, -3}) == -8);

static_assert(Vector2d{2.f, 5.f}.to<Size<int>>() == Size{2, 5});
#endif

//------------------------------

template<std::floating_point T>
struct Transform {
	using value_type = T;
	
	T x_to_x{1}, y_to_x{0}, offset_x{0},
	  x_to_y{0}, y_to_y{1}, offset_y{0};

	[[nodiscard]]
	constexpr auto operator==(Transform const&) const noexcept -> bool = default;

	/*
		Multiplies this 2x3 matrix to a column vector that has an implicit third column with the value 1.

		[a b c]   [x]   [ax+by+c]
		[d e f] * [y] = [dx+ey+f]
		          [1]
	*/
	template<utils::IsNumber U, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
	[[nodiscard]]
	constexpr auto operator*(_Vector<U> const vector) const noexcept {
		return _Vector{
			x_to_x*vector.x + y_to_x*vector.y + offset_x, 
			x_to_y*vector.x + y_to_y*vector.y + offset_y
		};
	}

	/*
		Returns the result of applying this transform to another transform.
		Note that the order of multiplication matters.

		(A*B)*v = A*(B*v) where v is a vector, A and B are transforms.

		[a b c]   [g h i]   [ag+bj ah+bk ai+bl+c]
		[d e f] * [j k l] = [dg+ej dh+ek di+el+f]
		[0 0 1]   [0 0 1]   [0     0     1      ]
	*/
	template<std::floating_point U>
	[[nodiscard]]
	constexpr auto operator*(Transform<U> const& other) const noexcept {
		return Transform<std::common_type_t<T, U>>{
			.x_to_x = x_to_x*other.x_to_x + y_to_x*other.x_to_y,
			.y_to_x = x_to_x*other.y_to_x + y_to_x*other.y_to_y,
			.offset_x = x_to_x*other.offset_x + y_to_x*other.offset_y + offset_x,
			.x_to_y = x_to_y*other.x_to_x + y_to_y*other.x_to_y,
			.y_to_y = x_to_y*other.y_to_x + y_to_y*other.y_to_y,
			.offset_y = x_to_y*other.offset_x + y_to_y*other.offset_y + offset_y
		};
	}
	/*
		A *= B is equivalent to A = B*A.
	*/
	template<std::floating_point U>
	constexpr auto operator*=(Transform<U> const& other) const noexcept -> Transform& {
		return *this = other * *this;	
	}

	/*
		Rotates transformed points anticlockwise from the right.
	*/
	auto rotate(IsAngle auto const angle) -> Transform& {
		/*
			[cos -sin  0]   [x_to_x y_to_x offset_x]
			[sin  cos  0] * [x_to_y y_to_y offset_y]
			[0    0    1]   [0      0      1       ]
		*/
		auto const [cos, sin] = cos_sin<long double>(angle);

		// Previous transformation
		auto const p = *this;

		x_to_x = static_cast<T>(cos*p.x_to_x - sin*p.x_to_y);
		y_to_x = static_cast<T>(cos*p.y_to_x - sin*p.y_to_y);
		offset_x = static_cast<T>(cos*p.offset_x - sin*p.offset_y);
		x_to_y = static_cast<T>(sin*p.x_to_x + cos*p.x_to_y);
		y_to_y = static_cast<T>(sin*p.y_to_x + cos*p.y_to_y);
		offset_y = static_cast<T>(sin*p.offset_x + cos*p.offset_y);

		return *this;
	}
	/*
		Rotates transformed points around an origin anticlockwise from the right.
	*/
	auto rotate(IsAngle auto const angle, Is2dVector auto const origin) -> Transform& {
		translate(-origin);
		rotate(angle);
		translate(origin);
		return *this;
	}

	/*
		Moves the translation by an offset.
	*/
	constexpr auto translate(Is2dVector auto const offset) noexcept -> Transform& {
		offset_x += offset.x;
		offset_y += offset.y;
		return *this;
	}
	/*
		Sets the absolute offset in coordinates caused by the transform.
	*/
	constexpr auto set_translation(Is2dVector auto const point) noexcept -> Transform& {
		offset_x = point.x;
		offset_y = point.y;
		return *this;
	}

	/*
		Scales the transform by a horizontal and vertical factor.
	*/
	constexpr auto scale(Is2dVector auto const scale_factor) noexcept -> Transform& {
		x_to_x *= scale_factor.x;
		y_to_x *= scale_factor.x;
		offset_x *= scale_factor.x;
		y_to_y *= scale_factor.y;
		x_to_y *= scale_factor.y;
		offset_y *= scale_factor.y;
		return *this;
	}
	constexpr auto scale_x(utils::IsNumber auto const scale_factor) noexcept -> Transform& {
		x_to_x *= scale_factor;
		y_to_x *= scale_factor;
		offset_x *= scale_factor;
		return *this;
	}
	constexpr auto scale_y(utils::IsNumber auto const scale_factor) noexcept -> Transform& {
		y_to_y *= scale_factor;
		x_to_y *= scale_factor;
		offset_y *= scale_factor;
		return *this;
	}
};

template<std::floating_point T>
auto operator<<(std::ostream& stream, Transform<T> const transform) -> std::ostream& {
	return stream << "| " << transform.x_to_x << ' ' << transform.y_to_x << ' ' << transform.offset_x << " |\n"
		<< "| " << transform.x_to_y << ' ' << transform.y_to_y << ' ' << transform.offset_y << " |\n";
}

/*
	Returns the inverse of a transformation matrix I such that:
	    [a b c]   [1 0 0]
	I * [d e f] = [0 1 0]
	    [0 0 1]   [0 0 1]
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto inverse(Transform<T> const t) noexcept -> Transform<T> {
	auto const divisor = t.x_to_x*t.y_to_y - t.y_to_x*t.x_to_y;
	return Transform{
		t.y_to_y/divisor,
		-t.y_to_x/divisor,
		(t.y_to_x*t.offset_y - t.offset_x*t.y_to_y)/divisor,
		-t.x_to_y/divisor,
		t.x_to_x/divisor,
		(t.offset_x*t.x_to_y - t.x_to_x*t.offset_y)/divisor,
	};
}

/*
	Returns a rotated copy of the Transform argument.
	See Transform::rotate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto rotated(Transform<T> transform, IsAngle auto const angle) -> Transform<T> {
	return transform.rotate(angle);
}
/*
	Returns a rotated copy of the Transform argument.
	See Transform::rotate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto rotated(Transform<T> transform, IsAngle auto const angle, Is2dVector auto const origin) -> Transform<T> {
	return transform.rotate(angle, origin);
}

/*
	Returns a translated copy of the Transform argument.
	See Transform::translate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto translated(Transform<T> transform, Is2dVector auto const offset) -> Transform<T> {
	return transform.translate(offset);
}
/*
	Returns a copy of the Transform argument with a specific absolute translation.
	See Transform::set_translation.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto with_translation(Transform<T> transform, Is2dVector auto const point) -> Transform<T> {
	return transform.set_translation(point);
}

/*
	Returns a scaled copy of the Transform argument.
	See Transform::scale.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr auto scaled(Transform<T> transform, Is2dVector auto const scale_factor) noexcept -> Transform<T> {
	return transform.scale(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr auto scaled_x(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept -> Transform<T> {
	return transform.scale_x(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr auto scaled_y(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept -> Transform<T> {
	return transform.scale_y(scale_factor);
}

#ifdef BUILD_TESTING
template<std::floating_point T>
constexpr auto get_is_approximately_identity(Transform<T> const t) -> bool {
	return approximately_equal(t.x_to_x, T{1}) && approximately_equal(t.y_to_x, T{}) && approximately_equal(t.offset_x, T{}) &&
		approximately_equal(t.x_to_y, T{}) && approximately_equal(t.y_to_y, T{1}) && approximately_equal(t.offset_y, T{});
}

static_assert(
	[]{
		constexpr auto a = Transform{
			11., 2.9, 3.5, 
			4.3, 5.7, 6.2
		};
		// return get_is_approximately_identity(a*inverse(a));// && get_is_approximately_identity(inverse(a)*a);
		return get_is_approximately_identity(a*inverse(a)) && get_is_approximately_identity(inverse(a)*a);
	}()
);
static_assert(
	[]{
		constexpr auto a = Transform{
			11.f, 2.9f, 3.5f, 
			4.3f, 5.7f, 6.2f
		};
		constexpr auto b = Transform{
			-4.8f, -3.6f, -2.6f, 
			-1.2f, 1.5f, -3.31f
		};

		constexpr auto c = Vector2d{3.14f, 5.158f};

		return a*(b*c) == (a*b)*c && a*b != b*a;
	}(),
	"avo::math::Transform does not hold the composition and non-commutative properties."
);
static_assert(
	[]{
		auto a = Transform{
			11.f, 2.9f, 3.5f, 
			4.3f, 5.7f, 6.2f
		};
		return scaled(a, square<Vector2d>(0.6f)) == scaled_x(a, 0.6f).scale_y(0.6f);
	}(),
	"The scaling functions for avo::math::Transform do not work properly."
);
#endif

//------------------------------

template<utils::IsNumber _Value>
struct Rectangle {
	using value_type = _Value;

	_Value left{}, top{}, right{}, bottom{};

	constexpr Rectangle() noexcept = default;
	constexpr Rectangle(_Value const p_left, _Value const p_top, _Value const p_right, _Value const p_bottom) noexcept :
		left{p_left}, top{p_top}, right{p_right}, bottom{p_bottom}
	{}
	constexpr Rectangle(Size<_Value> const size) noexcept :
		left{}, top{}, right{size.x}, bottom{size.y}
	{}
	constexpr Rectangle(Point<_Value> const position) noexcept :
		left{position.x}, top{position.y},
		right{position.x}, bottom{position.y}
	{}
	constexpr Rectangle(Point<_Value> const position, Size<_Value> const size) noexcept :
		left{position.x}, top{position.y},
		right{position.x + size.x}, bottom{position.y + size.y}
	{}
	constexpr Rectangle(Point<_Value> const top_left, Point<_Value> const bottom_right) noexcept :
		left{top_left.x}, top{top_left.y},
		right{bottom_right.x}, bottom{bottom_right.y}
	{}

	[[nodiscard]]
	constexpr auto operator==(Rectangle const&) const noexcept -> bool = default;

	[[nodiscard]]
	constexpr auto operator-() const noexcept -> Rectangle {
		return Rectangle{-right, -bottom, -left, -top};
	}
	
	[[nodiscard]]
	constexpr auto operator+(Is2dVector auto const vector) const noexcept {
		return Rectangle<decltype(left + vector.x)>{
			left + vector.x,
			top + vector.y,
			right + vector.x,
			bottom + vector.y
		};
	}
	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr auto operator+(Size<T> const size) const noexcept {
		return Rectangle<std::common_type_t<_Value, T>>{
			left,
			top,
			right + size.x,
			bottom + size.y
		};
	}
	[[nodiscard]]
	constexpr auto operator-(Is2dVector auto const vector) const noexcept {
		return *this + (-vector);
	}

	constexpr auto offset_x(_Value const offset) noexcept -> Rectangle& {
		left += offset;
		right += offset;
		return *this;
	}
	constexpr auto offset_y(_Value const offset) noexcept -> Rectangle& {
		top += offset;
		bottom += offset;
		return *this;
	}
	constexpr auto offset(Is2dVector auto const offset) noexcept -> Rectangle& {
		offset_x(offset.x);
		offset_y(offset.y);
		return *this;
	}
	template<utils::IsNumber T>
	constexpr auto offset(Size<T> const size_offset) noexcept -> Rectangle& {
		right += size_offset.x;
		bottom += size_offset.y;
		return *this;
	}
	constexpr auto operator+=(Is2dVector auto const offset) noexcept -> Rectangle& {
		return offset(offset);
	}
	constexpr auto operator-=(Is2dVector auto const offset) noexcept -> Rectangle& {
		return offset(-offset);
	}

	[[nodiscard]]
	constexpr auto operator*(utils::IsNumber auto const factor) const noexcept {
		return Rectangle<decltype(left*factor)>{
			left*factor,
			top*factor,
			right*factor,
			bottom*factor
		};
	}
	constexpr auto operator*=(utils::IsNumber auto const factor) noexcept -> Rectangle& {
		left *= factor;
		top *= factor;
		right *= factor;
		bottom *= factor;
		return *this;
	}
	[[nodiscard]]
	constexpr auto operator/(utils::IsNumber auto const divisor) const noexcept {
		return Rectangle<decltype(left/divisor)>{
			left/divisor,
			top/divisor,
			right/divisor,
			bottom/divisor
		};
	}
	constexpr auto operator/=(utils::IsNumber auto const divisor) noexcept -> Rectangle& {
		left /= divisor;
		top /= divisor;
		right /= divisor;
		bottom /= divisor;
		return *this;
	}

	constexpr auto set_width(_Value const width) noexcept -> Rectangle& {
		right = left + width;
		return *this;
	}
	constexpr auto set_height(_Value const height) noexcept -> Rectangle& {
		bottom = top + height;
		return *this;
	}
	constexpr auto set_size(Size<_Value> const size) noexcept -> Rectangle& {
		set_width(size.x);
		set_height(size.y);
		return *this;
	}

	[[nodiscard]]
	constexpr auto get_size() const noexcept -> Size<_Value> {
		return Size{right - left, bottom - top};
	}
	[[nodiscard]]
	constexpr auto get_width() const noexcept -> _Value {
		return right - left;
	}
	[[nodiscard]]
	constexpr auto get_height() const noexcept -> _Value {
		return bottom - top;
	}

	[[nodiscard]]
	constexpr auto get_top_left() const noexcept -> Point<_Value> {
		return Point{left, top};
	}
	template<bool keep_size = true>
	constexpr auto set_top_left(Point<_Value> const top_left) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			right += top_left.x - left;
			bottom += top_left.y - top;
		}
		left = top_left.x;
		top = top_left.y;
		return *this;
	}

	[[nodiscard]]
	constexpr auto get_top_right() const noexcept -> Point<_Value> {
		return Point{right, top};
	}
	template<bool keep_size = true>
	constexpr auto set_top_right(Point<_Value> const top_right) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			left += top_right.x - right;
			bottom += top_right.y - top;
		}
		right = top_right.x;
		top = top_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr auto get_bottom_right() const noexcept -> Point<_Value> {
		return Point{right, bottom};
	}
	template<bool keep_size = true>
	constexpr auto set_bottom_right(Point<_Value> const bottom_right) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			left += bottom_right.x - right;
			top += bottom_right.y - bottom;
		}
		right = bottom_right.x;
		bottom = bottom_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr auto get_bottom_left() const noexcept -> Point<_Value> {
		return Point{left, bottom};
	}
	template<bool keep_size = true>
	constexpr auto set_bottom_left(Point<_Value> const bottom_left) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			right += bottom_left.x - left;
			top += bottom_left.y - bottom;
		}
		left = bottom_left.x;
		bottom = bottom_left.y;
		return *this;
	}

	template<bool keep_size = true>
	constexpr auto set_left(_Value const new_left) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			right += new_left - left;
		}
		left = new_left;
		return *this;
	}
	template<bool keep_size = true>
	constexpr auto set_top(_Value const new_top) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			bottom += new_top - top;
		}
		top = new_top;
		return *this;
	}
	template<bool keep_size = true>
	constexpr auto set_right(_Value const new_right) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			left += new_right - right;
		}
		right = new_right;
		return *this;
	}
	template<bool keep_size = true>
	constexpr auto set_bottom(_Value const new_bottom) noexcept -> Rectangle& {
		if constexpr (keep_size) {
			top += new_bottom - bottom;
		}
		bottom = new_bottom;
		return *this;
	}

	template<utils::IsNumber T>
	constexpr auto set_center(Point<T> const center) noexcept -> Rectangle& {
		auto const half_size = get_size()/2;
		left = static_cast<_Value>(center.x - half_size.x);
		top = static_cast<_Value>(center.y - half_size.y);
		right = static_cast<_Value>(center.x + half_size.x);
		bottom = static_cast<_Value>(center.y + half_size.y);
		return *this;
	}
	constexpr auto set_center_x(utils::IsNumber auto const center_x) noexcept -> Rectangle& {
		auto const half_width = get_width()/2;
		left = static_cast<_Value>(center_x - half_width);
		right = static_cast<_Value>(center_x + half_width);
		return *this;
	}
	constexpr auto set_center_y(utils::IsNumber auto const center_y) noexcept -> Rectangle& {
		auto const half_height = get_height()/2;
		top = static_cast<_Value>(center_y - half_height);
		bottom = static_cast<_Value>(center_y + half_height);
		return *this;
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr auto get_center() const noexcept -> Point<T> {
		return Point{get_center_x(), get_center_y()};
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr auto get_center_x() const noexcept -> T {
		return std::midpoint(static_cast<T>(left), static_cast<T>(right));
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr auto get_center_y() const noexcept -> T {
		return std::midpoint(static_cast<T>(top), static_cast<T>(bottom));
	}

	constexpr auto move_top_left(Vector2d<_Value> const offset) noexcept -> Rectangle& {
		left += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr auto move_top_right(Vector2d<_Value> const offset) noexcept -> Rectangle& {
		right += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr auto move_bottom_left(Vector2d<_Value> const offset) noexcept -> Rectangle& {
		left += offset.x;
		bottom += offset.y;
		return *this;
	}
	constexpr auto move_bottom_right(Vector2d<_Value> const offset) noexcept -> Rectangle& {
		right += offset.x;
		bottom += offset.y;
		return *this;
	}

	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr auto to() const noexcept -> Rectangle<T> {
		return Rectangle<T>{
			static_cast<T>(left),
			static_cast<T>(top),
			static_cast<T>(right),
			static_cast<T>(bottom)
		};
	}

	/*
		If the size in any dimension is negative, it is set to 
		zero by moving the most negative coordinate.
		For example, if right < left, then right = left.
	*/
	constexpr auto clip_negative_space() noexcept -> Rectangle& {
		right = max(left, right);
		bottom = max(top, bottom);
		return *this;
	}
	constexpr auto round_outwards() noexcept -> Rectangle& {
		left = std::floor(left);
		top = std::floor(top);
		right = std::ceil(right);
		bottom = std::ceil(bottom);
		return *this;
	}

	constexpr auto bound(Rectangle<_Value> const bounds) noexcept -> Rectangle& {
		left = std::clamp(left, bounds.left, bounds.right);
		top = std::clamp(top, bounds.top, bounds.bottom);
		right = std::clamp(right, bounds.left, bounds.right);
		bottom = std::clamp(bottom, bounds.top, bounds.bottom);
		return *this;
	}
	template<utils::IsNumber T>
	constexpr auto contain(Rectangle<T> const rectangle) noexcept -> Rectangle& {
		/*
			If this is true then we need to round "outwards" so that this 
			rectangle also contains the other rectangle's fractional part.
		*/
		if constexpr (std::floating_point<T> && std::integral<_Value>) {
			left = min(left, static_cast<_Value>(std::floor(rectangle.left)));
			top = min(top, static_cast<_Value>(std::floor(rectangle.top)));
			right = max(right, static_cast<_Value>(std::ceil(rectangle.right)));
			bottom = max(bottom, static_cast<_Value>(std::ceil(rectangle.bottom)));
		}
		else {
			left = min(left, rectangle.left);
			top = min(top, rectangle.top);
			right = max(right, rectangle.right);
			bottom = max(bottom, rectangle.bottom);
		}
		return *this;
	}

	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr auto contains(Point<T> const point) const noexcept -> bool {
		using _Common = std::common_type_t<_Value, T>;
		return static_cast<_Common>(point.x) >= static_cast<_Common>(left) && 
			static_cast<_Common>(point.x) < static_cast<_Common>(right) &&
			static_cast<_Common>(point.y) >= static_cast<_Common>(top) && 
			static_cast<_Common>(point.y) < static_cast<_Common>(bottom);
	}
	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr auto contains(Rectangle<T> const rectangle) const noexcept -> bool {
		using _Common = std::common_type_t<_Value, T>;
		return static_cast<_Common>(rectangle.left) > static_cast<_Common>(left) && 
			static_cast<_Common>(rectangle.top) > static_cast<_Common>(top) && 
			static_cast<_Common>(rectangle.right) < static_cast<_Common>(right) && 
			static_cast<_Common>(rectangle.bottom) < static_cast<_Common>(bottom);
	}
	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr auto intersects(Rectangle<T> const rectangle) const noexcept -> bool {
		using _Common = std::common_type_t<_Value, T>;
		return static_cast<_Common>(rectangle.right) > static_cast<_Common>(left) && 
			static_cast<_Common>(rectangle.left) < static_cast<_Common>(right) &&
			static_cast<_Common>(rectangle.bottom) > static_cast<_Common>(top) && 
			static_cast<_Common>(rectangle.top) < static_cast<_Common>(bottom);
	}
};

template<typename T>
concept IsRectangle = requires(T x) { { Rectangle{x} } -> std::same_as<T>; };

template<utils::IsNumber T>
[[nodiscard]]
constexpr auto with_negative_space_clipped(Rectangle<T> rectangle) noexcept -> Rectangle<T> {
	return rectangle.clip_negative_space();
}

template<utils::IsNumber T, utils::IsNumber U>
[[nodiscard]]
constexpr auto scaled(Rectangle<T> const rectangle, U const scale_factor) noexcept {
	return rectangle * scale_factor;
}

template<template<typename> typename _Rectangle, utils::IsNumber T> 
	requires std::same_as<_Rectangle<T>, Rectangle<T>>
[[nodiscard]]
constexpr auto square(T const value) noexcept -> Rectangle<T> {
	return Rectangle{T{}, T{}, value, value};
}

#ifdef BUILD_TESTING

static_assert(Rectangle{Size{5, 8}}.to<float>() == Rectangle{0.f, 0.f, 5.f, 8.f});
static_assert(Rectangle{Point{9, 1}, Point{11, 6}} == Rectangle{9, 1, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.get_top_left() == Point{9, 1});
static_assert(Rectangle{9, 1, 11, 6}.get_top_right() == Point{11, 1});
static_assert(Rectangle{9, 1, 11, 6}.get_bottom_right() == Point{11, 6});
static_assert(Rectangle{9, 1, 11, 6}.get_bottom_left() == Point{9, 6});
static_assert(Rectangle{9, 1, 11, 6}.set_top_left<false>(Point{-2, -2}) == Rectangle{-2, -2, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.set_top_left(Point{-2, -2}) == Rectangle{-2, -2, 0, 3});
static_assert(Rectangle{9, 1, 11, 6}.move_top_left(Vector2d{-2, -3}) == Rectangle{7, -2, 11, 6});
static_assert(!Rectangle{3, 4, 18, 9}.contains(Rectangle{3, 4, 18, 9}));
static_assert(!Rectangle{3.f, 4.f, 18.f, 9.f}.contains(Rectangle{3.f, 4.f, 18.f, 9.f}));
static_assert(!Rectangle{3, 4, 18, 9}.contains(Rectangle{3.1f, 4.f, 18.f, 9.f}));
static_assert(Rectangle{3, 4, 18, 9}.contains(Rectangle{3.1f, 4.1f, 17.9f, 8.9f}));
static_assert(Rectangle{-10, -4, 1, -1}.intersects(Rectangle{0, -2, 100, 128}));
static_assert(!Rectangle{-10, -4, 1, -1}.intersects(Rectangle{1, -1, 100, 128}));
static_assert(square<Rectangle>(5.f) == Rectangle{0.f, 0.f, 5.f, 5.f});
static_assert(with_negative_space_clipped(Rectangle{4.f, 4.5f, 3.8f, 4.7f}) == Rectangle{4.f, 4.5f, 4.f, 4.7f});
static_assert(with_negative_space_clipped(Rectangle{4.f, 4.5f, 3.8f, 4.f}) == Rectangle{Point{4.f, 4.5f}});
static_assert(Rectangle{2, 3, 4, 5} + Size{3, 1} == Rectangle{2, 3, 7, 6});

#endif

//------------------------------

/*
	Cubic bezier animation easing.
	Try out this, can be useful for making your easing curves:
	http://bjornsundin.com/projects/cubic-bezier-easing

	Ease in example:
	Easing{Point{0.7f, 0.f}, Point{1.f, 1.f}}.ease_value(x)
	Ease out example:
	Easing{Point{0.f, 0.f}, Point{0.3f, 1.f}}.ease_value(x)
	See Easing::ease_value() for more info.

	Storing Easing objects in a Theme can be a good idea because you can use the same easings within your whole
	application, or different parts of it.
*/
struct Easing {
	Point<> c0, c1;

	constexpr auto operator==(Easing const&) const noexcept -> bool = default;

	static constexpr auto default_precision = 5e-3f;

	/*
		Transforms a normalized value according to a cubic bezier curve.
		c0 is the first control point and c1 is the second one.
		precision is the maximum amount of error in the output value.

		It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
		f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (value is x) and not t. This why we have a precision parameter.
	*/
	static constexpr auto ease_value(
		Point<> const c0, Point<> const c1, 
		float const value, float const precision = default_precision
	) noexcept -> float {
		constexpr auto extreme_value_threshold = 1e-5f;
		
		if (value <= extreme_value_threshold) {
			return 0.f;
		}
		if (value >= 1.f - extreme_value_threshold) {
			return 1.f;
		}

		auto t = value < 0.5f ? 0.25f : 0.75f;

		/*
			f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

			f'(x) = x0*(3 - 12*t + 9*t*t) + x1*(6*t - 9*t*t) + 3*t*t
					= x0*9*(t - 1)*(t - 1/3) + t*(x1*(6 - 9*t) + 3*t)
		*/

		auto error = 1.f;
		while (abs(error) > precision) {
			error = value - t * ((1.f - t) * (3.f * (1.f - t) * c0.x + 3.f * t * c1.x) + t * t);
			t += error / (c0.x * 9.f * (t - 1.f) * (t - 1.f / 3.f) + t * (c1.x * (6.f - 9.f * t) + 3.f * t));
		}

		return t * ((1.f - t) * (3.f * (1.f - t) * c0.y + 3.f * t * c1.y) + t * t);
	}

	auto ease_value(float const value, float const precision = default_precision) const noexcept
		-> float
	{
		return ease_value(c0, c1, value, precision);
	}
	auto ease_value_inverse(float const value, float const precision = default_precision) const noexcept {
		return ease_value(Point{c0.y, c0.x}, Point{c1.y, c1.x}, value, precision);
	}
};

} // namespace math

//------------------------------

/*
	ARGB formatted 32-bit packed color, where every channel has 8 bits.
*/
using ColorInt = std::uint32_t;

[[nodiscard]]
constexpr inline auto get_red_channel(ColorInt const color) noexcept -> std::uint8_t {
	return color >> 16 & 0xff;
}
[[nodiscard]]
constexpr inline auto get_green_channel(ColorInt const color) noexcept -> std::uint8_t {
	return color >> 8 & 0xff;
}
[[nodiscard]]
constexpr inline auto get_blue_channel(ColorInt const color) noexcept -> std::uint8_t {
	return color & 0xff;
}
[[nodiscard]]
constexpr inline auto get_alpha_channel(ColorInt const color) noexcept -> std::uint8_t {
	return color >> 24 & 0xff;
}

/*
	Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
	This means that a Color object is 4 times as big as a packed 32-bit color, but allows for more 
	precise and efficient operations.
*/
struct Color {
	float red{0.f},
          green{0.f},
          blue{0.f},
          alpha{1.f};
	
	constexpr Color() noexcept = default;
	/*
		The channels are clamped to the range [0, 1].
	*/
	constexpr Color(float const p_red, float const p_green, float const p_blue, float const p_alpha = 1.f) noexcept :
		red{std::clamp(p_red, 0.f, 1.f)}, 
		green{std::clamp(p_green, 0.f, 1.f)}, 
		blue{std::clamp(p_blue, 0.f, 1.f)}, 
		alpha{std::clamp(p_alpha, 0.f, 1.f)}
	{}
	/*
		The channels are in the range [0, 255]
	*/
	constexpr Color(
		std::uint8_t const p_red, 
		std::uint8_t const p_green, 
		std::uint8_t const p_blue, 
		std::uint8_t const p_alpha = static_cast<std::uint8_t>(255)
	) noexcept :
		red{p_red / 255.f},
		green{p_green / 255.f},
		blue{p_blue / 255.f},
		alpha{p_alpha / 255.f}
	{}
	/*
		The channels are clamped to the range [0, 255]
	*/
	template<std::integral T>
	constexpr Color(T const p_red, T const p_green, T const p_blue, T const p_alpha = static_cast<T>(255)) noexcept :
		red{std::clamp(p_red / 255.f, 0.f, 1.f)},
		green{std::clamp(p_green / 255.f, 0.f, 1.f)},
		blue{std::clamp(p_blue / 255.f, 0.f, 1.f)},
		alpha{std::clamp(p_alpha / 255.f, 0.f, 1.f)}
	{}

	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 1].
	*/
	constexpr Color(float const lightness, float const p_alpha = 1.f) noexcept :
		red{std::clamp(lightness, 0.f, 1.f)},
		green{std::clamp(lightness, 0.f, 1.f)},
		blue{std::clamp(lightness, 0.f, 1.f)},
		alpha{std::clamp(p_alpha, 0.f, 1.f)}
	{}
	/*
		Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
	*/
	constexpr Color(std::uint8_t const lightness, std::uint8_t const p_alpha = 255u) noexcept :
		red{lightness/255.f},
		green{red},
		blue{red},
		alpha{p_alpha/255.f}
	{}
	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 255].
	*/
	template<std::integral T>
	constexpr Color(T const lightness, T const p_alpha = static_cast<T>(255)) noexcept :
		red{std::clamp(lightness / 255.f, 0.f, 1.f)},
		green{red},
		blue{red},
		alpha{std::clamp(p_alpha / 255.f, 0.f, 1.f)}
	{}

	/*
		Creates a copy of another color but with a new alpha.
	*/
	constexpr Color(Color const color, float const p_alpha) noexcept :
		red{color.red}, 
		green{color.green}, 
		blue{color.blue}, 
		alpha{std::clamp(p_alpha, 0.f, 1.f)}
	{}
	/*
		Creates a copy of another color but with a new alpha.
	*/
	constexpr Color(Color const color, std::uint8_t const p_alpha) noexcept :
		red{color.red / 255.f},
		green{color.green / 255.f},
		blue{color.blue / 255.f},
		alpha{p_alpha / 255.f}
	{}
	/*
		Creates a copy of another color but with a new alpha.
	*/
	template<std::integral T>
	constexpr Color(Color const color, T const p_alpha) noexcept :
		red{color.red},
		green{color.green},
		blue{color.blue},
		alpha{std::clamp(p_alpha / 255.f, 0.f, 1.f)}
	{}

	/*
		Initializes with a 4-byte packed RGBA color.
	*/
	constexpr Color(ColorInt const color) noexcept :
		red{get_red_channel(color) / 255.f},
		green{get_green_channel(color) / 255.f},
		blue{get_blue_channel(color) / 255.f},
		alpha{get_alpha_channel(color) / 255.f}
	{}

	constexpr auto operator=(ColorInt const color) noexcept -> Color& {
		return *this = Color{color};
	}

	constexpr auto operator*(float const factor) const noexcept -> Color {
		return Color{red * factor, green * factor, blue * factor, alpha};
	}
	constexpr auto operator*=(float const factor) noexcept -> Color& {
		return *this = *this * factor;
	}
	constexpr auto operator/(float const divisor) const noexcept -> Color {
		return {red/divisor, green/divisor, blue/divisor, alpha};
	}
	constexpr auto operator/=(float const divisor) noexcept -> Color& {
		return *this = *this / divisor;
	}
};

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
	using value_type = std::uint64_t;
	
private:
	static value_type s_counter;
	value_type _count;

public:
	[[nodiscard]]
	constexpr operator value_type() const noexcept {
		return _count;
	}
	[[nodiscard]]
	constexpr auto operator==(Id const& id) const noexcept -> bool = default;

	constexpr explicit Id(value_type const id) noexcept :
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

	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::operator+=.
	*/
	auto add(std::function<FunctionType> listener) -> void {
		auto const lock = std::scoped_lock{_mutex};    
		_listeners.emplace_back(std::move(listener));
	}
	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::add.
	*/
	auto operator+=(std::function<FunctionType> listener) -> EventListeners& {
		add(std::move(listener));
		return *this;
	}

	/*
		Removes a listener from the EventListeners instance that matches the passed function.
		Equivalent to EventListeners::operator-=.
	*/
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
	/*
		Removes a listener from the EventListeners instance that matches the passed function.
		Equivalent to EventListeners::remove.
	*/
	auto operator-=(std::function<FunctionType> const& listener) -> EventListeners& {
		remove(listener);
		return *this;
	}

	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::operator().
	*/
	auto notify_all(_Arguments&& ... event_arguments) -> void {
		auto const lock = std::scoped_lock{_mutex};
		for (auto& listener : _listeners) {
			listener(std::forward<_Arguments>(event_arguments)...);
		}
	}
	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::notify_all.
	*/
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

//------------------------------



} // namespace avo
