// Included exactly once.

#include "common.hpp"

#include "avo/concurrency.hpp"
#include "avo/utils/static_map.hpp"
#include "avo/window.hpp"

#include <functional>
#include <thread>

//------------------------------

namespace avo::window {

namespace win {

[[nodiscard]]
bool get_is_vkey_down(int const vkey) {
	return GetAsyncKeyState(vkey) & (1 << 16);
}

//------------------------------

[[nodiscard]]
ModifierKeyFlags modifier_key_flags_from_native(DWORD const native) noexcept 
{
	using enum ModifierKeyFlags;
	
	auto result = None;

	if (native & MK_CONTROL) {
		result |= Control;
	}
	if (native & MK_SHIFT) {
		result |= Shift;
	}
	if (native & MK_LBUTTON) {
		result |= LeftMouse;
	}
	if (native & MK_MBUTTON) {
		result |= MiddleMouse;
	}
	if (native & MK_RBUTTON) {
		result |= RightMouse;
	}
	if (native & MK_XBUTTON1) {
		result |= X0Mouse;
	}
	if (native & MK_XBUTTON2) {
		result |= X1Mouse;
	}
	if (get_is_vkey_down(VK_MENU)) {
		result |= Alt;
	}

	return result;
}

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

struct NativeEvent {
	UINT message;
	WPARAM w_data;
	LPARAM l_data;
};

//------------------------------

class WindowThread {
	static WindowThread* instance_from_event(HWND const handle, UINT const message, LPARAM const l_data) 
	{
		if (message == WM_CREATE) 
		{
			auto const window = static_cast<WindowThread*>(reinterpret_cast<CREATESTRUCT*>(l_data)->lpCreateParams);
			SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			window->set_window_handle(handle);
			return window;
		}
		else {
			return reinterpret_cast<WindowThread*>(GetWindowLongPtr(handle, GWLP_USERDATA));
		}
	}
	
	static LRESULT CALLBACK handle_any_window_event(
		HWND const window_handle, UINT const message, WPARAM const w_data, LPARAM const l_data)
	{
		if (auto const instance = instance_from_event(window_handle, message, l_data))
		{
			if (auto const result = instance->handle_event(message, w_data, l_data))
			{
				return *result;
			}
		}
		return DefWindowProc(window_handle, message, w_data, l_data);
	}
	
public:
	void set_window_handle(HWND const handle) {
		handle_ = handle;
	}

	std::optional<LRESULT> handle_event(UINT const message, WPARAM const w_data, LPARAM const l_data)
	{
		channel_.send(message, w_data, l_data);

		switch (message) {
		case WM_CREATE:
			
			return 0;
		}
		return std::nullopt;
	}

	WindowThread(Parameters const& parameters, concurrency::Sender<NativeEvent> channel) :
		channel_{std::move(channel)},
		thread_{run_, this, parameters}
	{}

private:
	void run_(Parameters const& parameters) {

	}

	HWND handle_;
	concurrency::Sender<NativeEvent> channel_;
	std::jthread thread_;
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
		window_thread_{parameters}
	{

	}

private:
	Event event_from_native(win::NativeEvent event) {
		switch (event.message) {
		case WM_MOUSEMOVE: {
			auto const pos = unit_converter_.pixels_to_dip(math::Point{GET_X_LPARAM(event.l_data), GET_Y_LPARAM(event.l_data)})
			
			
			return event::MouseMove{
				.position{pos},
				.movement{}
			}
		}
			// return mouse_move_from_native_event(event);
		}
	}

	concurrency::Receiver<win::NativeEvent> channel_;
	win::WindowThread window_thread_;
};

} // namespace avo::window
