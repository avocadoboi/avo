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

#ifndef AVO_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_HPP_BJORN_SUNDIN_JUNE_2021

#if __has_include(<source_location>)
#	include <source_location>
#elif __has_include(<experimental/source_location>)
#	include <experimental/source_location>
	namespace std {
		using source_location = std::experimental::source_location;
	}
#endif

//------------------------------
// This is recommended. 
// Remove it if it causes you any problems.

using namespace std::chrono_literals;
using namespace std::string_view_literals;

//------------------------------

namespace avo {

// Physical screen pixels
using Pixels = std::int32_t;

// Density independent pixels
using Dip = float;

// Represents a proportion of something. Something that will be multiplied with a value.
using Factor = float;

} // namespace avo

namespace avo {

namespace window {

/*
	Converts between pixel and DIP (density independent pixel) units.
*/
class ScreenUnitConverter {
public:
	[[nodiscard]]
	constexpr Pixels dip_to_pixels(Dip const dip) const noexcept {
		return static_cast<Pixels>(dip * _dip_to_pixel_factor);
	}
	template<template<class> class Vector_> requires math::Is2dVectorTemplate<Vector_>
	[[nodiscard]]
	constexpr Vector_<Pixels> dip_to_pixels(Vector_<Dip> const dip) const noexcept 
	{
		return Vector_{
			dip_to_pixels(dip.x),
			dip_to_pixels(dip.y)
		};
	}
	[[nodiscard]]
	constexpr utils::IsMinMax auto dip_to_pixels(utils::IsMinMax auto const dip) const noexcept 
	{
		return utils::MinMax{
			dip_to_pixels(dip.min),
			dip_to_pixels(dip.max)
		};
	}
	
	[[nodiscard]]
	constexpr Dip pixels_to_dip(Pixels const pixels) const noexcept {
		return static_cast<Dip>(pixels) / _dip_to_pixel_factor;
	}
	template<template<class> class Vector_> requires math::Is2dVectorTemplate<Vector_>
	[[nodiscard]]
	constexpr Vector_<Dip> pixels_to_dip(Vector_<Pixels> const pixels) const noexcept 
	{
		return Vector_{
			pixels_to_dip(pixels.x),
			pixels_to_dip(pixels.y)
		};
	}
	[[nodiscard]]
	constexpr utils::IsMinMax auto pixels_to_dip(utils::IsMinMax auto const pixels) const noexcept 
	{
		return utils::MinMax{
			pixels_to_dip(pixels.min),
			pixels_to_dip(pixels.max)
		};
	}

	//------------------------------

	static constexpr float normal_dpi = 96.f;

