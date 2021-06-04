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

#include "avo.hpp"

#ifdef __CYGWIN__
#	define _WIN32
#endif

#ifdef _WIN32
#	include <windows.h>
#else
#	include <iconv.h>
#endif

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#ifdef None
#	undef None
#endif
#endif

namespace avo {

namespace unicode {

void enable_utf8_console() {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	// Pretty much everyone else uses UTF-8 by default.
}

#ifndef _WIN32
using IconvHandle = utils::UniqueHandle<iconv_t, decltype([](iconv_t const handle){ iconv_close(handle); })>;
#endif

std::optional<std::size_t> utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) 
{
#ifdef _WIN32
	auto const length = MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(output.data()), static_cast<int>(output.size())
	);

	if (length == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return {};
	}
	return length;
#else
	// I have no idea why the input string data parameter isn't char const**.
	// It shouldn't be modified, so a const_cast is made here.

	auto in_pointer = const_cast<char*>(input.data());
	auto in_bytes_left = input.size();
	auto out_pointer = reinterpret_cast<char*>(output.data());
	auto out_bytes_left = output.size()*sizeof(char16_t);

	if (iconv(
			IconvHandle{iconv_open("UTF-16LE", "UTF-8")}.get(), 
			&in_pointer, 
			&in_bytes_left, 
			&out_pointer, 
			&out_bytes_left
		) == static_cast<std::size_t>(-1)) 
	{
		return {};
	}
	
	return output.size() - out_bytes_left/sizeof(char16_t);
#endif
}

std::u16string utf8_to_utf16(std::string_view const input) {
#ifdef _WIN32
	auto result = std::u16string(MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		0, 0
	), '\0');

	MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(result.data()), static_cast<int>(result.size())
	);

	return result;
#else
	auto output = std::u16string(input.size(), u'\0');
	if (auto const length = utf8_to_utf16(input, std::span{output})) {
		// length includes null terminator because of std::span constructor.
		output.resize(*length);
		return output;
	}
	return {};
#endif
}

std::optional<std::size_t> utf16_to_utf8(std::u16string_view const input, std::span<char> const output) 
{
#ifdef _WIN32
	auto const length = WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		output.data(), static_cast<int>(output.size()),
		nullptr, nullptr
	);
	if (length == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return {};
	}
	return length;
#else
	auto in_pointer = const_cast<char*>(reinterpret_cast<char const*>(input.data()));
	auto in_bytes_left = input.size()*sizeof(char16_t);
	auto out_pointer = output.data();
	auto out_bytes_left = output.size();

	if (iconv(
			IconvHandle{iconv_open("UTF-8", "UTF-16")}.get(), 
			&in_pointer, 
			&in_bytes_left, 
			&out_pointer, 
			&out_bytes_left
		) == static_cast<std::size_t>(-1)) 
	{
		return {};
	}
	return output.size() - out_bytes_left;
#endif
}

std::string utf16_to_utf8(std::u16string_view const input) {
#ifdef _WIN32
	auto result = std::string(WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		0, 0, nullptr, nullptr
	), '\0');

	WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		result.data(), static_cast<int>(result.size()),
		nullptr, nullptr
	);

	return result;
#else
	auto output = std::string(input.size()*3, '\0');
	if (auto const length = utf16_to_utf8(input, std::span{output})) 
	{
		output.resize(*length);
		return output;
	}
	return {};
#endif
}

} // namespace unicode

