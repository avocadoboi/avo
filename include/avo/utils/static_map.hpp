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

#ifndef AVO_UTILS_STATIC_MAP_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_STATIC_MAP_HPP_BJORN_SUNDIN_JUNE_2021

namespace avo::utils {

template<std::move_constructible A, std::move_constructible B, std::size_t _size>
class StaticMap {
public:
    using first_type = A;
    using second_type = B;
	using value_type = std::pair<A, B>;

	using ContainerType = std::array<value_type, _size>;

	[[nodiscard]]
	constexpr auto begin() {
		return std::ranges::begin(_array);
	}
	[[nodiscard]]
	constexpr auto begin() const {
		return std::ranges::begin(_array);
	}
	[[nodiscard]]
	constexpr auto end() {
		return std::ranges::end(_array);
	}
	[[nodiscard]]
	constexpr auto end() const {
		return std::ranges::end(_array);
	}

	[[nodiscard]]
	constexpr std::size_t size() const noexcept {
		return _size;
	}
	[[nodiscard]]
	constexpr value_type* data() noexcept {
		return std::ranges::data(_array);
	}
	[[nodiscard]]
	constexpr value_type const* data() const noexcept {
		return std::ranges::data(_array);
	}

	[[nodiscard]]
	constexpr second_type* find(first_type const& key) {
		return const_cast<second_type*>(static_cast<StaticMap const*>(this)->find(key));
	}
	[[nodiscard]]
	constexpr second_type const* find(first_type const& key) const
	{
		if (auto const pos = std::ranges::find(_array, key, &value_type::first);
			pos != std::ranges::end(_array)) 
		{
			return &pos->second;
		}
		return nullptr;
	}

	[[nodiscard]]
	constexpr second_type const& find_or(first_type const& key, second_type const& default_value) const
	{
		if (auto const pos = std::ranges::find(_array, key, &value_type::first);
			pos != std::ranges::end(_array)) 
		{
			return pos->second;
		}
		return default_value;
	}

	[[nodiscard]]
	constexpr std::ranges::view auto find_all(first_type const& key) 
	{
		return _array | std::views::filter([&](value_type& pair) {
			return pair.first == key;
		}) | std::views::transform([&](value_type& pair) -> second_type& {
			return pair.second;
		});
	}
	[[nodiscard]]
	constexpr std::ranges::view auto find_all(first_type const& key) const 
	{
		return _array | std::views::filter([&](value_type const& pair) {
			return pair.first == key;
		}) | std::views::transform([&](value_type const& pair) -> second_type const& {
			return pair.second;
		});
	}

	template<bool is_key_second_type = false>
	[[nodiscard]]
	constexpr first_type* find(second_type const& key) 
		requires (!std::same_as<first_type, second_type>) || is_key_second_type
	{
		return const_cast<first_type*>(static_cast<StaticMap const*>(this)->find(key));
	}
	template<bool is_key_second_type = false>
	[[nodiscard]]
	constexpr first_type const* find(second_type const& key) const
		requires (!std::same_as<first_type, second_type>) || is_key_second_type
	{
		if (auto const pos = std::ranges::find(_array, key, &value_type::second);
			pos != std::ranges::end(_array)) 
		{
			return &pos->first;
		}
		return nullptr;
	}

	template<bool is_key_second_type = false>
	[[nodiscard]]
	constexpr first_type const& find_or(second_type const& key, first_type const& default_value) const
		requires (!std::same_as<first_type, second_type>) || is_key_second_type
	{
		if (auto const pos = std::ranges::find(_array, key, &value_type::second);
			pos != std::ranges::end(_array)) 
		{
			return pos->first;
		}
		return default_value;
	}

	template<bool is_key_second_type = false>
	[[nodiscard]]
	constexpr std::ranges::view auto find_all(second_type const& key) 
		requires (!std::same_as<first_type, second_type>) || is_key_second_type
	{
		return _array | std::views::filter([&](value_type& pair) {
			return pair.second == key;
		}) | std::views::transform([&](value_type& pair) -> first_type& {
			return pair.first;
		});
	}
	template<bool is_key_second_type = false>
	[[nodiscard]]
	constexpr std::ranges::view auto find_all(second_type const& key) const 
		requires (!std::same_as<first_type, second_type>) || is_key_second_type
	{
		return _array | std::views::filter([&](value_type const& pair) {
			return pair.second == key;
		}) | std::views::transform([&](value_type const& pair) -> first_type const& {
			return pair.first;
		});
	}

    template<IsInstantiationOf<std::pair> ... Element_>
    constexpr StaticMap(Element_&& ... element) :
        _array{std::forward<Element_>(element)...}
    {
		static_assert(sizeof...(element) == _size, "StaticMap was initialized with wrong number of pairs.");
	}
	constexpr StaticMap(std::initializer_list<std::pair<A, B>>&& elements) 
	{
		if (elements.size() != _size) {
			throw std::length_error{"StaticMap was initialized with wrong number of pairs."};
		}
		std::ranges::move(elements, _array.begin());
	}
	constexpr StaticMap(std::initializer_list<std::pair<A, B>> const& elements) 
	{
		if (elements.size() != _size) {
			throw std::length_error{"StaticMap was initialized with wrong number of pairs."};
		}
		std::ranges::copy(elements, _array.begin());
	}

private:
	ContainerType _array;
};

template<IsInstantiationOf<std::pair> ... Element_>
StaticMap(Element_&& ... element) -> StaticMap<decltype((element, ...).first), decltype((element, ...).second), sizeof...(Element_)>;

} // namespace avo::utils

#endif
