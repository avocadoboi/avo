#ifndef AVO_UTILS_STATIC_MAP_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_STATIC_MAP_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"

#include <algorithm>
#include <stdexcept>

namespace avo::utils {

template<std::move_constructible A, std::move_constructible B, std::size_t size_>
class StaticMap {
public:
	using first_type = A;
	using second_type = B;
	using value_type = std::pair<A, B>;

	using ContainerType = std::array<value_type, size_>;

	[[nodiscard]]
	constexpr auto begin() {
		return std::ranges::begin(array_);
	}
	[[nodiscard]]
	constexpr auto begin() const {
		return std::ranges::begin(array_);
	}
	[[nodiscard]]
	constexpr auto end() {
		return std::ranges::end(array_);
	}
	[[nodiscard]]
	constexpr auto end() const {
		return std::ranges::end(array_);
	}

	[[nodiscard]]
	constexpr std::size_t size() const noexcept {
		return size_;
	}
	[[nodiscard]]
	constexpr value_type* data() noexcept {
		return std::ranges::data(array_);
	}
	[[nodiscard]]
	constexpr value_type const* data() const noexcept {
		return std::ranges::data(array_);
	}

	[[nodiscard]]
	constexpr second_type* find(first_type const& key) {
		return const_cast<second_type*>(static_cast<StaticMap const*>(this)->find(key));
	}
	[[nodiscard]]
	constexpr second_type const* find(first_type const& key) const
	{
		if (auto const pos = std::ranges::find(array_, key, &value_type::first);
			pos != std::ranges::end(array_)) 
		{
			return &pos->second;
		}
		return nullptr;
	}

	[[nodiscard]]
	constexpr second_type const& find_or(first_type const& key, second_type const& default_value) const
	{
		if (auto const pos = std::ranges::find(array_, key, &value_type::first);
			pos != std::ranges::end(array_)) 
		{
			return pos->second;
		}
		return default_value;
	}

	[[nodiscard]]
	constexpr std::ranges::view auto find_all(first_type const& key) 
	{
		return array_ | std::views::filter([&](value_type& pair) {
			return pair.first == key;
		}) | std::views::transform([&](value_type& pair) -> second_type& {
			return pair.second;
		});
	}
	[[nodiscard]]
	constexpr std::ranges::view auto find_all(first_type const& key) const 
	{
		return array_ | std::views::filter([&](value_type const& pair) {
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
		if (auto const pos = std::ranges::find(array_, key, &value_type::second);
			pos != std::ranges::end(array_)) 
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
		if (auto const pos = std::ranges::find(array_, key, &value_type::second);
			pos != std::ranges::end(array_)) 
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
		return array_ | std::views::filter([&](value_type& pair) {
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
		return array_ | std::views::filter([&](value_type const& pair) {
			return pair.second == key;
		}) | std::views::transform([&](value_type const& pair) -> first_type const& {
			return pair.first;
		});
	}

	template<IsInstantiationOf<std::pair> ... Element_>
	constexpr StaticMap(Element_&& ... element) :
		array_{std::forward<Element_>(element)...}
	{
		static_assert(sizeof...(element) == size_, "StaticMap was initialized with wrong number of pairs.");
	}
	constexpr StaticMap(std::initializer_list<std::pair<A, B>>&& elements) 
	{
		if (elements.size() != size_) {
			throw std::length_error{"StaticMap was initialized with wrong number of pairs."};
		}
		std::ranges::move(elements, array_.begin());
	}
	constexpr StaticMap(std::initializer_list<std::pair<A, B>> const& elements) 
	{
		if (elements.size() != size_) {
			throw std::length_error{"StaticMap was initialized with wrong number of pairs."};
		}
		std::ranges::copy(elements, array_.begin());
	}

private:
	ContainerType array_;
};

template<IsInstantiationOf<std::pair> ... Element_>
StaticMap(Element_&& ... element) -> StaticMap<decltype((element, ...).first), decltype((element, ...).second), sizeof...(Element_)>;

} // namespace avo::utils

#endif
