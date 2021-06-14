// Included exactly once.

#include "avo/concurrency.hpp"
#include "avo/utils/unique_handle.hpp"
#include "avo/window.hpp"

#include <thread>

//------------------------------

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

//------------------------------

namespace avo::window {

namespace x11 {

template<utils::IsTrivial T, std::invocable<::Display*, T> Deleter_>
class DisplayResourceHandle {
public:
	[[nodiscard]]
	T get() const noexcept {
		return value_;
	}

	DisplayResourceHandle(::Display* const server, T const value) :
		server_{server},
		value_{value}
	{}

	DisplayResourceHandle() = default;
	~DisplayResourceHandle() {
		if (server_) {
			Deleter_{}(server_, value_);
		}
	}

	DisplayResourceHandle(DisplayResourceHandle const&) = delete;
	DisplayResourceHandle& operator=(DisplayResourceHandle const&) = delete;
	
	DisplayResourceHandle(DisplayResourceHandle&& other) noexcept :
		server_{other.server_},
		value_{other.value_}
	{
		other.server_ = nullptr;
	}
	DisplayResourceHandle& operator=(DisplayResourceHandle&& other) noexcept {
		server_ = other.server_;
		value_ = other.value_;
		other.server_ = nullptr;
		return *this;
	}

private:
	::Display* server_{};
	T value_{};
};

using DisplayHandle = std::unique_ptr<::Display, decltype([](auto x){ ::XCloseDisplay(x); })>;

using ColormapHandle = DisplayResourceHandle<::Colormap, decltype([](auto a, auto b){ ::XFreeColormap(a, b); })>;

using WindowHandle = DisplayResourceHandle<::Window, decltype([](auto a, auto b){ ::XDestroyWindow(a, b); })>;

using InputMethodHandle = utils::UniqueHandle<::XIM, decltype([](auto x){ ::XCloseIM(x); })>;
using InputContextHandle = utils::UniqueHandle<::XIC, decltype([](auto x){ ::XDestroyIC(x); })>;

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
	math::Size<Pixels> const window_size, math::Vector2d<math::Factor> const position_factor) noexcept
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
		return is_running_;
	}
	void stop() noexcept {
		is_running_ = false;
	}

	EventManager(::Display* const server, ::Window const window, _Handler& handler) :
		server_{server}, window_{window}, handler_{handler}
	{
		open_keyboard_input_();
		setup_events_();
		thread_ = std::jthread{&EventManager::run_event_loop_thread_, this};
	}
	~EventManager() = default;

	EventManager(EventManager const&) = delete;
	EventManager& operator=(EventManager const&) = delete;

	EventManager(EventManager&&) = delete;
	EventManager& operator=(EventManager&&) = delete;

private:
	void open_keyboard_input_() {
		input_method_ = x11::InputMethodHandle{::XOpenIM(server_, nullptr, nullptr, nullptr)};

		input_context_ = x11::InputContextHandle{::XCreateIC(
			input_method_.get(),
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing, // Input style flags.
			XNClientWindow, window_,
			XNFocusWindow, window_,
			nullptr // Null terminator.
		)};
	}
	void setup_events_() {
		// We want the window manager to tell us when the window should be closed.
		// WM_PROTOCOLS is the atom used to identify messages sent from the window manager in a ClientMessage.
		window_manager_client_message_type = ::XInternAtom(server_, "WM_PROTOCOLS", false);
		
		// This is the atom sent as the data in a ClientMessage with type WM_PROTOCOLS, to indicate the close event.
		window_close_event = ::XInternAtom(server_, "WM_DELETE_WINDOW", false);
		
		// Tell the window manager that we want it to send the event through WM_PROTOCOLS.
		::XSetWMProtocols(server_, window_, &window_close_event, 1);

		::XFlush(server_);
	}

	void run_event_loop_thread_() {
		XInitThreads();

		for (::XEvent event; is_running_;) {
			::XNextEvent(server_, &event);

			if (XFilterEvent(&event, window_)) {
				continue;
			}

			handler_.handle_event(event);
		}
	}

	::Display* server_;
	::Window window_;
	_Handler& handler_;

	bool is_running_{true};

	std::jthread thread_;

	x11::InputMethodHandle input_method_;
	x11::InputContextHandle input_context_;
};

} // namespace x11

class Window::Implementation {
public:
	void title(std::string_view const title) noexcept {
		x11::set_title(server_.get(), handle_.get(), title);
		::XFlush(server_.get());
	}
	[[nodiscard]]
	std::string title() const {
		return x11::get_title(server_.get(), handle_.get());
	}

