/*
MIT License

Copyright (c) 2020 Björn Sundin

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

#include <AvoGUI.hpp>

#include <Font data.hpp>

//------------------------------

#include <array>
#include <stack>
#include <random>
#include <ctime>

#if __has_include("filesystem")
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif

//------------------------------

#ifdef _WIN32
#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>
#undef DrawTextW
#undef min
#undef max

#include <windowsx.h>
#include <ShObjIdl.h>
#include <ShlObj_core.h>
#include <Shlwapi.h>
#include <dwmapi.h>

#include <d2d1effects.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <comdef.h>
#endif

//------------------------------

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <libpng/png.h>
#include <jpeglib.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <unistd.h>
#include <cstdio>
#include <fstream>

#undef None
#endif

//------------------------------

using Avo::Index;
using Avo::Count;
using Avo::Range;
using Avo::Indices;

//------------------------------

namespace Console {

//
// Private
//

auto Console::readString(std::string& p_string) -> void
{
#ifdef _WIN32
	// The only way to read unicode characters from the console on Windows is to read UTF-16 in a wide string.
	wchar_t buffer[1 << 10]; // We don't know before how much is going to be read, so we use a large enough buffer (1kb).
	DWORD buffer_size;
	ReadConsoleW(_inputHandle, buffer, std::size(buffer), &buffer_size, nullptr);
	buffer_size -= 2; // remove \r\n return characters

	// Because we use utf-8, convert it from UTF-16 to UTF-8 and store that in the output.
	auto outputSize = WideCharToMultiByte(CP_UTF8, 0, buffer, buffer_size, 0, 0, 0, 0);
	p_string.resize(outputSize);
	WideCharToMultiByte(CP_UTF8, 0, buffer, buffer_size, p_string.data(), outputSize, 0, 0);
#else
	// Most other platforms use UTF-8 by default.
	std::cin >> p_string;
#endif
}

//
// Public
//

Console::Console()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	_inputHandle = GetStdHandle(STD_INPUT_HANDLE);
#endif
}

} // namespace Console

//------------------------------

auto randomEngine = std::mt19937_64{static_cast<unsigned long long>(time(nullptr))};
auto uniformDistribution = std::uniform_real_distribution{0.l, 1.l};
auto normalDistribution = std::normal_distribution{0.l, 1.l};

auto Avo::random() -> long double
{
	return uniformDistribution(randomEngine);
}
auto Avo::random_normal() -> long double
{
	return normalDistribution(randomEngine);
}

//------------------------------

auto Avo::convert_utf8_to_utf16(std::string_view const p_input, Range<char16*> const p_output) -> void
{
#ifdef _WIN32
	auto const length = MultiByteToWideChar(
		CP_UTF8, 0, 
		p_input.data(), p_input.size(), 
		reinterpret_cast<wchar_t*>(p_output.data()), p_output.size()
	);
	p_output[length] = 0;
#endif
}
auto Avo::convert_utf8_to_utf16(std::string_view const p_input) -> std::u16string
{
#ifdef _WIN32
	auto result = std::u16string(MultiByteToWideChar(
		CP_UTF8, 0, 
		p_input.data(), p_input.size(), 
		0, 0
	), '\0');
	MultiByteToWideChar(
		CP_UTF8, 0, 
		p_input.data(), p_input.size(), 
		reinterpret_cast<wchar_t*>(result.data()), result.size()
	);
	return result;
#endif
}
auto Avo::get_number_of_units_in_utf_converted_string(std::string_view const p_input) -> Count
{
#ifdef _WIN32
	return MultiByteToWideChar(CP_UTF8, 0, p_input.data(), p_input.size(), 0, 0);
#endif
}

auto Avo::convert_utf16_to_utf8(std::u16string_view const p_input, Range<char8*> const p_output) -> void
{
#ifdef _WIN32
	auto const length = WideCharToMultiByte(
		CP_UTF8, 0, 
		reinterpret_cast<wchar_t const*>(p_input.data()), p_input.size(), 
		p_output.data(), p_output.size(), 
		0, 0
	);
	p_output[length] = 0;
#endif
}
auto Avo::convert_utf16_to_utf8(std::u16string_view const p_input) -> std::string
{
#ifdef _WIN32
	auto result = std::string(WideCharToMultiByte(
		CP_UTF8, 0, 
		reinterpret_cast<wchar_t const*>(p_input.data()), p_input.size(), 
		0, 0, 0, 0
	), '\0');
	WideCharToMultiByte(
		CP_UTF8, 0, 
		reinterpret_cast<wchar_t const*>(p_input.data()), p_input.size(), 
		result.data(), result.size(), 
		0, 0
	);
	return result;
#endif
}
auto Avo::get_number_of_units_in_utf_converted_string(std::u16string_view const p_input) -> Count
{
#ifdef _WIN32
	return WideCharToMultiByte(
		CP_UTF8, 0, 
		reinterpret_cast<wchar_t const*>(p_input.data()), p_input.size(), 
		0, 0, 0, 0
	);
#endif
}

//------------------------------
// class Avo::Animation
//------------------------------

auto Avo::Animation::queue_update() -> void {
	if (!_is_in_update_queue) {
		_gui->_animationUpdateQueue.emplace_back(this);
		_is_in_update_queue = true;
	}
}

auto Avo::Animation::cancel_all_updates() -> void {
	_are_updates_cancelled = true;

	// The animation might currently be in the animation update queue.
	auto& queue = _gui->_animationUpdateQueue;

	if (auto const position = std::find(queue.begin(), queue.end(), this);
		position != queue.end())
	{
		queue.erase(position);
	}
}

//------------------------------
// class Id
//------------------------------

Avo::Id::ValueType Avo::Id::s_counter = {};

//------------------------------
// class Avo::View
//------------------------------

//
// Private
//

auto Avo::View::calculate_absolute_position_relative_to(Avo::Point<> p_position) const -> Avo::Point<>
{
	auto container = get_parent<View>();
	while (container && container != get_gui())
	{
		p_position += container->get_top_left();
		container = container->get_parent<View>();
	}

	return p_position;
}

auto Avo::View::update_shadow() -> void
{
	constexpr auto minElevation = 0.00001f;
	constexpr auto maxElevation = 400.f;
	if (get_size() >= 1.f && _has_shadow && _elevation > minElevation && _elevation < maxElevation)
	{
		_shadow_image = get_drawing_context()->createRectangleShadowImage(get_size(), _corners, _elevation, get_theme_color(ThemeColors::shadow));
		_shadow_bounds = {
			Avo::Point{
				0.5f*(_bounds.right - _bounds.left - _shadow_image.get_width()),
				0.35f*(_bounds.bottom - _bounds.top - _shadow_image.get_height())
			}, _shadow_image.get_size()
		};
		_shadow_image.set_top_left(_shadow_bounds.get_top_left());
	}
	else
	{
		_shadow_bounds = _bounds.get_size();
	}
}

auto Avo::View::send_bounds_change_events(Avo::Rectangle<> const p_previousBounds) -> void
{
	if (p_previousBounds != _bounds)
	{
		if (auto const previousSize = p_previousBounds.get_size();
		    fabsf(previousSize.width - _bounds.get_width()) > 0.001f || 
			fabsf(previousSize.height - _bounds.get_height()) > 0.001f)
		{
			update_shadow(); // This is to update the shadow bounds and image.

			size_change_listeners(previousSize);

			update_clip_geometry();
		}

		if (this != get_gui() && get_parent<View>())
		{
			auto const mousePosition = get_gui()->get_window()->get_mouse_position() - get_parent<View>()->get_absolute_top_left();
			if (get_is_containing(mousePosition) != p_previousBounds.get_is_containing(mousePosition))
			{
				auto event = Avo::MouseEvent{};
				event.xy = mousePosition + get_parent<View>()->get_absolute_top_left();
				get_gui()->handle_global_mouse_move(event);
			}
		}

		boundsChangeListeners(p_previousBounds);
	}
}

//
// Protected
//

auto Avo::View::update_clip_geometry() -> void
{
	if (getHasCornerStyles())
	{
		_clip_geometry = get_gui()->get_drawing_context()->create_corner_rectangle_geometry(get_size(), _corners);
	}
}

//
// Public
//

Avo::View::View(Avo::View* const p_parent, Avo::Rectangle<> const p_bounds) :
	Component{p_parent},
	ProtectedRectangle{p_bounds},
	_shadow_bounds{p_bounds}
{
	mouse_down_listeners += Avo::bind(&View::handle_mouse_down, this);
	mouse_up_listeners += Avo::bind(&View::handle_mouse_up, this);
	mouse_scroll_listeners += Avo::bind(&View::handle_mouse_scroll, this);
	mouse_move_listeners += Avo::bind(&View::handle_mouse_move, this);
	mouse_enter_listeners += Avo::bind(&View::handle_mouse_enter, this);
	mouse_leave_listeners += Avo::bind(&View::handle_mouse_leave, this);
	mouse_background_enter_listeners += Avo::bind(&View::handle_mouse_background_enter, this);
	mouse_background_leave_listeners += Avo::bind(&View::handle_mouse_background_leave, this);

	dragDropEnterListeners += Avo::bind(&View::handle_drag_drop_enter, this);
	dragDropLeaveListeners += Avo::bind(&View::handle_drag_drop_leave, this);
	dragDropBackgroundEnterListeners += Avo::bind(&View::handle_drag_drop_background_enter, this);
	dragDropBackgroundLeaveListeners += Avo::bind(&View::handle_drag_drop_background_leave, this);
	dragDropMoveListeners += Avo::bind(&View::handle_drag_drop_move, this);
	drag_drop_finish_listeners += Avo::bind(&View::handle_drag_drop_finish, this);

	characterInputListeners += Avo::bind(&View::handle_character_input, this);
	keyboard_key_down_listeners += Avo::bind(&View::handle_keyboard_key_down, this);
	keyboard_key_up_listeners += Avo::bind(&View::handleKeyboardKeyUp, this);
	keyboard_focus_lose_listeners += Avo::bind(&View::handleKeyboardFocusLose, this);
	keyboard_focus_gain_listeners += Avo::bind(&View::handle_keyboard_focus_gain, this);

	themeColorChangeListeners += Avo::bind(&View::handle_theme_color_change, this);
	themeEasingChangeListeners += Avo::bind(&View::handleThemeEasingChange, this);
	themeValueChangeListeners += Avo::bind(&View::handle_theme_value_change, this);

	boundsChangeListeners += Avo::bind(&View::handle_bounds_change, this);
	size_change_listeners += Avo::bind((void (View::*)(Avo::Size<>))&View::handle_size_change, this);
	child_view_attachment_listeners += Avo::bind(&View::handle_child_view_attachment, this);
	child_view_detachment_listeners += Avo::bind(&View::handle_child_view_detachment, this);

	if (p_parent && p_parent != this) {
		set_parent(p_parent);

		_gui = _parent->get_gui();

		_theme = _parent->_theme;
	}
	else {
		_theme = std::make_shared<Theme>();
	}
}
Avo::View::View(Avo::View* const p_parent, Avo::Id const p_id, Avo::Rectangle<> const p_bounds) :
	View{p_parent, p_bounds}
{
	set_id(p_id, get_gui());
}

//------------------------------

auto Avo::View::get_drawing_context() const -> Avo::DrawingContext* {
	return _gui->get_drawing_context();
}
auto Avo::View::get_window() const -> Avo::Window* {
	return _gui->get_window();
}

//------------------------------

auto Avo::View::set_has_shadow(bool const p_hasShadow) -> void {
	if (_has_shadow != p_hasShadow) {
		_has_shadow = p_hasShadow;
		if (_has_shadow) {
			update_shadow();
		}
		else if (_shadow_image) {
			_shadow_image.destroy();
			_shadow_bounds = _bounds.get_size();
		}
	}
}

//------------------------------

auto Avo::View::queue_animation_update() -> void {
	if (!_isInAnimationUpdateQueue && _gui && _is_visible) {
		_gui->_viewAnimationUpdateQueue.push_back(this);

		_isInAnimationUpdateQueue = true;
	}
}

//------------------------------

auto Avo::View::handle_mouse_background_enter(MouseEvent const& p_event) -> void {
	get_gui()->get_window()->set_cursor(_cursor);
}

//------------------------------

auto Avo::View::invalidate() -> void {
	if (_gui) {
		_gui->invalidate();
	}
}

auto Avo::View::drawShadow(DrawingContext* const p_drawingContext) -> void {
	if (_shadow_image && _has_shadow) {
		p_drawingContext->set_color(Color{1.f});
		p_drawingContext->drawImage(_shadow_image, _opacity);
	}
}

//------------------------------

#ifdef _WIN32

/*
	This macro is used to more easily implement COM interfaces.
*/
#define IUnknownDefinition(p_interfaceName)\
private:\
	ULONG _referenceCount = 1ul;\
public:\
	auto __stdcall AddRef() -> ULONG override\
	{\
		return InterlockedIncrement(&_referenceCount);\
	}\
	auto __stdcall Release() -> ULONG override\
	{\
		auto const referenceCount = InterlockedDecrement(&_referenceCount);\
		if (!referenceCount)\
		{\
			delete this;\
			return 0;\
		}\
		return referenceCount;\
	}\
	auto __stdcall QueryInterface(IID const& p_id, void** const p_object) -> HRESULT override\
	{\
		if (p_id == IID_IUnknown || p_id == __uuidof(p_interfaceName))\
		{\
			*p_object = this;\
			AddRef();\
			return S_OK;\
		}\
		*p_object = 0;\
		return E_NOINTERFACE;\
	}

/*
	Holds a reference to a COM object.
	Essentially a std::shared_ptr but for COM objects.
*/
template<std::derived_from<IUnknown> T>
class ComReference {
	T* _pointer = nullptr;
public:
	operator T* () const {
		return _pointer;
	}
	auto operator&() -> T** {
		return &_pointer;
	}
	auto operator->() const -> T* {
		return _pointer;
	}
	auto operator*() const -> T* {
		return _pointer;
	}
	/*
		Steals our reference to the pointer and returns the raw pointer.
		This object then doesn't reference anything anymore.
	*/
	[[nodiscard]] 
	auto steal() -> T* {
		auto const oldPointer = _pointer;
		_pointer = nullptr;
		return oldPointer;
	}
	template<typename _Target>
	[[nodiscard]] 
	auto as() -> ComReference<_Target> {
		_Target* pointer;
		_pointer->QueryInterface(&pointer);
		return pointer;
	}

	/*
		Adds a reference.
	*/
	auto operator=(T* const pointer) -> auto& {
		if (_pointer) {
			_pointer->Release();
		}
		_pointer = pointer;
		if (_pointer) {
			_pointer->AddRef();
		}
		return *this;
	}
	auto operator=(ComReference<T>&& other) noexcept -> auto& {
		if (_pointer) {
			_pointer->Release();
		}
		_pointer = other._pointer;
		other._pointer = nullptr;
		return *this;
	}
	auto operator=(ComReference<T> const& other) -> auto& {
		if (_pointer) {
			_pointer->Release();
		}
		_pointer = other._pointer;
		if (_pointer) {
			_pointer->AddRef();
		}
		return *this;
	}

	/*
		Transfers ownership.
	*/
	ComReference(T* const pointer) :
		_pointer{pointer}
	{}
	ComReference(ComReference<T>&& other) noexcept :
		_pointer{other._pointer}
	{
		other._pointer = nullptr;
	}
	ComReference(ComReference<T> const& other) :
		_pointer{other._pointer}
	{
		if (_pointer) {
			_pointer->AddRef();
		}
	}
	ComReference() = default;
	~ComReference() {
		if (_pointer) {
			_pointer->Release();
		}
	}
};

template<typename T, typename ... Arguments>
auto make_com_reference(Arguments&& ... arguments) -> ComReference<T> {
	return ComReference<T>{new T{std::forward<Arguments>(arguments)...}};
}

//------------------------------

auto convert_windows_key_state_to_modifier_key_flags(unsigned short const key_state) noexcept -> Avo::ModifierKeyFlags
{
	auto modifier_flags = Avo::ModifierKeyFlags::None;

	if (key_state & MK_CONTROL)
		modifier_flags |= Avo::ModifierKeyFlags::Control;
	if (key_state & MK_SHIFT)
		modifier_flags |= Avo::ModifierKeyFlags::Shift;
	if (key_state & MK_LBUTTON)
		modifier_flags |= Avo::ModifierKeyFlags::LeftMouse;
	if (key_state & MK_MBUTTON)
		modifier_flags |= Avo::ModifierKeyFlags::MiddleMouse;
	if (key_state & MK_RBUTTON)
		modifier_flags |= Avo::ModifierKeyFlags::RightMouse;
	if (key_state & MK_XBUTTON1)
		modifier_flags |= Avo::ModifierKeyFlags::X0Mouse;
	if (key_state & MK_XBUTTON2)
		modifier_flags |= Avo::ModifierKeyFlags::X1Mouse;
	if (GetKeyState(VK_MENU) < 0)
		modifier_flags |= Avo::ModifierKeyFlags::Alt;

	return modifier_flags;
}

//------------------------------

class OleFormatEnumerator : public IEnumFORMATETC {
	IUnknownDefinition(IEnumFORMATETC)

private:
	std::vector<FORMATETC> _formats;
	Index _current_format_index{};

public:
	auto __stdcall Next(
		ULONG const number_of_formats_to_get, 
		FORMATETC* formats, 
		ULONG* const number_of_formats_gotten
	) -> HRESULT override 
	{
		auto number_of_formats_gotten = Count{};
		while (_current_format_index < _formats.size() && number_of_formats_gotten < number_of_formats_to_get) {
			*formats = _formats[_current_format_index];

			++_current_format_index;
			++number_of_formats_gotten;
			++formats;
		}
		if (number_of_formats_gotten) {
			*number_of_formats_gotten = number_of_formats_gotten;
		}
		return number_of_formats_to_get == number_of_formats_gotten ? S_OK : S_FALSE;
	}

	auto __stdcall Skip(ULONG const offset) -> HRESULT override {
		_current_format_index += offset;
		return _current_format_index < _formats.size() ? S_OK : S_FALSE;
	}

	auto __stdcall Reset() -> HRESULT override {
		_current_format_index = {};
		return S_OK;
	}

	auto __stdcall Clone(IEnumFORMATETC** const p_formatEnumerator) -> HRESULT override {
		auto const new_format_enumerator = new OleFormatEnumerator{_formats};
		new_format_enumerator->_current_format_index = _current_format_index;
		*p_formatEnumerator = new_format_enumerator;
		return S_OK;
	}

	//------------------------------
	
	OleFormatEnumerator(Range<FORMATETC*> formats) :
		_formats(formats.size())
	{
		for (auto a : Indices{formats}) {
			_formats[a] = formats[a];
			if (_formats[a].ptd) {
				_formats[a].ptd = static_cast<DVTARGETDEVICE*>(CoTaskMemAlloc(sizeof(DVTARGETDEVICE)));
				*_formats[a].ptd = *formats[a].ptd;
			}
		}
	}
	~OleFormatEnumerator() {
		for (auto& format : _formats) {
			if (format.ptd) {
				CoTaskMemFree(format.ptd);
			}
		}
	}
};

auto operator==(FORMATETC const& a, FORMATETC const& b) noexcept -> bool {
	return a.cfFormat == b.cfFormat &&
	       a.dwAspect == b.dwAspect &&
	       a.tymed & b.tymed &&
	       a.lindex == b.lindex;
}
auto operator!=(FORMATETC const& a, FORMATETC const& b) noexcept -> bool {
	return !(a == b);
}

/*
	Communicates data in drag and drop operations.
*/
class OleDataObject : public IDataObject {
	IUnknownDefinition(IDataObject)

private:
	std::vector<FORMATETC> _formats;
	std::vector<STGMEDIUM> _mediums;

	auto find_format(FORMATETC* const format) const  {
		return std::ranges::find(_formats, *format);
	}

public:
	auto __stdcall SetData(
		FORMATETC* const format, 
		STGMEDIUM* const medium, 
		BOOL const p_willRelease
	) -> HRESULT override {
		if (p_willRelease) {
			if (auto const format_iterator = find_format(format);
			    format_iterator == _formats.end())
			{
				_formats.push_back(*format);
				_mediums.push_back(*medium);
			}
			else {
				auto const index = format_iterator - _formats.begin();
				ReleaseStgMedium(&_mediums[index]);
				_mediums[index] = *medium;
			}
			return S_OK;
		}
		else
		{
			return E_NOTIMPL;
		}
	}
	auto __stdcall QueryGetData(FORMATETC* const format) -> HRESULT override
	{
		return find_format(format) == _formats.end() ? DV_E_FORMATETC : S_OK;
	}
	auto __stdcall GetData(FORMATETC* const format, STGMEDIUM* const medium) -> HRESULT override
	{
		if (auto const format_iterator = find_format(format);
		    format_iterator == _formats.end())
		{
			return DV_E_FORMATETC;
		}
		else
		{
			auto const& found_format = *format_iterator;
			auto const& found_medium = _mediums[format_iterator - _formats.begin()];
			
			medium->pUnkForRelease = nullptr;
			if ((medium->tymed = found_format.tymed) == TYMED_HGLOBAL)
			{
				// Copy memory from medium.hGlobal to medium->hGlobal
				auto const size = GlobalSize(found_medium.hGlobal);
				medium->hGlobal = GlobalAlloc(GMEM_FIXED, size); // Returns pointer
				if (auto const lockedPointer = GlobalLock(found_medium.hGlobal))
				{
					memcpy(medium->hGlobal, lockedPointer, size);
					GlobalUnlock(found_medium.hGlobal);
				}
				else
				{
					// No need to unlock, because it was an unsuccessful lock.
					return E_FAIL;
				}
			}
			else if (medium->tymed == TYMED_ISTREAM) {
				medium->pstm = SHCreateMemStream(nullptr, 0);

				auto stats = STATSTG{};
				found_medium.pstm->Stat(&stats, STATFLAG_NONAME);
				medium->pstm->SetSize(stats.cbSize);

				found_medium.pstm->Seek({}, SEEK_SET, nullptr);
				found_medium.pstm->CopyTo(medium->pstm, stats.cbSize, nullptr, nullptr);

				medium->pstm->Seek({}, SEEK_SET, nullptr);
			}

			return S_OK;
		}
	}
	// Non-allocating version of GetData
	auto __stdcall GetDataHere(FORMATETC* const format, STGMEDIUM* const medium) -> HRESULT override
	{
		if (auto const format_iterator = find_format(format);
		    format_iterator == _formats.end())
		{
			return DV_E_FORMATETC;
		}
		else {
			auto const& found_format = *format_iterator;
			auto const& found_medium = _mediums[format_iterator - _formats.begin()];
			
			medium->pUnkForRelease = nullptr;
			if ((medium->tymed = found_format.tymed) == TYMED_HGLOBAL) {
				// Copy memory from medium.hGlobal to medium->hGlobal
				auto const size = GlobalSize(found_medium.hGlobal);
				memcpy(GlobalLock(medium->hGlobal), GlobalLock(found_medium.hGlobal), size);
				GlobalUnlock(found_medium.hGlobal);
				GlobalUnlock(medium->hGlobal);
			}
			else if (medium->tymed == TYMED_ISTREAM) {
				found_medium.pstm->CopyTo(medium->pstm, ULARGE_INTEGER{.QuadPart = ULONGLONG_MAX}, 0, 0);
			}

			return S_OK;
		}
	}

	auto __stdcall GetCanonicalFormatEtc(FORMATETC*, FORMATETC* const p_formatOut) -> HRESULT override {
		p_formatOut->ptd = nullptr;
		return E_NOTIMPL;
	}
	auto __stdcall EnumFormatEtc(DWORD const p_direction, IEnumFORMATETC** const p_formatEnumerator) -> HRESULT override {
		if (p_direction == DATADIR_GET) {
			*p_formatEnumerator = new OleFormatEnumerator{_formats};
			return S_OK;
		}
		
		// it's DATADIR_SET
		return E_NOTIMPL;
	}

	auto __stdcall DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*) -> HRESULT override {
		return OLE_E_ADVISENOTSUPPORTED;
	}
	auto __stdcall DUnadvise(DWORD) -> HRESULT override {
		return OLE_E_ADVISENOTSUPPORTED;
	}
	auto __stdcall EnumDAdvise(IEnumSTATDATA**) -> HRESULT override {
		return OLE_E_ADVISENOTSUPPORTED;
	}

	//------------------------------

	OleDataObject(Range<FORMATETC*> const formats, Range<STGMEDIUM*> const mediums) :
		_formats(formats.begin(), formats.end()),
		_mediums(mediums.begin(), mediums.end())
	{}
	__stdcall ~OleDataObject() {
		for (auto& medium : _mediums) {
			ReleaseStgMedium(&medium);
		}
	}
};

//------------------------------

class OleDropSource : public IDropSource {
	IUnknownDefinition(IDropSource)

private:
	Avo::Gui* _gui;

	ComReference<IDragSourceHelper> _drag_image_helper;

public:
	auto set_drag_image(
		Avo::Image const& image, 
		Avo::Point<> const cursor_position, 
		IDataObject* const data_object
	) -> void {
		auto const size = p_image.get_original_pixel_size();
		auto const dip_to_pixel_factor = _gui->get_window()->get_dip_to_pixel_factor();

		auto dragImage = SHDRAGIMAGE{
			.sizeDragImage = {static_cast<long>(size.width), static_cast<long>(size.height)},
			.ptOffset = {
				static_cast<long>(cursor_position.x*dip_to_pixel_factor), 
				static_cast<long>(cursor_position.y*dip_to_pixel_factor)
			},
			.hbmpDragImage = static_cast<HBITMAP>(_gui->get_drawing_context()->createNativeImageFromImage(p_image)),
			.crColorKey = RGB(0, 0, 0),
		};

		_drag_image_helper->InitializeFromBitmap(&dragImage, data_object);
	}

	//------------------------------

	auto __stdcall QueryContinueDrag(BOOL const was_escape_pressed, DWORD const key_state) -> HRESULT override {
		if (was_escape_pressed) {
			return DRAGDROP_S_CANCEL;
		}
		if (!(key_state & MK_LBUTTON)) {
			return DRAGDROP_S_DROP;
		}

		return S_OK;
	}

	auto __stdcall GiveFeedback(DWORD const effect) -> HRESULT override {
		auto operation = Avo::DragDropOperation::None;
		switch (effect) {
			case DROPEFFECT_COPY:
				operation = Avo::DragDropOperation::Copy;
				break;
			case DROPEFFECT_MOVE:
				operation = Avo::DragDropOperation::Move;
				break;
			case DROPEFFECT_LINK:
				operation = Avo::DragDropOperation::Link;
		}
		_gui->drag_drop_operation_change_listeners(operation);
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

	OleDropSource(Avo::Gui* const gui) :
		_gui{gui}
	{
		CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_drag_image_helper));
	}
};

//------------------------------

constexpr auto CLIPBOARD_FORMAT_ADDITIONAL_DATA = L"AvoGUI additional data";

class OleClipboardData : public Avo::ClipboardData
{
private:
	ComReference<IDataObject> _data_object;

	std::array<FORMATETC, 80> _ole_formats{};
	Count _number_of_formats{};

	Count _number_of_files{};

	FORMATETC* _file_descriptor_format = nullptr;
	FORMATETC* _item_names_format = nullptr;
	FORMATETC* _text_format = nullptr;
	FORMATETC* additional_data_format = nullptr;

	uint32 _clipboard_format_file_contents;
	uint32 _clipboard_format_file_group_descriptor;
	uint32 _clipboard_format_additional_data;

	auto release_data_object() -> void {
		if (_data_object) {
			_data_object = nullptr;

			_number_of_formats = 0;
			_number_of_files = 0;

			_file_descriptor_format = nullptr;
			_item_names_format = nullptr;
			_text_format = nullptr;
			additional_data_format = nullptr;
		}
		for (auto& medium : _global_data_to_release) {
			GlobalUnlock(medium.hGlobal);
			ReleaseStgMedium(&medium);
		}
		_stream_buffers_to_release.clear();
		_global_data_to_release.clear();
		formats.clear();
	}

public:
	auto set_ole_data_object(IDataObject* const data_object) -> void {
		release_data_object();
		if (_data_object = data_object) {
			auto enumerator = ComReference<IEnumFORMATETC>{};
			_data_object->EnumFormatEtc(DATADIR_GET, &enumerator);

			auto number_of_formats = ULONG{};
			enumerator->Next(80, _ole_formats.data(), &number_of_formats);
			_number_of_formats = number_of_formats;

			for (auto const a : Range{_number_of_formats}) {
				auto const format = &_ole_formats[a];
				auto const format_id = _ole_formats[a].cfFormat;
				if (format_id == CF_HDROP && format->dwAspect == DVASPECT_CONTENT) {
					_item_names_format = format;
				}
				else if (format_id == _clipboard_format_file_group_descriptor) {
					_file_descriptor_format = format;
				}
				else if (format_id == _clipboard_format_file_contents) {
					_number_of_files++;
				}
				else if (format_id == CF_UNICODETEXT) {
					_text_format = format;
				}
				else if (format_id == _clipboard_format_additional_data) {
					additional_data_format = format;
				}
				formats.push_back(format->cfFormat);
			}
		}
	}
	auto getOleDataObject() -> IDataObject* {
		return _data_object;
	}

private:
	std::vector<STGMEDIUM> mutable _global_data_to_release;
	std::vector<Avo::DataVector> mutable _stream_buffers_to_release;
public:
	auto get_data_for_format(Index const format_index) const -> std::span<std::byte const> override {
		switch (_ole_formats[format_index].tymed) {
			case TYMED_HGLOBAL: {
				// GetData does not mutate _ole_formats.
				if (STGMEDIUM medium; _data_object->GetData(const_cast<FORMATETC*>(&_ole_formats[format_index]), &medium) == S_OK)
				{
					_global_data_to_release.push_back(medium);
					return {static_cast<std::byte const*>(GlobalLock(medium.hGlobal)), static_cast<Count>(GlobalSize(medium.hGlobal))};
				}
				break;
			}
			case TYMED_ISTREAM: {
				// GetData does not mutate _ole_formats.
				if (STGMEDIUM medium; _data_object->GetData(const_cast<FORMATETC*>(&_ole_formats[format_index]), &medium) == S_OK)
				{
					STATSTG stats;
					medium.pstm->Stat(&stats, STATFLAG_NONAME);

					auto& buffer = _stream_buffers_to_release.emplace_back(stats.cbSize.QuadPart);
					auto number_of_bytes_read = ULONG{};
					medium.pstm->Seek({0}, SEEK_SET, 0);
					medium.pstm->Read(static_cast<void*>(buffer.data()), stats.cbSize.QuadPart, &number_of_bytes_read);
					ReleaseStgMedium(&medium);

					return {buffer.data(), number_of_bytes_read};
				}
				break;
			}
		}
		return {nullptr, 0};
	}
	auto get_format_name(uint32 const format) const -> std::string override
	{
		wchar_t name[51];
		auto const length = GetClipboardFormatNameW(format, name, 50);
		if (!length) {
			switch (format) {
				case CF_BITMAP:
					return "CF_BITMAP";
				case CF_DIB:
					return "CF_DIB";
				case CF_DIF:
					return "CF_DIF";
				case CF_DSPBITMAP:
					return "CF_DSPBITMAP";
				case CF_DSPENHMETAFILE:
					return "CF_DSPENHMETAFILE";
				case CF_DSPMETAFILEPICT:
					return "CF_DSPMETAFILEPICT";
				case CF_DSPTEXT:
					return "CF_DSPTEXT";
				case CF_ENHMETAFILE:
					return "CF_ENHMETAFILE";
				case CF_GDIOBJFIRST:
					return "CF_GDIOBJFIRST";
				case CF_GDIOBJLAST:
					return "CF_GDIOBJLAST";
				case CF_HDROP:
					return "CF_HDROP";
				case CF_LOCALE:
					return "CF_LOCALE";
				case CF_METAFILEPICT:
					return "CF_METAFILEPICT";
				case CF_OEMTEXT:
					return "CF_OEMTEXT";
				case CF_OWNERDISPLAY:
					return "CF_OWNERDISPLAY";
				case CF_PALETTE:
					return "CF_PALETTE";
				case CF_PENDATA:
					return "CF_PENDATA";
				case CF_PRIVATEFIRST:
					return "CF_PRIVATEFIRST";
				case CF_PRIVATELAST:
					return "CF_PRIVATELAST";
				case CF_RIFF:
					return "CF_RIFF";
				case CF_SYLK:
					return "CF_SYLK";
				case CF_TEXT:
					return "CF_TEXT";
				case CF_TIFF:
					return "CF_TIFF";
				case CF_UNICODETEXT:
					return "CF_UNICODETEXT";
				case CF_WAVE:
					return "CF_WAVE";
			}
			return "Unknown";
		}
		name[50] = 0;
		return Avo::convert_utf16_to_utf8(reinterpret_cast<char16 const*>(name));
	}

private:
	template<typename StringType>
	auto get_string() const -> std::optional<StringType> {
		constexpr bool is_utf8 = std::same_as<StringType, std::string>;

		if (_text_format) {
			if (STGMEDIUM medium; _data_object->GetData(_text_format, &medium) == S_OK) {
				auto const cleanup = Avo::Cleanup{[&] {
					GlobalUnlock(medium.hGlobal);
					ReleaseStgMedium(&medium);
				}};
				if constexpr (is_utf8) {
					return Avo::convert_utf16_to_utf8(static_cast<char16 const*>(GlobalLock(medium.hGlobal)));
				}
				else {
					return static_cast<char16 const*>(GlobalLock(medium.hGlobal));
				}
			}
		}
		
		return {};
	}

public:
	auto get_string() const -> std::optional<std::string> override {
		return get_string<std::string>();
	}
	auto get_utf16_string() const -> std::optional<std::u16string> override {
		return get_string<std::u16string>();
	}
	auto get_has_string() const -> bool override {
		return _text_format;
	}

private:
	template<typename StringType>
	auto get_item_names() const -> std::vector<StringType> {
		if (_item_names_format) {
			std::vector<StringType> itemNames;

			if (auto medium = STGMEDIUM{}; _data_object->GetData(_item_names_format, &medium) == S_OK)
			{
				auto const cleanup_release_medium = Avo::Cleanup{[&] { ReleaseStgMedium(&medium); }};

				if (medium.tymed == TYMED_HGLOBAL) {
					auto const filename_structure = static_cast<DROPFILES const*>(GlobalLock(medium.hGlobal));
					auto const global_memory_lock_guard = Avo::Cleanup{[&] { GlobalUnlock(medium.hGlobal); }};

					// When you have to deal with C in C++...
					auto current_buffer_position = reinterpret_cast<char16 const*>(
						reinterpret_cast<char const*>(filename_structure) + filename_structure->pFiles
					);
					while (*current_buffer_position) {
						if constexpr (std::is_same_v<StringType, std::string>) {
							current_buffer_position += itemNames.emplace_back(Avo::convert_utf16_to_utf8(current_buffer_position)).size() + 1;
						}
						else {
							current_buffer_position += itemNames.emplace_back(current_buffer_position).size() + 1;
						}
					}
				}
			}
			return itemNames;
		}
		return get_file_names<StringType>();
	}

public:
	auto get_item_names() const -> std::vector<std::string> override {
		return get_item_names<std::string>();
	}
	auto get_utf16_item_names() const -> std::vector<std::u16string> override {
		return get_item_names<std::u16string>();
	}
	auto get_number_of_item_names() const -> Count override {
		if (_item_names_format) {
			auto number_of_item_names = Count{};

			if (auto medium = STGMEDIUM{}; _data_object->GetData(_item_names_format, &medium) == S_OK )
			{
				auto const cleanup_release_medium = Avo::Cleanup{[&] { ReleaseStgMedium(&medium); }};

				if (medium.tymed == TYMED_HGLOBAL)
				{
					auto filename_structure = static_cast<DROPFILES const*>(GlobalLock(medium.hGlobal));
					auto const global_memory_lock_guard = Avo::Cleanup{[&] { GlobalUnlock(medium.hGlobal); }};

					auto current_buffer_position = reinterpret_cast<wchar_t const*>(
						reinterpret_cast<char const*>(filename_structure) + filename_structure->pFiles
					);
					while (*current_buffer_position) {
						current_buffer_position += std::wcslen(current_buffer_position) + 1;
					}
				}
			}
			return number_of_item_names;
		}
		return get_number_of_files();
	}

private:
	template<typename StringType>
	auto get_file_names() const -> std::vector<StringType>
	{
		if (_file_descriptor_format) {
			auto file_names = std::vector<StringType>();

			if (auto medium = STGMEDIUM{}; _data_object->GetData(_file_descriptor_format, &medium) == S_OK) {
				Avo::Cleanup const cleanup_release_medium = [&]{ ReleaseStgMedium(&medium); };

				if (medium.tymed == TYMED_HGLOBAL) {
					auto const group_descriptor = static_cast<FILEGROUPDESCRIPTORW*>(GlobalLock(medium.hGlobal));
					Avo::Cleanup const cleanup_unlockGlobalMemory = [&]{ GlobalUnlock(medium.hGlobal); };
					
					file_names.reserve(group_descriptor->cItems);
					for (auto const a : Indices{file_names}) {
						auto const fileNameWideString = reinterpret_cast<char16 const*>(group_descriptor->fgd[a].cFileName);
						if constexpr (std::is_same_v<StringType, std::string>) {
							file_names[a] = Avo::convert_utf16_to_utf8(fileNameWideString);
						}
						else {
							file_names[a] = fileNameWideString;
						}
					}
				}
			}
			return file_names;
		}
		return {};
	}

public:
	auto get_file_names() const -> std::vector<std::string> override {
		return get_file_names<std::string>();
	}
	auto get_utf16_file_names() const -> std::vector<std::u16string> override {
		return get_file_names<std::u16string>();
	}

	auto get_file_contents() const -> std::vector<Avo::DataVector> override {
		if (_file_descriptor_format) {
			auto output = std::vector<Avo::DataVector>();

			for (auto a : Indices{_number_of_formats}) {
				if (_ole_formats[a].cfFormat == _clipboard_format_file_contents) {
					// GetData shouldn't mutate _ole_formats.
					if (auto medium = STGMEDIUM{}; 
					    _data_object->GetData(const_cast<FORMATETC*>(&_ole_formats[a]), &medium) == S_OK)
					{
						Avo::Cleanup cleanup_release_medium = [&]{ ReleaseStgMedium(&medium); };
						
						if (medium.tymed == TYMED_HGLOBAL) {
							auto* const pointer = static_cast<std::byte*>(GlobalLock(medium.hGlobal));
							output.emplace_back(pointer, pointer + GlobalSize(medium.hGlobal));
							GlobalUnlock(medium.hGlobal);
						}
						else if (medium.tymed == TYMED_ISTREAM) {
							auto stats = STATSTG{};
							medium.pstm->Stat(&stats, STATFLAG_NONAME);

							output.emplace_back(stats.cbSize.QuadPart);

							auto buffer_size = ULONG{};
							medium.pstm->Seek({0}, SEEK_SET, 0);
							medium.pstm->Read(output.back().data(), output.size(), &buffer_size);
							if (buffer_size != output.back().size()) {
								output.back().resize(buffer_size);
							}
						}
					}
				}
			}
			return output;
		}
		return {};
	}
	auto get_file_contents(Index p_index) const -> Avo::DataVector override
	{
		if (_file_descriptor_format) {
			auto currentIndex = Index{};
			for (auto const a : Indices{_number_of_formats}) {
				if (_ole_formats[a].cfFormat == _clipboard_format_file_contents) {
					if (currentIndex++ == p_index) {
						// GetData shouldn't mutate.
						if (auto medium = STGMEDIUM{}; _data_object->GetData(const_cast<FORMATETC*>(&_ole_formats[a]), &medium) == S_OK)
						{
							Avo::Cleanup const cleanup_release_medium = [&]{ ReleaseStgMedium(&medium); };

							if (medium.tymed == TYMED_HGLOBAL) {
								auto* const pointer = static_cast<std::byte*>(GlobalLock(medium.hGlobal));
								auto result = Avo::DataVector(pointer, pointer + GlobalSize(medium.hGlobal));
								GlobalUnlock(medium.hGlobal);
								return result;
							}
							else if (medium.tymed == TYMED_ISTREAM) {
								auto stats = STATSTG{};
								medium.pstm->Stat(&stats, STATFLAG_NONAME);

								auto result = Avo::DataVector(stats.cbSize.QuadPart);

								auto buffer_size = ULONG{};
								medium.pstm->Seek({0}, SEEK_SET, 0);
								medium.pstm->Read(result.data(), result.size(), &buffer_size);
								if (buffer_size != result.size()) {
									result.resize(buffer_size);
								}
								
								return result;
							}
						}
					}
				}
			}
		}

		return {};
	}
	auto get_number_of_files() const -> Count override {
		return _number_of_files;
	}

	auto get_additional_data() const -> uint64 override {
		if (!additional_data_format) {
			return 0u;
		}
		
		if (auto medium = STGMEDIUM{}; _data_object->GetData(additional_data_format, &medium) == S_OK) {
			return *static_cast<uint64*>(GlobalLock(medium.hGlobal)); GlobalUnlock(medium.hGlobal);
		}
		return 0u;
	}

	auto get_image() const -> Avo::Image override {
		if (_number_of_files) {
			auto const file = get_file_contents(0);
			return _gui->get_drawing_context()->create_image({file.data(), static_cast<Count>(file.size())});
		}
		return {};
	}

private:
	Avo::Gui* _gui;
public:
	OleClipboardData(Avo::Gui* gui) :
		_gui{gui}
	{
		_clipboard_format_file_contents = RegisterClipboardFormatW(CFSTR_FILECONTENTS);
		_clipboard_format_file_group_descriptor = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
		_clipboard_format_additional_data = RegisterClipboardFormatW(CLIPBOARD_FORMAT_ADDITIONAL_DATA);
	}
	~OleClipboardData() {
		release_data_object();
	}
};

//------------------------------

class OleDropTarget : public IDropTarget {
	IUnknownDefinition(IDropTarget)

private:	
	DWORD _currentEffect;
	auto updateEffect(DWORD* const effect) -> void {
		switch (_gui->get_global_drag_drop_operation(_drag_drop_event)) {
			case Avo::DragDropOperation::Copy:
				*effect = DROPEFFECT_COPY;
				break;
			case Avo::DragDropOperation::Move:
				*effect = DROPEFFECT_MOVE;
				break;
			case Avo::DragDropOperation::Link:
				*effect = DROPEFFECT_LINK;
				break;
			default:
				*effect = DROPEFFECT_NONE;
		}
		_currentEffect = *effect;
	}

	Avo::DragDropEvent _drag_drop_event;
	auto update_event_from_point(POINT& p_point) -> void {
		ScreenToClient(static_cast<HWND>(_gui->get_window()->get_native_handle()), &p_point);

		auto const previousPosition = _drag_drop_event.xy;
		_drag_drop_event.xy = _gui->get_window()->pixels_to_dips(Avo::Point<Avo::Pixels>{p_point.x, p_point.y});
		_drag_drop_event.movement = Avo::Vector2d{_drag_drop_event.xy - previousPosition};
	}

public:
	auto __stdcall DragEnter(
		IDataObject* const data_object, 
		DWORD const key_state, 
		POINTL const mouse_position, 
		DWORD* const effect
	) -> HRESULT {
		_drop_data.set_ole_data_object(data_object);

		auto client_mouse_position = POINT{mouse_position.x, mouse_position.y};
		_drop_image_viewer->DragEnter(_gui->get_window()->get_native_handle<HWND>(), data_object, &client_mouse_position, *effect);
		update_event_from_point(client_mouse_position);

		_drag_drop_event.modifier_keys = convert_windows_key_state_to_modifier_key_flags(key_state);

		auto const lock = _gui->create_thread_lock();
		_gui->handle_global_drag_drop_enter(_drag_drop_event);
		updateEffect(effect);

		return S_OK;
	}
	auto __stdcall DragOver(
		DWORD const key_state, 
		POINTL const mouse_position, 
		DWORD* const effect
	) -> HRESULT {
		auto client_mouse_position = POINT{mouse_position.x, mouse_position.y};
		_drop_image_viewer->DragOver(&client_mouse_position, *effect);
		update_event_from_point(client_mouse_position);

		_drag_drop_event.modifier_keys = convert_windows_key_state_to_modifier_key_flags(key_state);

		auto const lock = _gui->create_thread_lock();
		_gui->handle_global_drag_drop_move(_drag_drop_event);
		updateEffect(effect);

		return S_OK;
	}
	auto __stdcall DragLeave() -> HRESULT {
		_drop_image_viewer->DragLeave();

		if (POINT client_mouse_position; GetCursorPos(&client_mouse_position)) {
			update_event_from_point(client_mouse_position);
		}		

		_gui->lock_threads();
		_gui->handle_global_drag_drop_leave(_drag_drop_event);
		_gui->unlock_threads();
		
		_drop_data.set_ole_data_object(nullptr);

		return S_OK;
	}
	auto __stdcall Drop(
		IDataObject* const data_object, 
		DWORD const key_state, 
		POINTL const mouse_position, 
		DWORD* const effect
	) -> HRESULT
	{
		auto client_mouse_position = POINT{mouse_position.x, mouse_position.y};
		update_event_from_point(client_mouse_position);

		_drag_drop_event.modifier_keys = convert_windows_key_state_to_modifier_key_flags(key_state);

		_gui->lock_threads();
		_gui->handleGlobalDragDropFinish(_drag_drop_event);
		_gui->unlock_threads();

		client_mouse_position = {mouse_position.x, mouse_position.y};
		_drop_image_viewer->Drop(data_object, &client_mouse_position, *effect);

		_drop_data.set_ole_data_object(nullptr);

		return S_OK;
	}

	//------------------------------

private:
	Avo::Gui* _gui;
	OleClipboardData _drop_data;
	ComReference<IDropTargetHelper> _drop_image_viewer;

public:
	OleDropTarget(Avo::Gui* const gui) :
		_gui{gui},
		_drop_data{gui}
	{
		_drop_data.formats.reserve(15);
		_drag_drop_event.data = &_drop_data;

		CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_drop_image_viewer));
	}
};

//------------------------------

constexpr int WM_APP_CHANGE_SIZE = WM_APP;
constexpr int WM_APP_SET_IS_ENABLED = WM_APP + 1;

class WindowsWindow : public Avo::Window {
public:
	static std::atomic<uint32> s_number_of_windows;

private:
	HWND _window_handle = nullptr;
	Avo::WindowStyleFlags _cross_platform_styles;
	uint32 _styles{};

	auto get_has_custom_border() -> bool {
		return static_cast<bool>(_cross_platform_styles & Avo::WindowStyleFlags::CustomBorder);
	}
	auto convertWindowStyleFlagsToWindowsWindowStyleFlags(
		Avo::WindowStyleFlags const p_styleFlags, 
		bool const p_hasParent
	) -> uint32 {
		auto styles = uint32{};

		if (p_hasParent || static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::CustomBorder)) {
			styles |= WS_POPUP;
		}

		if (!static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::Invisible)) {
			styles |= WS_VISIBLE;
		}
		if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::CloseButton) || 
			static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::CustomBorder))
		{
			styles |= WS_CAPTION | WS_SYSMENU;
		}

		if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::Minimized)) {
			styles |= WS_MINIMIZE;
		}
		else if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::Maximized)) {
			styles |= WS_MAXIMIZE;
		}

		if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::MinimizeButton)) {
			styles |= WS_MINIMIZEBOX;
		}
		if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::MaximizeButton)) {
			styles |= WS_MAXIMIZEBOX;
		}
		if (static_cast<bool>(p_styleFlags & Avo::WindowStyleFlags::Resizable)) {
			styles |= WS_THICKFRAME;
		}

		return styles;
	}
	auto convert_windows_data_to_keyboard_key(uint64 const data) -> Avo::KeyboardKey {
		switch (data) {
			case VK_APPS:
				return Avo::KeyboardKey::Menu;
			case VK_BACK:
				return Avo::KeyboardKey::Backspace;
			case VK_CLEAR:
				return Avo::KeyboardKey::Clear;
			case VK_TAB:
				return Avo::KeyboardKey::Tab;
			case VK_RETURN:
				return Avo::KeyboardKey::Return;
			case VK_SHIFT:
				return Avo::KeyboardKey::Shift;
			case VK_CONTROL:
				return Avo::KeyboardKey::Control;
			case VK_MENU:
				return Avo::KeyboardKey::Alt;
			case VK_PAUSE:
				return Avo::KeyboardKey::Pause;
			case VK_CAPITAL:
				return Avo::KeyboardKey::CapsLock;
			case VK_ESCAPE:
				return Avo::KeyboardKey::Escape;
			case VK_SPACE:
				return Avo::KeyboardKey::Spacebar;
			case VK_PRIOR:
				return Avo::KeyboardKey::PageUp;
			case VK_NEXT:
				return Avo::KeyboardKey::PageDown;
			case VK_END:
				return Avo::KeyboardKey::End;
			case VK_HOME:
				return Avo::KeyboardKey::Home;
			case VK_LEFT:
				return Avo::KeyboardKey::Left;
			case VK_RIGHT:
				return Avo::KeyboardKey::Right;
			case VK_UP:
				return Avo::KeyboardKey::Up;
			case VK_DOWN:
				return Avo::KeyboardKey::Down;
			case VK_SNAPSHOT:
				return Avo::KeyboardKey::PrintScreen;
			case VK_INSERT:
				return Avo::KeyboardKey::Insert;
			case VK_DELETE:
				return Avo::KeyboardKey::Delete;
			case VK_HELP:
				return Avo::KeyboardKey::Help;
			case VK_NUMPAD0:
				return Avo::KeyboardKey::Numpad0;
			case VK_NUMPAD1:
				return Avo::KeyboardKey::Numpad1;
			case VK_NUMPAD2:
				return Avo::KeyboardKey::Numpad2;
			case VK_NUMPAD3:
				return Avo::KeyboardKey::Numpad3;
			case VK_NUMPAD4:
				return Avo::KeyboardKey::Numpad4;
			case VK_NUMPAD5:
				return Avo::KeyboardKey::Numpad5;
			case VK_NUMPAD6:
				return Avo::KeyboardKey::Numpad6;
			case VK_NUMPAD7:
				return Avo::KeyboardKey::Numpad7;
			case VK_NUMPAD8:
				return Avo::KeyboardKey::Numpad8;
			case VK_NUMPAD9:
				return Avo::KeyboardKey::Numpad9;
			case VK_ADD:
				return Avo::KeyboardKey::Add;
			case VK_SUBTRACT:
				return Avo::KeyboardKey::Subtract;
			case VK_MULTIPLY:
				return Avo::KeyboardKey::Multiply;
			case VK_DIVIDE:
				return Avo::KeyboardKey::Divide;
			case VK_F1:
				return Avo::KeyboardKey::F1;
			case VK_F2:
				return Avo::KeyboardKey::F2;
			case VK_F3:
				return Avo::KeyboardKey::F3;
			case VK_F4:
				return Avo::KeyboardKey::F4;
			case VK_F5:
				return Avo::KeyboardKey::F5;
			case VK_F6:
				return Avo::KeyboardKey::F6;
			case VK_F7:
				return Avo::KeyboardKey::F7;
			case VK_F8:
				return Avo::KeyboardKey::F8;
			case VK_F9:
				return Avo::KeyboardKey::F9;
			case VK_F10:
				return Avo::KeyboardKey::F10;
			case VK_F11:
				return Avo::KeyboardKey::F11;
			case VK_F12:
				return Avo::KeyboardKey::F12;
			case VK_F13:
				return Avo::KeyboardKey::F13;
			case VK_F14:
				return Avo::KeyboardKey::F14;
			case VK_F15:
				return Avo::KeyboardKey::F15;
			case VK_F16:
				return Avo::KeyboardKey::F16;
			case VK_F17:
				return Avo::KeyboardKey::F17;
			case VK_F18:
				return Avo::KeyboardKey::F18;
			case VK_F19:
				return Avo::KeyboardKey::F19;
			case VK_F20:
				return Avo::KeyboardKey::F20;
			case VK_F21:
				return Avo::KeyboardKey::F21;
			case VK_F22:
				return Avo::KeyboardKey::F22;
			case VK_F23:
				return Avo::KeyboardKey::F23;
			case VK_F24:
				return Avo::KeyboardKey::F24;
			case VK_NUMLOCK:
				return Avo::KeyboardKey::NumLock;
			case 0x30:
				return Avo::KeyboardKey::Number0;
			case 0x31:
				return Avo::KeyboardKey::Number1;
			case 0x32:
				return Avo::KeyboardKey::Number2;
			case 0x33:
				return Avo::KeyboardKey::Number3;
			case 0x34:
				return Avo::KeyboardKey::Number4;
			case 0x35:
				return Avo::KeyboardKey::Number5;
			case 0x36:
				return Avo::KeyboardKey::Number6;
			case 0x37:
				return Avo::KeyboardKey::Number7;
			case 0x38:
				return Avo::KeyboardKey::Number8;
			case 0x39:
				return Avo::KeyboardKey::Number9;
			case 0x41:
				return Avo::KeyboardKey::A;
			case 0x42:
				return Avo::KeyboardKey::B;
			case 0x43:
				return Avo::KeyboardKey::C;
			case 0x44:
				return Avo::KeyboardKey::D;
			case 0x45:
				return Avo::KeyboardKey::E;
			case 0x46:
				return Avo::KeyboardKey::F;
			case 0x47:
				return Avo::KeyboardKey::G;
			case 0x48:
				return Avo::KeyboardKey::H;
			case 0x49:
				return Avo::KeyboardKey::I;
			case 0x4A:
				return Avo::KeyboardKey::J;
			case 0x4B:
				return Avo::KeyboardKey::K;
			case 0x4C:
				return Avo::KeyboardKey::L;
			case 0x4D:
				return Avo::KeyboardKey::M;
			case 0x4E:
				return Avo::KeyboardKey::N;
			case 0x4F:
				return Avo::KeyboardKey::O;
			case 0x50:
				return Avo::KeyboardKey::P;
			case 0x51:
				return Avo::KeyboardKey::Q;
			case 0x52:
				return Avo::KeyboardKey::R;
			case 0x53:
				return Avo::KeyboardKey::S;
			case 0x54:
				return Avo::KeyboardKey::T;
			case 0x55:
				return Avo::KeyboardKey::U;
			case 0x56:
				return Avo::KeyboardKey::V;
			case 0x57:
				return Avo::KeyboardKey::W;
			case 0x58:
				return Avo::KeyboardKey::X;
			case 0x59:
				return Avo::KeyboardKey::Y;
			case 0x5A:
				return Avo::KeyboardKey::Z;
			case VK_OEM_COMMA:
				return Avo::KeyboardKey::Comma;
			case VK_OEM_PERIOD:
				return Avo::KeyboardKey::Period;
			case VK_OEM_PLUS:
				return Avo::KeyboardKey::Plus;
			case VK_OEM_MINUS:
				return Avo::KeyboardKey::Minus;
			case VK_OEM_1:
				return Avo::KeyboardKey::Regional1;
			case VK_OEM_2:
				return Avo::KeyboardKey::Regional2;
			case VK_OEM_3:
				return Avo::KeyboardKey::Regional3;
			case VK_OEM_4:
				return Avo::KeyboardKey::Regional4;
			case VK_OEM_5:
				return Avo::KeyboardKey::Regional5;
			case VK_OEM_6:
				return Avo::KeyboardKey::Regional6;
			case VK_OEM_7:
				return Avo::KeyboardKey::Regional7;
		}
		return Avo::KeyboardKey::None;
	}

	//------------------------------

	inline static auto const WINDOW_CLASS_NAME = L"AvoGUI Window";

	bool _has_created_window = false;
	std::condition_variable _hasCreatedWindowConditionVariable;
	std::mutex _hasCreatedWindowMutex;
	std::thread _messageThread;
	auto thread_createAndRun(
		std::string_view const p_title, 
	    Avo::Point<> const p_position, 
		Avo::Size<> const size, 
	    Avo::Window* const p_parent
	) -> void {
		if (_window_handle) {
			DestroyWindow(_window_handle);
			s_number_of_windows--;
		}
		else if (!s_number_of_windows) {
			auto const windowClass = WNDCLASSW{
				.style = CS_DBLCLKS,
				.lpfnWndProc = handleGlobalEvents,
				.hInstance = GetModuleHandle(0),
				.hIcon = nullptr,
				.hCursor = nullptr,
				.hbrBackground = nullptr,
				.lpszMenuName = nullptr,
				.lpszClassName = WINDOW_CLASS_NAME,
			};
			RegisterClassW(&windowClass);
		}

		//------------------------------

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		_dip_to_pixel_factor = GetDpiForSystem()/(float)USER_DEFAULT_SCREEN_DPI;

		// Calculate nonclient window rectangle from client size.
		auto window_rect = RECT{0, 0, (int)std::ceil(size.x*_dip_to_pixel_factor), (int)std::ceil(size.y*_dip_to_pixel_factor)};
		_size = {window_rect.right, window_rect.bottom}; // Client area
		
		AdjustWindowRect(&window_rect, _styles, 0);

		//------------------------------

		auto cursorPosition = POINT{};
		GetCursorPos(&cursorPosition);
		_mouse_position = {cursorPosition.x, cursorPosition.y};

		auto parentRect = RECT{};

		if (p_parent) {
			GetWindowRect(static_cast<HWND>(p_parent->get_native_handle()), &parentRect);
		}
		else {
			auto const monitor = MonitorFromPoint(cursorPosition, MONITOR_DEFAULTTONEAREST);
			auto monitorInfo = MONITORINFO{};
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &monitorInfo);
			parentRect = monitorInfo.rcWork;
		}

		//------------------------------

		auto const wide_title = Avo::convert_utf8_to_utf16(p_title);

		// _window_handle is initialized by the WM_CREATE event, before CreateWindow returns.

		auto const parentWindowHandle = p_parent ? static_cast<HWND>(p_parent->get_native_handle()) : nullptr;

		if (get_has_custom_border()) {
			CreateWindowW(
				WINDOW_CLASS_NAME,
				reinterpret_cast<LPCWSTR>(wide_title.c_str()),
				_styles,
				0, 0,
				0, 0,
				parentWindowHandle,
				0, // No menu
				GetModuleHandle(0),
				this // Additional window data - the instance
			);

			/*
				Remove border, can't do that just with styles without losing the window shadow.
			*/
			auto const margins = MARGINS{0, 0, 0, 1};
			DwmExtendFrameIntoClientArea(_window_handle, &margins);

			SetWindowPos(
				_window_handle, nullptr,
				parentRect.left + window_rect.left + p_position.x*(parentRect.right - parentRect.left - _size.x),
				parentRect.top + window_rect.top + p_position.y*(parentRect.bottom - parentRect.top - _size.y),
				_size.x,
				_size.y,
				SWP_FRAMECHANGED | SWP_NOZORDER
			);
		}
		else {
			CreateWindowW(
				WINDOW_CLASS_NAME,
				reinterpret_cast<LPCWSTR>(wide_title.c_str()),
				_styles,
				parentRect.left + window_rect.left + p_position.x*(parentRect.right - parentRect.left - window_rect.right + window_rect.left),
				parentRect.top + window_rect.top + p_position.y*(parentRect.bottom - parentRect.top - window_rect.bottom + window_rect.top),
				window_rect.right - window_rect.left,
				window_rect.bottom - window_rect.top,
				parentWindowHandle,
				0, // No menu
				GetModuleHandle(0),
				this // Additional window data - the instance
			);
		}
		s_number_of_windows++;

		if (!_is_running) {
			auto lock = std::unique_lock{_isRunningMutex};
			_isRunningConditionVariable.wait(lock, [=]() -> bool { return _is_running; });
		}

		auto message = MSG{};
		while (GetMessageW(&message, 0, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

private:
	bool _isOpen = false;
public:
	auto getIsOpen() const -> bool override {
		return _isOpen;
	}

	auto create(
		std::string_view const p_title, 
		Avo::Point<> const p_position, 
		Avo::Size<> const size, 
		Avo::WindowStyleFlags const p_styleFlags, 
		Avo::Window* const p_parent
	) -> void override 
	{
		_title = p_title;
		_cross_platform_styles = p_styleFlags;
		_styles = convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styleFlags, p_parent);
		
		_messageThread = std::thread{&WindowsWindow::thread_createAndRun, this, p_title, p_position, size, p_parent};
		if (!_has_created_window)
		{
			auto lock = std::unique_lock{_hasCreatedWindowMutex};
			_hasCreatedWindowConditionVariable.wait(lock, [=]() -> bool { return _has_created_window; });
		}
	}

	auto close() -> void override {
		if (_window_handle) {
			SendMessage(_window_handle, WM_CLOSE, 0, 0);
		}
	}

	//------------------------------

	auto enableUserInteraction() -> void override {
		PostMessage(_window_handle, WM_APP_SET_IS_ENABLED, 1, 0);
	}
	auto disableUserInteraction() -> void override {
		PostMessage(_window_handle, WM_APP_SET_IS_ENABLED, 0, 0);
	}
	auto getIsUserInteractionEnabled() -> bool override {
		return IsWindowEnabled(_window_handle);
	}

	//------------------------------

private:
	std::string _title;
public:
	auto set_title(std::string_view const p_title) -> void override {
		_title = p_title;
		auto const wide_title = Avo::convert_utf8_to_utf16(p_title);
		SetWindowTextW(_window_handle, reinterpret_cast<LPCWSTR>(wide_title.c_str()));
	}
	auto get_title() const -> std::string override {
		return _title;
	}

	//------------------------------

	auto setStyles(Avo::WindowStyleFlags const p_styles) -> void override {
		SetWindowLongPtr(_window_handle, GWL_STYLE, convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styles, GetParent(_window_handle)));
		SetWindowPos(_window_handle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
	auto getStyles() const -> Avo::WindowStyleFlags override {
		return _cross_platform_styles;
	}

	//------------------------------

	// Internal method used to initialize the window handle at the right moment.
	auto set_native_handle(HWND const p_handle) -> void {
		_window_handle = p_handle;
	}
	auto get_native_handle() const -> void* override {
		return _window_handle;
	}

	//------------------------------

private:
	bool _isFullscreen = false;
	RECT _windowRectBeforeFullscreen{};
	bool _wasWindowMaximizedBeforeFullscreen = false;
public:
	auto set_is_fullscreen(bool const p_isFullscreen) -> void override {
		if (_isFullscreen == p_isFullscreen) {
			return;
		}
		if (p_isFullscreen) {
			_wasWindowMaximizedBeforeFullscreen = false;
			if (_state == Avo::WindowState::Restored) {
				GetWindowRect(_window_handle, &_windowRectBeforeFullscreen);
			}
			else if (_state == Avo::WindowState::Maximized) {
				_wasWindowMaximizedBeforeFullscreen = true;
			}

			auto info = MONITORINFO{};
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(_window_handle, MONITOR_DEFAULTTONEAREST), &info);
			SetWindowLongPtr(_window_handle, GWL_STYLE, WS_VISIBLE | WS_MAXIMIZE);
			SetWindowPos(
				_window_handle, 0, 
				info.rcMonitor.left, info.rcMonitor.top, 
				info.rcMonitor.right - info.rcMonitor.left, 
				info.rcMonitor.bottom - info.rcMonitor.top, 
				SWP_NOZORDER | SWP_NOOWNERZORDER
			);
		}
		else {
			SetWindowLongPtr(
				_window_handle, GWL_STYLE, 
				_wasWindowMaximizedBeforeFullscreen*WS_MAXIMIZE | _styles & ~(WS_MAXIMIZE | WS_MINIMIZE)
			);
			if (_wasWindowMaximizedBeforeFullscreen) {
				SetWindowPos(
					_window_handle, 0, 0, 0, 0, 0, 
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
				);
			}
			else {
				SetWindowPos(
					_window_handle, 0, 
					_windowRectBeforeFullscreen.left, _windowRectBeforeFullscreen.top, 
					_windowRectBeforeFullscreen.right - _windowRectBeforeFullscreen.left, 
					_windowRectBeforeFullscreen.bottom - _windowRectBeforeFullscreen.top, 
					SWP_NOZORDER | SWP_NOOWNERZORDER
				);
			}
		}
		_isFullscreen = p_isFullscreen;
	}
	auto switchFullscreen() -> void override
	{
		set_is_fullscreen(!_isFullscreen);
	}
	auto getIsFullscreen() const -> bool override
	{
		return _isFullscreen;
	}

	//------------------------------

private:
	Avo::WindowState _state = Avo::WindowState::Restored;
public:
	auto hide() -> void override {
		ShowWindow(_window_handle, SW_HIDE);
	}
	auto show() -> void override {
		ShowWindow(_window_handle, SW_SHOW);
	}

	auto maximize() -> void override {
		ShowWindow(_window_handle, SW_MAXIMIZE);
	}
	auto minimize() -> void override {
		ShowWindow(_window_handle, SW_MINIMIZE);
	}
	auto restore() -> void override {
		ShowWindow(_window_handle, SW_RESTORE);
	}

	auto setState(Avo::WindowState const p_state) -> void override {
		if (p_state == Avo::WindowState::Maximized)
			ShowWindow(_window_handle, SW_MAXIMIZE);
		else if (p_state == Avo::WindowState::Minimized)
			ShowWindow(_window_handle, SW_MINIMIZE);
		else if (p_state == Avo::WindowState::Restored)
			ShowWindow(_window_handle, SW_RESTORE);
	}
	auto getState() const -> Avo::WindowState override {
		return _state;
	}

	//------------------------------

private:
	Avo::Point<Avo::Pixels> _position;
public:
	auto setPosition(Avo::Point<Avo::Pixels> const p_position) -> void override {
		SetWindowPos(_window_handle, 0, p_position.x, p_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		_position = p_position;
	}
	auto getPosition() const -> Avo::Point<Avo::Pixels> override {
		return _position;
	}
	auto getPositionX() const -> Avo::Pixels override {
		return _position.x;
	}
	auto getPositionY() const -> Avo::Pixels override {
		return _position.y;
	}

private:
	Avo::Size<Avo::Pixels> _size;
public:
	auto set_pixel_size(Avo::Size<Avo::Pixels> const size) -> void override {
		if (_window_handle) {
			if (get_has_custom_border()) {
				PostMessage(_window_handle, WM_APP_CHANGE_SIZE, static_cast<WPARAM>(size.width), static_cast<LPARAM>(size.height));
			}
			else {
				auto window_rect = RECT{0, 0, static_cast<int>(size.width), static_cast<int>(size.height)};
				AdjustWindowRect(&window_rect, _styles, 0);
			
				PostMessage(
					_window_handle, 
					WM_APP_CHANGE_SIZE, 
					static_cast<WPARAM>(window_rect.right - window_rect.left), 
					static_cast<LPARAM>(window_rect.bottom - window_rect.top)
				);
			}
		}
	}
	auto get_pixel_size() const -> Avo::Size<Avo::Pixels> override {
		return _size;
	}
	auto get_pixel_width() const -> Avo::Pixels override {
		return _size.x;
	}
	auto get_pixel_height() const -> Avo::Pixels override {
		return _size.y;
	}

	//------------------------------

private:
	Avo::Size<Avo::Pixels> _min_size;
public:
	auto set_min_pixel_size(Avo::Size<Avo::Pixels> const size) -> void override {
		_min_size = size;
	}
	auto get_min_pixel_size() const -> Avo::Size<Avo::Pixels> override {
		return _min_size;
	}
	auto get_min_pixel_width() const -> Avo::Pixels override {
		return _min_size.x;
	}
	auto get_min_pixel_height() const -> Avo::Pixels override {
		return _min_size.y;
	}

private:
	Avo::Size<Avo::Pixels> _max_size;
public:
	auto set_max_pixel_size(Avo::Size<Avo::Pixels> size) -> void override {
		_max_size = size;
	}
	auto get_max_pixel_size() const -> Avo::Size<Avo::Pixels> override {
		return _max_size;
	}
	auto getMaxPixelWidth() const -> Avo::Pixels override {
		return _max_size.x;
	}
	auto getMaxPixelHeight() const -> Avo::Pixels override {
		return _max_size.y;
	}

	//------------------------------

private:
	auto get_monitor_info() const -> MONITORINFO {
		auto info = MONITORINFO{};
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(_window_handle, MONITOR_DEFAULTTONEAREST), &info);
		return info;
	}

public:
	auto get_monitor_bounds() const -> Avo::Rectangle<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcMonitor;
		return {
			rect.left, rect.top, 
			rect.right, rect.bottom
		};
	}
	auto get_monitor_position() const -> Avo::Point<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcMonitor;
		return {rect.left, rect.top};
	}
	auto get_monitor_size() const -> Avo::Size<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcMonitor;
		return {
			rect.right - rect.left, 
			rect.bottom - rect.top
		};
	}
	auto get_monitor_width() const -> Avo::Pixels override {
		auto const rect = get_monitor_info().rcMonitor;
		return rect.right - rect.left;
	}
	auto get_monitor_height() const -> Avo::Pixels override {
		auto const rect = get_monitor_info().rcMonitor;
		return rect.bottom - rect.top;
	}

	//------------------------------

	auto get_work_area_bounds() const -> Avo::Rectangle<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcWork;
		return {
			rect.left, rect.top, 
			rect.right, rect.bottom 
		};
	}
	auto get_work_area_position() const -> Avo::Point<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcWork;
		return {rect.left, rect.top};
	}
	auto get_work_area_size() const -> Avo::Size<Avo::Pixels> override {
		auto const rect = get_monitor_info().rcWork;
		return {
			rect.right - rect.left, 
			rect.bottom - rect.top
		};
	}
	auto get_work_area_width() const -> Avo::Pixels override {
		auto const rect = get_monitor_info().rcWork;
		return rect.right - rect.left;
	}
	auto get_work_area_height() const -> Avo::Pixels override {
		auto const rect = get_monitor_info().rcWork;
		return rect.bottom - rect.top;
	}

	//------------------------------

private:
	static auto get_is_virtual_key_down(int const p_key) -> bool
	{
		return GetAsyncKeyState(p_key) & (1 << 16);
	}

public:
	auto get_is_key_down(Avo::KeyboardKey const p_key) const -> bool override
	{
		switch (p_key)
		{
			case Avo::KeyboardKey::A:
				return get_is_virtual_key_down(0x41);
			case Avo::KeyboardKey::B:
				return get_is_virtual_key_down(0x42);
			case Avo::KeyboardKey::C:
				return get_is_virtual_key_down(0x43);
			case Avo::KeyboardKey::D:
				return get_is_virtual_key_down(0x44);
			case Avo::KeyboardKey::E:
				return get_is_virtual_key_down(0x45);
			case Avo::KeyboardKey::F:
				return get_is_virtual_key_down(0x46);
			case Avo::KeyboardKey::G:
				return get_is_virtual_key_down(0x47);
			case Avo::KeyboardKey::H:
				return get_is_virtual_key_down(0x48);
			case Avo::KeyboardKey::I:
				return get_is_virtual_key_down(0x49);
			case Avo::KeyboardKey::J:
				return get_is_virtual_key_down(0x4A);
			case Avo::KeyboardKey::K:
				return get_is_virtual_key_down(0x4B);
			case Avo::KeyboardKey::L:
				return get_is_virtual_key_down(0x4C);
			case Avo::KeyboardKey::M:
				return get_is_virtual_key_down(0x4D);
			case Avo::KeyboardKey::N:
				return get_is_virtual_key_down(0x4E);
			case Avo::KeyboardKey::O:
				return get_is_virtual_key_down(0x4F);
			case Avo::KeyboardKey::P:
				return get_is_virtual_key_down(0x50);
			case Avo::KeyboardKey::Q:
				return get_is_virtual_key_down(0x51);
			case Avo::KeyboardKey::R:
				return get_is_virtual_key_down(0x52);
			case Avo::KeyboardKey::S:
				return get_is_virtual_key_down(0x53);
			case Avo::KeyboardKey::T:
				return get_is_virtual_key_down(0x54);
			case Avo::KeyboardKey::U:
				return get_is_virtual_key_down(0x55);
			case Avo::KeyboardKey::V:
				return get_is_virtual_key_down(0x56);
			case Avo::KeyboardKey::W:
				return get_is_virtual_key_down(0x57);
			case Avo::KeyboardKey::X:
				return get_is_virtual_key_down(0x58);
			case Avo::KeyboardKey::Y:
				return get_is_virtual_key_down(0x59);
			case Avo::KeyboardKey::Z:
				return get_is_virtual_key_down(0x5A);
			case Avo::KeyboardKey::Alt:
				return get_is_virtual_key_down(VK_MENU);
			case Avo::KeyboardKey::Backspace:
				return get_is_virtual_key_down(VK_BACK);
			case Avo::KeyboardKey::CapsLock:
				return get_is_virtual_key_down(VK_CAPITAL);
			case Avo::KeyboardKey::Clear:
				return get_is_virtual_key_down(VK_CLEAR);
			case Avo::KeyboardKey::Control:
				return get_is_virtual_key_down(VK_CONTROL);
			case Avo::KeyboardKey::Decimal:
				return get_is_virtual_key_down(VK_DECIMAL);
			case Avo::KeyboardKey::Delete:
				return get_is_virtual_key_down(VK_DELETE);
			case Avo::KeyboardKey::Down:
				return get_is_virtual_key_down(VK_DOWN);
			case Avo::KeyboardKey::End:
				return get_is_virtual_key_down(VK_END);
			case Avo::KeyboardKey::Enter:
				return get_is_virtual_key_down(VK_RETURN);
			case Avo::KeyboardKey::Escape:
				return get_is_virtual_key_down(VK_ESCAPE);
			case Avo::KeyboardKey::F1:
				return get_is_virtual_key_down(VK_F1);
			case Avo::KeyboardKey::F2:
				return get_is_virtual_key_down(VK_F2);
			case Avo::KeyboardKey::F3:
				return get_is_virtual_key_down(VK_F3);
			case Avo::KeyboardKey::F4:
				return get_is_virtual_key_down(VK_F4);
			case Avo::KeyboardKey::F5:
				return get_is_virtual_key_down(VK_F5);
			case Avo::KeyboardKey::F6:
				return get_is_virtual_key_down(VK_F6);
			case Avo::KeyboardKey::F7:
				return get_is_virtual_key_down(VK_F7);
			case Avo::KeyboardKey::F8:
				return get_is_virtual_key_down(VK_F8);
			case Avo::KeyboardKey::F9:
				return get_is_virtual_key_down(VK_F9);
			case Avo::KeyboardKey::F10:
				return get_is_virtual_key_down(VK_F10);
			case Avo::KeyboardKey::F11:
				return get_is_virtual_key_down(VK_F11);
			case Avo::KeyboardKey::F12:
				return get_is_virtual_key_down(VK_F12);
			case Avo::KeyboardKey::F13:
				return get_is_virtual_key_down(VK_F13);
			case Avo::KeyboardKey::F14:
				return get_is_virtual_key_down(VK_F14);
			case Avo::KeyboardKey::F15:
				return get_is_virtual_key_down(VK_F15);
			case Avo::KeyboardKey::F16:
				return get_is_virtual_key_down(VK_F16);
			case Avo::KeyboardKey::F17:
				return get_is_virtual_key_down(VK_F17);
			case Avo::KeyboardKey::F18:
				return get_is_virtual_key_down(VK_F18);
			case Avo::KeyboardKey::F19:
				return get_is_virtual_key_down(VK_F19);
			case Avo::KeyboardKey::F20:
				return get_is_virtual_key_down(VK_F20);
			case Avo::KeyboardKey::F21:
				return get_is_virtual_key_down(VK_F21);
			case Avo::KeyboardKey::F22:
				return get_is_virtual_key_down(VK_F22);
			case Avo::KeyboardKey::F23:
				return get_is_virtual_key_down(VK_F23);
			case Avo::KeyboardKey::F24:
				return get_is_virtual_key_down(VK_F24);
			case Avo::KeyboardKey::Help:
				return get_is_virtual_key_down(VK_HELP);
			case Avo::KeyboardKey::Home:
				return get_is_virtual_key_down(VK_HOME);
			case Avo::KeyboardKey::Insert:
				return get_is_virtual_key_down(VK_INSERT);
			case Avo::KeyboardKey::Left:
				return get_is_virtual_key_down(VK_LEFT);
			case Avo::KeyboardKey::Menu:
				return get_is_virtual_key_down(VK_APPS);
			case Avo::KeyboardKey::Number0:
				return get_is_virtual_key_down(0x30);
			case Avo::KeyboardKey::Number1:
				return get_is_virtual_key_down(0x31);
			case Avo::KeyboardKey::Number2:
				return get_is_virtual_key_down(0x32);
			case Avo::KeyboardKey::Number3:
				return get_is_virtual_key_down(0x33);
			case Avo::KeyboardKey::Number4:
				return get_is_virtual_key_down(0x34);
			case Avo::KeyboardKey::Number5:
				return get_is_virtual_key_down(0x35);
			case Avo::KeyboardKey::Number6:
				return get_is_virtual_key_down(0x36);
			case Avo::KeyboardKey::Number7:
				return get_is_virtual_key_down(0x37);
			case Avo::KeyboardKey::Number8:
				return get_is_virtual_key_down(0x38);
			case Avo::KeyboardKey::Number9:
				return get_is_virtual_key_down(0x39);
			case Avo::KeyboardKey::NumLock:
				return get_is_virtual_key_down(VK_NUMLOCK);
			case Avo::KeyboardKey::Numpad0:
				return get_is_virtual_key_down(VK_NUMPAD0);
			case Avo::KeyboardKey::Numpad1:
				return get_is_virtual_key_down(VK_NUMPAD1);
			case Avo::KeyboardKey::Numpad2:
				return get_is_virtual_key_down(VK_NUMPAD2);
			case Avo::KeyboardKey::Numpad3:
				return get_is_virtual_key_down(VK_NUMPAD3);
			case Avo::KeyboardKey::Numpad4:
				return get_is_virtual_key_down(VK_NUMPAD4);
			case Avo::KeyboardKey::Numpad5:
				return get_is_virtual_key_down(VK_NUMPAD5);
			case Avo::KeyboardKey::Numpad6:
				return get_is_virtual_key_down(VK_NUMPAD6);
			case Avo::KeyboardKey::Numpad7:
				return get_is_virtual_key_down(VK_NUMPAD7);
			case Avo::KeyboardKey::Numpad8:
				return get_is_virtual_key_down(VK_NUMPAD8);
			case Avo::KeyboardKey::Numpad9:
				return get_is_virtual_key_down(VK_NUMPAD9);
			case Avo::KeyboardKey::Add:
				return get_is_virtual_key_down(VK_ADD);
			case Avo::KeyboardKey::Subtract:
				return get_is_virtual_key_down(VK_SUBTRACT);
			case Avo::KeyboardKey::Multiply:
				return get_is_virtual_key_down(VK_MULTIPLY);
			case Avo::KeyboardKey::Divide:
				return get_is_virtual_key_down(VK_DIVIDE);
			case Avo::KeyboardKey::PageDown:
				return get_is_virtual_key_down(VK_NEXT);
			case Avo::KeyboardKey::PageUp:
				return get_is_virtual_key_down(VK_PRIOR);
			case Avo::KeyboardKey::Pause:
				return get_is_virtual_key_down(VK_PAUSE);
			case Avo::KeyboardKey::PrintScreen:
				return get_is_virtual_key_down(VK_SNAPSHOT);
			case Avo::KeyboardKey::Comma:
				return get_is_virtual_key_down(VK_OEM_COMMA);
			case Avo::KeyboardKey::Period:
				return get_is_virtual_key_down(VK_OEM_PERIOD);
			case Avo::KeyboardKey::Plus:
				return get_is_virtual_key_down(VK_OEM_PLUS);
			case Avo::KeyboardKey::Minus:
				return get_is_virtual_key_down(VK_OEM_MINUS);
			case Avo::KeyboardKey::Regional1:
				return get_is_virtual_key_down(VK_OEM_1);
			case Avo::KeyboardKey::Regional2:
				return get_is_virtual_key_down(VK_OEM_2);
			case Avo::KeyboardKey::Regional3:
				return get_is_virtual_key_down(VK_OEM_3);
			case Avo::KeyboardKey::Regional4:
				return get_is_virtual_key_down(VK_OEM_4);
			case Avo::KeyboardKey::Regional5:
				return get_is_virtual_key_down(VK_OEM_5);
			case Avo::KeyboardKey::Regional6:
				return get_is_virtual_key_down(VK_OEM_6);
			case Avo::KeyboardKey::Regional7:
				return get_is_virtual_key_down(VK_OEM_7);
			case Avo::KeyboardKey::Right:
				return get_is_virtual_key_down(VK_RIGHT);
			case Avo::KeyboardKey::Separator:
				return get_is_virtual_key_down(VK_SEPARATOR);
			case Avo::KeyboardKey::Shift:
				return get_is_virtual_key_down(VK_SHIFT);
			case Avo::KeyboardKey::Spacebar:
				return get_is_virtual_key_down(VK_SPACE);
			case Avo::KeyboardKey::Tab:
				return get_is_virtual_key_down(VK_TAB);
			case Avo::KeyboardKey::Up:
				return get_is_virtual_key_down(VK_UP);
		}
		return false;
	}
	auto get_is_mouse_button_down(Avo::MouseButton const p_button) const -> bool override {
		switch (p_button) {
			case Avo::MouseButton::Left:
				return get_is_virtual_key_down(VK_LBUTTON);
			case Avo::MouseButton::Middle:
				return get_is_virtual_key_down(VK_MBUTTON);
			case Avo::MouseButton::Right:
				return get_is_virtual_key_down(VK_RBUTTON);
			case Avo::MouseButton::X0:
				return get_is_virtual_key_down(VK_XBUTTON1);
			case Avo::MouseButton::X1:
				return get_is_virtual_key_down(VK_XBUTTON2);
		}
		return false;
	}

private:
	Avo::Point<int32> _mouse_position{-1, -1};
public:
	auto get_mouse_position() const -> Avo::Point<> override {
		return _mouse_position/_dip_to_pixel_factor;
	}

	//------------------------------

private:
	std::unordered_map<std::wstring_view, HCURSOR> _cursors;
	Avo::Cursor _cursor_type = Avo::Cursor::Unknown;
	HCURSOR _cursor_handle;

public:
	auto set_cursor(Avo::Cursor const p_cursor) -> void override {
		if (p_cursor == _cursor_type) {
			return;
		}

		auto name = std::wstring_view{};
		switch (p_cursor) {
			case Avo::Cursor::Arrow:
				name = IDC_ARROW;
				break;
			case Avo::Cursor::Blocked:
				name = IDC_NO;
				break;
			case Avo::Cursor::Hand:
				name = IDC_HAND;
				break;
			case Avo::Cursor::Ibeam:
				name = IDC_IBEAM;
				break;
			case Avo::Cursor::ResizeAll:
				name = IDC_SIZEALL;
				break;
			case Avo::Cursor::ResizeNESW:
				name = IDC_SIZENESW;
				break;
			case Avo::Cursor::ResizeNS:
				name = IDC_SIZENS;
				break;
			case Avo::Cursor::ResizeNWSE:
				name = IDC_SIZENWSE;
				break;
			case Avo::Cursor::ResizeWE:
				name = IDC_SIZEWE;
				break;
			case Avo::Cursor::Wait:
				name = IDC_WAIT;
				break;
		}
		
		if (auto const position = _cursors.find(name);
		    position == _cursors.end())
		{
			auto const new_cursor = LoadCursorW(nullptr, name);
			_cursors.insert({name, new_cursor});
			_cursor_handle = new_cursor;
			if (!_is_mouse_outside_client_area) {
				SetCursor(_cursor_handle);
			}
		}
		else {
			_cursor_handle = position->second;
			if (!_is_mouse_outside_client_area) {
				SetCursor(_cursor_handle);
			}
		}
		_cursor_type = p_cursor;
	}
	auto get_cursor() const -> Avo::Cursor override {
		return _cursor_type;
	}

	//------------------------------

private:
	ComReference<OleDropSource> _oleDropSource;
	ComReference<OleDropTarget> _oleDropTarget;

	CLIPFORMAT _clipboard_format_file_contents{};
	CLIPFORMAT _clipboard_format_file_group_descriptor{};
	CLIPFORMAT _clipboard_format_additional_data{};

	/*
		Assumes that medium is zero-initialized.
	*/
	auto create_additional_data(FORMATETC& format, STGMEDIUM& medium, uint64 const data) const -> void
	{
		format = FORMATETC{
			.cfFormat = _clipboard_format_additional_data,
			.dwAspect = DVASPECT_CONTENT,
			.lindex = -1,
			.tymed = TYMED_HGLOBAL
		};
		medium.tymed = TYMED_HGLOBAL;
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(data));
		*static_cast<uint64*>(medium.hGlobal) = data;
	}
	auto create_string_ole_data_object(std::u16string_view const p_string, uint64 const additional_data) const -> ComReference<OleDataObject>
	{
		auto formats = std::array<FORMATETC, 2>{};
		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;

		auto mediums = std::array<STGMEDIUM, 2>{};
		mediums[0].tymed = TYMED_HGLOBAL;

		//------------------------------

		auto const stringSize = (p_string.size() + 1)*sizeof(wchar_t);
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, stringSize);
		memcpy(mediums[0].hGlobal, p_string.data(), stringSize);

		//------------------------------

		create_additional_data(formats[1], mediums[1], additional_data);

		return make_com_reference<OleDataObject>(formats, mediums);
	}
	auto createImageOleDataObject(Avo::Image const& p_image, uint64 const additional_data) const -> ComReference<OleDataObject>
	{
		auto formats = std::array<FORMATETC, 3>{};
		auto mediums = std::array<STGMEDIUM, 3>{};

		formats[0].cfFormat = _clipboard_format_file_contents;
		formats[0].tymed = TYMED_ISTREAM;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;

		mediums[0].tymed = TYMED_ISTREAM;
		mediums[0].pstm = static_cast<IStream*>(_gui->get_drawing_context()->createImageFileDataNativeStream(p_image));

		//------------------------------

		formats[1].cfFormat = _clipboard_format_file_group_descriptor;
		formats[1].tymed = TYMED_HGLOBAL;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;

		mediums[1].tymed = TYMED_HGLOBAL;
		mediums[1].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

		auto group_descriptor = static_cast<FILEGROUPDESCRIPTORW*>(mediums[1].hGlobal);
		group_descriptor->cItems = 1;
		group_descriptor->fgd[0].dwFlags = FD_UNICODE;
		wcscpy_s(group_descriptor->fgd[0].cFileName, L"image.png");

		//------------------------------

		create_additional_data(formats[2], mediums[2], additional_data);

		return make_com_reference<OleDataObject>(formats, mediums);
	}
	auto create_file_ole_data_object(
		Avo::DataView const data, 
		std::u16string_view const p_name, 
		uint64 const additional_data
	) const -> ComReference<OleDataObject>
	{
		auto formats = std::array<FORMATETC, 4>{};
		auto mediums = std::array<STGMEDIUM, 4>{};

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;

		mediums[0].tymed = TYMED_HGLOBAL;

		auto const stringSize = (p_name.size() + 1)*sizeof(wchar_t);
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, stringSize);
		memcpy(mediums[0].hGlobal, p_name.data(), stringSize);

		//------------------------------

		formats[1].cfFormat = _clipboard_format_file_contents;
		formats[1].tymed = TYMED_ISTREAM;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;

		mediums[1].tymed = TYMED_ISTREAM;
		mediums[1].pstm = SHCreateMemStream(reinterpret_cast<BYTE const*>(data.data()), data.size());

		//------------------------------

		formats[2].cfFormat = _clipboard_format_file_group_descriptor;
		formats[2].tymed = TYMED_HGLOBAL;
		formats[2].dwAspect = DVASPECT_CONTENT;
		formats[2].lindex = -1;

		mediums[2].tymed = TYMED_HGLOBAL;
		mediums[2].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

		auto group_descriptor = static_cast<FILEGROUPDESCRIPTORW*>(mediums[2].hGlobal);
		group_descriptor->cItems = 1;
		group_descriptor->fgd[0].dwFlags = FD_UNICODE;
		memcpy(group_descriptor->fgd[0].cFileName, p_name.data(), stringSize);

		//------------------------------

		create_additional_data(formats[3], mediums[3], additional_data);

		return make_com_reference<OleDataObject>(formats, mediums);
	}
	auto create_file_ole_data_object(std::u16string_view const path_string, uint64 const additional_data) const 
		-> ComReference<OleDataObject>
	{
		auto const path = filesystem::path{path_string};
		auto const widePathStringSize = (path_string.size() + 1)*sizeof(wchar_t);

		auto formats = std::array<FORMATETC, 6>{};
		auto mediums = std::array<STGMEDIUM, 6>{};

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, widePathStringSize);
		memcpy(mediums[0].hGlobal, path_string.data(), widePathStringSize);

		//------------------------------

		formats[1].cfFormat = CF_HDROP;
		formats[1].tymed = TYMED_HGLOBAL;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;

		mediums[1].tymed = TYMED_HGLOBAL;
		mediums[1].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + widePathStringSize + sizeof(wchar_t));

		auto filename_structure = static_cast<DROPFILES*>(mediums[1].hGlobal);
		filename_structure->fNC = true;
		filename_structure->fWide = true;
		filename_structure->pt.x = 0;
		filename_structure->pt.y = 0;
		filename_structure->pFiles = sizeof(DROPFILES);

		memcpy(static_cast<char*>(mediums[1].hGlobal) + sizeof(DROPFILES), path_string.data(), widePathStringSize);
		*reinterpret_cast<wchar_t*>(static_cast<char*>(mediums[1].hGlobal) + sizeof(DROPFILES) + widePathStringSize) = 0;

		//------------------------------

		create_additional_data(formats[2], mediums[2], additional_data);

		//------------------------------

		if (filesystem::is_regular_file(path)) {
			formats[3].cfFormat = _clipboard_format_file_contents;
			formats[3].tymed = TYMED_ISTREAM;
			formats[3].dwAspect = DVASPECT_CONTENT;
			formats[3].lindex = -1;

			mediums[3].tymed = TYMED_ISTREAM;
			SHCreateStreamOnFileEx(reinterpret_cast<LPCWSTR>(path_string.data()), STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, 0, &mediums[3].pstm);

			//------------------------------

			formats[4].cfFormat = _clipboard_format_file_group_descriptor;
			formats[4].tymed = TYMED_HGLOBAL;
			formats[4].dwAspect = DVASPECT_CONTENT;
			formats[4].lindex = -1;

			mediums[4].tymed = TYMED_HGLOBAL;
			mediums[4].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

			auto group_descriptor = static_cast<FILEGROUPDESCRIPTORW*>(mediums[4].hGlobal);
			group_descriptor->cItems = 1;
			group_descriptor->fgd[0].dwFlags = FD_UNICODE;

			memcpy(group_descriptor->fgd[0].cFileName, path_string.data(), widePathStringSize);

			return make_com_reference<OleDataObject>(formats, mediums);
		}
		return make_com_reference<OleDataObject>(formats, mediums);
	}
	auto createFilesOleDataObject(
		Range<std::u16string*> const path_strings, 
		uint64 const additional_data
	) const -> ComReference<OleDataObject> {
		auto formats = std::array<FORMATETC, 2>{};
		auto mediums = std::array<STGMEDIUM, 2>{};

		//------------------------------
		// Create an HDROP format, which is just the paths of all items.

		formats[0].cfFormat = CF_HDROP;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;

		auto pathsStringSize = Count{};
		for (auto const& pathString : path_strings) {
			pathsStringSize += pathString.size() + 1;
		}
		pathsStringSize++;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + pathsStringSize*sizeof(wchar_t));

		auto filename_structure = static_cast<DROPFILES*>(mediums[0].hGlobal);
		filename_structure->fNC = true;
		filename_structure->fWide = true;
		filename_structure->pt.x = 0;
		filename_structure->pt.y = 0;
		filename_structure->pFiles = sizeof(DROPFILES);

		auto pathsString = reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(mediums[0].hGlobal) + sizeof(DROPFILES));
		auto pathsStringPosition = pathsString;
		for (auto const& pathString : path_strings) {
			std::copy(pathString.cbegin(), pathString.cend(), pathsStringPosition);
			pathsStringPosition[pathString.size()] = 0;
			//memcpy(pathsStringPosition, pathString.data(), (pathString.size() + 1)*sizeof(wchar_t));
			pathsStringPosition += pathString.size() + 1;
		}
		pathsString[pathsStringSize - 1] = 0;

		//------------------------------

		create_additional_data(formats[1], mediums[1], additional_data);

		//------------------------------

		return make_com_reference<OleDataObject>(formats, mediums);
	}

	auto do_drag_drop(
		OleDataObject* const data_object, 
		Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position
	) -> DWORD
	{
		auto const mousePositionBefore = _mouse_position;

		auto event = Avo::MouseEvent{};
		event.xy = _mouse_position/_dip_to_pixel_factor;
		if (get_is_mouse_button_down(Avo::MouseButton::Left))
		{
			event.mouse_button = Avo::MouseButton::Left;
		}
		else if (get_is_mouse_button_down(Avo::MouseButton::Middle))
		{
			event.mouse_button = Avo::MouseButton::Middle;
		}
		else if (get_is_mouse_button_down(Avo::MouseButton::Right))
		{
			event.mouse_button = Avo::MouseButton::Right;
		}
		else if (get_is_mouse_button_down(Avo::MouseButton::X0))
		{
			event.mouse_button = Avo::MouseButton::X0;
		}
		else if (get_is_mouse_button_down(Avo::MouseButton::X1))
		{
			event.mouse_button = Avo::MouseButton::X1;
		}
		if (event.mouse_button != Avo::MouseButton::None)
		{
			_gui->handleGlobalMouseUp(event);
		}

		if (p_dragImage)
		{
			_oleDropSource->set_drag_image(p_dragImage, drag_image_cursor_position, data_object);
		}

		_gui->unlock_threads();
		auto dropOperation = DWORD{DROPEFFECT_NONE};
		DoDragDrop(data_object, _oleDropSource, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK, &dropOperation);
		_gui->lock_threads();

		event.xy = pixels_to_dips(_mouse_position);
		event.movement = Avo::Vector2d{(_mouse_position - mousePositionBefore)/_dip_to_pixel_factor};
		event.mouse_button = Avo::MouseButton::None;
		_gui->handle_global_mouse_move(event);

		return dropOperation;
	}

	auto convert_native_drop_effect_to_drag_drop_operation(uint32 const p_dropEffect) 
		-> Avo::DragDropOperation
	{
		switch (p_dropEffect)
		{
		case DROPEFFECT_COPY:
			return Avo::DragDropOperation::Copy;
		case DROPEFFECT_MOVE:
			return Avo::DragDropOperation::Move;
		case DROPEFFECT_LINK:
			return Avo::DragDropOperation::Link;
		}
		return Avo::DragDropOperation::None;
	}

public:
	auto drag_and_drop_string(
		std::string_view const p_string, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		return drag_and_drop_string(Avo::convert_utf8_to_utf16(p_string), p_dragImage, drag_image_cursor_position, additional_data);
	}
	auto drag_and_drop_string(
		std::u16string_view const p_string, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto const dataObject = create_string_ole_data_object(p_string, additional_data);
		auto const dropOperation = do_drag_drop(dataObject, p_dragImage, drag_image_cursor_position);

		return convert_native_drop_effect_to_drag_drop_operation(dropOperation);
	}

	auto drag_and_drop_image(
		Avo::Image const& p_image, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto const dataObject = createImageOleDataObject(p_image, additional_data);
		auto const dropOperation = do_drag_drop(dataObject, p_dragImage, drag_image_cursor_position);

		return convert_native_drop_effect_to_drag_drop_operation(dropOperation);
	}

	auto drag_and_drop_file(
		Avo::DataView const data, std::string_view const p_name, 
		Avo::Image const& p_dragImage, Avo::Point<> const drag_image_cursor_position, 
		uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		return drag_and_drop_file(data, Avo::convert_utf8_to_utf16(p_name), p_dragImage, drag_image_cursor_position, additional_data);
	}
	auto drag_and_drop_file(
		Avo::DataView const data, std::u16string_view const p_name, 
		Avo::Image const& p_dragImage, Avo::Point<> const drag_image_cursor_position, 
		uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto const dataObject = create_file_ole_data_object(data, p_name, additional_data);
		auto const dropOperation = do_drag_drop(dataObject, p_dragImage, drag_image_cursor_position);

		return convert_native_drop_effect_to_drag_drop_operation(dropOperation);
	}

	auto drag_and_drop_file(
		std::string_view const path, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		return drag_and_drop_file(Avo::convert_utf8_to_utf16(path), p_dragImage, drag_image_cursor_position, additional_data);
	}
	auto drag_and_drop_file(
		std::u16string_view const path, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto const dataObject = create_file_ole_data_object(path, additional_data);
		auto const dropOperation = do_drag_drop(dataObject, p_dragImage, drag_image_cursor_position);

		return convert_native_drop_effect_to_drag_drop_operation(dropOperation);
	}

	auto drag_and_drop_files(
		Range<std::string*> const path_strings, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto widePathStrings = std::vector<std::u16string>(path_strings.size());
		for (auto const a : Indices{path_strings})
		{
			widePathStrings[a] = Avo::convert_utf8_to_utf16(path_strings[a]);
		}

		return drag_and_drop_files(Range{widePathStrings}, p_dragImage, drag_image_cursor_position, additional_data);
	}
	auto drag_and_drop_files(
		Range<std::u16string*> const path_strings, Avo::Image const& p_dragImage, 
		Avo::Point<> const drag_image_cursor_position, uint64 const additional_data
	) -> Avo::DragDropOperation override
	{
		auto const dataObject = createFilesOleDataObject(path_strings, additional_data);
		auto const dropOperation = do_drag_drop(dataObject, p_dragImage, drag_image_cursor_position);

		return convert_native_drop_effect_to_drag_drop_operation(dropOperation);
	}

	//------------------------------

	auto set_clipboard_string(std::string_view const p_string, uint64 const additional_data) const -> void override
	{
		set_clipboard_string(Avo::convert_utf8_to_utf16(p_string), additional_data);
	}
	auto set_clipboard_string(std::u16string_view const p_string, uint64 const additional_data) const -> void override
	{
		auto const dataObject = create_string_ole_data_object(p_string, additional_data);
		OleSetClipboard(dataObject);
	}

	auto set_clipboard_image(Avo::Image const& p_image, uint64 const additional_data) const -> void override
	{
		auto const dataObject = createImageOleDataObject(p_image, additional_data);
		OleSetClipboard(dataObject);
	}

	auto set_clipboard_file(Avo::DataView const data, std::string_view const p_name, uint64 const additional_data) const -> void override
	{
		auto const dataObject = create_file_ole_data_object(data, Avo::convert_utf8_to_utf16(p_name), additional_data);
		OleSetClipboard(dataObject);
	}
	auto set_clipboard_file(Avo::DataView const data, std::u16string_view const p_name, uint64 const additional_data) const -> void override
	{
		auto const dataObject = create_file_ole_data_object(data, p_name, additional_data);
		OleSetClipboard(dataObject);
	}

	auto set_clipboard_file(std::string_view const path, uint64 const additional_data) const -> void override
	{
		auto const dataObject = create_file_ole_data_object(Avo::convert_utf8_to_utf16(path), additional_data);
		OleSetClipboard(dataObject);
	}
	auto set_clipboard_file(std::u16string_view const path, uint64 const additional_data) const -> void override
	{
		auto const dataObject = create_file_ole_data_object(path, additional_data);
		OleSetClipboard(dataObject);
	}

	auto set_clipboard_files(Range<std::string*> const p_paths, uint64 const additional_data) const -> void override
	{
		auto widePathStrings = std::vector<std::u16string>(p_paths.size());
		for (auto const a : Indices{p_paths})
		{
			widePathStrings[a] = Avo::convert_utf8_to_utf16(p_paths[a]);
		}

		auto const dataObject = createFilesOleDataObject(widePathStrings, additional_data);
		OleSetClipboard(dataObject);
	}
	auto set_clipboard_files(Range<std::u16string*> const p_paths, uint64 const additional_data) const -> void override
	{
		auto const dataObject = createFilesOleDataObject(p_paths, additional_data);
		OleSetClipboard(dataObject);
	}

	auto get_clipboard_data() const -> std::unique_ptr<Avo::ClipboardData> override
	{
		auto dataObject = ComReference<IDataObject>{};
		OleGetClipboard(&dataObject);

		auto clipboardData = std::make_unique<OleClipboardData>(_gui);
		clipboardData->set_ole_data_object(dataObject);
		return clipboardData;
	}

	//------------------------------

private:
	bool _hasGottenInitialSizeMessageForCustomBorderWindows{};
	bool _is_mouse_outside_client_area = true;
	bool _isDoubleClicking{};

	auto createMouseButtonEvent(
		WPARAM const p_wparam, LPARAM const p_lparam, 
		Avo::MouseButton const p_button,
		bool const p_isDoubleClick = false
	) -> Avo::MouseEvent
	{
		Avo::MouseEvent mouseEvent;
		mouseEvent.xy = pixels_to_dips(Avo::Point{GET_X_LPARAM(p_lparam), GET_Y_LPARAM(p_lparam)});
		mouseEvent.mouse_button = p_button;
		mouseEvent.modifier_keys = convert_windows_key_state_to_modifier_key_flags(p_wparam);
		return mouseEvent;
	}
	static auto getIsKeyRepeated(LPARAM const p_lparam) -> bool
	{
		return p_lparam & (1 << 30);
	}
	
public:
	// Returns true if the event was handled
	auto handleEvent(
		UINT const p_message, 
		WPARAM const p_data_a, LPARAM const p_data_b
	) -> long long
	{
		switch (p_message)
		{
			case WM_CREATE:
			{
				OleInitialize(0);

				_oleDropSource = make_com_reference<OleDropSource>(_gui);
				_oleDropTarget = make_com_reference<OleDropTarget>(_gui);
				RegisterDragDrop(_window_handle, *_oleDropTarget);

				_clipboard_format_file_contents = RegisterClipboardFormatW(CFSTR_FILECONTENTS);
				_clipboard_format_file_group_descriptor = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
				_clipboard_format_additional_data = RegisterClipboardFormatW(CLIPBOARD_FORMAT_ADDITIONAL_DATA);

				//------------------------------

				EnableNonClientDpiScaling(_window_handle);

				/*
					LCS_WINDOWS_COLOR_SPACE is the default colorspace, but we want the background erase 
					color to be consistent with the colors of Direct2D and other potential graphics APIs 
					so it is changed to the sRGB color space.
				*/
				auto colorSpaceSettings = LOGCOLORSPACEW{
					/*.lcsSignature = */LCS_SIGNATURE,
					/*.lcsVersion = */0x400,
					/*.lcsSize = */sizeof(LOGCOLORSPACEW),
					/*.lcsCSType = */LCS_sRGB,
					/*.lcsIntent = */LCS_GM_ABS_COLORIMETRIC
				};

				auto colorSpace = CreateColorSpaceW(&colorSpaceSettings);
				SetColorSpace(GetDC(_window_handle), colorSpace);

				_isOpen = true;

				window_create_listeners(Avo::WindowEvent{this, _size/_dip_to_pixel_factor});

				return 0;
			}
			case WM_SIZE:
			{
				if (!_has_created_window)
				{
					_hasCreatedWindowMutex.lock();
					_has_created_window = true;
					_hasCreatedWindowMutex.unlock();
					_hasCreatedWindowConditionVariable.notify_one();
					return 0;
				}

				Avo::WindowEvent windowEvent;
				windowEvent.window = this;
				if (p_data_a == SIZE_MINIMIZED)
				{
					window_minimize_listeners(windowEvent);
					_state = Avo::WindowState::Minimized;
				}
				else if (_hasGottenInitialSizeMessageForCustomBorderWindows || !get_has_custom_border())
				{
					_size = {p_data_b & 0xffff, p_data_b >> 16 & 0xffff};
					windowEvent.size = pixels_to_dips(_size);

					if (p_data_a == SIZE_MAXIMIZED)
					{
						window_maximize_listeners(windowEvent);
						_state = Avo::WindowState::Maximized;
					}
					else if (p_data_a == SIZE_RESTORED && _state != Avo::WindowState::Restored)
					{
						window_restore_listeners(windowEvent);
						_state = Avo::WindowState::Restored;
					}
					window_size_change_listeners(windowEvent);
				}
				_hasGottenInitialSizeMessageForCustomBorderWindows = true;

				return 0;
			}
			case WM_APP_CHANGE_SIZE:
			{
				SetWindowPos(_window_handle, 0, 0, 0, (uint32)p_data_a, (uint32)p_data_b, SWP_NOMOVE | SWP_NOZORDER);

				return 0;
			}
			case WM_APP_SET_IS_ENABLED:
			{
				if (p_data_a)
				{
					EnableWindow(_window_handle, true);
					SetForegroundWindow(_window_handle);
				}
				else
				{
					auto child = GetWindow(_window_handle, GW_HWNDFIRST);
					if (child)
					{
						SetForegroundWindow(child);
						//SetActiveWindow(child);
						//SetFocus(child);
					}

					EnableWindow(_window_handle, false);
				}

				return 0;
			}
			case WM_ERASEBKGND:
			{
				auto deviceContext = reinterpret_cast<HDC>(p_data_a);

				auto rectangle = RECT{};
				GetUpdateRect(_window_handle, &rectangle, false);
				auto color = _gui->get_drawing_context()->getBackgroundColor(); // Thread safe I think?
				FillRect(deviceContext, &rectangle, CreateSolidBrush(RGB(color.red*255, color.green*255, color.blue*255)));

				return 1; // We erased it.
			}
			case WM_NCCALCSIZE:
			{
				if (get_has_custom_border() && p_data_a)
				{
					if (IsMaximized(_window_handle))
					{
						auto const parameters = reinterpret_cast<NCCALCSIZE_PARAMS*>(p_data_b);
						auto info = MONITORINFO{};
						info.cbSize = sizeof(MONITORINFO);
						GetMonitorInfo(MonitorFromRect(parameters->rgrc, MONITOR_DEFAULTTONEAREST), &info);

						parameters->rgrc[0] = info.rcWork;
					}
					return 0;
				}
				break;
			}
			case WM_NCMOUSEMOVE:
			{
				if (get_has_custom_border() && GetCapture() != _window_handle)
				{
					auto mousePosition = POINT{GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b)};
					ScreenToClient(_window_handle, &mousePosition);

					bool wasMousePositionInsideWindow = _mouse_position.x >= 0 && _mouse_position.y >= 0 
					                                 && _mouse_position.x < _size.x && _mouse_position.y < _size.y;
					if (!_is_mouse_outside_client_area || _is_mouse_outside_client_area && !wasMousePositionInsideWindow) // Is was outside of the nonclient area before this mousemove.
					{
						auto trackStructure = TRACKMOUSEEVENT{};
						trackStructure.dwFlags = TME_LEAVE | TME_NONCLIENT;
						trackStructure.cbSize = sizeof(TRACKMOUSEEVENT);
						trackStructure.hwndTrack = _window_handle;
						TrackMouseEvent(&trackStructure);
						if (_is_mouse_outside_client_area)
						{
							// The window will recieve WM_MOUSELEAVE - no need for extra mouse events, so return.
							_is_mouse_outside_client_area = true;
							return 0;
						}
					}

					_is_mouse_outside_client_area = true;

					// We want the GUI to recieve mouse move events even when the mouse is inside the nonclient area of the window - 
					// because it is in this case part of the GUI (since the CustomBorder style flag is true).
					if (mousePosition.x >= 0 && mousePosition.y >= 0 && mousePosition.x < _size.x && mousePosition.y < _size.y)
					{
						auto mouseEvent = Avo::MouseEvent{};
						mouseEvent.xy = pixels_to_dips(Avo::Point<Avo::Pixels>{mousePosition.x, mousePosition.y});
						mouseEvent.movement = Avo::Vector2d{mouseEvent.xy - pixels_to_dips(_mouse_position)};

						_mouse_position = {mousePosition.x, mousePosition.y};

						auto lock = _gui->create_thread_lock();
						_gui->handle_global_mouse_move(mouseEvent);
					}
					return 0;
				}
				break;
			}
			case WM_MOUSEMOVE:
			{
				auto xy = Avo::Point<Avo::Pixels>{GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b)};

				if (xy == _mouse_position)
				{
					return 0;
				}

				Avo::MouseEvent mouseEvent;
				mouseEvent.xy = xy/_dip_to_pixel_factor;
				mouseEvent.movement = Avo::Vector2d{(xy - _mouse_position)/_dip_to_pixel_factor};

				_mouse_position = xy;

				_gui->lock_threads();
				_gui->handle_global_mouse_move(mouseEvent);
				_gui->unlock_threads();

				if (_is_mouse_outside_client_area)
				{
					SetCursor(_cursor_handle);

					// This is to make the window recieve WM_MOUSELEAVE.
					auto trackStructure = TRACKMOUSEEVENT{};
					trackStructure.dwFlags = TME_LEAVE;
					trackStructure.cbSize = sizeof(TRACKMOUSEEVENT);
					trackStructure.hwndTrack = _window_handle;
					TrackMouseEvent(&trackStructure);

					_is_mouse_outside_client_area = false;
				}

				return 0;
			}
			case WM_NCMOUSELEAVE:
			case WM_MOUSELEAVE:
			{
				if (GetCapture() != _window_handle)
				{
					POINT winMousePosition;
					GetCursorPos(&winMousePosition);

					bool const isMouseOverWindow = WindowFromPoint(winMousePosition) == _window_handle;

					ScreenToClient(_window_handle, &winMousePosition);

					auto const mousePosition = Avo::Point<Avo::Pixels>{winMousePosition.x, winMousePosition.y};
					if (_size.get_is_containing(mousePosition) && isMouseOverWindow)
					{
						/*
							If it's a WM_MOUSELEAVE message, then it has entered the nonclient area if the new mouse position still is inside the 
							window. If it's a WM_NCMOUSELEAVE message, then it has entered the client area. Note that both these cases would mean 
							that the window has the CustomBorder style flag set.
						*/
						_is_mouse_outside_client_area = p_message == WM_MOUSELEAVE;
						return 0;
					}

					_is_mouse_outside_client_area = true;

					Avo::MouseEvent mouseEvent;
					mouseEvent.xy = mousePosition/_dip_to_pixel_factor;
					mouseEvent.movement = Avo::Vector2d{(mousePosition - _mouse_position)/_dip_to_pixel_factor};

					_mouse_position = mousePosition;

					{
						auto const lock = _gui->create_thread_lock();
						_gui->handle_global_mouse_move(mouseEvent);
						_gui->handleGlobalMouseLeave(mouseEvent);
					}
					_cursor_type = Avo::Cursor::Unknown;
				}
				return 0;
			}
			case WM_NCHITTEST:
			{
				if (get_has_custom_border())
				{
					if (p_data_b)
					{
						auto mousePosition = POINT{GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b)};
						ScreenToClient(_window_handle, &mousePosition);

						auto const area = _gui->get_window_border_area_at_position(
							pixels_to_dips(Avo::Point<Avo::Pixels>{mousePosition.x, mousePosition.y})
						);
						if (IsMaximized(_window_handle) && area != Avo::WindowBorderArea::Dragging && area != Avo::WindowBorderArea::None)
						{
							return HTCLIENT;
						}
						switch (area)
						{
							case Avo::WindowBorderArea::TopLeftResize:
								return HTTOPLEFT;
							case Avo::WindowBorderArea::TopResize:
								return HTTOP;
							case Avo::WindowBorderArea::TopRightResize:
								return HTTOPRIGHT;
							case Avo::WindowBorderArea::LeftResize:
								return HTLEFT;
							case Avo::WindowBorderArea::RightResize:
								return HTRIGHT;
							case Avo::WindowBorderArea::BottomLeftResize:
								return HTBOTTOMLEFT;
							case Avo::WindowBorderArea::BottomResize:
								return HTBOTTOM;
							case Avo::WindowBorderArea::BottomRightResize:
								return HTBOTTOMRIGHT;
							case Avo::WindowBorderArea::Dragging:
								return HTCAPTION;
							case Avo::WindowBorderArea::None:
								return HTCLIENT;
						}
					}
					return 0;
				}
				break;
			}
			case WM_DPICHANGED:
			{
				_gui->get_drawing_context()->setDpi(static_cast<float>(HIWORD(p_data_a)));
				_dip_to_pixel_factor = HIWORD(p_data_a)/static_cast<float>(USER_DEFAULT_SCREEN_DPI);
				auto newRectangle = reinterpret_cast<RECT*>(p_data_b);
				SetWindowPos(
					_window_handle, 0, 
					newRectangle->left, newRectangle->top, 
					newRectangle->right - newRectangle->left, newRectangle->bottom - newRectangle->top, 
					SWP_NOZORDER | SWP_NOACTIVATE
				);
				return 0;
			}
			case WM_GETMINMAXINFO:
			{
				auto minMaxInfo = reinterpret_cast<MINMAXINFO*>(p_data_b);

				auto rect = RECT{0, 0, _min_size.x, _min_size.y};
				AdjustWindowRect(&rect, _styles, 0);
				if (_min_size.x > 0U || _min_size.y > 0U)
				{
					minMaxInfo->ptMinTrackSize.x = rect.right - rect.left;
					minMaxInfo->ptMinTrackSize.y = rect.bottom - rect.top;
				}

				rect = {0, 0, _max_size.x, _max_size.y};
				AdjustWindowRect(&rect, _styles, 0);
				if (_max_size.x > 0U || _max_size.y > 0U)
				{
					minMaxInfo->ptMaxTrackSize.x = rect.right - rect.left;
					minMaxInfo->ptMaxTrackSize.y = rect.bottom - rect.top;
				}
				return 0;
			}
			case WM_MOVE:
			{
				RECT rect;
				GetWindowRect(_window_handle, &rect);
				_position = {rect.left, rect.top};

				return 0;
			}
			case WM_MOUSEWHEEL:
			{
				auto mousePosition = POINT{GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b)};				
				ScreenToClient(_window_handle, &mousePosition);

				Avo::MouseEvent mouseEvent;
				mouseEvent.xy = Avo::Point{mousePosition.x, mousePosition.y}/_dip_to_pixel_factor;
				mouseEvent.scrollDelta = GET_WHEEL_DELTA_WPARAM(p_data_a)/120.f;
				mouseEvent.modifier_keys = convert_windows_key_state_to_modifier_key_flags(GET_KEYSTATE_WPARAM(p_data_a));

				_gui->lock_threads();
				_gui->handle_global_mouse_scroll(mouseEvent);
				_gui->unlock_threads();

				return 0;
			}
			case WM_LBUTTONDOWN:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Left));
				_gui->unlock_threads();

				SetCapture(_window_handle);

				return 0;
			}
			case WM_LBUTTONUP:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseUp(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Left, _isDoubleClicking));
				_gui->unlock_threads();

				_isDoubleClicking = false;

				ReleaseCapture();

				return 0;
			}
			case WM_LBUTTONDBLCLK:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Left, true));
				_gui->unlock_threads();

				_isDoubleClicking = true;

				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Right));
				_gui->unlock_threads();

				return 0;
			}
			case WM_RBUTTONUP:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseUp(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Right, _isDoubleClicking));
				_gui->unlock_threads();

				_isDoubleClicking = false;

				return 0;
			}
			case WM_RBUTTONDBLCLK:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Right, true));
				_gui->unlock_threads();

				_isDoubleClicking = true;
				
				return 0;
			}
			case WM_MBUTTONDOWN:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Middle));
				_gui->unlock_threads();

				return 0;
			}
			case WM_MBUTTONUP:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseUp(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Middle, _isDoubleClicking));
				_gui->unlock_threads();

				_isDoubleClicking = false;

				return 0;
			}
			case WM_MBUTTONDBLCLK:
			{
				_gui->lock_threads();
				_gui->handleGlobalMouseDown(createMouseButtonEvent(p_data_a, p_data_b, Avo::MouseButton::Middle, true));
				_gui->unlock_threads();

				_isDoubleClicking = true;

				return 0;
			}
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				_gui->lock_threads();
				_gui->send_global_keyboard_key_down_events({convert_windows_data_to_keyboard_key(p_data_a), getIsKeyRepeated(p_data_b)});
				_gui->unlock_threads();

				return 0;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				_gui->lock_threads();
				_gui->sendGlobalKeyboardKeyUpEvents({convert_windows_data_to_keyboard_key(p_data_a)});
				_gui->unlock_threads();

				return 0;
			}
			case WM_CHAR:
			{
				// Length is 5 because 4 is the max number of bytes in a utf-8 encoded character, and the null terminator is included
				char character[5];
				Avo::convert_utf16_to_utf8(reinterpret_cast<char16 const*>(&p_data_a), {character, 5});

				_gui->lock_threads();
				_gui->send_global_character_input_events({character, getIsKeyRepeated(p_data_b)});
				_gui->unlock_threads();
				return 0;
			}
			case WM_MENUCHAR:
			{
				return 1 << 16;
			}
			case WM_CLOSE:
			{
				if (_will_close)
				{
					DeleteColorSpace(GetColorSpace(GetDC(_window_handle)));
					
					_oleDropTarget = nullptr;
					_oleDropSource = nullptr;
					RevokeDragDrop(_window_handle);
					OleUninitialize();

					_isOpen = false;
					DestroyWindow(_window_handle);
				}
				else
				{
					_will_close = set_window_close_events({this, _size/_dip_to_pixel_factor});
				}

				return 0;
			}
			case WM_DESTROY:
			{
				_window_handle = 0;
				s_number_of_windows--;
				if (!s_number_of_windows)
				{
					UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandle(0));
				}
				PostQuitMessage(0);

				return 0;
			}
		}
		return ~0LL;
	}

	static auto CALLBACK handleGlobalEvents(
		HWND const p_windowHandle, UINT const p_message, 
		WPARAM const p_data_a, LPARAM const p_data_b
	) -> LRESULT
	{
		WindowsWindow* window;
		if (p_message == WM_CREATE)
		{
			window = static_cast<WindowsWindow*>(reinterpret_cast<CREATESTRUCT*>(p_data_b)->lpCreateParams);
			SetWindowLongPtr(p_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			window->set_native_handle(p_windowHandle);
		}
		else
		{
			window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(p_windowHandle, GWLP_USERDATA));
		}
		if (window)
		{
			if (auto result = window->handleEvent(p_message, p_data_a, p_data_b);
			    result != ~0LL)
			{
				return result;
			}
		}
		return DefWindowProc(p_windowHandle, p_message, p_data_a, p_data_b);
	}

	//------------------------------

private:
	Avo::Gui* _gui;
public:
	WindowsWindow(Avo::Gui* gui) :
		_gui{gui} 
	{
		set_cursor(Avo::Cursor::Arrow);
	}
	// WindowsWindow(Avo::Gui* gui, std::string_view p_title, Avo::Size<> size,
	//               Avo::WindowStyleFlags p_styleFlags = Avo::WindowStyleFlags::Default, Avo::Window* p_parent = 0) :
	// 	_gui(gui), 
	// 	_cross_platform_styles(p_styleFlags)
	// {
	// 	Window::create(p_title, size, p_styleFlags, p_parent);

	// 	set_cursor(Avo::Cursor::Arrow);
	// }
	~WindowsWindow()
	{
		_messageThread.join();
		DestroyCursor(_cursor_handle);
	}
};
std::atomic<uint32> WindowsWindow::s_number_of_windows;

#endif

#ifdef __linux__
/*
	Some resources that I used:
	https://www.x.org/docs/X11/xlib.pdf
	https://specifications.freedesktop.org/wm-spec/wm-spec-1.4.html
	https://www.x.org/releases/X11R7.6/doc/xorg-docs/specs/ICCCM/icccm.html
	https://github.com/SFML/SFML/tree/master/src/SFML/Window/Unix
	The spec websites are really ugly and it's hard to read on them for me, a tip is to use a 
	"reader view" chrome extension or similar functionality in other browsers :) 
*/

constexpr float NORMAL_DPI = 96.f;

typedef Window XWindow;

class LinuxWindow : public Avo::Window
{
private:
	Avo::Gui* _gui = nullptr;

	XWindow _window_handle = 0;

	XIM _inputMethod = nullptr;
	XIC _inputContext = nullptr;

	Atom _windowManagerProtocolsMessageType{0};
	Atom _windowCloseEvent{0};

	Atom _backgroundColorMessageType{0};
	
	bool _isOpen{false};
	Avo::Point<uint32> _size;
	Avo::Point<> _min_size;
	Avo::Point<> _max_size;

	Avo::WindowStyleFlags _cross_platform_styles{Avo::WindowStyleFlags::None};

	float _dip_to_pixel_factor{1.f};

	//------------------------------

	KeySym convertKeyboardKeyToKeySym(Avo::KeyboardKey p_key) const
	{
		switch (p_key)
		{
			case Avo::KeyboardKey::Menu:
				return XK_Menu;
			case Avo::KeyboardKey::Backspace:
				return XK_BackSpace;
			case Avo::KeyboardKey::Clear:
				return XK_Clear;
			case Avo::KeyboardKey::Tab:
				return XK_Tab;
			case Avo::KeyboardKey::Return:
				return XK_Return;
			case Avo::KeyboardKey::Shift:
				return XK_Shift_L;
			case Avo::KeyboardKey::Control:
				return XK_Control_L;
			case Avo::KeyboardKey::Alt:
				return XK_Alt_L;
			case Avo::KeyboardKey::Pause:
				return XK_Pause;
			case Avo::KeyboardKey::CapsLock:
				return XK_Caps_Lock;
			case Avo::KeyboardKey::Escape:
				return XK_Escape;
			case Avo::KeyboardKey::Spacebar:
				return XK_space;
			case Avo::KeyboardKey::PageUp:
				return XK_Page_Up;
			case Avo::KeyboardKey::PageDown:
				return XK_Page_Down;
			case Avo::KeyboardKey::End:
				return XK_End;
			case Avo::KeyboardKey::Home:
				return XK_Home;
			case Avo::KeyboardKey::Left:
				return XK_Left;
			case Avo::KeyboardKey::Right:
				return XK_Right;
			case Avo::KeyboardKey::Up:
				return XK_Up;
			case Avo::KeyboardKey::Down:
				return XK_Down;
			case Avo::KeyboardKey::PrintScreen:
				return XK_Print;
			case Avo::KeyboardKey::Insert:
				return XK_Insert;
			case Avo::KeyboardKey::Delete:
				return XK_Delete;
			case Avo::KeyboardKey::Help:
				return XK_Help;
			case Avo::KeyboardKey::Numpad0:
				return XK_KP_0;
			case Avo::KeyboardKey::Numpad1:
				return XK_KP_1;
			case Avo::KeyboardKey::Numpad2:
				return XK_KP_2;
			case Avo::KeyboardKey::Numpad3:
				return XK_KP_3;
			case Avo::KeyboardKey::Numpad4:
				return XK_KP_4;
			case Avo::KeyboardKey::Numpad5:
				return XK_KP_5;
			case Avo::KeyboardKey::Numpad6:
				return XK_KP_6;
			case Avo::KeyboardKey::Numpad7:
				return XK_KP_7;
			case Avo::KeyboardKey::Numpad8:
				return XK_KP_8;
			case Avo::KeyboardKey::Numpad9:
				return XK_KP_9;
			case Avo::KeyboardKey::Add:
				return XK_KP_Add;
			case Avo::KeyboardKey::Subtract:
				return XK_KP_Subtract;
			case Avo::KeyboardKey::Multiply:
				return XK_KP_Multiply;
			case Avo::KeyboardKey::Divide:
				return XK_KP_Divide;
			case Avo::KeyboardKey::F1:
				return XK_F1;
			case Avo::KeyboardKey::F2:
				return XK_F2;
			case Avo::KeyboardKey::F3:
				return XK_F3;
			case Avo::KeyboardKey::F4:
				return XK_F4;
			case Avo::KeyboardKey::F5:
				return XK_F5;
			case Avo::KeyboardKey::F6:
				return XK_F6;
			case Avo::KeyboardKey::F7:
				return XK_F7;
			case Avo::KeyboardKey::F8:
				return XK_F8;
			case Avo::KeyboardKey::F9:
				return XK_F9;
			case Avo::KeyboardKey::F10:
				return XK_F10;
			case Avo::KeyboardKey::F11:
				return XK_F11;
			case Avo::KeyboardKey::F12:
				return XK_F12;
			case Avo::KeyboardKey::F13:
				return XK_F13;
			case Avo::KeyboardKey::F14:
				return XK_F14;
			case Avo::KeyboardKey::F15:
				return XK_F15;
			case Avo::KeyboardKey::F16:
				return XK_F16;
			case Avo::KeyboardKey::F17:
				return XK_F17;
			case Avo::KeyboardKey::F18:
				return XK_F18;
			case Avo::KeyboardKey::F19:
				return XK_F19;
			case Avo::KeyboardKey::F20:
				return XK_F20;
			case Avo::KeyboardKey::F21:
				return XK_F21;
			case Avo::KeyboardKey::F22:
				return XK_F22;
			case Avo::KeyboardKey::F23:
				return XK_F23;
			case Avo::KeyboardKey::F24:
				return XK_F24;
			case Avo::KeyboardKey::NumLock:
				return XK_Num_Lock;
			case Avo::KeyboardKey::Number0:
				return XK_0;
			case Avo::KeyboardKey::Number1:
				return XK_1;
			case Avo::KeyboardKey::Number2:
				return XK_2;
			case Avo::KeyboardKey::Number3:
				return XK_3;
			case Avo::KeyboardKey::Number4:
				return XK_4;
			case Avo::KeyboardKey::Number5:
				return XK_5;
			case Avo::KeyboardKey::Number6:
				return XK_6;
			case Avo::KeyboardKey::Number7:
				return XK_7;
			case Avo::KeyboardKey::Number8:
				return XK_8;
			case Avo::KeyboardKey::Number9:
				return XK_9;
			case Avo::KeyboardKey::A:
				return XK_A;
			case Avo::KeyboardKey::B:
				return XK_B;
			case Avo::KeyboardKey::C:
				return XK_C;
			case Avo::KeyboardKey::D:
				return XK_D;
			case Avo::KeyboardKey::E:
				return XK_E;
			case Avo::KeyboardKey::F:
				return XK_F;
			case Avo::KeyboardKey::G:
				return XK_G;
			case Avo::KeyboardKey::H:
				return XK_H;
			case Avo::KeyboardKey::I:
				return XK_I;
			case Avo::KeyboardKey::J:
				return XK_J;
			case Avo::KeyboardKey::K:
				return XK_K;
			case Avo::KeyboardKey::L:
				return XK_L;
			case Avo::KeyboardKey::M:
				return XK_M;
			case Avo::KeyboardKey::N:
				return XK_N;
			case Avo::KeyboardKey::O:
				return XK_O;
			case Avo::KeyboardKey::P:
				return XK_P;
			case Avo::KeyboardKey::Q:
				return XK_Q;
			case Avo::KeyboardKey::R:
				return XK_R;
			case Avo::KeyboardKey::S:
				return XK_S;
			case Avo::KeyboardKey::T:
				return XK_T;
			case Avo::KeyboardKey::U:
				return XK_U;
			case Avo::KeyboardKey::V:
				return XK_V;
			case Avo::KeyboardKey::W:
				return XK_W;
			case Avo::KeyboardKey::X:
				return XK_X;
			case Avo::KeyboardKey::Y:
				return XK_Y;
			case Avo::KeyboardKey::Z:
				return XK_Z;
			case Avo::KeyboardKey::Regional1:
				return XK_semicolon;
			case Avo::KeyboardKey::Regional2:
				return XK_slash;
			case Avo::KeyboardKey::Regional3:
				return XK_grave;
			case Avo::KeyboardKey::Regional4:
				return XK_bracketleft;
			case Avo::KeyboardKey::Regional5:
				return XK_backslash;
			case Avo::KeyboardKey::Regional6:
				return XK_bracketright;
			case Avo::KeyboardKey::Regional7:
				return XK_apostrophe;
		}
		return 0;
	}
	Avo::KeyboardKey convertKeySymToKeyboardKey(KeySym p_keySym) const
	{
		switch (p_keySym)
		{
			case XK_Menu:
				return Avo::KeyboardKey::Menu;
			case XK_BackSpace:
				return Avo::KeyboardKey::Backspace;
			case XK_Clear:
				return Avo::KeyboardKey::Clear;
			case XK_Tab:
				return Avo::KeyboardKey::Tab;
			case XK_Return:
				return Avo::KeyboardKey::Return;
			case XK_Shift_L:
			case XK_Shift_R:
				return Avo::KeyboardKey::Shift;
			case XK_Control_L:
			case XK_Control_R:
				return Avo::KeyboardKey::Control;
			case XK_Alt_L:
			case XK_Alt_R:
				return Avo::KeyboardKey::Alt;
			case XK_Pause:
				return Avo::KeyboardKey::Pause;
			case XK_Caps_Lock:
				return Avo::KeyboardKey::CapsLock;
			case XK_Escape:
				return Avo::KeyboardKey::Escape;
			case XK_space:
				return Avo::KeyboardKey::Spacebar;
			case XK_Page_Up:
				return Avo::KeyboardKey::PageUp;
			case XK_Page_Down:
				return Avo::KeyboardKey::PageDown;
			case XK_End:
				return Avo::KeyboardKey::End;
			case XK_Home:
				return Avo::KeyboardKey::Home;
			case XK_Left:
				return Avo::KeyboardKey::Left;
			case XK_Right:
				return Avo::KeyboardKey::Right;
			case XK_Up:
				return Avo::KeyboardKey::Up;
			case XK_Down:
				return Avo::KeyboardKey::Down;
			case XK_Print:
				return Avo::KeyboardKey::PrintScreen;
			case XK_Insert:
				return Avo::KeyboardKey::Insert;
			case XK_Delete:
				return Avo::KeyboardKey::Delete;
			case XK_Help:
				return Avo::KeyboardKey::Help;
			case XK_KP_0:
				return Avo::KeyboardKey::Numpad0;
			case XK_KP_1:
				return Avo::KeyboardKey::Numpad1;
			case XK_KP_2:
				return Avo::KeyboardKey::Numpad2;
			case XK_KP_3:
				return Avo::KeyboardKey::Numpad3;
			case XK_KP_4:
				return Avo::KeyboardKey::Numpad4;
			case XK_KP_5:
				return Avo::KeyboardKey::Numpad5;
			case XK_KP_6:
				return Avo::KeyboardKey::Numpad6;
			case XK_KP_7:
				return Avo::KeyboardKey::Numpad7;
			case XK_KP_8:
				return Avo::KeyboardKey::Numpad8;
			case XK_KP_9:
				return Avo::KeyboardKey::Numpad9;
			case XK_KP_Add:
				return Avo::KeyboardKey::Add;
			case XK_KP_Subtract:
				return Avo::KeyboardKey::Subtract;
			case XK_KP_Multiply:
				return Avo::KeyboardKey::Multiply;
			case XK_KP_Divide:
				return Avo::KeyboardKey::Divide;
			case XK_F1:
				return Avo::KeyboardKey::F1;
			case XK_F2:
				return Avo::KeyboardKey::F2;
			case XK_F3:
				return Avo::KeyboardKey::F3;
			case XK_F4:
				return Avo::KeyboardKey::F4;
			case XK_F5:
				return Avo::KeyboardKey::F5;
			case XK_F6:
				return Avo::KeyboardKey::F6;
			case XK_F7:
				return Avo::KeyboardKey::F7;
			case XK_F8:
				return Avo::KeyboardKey::F8;
			case XK_F9:
				return Avo::KeyboardKey::F9;
			case XK_F10:
				return Avo::KeyboardKey::F10;
			case XK_F11:
				return Avo::KeyboardKey::F11;
			case XK_F12:
				return Avo::KeyboardKey::F12;
			case XK_F13:
				return Avo::KeyboardKey::F13;
			case XK_F14:
				return Avo::KeyboardKey::F14;
			case XK_F15:
				return Avo::KeyboardKey::F15;
			case XK_F16:
				return Avo::KeyboardKey::F16;
			case XK_F17:
				return Avo::KeyboardKey::F17;
			case XK_F18:
				return Avo::KeyboardKey::F18;
			case XK_F19:
				return Avo::KeyboardKey::F19;
			case XK_F20:
				return Avo::KeyboardKey::F20;
			case XK_F21:
				return Avo::KeyboardKey::F21;
			case XK_F22:
				return Avo::KeyboardKey::F22;
			case XK_F23:
				return Avo::KeyboardKey::F23;
			case XK_F24:
				return Avo::KeyboardKey::F24;
			case XK_Num_Lock:
				return Avo::KeyboardKey::NumLock;
			case XK_0:
				return Avo::KeyboardKey::Number0;
			case XK_1:
				return Avo::KeyboardKey::Number1;
			case XK_2:
				return Avo::KeyboardKey::Number2;
			case XK_3:
				return Avo::KeyboardKey::Number3;
			case XK_4:
				return Avo::KeyboardKey::Number4;
			case XK_5:
				return Avo::KeyboardKey::Number5;
			case XK_6:
				return Avo::KeyboardKey::Number6;
			case XK_7:
				return Avo::KeyboardKey::Number7;
			case XK_8:
				return Avo::KeyboardKey::Number8;
			case XK_9:
				return Avo::KeyboardKey::Number9;
			case XK_A:
				return Avo::KeyboardKey::A;
			case XK_B:
				return Avo::KeyboardKey::B;
			case XK_C:
				return Avo::KeyboardKey::C;
			case XK_D:
				return Avo::KeyboardKey::D;
			case XK_E:
				return Avo::KeyboardKey::E;
			case XK_F:
				return Avo::KeyboardKey::F;
			case XK_G:
				return Avo::KeyboardKey::G;
			case XK_H:
				return Avo::KeyboardKey::H;
			case XK_I:
				return Avo::KeyboardKey::I;
			case XK_J:
				return Avo::KeyboardKey::J;
			case XK_K:
				return Avo::KeyboardKey::K;
			case XK_L:
				return Avo::KeyboardKey::L;
			case XK_M:
				return Avo::KeyboardKey::M;
			case XK_N:
				return Avo::KeyboardKey::N;
			case XK_O:
				return Avo::KeyboardKey::O;
			case XK_P:
				return Avo::KeyboardKey::P;
			case XK_Q:
				return Avo::KeyboardKey::Q;
			case XK_R:
				return Avo::KeyboardKey::R;
			case XK_S:
				return Avo::KeyboardKey::S;
			case XK_T:
				return Avo::KeyboardKey::T;
			case XK_U:
				return Avo::KeyboardKey::U;
			case XK_V:
				return Avo::KeyboardKey::V;
			case XK_W:
				return Avo::KeyboardKey::W;
			case XK_X:
				return Avo::KeyboardKey::X;
			case XK_Y:
				return Avo::KeyboardKey::Y;
			case XK_Z:
				return Avo::KeyboardKey::Z;
			case XK_semicolon:
				return Avo::KeyboardKey::Regional1;
			case XK_slash:
				return Avo::KeyboardKey::Regional2;
			case XK_grave:
				return Avo::KeyboardKey::Regional3;
			case XK_bracketleft:
				return Avo::KeyboardKey::Regional4;
			case XK_backslash:
				return Avo::KeyboardKey::Regional5;
			case XK_bracketright:
				return Avo::KeyboardKey::Regional6;
			case XK_apostrophe:
				return Avo::KeyboardKey::Regional7;
		}
		return Avo::KeyboardKey::None;
	}
	
	//------------------------------

	bool _has_created_window = false;
	std::condition_variable _hasCreatedWindowConditionVariable;
	std::mutex _hasCreatedWindowMutex;
	std::thread _messageThread;
	void thread_runEventLoop(std::string_view p_title, float p_x, float p_y, float p_width, float p_height, Avo::Window* p_parent)
	{
		XInitThreads();

		_server = XOpenDisplay(nullptr); // Open connection to server

		auto displayWidth = (float)XDisplayWidth(_server, 0);
		auto displayHeight = (float)XDisplayHeight(_server, 0);
		_dip_to_pixel_factor = displayWidth/(float)XDisplayWidthMM(_server, 0)*25.4f/NORMAL_DPI;
		_size.set(p_width*_dip_to_pixel_factor, p_height*_dip_to_pixel_factor);

		//------------------------------
		// Select the OpenGL visual to be used when creating the window.

		int framebufferAttributes[] =
		{
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

		int numberOfMatchingConfigurations = 0;
		GLXFBConfig* framebufferConfigurations = glXChooseFBConfig(_server, DefaultScreen(_server), framebufferAttributes, &numberOfMatchingConfigurations);

		XVisualInfo* visualInfo = glXGetVisualFromFBConfig(_server, *framebufferConfigurations);

		XFree(framebufferConfigurations);

		//------------------------------
		// Create window

		_colormap = XCreateColormap(_server, RootWindow(_server, visualInfo->screen), visualInfo->visual, 0);
		XSetWindowAttributes windowAttributes = {};
		windowAttributes.colormap = _colormap;
		windowAttributes.event_mask =
			ExposureMask |
			EnterWindowMask | LeaveWindowMask |
			StructureNotifyMask |
			PointerMotionMask |
			ButtonPressMask | ButtonReleaseMask |
			ButtonMotionMask |
			KeyPressMask | KeyReleaseMask;

		_window_handle = XCreateWindow(
			_server, p_parent ? (XWindow)p_parent->get_native_handle() : RootWindow(_server, visualInfo->screen),
			0, 0, // Initial x and y are ignored by the window manager
			(unsigned int)(p_width*_dip_to_pixel_factor),
			(unsigned int)(p_height*_dip_to_pixel_factor),
			0,
			visualInfo->depth,
			InputOutput,
			visualInfo->visual,
			CWEventMask | CWBorderPixel | CWColormap,
			&windowAttributes
		);

		XFree(visualInfo);

		set_title(p_title);
		XMapWindow(_server, _window_handle); // Show the window
		setPosition(p_x*(displayWidth - p_width*_dip_to_pixel_factor), p_y*(displayHeight - p_height*_dip_to_pixel_factor));

		//------------------------------
		// Open keyboard input

		_inputMethod = XOpenIM(_server, nullptr, nullptr, nullptr);
		_inputContext = XCreateIC(
			_inputMethod,
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing, // Input style flags
			XNClientWindow, _window_handle,
			XNFocusWindow, _window_handle,
			nullptr // Null terminator
		);

		//------------------------------

		_backgroundColorMessageType = XInternAtom(_server, "AVOGUI_SET_BACKGROUND_COLOR", false);

		// We want the window manager to tell us when the window should be closed.
		// WM_PROTOCOLS is the atom used to identify messages sent from the window manager in a ClientMessage.
		_windowManagerProtocolsMessageType = XInternAtom(_server, "WM_PROTOCOLS", true);
		// This is the atom sent as the data in a ClientMessage with type WM_PROTOCOLS, to indicate the close event.
		_windowCloseEvent = XInternAtom(_server, "WM_DELETE_WINDOW", 0);
		// Tell the window manager that we want it to send the event through WM_PROTOCOLS.
		XSetWMProtocols(_server, _window_handle, &_windowCloseEvent, 1);

		XFlush(_server); // Execute the command queue

		//------------------------------

		XEvent event;
		Time lastKeyPressTime = 0;
		KeyCode lastKeyPressKeyCode = 0;

		_isOpen = true;
		while (_isOpen)
		{
			XNextEvent(_server, &event);

			if (XFilterEvent(&event, _window_handle))
			{
				continue;
			}

			switch (event.type)
			{
				case Expose:
				{
					if (!event.xexpose.count)
					{
						_gui->invalidateRectangle(
							(float)event.xexpose.x/_dip_to_pixel_factor, (float)event.xexpose.y/_dip_to_pixel_factor,
							(float)event.xexpose.width/_dip_to_pixel_factor, (float)event.xexpose.height/_dip_to_pixel_factor
						);
					}
					break;
				}
				case ClientMessage:
				{
					if (event.xclient.message_type == _windowManagerProtocolsMessageType)
					{
						// Sent from the window manager when the user has tried to close the window,
						// it is up to us to decide whether to actually close and exit the application.
						if (event.xclient.data.l[0] == _windowCloseEvent)
						{
							if (_will_close)
							{
								_isOpen = false;
								XDestroyWindow(_server, _window_handle);
							}
							else
							{
								_will_close = set_window_close_events({this, _size.x/_dip_to_pixel_factor, _size.y/_dip_to_pixel_factor});
							}
						}
					}
					else if (event.xclient.message_type == _backgroundColorMessageType)
					{
						// Sent from drawing context
						XColor xColor;
						xColor.red = event.xclient.data.l[0];
						xColor.green = event.xclient.data.l[1];
						xColor.blue = event.xclient.data.l[2];
						xColor.flags = DoRed | DoGreen | DoBlue;
						XAllocColor(_server, _colormap, &xColor);
						XSetWindowBackground(_server, _window_handle, xColor.pixel);
					}
					break;
				}
				case MotionNotify:
				{

					break;
				}
				case GravityNotify:
				{
					break;
				}
				case ConfigureNotify:
				{
					if (!_has_created_window)
					{
						Avo::WindowEvent windowEvent = {this, p_width, p_height};
						window_create_listeners(windowEvent);

						window_size_change_listeners(windowEvent);

						_hasCreatedWindowMutex.lock();
						_has_created_window = true;
						_hasCreatedWindowMutex.unlock();
						_hasCreatedWindowConditionVariable.notify_one();
					}
					else if (_size.x != event.xconfigure.width || _size.y != event.xconfigure.height)
					{
						_size.set(event.xconfigure.width, event.xconfigure.height);
						window_size_change_listeners(Avo::WindowEvent{this, _size.x/_dip_to_pixel_factor, _size.y/_dip_to_pixel_factor});
					}
					break;
				}
				case ButtonPress:
				{
					break;
				}
				case ButtonRelease:
				{

					break;
				}
				case KeyPress:
				{
					// Length is 5 because 4 is the max number of bytes in a utf-8 encoded character
					char character[5];
					KeySym key;
					Status characterLookupStatus;
					int length = Xutf8LookupString(_inputContext, &event.xkey, character, 4, &key, &characterLookupStatus);

					Avo::KeyboardEvent keyboardEvent;
					keyboardEvent.isRepeated = lastKeyPressKeyCode == event.xkey.keycode && event.xkey.time < lastKeyPressTime + 2;
					if (characterLookupStatus == XLookupBoth || characterLookupStatus == XLookupChars)
					{
						keyboardEvent.character = character;
						_gui->lock_threads();
						_gui->send_global_character_input_events(keyboardEvent);
						_gui->unlock_threads();
					}
					if (characterLookupStatus == XLookupBoth || characterLookupStatus == XLookupKeySym)
					{
						keyboardEvent.character = "";
						keyboardEvent.key = convertKeySymToKeyboardKey(key);
						_gui->lock_threads();
						_gui->send_global_keyboard_key_down_events(keyboardEvent);
						_gui->unlock_threads();
					}

					lastKeyPressTime = event.xkey.time;

					break;
				}
				case KeyRelease:
				{
					Avo::KeyboardEvent keyboardEvent;

					// Try the four modifier groups until one matches
					for (uint32 a = 0; a < 4; a++)
					{
						if (Avo::KeyboardKey::None != (keyboardEvent.key = convertKeySymToKeyboardKey(XLookupKeysym(&event.xkey, a))))
						{
							break;
						}
					}

					if (keyboardEvent.key != Avo::KeyboardKey::None)
					{
						keyboardEvent.isRepeated = lastKeyPressKeyCode == event.xkey.keycode && event.xkey.time < lastKeyPressTime + 2;
						_gui->lock_threads();
						_gui->sendGlobalKeyboardKeyUpEvents(keyboardEvent);
						_gui->unlock_threads();
						lastKeyPressTime = event.xkey.time;
					}

					break;
				}
			} 
		}
		XDestroyIC(_inputContext);
		XCloseIM(_inputMethod);
		XFreeColormap(_server, _colormap);
		XCloseDisplay(_server);
	}

private:
	Display* _server = nullptr;
public:
	Display* getServer()
	{
		return _server;
	}

private:
	Colormap _colormap = 0;
public:
	Colormap getColormap()
	{
		return _colormap;
	}
	Atom getBackgroundColorMessageType()
	{
		return _backgroundColorMessageType;
	}

	void create(std::string_view p_title, float p_x, float p_y, float p_width, float p_height, Avo::WindowStyleFlags p_styleFlags, Avo::Window* p_parent) override
	{
		_cross_platform_styles = p_styleFlags;
		_messageThread = std::thread(&LinuxWindow::thread_runEventLoop, this, p_title, p_x, p_y, p_width, p_height, p_parent);

		if (!_has_created_window)
		{
			std::unique_lock<std::mutex> lock(_hasCreatedWindowMutex);
			_hasCreatedWindowConditionVariable.wait(lock, [=] { return (bool)_has_created_window; });
		}
	}
	void create(std::string_view p_title, float p_width, float p_height, Avo::WindowStyleFlags p_styleFlags = Avo::WindowStyleFlags::Default, Avo::Window* p_parent = nullptr) override
	{
		create(p_title, 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}

	void close() override 
	{
		if (_window_handle)
		{
			XEvent event;
			event.type = ClientMessage;
			event.xclient.message_type = _windowManagerProtocolsMessageType;
			event.xclient.data.l[0] = _windowCloseEvent;
			event.xclient.display = _server;
			event.xclient.window = _window_handle;
			event.xclient.format = 32;
			event.xclient.send_event = true;
			XSendEvent(_server, _window_handle, false, 0, &event);
			XFlush(_server);
		}
	}
	bool getIsOpen() const override
	{
		return _isOpen;
	}

	//------------------------------

	void enableUserInteraction() override 
	{
	}
	void disableUserInteraction() override 
	{
	}
	bool getIsUserInteractionEnabled() override 
	{
	}

	//------------------------------

	void set_title(std::string_view p_title) override 
	{
		XTextProperty textProperty;
		#ifdef X_HAVE_UTF8_STRING
		textProperty.encoding = XInternAtom(_server, "UTF8_STRING", 0);
		#else
		textProperty.encoding = XA_STRING;
		#endif
		textProperty.format = 8;
		textProperty.nitems = p_title.size();
		textProperty.value = (unsigned char*)p_title.data();
		XSetWMName(_server, _window_handle, &textProperty);
		XSetWMIconName(_server, _window_handle, &textProperty);
		XFlush(_server);
	}
	std::string get_title() const override
	{
		XTextProperty textProperty;
		XGetWMName(_server, _window_handle, &textProperty);
		return (char const*)textProperty.value;
	}

	//------------------------------

	void setStyles(Avo::WindowStyleFlags p_styles) override 
	{
	}
	Avo::WindowStyleFlags getStyles() const override
	{
	}

	//------------------------------

	void* get_native_handle() const override
	{
		return (void*)_window_handle;
	}

	//------------------------------

	void set_is_fullscreen(bool p_isFullscreen) override 
	{
	}
	void switchFullscreen() override 
	{
	}
	bool getIsFullscreen() const override
	{
	}

	//------------------------------

	void hide() override 
	{
	}
	void show() override 
	{
	}

	void maximize() override 
	{
	}
	void minimize() override 
	{
	}
	void restore() override 
	{
	}

	void setState(Avo::WindowState p_state) override 
	{
	}
	Avo::WindowState getState() const override
	{
	}

	//------------------------------

	void setPosition(int32 p_x, int32 p_y) override 
	{
		if (_window_handle)
		{
			XMoveWindow(_server, _window_handle, p_x, p_y);
			XFlush(_server);
		}
	}
	Avo::Point<int32> getPosition() const override
	{
		Avo::Point<int32> result;
		XWindow child;
		XTranslateCoordinates(_server, _window_handle, DefaultRootWindow(_server), 0, 0, &result.x, &result.y, &child);
		return result;
	}
	int32 getPositionX() const override
	{
		int32 x;
		int32 y;
		XWindow child;
		XTranslateCoordinates(_server, _window_handle, DefaultRootWindow(_server), 0, 0, &x, &y, &child);
		return x;
	}
	int32 getPositionY() const override
	{
		int32 x;
		int32 y;
		XWindow child;
		XTranslateCoordinates(_server, _window_handle, DefaultRootWindow(_server), 0, 0, &x, &y, &child);
		return y;
	}

	void set_size(float p_width, float p_height) override 
	{
		if (_window_handle)
		{
			XResizeWindow(_server, _window_handle, p_width*_dip_to_pixel_factor, p_height*_dip_to_pixel_factor);
			XFlush(_server);
		}
	}
	Avo::Point<> get_size() const override
	{
		return _size/_dip_to_pixel_factor;
	}
	float get_width() const override
	{
		return _size.x/_dip_to_pixel_factor;
	}
	float get_height() const override
	{
		return _size.y/_dip_to_pixel_factor;
	}

	void setMinSize(float p_minWidth, float p_minHeight) override 
	{
		XSizeHints sizeHints = {};
		sizeHints.flags = PMinSize;
		sizeHints.min_width = p_minWidth*_dip_to_pixel_factor;
		sizeHints.min_height = p_minHeight*_dip_to_pixel_factor; 
		XSetWMNormalHints(_server, _window_handle, &sizeHints);
		_min_size.set(p_minWidth, p_minHeight);
	}
	Avo::Point<> getMinSize() const override
	{
		return _min_size;
	}
	float getMinWidth() const override
	{
		return _min_size.x;
	}
	float getMinHeight() const override
	{
		return _min_size.y;
	}

	void setMaxSize(float p_maxWidth, float p_maxHeight) override 
	{
		XSizeHints sizeHints = {};
		sizeHints.flags = PMaxSize;
		sizeHints.max_width = p_maxWidth*_dip_to_pixel_factor;
		sizeHints.max_height = p_maxHeight*_dip_to_pixel_factor;
		XSetWMNormalHints(_server, _window_handle, &sizeHints);
		_max_size.set(p_maxWidth, p_maxHeight);
	}
	Avo::Point<> getMaxSize() const override
	{
		return _max_size;
	}
	float getMaxWidth() const override
	{
		return _max_size.x;
	}
	float getMaxHeight() const override
	{
		return _max_size.y;
	}

	//------------------------------

	Avo::Rectangle<uint32> get_monitor_bounds() const override
	{
	}
	Avo::Point<uint32> get_monitor_position() const override
	{
	}
	Avo::Point<uint32> get_monitor_size() const override
	{
	}
	uint32 get_monitor_width() const override
	{
	}
	uint32 get_monitor_height() const override
	{
	}

	//------------------------------

	Avo::Rectangle<uint32> get_work_area_bounds() const override
	{
	}
	Avo::Point<uint32> get_work_area_position() const override
	{
	}
	Avo::Point<uint32> get_work_area_size() const override
	{
	}
	uint32 get_work_area_width() const override
	{
	}
	uint32 get_work_area_height() const override
	{
	}

	//------------------------------

	bool get_is_key_down(Avo::KeyboardKey p_key) const override
	{
		KeySym keySym = convertKeyboardKeyToKeySym(p_key);
		KeyCode keyCode = XKeysymToKeycode(_server, keySym);
		
		char keymap[32];
		XQueryKeymap(_server, keymap);
		if (keyCode)
		{
			bool result = keymap[keyCode/8] & 1 << keyCode % 8; // Look up operator precedence ;)
			if (!result && keySym == XK_Control_L)
			{
				return keymap[XKeysymToKeycode(_server, XK_Control_R)/8] & 1 << keyCode % 8;
			}
			if (!result && keySym == XK_Shift_L)
			{
				return keymap[XKeysymToKeycode(_server, XK_Shift_R)/8] & 1 << keyCode % 8;
			}
			if (!result && keySym == XK_Alt_L)
			{
				return keymap[XKeysymToKeycode(_server, XK_Alt_R)/8] & 1 << keyCode % 8;
			}
			return result;
		}
		return false;
	}
	bool get_is_mouse_button_down(Avo::MouseButton p_button) const override
	{
	}
	Avo::Point<> get_mouse_position() const override
	{
	}

	//------------------------------

	void set_cursor(Avo::Cursor p_cursor) override 
	{
		
	}
	Avo::Cursor get_cursor() const override
	{
	}

	//------------------------------

	float get_dip_to_pixel_factor() const override
	{
		return _dip_to_pixel_factor;
	}

	//------------------------------

	Avo::DragDropOperation drag_and_drop_string(std::string_view p_string, Avo::Image const& p_dragImage,
		Avo::Point<> drag_image_cursor_position, uint64 additional_data = 0u) override
	{
	}
	Avo::DragDropOperation drag_and_drop_string(std::u16string_view p_string, Avo::Image const& p_dragImage,
		Avo::Point<> drag_image_cursor_position, uint64 additional_data = 0u) override
	{
	}

	Avo::DragDropOperation drag_and_drop_image(Avo::Image const& p_image, Avo::Image const& p_dragImage,
		Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}

	Avo::DragDropOperation drag_and_drop_file(uint8 const* data, uint32 p_dataSize, std::string_view p_name,
		Avo::Image const& p_dragImage, Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}
	Avo::DragDropOperation drag_and_drop_file(uint8 const* data, uint32 p_dataSize, std::u16string_view p_name,
		Avo::Image const& p_dragImage, Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}
	Avo::DragDropOperation drag_and_drop_file(std::vector<uint8> const& data, std::string_view p_name,
		Avo::Image const& p_dragImage, Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}
	Avo::DragDropOperation drag_and_drop_file(std::vector<uint8> const& data, std::u16string_view p_name, 
		Avo::Image const& p_dragImage, Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}
	Avo::DragDropOperation drag_and_drop_file(std::string_view path, Avo::Image const& p_dragImage, 
		Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}
	Avo::DragDropOperation drag_and_drop_file(std::u16string_view path, Avo::Image const& p_dragImage, 
		Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}

	Avo::DragDropOperation drag_and_drop_files(std::vector<std::string> const& p_paths, Avo::Image const& p_dragImage, 
		Avo::Point<> drag_image_cursor_position, uint64 additional_data = 0u) override
	{
	}
	Avo::DragDropOperation drag_and_drop_files(std::vector<std::u16string> const& p_paths, Avo::Image const& p_dragImage, 
		Avo::Point<> drag_image_cursor_position, uint64 additional_data) override
	{
	}

	//------------------------------

	void set_clipboard_string(std::u16string_view p_string, uint64 additional_data) const override
	{
	}
	void set_clipboard_string(std::string_view p_string, uint64 additional_data) const override
	{
	}

	void set_clipboard_image(Avo::Image const& p_image, uint64 additional_data) const override
	{
	}

	void set_clipboard_file(uint8 const* data, uint32 p_dataSize, std::string_view p_name, uint64 additional_data) const override
	{
	}
	void set_clipboard_file(uint8 const* data, uint32 p_dataSize, std::u16string_view p_name, uint64 additional_data) const override
	{
	}
	void set_clipboard_file(std::vector<uint8> const& data, std::string_view p_name, uint64 additional_data) const override
	{
	}
	void set_clipboard_file(std::vector<uint8> const& data, std::u16string_view p_name, uint64 additional_data) const override
	{
	}
	void set_clipboard_file(std::string_view path, uint64 additional_data) const override
	{
	}
	void set_clipboard_file(std::u16string_view path, uint64 additional_data) const override
	{
	}

	void set_clipboard_files(std::vector<std::string> const& p_paths, uint64 additional_data) const override
	{
	}
	void set_clipboard_files(std::vector<std::u16string> const& p_paths, uint64 additional_data) const override
	{
	}

	std::unique_ptr<Avo::ClipboardData> get_clipboard_data() const override
	{
		return nullptr;
	}

	LinuxWindow(Avo::Gui* gui) :
		_gui{gui}
	{
	}
	~LinuxWindow()
	{
		if (_messageThread.joinable())
		{
			_messageThread.join();
		}
	}
};
#endif

#ifdef _WIN32
auto convertRectangleToRectf(Avo::Rectangle<> const p_rectangle) -> D2D1_RECT_F
{
	return {p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom};
}
auto convertSizefToSize(D2D1_SIZE_F const size) -> Avo::Size<>
{
	return {size.width, size.height};
}
auto convertSizefToSize(Avo::Size<> const size) -> D2D1_SIZE_F
{
	return {size.width, size.height};
}


class Direct2dImage : public Avo::Image
{
private:
	Avo::Rectangle<> _cropRectangle;
public:
	auto set_crop_rectangle(Avo::Rectangle<> p_rectangle) -> void override
	{
		_cropRectangle = p_rectangle;
	}
	auto get_crop_rectangle() const -> Avo::Rectangle<> override
	{
		return _cropRectangle;
	}

	//------------------------------

private:
	Avo::ImageBoundsSizing _boundsSizing = Avo::ImageBoundsSizing::Stretch;
public:
	auto set_bounds_sizing(Avo::ImageBoundsSizing p_sizeMode) -> void override
	{
		_boundsSizing = p_sizeMode;
	}
	auto getBoundsSizing() const -> Avo::ImageBoundsSizing override
	{
		return _boundsSizing;
	}

private:
	Avo::Point<> _boundsPositioning{0.5f, 0.5f};
public:
	auto setBoundsPositioning(Avo::Point<Avo::Factor> p_position) -> void override
	{
		_boundsPositioning = p_position;
	}
	auto setBoundsPositioningX(Avo::Factor p_x) -> void override
	{
		_boundsPositioning.x = p_x;
	}
	auto setBoundsPositioningY(Avo::Factor p_y) -> void override
	{
		_boundsPositioning.y = p_y;
	}
	auto getBoundsPositioning() const -> Avo::Point<Avo::Factor> override
	{
		return _boundsPositioning;
	}
	auto getBoundsPositioningX() const -> float override
	{
		return _boundsPositioning.x;
	}
	auto getBoundsPositioningY() const -> float override
	{
		return _boundsPositioning.x;
	}

	//------------------------------

private:
	Avo::ImageScalingMethod _scalingMethod = Avo::ImageScalingMethod::Smooth;
public:
	auto setScalingMethod(Avo::ImageScalingMethod p_scalingMethod) -> void override
	{
		_scalingMethod = p_scalingMethod;
	}
	auto getScalingMethod() const -> Avo::ImageScalingMethod override
	{
		return _scalingMethod;
	}

	//------------------------------

private:
	float _opacity = 1.f;
public:
	auto set_opacity(Avo::Factor p_opacity) -> void override
	{
		_opacity = p_opacity;
	}
	auto get_opacity() const -> Avo::Factor override
	{
		return _opacity;
	}

	//------------------------------

private:
	ComReference<ID2D1Bitmap1> _image;
public:
	auto getD2dBitmap() const -> ID2D1Bitmap1*
	{
		return _image;
	}

	auto get_original_size() const -> Avo::Size<> override
	{
		auto const size = _image->GetSize();
		return {size.width, size.height};
	}
	auto get_original_width() const -> Avo::Dip override
	{
		return _image->GetSize().width;
	}
	auto get_original_height() const -> Avo::Dip override
	{
		return _image->GetSize().height;
	}

	auto get_original_pixel_size() const -> Avo::Size<Avo::Pixels> override
	{
		auto const size = _image->GetPixelSize();
		return {static_cast<Avo::Pixels>(size.width), static_cast<Avo::Pixels>(size.height)};
	}
	auto get_original_pixel_width() const -> Avo::Pixels override
	{
		return _image->GetPixelSize().width;
	}
	auto get_original_pixel_height() const -> Avo::Pixels override
	{
		return _image->GetPixelSize().height;
	}

	auto get_inner_width() const -> float override
	{
		if (_boundsSizing != Avo::ImageBoundsSizing::Stretch &&
		    _boundsSizing == Avo::ImageBoundsSizing::Contain != 
		    _bounds.get_width()/_bounds.get_height() < _image->GetSize().width/_image->GetSize().height)
		{
			return _bounds.get_height()*_image->GetSize().width/_image->GetSize().height;
		}
		return _bounds.get_width();
	}
	auto get_inner_height() const -> float override
	{
		if (_boundsSizing != Avo::ImageBoundsSizing::Stretch &&
		    _boundsSizing == Avo::ImageBoundsSizing::Contain != 
		    _bounds.get_width()/_bounds.get_height() > _image->GetSize().width/_image->GetSize().height)
		{
			return _bounds.get_width()*_image->GetSize().height/_image->GetSize().width;
		}
		return _bounds.get_height();
	}
	auto get_inner_size() const -> Avo::Point<> override
	{
		return {get_inner_width(), get_inner_height()};
	}
	auto get_inner_bounds() const -> Avo::Rectangle<> override
	{
		if (_boundsSizing == Avo::ImageBoundsSizing::Stretch)
		{
			return _bounds;
		}

		auto innerBounds = _bounds;

		auto const areBoundsProportionallyWider = _bounds.get_width()/_bounds.get_height() > _image->GetSize().width/_image->GetSize().height;
		if (_boundsSizing == Avo::ImageBoundsSizing::Fill != areBoundsProportionallyWider)
		{
			innerBounds.set_width(_bounds.get_height()*_image->GetSize().width/_image->GetSize().height);
		}
		else if (_boundsSizing == Avo::ImageBoundsSizing::Contain != areBoundsProportionallyWider)
		{
			innerBounds.set_height(_bounds.get_width()*_image->GetSize().height/_image->GetSize().width);
		}

		return innerBounds += Avo::Vector2d{_boundsPositioning*(_bounds.get_size() - innerBounds.get_size())};
	}

	//------------------------------

	Direct2dImage(ID2D1Bitmap1* p_image) :
		_image{p_image},
		_cropRectangle{0.f, 0.f, p_image->GetSize().width, p_image->GetSize().height}
	{
		_bounds = _cropRectangle;
	}
};

//------------------------------

class DirectWriteText : public Avo::Text
{
private:
	auto convertTextRange(TextRange const& p_range) -> DWRITE_TEXT_RANGE
	{
		return DWRITE_TEXT_RANGE{
			/*.startPosition =*/static_cast<UINT32>(p_range.length > 0 ? p_range.startPosition : 
		    	std::max(static_cast<Index>(0), p_range.startPosition - p_range.length)),
			/*.length =*/static_cast<UINT32>(p_range.length > 0 ? p_range.length : (p_range.length == 0 ? _string.size() : -p_range.length))
		};
	}

protected:
	auto handle_protected_rectangle_change(Avo::Rectangle<> p_old) -> void override
	{
		auto const size = _bounds.get_size();
		if (size.x != p_old.get_width())
		{
			_handle->SetMaxWidth(size.x);
		}
		if (size.y != p_old.get_height())
		{
			_handle->SetMaxHeight(size.y);
		}
	}

public:
	auto set_word_wrapping(Avo::WordWrapping p_wordWrapping) -> void override
	{
		switch (p_wordWrapping)
		{
			case Avo::WordWrapping::Always:
				_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
				break;
			case Avo::WordWrapping::Emergency:
				_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
				break;
			case Avo::WordWrapping::Never:
				_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
				break;
			case Avo::WordWrapping::WholeWord:
				_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_WHOLE_WORD);
		}
	}
	auto get_word_wrapping() const -> Avo::WordWrapping override
	{
		switch (_handle->GetWordWrapping())
		{
			case DWRITE_WORD_WRAPPING_WRAP:
				return Avo::WordWrapping::Always;
			case DWRITE_WORD_WRAPPING_EMERGENCY_BREAK:
				return Avo::WordWrapping::Emergency;
			case DWRITE_WORD_WRAPPING_NO_WRAP:
				return Avo::WordWrapping::Never;
			case DWRITE_WORD_WRAPPING_WHOLE_WORD:
				return Avo::WordWrapping::WholeWord;
		}
		return Avo::WordWrapping::Unknown;
	}

	auto fit_size_to_text() -> void override
	{
		DWRITE_TEXT_METRICS metrics;
		_handle->GetMetrics(&metrics);

		DWRITE_OVERHANG_METRICS overhangMetrics;
		_handle->GetOverhangMetrics(&overhangMetrics);

		ProtectedRectangle::set_size({metrics.width, _handle->GetMaxHeight() + overhangMetrics.bottom + _isTopTrimmed*overhangMetrics.top});
	}
	auto fit_width_to_text() -> void override
	{
		DWRITE_TEXT_METRICS metrics;
		_handle->GetMetrics(&metrics);
		ProtectedRectangle::set_width(metrics.width);
	}
	auto fit_height_to_text() -> void override
	{
		DWRITE_OVERHANG_METRICS overhangMetrics;
		_handle->GetOverhangMetrics(&overhangMetrics);

		ProtectedRectangle::set_height(_handle->GetMaxHeight() + overhangMetrics.bottom + _isTopTrimmed*overhangMetrics.top);
	}
	auto get_minimum_size() const -> Avo::Point<> override
	{
		DWRITE_TEXT_METRICS metrics;
		_handle->GetMetrics(&metrics);

		DWRITE_OVERHANG_METRICS overhangMetrics;
		_handle->GetOverhangMetrics(&overhangMetrics);

		return {metrics.width, _handle->GetMaxHeight() + overhangMetrics.bottom + _isTopTrimmed*overhangMetrics.top};
	}
	auto get_minimum_width() const -> float override
	{
		DWRITE_TEXT_METRICS metrics;
		_handle->GetMetrics(&metrics);
		return metrics.width;
	}
	auto get_minimum_height() const -> float override
	{
		DWRITE_OVERHANG_METRICS overhangMetrics;
		_handle->GetOverhangMetrics(&overhangMetrics);

		return _handle->GetMaxHeight() + overhangMetrics.bottom + _isTopTrimmed*overhangMetrics.top;
	}

	//------------------------------

private:
	bool _isTopTrimmed = false;
public:
	auto set_is_top_trimmed(bool p_isTopTrimmed) -> void override
	{
		_isTopTrimmed = p_isTopTrimmed;
	}
	auto get_is_top_trimmed() const -> bool override
	{
		return _isTopTrimmed;
	}

	//------------------------------

	auto get_character_position(Index p_characterIndex, bool p_isRelativeToOrigin) const -> Avo::Point<> override
	{
		Avo::Point<> result;
		DWRITE_HIT_TEST_METRICS metrics;
		_handle->HitTestTextPosition(
			Avo::getUnitIndexFromCharacterIndex(std::u16string_view{_wideString}, p_characterIndex), 
			false, &result.x, &result.y, &metrics
		);
		if (p_isRelativeToOrigin)
		{
			result += _bounds.get_top_left();
		}
		return result;
	}
	auto get_character_size(Index p_characterIndex) const -> Avo::Point<> override
	{
		float x;
		float y;
		DWRITE_HIT_TEST_METRICS metrics;
		_handle->HitTestTextPosition(
			Avo::getUnitIndexFromCharacterIndex(std::u16string_view{_wideString}, p_characterIndex), 
			false, &x, &y, &metrics
		);
		return {metrics.width, metrics.height};
	}
	auto get_character_bounds(Index p_characterIndex, bool p_isRelativeToOrigin) const -> Avo::Rectangle<> override
	{
		Avo::Rectangle<> result;
		DWRITE_HIT_TEST_METRICS metrics;
		_handle->HitTestTextPosition(
			Avo::getUnitIndexFromCharacterIndex(std::u16string_view{_wideString}, p_characterIndex), 
			false, &result.left, &result.top, &metrics
		);
		if (p_isRelativeToOrigin)
		{
			result.left += _bounds.left;
			result.top += _bounds.top;
		}
		result.right = result.left + metrics.width;
		result.bottom = result.top + metrics.height;
		return result;
	}
	auto get_nearest_character_index(Avo::Point<> p_point, bool p_isRelativeToOrigin) const -> Index override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics;

		if (p_isRelativeToOrigin)
		{
			p_point -= _bounds.get_top_left();
		}
		_handle->HitTestPoint(
			p_point.x, p_point.y, 
			&isTrailingHit, &isInside, &metrics
		);
		return Avo::getCharacterIndexFromUnitIndex(std::u16string_view{_wideString}, metrics.textPosition) + isTrailingHit;// *isInside;
	}
	auto get_nearest_character_index_and_position(Avo::Point<> p_point, bool p_isRelativeToOrigin) const -> std::pair<Index, Avo::Point<>> override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics;

		if (p_isRelativeToOrigin)
		{
			p_point -= _bounds.get_top_left();
		}
		_handle->HitTestPoint(
			p_point.x, p_point.y, 
			&isTrailingHit, &isInside, &metrics
		);
		return {
			Avo::getCharacterIndexFromUnitIndex(std::u16string_view{_wideString}, metrics.textPosition) + isTrailingHit,// *isInside,
			Avo::Point<>{
				metrics.left + isTrailingHit*metrics.width + p_isRelativeToOrigin*_bounds.left, 
				metrics.top + p_isRelativeToOrigin*_bounds.top
			}
		};
	}
	auto get_nearest_character_index_and_bounds(Avo::Point<> p_point, bool p_isRelativeToOrigin) const 
		-> std::pair<Index, Avo::Rectangle<>> override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics;

		if (p_isRelativeToOrigin)
		{
			p_point -= _bounds.get_top_left();
		}
		_handle->HitTestPoint(
			p_point.x, p_point.y, 
			&isTrailingHit, &isInside, &metrics
		);

		auto bounds = Avo::Rectangle<>{
			metrics.left + isTrailingHit*metrics.width + p_isRelativeToOrigin*_bounds.left,
			metrics.top + p_isRelativeToOrigin*_bounds.top
		};
		bounds.right = bounds.left + metrics.width;
		bounds.bottom = bounds.top + metrics.height;

		return {
			Avo::getCharacterIndexFromUnitIndex(std::u16string_view{_wideString}, metrics.textPosition) + isTrailingHit,
			bounds
		};
	}

	//------------------------------

	static auto convertTextAlignToDirectWrite(Avo::TextAlign p_textAlign)
	{
		switch (p_textAlign)
		{
			case Avo::TextAlign::Left:
				return DWRITE_TEXT_ALIGNMENT_LEADING;
			case Avo::TextAlign::Center:
				return DWRITE_TEXT_ALIGNMENT_CENTER;
			case Avo::TextAlign::Right:
				return DWRITE_TEXT_ALIGNMENT_TRAILING;
			case Avo::TextAlign::Fill:
				return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
		}
		return static_cast<DWRITE_TEXT_ALIGNMENT>(-1);
	}

	auto set_text_align(Avo::TextAlign p_textAlign) -> void override
	{
		_handle->SetTextAlignment(convertTextAlignToDirectWrite(p_textAlign));
	}
	auto get_text_align() const -> Avo::TextAlign override
	{
		switch (_handle->GetTextAlignment())
		{
			case DWRITE_TEXT_ALIGNMENT_LEADING:
				return Avo::TextAlign::Left;
			case DWRITE_TEXT_ALIGNMENT_CENTER:
				return Avo::TextAlign::Center;
			case DWRITE_TEXT_ALIGNMENT_TRAILING:
				return Avo::TextAlign::Right;
			case DWRITE_TEXT_ALIGNMENT_JUSTIFIED:
				return Avo::TextAlign::Fill;
		}
		return Avo::TextAlign::Unknown;
	}

	static auto convertReadingDirectionToDirectWrite(Avo::ReadingDirection p_readingDirection)
	{
		switch (p_readingDirection)
		{
			case Avo::ReadingDirection::LeftToRight:
				return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
			case Avo::ReadingDirection::RightToLeft:
				return DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
			case Avo::ReadingDirection::TopToBottom:
				return DWRITE_READING_DIRECTION_TOP_TO_BOTTOM;
			case Avo::ReadingDirection::BottomToTop:
				return DWRITE_READING_DIRECTION_BOTTOM_TO_TOP;
		}
		return static_cast<DWRITE_READING_DIRECTION>(-1);
	}

	auto setReadingDirection(Avo::ReadingDirection p_readingDirection) -> void override
	{
		_handle->SetReadingDirection(convertReadingDirectionToDirectWrite(p_readingDirection));
	}
	auto getReadingDirection() const -> Avo::ReadingDirection override
	{
		switch (_handle->GetReadingDirection())
		{
			case DWRITE_READING_DIRECTION_LEFT_TO_RIGHT:
				return Avo::ReadingDirection::LeftToRight;
			case DWRITE_READING_DIRECTION_RIGHT_TO_LEFT:
				return Avo::ReadingDirection::RightToLeft;
			case DWRITE_READING_DIRECTION_TOP_TO_BOTTOM:
				return Avo::ReadingDirection::TopToBottom;
			case DWRITE_READING_DIRECTION_BOTTOM_TO_TOP:
				return Avo::ReadingDirection::BottomToTop;
		}
		return Avo::ReadingDirection::Unknown;
	}

	//------------------------------

	auto set_font_family(std::string_view const p_name, TextRange const p_range) -> void override
	{
		char16 wideName[100];
		Avo::convert_utf8_to_utf16(p_name, wideName);

		_handle->SetFontFamilyName(reinterpret_cast<wchar_t const*>(wideName), convertTextRange(p_range));
	}

	//------------------------------

	auto set_character_spacing(float const p_leading, float const p_trailing, TextRange const p_range) -> void override
	{
		_handle->SetCharacterSpacing(p_leading, p_trailing, 0.f, convertTextRange(p_range));
	}
	auto get_leading_character_spacing(Index const p_characterIndex) const -> float override
	{
		float leadingSpacing;
		_handle->GetCharacterSpacing(p_characterIndex, &leadingSpacing, 0, 0);
		return leadingSpacing;
	}
	auto getTrailingCharacterSpacing(Index const p_characterIndex) const -> float override
	{
		float trailingSpacing;
		_handle->GetCharacterSpacing(p_characterIndex, 0, &trailingSpacing, 0);
		return trailingSpacing;
	}

	//------------------------------

	auto setLineHeight(float const p_lineHeight) -> void override
	{
		_handle->SetLineSpacing(DWRITE_LINE_SPACING_METHOD::DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, p_lineHeight, p_lineHeight*0.8f);
	}
	auto getLineHeight() const -> float override
	{
		DWRITE_LINE_SPACING_METHOD method;
		float height;
		float baseline;
		_handle->GetLineSpacing(&method, &height, &baseline);
		return height;
	}

	//------------------------------

	auto set_font_weight(Avo::FontWeight const p_fontWeight, TextRange const p_range) -> void override
	{
		_handle->SetFontWeight(static_cast<DWRITE_FONT_WEIGHT>(p_fontWeight), convertTextRange(p_range));
	}
	auto getFontWeight(Index const p_characterPosition) const -> Avo::FontWeight override
	{
		DWRITE_FONT_WEIGHT fontWeight;
		_handle->GetFontWeight(p_characterPosition, &fontWeight);
		return static_cast<Avo::FontWeight>(fontWeight);
	}

	//------------------------------

	auto setFontStyle(Avo::FontStyle const p_fontStyle, TextRange const p_range) -> void override
	{
		_handle->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(p_fontStyle), convertTextRange(p_range));
	}
	auto getFontStyle(Index const p_characterPosition) const -> Avo::FontStyle override
	{
		DWRITE_FONT_STYLE directwriteFontStyle;
		_handle->GetFontStyle(p_characterPosition, &directwriteFontStyle);
		return static_cast<Avo::FontStyle>(directwriteFontStyle);
	}

	//------------------------------

	auto setFontStretch(Avo::FontStretch const p_fontStretch, TextRange const p_range) -> void override
	{
		_handle->SetFontStretch(static_cast<DWRITE_FONT_STRETCH>(p_fontStretch), convertTextRange(p_range));
	}
	auto getFontStretch(Index const p_characterPosition) -> Avo::FontStretch
	{
		DWRITE_FONT_STRETCH fontStretch;
		_handle->GetFontStretch(p_characterPosition, &fontStretch);
		return static_cast<Avo::FontStretch>(fontStretch);
	}

	//------------------------------

	auto set_font_size(float const p_fontSize, TextRange const p_range) -> void override
	{
		_handle->SetFontSize(p_fontSize, convertTextRange(p_range));
	}
	auto get_font_size(Index const p_characterPosition) const -> float override
	{
		float fontSize;
		_handle->GetFontSize(p_characterPosition, &fontSize);
		return fontSize;
	}

	//------------------------------

private:
	std::u16string _wideString;
	std::string _string;
public:
	auto get_string() const -> std::string_view override
	{
		return _string;
	}

	//------------------------------

private:
	ComReference<IDWriteTextLayout1> _handle;
public:
	auto getDWriteTextLayout() const -> IDWriteTextLayout1*
	{
		return _handle;
	}

	//------------------------------

	DirectWriteText(
		IDWriteTextLayout1* const p_handle, std::u16string_view const p_wideString, 
	    std::string_view const p_string, Avo::Rectangle<float> const p_bounds
	) : 
		_handle{p_handle},
		_wideString{p_wideString},
		_string{p_string}
	{
		_bounds = p_bounds; // Belongs to ProtectedRectangle
		if (!_bounds.right && !_bounds.bottom)
		{
			_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			fit_size_to_text();
		}
		else
		{
			_handle->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
		}
	}
};

//class FontData
//{
//public:
//	void const* data;
//	uint32 dataSize;
//
//	FontData(void const* data, uint32 p_dataSize) :
//		data(data), dataSize(p_dataSize)
//	{}
//};
using FontData = std::shared_ptr<Avo::DataVector>;

class FontFileStream : public IDWriteFontFileStream
{
	IUnknownDefinition(IDWriteFontFileStream)

	//------------------------------

private:
	FontData _fontData;
public:
	auto __stdcall ReadFileFragment(
		void const** const p_fragment, UINT64 const p_fileOffset, 
	    UINT64 const p_fragmentSize, void** const p_fragmentContext
	) -> HRESULT override
	{
		if (p_fileOffset + p_fragmentSize > _fontData->size() || !p_fragmentSize)
		{
			*p_fragment = nullptr;
			*p_fragmentContext = nullptr;
			return E_FAIL;
		}

		*p_fragment = _fontData->data() + p_fileOffset;
		*p_fragmentContext = nullptr;

		return S_OK;
	}
	auto __stdcall ReleaseFileFragment(void* const p_fragmentContext) -> void override {}

	auto __stdcall GetFileSize(UINT64* const p_fileSize) -> HRESULT override
	{
		*p_fileSize = _fontData->size();
		return S_OK;
	}
	auto __stdcall GetLastWriteTime(UINT64* const p_lastWriteTime) -> HRESULT override
	{
		*p_lastWriteTime = {};
		return E_NOTIMPL;
	}

	//------------------------------

	FontFileStream(FontData const& p_fontData) :
		_fontData{p_fontData}
	{
	}
};

class FontFileLoader : public IDWriteFontFileLoader
{
	IUnknownDefinition(IDWriteFontFileLoader)

	//------------------------------

	auto __stdcall CreateStreamFromKey(
		void const* const data, UINT32 const p_dataSize, 
	    IDWriteFontFileStream** const p_stream) -> HRESULT override
	{
		if (p_dataSize != sizeof(FontData) || !data)
		{
			*p_stream = nullptr;
			return E_INVALIDARG;
		}
		*p_stream = new FontFileStream{*static_cast<FontData const*>(data)};
		return S_OK;
	}
};
class FontFileEnumerator : public IDWriteFontFileEnumerator
{
	IUnknownDefinition(IDWriteFontFileEnumerator)

	//------------------------------

private:
	IDWriteFontFile* _currentFontFile = nullptr;
public:
	auto __stdcall GetCurrentFontFile(IDWriteFontFile** const p_fontFile) -> HRESULT override
	{
		*p_fontFile = _currentFontFile;
		if (_currentFontFile)
		{
			return S_OK;
		}
		return E_FAIL;
	}

private:
	IDWriteFactory* _factory;
	FontFileLoader* _fontFileLoader;

	std::vector<FontData> const* _fontData;
	Index _currentFontFileIndex = -1;
public:
	auto __stdcall MoveNext(BOOL* const p_hasCurrentFile) -> HRESULT override
	{
		_currentFontFileIndex++;

		if (_currentFontFileIndex >= _fontData->size())
		{
			*p_hasCurrentFile = false;
			_currentFontFile = nullptr;
		}
		else
		{
			*p_hasCurrentFile = 1;
			_factory->CreateCustomFontFileReference(
				static_cast<void const*>(_fontData->data() + _currentFontFileIndex), 
				sizeof(FontData), _fontFileLoader, &_currentFontFile
			);
		}
		return S_OK;
	}

public:
	FontFileEnumerator(
		IDWriteFactory* const p_factory, 
		FontFileLoader* const p_fontFileLoader, 
		std::vector<FontData> const* const data
	) :
		_factory{p_factory},
		_fontFileLoader{p_fontFileLoader},
		_fontData{data}
	{}
};

class FontCollectionLoader : public IDWriteFontCollectionLoader
{
	IUnknownDefinition(IDWriteFontCollectionLoader)

	//------------------------------

	auto __stdcall CreateEnumeratorFromKey(
		IDWriteFactory* const p_factory, 
		void const* const data, UINT32 const p_dataSize, 
	    IDWriteFontFileEnumerator** const p_fontFileEnumerator
	) -> HRESULT
	{
		*p_fontFileEnumerator = new FontFileEnumerator{
			p_factory, _fontFileLoader, 
			static_cast<std::vector<FontData> const*>(data)
		};
		return S_OK;
	}

private:
	FontFileLoader* _fontFileLoader;
public:
	FontCollectionLoader(FontFileLoader* const p_fontFileLoader) :
		_fontFileLoader{p_fontFileLoader}
	{
	}
};

//------------------------------

class Direct2dGeometry : public Avo::Geometry
{
private:
	ComReference<ID2D1GeometryRealization> _strokedRealization;

public:
	auto setStrokedRealization(ComReference<ID2D1GeometryRealization> const& p_realization) -> void
	{
		_strokedRealization = p_realization;
	}
	auto getStrokedRealization() const -> ID2D1GeometryRealization*
	{
		return _strokedRealization;
	}

private:
	ComReference<ID2D1GeometryRealization> _filledRealization;
public:
	auto setFilledRealization(ComReference<ID2D1GeometryRealization> const& p_realization) -> void
	{
		_filledRealization = p_realization;
	}
	auto getFilledRealization() const -> ID2D1GeometryRealization*
	{
		return _filledRealization;
	}

private:
	ComReference<ID2D1Geometry> _geometry;
public:
	auto getGeometry() const -> ID2D1Geometry*
	{
		return _geometry;
	}

	Direct2dGeometry(ComReference<ID2D1Geometry> const& p_geometry) :
		_geometry{p_geometry}
	{
	}
};

//------------------------------

struct Direct2dLinearGradient : public Avo::LinearGradient
{
	auto set_offset(Avo::Point<> const offset) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(offset.x, offset.y));
	}
	auto setOffsetX(Avo::Dip const p_x) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, getOffsetY()));
	}
	auto setOffsetY(Avo::Dip const p_y) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(getOffsetX(), p_y));
	}
	auto getOffset() const -> Avo::Point<> override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return Avo::Point<>(matrix._31, matrix._32);
	}
	auto getOffsetX() const -> float override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return matrix._31;
	}
	auto getOffsetY() const -> float override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return matrix._32;
	}

	auto setStartPosition(Avo::Point<> const p_position) -> void override
	{
		_brush->SetStartPoint({p_position.x, p_position.y});
	}
	auto getStartPosition() const -> Avo::Point<> override
	{
		return {_brush->GetStartPoint().x, _brush->GetStartPoint().y};
	}
	auto getStartPositionX() const -> Avo::Dip override
	{
		return _brush->GetStartPoint().x;
	}
	auto getStartPositionY() const -> Avo::Dip override
	{
		return _brush->GetStartPoint().y;
	}

	auto setEndPosition(Avo::Point<> const p_position) -> void override
	{
		_brush->SetEndPoint({p_position.x, p_position.y});
	}
	auto getEndPosition() const -> Avo::Point<> override
	{
		return {_brush->GetEndPoint().x, _brush->GetEndPoint().y};
	}
	auto getEndPositionX() const -> float override
	{
		return _brush->GetEndPoint().x;
	}
	auto getEndPositionY() const -> float override
	{
		return _brush->GetEndPoint().y;
	}

private:
	ComReference<ID2D1LinearGradientBrush> _brush;

public:
	auto getD2dBrush() const -> ID2D1LinearGradientBrush*
	{
		return _brush;
	}

	Direct2dLinearGradient(ComReference<ID2D1LinearGradientBrush> const& p_brush) :
		_brush{p_brush}
	{
	}
};

//------------------------------

struct Direct2dRadialGradient : public Avo::RadialGradient
{
	auto set_offset(Avo::Point<> const offset) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(offset.x, offset.y));
	}
	auto setOffsetX(Avo::Dip const p_x) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, getOffsetY()));
	}
	auto setOffsetY(Avo::Dip const p_y) -> void override
	{
		_brush->SetTransform(D2D1::Matrix3x2F::Translation(getOffsetX(), p_y));
	}
	auto getOffset() const -> Avo::Point<> override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return {matrix._31, matrix._32};
	}
	auto getOffsetX() const -> float override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return matrix._31;
	}
	auto getOffsetY() const -> float override
	{
		D2D1::Matrix3x2F matrix;
		_brush->GetTransform(&matrix);
		return matrix._32;
	}

	auto setStartPosition(Avo::Point<> const offset) -> void override
	{
		_brush->SetCenter({offset.x, offset.y});
	}
	auto getStartPosition() const -> Avo::Point<> override
	{
		return {_brush->GetCenter().x, _brush->GetCenter().y};
	}
	auto getStartPositionX() const -> Avo::Dip override
	{
		return _brush->GetCenter().x;
	}
	auto getStartPositionY() const -> Avo::Dip override
	{
		return _brush->GetCenter().y;
	}

	auto setRadius(Avo::Size<> const p_radius) -> void override
	{
		_brush->SetRadiusX(p_radius.x);
		_brush->SetRadiusY(p_radius.y);
	}
	auto getRadius() const -> Avo::Size<> override
	{
		return {_brush->GetRadiusX(), _brush->GetRadiusY()};
	}
	auto getRadiusX() const -> float override
	{
		return _brush->GetRadiusX();
	}
	auto getRadiusY() const -> float override
	{
		return _brush->GetRadiusY();
	}

private:
	ComReference<ID2D1RadialGradientBrush> _brush;
public:
	auto getD2dBrush() const -> ID2D1RadialGradientBrush*
	{
		return _brush;
	}

	Direct2dRadialGradient(ComReference<ID2D1RadialGradientBrush> const& p_brush) :
		_brush{p_brush}
	{}
};

//------------------------------

struct Direct2DDrawingState : public Avo::DrawingState
{
private:
	ComReference<ID2D1DrawingStateBlock1> _drawingState;

public:
	auto getD2dStateBlock() -> ID2D1DrawingStateBlock1*
	{
		return _drawingState;
	}

	Direct2DDrawingState(ComReference<ID2D1DrawingStateBlock1> const& p_drawingState) :
		_drawingState{p_drawingState}
	{}
};

//------------------------------

class Direct2dDrawingContext : public Avo::DrawingContext
{
public:
	static ComReference<IWICImagingFactory2> s_imagingFactory;
	static ComReference<ID2D1Factory2> s_direct2DFactory;
	static ComReference<IDWriteFactory1> s_directWriteFactory;

	static ComReference<FontCollectionLoader> s_fontCollectionLoader;
	static ComReference<FontFileLoader> s_fontFileLoader;

	static auto createStaticResources() -> void
	{
		if (!s_imagingFactory)
		{
			CoInitialize(0);
			CoCreateInstance(CLSID_WICImagingFactory2, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&s_imagingFactory));
		}

		//------------------------------

		if (!s_direct2DFactory)
		{
#ifdef _DEBUG
			D2D1_FACTORY_OPTIONS options;
			options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &s_direct2DFactory);
#else
			D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &s_direct2DFactory);
#endif
		}

		if (!s_directWriteFactory)
		{
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(s_directWriteFactory), 
				reinterpret_cast<IUnknown**>(&s_directWriteFactory)
			);

			s_fontFileLoader = make_com_reference<FontFileLoader>();
			s_directWriteFactory->RegisterFontFileLoader(s_fontFileLoader);

			s_fontCollectionLoader = make_com_reference<FontCollectionLoader>(s_fontFileLoader);
			s_directWriteFactory->RegisterFontCollectionLoader(s_fontCollectionLoader);
		}
	}
	static auto destroyStaticResources() -> void
	{
		if (!WindowsWindow::s_number_of_windows)
		{
			if (s_directWriteFactory)
			{
				s_directWriteFactory->UnregisterFontCollectionLoader(s_fontCollectionLoader);
				s_directWriteFactory->UnregisterFontFileLoader(s_fontFileLoader);
			}
			s_fontFileLoader = nullptr;
			s_fontCollectionLoader = nullptr;
			s_directWriteFactory = nullptr;
			s_direct2DFactory = nullptr;
			s_imagingFactory = nullptr;
		}
	}

	struct Direct2dLock
	{
		ComReference<ID2D1Multithread> multithreader;
		Direct2dLock()
		{
			s_direct2DFactory->QueryInterface(IID_PPV_ARGS(&multithreader));
			multithreader->Enter();
		}
		~Direct2dLock()
		{
			multithreader->Leave();
		}
		
		Direct2dLock(Direct2dLock const&) = delete;
		Direct2dLock(Direct2dLock&&) = delete;
		auto operator=(Direct2dLock const&) -> auto& = delete;
		auto operator=(Direct2dLock&&) -> auto& = delete;
	};

private:
	Avo::Window* _window = nullptr;

	ComReference<ID2D1DeviceContext1> _context;
	ComReference<IDXGISwapChain1> _swapChain;

	ComReference<ID2D1SolidColorBrush> _solidColorBrush;
	ComReference<ID2D1Brush> _currentBrush;
	float _brushOpacity = 1.f;

	//------------------------------

	auto createCornerRectanglePathGeometry(
		Avo::Rectangle<> const p_rectangle, 
		Avo::RectangleCorners const& p_corners, 
		bool const p_isFilled
	) -> ComReference<ID2D1PathGeometry1>
	{
		ComReference<ID2D1PathGeometry1> geometry;
		s_direct2DFactory->CreatePathGeometry(&geometry);

		ComReference<ID2D1GeometrySink> sink;
		geometry->Open(&sink);

		sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		sink->BeginFigure(
			{p_rectangle.left, p_rectangle.top + p_corners.top_left_size_y}, 
			p_isFilled ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW
		);

		if (p_corners.top_left_size_x && p_corners.top_left_size_y)
		{
			if (p_corners.top_left_type == Avo::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						{p_rectangle.left + p_corners.top_left_size_x, p_rectangle.top}, 
						{p_corners.top_left_size_x, p_corners.top_left_size_y}, 
						0.f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine({p_rectangle.left + p_corners.top_left_size_x, p_rectangle.top});
			}
		}
		sink->AddLine({p_rectangle.right - p_corners.top_right_size_x, p_rectangle.top});
		if (p_corners.top_right_size_x && p_corners.top_right_size_y)
		{
			if (p_corners.top_right_type == Avo::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						{p_rectangle.right, p_rectangle.top + p_corners.top_right_size_y}, 
						{p_corners.top_right_size_x, p_corners.top_right_size_y}, 
						0.f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine({p_rectangle.right, p_rectangle.top + p_corners.top_right_size_y});
			}
		}
		sink->AddLine({p_rectangle.right, p_rectangle.bottom - p_corners.bottom_right_size_y});
		if (p_corners.bottom_right_size_x && p_corners.bottom_right_size_y)
		{
			if (p_corners.bottom_right_type == Avo::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						{p_rectangle.right - p_corners.bottom_right_size_x, p_rectangle.bottom}, 
						{p_corners.bottom_right_size_x, p_corners.bottom_right_size_y}, 
						0.f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine({p_rectangle.right - p_corners.bottom_right_size_x, p_rectangle.bottom});
			}
		}
		sink->AddLine({p_rectangle.left + p_corners.bottom_left_size_x, p_rectangle.bottom});
		if (p_corners.bottom_left_size_x && p_corners.bottom_left_size_y)
		{
			if (p_corners.bottom_left_type == Avo::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						{p_rectangle.left, p_rectangle.bottom - p_corners.bottom_left_size_y}, 
						{p_corners.bottom_left_size_x, p_corners.bottom_left_size_y}, 
						0.f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine({p_rectangle.left, p_rectangle.bottom - p_corners.bottom_left_size_y});
			}
		}
		sink->AddLine({p_rectangle.left, p_rectangle.top + p_corners.top_left_size_y});

		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		sink->Close();

		return geometry;
	}

	D2D1_STROKE_STYLE_PROPERTIES1 _strokeStyleProperties;
	ComReference<ID2D1StrokeStyle1> _strokeStyle;
	auto updateStrokeStyle() -> void
	{
		_strokeStyle = nullptr; // Releases, important
		s_direct2DFactory->CreateStrokeStyle(_strokeStyleProperties, 0, 0, &_strokeStyle);
	}

	//------------------------------

	auto realizeStrokedGeometry(Direct2dGeometry* const p_geometry, float const p_strokeWidth) -> void
	{
		if (!p_geometry->getStrokedRealization())
		{
			D2D1_MATRIX_3X2_F transform;
			_context->GetTransform(&transform);

			auto dpiX = float{USER_DEFAULT_SCREEN_DPI};
			auto dpiY = float{USER_DEFAULT_SCREEN_DPI};
			_context->GetDpi(&dpiX, &dpiY);

			auto geometryRealization = ComReference<ID2D1GeometryRealization>{};
			_context->CreateStrokedGeometryRealization(
				p_geometry->getGeometry(), 
				D2D1::ComputeFlatteningTolerance(transform, dpiX, dpiY), 
				p_strokeWidth, _strokeStyle, &geometryRealization
			);

			p_geometry->setStrokedRealization(geometryRealization);
		}
	}
	auto realizeFilledGeometry(Direct2dGeometry* const p_geometry) -> void
	{
		if (!p_geometry->getFilledRealization())
		{
			D2D1_MATRIX_3X2_F transform;
			_context->GetTransform(&transform);

			auto dpiX = float{USER_DEFAULT_SCREEN_DPI};
			auto dpiY = float{USER_DEFAULT_SCREEN_DPI};
			_context->GetDpi(&dpiX, &dpiY);

			auto geometryRealization = ComReference<ID2D1GeometryRealization>{};
			_context->CreateFilledGeometryRealization(
				p_geometry->getGeometry(), 
				D2D1::ComputeFlatteningTolerance(transform, dpiX, dpiY), 
				&geometryRealization
			);

			p_geometry->setFilledRealization(geometryRealization);
		}
	}

	std::recursive_mutex _targetMutex;
public:
	auto beginDrawing() -> void override
	{
		_targetMutex.lock();
		_context->BeginDraw();
	}
	auto finishDrawing() -> void override
	{
		_context->EndDraw();
		_targetMutex.unlock();

		auto const lock = Direct2dLock{};
		_swapChain->Present(1, _isVsyncEnabled ? 0 : (DXGI_PRESENT_DO_NOT_WAIT | DXGI_PRESENT_RESTART));
	}

	auto createDrawingState() -> Avo::DrawingState override
	{
		auto drawingState = ComReference<ID2D1DrawingStateBlock1>{};
		s_direct2DFactory->CreateDrawingStateBlock(&drawingState);
		return createDrawingStateFromImplementation(std::make_shared<Direct2DDrawingState>(drawingState));
	}
	auto saveDrawingState(Avo::DrawingState const& p_drawingState) -> void override
	{
		_context->SaveDrawingState(static_cast<Direct2DDrawingState*>(p_drawingState.get_implementation())->getD2dStateBlock());
	}
	auto restoreDrawingState(Avo::DrawingState const& p_drawingState) -> void override
	{
		_context->RestoreDrawingState(static_cast<Direct2DDrawingState*>(p_drawingState.get_implementation())->getD2dStateBlock());
	}

	//------------------------------

	auto set_is_fullscreen(bool p_isFullscreen) -> void override
	{
		_window->set_is_fullscreen(p_isFullscreen);
	}
	auto switchFullscreen() -> void override
	{
		_window->switchFullscreen();
	}
	auto getIsFullscreen() -> bool override
	{
		return _window->getIsFullscreen();
	}

	//------------------------------

private:
	bool _isVsyncEnabled = true;
public:
	auto enableVsync() -> void override
	{
		_isVsyncEnabled = true;
	}
	auto disableVsync() -> void override 
	{
		_isVsyncEnabled = false;
	}
	auto getIsVsyncEnabled() -> bool override 
	{
		return _isVsyncEnabled;
	}

	//------------------------------

	auto setBackgroundColor(Avo::Color const p_color) -> void override
	{
		if (_swapChain)
		{
			auto const dxgiColor = DXGI_RGBA{
				p_color.red,
				p_color.green,
				p_color.blue,
				p_color.alpha
			};

			auto const lock = Direct2dLock{};
			_swapChain->SetBackgroundColor(&dxgiColor);
		}
	}
	auto getBackgroundColor() -> Avo::Color override
	{
		if (_swapChain)
		{
			auto const lock = Direct2dLock{};

			auto dxgiColor = DXGI_RGBA{};
			_swapChain->GetBackgroundColor(&dxgiColor);

			return {dxgiColor.r, dxgiColor.g, dxgiColor.b, dxgiColor.a};
		}
		return 0.5f;
	}

	//------------------------------

	auto setDpi(float const p_dpi) -> void override
	{
		_context->SetDpi(p_dpi, p_dpi);
	}
	auto getDpi() -> float override
	{
		auto DPIX = float{};
		auto DPIY = float{};
		_context->GetDpi(&DPIX, &DPIY);
		return DPIX;
	}

	//------------------------------

private:
	auto getD2dTransform() -> D2D1_MATRIX_3X2_F
	{
		D2D1_MATRIX_3X2_F transform;
		_context->GetTransform(&transform);
		return transform;
	}

	template<typename Lambda>
	auto changeD2dTransform(Lambda p_change) -> void
	{
		D2D1_MATRIX_3X2_F transform;
		_context->GetTransform(&transform);
		p_change(transform);
		_context->SetTransform(transform);
	}
	
	//------------------------------

public:
	auto move_origin(Avo::Vector2d<> const offset) -> void override
	{
		changeD2dTransform([&](D2D1_MATRIX_3X2_F& transform){
			transform.dx += offset.x;
			transform.dy += offset.y;
		});
	}
	auto setOrigin(Avo::Point<> const origin) -> void override
	{
		changeD2dTransform([&](D2D1_MATRIX_3X2_F& transform){
			transform.dx = origin.x;
			transform.dy = origin.y;
		});
	}
	auto getOrigin() -> Avo::Point<> override
	{
		auto const transform = getD2dTransform();
		return {transform.dx, transform.dy};
	}

	//------------------------------

private:
	Avo::Vector2d<Avo::Factor> _scale = 1.f;
public:
	auto scale(Avo::Vector2d<Avo::Factor> const p_scale) -> void override
	{
		changeD2dTransform([&](D2D1_MATRIX_3X2_F& transform){
			transform.m11 *= p_scale.x;
			transform.m22 *= p_scale.y;
			transform.m21 *= p_scale.x;
			transform.m12 *= p_scale.y;
			_scale.x *= p_scale.x;
			_scale.y *= p_scale.y;
		});
	}
	auto scale(Avo::Vector2d<Avo::Factor> const p_scale, Avo::Point<> const origin) -> void override
	{
		changeD2dTransform([&](D2D1_MATRIX_3X2_F& transform){
			transform.m11 *= p_scale.x;
			transform.m22 *= p_scale.y;
			transform.m21 *= p_scale.x;
			transform.m12 *= p_scale.y;
			_scale.x *= p_scale.x;
			_scale.y *= p_scale.y;
			transform.dx += (origin.x - transform.dx)*(1.f - p_scale.x);
			transform.dy += (origin.y - transform.dy)*(1.f - p_scale.y);
		});
	}
	auto set_scale(Avo::Vector2d<Avo::Factor> const p_scale) -> void override
	{
		scale(p_scale/_scale);	
	}
	auto set_scale(Avo::Vector2d<Avo::Factor> const p_scale, Avo::Point<> const origin) -> void override
	{
		scale(p_scale/_scale, origin);
	}
	auto get_scale() -> Avo::Vector2d<Avo::Factor> override
	{
		return _scale;
	}
	auto get_scale_x() -> Avo::Factor override
	{
		return _scale.x;
	}
	auto get_scale_y() -> Avo::Factor override
	{
		return _scale.y;
	}

	//------------------------------

	auto rotate(Avo::Arithmetic<float, Avo::Radians> const p_radians) -> void override
	{
		rotate(Avo::Degrees{p_radians});
	}
	auto rotate(Avo::Arithmetic<float, Avo::Degrees> const p_degrees) -> void override
	{
		_context->SetTransform(getD2dTransform()*D2D1::Matrix3x2F::Rotation(p_degrees.value));
	}
	auto rotate(Avo::Arithmetic<float, Avo::Radians> const p_radians, Avo::Point<> const origin) -> void override
	{
		rotate(Avo::Degrees{p_radians}, origin);
	}
	auto rotate(Avo::Arithmetic<float, Avo::Degrees> const p_degrees, Avo::Point<> const origin) -> void override
	{
		_context->SetTransform(
			getD2dTransform()*D2D1::Matrix3x2F::Rotation(p_degrees.value, {origin.x, origin.y})
		);
	}

	//------------------------------

	auto reset_transformations() -> void override {
		_context->SetTransform(D2D1::Matrix3x2F::Identity());
		_scale.x = 1.f;
		_scale.y = 1.f;
	}

	//------------------------------

private:
	ComReference<ID2D1Bitmap1> _target_window_bitmap;
	
	auto create_target_window_bitmap_from_dxgi_back_buffer(IDXGISurface* const back_buffer, float const p_dpi) -> void
	{
		_context->CreateBitmapFromDxgiSurface(
			back_buffer,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
				p_dpi, p_dpi
			),
			&_target_window_bitmap
		);
	}

public:
	auto set_size(Avo::Size<> const size) -> void override
	{
		if (auto const oldSize = _context->GetSize();
		    oldSize.width == size.x && oldSize.height == size.y)
		{
			return;
		}

		auto const targetLock = std::scoped_lock{_targetMutex};

		auto const wasOldTargetWindow = [&]() -> bool {
			ComReference<ID2D1Image> oldTarget;
			_context->GetTarget(&oldTarget); // Adds a reference which will be removed by ~ComReference()
			return oldTarget == _target_window_bitmap;
		}();
		if (wasOldTargetWindow)
		{
			// Need to temporarily remove the window bitmap as the target for drawing, because otherwise it 
			// won't be deleted when DXGISwapChain1::ResizeBuffers is called (which is required).
			_context->SetTarget(nullptr);
		}
		_target_window_bitmap = nullptr; // Releases our reference

		auto const dpi = getDpi();

		auto newDxgiBackBuffer = ComReference<IDXGISurface>{};
		{
			// We'll be doing DXGI calls which aren't automatically protected by Direct2D. 
			auto const lock = Direct2dLock{}; 

			// Resize buffers, creating new ones
			auto const newSize = size*(dpi/USER_DEFAULT_SCREEN_DPI);
			_swapChain->ResizeBuffers(0, newSize.x, newSize.y, DXGI_FORMAT_UNKNOWN, 0);

			// Get the new back buffer
			_swapChain->GetBuffer(0, IID_PPV_ARGS(&newDxgiBackBuffer));
		}

		create_target_window_bitmap_from_dxgi_back_buffer(newDxgiBackBuffer, dpi);

		if (wasOldTargetWindow)
		{
			_context->SetTarget(_target_window_bitmap);
		}
	}
	auto get_size() -> Avo::Size<> override
	{
		auto const size = _context->GetSize();
		return {size.width, size.height};
	}

	//------------------------------

	auto clear(Avo::Color const p_color) -> void override
	{
		_context->Clear({p_color.red, p_color.green, p_color.blue, p_color.alpha});
	}
	auto clear() -> void override
	{
		_context->Clear({});
	}

	//------------------------------

	auto fill_rectangle(Avo::Rectangle<> const p_rectangle) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->FillRectangle(convertRectangleToRectf(p_rectangle), _currentBrush);
	}
	auto fill_rectangle(Avo::Rectangle<> const p_rectangle, Avo::RectangleCorners const& p_corners) -> void override
	{
		auto const pathGeometry = createCornerRectanglePathGeometry(p_rectangle, p_corners, true);

		_currentBrush->SetOpacity(_brushOpacity);
		_context->FillGeometry(pathGeometry, _currentBrush);
	}
	auto fill_rounded_rectangle(Avo::Rectangle<> const p_rectangle, Avo::Size<> const p_radius) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->FillRoundedRectangle(
			D2D1::RoundedRect(convertRectangleToRectf(p_rectangle), p_radius.width, p_radius.height), 
			_currentBrush
		);
	}

	//------------------------------

	auto stroke_rectangle(Avo::Rectangle<> const p_rectangle, float const p_strokeWidth) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawRectangle(
			convertRectangleToRectf(p_rectangle), 
			_currentBrush, p_strokeWidth, _strokeStyle
		);
	}
	auto stroke_rectangle(
		Avo::Rectangle<> const p_rectangle, 
		Avo::RectangleCorners const& p_corners, 
		float const p_strokeWidth
	) -> void override
	{
		auto const pathGeometry = createCornerRectanglePathGeometry(p_rectangle, p_corners, false);

		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawGeometry(pathGeometry, _currentBrush, p_strokeWidth, _strokeStyle);
	}
	auto stroke_rounded_rectangle(Avo::Rectangle<> const p_rectangle, Avo::Size<> const p_radius, Avo::Dip const p_strokeWidth) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawRoundedRectangle(
			D2D1::RoundedRect(convertRectangleToRectf(p_rectangle), p_radius.width, p_radius.height), 
			_currentBrush, p_strokeWidth, _strokeStyle
		);
	}

	//------------------------------

	auto fill_circle(Avo::Point<> p_center, Avo::Dip p_radius) -> void override
	{
		fill_ellipse(p_center, p_radius);
	}
	auto fill_ellipse(Avo::Point<> p_center, Avo::Size<> p_radius) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->FillEllipse(
			D2D1::Ellipse({p_center.x, p_center.y}, p_radius.width, p_radius.height), 
			_currentBrush
		);
	}

	auto stroke_circle(Avo::Point<> p_center, Avo::Dip p_radius, Avo::Dip p_strokeWidth) -> void override
	{
		stroke_ellipse(p_center, p_radius, p_strokeWidth);
	}
	auto stroke_ellipse(Avo::Point<> p_center, Avo::Size<> p_radius, Avo::Dip p_strokeWidth) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawEllipse(
			D2D1::Ellipse({p_center.x, p_center.y}, p_radius.x, p_radius.y), 
			_currentBrush, p_strokeWidth, _strokeStyle
		);
	}

	//------------------------------

	auto draw_line(Avo::Point<> p_0, Avo::Point<> p_1, Avo::Dip p_thickness) -> void override
	{
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawLine(
			{p_0.x, p_0.y},
			{p_1.x, p_1.y},
			_currentBrush,
			p_thickness, _strokeStyle
		);
	}

	//------------------------------

private:
	static auto createPathGeometry(Range<Avo::Point<> const*> const p_vertices, bool const p_isStroked, bool const p_isClosed)
		-> ComReference<ID2D1PathGeometry1>
	{
		auto path = ComReference<ID2D1PathGeometry1>{};
		s_direct2DFactory->CreatePathGeometry(&path);

		auto sink = ComReference<ID2D1GeometrySink>{};
		path->Open(&sink);

		sink->BeginFigure({p_vertices[0].x, p_vertices[0].y}, p_isStroked ? D2D1_FIGURE_BEGIN_HOLLOW : D2D1_FIGURE_BEGIN_FILLED);
		for (auto const a : Indices{1, p_vertices})
		{
			sink->AddLine({p_vertices[a].x, p_vertices[a].y});
		}
		sink->EndFigure(p_isClosed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

		sink->Close();
		return path;
	}
	static auto getD2dGeometryFromGeometry(Avo::Geometry const& p_geometry) -> Direct2dGeometry*
	{
		return static_cast<Direct2dGeometry*>(p_geometry.get_implementation());
	}

public:
	auto stroke_shape(Range<Avo::Point<>*> const p_vertices, float const p_lineThickness, bool const p_isClosed) -> void override
	{
		if (!p_vertices.size())
		{
			return;
		}

		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawGeometry(createPathGeometry(p_vertices, true, p_isClosed), _currentBrush, p_lineThickness, _strokeStyle);
	}

	auto fill_shape(Range<Avo::Point<>*> const p_vertices) -> void override
	{
		if (!p_vertices.size())
		{
			return;
		}

		_currentBrush->SetOpacity(_brushOpacity);
		_context->FillGeometry(createPathGeometry(p_vertices, false, true), _currentBrush);
	}

	//------------------------------

	auto stroke_geometry(Avo::Geometry const& p_geometry, float const p_strokeWidth) -> void override
	{
		auto* const implementation = getD2dGeometryFromGeometry(p_geometry);
		if (!implementation->getStrokedRealization())
		{
			realizeStrokedGeometry(implementation, p_strokeWidth);
		}
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawGeometryRealization(implementation->getStrokedRealization(), _currentBrush);
	}
	auto fill_geometry(Avo::Geometry const& p_geometry) -> void override
	{
		auto* const implementation = getD2dGeometryFromGeometry(p_geometry);
		if (!implementation->getFilledRealization())
		{
			realizeFilledGeometry(implementation);
		}
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawGeometryRealization(implementation->getFilledRealization(), _currentBrush);
	}

	//------------------------------

	auto create_rounded_rectangle_geometry(Avo::Rectangle<> const p_rectangle, Avo::Dip const p_radius, bool const p_isStroked) -> Avo::Geometry override
	{
		auto geometry = ComReference<ID2D1RoundedRectangleGeometry>{};
		s_direct2DFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect(convertRectangleToRectf(p_rectangle), p_radius, p_radius), &geometry
		);

		return createGeometryFromImplementation(std::make_shared<Direct2dGeometry>(geometry.as<ID2D1Geometry>()));
	}

	auto create_corner_rectangle_geometry(Avo::Rectangle<> p_rectangle, Avo::RectangleCorners const& p_corners, bool p_isStroked) -> Avo::Geometry override
	{
		return createGeometryFromImplementation(std::make_shared<Direct2dGeometry>(
			createCornerRectanglePathGeometry(p_rectangle, p_corners, true).as<ID2D1Geometry>()
		));
	}

	//------------------------------

	auto create_polygon_geometry(Range<Avo::Point<> const*> p_vertices, bool p_isStroked, bool p_isClosed) -> Avo::Geometry override
	{
		if (!p_vertices.size())
		{
			return {};
		}

		return createGeometryFromImplementation(std::make_shared<Direct2dGeometry>(
			createPathGeometry(p_vertices, p_isStroked, p_isClosed).as<ID2D1Geometry>()
		));
	}

	//------------------------------

	auto set_line_cap(Avo::LineCap p_lineCap) -> void override
	{
		switch (p_lineCap)
		{
			case Avo::LineCap::Flat:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
				break;
			case Avo::LineCap::Round:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_ROUND;
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_ROUND;
				break;
			case Avo::LineCap::Square:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_SQUARE;
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_SQUARE;
				break;
			case Avo::LineCap::Triangle:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_TRIANGLE;
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	auto set_start_line_cap(Avo::LineCap p_lineCap) -> void override
	{
		switch (p_lineCap)
		{
			case Avo::LineCap::Flat:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
				break;
			case Avo::LineCap::Round:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_ROUND;
				break;
			case Avo::LineCap::Square:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_SQUARE;
				break;
			case Avo::LineCap::Triangle:
				_strokeStyleProperties.startCap = D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	auto setEndLineCap(Avo::LineCap p_lineCap) -> void override
	{
		switch (p_lineCap)
		{
			case Avo::LineCap::Flat:
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
				break;
			case Avo::LineCap::Round:
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_ROUND;
				break;
			case Avo::LineCap::Square:
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_SQUARE;
				break;
			case Avo::LineCap::Triangle:
				_strokeStyleProperties.endCap = D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	auto getStartLineCap() -> Avo::LineCap override
	{
		switch (_strokeStyleProperties.startCap)
		{
			case D2D1_CAP_STYLE_FLAT:
				return Avo::LineCap::Flat;
			case D2D1_CAP_STYLE_ROUND:
				return Avo::LineCap::Round;
			case D2D1_CAP_STYLE_SQUARE:
				return Avo::LineCap::Square;
			case D2D1_CAP_STYLE_TRIANGLE:
				return Avo::LineCap::Triangle;
		}
		return Avo::LineCap::Triangle;
	}
	auto getEndLineCap() -> Avo::LineCap override
	{
		switch (_strokeStyleProperties.endCap)
		{
			case D2D1_CAP_STYLE_FLAT:
				return Avo::LineCap::Flat;
			case D2D1_CAP_STYLE_ROUND:
				return Avo::LineCap::Round;
			case D2D1_CAP_STYLE_SQUARE:
				return Avo::LineCap::Square;
			case D2D1_CAP_STYLE_TRIANGLE:
				return Avo::LineCap::Triangle;
		}
		return Avo::LineCap::Triangle;
	}

	//------------------------------

	auto setLineJoin(Avo::LineJoin p_lineJoin) -> void override
	{
		switch (p_lineJoin)
		{
			case Avo::LineJoin::Bevel:
				_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_BEVEL;
				break;
			case Avo::LineJoin::Miter:
				_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_MITER;
				break;
			case Avo::LineJoin::Round:
				_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
				break;
		}
		updateStrokeStyle();
	}
	auto getLineJoin() -> Avo::LineJoin override
	{
		switch (_strokeStyleProperties.lineJoin)
		{
			case D2D1_LINE_JOIN_BEVEL:
				return Avo::LineJoin::Bevel;
			case D2D1_LINE_JOIN_MITER:
				return Avo::LineJoin::Miter;
			case D2D1_LINE_JOIN_ROUND:
				return Avo::LineJoin::Round;
		}
		return Avo::LineJoin::Miter;
	}

	auto setLineJoinMiterLimit(float p_miterLimit) -> void override
	{
		_strokeStyleProperties.miterLimit = p_miterLimit;
		updateStrokeStyle();
	}
	auto getLineJoinMiterLimit() -> float override
	{
		return _strokeStyleProperties.miterLimit;
	}

	//------------------------------

	auto setLineDashStyle(Avo::LineDashStyle p_dashStyle) -> void override
	{
		switch (p_dashStyle)
		{
			case Avo::LineDashStyle::Solid:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_SOLID;
				break;
			case Avo::LineDashStyle::Dash:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DASH;
				break;
			case Avo::LineDashStyle::Dot:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DOT;
				break;
			case Avo::LineDashStyle::DashDot:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DASH_DOT;
				break;
			case Avo::LineDashStyle::DashDotDot:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DASH_DOT_DOT;
				break;
			case Avo::LineDashStyle::Custom:
				_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_CUSTOM;
		}
		updateStrokeStyle();
	}
	auto getLineDashStyle() -> Avo::LineDashStyle override
	{
		switch (_strokeStyleProperties.dashStyle)
		{
			case D2D1_DASH_STYLE_SOLID:
				return Avo::LineDashStyle::Solid;
			case D2D1_DASH_STYLE_DASH:
				return Avo::LineDashStyle::Dash;
			case D2D1_DASH_STYLE_DOT:
				return Avo::LineDashStyle::Dot;
			case D2D1_DASH_STYLE_DASH_DOT:
				return Avo::LineDashStyle::DashDot;
			case D2D1_DASH_STYLE_DASH_DOT_DOT:
				return Avo::LineDashStyle::DashDotDot;
			case D2D1_DASH_STYLE_CUSTOM:
				return Avo::LineDashStyle::Custom;
		}
		return Avo::LineDashStyle::Unknown;
	}

	auto setLineDashOffset(float p_dashOffset) -> void override
	{
		_strokeStyleProperties.dashOffset = p_dashOffset;
		updateStrokeStyle();
	}
	auto getLineDashOffset() -> float override
	{
		return _strokeStyleProperties.dashOffset;
	}

	auto setLineDashCap(Avo::LineCap p_dashCap) -> void override
	{
		switch (p_dashCap)
		{
			case Avo::LineCap::Flat:
				_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
				break;
			case Avo::LineCap::Round:
				_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_ROUND;
				break;
			case Avo::LineCap::Square:
				_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_SQUARE;
				break;
			case Avo::LineCap::Triangle:
				_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	auto getLineDashCap() -> Avo::LineCap override
	{
		switch (_strokeStyleProperties.dashCap)
		{
			case D2D1_CAP_STYLE_FLAT:
				return Avo::LineCap::Flat;
			case D2D1_CAP_STYLE_ROUND:
				return Avo::LineCap::Round;
			case D2D1_CAP_STYLE_SQUARE:
				return Avo::LineCap::Square;
			case D2D1_CAP_STYLE_TRIANGLE:
				return Avo::LineCap::Triangle;
		}
		return Avo::LineCap::Unknown;
	}

	//------------------------------

private:
	std::stack<bool> _clipTypeStack;

	auto pushGeometryLayer(ID2D1Geometry* const p_geometry, float const p_opacity) -> void
	{
		_context->PushLayer(
			D2D1::LayerParameters1(
				D2D1::InfiniteRect(), p_geometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(), p_opacity, nullptr, 
				D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
			), nullptr
		);
		_clipTypeStack.push(true);
	}
	
public:
	auto pushClipGeometry(Avo::Geometry const& p_geometry, float const p_opacity) -> void override
	{
		pushGeometryLayer(static_cast<Direct2dGeometry*>(p_geometry.get_implementation())->getGeometry(), p_opacity);
	}

	auto pushClipShape(Range<Avo::Point<> const*> const p_points, float const p_opacity) -> void override
	{
		if (!p_points.size())
		{
			return;
		}

		pushGeometryLayer(createPathGeometry(p_points, false, true), p_opacity);
	}
	auto popClipShape() -> void override
	{
		if (_clipTypeStack.size())
		{
			if (_clipTypeStack.top())
			{
				_context->PopLayer();
			}
			else
			{
				_context->PopAxisAlignedClip();
			}
			_clipTypeStack.pop();
		}
	}

	auto pushClipRectangle(Avo::Rectangle<> const p_rectangle, float const p_opacity) -> void override
	{
		auto const rectangle = convertRectangleToRectf(p_rectangle);

		constexpr auto opaqueThreshold = 254./255.;
		if (p_opacity > opaqueThreshold)
		{
			_context->PushAxisAlignedClip(rectangle, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			_clipTypeStack.push(false);
		}
		else
		{
			auto geometry = ComReference<ID2D1RectangleGeometry>{};
			s_direct2DFactory->CreateRectangleGeometry(rectangle, &geometry);
			pushGeometryLayer(geometry, p_opacity);
		}
	}

	auto pushClipRectangle(
		Avo::Rectangle<> const p_rectangle, Avo::RectangleCorners const& p_corners, float const p_opacity
	) -> void override
	{
		pushGeometryLayer(createCornerRectanglePathGeometry(p_rectangle, p_corners, true), p_opacity);
	}

	auto pushRoundedClipRectangle(Avo::Rectangle<> const p_rectangle, float const p_radius, float const p_opacity) -> void override
	{
		auto geometry = ComReference<ID2D1RoundedRectangleGeometry>{};
		s_direct2DFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect(
				convertRectangleToRectf(p_rectangle),
				p_radius, p_radius
			), &geometry
		);

		pushGeometryLayer(geometry, p_opacity);
	}

	//------------------------------

private:
	template<typename Lambda>
	auto createShadowImage(
		Avo::Size<> const size, float p_blur, 
		Avo::Color const p_color, Lambda const& p_drawShape
	) -> Avo::Image
	{
		if (!size || !p_color.alpha) return {};

		p_blur *= 2.f/3.f;

		auto const targetLock = std::scoped_lock{_targetMutex};

		auto dpiX = float{};
		auto dpiY = float{};
		_context->GetDpi(&dpiX, &dpiY);
		auto const dip_to_pixel_factor = dpiX/USER_DEFAULT_SCREEN_DPI;

		auto targetBefore = ComReference<ID2D1Image>{};
		_context->GetTarget(&targetBefore);
		_context->SetDpi(USER_DEFAULT_SCREEN_DPI, USER_DEFAULT_SCREEN_DPI);

		auto inputBitmap = ComReference<ID2D1Bitmap1>{};
		_context->CreateBitmap(
			D2D1::SizeU(size.width, size.height),
			nullptr, 0,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &inputBitmap
		);

		_context->SetTarget(inputBitmap);

		_context->BeginDraw();
		p_drawShape();
		_context->EndDraw();
		_context->SetDpi(dpiX, dpiY);

		//------------------------------
		// Apply effect

		auto shadowEffect = ComReference<ID2D1Effect>{};
		_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

		shadowEffect->SetInput(0, inputBitmap);
		shadowEffect->SetValue(
			D2D1_SHADOW_PROP_COLOR,
			D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
		);
		shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur/dip_to_pixel_factor);

		//------------------------------
		// Convert to bitmap

		auto outputBitmap = static_cast<ID2D1Bitmap1*>(nullptr);
		auto const outputSize = Avo::Size<UINT32>{size + 6.f*p_blur};

		_context->CreateBitmap(
			{outputSize.x, outputSize.y},
			0, outputSize.width*4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &outputBitmap
		);

		_context->SetTarget(outputBitmap);
		_context->BeginDraw();
		clear();
		_context->DrawImage(shadowEffect, {p_blur*3.f/dip_to_pixel_factor, p_blur*3.f/dip_to_pixel_factor});
		_context->EndDraw();
		_context->SetTarget(targetBefore);

		return createImageFromImplementation(std::make_shared<Direct2dImage>(outputBitmap));
	}

public:
	auto createRectangleShadowImage(Avo::Size<> size, float p_blur, Avo::Color p_color) -> Avo::Image override
	{
		return createShadowImage(size, p_blur, p_color, [&]{ clear(0.f); });
	}
	auto createRectangleShadowImage(Avo::Size<> size, Avo::RectangleCorners const& p_corners, 
	                                float p_blur, Avo::Color p_color) -> Avo::Image override
	{
		return createShadowImage(size, p_blur, p_color, [&]{
			clear();
			set_color(0.f);
			fill_rectangle(size, p_corners);
		});
	}
	auto createRoundedRectangleShadowImage(Avo::Size<> size, float p_radius, float p_blur, Avo::Color p_color) -> Avo::Image override
	{
		return createShadowImage(size, p_blur, p_color, [&]{
			clear();
			set_color(0.f);
			fill_rounded_rectangle(size, p_radius);
		});
	}

	//------------------------------

	auto create_image(std::byte const* p_pixelData, Avo::Size<Avo::Pixels> size) -> Avo::Image override
	{
		auto bitmap = static_cast<ID2D1Bitmap1*>(nullptr);
		_context->CreateBitmap(
			{static_cast<UINT32>(size.width), static_cast<UINT32>(size.height)},
			p_pixelData, size.width*4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_NONE,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &bitmap
		);
		return createImageFromImplementation(std::make_shared<Direct2dImage>(bitmap));
	}
	auto create_image(Avo::DataView p_imageData) -> Avo::Image override
	{
		auto stream = ComReference<IWICStream>{};
		s_imagingFactory->CreateStream(&stream);
		if (!stream) return {};

		// The stream won't modify p_imageData.
		stream->InitializeFromMemory(const_cast<BYTE*>(reinterpret_cast<BYTE const*>(p_imageData.data())), p_imageData.size());

		auto decoder = ComReference<IWICBitmapDecoder>{};
		s_imagingFactory->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnDemand, &decoder);
		if (!decoder) return {};

		auto frame = ComReference<IWICBitmapFrameDecode>{};
		decoder->GetFrame(0, &frame);
		if (!frame) return {};

		auto formatConverter = ComReference<IWICFormatConverter>{};
		s_imagingFactory->CreateFormatConverter(&formatConverter);
		formatConverter->Initialize(
			frame, 
			GUID_WICPixelFormat32bppPBGRA, 
			WICBitmapDitherTypeNone, 
			nullptr, 0.f, 
			WICBitmapPaletteTypeMedianCut
		);

		auto bitmap = static_cast<ID2D1Bitmap1*>(nullptr);
		_context->CreateBitmapFromWicBitmap(formatConverter, 0, &bitmap);
		if (!bitmap) return {};

		return createImageFromImplementation(std::make_shared<Direct2dImage>(bitmap));
	}
	auto create_image(std::string_view p_filePath) -> Avo::Image override
	{
		auto wideFilePath = std::array<char16, MAX_PATH>{};
		Avo::convert_utf8_to_utf16(p_filePath, wideFilePath);

		auto decoder = ComReference<IWICBitmapDecoder>{};
		s_imagingFactory->CreateDecoderFromFilename(
			reinterpret_cast<LPCWSTR>(wideFilePath.data()), 
			nullptr, GENERIC_READ, 
			WICDecodeMetadataCacheOnLoad, 
			&decoder
		);
		if (!decoder) return {};

		auto frame = ComReference<IWICBitmapFrameDecode>{};
		decoder->GetFrame(0, &frame);
		if (!frame) return {};

		auto formatConverter = ComReference<IWICFormatConverter>{};
		s_imagingFactory->CreateFormatConverter(&formatConverter);
		formatConverter->Initialize(
			frame, 
			GUID_WICPixelFormat32bppPBGRA, 
			WICBitmapDitherTypeNone, 
			nullptr, 0.f, 
			WICBitmapPaletteTypeMedianCut
		);
		if (!formatConverter) return {};

		auto bitmap = static_cast<ID2D1Bitmap1*>(nullptr);
		_context->CreateBitmapFromWicBitmap(formatConverter, nullptr, &bitmap);
		if (!bitmap) return {};

		return createImageFromImplementation(std::make_shared<Direct2dImage>(bitmap));
	}
	auto createImageFromHandle(void* p_handle) -> Avo::Image override
	{
		auto wicBitmap = ComReference<IWICBitmap>{};

		if (auto result = s_imagingFactory->CreateBitmapFromHICON(static_cast<HICON>(p_handle), &wicBitmap);
		    result < 0)
		{
			wicBitmap = nullptr;
			s_imagingFactory->CreateBitmapFromHBITMAP(static_cast<HBITMAP>(p_handle), nullptr, WICBitmapAlphaChannelOption::WICBitmapUseAlpha, &wicBitmap);
		}

		auto formatConverter = ComReference<IWICFormatConverter>{};
		s_imagingFactory->CreateFormatConverter(&formatConverter);
		formatConverter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
		if (!formatConverter) return {};

		auto bitmap = ComReference<ID2D1Bitmap1>{};
		_context->CreateBitmapFromWicBitmap(formatConverter, &bitmap);
		if (!bitmap) return {};

		return createImageFromImplementation(std::make_shared<Direct2dImage>(bitmap));
	}

	//------------------------------

private:
	static auto getD2dBitmapFromImage(Avo::Image const& p_image) -> ID2D1Bitmap1*
	{
		return dynamic_cast<Direct2dImage*>(p_image.get_implementation())->getD2dBitmap();
	}

public:
	auto drawImage(Avo::Image const& p_image, float p_multiplicativeOpacity) -> void override
	{
		if (!p_image) return;

		auto const cropRectangle = p_image.get_crop_rectangle(),
		           innerBounds = p_image.get_inner_bounds();

		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawBitmap( // SO MANY PARAMETERS
			getD2dBitmapFromImage(p_image),
			{innerBounds.left, innerBounds.top, innerBounds.right, innerBounds.bottom},
			p_image.get_opacity()*_currentBrush->GetOpacity()*p_multiplicativeOpacity,
			p_image.getScalingMethod() == Avo::ImageScalingMethod::Pixelated ? 
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			{cropRectangle.left, cropRectangle.top, cropRectangle.right, cropRectangle.bottom}
		);
	}

	//------------------------------

private:
	auto writeImageFileDataToStream(Avo::Image const& p_image, Avo::ImageFormat format, IStream* p_stream) -> bool
	{
		auto direct2dBitmap = getD2dBitmapFromImage(p_image);

		auto formatGuid = GUID{};
		switch (format)
		{
			case Avo::ImageFormat::Jpeg:
				formatGuid = GUID_ContainerFormatJpeg;
				break;
			case Avo::ImageFormat::Png:
				formatGuid = GUID_ContainerFormatPng;
				break;
			case Avo::ImageFormat::Bmp:
				formatGuid = GUID_ContainerFormatBmp;
				break;
			case Avo::ImageFormat::Ico:
				formatGuid = GUID_ContainerFormatIco;
		}

		auto bitmapEncoder = ComReference<IWICBitmapEncoder>{};
		s_imagingFactory->CreateEncoder(formatGuid, nullptr, &bitmapEncoder);
		if (!bitmapEncoder) return false;
		
		bitmapEncoder->Initialize(p_stream, WICBitmapEncoderNoCache);

		auto frameEncoder = ComReference<IWICBitmapFrameEncode>{};
		bitmapEncoder->CreateNewFrame(&frameEncoder, nullptr);
		if (!frameEncoder) return false;

		frameEncoder->Initialize(nullptr);

		auto device = ComReference<ID2D1Device>{};
		_context->GetDevice(&device);
		if (!device) return false;

		auto imageEncoder = ComReference<IWICImageEncoder>{};
		s_imagingFactory->CreateImageEncoder(device, &imageEncoder);
		if (!imageEncoder) return false;

		imageEncoder->WriteFrame(direct2dBitmap, frameEncoder, nullptr);

		frameEncoder->Commit();
		bitmapEncoder->Commit();

		return true;
	}
	
public:
	auto createImageFileData(Avo::Image const& p_image, Avo::ImageFormat format) -> Avo::DataVector override
	{
		auto outputStream = ComReference{static_cast<IStream*>(createImageFileDataNativeStream(p_image, format))};

		STATSTG streamInfo;
		outputStream->Stat(&streamInfo, STATFLAG_NONAME);

		auto buffer = Avo::DataVector(streamInfo.cbSize.QuadPart);

		ULONG numberOfBytesWritten;
		outputStream->Seek({}, SEEK_SET, nullptr);
		outputStream->Read(buffer.data(), buffer.size(), &numberOfBytesWritten);

		if (numberOfBytesWritten < buffer.size())
		{
			buffer.resize(numberOfBytesWritten);
		}

		return buffer;
	}
	auto createImageFileDataNativeStream(Avo::Image const& p_image, Avo::ImageFormat format) -> void* override
	{
		if (auto outputStream = ComReference{SHCreateMemStream(0, 0)}; 
		    writeImageFileDataToStream(p_image, format, outputStream))
		{
			return outputStream.steal();
		}
		return {};
	}
	auto saveImageToFile(Avo::Image const& p_image, std::string_view p_filePath, Avo::ImageFormat format) -> bool override
	{
		auto filePath = std::array<char16, MAX_PATH>{};
		Avo::convert_utf8_to_utf16(p_filePath, filePath);
		
		auto outputStream = ComReference<IStream>{};
		SHCreateStreamOnFileW(reinterpret_cast<LPCWSTR>(filePath.data()), STGM_CREATE | STGM_WRITE, &outputStream);

		return writeImageFileDataToStream(p_image, format, outputStream);
	}

	//------------------------------

	auto createNativeImageFromImage(Avo::Image const& p_image) -> void* override
	{
		auto sourceBitmap = getD2dBitmapFromImage(p_image);
		auto size = sourceBitmap->GetPixelSize();

		auto cpuBitmap = ComReference<ID2D1Bitmap1>{};
		if (sourceBitmap->GetOptions() & D2D1_BITMAP_OPTIONS_CPU_READ)
		{
			cpuBitmap = sourceBitmap;
		}
		else
		{
			_context->CreateBitmap(
				size, nullptr, 0, 
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, 
					sourceBitmap->GetPixelFormat()
				), 
				&cpuBitmap
			);
		}

		auto destinationPoint = D2D1_POINT_2U{0, 0};
		auto sourceRectangle = D2D1_RECT_U{0, 0, size.width, size.height};
		cpuBitmap->CopyFromBitmap(&destinationPoint, sourceBitmap, &sourceRectangle);

		// Pixel data
		auto mappedRectangle = D2D1_MAPPED_RECT{};
		cpuBitmap->Map(D2D1_MAP_OPTIONS_READ, &mappedRectangle);

		auto bitmapInfoHeader = BITMAPINFOHEADER{};		
		bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader.biWidth = ceil(size.width/16.f)*16;
		bitmapInfoHeader.biHeight = -(long)size.height;
		bitmapInfoHeader.biBitCount = 32;
		bitmapInfoHeader.biCompression = BI_RGB;
		bitmapInfoHeader.biPlanes = 1;

		auto result = CreateDIBitmap(
			GetDC(static_cast<HWND>(_window->get_native_handle())), 
			&bitmapInfoHeader, CBM_INIT, mappedRectangle.bits,
			reinterpret_cast<BITMAPINFO*>(&bitmapInfoHeader), DIB_RGB_COLORS
		);

		cpuBitmap->Unmap();

		return result;
	}

	//------------------------------

private:
	auto createGradientStopCollection(Range<Avo::GradientStop*> p_stops)
	{
		auto gradientStops = std::vector<D2D1_GRADIENT_STOP>(p_stops.size());
		for (auto a : Indices{p_stops})
		{
			auto const& color = p_stops[a].color;
			gradientStops[a].color = {color.red, color.green, color.blue, color.alpha};
			gradientStops[a].position = p_stops[a].position;
		}

		auto stopCollection = ComReference<ID2D1GradientStopCollection>{};
		_context->CreateGradientStopCollection(gradientStops.data(), p_stops.size(), &stopCollection);
		return stopCollection;
	}

public:
	auto createLinearGradient(Range<Avo::GradientStop*> p_gradientStops, 
	                          Avo::Point<> p_start, Avo::Point<> p_end) 
		-> Avo::LinearGradient override
	{
		auto stopCollection = createGradientStopCollection(p_gradientStops);

		auto brush = ComReference<ID2D1LinearGradientBrush>{};
		_context->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties({p_start.x, p_start.y}, {p_end.x, p_end.y}),
			stopCollection, &brush
		);

		return createLinearGradientFromImplementation(std::make_shared<Direct2dLinearGradient>(brush));
	}
	auto createRadialGradient(Range<Avo::GradientStop*> p_gradientStops, 
	                          Avo::Point<> p_start, Avo::Point<> p_radius) 
		-> Avo::RadialGradient override
	{
		auto stopCollection = createGradientStopCollection(p_gradientStops);

		auto brush = ComReference<ID2D1RadialGradientBrush>{};
		_context->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties({p_start.x, p_start.y}, {}, p_radius.x, p_radius.y), 
			stopCollection, &brush
		);

		return createRadialGradientFromImplementation(std::make_shared<Direct2dRadialGradient>(brush));
	}

	auto setGradient(Avo::LinearGradient const& p_gradient) -> void override
	{
		_currentBrush = dynamic_cast<Direct2dLinearGradient*>(p_gradient.get_implementation())->getD2dBrush();
	}
	auto setGradient(Avo::RadialGradient const& p_gradient) -> void override
	{
		_currentBrush = dynamic_cast<Direct2dRadialGradient*>(p_gradient.get_implementation())->getD2dBrush();
	}

	auto set_color(Avo::Color p_color) -> void override
	{
		_solidColorBrush->SetColor({p_color.red, p_color.green, p_color.blue, p_color.alpha});
		_currentBrush = _solidColorBrush;
	}
	auto set_opacity(float p_opacity) -> void override
	{
		_brushOpacity = p_opacity;
	}

	//------------------------------

private:
	ComReference<IDWriteFontCollection> _fontCollection;
	std::vector<FontData> _fontData;

	auto updateFontCollection() -> void
	{
		_fontCollection = nullptr; // Releases, important.
		s_directWriteFactory->CreateCustomFontCollection(s_fontCollectionLoader, &_fontData, sizeof(_fontData), &_fontCollection);
	}

public:
	auto addFont(std::string_view p_filePath) -> void override
	{
		_fontData.emplace_back(std::make_shared<Avo::DataVector>(Avo::readFile(p_filePath)));
		updateFontCollection();
	}

	auto addFont(Avo::DataVector&& data) -> void override
	{
		_fontData.emplace_back(std::make_shared<Avo::DataVector>(std::move(data)));
		updateFontCollection();
	}
	auto addFont(Avo::DataView data) -> void override
	{
		_fontData.emplace_back(std::make_shared<Avo::DataVector>(data.data(), data.data() + data.size()));
		updateFontCollection();
	}

	//------------------------------

private:
	ComReference<IDWriteTextFormat> _text_format;
	static constexpr auto maxFontFamilyNameSize = 200;
public:
	auto setDefaultTextProperties(Avo::TextProperties const& p_textProperties) -> void override
	{
		auto fontFamily = std::array<char16, maxFontFamilyNameSize>{};
		Avo::convert_utf8_to_utf16(p_textProperties.fontFamilyName, fontFamily);

		_text_format = nullptr; // Releases, important.
		s_directWriteFactory->CreateTextFormat(
			reinterpret_cast<WCHAR const*>(fontFamily.data()), 
			_fontCollection, 
			static_cast<DWRITE_FONT_WEIGHT>(p_textProperties.fontWeight),
			static_cast<DWRITE_FONT_STYLE>(p_textProperties.fontStyle), 
			static_cast<DWRITE_FONT_STRETCH>(p_textProperties.fontStretch),
			p_textProperties.fontSize, L"", &_text_format
		);
		_text_format->SetTextAlignment(DirectWriteText::convertTextAlignToDirectWrite(p_textProperties.textAlign));
		_text_format->SetReadingDirection(DirectWriteText::convertReadingDirectionToDirectWrite(p_textProperties.readingDirection));
		_text_format->SetLineSpacing(
			DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 
			p_textProperties.lineHeight, p_textProperties.lineHeight*0.8f
		);
		_textProperties = p_textProperties;
	}
	auto getDefaultTextProperties() -> Avo::TextProperties override
	{
		return _textProperties;
	}

	//------------------------------

private:
	auto getDWriteTextLayoutFromText(Avo::Text const& p_text) -> IDWriteTextLayout1*
	{
		return dynamic_cast<DirectWriteText*>(p_text.get_implementation())->getDWriteTextLayout();
	}

public:
	auto create_text(std::string_view p_string, float p_fontSize, Avo::Rectangle<> p_bounds) -> Avo::Text override
	{
		p_bounds.clipNegativeSpace();

		auto wideString = Avo::convert_utf8_to_utf16(p_string);

		auto textLayout = static_cast<IDWriteTextLayout1*>(nullptr);
		s_directWriteFactory->CreateTextLayout( // So many parameters...
			reinterpret_cast<WCHAR const*>(wideString.data()), wideString.size(), 
			_text_format, 
			p_bounds.get_width(), p_bounds.get_height(), 
			reinterpret_cast<IDWriteTextLayout**>(&textLayout)
		);
		
		auto textRange = DWRITE_TEXT_RANGE{0, static_cast<UINT32>(wideString.size())};
		textLayout->SetFontSize(p_fontSize, textRange);
		textLayout->SetCharacterSpacing(
			_textProperties.characterSpacing/2, 
			_textProperties.characterSpacing/2, 
			0.f, textRange
		);

		return createTextFromImplementation(std::make_shared<DirectWriteText>(textLayout, wideString, p_string, p_bounds));
	}
	auto draw_text(Avo::Text const& p_text) -> void override
	{
		if (!p_text)
		{
			return;
		}
		auto* const textLayout = getDWriteTextLayoutFromText(p_text);

		auto name = std::array<WCHAR, maxFontFamilyNameSize>{};
		textLayout->GetFontFamilyName(name.data(), name.size());

		auto overhangMetrics = DWRITE_OVERHANG_METRICS{};
		if (p_text.get_is_top_trimmed())
		{
			textLayout->GetOverhangMetrics(&overhangMetrics);
		}
		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawTextLayout(
			D2D1::Point2F(p_text.getLeft(), p_text.getTop() + overhangMetrics.top),
			textLayout, _currentBrush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
		);
	}
	auto draw_text(std::string_view p_string, Avo::Rectangle<> p_rectangle) -> void override
	{
		if (p_string == "") return;

		auto wideString = Avo::convert_utf8_to_utf16(p_string);

		_currentBrush->SetOpacity(_brushOpacity);
		_context->DrawTextW(
			reinterpret_cast<WCHAR const*>(wideString.data()), wideString.size(), 
			_text_format, convertRectangleToRectf(p_rectangle),
			_currentBrush, D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_NONE
		);
	}
	auto draw_text(std::string_view p_string, Avo::Point<> p_position) -> void override
	{
		draw_text(p_string, Avo::Rectangle{p_position, convertSizefToSize(_context->GetSize())*2});
	}

	//------------------------------

private:
	/*
		This is all pretty hard to follow. It's DirectX's fault. Such a cluttered API. 
		Maybe also because of their use of COM interfaces which are a painful blend of C and C++.
	*/
	auto createSwapChainAndDrawingContext(float p_dpi) -> void
	{
		/*
			Create the Direct3D device, which will be used later to create the Direct2D drawing 
			context used for drawing as well as the swap chain used for swapping front and back 
			buffers and acts as DirectX's interface to the window.
		*/
		auto d3dDevice = ComReference<ID3D11Device>{};
		{
			auto d3dDeviceContext = ComReference<ID3D11DeviceContext>{};

			auto featureLevels = std::array{
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1,
			};
			auto featureLevel = D3D_FEATURE_LEVEL{};
			D3D11CreateDevice(
				0,
				D3D_DRIVER_TYPE_HARDWARE,
				0,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT
	#ifdef _DEBUG 
				| D3D11_CREATE_DEVICE_DEBUG
	#endif
				,
				featureLevels.data(),
				featureLevels.size(),
				D3D11_SDK_VERSION,
				&d3dDevice,
				&featureLevel,
				&d3dDeviceContext
			);
		}


		//------------------------------
		// Get dxgi factory from the Direct3D device.
		// Both of those will be used afterwards to create the dxgi swap chain.

		auto dxgiFactory = ComReference<IDXGIFactory2>{};
		{
			/*
				First we obtain the DXGI device from the Direct3D device.
				This will first be used to create a Direct2D device, which 
				in turn is used to create our Direct2D drawing context.
				Afterwards, we obtain the DXGI factory from the DXGI device,
				which will be needed next outside this scope to create the 
				DXGI swap chain.
			*/
			
			auto dxgiDevice = ComReference<IDXGIDevice1>{};
			d3dDevice->QueryInterface(&dxgiDevice);
			dxgiDevice->SetMaximumFrameLatency(1U);

			{
				// Create the Direct2D device and context.
				auto direct2DDevice = ComReference<ID2D1Device1>{}; 
				s_direct2DFactory->CreateDevice(dxgiDevice, &direct2DDevice);
				direct2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &_context);
				_context->SetDpi(p_dpi, p_dpi);
			}

			// Obtain DXGI factory from DXGI device.
			auto dxgiAdapter = ComReference<IDXGIAdapter>{};
			dxgiDevice->GetAdapter(&dxgiAdapter);
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		}

		// We don't want DXGI changing the fullscreen state using its own key bindings...
		auto windowHandle = static_cast<HWND>(_window->get_native_handle());
		dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_WINDOW_CHANGES);

		//------------------------------
		// Create swap chain, which holds the front and back buffers and is connected to the window.

		auto size = Avo::Size<UINT>{_window->get_size()*p_dpi/USER_DEFAULT_SCREEN_DPI};
		auto swapChainDescription = DXGI_SWAP_CHAIN_DESC1{
			/*.Width = */size.width,
			/*.Height = */size.height,
			/*.Format = */DXGI_FORMAT_B8G8R8A8_UNORM,
			/*.Stereo = */false,
			/*.SampleDesc = */{1, 0},
			/*.BufferUsage = */DXGI_USAGE_RENDER_TARGET_OUTPUT,
			/*.BufferCount = */2,
			/*.Scaling = */DXGI_SCALING::DXGI_SCALING_NONE,
			/*.SwapEffect = */DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
		};

		dxgiFactory->CreateSwapChainForHwnd(
			d3dDevice, windowHandle,
			&swapChainDescription, nullptr,
			nullptr, &_swapChain
		);
	}
	auto initializeContext() -> void
	{
		float dpi = GetDpiForSystem();

		createSwapChainAndDrawingContext(dpi);

		//------------------------------
		// Initialize the background color of the swap chain's buffers.

		auto color = DXGI_RGBA{0.5f, 0.5f, 0.5f, 1.f};
		_swapChain->SetBackgroundColor(&color);

		//------------------------------
		// Create a target bitmap which is connected to the back buffer of the window.

		{
			auto lock = Direct2dLock{};
			
			ComReference<IDXGISurface> dxgiBackBuffer;
			_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

			create_target_window_bitmap_from_dxgi_back_buffer(dxgiBackBuffer, dpi);
		}

		_context->SetTarget(_target_window_bitmap);
	}
	auto initializeFonts() -> void
	{
		auto addData = [&](auto data, auto size) {
			auto const start = reinterpret_cast<std::byte const*>(data);
			_fontData.emplace_back(std::make_shared<Avo::DataVector>(start, start + size));
		};

		_fontData.reserve(8);
		addData(FONT_DATA_ROBOTO_LIGHT, FONT_DATA_SIZE_ROBOTO_LIGHT);
		addData(FONT_DATA_ROBOTO_REGULAR, FONT_DATA_SIZE_ROBOTO_REGULAR);
		addData(FONT_DATA_ROBOTO_MEDIUM, FONT_DATA_SIZE_ROBOTO_MEDIUM);
		addData(FONT_DATA_ROBOTO_BOLD, FONT_DATA_SIZE_ROBOTO_BOLD);
		addData(FONT_DATA_MATERIAL_ICONS, FONT_DATA_SIZE_MATERIAL_ICONS);
		updateFontCollection();

		setDefaultTextProperties(_textProperties);

		// Just for debugging...
		//std::vector<wchar_t*> fontFamilyNames;
		//for (uint32 a = 0; a < _fontCollection->GetFontFamilyCount(); a++)
		//{
		//	IDWriteFontFamily* fontFamily;
		//	_fontCollection->GetFontFamily(a, &fontFamily);
		//	IDWriteLocalizedStrings* names;
		//	fontFamily->GetFamilyNames(&names);
		//	wchar_t* buffer = new wchar_t[30];
		//	names->GetString(0, buffer, 30);
		//	fontFamilyNames.push_back(buffer);
		//}

		//_context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	}
	auto initializeDrawing() -> void
	{
		// Initial color doesn't matter, it'll be changed afterwards.
		_context->CreateSolidColorBrush({}, &_solidColorBrush);
		_currentBrush = _solidColorBrush;

		_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
		_strokeStyleProperties.dashOffset = 1.f;
		_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_SOLID;
		_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
		_strokeStyleProperties.miterLimit = 0.f;
		_strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
		_strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
		_strokeStyleProperties.transformType = D2D1_STROKE_TRANSFORM_TYPE::D2D1_STROKE_TRANSFORM_TYPE_NORMAL;
		updateStrokeStyle();
	}

public:
	Direct2dDrawingContext(Avo::Window* p_window) :
		_window{p_window}
	{
		initializeContext();
		
		initializeDrawing();

		initializeFonts();
	}
	~Direct2dDrawingContext()
	{
		destroyStaticResources();
	}
};
ComReference<ID2D1Factory2> Direct2dDrawingContext::s_direct2DFactory;
ComReference<IDWriteFactory1> Direct2dDrawingContext::s_directWriteFactory;
ComReference<FontCollectionLoader> Direct2dDrawingContext::s_fontCollectionLoader;
ComReference<FontFileLoader> Direct2dDrawingContext::s_fontFileLoader;
ComReference<IWICImagingFactory2> Direct2dDrawingContext::s_imagingFactory;
#endif
#ifdef __linux__

bool areOpenGlFunctionsLoaded = false;

PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

//------------------------------

class OpenGlShader
{
private:
	GLuint _programID{0};
	GLuint _vertexArrayObjectID{0};

	GLuint _vertexBufferID{0};
	uint32 _vertexBufferSize{0};
	uint32 _vertexSize{0};

	GLuint _indexBufferID{0};
	uint32 _indexBufferSize{0};

public:
	OpenGlShader() = default;
	~OpenGlShader()
	{
		if (_programID)
		{
			glDeleteProgram(_programID);
		}
	}

	void compile(char const* p_vertexShaderSource, char const* p_fragmentShaderSource)
	{
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderID, 1, &p_vertexShaderSource, nullptr);
		glCompileShader(vertexShaderID);

		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderID, 1, &p_fragmentShaderSource, nullptr);
		glCompileShader(fragmentShaderID);

#ifdef _DEBUG
		GLint wasSuccessful = 1;

		// Check for vertex shader compile errors
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &wasSuccessful);
		if (!wasSuccessful)
		{
			GLchar message[256];
			glGetShaderInfoLog(vertexShaderID, 256, nullptr, message);
			Console::println("\nVertex shader compile error(s)!\n", message);
		}

		// Check for fragment shader compile errors
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &wasSuccessful);
		if (!wasSuccessful)
		{
			GLchar message[256];
			glGetShaderInfoLog(fragmentShaderID, 256, nullptr, message);
			Console::println("\nFragment shader compile error(s)!\n", message);
		}
#endif

		_programID = glCreateProgram();
		glAttachShader(_programID, vertexShaderID);
		glAttachShader(_programID, fragmentShaderID);
		glLinkProgram(_programID);

		glDeleteShader(fragmentShaderID);
		glDeleteShader(vertexShaderID);

		_vertexArrayObjectID = 0;
		glGenVertexArrays(1, &_vertexArrayObjectID);

		_vertexBufferID = 0;
		glGenBuffers(1, &_vertexBufferID);
	}

	void setVertexData(float* data, uint32 p_dataSize, uint32 p_vertexSize, GLenum p_usage = GL_STATIC_DRAW)
	{
        glBindVertexArray(_vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, p_dataSize, data, p_usage);

		_vertexBufferSize = p_dataSize;
		_vertexSize = p_vertexSize;
	}
	/*
		Sets information about a vertex shader input.
		p_shaderInputIndex is the layout index of the "in" variable in the vertex shader.
		p_numberOfComponentsInShaderInput is the number of floats stored in the "in" variable, and is either 1, 2, 3 or 4.
		p_pointerPosition is the start position of the input in the vertex buffer, in number of floats.
	*/
	void configureVertexShaderInput(uint32 p_shaderInputIndex, uint32 p_numberOfComponentsInShaderInput, uint32 p_pointerPosition)
	{
		glBindVertexArray(_vertexArrayObjectID);

		glVertexAttribPointer(p_shaderInputIndex, p_numberOfComponentsInShaderInput, GL_FLOAT, GL_FALSE, _vertexSize, (void*)(p_pointerPosition*sizeof(float)));
		glEnableVertexAttribArray(p_shaderInputIndex);
	}

	void setVertexDataOrder(uint32* p_indices, uint32 p_indexCount, GLenum p_usage = GL_STATIC_DRAW)
	{
        glBindVertexArray(_vertexArrayObjectID);

	    glGenBuffers(1, &_indexBufferID);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferID);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indexCount, p_indices, p_usage);

	    _indexBufferSize = p_indexCount;
	}

    void setUniformValue(char const* p_name, float p_value)
	{
		glUniform1f(glGetUniformLocation(_programID, p_name), p_value);
	}
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1)
	{
		glUniform2f(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1);
	}
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1, float p_value_2)
	{
		glUniform3f(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1, p_value_2);
	}
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1, float p_value_2, float p_value_3)
	{
		glUniform4f(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1, p_value_2, p_value_3);
	}
	void setUniformValue(char const* p_name, int p_value)
	{
		glUniform1i(glGetUniformLocation(_programID, p_name), p_value);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1)
	{
		glUniform2i(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1, int p_value_2)
	{
		glUniform3i(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1, p_value_2);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1, int p_value_2, int p_value_3)
	{
		glUniform4i(glGetUniformLocation(_programID, p_name), p_value_0, p_value_1, p_value_2, p_value_3);
	}
    void setUniformColor(char const* p_name, Avo::Color p_color)
    {
		glUniform4f(glGetUniformLocation(_programID, p_name), p_color.red, p_color.green, p_color.blue, p_color.alpha);
	}
	/*
		Sets a matrix uniform with 2 rows and 3 columns.
		The values are in column-major order.
	*/
	void setUniformMatrix3x3(char const* p_name, float* p_values)
	{
		glUniformMatrix3fv(glGetUniformLocation(_programID, p_name), 1, GL_FALSE, p_values);
	}

	void use()
	{
		glUseProgram(_programID);
	}

    /*
		If p_numberOfVerticesToRender is -1, it renders all vertices or all referred to by a previous call to setVertexDataOrder.
    */
    void draw(uint32 p_startVertexIndex = 0, uint32 p_numberOfVerticesToDraw = -1, GLenum p_mode = GL_TRIANGLES)
    {
	    glBindVertexArray(_vertexArrayObjectID);

        if (_indexBufferID)
        {
            glDrawElements(p_mode, p_numberOfVerticesToDraw == -1 ? _indexBufferSize : p_numberOfVerticesToDraw, GL_UNSIGNED_INT, nullptr);
		}
        else
        {
            glDrawArrays(p_mode, p_startVertexIndex, p_numberOfVerticesToDraw == -1 ? _vertexBufferSize/_vertexSize : p_numberOfVerticesToDraw);
		}
	}
};

//------------------------------

class ClippingShape
{
private:
	Avo::Rectangle<> _bounds;
	
public:
	explicit ClippingShape(Avo::Rectangle<> p_bounds) :
		_bounds(p_bounds)
	{
	}
	ClippingShape(float p_left, float p_top, float p_right, float p_bottom) :
		_bounds(p_left, p_top, p_right, p_bottom)
	{
	}

	Avo::Rectangle<> get_bounds()
	{
		return _bounds;
	}
};

class OpenGlDrawCall
{
public:
	uint32 vertexBufferStartIndex{0};
	uint32 numberOfVertices{0};
	GLenum primitiveType{GL_TRIANGLES};
	Avo::Color color;
	int renderMode{0};
	float lineWidth{1.f};
	GLuint texture{0u};
	float transform[9];
};

class OpenGlGeometry : public Avo::Geometry
{
public:
	std::vector<float> vertexBuffer;
	bool isStroked{false};
};

class OpenGlImage : public Avo::Image
{
private:
	GLuint _textureID;
	Avo::Point<uint32> _originalSize;

	Avo::ImageScalingMethod _scalingMethod{Avo::ImageScalingMethod::Pixelated};
	Avo::ImageBoundsSizing _boundsSizing{Avo::ImageBoundsSizing::Contain};
	Avo::Point<> _boundsPositioning{0.5f, 0.5f};

	Avo::Rectangle<> _cropRectangle;
	float _opacity{1.f};

public:
	OpenGlImage(GLuint p_textureID, uint32 p_width, uint32 p_height, float p_dipToPixelFactor) :
		_textureID(p_textureID), _originalSize(p_width, p_height)
	{
		_bounds.set(0.f, 0.f, p_width*p_dipToPixelFactor, p_height*p_dipToPixelFactor);
		_cropRectangle = _bounds;
	}

	//------------------------------

	void set_crop_rectangle(Avo::Rectangle<> p_rectangle) override
	{
		_cropRectangle = p_rectangle;
	}
	Avo::Rectangle<> get_crop_rectangle() const override
	{
		return _cropRectangle;
	}

	Avo::Point<uint32> get_original_size() const override
	{
		return _originalSize;
	}
	uint32 get_original_width() const override
	{
		return _originalSize.x;
	}
	uint32 get_original_height() const override
	{
		return _originalSize.y;
	}

	//------------------------------

	void set_bounds_sizing(Avo::ImageBoundsSizing p_sizeMode) override
	{
		_boundsSizing = p_sizeMode;
	}
	Avo::ImageBoundsSizing getBoundsSizing() const override
	{
		return _boundsSizing;
	}

	void setBoundsPositioning(float p_x, float p_y) override
	{
		_boundsPositioning.set(p_x, p_y);
	}
	void setBoundsPositioningX(float p_x) override
	{
		_boundsPositioning.x = p_x;
	}
	void setBoundsPositioningY(float p_y) override
	{
		_boundsPositioning.y = p_y;
	}
	Avo::Point<> getBoundsPositioning() const override
	{
		return _boundsPositioning;
	}
	float getBoundsPositioningX() const override
	{
		return _boundsPositioning.x;
	}
	float getBoundsPositioningY() const override
	{
		return _boundsPositioning.x;
	}

	//------------------------------

	void setScalingMethod(Avo::ImageScalingMethod p_scalingMethod) override
	{
		_scalingMethod = p_scalingMethod;
	}
	Avo::ImageScalingMethod getScalingMethod() const override
	{
		return _scalingMethod;
	}

	//------------------------------

	void set_opacity(float p_opacity) override
	{
		_opacity = p_opacity;
	}
	float get_opacity() const override
	{
		return _opacity;
	}

	//------------------------------

	float get_inner_width() const override
	{
		if (_boundsSizing != Avo::ImageBoundsSizing::Stretch &&
		    _boundsSizing == Avo::ImageBoundsSizing::Contain != _bounds.get_width()/_bounds.get_height() < _originalSize.x/(float)_originalSize.y)
		{
			return _bounds.get_height()*_originalSize.x/(float)_originalSize.y;
		}
		return _bounds.get_width();
	}
	float get_inner_height() const override
	{
		if (_boundsSizing != Avo::ImageBoundsSizing::Stretch &&
		    _boundsSizing == Avo::ImageBoundsSizing::Contain != _bounds.get_width()/_bounds.get_height() > _originalSize.x/(float)_originalSize.y)
		{
			return _bounds.get_width()*_originalSize.y/(float)_originalSize.x;
		}
		return _bounds.get_height();
	}
	Avo::Point<> get_inner_size() const override
	{
		return Avo::Point<>(get_inner_width(), get_inner_height());
	}
	Avo::Rectangle<> get_inner_bounds() const override
	{
		if (_boundsSizing == Avo::ImageBoundsSizing::Stretch)
		{
			return _bounds;
		}

		Avo::Rectangle<> innerBounds = _bounds;

		bool areBoundsProportionallyWider = _bounds.get_width()/_bounds.get_height() > _originalSize.x/(float)_originalSize.y;
		if (_boundsSizing == Avo::ImageBoundsSizing::Fill != areBoundsProportionallyWider)
		{
			innerBounds.set_width(_bounds.get_height()*_originalSize.x/(float)_originalSize.y);
		}
		else if (_boundsSizing == Avo::ImageBoundsSizing::Contain != areBoundsProportionallyWider)
		{
			innerBounds.set_height(_bounds.get_width()*_originalSize.y/(float)_originalSize.x);
		}

		innerBounds.move(_boundsPositioning.x*(_bounds.get_width() - innerBounds.get_width()), _boundsPositioning.y*(_bounds.get_height() - innerBounds.get_height()));

		return innerBounds;
	}

	void* getHandle() const override
	{
		return (void*)&_textureID;
	}
};

class OpenGlDrawingContext :
	public Avo::DrawingContext
{
private:
	static constexpr uint32 CIRCLE_RESOLUTION{50};
	static constexpr uint32 VERTEX_DATA_LENGTH{8};

	static bool s_areStaticVariablesInitialized;
	static Avo::Point<> s_unitCirclePoints[CIRCLE_RESOLUTION + 1];

	static void initializeStaticVariables()
	{
		if (s_areStaticVariablesInitialized)
		{
			return;
		}

		s_unitCirclePoints[0].set(1.f, 0.f);
		s_unitCirclePoints[CIRCLE_RESOLUTION].set(1.f, 0.f);
		for (uint32 a = 1; a < CIRCLE_RESOLUTION; a++)
		{
			s_unitCirclePoints[a].setPolar((float)Avo::TAU*a/(float)CIRCLE_RESOLUTION);
		}

		s_areStaticVariablesInitialized = true;
	}

private:
	LinuxWindow* _window;
	XWindow _window_handle;
	Display* _server;
	GLXContext _context;
	Avo::Color _backgroundColor;
	
	float _transformMatrix[9]{};
	Avo::Point<> _scale{1.f, 1.f};

	Avo::Color _current_color;
	OpenGlShader _renderShader;
	std::vector<OpenGlDrawCall> _drawCalls;
	std::vector<float> _vertexBuffer;

	std::stack<ClippingShape> _clippingShapeStack;
	Avo::LineJoin _lineJoin{Avo::LineJoin::Miter};
	Avo::LineCap _lineCap{Avo::LineCap::Round};

	Avo::Point<> _size;

	//------------------------------

	static void loadOpenGlFunctions()
	{
		if (areOpenGlFunctionsLoaded)
		{
			return;
		}
		
		__GLXextFuncPtr function = nullptr;
#define load(p_name) function = glXGetProcAddressARB((GLubyte const*)(#p_name)); memcpy(&p_name, &function, sizeof(p_name))

		load(glXCreateContextAttribsARB);
		load(glGenBuffers);
		load(glBindBuffer);
		load(glBufferData);
		load(glCreateShader);
		load(glShaderSource);
		load(glCompileShader);
		load(glGetShaderiv);
		load(glGetShaderInfoLog);
		load(glCreateProgram);
		load(glAttachShader);
		load(glLinkProgram);
		load(glDeleteProgram);
		load(glDeleteShader);
		load(glVertexAttribPointer);
		load(glEnableVertexAttribArray);
		load(glGenVertexArrays);
		load(glBindVertexArray);
		load(glUseProgram);
		load(glGetUniformLocation);
		load(glUniform1f);
		load(glUniform2f);
		load(glUniform3f);
		load(glUniform4f);
		load(glUniform1i);
		load(glUniform2i);
		load(glUniform3i);
		load(glUniform4i);
		load(glUniformMatrix3fv);
		load(glGenerateMipmap);

#undef load

		areOpenGlFunctionsLoaded = true;
	}
	/*
		This is called after the vertices have been added to the buffer.
	*/
	void addDrawCall(uint32 p_numberOfVertices, GLuint p_texture = 0u, GLenum p_primitiveType = GL_TRIANGLES, float p_opacity = 1.f)
	{
		OpenGlDrawCall drawCall;
		drawCall.vertexBufferStartIndex = uint32(_vertexBuffer.size()/VERTEX_DATA_LENGTH - p_numberOfVertices);
		drawCall.numberOfVertices = p_numberOfVertices;
		drawCall.primitiveType = p_primitiveType;
		drawCall.color = {_current_color, p_opacity};
		drawCall.renderMode = bool(p_texture) + 1;
		drawCall.texture = p_texture;
		_drawCalls.push_back(drawCall);
		memcpy(_drawCalls.back().transform, _transformMatrix, sizeof(_transformMatrix));
	}
	/*
		This is called after the vertices have been added to the buffer.
	*/
	void addDrawCall(uint32 p_numberOfVertices, float p_lineWidth, GLenum p_primitiveType = GL_TRIANGLES)
	{
		OpenGlDrawCall drawCall;
		drawCall.vertexBufferStartIndex = uint32(_vertexBuffer.size()/VERTEX_DATA_LENGTH - p_numberOfVertices);
		drawCall.numberOfVertices = p_numberOfVertices;
		drawCall.primitiveType = p_primitiveType;
		drawCall.color = _current_color;
		drawCall.renderMode = 0;
		drawCall.lineWidth = p_lineWidth;
		_drawCalls.push_back(drawCall);
		memcpy(_drawCalls.back().transform, _transformMatrix, sizeof(_transformMatrix));
	}

	static uint32 getCirclePointStrideForRadiusSquared(float p_radiusSquared)
	{
		return p_radiusSquared <= 25.f*25.f ? (p_radiusSquared <= 5.f*5.f ? 4 : 2) : 1;
	}
	/*
		The primitive type is GL_TRIANGLES
	*/
	static void createStrokedCornerRectangleGeometry(std::vector<float>& p_buffer, float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners p_corners)
	{
		if (p_right - p_left < p_corners.top_left_size_x + p_corners.top_right_size_y)
		{
			float factor = (p_right - p_left)/(p_corners.top_left_size_x + p_corners.top_right_size_x);
			p_corners.top_left_size_x *= factor;
			p_corners.top_right_size_x *= factor;
		}
		if (p_right - p_left < p_corners.bottom_left_size_x + p_corners.bottom_right_size_y)
		{
			float factor = (p_right - p_left)/(p_corners.bottom_left_size_x + p_corners.bottom_right_size_x);
			p_corners.bottom_left_size_x *= factor;
			p_corners.bottom_right_size_x *= factor;
		}
		if (p_bottom - p_top < p_corners.top_left_size_x + p_corners.bottom_left_size_y)
		{
			float factor = (p_bottom - p_top)/(p_corners.top_left_size_x + p_corners.bottom_left_size_y);
			p_corners.top_left_size_x *= factor;
			p_corners.bottom_left_size_x *= factor;
		}
		if (p_bottom - p_top < p_corners.top_right_size_x + p_corners.bottom_right_size_y)
		{
			float factor = (p_bottom - p_top)/(p_corners.top_right_size_x + p_corners.bottom_right_size_y);
			p_corners.top_right_size_x *= factor;
			p_corners.bottom_right_size_x *= factor;
		}

		p_buffer.insert(
			p_buffer.end(),
			{
				p_left + p_corners.top_left_size_x  , p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.top_left_size_x  , p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.top_left_size_x  , p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,

				p_left + p_corners.bottom_left_size_x  , p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.bottom_left_size_x  , p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.bottom_left_size_x  , p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,

				p_left, p_top + p_corners.top_left_size_y      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_left, p_top + p_corners.top_left_size_y      , 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottom_left_size_y, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottom_left_size_y, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottom_left_size_y, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_left, p_top + p_corners.top_left_size_y      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,

				p_right, p_top + p_corners.top_right_size_y      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_right, p_top + p_corners.top_right_size_y      , 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottom_right_size_y, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottom_right_size_y, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottom_right_size_y, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_right, p_top + p_corners.top_right_size_y      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f
			}
		);

		if (p_corners.top_left_type == Avo::RectangleCornerType::Cut)
		{
			Avo::Point<> normal{-p_corners.top_left_size_y, -p_corners.top_left_size_x};
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                         , p_top + p_corners.top_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_left                         , p_top + p_corners.top_left_size_y, -1.f         , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.top_left_size_x, p_top                         , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.top_left_size_x, p_top                         , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.top_left_size_x, p_top                         , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_left                         , p_top + p_corners.top_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.top_left_size_x*p_corners.top_left_size_y);
			float lastX = p_corners.top_left_size_x;
			float lastY = 0.f;
			Avo::Point<> lastNormal(-lastX, -lastY);
			lastNormal.normalize_fast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_left_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_left_size_y*s_unitCirclePoints[a].y;
				Avo::Point<> normal{-x, -y};
				normal.normalize_fast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - x    , p_top + p_corners.top_left_size_y - y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - x    , p_top + p_corners.top_left_size_y - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - x    , p_top + p_corners.top_left_size_y - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
					}
				);
				lastX = x;
				lastY = y;
				lastNormal = normal;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x  , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x        , p_top                                 , 0.f           , -1.f         , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x        , p_top                                 , 0.f           , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x        , p_top                                 , 0.f           , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, -lastNormal.x , -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x  , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.top_left_type == Avo::RectangleCornerType::Cut)
		{
			Avo::Point<> normal{p_corners.top_right_size_y, -p_corners.top_right_size_x};
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                          , p_top + p_corners.top_right_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_right                          , p_top + p_corners.top_right_size_y, 1.f          , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.top_right_size_x, p_top                          , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.top_right_size_x, p_top                          , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.top_right_size_x, p_top                          , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_right                          , p_top + p_corners.top_right_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.top_right_size_x*p_corners.top_right_size_y);
			float lastX = p_corners.top_right_size_x;
			float lastY = 0.f;
			Avo::Point<> lastNormal{lastX, -lastY};
			lastNormal.normalize_fast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_right_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_right_size_y*s_unitCirclePoints[a].y;
				Avo::Point<> normal{x, -y};
				normal.normalize_fast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + x    , p_top + p_corners.top_left_size_y - y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + x    , p_top + p_corners.top_left_size_y - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + x    , p_top + p_corners.top_left_size_y - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
				lastNormal = normal;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x        , p_top                                 , 0.f          , -1.f         , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x        , p_top                                 , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x        , p_top                                 , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_left_size_y - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.top_left_type == Avo::RectangleCornerType::Cut)
		{
			Avo::Point<> normal{-p_corners.bottom_left_size_y, p_corners.bottom_left_size_x};
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                            , p_bottom - p_corners.bottom_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_left                            , p_bottom - p_corners.bottom_left_size_y, -1.f         , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottom_left_size_x, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottom_left_size_x, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottom_left_size_x, p_bottom                            , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_left                            , p_bottom - p_corners.bottom_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottom_left_size_x*p_corners.bottom_left_size_y);
			float lastX = p_corners.bottom_left_size_x;
			float lastY = 0.f;
			Avo::Point<> lastNormal{-lastX, lastY};
			lastNormal.normalize_fast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_left_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_left_size_y*s_unitCirclePoints[a].y;
				Avo::Point<> normal{-x, y};
				normal.normalize_fast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - x    , p_bottom - p_corners.bottom_left_size_y + y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - x    , p_bottom - p_corners.bottom_left_size_y + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - x    , p_bottom - p_corners.bottom_left_size_y + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
				lastNormal = normal;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x        , p_bottom                                    , 0.f          , 1.f          , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x        , p_bottom                                    , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x        , p_bottom                                    , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.top_left_type == Avo::RectangleCornerType::Cut)
		{
			Avo::Point<> normal{p_corners.bottom_left_size_y, p_corners.bottom_left_size_x};
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                             , p_bottom - p_corners.bottom_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_right                             , p_bottom - p_corners.bottom_left_size_y, 1.f          , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottom_right_size_x, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottom_right_size_x, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottom_right_size_x, p_bottom                            , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_right                             , p_bottom - p_corners.bottom_left_size_y, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottom_right_size_x*p_corners.bottom_right_size_y);
			float lastX = p_corners.bottom_right_size_x;
			float lastY = 0.f;
			Avo::Point<> lastNormal{lastX, lastY};
			lastNormal.normalize_fast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_right_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_right_size_y*s_unitCirclePoints[a].y;
				Avo::Point<> normal{x, y};
				normal.normalize_fast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + x    , p_bottom - p_corners.bottom_right_size_y + y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + x    , p_bottom - p_corners.bottom_right_size_y + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + x    , p_bottom - p_corners.bottom_right_size_y + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
				lastNormal = normal;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x        , p_bottom                                     , 0.f          , 1.f          , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x        , p_bottom                                     , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x        , p_bottom                                     , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
	}
	/*
		The primitive type is GL_TRIANGLES
	*/
	static void createFilledCornerRectangleGeometry(std::vector<float>& p_buffer, float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners p_corners)
	{
		p_left -= 0.5f;
		p_top -= 0.5f;
		p_right += 0.5f;
		p_bottom += 0.5f;
		if (p_right - p_left < p_corners.top_left_size_x + p_corners.top_right_size_y)
		{
			float factor = (p_right - p_left)/(p_corners.top_left_size_x + p_corners.top_right_size_x);
			p_corners.top_left_size_x *= factor;
			p_corners.top_right_size_x *= factor;
		}
		if (p_right - p_left < p_corners.bottom_left_size_x + p_corners.bottom_right_size_y)
		{
			float factor = (p_right - p_left)/(p_corners.bottom_left_size_x + p_corners.bottom_right_size_x);
			p_corners.bottom_left_size_x *= factor;
			p_corners.bottom_right_size_x *= factor;
		}
		if (p_bottom - p_top < p_corners.top_left_size_x + p_corners.bottom_left_size_y)
		{
			float factor = (p_bottom - p_top)/(p_corners.top_left_size_x + p_corners.bottom_left_size_y);
			p_corners.top_left_size_x *= factor;
			p_corners.bottom_left_size_x *= factor;
		}
		if (p_bottom - p_top < p_corners.top_right_size_x + p_corners.bottom_right_size_y)
		{
			float factor = (p_bottom - p_top)/(p_corners.top_right_size_x + p_corners.bottom_right_size_y);
			p_corners.top_right_size_x *= factor;
			p_corners.bottom_right_size_x *= factor;
		}

		// Create the "body"
		float width = p_right - p_left;
		p_buffer.insert(
			p_buffer.end(),
			{
				// Horizontal center bar
				p_left , p_top + p_corners.top_left_size_y       , 1.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_top + p_corners.top_right_size_y      , 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottom_right_size_y, 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottom_right_size_y, 1.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_left , p_bottom - p_corners.bottom_left_size_y , 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_left , p_top + p_corners.top_left_size_y       , 0.f, width, 0.5f, 2.f, 0.f, 0.f,

				// Top center bar
				p_left + p_corners.top_left_size_x  , p_top                          , 0.f , p_corners.top_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top                          , 0.f , p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top + p_corners.top_right_size_y, 0.5f, p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.top_right_size_x, p_top + p_corners.top_right_size_y, 0.5f, p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.top_left_size_x  , p_top + p_corners.top_left_size_y , 0.5f, p_corners.top_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.top_left_size_x  , p_top                          , 0.f , p_corners.top_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f,

				// Bottom center bar
				p_left + p_corners.bottom_left_size_x  , p_bottom                             , 0.f , p_corners.bottom_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom                             , 0.f , p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom - p_corners.bottom_right_size_y, 0.5f, p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottom_right_size_x, p_bottom - p_corners.bottom_right_size_y, 0.5f, p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.bottom_left_size_x  , p_bottom - p_corners.bottom_left_size_y , 0.5f, p_corners.bottom_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.bottom_left_size_x  , p_bottom                             , 0.f , p_corners.bottom_left_size_y*2.f , 0.5f, 2.f, 0.f, 0.f
			}
		);

		// Create the corners
		if (p_corners.top_left_type == Avo::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.top_left_size_x*p_corners.top_left_size_y/(float)Avo::Point<>::getLengthFast(p_corners.top_left_size_x, p_corners.top_left_size_y);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                         , p_top + p_corners.top_left_size_y, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.top_left_size_x, p_top                         , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.top_left_size_x, p_top + p_corners.top_left_size_y, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.top_left_size_x*p_corners.top_left_size_y);
			float lastX = p_corners.top_left_size_x;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_left_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_left_size_y*s_unitCirclePoints[a].y;
				// Length of vector (0.5*(x1 + x2), 0.5*(y1 + y2)), doubled because the anti-aliasing attribute of the inner vertex is 0.5 (we don't want anti-aliasing for that vertex)
				auto distance = (float)Avo::Point<>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.top_left_size_x        , p_top + p_corners.top_left_size_y        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - x    , p_top + p_corners.top_left_size_y - y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.top_left_size_x        , p_top + p_corners.top_left_size_y        , 0.5f, p_corners.top_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x - lastX, p_top + p_corners.top_left_size_y - lastY, 0.f , p_corners.top_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.top_left_size_x        , p_top                                 , 0.f , p_corners.top_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.top_right_type == Avo::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.top_right_size_x*p_corners.top_right_size_y/(float)Avo::Point<>::getLengthFast(p_corners.top_right_size_x, p_corners.top_right_size_y);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                          , p_top + p_corners.top_right_size_y, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.top_right_size_x, p_top                          , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.top_right_size_x, p_top + p_corners.top_right_size_y, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.top_right_size_x*p_corners.top_right_size_y);
			float lastX = p_corners.top_right_size_x;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.top_right_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.top_right_size_y*s_unitCirclePoints[a].y;
				auto distance = (float)Avo::Point<>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.top_right_size_x        , p_top + p_corners.top_right_size_y        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_right_size_y - lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + x    , p_top + p_corners.top_right_size_y - y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.top_right_size_x        , p_top + p_corners.top_right_size_y        , 0.5f, p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x + lastX, p_top + p_corners.top_right_size_y - lastY, 0.f , p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.top_right_size_x        , p_top                                  , 0.f , p_corners.top_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}

		if (p_corners.bottom_left_type == Avo::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.bottom_left_size_x*p_corners.bottom_left_size_y/(float)Avo::Point<>::getLengthFast(p_corners.bottom_left_size_x, p_corners.bottom_left_size_y);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                            , p_bottom - p_corners.bottom_left_size_y, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.bottom_left_size_x, p_bottom                            , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.bottom_left_size_x, p_bottom - p_corners.bottom_left_size_y, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottom_left_size_x*p_corners.bottom_left_size_y);
			float lastX = p_corners.bottom_left_size_x;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.bottom_left_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.bottom_left_size_y*s_unitCirclePoints[a].y;
				auto distance = (float)Avo::Point<>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottom_left_size_x        , p_bottom - p_corners.bottom_left_size_y        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - x    , p_bottom - p_corners.bottom_left_size_y + y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottom_left_size_x        , p_bottom - p_corners.bottom_left_size_y        , 0.5f, p_corners.bottom_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x - lastX, p_bottom - p_corners.bottom_left_size_y + lastY, 0.f , p_corners.bottom_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottom_left_size_x        , p_bottom                                    , 0.f , p_corners.bottom_left_size_y*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}

		if (p_corners.bottom_right_type == Avo::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.bottom_right_size_x*p_corners.bottom_right_size_y/(float)Avo::Point<>::getLengthFast(p_corners.bottom_right_size_x, p_corners.bottom_right_size_y);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                             , p_bottom - p_corners.bottom_right_size_y, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.bottom_right_size_x, p_bottom                             , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.bottom_right_size_x, p_bottom - p_corners.bottom_right_size_y, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottom_right_size_x*p_corners.bottom_right_size_y);
			float lastX = p_corners.bottom_right_size_x;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.bottom_right_size_x*s_unitCirclePoints[a].x;
				float y = p_corners.bottom_right_size_y*s_unitCirclePoints[a].y;
				auto distance = (float)Avo::Point<>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottom_right_size_x        , p_bottom - p_corners.bottom_right_size_y        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + x    , p_bottom - p_corners.bottom_right_size_y + y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
					}
				);
				lastX = x;
				lastY = y;
			}
			if (CIRCLE_RESOLUTION % (4*stride))
			{
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottom_right_size_x        , p_bottom - p_corners.bottom_right_size_y        , 0.5f, p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x + lastX, p_bottom - p_corners.bottom_right_size_y + lastY, 0.f , p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottom_right_size_x        , p_bottom                                     , 0.f , p_corners.bottom_right_size_y*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}
	}

public:
	explicit OpenGlDrawingContext(Avo::Window* p_window) :
		_window((LinuxWindow*)p_window), _window_handle((XWindow)p_window->get_native_handle())
	{
		loadOpenGlFunctions();

		initializeStaticVariables();

		//------------------------------
		// Create the OpenGL context.
		// Need to find the framebuffer configuration that was used to create the window, because our context should match it.

		_server = XOpenDisplay(nullptr);

		auto* linuxWindow = (LinuxWindow*)p_window;

		_dip_to_pixel_factor = linuxWindow->get_dip_to_pixel_factor();

		XWindowAttributes windowAttributes; // Used to match the VisualID
		XGetWindowAttributes(_server, _window_handle, &windowAttributes);

		int numberOfConfigurations = 0;
		GLXFBConfig* framebufferConfigurations = glXChooseFBConfig(_server, DefaultScreen(_server), nullptr, &numberOfConfigurations);
		GLXFBConfig framebufferConfiguration = *framebufferConfigurations;
		// for (uint32 a = 0; a < numberOfConfigurations; a++)
		for (auto const a : Avo::Range{numberOfConfigurations}) {
			XVisualInfo* visualInfo = glXGetVisualFromFBConfig(_server, framebufferConfigurations[a]);
			if (!visualInfo) {
				continue;
			}
			if (visualInfo->visualid == windowAttributes.visual->visualid) {
				framebufferConfiguration = framebufferConfigurations[a];
				XFree(visualInfo);
				break;
			}
			XFree(visualInfo);
		}

		auto const contextAttributes[] = std::array{
			// Our minimum supported OpenGL version is 3.3
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
			#ifdef _DEBUG
			| GLX_CONTEXT_DEBUG_BIT_ARB
			#endif
			, 0 // Null terminator
		};
		_context = glXCreateContextAttribsARB(_server, framebufferConfiguration, nullptr, true, contextAttributes);

		XFree(framebufferConfigurations);

		//------------------------------

		glXMakeCurrent(_server, _window_handle, _context);
		glViewport(0, 0, linuxWindow->get_width()*_dip_to_pixel_factor, linuxWindow->get_height()*_dip_to_pixel_factor);
		_size = linuxWindow->get_size();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//------------------------------

		static char const* vertexShaderSource =
R"(
#version 330 core
layout (location = 0) in vec2 in_vertex;
layout (location = 1) in vec4 in_antiAliasAttribute;
layout (location = 2) in vec2 in_textureCoordinate;

out vec4 pass_antiAliasAttribute;
out vec2 pass_textureCoordinates;

uniform mat3 u_transform;
uniform mat3 u_viewTransform;
uniform int u_renderMode;
uniform float u_lineWidth;

void main()
{
	if (u_renderMode != 0)
	{
		pass_antiAliasAttribute = in_antiAliasAttribute;
		gl_Position = vec4(u_viewTransform*u_transform*vec3(in_vertex, 1.f), 1.f);
	}
	else
	{
		pass_antiAliasAttribute.xy = in_antiAliasAttribute.xy*(u_lineWidth*0.5f);
		gl_Position = vec4(u_viewTransform*u_transform*vec3(in_vertex + pass_antiAliasAttribute.xy, 1.f), 1.f);
	}
	if (u_renderMode == 2)
	{
		pass_textureCoordinates = in_textureCoordinate;
	}
}
)";

		// pass_edgeAttribute contains the data needed to perform anti-aliasing using automatic vertex attribute interpolation.
		static char const* fragmentShaderSource =
R"(
#version 330 core
in vec4 pass_antiAliasAttribute;
in vec2 pass_textureCoordinates;

out vec4 out_fragmentColor;

uniform vec4 u_color;
uniform int u_renderMode;
uniform float u_lineWidth;
uniform sampler2D u_texture;

void main()
{
	if (u_renderMode == 0) {
		out_fragmentColor = vec4(u_color.rgb, u_color.a*min(pass_antiAliasAttribute.z*u_lineWidth, (1.f - pass_antiAliasAttribute.z)*u_lineWidth));
	}
	else if (u_renderMode == 1) {
		out_fragmentColor = vec4(u_color.rgb, u_color.a*min(min(pass_antiAliasAttribute.z*pass_antiAliasAttribute.w, (1.f - pass_antiAliasAttribute.z)*pass_antiAliasAttribute.w), min(pass_antiAliasAttribute.x*pass_antiAliasAttribute.y, (1.f - pass_antiAliasAttribute.x)*pass_antiAliasAttribute.y)));
	}
	else if (u_renderMode == 2) {
		out_fragmentColor = texture(u_texture, pass_textureCoordinates);
		out_fragmentColor.a *= min(min(pass_antiAliasAttribute.z*pass_antiAliasAttribute.w, (1.f - pass_antiAliasAttribute.z)*pass_antiAliasAttribute.w), min(pass_antiAliasAttribute.x*pass_antiAliasAttribute.y, (1.f - pass_antiAliasAttribute.x)*pass_antiAliasAttribute.y));
	}
}
)";
		_renderShader.compile(vertexShaderSource, fragmentShaderSource);
		_renderShader.use();

		// Identity matrix
		_transformMatrix[0] = 1.f;
		_transformMatrix[4] = 1.f;
		_transformMatrix[8] = 1.f;

		_renderShader.setUniformMatrix3x3("u_transform", _transformMatrix);

		//------------------------------

		_drawCalls.reserve(256);
		_vertexBuffer.reserve(8192);
	}
	~OpenGlDrawingContext() final
	{
		glXDestroyContext(_server, _context);
		XCloseDisplay(_server);
	}

	//------------------------------

	void beginDrawing() override
	{
		glXMakeCurrent(_server, _window_handle, _context);
		_drawCalls.clear();
		_vertexBuffer.clear();

		//glClear(GL_COLOR_BUFFER_BIT);
	}
	void finishDrawing(std::vector<Avo::Rectangle<>> const& p_updatedRectangles) override
	{
		// GL_STATIC_DRAW hints that the buffer is modified once and used many times
		_renderShader.setVertexData(_vertexBuffer.data(), _vertexBuffer.size()*sizeof(float), VERTEX_DATA_LENGTH*sizeof(float), GL_STATIC_DRAW);
		_renderShader.configureVertexShaderInput(0, 2, 0);
		_renderShader.configureVertexShaderInput(1, 4, 2);
		_renderShader.configureVertexShaderInput(2, 2, 6);

		for (auto& drawCall : _drawCalls)
		{
			_renderShader.setUniformColor("u_color", drawCall.color);
			_renderShader.setUniformMatrix3x3("u_transform", drawCall.transform);
			if (drawCall.renderMode == 0)
			{
				_renderShader.setUniformValue("u_lineWidth", drawCall.lineWidth);
			}
			else if (drawCall.renderMode == 2)
			{
				glBindTexture(GL_TEXTURE_2D, drawCall.texture);
			}
			_renderShader.setUniformValue("u_renderMode", drawCall.renderMode);
			_renderShader.draw(drawCall.vertexBufferStartIndex, drawCall.numberOfVertices, drawCall.primitiveType);
		}

		//------------------------------

		glXSwapBuffers(_server, _window_handle);
	}

	//------------------------------

	Avo::DrawingState* createDrawingState() override
	{
		return nullptr;
	}

	void saveDrawingState(Avo::DrawingState* p_drawingState) override
	{
	}

	void restoreDrawingState(Avo::DrawingState* p_drawingState) override
	{
	}

	//------------------------------

	void set_is_fullscreen(bool p_isFullscreen) override
	{
	}
	void switchFullscreen() override
	{
	}
	bool getIsFullscreen() override
	{
	}

	//------------------------------

	void enableVsync() override
	{
	}
	void disableVsync() override
	{
	}
	bool getIsVsyncEnabled() override
	{
		return true;
	}

	//------------------------------

	void setBackgroundColor(Avo::Color p_color) override
	{
		// If we try to set the background color here directly, some methods block until XNextEvent in the event thread returns...
		// So send the color in a client event instead and let the event thread set the background color of the window.

		//Display* server = _window->getServer();
		//XLockDisplay(server);
		//
		//XEvent event;
		//event.type = ClientMessage;
		//event.xclient.message_type = _window->getBackgroundColorMessageType();
		//event.xclient.format = 32; // Using data.l, 32 bits per data item
		//event.xclient.data.l[0] = long(p_color.red*65535);
		//event.xclient.data.l[1] = long(p_color.green*65535);
		//event.xclient.data.l[2] = long(p_color.blue*65535);
		//event.xclient.display = server;
		//event.xclient.window = _window_handle;
		//event.xclient.send_event = true;
		//XSendEvent(server, _window_handle, false, 0, &event);
		//XFlush(server);
		//
		//XUnlockDisplay(server);

		//XColor xColor;
		//xColor.red = (unsigned short)(p_color.red*65535);
		//xColor.green = (unsigned short)(p_color.green*65535);
		//xColor.blue = (unsigned short)(p_color.blue*65535);
		//xColor.flags = DoRed | DoGreen | DoBlue;
		//XAllocColor(_server, _window->getColormap(), &xColor);
		//XSetWindowBackground(_server, _window_handle, xColor.pixel);

		_backgroundColor = p_color;
	}
	Avo::Color getBackgroundColor() override
	{
		return _backgroundColor;
	}

	//------------------------------

	float getDpi() override
	{
		return _dip_to_pixel_factor*NORMAL_DPI;
	}
	void setDpi(float p_dpi) override
	{
		_dip_to_pixel_factor = p_dpi/NORMAL_DPI;
	}

	//------------------------------

	void move_origin(Avo::Point<> offset) override
	{
		_transformMatrix[7] -= offset.x;
		_transformMatrix[8] -= offset.y;
	}
	void setOrigin(Avo::Point<> origin) override
	{
		_transformMatrix[7] = -origin.x;
		_transformMatrix[8] = -origin.y;
	}
	Avo::Point<> getOrigin() override
	{
		return {-_transformMatrix[7], -_transformMatrix[8]};
	}

	//------------------------------

	void scale(float p_scale) override
	{
		scale(p_scale, p_scale);
	}
	void scale(float p_scaleX, float p_scaleY) override
	{
		_scale.x *= p_scaleX;
		_scale.y *= p_scaleY;
		_transformMatrix[0] *= p_scaleX;
		_transformMatrix[1] *= p_scaleY;
		_transformMatrix[3] *= p_scaleX;
		_transformMatrix[4] *= p_scaleY;
	}
	void scale(float p_scale, Avo::Point<> origin) override
	{
		scale(p_scale, p_scale, origin.x, origin.y);
	}
	void scale(float p_scaleX, float p_scaleY, Avo::Point<> origin) override
	{
		scale(p_scaleX, p_scaleY, origin.x, origin.y);
	}
	void scale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale, p_scale, p_originX, p_originY);
	}
	void scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		_scale.x *= p_scaleX;
		_scale.y *= p_scaleY;
		_transformMatrix[0] *= p_scaleX;
		_transformMatrix[1] *= p_scaleX;
		_transformMatrix[3] *= p_scaleY;
		_transformMatrix[4] *= p_scaleY;
		_transformMatrix[6] += (p_originX - _transformMatrix[6])*(1.f - p_scaleX);
		_transformMatrix[7] += (p_originY - _transformMatrix[7])*(1.f - p_scaleY);
	}
	void set_scale(float p_scale) override
	{
		scale(p_scale/_scale.x, p_scale/_scale.y);
	}
	void set_scale(float p_scaleX, float p_scaleY) override
	{
		scale(p_scaleX/_scale.x, p_scaleY/_scale.y);
	}
	void set_scale(float p_scale, Avo::Point<> origin) override
	{
		scale(p_scale/_scale.x, p_scale/_scale.y, origin.x, origin.y);
	}
	void set_scale(float p_scaleX, float p_scaleY, Avo::Point<> origin) override
	{
		scale(p_scaleX/_scale.x, p_scaleY/_scale.y, origin.x, origin.y);
	}
	void set_scale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale/_scale.x, p_scale/_scale.y, p_originX, p_originY);
	}
	void set_scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		scale(p_scaleX/_scale.x, p_scaleY/_scale.y, p_originX, p_originY);
	}
	Avo::Point<> get_scale() override
	{
		return _scale;
	}
	float get_scale_x() override
	{
		return _scale.x;
	}
	float get_scale_y() override
	{
		return _scale.y;
	}

	//------------------------------

	void rotate(float p_radians) override
	{
		float cos = std::cos(p_radians);
		float sin = std::sin(p_radians);

		float previousMatrix[9];
		memcpy(previousMatrix, _transformMatrix, sizeof(previousMatrix));

		/*
			Matrix multiplication:
			[cos -sin  0]   [m[0] m[3] m[6]]   [m[0]cos - m[1]sin  m[3]cos - m[4]sin  m[6]cos - m[7]sin]
			[sin  cos  0]*[m[1] m[4] m[7]] = [m[0]sin + m[1]cos  m[3]sin + m[4]cos  m[6]sin + m[7]cos]
			[0    0    1]   [0    0    1   ]   [0                  0                  1                ]
		*/
		_transformMatrix[0] = cos*previousMatrix[0] - sin*previousMatrix[1];
		_transformMatrix[1] = sin*previousMatrix[0] + cos*previousMatrix[1];
		_transformMatrix[3] = cos*previousMatrix[3] - sin*previousMatrix[4];
		_transformMatrix[4] = sin*previousMatrix[3] + cos*previousMatrix[4];
		_transformMatrix[6] = cos*previousMatrix[6] - sin*previousMatrix[7];
		_transformMatrix[7] = sin*previousMatrix[6] + cos*previousMatrix[7];
	}
	void rotate(float p_radians, Avo::Point<> origin) override
	{
		rotate(p_radians, origin.x, origin.y);
	}
	void rotate(float p_radians, float p_originX, float p_originY) override
	{
		float cos = std::cos(p_radians);
		float sin = std::sin(p_radians);

		float previousMatrix[9];
		memcpy(previousMatrix, _transformMatrix, sizeof(previousMatrix));
		previousMatrix[6] -= p_originX;
		previousMatrix[7] -= p_originY;

		/*
			Matrix multiplication:
			[cos -sin  0]   [m[0] m[3] m[6]]   [m[0]cos - m[1]sin  m[3]cos - m[4]sin  m[6]cos - m[7]sin]
			[sin  cos  0]*[m[1] m[4] m[7]] = [m[0]sin + m[1]cos  m[3]sin + m[4]cos  m[6]sin + m[7]cos]
			[0    0    1]   [0    0    1   ]   [0                  0                  1                ]
		*/
		_transformMatrix[0] = cos*previousMatrix[0] - sin*previousMatrix[1];
		_transformMatrix[1] = sin*previousMatrix[0] + cos*previousMatrix[1];
		_transformMatrix[3] = cos*previousMatrix[3] - sin*previousMatrix[4];
		_transformMatrix[4] = sin*previousMatrix[3] + cos*previousMatrix[4];
		_transformMatrix[6] = cos*previousMatrix[6] - sin*previousMatrix[7] + p_originX;
		_transformMatrix[7] = sin*previousMatrix[6] + cos*previousMatrix[7] + p_originY;
	}

	//------------------------------

	void reset_transformations() override
	{
		_transformMatrix[0] = 1.f;
		_transformMatrix[1] = 0.f;
		_transformMatrix[2] = 0.f;
		_transformMatrix[3] = 0.f;
		_transformMatrix[4] = 1.f;
		_transformMatrix[5] = 0.f;
		_transformMatrix[6] = 0.f;
		_transformMatrix[7] = 0.f;
		_transformMatrix[8] = 1.f;
	}

	//------------------------------

	void set_size(Avo::Point<> size) override
	{
		set_size(size.x, size.y);
	}
	void set_size(float p_width, float p_height) override
	{
		glXMakeCurrent(_server, _window_handle, _context);
		glViewport(0, 0, p_width*_dip_to_pixel_factor, p_height*_dip_to_pixel_factor);
		_size.set(p_width, p_height);

		// Column-major order matrix
		float transform[9] =
		{
			2.f/_size.x, 0.f          , 0.f,
			0.f         , -2.f/_size.y, 0.f,
			-1.f        , 1.f          , 1.f
		};
		_renderShader.setUniformMatrix3x3("u_viewTransform", transform);
	}
	Avo::Point<> get_size() override
	{
		return _size;
	}

	//------------------------------

	void clear(Avo::Color p_color) override
	{
		glClearColor(p_color.red, p_color.green, p_color.blue, p_color.alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void clear() override
	{
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//------------------------------

	void fill_rectangle(float p_left, float p_top, float p_right, float p_bottom) override
	{
		// Anti-aliasing requires an extra row of pixels
		p_left -= 0.5f;
		p_top -= 0.5f;
		p_right += 0.5f;
		p_bottom += 0.5f;

		float height = p_bottom - p_top;
		float width = p_right - p_left;

		_vertexBuffer.insert(
			_vertexBuffer.end(),
			{
				p_left,  p_top,    1.f, height, 0.f, width, 0.f, 0.f,
				p_left,  p_bottom, 0.f, height, 0.f, width, 0.f, 0.f,
				p_right, p_bottom, 0.f, height, 1.f, width, 0.f, 0.f,

				p_right, p_bottom, 1.f, height, 0.f, width, 0.f, 0.f,
				p_left,  p_top,    0.f, height, 1.f, width, 0.f, 0.f,
				p_right, p_top,    0.f, height, 0.f, width, 0.f, 0.f
			}
		);
		addDrawCall(6u);
	}
	void fill_rectangle(Avo::Point<> p_position, Avo::Point<> size) override
	{
		fill_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y);
	}
	void fill_rectangle(Avo::Rectangle<> p_rectangle) override
	{
		fill_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
	}
	void fill_rectangle(float p_width, float p_height) override
	{
		fill_rectangle(0.f, 0.f, p_width, p_height);
	}
	void fill_rectangle(Avo::Point<> size) override
	{
		fill_rectangle(0.f, 0.f, size.x, size.y);
	}

	void fill_rectangle(Avo::Rectangle<> p_rectangle, Avo::RectangleCorners const& p_rectangleCorners) override
	{
		fill_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_rectangleCorners);
	}
	void fill_rectangle(Avo::Point<> p_position, Avo::Point<> size, Avo::RectangleCorners const& p_rectangleCorners) override
	{
		fill_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, p_rectangleCorners);
	}
	void fill_rectangle(Avo::Point<> size, Avo::RectangleCorners const& p_rectangleCorners) override
	{
		fill_rectangle(0.f, 0.f, size.x, size.y, p_rectangleCorners);
	}
	void fill_rectangle(float p_width, float p_height, Avo::RectangleCorners const& p_rectangleCorners) override
	{
		fill_rectangle(0.f, 0.f, p_width, p_height, p_rectangleCorners);
	}
	void fill_rectangle(float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners const& p_rectangleCorners) override
	{
		size_t sizeBefore = _vertexBuffer.size();
		createFilledCornerRectangleGeometry(_vertexBuffer, p_left, p_top, p_right, p_bottom, p_rectangleCorners);
		addDrawCall((_vertexBuffer.size() - sizeBefore)/VERTEX_DATA_LENGTH);
	}

	void fill_rounded_rectangle(Avo::Rectangle<> p_rectangle, float p_radius) override
	{
		fill_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, Avo::RectangleCorners(p_radius));
	}
	void fill_rounded_rectangle(Avo::Point<> p_position, Avo::Point<> size, float p_radius) override
	{
		fill_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, Avo::RectangleCorners(p_radius));
	}
	void fill_rounded_rectangle(Avo::Point<> size, float p_radius) override
	{
		fill_rectangle(0.f, 0.f, size.x, size.y, Avo::RectangleCorners(p_radius));
	}
	void fill_rounded_rectangle(float p_width, float p_height, float p_radius) override
	{
		fill_rectangle(0.f, 0.f, p_width, p_height, Avo::RectangleCorners(p_radius));
	}
	void fill_rounded_rectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) override
	{
		fill_rectangle(p_left, p_top, p_right, p_bottom, Avo::RectangleCorners(p_radius));
	}

	//------------------------------

	void stroke_rectangle(Avo::Rectangle<> p_rectangle, float p_strokeWidth) override
	{
		stroke_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_strokeWidth);
	}
	void stroke_rectangle(Avo::Point<> p_position, Avo::Point<> size, float p_strokeWidth) override
	{
		stroke_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, p_strokeWidth);
	}
	void stroke_rectangle(Avo::Point<> size, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, size.x, size.y, p_strokeWidth);
	}
	void stroke_rectangle(float p_width, float p_height, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, p_width, p_height, p_strokeWidth);
	}
	void stroke_rectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth) override
	{
		// Anti-aliasing requires an extra row of pixels
		p_strokeWidth += 1.f;
		p_left -= p_strokeWidth*0.5f;
		p_top -= p_strokeWidth*0.5f;
		p_right += p_strokeWidth*0.5f;
		p_bottom += p_strokeWidth*0.5f;

		float height = p_bottom - p_top;
		float width = p_right - p_left;
		_vertexBuffer.insert(
			_vertexBuffer.end(),
			{
				// Top
				p_left , p_top                , 1.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_left , p_top + p_strokeWidth, 0.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_right, p_top + p_strokeWidth, 0.f, p_strokeWidth, 1.f, width, 0.f, 0.f,
				p_right, p_top + p_strokeWidth, 1.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_left , p_top                , 0.f, p_strokeWidth, 1.f, width, 0.f, 0.f,
				p_right, p_top                , 0.f, p_strokeWidth, 0.f, width, 0.f, 0.f,

				// Bottom
				p_left , p_bottom - p_strokeWidth, 1.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_left , p_bottom                , 0.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_right, p_bottom                , 0.f, p_strokeWidth, 1.f, width, 0.f, 0.f,
				p_right, p_bottom                , 1.f, p_strokeWidth, 0.f, width, 0.f, 0.f,
				p_left , p_bottom - p_strokeWidth, 0.f, p_strokeWidth, 1.f, width, 0.f, 0.f,
				p_right, p_bottom - p_strokeWidth, 0.f, p_strokeWidth, 0.f, width, 0.f, 0.f,

				// Left
				p_left                , p_top + p_strokeWidth - 1.f   , 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_left                , p_bottom - p_strokeWidth + 1.f, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_strokeWidth, p_bottom - p_strokeWidth + 1.f, 0.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_strokeWidth, p_bottom - p_strokeWidth + 1.f, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_left                , p_top + p_strokeWidth - 1.f   , 0.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_strokeWidth, p_top + p_strokeWidth - 1.f   , 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,

				// Right
				p_right - p_strokeWidth, p_top + p_strokeWidth - 1.f   , 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_strokeWidth, p_bottom - p_strokeWidth + 1.f, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_right                , p_bottom - p_strokeWidth + 1.f, 0.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_right                , p_bottom - p_strokeWidth + 1.f, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_strokeWidth, p_top + p_strokeWidth - 1.f   , 0.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
				p_right                , p_top + p_strokeWidth - 1.f   , 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f,
			}
		);
		addDrawCall(24);
	}

	void stroke_rectangle(Avo::Rectangle<> p_rectangle, Avo::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		stroke_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_rectangleCorners, p_strokeWidth);
	}
	void stroke_rectangle(Avo::Point<> p_position, Avo::Point<> size, Avo::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		stroke_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, p_rectangleCorners, p_strokeWidth);
	}
	void stroke_rectangle(Avo::Point<> size, Avo::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, size.x, size.y, p_rectangleCorners, p_strokeWidth);
	}
	void stroke_rectangle(float p_width, float p_height, Avo::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, p_width, p_height, p_rectangleCorners, p_strokeWidth);
	}
	void stroke_rectangle(float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		p_strokeWidth += 1.f;
		size_t sizeBefore = _vertexBuffer.size();
		createStrokedCornerRectangleGeometry(_vertexBuffer, p_left, p_top, p_right, p_bottom, p_rectangleCorners);
		addDrawCall((_vertexBuffer.size() - sizeBefore)/VERTEX_DATA_LENGTH, p_strokeWidth);
	}

	void stroke_rounded_rectangle(Avo::Rectangle<> p_rectangle, float p_radius, float p_strokeWidth) override
	{
		stroke_rectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, Avo::RectangleCorners(p_radius), p_strokeWidth);
	}
	void stroke_rounded_rectangle(Avo::Point<> p_position, Avo::Point<> size, float p_radius, float p_strokeWidth) override
	{
		stroke_rectangle(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, Avo::RectangleCorners(p_radius), p_strokeWidth);
	}
	void stroke_rounded_rectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth) override
	{
		stroke_rectangle(p_left, p_top, p_right, p_bottom, Avo::RectangleCorners(p_radius), p_strokeWidth);
	}

	void stroke_rounded_rectangle(Avo::Point<> size, float p_radius, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, size.x, size.y, Avo::RectangleCorners(p_radius), p_strokeWidth);
	}
	void stroke_rounded_rectangle(float p_width, float p_height, float p_radius, float p_strokeWidth) override
	{
		stroke_rectangle(0.f, 0.f, p_width, p_height, Avo::RectangleCorners(p_radius), p_strokeWidth);
	}

	//------------------------------

	void fill_circle(Avo::Point<> p_position, float p_radius) override
	{
		fill_circle(p_position.x, p_position.y, p_radius);
	}
	void fill_circle(float p_x, float p_y, float p_radius) override
	{
		p_radius += 0.5f;
		_vertexBuffer.insert(_vertexBuffer.end(), {p_x, p_y, 0.5f, p_radius*2.f, 0.5f, 2.f, 0.f, 0.f}); // Only want anti-aliasing on the outer edges of the triangles.
		for (uint32 a = 0; a <= CIRCLE_RESOLUTION; a++)
		{
			_vertexBuffer.insert(_vertexBuffer.end(), {s_unitCirclePoints[a].x*p_radius + p_x, s_unitCirclePoints[a].y*p_radius + p_y, 0.f, p_radius*2.f, 0.5f, 2.f, 0.f, 0.f,});
		}
		addDrawCall(CIRCLE_RESOLUTION + 2, 0u, (GLenum)GL_TRIANGLE_FAN);
	}

	void stroke_circle(Avo::Point<> p_position, float p_radius, float p_strokeWidth) override
	{
		stroke_circle(p_position.x, p_position.y, p_radius, p_strokeWidth);
	}
	void stroke_circle(float p_x, float p_y, float p_radius, float p_strokeWidth) override
	{
		p_strokeWidth += 1.f; // Anti-aliasing needs extra pixels
		p_radius += p_strokeWidth*0.5f;

		float innerAntiAliasingStart = 1.f - p_radius/p_strokeWidth;
		_vertexBuffer.insert(_vertexBuffer.end(), {p_x, p_y, innerAntiAliasingStart, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f});
		for (uint32 a = 0; a <= CIRCLE_RESOLUTION; a++)
		{
			_vertexBuffer.insert(_vertexBuffer.end(), {s_unitCirclePoints[a].x*p_radius + p_x, s_unitCirclePoints[a].y*p_radius + p_y, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f});
		}
		addDrawCall(CIRCLE_RESOLUTION + 2, 0u, (GLenum)GL_TRIANGLE_FAN);
	}

	//------------------------------

	void draw_line(Avo::Point<> p_point_0, Avo::Point<> p_point_1, float p_thickness) override
	{
		draw_line(p_point_0.x, p_point_0.y, p_point_1.x, p_point_1.y);
	}
	void draw_line(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) override
	{
		float distance = Avo::fastSqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		float normalX = (p_y1 - p_y0)/distance;
		float normalY = -(p_x1 - p_x0)/distance;

		// Anti-aliasing requires extra pixels
		p_x0 += 0.5f*normalY;
		p_y0 -= 0.5f*normalX;
		p_x1 -= 0.5f*normalY;
		p_y1 += 0.5f*normalX;
		distance += 1.f;

		p_thickness += 1.f;
		normalX *= p_thickness*0.5f;
		normalY *= p_thickness*0.5f;

		_vertexBuffer.insert(
			_vertexBuffer.end(),
			{
				p_x0 + normalX, p_y0 + normalY, 1.f, p_thickness, 0.f, distance, 0.f, 0.f,
				p_x0 - normalX, p_y0 - normalY, 0.f, p_thickness, 0.f, distance, 0.f, 0.f,
				p_x1 - normalX, p_y1 - normalY, 0.f, p_thickness, 1.f, distance, 0.f, 0.f,

				p_x1 - normalX, p_y1 - normalY, 1.f, p_thickness, 0.f, distance, 0.f, 0.f,
				p_x0 + normalX, p_y0 + normalY, 0.f, p_thickness, 1.f, distance, 0.f, 0.f,
				p_x1 + normalX, p_y1 + normalY, 0.f, p_thickness, 0.f, distance, 0.f, 0.f
			}
		);
		addDrawCall(6u);
	}

	//------------------------------

	void stroke_shape(std::vector<Avo::Point<>> const& p_vertices, float p_lineThickness, bool p_isClosed) override
	{
	}
	void stroke_shape(Avo::Point<> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed) override
	{
	}
	void fill_shape(std::vector<Avo::Point<>> const& p_vertices) override
	{
	}
	void fill_shape(Avo::Point<> const* p_vertices, uint32 p_numberOfVertices) override
	{
	}

	//------------------------------

	void stroke_geometry(Avo::Geometry const& p_geometry, float p_strokeWidth) override
	{
		auto geometry = (OpenGlGeometry*)getGeometryImplementation(p_geometry);
		_vertexBuffer.insert(_vertexBuffer.end(), geometry->vertexBuffer.begin(), geometry->vertexBuffer.end());
		addDrawCall(geometry->vertexBuffer.size(), p_strokeWidth);
	}
	void fill_geometry(Avo::Geometry const& p_geometry) override
	{
		auto geometry = (OpenGlGeometry*)getGeometryImplementation(p_geometry);
		_vertexBuffer.insert(_vertexBuffer.end(), geometry->vertexBuffer.begin(), geometry->vertexBuffer.end());
		addDrawCall(geometry->vertexBuffer.size());
	}

	//------------------------------

	Avo::Geometry create_rounded_rectangle_geometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius, bool p_isStroked) override
	{
		create_corner_rectangle_geometry(p_left, p_top, p_right, p_bottom, Avo::RectangleCorners(p_radius), p_isStroked);
	}
	Avo::Geometry create_rounded_rectangle_geometry(Avo::Point<> p_position, Avo::Point<> size, float p_radius, bool p_isStroked) override
	{
		create_corner_rectangle_geometry(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, Avo::RectangleCorners{p_radius}, p_isStroked);
	}
	Avo::Geometry create_rounded_rectangle_geometry(Avo::Rectangle<> p_rectangle, float p_radius, bool p_isStroked) override
	{
		create_corner_rectangle_geometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, Avo::RectangleCorners{p_radius}, p_isStroked);
	}
	Avo::Geometry create_rounded_rectangle_geometry(float p_width, float p_height, float p_radius, bool p_isStroked) override
	{
		create_corner_rectangle_geometry(0.f, 0.f, p_width, p_height, Avo::RectangleCorners{p_radius}, p_isStroked);
	}
	Avo::Geometry create_rounded_rectangle_geometry(Avo::Point<> size, float p_radius, bool p_isStroked) override
	{
		create_corner_rectangle_geometry(0.f, 0.f, size.x, size.y, Avo::RectangleCorners{p_radius}, p_isStroked);
	}

	Avo::Geometry create_corner_rectangle_geometry(float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		auto geometry = new OpenGlGeometry;

		if (p_isStroked)
		{
			createStrokedCornerRectangleGeometry(geometry->vertexBuffer, p_left, p_top, p_right, p_bottom, p_corners);
		}
		else
		{
			createFilledCornerRectangleGeometry(geometry->vertexBuffer, p_left, p_top, p_right, p_bottom, p_corners);
		}
		geometry->isStroked = p_isStroked;

		return createGeometryFromImplementation(geometry);
	}
	Avo::Geometry create_corner_rectangle_geometry(Avo::Point<> p_position, Avo::Point<> size, Avo::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return create_corner_rectangle_geometry(p_position.x, p_position.y, p_position.x + size.x, p_position.y + size.y, p_corners, p_isStroked);
	}
	Avo::Geometry create_corner_rectangle_geometry(Avo::Rectangle<> p_rectangle, Avo::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return create_corner_rectangle_geometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_isStroked);
	}
	Avo::Geometry create_corner_rectangle_geometry(float p_width, float p_height, Avo::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return create_corner_rectangle_geometry(0.f, 0.f, p_width, p_height, p_corners, p_isStroked);
	}
	Avo::Geometry create_corner_rectangle_geometry(Avo::Point<> size, Avo::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return create_corner_rectangle_geometry(0.f, 0.f, size.x, size.y, p_corners, p_isStroked);
	}

	//------------------------------

	Avo::Geometry create_polygon_geometry(Avo::Point<> const* p_vertices, uint32 p_numberOfVertices, bool p_isStroked, bool p_isClosed) override
	{
		return createGeometryFromImplementation(new OpenGlGeometry);
	}

	//------------------------------

	void set_line_cap(Avo::LineCap p_lineCap) override
	{
	}
	void set_start_line_cap(Avo::LineCap p_lineCap) override
	{
	}
	void setEndLineCap(Avo::LineCap p_lineCap) override
	{
	}
	Avo::LineCap getStartLineCap() override
	{
	}
	Avo::LineCap getEndLineCap() override
	{
	}

	//------------------------------

	void setLineDashStyle(Avo::LineDashStyle p_dashStyle) override
	{
	}
	Avo::LineDashStyle getLineDashStyle() override
	{
	}

	void setLineDashOffset(float p_dashOffset) override
	{
	}
	float getLineDashOffset() override
	{
	}

	void setLineDashCap(Avo::LineCap p_dashCap) override
	{
	}
	Avo::LineCap getLineDashCap() override
	{
	}

	//------------------------------

	void setLineJoin(Avo::LineJoin p_lineJoin) override
	{
	}
	Avo::LineJoin getLineJoin() override
	{
	}

	void setLineJoinMiterLimit(float p_miterLimit) override
	{
	}
	float getLineJoinMiterLimit() override
	{
	}

	//------------------------------

	void pushClipGeometry(Avo::Geometry const& p_geometry, float p_opacity) override
	{
	}

	//------------------------------

	void pushClipShape(std::vector<Avo::Point<>> const& p_points, float p_opacity) override
	{
	}
	void pushClipShape(Avo::Point<> const* p_points, uint32 p_numberOfPoints, float p_opacity) override
	{
	}

	void popClipShape() override
	{
		if (!_clippingShapeStack.empty())
		{
			_clippingShapeStack.pop();
			if (!_clippingShapeStack.empty())
			{
				Avo::Rectangle<> bounds = _clippingShapeStack.top().get_bounds();
				glScissor(bounds.left, bounds.top, std::ceil(bounds.get_width()), std::ceil(bounds.get_height()));
			}
			else
			{
				glScissor(0, 0, _size.x*_dip_to_pixel_factor, _size.y*_dip_to_pixel_factor);
			}
		}
	}

	//------------------------------

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_opacity) override
	{
		if (!_clippingShapeStack.empty())
		{
			Avo::Rectangle<> clippingBounds = _clippingShapeStack.top().get_bounds().bound(p_left, p_top, p_right, p_bottom);
			_clippingShapeStack.push(ClippingShape(clippingBounds));
			glScissor(clippingBounds.left, clippingBounds.bottom, std::ceil(clippingBounds.get_width()), std::ceil(clippingBounds.get_height()));
		}
		else
		{
			_clippingShapeStack.push(ClippingShape(p_left, p_bottom, p_right, p_bottom));
			glScissor(p_left, p_bottom, std::ceil(p_right - p_left), std::ceil(p_bottom - p_top));
		}
	}
	void pushClipRectangle(Avo::Rectangle<> p_rectangle, float p_opacity) override
	{
		pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_opacity);
	}
	void pushClipRectangle(Avo::Point<> size, float p_opacity) override
	{
		pushClipRectangle(0, 0, size.x, size.y, p_opacity);
	}

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, Avo::RectangleCorners const& p_corners, float p_opacity) override
	{
	}
	void pushClipRectangle(Avo::Rectangle<> p_rectangle, Avo::RectangleCorners const& p_corners, float p_opacity) override
	{
	}
	void pushClipRectangle(Avo::Point<> size, Avo::RectangleCorners const& p_corners, float p_opacity) override
	{
	}

	//------------------------------

	void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_opacity) override
	{
	}
	void pushRoundedClipRectangle(Avo::Rectangle<> p_rectangle, float p_radius, float p_opacity) override
	{
	}
	void pushRoundedClipRectangle(Avo::Point<> size, float p_radius, float p_opacity) override
	{
	}

	//------------------------------

	Avo::Image createRectangleShadowImage(Avo::Point<> size, float p_blur, Avo::Color p_color) override
	{
	}
	Avo::Image createRectangleShadowImage(float p_width, float p_height, float p_blur, Avo::Color p_color) override
	{
	}

	Avo::Image createRectangleShadowImage(Avo::Point<> size, Avo::RectangleCorners const& p_corners, float p_blur, Avo::Color p_color) override
	{
	}
	Avo::Image createRectangleShadowImage(float p_width, float p_height, Avo::RectangleCorners const& p_corners, float p_blur, Avo::Color p_color) override
	{
	}

	//------------------------------

	Avo::Image createRoundedRectangleShadowImage(Avo::Point<> size, float p_radius, float p_blur, Avo::Color p_color) override
	{
	}
	Avo::Image createRoundedRectangleShadowImage(float p_width, float p_height, float p_radius, float p_blur, Avo::Color p_color) override
	{
	}

	//------------------------------

	Avo::Image create_image(uint8 const* p_pixelData, uint32 p_width, uint32 p_height) override
	{
		// Make context usable from this thread (thread might be different)
		glXMakeCurrent(_server, _window_handle, _context); 

		GLuint texture = 0;
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, p_pixelData);
		glGenerateMipmap(GL_TEXTURE_2D);

		return createImageFromImplementation(new OpenGlImage{texture, p_width, p_height, _dip_to_pixel_factor});
	}
	Avo::Image create_image(uint8 const* p_imageData, uint32 size) override
	{
		png_image pngImage{};
		pngImage.version = PNG_IMAGE_VERSION;
		pngImage.format = PNG_FORMAT_BGRA;

		if (!png_image_begin_read_from_memory(&pngImage, p_imageData, size))
		{
			return Avo::Image{};
		}

		std::vector<png_byte> data(PNG_IMAGE_SIZE(pngImage));
		if (!png_image_finish_read(&pngImage, nullptr, data.data(), 0, nullptr))
		{
			return Avo::Image{};
		}

		return create_image(data.data(), pngImage.width, pngImage.height);
	}
	Avo::Image create_image(std::string_view p_filePath) override
	{
		if (!filesystem::is_regular_file(p_filePath))
		{
			return Avo::Image{};
		}
		switch (getImageFormatOfFile(p_filePath))
		{
			case Avo::ImageFormat::Png:
			{
				png_image pngImage{};
				pngImage.version = PNG_IMAGE_VERSION;

				if (!png_image_begin_read_from_file(&pngImage, p_filePath.data()))
				{
					return Avo::Image{};
				}

				pngImage.format = PNG_FORMAT_BGRA;

				std::vector<png_byte> data(PNG_IMAGE_SIZE(pngImage));
				if (!png_image_finish_read(&pngImage, nullptr, data.data(), 0, nullptr))
				{
					return Avo::Image{};
				}

				return create_image(data.data(), pngImage.width, pngImage.height);
			}
			case Avo::ImageFormat::Jpeg:
			{
				FILE* file{fopen(p_filePath.data(), "rb")};
				if (!file)
				{
					return Avo::Image{};
				}

				jpeg_error_mgr errorHandler;
				errorHandler.error_exit = [] (j_common_ptr p_info) -> void {
					p_info->err->output_message(p_info);
				};

				jpeg_decompress_struct decompressor{};
				decompressor.err = jpeg_std_error(&errorHandler);

				jpeg_create_decompress(&decompressor);
				jpeg_stdio_src(&decompressor, file);
				jpeg_read_header(&decompressor, true);

				decompressor.out_color_space = JCS_EXT_BGRA;
				jpeg_start_decompress(&decompressor);

				uint32 rowStride = decompressor.output_width*decompressor.output_components;
				JSAMPARRAY rowBuffer = (*decompressor.mem->alloc_sarray)((j_common_ptr)&decompressor, JPOOL_IMAGE, rowStride, 1);

				std::vector<png_byte> data(rowStride*decompressor.output_height);

				while (decompressor.output_scanline < decompressor.output_height)
				{
					jpeg_read_scanlines(&decompressor, rowBuffer, 1);
					std::memcpy(data.data() + (decompressor.output_scanline - 1)*rowStride, rowBuffer[0], rowStride);
				}

				auto image = create_image(data.data(), decompressor.output_width, decompressor.output_height);

				jpeg_finish_decompress(&decompressor);
				jpeg_destroy_decompress(&decompressor);
				fclose(file);

				return image;
			}
		}
		return Avo::Image{};
	}
	Avo::Image createImageFromHandle(void* p_handle) override
	{
		return Avo::Image{};
	}
	void drawImage(Avo::Image const& p_image, float p_multiplicativeOpacity) override
	{
		Avo::Rectangle<> innerBounds = p_image.get_inner_bounds();
		float width = innerBounds.get_width();
		float height = innerBounds.get_height();

		Avo::Rectangle<> cropRectangle = p_image.get_crop_rectangle();
		Avo::Point<> originalSize = p_image.get_original_size();
		cropRectangle.left /= originalSize.x;
		cropRectangle.top /= originalSize.y;
		cropRectangle.right /= originalSize.x;
		cropRectangle.bottom /= originalSize.y;

		_vertexBuffer.insert(
			_vertexBuffer.end(),
			{
				innerBounds.left , innerBounds.top   , 1.f, width, 0.f, height, cropRectangle.left , cropRectangle.top,
				innerBounds.right, innerBounds.top   , 0.f, width, 0.f, height, cropRectangle.right, cropRectangle.top,
				innerBounds.right, innerBounds.bottom, 0.f, width, 1.f, height, cropRectangle.right, cropRectangle.bottom,

				innerBounds.left , innerBounds.top   , 0.f, width, 1.f, height, cropRectangle.left , cropRectangle.top,
				innerBounds.left , innerBounds.bottom, 0.f, width, 0.f, height, cropRectangle.left , cropRectangle.bottom,
				innerBounds.right, innerBounds.bottom, 1.f, width, 0.f, height, cropRectangle.right, cropRectangle.bottom,
			}
		);
		addDrawCall(6, *(GLuint*)p_image.getHandle());
	}

	//------------------------------

	std::vector<uint8> createImageFileData(Avo::Image const& p_image, Avo::ImageFormat format) override
	{
	}
	void* createImageFileDataNativeStream(Avo::Image const& p_image, Avo::ImageFormat format) override
	{
	}
	void saveImageToFile(Avo::Image const& p_image, std::string_view p_filePath, Avo::ImageFormat format) override
	{
	}

	//------------------------------

	void* createNativeImageFromImage(Avo::Image const& p_image) override
	{
	}

	//------------------------------

	Avo::LinearGradient createLinearGradient(std::vector<Avo::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_endX, float p_endY) override
	{
	}
	Avo::LinearGradient createLinearGradient(std::vector<Avo::GradientStop> const& p_gradientStops, Avo::Point<> p_startPosition, Avo::Point<> p_endPosition) override
	{
	}

	Avo::RadialGradient createRadialGradient(std::vector<Avo::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radiusX, float p_radiusY) override
	{
	}
	Avo::RadialGradient createRadialGradient(std::vector<Avo::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) override
	{
	}
	Avo::RadialGradient createRadialGradient(std::vector<Avo::GradientStop> const& p_gradientStops, Avo::Point<> p_startPosition, float p_radiusX, float p_radiusY) override
	{
	}
	Avo::RadialGradient createRadialGradient(std::vector<Avo::GradientStop> const& p_gradientStops, Avo::Point<> p_startPosition, float p_radius) override
	{
	}
	Avo::RadialGradient createRadialGradient(std::vector<Avo::GradientStop> const& p_gradientStops, Avo::Point<> p_startPosition, Avo::Point<> p_radius) override
	{
	}

	void setGradient(Avo::LinearGradient const& p_gradient) override
	{
	}
	void setGradient(Avo::RadialGradient const& p_gradient) override
	{
	}
	void set_color(Avo::Color p_color) override
	{
		_current_color = p_color;
	}

	void set_opacity(float p_opacity) override
	{
	}

	//------------------------------

	void addFont(std::string_view data) override
	{
	}
	void addFont(std::vector<uint8> const& data) override
	{
	}
	void addFont(std::vector<uint8>&& data) override
	{
	}
	void addFont(uint8 const* data, uint32 p_dataSize) override
	{
	}

	//------------------------------

	void setDefaultTextProperties(Avo::TextProperties const& p_textProperties) override
	{
	}
	Avo::TextProperties getDefaultTextProperties() override
	{
	}

	//------------------------------

	Avo::Text create_text(std::string_view p_string, float p_fontSize, Avo::Rectangle<> p_bounds) override
	{
	}
	void draw_text(Avo::Text const& p_text) override
	{
	}

	void draw_text(std::string_view p_string, Avo::Rectangle<> p_rectangle) override
	{
	}
	void draw_text(std::string_view p_string, float p_left, float p_top, float p_right, float p_bottom) override
	{
	}
	void draw_text(std::string_view p_string, Avo::Point<> p_position, Avo::Point<> size) override
	{
	}
	void draw_text(std::string_view p_string, float p_x, float p_y) override
	{
	}
	void draw_text(std::string_view p_string, Avo::Point<> p_position) override
	{
	}
};

bool OpenGlDrawingContext::s_areStaticVariablesInitialized = false;
Avo::Point<> OpenGlDrawingContext::s_unitCirclePoints[OpenGlDrawingContext::CIRCLE_RESOLUTION + 1];

#endif

//------------------------------
// class GUI
//------------------------------

auto Avo::Gui::get_top_mouse_listeners_at(Avo::Point<> const p_coordinates) -> std::vector<Avo::View*>
{
	std::vector<Avo::View*> result;
	if (get_are_mouse_events_enabled())
	{
		result = {this};
	}
	else
	{
		result = {};
	}
	result.reserve(10);

	Avo::View* container = this;
	Index startIndex = get_number_of_views() - 1;

	bool hasFoundTopView = false;

	while (true)
	{
	loopStart:
		for (Index a = startIndex; a >= 0; a--)
		{
			auto child = container->get_view(a);
			// Invisible views and their children do not receive mouse events.
			if (child->get_is_visible() && child->get_is_containing_absolute(p_coordinates))
			{
				bool hasChildren = child->get_number_of_views();

				if (child->get_are_mouse_events_enabled())
				{
					result.push_back(child);
				}
				if (hasChildren)
				{
					container = child;
					startIndex = container->get_number_of_views() - 1;
					goto loopStart; // I have determined this is the least messy way to do it pls don't kill me
				}
				else
				{
					// We only continue of it's an overlay view, meaning mouse events can pass through it.
					if (!child->get_is_overlay())
					{
						// This is only used to determine if the outer loop should be exited afterwards.
						hasFoundTopView = true;
						break;
					}
				}
			}
		}

		// If an overlay view has children that are not overlay views and one of those are targeted by 
		// the mouse event, mouse events won't pass through!
		if (!container->get_is_overlay() || hasFoundTopView || container == this)
		{
			break;
		}

		startIndex = container->get_index() - 1;
		container = container->get_parent<View>();
	}
	return result;
}

auto Avo::Gui::thread_run_animation_loop() -> void
{	
	using namespace std::chrono;
	using namespace std::chrono_literals;

	using _Duration = duration<double, std::nano>;
	auto syncInterval = _Duration{1.s/60.};

	auto timeBefore = steady_clock::now();

	while (!_window->get_will_close())
	{
		{
			auto const lock = create_thread_lock();
			for (auto const a : Indices{_viewAnimationUpdateQueue})
			{
				_viewAnimationUpdateQueue.front()->_isInAnimationUpdateQueue = false;
				_viewAnimationUpdateQueue.front()->update_animations();
				_viewAnimationUpdateQueue.pop_front();
			}
		}

		{
			auto const lock = create_thread_lock();
			for (auto const a : Indices{_animationUpdateQueue})
			{
				_animationUpdateQueue.front()->update();
				_animationUpdateQueue.pop_front();
			}
		}

		if (_isInvalid)
		{
			drawViews();

			if (!get_drawing_context()->getIsVsyncEnabled())
			{
				std::this_thread::sleep_for(syncInterval);
			}
		}
		else
		{
			std::this_thread::sleep_for(syncInterval);
		}
		auto const timeAfter = steady_clock::now();
		syncInterval = std::max(_Duration{1.ms}, syncInterval + (1.s/60. - (timeAfter - timeBefore))/2.);
		//syncInterval = std::max(1000000, int32(syncInterval + 0.5*(16666667 - (timeAfter - timeBefore).count())));
		timeBefore = timeAfter;
	}

	{
		auto const lock = create_thread_lock();
		while (!_viewAnimationUpdateQueue.empty())
		{
			_viewAnimationUpdateQueue.front()->_isInAnimationUpdateQueue = false;
			_viewAnimationUpdateQueue.pop_front();
		}
	}

	// This will cause the window to be destroyed, because _window->get_will_close() is true.
	_window->close();
}

Count Avo::Gui::s_number_of_instances = 0u;

Avo::Gui::Gui() :
	Avo::View{nullptr}
{
	++s_number_of_instances;

	_gui = this;
}
Avo::Gui::Gui(Component* const p_parent) :
	Gui{}
{
	Component::set_parent(p_parent);
}
Avo::Gui::~Gui()
{
	--s_number_of_instances;
}

auto Avo::Gui::create(
	std::string_view const p_title, 
	Avo::Point<Avo::Factor> const p_position, 
	Avo::Size<> const size, 
	WindowStyleFlags const p_windowFlags, 
	Gui* const p_parent
) -> void
{
	if (p_parent)
	{
		_parent = p_parent;
	}

#ifdef _WIN32
	Direct2dDrawingContext::createStaticResources();
	_window = std::make_unique<WindowsWindow>(this);
#endif
#ifdef __linux__
	_window = std::make_unique<LinuxWindow>(this);
#endif

	_window->window_create_listeners += Avo::bind(&Gui::handle_window_create, this);
	_window->window_size_change_listeners += Avo::bind(&Gui::handle_window_size_change, this);

	_absolutePosition = 0;
	_window->create(p_title, p_position, size, p_windowFlags, p_parent ? p_parent->get_window() : nullptr);
}

//------------------------------

auto Avo::Gui::get_view_at(Avo::Point<> const p_coordinates) -> Avo::View*
{
	auto currentContainer = static_cast<Avo::View*>(this);
	while (true)
	{
		auto const hitViewIterator = std::find_if(
			currentContainer->rbegin(), currentContainer->rend(), 
			[&](Avo::View* const view){
				return view->get_is_visible() && !view->get_is_overlay() && view->get_is_containing_absolute(p_coordinates);
			}
		);
		if (hitViewIterator == currentContainer->rend())
		{
			return currentContainer;
		}
		else
		{
			auto* const hitView = *hitViewIterator;
			if (hitView->get_has_views())
			{
				currentContainer = hitView;
			}
			else
			{
				return hitView;
			}
		}
	}
}

//------------------------------

auto Avo::Gui::handle_window_create(WindowEvent const& p_event) -> void
{
#ifdef _WIN32
	_drawingContext = std::make_unique<Direct2dDrawingContext>(_window.get());
#elif __linux__
	_drawingContext = std::make_unique<OpenGlDrawingContext>(_window.get());
#endif
	_drawingContextState = _drawingContext->createDrawingState();

	_bounds = p_event.size;
	_shadow_bounds = _bounds;
	_lastUpdatedWindowSize = p_event.size;
}
auto Avo::Gui::handle_window_size_change(WindowEvent const& p_event) -> void
{
	auto const lock = create_thread_lock();
	_drawingContext->set_size(p_event.size);

	_bounds = p_event.size;
	_shadow_bounds = _bounds;

	send_bounds_change_events(_lastUpdatedWindowSize);

	_lastUpdatedWindowSize = p_event.size;

	invalidate();
}

//------------------------------

auto Avo::Gui::handle_global_drag_drop_move(DragDropEvent& p_event) -> void
{
	auto container = static_cast<Avo::View*>(this);
	auto startIndex = static_cast<Avo::Index>(_child_views.size()) - 1;

	auto* const thisView = static_cast<Avo::View*>(this);

	auto wasHoveringStack = std::stack<bool>{};
	wasHoveringStack.push(thisView->_isDraggingOver);

	if (get_is_containing(p_event.xy))
	{
		if (_areDragDropEventsEnabled)
		{
			if (thisView->_isDraggingOver)
			{
				dragDropMoveListeners(p_event);
			}
			else
			{
				dragDropEnterListeners(p_event);

				if (startIndex < 0)
				{
					dragDropBackgroundEnterListeners(p_event);
				}
			}
		}
		thisView->_isDraggingOver = true;
	}
	else if (thisView->_isDraggingOver)
	{
		if (_areDragDropEventsEnabled)
		{
			dragDropLeaveListeners(p_event);

			if (startIndex < 0)
			{
				dragDropBackgroundLeaveListeners(p_event);
			}
		}
		thisView->_isDraggingOver = false;
	}

	auto absolute = p_event.xy;
	if (startIndex >= 0)
	{
		auto hasInvisibleParent = bool{};
		auto hasOverlayParent = bool{};
		auto hasFoundEnterViews = bool{};
		auto hasFoundLeaveViews = bool{};
		while (true)
		{
		loopStart:
			for (auto a = startIndex; a >= 0; a--)
			{
				auto* const child = container->_child_views[a];

				if (container->_isDraggingOver && child->get_is_containing_absolute(absolute) && 
				    child->get_is_visible() && !hasInvisibleParent && !hasFoundEnterViews)
				{
					if (child->_areDragDropEventsEnabled)
					{
						p_event.xy = absolute - child->get_absolute_top_left();
					}

					auto isContainer = !child->_child_views.empty();

					if (child->_isDraggingOver)
					{
						if (child->_areDragDropEventsEnabled)
						{
							child->dragDropMoveListeners(p_event);
						}
					}
					else
					{
						if (child->_areDragDropEventsEnabled)
						{
							child->dragDropEnterListeners(p_event);
							if (!isContainer)
							{
								child->dragDropBackgroundEnterListeners(p_event);
							}
						}
					}

					if (isContainer)
					{
						wasHoveringStack.push(child->_isDraggingOver);
						child->_isDraggingOver = true;
						if (child->get_is_overlay())
						{
							hasOverlayParent = true;
						}
						container = child;
						startIndex = child->get_number_of_views() - 1;
						goto loopStart;
					}
					else
					{
						if (!hasOverlayParent && !child->get_is_overlay())
						{
							hasFoundEnterViews = true;
							if (child->_isDraggingOver)
							{
								hasFoundLeaveViews = true;
								break;
							}
							else if (hasFoundLeaveViews)
							{
								child->_isDraggingOver = true;
								break;
							}
						}
						child->_isDraggingOver = true;
					}
				}
				else if (child->_isDraggingOver && !hasFoundLeaveViews)
				{
					auto isContainer = static_cast<bool>(child->get_number_of_views());

					if (child->_areDragDropEventsEnabled)
					{
						p_event.xy = absolute - child->get_absolute_top_left();
						child->dragDropLeaveListeners(p_event);
						if (!isContainer)
						{
							child->dragDropBackgroundLeaveListeners(p_event);
						}
					}

					if (isContainer)
					{
						wasHoveringStack.push(child->_isDraggingOver);
						child->_isDraggingOver = false;

						if (child->_is_overlay)
						{
							hasOverlayParent = true;
						}
						if (!child->_is_visible)
						{
							hasInvisibleParent = true;
						}
						container = child;
						startIndex = child->get_number_of_views() - 1;
						goto loopStart;
					}
					else
					{
						child->_isDraggingOver = false;
						if (!hasOverlayParent && !child->_is_overlay)
						{
							hasFoundLeaveViews = true;
							if (hasFoundEnterViews)
							{
								break;
							}
						}
					}
				}
			}

			if (wasHoveringStack.top() && container->_isDraggingOver && hasFoundLeaveViews && !hasFoundEnterViews ||
				!wasHoveringStack.top() && container->_isDraggingOver && !hasFoundEnterViews)
			{
				hasFoundEnterViews = true;
				if (container->_areDragDropEventsEnabled)
				{
					p_event.xy = absolute - container->get_absolute_top_left();
					container->dragDropBackgroundEnterListeners(p_event);
				}
			}
			else if (wasHoveringStack.top() && container->_isDraggingOver && hasFoundEnterViews && !hasFoundLeaveViews ||
			         wasHoveringStack.top() && !container->_isDraggingOver && !hasFoundLeaveViews)
			{
				hasFoundLeaveViews = true;
				if (container->_areDragDropEventsEnabled)
				{
					p_event.xy = absolute - container->get_absolute_top_left();
					container->dragDropBackgroundLeaveListeners(p_event);
				}
			}
			else if (wasHoveringStack.top() && container->_isDraggingOver)
			{
				hasFoundEnterViews = true;
				hasFoundLeaveViews = true;
			}

			if (container == this)
			{
				break;
			}

			if (container->get_is_overlay())
			{
				wasHoveringStack.pop();
				hasOverlayParent = false;
				startIndex = container->get_index() - 1;
				container = container->get_parent<View>();
			}
			else
			{
				while (container != this && wasHoveringStack.top() != container->_isDraggingOver)
				{
					wasHoveringStack.pop();
					startIndex = container->get_index() - 1;
					container = container->get_parent<View>();
					if (container->get_is_overlay())
					{
						hasOverlayParent = false;
					}
					if (!container->get_is_visible())
					{
						hasInvisibleParent = false;
					}
				}

				if (hasFoundLeaveViews && hasFoundEnterViews)
				{
					break;
				}
			}
		}
	}
	p_event.xy = absolute;
}
auto Avo::Gui::handle_global_drag_drop_leave(DragDropEvent& p_event) -> void
{
	if (_isDraggingOver)
	{
		if (_areDragDropEventsEnabled)
		{
			dragDropLeaveListeners(p_event);
			dragDropBackgroundLeaveListeners(p_event);
		}
		_isDraggingOver = false;
	}

	auto const absolute = p_event.xy;

	Avo::View* container = this;
	Index startIndex = _child_views.size() - 1;
	Count numberOfOverlayParents = 0;
	while (true)
	{
	loopStart:
		for (Index a = startIndex; a >= 0; a--)
		{
			Avo::View* child = container->_child_views[a];

			if (child->_isDraggingOver)
			{
				if (child->_areDragDropEventsEnabled)
				{
					p_event.xy = absolute - child->get_absolute_top_left();
					child->dragDropLeaveListeners(p_event);
					child->dragDropBackgroundLeaveListeners(p_event);
				}
				child->_isDraggingOver = false;

				if (!child->_child_views.empty())
				{
					startIndex = child->_child_views.size() - 1;
					container = child;
					if (container->_is_overlay)
					{
						numberOfOverlayParents++;
					}
					goto loopStart;
				}
				else if (!child->_is_overlay)
				{
					break;
				}
			}
		}
		if (container == this)
		{
			break;
		}
		else if (container->_is_overlay)
		{
			startIndex = container->_index - 1;
			container = container->_parent;
			numberOfOverlayParents--;
		}
		else if (numberOfOverlayParents)
		{
			while (!container->_is_overlay && container != this)
			{
				container = container->_parent;
			}
			if (container == this)
			{
				break;
			}
			startIndex = container->_index - 1;
			container = container->_parent;
			numberOfOverlayParents--;
		}
		else
		{
			break;
		}
	}
}

//------------------------------

auto Avo::Gui::handle_global_mouse_move(MouseEvent& p_event) -> void
{
	// This is false if it's called from a view just to send mouse leave and mouse enter events,
	// if a view has been moved from the mouse for example.
	auto const wasMouseReallyMoved = bool{p_event.movement};

	auto const absolute = p_event.xy;

	if (!_pressed_mouse_event_listeners.empty())
	{
		if (wasMouseReallyMoved)
		{
			for (auto* const pressedView : _pressed_mouse_event_listeners)
			{
				p_event.xy = absolute - pressedView->get_absolute_top_left();
				pressedView->mouse_move_listeners(p_event);
			}
		}
	}
	else
	{
		Avo::View* container = this;
		Index startIndex = _child_views.size() - 1;

		auto* const thisView = static_cast<View*>(this);

		std::stack<bool> wasHoveringStack;
		wasHoveringStack.push(thisView->_is_mouse_hovering);

		if (get_is_containing(p_event.xy))
		{
			if (_are_mouse_events_enabled)
			{
				if (thisView->_is_mouse_hovering)
				{
					if (wasMouseReallyMoved)
					{
						mouse_move_listeners(p_event);
					}
				}
				else
				{
					mouse_enter_listeners(p_event);

					if (startIndex < 0)
					{
						mouse_background_enter_listeners(p_event);
					}
				}
			}
			thisView->_is_mouse_hovering = true;
		}
		else if (thisView->_is_mouse_hovering)
		{
			if (get_are_mouse_events_enabled())
			{
				mouse_leave_listeners(p_event);

				if (startIndex < 0)
				{
					mouse_background_leave_listeners(p_event);
				}
			}
			thisView->_is_mouse_hovering = false;
		}

		if (startIndex >= 0)
		{
			bool hasInvisibleParent = false;
			bool hasOverlayParent = false;
			bool hasFoundEnterViews = false;
			bool hasFoundLeaveViews = false;
			while (true)
			{
			loopStart:
				for (Index a = startIndex; a >= 0; a--)
				{
					auto child = container->_child_views[a];

					if (container->_is_mouse_hovering && child->get_is_containing_absolute(absolute) && 
					    child->_is_visible && !hasInvisibleParent && !hasFoundEnterViews)
					{
						if (child->_are_mouse_events_enabled)
						{
							p_event.xy = absolute - child->get_absolute_top_left();
						}

						bool isContainer = !child->_child_views.empty();

						if (child->_is_mouse_hovering)
						{
							if (child->_are_mouse_events_enabled && wasMouseReallyMoved)
							{
								child->mouse_move_listeners(p_event);
							}
						}
						else
						{
							if (child->_are_mouse_events_enabled)
							{
								child->mouse_enter_listeners(p_event);
								if (!isContainer)
								{
									child->mouse_background_enter_listeners(p_event);
								}
							}
						}

						if (isContainer)
						{
							wasHoveringStack.push(child->_is_mouse_hovering);
							child->_is_mouse_hovering = true;
							if (child->_is_overlay)
							{
								hasOverlayParent = true;
							}
							container = child;
							startIndex = child->get_number_of_views() - 1;
							goto loopStart;
						}
						else
						{
							if (!hasOverlayParent && !child->_is_overlay)
							{
								hasFoundEnterViews = true;
								if (child->_is_mouse_hovering)
								{
									hasFoundLeaveViews = true;
									break;
								}
								else if (hasFoundLeaveViews)
								{
									child->_is_mouse_hovering = true;
									break;
								}
							}
							child->_is_mouse_hovering = true;
						}
					}
					else if (child->_is_mouse_hovering && !hasFoundLeaveViews)
					{
						bool isContainer = child->get_number_of_views();

						if (child->_are_mouse_events_enabled)
						{
							p_event.xy = absolute - child->get_absolute_top_left();
							child->mouse_leave_listeners(p_event);
							if (!isContainer)
							{
								child->mouse_background_leave_listeners(p_event);
							}
						}

						if (isContainer)
						{
							wasHoveringStack.push(child->_is_mouse_hovering);
							child->_is_mouse_hovering = false;

							if (child->_is_overlay)
							{
								hasOverlayParent = true;
							}
							if (!child->_is_visible)
							{
								hasInvisibleParent = true;
							}
							container = child;
							startIndex = child->get_number_of_views() - 1;
							goto loopStart;
						}
						else
						{
							child->_is_mouse_hovering = false;
							if (!hasOverlayParent && !child->_is_overlay)
							{
								hasFoundLeaveViews = true;
								if (hasFoundEnterViews)
								{
									break;
								}
							}
						}
					}
				}

				if (wasHoveringStack.top() && container->_is_mouse_hovering && hasFoundLeaveViews && !hasFoundEnterViews ||
					!wasHoveringStack.top() && container->_is_mouse_hovering && !hasFoundEnterViews)
				{
					hasFoundEnterViews = true;
					if (container->_are_mouse_events_enabled)
					{
						p_event.xy = absolute - container->get_absolute_top_left();
						container->mouse_background_enter_listeners(p_event);
					}
				}
				else if (wasHoveringStack.top() && container->_is_mouse_hovering && hasFoundEnterViews && !hasFoundLeaveViews ||
					wasHoveringStack.top() && !container->_is_mouse_hovering && !hasFoundLeaveViews)
				{
					hasFoundLeaveViews = true;
					if (container->_are_mouse_events_enabled)
					{
						p_event.xy = absolute - container->get_absolute_top_left();
						container->mouse_background_leave_listeners(p_event);
					}
				}
				else if (wasHoveringStack.top() && container->_is_mouse_hovering)
				{
					hasFoundEnterViews = true;
					hasFoundLeaveViews = true;
				}

				if (container == this)
				{
					break;
				}

				if (container->_is_overlay)
				{
					wasHoveringStack.pop();
					hasOverlayParent = false;
					startIndex = container->get_index() - 1;
					container = container->get_parent<View>();
				}
				else
				{
					while (container != this && wasHoveringStack.top() != container->_is_mouse_hovering)
					{
						wasHoveringStack.pop();
						startIndex = container->get_index() - 1;
						container = container->get_parent<View>();
						if (container->_is_overlay)
						{
							hasOverlayParent = false;
						}
						if (!container->_is_visible)
						{
							hasInvisibleParent = false;
						}
					}

					if (hasFoundLeaveViews && hasFoundEnterViews)
					{
						break;
					}
				}
			}
		}
	}
}
auto Avo::Gui::handleGlobalMouseLeave(MouseEvent& p_event) -> void
{
	if (!_pressed_mouse_event_listeners.empty())
	{
		return;
	}

	if (_is_mouse_hovering)
	{
		if (_are_mouse_events_enabled)
		{
			mouse_leave_listeners(p_event);
			mouse_background_leave_listeners(p_event);
		}
		_is_mouse_hovering = false;
	}

	auto const absolute = p_event.xy;

	Avo::View* container = this;
	Index startIndex = _child_views.size() - 1;
	Count numberOfOverlayParents = 0;
	while (true)
	{
		loopStart:
		for (Index a = startIndex; a >= 0; a--)
		{
			Avo::View* const child = container->_child_views[a];

			if (child->_is_mouse_hovering)
			{
				if (child->_are_mouse_events_enabled)
				{
					p_event.xy = absolute - child->get_absolute_top_left();
					child->mouse_leave_listeners(p_event);
					child->mouse_background_leave_listeners(p_event);
				}
				child->_is_mouse_hovering = false;

				if (child->_child_views.size())
				{
					startIndex = child->_child_views.size() - 1;
					container = child;
					if (container->_is_overlay)
					{
						numberOfOverlayParents++;
					}
					goto loopStart;
				}
				else if (!child->_is_overlay)
				{
					break;
				}
			}
		}
		if (container == this)
		{
			break;
		}
		else if (container->_is_overlay)
		{
			startIndex = container->_index - 1;
			container = container->_parent;
			numberOfOverlayParents--;
		}
		else if (numberOfOverlayParents)
		{
			while (!container->_is_overlay && container != this)
			{
				container = container->_parent;
			}
			if (container == this)
			{
				break;
			}
			startIndex = container->_index - 1;
			container = container->_parent;
			numberOfOverlayParents--;
		}
		else
		{
			break;
		}
	}
}

//------------------------------

auto Avo::Gui::drawViews() -> void
{
	if (_isInvalid)
	{
		lock_threads(); // State needs to be static during drawing.

		_drawingContext->beginDrawing();

		auto currentContainer = static_cast<Avo::View*>(this);
		auto startPosition = Index{};

		_drawingContext->reset_transformations();
		_drawingContext->set_opacity(1.f);

		_drawingContext->clear(get_theme_color(ThemeColors::background));

		draw(_drawingContext.get());

		while (true)
		{
			auto isDoneWithContainer = true;

			auto const prepareViewDrawing = [this](View* view) {
				_drawingContext->reset_transformations();
				_drawingContext->setOrigin(view->get_absolute_top_left());
				_drawingContext->set_opacity(1.f);
			};

			auto const drawViewOverlay = [this, &prepareViewDrawing](View* view) {
				prepareViewDrawing(view);
				view->drawOverlay(_drawingContext.get());

				_drawingContext->popClipShape();
			};
			
			for (auto const a : Indices{startPosition, currentContainer->get_views()})
			{
				auto const view = currentContainer->get_view(a);

				if (view->get_size() <= 0.f || !view->get_is_visible())
				{
					continue;
				}

				auto const drawViewShadow = [this, view, &prepareViewDrawing] {
					prepareViewDrawing(view);
					view->drawShadow(_drawingContext.get());
				};

				if (view->get_is_intersecting(currentContainer->get_size()))
				{
					drawViewShadow();

					if (view->getHasCornerStyles())
					{
						_drawingContext->pushClipGeometry(view->_clip_geometry, view->_opacity);
					}
					else
					{
						_drawingContext->pushClipRectangle(view->get_size(), view->_opacity);
					}

					view->draw(_drawingContext.get());

					if (view->get_has_views())
					{
						currentContainer = view;
						startPosition = 0;
						isDoneWithContainer = false;
						break;
					}
					else
					{
						drawViewOverlay(view);
					}
				}
				// Only the shadow is visible because of clipping
				else if (view->get_absolute_shadow_bounds().get_is_intersecting(currentContainer->get_size()))
				{
					drawViewShadow();
				}
			}
			if (isDoneWithContainer)
			{
				if (currentContainer == this)
				{
					break;
				}

				drawViewOverlay(currentContainer);

				startPosition = currentContainer->get_index() + 1U;
				currentContainer = currentContainer->get_parent<View>();
			}
		}

		_drawingContext->reset_transformations();
		_drawingContext->set_opacity(1.f);
		drawOverlay(_drawingContext.get());
		_drawingContext->popClipShape();

		unlock_threads();
		_drawingContext->restoreDrawingState(_drawingContextState);
		_drawingContext->finishDrawing();
	}
}

//------------------------------
// class Avo::OpenFileDialog
//------------------------------

auto Avo::OpenFileDialog::open() const -> std::vector<std::string>
{
#ifdef _WIN32
	auto const dialog = ComReference<IFileOpenDialog>{};
	CoCreateInstance(CLSID_FileOpenDialog, 0, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&dialog));

	auto wide_title = std::array<char16, 256>{};
	convert_utf8_to_utf16(_title, wide_title);
	dialog->SetTitle(reinterpret_cast<LPCWSTR>(wide_title.data()));

	//------------------------------
	// Create the extension filters that the user will choose from.

	auto filters = std::vector<COMDLG_FILTERSPEC>(_file_extensions.size());

	constexpr auto max_filter_string_length = 100;
	constexpr auto half_max_filter_string_length = max_filter_string_length/2;

	// I made 1 big string buffer to decrease the number of allocations.
	auto const filter_string_buffer = std::vector<wchar_t>(max_filter_string_length*_file_extensions.size());
	for (auto const a : Indices{_file_extensions}) {
		auto start = filter_string_buffer.data() + a*max_filter_string_length;
		convert_utf8_to_utf16(_file_extensions[a].name, {reinterpret_cast<char16*>(start), half_max_filter_string_length});
		filters[a].pszName = start;

		start = filter_string_buffer.data() + a*max_filter_string_length + 50;
		convert_utf8_to_utf16(_file_extensions[a].extensions, {reinterpret_cast<char16*>(start), half_max_filter_string_length});
		filters[a].pszSpec = start;
	}
	dialog->SetFileTypes(_file_extensions.size(), filters.data());

	//------------------------------

	if (_can_select_multiple_files) {
		auto options = FILEOPENDIALOGOPTIONS{};
		dialog->GetOptions(&options);
		dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
	}

	//------------------------------

	auto result = std::vector<std::string>();
	if (SUCCEEDED(dialog->Show(_gui ? static_cast<HWND>(_gui->get_window()->get_native_handle()) : nullptr))) {
		if (_can_select_multiple_files) {
			auto items = ComReference<IShellItemArray>{};
			dialog->GetResults(&items);

			auto numberOfResults = DWORD{};
			items->GetCount(&numberOfResults);
			result.resize(numberOfResults);

			for (auto resultIndex : Range{numberOfResults})
			{
				auto item = ComReference<IShellItem>{};
				items->GetItemAt(resultIndex, &item);

				auto name = LPWSTR{};
				item->GetDisplayName(SIGDN_FILESYSPATH, &name);
				if (name)
				{
					result[resultIndex] = Avo::convert_utf16_to_utf8(reinterpret_cast<char16 const*>(name));
					CoTaskMemFree(name);
				}
			}
		}
		else
		{
			auto item = ComReference<IShellItem>{};
			dialog->GetResult(&item);

			auto name = LPWSTR{};
			item->GetDisplayName(SIGDN_FILESYSPATH, &name);
			result.resize(1);
			if (name)
			{
				result[0] = Avo::convert_utf16_to_utf8(reinterpret_cast<char16 const*>(name));
				CoTaskMemFree(name);
			}
		}
	}
	return result;
#endif
}
