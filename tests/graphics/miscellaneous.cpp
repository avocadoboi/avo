#include <avo/graphics/miscellaneous.hpp>

static_assert([]{
	constexpr auto corners = avo::graphics::RectangleCorners{
		.top_left{{5.f, 5.f}}, .top_right{{5.f, 5.f}}, 
		.bottom_left{{5.f, 5.f}}, .bottom_right{{5.f, 5.f}}
	};
	return avo::graphics::RectangleCorners<>::uniform({avo::math::Size{5.f, 5.f}, avo::graphics::CornerType::Round}) == corners;
}(), "avo::graphics::RectangleCorners do not work as expected.");
