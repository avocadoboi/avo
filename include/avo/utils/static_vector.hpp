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

#ifndef AVO_UTILS_STATIC_VECTOR_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_STATIC_VECTOR_HPP_BJORN_SUNDIN_JUNE_2021

#include <algorithm>
#include <array>
#include <concepts>
#include <stdexcept>

namespace avo::utils {

template<class T, std::size_t _capacity>
class StaticVector final {
public:
	static constexpr auto capacity = _capacity;

	constexpr StaticVector& push_back(T&& element) {
		_array.at(_current_size++) = std::forward<T>(element);
		return *this;
	}
	constexpr StaticVector& pop_back() {
		--_current_size;
		return *this;
	}

	[[nodiscard]]
	constexpr auto begin() const {
		return _array.begin();
	}
	[[nodiscard]]
	constexpr auto begin() {
		return _array.begin();
	}
	[[nodiscard]]
	constexpr auto end() const {
		return _array.end();
	}
	[[nodiscard]]
	constexpr auto end() {
		return _array.end();
	}

	[[nodiscard]]
	constexpr T& operator[](std::size_t const i) {
		return _array[i];
	}
	[[nodiscard]]
	constexpr T const& operator[](std::size_t const i) const {
		return _array[i];
	}

	[[nodiscard]]
	constexpr T& back() {
		return _array.at(_current_size - 1);
	}
	[[nodiscard]]
	constexpr T const& back() const {
		return _array.at(_current_size - 1);
	}
	
	[[nodiscard]]
	constexpr T* data() {
		return _array.data();
	}
	[[nodiscard]]
	constexpr T const* data() const {
		return _array.data();
	}

	[[nodiscard]]
	constexpr std::size_t size() const noexcept {
		return _current_size;
	}

	constexpr StaticVector() = default;

	template<std::same_as<T> ... Elements_> 
	constexpr StaticVector(Elements_&& ... elements) :
		_array{std::forward<Elements_>(elements)...},
		_current_size{sizeof...(elements)}
	{}
	constexpr StaticVector(std::initializer_list<T>&& elements) 
	{
		if (elements.size() > capacity) {
			throw std::length_error{"Initializer list used to initialize StaticVector was too long."};
		}
		std::ranges::move(elements, _array.begin());
	}
	constexpr StaticVector(std::initializer_list<T> const& elements) 
	{
		if (elements.size() > capacity) {
			throw std::length_error{"Initializer list used to initialize StaticVector was too long."};
		}
		std::ranges::copy(elements, _array.begin());
	}

private:
	std::array<T, capacity> _array;
	std::size_t _current_size{};
};

} // avo::utils

#endif
