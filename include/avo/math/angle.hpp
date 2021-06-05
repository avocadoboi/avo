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

#ifndef AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "arithmetic_wrapper.hpp"

#include <numbers>
#include <cmath>

namespace avo::math {

template<std::floating_point T>
struct Radians : ArithmeticBase<T> {};

template<class T>
Radians(T) -> Radians<T>;

template<utils::IsNumber T>
struct Degrees : ArithmeticBase<T> {};

template<class T>
Degrees(T) -> Degrees<T>;

template<class T>
concept IsDegrees = utils::IsInstantiationOf<T, Degrees>;

template<class T>
concept IsRadians = utils::IsInstantiationOf<T, Radians>;

template<class T>
concept IsAngle = IsRadians<T> || IsDegrees<T>;

/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<utils::IsNumber To_, std::floating_point From_>
[[nodiscard]]
constexpr Degrees<To_> to_degrees(Radians<From_> const radians) noexcept {
	if constexpr (std::integral<To_>) {
		return Degrees{static_cast<To_>(std::round(radians.value / std::numbers::pi_v<From_> * static_cast<From_>(180)))};
	}
	return Degrees{static_cast<To_>(radians.value / std::numbers::pi_v<From_> * static_cast<From_>(180))};
}
/*
	Converts an angle to degrees.
	If the destination type is integral then the value is rounded.
*/
template<utils::IsNumber To_, utils::IsNumber From_>
[[nodiscard]]
constexpr Degrees<To_> to_degrees(Degrees<From_> const degrees) noexcept {
	if constexpr (std::integral<To_> && std::floating_point<From_>) {
		return Degrees{static_cast<To_>(std::round(degrees.value))};
	}
	return Degrees{static_cast<To_>(degrees.value)};
}

/*
	Converts an angle to radians.
*/
template<std::floating_point To_, utils::IsNumber From_>
[[nodiscard]]
constexpr Radians<To_> to_radians(Degrees<From_> const degrees) noexcept {
	return Radians{static_cast<To_>(degrees.value / static_cast<To_>(180) * std::numbers::pi_v<To_>)};
}
/*
	Converts an angle to radians.
*/
template<std::floating_point To_, std::floating_point From_>
[[nodiscard]]
constexpr Radians<To_> to_radians(Radians<From_> const radians) noexcept {
	return Radians{static_cast<To_>(radians.value)};
}

/*
	Converts an angle to another angle type.
*/
template<IsRadians To_>
[[nodiscard]]
constexpr To_ angle_as(IsAngle auto const angle) noexcept {
	return to_radians<typename To_::value_type>(angle);
}

/*
	Converts an angle to another angle type.
*/
template<IsDegrees To_>
[[nodiscard]]
constexpr To_ angle_as(IsAngle auto const angle) noexcept {
	return to_degrees<typename To_::value_type>(angle);
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Degrees<T> const degrees) noexcept {
	return degrees.value / 360;
}
template<std::floating_point Return_, std::integral T>
[[nodiscard]]
constexpr Return_ normalized(Degrees<T> const degrees) noexcept {
	return static_cast<Return_>(degrees.value) / 360;
}

template<std::floating_point T>
[[nodiscard]]
constexpr T normalized(Radians<T> const radians) noexcept {
	return radians.value / (std::numbers::pi_v<T>*2);
}

inline namespace angle_literals {

constexpr Degrees<double> operator"" _deg(long double const value) noexcept {
	return Degrees{static_cast<double>(value)};
}
constexpr Degrees<float> operator"" _degf(long double const value) noexcept {
	return Degrees{static_cast<float>(value)};
}
constexpr Degrees<int> operator"" _deg(unsigned long long const value) noexcept {
	return Degrees{static_cast<int>(value)};
}
constexpr Radians<double> operator"" _rad(long double const value) noexcept {
	return Radians{static_cast<double>(value)};
}
constexpr Radians<float> operator"" _radf(long double const value) noexcept {
	return Radians{static_cast<float>(value)};
}

} // namespace angle_literals

namespace literals {

using namespace angle_literals;

} // namespace literals

} // namespace avo::math

#endif 
