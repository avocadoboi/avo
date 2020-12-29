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
#include <iostream>
#include <numeric>
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
    A random number generator, a small abstraction on top of the standard library.
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

/*
    Enables utf-8 encoded console output on Windows.
    Pretty much all other platforms use utf-8 by default.
*/
auto enable_utf8_console() -> void;

/*
	Converts a UTF-8 encoded char string to a UTF-16 encoded char16 string.
*/
auto utf8_to_utf16(std::string_view input, std::span<char16_t> output) -> void;
/*
	Converts a UTF-8 encoded string to a UTF-16 encoded std::u16string.
*/
auto utf8_to_utf16(std::string_view input) -> std::u16string;
/*
	Returns the number of UTF-16 encoded char16 units that would be used to 
    represent the same characters in a UTF-8 encoded char string.
*/
auto utf8_to_utf16_unit_count(std::string_view input) -> std::size_t;

/*
	Converts a UTF-16 encoded char16 string to a UTF-8 encoded char string.
*/
auto utf16_to_utf8(std::u16string_view input, std::span<char> output) -> void;
/*
	Converts a UTF-16 char16 string to a UTF-8 encoded std::string.
*/
auto utf16_to_utf8(std::u16string_view input) -> std::string;
/*
	Returns the number of UTF-8 encoded char units that would be used to 
    represent the same characters in a UTF-16 encoded char16 string.
*/
auto utf16_to_utf8_unit_count(std::u16string_view input) -> std::size_t;

} // namespace utils

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

} // namespace math

} // namespace avo