namespace window {

#ifdef _WIN32
namespace win {

[[nodiscard]]
constexpr DWORD style_flags_to_native(StyleFlags const flags, bool const has_parent) noexcept {
	auto native_flags = DWORD{};

	using enum StyleFlags;

	if (has_parent || utils::has_flag(flags, CustomBorder)) {
		native_flags |= WS_POPUP;
	}
	if (!utils::has_flag(flags, Invisible)) {
		native_flags |= WS_VISIBLE;
	}
	if (utils::has_flag(flags, CloseButton | CustomBorder)) 
	{ // CloseButton or CustomBorder is true; not necessarily both
		native_flags |= WS_CAPTION | WS_SYSMENU;
	}
	if (utils::has_flag(flags, MinimizeButton)) {
		native_flags |= WS_MINIMIZEBOX;
	}
	if (utils::has_flag(flags, MaximizeButton)) {
		native_flags |= WS_MAXIMIZEBOX;
	}
	if (utils::has_flag(flags, Resizable)) {
		native_flags |= WS_THICKFRAME;
	}

	return native_flags;
}

constexpr auto native_mouse_button_map = [] {
	using enum MouseButton;
	return utils::StaticMap<int, MouseButton, 5>{
		{VK_LBUTTON, Left},
		{VK_MBUTTON, Middle},
		{VK_RBUTTON, Right},
		{VK_XBUTTON1, X0},
		{VK_XBUTTON2, X1},
	};
}();

constexpr auto native_key_map = [] {
	using enum KeyboardKey;
	return utils::StaticMap<int, KeyboardKey, 110>{
		{VK_APPS, Menu},
		{VK_BACK, Backspace},
		{VK_CLEAR, Clear},
		{VK_TAB, Tab},
		{VK_RETURN, Return},
		{VK_SHIFT, Shift},
		{VK_CONTROL, Control},
		{VK_MENU, Alt},
		{VK_PAUSE, Pause},
		{VK_CAPITAL, CapsLock},
		{VK_ESCAPE, Escape},
		{VK_SPACE, Spacebar},
		{VK_PRIOR, PageUp},
		{VK_NEXT, PageDown},
		{VK_END, End},
		{VK_HOME, Home},
		{VK_LEFT, Left},
		{VK_RIGHT, Right},
		{VK_UP, Up},
		{VK_DOWN, Down},
		{VK_SNAPSHOT, PrintScreen},
		{VK_INSERT, Insert},
		{VK_DELETE, Delete},
		{VK_HELP, Help},
		{VK_NUMPAD0, Numpad0},
		{VK_NUMPAD1, Numpad1},
		{VK_NUMPAD2, Numpad2},
		{VK_NUMPAD3, Numpad3},
		{VK_NUMPAD4, Numpad4},
		{VK_NUMPAD5, Numpad5},
		{VK_NUMPAD6, Numpad6},
		{VK_NUMPAD7, Numpad7},
		{VK_NUMPAD8, Numpad8},
		{VK_NUMPAD9, Numpad9},
		{VK_ADD, Add},
		{VK_SUBTRACT, Subtract},
		{VK_MULTIPLY, Multiply},
		{VK_DIVIDE, Divide},
		{VK_F1, F1},
		{VK_F2, F2},
		{VK_F3, F3},
		{VK_F4, F4},
		{VK_F5, F5},
		{VK_F6, F6},
		{VK_F7, F7},
		{VK_F8, F8},
		{VK_F9, F9},
		{VK_F10, F10},
		{VK_F11, F11},
		{VK_F12, F12},
		{VK_F13, F13},
		{VK_F14, F14},
		{VK_F15, F15},
		{VK_F16, F16},
		{VK_F17, F17},
		{VK_F18, F18},
		{VK_F19, F19},
		{VK_F20, F20},
		{VK_F21, F21},
		{VK_F22, F22},
		{VK_F23, F23},
		{VK_F24, F24},
		{VK_NUMLOCK, NumLock},
		{0x30, Number0},
		{0x31, Number1},
		{0x32, Number2},
		{0x33, Number3},
		{0x34, Number4},
		{0x35, Number5},
		{0x36, Number6},
		{0x37, Number7},
		{0x38, Number8},
		{0x39, Number9},
		{0x41, A},
		{0x42, B},
		{0x43, C},
		{0x44, D},
		{0x45, E},
		{0x46, F},
		{0x47, G},
		{0x48, H},
		{0x49, I},
		{0x4A, J},
		{0x4B, K},
		{0x4C, L},
		{0x4D, M},
		{0x4E, N},
		{0x4F, O},
		{0x50, P},
		{0x51, Q},
		{0x52, R},
		{0x53, S},
		{0x54, T},
		{0x55, U},
		{0x56, V},
		{0x57, W},
		{0x58, X},
		{0x59, Y},
		{0x5A, Z},
		{VK_OEM_COMMA, Comma},
		{VK_OEM_PERIOD, Period},
		{VK_OEM_PLUS, Plus},
		{VK_OEM_MINUS, Minus},
		{VK_OEM_1, Regional1},
		{VK_OEM_2, Regional2},
		{VK_OEM_3, Regional3},
		{VK_OEM_4, Regional4},
		{VK_OEM_5, Regional5},
		{VK_OEM_6, Regional6},
		{VK_OEM_7, Regional7},
	};
}();

//------------------------------

[[nodiscard]]
bool get_is_vkey_down(int const vkey) {
	return GetAsyncKeyState(*native_key) & (1 << 16);
}

//------------------------------

class WindowThread {
public:
	
