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
template<utils::IsTrivial T, std::invocable<::Display*, T> _Deleter>
class XDisplayResourceHandle {
public:
	T get() const noexcept {
		return _value;
	}

	XDisplayResourceHandle(::Display* const server, T const value) :
		_server{server},
		_value{value}
	{}

	XDisplayResourceHandle() = default;
	~XDisplayResourceHandle() {
		if (_server) {
			_Deleter{}(_server, _value);
		}
	}

	XDisplayResourceHandle(XDisplayResourceHandle const&) = delete;
	XDisplayResourceHandle& operator=(XDisplayResourceHandle const&) = delete;
	
	XDisplayResourceHandle(XDisplayResourceHandle&& other) noexcept :
		_server{other._server},
		_value{other._value}
	{
		other._server = nullptr;
	}
	XDisplayResourceHandle& operator=(XDisplayResourceHandle&& other) noexcept {
		_server = other._server;
		_value = other._value;
		other._server = nullptr;
		return *this;
	}

private:
	::Display* _server{};
	T _value{};
};

using XDisplayHandle = std::unique_ptr<::Display, decltype([](auto* const x){ ::XCloseDisplay(x); })>;

using XColormapHandle = XDisplayResourceHandle<::Colormap, decltype([](auto a, auto b){ ::XFreeColormap(a, b); })>;

using XWindowHandle = XDisplayResourceHandle<::Window, decltype([](auto a, auto b){ ::XDestroyWindow(a, b); })>;

template<typename T>
using XFreeHandle = std::unique_ptr<T, decltype([](T* info){ ::XFree(info); })>;

XFreeHandle<::XVisualInfo> select_opengl_visual(::Display* const server) {
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

Factor calculate_dip_to_pixel_factor(::Display* const display) {
	constexpr auto normal_dpi = 96.f;
	return static_cast<Factor>(::XDisplayWidth(display, 0))/static_cast<Factor>(::XDisplayWidthMM(display, 0))*25.4f/normal_dpi;
}

math::Size<Pixels> get_screen_size(::Display* const display) {
	return math::Size{
		static_cast<Pixels>(::XDisplayWidth(display, 0)),
		static_cast<Pixels>(::XDisplayHeight(display, 0)),
	};
}

class Window::Implementation {
public:
	void title(std::string title) {
		auto text_property = XTextProperty{
			.value = reinterpret_cast<unsigned char*>(title.data()),
			#ifdef X_HAVE_UTF8_STRING
			.encoding = XInternAtom(_server.get(), "UTF8_STRING", 0),
			#else
			.encoding = XA_STRING,
			#endif
			.format = 8,
			.nitems = static_cast<unsigned long>(title.size()),
		};
		XSetWMName(_server.get(), _handle.get(), &text_property);
		XSetWMIconName(_server.get(), _handle.get(), &text_property);
		XFlush(_server.get());
	}

	void position(math::Point<Pixels> const position) {
		XMoveWindow(_server.get(), _handle.get(), position.x, position.y);
		XFlush(_server.get());
	}

	::Window native_handle() const {
		return _handle.get();
	}

	Implementation(WindowParameters&& parameters) :
		_parameters{parameters},
		_thread{utils::bind(&Implementation::_run, this)}
	{}

private:
	void _run() {
		// If an automatic thread locking mechanism for Xlib is required in the future
		// XInitThreads();

		_server = XDisplayHandle{::XOpenDisplay(nullptr)};

		_dip_to_pixel_factor = calculate_dip_to_pixel_factor(_server.get());
		_screen_size = get_screen_size(_server.get());

		_create_window();
		_run_event_loop();
	}
	void _create_window() {
		auto const visual_info = select_opengl_visual(_server.get());
		_colormap = XColormapHandle{
			_server.get(), 
			::XCreateColormap(_server.get(), RootWindow(_server.get(), visual_info->screen), visual_info->visual, 0)
		};

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
		_handle = XWindowHandle{
			_server.get(),
			::XCreateWindow(
				_server.get(),
				_parameters.parent 
					? std::any_cast<::Window>(_parameters.parent->native_handle()) 
					: RootWindow(_server.get(), visual_info->screen),
				0, 0, // Initial x and y are ignored by the window manager
				static_cast<unsigned int>(_parameters.size.x*_dip_to_pixel_factor),
				static_cast<unsigned int>(_parameters.size.y*_dip_to_pixel_factor),
				0,
				visual_info->depth,
				InputOutput,
				visual_info->visual,
				CWEventMask | CWBorderPixel | CWColormap,
				&window_attributes
			)
		};
		title(_parameters.title);
		::XMapWindow(_server.get(), _handle.get());
		position({
			static_cast<Pixels>(std::lerp(0.f, static_cast<float>(_screen_size.x) - _parameters.size.x, _parameters.position_factor.x)),
			static_cast<Pixels>(std::lerp(0.f, static_cast<float>(_screen_size.y) - _parameters.size.y, _parameters.position_factor.y))
		});
	}
	void _run_event_loop() {
		std::this_thread::sleep_for(20s);
	}

	WindowParameters _parameters;
	math::Size<Pixels> _screen_size;
	Factor _dip_to_pixel_factor;

	XDisplayHandle _server;
	XColormapHandle _colormap;
	XWindowHandle _handle;

	std::jthread _thread;
};
#endif

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
