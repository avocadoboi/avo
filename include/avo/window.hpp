#ifndef AVO_WINDOW_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_WINDOW_HPP_BJORN_SUNDIN_JUNE_2021

#include "event_listeners.hpp"
#include "graphics/miscellaneous.hpp"
#include "math/miscellaneous.hpp"
#include "math/vector2d.hpp"
#include "util/miscellaneous.hpp"

#include <any>
#include <variant>

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
	template<template<class> class Vector_>
		requires math::Is2dVector<Vector_<Pixels>> && math::Is2dVector<Vector_<Dip>>
	[[nodiscard]]
	constexpr Vector_<Pixels> dip_to_pixels(Vector_<Dip> const dip) const noexcept 
	{
		return Vector_{
			dip_to_pixels(dip.x),
			dip_to_pixels(dip.y)
		};
	}
	[[nodiscard]]
	constexpr util::IsMinMax auto dip_to_pixels(util::IsMinMax auto const dip) const noexcept 
	{
		return util::MinMax{
			dip_to_pixels(dip.min),
			dip_to_pixels(dip.max)
		};
	}
	
	[[nodiscard]]
	constexpr Dip pixels_to_dip(Pixels const pixels) const noexcept {
		return static_cast<Dip>(pixels) / dip_to_pixel_factor_;
	}
	template<template<class> class Vector_> 
		requires math::Is2dVector<Vector_<Pixels>> && math::Is2dVector<Vector_<Dip>>
	[[nodiscard]]
	constexpr Vector_<Dip> pixels_to_dip(Vector_<Pixels> const pixels) const noexcept 
	{
		return Vector_{
			pixels_to_dip(pixels.x),
			pixels_to_dip(pixels.y)
		};
	}
	[[nodiscard]]
	constexpr util::IsMinMax auto pixels_to_dip(util::IsMinMax auto const pixels) const noexcept 
	{
		return util::MinMax{
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
	Enter,
	Shift,
	Control,
	Super,
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
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
	F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
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
	Returns the current mouse position in pixel screen coordinates.
*/
[[nodiscard]]
math::Point<Pixels> get_mouse_position();

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

} // namespace avo::window

namespace avo::util {

template<>
inline constexpr bool is_bit_flag<avo::window::StyleFlags> = true;

} // namespace avo::util

//------------------------------

namespace avo::window {

namespace event {

struct MouseMove {
	math::Point<Dip> position;
	math::Vector2d<Dip> movement;
};
struct MouseLeave {
	math::Point<Dip> position;
	math::Vector2d<Dip> movement;
};
struct MouseScroll {
	math::Point<Dip> position;
	float scroll_delta;
};
struct MouseDown {
	math::Point<Dip> position;
	MouseButton button;
	bool is_double_click;
};
struct MouseUp {
	math::Point<Dip> position;
	MouseButton button;
};
struct KeyDown {
	KeyboardKey key;
	bool is_repeated;
};
struct KeyUp {
	KeyboardKey key;
};
struct CharacterInput {
	std::string character;
	bool is_repeated;
};
struct FocusGain {};
struct FocusLose {};
struct SizeChange {
	math::Size<Dip> size;
};
struct StateChange {
	State state;
};
struct Closed {};
struct DpiChange {
	float dpi;
};

} // namespace event

using Event = std::variant<
	event::MouseMove, 
	event::MouseLeave,
	event::MouseScroll, 
	event::MouseDown, 
	event::MouseUp,
	event::KeyDown,
	event::KeyUp,
	event::CharacterInput,
	event::FocusGain,
	event::FocusLose,
	event::SizeChange,
	event::StateChange,
	event::Closed,
	event::DpiChange
>;

//------------------------------

class Window;

template<util::IsNumber T>
using MinMaxSize = util::MinMax<math::Size<T>>;

struct Parameters {
	std::string_view title;
	math::Vector2d<math::Factor> position_factor{};
	math::Size<Dip> size{};
	MinMaxSize<Dip> min_max_size{};
	StyleFlags style{StyleFlags::Default};
	State state{State::Restored};
	Window* parent{};
};

//------------------------------

/*
	A Window creates a new window and automatically runs an event loop in a separate thread to keep it responsive and prevent blocking behavior.
	Events are safely sent to the thread of the Window class and can be retrieved using await_event and take_event.
	If you don't call await_event or take_event regularly, the properties that can be retrieved using the accessor methods of this class will not be 
	updated. The window would still be responsive however.

	A window can be safely created with a parent window created and/or mutated on another thread.
*/
class Window final {
public:
	void title(std::string_view);

	[[nodiscard]]
	std::string title() const;

	bool toggle_fullscreen();

	[[nodiscard]]
	bool is_fullscreen() const;

	void position(math::Point<Pixels>);

	void min_max_size(MinMaxSize<Dip>);
	
	[[nodiscard]]
	MinMaxSize<Dip> min_max_size() const;

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
	float dpi() const;

	[[nodiscard]]
	bool is_open() const;

	[[nodiscard]]
	std::any native_handle() const;

	[[nodiscard]]
	Event await_event();

	[[nodiscard]]
	std::optional<Event> take_event();
	
	explicit Window(Parameters const& parameters);

	~Window(); // = default in .cpp

	Window(Window&&) noexcept; // = default in .cpp
	Window& operator=(Window&&) noexcept; // = default in .cpp

	Window(Window const&) = delete;
	Window& operator=(Window const&) = delete;
	
private:
	class Implementation;
	std::unique_ptr<Implementation> implementation_;
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
		parameters_.min_max_size.min = min_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& max_size(math::Size<Dip> const max_size) && noexcept 
	{
		parameters_.min_max_size.max = max_size;
		return std::move(*this);
	}
	[[nodiscard]]
	Builder&& min_max_size(MinMaxSize<Dip> const min_max) && noexcept 
	{
		parameters_.min_max_size = min_max;
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

	explicit Builder(std::string_view const title) :
		parameters_{.title{title}, .position_factor{0.5f, 0.5f}}
	{}

	friend inline Builder create(std::string_view);
};

[[nodiscard]]
inline Builder create(std::string_view const title) {
	return Builder{title};
}

//------------------------------

namespace detail {

template<class T>
struct ListenerVariant;

template<class ... Event_>
struct ListenerVariant<std::variant<Event_...>> {
	using type = std::variant<std::function<void(Event_ const&)>...>;
};

} // namespace detail

using EventListener = detail::ListenerVariant<Event>::type;

//------------------------------

namespace detail {

template<class T, class EventVariant_>
struct IsEventListener;

template<class T, class ... Event_>
struct IsEventListener<T, std::variant<Event_...>> {
	static constexpr bool value = (std::invocable<T, Event_ const&> || ...);
};

} // namespace detail

/*
	True for types which are invocable with a const reference to any of the Event variants in the avo::window::event namespace.
*/
template<typename T>
concept IsEventListener = detail::IsEventListener<T, Event>::value;

//------------------------------

class EventManager {
public:
	/*
		Notifies listeners of any events currently available from the window.
	*/
	void update(Window& window) {
		while (auto const event = window.take_event()) {
			send_event_(*event);
		}
	}
	/*
		Waits for one event from the window and notifies any listeners.
	*/
	void update_wait(Window& window) {
		send_event_(window.await_event());
	}
	/*
		Blocks until the window has been closed, automatically notifying event listeners of new events from the window.
	*/
	void run(Window& window) {
		while (window.is_open()) {
			update_wait(window);
		}
	}

	/*
		Adds an event listener invocable to be notified when an event of the type of its parameter is available.
		This overload takes any invocable that isn't a std::function instantiation.
	*/
	template<IsEventListener Listener_>
	EventManager& add_listener(Listener_&& listener) {
		listeners_.emplace_back(std::function{std::forward<Listener_>(listener)});
		return *this;
	}

	/*
		Adds an event listener invocable to be notified when an event of the type of its parameter is available.
		This overload only takes std::function instantiations.
	*/
	template<IsEventListener Listener_> 
		requires std::constructible_from<EventListener, Listener_>
	EventManager& add_listener(Listener_&& listener) {
		listeners_.emplace_back(std::forward<Listener_>(listener));
		return *this;
	}

private:
	void send_event_(Event const& event_variant)
	{
		auto listener = std::ranges::find_if(listeners_, [&](auto const& listener) {
			return listener.index() == event_variant.index();
		});
		if (listener != listeners_.end()) 
		{
			std::visit([]<class Listener_, class Event_>
				(Listener_& listener, Event_ const& event) {
					if constexpr (std::same_as<Listener_, std::function<void(Event_ const&)>>) {
						listener(event);
					}
				}, *listener, event_variant);
		}
	}

	std::vector<EventListener> listeners_;
};

} // namespace avo::window

#endif
