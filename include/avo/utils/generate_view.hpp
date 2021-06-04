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

#ifndef AVO_UTILS_GENERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_GENERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021

namespace avo::utils {

template<std::regular_invocable Generator_> requires 
	std::copy_constructible<Generator_> && 
	IsInstantiationOf<std::invoke_result_t<Generator_>, std::optional>
class GenerateView : public std::ranges::view_interface<GenerateView<Generator_>>
{
public:
	class Iterator {
	public:
		using value_type = std::invoke_result_t<Generator_>::value_type;
		using reference = value_type const&;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
		using iterator_concept = std::input_iterator_tag;
	
		[[nodiscard]]
		constexpr reference operator*() const& {
			return _last_value;
		}
		[[nodiscard]]
		constexpr value_type operator*() && {
			return std::move(_last_value);
		}

		constexpr Iterator& operator++() 
		{
			if (_generate) {
				if (auto value = (*_generate)()) {
					_last_value = std::move(*value);
				}
				else {
					_generate = std::nullopt;
				}
			}
			return *this;
		}
		[[nodiscard("Use prefix increment if you don't need the previous iterator.")]]
		constexpr Iterator operator++(int) 
		{
			auto before = *this;
			operator++();
			return before;
		}
		[[nodiscard]]
		constexpr bool operator==(std::default_sentinel_t) const noexcept {
			return !_generate;
		}
		[[nodiscard]]
		constexpr bool operator==(Iterator const&) const noexcept
			requires std::equality_comparable<value_type> &&
				std::equality_comparable<Generator_>
			= default;

		constexpr Iterator(Generator_ const& generate) :
			_generate{generate}
		{
			operator++();
		}

		constexpr Iterator() = default;
		constexpr ~Iterator() = default;

		constexpr Iterator(Iterator const&) = default;

		constexpr Iterator& operator=(Iterator const&) 
			requires std::copyable<Generator_>
			= default;
		constexpr Iterator& operator=(Iterator const& other)
			requires (!std::copyable<Generator_>)
		{
			if (other._generate) {
				_generate.emplace(*other._generate);
			}
			else {
				_generate = std::nullopt;
			}
			_last_value = other._last_value;
			return *this;
		}

		constexpr Iterator(Iterator&&) = default;

		constexpr Iterator& operator=(Iterator&&) requires std::movable<Generator_> = default;
		constexpr Iterator& operator=(Iterator&& other) noexcept 
			requires (!std::movable<Generator_>) 
		{
			if (other._generate) {
				_generate.emplace(std::move(*other._generate));
			}
			else {
				_generate = std::nullopt;
			}
			_last_value = std::move(other._last_value);
			return *this;
		}

	private:
		value_type _last_value{};
		std::optional<Generator_> _generate{};
	};
	
	[[nodiscard]]
	constexpr Iterator begin() const& {
		return Iterator{_generate};
	}
	[[nodiscard]]
	constexpr Iterator begin() && {
		return Iterator{std::move(_generate)};
	}

	[[nodiscard]]
	constexpr std::default_sentinel_t end() const noexcept {
		return {};
	}
	
	constexpr GenerateView(Generator_ generate) noexcept :
		_generate{std::move(generate)}
	{}

private:
	Generator_ _generate;
};

} // namespace avo::utils

namespace std::ranges {

template<class T>
constexpr auto enable_borrowed_range<avo::utils::GenerateView<T>> = true;

} // namespace std::ranges

namespace avo::utils {

template<class Generator_>
constexpr GenerateView<Generator_> generate(Generator_&& generator) {
	return GenerateView{std::forward<Generator_>(generator)};
}

} // namespace avo::utils

#endif