	WindowThread(Parameters const&) {

	}

private:


	std::jthread _thread;
};

} // namespace win

//------------------------------

bool get_is_key_down(KeyboardKey const key) 
{
	if (auto const native_key = win::native_key_map.find(key)) 
	{
		return win::get_is_vkey_down(*native_key);
	}
	return false;
}
bool get_is_mouse_button_down(MouseButton const button)
{
	if (auto const native_key = win::native_mouse_button_map.find(button))
	{
		return win::get_is_vkey_down(*native_key);
	}
	return false;
}

//------------------------------

class Window::Implementation {
public:
	void title(std::string_view const) {
	}
	std::string title() const {
		return {};
	}

	bool toggle_fullscreen() {
		return {};
	}

	void position(math::Point<Pixels> const) {
	}
	
	void min_max_size(MinMaxSizes<Dip> const) {
	}
	MinMaxSizes<Dip> min_max_size() const {
		return {};
	}

	void min_size(math::Size<Dip> const) {
	}
	math::Size<Dip> min_size() const {
		return {};
	}

	void max_size(math::Size<Dip> const) {
	}
	math::Size<Dip> max_size() const {
		return {};
	}

	void size(math::Size<Dip> const) {
	}
	math::Size<Dip> size() const {
		return {};
	}

	bool is_open() const {
		return {};
	}

	std::any native_handle() const {
		return 0;
	}

	Implementation(Parameters const& parameters) :
		_window_thread{parameters}
	{

	}

private:
	win::WindowThread _window_thread;
};
#endif

#ifdef __linux__

namespace x11 {

template<utils::IsTrivial T, std::invocable<::Display*, T> _Deleter>
class DisplayResourceHandle {
public:
	[[nodiscard]]
	T get() const noexcept {
		return _value;
	}

	DisplayResourceHandle(::Display* const server, T const value) :
		_server{server},
		_value{value}
	{}

	DisplayResourceHandle() = default;
	~DisplayResourceHandle() {
		if (_server) {
			_Deleter{}(_server, _value);
		}
	}

	DisplayResourceHandle(DisplayResourceHandle const&) = delete;
	DisplayResourceHandle& operator=(DisplayResourceHandle const&) = delete;
	
