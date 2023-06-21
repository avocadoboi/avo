#ifndef AVO_UTILS_STATIC_VECTOR_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_STATIC_VECTOR_HPP_BJORN_SUNDIN_JUNE_2021

#include <algorithm>
#include <array>
#include <concepts>
#include <stdexcept>

namespace avo::util {

template<class T, std::size_t capacity_>
class StaticVector final {
public:
	static constexpr auto capacity = capacity_;

	constexpr StaticVector& push_back(T&& element) {
		array_.at(current_size_++) = std::forward<T>(element);
		return *this;
	}
	constexpr StaticVector& pop_back() {
		--current_size_;
		return *this;
	}

	[[nodiscard]]
	constexpr auto begin() const {
		return array_.begin();
	}
	[[nodiscard]]
	constexpr auto begin() {
		return array_.begin();
	}
	[[nodiscard]]
	constexpr auto end() const {
		return array_.end();
	}
	[[nodiscard]]
	constexpr auto end() {
		return array_.end();
	}

	[[nodiscard]]
	constexpr T& operator[](std::size_t const i) {
		return array_[i];
	}
	[[nodiscard]]
	constexpr T const& operator[](std::size_t const i) const {
		return array_[i];
	}

	[[nodiscard]]
	constexpr T& back() {
		return array_.at(current_size_ - 1);
	}
	[[nodiscard]]
	constexpr T const& back() const {
		return array_.at(current_size_ - 1);
	}
	
	[[nodiscard]]
	constexpr T* data() {
		return array_.data();
	}
	[[nodiscard]]
	constexpr T const* data() const {
		return array_.data();
	}

	[[nodiscard]]
	constexpr std::size_t size() const {
		return current_size_;
	}

	constexpr StaticVector() = default;

	template<std::same_as<T> ... Elements_> 
	constexpr StaticVector(Elements_&& ... elements) :
		array_{std::forward<Elements_>(elements)...},
		current_size_{sizeof...(elements)}
	{}
	constexpr StaticVector(std::initializer_list<T>&& elements) 
	{
		if (elements.size() > capacity) {
			throw std::length_error{"Initializer list used to initialize StaticVector was too long."};
		}
		std::ranges::move(elements, array_.begin());
	}
	constexpr StaticVector(std::initializer_list<T> const& elements) 
	{
		if (elements.size() > capacity) {
			throw std::length_error{"Initializer list used to initialize StaticVector was too long."};
		}
		std::ranges::copy(elements, array_.begin());
	}

private:
	std::array<T, capacity> array_;
	std::size_t current_size_{};
};

} // avo::util

#endif
