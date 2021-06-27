#ifndef AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

#include <ranges>

namespace avo::util {

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
				--current_value_;
			}
			else {
				++current_value_;
			}
			return *this;
		}
		constexpr Iterator operator++(int) noexcept {
			if constexpr (is_reverse) {
				return Iterator{current_value_--};
			}
			else {
				return Iterator{current_value_++};
			}
		}

		[[nodiscard]]
		constexpr Iterator operator+(difference_type const offset) const noexcept {
			if constexpr (is_reverse) {
				return current_value_ - offset;
			}
			else {
				return current_value_ + offset;
			}
		}
		constexpr Iterator& operator+=(difference_type const offset) noexcept {
			if constexpr (is_reverse) {
				current_value_ -= offset;
			}
			else {
				current_value_ += offset;
			}
			return *this;
		}

		constexpr Iterator operator--(int) noexcept {
			if constexpr (is_reverse) {
				return Iterator{current_value_++};
			}
			else {
				return Iterator{current_value_--};
			}
		}
		constexpr Iterator& operator--() noexcept {
			if constexpr (is_reverse) {
				++current_value_;
			}
			else {
				--current_value_;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr Iterator operator-(difference_type const offset) const noexcept {
			if constexpr (is_reverse) {
				return current_value_ + offset;
			}
			else {
				return current_value_ - offset;
			}
		}
		constexpr Iterator& operator-=(difference_type const offset) noexcept {
			if constexpr (is_reverse) {
				current_value_ += offset;
			}
			else {
				current_value_ -= offset;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator*() const noexcept {
			return current_value_;
		}

		[[nodiscard]]
		constexpr value_type operator[](difference_type const offset) const noexcept {
			return static_cast<value_type>(current_value_ + offset);
		}

		[[nodiscard]]
		constexpr auto operator<=>(Iterator const&) const = default;

		constexpr Iterator() = default;
		constexpr Iterator(Value_ const value) noexcept :
			current_value_{value}
		{}

	private:
		value_type current_value_;
	};

	[[nodiscard]]
	constexpr Range<Value_, !is_reverse> reverse() const noexcept {
		return {*(end_ - 1), *start_};
	}

	[[nodiscard]]
	constexpr Iterator begin() const noexcept {
		return start_;
	}

	[[nodiscard]]
	constexpr Iterator end() const noexcept {
		return end_;
	}

// TODO: Remove when MSVC doesn't get confused by the = default.
#ifdef _MSC_VER
	constexpr bool operator==(Range const& other) const {
		return start_ == other.start_ && end_ == other.end_;
	}
#else
	constexpr bool operator==(Range const&) const = default;
#endif

	/*
		Creates a range of integers starting with start and ending with inclusive_end.
	*/
	constexpr Range(Value_ const start, Value_ const inclusive_end) noexcept requires (!is_reverse) :
		start_{start},
		end_{inclusive_end + 1}
	{}
	constexpr Range(Value_ const start, Value_ const inclusive_end) noexcept requires is_reverse :
		start_{start},
		end_{inclusive_end - 1}
	{}
	/*
		Creates a range of integers starting with 0 and ending with count - 1.
	*/
	constexpr Range(Value_ const count) noexcept :
		start_{0},
		end_{count}
	{}

	constexpr Range() = default;

private:
	Iterator start_;
	Iterator end_;
};

} // namespace avo::util

template<std::integral T, bool is_reversed>
constexpr bool std::ranges::enable_borrowed_range<avo::util::Range<T, is_reversed>> = true;

namespace avo::util {

/*
	Has nothing to do with the std::ranges::range concept.
	Is true if T is an instance of avo::util::Range.
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

} // namespace avo::util

#endif
