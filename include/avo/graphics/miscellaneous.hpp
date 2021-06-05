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
