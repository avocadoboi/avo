#ifndef AVO_UTILS_ENUMERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_ENUMERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021

#include <ranges>

namespace avo::utils {

template<class T>
struct EnumeratedElement final {
	std::size_t index;
	T& element;
};

/*
	A range of (index, element) pairs referring to the elements of another range.
*/
template<std::ranges::input_range T> 
	requires std::ranges::view<T>
class EnumerateView final : public std::ranges::view_interface<EnumerateView<T>> {
public:
	class Iterator final {
	public:
		using BaseIterator = std::ranges::iterator_t<T>;

		using value_type = EnumeratedElement<std::remove_reference_t<std::ranges::range_reference_t<T>>>;
		using reference = value_type; // value_type is already a reference type.
		using pointer = value_type*;
		using difference_type = std::iter_difference_t<BaseIterator>;
		using iterator_category = std::iterator_traits<BaseIterator>::iterator_category;
		using iterator_concept = iterator_category;

		constexpr Iterator& operator++() {
			++base_iterator_;
			++index_;
			return *this;
		}
		constexpr void operator++(int) {
			operator++();
		}
		constexpr Iterator operator++(int) 
			requires std::ranges::forward_range<T>
		{
			return Iterator{base_iterator_++, index_++};
		}

		constexpr Iterator& operator--() 
			requires std::ranges::bidirectional_range<T>
		{
			--base_iterator_;
			--index_;
			return *this;
		}
		constexpr Iterator operator--(int) 
			requires std::ranges::bidirectional_range<T>
		{
			return Iterator{base_iterator_--, index_--};
		}

		constexpr Iterator& operator+=(difference_type const offset)
			requires std::ranges::random_access_range<T>
		{
			base_iterator_ += offset;
			index_ += offset;
			return *this;
		}
		constexpr Iterator& operator-=(difference_type const offset)
			requires std::ranges::random_access_range<T>
		{
			base_iterator_ -= offset;
			index_ -= offset;
			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator[](difference_type const i) const
			requires std::ranges::random_access_range<T>
		{
			return {index_ + i, base_iterator_[i]};
		}

		[[nodiscard]]
		constexpr Iterator operator+(difference_type const offset) const
			requires std::ranges::random_access_range<T>
		{
			return Iterator{base_iterator_ + offset, index_ + offset};
		}
		[[nodiscard]]
		friend constexpr Iterator operator+(difference_type const offset, Iterator const& iterator)
			requires std::ranges::random_access_range<T>
		{
			return Iterator{iterator.base_iterator_ + offset, iterator.index_ + offset};
		}
		[[nodiscard]]
		constexpr Iterator operator-(difference_type const offset) const
			requires std::ranges::random_access_range<T>
		{
			return Iterator{base_iterator_ - offset, index_ - offset};
		}
		[[nodiscard]]
		constexpr difference_type operator-(Iterator const& other) const
			requires std::ranges::random_access_range<T>
		{
			return base_iterator_ - other.base_iterator_;
		}

		[[nodiscard]]
		constexpr bool operator==(Iterator const& other) const noexcept 
			requires std::equality_comparable<BaseIterator>
		{
			return base_iterator_ == other.base_iterator_;
		}
		[[nodiscard]]
		constexpr auto operator<=>(Iterator const& other) const noexcept
			requires std::ranges::random_access_range<T> && std::three_way_comparable<BaseIterator>
		{
			return base_iterator_ <=> other.base_iterator_;
		}

		[[nodiscard]]
		constexpr value_type operator*() const {
			return value_type{index_, *base_iterator_};
		}

		constexpr Iterator() = default;
		constexpr Iterator(BaseIterator const base_iterator, std::size_t const index) :
			base_iterator_{base_iterator},
			index_{index}
		{}

	private:
		BaseIterator base_iterator_;
		std::size_t index_{};
	};
	
	[[nodiscard]]
	constexpr T base() const& 
		requires std::copy_constructible<T>
	{
		return base_;
	}
	[[nodiscard]]
	constexpr T base() && {
		return std::move(base_);
	}

	[[nodiscard]]
	constexpr Iterator begin() const {
		return {std::ranges::begin(base_), std::size_t{}};
	}

	[[nodiscard]]
	constexpr Iterator end() const {
		return {std::ranges::end(base_), static_cast<std::size_t>(std::ranges::distance(base_))};
	}

	[[nodiscard]]
	constexpr auto size() const 
		requires std::ranges::sized_range<T>
	{
		return std::ranges::size(base_);
	}

	template<std::ranges::input_range Range_>
		requires std::ranges::viewable_range<Range_>
	constexpr explicit EnumerateView(Range_&& range) :
		base_{std::views::all(std::forward<Range_>(range))}
	{}

	constexpr explicit EnumerateView(T view) :
		base_{std::move(view)}
	{}

	constexpr EnumerateView() = default;

private:
	T base_;
};

template<class T>
EnumerateView(T&&) -> EnumerateView<std::views::all_t<T>>;

/*
	Takes a range and returns a range of (index, element) pairs referring to the original range. 
*/
constexpr auto enumerate = []<std::ranges::input_range T>(T&& range) {
	return EnumerateView{std::forward<T>(range)};
};

/*
	Takes a range and returns a range of (index, element) pairs referring to the original range. 
*/
template<std::ranges::input_range T>
constexpr auto operator|(T&& range, decltype(enumerate)) {
	return enumerate(std::forward<T>(range));
}

} // namespace avo::utils

namespace std::ranges {

template<class T>
constexpr auto enable_borrowed_range<avo::utils::EnumerateView<T>> = std::ranges::borrowed_range<T>;

} // namespace std::ranges

#endif
