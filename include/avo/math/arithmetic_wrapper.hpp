#ifndef AVO_MATH_ARITHMETIC_WRAPPER_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_ARITHMETIC_WRAPPER_HPP_BJORN_SUNDIN_JUNE_2021

#include "../utils/concepts.hpp"

#include <fmt/format.h>

namespace avo::math {

/*
	This can be used to define a strong type that wraps a primitive arithmetic type.
	Arithmetic operations can be made on types that derive from this struct.
*/
template<utils::IsNumber Value_>
struct ArithmeticBase {
	using value_type = Value_;
	
	value_type value;

	[[nodiscard]]
	constexpr explicit operator value_type() const noexcept {
		return value;
	}
};

template<class T>
concept IsArithmeticWrapper = std::derived_from<T, ArithmeticBase<typename T::value_type>>;

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr bool operator==(Class_<A> const first, Class_<B> const second) noexcept {
	return first.value == second.value;
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator<=>(Class_<A> const first, Class_<B> const second) noexcept {
	return first.value <=> second.value;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator+(Class_<A> const first, Class_<B> const second) noexcept {
	return Class_{first.value + second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator+=(Class_<A>& first, Class_<B> const second) noexcept {
	first.value += second.value;
	return first;
}

template<utils::IsNumber A, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(Class_<A> const value) noexcept {
	return Class_{-value.value};
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator-(Class_<A> const first, Class_<B> const second) noexcept {
	return Class_{first.value - second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator-=(Class_<A>& first, Class_<B> const second) noexcept {
	first.value -= second.value;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(Class_<A> const first, B const second) noexcept {
	return Class_{first.value*second};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator*(B first, Class_<A> const second) noexcept {
	return Class_{first*second.value};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator*=(Class_<A>& first, B const second) noexcept {
	first.value *= second;
	return first;
}

template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
[[nodiscard]]
constexpr auto operator/(Class_<A> const first, B const second) noexcept {
	return Class_{first.value/second};
}
template<utils::IsNumber A, utils::IsNumber B, template<utils::IsNumber> class Class_>
	requires std::derived_from<Class_<A>, ArithmeticBase<A>>
constexpr Class_<A>& operator/=(Class_<A>& first, B const second) noexcept {
	first.value /= second;
	return first;
}

} // namespace avo::math

template<class T> 
struct fmt::formatter<T, std::enable_if_t<avo::math::IsArithmeticWrapper<T>, char>> 
	: fmt::formatter<typename T::value_type> 
{
	auto format(T const arithmetic, auto& context) {
		return formatter<typename T::value_type>::format(arithmetic.value, context);
	}
};

#endif
