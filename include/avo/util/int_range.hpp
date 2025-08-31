#ifndef AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_INT_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

// #include <mdspan>
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
		using difference_type = std::ptrdiff_t;
		using iterator_concept = std::random_access_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;

		constexpr Iterator& operator++() {
			if constexpr (is_reverse) {
				--current_value_;
			}
			else {
				++current_value_;
			}
			return *this;
		}
		constexpr Iterator operator++(int) {
			if constexpr (is_reverse) {
				return Iterator{current_value_--};
			}
			else {
				return Iterator{current_value_++};
			}
		}

		[[nodiscard]]
		constexpr Iterator operator+(difference_type const offset) const {
			if constexpr (is_reverse) {
				return current_value_ - offset;
			}
			else {
				return current_value_ + offset;
			}
		}
		[[nodiscard]]
		friend constexpr Iterator operator+(difference_type const offset, Iterator const iterator) {
			return iterator + offset;
		}
		constexpr Iterator& operator+=(difference_type const offset) {
			if constexpr (is_reverse) {
				current_value_ -= offset;
			}
			else {
				current_value_ += offset;
			}
			return *this;
		}

		constexpr Iterator operator--(int) {
			if constexpr (is_reverse) {
				return Iterator{current_value_++};
			}
			else {
				return Iterator{current_value_--};
			}
		}
		constexpr Iterator& operator--() {
			if constexpr (is_reverse) {
				++current_value_;
			}
			else {
				--current_value_;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr Iterator operator-(difference_type const offset) const {
			if constexpr (is_reverse) {
				return static_cast<Value_>(current_value_ + offset);
			}
			else {
				return static_cast<Value_>(current_value_ - offset);
			}
		}
		[[nodiscard]]
		constexpr difference_type operator-(Iterator const other) const {
			if constexpr (is_reverse) {
				return other.current_value_ - current_value_;
			}
			else {
				return current_value_ - other.current_value_;
			}
		}
		constexpr Iterator& operator-=(difference_type const offset) {
			if constexpr (is_reverse) {
				current_value_ += offset;
			}
			else {
				current_value_ -= offset;
			}
			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator*() const {
			return current_value_;
		}

		[[nodiscard]]
		constexpr value_type operator[](difference_type const offset) const {
			return static_cast<value_type>(current_value_ + offset);
		}

		[[nodiscard]]
		constexpr auto operator<=>(Iterator const&) const = default;

		constexpr Iterator() = default;
		constexpr Iterator(Value_ const value) :
			current_value_{value}
		{}

	private:
		value_type current_value_;
	};

	[[nodiscard]]
	constexpr Range<Value_, not is_reverse> reverse() const {
		return {*(end_ - 1), *start_};
	}

	[[nodiscard]]
	constexpr Iterator begin() const {
		return start_;
	}

	[[nodiscard]]
	constexpr Iterator end() const {
		return end_;
	}

	[[nodiscard]]
	constexpr Value_ size() const {
		// This will always be non-negative even for reversed ranges, 
		// see the definition of the iterator subtraction operator above.
		return static_cast<Value_>(end_ - start_);
	}

	// Cannot use default implementation on Clang or MSVC.  
	constexpr bool operator==(Range const& other) const {
		return start_ == other.start_ && end_ == other.end_;
	}

	/*
		Creates a range of integers starting with start and ending with inclusive_end.
	*/
	constexpr Range(Value_ const start, Value_ const inclusive_end) requires (not is_reverse) :
		start_{start},
		end_{inclusive_end + 1}
	{}
	constexpr Range(Value_ const start, Value_ const inclusive_end) requires (is_reverse) :
		start_{start},
		end_{inclusive_end - 1}
	{}
	/*
		Creates a range of integers starting with 0 and ending with count - 1.
	*/
	constexpr explicit Range(Value_ const count) :
		start_{0},
		end_{count}
	{}

	constexpr Range() = default;

private:
	Iterator start_;
	Iterator end_;
};

// Clang complains without this
template <std::integral T>
Range(T, T) -> Range<T, false>;

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

namespace detail {

struct Indices {
	constexpr Range<std::size_t> operator()(std::ranges::sized_range auto const& range) const {
		return Range{std::ranges::size(range)};
	}
	// template<typename T, std::integral I, std::size_t ... dimensions>
	// constexpr auto operator()(std::mdspan<T, std::extents<I, dimensions...>> const span) const {
	// 	return operator()(span.extents());
	// }

	// // Static extents
	// template<std::integral I, std::size_t ... dimensions>
	// constexpr auto operator()(std::extents<I, dimensions...>) const 
	// {
	// 	return std::views::cartesian_product(Range{dimensions} ...);
	// }

	// // Dynamic extents
	// template<std::integral I, std::size_t ... dimensions>
	// constexpr auto operator()(std::extents<I, dimensions...> const span) const 
	// 	requires ((dimensions == std::dynamic_extent) && ...)
	// {
	// 	return helper_(span, std::make_index_sequence<sizeof...(dimensions)>{});
	// }
	
private:
	// template<std::integral I, std::size_t ... dimensions, std::size_t ... indices>
	// constexpr auto helper_(std::extents<I, dimensions...> const span, std::index_sequence<indices...>) const {
	// 	return std::views::cartesian_product(Range{span.extent(indices)} ...);
	// }
};

constexpr auto operator|(auto const& range, Indices const indices) {
	return indices(range);
}

}

/*
	Takes any range and returns a range containing the indices of the elements of the original range.
*/
constexpr auto indices = detail::Indices{};

} // namespace avo::util

#endif
