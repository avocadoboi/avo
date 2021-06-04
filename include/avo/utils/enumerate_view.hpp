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

#ifndef AVO_UTILS_ENUMERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_ENUMERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021

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
			++_base_iterator;
			++_index;
			return *this;
		}
		constexpr void operator++(int) {
			operator++();
		}
		constexpr Iterator operator++(int) 
			requires std::ranges::forward_range<T>
		{
			return Iterator{_base_iterator++, _index++};
		}

		constexpr Iterator& operator--() 
			requires std::ranges::bidirectional_range<T>
		{
			--_base_iterator;
			--_index;
			return *this;
		}
		constexpr Iterator operator--(int) 
			requires std::ranges::bidirectional_range<T>
		{
			return Iterator{_base_iterator--, _index--};
		}

		constexpr Iterator& operator+=(difference_type const offset)
			requires std::ranges::random_access_range<T>
		{
			_base_iterator += offset;
			_index += offset;
			return *this;
		}
		constexpr Iterator& operator-=(difference_type const offset)
			requires std::ranges::random_access_range<T>
		{
			_base_iterator -= offset;
			_index -= offset;
			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator[](difference_type const i) const
			requires std::ranges::random_access_range<T>
		{
			return {_index + i, _base_iterator[i]};
		}

		[[nodiscard]]
		constexpr Iterator operator+(difference_type const offset) const
			requires std::ranges::random_access_range<T>
		{
			return Iterator{_base_iterator + offset, _index + offset};
		}
		[[nodiscard]]
		friend constexpr Iterator operator+(difference_type const offset, Iterator const& iterator)
			requires std::ranges::random_access_range<T>
		{
			return Iterator{iterator._base_iterator + offset, iterator._index + offset};
		}
		[[nodiscard]]
		constexpr Iterator operator-(difference_type const offset) const
			requires std::ranges::random_access_range<T>
		{
			return Iterator{_base_iterator - offset, _index - offset};
		}
		[[nodiscard]]
		constexpr difference_type operator-(Iterator const& other) const
			requires std::ranges::random_access_range<T>
		{
			return _base_iterator - other._base_iterator;
		}

		[[nodiscard]]
		constexpr bool operator==(Iterator const& other) const noexcept 
			requires std::equality_comparable<BaseIterator>
		{
			return _base_iterator == other._base_iterator;
		}
		[[nodiscard]]
		constexpr auto operator<=>(Iterator const& other) const noexcept
			requires std::ranges::random_access_range<T> && std::three_way_comparable<BaseIterator>
		{
			return _base_iterator <=> other._base_iterator;
		}

		[[nodiscard]]
		constexpr value_type operator*() const {
			return value_type{_index, *_base_iterator};
		}

		constexpr Iterator() = default;
		constexpr Iterator(BaseIterator const base_iterator, std::size_t const index) :
			_base_iterator{base_iterator},
			_index{index}
		{}

	private:
		BaseIterator _base_iterator;
		std::size_t _index{};
	};
	
	[[nodiscard]]
	constexpr T base() const& 
		requires std::copy_constructible<T>
	{
		return _base;
	}
	[[nodiscard]]
	constexpr T base() && {
		return std::move(_base);
	}

	[[nodiscard]]
	constexpr Iterator begin() const {
		return {std::ranges::begin(_base), std::size_t{}};
	}

	[[nodiscard]]
	constexpr Iterator end() const {
		return {std::ranges::end(_base), static_cast<std::size_t>(std::ranges::distance(_base))};
	}

	[[nodiscard]]
	constexpr auto size() const 
		requires std::ranges::sized_range<T>
	{
		return std::ranges::size(_base);
	}

	template<std::ranges::input_range Range_>
		requires std::ranges::viewable_range<Range_>
	constexpr explicit EnumerateView(Range_&& range) :
		_base{std::views::all(std::forward<Range_>(range))}
	{}

	constexpr explicit EnumerateView(T view) :
		_base{std::move(view)}
	{}

	constexpr EnumerateView() = default;

private:
	T _base;
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
