#ifndef AVO_MATH_EASING_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_EASING_HPP_BJORN_SUNDIN_JUNE_2021

#include "vector2d.hpp"

namespace avo::math {

/*
	Cubic bezier animation easing.
	Try out this, can be useful for making your easing curves:
	http://bjornsundin.com/projects/cubic-bezier-easing

	Ease in example:
	Easing{Point{0.7f, 0.f}, Point{1.f, 1.f}}.ease_value(x)
	Ease out example:
	Easing{Point{0.f, 0.f}, Point{0.3f, 1.f}}.ease_value(x)
	See Easing::ease_value() for more info.

	Storing Easing objects in a Theme can be a good idea because you can use the same easings within your whole
	application, or different parts of it.
*/
struct Easing final {
	Point<> c0, c1;

	constexpr bool operator==(Easing const&) const = default;

	static constexpr auto default_precision = 5e-3f;

	/*
		Transforms a normalized value according to a cubic bezier curve.
		c0 is the first control point and c1 is the second one.
		precision is the maximum amount of error in the output value.

		It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
		f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (value is x) and not t. This why we have a precision parameter.
	*/
	static constexpr float ease_value(
		Point<> const c0, Point<> const c1, 
		float const value, float const precision = default_precision
	) noexcept {
		constexpr auto extreme_value_threshold = 1e-5f;
		
		if (value <= extreme_value_threshold) {
			return 0.f;
		}
		if (value >= 1.f - extreme_value_threshold) {
			return 1.f;
		}

		auto t = value < 0.5f ? 0.25f : 0.75f;

		/*
			f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

			f'(x) = x0*(3 - 12*t + 9*t*t) + x1*(6*t - 9*t*t) + 3*t*t
				  = x0*9*(t - 1)*(t - 1/3) + t*(x1*(6 - 9*t) + 3*t)
		*/

		for (auto error = 1.f; math::abs(error) > precision;) {
			error = value - t * ((1.f - t) * (3.f * (1.f - t) * c0.x + 3.f * t * c1.x) + t * t);
			t += error / (c0.x * 9.f * (t - 1.f) * (t - 1.f / 3.f) + t * (c1.x * (6.f - 9.f * t) + 3.f * t));
		}

		return t * ((1.f - t) * (3.f * (1.f - t) * c0.y + 3.f * t * c1.y) + t * t);
	}

	constexpr float ease_value(float const value, float const precision = default_precision) const noexcept {
		return ease_value(c0, c1, value, precision);
	}
	constexpr float ease_value_inverse(float const value, float const precision = default_precision) const noexcept {
		return ease_value(Point{c0.y, c0.x}, Point{c1.y, c1.x}, value, precision);
	}
};

} // namespace avo::math

#endif
