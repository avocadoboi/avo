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

#ifndef AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

#include <ranges>

namespace avo::utils {

/*
	Represents a range of integers that can be iterated to produce the integers within the range.
*/
template<std::integral Value_, bool is_reverse = false>
class Range final : public std::ranges::view_interface<Range<Value_, is_reverse>> {
public:
	using value_type = Value_;

	class Iterator final {
	public:
		using value_type = std::remove_cv_t<Value_>;
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
		constexpr auto operator<=>(Iterator const& other) const = default;

		constexpr Iterator() = default;
		constexpr Iterator(Value_ const value) noexcept :
			_current_value{value}
		{}

	private:
		value_type _current_value;
	};

	[[nodiscard]]
	constexpr Range<Value_, !is_reverse> reverse() const noexcept {
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

	constexpr bool operator==(Range const&) const = default;

	/*
		Creates a range of integers starting with start and ending with inclusive_end.
	*/
	constexpr Range(Value_ const start, Value_ const inclusive_end) noexcept requires (!is_reverse) :
		_start{start},
		_end{inclusive_end + 1}
	{}
	constexpr Range(Value_ const start, Value_ const inclusive_end) noexcept requires is_reverse :
		_start{start},
		_end{inclusive_end - 1}
	{}
	/*
		Creates a range of integers starting with 0 and ending with count - 1.
	*/
	constexpr Range(Value_ const count) noexcept :
		_start{0},
		_end{count}
	{}

	constexpr Range() = default;

private:
	Iterator _start;
	Iterator _end;
};

} // namespace avo::utils

template<std::integral T, bool is_reversed>
constexpr bool std::ranges::enable_borrowed_range<avo::utils::Range<T, is_reversed>> = true;

namespace avo::utils {

/*
	Has nothing to do with the std::ranges::range concept.
	Is true if T is an instance of avo::utils::Range.
*/
template<class T, class Value_ = typename T::value_type>
concept IsIntRange = requires(T range) {
	{ Range{range} } -> std::same_as<T>;
	requires std::same_as<Value_, typename T::value_type>;
};

//------------------------------

/*
	Takes any range and returns a range containing the indices of the elements of the original range.
*/
constexpr auto indices = [](std::ranges::sized_range auto&& range) 
	-> Range<std::size_t> 
{
	return std::ranges::size(range);
};

constexpr auto operator|(std::ranges::sized_range auto&& range, decltype(indices))
	-> Range<std::size_t>
{
	return std::ranges::size(range);
}

} // namespace avo::utils

#endif