	DisplayResourceHandle(DisplayResourceHandle&& other) noexcept :
		_server{other._server},
		_value{other._value}
	{
		other._server = nullptr;
	}
	DisplayResourceHandle& operator=(DisplayResourceHandle&& other) noexcept {
		_server = other._server;
		_value = other._value;
		other._server = nullptr;
		return *this;
	}

private:
	::Display* _server{};
	T _value{};
};

using DisplayHandle = std::unique_ptr<::Display, decltype([](auto x){ ::XCloseDisplay(x); })>;

using ColormapHandle = DisplayResourceHandle<::Colormap, decltype([](auto a, auto b){ ::XFreeColormap(a, b); })>;

using WindowHandle = DisplayResourceHandle<::Window, decltype([](auto a, auto b){ ::XDestroyWindow(a, b); })>;

using InputMethodHandle = utils::UniqueHandle<XIM, decltype([](auto x){ ::XCloseIM(x); })>;
using InputContextHandle = utils::UniqueHandle<XIC, decltype([](auto x){ ::XDestroyIC(x); })>;

template<typename T>
using XFreeHandle = std::unique_ptr<T, decltype([](T* info){ ::XFree(info); })>;

//------------------------------

[[nodiscard]]
float get_dpi(::Display* const display) noexcept 
{
	return static_cast<float>(::XDisplayWidth(display, 0))/static_cast<float>(::XDisplayWidthMM(display, 0))*25.4f;
}

//------------------------------

[[nodiscard]]
XFreeHandle<::XVisualInfo> select_opengl_visual(::Display* const server) noexcept 
{
	constexpr auto framebuffer_attributes = std::array{
		GLX_X_RENDERABLE, 1,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_DEPTH_SIZE, 0, // 2D graphics, no z-buffering.
		GLX_STENCIL_SIZE, 0,
		GLX_DOUBLEBUFFER, 1,
		0 // Null terminator
	};

	auto number_of_matching_configurations = 0;
	auto const framebuffer_configurations = XFreeHandle<::GLXFBConfig>{::glXChooseFBConfig(
		server, 
		DefaultScreen(server), 
		framebuffer_attributes.data(), 
		&number_of_matching_configurations
	)};
	return XFreeHandle<::XVisualInfo>{::glXGetVisualFromFBConfig(server, *framebuffer_configurations.get())};
}

[[nodiscard]]
ColormapHandle create_colormap(::Display* const server, ::XVisualInfo const* const visual_info) noexcept 
{
	return {
		server, 
		::XCreateColormap(server, RootWindow(server, visual_info->screen), visual_info->visual, 0)
	};
}

[[nodiscard]]
x11::WindowHandle create_window_handle(::Display* const server, ::XVisualInfo const* const visual_info, 
	XSetWindowAttributes& attributes, Window* const parent, math::Size<Pixels> const size)
{
	return x11::WindowHandle{
		server,
		::XCreateWindow(
			server,
			parent 
				? std::any_cast<::Window>(parent->native_handle()) 
				: RootWindow(server, visual_info->screen),
			0, 0, // Initial x and y are ignored by the window manager
			size.x, size.y,
			0,
			visual_info->depth,
			InputOutput,
			visual_info->visual,
			CWEventMask | CWBorderPixel | CWColormap,
			&attributes
		)
	};
}

//------------------------------

[[nodiscard]]
math::Size<Pixels> get_screen_size(::Display* const display) noexcept 
{
	return math::Size{
		static_cast<Pixels>(::XDisplayWidth(display, 0)),
		static_cast<Pixels>(::XDisplayHeight(display, 0)),
	};
}

void set_factor_position(::Display* const server, ::Window const window, 
	math::Size<Pixels> const window_size, math::Vector2d<Factor> const position_factor) noexcept
{
	auto const screen_size = get_screen_size(server);
	auto const position = math::Point{
		static_cast<Pixels>(std::lerp(0.f, static_cast<float>(screen_size.x - window_size.x), position_factor.x)),
		static_cast<Pixels>(std::lerp(0.f, static_cast<float>(screen_size.y - window_size.y), position_factor.y))
	};
	::XMoveWindow(server, window, position.x, position.y);
}

//------------------------------

void set_title(::Display* const server, ::Window const window, std::string_view const title) noexcept 
{
	auto text_property = ::XTextProperty{
		// It's not going to modify the value.
		.value = const_cast<unsigned char*>(reinterpret_cast<unsigned char const*>(title.data())),
		#ifdef X_HAVE_UTF8_STRING
		.encoding = ::XInternAtom(server, "UTF8_STRING", 0),
		#else
		.encoding = XA_STRING,
		#endif
		.format = 8,
		.nitems = static_cast<unsigned long>(title.size()),
	};
	
	::XSetWMName(server, window, &text_property);
	::XSetWMIconName(server, window, &text_property);
}

[[nodiscard]]
std::string get_title(::Display* const server, ::Window const window) 
{
	::XTextProperty text_property;
	auto const cleanup = utils::Cleanup{[&]{ ::XFree(text_property.value); }};
	
	::XGetWMName(server, window, &text_property);

	return std::string(reinterpret_cast<char*>(text_property.value), text_property.nitems);
}

//------------------------------

void set_min_size(::Display* const server, ::Window const window, math::Size<Pixels> const min) noexcept 
{
	auto size_hints = ::XSizeHints{
		.flags = PMinSize,
		.min_width = min.x,
		.min_height = min.y,
	};
	::XSetWMNormalHints(server, window, &size_hints);
}
void set_max_size(::Display* const server, ::Window const window, math::Size<Pixels> const max) noexcept 
{
	auto size_hints = ::XSizeHints{
		.flags = PMaxSize,
		.max_width = max.x,
		.max_height = max.y,
	};
	::XSetWMNormalHints(server, window, &size_hints);
}
void set_min_max_size(::Display* const server, ::Window const window, MinMaxSizes<Pixels> const sizes) noexcept 
{
	auto size_hints = ::XSizeHints{
		.flags = PMinSize | PMaxSize,
		.min_width = sizes.min.x,
		.min_height = sizes.min.y,
		.max_width = sizes.max.x,
		.max_height = sizes.max.y,
	};
	::XSetWMNormalHints(server, window, &size_hints);
}

MinMaxSizes<Pixels> get_min_max_sizes(::Display* const server, ::Window const window) noexcept 
{
	XSizeHints result;
	long supplied_return;

	::XGetWMNormalHints(server, window, &result, &supplied_return);
	
	return MinMaxSizes<Pixels>{
		.min{result.min_width, result.min_height},
		.max{result.max_width, result.max_height},
	};
}

//------------------------------

math::Point<Pixels> get_position(::Display* const server, ::Window const window) noexcept 
{
	math::Point<Pixels> result;
	::Window child;
	::XTranslateCoordinates(server, window, DefaultRootWindow(server), 0, 0, &result.x, &result.y, &child);
	return result;
}

//------------------------------
// Black magic I guess? Only found in some StackOverflow answers...

struct MwmHints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
};
enum {
	MWM_HINTS_FUNCTIONS = (1L << 0),
	MWM_HINTS_DECORATIONS =  (1L << 1),

