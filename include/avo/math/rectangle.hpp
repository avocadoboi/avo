#ifndef AVO_MATH_RECTANGLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_RECTANGLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "vector2d.hpp"

#include <numeric>

namespace avo::math {

template<util::IsNumber Value_>
struct Rectangle final {
	using value_type = Value_;

	Value_ left{}, top{}, right{}, bottom{};

	constexpr Rectangle() = default;
	constexpr Rectangle(Value_ const p_left, Value_ const p_top, Value_ const p_right, Value_ const p_bottom) :
		left{p_left}, top{p_top}, right{p_right}, bottom{p_bottom}
	{}
	constexpr Rectangle(Size<Value_> const size) :
		left{}, top{}, right{size.x}, bottom{size.y}
	{}
	constexpr Rectangle(Point<Value_> const position) :
		left{position.x}, top{position.y},
		right{position.x}, bottom{position.y}
	{}
	constexpr Rectangle(Point<Value_> const position, Size<Value_> const size) :
		left{position.x}, top{position.y},
		right{position.x + size.x}, bottom{position.y + size.y}
	{}
	constexpr Rectangle(Point<Value_> const top_left, Point<Value_> const bottom_right) :
		left{top_left.x}, top{top_left.y},
		right{bottom_right.x}, bottom{bottom_right.y}
	{}

	[[nodiscard]]
	constexpr bool operator==(Rectangle const&) const = default;

	[[nodiscard]]
	constexpr Rectangle operator-() const {
		return Rectangle{-right, -bottom, -left, -top};
	}
	
	[[nodiscard]]
	constexpr auto operator+(Is2dVector auto const vector) const {
		return Rectangle<decltype(left + vector.x)>{
			left + vector.x,
			top + vector.y,
			right + vector.x,
			bottom + vector.y
		};
	}
	template<util::IsNumber T>
	[[nodiscard]]
	constexpr auto operator+(Size<T> const size) const {
		return Rectangle<std::common_type_t<Value_, T>>{
			left,
			top,
			right + size.x,
			bottom + size.y
		};
	}
	[[nodiscard]]
	constexpr auto operator-(Is2dVector auto const vector) const {
		return *this + (-vector);
	}

	constexpr Rectangle& offset_x(Value_ const offset) {
		left += offset;
		right += offset;
		return *this;
	}
	constexpr Rectangle& offset_y(Value_ const offset) {
		top += offset;
		bottom += offset;
		return *this;
	}
	constexpr Rectangle& offset(Is2dVector auto const offset) {
		offset_x(offset.x);
		offset_y(offset.y);
		return *this;
	}
	template<util::IsNumber T>
	constexpr Rectangle& offset(Size<T> const size_offset) {
		right += size_offset.x;
		bottom += size_offset.y;
		return *this;
	}
	constexpr Rectangle& operator+=(Is2dVector auto const offset) {
		return offset(offset);
	}
	constexpr Rectangle& operator-=(Is2dVector auto const offset) {
		return offset(-offset);
	}

	[[nodiscard]]
	constexpr auto operator*(util::IsNumber auto const factor) const {
		return Rectangle<decltype(left*factor)>{
			left*factor,
			top*factor,
			right*factor,
			bottom*factor
		};
	}
	constexpr Rectangle& operator*=(util::IsNumber auto const factor) {
		left *= factor;
		top *= factor;
		right *= factor;
		bottom *= factor;
		return *this;
	}
	[[nodiscard]]
	constexpr auto operator/(util::IsNumber auto const divisor) const {
		return Rectangle<decltype(left/divisor)>{
			left/divisor,
			top/divisor,
			right/divisor,
			bottom/divisor
		};
	}
	constexpr Rectangle& operator/=(util::IsNumber auto const divisor) {
		left /= divisor;
		top /= divisor;
		right /= divisor;
		bottom /= divisor;
		return *this;
	}

	constexpr Rectangle& width(Value_ const width) {
		right = left + width;
		return *this;
	}
	constexpr Rectangle& height(Value_ const height) {
		bottom = top + height;
		return *this;
	}
	constexpr Rectangle& size(Size<Value_> const size) {
		width(size.x);
		height(size.y);
		return *this;
	}

	[[nodiscard]]
	constexpr Size<Value_> size() const {
		return Size{right - left, bottom - top};
	}
	[[nodiscard]]
	constexpr Value_ width() const {
		return right - left;
	}
	[[nodiscard]]
	constexpr Value_ height() const {
		return bottom - top;
	}

