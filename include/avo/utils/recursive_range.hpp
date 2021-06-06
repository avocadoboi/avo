#ifndef AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"
#include "generate_view.hpp"
#include "miscellaneous.hpp"

#include <stack>
#include <variant>

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
			if (std::holds_alternative<T*>(current_position_)) {
				current_position_ = std::ranges::begin(*std::get<T*>(current_position_));
			}
			else {
				increment_iterator_();
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
			return std::holds_alternative<T*>(current_position_) ?
				*std::get<T*>(current_position_) :
				*std::get<BaseIterator>(current_position_);
		}

		[[nodiscard]]
		bool operator==(std::default_sentinel_t) const noexcept {
			return std::holds_alternative<BaseIterator>(current_position_) && 
				std::get<BaseIterator>(current_position_) == end_;
		}
		[[nodiscard]]
		bool operator==(Iterator const& other) const noexcept {
			return current_position_ == other.current_position_;
		}
	
		Iterator() = default;
		explicit Iterator(T* const range) :
			current_position_{range},
			end_{std::ranges::end(*range)}
		{}

	private:
		void increment_iterator_() {
			auto& pos = std::get<BaseIterator>(current_position_);
			if (pos == end_) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				++pos;
				while (!parent_stack_.empty() && pos == std::ranges::end(*parent_stack_.top())) {
					pos = parent_stack_.top();
					parent_stack_.pop();
					++pos;
				}
			}
			else {
				parent_stack_.push(pos);
				pos = std::ranges::begin(*pos);				
			}
		}
		
		std::variant<T*, BaseIterator> current_position_;
		BaseIterator end_;
		std::stack<BaseIterator> parent_stack_;
	};

	[[nodiscard]]
	Iterator begin() const {
		return Iterator{range_};
	}
	[[nodiscard]]
	std::default_sentinel_t end() const noexcept {
		return {};
	}

	FlattenedView() = default;
	explicit FlattenedView(T& range) noexcept :
		range_{&range}
	{}

private:
	T* range_{};
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
			if (std::holds_alternative<T*>(current_position_)) {
				current_position_ = std::ranges::begin(*std::get<T*>(current_position_));
			}
			else {
				increment_iterator_();
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
			return std::holds_alternative<T*>(current_position_) ?
				*std::get<T*>(current_position_) :
				*std::get<BaseIterator>(current_position_);
		}

		[[nodiscard]]
		bool operator==(std::default_sentinel_t) const noexcept {
			return std::holds_alternative<BaseIterator>(current_position_) && 
				std::get<BaseIterator>(current_position_) == end_;
		}
		[[nodiscard]]
		bool operator==(Iterator const& other) const noexcept {
			return current_position_ == other.current_position_;
		}
	
		Iterator() = default;
		Iterator(T* const range) :
			current_position_{range},
			end_{std::ranges::end(*range)}
		{}

	private:
		static BaseIterator get_iterator_of_node_(T& node) {
			T* const parent = get_parent(node);
			return std::ranges::begin(*parent) + (&node - &*std::ranges::begin(*parent));
		}
	
		void increment_iterator_() {
			auto& pos = std::get<BaseIterator>(current_position_);
			if (pos == end_) {
				return;
			}
			else if (is_recursive_iterator_empty(pos)) {
				T* parent = get_parent(*pos);
				++pos;
				while (pos != end_ && pos == std::ranges::end(*parent)) {
					pos = get_iterator_of_node_(*parent);
					parent = get_parent(*pos);
					++pos;
				}
			}
			else {
				pos = std::ranges::begin(*pos);
			}
		}
		
		std::variant<T*, BaseIterator> current_position_;
		BaseIterator end_;
	};

	[[nodiscard]]
	Iterator begin() const {
		return Iterator{range_};
	}
	[[nodiscard]]
	std::default_sentinel_t end() const noexcept {
		return {};
	}

	FlattenedView() = default;
	explicit FlattenedView(T& range) noexcept :
		range_{&range}
	{}

private:
	T* range_{};
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