	constexpr ScreenUnitConverter() = default;
	constexpr explicit ScreenUnitConverter(float const dpi) noexcept :
		_dip_to_pixel_factor{dpi/normal_dpi}
	{}

private:
	Factor _dip_to_pixel_factor{1};
};

//------------------------------

enum class KeyboardKey {
	None = 0,
	Backspace,
	Clear,
	Tab,
	Return, // Enter and return have the same value.
	Enter = Return, // Enter and return have the same value.
	Shift,
	Control,
	Menu,
	Alt,
	CapsLock,
	Escape,
	Spacebar,
	PageUp, PageDown, Home, End,
	PrintScreen,
	Insert,
	Delete,
	Pause,
	Help,
	Separator,
	Left, Right, Up, Down,
	NumLock,
	Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
	Add, Subtract, Multiply, Divide, Decimal,
	Number0, Number1, Number2, Number3, Number4, Number5, Number6, Number7, Number8, Number9,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
	Comma, Period, Plus, Minus,
	// These keys vary by country/region.
	Regional1, Regional2, Regional3, Regional4, Regional5, Regional6, Regional7, Regional8
};

enum class MouseButton {
	None = 0,
	Left,
	Middle,
	Right,
	X0,
	X1
};

/*
	Returns whether a keyboard key is currently pressed.
*/
[[nodiscard]]
bool is_key_down(KeyboardKey);
/*
	Returns whether a mouse button is currently pressed.
*/
[[nodiscard]]
bool is_mouse_button_down(MouseButton);

//------------------------------

enum class BorderArea {
	None = 0, // The area of the window is not part of the window border, meaning any mouse events are handled only by the GUI.
	TopLeftResize,
	TopResize,
	TopRightResize,
	LeftResize,
	RightResize,
	BottomLeftResize,
	BottomResize,
	BottomRightResize,
	Dragging // The area of the window is used for dragging the window, normally the title bar.
};

enum class State {
	Minimized,
	Maximized,
	Restored
};

//------------------------------

enum class StyleFlags : std::uint32_t {
	None =            0,
	CloseButton =     1,
	Invisible =       1 << 1, // Makes the window invisible at first. You can make it visible afterwards.
	MinimizeButton =  1 << 2,
	MaximizeButton =  1 << 3,
	Resizable =       1 << 4,
	CustomBorder =    1 << 5, // This makes the client area take up the full window, and the GUI determines which areas are for resizing and moving the window.
	DefaultCustom =   CustomBorder | MaximizeButton | MinimizeButton | Resizable,
	Default =         CloseButton | MinimizeButton | MaximizeButton | Resizable,
	DefaultNoResize = CloseButton | MinimizeButton
};

[[nodiscard]]
constexpr StyleFlags operator|(StyleFlags const left, StyleFlags const right) noexcept 
{
	return static_cast<StyleFlags>(static_cast<std::uint32_t>(left) | static_cast<std::uint32_t>(right));
}
constexpr StyleFlags& operator|=(StyleFlags& left, StyleFlags const right) noexcept 
{
	return left = left | right;
}
[[nodiscard]]
constexpr StyleFlags operator&(StyleFlags const left, StyleFlags const right) noexcept 
{
	return static_cast<StyleFlags>(static_cast<std::uint32_t>(left) & static_cast<std::uint32_t>(right));
}
constexpr StyleFlags& operator&=(StyleFlags& left, StyleFlags const right) noexcept 
{
	return left = left & right;
}

//------------------------------

class Window;

template<utils::IsNumber T>
using MinMaxSizes = utils::MinMax<math::Size<T>>;

struct Parameters {
	std::string_view title;
	math::Vector2d<Factor> position_factor{};
	math::Size<Dip> size{};
	MinMaxSizes<Dip> size_bounds{};
	StyleFlags style{StyleFlags::Default};
	State state{State::Restored};
	Window* parent{};
};

//------------------------------

class Window final {
	friend class Builder;
	
public:
	void title(std::string_view);

	[[nodiscard]]
	std::string title() const;

	bool toggle_fullscreen();

	void position(math::Point<Pixels>);

	void min_max_size(MinMaxSizes<Dip>);
	
	[[nodiscard]]
	MinMaxSizes<Dip> min_max_size() const;

	void max_size(math::Size<Dip>);
	
	[[nodiscard]]
	math::Size<Dip> max_size() const;
	
	void min_size(math::Size<Dip>);
	
	[[nodiscard]]
	math::Size<Dip> min_size() const;

	void size(math::Size<Dip>);
	
	[[nodiscard]]
	math::Size<Dip> size() const;

	[[nodiscard]]
	bool is_open() const;

	[[nodiscard]]
	std::any native_handle() const;
	
	Window() = delete;
	~Window(); // = default in .cpp

	Window(Window&&) noexcept; // = default in .cpp
	Window& operator=(Window&&) noexcept; // = default in .cpp

	Window(Window const&) = delete;
	Window& operator=(Window const&) = delete;
	
private:
	class Implementation;
	std::unique_ptr<Implementation> _implementation;

	explicit Window(Parameters const& parameters);
};

//------------------------------

class Builder final {	
public:
	[[nodiscard]]
	Window open() &&;

