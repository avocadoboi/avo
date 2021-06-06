#ifndef AVO_MATH_TRANSFORM_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_TRANSFORM_HPP_BJORN_SUNDIN_JUNE_2021

#include "vector2d.hpp"

namespace avo::math {

template<std::floating_point T>
struct Transform final {
	using value_type = T;
	
	T x_to_x{1}, y_to_x{0}, offset_x{0},
	  x_to_y{0}, y_to_y{1}, offset_y{0};

	[[nodiscard]]
	constexpr bool operator==(Transform const&) const = default;

	/*
		Multiplies this 2x3 matrix to a column vector that has an implicit third column with the value 1.

		[a b c]   [x]   [ax+by+c]
		[d e f] * [y] = [dx+ey+f]
		          [1]
	*/
	template<utils::IsNumber U, template<class> class Vector_> requires Is2dVectorTemplate<Vector_>
	[[nodiscard]]
	constexpr auto operator*(Vector_<U> const vector) const noexcept {
		return Vector_{
			x_to_x*vector.x + y_to_x*vector.y + offset_x, 
			x_to_y*vector.x + y_to_y*vector.y + offset_y
		};
	}

	/*
		Returns the result of applying this transform to another transform.
		Note that the order of multiplication matters.

		(A*B)*v = A*(B*v) where v is a vector, A and B are transforms.

		[a b c]   [g h i]   [ag+bj ah+bk ai+bl+c]
		[d e f] * [j k l] = [dg+ej dh+ek di+el+f]
		[0 0 1]   [0 0 1]   [0     0     1      ]
	*/
	template<std::floating_point U>
	[[nodiscard]]
	constexpr auto operator*(Transform<U> const& other) const noexcept {
		return Transform<std::common_type_t<T, U>>{
			.x_to_x = x_to_x*other.x_to_x + y_to_x*other.x_to_y,
			.y_to_x = x_to_x*other.y_to_x + y_to_x*other.y_to_y,
			.offset_x = x_to_x*other.offset_x + y_to_x*other.offset_y + offset_x,
			.x_to_y = x_to_y*other.x_to_x + y_to_y*other.x_to_y,
			.y_to_y = x_to_y*other.y_to_x + y_to_y*other.y_to_y,
			.offset_y = x_to_y*other.offset_x + y_to_y*other.offset_y + offset_y
		};
	}
	/*
		A *= B is equivalent to A = B*A.
	*/
	template<std::floating_point U>
	constexpr Transform& operator*=(Transform<U> const& other) const noexcept {
		return *this = other * *this;	
	}

	/*
		Rotates transformed points anticlockwise from the right.
	*/
	Transform& rotate(IsAngle auto const angle) {
		/*
			[cos -sin  0]   [x_to_x y_to_x offset_x]
			[sin  cos  0] * [x_to_y y_to_y offset_y]
			[0    0    1]   [0      0      1       ]
		*/
		auto const [cos, sin] = cos_sin<long double>(angle);

		// Previous transformation
		auto const p = *this;

		x_to_x = static_cast<T>(cos*p.x_to_x - sin*p.x_to_y);
		y_to_x = static_cast<T>(cos*p.y_to_x - sin*p.y_to_y);
		offset_x = static_cast<T>(cos*p.offset_x - sin*p.offset_y);
		x_to_y = static_cast<T>(sin*p.x_to_x + cos*p.x_to_y);
		y_to_y = static_cast<T>(sin*p.y_to_x + cos*p.y_to_y);
		offset_y = static_cast<T>(sin*p.offset_x + cos*p.offset_y);

		return *this;
	}
	/*
		Rotates transformed points around an origin anticlockwise from the right.
	*/
	Transform& rotate(IsAngle auto const angle, Is2dVector auto const origin) {
		translate(-origin);
		rotate(angle);
		translate(origin);
		return *this;
	}

