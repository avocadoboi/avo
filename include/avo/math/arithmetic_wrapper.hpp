#ifndef AVO_MATH_ARITHMETIC_WRAPPER_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_ARITHMETIC_WRAPPER_HPP_BJORN_SUNDIN_JUNE_2021

#include "../util/concepts.hpp"

#include <fmt/format.h>

namespace avo::math {

/*
	This can be used to define a strong type that wraps a primitive arithmetic type.
	Arithmetic operations can be made on types that derive from this struct.
*/
template<util::IsNumber Value_>
struct ArithmeticBase {
	using value_type = Value_;
	
	value_type value;

	[[nodiscard]]
	constexpr explicit operator value_type() const {
		return value;
	}
};

template<class T>
concept IsArithmeticWrapper = std::derived_from<T, ArithmeticBase<typename T::value_type>>;

template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr bool operator==(Class_<A> const first, Class_<B> const second) {
	return first.value == second.value;
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator<=>(Class_<A> const first, Class_<B> const second) {
	return first.value <=> second.value;
}

template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator+(Class_<A> const first, Class_<B> const second) {
	return Class_{first.value + second.value};
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator+=(Class_<A>& first, Class_<B> const second) {
	first.value += second.value;
	return first;
}

template<util::IsNumber A, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(Class_<A> const value) {
	return Class_{-value.value};
}

template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(Class_<A> const first, Class_<B> const second) {
	return Class_{first.value - second.value};
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator-=(Class_<A>& first, Class_<B> const second) {
	first.value -= second.value;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(Class_<A> const first, B const second) {
	return Class_{first.value*second};
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(B first, Class_<A> const second) {
	return Class_{first*second.value};
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator*=(Class_<A>& first, B const second) {
	first.value *= second;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator/(Class_<A> const first, B const second) {
	return Class_{first.value/second};
}
template<util::IsNumber A, util::IsNumber B, template<util::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator/=(Class_<A>& first, B const second) {
	first.value /= second;
	return first;
}

} // namespace avo::math

template<class T> 
struct fmt::formatter<T, std::enable_if_t<avo::math::IsArithmeticWrapper<T>, char>> 
	: fmt::formatter<typename T::value_type> 
{
	auto format(std::same_as<T> auto const arithmetic, auto& context) {
		return formatter<typename T::value_type>::format(arithmetic.value, context);
	}
};

#endif
