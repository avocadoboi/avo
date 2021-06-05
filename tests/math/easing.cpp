#include <avo/math/easing.hpp>

using avo::math::Easing;

static_assert([]{
	auto const easing = Easing{{0.4f, 0.1f}, {0.7f, 0.5f}};
	return avo::math::approximately_equal(easing.ease_value_inverse(easing.ease_value(0.35f, 1e-5f), 1e-5f), 0.35f, 1e-5f);
}());
