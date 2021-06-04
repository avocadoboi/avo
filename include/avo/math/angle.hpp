

#ifndef AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_ANGLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "arithmetic_wrapper.hpp"

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

#ifdef BUILD_TESTING
static_assert(utils::IsTrivial<Degrees<int>>);
static_assert(std::same_as<decltype(Degrees{5} + Degrees{3.1}), Degrees<double>>);
static_assert(std::same_as<decltype(Degrees{5}*3.1), Degrees<double>>);
static_assert(3.f*(Radians{5.f} + Radians{3.f}*3.f)/2.f - Radians{3.f} == Radians{18.f}, "Radian arithmetic does not work");
static_assert([]{
	auto angle = Radians{5.f};
	angle += Radians{2.f};
	if (angle != Radians{7.f}) return false;
	angle -= Radians{2.f};
	if (angle != Radians{5.f}) return false;
	angle /= 2.f;
	if (angle != Radians{2.5f}) return false;
	angle *= 4.f;
	if (angle != Radians{10.f}) return false;
	return true;
}(), "Radian arithmetic does not work.");
static_assert(80_deg == Degrees{80} && -80_deg == Degrees{-80});
static_assert(80._deg == Degrees{80.} && 80._degf == Degrees{80.f});
static_assert(3.14_rad == Radians{3.14} && 3.14_radf == Radians{3.14f});
static_assert(Degrees{80} > Degrees{-30} && Degrees{-30} < Degrees{80});
static_assert(Degrees{50} == Degrees{50} && Degrees{50} != Degrees{51});
static_assert(to_radians<float>(Degrees{180.f}) == Radians{std::numbers::pi_v<float>});
static_assert(to_degrees<int>(Radians{std::numbers::pi_v<float>}) == Degrees{180});
static_assert(to_degrees<float>(Degrees{50}) == Degrees{50.f});
static_assert(normalized<float>(Degrees{90}) == 0.25f);
static_assert(normalized(Degrees{90.f}) == 0.25f);
static_assert(normalized(Radians{std::numbers::pi_v<float>/2}) == 0.25f);
#endif // BUILD_TESTING
	
} // namespace avo::math

#endif 
