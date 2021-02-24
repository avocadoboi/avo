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
#include <any>
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
#include <memory>
#include <mutex>
#include <numbers>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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
class Range : public std::ranges::view_interface<Range<_Value, is_reverse>> {
public:
	using value_type = _Value;

	class Iterator {
	public:
		using value_type = std::remove_cv_t<_Value>;
		using difference_type = value_type;
		using iterator_concept = std::random_access_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;

		constexpr Iterator& operator++() noexcept {
			if constexpr (is_reverse) {
				--_current_value;
			}
			else {
				++_current_value;
			}
			return *this;
		}
		constexpr Iterator operator++(int) noexcept {
			if constexpr (is_reverse) {
				return Iterator{_current_value--};
			}
			else {
				return Iterator{_current_value++};
			}
		}

		[[nodiscard]]
		constexpr Iterator operator+(difference_type const offset) const noexcept {
			if constexpr (is_reverse) {
				return _current_value - offset;
			}
			else {
				return _current_value + offset;
			}
		}
		constexpr Iterator& operator+=(difference_type const offset) noexcept {
			if constexpr (is_reverse) {
				_current_value -= offset;
			}
			else {
				_current_value += offset;
			}
			return *this;
		}

		constexpr Iterator operator--(int) noexcept {
			if constexpr (is_reverse) {
				return Iterator{_current_value++};
			}
			else {
				return Iterator{_current_value--};
			}
		}
		constexpr Iterator& operator--() noexcept {
			if constexpr (is_reverse) {
				++_current_value;
			}
			else {
				--_current_value;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr Iterator operator-(difference_type const offset) const noexcept {
			if constexpr (is_reverse) {
				return _current_value + offset;
			}
			else {
				return _current_value - offset;
			}        
		}
		constexpr Iterator& operator-=(difference_type const offset) noexcept {
			if constexpr (is_reverse) {
				_current_value += offset;
			}
			else {
				_current_value -= offset;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator*() const noexcept {
			return _current_value;
		}

		[[nodiscard]]
		constexpr value_type operator[](difference_type const offset) const noexcept {
			return static_cast<value_type>(_current_value + offset);
		}

		[[nodiscard]]
		constexpr auto operator<=>(Iterator const& other) const noexcept = default;

		constexpr Iterator() noexcept = default;
		constexpr Iterator(_Value const value) noexcept :
			_current_value{value}
		{}

	private:
		value_type _current_value;
	};

	[[nodiscard]]
	constexpr Range<_Value, !is_reverse> reverse() const noexcept {
		return {*(_end - 1), *_start};
	}

	[[nodiscard]]
	constexpr Iterator begin() const noexcept{
		return _start;
	}

	[[nodiscard]]
	constexpr Iterator end() const noexcept {
		return _end;
	}

	constexpr bool operator==(Range const&) const noexcept = default;

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

private:
	Iterator _start;
	Iterator _end;
};

} // namespace utils

} // namespace avo

namespace std::ranges {

template<std::integral T, bool is_reversed>
constexpr auto enable_borrowed_range<avo::utils::Range<T, is_reversed>> = true;

} // namespace std::ranges

namespace avo {
	
namespace utils {

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
static_assert(
	std::ranges::equal(
		Range{-5, 3} | std::views::transform([](int i){ return i*2; }) | std::views::reverse,
		std::array{6, 4, 2, 0, -2, -4, -6, -8, -10}
	),
	"avo::utils::Range with standard ranges library does not work."
);
static_assert(
	std::ranges::equal(
		Range{-5, 3} | std::views::reverse,
		std::array{3, 2, 1, 0, -1, -2, -3, -4, -5}
	),
	"avo::utils::Range with std::views::reverse does not work."
);
#endif // BUILD_TESTING

//------------------------------

/*
	Takes any range and returns a range containing the indices of the elements of the original range.
*/
[[nodiscard]]
constexpr Range<std::size_t> indices(std::ranges::sized_range auto&& range) {
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
struct EnumeratedElement final {
	std::size_t index;
	T& element;
};

/*
	Takes any range and returns a new range of (index, element) pairs referring to the original range.
	The original range isn't moved anywhere.
*/
[[nodiscard]]
constexpr std::ranges::view auto enumerate(std::ranges::range auto& range) 
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
constexpr std::ranges::range auto enumerate(std::ranges::range auto&& range);

/*
	A range of (index, element) pairs referring to the elements of an owned range.
	The original range is moved into and owned by the enumerated range.
*/
template<std::ranges::range T> requires std::movable<T>
class EnumeratedRange final {
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
		
		constexpr Iterator operator++(int) {
			return Iterator{++_base_iterator, ++_index};
		}
		constexpr Iterator& operator++() {
			++_base_iterator;
			++_index;
			return *this;
		}

		[[nodiscard]]
		constexpr bool operator==(Iterator const& other) const noexcept {
			return _base_iterator == other._base_iterator;
		}

		[[nodiscard]]
		constexpr value_type operator*() const {
			return EnumeratedElement{_index, *_base_iterator};
		}

		constexpr Iterator() = default;
		constexpr Iterator(_BaseIterator const base_iterator, std::size_t const index) :
			_base_iterator{base_iterator},
			_index{index}
		{}

	private:
		_BaseIterator _base_iterator;
		std::size_t _index;
	};

	[[nodiscard]]
	constexpr Iterator begin() const {
		return Iterator{std::begin(_range), std::size_t{}};
	}
	[[nodiscard]]
	constexpr Iterator end() const {
		return Iterator{std::end(_range), std::size_t{}};
	}

	friend constexpr std::ranges::range auto enumerate<>(T&& range);

private:
	constexpr EnumeratedRange(T&& range) noexcept :
		_range{std::forward<T>(range)}
	{}
	constexpr EnumeratedRange() = default;
};

constexpr std::ranges::range auto enumerate(std::ranges::range auto&& range) {
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
#endif // BUILD_TESTING

//------------------------------

template<typename T, bool has_parent_reference = false>
concept IsRecursiveRange = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, std::remove_cvref_t<T>>
	&& (!has_parent_reference || 
		std::random_access_iterator<std::ranges::iterator_t<T>> &&
		(IsAnyOf<decltype(T::parent), T&, T*> || 
		requires(T range) { { range.parent() } -> IsAnyOf<T&, T*>; }));

template<typename T>
concept IsRecursiveIterator = IsRecursiveRange<std::iter_value_t<T>>;

[[nodiscard]]
constexpr bool is_recursive_iterator_empty(IsRecursiveIterator auto const iterator) {
	return std::begin(*iterator) == std::end(*iterator);
}

template<IsRecursiveRange T>
class FlattenedView : std::ranges::view_interface<FlattenedView<T>> {
public:
	using BaseIterator = std::ranges::iterator_t<T>;

	class Iterator {
	public:
		using value_type = std::iter_value_t<BaseIterator>;
		using reference = std::iter_reference_t<BaseIterator>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
		using iterator_concept = std::input_iterator_tag;
	
		Iterator& operator++() {
			if (std::holds_alternative<T*>(_current_position)) {
				_current_position = std::begin(*std::get<T*>(_current_position));
			}
			else {
				_increment_iterator();
			}
			return *this;
		}
		Iterator operator++(int) {
			auto previous = *this;
			++*this;
			return previous;
		}

		[[nodiscard]]
		reference operator*() const {
			return std::holds_alternative<T*>(_current_position) ?
				*std::get<T*>(_current_position) :
				*std::get<BaseIterator>(_current_position);
		}

		[[nodiscard]]
		bool operator==(std::default_sentinel_t) const noexcept {
			return std::holds_alternative<BaseIterator>(_current_position) && 
				std::get<BaseIterator>(_current_position) == _end;
		}
	
		Iterator() = default;
		Iterator(T* const range) :
			_current_position{range},
			_end{std::end(*range)}
		{}

	private:
		void _increment_iterator() {
			auto& pos = std::get<BaseIterator>(_current_position);
			if (pos == _end) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				++pos;
				while (!_parent_stack.empty() && pos == std::end(*_parent_stack.top())) {
					pos = _parent_stack.top();
					_parent_stack.pop();
					++pos;
				}
			}
			else {
				_parent_stack.push(pos);
				pos = std::begin(*pos);				
			}
		}
		
		std::variant<T*, BaseIterator> _current_position;
		BaseIterator _end;
		std::stack<BaseIterator> _parent_stack;
	};

	[[nodiscard]]
	Iterator begin() const {
		return Iterator{_range};
	}
	[[nodiscard]]
	std::default_sentinel_t end() const noexcept {
		return {};
	}

	FlattenedView() noexcept = default;
	explicit FlattenedView(T& range) noexcept :
		_range{&range}
	{}

private:
	T* _range{};
};

template<IsRecursiveRange T> requires IsRecursiveRange<T, true>
class FlattenedView<T> : std::ranges::view_interface<FlattenedView<T>> {
public:
	using BaseIterator = std::ranges::iterator_t<T>;

	class Iterator {
	public:
		using value_type = std::iter_value_t<BaseIterator>;
		using reference = std::iter_reference_t<BaseIterator>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
		using iterator_concept = std::input_iterator_tag;
	
		Iterator& operator++() {
			if (std::holds_alternative<T*>(_current_position)) {
				_current_position = std::begin(*std::get<T*>(_current_position));
			}
			else {
				_increment_iterator();
			}
			return *this;
		}
		Iterator operator++(int) {
			auto previous = *this;
			++*this;
			return previous;
		}

		[[nodiscard]]
		reference operator*() const {
			return std::holds_alternative<T*>(_current_position) ?
				*std::get<T*>(_current_position) :
				*std::get<BaseIterator>(_current_position);
		}

		[[nodiscard]]
		bool operator==(std::default_sentinel_t) const noexcept {
			return std::holds_alternative<BaseIterator>(_current_position) && 
				std::get<BaseIterator>(_current_position) == _end;
		}
	
		Iterator() = default;
		Iterator(T* const range) :
			_current_position{range},
			_end{std::end(*range)}
		{}

	private:
		static T* get_parent_of_node(T& node) noexcept 
			requires std::same_as<decltype(T::parent), T&> 
		{
			return &node.parent;
		}
		static T* get_parent_of_node(T& node) noexcept 
			requires std::same_as<decltype(T::parent), T*> 
		{
			return node.parent;
		}
		static T* get_parent_of_node(T& node) noexcept 
			requires requires { { node.parent() } -> std::same_as<T&>; } 
		{
			return &node.parent();
		}
		static T* get_parent_of_node(T& node) noexcept 
			requires requires { { node.parent() } -> std::same_as<T*>; } 
		{
			return node.parent();
		}
	
