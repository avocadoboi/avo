// Included exactly once.

#include "avo/concurrency.hpp"
#include "avo/math/rectangle.hpp"
#include "avo/unicode.hpp"
#include "avo/util/static_map.hpp"
#include "avo/window.hpp"

#include <cassert>
#include <functional>
#include <thread>

//------------------------------

#include "common.hpp"
#include <windowsx.h>

//------------------------------

using namespace std::string_view_literals;

//------------------------------

namespace avo::window {

namespace win {

[[nodiscard]]
bool get_is_vkey_down(int const vkey) {
	return GetAsyncKeyState(vkey) & (1 << 16);
}

//------------------------------

[[nodiscard]]
constexpr DWORD style_flags_to_native(StyleFlags const flags, bool const has_parent) noexcept 
{
	auto native_flags = DWORD{};

	using enum StyleFlags;

	if (has_parent || util::has_flag(flags, CustomBorder)) {
		native_flags |= WS_POPUP;
	}
	if (not util::has_flag(flags, Invisible)) {
		native_flags |= WS_VISIBLE;
	}
	if (util::has_flag(flags, CloseButton | CustomBorder)) 
	{ // CloseButton or CustomBorder is true; not necessarily both
		native_flags |= WS_CAPTION | WS_SYSMENU;
	}
	if (util::has_flag(flags, MinimizeButton)) {
		native_flags |= WS_MINIMIZEBOX;
	}
	if (util::has_flag(flags, MaximizeButton)) {
		native_flags |= WS_MAXIMIZEBOX;
	}
	if (util::has_flag(flags, Resizable)) {
		native_flags |= WS_THICKFRAME;
	}

	return native_flags;
}

[[nodiscard]]
math::Rectangle<Pixels> window_rectangle_from_client_size(::DWORD const native_flags, math::Size<Pixels> const size) 
{
	auto rect = ::RECT{0, 0, size.x, size.y};
	::AdjustWindowRect(&rect, native_flags, ::BOOL{});
	return {rect.left, rect.top, rect.right, rect.bottom};
}
[[nodiscard]]
math::Rectangle<Pixels> window_rectangle_from_client_size(::HWND const handle, math::Size<Pixels> const size) {
	return window_rectangle_from_client_size(::GetWindowLong(handle, GWL_STYLE), size);
}

[[nodiscard]]
math::Rectangle<Pixels> get_parent_rectangle(::HWND const parent)
{
	if (parent) {
		// The result should be the client rectangle of the parent window in screen coordinates.
		// There is no native function to get this directly.

		// window_rect is used to get the width of the non-client area to the left of the client area,
		// and the height of the non-client area to the top of the client area.
		// First, we get the client area, which is really just the size because it's always at (0, 0), 
		// and then we use AdjustWindowRect to expand the window borders.
		// This results in the left and top coordinates being negative if there is any non-client area there.
		::RECT window_rect;
		::GetClientRect(parent, &window_rect);
		::AdjustWindowRect(&window_rect, GetWindowStyle(parent), false);
		
		// Get the non-client rectangle of the parent in screen coordinates.
		::RECT result;
		::GetWindowRect(parent, &result);

		// Subtract the borders assuming the left, bottom and right non-client sizes are the same.
		return {
			result.left - window_rect.left,
			result.top - window_rect.top,
			result.right + window_rect.left,
			result.bottom + window_rect.left,
		};
	}
	else 
	{ // No parent window, return the work area of the monitor at the cursor position.
		::POINT cursor_position;
		::GetCursorPos(&cursor_position);
		
		auto const monitor = ::MonitorFromPoint(cursor_position, MONITOR_DEFAULTTONEAREST);
		auto monitor_info = ::MONITORINFO{
			.cbSize{static_cast<::DWORD>(sizeof(::MONITORINFO))}
		};
		::GetMonitorInfo(monitor, &monitor_info);

		return {
			monitor_info.rcWork.left,
			monitor_info.rcWork.top,
			monitor_info.rcWork.right,
			monitor_info.rcWork.bottom,
		};
	}
}

//------------------------------

void set_window_position(::HWND const handle, math::Point<Pixels> const position)
{
	::SetWindowPos(
		handle, ::HWND{}, 
		position.x, position.y, 
		0, 0, 
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
	);
}
void set_window_size(::HWND const handle, math::Size<Pixels> const size) 
{
	auto const full_rect = window_rectangle_from_client_size(handle, size);
	::SetWindowPos(
		handle, ::HWND{}, 0, 0, 
		full_rect.width(), full_rect.height(), 
		SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
	);
}

[[nodiscard]]
math::Point<Pixels> position_from_parent_relative_factor(math::Vector2d<float> const factor, math::Size<Pixels> const pixel_size, 
	math::Rectangle<Pixels> const window_rect, ::HWND const parent)
{
	auto const parent_rect = get_parent_rectangle(parent);
	
	return math::interpolate(
		// At (0.f, 0.f) we want it to show the title but not the shadow/left border.
		(parent_rect.top_left() + math::Point{window_rect.left, 0}).to<float>(),
		// At (1.f, 1.f) we want to subtract the space to the left and above the client area from the window position so that the bottom right 
		// corner of the client rectangle touches the bottom right corner of the parent rectangle.
		(parent_rect.bottom_right() + window_rect.top_left() - pixel_size.to<math::Point>()).to<math::Point<float>>(),
		factor
	).to<Pixels>();
}

//------------------------------

class FullscreenToggle {
public:
	bool toggle() {
		assert(window_);
		if (is_fullscreen_) {
			restore_();
		}
		else {
			make_fullscreen_();
		}
		return is_fullscreen_ = not is_fullscreen_;
	}
	[[nodiscard]]
	bool is_fullscreen() const noexcept {
		return is_fullscreen_;
	}