	/*
		Moves the translation by an offset.
	*/
	constexpr Transform& translate(Is2dVector auto const offset) noexcept {
		offset_x += offset.x;
		offset_y += offset.y;
		return *this;
	}
	/*
		Sets the absolute offset in coordinates caused by the transform.
	*/
	constexpr Transform& translation(Is2dVector auto const point) noexcept {
		offset_x = point.x;
		offset_y = point.y;
		return *this;
	}

	/*
		Scales the transform by a horizontal and vertical factor.
	*/
	constexpr Transform& scale(Is2dVector auto const scale_factor) noexcept {
		x_to_x *= scale_factor.x;
		y_to_x *= scale_factor.x;
		offset_x *= scale_factor.x;
		y_to_y *= scale_factor.y;
		x_to_y *= scale_factor.y;
		offset_y *= scale_factor.y;
		return *this;
	}
	constexpr Transform& scale_x(utils::IsNumber auto const scale_factor) noexcept {
		x_to_x *= scale_factor;
		y_to_x *= scale_factor;
		offset_x *= scale_factor;
		return *this;
	}
	constexpr Transform& scale_y(utils::IsNumber auto const scale_factor) noexcept {
		y_to_y *= scale_factor;
		x_to_y *= scale_factor;
		offset_y *= scale_factor;
		return *this;
	}
};

template<class T>
concept IsTransform = utils::IsInstantiationOf<T, Transform>;

/*
	Returns the inverse of a transformation matrix I such that:
	    [a b c]   [1 0 0]
	I * [d e f] = [0 1 0]
	    [0 0 1]   [0 0 1]
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> inverse(Transform<T> const t) noexcept {
	auto const divisor = t.x_to_x*t.y_to_y - t.y_to_x*t.x_to_y;
	return Transform{
		t.y_to_y/divisor,
		-t.y_to_x/divisor,
		(t.y_to_x*t.offset_y - t.offset_x*t.y_to_y)/divisor,
		-t.x_to_y/divisor,
		t.x_to_x/divisor,
		(t.offset_x*t.x_to_y - t.x_to_x*t.offset_y)/divisor,
	};
}

/*
	Returns a rotated copy of the Transform argument.
	See Transform::rotate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> rotated(Transform<T> transform, IsAngle auto const angle) {
	return transform.rotate(angle);
}
/*
	Returns a rotated copy of the Transform argument.
	See Transform::rotate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> rotated(Transform<T> transform, IsAngle auto const angle, Is2dVector auto const origin) {
	return transform.rotate(angle, origin);
}

/*
	Returns a translated copy of the Transform argument.
	See Transform::translate.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> translated(Transform<T> transform, Is2dVector auto const offset) {
	return transform.translate(offset);
}
/*
	Returns a copy of the Transform argument with a specific absolute translation.
	See Transform::translation.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> with_translation(Transform<T> transform, Is2dVector auto const point) {
	return transform.translation(point);
}

/*
	Returns a scaled copy of the Transform argument.
	See Transform::scale.
*/
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled(Transform<T> transform, Is2dVector auto const scale_factor) noexcept {
	return transform.scale(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled_x(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept {
	return transform.scale_x(scale_factor);
}
template<std::floating_point T>
[[nodiscard]]
constexpr Transform<T> scaled_y(Transform<T> transform, utils::IsNumber auto const scale_factor) noexcept {
	return transform.scale_y(scale_factor);
}

} // namespace avo::math

template<class T>
struct fmt::formatter<T, std::enable_if_t<avo::math::IsTransform<T>, char>> 
	: fmt::formatter<typename T::value_type> 
{
	using formatter<typename T::value_type>::format;

	auto format(T const t, auto& context) {
		fmt::format_to(context.out(), "[");
		format(t.x_to_x, context);
		fmt::format_to(context.out(), " ");
		format(t.y_to_x, context);
		fmt::format_to(context.out(), " ");
		format(t.offset_x, context);
		fmt::format_to(context.out(), "]\n[");
		format(t.x_to_y, context);
		fmt::format_to(context.out(), " ");		
		format(t.y_to_y, context);
		fmt::format_to(context.out(), " ");
		format(t.offset_y, context);
		return fmt::format_to(context.out(), "]");
	}
};

#endif