	[[nodiscard]]
	Builder&& position(math::Vector2d<Factor> const pos) && noexcept 
	{
		_parameters.position_factor = pos;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& size(math::Size<Dip> const size) && noexcept 
	{
		_parameters.size = size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& min_size(math::Size<Dip> const min_size) && noexcept 
	{
		_parameters.size_bounds.min = min_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& max_size(math::Size<Dip> const max_size) && noexcept 
	{
		_parameters.size_bounds.max = max_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& min_max_size(MinMaxSizes<Dip> const min_max) && noexcept 
	{
		_parameters.size_bounds = min_max;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& style(StyleFlags const style) && noexcept 
	{
		_parameters.style = style;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& state(State const state) && noexcept 
	{
		_parameters.state = state;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& with_parent(Window& parent) && noexcept 
	{
		_parameters.parent = &parent;
		return std::move(*this);
	}

	Builder() = delete;
	~Builder() = default;

	Builder(Builder&&) = delete;
	Builder& operator=(Builder&&) = delete;
	Builder(Builder const&) = delete;
	Builder& operator=(Builder const&) = delete;

private:
	Parameters _parameters;

	Builder(std::string_view const title) :
		_parameters{.title = title}
	{}

	friend Builder window(std::string_view);
};

[[nodiscard]]
inline Builder window(std::string_view const title) 
{
	return {title};
}

} // namespace window

//------------------------------

namespace graphics {

enum class CornerType {
	Round,
	Cut
};

struct Corner {
	math::Size<Dip> size;
	CornerType type;

	constexpr bool operator==(Corner const&) const = default;
};

template<class Corner_ = Corner>
struct RectangleCorners {
	using value_type = Corner_;
	
	Corner_ top_left;
	Corner_ top_right;
	Corner_ bottom_left;
	Corner_ bottom_right;

	constexpr bool operator==(RectangleCorners const&) const 
		requires std::equality_comparable<Corner_> 
		= default;

	static constexpr RectangleCorners uniform(Corner_ const corner) {
		return {corner, corner, corner, corner};
	}
};

#ifdef BUILD_TESTING
static_assert([]{
	auto corners = RectangleCorners{
		.top_left{{5.f, 5.f}}, .top_right{{5.f, 5.f}}, 
		.bottom_left{{5.f, 5.f}}, .bottom_right{{5.f, 5.f}}
	};
	return RectangleCorners<>::uniform({math::Size{5.f, 5.f}, CornerType::Round}) == corners;
}());
#endif

} // namespace graphics

//------------------------------

namespace gui {

namespace font_families {

constexpr auto 
	roboto = std::string_view{"Roboto"},
	material_icons = std::string_view{"Material Icons"};

} // namespace font_families

/*
	Default theme color IDs.
*/
namespace theme_colors {

inline auto const 
	background = Id::next(),
	on_background = Id::next(),

	primary = Id::next(),
	primary_on_background = Id::next(),
	on_primary = Id::next(),

	secondary = Id::next(),
	secondary_on_background = Id::next(),
	on_secondary = Id::next(),

	selection = Id::next(),
	shadow = Id::next();

} // namespace theme_colors

/*
	Default theme easing IDs.
*/
namespace theme_easings {

inline auto const 
	in = Id::next(),
	out = Id::next(),
	in_out = Id::next(),
	symmetrical_in_out = Id::next();

} // namespace theme_easings

/*
	Default theme value names.
*/
namespace theme_values {

inline auto const 
	hover_animation_speed = Id::next(),
	hover_animation_duration = Id::next();

} // namespace theme_values

/*
	A theme consists of different variables that change the look and feel of the parts of the GUI that are using the theme.
	Can be used for changing and accessing any values, colors and easings.
	All the default IDs are in avo::theme_colors, avo::theme_easings and avo::theme_values.
*/
struct Theme final {
	std::unordered_map<Id, Color> colors{
		{theme_colors::background, Color{0xfffefefe}},
		{theme_colors::on_background, Color{0xff070707}},
		{theme_colors::primary, Color{0xff6200ea}},
		{theme_colors::primary_on_background, Color{0xff512da8}},
		{theme_colors::on_primary, Color{1.f}},
		{theme_colors::secondary, Color{0xff1de9b6}},
		{theme_colors::secondary_on_background, Color{0xff00bfa5}},
		{theme_colors::on_secondary, Color{0xff070707}},
		{theme_colors::selection, Color{0x90488db5}},
		{theme_colors::shadow, Color{0x68000000}},
	};
	std::unordered_map<Id, Easing> easings{
		{theme_easings::in, Easing{{0.6f, 0.f}, {0.8f, 0.2f}}},
		{theme_easings::out, Easing{{0.1f, 0.9f}, {0.2f, 1.f}}},
		{theme_easings::in_out, Easing{{0.4f, 0.f}, {0.f, 1.f}}},
		{theme_easings::symmetrical_in_out, Easing{{0.6f, 0.f}, {0.4f, 1.f}}},
	};
	std::unordered_map<Id, float> values{
		// 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.
		{theme_values::hover_animation_speed, 1.f/6.f},
		{theme_values::hover_animation_duration, 60.f},
	};
};

} // namespace gui

} // namespace avo

#endif