	bool toggle_fullscreen() {
		x11::set_window_state(server_.get(), handle_.get(), x11::StateAction::Toggle, "_NET_WM_STATE_FULLSCREEN");
		::XFlush(server_.get());
		return is_fullscreen_ = !is_fullscreen_;
	}

	void position(math::Point<Pixels> const position) noexcept {
		::XMoveWindow(server_.get(), handle_.get(), position.x, position.y);
		::XFlush(server_.get());
	}

	void size(math::Size<Dip> const size) {
		size_ = size;
		auto const pixel_size = unit_converter_.dip_to_pixels(size);
		::XResizeWindow(server_.get(), handle_.get(), pixel_size.x, pixel_size.y);
		::XFlush(server_.get());
	}
	[[nodiscard]]
	math::Size<Dip> size() const noexcept {
		return size_;
	}

	void min_max_size(MinMaxSizes<Dip> const min_max) noexcept {
		x11::set_min_max_size(server_.get(), handle_.get(), unit_converter_.dip_to_pixels(min_max));
	}
	[[nodiscard]]
	MinMaxSizes<Dip> min_max_size() const noexcept {
		return unit_converter_.pixels_to_dip(x11::get_min_max_sizes(server_.get(), handle_.get()));
	}

	void min_size(math::Size<Dip> const min) noexcept {
		x11::set_min_size(server_.get(), handle_.get(), unit_converter_.dip_to_pixels(min));
	}
	[[nodiscard]]
	math::Size<Dip> min_size() const noexcept {
		return unit_converter_.pixels_to_dip(x11::get_min_max_sizes(server_.get(), handle_.get()).min);
	}

	void max_size(math::Size<Dip> const max) noexcept {
		x11::set_max_size(server_.get(), handle_.get(), unit_converter_.dip_to_pixels(max));
	}
	[[nodiscard]]
	math::Size<Dip> max_size() const noexcept {
		return unit_converter_.pixels_to_dip(x11::get_min_max_sizes(server_.get(), handle_.get()).max);
	}

	[[nodiscard]]
	bool is_open() const noexcept {
		return event_manager_->is_running();
	}

	[[nodiscard]]
	::Window native_handle() const {
		return handle_.get();
	}

	Implementation(Parameters const& parameters) :
		server_{::XOpenDisplay(nullptr)},
		size_{parameters.size},
		unit_converter_{x11::get_dpi(server_.get())}
	{
		create_window_(parameters);

		event_manager_.emplace(server_.get(), handle_.get(), *this);
	}

	void handle_event(::XEvent const& event) {
		switch (event.type) {
			case ConfigureNotify:
				handle_configure_notify_(event);
				break;
			case ClientMessage:
				handle_client_message_(event);
				break;
		};
	}

private:
	void create_window_(Parameters const& parameters) {
		auto const visual_info = x11::select_opengl_visual(server_.get());

		colormap_ = x11::create_colormap(server_.get(), visual_info.get());

		auto const pixel_size = unit_converter_.dip_to_pixels(parameters.size);

		auto window_attributes = ::XSetWindowAttributes{
			.event_mask = x11::EventManager<Implementation>::event_mask,
			.colormap = colormap_.get(),
		};
		handle_ = x11::create_window_handle(server_.get(), visual_info.get(), 
			window_attributes, parameters.parent, pixel_size);

		x11::initialize_styles(server_.get(), handle_.get(), parameters, unit_converter_);
	}

	void handle_client_message_(::XEvent const& event) {
		if (event.xclient.message_type == event_manager_->window_manager_client_message_type) {
			// Sent from the window manager when the user has tried to close the window,
			// it is up to us to decide whether to actually close and exit the application.
			if (static_cast<::Atom>(event.xclient.data.l[0]) == event_manager_->window_close_event) {
				event_manager_->stop();
			}
		}
	}
	void handle_configure_notify_(::XEvent const& event) {
		size_ = unit_converter_.pixels_to_dip(math::Size{event.xconfigure.width, event.xconfigure.height});
	}

	x11::DisplayHandle server_;
	x11::WindowHandle handle_;
	x11::ColormapHandle colormap_;
	
	bool is_fullscreen_{};
	math::Size<Dip> size_;

	ScreenUnitConverter unit_converter_;

	std::optional<x11::EventManager<Window::Implementation>> event_manager_;
};

} // namespace avo::window