	MWM_FUNC_ALL = (1L << 0),
	MWM_FUNC_RESIZE = (1L << 1),
	MWM_FUNC_MOVE = (1L << 2),
	MWM_FUNC_MINIMIZE = (1L << 3),
	MWM_FUNC_MAXIMIZE = (1L << 4),
	MWM_FUNC_CLOSE = (1L << 5)
};

void set_motif_wm_hints(::Display* const server, ::Window const window, MwmHints hints) noexcept {
	auto const property = ::XInternAtom(server, "_MOTIF_WM_HINTS", false);
	
	::XChangeProperty(
		server, window, 
		property, property,
		32,
		PropModeReplace,
		reinterpret_cast<unsigned char*>(&hints),
		static_cast<int>(sizeof(hints)/4)
	);
}

void set_allowed_actions(::Display* const server, ::Window const window, StyleFlags const style) noexcept {
	auto hints = MwmHints{
		.flags = MWM_HINTS_FUNCTIONS,
		.functions = MWM_FUNC_MOVE | MWM_FUNC_RESIZE
	};

	if (utils::has_flag(style, StyleFlags::MinimizeButton)) {
		hints.functions |= MWM_FUNC_MINIMIZE;
	}
	if (utils::has_flag(style, StyleFlags::MaximizeButton)) {
		hints.functions |= MWM_FUNC_MAXIMIZE;
	}
	if (utils::has_flag(style, StyleFlags::CloseButton)) {
		hints.functions |= MWM_FUNC_CLOSE;
	}

	set_motif_wm_hints(server, window, hints);
}

void make_window_borderless(::Display* const server, ::Window const window) noexcept {
	set_motif_wm_hints(server, window, MwmHints{.flags = MWM_HINTS_DECORATIONS});
}

//------------------------------

void initialize_styles(::Display* const server, ::Window const window, 
	Parameters const& parameters, ScreenUnitConverter const& converter) noexcept 
{
	set_title(server, window, parameters.title);

	set_allowed_actions(server, window, parameters.style);

	if (utils::has_flag(parameters.style, StyleFlags::CustomBorder)) {
		make_window_borderless(server, window);
	}

	if (!utils::has_flag(parameters.style, StyleFlags::Resizable)) {
		auto const size = converter.dip_to_pixels(parameters.size);
		set_min_max_size(server, window, {.min=size, .max=size});
	}
	else if (parameters.size_bounds) {
		set_min_max_size(server, window, converter.dip_to_pixels(parameters.size_bounds));
	}	

	// Show the window.
	if (!utils::has_flag(parameters.style, StyleFlags::Invisible)) {
		::XMapWindow(server, window);
	}

	set_factor_position(server, window, converter.dip_to_pixels(parameters.size), parameters.position_factor);
	::XFlush(server);
}

//------------------------------

enum class StateAction {
	// The order is important.
	// https://specifications.freedesktop.org/wm-spec/wm-spec-1.5.html#idm45623487835680
	Remove,
	Add,
	Toggle
};

void set_window_state(::Display* const server, ::Window const window, 
	StateAction const state_action, std::string_view const action_0, std::string_view const action_1 = {}) 
{
	// ::XInternAtoms(server, const_cast<char**>(states.data()), count, false, atoms.data());
	
	auto event = ::XEvent{
		.type = ClientMessage
	};
	event.xclient = {
		.window = window,
		.message_type = ::XInternAtom(server, "_NET_WM_STATE", false),
		.format = 32
	};
	event.xclient.data.l[0] = static_cast<long>(state_action);
	event.xclient.data.l[1] = ::XInternAtom(server, action_0.data(), false);
	if (!action_1.empty()) {
		event.xclient.data.l[2] = ::XInternAtom(server, action_1.data(), false);
	}

	XSendEvent(server, window, false, SubstructureNotifyMask, &event);
}

// template<typename T>
// concept IsXEventHandler = requires(T x, ::XEvent const& event) {
// 	{ x.handle_event(event) } -> std::same_as<void>;
// };

template<typename/* IsXEventHandler */ _Handler>
class EventManager {
public:
	static constexpr auto event_mask = ExposureMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask | 
		PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask |
		ButtonMotionMask |
		KeyPressMask | KeyReleaseMask;