	/*
		Assumes the window is not fullscreen initially.
	*/
	explicit FullscreenToggle(::HWND const window) :
		window_{window}
	{}

	FullscreenToggle() = default;

private:
	void restore_() {
		::SetWindowLong(window_, GWL_STYLE, style_before_);
		if (was_maximized_) {
			::SendMessage(window_, WM_SYSCOMMAND, SC_MAXIMIZE, {});
		}
		else {
			::SetWindowPos(
				window_, nullptr,
				rectangle_before_.left, 
				rectangle_before_.top, 
				rectangle_before_.right - rectangle_before_.left,
				rectangle_before_.bottom - rectangle_before_.top,
				SWP_ASYNCWINDOWPOS | SWP_NOOWNERZORDER | SWP_NOZORDER
			);
		}
	}
	void make_fullscreen_() {
		was_maximized_ = static_cast<bool>(::IsZoomed(window_));
		if (not was_maximized_) {
			::GetWindowRect(window_, &rectangle_before_);
		}
		style_before_ = ::GetWindowLong(window_, GWL_STYLE);

		::SetWindowLong(window_, GWL_STYLE, WS_VISIBLE | WS_MAXIMIZE);

		auto monitor = ::MONITORINFO{
			.cbSize{sizeof(::MONITORINFO)}
		};
		::GetMonitorInfo(::MonitorFromWindow(window_, MONITOR_DEFAULTTONEAREST), &monitor);

		::SetWindowPos(
			window_, nullptr,
			monitor.rcMonitor.left, monitor.rcMonitor.top,
			monitor.rcMonitor.right - monitor.rcMonitor.left,
			monitor.rcMonitor.bottom - monitor.rcMonitor.top,
			SWP_ASYNCWINDOWPOS | SWP_NOOWNERZORDER | SWP_NOZORDER
		);
	}

	::HWND window_;

	bool is_fullscreen_{};
	
