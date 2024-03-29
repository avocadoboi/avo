#ifndef AVO_MATH_OPERATIONS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_OPERATIONS_HPP_BJORN_SUNDIN_JUNE_2021

#include "../util/concepts.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace avo::math {

/*
	Returns 1 if the number is positive, 0 if it is 0 and -1 if it is negative.
*/
template<util::IsNumber T>
[[nodiscard]]
constexpr T sign(T const number) {
	return static_cast<T>((number > T{}) - (number < T{}));
}

template<std::floating_point T>
[[nodiscard]]
constexpr T unit_clamp(T const value) {
	return std::clamp(value, T{}, T{1});
}

template<util::IsNumber Return_, std::floating_point T>
[[nodiscard]]
constexpr Return_ floor(T const number) {
	if (std::is_constant_evaluated()) {
		return static_cast<Return_>(static_cast<std::int64_t>(number) - static_cast<std::int64_t>(number < 0));
	}
	else {
		return static_cast<Return_>(std::floor(number));
	}
}
template<util::IsNumber Return_, std::floating_point T>
[[nodiscard]]
constexpr Return_ ceil(T const number) {
	if (std::is_constant_evaluated()) {
		return static_cast<Return_>(static_cast<std::int64_t>(number) + static_cast<std::int64_t>(number > 0));
	}
	else {
		return static_cast<Return_>(std::ceil(number));
	}
}

template<util::IsNumber Return_, std::floating_point T>
[[nodiscard]]
constexpr Return_ round(T const number) {
	if (std::is_constant_evaluated()) {
		return static_cast<Return_>(static_cast<std::int64_t>(number + T{0.5}*sign(number)));
	}
	else {
		return static_cast<Return_>(std::round(number));
	}
}

template<util::IsNumber T>
[[nodiscard]]
constexpr T abs(T const number) {
	if (std::is_constant_evaluated()) {
		return number >= T{} ? number : -number;
	}
	else {
		return std::abs(number);
	}
}

template<std::floating_point T>
[[nodiscard]]
constexpr bool approximately_equal(T const a, T const b, T const max_difference = static_cast<T>(1e-6))
{
	return math::abs(a - b) <= max_difference;
}

/*
	Returns a number multiplied by itself.
	Can be useful if you want to quickly square a longer expression.
*/
template<util::IsNumber T>
[[nodiscard]]
constexpr T square(T const x) {
	return x*x;
}

/*
	Returns the inverse square root of a float using a faster but less accurate algorithm.
	It is about 8% to 15% faster than 1.f/std::sqrt(x) with gcc -O3 on my computer.
*/
[[nodiscard]]
inline float fast_inverse_sqrt(float const input) 
{
	static_assert(std::numeric_limits<float>::is_iec559, "fast_inverse_sqrt error: float type must follow IEEE 754-1985 standard.");
	static_assert(sizeof(float) == 4, "fast_inverse_sqrt error: sizeof(float) must be 4.");

	std::uint32_t bits;
	std::memcpy(&bits, &input, 4);

	bits = 0x5f3759df - bits/2;

	float approximation;
	std::memcpy(&approximation, &bits, 4);

	return approximation*(1.5f - 0.5f*input*approximation*approximation);
}

template<class T>
[[nodiscard]]
constexpr decltype(auto) max(T&& value) {
	return std::forward<T>(value);
}
/*
	Returns the parameter that is compared largest.
	All types must be totally ordered with each other.
	All types must be convertible to each other, but this is 
	best checked by the actual code than by a constraint.
*/
template<class T0, class T1, class ... T2> requires std::totally_ordered_with<T0, T1>
[[nodiscard]]
constexpr decltype(auto) max(T0&& first, T1&& second, T2&& ... arguments) 
{
	return (first > second) ? 
		max(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		max(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

template<class T>
[[nodiscard]]
constexpr decltype(auto) min(T&& value) {
	return std::forward<T>(value);
}
/*
	Returns the parameter that is compared smallest.
	All types must be totally ordered with each other.
	All types must be convertible to each other, but this is 
	best checked by the actual code than by a constraint.
*/
template<class T0, class T1, class ... T2> requires std::totally_ordered_with<T0, T1>
[[nodiscard]]
constexpr decltype(auto) min(T0&& first, T1&& second, T2&& ... arguments) 
{
	return (first < second) ? 
		min(std::forward<T0>(first), std::forward<T2>(arguments)...) : 
		min(std::forward<T1>(second), std::forward<T2>(arguments)...);
}

} // namespace avo::math

#endif