	::Atom window_manager_client_message_type;
	::Atom window_close_event;

	bool is_running() const noexcept {
		return _is_running;
	}
	void stop() noexcept {
		_is_running = false;
	}

	EventManager(::Display* const server, ::Window const window, _Handler& handler) :
		_server{server}, _window{window}, _handler{handler}
	{
		_open_keyboard_input();
		_setup_events();
		_thread = std::jthread{std::bind_front(&EventManager::_run_event_loop_thread, this)};
	}
	~EventManager() = default;

	EventManager(EventManager const&) = delete;
	EventManager& operator=(EventManager const&) = delete;

	EventManager(EventManager&&) = delete;
	EventManager& operator=(EventManager&&) = delete;

private:
	void _open_keyboard_input() {
		_input_method = x11::InputMethodHandle{::XOpenIM(_server, nullptr, nullptr, nullptr)};

		_input_context = x11::InputContextHandle{::XCreateIC(
			_input_method.get(),
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing, // Input style flags.
			XNClientWindow, _window,
			XNFocusWindow, _window,
			nullptr // Null terminator.
		)};
	}
	void _setup_events() {
		// We want the window manager to tell us when the window should be closed.
		// WM_PROTOCOLS is the atom used to identify messages sent from the window manager in a ClientMessage.
		window_manager_client_message_type = ::XInternAtom(_server, "WM_PROTOCOLS", false);
		
		// This is the atom sent as the data in a ClientMessage with type WM_PROTOCOLS, to indicate the close event.
		window_close_event = ::XInternAtom(_server, "WM_DELETE_WINDOW", false);
		
		// Tell the window manager that we want it to send the event through WM_PROTOCOLS.
		::XSetWMProtocols(_server, _window, &window_close_event, 1);

		::XFlush(_server);
	}

