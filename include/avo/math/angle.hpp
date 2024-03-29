#ifndef AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "arithmetic_wrapper.hpp"
#include "operations.hpp"

#include <numbers>
#include <cmath>

namespace avo::math {

template<std::floating_point T>
struct Radians : ArithmeticBase<T> {};

template<class T>
Radians(T) -> Radians<T>;

template<util::IsNumber T>
struct Degrees : ArithmeticBase<T> {};

template<class T>
Degrees(T) -> Degrees<T>;

template<class T>
concept IsDegrees = util::IsInstantiationOf<T, Degrees>;

template<class T>
concept IsRadians = util::IsInstantiationOf<T, Radians>;

template<class T>
concept IsAngle = IsRadians<T> || IsDegrees<T>;

/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<util::IsNumber To_, std::floating_point From_>
[[nodiscard]]
constexpr Degrees<To_> to_degrees(Radians<From_> const radians) {
	if constexpr (std::integral<To_>) {
		return Degrees{math::round<To_>(radians.value / std::numbers::pi_v<From_> * static_cast<From_>(180))};
	}
	else {
		return Degrees{static_cast<To_>(radians.value / std::numbers::pi_v<From_> * static_cast<From_>(180))};
	}
}
/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<util::IsNumber To_, util::IsNumber From_>
[[nodiscard]]
constexpr Degrees<To_> to_degrees(Degrees<From_> const degrees) {
	if constexpr (std::integral<To_> && std::floating_point<From_>) {
		return Degrees{math::round<To_>(degrees.value)};
	}
	return Degrees{static_cast<To_>(degrees.value)};
}

/*
	Converts an angle to radians.
*/
template<std::floating_point To_, util::IsNumber From_>
[[nodiscard]]
constexpr Radians<To_> to_radians(Degrees<From_> const degrees) {
	return Radians{static_cast<To_>(degrees.value / static_cast<To_>(180) * std::numbers::pi_v<To_>)};
}
/*
	Converts an angle to radians.
*/
template<std::floating_point To_, std::floating_point From_>
[[nodiscard]]
constexpr Radians<To_> to_radians(Radians<From_> const radians) {
	return Radians{static_cast<To_>(radians.value)};
}

/*
	Converts an angle to another angle type.
*/
template<IsRadians To_>
[[nodiscard]]
constexpr To_ angle_as(IsAngle auto const angle) {
	return to_radians<typename To_::value_type>(angle);
}

/*
	Converts an angle to another angle type.
*/
template<IsDegrees To_>
[[nodiscard]]
constexpr To_ angle_as(IsAngle auto const angle) {
	return to_degrees<typename To_::value_type>(angle);
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Degrees<T> const degrees) {
	return degrees.value / 360;
}
template<std::floating_point Return_, std::integral T>
[[nodiscard]]
constexpr Return_ normalized(Degrees<T> const degrees) {
	return static_cast<Return_>(degrees.value) / 360;
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Radians<T> const radians) {
	return radians.value / (std::numbers::pi_v<T>*2);
}

/*
	Returns the pair of cosine and sine values for any angle.
*/
template<std::floating_point Return_>
std::pair<Return_, Return_> cos_sin(IsAngle auto angle) {
	auto const radians = to_radians<Return_>(angle);
	return std::pair{ std::cos(radians.value), std::sin(radians.value) };
}

inline namespace angle_literals {

constexpr Degrees<double> operator"" _deg(long double const value) {
	return Degrees{static_cast<double>(value)};
}
constexpr Degrees<float> operator"" _degf(long double const value) {
	return Degrees{static_cast<float>(value)};
}
constexpr Degrees<int> operator"" _deg(unsigned long long const value) {
	return Degrees{static_cast<int>(value)};
}
constexpr Radians<double> operator"" _rad(long double const value) {
	return Radians{static_cast<double>(value)};
}
constexpr Radians<float> operator"" _radf(long double const value) {
	return Radians{static_cast<float>(value)};
}

} // namespace angle_literals

namespace literals {

using namespace angle_literals;

} // namespace literals

} // namespace avo::math

#endif 
