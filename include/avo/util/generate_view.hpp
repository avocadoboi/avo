#ifndef AVO_UTILS_GENERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_GENERATE_VIEW_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"

#include <optional>

namespace avo::util {

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
			return last_value_;
		}
		[[nodiscard]]
		constexpr value_type operator*() && {
			return std::move(last_value_);
		}

		constexpr Iterator& operator++() 
		{
			if (generate_) {
				if (auto value = (*generate_)()) {
					last_value_ = std::move(*value);
				}
				else {
					generate_ = std::nullopt;
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
		constexpr bool operator==(std::default_sentinel_t) const {
			return not generate_;
		}
		[[nodiscard]]
		constexpr bool operator==(Iterator const&) const
			requires std::equality_comparable<value_type> &&
				std::equality_comparable<Generator_>
			= default;

		constexpr Iterator(Generator_ const& generate) :
			generate_{generate}
		{
			operator++();
		}

		constexpr Iterator() = default;
		constexpr ~Iterator() = default;

		constexpr Iterator(Iterator const&) = default;

// TODO: remove MSVC special treatment when it stops being buggy
#ifdef _MSC_VER
		constexpr Iterator& operator=(Iterator const& other)
			requires std::copyable<Generator_>
		{
			last_value_ = other.last_value_;
			generate_ = other.generate_;
			return *this;
		}
#else
		constexpr Iterator& operator=(Iterator const&) 
			requires std::copyable<Generator_>
			= default;
#endif
			
		constexpr Iterator& operator=(Iterator const& other)
			requires (not std::copyable<Generator_>)
		{
			if (other.generate_) {
				generate_.emplace(*other.generate_);
			}
			else {
				generate_ = std::nullopt;
			}
			last_value_ = other.last_value_;
			return *this;
		}

		constexpr Iterator(Iterator&&) = default;

#ifdef _MSC_VER
		constexpr Iterator& operator=(Iterator&& other)
			requires std::movable<Generator_>
		{
			last_value_ = std::move(other.last_value_);
			generate_ = std::move(other.generate_);
			return *this;
		}
#else
		constexpr Iterator& operator=(Iterator&&) 
			requires std::movable<Generator_> 
			= default;
#endif
		constexpr Iterator& operator=(Iterator&& other) 
			requires (not std::movable<Generator_>) 
		{
			if (other.generate_) {
				generate_.emplace(std::move(*other.generate_));
			}
			else {
				generate_ = std::nullopt;
			}
			last_value_ = std::move(other.last_value_);
			return *this;
		}

	private:
		value_type last_value_{};
		std::optional<Generator_> generate_{};
	};
	
	[[nodiscard]]
	constexpr Iterator begin() const& {
		return Iterator{generate_};
	}
	[[nodiscard]]
	constexpr Iterator begin() && {
		return Iterator{std::move(generate_)};
	}

	[[nodiscard]]
	constexpr std::default_sentinel_t end() const {
		return {};
	}
	
	constexpr GenerateView(Generator_ generate) :
		generate_{std::move(generate)}
	{}

	constexpr GenerateView(GenerateView const&) = default;
	constexpr GenerateView& operator=(GenerateView const&)
		requires std::copyable<Generator_>
		= default;
	constexpr GenerateView& operator=(GenerateView const& other)
		requires std::copy_constructible<Generator_>
	{
		if (&other != this) {
			std::destroy_at(std::addressof(generate_));
			std::construct_at(std::addressof(generate_), other.generate_);
		}

		return *this;
	}
	
	constexpr GenerateView(GenerateView&&) = default;
	constexpr GenerateView& operator=(GenerateView&&)
		requires std::movable<Generator_>
		= default;
	constexpr GenerateView& operator=(GenerateView&& other)
		requires std::move_constructible<Generator_>
	{
		if (&other != this) {
			std::destroy_at(std::addressof(generate_));
			std::construct_at(std::addressof(generate_), std::move(other));
		}

		return *this;
	}

private:
	Generator_ generate_;
};

} // namespace avo::util

namespace std::ranges {

template<class T>
constexpr auto enable_borrowed_range<avo::util::GenerateView<T>> = true;

} // namespace std::ranges

namespace avo::util {

template<class Generator_>
constexpr GenerateView<Generator_> generate(Generator_&& generator) {
	return GenerateView{std::forward<Generator_>(generator)};
}

} // namespace avo::util

#endif