	void _run_event_loop_thread() {
		XInitThreads();

		for (::XEvent event; _is_running;) {
			::XNextEvent(_server, &event);

			if (XFilterEvent(&event, _window)) {
				continue;
			}

			_handler.handle_event(event);
		}
	}

	::Display* _server;
	::Window _window;
	_Handler& _handler;

	bool _is_running{true};

	std::jthread _thread;

	x11::InputMethodHandle _input_method;
	x11::InputContextHandle _input_context;
};

} // namespace x11

class Window::Implementation {
public:
	void title(std::string_view const title) noexcept {
		x11::set_title(_server.get(), _handle.get(), title);
		::XFlush(_server.get());
	}
	[[nodiscard]]
	std::string title() const {
		return x11::get_title(_server.get(), _handle.get());
	}

	bool toggle_fullscreen() {
		x11::set_window_state(_server.get(), _handle.get(), x11::StateAction::Toggle, "_NET_WM_STATE_FULLSCREEN");
		::XFlush(_server.get());
		return _is_fullscreen = !_is_fullscreen;
	}

	void position(math::Point<Pixels> const position) noexcept {
		::XMoveWindow(_server.get(), _handle.get(), position.x, position.y);
		::XFlush(_server.get());
	}

	void size(math::Size<Dip> const size) {
		_size = size;
		auto const pixel_size = _unit_converter.dip_to_pixels(size);
		::XResizeWindow(_server.get(), _handle.get(), pixel_size.x, pixel_size.y);
		::XFlush(_server.get());
	}
	[[nodiscard]]
	math::Size<Dip> size() const noexcept {
		return _size;
	}

	void min_max_size(MinMaxSizes<Dip> const min_max) noexcept {
		x11::set_min_max_size(_server.get(), _handle.get(), _unit_converter.dip_to_pixels(min_max));
	}
	[[nodiscard]]
	MinMaxSizes<Dip> min_max_size() const noexcept {
		return _unit_converter.pixels_to_dip(x11::get_min_max_sizes(_server.get(), _handle.get()));
	}

	void min_size(math::Size<Dip> const min) noexcept {
		x11::set_min_size(_server.get(), _handle.get(), _unit_converter.dip_to_pixels(min));
	}
	[[nodiscard]]
	math::Size<Dip> min_size() const noexcept {
		return _unit_converter.pixels_to_dip(x11::get_min_max_sizes(_server.get(), _handle.get()).min);
	}

	void max_size(math::Size<Dip> const max) noexcept {
		x11::set_max_size(_server.get(), _handle.get(), _unit_converter.dip_to_pixels(max));
	}
	[[nodiscard]]
	math::Size<Dip> max_size() const noexcept {
		return _unit_converter.pixels_to_dip(x11::get_min_max_sizes(_server.get(), _handle.get()).max);
	}

