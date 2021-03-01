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

#include "AvoGUI.hpp"

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
auto enable_utf8_console() -> void {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	// Pretty much everyone else uses UTF-8 by default.
}

#ifndef _WIN32
using IconvHandle = utils::UniqueHandle<iconv_t, decltype([](iconv_t const handle){iconv_close(handle);})>;
#endif

auto utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) -> std::optional<std::size_t> {
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
auto utf8_to_utf16(std::string_view const input) -> std::u16string {
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

auto utf16_to_utf8(std::u16string_view const input, std::span<char> const output) -> std::optional<std::size_t> {
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
auto utf16_to_utf8(std::u16string_view const input) -> std::string {
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
	if (auto const length = utf16_to_utf8(input, std::span{output})) {
		output.resize(*length);
		return output;
	}
	return {};
#endif
}

} // namespace unicode

#ifdef _WIN32
class Window::Implementation {
public:
	Implementation(WindowParameters&& parameters) :
		_parameters{parameters}
	{}
private:
	WindowParameters _parameters;
};
#endif

#ifdef __linux__

namespace utils::x11 {

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
XFreeHandle<::XVisualInfo> select_opengl_visual(::Display* const server) noexcept {
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

//------------------------------

[[nodiscard]]
Factor calculate_dip_to_pixel_factor(::Display* const display) noexcept {
	constexpr auto normal_dpi = 96.f;
	return static_cast<Factor>(::XDisplayWidth(display, 0))/static_cast<Factor>(::XDisplayWidthMM(display, 0))*25.4f/normal_dpi;
}

[[nodiscard]]
math::Size<Pixels> get_screen_size(::Display* const display) noexcept {
	return math::Size{
		static_cast<Pixels>(::XDisplayWidth(display, 0)),
		static_cast<Pixels>(::XDisplayHeight(display, 0)),
	};
}

//------------------------------

void set_window_title(::Display* const server, ::Window const window, std::string_view const title) noexcept {
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

	::XFlush(server);
}

[[nodiscard]]
std::string get_window_title(::Display* const server, ::Window const window) {
	::XTextProperty text_property;
	auto const cleanup = utils::Cleanup{[&]{ ::XFree(text_property.value); }};
	
	::XGetWMName(server, window, &text_property);

	return std::string(reinterpret_cast<char*>(text_property.value), text_property.nitems);
}

//------------------------------

math::Point<Pixels> get_window_position(::Display* const server, ::Window const window) noexcept {
	math::Point<Pixels> result;
	::Window child;
	::XTranslateCoordinates(server, window, DefaultRootWindow(server), 0, 0, &result.x, &result.y, &child);
	return result;
}

} // namespace utils::x11

//------------------------------

class WindowStyleManager {
public:
	[[nodiscard]]
	Pixels dip_to_pixels(Dip const dip) const noexcept {
		return static_cast<Pixels>(dip * _dip_to_pixel_factor);
	}
	template<template<typename> typename _Vector>
	[[nodiscard]]
	_Vector<Pixels> dip_to_pixels(_Vector<Dip> const dip) const noexcept {
		return _Vector{
			dip_to_pixels(dip.x),
			dip_to_pixels(dip.y)
		};
	}
	
	[[nodiscard]]
	Dip pixels_to_dip(Pixels const pixels) const noexcept {
		return static_cast<Dip>(pixels) / _dip_to_pixel_factor;
	}
	template<template<typename> typename _Vector>
	[[nodiscard]]
	_Vector<Dip> pixels_to_dip(_Vector<Pixels> const pixels) const noexcept {
		return _Vector{
			pixels_to_dip(pixels.x),
			pixels_to_dip(pixels.y)
		};
	}

	//------------------------------

	[[nodiscard]]
	bool get_is_resizable() const noexcept {
		return (_parameters.style & WindowStyleFlags::Resizable) != WindowStyleFlags::None;
	}

	void initialize_styles(::Display* const server, ::Window const window) const noexcept {
		utils::x11::set_window_title(server, window, _parameters.title);

		update_resizable(server, window);
		update_min_max_sizes(server, window);
	}
	void update_resizable(::Display* const server, ::Window const window) const noexcept {
		if (!get_is_resizable()) {
			auto const size = dip_to_pixels(_parameters.size);
			auto size_hints = ::XSizeHints{
				.flags = PMinSize | PMaxSize,
				.min_width = size.x,
				.min_height = size.y,
				.max_width = size.x,
				.max_height = size.y,
			};
			XSetWMNormalHints(server, window, &size_hints);
		}
	}
	void update_min_max_sizes(::Display* const server, ::Window const window) const noexcept {
		if (get_is_resizable() && (_parameters.min_size || _parameters.max_size)) {
			auto const min = dip_to_pixels(_parameters.min_size);
			auto const max = dip_to_pixels(_parameters.max_size);
			auto size_hints = ::XSizeHints{
				.flags = PMinSize | PMaxSize,
				.min_width = min.x,
				.min_height = min.y,
				.max_width = max.x,
				.max_height = max.y,
			};
			XSetWMNormalHints(server, window, &size_hints);
		}
	}

	WindowParameters const& parameters() const noexcept {
		return _parameters;
	}

	WindowStyleManager(::Display* const server, WindowParameters&& parameters) :
		_parameters{std::move(parameters)}
	{
		_dip_to_pixel_factor = utils::x11::calculate_dip_to_pixel_factor(server);
	}

private:
	WindowParameters _parameters;
	Factor _dip_to_pixel_factor;
};

class WindowEventManager {
public:
private:
};

class Window::Implementation {
public:
	void title(std::string_view const title) noexcept {
		utils::x11::set_window_title(_server.get(), _handle.get(), title);
	}
	[[nodiscard]]
	std::string title() const {
		return utils::x11::get_window_title(_server.get(), _handle.get());
	}

	void position(math::Point<Pixels> const position) noexcept {
		::XMoveWindow(_server.get(), _handle.get(), position.x, position.y);
		::XFlush(_server.get());
	}

	void size(math::Size<Dip> const size) {
		_size = size;
		auto const pixel_size = _style_manager.dip_to_pixels(size);//.to<math::Size<unsigned int>>();
		::XResizeWindow(_server.get(), _handle.get(), pixel_size.x, pixel_size.y);
		::XFlush(_server.get());
	}
	[[nodiscard]]
	math::Size<Dip> size() const noexcept {
		return _size;
	}

	[[nodiscard]]
	bool is_open() const noexcept {
		return _is_open;
	}

	[[nodiscard]]
	::Window native_handle() const {
		return _handle.get();
	}

	Implementation(WindowParameters&& parameters) :
		_server{::XOpenDisplay(nullptr)},
		_size{parameters.size},
		_style_manager{_server.get(), std::move(parameters)}
	{
		_create_window();

		_open_keyboard_input();

		_setup_events();

		_thread = std::jthread{utils::bind(&Implementation::_run_event_loop_thread, this)};
	}

private:
	void _create_window() {
		auto const visual_info = utils::x11::select_opengl_visual(_server.get());

		_colormap = utils::x11::ColormapHandle{
			_server.get(), 
			::XCreateColormap(_server.get(), RootWindow(_server.get(), visual_info->screen), visual_info->visual, 0)
		};

		auto const& parameters = _style_manager.parameters();

		auto window_attributes = ::XSetWindowAttributes{
			.event_mask = ExposureMask | 
				EnterWindowMask | LeaveWindowMask |
				StructureNotifyMask | 
				PointerMotionMask |
				ButtonPressMask | ButtonReleaseMask |
				ButtonMotionMask |
				KeyPressMask | KeyReleaseMask,
			.colormap = _colormap.get(),
		};
		_handle = utils::x11::WindowHandle{
			_server.get(),
			::XCreateWindow(
				_server.get(),
				parameters.parent 
					? std::any_cast<::Window>(parameters.parent->native_handle()) 
					: RootWindow(_server.get(), visual_info->screen),
				0, 0, // Initial x and y are ignored by the window manager
				static_cast<unsigned int>(_style_manager.dip_to_pixels(_size.x)),
				static_cast<unsigned int>(_style_manager.dip_to_pixels(_size.y)),
				0,
				visual_info->depth,
				InputOutput,
				visual_info->visual,
				CWEventMask | CWBorderPixel | CWColormap,
				&window_attributes
			)
		};

		_style_manager.initialize_styles(_server.get(), _handle.get());
		
		// Show the window.
		::XMapWindow(_server.get(), _handle.get());

		auto const screen_size = utils::x11::get_screen_size(_server.get());
		position({
			static_cast<Pixels>(std::lerp(0.f, static_cast<float>(screen_size.x) - parameters.size.x, parameters.position_factor.x)),
			static_cast<Pixels>(std::lerp(0.f, static_cast<float>(screen_size.y) - parameters.size.y, parameters.position_factor.y))
		});
	}
	void _open_keyboard_input() {
		_input_method = utils::x11::InputMethodHandle{::XOpenIM(_server.get(), nullptr, nullptr, nullptr)};

		_input_context = utils::x11::InputContextHandle{::XCreateIC(
			_input_method.get(),
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing, // Input style flags.
			XNClientWindow, _handle.get(),
			XNFocusWindow, _handle.get(),
			nullptr // Null terminator.
		)};
	}
	void _setup_events() {
		// We want the window manager to tell us when the window should be closed.
		// WM_PROTOCOLS is the atom used to identify messages sent from the window manager in a ClientMessage.
		_window_manager_client_message_type = ::XInternAtom(_server.get(), "WM_PROTOCOLS", true);
		
		// This is the atom sent as the data in a ClientMessage with type WM_PROTOCOLS, to indicate the close event.
		_window_close_event = ::XInternAtom(_server.get(), "WM_DELETE_WINDOW", 0);
		
		// Tell the window manager that we want it to send the event through WM_PROTOCOLS.
		::XSetWMProtocols(_server.get(), _handle.get(), &_window_close_event, 1);

		::XFlush(_server.get());
	}

	void _run_event_loop_thread() {
		XInitThreads();

		for (::XEvent event; _is_open;) {
			::XNextEvent(_server.get(), &event);

			if (XFilterEvent(&event, _handle.get())) {
				continue;
			}

			_handle_event(event);
		}
	}
	void _handle_event(::XEvent const& event) {
		switch (event.type) {
			case ConfigureNotify:
				_handle_configure_notify(event);
				break;
			case ClientMessage:
				_handle_client_message(event);
				break;
		};
	}
	void _handle_client_message(::XEvent const& event) {
		if (event.xclient.message_type == _window_manager_client_message_type) {
			// Sent from the window manager when the user has tried to close the window,
			// it is up to us to decide whether to actually close and exit the application.
			if (static_cast<::Atom>(event.xclient.data.l[0]) == _window_close_event) {
				_is_open = false;
			}
		}
	}
	void _handle_configure_notify(::XEvent const& event) {
		_size = _style_manager.pixels_to_dip(math::Size{event.xconfigure.width, event.xconfigure.height});
	}

	bool _is_open{true};

	utils::x11::DisplayHandle _server;
	utils::x11::WindowHandle _handle;
	utils::x11::ColormapHandle _colormap;
	
	math::Size<Dip> _size;

	WindowStyleManager _style_manager;

	::Atom _window_manager_client_message_type;
	::Atom _window_close_event;
	utils::x11::InputMethodHandle _input_method;
	utils::x11::InputContextHandle _input_context;

	std::jthread _thread;
};
#endif

void Window::title(std::string_view const title) {
	_implementation->title(title);
}
std::string Window::title() const {
	return _implementation->title();
}

void Window::position(math::Point<Pixels> const position) {
	_implementation->position(position);
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

Window::Window(WindowParameters&& parameters) :
	_implementation{std::make_unique<Implementation>(std::move(parameters))}
{}

Window::~Window() = default;

Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

Window WindowBuilder::open() && {
	return Window{std::move(_parameters)};
}

} // namespace avo
