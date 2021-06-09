#ifndef AVO_WINDOW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_WINDOW_HPP_BJORN_SUNDIN_JUNE_2021

#include "graphics/miscellaneous.hpp"
#include "math/miscellaneous.hpp"
#include "math/vector2d.hpp"
#include "utils/miscellaneous.hpp"

#include <any>

namespace avo::window {

using graphics::Dip, graphics::Pixels;

/*
	Converts between pixel and DIP (density independent pixel) units.
*/
class ScreenUnitConverter {
public:
	[[nodiscard]]
	constexpr Pixels dip_to_pixels(Dip const dip) const noexcept {
		return static_cast<Pixels>(dip * dip_to_pixel_factor_);
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
		return static_cast<Dip>(pixels) / dip_to_pixel_factor_;
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
		dip_to_pixel_factor_{dpi/normal_dpi}
	{}

private:
	math::Factor dip_to_pixel_factor_{1};
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
bool get_is_key_down(KeyboardKey);
/*
	Returns whether a mouse button is currently pressed.
*/
[[nodiscard]]
bool get_is_mouse_button_down(MouseButton);

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
	math::Vector2d<math::Factor> position_factor{};
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
	std::unique_ptr<Implementation> implementation_;

	explicit Window(Parameters const& parameters);
};

//------------------------------

class Builder final {	
public:
	[[nodiscard]]
	Window open() &&;

	[[nodiscard]]
	Builder&& position(math::Vector2d<math::Factor> const pos) && noexcept 
	{
		parameters_.position_factor = pos;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& size(math::Size<Dip> const size) && noexcept 
	{
		parameters_.size = size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& min_size(math::Size<Dip> const min_size) && noexcept 
	{
		parameters_.size_bounds.min = min_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& max_size(math::Size<Dip> const max_size) && noexcept 
	{
		parameters_.size_bounds.max = max_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& min_max_size(MinMaxSizes<Dip> const min_max) && noexcept 
	{
		parameters_.size_bounds = min_max;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& style(StyleFlags const style) && noexcept 
	{
		parameters_.style = style;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& state(State const state) && noexcept 
	{
		parameters_.state = state;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& with_parent(Window& parent) && noexcept 
	{
		parameters_.parent = &parent;
		return std::move(*this);
	}

	Builder() = delete;
	~Builder() = default;

	Builder(Builder&&) = delete;
	Builder& operator=(Builder&&) = delete;
	Builder(Builder const&) = delete;
	Builder& operator=(Builder const&) = delete;

private:
	Parameters parameters_;

	Builder(std::string_view const title) :
		parameters_{.title = title}
	{}

	friend Builder window(std::string_view);
};

[[nodiscard]]
inline Builder window(std::string_view const title) {
	return {title};
}

} // namespace avo::window

#endif