		static BaseIterator get_iterator_of_node(T& node) {
			auto* const parent = get_parent_of_node(node);
			return std::begin(*parent) + (&node - &*std::begin(*parent));
		}
	
		void _increment_iterator() {
			auto& pos = std::get<BaseIterator>(_current_position);
			if (pos == _end) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				auto* parent = get_parent_of_node(*pos);
				++pos;
				while (pos != _end && pos == std::end(*parent)) {
					pos = get_iterator_of_node(*parent);
					parent = get_parent_of_node(*pos);
					++pos;
				}
			}
			else {
				pos = std::begin(*pos);
			}
		}
		
		std::variant<T*, BaseIterator> _current_position;
		BaseIterator _end;
	};

	[[nodiscard]]
	Iterator begin() const {
		return Iterator{_range};
	}
	[[nodiscard]]
	std::default_sentinel_t end() const noexcept {
		return {};
	}

	FlattenedView() noexcept = default;
	explicit FlattenedView(T& range) noexcept :
		_range{&range}
	{}

private:
	T* _range{};
};

} // namespace utils

} // namespace avo

namespace std::ranges {

template<typename T>
constexpr auto enable_borrowed_range<avo::utils::FlattenedView<T>> = true;

} // namespace std::ranges

namespace avo {
	
namespace utils {

constexpr auto flatten = [](IsRecursiveRange auto& range) {
	return FlattenedView{range};
};

template<IsRecursiveRange T>
FlattenedView<T> operator|(T& range, decltype(flatten) const) {
	return FlattenedView{range};
}

//------------------------------

/*
	Removes all elements matching the value argument from a vector, without keeping the order of the elements.
*/
template<std::equality_comparable T>
std::vector<T>& unordered_erase(std::vector<T>& vector, T const& value) {
	for (auto pos = vector.begin(); 
		(pos = std::ranges::find(pos, vector.end(), value)) != vector.end();)
	{
		*pos = std::move(vector.back());
		vector.pop_back();
	}
	return vector;
}

//------------------------------

/*
	This can be called when the program reaches a path that should never be reachable.
	It prints error output and exits the program.
*/
#ifdef __cpp_lib_source_location
[[noreturn]]
inline void unreachable(std::source_location const& source_location = std::source_location::current()) {
	// TODO: use std::format when supported
	// std::cerr << std::format("Reached an unreachable code path in file {}, in function {}, on line {}.", 
	// 	source_location.file_name(), source_location.function_name(), source_location.line());
	std::cerr << "Reached an unreachable code path in file " << source_location.file_name() << 
		", in function " << source_location.function_name() << ", on line " << source_location.line() << ".\n";
	std::exit(1);
}
#else
[[noreturn]]
inline void unreachable() {
	std::cerr << "Reached an unreachable code path, exiting.\n";
	std::exit(1);
}
#endif

//------------------------------

/*
	Used to invoke a lambda at the end of a scope.
*/
template<std::invocable T>
class [[nodiscard]] Cleanup final {
public:
	Cleanup(Cleanup&&) noexcept = delete;
	Cleanup& operator=(Cleanup&&) noexcept = delete;

	Cleanup(Cleanup const&) = delete;
	Cleanup& operator=(Cleanup const&) = delete;

	[[nodiscard]] 
	Cleanup(T&& callable) :
		_callable{std::forward<T>(callable)}
	{}

	Cleanup() = delete;
	~Cleanup() {
		_callable();
	}

private:
	T _callable;
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
class UniqueHandle final {
	_Type _handle{invalid_handle};

