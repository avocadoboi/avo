#ifndef AVO_GRAPHICS_MISCELLANEOUS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_GRAPHICS_MISCELLANEOUS_HPP_BJORN_SUNDIN_JUNE_2021

#include "../math/vector2d.hpp"

namespace avo::graphics {

// Physical screen pixels
using Pixels = std::int32_t;

// Density independent pixels
using Dip = float;

//------------------------------

enum class CornerType {
	Round,
	Cut
};

struct Corner {
	math::Size<Dip> size;
	CornerType type;

	[[nodiscard]]
	constexpr bool operator==(Corner const&) const = default;
};

template<std::copy_constructible Corner_ = Corner>
struct RectangleCorners {
	using value_type = Corner_;
	
	Corner_ top_left;
	Corner_ top_right;
	Corner_ bottom_left;
	Corner_ bottom_right;

	[[nodiscard]]
	constexpr bool operator==(RectangleCorners const&) const 
		requires std::equality_comparable<Corner_> 
		= default;

	static constexpr RectangleCorners uniform(Corner_ const corner) {
		return {corner, corner, corner, corner};
	}
};

} // namespace avo::graphics

#endif
