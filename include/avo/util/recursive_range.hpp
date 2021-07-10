#ifndef AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_RECURSIVE_RANGE_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"
#include "generate_view.hpp"
#include "miscellaneous.hpp"

#include <stack>
#include <variant>

namespace avo::util {

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
	&& (not has_parent_reference || std::ranges::forward_range<T> &&
		(IsAnyOf<decltype(T::parent), T&, T*> || requires(T range) { { range.parent() } -> IsAnyOf<T&, T*>; }));

template<class T>
concept IsRecursiveIterator = IsRecursiveRange<std::iter_value_t<T>>;

namespace detail
{

// TODO: Put these requires-expressions inline in get_parent when MSVC accepts that.

template<class T>
concept HasPointerParentFunction = requires (T range) { 
	{ range.parent() } -> std::same_as<T*>; 
};
template<class T>
concept HasReferenceParentFunction = requires (T range) { 
	{ range.parent() } -> std::same_as<T&>; 
};

} // namespace detail

/*
	Returns a pointer to the parent of a recursive range.
	See avo::util::IsRecursiveRange.
*/
template<IsRecursiveRange<true> T>
[[nodiscard]]
constexpr T* get_parent(T& range) {
	if constexpr (detail::HasPointerParentFunction<T>)
	{
		return range.parent();
	}
	else if constexpr (detail::HasReferenceParentFunction<T>)
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
	else {
		unreachable();
	}
}

/*
	Returns a range of a recursive range's parents, traversed upwards.
	See avo::util::IsRecursiveRange.
*/
template<IsRecursiveRange<true> Range_> 
[[nodiscard]]
constexpr auto view_parents(Range_& range) 
{
	return generate([current = &range]() mutable -> std::optional<Range_*> {
		current = get_parent(*current);
		if (current) {
			return current;
		}
		return {};
	});
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
			if (std::holds_alternative<T*>(pos_)) {
				pos_ = std::ranges::begin(*std::get<T*>(pos_));
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
			return std::holds_alternative<T*>(pos_) ?
				*std::get<T*>(pos_) :
				*std::get<BaseIterator>(pos_);
		}

		[[nodiscard]]
		bool operator==(std::default_sentinel_t) const noexcept {
			return std::holds_alternative<BaseIterator>(pos_) && 
				parent_stack_.empty() &&
				std::get<BaseIterator>(pos_) == end_;
		}
		[[nodiscard]]
		bool operator==(Iterator const& other) const noexcept {
			return 
				not parent_stack_.empty() && 
				parent_stack_.size() == other.parent_stack_.size() &&
				parent_stack_.top() == other.parent_stack_.top() && 
				pos_ == other.pos_;
		}
	
		Iterator() = default;
		explicit Iterator(T* const range) :
			pos_{range},
			end_{std::ranges::end(*range)}
		{}

	private:
		void increment_iterator_() {
			auto& pos = std::get<BaseIterator>(pos_);
			if (parent_stack_.empty() && pos == end_) {
				return;
			}
			else if (std::ranges::empty(*pos)) {
				++pos;
				while (not parent_stack_.empty() && pos == std::ranges::end(*parent_stack_.top())) {
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
		
		// T* when the iterator points to the root range (that we don't have a base iterator to).
		// We want our iterators to dereference to the root range at first.
		std::variant<T*, BaseIterator> pos_;
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

template<IsRecursiveRange T> 
	requires IsRecursiveRange<T, true> && std::ranges::contiguous_range<T>
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
	
		constexpr Iterator& operator++() {
			if (parent_count_ == 1 && pos_ == end_) {
				return *this;
			}
			if (std::ranges::empty(*pos_)) {
				T* parent = get_parent(*pos_);
				++pos_;
				while (parent_count_ != 1 && pos_ == std::ranges::data(*parent) + std::ranges::size(*parent)) {
					pos_ = parent;
					parent = get_parent(*pos_);
					++pos_;
					--parent_count_;
				}
			}
			else {
				pos_ = std::ranges::data(*pos_);
				++parent_count_;
			}
			return *this;
		}
		constexpr Iterator operator++(int) {
			auto previous = *this;
			++*this;
			return previous;
		}

		[[nodiscard]]
		constexpr reference operator*() const {
			return *pos_;
		}

		[[nodiscard]]
		constexpr bool operator==(std::default_sentinel_t) const noexcept {
			return pos_ == end_;
		}
		[[nodiscard]]
		constexpr bool operator==(Iterator const&) const noexcept = default;
	
		constexpr Iterator() = default;
		constexpr Iterator(T* const range) :
			pos_{range},
			end_{std::ranges::data(*range) + std::ranges::size(*range)}
		{}

	private:		
		T* pos_;
		T* end_;
		std::size_t parent_count_{};
	};

	[[nodiscard]]
	constexpr Iterator begin() const {
		return Iterator{range_};
	}
	[[nodiscard]]
	constexpr std::default_sentinel_t end() const noexcept {
		return {};
	}

	constexpr FlattenedView() = default;
	constexpr explicit FlattenedView(T& range) noexcept :
		range_{&range}
	{}

private:
	T* range_{};
};

} // namespace avo::util

namespace std::ranges {

template<class T>
constexpr auto enable_borrowed_range<avo::util::FlattenedView<T>> = true;

} // namespace std::ranges

namespace avo::util {

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

} // namespace avo::util

#endif