	[[nodiscard]]
	constexpr Point<Value_> top_left() const {
		return Point{left, top};
	}
	template<bool keep_size = true>
	constexpr Rectangle& top_left(Point<Value_> const top_left) {
		if constexpr (keep_size) {
			right += top_left.x - left;
			bottom += top_left.y - top;
		}
		left = top_left.x;
		top = top_left.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<Value_> top_right() const {
		return Point{right, top};
	}
	template<bool keep_size = true>
	constexpr Rectangle& top_right(Point<Value_> const top_right) {
		if constexpr (keep_size) {
			left += top_right.x - right;
			bottom += top_right.y - top;
		}
		right = top_right.x;
		top = top_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<Value_> bottom_right() const {
		return Point{right, bottom};
	}
	template<bool keep_size = true>
	constexpr Rectangle& bottom_right(Point<Value_> const bottom_right) {
		if constexpr (keep_size) {
			left += bottom_right.x - right;
			top += bottom_right.y - bottom;
		}
		right = bottom_right.x;
		bottom = bottom_right.y;
		return *this;
	}

	[[nodiscard]]
	constexpr Point<Value_> bottom_left() const {
		return Point{left, bottom};
	}
	template<bool keep_size = true>
	constexpr Rectangle& bottom_left(Point<Value_> const bottom_left) {
		if constexpr (keep_size) {
			right += bottom_left.x - left;
			top += bottom_left.y - bottom;
		}
		left = bottom_left.x;
		bottom = bottom_left.y;
		return *this;
	}

	template<bool keep_size = true>
	constexpr Rectangle& set_left(Value_ const new_left) {
		if constexpr (keep_size) {
			right += new_left - left;
		}
		left = new_left;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_top(Value_ const new_top) {
		if constexpr (keep_size) {
			bottom += new_top - top;
		}
		top = new_top;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_right(Value_ const new_right) {
		if constexpr (keep_size) {
			left += new_right - right;
		}
		right = new_right;
		return *this;
	}
	template<bool keep_size = true>
	constexpr Rectangle& set_bottom(Value_ const new_bottom) {
		if constexpr (keep_size) {
			top += new_bottom - bottom;
		}
		bottom = new_bottom;
		return *this;
	}

	template<util::IsNumber T>
	constexpr Rectangle& center(Point<T> const center) {
		auto const half_size = size()/2;
		left = static_cast<Value_>(center.x - half_size.x);
		top = static_cast<Value_>(center.y - half_size.y);
		right = static_cast<Value_>(center.x + half_size.x);
		bottom = static_cast<Value_>(center.y + half_size.y);
		return *this;
	}
	constexpr Rectangle& center_x(util::IsNumber auto const center_x) {
		auto const half_width = width()/2;
		left = static_cast<Value_>(center_x - half_width);
		right = static_cast<Value_>(center_x + half_width);
		return *this;
	}
	constexpr Rectangle& center_y(util::IsNumber auto const center_y) {
		auto const half_height = height()/2;
		top = static_cast<Value_>(center_y - half_height);
		bottom = static_cast<Value_>(center_y + half_height);
		return *this;
	}
	template<util::IsNumber T = Value_>
	[[nodiscard]]
	constexpr Point<T> center() const {
		return Point{center_x(), center_y()};
	}
	template<util::IsNumber T = Value_>
	[[nodiscard]]
	constexpr T center_x() const {
		return std::midpoint(static_cast<T>(left), static_cast<T>(right));
	}
	template<util::IsNumber T = Value_>
	[[nodiscard]]
	constexpr Point<T> center_y() const {
		return std::midpoint(static_cast<T>(top), static_cast<T>(bottom));
	}

	constexpr Rectangle& move_top_left(Vector2d<Value_> const offset) {
		left += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr Rectangle& move_top_right(Vector2d<Value_> const offset) {
		right += offset.x;
		top += offset.y;
		return *this;
	}
	constexpr Rectangle& move_bottom_left(Vector2d<Value_> const offset) {
		left += offset.x;
		bottom += offset.y;
		return *this;
	}
	constexpr Rectangle& move_bottom_right(Vector2d<Value_> const offset) {
		right += offset.x;
		bottom += offset.y;
		return *this;
	}

	template<util::IsNumber T>
	[[nodiscard]]
	constexpr Rectangle<T> to() const {
		return Rectangle<T>{
			static_cast<T>(left),
			static_cast<T>(top),
			static_cast<T>(right),
			static_cast<T>(bottom)
		};
	}

	/*
		If the size in any dimension is negative, it is set to 
		zero by moving the most negative coordinate.
		For example, if right < left, then right = left.
	*/
	constexpr Rectangle& clip_negative_space() {
		right = max(left, right);
		bottom = max(top, bottom);
		return *this;
	}
	constexpr Rectangle& round_outwards() {
		left = floor<Value_>(left);
		top = floor<Value_>(top);
		right = ceil<Value_>(right);
		bottom = ceil<Value_>(bottom);
		return *this;
	}

	constexpr Rectangle& bound(Rectangle<Value_> const bounds) {
		left = std::clamp(left, bounds.left, bounds.right);
		top = std::clamp(top, bounds.top, bounds.bottom);
		right = std::clamp(right, bounds.left, bounds.right);
		bottom = std::clamp(bottom, bounds.top, bounds.bottom);
		return *this;
	}
	template<util::IsNumber T>
	constexpr Rectangle& contain(Rectangle<T> const rectangle) {
		/*
			If this is true then we need to round "outwards" so that this 
			rectangle also contains the other rectangle's fractional part.
		*/
		if constexpr (std::floating_point<T> && std::integral<Value_>) {
			left = min(left, floor<Value_>(rectangle.left));
			top = min(top, floor<Value_>(rectangle.top));
			right = max(right, ceil<Value_>(rectangle.right));
			bottom = max(bottom, ceil<Value_>(rectangle.bottom));
		}
		else {
			left = min(left, rectangle.left);
			top = min(top, rectangle.top);
			right = max(right, rectangle.right);
			bottom = max(bottom, rectangle.bottom);
		}
		return *this;
	}

	template<util::IsNumber T>
	[[nodiscard]]
	constexpr bool contains(Point<T> const point) const {
		using Common_ = std::common_type_t<Value_, T>;
		return static_cast<Common_>(point.x) >= static_cast<Common_>(left) && 
			static_cast<Common_>(point.x) < static_cast<Common_>(right) &&
			static_cast<Common_>(point.y) >= static_cast<Common_>(top) && 
			static_cast<Common_>(point.y) < static_cast<Common_>(bottom);
	}
	template<util::IsNumber T>
	[[nodiscard]]
	constexpr bool contains(Rectangle<T> const rectangle) const {
		using Common_ = std::common_type_t<Value_, T>;
		return static_cast<Common_>(rectangle.left) > static_cast<Common_>(left) && 
			static_cast<Common_>(rectangle.top) > static_cast<Common_>(top) && 
			static_cast<Common_>(rectangle.right) < static_cast<Common_>(right) && 
			static_cast<Common_>(rectangle.bottom) < static_cast<Common_>(bottom);
	}
	template<util::IsNumber T>
	[[nodiscard]]
	constexpr bool intersects(Rectangle<T> const rectangle) const {
		using Common_ = std::common_type_t<Value_, T>;
		return static_cast<Common_>(rectangle.right) > static_cast<Common_>(left) && 
			static_cast<Common_>(rectangle.left) < static_cast<Common_>(right) &&
			static_cast<Common_>(rectangle.bottom) > static_cast<Common_>(top) && 
			static_cast<Common_>(rectangle.top) < static_cast<Common_>(bottom);
	}
};

template<class T>
concept IsRectangle = util::IsInstantiationOf<T, Rectangle>;

template<util::IsNumber T>
[[nodiscard]]
constexpr Rectangle<T> with_negative_space_clipped(Rectangle<T> rectangle) {
	return rectangle.clip_negative_space();
}

template<util::IsNumber T, util::IsNumber U>
[[nodiscard]]
constexpr auto scaled(Rectangle<T> const rectangle, U const scale_factor) {
	return rectangle * scale_factor;
}

template<template<class> class Rectangle_, util::IsNumber T> 
	requires std::same_as<Rectangle_<T>, Rectangle<T>>
[[nodiscard]]
constexpr Rectangle<T> square(T const value) {
	return Rectangle{T{}, T{}, value, value};
}

} // namespace avo::math

template<class T>
struct fmt::formatter<T, std::enable_if_t<avo::math::IsRectangle<T>, char>> 
	: fmt::formatter<typename T::value_type> 
{
	using formatter<typename T::value_type>::format;
	
	auto format(std::same_as<T> auto const rectangle, auto& context) {
		fmt::format_to(context.out(), "(");
		format(rectangle.left, context);
		fmt::format_to(context.out(), ", ");
		format(rectangle.top, context);
		fmt::format_to(context.out(), ", ");
		format(rectangle.right, context);
		fmt::format_to(context.out(), ", ");
		format(rectangle.bottom, context);
		return fmt::format_to(context.out(), ")");
	}
};

#endif
