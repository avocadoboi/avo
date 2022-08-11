#ifndef AVO_UTILS_CIRCULAR_STACK_BUFFER_HPP_BJORN_SUNDIN_MARCH_2022
#define AVO_UTILS_CIRCULAR_STACK_BUFFER_HPP_BJORN_SUNDIN_MARCH_2022

#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>

namespace avo::util {

template<std::movable T, std::size_t capacity> requires (capacity > 0)
class CircularStackBuffer final {
public:
	template<bool is_const>
	class Iterator final {
	public:
		using value_type = T;
		using reference = std::conditional_t<is_const, value_type const&,  value_type&>;
		using pointer = std::conditional_t<is_const, value_type const*, value_type*>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;
		using iterator_concept = iterator_category;

		constexpr Iterator& operator++() {
			++pos_;
			if (pos_ == buffer_ + capacity) {
				pos_ = buffer_;
			}
			is_at_end_ = pos_ == tail_;
			
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
		constexpr bool operator==(Iterator const&) const = default;
		[[nodiscard]]
		constexpr bool operator==(std::default_sentinel_t) const {
			return is_at_end_;
		}
		[[nodiscard]]
		constexpr bool operator!=(std::default_sentinel_t) const {
			return !is_at_end_;
		}

		constexpr Iterator() = default;
		constexpr Iterator(pointer const buffer, pointer const pos, pointer const tail) :
			buffer_{buffer},
			pos_{pos},
			tail_{tail}
		{}

	private:
		pointer buffer_;
		pointer pos_;
		pointer tail_;
		bool is_at_end_{false};
	};

	[[nodiscard]]
	constexpr Iterator<false> begin() {
		return Iterator<false>{buffer_.begin(), buffer_.begin() + head_, buffer_.begin() + tail_};
	}
	[[nodiscard]]
	constexpr Iterator<true> begin() const {
		return Iterator<true>{buffer_.begin(), buffer_.begin() + head_, buffer_.begin() + tail_};
	}
	[[nodiscard]]
	constexpr std::default_sentinel_t end() const {
		return {};
	}

	[[nodiscard]]
	constexpr std::size_t size() const {
		return tail_ > head_ ? tail_ - head_ : is_empty_ ? 0 : capacity - head_ + tail_;
	}
	[[nodiscard]]
	constexpr bool is_empty() const {
		return is_empty_;
	}

	[[nodiscard]]
	constexpr T const& front() const {
		assert(not is_empty_);
		return buffer_[head_];
	}
	[[nodiscard]]
	constexpr T& front() {
		assert(not is_empty_);
		return buffer_[head_];
	}

	[[nodiscard]]
	constexpr T const& back() const {
		assert(not is_empty_);
		return buffer_[tail_ == 0 ? capacity - 1 : tail_ - 1];
	}
	[[nodiscard]]
	constexpr T& back() {
		assert(not is_empty_);
		return buffer_[tail_ == 0 ? capacity - 1 : tail_ - 1];
	}

	[[nodiscard]]
	constexpr T const& operator[](std::size_t const i) const {
		assert(i < size());
		return buffer_[(head_ + i) % capacity];
	}
	[[nodiscard]]
	constexpr T& operator[](std::size_t const i) {
		assert(i < size());
		return buffer_[(head_ + i) % capacity];
	}

	template<class U> requires std::same_as<std::remove_cvref_t<U>, T>
	constexpr void push_front(U&& element) 
	{
		if (is_empty_) {
			// If the buffer is empty we want to insert at the current head, not before it.
			push_back(std::forward<U>(element));
		}
		else {
			auto const is_autocannibalic = head_ == tail_;

			// std::numeric_limits<std::size_t>::max() % capacity only equals capacity - 1 if capacity is a power of two,
			// so we can't just do (head_ - 1) % capacity.
			head_ = head_ == 0 ? capacity - 1 : head_ - 1;

			buffer_[head_] = std::forward<U>(element);

			if (is_autocannibalic) {
				tail_ = head_;
			}
		}
	}

	constexpr void pop_front() {
		if (is_empty_) {
			return;
		}

		std::destroy_at(&buffer_[head_]);
		head_ = (head_ + 1) % capacity;

		if (head_ == tail_) {
			is_empty_ = true;
		}
	}

	template<class U> requires std::same_as<std::remove_cvref_t<U>, T>
	constexpr void push_back(U&& element) 
	{
		buffer_[tail_] = std::forward<U>(element);

		auto const is_autocannibalic = not is_empty_ && head_ == tail_;
		
		tail_ = (tail_ + 1) % capacity;
		
		if (is_autocannibalic) {
			head_ = tail_;
		}
		if (is_empty_) {
			is_empty_ = false;
		}
	}
	
	constexpr void pop_back() {
		if (is_empty_) {
			return;
		}

		tail_ = tail_ == 0 ? capacity - 1 : tail_ - 1;
		std::destroy_at(&buffer_[tail_]);

		if (tail_ == head_) {
			is_empty_ = true;
		}
	}

	constexpr void resize(std::size_t const new_size) {
		auto const new_tail = (head_ + new_size) % capacity;
		if (new_size < size()) {
			std::ranges::destroy(Iterator<false>{buffer_.begin(), buffer_.begin() + new_tail, buffer_.begin() + tail_}, std::default_sentinel);
		}
		tail_ = new_tail;
		is_empty_ = new_size == std::size_t{};
	}

private:
	std::array<T, capacity> buffer_;
	std::size_t head_{};
	std::size_t tail_{};
	bool is_empty_{true};
};

} // namespace avo::util

#endif
