#ifndef AVO_MATH_VECTOR2D_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_VECTOR2D_HPP_BJORN_SUNDIN_JUNE_2021

#include "angle.hpp"

#include <cmath>

namespace avo::math {

template<util::IsNumber>
struct Vector2dBase;

/*
	Evaluates to whether a type is a 2d vector or not.
*/
template<class T, class Value_ = typename T::value_type>
concept Is2dVector = std::derived_from<T, Vector2dBase<Value_>>;

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr bool operator==(Class_<A> const first, Class_<B> const second) {
	return first.x == second.x && first.y == second.y;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr std::partial_ordering operator<=>(Class_<A> const first, Class_<B> const second) 
{
	if (first.x < second.x && first.y < second.y) {
		return std::partial_ordering::less;
	}
	if (first.x > second.x && first.y > second.y) {
		return std::partial_ordering::greater;
	}
	if (first == second) {
		return std::partial_ordering::equivalent;
	}
	return std::partial_ordering::unordered;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_> 
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator+(Class_<A> const first, Class_<B> const second) {
	return Class_{first.x + second.x, first.y + second.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator+=(Class_<A>& first, Class_<B> const second) {
	first.x += second.x;
	first.y += second.y;
	return first;
}

template<Is2dVector Vector_>
[[nodiscard]]
constexpr Vector_ operator-(Vector_ const vector) {
	return Vector_{vector.x, vector.y};
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator-(Class_<A> const first, Class_<B> const second) {
	return Class_{first.x - second.x, first.y - second.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator-=(Class_<A>& first, Class_<B> const second) {
	first.x -= second.x;
	first.y -= second.y;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator*(Class_<A> const first, B const second) {
	return Class_{first.x*second, first.y*second};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator*(B const first, Class_<A> const second) {
	return Class_{second.x*first, second.y*first};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator*=(Class_<A>& first, B const second) {
	first.x *= second;
	first.y *= second;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator*(Class_<A> const first, Class_<B> const second) {
	return Class_{first.x*second.x, first.y*second.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator*=(Class_<A>& first, Class_<B> const second) {
	first.x *= second.x;
	first.y *= second.y;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto scaled(Class_<A> const first, B const second) {
	return first * second;
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto scaled(Class_<A> const first, Class_<B> const second) {
	return first * second;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator/(Class_<A> const first, B const second) {
	return Class_{first.x/second, first.y/second};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator/(B const first, Class_<A> const second) {
	return Class_{first/second.x, first/second.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator/=(Class_<A>& first, B const second) {
	first.x /= second;
	first.y /= second;
	return first;
}

template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
[[nodiscard]]
constexpr auto operator/(Class_<A> const first, Class_<B> const second) {
	return Class_{first.x/second.x, first.y/second.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Class_>
	requires Is2dVector<Class_<A>> && Is2dVector<Class_<B>>
constexpr Class_<A>& operator/=(Class_<A>& first, Class_<B> const second) {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

/*
	Creates a square 2d vector, that is a vector with both coordinates equal.
*/
template<template<class> class Vector_, util::IsNumber Value_> requires Is2dVector<Vector_<Value_>>
constexpr Vector_<Value_> square(Value_ const side_length) {
	return Vector_{side_length, side_length};
}

template<std::floating_point T, template<class> class Vector_> requires Is2dVector<Vector_<T>>
constexpr Vector_<T> interpolate(Vector_<T> const a, Vector_<T> const b, T const c) {
	return Vector_{
		std::lerp(a.x, b.x, c),
		std::lerp(a.y, b.y, c),
	};
}

/*
	Creates a 2d vector from polar coordinates.
	The angle goes anticlockwise.
*/
template<template<class> class Vector_, std::floating_point Length_> requires Is2dVector<Vector_<Length_>>
[[nodiscard]]
auto polar(IsAngle auto const angle, Length_ const length) {
	auto const [x, y] = cos_sin(angle);
	return Vector_{x*length, y*length};
}
/*
	Creates a unit 2d vector from an angle that goes anticlockwise.
*/
template<template<class> class Vector_> requires Is2dVector<Vector_<float>>
[[nodiscard]]
auto polar(IsAngle auto const angle) {
	auto const [x, y] = cos_sin(angle);
	return Vector_{x, y};
}

template<Is2dVector Vector_>
[[nodiscard]]
constexpr Vector_ with_negative_space_clipped(Vector_ vector) {
	vector.clip_negative_space();
	return vector;
}

/*
	Returns the dot product of two 2d vectors.
*/
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr auto dot(Vector_<A> const first, Vector_<B> const second) {
	return first.x*second.x + first.y*second.y;
}

/*
	Returns the cross product of two 2d vectors.
*/
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr auto cross(Vector_<A> const first, Vector_<B> const second) {
	return first.x*second.y - first.y*second.x;
}

/*
	Returns the euclidean distance between two 2d vectors.
*/
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
auto distance(Vector_<A> const first, Vector_<B> const second) {
	return std::hypot(second.x - first.x, second.y - first.y);
}
/*
	Returns the squared euclidean distance between two 2d vectors.
*/
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr auto distance_squared(Vector_<A> const first, Vector_<B> const second) {
	return square(second.x - first.x) + square(second.y - first.y);
}

/*
	Returns a vector that has the same angle as the argument but with a length of 1.
*/
template<Is2dVector T>
[[nodiscard]]
T normalized(T vector) {
	vector.normalize();
	return vector;
}
/*
	Returns a vector that has the same angle as the argument but with a length of 1, using a slightly faster algorithm.
*/
template<Is2dVector T>
[[nodiscard]]
T normalized_fast(T vector) {
	vector.normalize_fast();
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise.
*/
template<Is2dVector T>
[[nodiscard]]
T rotated(T vector, IsAngle auto const angle) {
	vector.rotate(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but is rotated by an angle clockwise relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
T rotated(T vector, IsAngle auto const angle, Is2dVector auto const origin) {
	vector.rotate(angle, origin);
	return vector;
}

/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise.
*/
template<Is2dVector T>
[[nodiscard]]
T with_angle(T vector, IsAngle auto const angle) {
	vector.angle(angle);
	return vector;
}
/*
	Returns a vector that has the same length as the input vector but has a different angle, measured anticlockwise
	relative to an origin.
*/
template<Is2dVector T>
[[nodiscard]]
T with_angle(T vector, IsAngle auto const angle, Is2dVector auto const origin) {
	vector.angle(angle, origin);
	return vector;
}

/*
	All 2d vector types derive from this type.
	It provides a bunch of 2d vector operations.
*/
template<util::IsNumber Value_>
struct Vector2dBase {
	using value_type = Value_;

	Value_ x, y;

	constexpr explicit operator bool() const {
		return x || y;
	}

	/*
		Returns the magnitude of the vector, or the hypotenuse of the triangle.
	*/
	[[nodiscard]]
	std::floating_point auto length() const {
		return std::hypot(x, y);
	}
	/*
		Returns the squared magnitude of the vector, or the squared hypotenuse of the triangle.
	*/
	[[nodiscard]]
	constexpr Value_ length_squared() const {
		return x*x + y*y;
	}

	/*
		Sets any negative coordinates to 0.
	*/
	constexpr void clip_negative_space() {
		x = max(Value_{}, x);
		y = max(Value_{}, y);
	}

	/*
		Rotates the vector by an angle clockwise.
	*/
	void rotate(IsAngle auto const angle) {
		// A very small change in angle could result in a very big change in cartesian coordinates.
		// Therefore we use long double for these calculations and not Value_.
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const x_before = x;
		x = static_cast<Value_>(x*cos - y*sin);
		y = static_cast<Value_>(y*cos + x_before*sin);
	}
	/*
		Rotates the vector by an angle clockwise relative to an origin.
	*/
	void rotate(IsAngle auto const angle, Is2dVector auto const origin) {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const x_before = x;
		x = static_cast<Value_>((x - origin.x)*cos - (y - origin.y)*sin + origin.x);
		y = static_cast<Value_>((y - origin.y)*cos + (x_before - origin.x)*sin + origin.y);
	}

	/*
		Sets the angle of the vector measured anticlockwise from the right side.
	*/
	void angle(IsAngle auto const angle) {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const current_length = length();
		x = static_cast<Value_>(cos*current_length);
		y = static_cast<Value_>(sin*current_length);
	}
	/*
		Sets the angle of the vector measured anticlockwise from the right side relative to an origin.
	*/
	void angle(IsAngle auto const angle, Is2dVector auto const origin) {
		auto const [cos, sin] = cos_sin<long double>(angle);
		auto const length = distance(*this, origin);
		x = static_cast<Value_>(cos*length + origin.x);
		y = static_cast<Value_>(sin*length + origin.y);
	}

	/*
		Returns the angle of the vector measured anticlockwise from the right side.
	*/
	template<IsAngle Angle_>
	[[nodiscard]]
	Angle_ angle() const {
		if (x == 0 && y == 0) {
			return Angle_{};
		}
		auto const atan2 = std::atan2(static_cast<long double>(y), static_cast<long double>(x));
		if (atan2 < 0) {
			return angle_as<Angle_>(Radians{atan2 + 2*std::numbers::pi_v<long double>});
		}
		return angle_as<Angle_>(Radians{atan2});
	}
	/*
		Returns the angle of the vector measured anticlockwise from the right side relative to an origin.
	*/
	template<IsAngle Angle_>
	[[nodiscard]]
	Angle_ angle(Is2dVector auto const origin) const {
		if (x == origin.x && y == origin.y) {
			return Angle_{};
		}
		auto const atan2 = std::atan2(
			static_cast<long double>(y - origin.y), 
			static_cast<long double>(x - origin.x)
		);
		if (atan2 < 0) {
			return angle_as<Angle_>(Radians{atan2 + 2*std::numbers::pi_v<long double>});
		}
		return angle_as<Angle_>(Radians{atan2});
	}

	/*
		Keeps the angle of the vector but sets its length to 1.
	*/
	void normalize() {
		auto const current_length = length();
		x /= current_length;
		y /= current_length;
	}
	/*
		Keeps the angle of the vector but sets its length to 1 using a slightly faster algorithm.
	*/
	void normalize_fast() {
		auto const inverse_length = fast_inverse_sqrt(length_squared());
		x *= inverse_length;
		y *= inverse_length;
	}

	// TODO: Define these templates in this class when MSVC stops breaking from the using declaration.
	// template<Is2dVector Vector_>
	// [[nodiscard]]
	// constexpr Vector_ to() const {
	// 	return Vector_{
	// 		static_cast<typename Vector_::value_type>(x),
	// 		static_cast<typename Vector_::value_type>(y)
	// 	};
	// }

	// template<template<class> class Vector_>
	// 	requires Is2dVector<Vector_<Value_>>
	// [[nodiscard]]
	// constexpr Vector_<Value_> to() const {
	// 	return Vector_{x, y};
	// }
};

template<util::IsNumber Value_ = float>
struct Vector2d : Vector2dBase<Value_> {
	using Vector2dBase<Value_>::x;
	using Vector2dBase<Value_>::y;
	
	// using Vector2dBase<Value_>::to;
	template<Is2dVector Vector_>
	[[nodiscard]]
	constexpr Vector_ to() const {
		return Vector_{
			static_cast<typename Vector_::value_type>(x),
			static_cast<typename Vector_::value_type>(y)
		};
	}
	template<template<class> class Vector_>
		requires Is2dVector<Vector_<Value_>>
	[[nodiscard]]
	constexpr Vector_<Value_> to() const {
		return Vector_{x, y};
	}
	
	template<util::IsNumber T>
	[[nodiscard]]
	constexpr Vector2d<T> to() const {
		return {static_cast<T>(x), static_cast<T>(y)};
	}
};

template<class T>
Vector2d(T, T) -> Vector2d<T>;

template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr auto operator*(Vector2d<A> const factor, Vector_<B> const vector) {
	return Vector_{factor.x*vector.x, factor.y*vector.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr auto operator*(Vector_<A> const vector, Vector2d<B> const factor) {
	return Vector_{factor.x*vector.x, factor.y*vector.y};
}
template<util::IsNumber A, util::IsNumber B, template<class> class Vector_>
	requires Is2dVector<Vector_<A>> && Is2dVector<Vector_<B>>
[[nodiscard]]
constexpr Vector_<A>& operator*=(Vector_<A>& vector, Vector2d<B> const factor) {
	vector.x *= factor.x;
	vector.y *= factor.y;
	return vector;
}

template<std::floating_point T, template<class> class Vector_> requires Is2dVector<Vector_<T>>
constexpr Vector_<T> interpolate(Vector_<T> const a, Vector_<T> const b, Vector2d<T> const c) {
	return Vector_{
		std::lerp(a.x, b.x, c.x),
		std::lerp(a.y, b.y, c.y),
	};
}

template<util::IsNumber Value_ = float>
struct Point : Vector2dBase<Value_> {
	using Vector2dBase<Value_>::x;
	using Vector2dBase<Value_>::y;

	// using Vector2dBase<Value_>::to;
	template<Is2dVector Vector_>
	[[nodiscard]]
	constexpr Vector_ to() const {
		return Vector_{
			static_cast<typename Vector_::value_type>(x),
			static_cast<typename Vector_::value_type>(y)
		};
	}
	template<template<class> class Vector_>
		requires Is2dVector<Vector_<Value_>>
	[[nodiscard]]
	constexpr Vector_<Value_> to() const {
		return Vector_{x, y};
	}
	
	template<util::IsNumber T>
	[[nodiscard]]
	constexpr Point<T> to() const {
		return {static_cast<T>(x), static_cast<T>(y)};
	}
};

template<class T>
Point(T, T) -> Point<T>;

template<util::IsNumber Value_ = float>
struct Size : Vector2dBase<Value_> {
	using Vector2dBase<Value_>::x;
	using Vector2dBase<Value_>::y;

	// using Vector2dBase<Value_>::to;
	template<Is2dVector Vector_>
	[[nodiscard]]
	constexpr Vector_ to() const {
		return Vector_{
			static_cast<typename Vector_::value_type>(x),
			static_cast<typename Vector_::value_type>(y)
		};
	}
	template<template<class> class Vector_>
		requires Is2dVector<Vector_<Value_>>
	[[nodiscard]]
	constexpr Vector_<Value_> to() const {
		return Vector_{x, y};
	}

	template<util::IsNumber T>
	[[nodiscard]]
	constexpr Size<T> to() const {
		return {static_cast<T>(x), static_cast<T>(y)};
	}
};

template<class T>
Size(T, T) -> Size<T>;

} // namespace avo::math

// SFINAE seems to be necessary for template specialization here.
template<class T>
struct fmt::formatter<T, std::enable_if_t<avo::math::Is2dVector<T>, char>> 
	: fmt::formatter<typename T::value_type> 
{
	using formatter<typename T::value_type>::format;

	// TODO: Replace this concept with just T const when MSVC stops complaining. Same for the other custom formatters.
	auto format(std::same_as<T> auto const vector, auto& context) {
		fmt::format_to(context.out(), "(");
		format(vector.x, context);
		fmt::format_to(context.out(), ", ");
		format(vector.y, context);
		return fmt::format_to(context.out(), ")");
	}
};	
	
#endif
