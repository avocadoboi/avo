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

#ifndef AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

namespace avo::utils {

/*
	Evaluates to whether the type T is a range whose value type is the same as the base range.

	If has_parent_reference is true, then:
	* The range is a random access range.
	* The parent of a given range can be queried through either a member variable 
	or a member function returning a reference or pointer to the parent range. 
	The name of this member variable or function must be "parent".

	Recursive ranges with parent references do not require a separate stack 
	to keep track of parents when traversed recursively through a FlattenedView.
*/
template<class T, bool has_parent_reference = false>
concept IsRecursiveRange = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, std::remove_cvref_t<T>>
	&& (!has_parent_reference || std::ranges::random_access_range<T> &&
		(IsAnyOf<decltype(T::parent), T&, T*> || requires(T range) { { range.parent() } -> IsAnyOf<T&, T*>; }));

template<class T>
concept IsRecursiveIterator = IsRecursiveRange<std::iter_value_t<T>>;

/*
	Returns a pointer to the parent of a recursive range.
	See avo::utils::IsRecursiveRange.
*/
template<IsRecursiveRange<true> T>
[[nodiscard]]
constexpr T* get_parent(T& range) {
	if constexpr (requires { { range.parent() } -> std::same_as<T*>; })
	{
		return range.parent();
	}
	else if constexpr (requires { { range.parent() } -> std::same_as<T&>; })
	{
		return &range.parent();
	}
	else if constexpr (std::same_as<decltype(range.parent), T*>)
	{
		return range.parent;
	}
	else if constexpr (std::same_as<decltype(range.parent), T&>)
	{
		return &range.parent;
	}
	unreachable();
}

/*
	Returns a range of a recursive range's parents, traversed upwards.
	See avo::utils::IsRecursiveRange.
*/
template<IsRecursiveRange<true> Range_> 
[[nodiscard]]
constexpr auto view_parents(Range_& range) 
{
	return generate([current = &range]() mutable -> std::optional<Range_*> {
		if (current = get_parent(*current)) {
			return current;
		}
		return {};
	});
}

[[nodiscard]]
constexpr bool is_recursive_iterator_empty(IsRecursiveIterator auto const iterator) {
	return std::ranges::begin(*iterator) == std::ranges::end(*iterator);
}

template<IsRecursiveRange T>
class FlattenedView final : public std::ranges::view_interface<FlattenedView<T>> {
public:
	using BaseIterator = std::ranges::iterator_t<T>;

	class Iterator final {
	public:
		using value_type = std::iter_value_t<BaseIterator>;
		using reference = std::iter_reference_t<BaseIterator>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;
		using iterator_concept = std::forward_iterator_tag;
	
		Iterator& operator++() {
			if (std::holds_alternative<T*>(_current_position)) {
				_current_position = std::ranges::begin(*std::get<T*>(_current_position));
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
		[[nodiscard]]
		bool operator==(Iterator const& other) const noexcept {
			return _current_position == other._current_position;
		}
	
		Iterator() = default;
		explicit Iterator(T* const range) :
			_current_position{range},
			_end{std::ranges::end(*range)}
		{}

	private:
		void _increment_iterator() {
			auto& pos = std::get<BaseIterator>(_current_position);
			if (pos == _end) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				++pos;
				while (!_parent_stack.empty() && pos == std::ranges::end(*_parent_stack.top())) {
					pos = _parent_stack.top();
					_parent_stack.pop();
					++pos;
				}
			}
			else {
				_parent_stack.push(pos);
				pos = std::ranges::begin(*pos);				
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

	FlattenedView() = default;
	explicit FlattenedView(T& range) noexcept :
		_range{&range}
	{}

private:
	T* _range{};
};

template<IsRecursiveRange T> requires IsRecursiveRange<T, true>
class FlattenedView<T> final : public std::ranges::view_interface<FlattenedView<T>> {
public:
	using BaseIterator = std::ranges::iterator_t<T>;

	class Iterator final {
	public:
		using value_type = std::iter_value_t<BaseIterator>;
		using reference = std::iter_reference_t<BaseIterator>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;
		using iterator_concept = std::forward_iterator_tag;
	
		Iterator& operator++() {
			if (std::holds_alternative<T*>(_current_position)) {
				_current_position = std::ranges::begin(*std::get<T*>(_current_position));
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
		[[nodiscard]]
		bool operator==(Iterator const& other) const noexcept {
			return _current_position == other._current_position;
		}
	
		Iterator() = default;
		Iterator(T* const range) :
			_current_position{range},
			_end{std::ranges::end(*range)}
		{}

	private:
		static BaseIterator _get_iterator_of_node(T& node) {
			T* const parent = get_parent(node);
			return std::ranges::begin(*parent) + (&node - &*std::ranges::begin(*parent));
		}
	
		void _increment_iterator() {
			auto& pos = std::get<BaseIterator>(_current_position);
			if (pos == _end) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				T* parent = get_parent(*pos);
				++pos;
				while (pos != _end && pos == std::ranges::end(*parent)) {
					pos = _get_iterator_of_node(*parent);
					parent = get_parent(*pos);
					++pos;
				}
			}
			else {
				pos = std::ranges::begin(*pos);
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

	FlattenedView() = default;
	explicit FlattenedView(T& range) noexcept :
		_range{&range}
	{}

private:
	T* _range{};
};

} // namespace avo::utils

namespace std::ranges {

template<class T>
constexpr auto enable_borrowed_range<avo::utils::FlattenedView<T>> = true;

} // namespace std::ranges

namespace avo::utils {

/*
	Returns a view over a recursive range traversed recursively so 
	that all children can be iterated over.
*/
constexpr auto flatten = [](IsRecursiveRange auto& range) {
	return FlattenedView{range};
};

/*
	Returns a view over a recursive range traversed recursively so 
	that all children can be iterated over.
*/
template<IsRecursiveRange T>
constexpr FlattenedView<T> operator|(T& range, decltype(flatten) const) {
	return FlattenedView{range};
}

} // namespace avo::utils

#endif