	[[nodiscard]]
	bool is_open() const noexcept {
		return _event_manager->is_running();
	}

	[[nodiscard]]
	::Window native_handle() const {
		return _handle.get();
	}

	Implementation(Parameters const& parameters) :
		_server{::XOpenDisplay(nullptr)},
		_size{parameters.size},
		_unit_converter{x11::get_dpi(_server.get())}
	{
		_create_window(parameters);

		_event_manager.emplace(_server.get(), _handle.get(), *this);
	}

	void handle_event(::XEvent const& event) {
		switch (event.type) {
			case ConfigureNotify:
				_handle_configure_notify(event);
				break;
			case ClientMessage:
				_handle_client_message(event);
				break;
		};
	}

private:
	void _create_window(Parameters const& parameters) {
		auto const visual_info = x11::select_opengl_visual(_server.get());

		_colormap = x11::create_colormap(_server.get(), visual_info.get());

		auto const pixel_size = _unit_converter.dip_to_pixels(parameters.size);

		auto window_attributes = ::XSetWindowAttributes{
			.event_mask = x11::EventManager<Implementation>::event_mask,
			.colormap = _colormap.get(),
		};
		_handle = x11::create_window_handle(_server.get(), visual_info.get(), 
			window_attributes, parameters.parent, pixel_size);

		x11::initialize_styles(_server.get(), _handle.get(), parameters, _unit_converter);
	}

	void _handle_client_message(::XEvent const& event) {
		if (event.xclient.message_type == _event_manager->window_manager_client_message_type) {
			// Sent from the window manager when the user has tried to close the window,
			// it is up to us to decide whether to actually close and exit the application.
			if (static_cast<::Atom>(event.xclient.data.l[0]) == _event_manager->window_close_event) {
				_event_manager->stop();
			}
		}
	}
	void _handle_configure_notify(::XEvent const& event) {
		_size = _unit_converter.pixels_to_dip(math::Size{event.xconfigure.width, event.xconfigure.height});
	}

	x11::DisplayHandle _server;
	x11::WindowHandle _handle;
	x11::ColormapHandle _colormap;
	
	bool _is_fullscreen{};
	math::Size<Dip> _size;

	ScreenUnitConverter _unit_converter;

	std::optional<x11::EventManager<Window::Implementation>> _event_manager;
};
#endif // __linux__

void Window::title(std::string_view const title) {
	_implementation->title(title);
}
std::string Window::title() const {
	return _implementation->title();
}

bool Window::toggle_fullscreen() {
	return _implementation->toggle_fullscreen();
}

void Window::position(math::Point<Pixels> const position) {
	_implementation->position(position);
}

void Window::min_max_size(MinMaxSizes<Dip> const min_max) {
	_implementation->min_max_size(min_max);
}
MinMaxSizes<Dip> Window::min_max_size() const {
	return _implementation->min_max_size();
}

void Window::min_size(math::Size<Dip> const min) {
	_implementation->min_size(min);
}
math::Size<Dip> Window::min_size() const {
	return _implementation->min_size();
}

void Window::max_size(math::Size<Dip> const max) {
	_implementation->max_size(max);
}
math::Size<Dip> Window::max_size() const {
	return _implementation->max_size();
}

void Window::size(math::Size<Dip> const size) {
	_implementation->size(size);
}
math::Size<Dip> Window::size() const {
	return _implementation->size();
}

bool Window::is_open() const {
	return _implementation->is_open();
}

std::any Window::native_handle() const {
	return _implementation->native_handle();
}

Window::Window(Parameters const& parameters) :
	_implementation{std::make_unique<Implementation>(parameters)}
{}

Window::~Window() = default;

Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

Window Builder::open() && {
	return Window{_parameters};
}

} // namespace window

} // namespace avo