	bool was_maximized_;
	::DWORD style_before_;
	::RECT rectangle_before_;
};

//------------------------------

void set_window_title(::HWND const handle, std::string_view const title)
{
	auto const wide_title = unicode::utf8_to_utf16(title);
	::SetWindowText(handle, reinterpret_cast<LPCWSTR>(wide_title.data()));
}

[[nodiscard]]
std::string get_window_title(::HWND const handle)
{
	constexpr auto max_length = std::size_t{256};
	std::array<char16_t, max_length> buffer;

	auto const length = ::GetWindowText(handle, reinterpret_cast<LPWSTR>(buffer.data()), max_length);
	assert(length >= 0); // According to the GetWindowTextW docs it will never return a negative value.

	return unicode::utf16_to_utf8(std::u16string_view{buffer.data(), static_cast<std::size_t>(length)});
}

//------------------------------

constexpr bool get_is_key_repeated(::LPARAM const l_data) noexcept {
	return l_data & (1 << 30);
}

constexpr MouseButton x_button_from_w_data(::WPARAM const w_data) noexcept {
	return HIWORD(w_data) == XBUTTON1 ? MouseButton::X0 : MouseButton::X1;
}

//------------------------------

constexpr auto native_mouse_button_map = [] {
	using enum MouseButton;
	return util::StaticMap<int, MouseButton, 5>{
		{VK_LBUTTON, Left},
		{VK_MBUTTON, Middle},
		{VK_RBUTTON, Right},
		{VK_XBUTTON1, X0},
		{VK_XBUTTON2, X1},
	};
}();

constexpr auto native_key_map = [] {
	using enum KeyboardKey;
	return util::StaticMap<int, KeyboardKey, 112>{
		{VK_APPS, Menu},
		{VK_BACK, Backspace},
		{VK_CLEAR, Clear},
		{VK_TAB, Tab},
		{VK_RETURN, Enter},
		{VK_SHIFT, Shift},
		{VK_CONTROL, Control},
		{VK_LWIN, Super},
		{VK_RWIN, Super},
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
HINSTANCE get_instance_handle() {
	return GetModuleHandle(nullptr);
}

//------------------------------

[[nodiscard]]
math::Point<Pixels> get_window_mouse_position(::HWND const handle)
{
	::POINT result;
	::GetCursorPos(&result);
	::ScreenToClient(handle, &result);
	return {result.x, result.y};
}

//------------------------------

class WindowThread {
public:
	/*
		Waits until the window handle is valid.
	*/
	void wait_until_window_created() const {
		window_created_flag_.wait(false);
	}
	/*
		Must only be called after window creation.
	*/
	::HWND get_handle() const {
		return handle_;
	}

	void set_min_max_size(MinMaxSize<Dip> const sizes) {
		min_max_size_ = unit_converter_.dip_to_pixels(sizes);
	}
	void set_min_size(math::Size<Dip> const size) {
		min_max_size_ = util::MinMax{unit_converter_.dip_to_pixels(size), min_max_size_.load().max};
	}
	void set_max_size(math::Size<Dip> const size) {
		min_max_size_ = util::MinMax{min_max_size_.load().min, unit_converter_.dip_to_pixels(size)};
	}
	MinMaxSize<Dip> get_min_max_size() const {
		return unit_converter_.pixels_to_dip(min_max_size_.load());
	}

	WindowThread(Parameters const& parameters, concurrency::Sender<Event> channel) :
		channel_{std::move(channel)},
		thread_{&WindowThread::run_, this, parameters}
	{}

private:
	/*
		Manages the "window class", properties common to the window instances created by this library in this process.
		Each window instance has its own WindowClass instance to manage the window class lifetime.
	*/
	class WindowClass {
	public:
		static constexpr auto class_name = L"AVO Window Class"sv;

		WindowClass() 
		{
			if (not s_instance_count_++) 
			{
				auto const properties = ::WNDCLASSW{
					.style{CS_DBLCLKS}, // We want double click events
					.lpfnWndProc{s_handle_any_window_event_},
					.hInstance{get_instance_handle()},
					.hCursor{::LoadCursor(nullptr, IDC_ARROW)},
					.lpszClassName{class_name.data()}
				};
				::RegisterClass(&properties);
			}
		}
		~WindowClass() {
			if (not --s_instance_count_) {
				::UnregisterClass(class_name.data(), get_instance_handle());
			}
		}

		WindowClass(WindowClass&&) = delete;
		WindowClass& operator=(WindowClass&&) = delete;
		WindowClass(WindowClass const&) = delete;
		WindowClass& operator=(WindowClass const&) = delete;

	private:
		static WindowThread* s_instance_from_event_(HWND const handle, UINT const message, LPARAM const l_data) 
		{
			if (message == WM_CREATE)
			{
				auto const window = static_cast<WindowThread*>(reinterpret_cast<CREATESTRUCT*>(l_data)->lpCreateParams);
				::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
				window->set_window_handle_(handle);
				return window;
			}
			else {
				return reinterpret_cast<WindowThread*>(::GetWindowLongPtr(handle, GWLP_USERDATA));
			}
		}
		
		static LRESULT CALLBACK s_handle_any_window_event_(
			HWND const window_handle, UINT const message, WPARAM const w_data, LPARAM const l_data)
		{
			if (auto const instance = s_instance_from_event_(window_handle, message, l_data))
			{
				if (auto const result = instance->handle_event_(message, w_data, l_data))
				{
					return *result;
				}
			}
			return ::DefWindowProc(window_handle, message, w_data, l_data);
		}
	
		static inline std::atomic<int> s_instance_count_{};
	};
	
	//------------------------------

	void set_window_handle_(::HWND const handle) {
		handle_ = handle;
	}

	std::optional<::LRESULT> handle_event_(::UINT const message, ::WPARAM const w_data, ::LPARAM const l_data)
	{
		switch (message) {
		case WM_GETMINMAXINFO:
			return handle_min_max_size_query(l_data);
		case WM_DPICHANGED:
			return handle_dpi_change_(w_data, l_data);
		case WM_MOUSEMOVE:
			return handle_mouse_move_(l_data);
		case WM_MOUSEWHEEL:
			return handle_mouse_scroll_(w_data);
		case WM_MOUSELEAVE:
			return handle_mouse_leave_();
		case WM_LBUTTONDBLCLK:
			return handle_mouse_down_(l_data, MouseButton::Left, true);
		case WM_MBUTTONDBLCLK:
			return handle_mouse_down_(l_data, MouseButton::Middle, true);
		case WM_RBUTTONDBLCLK:
			return handle_mouse_down_(l_data, MouseButton::Right, true);
		case WM_XBUTTONDBLCLK:
			return handle_mouse_down_(l_data, x_button_from_w_data(w_data), true);
		case WM_LBUTTONDOWN:
			return handle_mouse_down_(l_data, MouseButton::Left, false);
		case WM_MBUTTONDOWN:
			return handle_mouse_down_(l_data, MouseButton::Middle, false);
		case WM_RBUTTONDOWN:
			return handle_mouse_down_(l_data, MouseButton::Right, false);
		case WM_XBUTTONDOWN:
			return handle_mouse_down_(l_data, x_button_from_w_data(w_data), false);
		case WM_LBUTTONUP:
			return handle_mouse_up_(l_data, MouseButton::Left);
		case WM_MBUTTONUP:
			return handle_mouse_up_(l_data, MouseButton::Middle);
		case WM_RBUTTONUP:
			return handle_mouse_up_(l_data, MouseButton::Right);
		case WM_XBUTTONUP:
			return handle_mouse_up_(l_data, x_button_from_w_data(w_data));
		case WM_CHAR:
			return handle_character_input_(w_data, l_data);
		case WM_KEYDOWN:
			return handle_key_down_(w_data, l_data);
		case WM_KEYUP:
			return handle_key_up_(w_data);
		case WM_SIZE:
			return handle_size_change_(w_data, l_data);
		case WM_SETFOCUS:
			return handle_focus_gain_();
		case WM_KILLFOCUS:
			return handle_focus_lose_();
		case WM_DESTROY:
			return handle_closed_();
		}
		return std::nullopt;
	}

	::LRESULT handle_min_max_size_query(::LPARAM const l_data) 
	{
		if (auto const sizes = min_max_size_.load();
			sizes.max > math::Size<Pixels>{})
		{
			auto* const info = reinterpret_cast<::LPMINMAXINFO>(l_data);

			if (sizes.min > math::Size<Pixels>{}) 
			{
				auto const adjusted = window_rectangle_from_client_size(handle_, sizes.min);
				info->ptMinTrackSize = ::POINT{adjusted.width(), adjusted.height()};
			}
			else {
				info->ptMinTrackSize = ::POINT{};
			}

			auto const adjusted = window_rectangle_from_client_size(handle_, sizes.max);
			info->ptMaxTrackSize = ::POINT{adjusted.width(), adjusted.height()};
		}
		
		return {};
	}
	::LRESULT handle_dpi_change_(::WPARAM const w_data, ::LPARAM const l_data)
	{
		channel_.send(event::DpiChange{.dpi{static_cast<float>(HIWORD(w_data))}});

		auto const new_rectangle = reinterpret_cast<RECT const*>(l_data);
		::SetWindowPos(
			handle_, nullptr,
			new_rectangle->left,
			new_rectangle->top,
			new_rectangle->right - new_rectangle->left,
			new_rectangle->bottom - new_rectangle->top,
			SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER
		);
		return {};
	}
	::LRESULT handle_mouse_move_(::LPARAM const l_data) 
	{
		if (not is_mouse_hovering_)
		{
			is_mouse_hovering_ = true;

			auto track_parameters = ::TRACKMOUSEEVENT{
				.cbSize{sizeof(::TRACKMOUSEEVENT)},
				.dwFlags{TME_LEAVE},
				.hwndTrack = handle_,
			};
			::TrackMouseEvent(&track_parameters);
		}

		auto const new_position = math::Point{GET_X_LPARAM(l_data), GET_Y_LPARAM(l_data)};
		
		if (new_position != mouse_position_) 
		{
			channel_.send(event::MouseMove{
				.position{unit_converter_.pixels_to_dip(new_position)},
				.movement{unit_converter_.pixels_to_dip(new_position - mouse_position_)}
			});

			mouse_position_ = new_position;
		}
		
		return {};
	}
	::LRESULT handle_mouse_scroll_(::WPARAM const w_data)
	{
		channel_.send(event::MouseScroll{
			.position{unit_converter_.pixels_to_dip(mouse_position_)},
			.scroll_delta{GET_WHEEL_DELTA_WPARAM(w_data)/120.f}
		});

		return {};
	}
	
	::LRESULT handle_mouse_leave_()
	{
		is_mouse_hovering_ = false;

		auto const new_position = get_window_mouse_position(handle_);

		channel_.send(event::MouseLeave{
			.position{unit_converter_.pixels_to_dip(new_position)},
			.movement{unit_converter_.pixels_to_dip(new_position - mouse_position_)}
		});
		
		return {};
	}
	::LRESULT handle_mouse_down_(::LPARAM const l_data, MouseButton const button, bool const is_double_click)
	{
		channel_.send(event::MouseDown{
			.position{unit_converter_.pixels_to_dip(math::Point{GET_X_LPARAM(l_data), GET_Y_LPARAM(l_data)})},
			.button{button},
			.is_double_click{is_double_click}
		});
		return {};
	}
	::LRESULT handle_mouse_up_(::LPARAM const l_data, MouseButton const button)
	{
		channel_.send(event::MouseUp{
			.position{unit_converter_.pixels_to_dip(math::Point{GET_X_LPARAM(l_data), GET_Y_LPARAM(l_data)})},
			.button{button},
		});
		return {};
	}

	::LRESULT handle_character_input_(::WPARAM const w_data, ::LPARAM const l_data)
	{
		// The size is 5 because 4 is the maximum number of code points in a UTF-8 encoded character,
		// and the null terminator is included.
		std::array<char, 5> character;

		auto const length = unicode::utf16_to_utf8(reinterpret_cast<char16_t const*>(&w_data), character);

		if (length && *length) {
			channel_.send(event::CharacterInput{
				.character{character.data(), *length},
				.is_repeated{get_is_key_repeated(l_data)}
			});
		}

		return {};
	}
	::LRESULT handle_key_down_(::WPARAM const w_data, ::LPARAM const l_data)
	{
		channel_.send(event::KeyDown{
			.key{native_key_map.find_or(static_cast<int>(w_data), KeyboardKey::None)},
			.is_repeated{get_is_key_repeated(l_data)}
		});
		return {};
	}
	::LRESULT handle_key_up_(::WPARAM const w_data)
	{
		channel_.send(event::KeyUp{native_key_map.find_or(static_cast<int>(w_data), KeyboardKey::None)});
		return {};
	}

	::LRESULT handle_size_change_(::WPARAM const w_data, ::LPARAM const l_data)
	{
		if (w_data == SIZE_MINIMIZED)
		{
			state_ = State::Minimized;
			channel_.send(event::StateChange{state_});
		}
		else {
			if (w_data == SIZE_MAXIMIZED) 
			{
				state_ = State::Maximized;
				channel_.send(event::StateChange{state_});
			}
			else if (w_data == SIZE_RESTORED && state_ != State::Restored)
			{
				state_ = State::Restored;
				channel_.send(event::StateChange{state_});
			}

			channel_.send(event::SizeChange{
				unit_converter_.pixels_to_dip(math::Size{static_cast<int>(LOWORD(l_data)), static_cast<int>(HIWORD(l_data))})
			});
		}

		return {};
	}

	::LRESULT handle_focus_gain_() {
		channel_.send(event::FocusGain{});
		return {};
	}
	::LRESULT handle_focus_lose_() {
		channel_.send(event::FocusLose{});
		return {};
	}

	::LRESULT handle_closed_() 
	{
		channel_.send(event::Closed{});
		
		PostQuitMessage(0);
		return {};
	}

	//------------------------------

	void run_(Parameters const& parameters) 
	{
		create_window_(parameters);
		run_event_loop_();
	}

	void create_window_(Parameters const& parameters) 
	{
		initialize_dpi_();

		min_max_size_ = unit_converter_.dip_to_pixels(parameters.min_max_size);
		
		auto const styles = style_flags_to_native(parameters.style, parameters.parent);

		auto const pixel_size = unit_converter_.dip_to_pixels(parameters.size);
		auto const window_rect = window_rectangle_from_client_size(styles, pixel_size);

		auto const parent = parameters.parent ? std::any_cast<::HWND>(parameters.parent->native_handle()) : ::HWND{};

		auto const window_position = position_from_parent_relative_factor(parameters.position_factor, pixel_size, window_rect, parent);

		auto const wide_title = unicode::utf8_to_utf16(parameters.title);

		::CreateWindow(
			WindowClass::class_name.data(),
			reinterpret_cast<::LPCWSTR>(wide_title.data()),
			styles,
			window_position.x, window_position.y,
			window_rect.width(), window_rect.height(),
			parent,
			nullptr,
			get_instance_handle(),
			this // Additional window data, the WindowThread instance.
		);

		window_created_flag_.test_and_set();
		window_created_flag_.notify_one();
	}

	void run_event_loop_() 
	{
		::MSG message;

		while (::GetMessage(&message, nullptr, ::UINT{}, ::UINT{}))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void initialize_dpi_() 
	{
		::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		unit_converter_ = ScreenUnitConverter{static_cast<float>(GetDpiForSystem())};
	}

	ScreenUnitConverter unit_converter_{};

	// Used to provide mouse movement for events.
	math::Point<Pixels> mouse_position_{};

	// Used to tell when to call ::TrackMouseEvent to receive WM_MOUSELEAVE when the mouse leaves the window.
	bool is_mouse_hovering_{};

	// Used to tell when a WM_SIZE message indicates that the window was restored and not just resized.
	State state_{State::Restored};

	// Used to respond to WM_GETMINMAXINFO. It's atomic because it is set by user code.
	std::atomic<MinMaxSize<Pixels>> min_max_size_{};

	WindowClass window_class_;
	::HWND handle_{};
	std::atomic_flag window_created_flag_{};

	concurrency::Sender<Event> channel_;
	std::jthread thread_;
};

} // namespace win

//------------------------------

math::Point<Pixels> get_mouse_position() 
{
	::POINT result;
	::GetCursorPos(&result);
	return {result.x, result.y};
}

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
	void title(std::string_view const title) {
		win::set_window_title(window_thread_.get_handle(), title);
	}
	[[nodiscard]]
	std::string title() const {
		return win::get_window_title(window_thread_.get_handle());
	}

	bool toggle_fullscreen() {
		return fullscreen_toggle_.toggle();
	}
	[[nodiscard]]
	bool is_fullscreen() const {
		return fullscreen_toggle_.is_fullscreen();
	}

	void position(math::Point<Pixels> const position) {
		win::set_window_position(window_thread_.get_handle(), position);
	}
	
	void min_max_size(MinMaxSize<Dip> const sizes) {
		window_thread_.set_min_max_size(sizes);
	}
	[[nodiscard]]
	MinMaxSize<Dip> min_max_size() const {
		return window_thread_.get_min_max_size();
	}

	void min_size(math::Size<Dip> const size) {
		window_thread_.set_min_size(size);
	}
	[[nodiscard]]
	math::Size<Dip> min_size() const {
		return window_thread_.get_min_max_size().min;
	}

	void max_size(math::Size<Dip> const size) {
		window_thread_.set_max_size(size);
	}
	[[nodiscard]]
	math::Size<Dip> max_size() const {
		return window_thread_.get_min_max_size().max;
	}

	void size(math::Size<Dip> const size) {
		win::set_window_size(window_thread_.get_handle(), ScreenUnitConverter{dpi_}.dip_to_pixels(size));
	}
	[[nodiscard]]
	math::Size<Dip> size() const {
		return size_;
	}

	[[nodiscard]]
	bool is_open() const {
		return is_open_;
	}

	[[nodiscard]]
	float dpi() const {
		return dpi_;
	}

	[[nodiscard]]
	std::any native_handle() const {
		return window_thread_.get_handle();
	}

	[[nodiscard]]
	Event await_event() 
	{
		auto event = channel_.receive();

		if (auto const dpi_event = std::get_if<event::DpiChange>(&event))
		{
			dpi_ = dpi_event->dpi;
		}
		else if (auto const size_event = std::get_if<event::SizeChange>(&event))
		{
			size_ = size_event->size;
		}
		else if (std::holds_alternative<event::Closed>(event))
		{
			is_open_ = false;
		}

		return event;
	}

	[[nodiscard]]
	std::optional<Event> take_event() 
	{
		if (not channel_.was_queue_recently_empty()) {
			return await_event();
		}
		return std::nullopt;
	}

	static constexpr auto max_queue_size = std::size_t{128};

	Implementation(
		Parameters const& parameters, 
		concurrency::Channel<Event> channel = concurrency::create_channel<Event>(max_queue_size)
	) : 
		size_{parameters.size},
		channel_{std::move(channel.receiver)},
		window_thread_{win::WindowThread{parameters, std::move(channel.sender)}}
	{
		window_thread_.wait_until_window_created();
		fullscreen_toggle_ = win::FullscreenToggle{window_thread_.get_handle()};
	}

private:
	float dpi_{ScreenUnitConverter::normal_dpi};
	math::Size<Dip> size_;
	bool is_open_{true};
	win::FullscreenToggle fullscreen_toggle_;

	concurrency::Receiver<Event> channel_;
	win::WindowThread window_thread_;
};

} // namespace avo::window