	void close() {
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
	_Type get() const {
		return _handle;
	}
	[[nodiscard]]
	_Type& get() {
		return _handle;
	}

	[[nodiscard]]
	_Type const* operator->() const {
		return &_handle;
	}
	[[nodiscard]]
	_Type* operator->() {
		return &_handle;
	}

	[[nodiscard]]
	_Type const* operator&() const {
		return &_handle;
	}
	[[nodiscard]]
	_Type* operator&() {
		return &_handle;
	}

	[[nodiscard]]
	explicit operator bool() const {
		return _handle != invalid_handle;
	}
	[[nodiscard]]
	bool operator!() const {
		return _handle == invalid_handle;
	}

	explicit UniqueHandle(_Type handle) :
		_handle{handle}
	{}
	UniqueHandle& operator=(_Type handle) {
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
	UniqueHandle& operator=(UniqueHandle&& handle) noexcept {
		_handle = handle._handle;
		handle._handle = invalid_handle;
		return *this;
	}

	UniqueHandle(UniqueHandle const&) = delete;
	UniqueHandle& operator=(UniqueHandle const&) = delete;
};

//------------------------------

using DataVector = std::vector<std::byte>;
using DataView = std::span<std::byte const>;
using DataRange = std::span<std::byte>;

[[nodiscard]] 
inline DataVector read_file(std::string const path) {
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

			constexpr bool get() const {
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

			constexpr bool get() {
				return b;
			}
		};
		auto instance = Test{true};
		auto const get_function = bind(&Test::get, &instance);
		return get_function() == true;
	}(),
	"avo::utils::bind does not work with mutable objects."
);
#endif // BUILD_TESTING

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
void enable_utf8_console();

//------------------------------

/*
	Converts a UTF-8 encoded char string to a UTF-16 encoded char16 string.
	Returns the length of the converted string, in code point units (char16_t).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
std::optional<std::size_t> utf8_to_utf16(std::string_view input, std::span<char16_t> output);
/*
	Converts a UTF-8 encoded string to a UTF-16 encoded std::u16string.
*/
[[nodiscard]]
std::u16string utf8_to_utf16(std::string_view input);

/*
	Converts a UTF-16 encoded char16 string to a UTF-8 encoded char string.
	Returns the length of the converted string, in code point units (char).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
std::optional<std::size_t> utf16_to_utf8(std::u16string_view input, std::span<char> output);
/*
	Converts a UTF-16 char16 string to a UTF-8 encoded std::string.
*/
[[nodiscard]]
std::string utf16_to_utf8(std::u16string_view input);

//------------------------------

/*
	Returns the number of UTF-8 code points a character starting with 
	first_code_point_in_character consists of in total.
	Returns 0 if the code point is not the first one in a character.
	Returns -1 if the code point is an invalid UTF-8 code point.
*/
[[nodiscard]]
constexpr int code_point_count(char const first_code_point_in_character) noexcept {
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
constexpr int code_point_count(char16_t const first_code_point_in_character) noexcept {
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
constexpr bool is_first_code_point(char const code_point) noexcept {
	return (code_point & 0xc0) != 0x80;
}

/*
	Returns whether p_unit is the start of a UTF-16 encoded character
*/
[[nodiscard]]
constexpr bool is_first_code_point(char16_t const code_point) noexcept {
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
constexpr std::size_t code_point_index(std::basic_string_view<T> const string, std::size_t const character_index) 
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
constexpr std::size_t character_index(std::basic_string_view<T> const string, std::size_t const code_point_index) 
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
constexpr std::size_t character_count(std::basic_string_view<T> const string) {
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
	code_point_index(std::string_view{"🪢 här √ är knut"}, 10) == 17 &&
	code_point_index(std::string_view{"🪢 här 🪢 är knut"}, 10) == 18, 
	"code_point_index does not work correctly with UTF-8."
);
static_assert(
	code_point_index(std::u16string_view{u"🪢 här √ är knut"}, 10) == 11 &&
	code_point_index(std::u16string_view{u"🪢 här 🪢 är knut"}, 10) == 12, 
	"code_point_index does not work correctly with UTF-16."
);
static_assert(
	character_index(std::string_view{"🪢 här √ är knut"}, 17) == 10 &&
	character_index(std::string_view{"🪢 här 🪢 är knut"}, 18) == 10, 
	"character_index does not work correctly with UTF-8."
);
static_assert(
	character_index(std::u16string_view{u"🪢 här √ är knut"}, 11) == 10 &&
	character_index(std::u16string_view{u"🪢 här 🪢 är knut"}, 12) == 10, 
	"character_index does not work correctly with UTF-16."
);
#endif // BUILD_TESTING

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
constexpr bool operator==(_Class<A> const first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator+=(_Class<A>& first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator-=(_Class<A>& first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator*=(_Class<A>& first, B const second) noexcept {
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
constexpr _Class<A>& operator/=(_Class<A>& first, B const second) noexcept {
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
constexpr Degrees<_To> to_degrees(Radians<_From> const radians) noexcept {
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
constexpr Degrees<_To> to_degrees(Degrees<_From> const degrees) noexcept {
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
constexpr Radians<_To> to_radians(Degrees<_From> const degrees) noexcept {
	return Radians{static_cast<_To>(degrees.value / static_cast<_To>(180) * std::numbers::pi_v<_To>)};
}
/*
	Converts an angle to radians.
*/
template<std::floating_point _To, std::floating_point _From>
[[nodiscard]]
constexpr Radians<_To> to_radians(Radians<_From> const radians) noexcept {
	return Radians{static_cast<_To>(radians.value)};
}

/*
	Converts an angle to another angle type.
*/
template<IsRadians _To>
[[nodiscard]]
constexpr _To angle_as(IsAngle auto const angle) noexcept {
	return to_radians<typename _To::value_type>(angle);
}

/*
	Converts an angle to another angle type.
*/
template<IsDegrees _To>
[[nodiscard]]
constexpr _To angle_as(IsAngle auto const angle) noexcept {
	return to_degrees<typename _To::value_type>(angle);
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Degrees<T> const degrees) noexcept {
	return degrees.value / 360;
}
template<std::floating_point _Return, std::integral T>
[[nodiscard]]
constexpr _Return normalized(Degrees<T> const degrees) noexcept {
	return static_cast<_Return>(degrees.value) / 360;
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Radians<T> const radians) noexcept {
	return radians.value / (std::numbers::pi_v<T>*2);
}

inline namespace angle_literals {

constexpr Degrees<double> operator"" _deg(long double const value) noexcept {
	return Degrees{static_cast<double>(value)};
}
constexpr Degrees<float> operator"" _degf(long double const value) noexcept {
	return Degrees{static_cast<float>(value)};
}
constexpr Degrees<int> operator"" _deg(unsigned long long const value) noexcept {
	return Degrees{static_cast<int>(value)};
}
constexpr Radians<double> operator"" _rad(long double const value) noexcept {
	return Radians{static_cast<double>(value)};
}
constexpr Radians<float> operator"" _radf(long double const value) noexcept {
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
static_assert(normalized<float>(Degrees{90}) == 0.25f);
static_assert(normalized(Degrees{90.f}) == 0.25f);
static_assert(normalized(Radians{std::numbers::pi_v<float>/2}) == 0.25f);
#endif // BUILD_TESTING

//------------------------------

template<std::floating_point T>
[[nodiscard]]
constexpr bool approximately_equal(T const a, T const b, T const max_difference = static_cast<T>(1e-6))  
{
	return std::abs(a - b) <= max_difference;
}

/*
	Returns 1 if the number is positive, 0 if it is 0 and -1 if it is negative.
*/
template<utils::IsNumber T>
[[nodiscard]]
constexpr T sign(T const number) {
	return std::copysign(T{1}, number);
}

template<std::floating_point T>
[[nodiscard]]
constexpr T unit_clamp(T const value) {
	return std::clamp(value, T{}, T{1});
}

template<utils::IsNumber _Return, utils::IsNumber T>
[[nodiscard]]
constexpr _Return floor(T const number) {
	if (std::is_constant_evaluated()) {
		return static_cast<_Return>(static_cast<std::int64_t>(number) - static_cast<std::int64_t>(number < 0));
	}
	else {
		return static_cast<_Return>(std::floor(number));
	}
}
template<utils::IsNumber _Return, utils::IsNumber T>
[[nodiscard]]
constexpr _Return ceil(T const number) {
	if (std::is_constant_evaluated()) {
		return static_cast<_Return>(static_cast<std::int64_t>(number) + static_cast<std::int64_t>(number > 0));
	}
	else {
		return static_cast<_Return>(std::ceil(number));
	}
}

template<utils::IsNumber T>
[[nodiscard]]
constexpr T abs(T const number) {
	if (std::is_constant_evaluated()) {
		return number >= T{} ? number : -number;
	}
	else {
		return std::abs(number);
	}
}

/*
	Returns a number multiplied by itself.
	Can be useful if you want to quickly square a longer expression.
*/
template<utils::IsNumber T>
[[nodiscard]]
constexpr T square(T const x) noexcept {
	return x*x;
}

/*
	Returns the inverse square root of a float using a faster but less accurate algorithm.
	It is about 8% to 15% faster than 1.f/std::sqrt(x) with gcc -O3 on my computer.
*/
[[nodiscard]]
inline float fast_inverse_sqrt(float const input) noexcept 
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
std::pair<_Return, _Return> cos_sin(IsAngle auto angle) {
	auto const radians = to_radians<_Return>(angle);
	return std::pair{std::cos(radians.value), std::sin(radians.value)};
}

template<typename T>
[[nodiscard]]
constexpr decltype(auto) max(T&& value) {
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
constexpr decltype(auto) max(T0&& first, T1&& second, T2&& ... arguments) 
{
	return (first > second) ? 
		max(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		max(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

template<typename T>
[[nodiscard]]
constexpr decltype(auto) min(T&& value) {
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
constexpr decltype(auto) min(T0&& first, T1&& second, T2&& ... arguments) 
{
	return (first < second) ? 
		min(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		min(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

#ifdef BUILD_TESTING
static_assert(floor<double>(-4.5) == -5. && floor<int>(-4.4) == -5);
static_assert(floor<double>(4.5) == 4. && floor<int>(4.7) == 4);
static_assert(ceil<double>(-4.5) == -4. && ceil<int>(-4.4) == -4);
static_assert(ceil<double>(4.5) == 5. && ceil<int>(4.7) == 5);

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
#endif // BUILD_TESTING

//------------------------------

/*
	A random number generator, a small abstraction on top of a subset 
	of the standard library random utilities.
*/
class Random final {
public:
	/*
		Generates a new uniformly distributed random floating point number in the range [min, max).
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next(T const min = T{}, T const max = T{1}) {
		return std::uniform_real_distribution<T>{min, max}(_engine);
	}
	/*
		Generates a new uniformly distributed random integer in the range [min, max].
	*/
	template<std::integral T> requires (!std::same_as<T, bool>)
	[[nodiscard]]
	T next(T const min = T{}, T const max = T{1}) {
		return std::uniform_int_distribution<T>{min, max}(_engine);
	}
	/*
		Returns a random coin flip, true or false.
	*/
	template<std::same_as<bool> T>
	[[nodiscard]]
	T next() {
		return static_cast<T>(std::uniform_int_distribution{0, 1}(_engine));
	}
	/*
		Generates a new random floating point number distributed according to a gaussian distribution
		with a mean and a standard deviation.
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next_normal(T const mean, T const standard_deviation) {
		return std::normal_distribution<T>{mean, standard_deviation};
	}

	Random(std::uint_fast32_t const seed) :
		_engine{seed}
	{}
	Random() = default;

private:
	std::default_random_engine _engine{std::random_device{}()};
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

std::ostream& operator<<(std::ostream& stream, Is2dVector auto const vector) {
	stream << '(' << vector.x << ", " << vector.y << ')';
	return stream;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr bool operator==(_Class<A> const first, _Class<B> const second) noexcept {
	return first.x == second.x && first.y == second.y;
}

template<utils::IsNumber A, utils::IsNumber B, template<typename> typename _Class> requires Is2dVectorTemplate<_Class>
[[nodiscard]]
constexpr std::partial_ordering operator<=>(_Class<A> const first, _Class<B> const second) noexcept 
{
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
constexpr _Class<A>& operator+=(_Class<A>& first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator-=(_Class<A>& first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator*=(_Class<A>& first, B const second) noexcept {
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
constexpr _Class<A>& operator*=(_Class<A>& first, _Class<B> const second) noexcept {
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
constexpr _Class<A>& operator/=(_Class<A>& first, B const second) noexcept {
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
constexpr _Class<A>& operator/=(_Class<A>& first, _Class<B> const second) noexcept {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

/*
	Creates a square 2d vector, that is a vector with both coordinates equal.
*/
template<template<typename> typename _Vector, utils::IsNumber _Value> requires Is2dVectorTemplate<_Vector>
constexpr _Vector<_Value> square(_Value const side_length) {
	return _Vector{side_length, side_length};
}

template<std::floating_point T, template<typename> typename _Vector> requires Is2dVectorTemplate<_Vector>
constexpr _Vector<T> interpolate(_Vector<T> const a, _Vector<T> const b, T const c) {
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
constexpr _Vector<T> with_negative_space_clipped(_Vector<T> vector) noexcept {
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
T normalized(T vector) {
	vector.normalize();
	return vector;
}
/*
	Returns a vector that has the same angle as the argument but with a length of 1, using a slightly faster algorithm.
*/
template<Is2dVector T>
[[nodiscard]]
T normalized_fast(T vector) {
	vector.normalize_fast();
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise.
*/
template<Is2dVector T>
[[nodiscard]]
T rotated(T vector, IsAngle auto const angle) {
	vector.rotate(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
T rotated(T vector, IsAngle auto const angle, Is2dVector auto const origin) {
	vector.rotate(angle, origin);
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise.
*/
template<Is2dVector T>
[[nodiscard]]
T with_angle(T vector, IsAngle auto const angle) {
	vector.angle(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise
	relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
T with_angle(T vector, IsAngle auto const angle, Is2dVector auto const origin) {
	vector.angle(angle, origin);
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
	auto length() const {
		return std::hypot(x, y);
	}
	/*
		Returns the squared magnitude of the vector, or the squared hypotenuse of the triangle.
	*/
	[[nodiscard]]
	constexpr _Value length_squared() const noexcept {
		return x*x + y*y;
	}

	/*
		Sets any negative coordinates to 0.
	*/
	constexpr void clip_negative_space() noexcept {
		x = max(_Value{}, x);
		y = max(_Value{}, y);
	}

	/*
		Rotates the vector by an angle clockwise.
	*/
	void rotate(IsAngle auto const angle) {
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
	void rotate(IsAngle auto const angle, Is2dVector auto const origin) {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const x_before = x;
		x = static_cast<_Value>((x - origin.x)*cos - (y - origin.y)*sin + origin.x);
		y = static_cast<_Value>((y - origin.y)*cos + (x_before - origin.x)*sin + origin.y);
	}

	/*
		Sets the angle of the vector measured anticlockwise from the right side.
	*/
	void angle(IsAngle auto const angle) {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const current_length = length();
		x = static_cast<_Value>(cos*current_length);
		y = static_cast<_Value>(sin*current_length);
	}
	/*
		Sets the angle of the vector measured anticlockwise from the right side relative to an origin.
	*/
	void angle(IsAngle auto const angle, Is2dVector auto const origin) {
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
	_Angle angle() const {
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
	_Angle angle(Is2dVector auto const origin) const {
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
	void normalize() {
		auto const current_length = length();
		x /= current_length;
		y /= current_length;
	}
	/*
		Keeps the angle of the vector but sets its length to 1 using a slightly faster algorithm.
	*/
	void normalize_fast() {
		auto const inverse_length = fast_inverse_sqrt(length_squared());
		x *= inverse_length;
		y *= inverse_length;
	}

	template<Is2dVector _Vector>
	[[nodiscard]]
	constexpr _Vector to() const noexcept {
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
#endif // BUILD_TESTING

//------------------------------

template<std::floating_point T>
struct Transform final {
	using value_type = T;
	
	T x_to_x{1}, y_to_x{0}, offset_x{0},
	  x_to_y{0}, y_to_y{1}, offset_y{0};

	[[nodiscard]]
	constexpr bool operator==(Transform const&) const noexcept = default;

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
	constexpr Transform& operator*=(Transform<U> const& other) const noexcept {
		return *this = other * *this;	
	}

	/*
		Rotates transformed points anticlockwise from the right.
	*/
	Transform& rotate(IsAngle auto const angle) {
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
	Transform& rotate(IsAngle auto const angle, Is2dVector auto const origin) {
		translate(-origin);
		rotate(angle);
		translate(origin);
		return *this;
	}

	/*
		Moves the translation by an offset.
	*/
	constexpr Transform& translate(Is2dVector auto const offset) noexcept {
		offset_x += offset.x;
		offset_y += offset.y;
		return *this;
	}
	/*
		Sets the absolute offset in coordinates caused by the transform.
	*/
	constexpr Transform& translation(Is2dVector auto const point) noexcept {
		offset_x = point.x;
		offset_y = point.y;
		return *this;
	}

	/*
		Scales the transform by a horizontal and vertical factor.
	*/
	constexpr Transform& scale(Is2dVector auto const scale_factor) noexcept {
		x_to_x *= scale_factor.x;
		y_to_x *= scale_factor.x;
		offset_x *= scale_factor.x;
		y_to_y *= scale_factor.y;
		x_to_y *= scale_factor.y;
		offset_y *= scale_factor.y;
		return *this;
	}
	constexpr Transform& scale_x(utils::IsNumber auto const scale_factor) noexcept {
		x_to_x *= scale_factor;
		y_to_x *= scale_factor;
		offset_x *= scale_factor;
		return *this;
	}
	constexpr Transform& scale_y(utils::IsNumber auto const scale_factor) noexcept {
		y_to_y *= scale_factor;
		x_to_y *= scale_factor;
		offset_y *= scale_factor;
		return *this;
	}
};

template<std::floating_point T>
std::ostream& operator<<(std::ostream& stream, Transform<T> const transform) {
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
constexpr Transform<T> inverse(Transform<T> const t) noexcept {
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
constexpr Transform<T> rotated(Transform<T> transform, IsAngle auto const angle) {
	return transform.rotate(angle);
}
/*
	Returns a rotated copy of the Transform argument.
	See Transform::rotate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> rotated(Transform<T> transform, IsAngle auto const angle, Is2dVector auto const origin) {
	return transform.rotate(angle, origin);
}

/*
	Returns a translated copy of the Transform argument.
	See Transform::translate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> translated(Transform<T> transform, Is2dVector auto const offset) {
	return transform.translate(offset);
}
/*
	Returns a copy of the Transform argument with a specific absolute translation.
	See Transform::translation.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> with_translation(Transform<T> transform, Is2dVector auto const point) {
	return transform.translation(point);
}

/*
	Returns a scaled copy of the Transform argument.
	See Transform::scale.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled(Transform<T> transform, Is2dVector auto const scale_factor) noexcept {
	return transform.scale(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled_x(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept {
	return transform.scale_x(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled_y(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept {
	return transform.scale_y(scale_factor);
}

#ifdef BUILD_TESTING
template<std::floating_point T>
constexpr bool get_is_approximately_identity(Transform<T> const t) {
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
		constexpr auto a = Transform{
			11.f, 2.9f, 3.5f, 
			4.3f, 5.7f, 6.2f
		};
		return scaled(a, square<Vector2d>(0.6f)) == scaled_x(a, 0.6f).scale_y(0.6f);
	}(),
	"The scaling functions for avo::math::Transform do not work properly."
);
#endif // BUILD_TESTING

//------------------------------

template<utils::IsNumber _Value>
struct Rectangle final {
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
	constexpr bool operator==(Rectangle const&) const noexcept = default;

	[[nodiscard]]
	constexpr Rectangle operator-() const noexcept {
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

	constexpr Rectangle& offset_x(_Value const offset) noexcept {
		left += offset;
		right += offset;
		return *this;
	}
	constexpr Rectangle& offset_y(_Value const offset) noexcept {
		top += offset;
		bottom += offset;
		return *this;
	}
	constexpr Rectangle& offset(Is2dVector auto const offset) noexcept {
		offset_x(offset.x);
		offset_y(offset.y);
		return *this;
	}
	template<utils::IsNumber T>
	constexpr Rectangle& offset(Size<T> const size_offset) noexcept {
		right += size_offset.x;
		bottom += size_offset.y;
		return *this;
	}
	constexpr Rectangle& operator+=(Is2dVector auto const offset) noexcept {
		return offset(offset);
	}
	constexpr Rectangle& operator-=(Is2dVector auto const offset) noexcept {
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
	constexpr Rectangle& operator*=(utils::IsNumber auto const factor) noexcept {
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
	constexpr Rectangle& operator/=(utils::IsNumber auto const divisor) noexcept {
		left /= divisor;
		top /= divisor;
		right /= divisor;
		bottom /= divisor;
		return *this;
	}

	constexpr Rectangle& width(_Value const width) noexcept {
		right = left + width;
		return *this;
	}
	constexpr Rectangle& height(_Value const height) noexcept {
		bottom = top + height;
		return *this;
	}
	constexpr Rectangle& size(Size<_Value> const size) noexcept {
		width(size.x);
		height(size.y);
		return *this;
	}

	[[nodiscard]]
	constexpr Size<_Value> size() const noexcept {
		return Size{right - left, bottom - top};
	}
	[[nodiscard]]
	constexpr _Value width() const noexcept {
		return right - left;
	}
	[[nodiscard]]
	constexpr _Value height() const noexcept {
		return bottom - top;
	}

	[[nodiscard]]
	constexpr Point<_Value> top_left() const noexcept {
		return Point{left, top};
	}
	template<bool keep_size = true>
	constexpr Rectangle& top_left(Point<_Value> const top_left) noexcept {
		if constexpr (keep_size) {
			right += top_left.x - left;
			bottom += top_left.y - top;
		}
		left = top_left.x;
		top = top_left.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<_Value> top_right() const noexcept {
		return Point{right, top};
	}
	template<bool keep_size = true>
	constexpr Rectangle& top_right(Point<_Value> const top_right) noexcept {
		if constexpr (keep_size) {
			left += top_right.x - right;
			bottom += top_right.y - top;
		}
		right = top_right.x;
		top = top_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<_Value> bottom_right() const noexcept {
		return Point{right, bottom};
	}
	template<bool keep_size = true>
	constexpr Rectangle& bottom_right(Point<_Value> const bottom_right) noexcept {
		if constexpr (keep_size) {
			left += bottom_right.x - right;
			top += bottom_right.y - bottom;
		}
		right = bottom_right.x;
		bottom = bottom_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<_Value> bottom_left() const noexcept {
		return Point{left, bottom};
	}
	template<bool keep_size = true>
	constexpr Rectangle& bottom_left(Point<_Value> const bottom_left) noexcept {
		if constexpr (keep_size) {
			right += bottom_left.x - left;
			top += bottom_left.y - bottom;
		}
		left = bottom_left.x;
		bottom = bottom_left.y;
		return *this;
	}

	template<bool keep_size = true>
	constexpr Rectangle& set_left(_Value const new_left) noexcept {
		if constexpr (keep_size) {
			right += new_left - left;
		}
		left = new_left;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_top(_Value const new_top) noexcept {
		if constexpr (keep_size) {
			bottom += new_top - top;
		}
		top = new_top;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_right(_Value const new_right) noexcept {
		if constexpr (keep_size) {
			left += new_right - right;
		}
		right = new_right;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_bottom(_Value const new_bottom) noexcept {
		if constexpr (keep_size) {
			top += new_bottom - bottom;
		}
		bottom = new_bottom;
		return *this;
	}

	template<utils::IsNumber T>
	constexpr Rectangle& center(Point<T> const center) noexcept {
		auto const half_size = size()/2;
		left = static_cast<_Value>(center.x - half_size.x);
		top = static_cast<_Value>(center.y - half_size.y);
		right = static_cast<_Value>(center.x + half_size.x);
		bottom = static_cast<_Value>(center.y + half_size.y);
		return *this;
	}
	constexpr Rectangle& center_x(utils::IsNumber auto const center_x) noexcept {
		auto const half_width = width()/2;
		left = static_cast<_Value>(center_x - half_width);
		right = static_cast<_Value>(center_x + half_width);
		return *this;
	}
	constexpr Rectangle& center_y(utils::IsNumber auto const center_y) noexcept {
		auto const half_height = height()/2;
		top = static_cast<_Value>(center_y - half_height);
		bottom = static_cast<_Value>(center_y + half_height);
		return *this;
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr Point<T> center() const noexcept {
		return Point{center_x(), center_y()};
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr T center_x() const noexcept {
		return std::midpoint(static_cast<T>(left), static_cast<T>(right));
	}
	template<utils::IsNumber T = _Value>
	[[nodiscard]]
	constexpr Point<T> center_y() const noexcept {
		return std::midpoint(static_cast<T>(top), static_cast<T>(bottom));
	}

	constexpr Rectangle& move_top_left(Vector2d<_Value> const offset) noexcept {
		left += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr Rectangle& move_top_right(Vector2d<_Value> const offset) noexcept {
		right += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr Rectangle& move_bottom_left(Vector2d<_Value> const offset) noexcept {
		left += offset.x;
		bottom += offset.y;
		return *this;
	}
	constexpr Rectangle& move_bottom_right(Vector2d<_Value> const offset) noexcept {
		right += offset.x;
		bottom += offset.y;
		return *this;
	}

	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr Rectangle<T> to() const noexcept {
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
	constexpr Rectangle& clip_negative_space() noexcept {
		right = max(left, right);
		bottom = max(top, bottom);
		return *this;
	}
	constexpr Rectangle& round_outwards() noexcept {
		left = floor<_Value>(left);
		top = floor<_Value>(top);
		right = ceil<_Value>(right);
		bottom = ceil<_Value>(bottom);
		return *this;
	}

	constexpr Rectangle& bound(Rectangle<_Value> const bounds) noexcept {
		left = std::clamp(left, bounds.left, bounds.right);
		top = std::clamp(top, bounds.top, bounds.bottom);
		right = std::clamp(right, bounds.left, bounds.right);
		bottom = std::clamp(bottom, bounds.top, bounds.bottom);
		return *this;
	}
	template<utils::IsNumber T>
	constexpr Rectangle& contain(Rectangle<T> const rectangle) noexcept {
		/*
			If this is true then we need to round "outwards" so that this 
			rectangle also contains the other rectangle's fractional part.
		*/
		if constexpr (std::floating_point<T> && std::integral<_Value>) {
			left = min(left, floor<_Value>(rectangle.left));
			top = min(top, floor<_Value>(rectangle.top));
			right = max(right, ceil<_Value>(rectangle.right));
			bottom = max(bottom, ceil<_Value>(rectangle.bottom));
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
	constexpr bool contains(Point<T> const point) const noexcept {
		using _Common = std::common_type_t<_Value, T>;
		return static_cast<_Common>(point.x) >= static_cast<_Common>(left) && 
			static_cast<_Common>(point.x) < static_cast<_Common>(right) &&
			static_cast<_Common>(point.y) >= static_cast<_Common>(top) && 
			static_cast<_Common>(point.y) < static_cast<_Common>(bottom);
	}
	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr bool contains(Rectangle<T> const rectangle) const noexcept {
		using _Common = std::common_type_t<_Value, T>;
		return static_cast<_Common>(rectangle.left) > static_cast<_Common>(left) && 
			static_cast<_Common>(rectangle.top) > static_cast<_Common>(top) && 
			static_cast<_Common>(rectangle.right) < static_cast<_Common>(right) && 
			static_cast<_Common>(rectangle.bottom) < static_cast<_Common>(bottom);
	}
	template<utils::IsNumber T>
	[[nodiscard]]
	constexpr bool intersects(Rectangle<T> const rectangle) const noexcept {
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
constexpr Rectangle<T> with_negative_space_clipped(Rectangle<T> rectangle) noexcept {
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
constexpr Rectangle<T> square(T const value) noexcept {
	return Rectangle{T{}, T{}, value, value};
}

#ifdef BUILD_TESTING

static_assert(Rectangle{Size{5, 8}}.to<float>() == Rectangle{0.f, 0.f, 5.f, 8.f});
static_assert(Rectangle{Point{9, 1}, Point{11, 6}} == Rectangle{9, 1, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left() == Point{9, 1});
static_assert(Rectangle{9, 1, 11, 6}.top_right() == Point{11, 1});
static_assert(Rectangle{9, 1, 11, 6}.bottom_right() == Point{11, 6});
static_assert(Rectangle{9, 1, 11, 6}.bottom_left() == Point{9, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left<false>(Point{-2, -2}) == Rectangle{-2, -2, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left(Point{-2, -2}) == Rectangle{-2, -2, 0, 3});
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

#endif // BUILD_TESTING

} // namespace math

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
struct Easing final {
	math::Point<> c0, c1;

	constexpr bool operator==(Easing const&) const noexcept = default;

	static constexpr auto default_precision = 5e-3f;

	/*
		Transforms a normalized value according to a cubic bezier curve.
		c0 is the first control point and c1 is the second one.
		precision is the maximum amount of error in the output value.

		It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
		f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (value is x) and not t. This why we have a precision parameter.
	*/
	static constexpr float ease_value(
		math::Point<> const c0, math::Point<> const c1, 
		float const value, float const precision = default_precision
	) noexcept {
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
		while (math::abs(error) > precision) {
			error = value - t * ((1.f - t) * (3.f * (1.f - t) * c0.x + 3.f * t * c1.x) + t * t);
			t += error / (c0.x * 9.f * (t - 1.f) * (t - 1.f / 3.f) + t * (c1.x * (6.f - 9.f * t) + 3.f * t));
		}

		return t * ((1.f - t) * (3.f * (1.f - t) * c0.y + 3.f * t * c1.y) + t * t);
	}

	constexpr float ease_value(float const value, float const precision = default_precision) const noexcept
	{
		return ease_value(c0, c1, value, precision);
	}
	constexpr float ease_value_inverse(float const value, float const precision = default_precision) const noexcept {
		return ease_value(math::Point{c0.y, c0.x}, math::Point{c1.y, c1.x}, value, precision);
	}
};

#ifdef BUILD_TESTING
static_assert([]{
	auto const easing = Easing{{0.4f, 0.1f}, {0.7f, 0.5f}};
	return math::approximately_equal(easing.ease_value_inverse(easing.ease_value(0.35f, 1e-5f), 1e-5f), 0.35f, 1e-5f);
}());
#endif // BUILD_TESTING

//------------------------------

/*
	ARGB formatted 32-bit packed color, where every channel has 8 bits.
*/
using ColorInt = std::uint32_t;

[[nodiscard]]
constexpr inline std::uint8_t red_channel(ColorInt const color) noexcept {
	return static_cast<std::uint8_t>(color >> 16 & 0xff);
}
[[nodiscard]]
constexpr inline std::uint8_t green_channel(ColorInt const color) noexcept {
	return static_cast<std::uint8_t>(color >> 8 & 0xff);
}
[[nodiscard]]
constexpr inline std::uint8_t blue_channel(ColorInt const color) noexcept {
	return static_cast<std::uint8_t>(color & 0xff);
}
[[nodiscard]]
constexpr inline std::uint8_t alpha_channel(ColorInt const color) noexcept {
	return static_cast<std::uint8_t>(color >> 24 & 0xff);
}

/*
	Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
	This means that a Color object is 4 times as big as a packed 32-bit color, but allows for more 
	precise and efficient operations.
*/
struct Color final {
	float red{}, green{}, blue{}, alpha{1.f};
	
	constexpr Color() noexcept = default;
	/*
		The channels are clamped to the range [0, 1].
	*/
	constexpr Color(float const p_red, float const p_green, float const p_blue, float const p_alpha = 1.f) noexcept :
		red{math::unit_clamp(p_red)}, 
		green{math::unit_clamp(p_green)}, 
		blue{math::unit_clamp(p_blue)}, 
		alpha{math::unit_clamp(p_alpha)}
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
		red{math::unit_clamp(p_red / 255.f)},
		green{math::unit_clamp(p_green / 255.f)},
		blue{math::unit_clamp(p_blue / 255.f)},
		alpha{math::unit_clamp(p_alpha / 255.f)}
	{}

	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 1].
	*/
	explicit constexpr Color(float const lightness, float const p_alpha = 1.f) noexcept :
		red{math::unit_clamp(lightness)},
		green{math::unit_clamp(lightness)},
		blue{math::unit_clamp(lightness)},
		alpha{math::unit_clamp(p_alpha)}
	{}
	/*
		Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
	*/
	explicit constexpr Color(std::uint8_t const lightness, std::uint8_t const p_alpha = 255u) noexcept :
		red{lightness/255.f},
		green{red},
		blue{red},
		alpha{p_alpha/255.f}
	{}
	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 255].
	*/
	template<std::integral T>
	explicit constexpr Color(T const lightness, T const p_alpha = static_cast<T>(255)) noexcept :
		red{math::unit_clamp(static_cast<float>(lightness) / 255.f)},
		green{red},
		blue{red},
		alpha{math::unit_clamp(static_cast<float>(p_alpha) / 255.f)}
	{}

	/*
		Creates a copy of another color but with a new alpha.
	*/
	constexpr Color(Color const color, float const p_alpha) noexcept :
		red{color.red}, 
		green{color.green}, 
		blue{color.blue}, 
		alpha{math::unit_clamp(p_alpha)}
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
		alpha{math::unit_clamp(p_alpha / 255.f)}
	{}

	/*
		Initializes with a 4-byte packed RGBA color.
	*/
	explicit constexpr Color(ColorInt const color) noexcept :
		red{red_channel(color) / 255.f},
		green{green_channel(color) / 255.f},
		blue{blue_channel(color) / 255.f},
		alpha{alpha_channel(color) / 255.f}
	{}

	constexpr Color& operator=(ColorInt const color) noexcept {
		return *this = Color{color};
	}

	[[nodiscard]]
	constexpr bool operator==(Color const&) const noexcept = default;

	[[nodiscard]]
	static constexpr Color rgba(float const red, float const green, float const blue, float const alpha = 1.f) noexcept {
		return Color{red, green, blue, alpha};
	}
	[[nodiscard]]
	static constexpr Color rgb(float const red, float const green, float const blue) noexcept {
		return Color{red, green, blue};
	}

	/*
		Creates a color from hue, saturation, brightness and alpha values.
		They are all clamped to the range [0, 1].
		The difference between HSB and HSL is that the lightness value goes from black to white 
		while brightness goes from black to full color brightness. 
		HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
	*/
	[[nodiscard]]
	static constexpr Color hsba(float hue, float const saturation, float brightness, float const alpha = 1.f) noexcept
	{
		hue -= math::floor<float>(hue);
		brightness = math::unit_clamp(brightness);
		auto const factor = brightness * math::unit_clamp(saturation);

		return Color{
			brightness + factor * (math::unit_clamp(1.f - (hue - 1.f / 6.f) * 6.f) + math::unit_clamp((hue - 4.f / 6.f) * 6.f) - 1.f),
			brightness + factor * (std::min(1.f, hue * 6.f) - math::unit_clamp((hue - 3.f / 6.f) * 6.f) - 1.f),
			brightness + factor * (math::unit_clamp((hue - 2.f / 6.f) * 6.f) - math::unit_clamp((hue - 5.f / 6.f) * 6.f) - 1.f),
			alpha,
		};
	}
	/*
		Calls Color::hsba.
	*/
	[[nodiscard]]
	static constexpr Color hsb(float const hue, float const saturation, float const brightness) noexcept {
		return hsba(hue, saturation, brightness);
	}
	[[nodiscard]]
	static constexpr Color hsba(math::IsAngle auto const hue, float const saturation, float const brightness, float const alpha = 1.f) noexcept {
		return hsba(math::normalized<float>(hue), saturation, brightness, alpha);
	}
	[[nodiscard]]
	static constexpr Color hsb(math::IsAngle auto const hue, float const saturation, float const brightness) noexcept {
		return hsba(math::normalized<float>(hue), saturation, brightness);
	}

	/*
		Creates a color from hue, saturation, lightness and alpha values.
		They are all floats in the range [0, 1].
		The difference between HSB and HSL is that the lightness value goes from black to white 
		while brightness goes from black to full color brightness. 
		HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
	*/
	[[nodiscard]]
	static constexpr Color hsla(float hue, float const saturation, float lightness, float const alpha = 1.f) noexcept 
	{
		hue -= math::floor<float>(hue);
		lightness = math::unit_clamp(lightness);
		auto const factor = 2.f * math::unit_clamp(saturation)*(lightness < 0.5f ? lightness : (1.f - lightness));

		return Color{
			lightness + factor * (math::unit_clamp(1.f - (hue - 1.f / 6.f) * 6.f) + math::unit_clamp((hue - 4.f / 6.f) * 6.f) - 0.5f),
			lightness + factor * (std::min(1.f, hue * 6.f) - math::unit_clamp((hue - 3.f / 6.f) * 6.f) - 0.5f),
			lightness + factor * (math::unit_clamp((hue - 2.f / 6.f) * 6.f) - math::unit_clamp((hue - 5.f / 6.f) * 6.f) - 0.5f),
			alpha
		};
	}
	/*
		Calls Color::hsla.
	*/
	[[nodiscard]]
	static constexpr Color hsl(float const hue, float const saturation, float const lightness) noexcept {
		return hsla(hue, saturation, lightness);
	}
	[[nodiscard]]
	static constexpr Color hsla(math::IsAngle auto const hue, float const saturation, float const lightness, float const alpha = 1.f) noexcept {
		return hsla(math::normalized<float>(hue), saturation, lightness, alpha);
	}
	[[nodiscard]]
	static constexpr Color hsl(math::IsAngle auto const hue, float const saturation, float const lightness) noexcept {
		return hsla(math::normalized<float>(hue), saturation, lightness);
	}

	/*
		Changes the hue of the color. The hue is a float in the range [0, 1].
	*/
	constexpr Color& hue(float new_hue) noexcept {
		new_hue -= math::floor<float>(new_hue);

		auto const min_channel = math::min(red, green, blue);
		auto const max_channel = math::max(red, green, blue);
		
		red = min_channel + (max_channel - min_channel) * 
			(math::unit_clamp(1.f - (new_hue - 1.f / 6.f) * 6.f) + math::unit_clamp((new_hue - 4.f / 6.f) * 6.f));
			
		green = min_channel + (max_channel - min_channel) * 
			(std::min(1.f, new_hue * 6.f) - math::unit_clamp((new_hue - 3.f / 6.f) * 6.f));
			
		blue = min_channel + (max_channel - min_channel) * 
			(math::unit_clamp((new_hue - 2.f / 6.f) * 6.f) - math::unit_clamp((new_hue - 5.f / 6.f) * 6.f));
			
		return *this;
	}
	constexpr Color& hue(math::IsAngle auto const hue_angle) noexcept {
		return hue(math::normalized<float>(hue_angle));
	}
	/*
		Returns the hue of the color. The hue is a float in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr float hue() const noexcept {
		if (red + green + blue == 0.f) {
			return 0.f;
		}

		if (red > green) {
			if (red > blue) {
				if (green > blue) {
					// (1, 0, 0) -> (1, 1, 0) : 0/6 < hue < 1/6, max = red, min = blue
					return (green - blue) / (red - blue) / 6.f;
				}
				else {
					// (1, 0, 1) -> (1, 0, 0) : 5/6 < hue < 6/6, max = red, min = green
					return 1.f - (blue - green) / (red - green) / 6.f;
				}
			}
			else {
				// (0, 0, 1) -> (1, 0, 1) : 4/6 < hue < 5/6, max = blue, min = green
				return (4.f + (red - green) / (blue - green)) / 6.f;
			}
		}
		else {
			if (green > blue) {
				if (red > blue) {
					// (1, 1, 0) -> (0, 1, 0) : 1/6 < hue < 2/6, max = green, min = blue
					return (2.f - (red - blue) / (green - blue)) / 6.f;
				}
				else {
					// (0, 1, 0) -> (0, 1, 1) : 2/6 < hue < 3/6, max = green, min = red
					return (2.f + (blue - red) / (green - red)) / 6.f;
				}
			}
			else {
				// (0, 1, 1) -> (0, 0, 1) : 3/6 < hue < 4/6, max = blue, min = red
				return (4.f - (green - red) / (blue - red)) / 6.f;
			}
		}
	}
	template<math::IsRadians _Return>
	[[nodiscard]]
	constexpr _Return hue() const noexcept {
		using _Value = typename _Return::value_type;
		return _Return{static_cast<_Value>(hue()*std::numbers::pi_v<typename _Return::value_type>*2)};
	}
	template<math::IsDegrees _Return>
	[[nodiscard]]
	constexpr _Return hue() const noexcept {
		using _Value = typename _Return::value_type;
		if constexpr (std::integral<_Value>) {
			return _Return{static_cast<_Value>(std::round(hue()*360))};
		}
		return _Return{static_cast<_Value>(hue()*360)};
	}

	/*
		Sets the HSB saturation of the color. The saturation is a float in the range [0, 1].
		HSB saturation can change lightness, and HSL saturation can change brightness.
		Keep in mind that you can't change the saturation if the color is grayscale, because only RGBA values are stored.
	*/
	constexpr Color& hsb_saturation(float saturation) noexcept {
		if (red == green && red == blue) {
			return *this;
		}

		saturation = math::unit_clamp(saturation);
		auto const factor = saturation/hsb_saturation();

		auto const brightness = math::max(red, green, blue);
		red = brightness + factor*(red - brightness);
		green = brightness + factor*(green - brightness);
		blue = brightness + factor*(blue - brightness);

		return *this;
	}
	/*
		Returns the HSB saturation of the color. The saturation is a float in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr float hsb_saturation() const noexcept {
		if (auto const current_brightness = brightness()) {
			return 1.f - math::min(red, green, blue)/current_brightness;
		}
		return 0.f;
	}

	/*
		Sets the HSL saturation of the color. The saturation is a float in the range [0, 1].
		HSB saturation can change lightness, and HSL saturation can change brightness.
		Keep in mind that you can't change the saturation if the color is gray, since only RGBA values are stored.
	*/
	constexpr Color& hsl_saturation(float saturation) noexcept {
		saturation = math::unit_clamp(saturation);

		auto const saturation_before = hsl_saturation();
		if (saturation_before == 0.f) {
			return *this;
		}
		
		auto const factor = saturation/saturation_before;
		auto const current_lightness = lightness();
		red = current_lightness + factor*(red - current_lightness);
		green = current_lightness + factor*(green - current_lightness);
		blue = current_lightness + factor*(blue - current_lightness);

		return *this;
	}
	/*
		Returns the HSL saturation of the color. The saturation is a float in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr float hsl_saturation() const noexcept {
		auto const min_channel = math::min(red, green, blue);
		auto const max_channel = math::max(red, green, blue);
		if (min_channel == max_channel) {
			return 0.f;
		}
		return std::max(
			(max_channel - min_channel)/(max_channel + min_channel), 
			(max_channel - min_channel)/(2.f - max_channel - min_channel)
		);
	}

	/*
		Sets the brightness of the color. The brightness is a float in the range [0, 1]. A brightness of 0 makes the
		color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
	*/
	constexpr Color& brightness(float new_brightness) noexcept {
		new_brightness = math::unit_clamp(new_brightness);

		if (red == green && red == blue) {
			red = new_brightness;
			green = new_brightness;
			blue = new_brightness;
			return *this;
		}

		auto const old_brightness = brightness();
		red *= new_brightness/old_brightness;
		green *= new_brightness/old_brightness;
		blue *= new_brightness/old_brightness;

		return *this;
	}
	/*
		Returns the brightness of the color. The brightness is a float in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr float brightness() const noexcept {
		return math::max(red, green, blue);
	}

	/*
		Changes the lightness of the color. The lightness a float in the range [0, 1]. A lightness of 0 makes the
		color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
	*/
	constexpr Color& lightness(float new_lightness) noexcept {
		new_lightness = math::unit_clamp(new_lightness);

		if (red == green && red == blue) {
			red = new_lightness;
			green = new_lightness;
			blue = new_lightness;
			return *this;
		}

		if (auto const previous_lightness = lightness(); previous_lightness <= 0.5f) {
			if (new_lightness <= 0.5f) {
				red = red * new_lightness / previous_lightness;
				green = green * new_lightness / previous_lightness;
				blue = blue * new_lightness / previous_lightness;
			}
			else {
				red = (red - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
				green = (green - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
				blue = (blue - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
			}
		}
		else {
			if (new_lightness <= 0.5) {
				red = (red - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
				green = (green - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
				blue = (blue - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
			}
			else {
				red = (red - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
				green = (green - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
				blue = (blue - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
			}
		}
		return *this;
	}

	/*
		Returns the lightness of the color. The lightness is a float in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr float lightness() const noexcept {
		return 0.5f*(math::min(red, green, blue) + math::max(red, green, blue));
	}

	/*
		A contrast of 0 makes the color gray, 0.5 leaves it unchanged and 1 is maximum contrast.
	*/
	constexpr Color& contrast(float const contrast) noexcept {
		if (contrast == 0.5) {
			return *this;
		}
		if (contrast < 0.5f) {
			red = (red - 0.5f) * contrast * 2.f + 0.5f;
			green = (green - 0.5f) * contrast * 2.f + 0.5f;
			blue = (blue - 0.5f) * contrast * 2.f + 0.5f;
		}
		else {
			red = (static_cast<float>(red >= 0.5f) - red) * (contrast * 2.f - 1.f) + red;
			green = (static_cast<float>(green >= 0.5f) - green) * (contrast * 2.f - 1.f) + green;
			blue = (static_cast<float>(blue >= 0.5f) - blue) * (contrast * 2.f - 1.f) + blue;
		}
		return *this;
	}

	/*
		Packs the color into a 32-bit integer in ARGB format.
	*/
	[[nodiscard]]
	constexpr ColorInt get_packed() const noexcept {
		return (static_cast<ColorInt>(alpha*0xff) << 24) | (static_cast<ColorInt>(red*0xff) << 16) | 
			(static_cast<ColorInt>(green*0xff) << 8) | (static_cast<ColorInt>(blue*0xff));
	}

	[[nodiscard]]
	constexpr Color operator+(Color const other) const noexcept {
		return Color{red + other.red, green + other.green, blue + other.blue, alpha + other.alpha};
	}
	constexpr Color& operator+=(Color const other) noexcept {
		return *this = *this + other;
	}
	[[nodiscard]]
	constexpr Color operator-(Color const other) const noexcept {
		return Color{red - other.red, green - other.green, blue - other.blue, alpha - other.alpha};
	}
	constexpr Color& operator-=(Color const other) noexcept {
		return *this = *this - other;
	}

	[[nodiscard]]
	constexpr Color operator*(float const factor) const noexcept {
		return Color{red * factor, green * factor, blue * factor, alpha};
	}
	constexpr Color& operator*=(float const factor) noexcept {
		return *this = *this * factor;
	}
	[[nodiscard]]
	constexpr Color operator/(float const divisor) const noexcept {
		return Color{red/divisor, green/divisor, blue/divisor, alpha};
	}
	constexpr Color& operator/=(float const divisor) noexcept {
		return *this = *this / divisor;
	}

	[[nodiscard]]
	constexpr Color operator+(float const delta) const noexcept {
		return Color{red + delta, green + delta, blue + delta};
	}
	constexpr Color& operator+=(float const delta) noexcept {
		return *this = *this + delta;
	}
	[[nodiscard]]
	constexpr Color operator-(float const delta) const noexcept {
		return Color{red - delta, green - delta, blue - delta};
	}
	constexpr Color& operator-=(float const delta) noexcept {
		return *this = *this - delta;
	}
};

[[nodiscard]]
constexpr Color operator*(float const factor, Color const color) noexcept {
	return color * factor;
}
[[nodiscard]]
constexpr Color operator+(float const factor, Color const color) noexcept {
	return color + factor;
}
[[nodiscard]]
constexpr Color operator-(float const term, Color const color) noexcept {
	return Color{term - color.red, term - color.green, term - color.blue};
}

inline std::ostream& operator<<(std::ostream& stream, Color const color) {
	return stream << "rgba(" << color.red << ", " << color.green 
		<< ", " << color.blue << ", " << color.alpha << ")";
}

namespace math {

/*
	Linearly interpolates a color between start and end. Each channel is faded individually.
	If progress is 0, start is returned. If progress is 1, end is returned.
*/
[[nodiscard]]
constexpr Color interpolate(Color const start, Color const end, float const progress) noexcept {
	return Color{
		std::lerp(start.red, end.red, progress),
		std::lerp(start.green, end.green, progress),
		std::lerp(start.blue, end.blue, progress),
		std::lerp(start.alpha, end.alpha, progress),
	};
}

} // namespace math

#ifdef BUILD_TESTING
static_assert(Color::hsb(math::Degrees{30}, 1.f, 1.f).hue<math::Degrees<int>>() == math::Degrees{30});
static_assert(Color::hsb(math::Degrees{180}, 1.f, 1.f).hue() == 0.5f);
static_assert(Color::hsb(math::Degrees{30}, 0.77f, 1.f).hsb_saturation() == 0.77f);

static_assert(Color::hsl(math::Degrees{30}, 1.f, 0.8f).hue<math::Degrees<int>>() == math::Degrees{30});
static_assert(Color::hsl(math::Degrees{180}, 1.f, 0.8f).hue() == 0.5f);
static_assert(Color::hsl(math::Degrees{30}, 0.77f, 0.8f).hsl_saturation() == 0.77f);

static_assert(Color{0.1f} == Color{0.1f, 0.1f, 0.1f, 1.f});
static_assert(Color{0.1f} == Color::rgb(0.1f, 0.1f, 0.1f));
static_assert(Color{0.1f, 0.2f, 0.3f} + Color{1.f, 0.7f, 0.5f} == Color{1.f, 0.9f, 0.8f});
static_assert(Color{1.f, 0.9f, 0.8f} - Color{1.f, 0.7f, 0.5f, 0.5f} == Color{0.f, 0.9f - 0.7f, 0.8f - 0.5f, 0.5f});
static_assert(Color{0.1f, 0.2f, 0.3f} + 0.2f == Color{0.3f, 0.4f, 0.5f});
static_assert(Color{0.2f, 0.3f, 0.4f} - 0.2f == Color{0.f, 0.3f - 0.2f, 0.2f});
static_assert(1.f - Color{0.2f, 0.3f, 0.4f} == Color{0.8f, 0.7f, 0.6f});
static_assert(Color{0.2f, 0.3f, 0.4f} * 2.f == Color{0.4f, 0.6f, 0.8f});

static_assert(Color{0.1f, 0.2f, 0.9f}.hue(math::Degrees{71}).hue<math::Degrees<int>>() == math::Degrees{71});
static_assert(Color{0.1f, 0.2f, 0.9f}.hue(0.3f).hue() == 0.3f);
static_assert(Color{0.1f, 0.2f, 0.9f}.brightness(0.3f).brightness() == 0.3f);
static_assert(Color{0.1f, 0.2f, 0.9f}.lightness(0.3f).lightness() == 0.3f);
static_assert(math::approximately_equal(Color{0.1f, 0.2f, 0.9f}.hsl_saturation(0.3f).hsl_saturation(), 0.3f));
static_assert(Color{0.1f, 0.2f, 0.9f}.hsb_saturation(0.3f).hsb_saturation() == 0.3f);

static_assert(math::interpolate(Color{0.2f, 0.3f, 0.4f}, Color{0.8f, 0.7f, 0.6f}, 0.5f) == Color{0.5f});

static_assert(alpha_channel(0xabcdef12) == 0xab);
static_assert(red_channel(0xabcdef12) == 0xcd);
static_assert(green_channel(0xabcdef12) == 0xef);
static_assert(blue_channel(0xabcdef12) == 0x12);
#endif // BUILD_TESTING

//------------------------------

/*
	To generate a new unique ID, use the function Id::next().
	To create an ID with a specific value (not guaranteed to be unique), use the constructor.
	An ID which converts to Id::value_type{} is considered invalid, and is the default value.
*/
class Id final {
public:
	using value_type = std::uint64_t;

	[[nodiscard]]
	constexpr explicit operator value_type() const noexcept {
		return _count;
	}

	[[nodiscard]]
	constexpr value_type value() const noexcept {
		return _count;
	}

	[[nodiscard]]
	constexpr bool operator==(Id const& id) const noexcept = default;

	constexpr explicit Id(value_type const id) noexcept :
		_count{id}
	{}
	Id() noexcept = default;

	/*
		Generates a new unique ID, assuming all IDs are generated by this function.
	*/
	[[nodiscard]]
	static Id next() noexcept {
		static auto counter = value_type{};
		return Id{++counter};
	}

private:
	value_type _count{};
};

inline std::ostream& operator<<(std::ostream& stream, Id const id) {
	return stream << id.value();
}

} // namespace avo

namespace std {

template<>
struct hash<avo::Id> {
	std::size_t operator()(avo::Id const id) const noexcept {
		return hash<avo::Id::value_type>{}(id.value());
	}
};

} // namespace std

namespace avo {

template<typename T>
class EventListeners;

/*
	This is a class used to easily manage event listeners. Any type of callable can be a listener.
	The return type and arguments have to be the same for all listeners added to one instance of EventListeners.
*/
template<typename _Return, typename ... _Arguments>
class EventListeners<_Return(_Arguments...)> final {
public:
	using FunctionType = _Return(_Arguments...);
	using ContainerType = std::vector<std::function<FunctionType>>;
	
	using Iterator = std::ranges::iterator_t<ContainerType>;
	using ConstIterator = std::ranges::iterator_t<ContainerType const>;
	
	[[nodiscard]]
	Iterator begin() noexcept {
		return _listeners.begin();
	}
	[[nodiscard]]
	ConstIterator begin() const noexcept {
		return _listeners.begin();
	}
	[[nodiscard]]
	Iterator end() noexcept {
		return _listeners.end();
	}
	[[nodiscard]]
	ConstIterator end() const noexcept {
		return _listeners.end();
	}

	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::operator+=.
	*/
	void add(std::function<FunctionType> listener) {
		auto const lock = std::scoped_lock{_mutex};    
		_listeners.emplace_back(std::move(listener));
	}
	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::add.
	*/
	EventListeners& operator+=(std::function<FunctionType> listener) {
		add(std::move(listener));
		return *this;
	}

	/*
		Removes a listener from the EventListeners instance that matches the passed function.
		Equivalent to EventListeners::operator-=.
	*/
	void remove(std::function<FunctionType> const& listener) {
		auto const lock = std::scoped_lock{_mutex};
		auto const& listener_type = listener.target_type();
		auto const found_position = std::ranges::find_if(_listeners, [&](auto const& listener_element) {
			// template keyword is used to expicitly tell the compiler that target is a template method for
			// std::function<FunctionType> and < shouldn't be parsed as the less-than operator
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
	EventListeners& operator-=(std::function<FunctionType> const& listener) {
		remove(listener);
		return *this;
	}

	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::operator().
	*/
	void notify_all(_Arguments&& ... event_arguments) {
		auto const lock = std::scoped_lock{_mutex};
		for (auto& listener : _listeners) {
			listener(std::forward<_Arguments>(event_arguments)...);
		}
	}
	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::notify_all.
	*/
	void operator()(_Arguments&& ... event_arguments) {
		notify_all(std::forward<_Arguments>(event_arguments)...);
	}

	EventListeners() = default;

	EventListeners(EventListeners&& other) noexcept :
		_listeners{std::move(other._listeners)}
	{}
	EventListeners(EventListeners const&) = delete;

	EventListeners& operator=(EventListeners&& other) noexcept {
		_listeners = std::move(other._listeners);
		return *this;
	}
	EventListeners& operator=(EventListeners const&) = delete;

private:
	std::recursive_mutex _mutex;
	ContainerType _listeners;
};

//------------------------------

namespace font_families {

constexpr auto 
	roboto = std::string_view{"Roboto"},
	material_icons = std::string_view{"Material Icons"};

} // namespace font_families

/*
	Default theme color IDs.
*/
namespace theme_colors {

inline auto const 
	background = Id::next(),
	on_background = Id::next(),

	primary = Id::next(),
	primary_on_background = Id::next(),
	on_primary = Id::next(),

	secondary = Id::next(),
	secondary_on_background = Id::next(),
	on_secondary = Id::next(),

	selection = Id::next(),
	shadow = Id::next();

} // namespace theme_colors

/*
	Default theme easing IDs.
*/
namespace theme_easings {

inline auto const 
	in = Id::next(),
	out = Id::next(),
	in_out = Id::next(),
	symmetrical_in_out = Id::next();

} // namespace theme_easings

/*
	Default theme value names.
*/
namespace theme_values {

inline auto const 
	hover_animation_speed = Id::next(),
	hover_animation_duration = Id::next();

} // namespace theme_values

/*
	A theme consists of different variables that change the look and feel of the parts of the GUI that are using the theme.
	Can be used for changing and accessing any values, colors and easings.
	All the default IDs are in ThemeColors, ThemeEasings and ThemeValues.
*/
struct Theme final {
	std::unordered_map<Id, Color> colors{
		{theme_colors::background, Color{0xfffefefe}},
		{theme_colors::on_background, Color{0xff070707}},
		{theme_colors::primary, Color{0xff6200ea}},
		{theme_colors::primary_on_background, Color{0xff512da8}},
		{theme_colors::on_primary, Color{1.f}},
		{theme_colors::secondary, Color{0xff1de9b6}},
		{theme_colors::secondary_on_background, Color{0xff00bfa5}},
		{theme_colors::on_secondary, Color{0xff070707}},
		{theme_colors::selection, Color{0x90488db5}},
		{theme_colors::shadow, Color{0x68000000}},
	};
	std::unordered_map<Id, Easing> easings{
		{theme_easings::in, Easing{{0.6f, 0.f}, {0.8f, 0.2f}}},
		{theme_easings::out, Easing{{0.1f, 0.9f}, {0.2f, 1.f}}},
		{theme_easings::in_out, Easing{{0.4f, 0.f}, {0.f, 1.f}}},
		{theme_easings::symmetrical_in_out, Easing{{0.6f, 0.f}, {0.4f, 1.f}}},
	};
	std::unordered_map<Id, float> values{
		// 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.
		{theme_values::hover_animation_speed, 1.f/6.f},
		{theme_values::hover_animation_duration, 60.f},
	};
};

//------------------------------

/*
	A Node is a container that stores pointers to instances of itself.
	This means it's a tree data structure. Nodes can have IDs which can be 
	used to retrieve them from the tree. They can also store a pointer to
	an arbitrary object. 
	
	A node does not own its child nodes - child nodes are added to a node tree
	by constructing them with a reference to its parent. This means that nodes
	can be stored in any way you wish; on the stack or on the heap.

	This type can be used to build a tree of software components.
	Each component stores an instance of a Node constructed with its parent node.
	This enables retrieval of other software components in the tree by their IDs.

	Move constructing and move assigning *can* be expensive. This is because a
	Node holds a pointer to its current parent and to all of its current children.
	The child pointer of the parent of the node to be moved as well as the parent
	pointer of all of the children of the node to be moved need to be updated.
*/
class Node final {
public:
	using ContainerType = std::vector<Node*>;

private:
	template<bool is_const>
	class _Iterator {
	public:
		using BaseIterator = std::ranges::iterator_t<std::conditional_t<is_const, ContainerType const, ContainerType>>;

		using value_type = Node;
		using reference = std::conditional_t<is_const, Node const&, Node&>;
		using pointer = std::remove_reference_t<reference>*;
		using difference_type = std::iter_difference_t<BaseIterator>;
		using iterator_category = std::iterator_traits<BaseIterator>::iterator_category;
		using iterator_concept = iterator_category;

		_Iterator& operator++() {
			++_base_iterator;
			return *this;
		}
		_Iterator operator++(int) {
			auto before = *this;
			++_base_iterator;
			return before;
		}
		_Iterator& operator--() {
			--_base_iterator;
			return *this;
		}
		_Iterator operator--(int) {
			auto before = *this;
			--_base_iterator;
			return before;
		}

		// reference operator*() {
		// 	return **_base_iterator;
		// }
		[[nodiscard]]
		reference operator*() const {
			return **_base_iterator;
		}

		// reference operator[](std::size_t const index) {
		// 	return *_base_iterator[index];
		// }
		[[nodiscard]]
		reference operator[](std::size_t const index) const {
			return *_base_iterator[index];
		}

		_Iterator& operator+=(difference_type const offset) {
			_base_iterator += offset;
			return *this;
		}
		_Iterator& operator-=(difference_type const offset) {
			_base_iterator -= offset;
			return *this;
		}
		
		[[nodiscard]]
		_Iterator operator+(difference_type const offset) const {
			return _Iterator{_base_iterator + offset};
		}
		[[nodiscard]]
		friend _Iterator operator+(difference_type const offset, _Iterator const iterator) {
			return _Iterator{offset + iterator._base_iterator};
		}
		[[nodiscard]]
		_Iterator operator-(_Iterator const offset) const {
			return _Iterator{_base_iterator - offset};
		}

		[[nodiscard]]
		bool operator<=>(_Iterator const&) const noexcept = default;
	
		_Iterator() = default;
		explicit _Iterator(BaseIterator const base_iterator) :
			_base_iterator{base_iterator}
		{}
	
	private:
		BaseIterator _base_iterator;
	};
	
public:
	using Iterator = _Iterator<false>;
	using ConstIterator = _Iterator<true>;

	[[nodiscard]]
	Iterator begin() {
		return Iterator{std::begin(_children)};
	}
	[[nodiscard]]
	ConstIterator begin() const {
		return ConstIterator{std::begin(_children)};
	}
	[[nodiscard]]
	Iterator end() {
		return Iterator{std::end(_children)};
	}
	[[nodiscard]]
	ConstIterator end() const {
		return ConstIterator{std::end(_children)};
	}

	[[nodiscard]]
	std::size_t size() const {
		return _children.size();
	}

	[[nodiscard]]
	Node& operator[](std::size_t const index) {
		return *_children[index];
	}
	[[nodiscard]]
	Node const& operator[](std::size_t const index) const {
		return *_children[index];
	}
	[[nodiscard]]
	Node& at(std::size_t const index) {
		return *_children.at(index);
	}
	[[nodiscard]]
	Node const& at(std::size_t const index) const {
		return *_children.at(index);
	}

	[[nodiscard]]
	Node& root() const {
		return *_root;
	}

	[[nodiscard]]
	Node* parent() const {
		return _parent;
	}
	/*
		Sets the parent of the node.
		Returns a reference to this node.
	*/
	Node& parent(Node& parent) {
		if (&parent == this) {
			detach();
		}
		else {
			_remove_from_parent();
			
			_parent = &parent;
			_root = parent._root;

			_add_to_parent();
		}
		return *this;
	}
	/*
		Detaches the node from its parent, making it a root node.
	*/
	Node& detach() noexcept {
		_remove_from_parent();
		_parent = nullptr;
		_root = this;
		return *this;
	}

	[[nodiscard]]
	Id id() const noexcept {
		return _id;
	}
	Node& id(Id const new_id) noexcept {
		_id = new_id;
		return *this;
	}

	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<typename _Component>
	[[nodiscard]]
	_Component* component() {
		return _get_component<_Component>();
	}
	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<typename _Component>
	[[nodiscard]]
	_Component const* component() const {
		return _get_component<_Component>();
	}

	Node() : _root{this}
	{}
	template<typename _Component> 
	Node(Id const id, _Component& component) :
		_root{this},
		_id{id},
		_component{&component}
	{}
	template<typename _Component> 
	Node(Id const id) :
		_root{this},
		_id{id}
	{}
	template<typename _Component> 
	Node(_Component& component) :
		_root{this},
		_component{&component}
	{}
	template<typename _Component> 
	Node(Node& parent, Id const id, _Component& component) :
		_root{parent._root},
		_parent{&parent},
		_id{id},
		_component{&component}
	{
		_add_to_parent();
	}
	template<typename _Component> 
	Node(Node& parent, _Component& component) :
		_root{parent._root},
		_parent{&parent},
		_component{&component}
	{
		_add_to_parent();
	}
	template<typename _Component> 
	Node(Node& parent, Id const id) :
		_root{parent._root},
		_parent{&parent},
		_id{id}
	{
		_add_to_parent();
	}
	~Node() {
		_remove_from_tree();
	}

	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;
	
	Node(Node&& other) noexcept {
		_move_construct(std::move(other));
	}
	Node& operator=(Node&& other) noexcept {
		_remove_from_tree();
		_move_construct(std::move(other));
		return *this;
	}

private:
	template<typename _Component>
	_Component* _get_component() const {
		if (_component.type() == typeid(_Component*)) {
			return std::any_cast<_Component*>(_component);
		}
		else {
			return nullptr;
		}
	}

	void _remove_from_parent() {
		if (_parent) {
			utils::unordered_erase(_parent->_children, this);
		}
	}
	void _add_to_parent() {
		if (_parent) {
			_parent->_children.push_back(this);
		}
	}
	void _remove_from_tree() {
		_remove_from_parent();
		if (!_children.empty()) {
			std::ranges::for_each(_children, &Node::detach);
		}
	}
	void _move_construct(Node&& other) {
		_root = other._root;
		if (_root == &other) {
			_root = this;
		}
		
		if (_parent = other._parent) {
			if (auto const previous_child_pos = std::ranges::find(_parent->_children, &other);
				previous_child_pos != _parent->_children.end()) 
			{
				*previous_child_pos = this;
			}
		}

		_children = std::move(other._children);
		for (auto* const child : _children) {
			child->_parent = this;
		}

		_id = other._id;
		_component = std::move(other._component);
	}

	Node* _root{};
	Node* _parent{};
	ContainerType _children;

	Id _id{};
	std::any _component{};
};

template<typename _Node> requires std::same_as<std::remove_cvref_t<_Node>, Node>
[[nodiscard]]
_Node* find_node_by_id(_Node& node, Id const id) {
	constexpr auto id_from_node = [](_Node& node){ return node.id(); };
	auto const flattened = avo::utils::flatten(node);
	if (auto const pos = std::ranges::find(flattened, id, id_from_node);
		pos == std::ranges::end(flattened)) 
	{
		return nullptr;
	}
	else {
		return &*pos;
	}
}

template<typename _Node> requires std::same_as<std::remove_cvref_t<_Node>, Node>
[[nodiscard]]
std::ranges::view auto find_nodes_by_id(_Node& node, Id const id) {
	return node | avo::utils::flatten | std::views::filter([=](_Node& node) { return node.id() == id; });
}

template<typename _Component>
[[nodiscard]]
_Component const* find_component_by_id(Node const& parent, Id const id) {
	if (auto* const node = find_node_by_id(parent, id)) {
		return node->template component<_Component>();
	}
	else {
		return nullptr;
	}
}
template<typename _Component>
[[nodiscard]]
_Component* find_component_by_id(Node& parent, Id const id) {
	if (auto* const node = find_node_by_id(parent, id)) {
		return node->template component<_Component>();
	}
	else {
		return nullptr;
	}
}

template<typename _Component, typename _Node> requires std::same_as<std::remove_cvref_t<_Node>, Node>
[[nodiscard]]
std::ranges::view auto find_components_by_id(_Node& node, Id const id) {
	return find_nodes_by_id(node, id) 
		| std::views::transform([](auto& found){ return found.template component<_Component>(); }) 
		| std::views::filter([](auto* found){ return found != nullptr; })
		| std::views::transform([](auto* found) -> auto& { return *found; });
}

} // namespace avo
