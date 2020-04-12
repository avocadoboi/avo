#include "AvoGUI.hpp"

#include "Font data.hpp"

//------------------------------

#include <stack>
#include <random>
#include <chrono>
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

#pragma comment(lib, "Shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
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
#include <stdio.h>
#include <fstream>

#undef None
#endif

//------------------------------

std::default_random_engine randomEngine(time(nullptr));
std::uniform_real_distribution<double> uniformDistribution(0.0, 1.0);
std::normal_distribution<double> normalDistribution(0.0, 1.0);

double AvoGUI::random()
{
	return uniformDistribution(randomEngine);
}
double AvoGUI::randomNormal()
{
	return normalDistribution(randomEngine);
}

//------------------------------

void AvoGUI::convertUtf8ToUtf16(char const* p_input, wchar_t* p_output, uint32 p_numberOfUnitsInOutput)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_UTF8, 0, p_input, -1, p_output, p_numberOfUnitsInOutput);
#endif
}
void AvoGUI::convertUtf8ToUtf16(std::string const& p_input, wchar_t* p_output, uint32 p_numberOfUnitsInOutput)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_UTF8, 0, p_input.data(), p_input.size() + 1, p_output, p_numberOfUnitsInOutput);
#endif
}
void AvoGUI::convertUtf8ToUtf16(char const* p_input, uint32 p_numberOfUnitsInInput, wchar_t* p_output, uint32 p_numberOfUnitsInOutput)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, p_output, p_numberOfUnitsInOutput);
#endif
}
std::wstring AvoGUI::convertUtf8ToUtf16(std::string const& p_input)
{
#ifdef _WIN32
	std::wstring result;
	result.resize(MultiByteToWideChar(CP_UTF8, 0, p_input.data(), p_input.size(), 0, 0));
	MultiByteToWideChar(CP_UTF8, 0, p_input.data(), p_input.size(), (wchar_t*)result.data(), result.size());
	return result;
#endif
}
std::wstring AvoGUI::convertUtf8ToUtf16(char const* p_input)
{
#ifdef _WIN32
	std::wstring result;
	result.resize(MultiByteToWideChar(CP_UTF8, 0, p_input, -1, 0, 0) - 1);
	MultiByteToWideChar(CP_UTF8, 0, p_input, -1, (wchar_t*)result.data(), result.size());
	return result;
#endif
}
std::wstring AvoGUI::convertUtf8ToUtf16(char const* p_input, uint32 p_numberOfUnitsInInput)
{
#ifdef _WIN32
	std::wstring result;
	result.resize(MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0));
	MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, (wchar_t*)result.data(), result.size());
	return result;
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(char const* p_input)
{
#ifdef _WIN32
	return MultiByteToWideChar(CP_UTF8, 0, p_input, -1, 0, 0);
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(char const* p_input, int32 p_numberOfUnitsInInput)
{
#ifdef _WIN32
	return MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0);
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(std::string const& p_string)
{
#ifdef _WIN32
	return MultiByteToWideChar(CP_UTF8, 0, p_string.data(), p_string.size() + 1, 0, 0);
#endif
}

void AvoGUI::convertUtf16ToUtf8(wchar_t const* p_input, char* p_output, uint32 p_numberOfCharactersInOutput)
{
#ifdef _WIN32
	WideCharToMultiByte(CP_UTF8, 0, p_input, -1, p_output, p_numberOfCharactersInOutput, 0, 0);
#endif
}
void AvoGUI::convertUtf16ToUtf8(wchar_t const* p_input, uint32 p_numberOfUnitsInInput, char* p_output, uint32 p_numberOfCharactersInResult)
{
#ifdef _WIN32
	WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, p_output, p_numberOfCharactersInResult, 0, 0);
#endif
}
void AvoGUI::convertUtf16ToUtf8(std::wstring const& p_input, char* p_output, uint32 p_numberOfCharactersInOutput)
{
#ifdef _WIN32
	WideCharToMultiByte(CP_UTF8, 0, p_input.data(), p_input.size() + 1, p_output, p_numberOfCharactersInOutput, 0, 0);
#endif
}
std::string AvoGUI::convertUtf16ToUtf8(std::wstring const& p_input)
{
#ifdef _WIN32
	std::string result;
	result.resize(WideCharToMultiByte(CP_UTF8, 0, p_input.c_str(), p_input.size(), 0, 0, 0, 0));
	WideCharToMultiByte(CP_UTF8, 0, p_input.c_str(), p_input.size(), (char*)result.data(), result.size(), 0, 0);
	return result;
#endif
}
std::string AvoGUI::convertUtf16ToUtf8(wchar_t const* p_input)
{
#ifdef _WIN32
	std::string result;
	result.resize(WideCharToMultiByte(CP_UTF8, 0, p_input, -1, 0, 0, 0, 0) - 1);
	WideCharToMultiByte(CP_UTF8, 0, p_input, -1, (char*)result.data(), result.size(), 0, 0);
	return result;
#endif
}
std::string AvoGUI::convertUtf16ToUtf8(wchar_t const* p_input, uint32 p_numberOfUnitsInInput)
{
#ifdef _WIN32
	std::string result;
	result.resize(WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0, 0, 0));
	WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, (char*)result.data(), result.size(), 0, 0);
	return result;
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input)
{
#ifdef _WIN32
	return WideCharToMultiByte(CP_UTF8, 0, p_input, -1, 0, 0, 0, 0);
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input, uint32 p_numberOfUnitsInInput)
{
#ifdef _WIN32
	return WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0, 0, 0);
#endif
}
uint32 AvoGUI::getNumberOfUnitsInUtfConvertedString(std::wstring const& p_string)
{
#ifdef _WIN32
	return WideCharToMultiByte(CP_UTF8, 0, p_string.data(), p_string.size() + 1, 0, 0, 0, 0);
#endif
}

//------------------------------
// class AvoGUI::Easing
//------------------------------

/*
f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

f'(x) = x0*(3 - 12*t + 9*t*t) + x1*(6*t - 9*t*t) + 3*t*t
*/

float AvoGUI::Easing::easeValue(float p_value, float p_precision) const
{
	if (p_value <= 0.0001f)
	{
		return 0.f;
	}
	if (p_value >= 0.9999f)
	{
		return 1.f;
	}

	float t = p_value < 0.5f ? 0.25f : 0.75f;

	float error = 1;
	while (abs(error) > p_precision)
	{
		error = p_value - t * ((1.f - t) * (3.f * (1.f - t) * x0 + 3.f * t * x1) + t * t);
		t += error / (x0 * (3.f - 12.f * t + 9.f * t * t) + x1 * (6.f * t - 9.f * t * t) + 3.f * t * t);
	}

	return t * ((1.f - t) * (3.f * (1.f - t) * y0 + 3.f * t * y1) + t * t);
}

//------------------------------
// class AvoGUI::Rectangle
//------------------------------

template <typename RectangleType>
bool AvoGUI::Rectangle<RectangleType>::getIsContaining(ProtectedRectangle* p_protectedRectangle) const
{
	return p_protectedRectangle->getLeft() >= left && p_protectedRectangle->getTop() >= top
		&& p_protectedRectangle->getRight() <= right && p_protectedRectangle->getBottom() <= bottom;
}
template <typename RectangleType>
bool AvoGUI::Rectangle<RectangleType>::getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const
{
	return p_protectedRectangle->getRight() >= left && p_protectedRectangle->getBottom() >= top
		&& p_protectedRectangle->getLeft() <= right && p_protectedRectangle->getTop() <= bottom;
}

//------------------------------
// class AvoGUI::View
//------------------------------

//
// Private
//

AvoGUI::Point<float> AvoGUI::View::calculateAbsolutePositionRelativeTo(AvoGUI::Point<float> p_position) const
{
	AvoGUI::View* container = getParent();
	while (container && container != getGui())
	{
		p_position += container->getTopLeft();
		container = container->getParent();
	}

	return p_position;
}
void AvoGUI::View::addChild(AvoGUI::View* p_view)
{
	if (p_view)
	{
		p_view->m_index = m_children.size();
		m_children.push_back(p_view);
		updateViewDrawingIndex(p_view);

		sendChildAttachmentEvents(p_view);
	}
}

void AvoGUI::View::updateViewDrawingIndex(AvoGUI::View* p_view)
{
	uint32 numberOfViews = (uint32)m_children.size();
	if (numberOfViews <= 1 || p_view->getParent() != this)
	{
		return;
	}

	float elevation = p_view->getElevation();
	if (!p_view->getIndex() || (p_view->getIndex() < numberOfViews - 1U && m_children[p_view->getIndex() + 1U]->getElevation() < elevation))
	{
		for (uint32 a = p_view->getIndex(); a < numberOfViews; a++)
		{
			if (a == numberOfViews - 1U || m_children[a + 1U]->getElevation() >= elevation)
			{
				m_children[a] = p_view;
				p_view->m_index = a;
				return;
			}
			else
			{
				m_children[a] = m_children[a + 1U];
				m_children[a]->m_index = a;
			}
		}
	}
	else
	{
		for (int32 a = p_view->getIndex(); a >= 0; a--)
		{
			if (!a || m_children[a - 1]->getElevation() <= elevation)
			{
				m_children[a] = p_view;
				p_view->m_index = a;
				return;
			}
			else
			{
				m_children[a] = m_children[a - 1];
				m_children[a]->m_index = a;
			}
		}
	}
}

void AvoGUI::View::updateShadow()
{
	if (getWidth() >= 1.f && getHeight() >= 1.f && m_elevation > 0.00001f && m_hasShadow && m_elevation < 400.f)
	{
		if (m_shadowImage)
		{
			m_shadowImage->forget();
		}
		m_shadowImage = m_gui->getDrawingContext()->createRectangleShadowImage(getSize(), m_corners, m_elevation, m_theme->colors[ThemeColors::shadow]);
		m_shadowBounds = AvoGUI::Rectangle<float>(
			AvoGUI::Point<float>(
				0.5f * (m_bounds.right - m_bounds.left - (float)m_shadowImage->getWidth()),
				0.35f * (m_bounds.bottom - m_bounds.top - (float)m_shadowImage->getHeight())
				), m_shadowImage->getSize()
			);
		m_shadowImage->setTopLeft(m_shadowBounds.getTopLeft());
	}
	else
	{
		if (m_shadowImage)
		{
			m_shadowImage->forget();
			m_shadowImage = 0;
		}
		m_shadowBounds = m_bounds.createCopyAtOrigin();
	}
}

//
// Protected
//

void AvoGUI::View::sendBoundsChangeEvents(AvoGUI::Rectangle<float> const& p_previousBounds)
{
	if (p_previousBounds.left != m_bounds.left || p_previousBounds.right != m_bounds.right ||
		p_previousBounds.top != m_bounds.top || p_previousBounds.bottom != m_bounds.bottom)
	{
		if (fabsf(p_previousBounds.getWidth() - m_bounds.getWidth()) > 0.001f || fabsf(p_previousBounds.getHeight() - m_bounds.getHeight()) > 0.001f)
		{
			updateShadow(); // This is to update the shadow bounds and image.

			sendSizeChangeEvents(p_previousBounds.getWidth(), p_previousBounds.getHeight());

			updateClipGeometry();
		}

		if (getParent())
		{
			AvoGUI::Point<float> const& mousePosition = getGui()->getWindow()->getMousePosition() - getParent()->getAbsoluteTopLeft();
			if (getIsContaining(mousePosition) != p_previousBounds.getIsContaining(mousePosition))
			{
				MouseEvent event;
				event.x = mousePosition.x + getParent()->getAbsoluteLeft();
				event.y = mousePosition.y + getParent()->getAbsoluteTop();
				getGui()->handleGlobalMouseMove(event);
			}
		}

		m_boundsChangeListeners.notifyAll(this, p_previousBounds);
	}
}

void AvoGUI::View::updateClipGeometry()
{
	if (getHasCornerStyles())
	{
		if (m_clipGeometry)
		{
			if (m_clipGeometry->getReferenceCount() > 1)
			{
				m_clipGeometry->forget();
				return;
			}
			m_clipGeometry->forget();
		}
		m_clipGeometry = getGui()->getDrawingContext()->createCornerRectangleGeometry(getSize(), m_corners);
	}
}

//
// Public
//

AvoGUI::View::View(AvoGUI::View* p_parent, AvoGUI::Rectangle<float> const& p_bounds) :
	ProtectedRectangle(p_bounds), m_shadowBounds(p_bounds)
{
	if (p_parent && p_parent != this)
	{
		setParent(p_parent);

		m_gui = m_parent->getGui();

		m_theme = (Theme*)m_parent->getTheme();
		m_theme->remember();
	}
	else
	{
		m_theme = new Theme();
	}
}
AvoGUI::View::~View()
{
	if (m_id)
	{
		if (m_gui == this)
		{
			((AvoGUI::Gui*)this)->getParent()->m_viewsById.erase(m_id);
		}
		else
		{
			m_gui->m_viewsById.erase(m_id);
		}
	}
	m_theme->forget();
	if (m_shadowImage)
	{
		m_shadowImage->forget();
	}
	if (m_clipGeometry)
	{
		m_clipGeometry->forget();
	}
	removeAllChildren();
	if (m_parent)
	{
		remember();
		m_parent->removeChild(m_index);
	}
}

//------------------------------

void AvoGUI::View::setClipGeometry(Geometry* p_geometry)
{
	if (m_clipGeometry)
	{
		m_clipGeometry->forget();
	}
	p_geometry->remember();
	m_clipGeometry = p_geometry;
}

//------------------------------

AvoGUI::DrawingContext* AvoGUI::View::getDrawingContext()
{
	return m_gui->getDrawingContext();
}

//------------------------------

void AvoGUI::View::setParent(AvoGUI::View* p_container)
{
	if (p_container == m_parent)
	{
		return;
	}

	if (m_parent)
	{
		remember();
		m_parent->removeChild(this); // This forgets this view
	}

	if (p_container)
	{
		m_parent = p_container;
		m_gui = m_parent->getGui();

		m_index = m_parent->getNumberOfChildren();
		if (m_parent == this)
		{
			m_layerIndex = 0;
		}
		else
		{
			m_layerIndex = m_parent->getLayerIndex() + 1U;
		}
		m_absolutePosition.x = m_parent->getAbsoluteLeft() + m_bounds.left;
		m_absolutePosition.y = m_parent->getAbsoluteTop() + m_bounds.top;
		m_parent->addChild(this);
		m_parent->updateViewDrawingIndex(this);
	}
	else
	{
		m_parent = 0;
		m_layerIndex = 0;
		m_index = 0;
	}
}

void AvoGUI::View::removeChild(AvoGUI::View* p_view)
{
	if (p_view && p_view->getParent() == this)
	{
		removeChild(p_view->getIndex());
	}
}
void AvoGUI::View::removeChild(uint32 p_viewIndex)
{
	AvoGUI::View* childToRemove = m_children[p_viewIndex];
	sendChildDetachmentEvents(childToRemove);

	childToRemove->m_parent = nullptr;
	childToRemove->forget();

	for (uint32 a = p_viewIndex; a < m_children.size() - 1; a++)
	{
		m_children[a] = m_children[a + 1];
		m_children[a]->m_index = a;
	}
	m_children.pop_back();
}
void AvoGUI::View::removeAllChildren()
{
	while (!m_children.empty()) // That function naming, ew... Why didn't they call it getIsEmpty? empty() should be emptying something >:^(
	{
		AvoGUI::View* child = m_children.back();

		sendChildDetachmentEvents(child);

		child->m_parent = nullptr;
		child->forget();

		m_children.pop_back();
	}
}
void AvoGUI::View::setId(uint64 p_id)
{
	if (m_id != p_id)
	{
		if (m_gui == this && m_gui->getParent())
		{
			if (m_id)
			{
				m_gui->getParent()->m_viewsById.erase(m_id);
			}
			m_id = p_id;
			if (p_id)
			{
				m_gui->getParent()->m_viewsById[p_id] = this;
			}
		}
		else
		{
			if (m_id)
			{
				m_gui->m_viewsById.erase(m_id);
			}
			m_id = p_id;
			if (p_id)
			{
				m_gui->m_viewsById[p_id] = this;
			}
		}
	}
}

//------------------------------

void AvoGUI::View::setIsVisible(bool p_isVisible)
{
	if (p_isVisible != m_isVisible)
	{
		m_isVisible = p_isVisible;

		//AvoGUI::Point<float> const& mousePosition = getGUI()->getWindow()->getMousePosition();
		//if (getGUI()->getIsContaining(mousePosition))
		//{
		//	MouseEvent mouseEvent;
		//	mouseEvent.x = mousePosition.x;
		//	mouseEvent.y = mousePosition.y;
		//	mouseEvent.movementX = 0;
		//	mouseEvent.movementY = 0;
		//	getGUI()->handleGlobalMouseMove(mouseEvent);
		//}
	}
}

//------------------------------

void AvoGUI::View::setElevation(float p_elevation)
{
	p_elevation = float(p_elevation < 0.f) * FLT_MAX + p_elevation;

	if (m_elevation != p_elevation)
	{
		m_elevation = p_elevation;
		updateShadow();
		m_parent->updateViewDrawingIndex(this);
	}
}

void AvoGUI::View::setHasShadow(bool p_hasShadow)
{
	if (m_hasShadow != p_hasShadow)
	{
		m_hasShadow = p_hasShadow;
		if (m_hasShadow)
		{
			updateShadow();
		}
		else if (m_shadowImage)
		{
			m_shadowImage->forget();
			m_shadowImage = 0;
			m_shadowBounds = m_bounds.createCopyAtOrigin();
		}
	}
}

//------------------------------

void AvoGUI::View::queueAnimationUpdate()
{
	if (!m_isInAnimationUpdateQueue && m_gui && m_isVisible)
	{
		m_gui->m_animationUpdateQueue.push_back(this);
		remember();

		m_isInAnimationUpdateQueue = true;
	}
}

//------------------------------

void AvoGUI::View::handleMouseBackgroundEnter(MouseEvent const& p_event)
{
	getGui()->getWindow()->setCursor(m_cursor);
}

//------------------------------

void AvoGUI::View::invalidate()
{
	if (m_gui)
	{
		AvoGUI::Rectangle<float> shadowBounds(getAbsoluteShadowBounds());

		float dipToPixelFactor = m_gui->m_window->getDipToPixelFactor();
		shadowBounds.left = floor(shadowBounds.left * dipToPixelFactor) / dipToPixelFactor;
		shadowBounds.top = floor(shadowBounds.top * dipToPixelFactor) / dipToPixelFactor;
		shadowBounds.right = ceil(shadowBounds.right * dipToPixelFactor) / dipToPixelFactor;
		shadowBounds.bottom = ceil(shadowBounds.bottom * dipToPixelFactor) / dipToPixelFactor;

		if (shadowBounds == m_lastInvalidatedShadowBounds || (!m_lastInvalidatedShadowBounds.getWidth() && !m_lastInvalidatedShadowBounds.getHeight()))
		{
			m_gui->invalidateRectangle(shadowBounds);
		}
		else if (shadowBounds.getIsIntersecting(m_lastInvalidatedShadowBounds))
		{
			m_gui->invalidateRectangle(m_lastInvalidatedShadowBounds.createContainedCopy(shadowBounds));
		}
		else
		{
			m_gui->invalidateRectangle(shadowBounds);
			m_gui->invalidateRectangle(m_lastInvalidatedShadowBounds);
		}

		m_lastInvalidatedShadowBounds = shadowBounds;
	}
}

void AvoGUI::View::drawShadow(DrawingContext* p_drawingContext)
{
	if (m_shadowImage && m_hasShadow)
	{
		p_drawingContext->setColor(Color(1.f));
		p_drawingContext->drawImage(m_shadowImage, m_opacity);
	}
}

//------------------------------

#pragma region Platform-specific window implementations
#ifdef _WIN32

#define IUnknownDefinition(p_interfaceName)\
ULONG __stdcall AddRef() override\
{\
return InterlockedIncrement(&m_referenceCount);\
}\
ULONG __stdcall Release() override\
{\
uint32 referenceCount = InterlockedDecrement(&m_referenceCount);\
if (!referenceCount)\
{\
	delete this;\
	return 0;\
}\
return referenceCount;\
}\
HRESULT __stdcall QueryInterface(IID const& p_id, void** p_object) override\
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

//------------------------------

AvoGUI::ModifierKeyFlags convertWindowsKeyStateToModifierKeyFlags(unsigned short p_keyState)
{
	AvoGUI::ModifierKeyFlags modifierFlags = AvoGUI::ModifierKeyFlags::None;

	if (p_keyState & MK_CONTROL)
		modifierFlags |= AvoGUI::ModifierKeyFlags::Control;
	if (p_keyState & MK_SHIFT)
		modifierFlags |= AvoGUI::ModifierKeyFlags::Shift;
	if (p_keyState & MK_LBUTTON)
		modifierFlags |= AvoGUI::ModifierKeyFlags::LeftMouse;
	if (p_keyState & MK_MBUTTON)
		modifierFlags |= AvoGUI::ModifierKeyFlags::MiddleMouse;
	if (p_keyState & MK_RBUTTON)
		modifierFlags |= AvoGUI::ModifierKeyFlags::RightMouse;
	if (p_keyState & MK_XBUTTON1)
		modifierFlags |= AvoGUI::ModifierKeyFlags::X0Mouse;
	if (p_keyState & MK_XBUTTON2)
		modifierFlags |= AvoGUI::ModifierKeyFlags::X1Mouse;
	if (GetKeyState(VK_MENU) < 0)
		modifierFlags |= AvoGUI::ModifierKeyFlags::Alt;

	return modifierFlags;
}

//------------------------------

class OleFormatEnumerator : public IEnumFORMATETC
{
private:
	uint32 m_referenceCount{ 1 };

	FORMATETC* m_formats{ nullptr };
	uint32 m_numberOfFormats;
	uint32 m_currentFormatIndex{ 0 };

public:
	OleFormatEnumerator(FORMATETC* p_formats, uint32 p_numberOfFormats) :
		m_numberOfFormats(p_numberOfFormats)
	{
		m_formats = new FORMATETC[p_numberOfFormats];
		for (uint32 a = 0; a < p_numberOfFormats; a++)
		{
			m_formats[a] = p_formats[a];
			if (m_formats[a].ptd)
			{
				m_formats[a].ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));
				*m_formats[a].ptd = *p_formats[a].ptd;
			}
		}
	}
	~OleFormatEnumerator()
	{
		for (uint32 a = 0; a < m_numberOfFormats; a++)
		{
			if (m_formats[a].ptd)
			{
				CoTaskMemFree(m_formats[a].ptd);
			}
		}
		delete[] m_formats;
	}

	//------------------------------

	IUnknownDefinition(IEnumFORMATETC)

	//------------------------------

	HRESULT __stdcall Next(ULONG p_numberOfFormatsToGet, FORMATETC* p_formats, ULONG* p_numberOfFormatsGotten) override
	{
		uint32 numberOfFormatsGotten = 0;
		while (m_currentFormatIndex < m_numberOfFormats && numberOfFormatsGotten <= p_numberOfFormatsToGet)
		{
			*p_formats = m_formats[m_currentFormatIndex];

			m_currentFormatIndex++;
			numberOfFormatsGotten++;
			p_formats++;
		}
		if (p_numberOfFormatsGotten)
		{
			*p_numberOfFormatsGotten = numberOfFormatsGotten;
		}
		return p_numberOfFormatsToGet != numberOfFormatsGotten;
	}

	HRESULT __stdcall Skip(ULONG p_offset) override
	{
		m_currentFormatIndex += p_offset;
		return m_currentFormatIndex < m_numberOfFormats ? S_OK : S_FALSE;
	}

	HRESULT __stdcall Reset() override
	{
		m_currentFormatIndex = 0;
		return S_OK;
	}

	HRESULT __stdcall Clone(IEnumFORMATETC** p_formatEnumerator) override
	{
		OleFormatEnumerator* newFormatEnumerator = new OleFormatEnumerator(m_formats, m_numberOfFormats);
		newFormatEnumerator->m_currentFormatIndex = m_currentFormatIndex;
		*p_formatEnumerator = newFormatEnumerator;
		return S_OK;
	}
};

/*
	Communicates data in drag and drop operations.
*/
class OleDataObject : public IDataObject
{
private:
	uint32 m_referenceCount;

	std::vector<FORMATETC> m_formats;
	std::vector<STGMEDIUM> m_mediums;

public:
	OleDataObject(FORMATETC* p_formats, STGMEDIUM* p_mediums, uint32 p_numberOfFormats) :
		m_referenceCount(1)
	{
		m_formats.resize(p_numberOfFormats);
		m_mediums.resize(p_numberOfFormats);

		for (uint32 a = 0; a < p_numberOfFormats; a++)
		{
			m_formats[a] = p_formats[a];
			m_mediums[a] = p_mediums[a];
		}
	}
	~OleDataObject()
	{
		for (STGMEDIUM& medium : m_mediums)
		{
			ReleaseStgMedium(&medium);
		}
	}

	//------------------------------

	IUnknownDefinition(IDataObject)

	//------------------------------

	HRESULT __stdcall SetData(FORMATETC* p_format, STGMEDIUM* p_medium, BOOL p_willRelease) override
	{
		if (p_willRelease)
		{
			bool hasFormat = false;
			for (uint32 a = 0; a < m_formats.size(); a++)
			{
				if (m_formats[a].cfFormat == p_format->cfFormat &&
					m_formats[a].dwAspect == p_format->dwAspect &&
					m_formats[a].tymed & p_format->tymed &&
					m_formats[a].lindex == p_format->lindex)
				{
					hasFormat = true;
					ReleaseStgMedium(&m_mediums[a]);
					m_mediums[a] = *p_medium;
				}
			}
			if (!hasFormat)
			{
				m_formats.push_back(*p_format);
				m_mediums.push_back(*p_medium);
			}
			return S_OK;
		}
		else
		{
			return E_NOTIMPL;
		}
	}
	HRESULT __stdcall QueryGetData(FORMATETC* p_format) override
	{
		for (uint32 a = 0; a < m_formats.size(); a++)
		{
			if (m_formats[a].cfFormat == p_format->cfFormat &&
				m_formats[a].dwAspect == p_format->dwAspect &&
				m_formats[a].tymed & p_format->tymed && 
				m_formats[a].lindex == p_format->lindex)
			{
				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}
	HRESULT __stdcall GetData(FORMATETC* p_format, STGMEDIUM* p_medium) override
	{
		for (uint32 a = 0; a < m_formats.size(); a++)
		{
			if (m_formats[a].cfFormat == p_format->cfFormat &&
				m_formats[a].dwAspect == p_format->dwAspect &&
				m_formats[a].tymed & p_format->tymed &&
				m_formats[a].lindex == p_format->lindex)
			{
				p_medium->pUnkForRelease = 0;
				if ((p_medium->tymed = m_formats[a].tymed) == TYMED_HGLOBAL)
				{
					// Copy memory from m_mediums[a].hGlobal to p_medium->hGlobal
					SIZE_T size = GlobalSize(m_mediums[a].hGlobal);
					p_medium->hGlobal = GlobalAlloc(GMEM_FIXED, size); // Returns pointer
					memcpy(p_medium->hGlobal, GlobalLock(m_mediums[a].hGlobal), size);
					GlobalUnlock(m_mediums[a].hGlobal);
				}
				else if (p_medium->tymed == TYMED_ISTREAM)
				{
					p_medium->pstm = SHCreateMemStream(0, 0);

					STATSTG stats;
					m_mediums[a].pstm->Stat(&stats, STATFLAG_NONAME);
					p_medium->pstm->SetSize(stats.cbSize);

					m_mediums[a].pstm->Seek({ 0 }, SEEK_SET, 0);
					m_mediums[a].pstm->CopyTo(p_medium->pstm, stats.cbSize, 0, 0);

					p_medium->pstm->Seek({ 0 }, SEEK_SET, 0);
				}

				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}
	// Non-allocating version of GetData
	HRESULT __stdcall GetDataHere(FORMATETC* p_format, STGMEDIUM* p_medium) override
	{
		for (uint32 a = 0; a < m_formats.size(); a++)
		{
			if (m_formats[a].cfFormat == p_format->cfFormat &&
				m_formats[a].dwAspect == p_format->dwAspect &&
				m_formats[a].tymed & p_format->tymed &&
				m_formats[a].lindex == p_format->lindex)
			{
				p_medium->pUnkForRelease = 0;
				if ((p_medium->tymed = m_formats[a].tymed) == TYMED_HGLOBAL)
				{
					// Copy memory from m_mediums[a].hGlobal to p_medium->hGlobal
					SIZE_T size = GlobalSize(m_mediums[a].hGlobal);
					memcpy(GlobalLock(p_medium->hGlobal), GlobalLock(m_mediums[a].hGlobal), size);
					GlobalUnlock(m_mediums[a].hGlobal);
					GlobalUnlock(p_medium->hGlobal);
				}
				else if (p_medium->tymed == TYMED_ISTREAM)
				{
					ULARGE_INTEGER size;
					size.QuadPart = ULONGLONG_MAX;
					m_mediums[a].pstm->CopyTo(p_medium->pstm, size, 0, 0);
				}

				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}

	HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC* p_formatIn, FORMATETC* p_formatOut) override
	{
		p_formatOut->ptd = 0;
		return E_NOTIMPL;
	}
	HRESULT __stdcall EnumFormatEtc(DWORD p_direction, IEnumFORMATETC** p_formatEnumerator) override
	{
		if (p_direction == DATADIR_GET)
		{
			*p_formatEnumerator = new OleFormatEnumerator(m_formats.data(), m_formats.size());
			return S_OK;
		}
		
		// it's DATADIR_SET
		return E_NOTIMPL;
	}

	HRESULT __stdcall DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*) override
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}
	HRESULT __stdcall DUnadvise(DWORD) override
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}
	HRESULT __stdcall EnumDAdvise(IEnumSTATDATA**) override
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}
};

//------------------------------

class OleDropSource :
	public IDropSource
{
private:
	uint32 m_referenceCount;
	AvoGUI::Gui* m_gui;

	IDragSourceHelper* m_dragImageHelper;

public:
	OleDropSource(AvoGUI::Gui* p_gui) :
		m_referenceCount(1), m_gui(p_gui)
	{
		CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_dragImageHelper));
	}
	~OleDropSource()
	{
		m_dragImageHelper->Release();
	}

	//------------------------------

	IUnknownDefinition(IDropSource)

	//------------------------------

	void setDragImage(AvoGUI::Image* p_image, AvoGUI::Point<float> const& p_cursorPosition, IDataObject* p_dataObject)
	{
		D2D_SIZE_U size = ((ID2D1Bitmap1*)p_image->getHandle())->GetPixelSize();
		float dipToPixelFactor = m_gui->getWindow()->getDipToPixelFactor();

		SHDRAGIMAGE dragImage;
		dragImage.crColorKey = RGB(0, 0, 0);
		dragImage.hbmpDragImage = (HBITMAP)m_gui->getDrawingContext()->createNativeImageFromImage(p_image);
		dragImage.ptOffset = { long(p_cursorPosition.x*dipToPixelFactor), long(p_cursorPosition.y * dipToPixelFactor) };
		dragImage.sizeDragImage = { (long)size.width, (long)size.height };
		HRESULT result = m_dragImageHelper->InitializeFromBitmap(&dragImage, p_dataObject);
	}

	//------------------------------

	HRESULT __stdcall QueryContinueDrag(BOOL p_wasEscapePressed, DWORD p_keyState)
	{
		if (p_wasEscapePressed)
		{
			return DRAGDROP_S_CANCEL;
		}
		if (!(p_keyState & MK_LBUTTON))
		{
			return DRAGDROP_S_DROP;
		}

		return S_OK;
	}

	HRESULT __stdcall GiveFeedback(DWORD p_effect)
	{
		AvoGUI::DragDropOperation operation = AvoGUI::DragDropOperation::None;
		switch (p_effect)
		{
			case DROPEFFECT_COPY:
				operation = AvoGUI::DragDropOperation::Copy;
				break;
			case DROPEFFECT_MOVE:
				operation = AvoGUI::DragDropOperation::Move;
				break;
			case DROPEFFECT_LINK:
				operation = AvoGUI::DragDropOperation::Link;
		}
		m_gui->handleGlobalDragDropOperationChange(operation);
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}
};

//------------------------------

class OleClipboardData :
	public AvoGUI::ClipboardData
{
private:
	IDataObject* m_dataObject{ nullptr };
	FORMATETC* m_oleFormats{ nullptr };
	uint32 m_numberOfFormats{ 0 };
	uint32 m_numberOfFiles{ 0 };

	FORMATETC* m_fileDescriptorFormat{ nullptr };
	FORMATETC* m_itemNamesFormat{ nullptr };
	FORMATETC* m_textFormat{ nullptr };

	std::vector<STGMEDIUM> m_globalDataToRelease;
	std::vector<char const*> m_streamBuffersToRelease;

	uint32 m_clipboardFormat_fileContents;
	uint32 m_clipboardFormat_fileGroupDescriptor;

	AvoGUI::Gui* m_gui;

	void releaseDataObject()
	{
		if (m_dataObject)
		{
			m_dataObject->Release();
			m_dataObject = 0;
			delete[] m_oleFormats;
			m_oleFormats = 0;
			m_numberOfFormats = 0;
			m_numberOfFiles = 0;
			m_fileDescriptorFormat = 0;
			m_itemNamesFormat = 0;
			m_textFormat = 0;
		}
		for (STGMEDIUM& medium : m_globalDataToRelease)
		{
			GlobalUnlock(medium.hGlobal);
			ReleaseStgMedium(&medium);
		}
		for (char const* buffer : m_streamBuffersToRelease)
		{
			delete[] buffer;
		}
		m_globalDataToRelease.clear();
		m_streamBuffersToRelease.clear();
		formats.clear();
	}

public:
	OleClipboardData(AvoGUI::Gui* p_gui) :
		m_gui(p_gui)
	{
		m_clipboardFormat_fileContents = RegisterClipboardFormatW(CFSTR_FILECONTENTS);
		m_clipboardFormat_fileGroupDescriptor = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
	}
	~OleClipboardData()
	{
		releaseDataObject();
	}

	void setOleDataObject(IDataObject* p_dataObject)
	{
		releaseDataObject();
		m_dataObject = p_dataObject;
		if (m_dataObject)
		{
			m_dataObject->AddRef();

			IEnumFORMATETC* enumerator = 0;
			m_dataObject->EnumFormatEtc(DATADIR_GET, &enumerator);

			m_oleFormats = new FORMATETC[80];
			enumerator->Next(80, m_oleFormats, (ULONG*)&m_numberOfFormats);
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				uint32 format = m_oleFormats[a].cfFormat;
				if (format == CF_HDROP && m_oleFormats[a].dwAspect == DVASPECT_CONTENT)
				{
					m_itemNamesFormat = m_oleFormats + a;
				}
				else if (format == m_clipboardFormat_fileGroupDescriptor)
				{
					m_fileDescriptorFormat = m_oleFormats + a;
				}
				else if (format == m_clipboardFormat_fileContents)
				{
					m_numberOfFiles++;
				}
				else if (format == CF_UNICODETEXT)
				{
					m_textFormat = m_oleFormats + a;
				}
				formats.push_back(m_oleFormats[a].cfFormat);
			}

			enumerator->Release();
		}
	}
	IDataObject* getOleDataObject()
	{
		return m_dataObject;
	}

	AvoGUI::DragDropFormatData getDataForFormat(uint32 p_formatIndex) const override
	{
		switch (m_oleFormats[p_formatIndex].tymed)
		{
			case TYMED_HGLOBAL:
			{
				STGMEDIUM medium;
				HRESULT result = m_dataObject->GetData(m_oleFormats + p_formatIndex, &medium);
				if (result == S_OK)
				{
					AvoGUI::DragDropFormatData data { (char const*)GlobalLock(medium.hGlobal), GlobalSize(medium.hGlobal) };
					((OleClipboardData*)this)->m_globalDataToRelease.push_back(medium);
					return data;
				}
				break;
			}
			case TYMED_ISTREAM:
			{
				STGMEDIUM medium;
				HRESULT result = m_dataObject->GetData(m_oleFormats + p_formatIndex, &medium);
				if (result == S_OK)
				{
					STATSTG stats;
					medium.pstm->Stat(&stats, STATFLAG_NONAME);

					char const* buffer = new char[stats.cbSize.QuadPart];
					ULONG numberOfBytesRead = 0;
					medium.pstm->Seek({ 0 }, SEEK_SET, 0);
					medium.pstm->Read((void*)buffer, stats.cbSize.QuadPart, &numberOfBytesRead);
					ReleaseStgMedium(&medium);
					((OleClipboardData*)this)->m_streamBuffersToRelease.push_back(buffer);

					return { buffer, numberOfBytesRead };
				}
				break;
			}
		}
		return { 0, 0 };
	}
	std::string getFormatName(uint32 p_format) const override
	{
		wchar_t name[51];
		int length = GetClipboardFormatNameW(p_format, name, 50);
		if (!length)
		{
			switch (p_format)
			{
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
		return AvoGUI::convertUtf16ToUtf8(name);
	}

	std::string getString() const override
	{
		if (m_textFormat)
		{
			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_textFormat, &medium);
			if (result == S_OK)
			{
				std::string string = AvoGUI::convertUtf16ToUtf8((wchar_t*)GlobalLock(medium.hGlobal));
				GlobalUnlock(medium.hGlobal);
				ReleaseStgMedium(&medium);
				return string;
			}
		}
		return "";
	}
	std::wstring getUtf16String() const override
	{
		if (m_textFormat)
		{
			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_textFormat, &medium);
			if (result == S_OK)
			{
				std::wstring string = (wchar_t*)GlobalLock(medium.hGlobal);
				GlobalUnlock(medium.hGlobal);
				ReleaseStgMedium(&medium);
				return string;
			}
		}
		return L"";
	}
	bool getHasString() const
	{
		return m_textFormat;
	}

	std::vector<std::string> getItemNames() const override
	{
		if (m_itemNamesFormat)
		{
			std::vector<std::string> itemNames;

			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_itemNamesFormat, &medium);
			if (result == S_OK)
			{
				if (medium.tymed == TYMED_HGLOBAL)
				{
					DROPFILES* filenameStructure = (DROPFILES*)GlobalLock(medium.hGlobal);

					wchar_t const* currentBufferPosition = (wchar_t const*)((char*)filenameStructure + filenameStructure->pFiles) + 1;
					itemNames.push_back(AvoGUI::convertUtf16ToUtf8(currentBufferPosition - 1));
					while (true)
					{
						if (!*currentBufferPosition)
						{
							if (!*(currentBufferPosition + 1))
							{
								break;
							}
							itemNames.push_back(AvoGUI::convertUtf16ToUtf8(currentBufferPosition + 1));
						}
						currentBufferPosition++;
					}

					GlobalUnlock(medium.hGlobal);
				}
				ReleaseStgMedium(&medium);
			}
			return itemNames;
		}
		return getFileNames();
	}
	std::vector<std::wstring> getUtf16ItemNames() const override
	{
		if (m_itemNamesFormat)
		{
			std::vector<std::wstring> itemNames;

			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_itemNamesFormat, &medium);
			if (result == S_OK)
			{
				if (medium.tymed == TYMED_HGLOBAL)
				{
					DROPFILES* filenameStructure = (DROPFILES*)GlobalLock(medium.hGlobal);

					wchar_t const* currentBufferPosition = (wchar_t const*)((char*)filenameStructure + filenameStructure->pFiles) + 1;
					itemNames.push_back(currentBufferPosition - 1);
					while (true)
					{
						if (!*currentBufferPosition)
						{
							if (!*(currentBufferPosition + 1))
							{
								break;
							}
							itemNames.push_back(currentBufferPosition + 1);
						}
						currentBufferPosition++;
					}

					GlobalUnlock(medium.hGlobal);
				}
				ReleaseStgMedium(&medium);
			}
			return itemNames;
		}
		return getUtf16FileNames();
	}
	uint32 getNumberOfItemNames() const override
	{
		if (m_itemNamesFormat)
		{
			uint32 numberOfItemNames = 0;

			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_itemNamesFormat, &medium);
			if (result == S_OK)
			{
				if (medium.tymed == TYMED_HGLOBAL)
				{
					DROPFILES* filenameStructure = (DROPFILES*)GlobalLock(medium.hGlobal);

					wchar_t const* currentBufferPosition = (wchar_t const*)((char*)filenameStructure + filenameStructure->pFiles) + 1;
					while (true)
					{
						if (!*currentBufferPosition)
						{
							numberOfItemNames++;
							if (!*(currentBufferPosition + 1))
							{
								break;
							}
						}
						currentBufferPosition++;
					}

					GlobalUnlock(medium.hGlobal);
				}
				ReleaseStgMedium(&medium);
			}
			return numberOfItemNames;
		}
		return getNumberOfFiles();
	}

	std::vector<std::string> getFileNames() const
	{
		if (m_fileDescriptorFormat)
		{
			std::vector<std::string> fileNames;

			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_fileDescriptorFormat, &medium);
			if (result == S_OK)
			{
				if (medium.tymed == TYMED_HGLOBAL)
				{
					FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)GlobalLock(medium.hGlobal);
					fileNames.reserve(groupDescriptor->cItems);
					for (uint32 a = 0; a < fileNames.size(); a++)
					{
						fileNames[a] = AvoGUI::convertUtf16ToUtf8(groupDescriptor->fgd[a].cFileName);
					}
					GlobalUnlock(medium.hGlobal);
				}
				ReleaseStgMedium(&medium);
			}
			return fileNames;
		}
		return { };
	}
	std::vector<std::wstring> getUtf16FileNames() const
	{
		if (m_fileDescriptorFormat)
		{
			std::vector<std::wstring> fileNames;

			STGMEDIUM medium;
			HRESULT result = m_dataObject->GetData(m_fileDescriptorFormat, &medium);
			if (result == S_OK)
			{
				if (medium.tymed == TYMED_HGLOBAL)
				{
					FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)GlobalLock(medium.hGlobal);
					fileNames.reserve(groupDescriptor->cItems);
					for (uint32 a = 0; a < fileNames.size(); a++)
					{
						fileNames[a] = groupDescriptor->fgd[a].cFileName;
					}
					GlobalUnlock(medium.hGlobal);
				}
				ReleaseStgMedium(&medium);
			}
			return fileNames;
		}
		return { };
	}
	std::vector<std::string> getFileContents() const
	{
		if (m_fileDescriptorFormat)
		{
			std::vector<std::string> output;

			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				if (m_oleFormats[a].cfFormat == m_clipboardFormat_fileContents)
				{
					STGMEDIUM medium;
					HRESULT errorCode = m_dataObject->GetData(m_oleFormats + a, &medium);
					if (errorCode == S_OK)
					{
						if (medium.tymed == TYMED_HGLOBAL)
						{
							output.push_back(std::string());
							output.back().assign((char*)GlobalLock(medium.hGlobal), GlobalSize(medium.hGlobal));
							GlobalUnlock(medium.hGlobal);
						}
						else if (medium.tymed == TYMED_ISTREAM)
						{
							STATSTG stats;
							medium.pstm->Stat(&stats, STATFLAG_NONAME);

							output.push_back(std::string());
							output.back().resize(stats.cbSize.QuadPart);

							ULONG bufferSize = 0;
							medium.pstm->Seek({ 0 }, SEEK_SET, 0);
							medium.pstm->Read((char*)output.back().data(), output.size(), &bufferSize);
							if (bufferSize != output.back().size())
							{
								output.back().resize(bufferSize);
							}
						}
						ReleaseStgMedium(&medium);
					}
				}
			}
			return output;
		}
		return { };
	}
	std::string getFileContents(uint32 p_index) const 
	{
		if (m_fileDescriptorFormat)
		{
			uint32 currentIndex = 0;
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				if (m_oleFormats[a].cfFormat == m_clipboardFormat_fileContents)
				{
					if (currentIndex++ == p_index)
					{
						std::string output;
						STGMEDIUM medium;
						HRESULT errorCode = m_dataObject->GetData(m_oleFormats + a, &medium);
						if (errorCode == S_OK)
						{
							if (medium.tymed == TYMED_HGLOBAL)
							{
								output.assign((char*)GlobalLock(medium.hGlobal), GlobalSize(medium.hGlobal));
								GlobalUnlock(medium.hGlobal);
							}
							else if (medium.tymed == TYMED_ISTREAM)
							{
								STATSTG stats;
								medium.pstm->Stat(&stats, STATFLAG_NONAME);

								output.resize(stats.cbSize.QuadPart);

								ULONG bufferSize = 0;
								medium.pstm->Seek({ 0 }, SEEK_SET, 0);
								HRESULT result = medium.pstm->Read((char*)output.data(), output.size(), &bufferSize);
								if (bufferSize != output.size())
								{
									output.resize(bufferSize);
								}
							}
							ReleaseStgMedium(&medium);
						}
						return output;
					}
				}
			}
		}

		return "";
	}
	uint32 getNumberOfFiles() const 
	{
		return m_numberOfFiles;
	}

	AvoGUI::Image* getImage() const override
	{
		if (m_numberOfFiles)
		{
			std::string file = getFileContents(0);
			return m_gui->getDrawingContext()->createImage(file.data(), file.size());
		}
		return 0;
	}
};

//------------------------------

class OleDropTarget :
	public IDropTarget
{
private:
	uint32 m_referenceCount;

	AvoGUI::Gui* m_gui;

	OleClipboardData m_dropData;
	AvoGUI::DragDropEvent m_dragDropEvent;

	IDropTargetHelper* m_dropImageViewer;
	uint32 m_currentEffect;

public:
	OleDropTarget(AvoGUI::Gui* p_gui) :
		m_referenceCount(1), m_gui(p_gui), m_dropData(p_gui)
	{
		m_dropData.formats.reserve(15);
		m_dragDropEvent.data = &m_dropData;

		CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_dropImageViewer));
	}
	~OleDropTarget()
	{
		m_dropImageViewer->Release();
	}

	//------------------------------

	IUnknownDefinition(IDropTarget)

	//------------------------------

	HRESULT __stdcall DragEnter(IDataObject* p_dataObject, DWORD p_keyState, POINTL p_mousePosition, DWORD* p_effect)
	{
		POINT clientMousePosition = { p_mousePosition.x, p_mousePosition.y };
		m_dropImageViewer->DragEnter((HWND)m_gui->getWindow()->getNativeHandle(), p_dataObject, &clientMousePosition, *p_effect);
		m_dropData.setOleDataObject(p_dataObject);

		//------------------------------

		ScreenToClient((HWND)m_gui->getWindow()->getNativeHandle(), &clientMousePosition);

		float newX = clientMousePosition.x / m_gui->getWindow()->getDipToPixelFactor();
		float newY = clientMousePosition.y / m_gui->getWindow()->getDipToPixelFactor();

		m_dragDropEvent.movementX = newX - m_dragDropEvent.x;
		m_dragDropEvent.movementY = newY - m_dragDropEvent.y;
		m_dragDropEvent.x = newX;
		m_dragDropEvent.y = newY;
		m_dragDropEvent.modifierKeys = convertWindowsKeyStateToModifierKeyFlags(p_keyState);

		m_gui->excludeAnimationThread();
		m_gui->handleGlobalDragDropEnter(m_dragDropEvent);
		switch (m_gui->getGlobalDragDropOperation(m_dragDropEvent))
		{
			case AvoGUI::DragDropOperation::Copy:
				*p_effect = DROPEFFECT_COPY;
				break;
			case AvoGUI::DragDropOperation::Move:
				*p_effect = DROPEFFECT_MOVE;
				break;
			case AvoGUI::DragDropOperation::Link:
				*p_effect = DROPEFFECT_LINK;
				break;
			default:
				*p_effect = DROPEFFECT_NONE;
		}
		m_gui->includeAnimationThread();
		m_currentEffect = *p_effect;

		return S_OK;
	}
	HRESULT __stdcall DragOver(DWORD p_keyState, POINTL p_mousePosition, DWORD* p_effect)
	{
		POINT clientMousePosition = { p_mousePosition.x, p_mousePosition.y };
		m_dropImageViewer->DragOver(&clientMousePosition, *p_effect);

		ScreenToClient((HWND)m_gui->getWindow()->getNativeHandle(), &clientMousePosition);

		float newX = clientMousePosition.x / m_gui->getWindow()->getDipToPixelFactor();
		float newY = clientMousePosition.y / m_gui->getWindow()->getDipToPixelFactor();

		m_dragDropEvent.movementX = newX - m_dragDropEvent.x;
		m_dragDropEvent.movementY = newY - m_dragDropEvent.y;
		m_dragDropEvent.x = newX;
		m_dragDropEvent.y = newY;
		m_dragDropEvent.modifierKeys = convertWindowsKeyStateToModifierKeyFlags(p_keyState);

		m_gui->excludeAnimationThread();
		m_gui->handleGlobalDragDropMove(m_dragDropEvent);
		switch (m_gui->getGlobalDragDropOperation(m_dragDropEvent))
		{
			case AvoGUI::DragDropOperation::Copy:
				*p_effect = DROPEFFECT_COPY;
				break;
			case AvoGUI::DragDropOperation::Move:
				*p_effect = DROPEFFECT_MOVE;
				break;
			case AvoGUI::DragDropOperation::Link:
				*p_effect = DROPEFFECT_LINK;
				break;
			default:
				*p_effect = DROPEFFECT_NONE;
		}
		m_gui->includeAnimationThread();
		m_currentEffect = *p_effect;

		return S_OK;
	}
	HRESULT __stdcall DragLeave()
	{
		m_dropImageViewer->DragLeave();

		POINT clientMousePosition;
		GetCursorPos(&clientMousePosition);
		ScreenToClient((HWND)m_gui->getWindow()->getNativeHandle(), &clientMousePosition);

		float newX = clientMousePosition.x / m_gui->getWindow()->getDipToPixelFactor();
		float newY = clientMousePosition.y / m_gui->getWindow()->getDipToPixelFactor();

		m_dragDropEvent.movementX = newX - m_dragDropEvent.x;
		m_dragDropEvent.movementY = newY - m_dragDropEvent.y;
		m_dragDropEvent.x = newX;
		m_dragDropEvent.y = newY;

		m_gui->excludeAnimationThread();
		m_gui->handleGlobalDragDropLeave(m_dragDropEvent);
		m_gui->includeAnimationThread();

		m_dropData.setOleDataObject(0);
		return S_OK;
	}
	HRESULT __stdcall Drop(IDataObject* p_dataObject, DWORD p_keyState, POINTL p_mousePosition, DWORD* p_effect)
	{
		POINT clientMousePosition = { p_mousePosition.x, p_mousePosition.y };
		ScreenToClient((HWND)m_gui->getWindow()->getNativeHandle(), &clientMousePosition);

		float newX = clientMousePosition.x / m_gui->getWindow()->getDipToPixelFactor();
		float newY = clientMousePosition.y / m_gui->getWindow()->getDipToPixelFactor();

		m_dragDropEvent.movementX = newX - m_dragDropEvent.x;
		m_dragDropEvent.movementY = newY - m_dragDropEvent.y;
		m_dragDropEvent.x = newX;
		m_dragDropEvent.y = newY;
		m_dragDropEvent.modifierKeys = convertWindowsKeyStateToModifierKeyFlags(p_keyState);

		m_gui->excludeAnimationThread();
		m_gui->handleGlobalDragDropFinish(m_dragDropEvent);
		m_gui->includeAnimationThread();

		clientMousePosition.x = p_mousePosition.x;
		clientMousePosition.y = p_mousePosition.y;
		m_dropImageViewer->Drop(p_dataObject, &clientMousePosition, *p_effect);

		m_dropData.setOleDataObject(0);

		return S_OK;
	}
};

//------------------------------

constexpr int WM_APP_CHANGE_SIZE = WM_APP;
constexpr int WM_APP_SET_IS_ENABLED = WM_APP + 1;

class WindowsWindow : public AvoGUI::Window
{
private:
	AvoGUI::Gui* m_gui;

	OleDropSource* m_oleDropSource{ nullptr };
	OleDropTarget* m_oleDropTarget{ nullptr };

	uint32 m_clipboardFormat_fileContents{ 0 };
	uint32 m_clipboardFormat_fileGroupDescriptor{ 0 };

	//------------------------------

	HWND m_windowHandle{ 0 };
	AvoGUI::WindowStyleFlags m_crossPlatformStyles;
	uint32 m_styles{ 0 };
	AvoGUI::WindowState m_state{ AvoGUI::WindowState::Restored };

	//------------------------------

	bool m_hasGottenInitialSizeMessageForCustomBorderWindows{ false };
	bool m_isOpen{ false };
	AvoGUI::Point<int32> m_position;
	AvoGUI::Point<uint32> m_size;
	AvoGUI::Point<uint32> m_minSize;
	AvoGUI::Point<uint32> m_maxSize;

	float m_dipToPixelFactor{ 1.f };

	bool m_isFullscreen{ false };
	RECT m_windowRectBeforeFullscreen;
	bool m_wasWindowMaximizedBeforeFullscreen{ false };

	//------------------------------

	bool m_isMouseOutsideClientArea{ true };
	AvoGUI::Point<int32> m_mousePosition{ -1, -1 };
	HCURSOR m_cursorHandle{ 0 };
	AvoGUI::Cursor m_cursorType{ (AvoGUI::Cursor)-1 };

	//------------------------------

	bool getHasCustomBorder()
	{
		return bool(m_crossPlatformStyles & AvoGUI::WindowStyleFlags::CustomBorder);
	}
	uint32 convertWindowStyleFlagsToWindowsWindowStyleFlags(AvoGUI::WindowStyleFlags p_styleFlags, bool p_hasParent)
	{
		uint32 styles = 0;

		if (p_hasParent || bool(p_styleFlags & AvoGUI::WindowStyleFlags::CustomBorder))
		{
			styles |= WS_POPUP;
		}

		if (!bool(p_styleFlags & AvoGUI::WindowStyleFlags::Invisible))
			styles |= WS_VISIBLE;
		if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::Border) || 
			bool(p_styleFlags & AvoGUI::WindowStyleFlags::CustomBorder))
			styles |= WS_CAPTION | WS_SYSMENU;

		if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::Minimized))
			styles |= WS_MINIMIZE;
		else if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::Maximized))
			styles |= WS_MAXIMIZE;

		if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::MinimizeButton))
			styles |= WS_MINIMIZEBOX;
		if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::MaximizeButton))
			styles |= WS_MAXIMIZEBOX;
		if (bool(p_styleFlags & AvoGUI::WindowStyleFlags::ResizeBorder))
			styles |= WS_THICKFRAME;

		return styles;
	}
	AvoGUI::KeyboardKey convertWindowsDataToKeyboardKey(uint64 p_data)
	{
		switch (p_data)
		{
			case VK_APPS:
				return AvoGUI::KeyboardKey::Menu;
			case VK_BACK:
				return AvoGUI::KeyboardKey::Backspace;
			case VK_CLEAR:
				return AvoGUI::KeyboardKey::Clear;
			case VK_TAB:
				return AvoGUI::KeyboardKey::Tab;
			case VK_RETURN:
				return AvoGUI::KeyboardKey::Return;
			case VK_SHIFT:
				return AvoGUI::KeyboardKey::Shift;
			case VK_CONTROL:
				return AvoGUI::KeyboardKey::Control;
			case VK_MENU:
				return AvoGUI::KeyboardKey::Alt;
			case VK_PAUSE:
				return AvoGUI::KeyboardKey::Pause;
			case VK_CAPITAL:
				return AvoGUI::KeyboardKey::CapsLock;
			case VK_ESCAPE:
				return AvoGUI::KeyboardKey::Escape;
			case VK_SPACE:
				return AvoGUI::KeyboardKey::Spacebar;
			case VK_PRIOR:
				return AvoGUI::KeyboardKey::PageUp;
			case VK_NEXT:
				return AvoGUI::KeyboardKey::PageDown;
			case VK_END:
				return AvoGUI::KeyboardKey::End;
			case VK_HOME:
				return AvoGUI::KeyboardKey::Home;
			case VK_LEFT:
				return AvoGUI::KeyboardKey::Left;
			case VK_RIGHT:
				return AvoGUI::KeyboardKey::Right;
			case VK_UP:
				return AvoGUI::KeyboardKey::Up;
			case VK_DOWN:
				return AvoGUI::KeyboardKey::Down;
			case VK_SNAPSHOT:
				return AvoGUI::KeyboardKey::PrintScreen;
			case VK_INSERT:
				return AvoGUI::KeyboardKey::Insert;
			case VK_DELETE:
				return AvoGUI::KeyboardKey::Delete;
			case VK_HELP:
				return AvoGUI::KeyboardKey::Help;
			case VK_NUMPAD0:
				return AvoGUI::KeyboardKey::Numpad0;
			case VK_NUMPAD1:
				return AvoGUI::KeyboardKey::Numpad1;
			case VK_NUMPAD2:
				return AvoGUI::KeyboardKey::Numpad2;
			case VK_NUMPAD3:
				return AvoGUI::KeyboardKey::Numpad3;
			case VK_NUMPAD4:
				return AvoGUI::KeyboardKey::Numpad4;
			case VK_NUMPAD5:
				return AvoGUI::KeyboardKey::Numpad5;
			case VK_NUMPAD6:
				return AvoGUI::KeyboardKey::Numpad6;
			case VK_NUMPAD7:
				return AvoGUI::KeyboardKey::Numpad7;
			case VK_NUMPAD8:
				return AvoGUI::KeyboardKey::Numpad8;
			case VK_NUMPAD9:
				return AvoGUI::KeyboardKey::Numpad9;
			case VK_ADD:
				return AvoGUI::KeyboardKey::Add;
			case VK_SUBTRACT:
				return AvoGUI::KeyboardKey::Subtract;
			case VK_MULTIPLY:
				return AvoGUI::KeyboardKey::Multiply;
			case VK_DIVIDE:
				return AvoGUI::KeyboardKey::Divide;
			case VK_F1:
				return AvoGUI::KeyboardKey::F1;
			case VK_F2:
				return AvoGUI::KeyboardKey::F2;
			case VK_F3:
				return AvoGUI::KeyboardKey::F3;
			case VK_F4:
				return AvoGUI::KeyboardKey::F4;
			case VK_F5:
				return AvoGUI::KeyboardKey::F5;
			case VK_F6:
				return AvoGUI::KeyboardKey::F6;
			case VK_F7:
				return AvoGUI::KeyboardKey::F7;
			case VK_F8:
				return AvoGUI::KeyboardKey::F8;
			case VK_F9:
				return AvoGUI::KeyboardKey::F9;
			case VK_F10:
				return AvoGUI::KeyboardKey::F10;
			case VK_F11:
				return AvoGUI::KeyboardKey::F11;
			case VK_F12:
				return AvoGUI::KeyboardKey::F12;
			case VK_F13:
				return AvoGUI::KeyboardKey::F13;
			case VK_F14:
				return AvoGUI::KeyboardKey::F14;
			case VK_F15:
				return AvoGUI::KeyboardKey::F15;
			case VK_F16:
				return AvoGUI::KeyboardKey::F16;
			case VK_F17:
				return AvoGUI::KeyboardKey::F17;
			case VK_F18:
				return AvoGUI::KeyboardKey::F18;
			case VK_F19:
				return AvoGUI::KeyboardKey::F19;
			case VK_F20:
				return AvoGUI::KeyboardKey::F20;
			case VK_F21:
				return AvoGUI::KeyboardKey::F21;
			case VK_F22:
				return AvoGUI::KeyboardKey::F22;
			case VK_F23:
				return AvoGUI::KeyboardKey::F23;
			case VK_F24:
				return AvoGUI::KeyboardKey::F24;
			case VK_NUMLOCK:
				return AvoGUI::KeyboardKey::NumLock;
			case 0x30:
				return AvoGUI::KeyboardKey::Number0;
			case 0x31:
				return AvoGUI::KeyboardKey::Number1;
			case 0x32:
				return AvoGUI::KeyboardKey::Number2;
			case 0x33:
				return AvoGUI::KeyboardKey::Number3;
			case 0x34:
				return AvoGUI::KeyboardKey::Number4;
			case 0x35:
				return AvoGUI::KeyboardKey::Number5;
			case 0x36:
				return AvoGUI::KeyboardKey::Number6;
			case 0x37:
				return AvoGUI::KeyboardKey::Number7;
			case 0x38:
				return AvoGUI::KeyboardKey::Number8;
			case 0x39:
				return AvoGUI::KeyboardKey::Number9;
			case 0x41:
				return AvoGUI::KeyboardKey::A;
			case 0x42:
				return AvoGUI::KeyboardKey::B;
			case 0x43:
				return AvoGUI::KeyboardKey::C;
			case 0x44:
				return AvoGUI::KeyboardKey::D;
			case 0x45:
				return AvoGUI::KeyboardKey::E;
			case 0x46:
				return AvoGUI::KeyboardKey::F;
			case 0x47:
				return AvoGUI::KeyboardKey::G;
			case 0x48:
				return AvoGUI::KeyboardKey::H;
			case 0x49:
				return AvoGUI::KeyboardKey::I;
			case 0x4A:
				return AvoGUI::KeyboardKey::J;
			case 0x4B:
				return AvoGUI::KeyboardKey::K;
			case 0x4C:
				return AvoGUI::KeyboardKey::L;
			case 0x4D:
				return AvoGUI::KeyboardKey::M;
			case 0x4E:
				return AvoGUI::KeyboardKey::N;
			case 0x4F:
				return AvoGUI::KeyboardKey::O;
			case 0x50:
				return AvoGUI::KeyboardKey::P;
			case 0x51:
				return AvoGUI::KeyboardKey::Q;
			case 0x52:
				return AvoGUI::KeyboardKey::R;
			case 0x53:
				return AvoGUI::KeyboardKey::S;
			case 0x54:
				return AvoGUI::KeyboardKey::T;
			case 0x55:
				return AvoGUI::KeyboardKey::U;
			case 0x56:
				return AvoGUI::KeyboardKey::V;
			case 0x57:
				return AvoGUI::KeyboardKey::W;
			case 0x58:
				return AvoGUI::KeyboardKey::X;
			case 0x59:
				return AvoGUI::KeyboardKey::Y;
			case 0x5A:
				return AvoGUI::KeyboardKey::Z;
			case VK_OEM_COMMA:
				return AvoGUI::KeyboardKey::Comma;
			case VK_OEM_PERIOD:
				return AvoGUI::KeyboardKey::Period;
			case VK_OEM_PLUS:
				return AvoGUI::KeyboardKey::Plus;
			case VK_OEM_MINUS:
				return AvoGUI::KeyboardKey::Minus;
			case VK_OEM_1:
				return AvoGUI::KeyboardKey::Regional1;
			case VK_OEM_2:
				return AvoGUI::KeyboardKey::Regional2;
			case VK_OEM_3:
				return AvoGUI::KeyboardKey::Regional3;
			case VK_OEM_4:
				return AvoGUI::KeyboardKey::Regional4;
			case VK_OEM_5:
				return AvoGUI::KeyboardKey::Regional5;
			case VK_OEM_6:
				return AvoGUI::KeyboardKey::Regional6;
			case VK_OEM_7:
				return AvoGUI::KeyboardKey::Regional7;
		}
		return AvoGUI::KeyboardKey::None;
	}

	OleDataObject* createStringOleDataObject(char const* p_string, uint32 p_length) const
	{
		FORMATETC format;
		format.cfFormat = CF_UNICODETEXT;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		STGMEDIUM medium;
		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;

		uint32 wideStringLength = AvoGUI::getNumberOfUnitsInUtfConvertedString(p_string);
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, wideStringLength * sizeof(wchar_t));
		AvoGUI::convertUtf8ToUtf16(p_string, p_length == -1 ? -1 : p_length + 1, (wchar_t*)medium.hGlobal, wideStringLength);

		return new OleDataObject(&format, &medium, 1);
	}
	OleDataObject* createStringOleDataObject(wchar_t const* p_string, uint32 p_length) const
	{
		FORMATETC format;
		format.cfFormat = CF_UNICODETEXT;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		STGMEDIUM medium;
		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;

		medium.hGlobal = GlobalAlloc(GMEM_FIXED, (p_length + 1) * sizeof(wchar_t));
		memcpy(medium.hGlobal, p_string, p_length * sizeof(wchar_t));
		*((wchar_t*)medium.hGlobal + p_length) = 0;

		return new OleDataObject(&format, &medium, 1);
	}
	OleDataObject* createImageOleDataObject(AvoGUI::Image* p_image) const
	{
		FORMATETC formats[2];
		STGMEDIUM mediums[2];

		ID2D1Bitmap1* bitmap = (ID2D1Bitmap1*)p_image->getHandle();

		formats[0].cfFormat = m_clipboardFormat_fileContents;
		formats[0].tymed = TYMED_ISTREAM;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;
		formats[0].ptd = 0;

		mediums[0].tymed = TYMED_ISTREAM;
		mediums[0].pUnkForRelease = 0;
		mediums[0].pstm = (IStream*)m_gui->getDrawingContext()->createImageFileDataNativeStream(p_image);

		//------------------------------

		formats[1].cfFormat = m_clipboardFormat_fileGroupDescriptor;
		formats[1].tymed = TYMED_HGLOBAL;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;
		formats[1].ptd = 0;

		mediums[1].tymed = TYMED_HGLOBAL;
		mediums[1].pUnkForRelease = 0;
		mediums[1].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

		FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)mediums[1].hGlobal;
		groupDescriptor->cItems = 1;
		groupDescriptor->fgd[0].dwFlags = FD_UNICODE;
		wcscpy_s(groupDescriptor->fgd[0].cFileName, L"image.png");

		return new OleDataObject(formats, mediums, 2);
	}
	OleDataObject* createFileOleDataObject(char const* p_data, uint32 p_dataSize, std::string const& p_name) const
	{
		FORMATETC formats[3];
		STGMEDIUM mediums[3];

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;
		formats[0].ptd = 0;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].pUnkForRelease = 0;

		uint32 wideStringLength = AvoGUI::getNumberOfUnitsInUtfConvertedString(p_name);
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, wideStringLength * sizeof(wchar_t));
		AvoGUI::convertUtf8ToUtf16(p_name.data(), p_name.size() + 1, (wchar_t*)mediums[0].hGlobal, wideStringLength);

		//------------------------------

		formats[1].cfFormat = m_clipboardFormat_fileContents;
		formats[1].tymed = TYMED_ISTREAM;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;
		formats[1].ptd = 0;

		mediums[1].pUnkForRelease = 0;
		mediums[1].tymed = TYMED_ISTREAM;
		mediums[1].pstm = SHCreateMemStream((BYTE const*)p_data, p_dataSize);

		//------------------------------

		formats[2].cfFormat = m_clipboardFormat_fileGroupDescriptor;
		formats[2].tymed = TYMED_HGLOBAL;
		formats[2].dwAspect = DVASPECT_CONTENT;
		formats[2].lindex = -1;
		formats[2].ptd = 0;

		mediums[2].tymed = TYMED_HGLOBAL;
		mediums[2].pUnkForRelease = 0;
		mediums[2].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

		FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)mediums[2].hGlobal;
		groupDescriptor->cItems = 1;
		groupDescriptor->fgd[0].dwFlags = FD_UNICODE;
		wcsncpy_s(groupDescriptor->fgd[0].cFileName, (wchar_t*)mediums[0].hGlobal, wideStringLength);

		return new OleDataObject(formats, mediums, 3);
	}
	OleDataObject* createFileOleDataObject(char const* p_data, uint32 p_dataSize, std::wstring const& p_name) const
	{
		FORMATETC formats[3];
		STGMEDIUM mediums[3];

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;
		formats[0].ptd = 0;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].pUnkForRelease = 0;

		uint32 wideStringLength = AvoGUI::getNumberOfUnitsInUtfConvertedString(p_name);
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, wideStringLength * sizeof(wchar_t));
		memcpy(mediums[0].hGlobal, p_name.data(), sizeof(wchar_t)*(p_name.size() + 1));

		//------------------------------

		formats[1].cfFormat = m_clipboardFormat_fileContents;
		formats[1].tymed = TYMED_ISTREAM;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;
		formats[1].ptd = 0;

		mediums[1].pUnkForRelease = 0;
		mediums[1].tymed = TYMED_ISTREAM;
		mediums[1].pstm = SHCreateMemStream((BYTE const*)p_data, p_dataSize);

		//------------------------------

		formats[2].cfFormat = m_clipboardFormat_fileGroupDescriptor;
		formats[2].tymed = TYMED_HGLOBAL;
		formats[2].dwAspect = DVASPECT_CONTENT;
		formats[2].lindex = -1;
		formats[2].ptd = 0;

		mediums[2].tymed = TYMED_HGLOBAL;
		mediums[2].pUnkForRelease = 0;
		mediums[2].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

		FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)mediums[2].hGlobal;
		groupDescriptor->cItems = 1;
		groupDescriptor->fgd[0].dwFlags = FD_UNICODE;
		wcsncpy_s(groupDescriptor->fgd[0].cFileName, (wchar_t*)mediums[0].hGlobal, wideStringLength);

		return new OleDataObject(formats, mediums, 3);
	}
	OleDataObject* createFileOleDataObject(std::string const& p_path) const
	{
		std::filesystem::path path(std::filesystem::u8path(p_path));
		std::wstring widePathString = path.wstring();
		uint32 widePathStringSize = (widePathString.size() + 1) * sizeof(wchar_t);

		FORMATETC formats[4];
		STGMEDIUM mediums[4];

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;
		formats[0].ptd = 0;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].pUnkForRelease = 0;
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, widePathStringSize);
		memcpy(mediums[0].hGlobal, widePathString.data(), widePathStringSize);

		//------------------------------

		formats[1].cfFormat = CF_HDROP;
		formats[1].tymed = TYMED_HGLOBAL;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;
		formats[1].ptd = 0;

		mediums[1].tymed = TYMED_HGLOBAL;
		mediums[1].pUnkForRelease = 0;
		mediums[1].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + widePathStringSize + sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)mediums[1].hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		memcpy((char*)mediums[1].hGlobal + sizeof(DROPFILES), widePathString.data(), widePathStringSize);
		*(wchar_t*)((char*)mediums[1].hGlobal + sizeof(DROPFILES) + widePathStringSize) = 0;

		//------------------------------

		OleDataObject* dataObject = 0;
		if (filesystem::is_regular_file(path))
		{
			formats[2].cfFormat = m_clipboardFormat_fileContents;
			formats[2].tymed = TYMED_ISTREAM;
			formats[2].dwAspect = DVASPECT_CONTENT;
			formats[2].lindex = -1;
			formats[2].ptd = 0;

			mediums[2].tymed = TYMED_ISTREAM;
			mediums[2].pUnkForRelease = 0;
			SHCreateStreamOnFileEx(widePathString.data(), STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, 0, &mediums[2].pstm);

			//------------------------------

			formats[3].cfFormat = m_clipboardFormat_fileGroupDescriptor;
			formats[3].tymed = TYMED_HGLOBAL;
			formats[3].dwAspect = DVASPECT_CONTENT;
			formats[3].lindex = -1;
			formats[3].ptd = 0;

			mediums[3].tymed = TYMED_HGLOBAL;
			mediums[3].pUnkForRelease = 0;
			mediums[3].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

			FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)mediums[3].hGlobal;
			groupDescriptor->cItems = 1;
			groupDescriptor->fgd[0].dwFlags = FD_UNICODE;

			std::wstring filename = path.filename().wstring();
			memcpy(groupDescriptor->fgd[0].cFileName, filename.data(), (filename.size() + 1) * sizeof(wchar_t));

			return new OleDataObject(formats, mediums, 4);
		}
		return new OleDataObject(formats, mediums, 2);
	}
	OleDataObject* createFileOleDataObject(std::wstring const& p_path) const
	{
		filesystem::path path(p_path);
		uint32 widePathStringSize = (p_path.size() + 1) * sizeof(wchar_t);

		FORMATETC formats[4];
		STGMEDIUM mediums[4];

		formats[0].cfFormat = CF_UNICODETEXT;
		formats[0].tymed = TYMED_HGLOBAL;
		formats[0].dwAspect = DVASPECT_CONTENT;
		formats[0].lindex = -1;
		formats[0].ptd = 0;

		mediums[0].tymed = TYMED_HGLOBAL;
		mediums[0].pUnkForRelease = 0;
		mediums[0].hGlobal = GlobalAlloc(GMEM_FIXED, widePathStringSize);
		memcpy(mediums[0].hGlobal, p_path.data(), widePathStringSize);

		//------------------------------

		formats[1].cfFormat = CF_HDROP;
		formats[1].tymed = TYMED_HGLOBAL;
		formats[1].dwAspect = DVASPECT_CONTENT;
		formats[1].lindex = -1;
		formats[1].ptd = 0;

		mediums[1].tymed = TYMED_HGLOBAL;
		mediums[1].pUnkForRelease = 0;
		mediums[1].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + widePathStringSize + sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)mediums[1].hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		memcpy((char*)mediums[1].hGlobal + sizeof(DROPFILES), p_path.data(), widePathStringSize);
		*(wchar_t*)((char*)mediums[1].hGlobal + sizeof(DROPFILES) + widePathStringSize) = 0;

		//------------------------------

		OleDataObject* dataObject = 0;
		if (filesystem::is_regular_file(path))
		{
			formats[2].cfFormat = m_clipboardFormat_fileContents;
			formats[2].tymed = TYMED_ISTREAM;
			formats[2].dwAspect = DVASPECT_CONTENT;
			formats[2].lindex = -1;
			formats[2].ptd = 0;

			mediums[2].tymed = TYMED_ISTREAM;
			mediums[2].pUnkForRelease = 0;
			SHCreateStreamOnFileEx(p_path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, 0, &mediums[2].pstm);

			//------------------------------

			formats[3].cfFormat = m_clipboardFormat_fileGroupDescriptor;
			formats[3].tymed = TYMED_HGLOBAL;
			formats[3].dwAspect = DVASPECT_CONTENT;
			formats[3].lindex = -1;
			formats[3].ptd = 0;

			mediums[3].tymed = TYMED_HGLOBAL;
			mediums[3].pUnkForRelease = 0;
			mediums[3].hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(FILEGROUPDESCRIPTORW));

			FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)mediums[3].hGlobal;
			groupDescriptor->cItems = 1;
			groupDescriptor->fgd[0].dwFlags = FD_UNICODE;

			std::wstring filename = path.filename().wstring();
			memcpy(groupDescriptor->fgd[0].cFileName, filename.data(), (filename.size() + 1) * sizeof(wchar_t));

			return new OleDataObject(formats, mediums, 4);
		}
		return new OleDataObject(formats, mediums, 2);
	}
	OleDataObject* createFilesOleDataObject(std::string* p_pathStrings, uint32 p_numberOfPaths) const
	{
		FORMATETC format;
		STGMEDIUM medium;

		//------------------------------
		// Create an HDROP format, which is just the paths of all items.

		format.cfFormat = CF_HDROP;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		std::vector<std::wstring> widePathStrings(p_numberOfPaths);

		uint32 pathsStringSize = 0;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			widePathStrings[a] = AvoGUI::convertUtf8ToUtf16(p_pathStrings[a]);
			pathsStringSize += widePathStrings[a].size() + 1;
		}
		pathsStringSize++;

		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + pathsStringSize * sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)medium.hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		wchar_t* pathsString = (wchar_t*)((char*)medium.hGlobal + sizeof(DROPFILES));
		wchar_t* pathsStringPosition = pathsString;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			memcpy(pathsStringPosition, widePathStrings[a].data(), (widePathStrings[a].size() + 1) * sizeof(wchar_t));
			pathsStringPosition += widePathStrings[a].size() + 1;
		}
		pathsString[pathsStringSize - 1] = 0;

		//------------------------------

		return new OleDataObject(&format, &medium, 1);
	}
	OleDataObject* createFilesOleDataObject(char const* const* p_pathStrings, uint32 p_numberOfPaths) const
	{
		FORMATETC format;
		STGMEDIUM medium;

		//------------------------------
		// Create an HDROP format, which is just the paths of all items.

		format.cfFormat = CF_HDROP;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		std::vector<std::wstring> widePathStrings(p_numberOfPaths);

		uint32 pathsStringSize = 0;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			widePathStrings[a] = AvoGUI::convertUtf8ToUtf16(p_pathStrings[a]);
			pathsStringSize += widePathStrings[a].size() + 1;
		}
		pathsStringSize++;

		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + pathsStringSize * sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)medium.hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		wchar_t* pathsString = (wchar_t*)((char*)medium.hGlobal + sizeof(DROPFILES));
		wchar_t* pathsStringPosition = pathsString;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			memcpy(pathsStringPosition, widePathStrings[a].data(), (widePathStrings[a].size() + 1) * sizeof(wchar_t));
			pathsStringPosition += widePathStrings[a].size() + 1;
		}
		pathsString[pathsStringSize - 1] = 0;

		//------------------------------

		return new OleDataObject(&format, &medium, 1);
	}
	OleDataObject* createFilesOleDataObject(std::wstring* p_pathStrings, uint32 p_numberOfPaths) const
	{
		FORMATETC format;
		STGMEDIUM medium;

		//------------------------------
		// Create an HDROP format, which is just the paths of all items.

		format.cfFormat = CF_HDROP;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		uint32 pathsStringSize = 0;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			pathsStringSize += p_pathStrings[a].size() + 1;
		}
		pathsStringSize++;

		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + pathsStringSize * sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)medium.hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		wchar_t* pathsString = (wchar_t*)((char*)medium.hGlobal + sizeof(DROPFILES));
		wchar_t* pathsStringPosition = pathsString;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			memcpy(pathsStringPosition, p_pathStrings[a].data(), (p_pathStrings[a].size() + 1) * sizeof(wchar_t));
			pathsStringPosition += p_pathStrings[a].size() + 1;
		}
		pathsString[pathsStringSize - 1] = 0;

		//------------------------------

		return new OleDataObject(&format, &medium, 1);
	}
	OleDataObject* createFilesOleDataObject(wchar_t const* const* p_pathStrings, uint32 p_numberOfPaths) const
	{
		FORMATETC format;
		STGMEDIUM medium;

		//------------------------------
		// Create an HDROP format, which is just the paths of all items.

		format.cfFormat = CF_HDROP;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.ptd = 0;

		std::vector<std::wstring> widePathStrings(p_numberOfPaths);

		uint32 pathsStringSize = 0;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			widePathStrings[a] = p_pathStrings[a];
			pathsStringSize += widePathStrings[a].size() + 1;
		}
		pathsStringSize++;

		medium.tymed = TYMED_HGLOBAL;
		medium.pUnkForRelease = 0;
		medium.hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(DROPFILES) + pathsStringSize * sizeof(wchar_t));

		DROPFILES* filenameStructure = (DROPFILES*)medium.hGlobal;
		filenameStructure->fNC = true;
		filenameStructure->fWide = true;
		filenameStructure->pt.x = 0;
		filenameStructure->pt.y = 0;
		filenameStructure->pFiles = sizeof(DROPFILES);

		wchar_t* pathsString = (wchar_t*)((char*)medium.hGlobal + sizeof(DROPFILES));
		wchar_t* pathsStringPosition = pathsString;
		for (uint32 a = 0; a < p_numberOfPaths; a++)
		{
			memcpy(pathsStringPosition, widePathStrings[a].data(), (widePathStrings[a].size() + 1) * sizeof(wchar_t));
			pathsStringPosition += widePathStrings[a].size() + 1;
		}
		pathsString[pathsStringSize - 1] = 0;

		//------------------------------

		return new OleDataObject(&format, &medium, 1);
	}

	uint32 doDragDrop(OleDataObject* p_dataObject, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		AvoGUI::Point<uint32> mousePositionBefore(m_mousePosition);

		AvoGUI::MouseEvent event;
		event.x = m_mousePosition.x / m_dipToPixelFactor;
		event.y = m_mousePosition.y / m_dipToPixelFactor;
		if (getIsMouseButtonDown(AvoGUI::MouseButton::Left))
		{
			event.mouseButton = AvoGUI::MouseButton::Left;
		}
		else if (getIsMouseButtonDown(AvoGUI::MouseButton::Middle))
		{
			event.mouseButton = AvoGUI::MouseButton::Middle;
		}
		else if (getIsMouseButtonDown(AvoGUI::MouseButton::Right))
		{
			event.mouseButton = AvoGUI::MouseButton::Right;
		}
		else if (getIsMouseButtonDown(AvoGUI::MouseButton::X0))
		{
			event.mouseButton = AvoGUI::MouseButton::X0;
		}
		else if (getIsMouseButtonDown(AvoGUI::MouseButton::X1))
		{
			event.mouseButton = AvoGUI::MouseButton::X1;
		}
		if (event.mouseButton != AvoGUI::MouseButton::None)
		{
			m_gui->handleGlobalMouseUp(event);
		}

		if (p_dragImage)
		{
			m_oleDropSource->setDragImage(p_dragImage, p_dragImageCursorPosition, p_dataObject);
		}

		m_gui->includeAnimationThread();
		DWORD dropOperation = DROPEFFECT_NONE;
		DoDragDrop(p_dataObject, m_oleDropSource, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK, &dropOperation);
		m_gui->excludeAnimationThread();

		event.x = m_mousePosition.x / m_dipToPixelFactor;
		event.y = m_mousePosition.y / m_dipToPixelFactor;
		event.movementX = (m_mousePosition.x - mousePositionBefore.x) / m_dipToPixelFactor;
		event.movementY = (m_mousePosition.y - mousePositionBefore.y) / m_dipToPixelFactor;
		event.mouseButton = AvoGUI::MouseButton::None;
		m_gui->handleGlobalMouseMove(event);

		return dropOperation;
	}

	//------------------------------

	bool m_hasCreatedWindow = false;
	std::condition_variable m_hasCreatedWindowConditionVariable;
	std::mutex m_hasCreatedWindowMutex;
	std::thread m_messageThread;
	void thread_createAndRun(char const* p_title, uint32 p_titleSize, float p_x, float p_y, float p_width, float p_height, AvoGUI::Window* p_parent)
	{
		if (m_windowHandle)
		{
			DestroyWindow(m_windowHandle);
			s_numberOfWindows--;
		}
		else if (!s_numberOfWindows)
		{
			WNDCLASSW windowClass = { };
			windowClass.lpszClassName = WINDOW_CLASS_NAME;
			windowClass.hInstance = GetModuleHandle(0);
			windowClass.lpfnWndProc = handleGlobalEvents;
			windowClass.hbrBackground = (HBRUSH)0;
			windowClass.hCursor = 0;
			windowClass.style = CS_DBLCLKS;
			windowClass.hIcon = 0;
			windowClass.cbClsExtra = 0;

			RegisterClassW(&windowClass);
		}

		//------------------------------

		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		m_dipToPixelFactor = GetDpiForSystem() / (float)USER_DEFAULT_SCREEN_DPI;

		// Calculate nonclient window rectangle from client size.
		RECT windowRect = { 0, 0, (int)std::ceil(p_width * m_dipToPixelFactor), (int)std::ceil(p_height * m_dipToPixelFactor) };
		m_size.set(windowRect.right, windowRect.bottom); // Client area
		
		AdjustWindowRect(&windowRect, m_styles, 0);

		//------------------------------

		POINT cursorPosition = { 0 };
		GetCursorPos(&cursorPosition);
		m_mousePosition.set(cursorPosition.x, cursorPosition.y);

		RECT parentRect = { 0 };

		if (p_parent)
		{
			GetWindowRect((HWND)p_parent->getNativeHandle(), &parentRect);
		}
		else
		{
			HMONITOR monitor = MonitorFromPoint(cursorPosition, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &monitorInfo);
			parentRect = monitorInfo.rcWork;
		}

		//------------------------------

		std::wstring wideTitle = AvoGUI::convertUtf8ToUtf16(p_title, p_titleSize);

		// m_windowHandle is initialized by the WM_CREATE event, before CreateWindow returns.

		if (getHasCustomBorder())
		{
			CreateWindowW(
				WINDOW_CLASS_NAME,
				wideTitle.c_str(),
				m_styles,
				0, 0,
				0, 0,
				p_parent ? (HWND)p_parent->getNativeHandle() : 0,
				0, // No menu
				GetModuleHandle(0),
				this // Additional window data - the instance
			);

			/*
				Remove border, can't do that just with styles without losing the window shadow.
			*/
			MARGINS margins = { 0, 0, 0, 1 };
			DwmExtendFrameIntoClientArea(m_windowHandle, &margins);

			SetWindowPos(
				m_windowHandle, nullptr,
				parentRect.left + windowRect.left + p_x * (parentRect.right - parentRect.left - m_size.x),
				parentRect.top + windowRect.top + p_y * (parentRect.bottom - parentRect.top - m_size.y),
				m_size.x,
				m_size.y,
				SWP_FRAMECHANGED | SWP_NOZORDER
			);
		}
		else
		{
			CreateWindowW(
				WINDOW_CLASS_NAME,
				wideTitle.c_str(),
				m_styles,
				parentRect.left + windowRect.left + p_x * (parentRect.right - parentRect.left - windowRect.right + windowRect.left),
				parentRect.top + windowRect.top + p_y * (parentRect.bottom - parentRect.top - windowRect.bottom + windowRect.top),
				windowRect.right - windowRect.left,
				windowRect.bottom - windowRect.top,
				p_parent ? (HWND)p_parent->getNativeHandle() : 0,
				0, // No menu
				GetModuleHandle(0),
				this // Additional window data - the instance
			);
		}
		s_numberOfWindows++;

		MSG message = { };
		while (GetMessage(&message, 0, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

public:
	static std::atomic<uint32> s_numberOfWindows;
	static wchar_t const* const WINDOW_CLASS_NAME;

	//------------------------------

	WindowsWindow(AvoGUI::Gui* p_gui) :
		m_gui(p_gui), m_crossPlatformStyles((AvoGUI::WindowStyleFlags)0)
	{
		setCursor(AvoGUI::Cursor::Arrow);
	}
	WindowsWindow(AvoGUI::Gui* p_gui, char const* p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) :
		m_gui(p_gui), m_crossPlatformStyles(p_styleFlags)
	{
		create(p_title, p_width, p_height, p_styleFlags, p_parent);

		setCursor(AvoGUI::Cursor::Arrow);
	}
	~WindowsWindow()
	{
		m_messageThread.join();
		DestroyCursor(m_cursorHandle);
	}

	//------------------------------

	void create(char const* p_title, uint32 p_titleSize, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		m_crossPlatformStyles = p_styleFlags;
		m_styles = convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styleFlags, p_parent);
		
		m_messageThread = std::thread(&WindowsWindow::thread_createAndRun, this, p_title, p_titleSize, p_x, p_y, p_width, p_height, p_parent);
		if (!m_hasCreatedWindow)
		{
			std::unique_lock<std::mutex> lock(m_hasCreatedWindowMutex);
			m_hasCreatedWindowConditionVariable.wait(lock, [=] { return (bool)m_hasCreatedWindow; });
		}
	}
	void create(char const* p_title, uint32 p_titleSize, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		create(p_title, p_titleSize, 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(char const* p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		create(p_title, strlen(p_title), p_x, p_y, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(char const* p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		create(p_title, strlen(p_title), 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(std::string const& p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		create(p_title.data(), p_title.size(), p_x, p_y, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(std::string const& p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = 0) override
	{
		create(p_title.data(), p_title.size(), 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}

	void close() override
	{
		if (m_windowHandle)
		{
			SendMessage(m_windowHandle, WM_CLOSE, 0, 0);
		}
	}
	bool getIsOpen() const override
	{
		return m_isOpen;
	}

	//------------------------------

	void enableUserInteraction() override
	{
		PostMessage(m_windowHandle, WM_APP_SET_IS_ENABLED, 1, 0);
	}
	void disableUserInteraction() override
	{
		PostMessage(m_windowHandle, WM_APP_SET_IS_ENABLED, 0, 0);
	}
	bool getIsUserInteractionEnabled() override
	{
		return IsWindowEnabled(m_windowHandle);
	}

	//------------------------------

	void setTitle(char const* p_title, uint32 p_titleSize) override
	{
		std::wstring wideTitle = AvoGUI::convertUtf8ToUtf16(p_title, p_titleSize);
		SetWindowTextW(m_windowHandle, wideTitle.c_str());
	}
	void setTitle(char const* p_title) override
	{
		std::wstring wideTitle = AvoGUI::convertUtf8ToUtf16(p_title);
		SetWindowTextW(m_windowHandle, wideTitle.c_str());
	}
	void setTitle(std::string const& p_title) override
	{
		std::wstring wideTitle = AvoGUI::convertUtf8ToUtf16(p_title);
		SetWindowTextW(m_windowHandle, wideTitle.c_str());
	}
	std::string getTitle() const override
	{
		std::wstring wideTitle;
		wideTitle.resize(150);
		GetWindowTextW(m_windowHandle, (wchar_t*)wideTitle.data(), wideTitle.size());
		return AvoGUI::convertUtf16ToUtf8(wideTitle);
	}

	//------------------------------

	void setStyles(AvoGUI::WindowStyleFlags p_styles) override
	{
		SetWindowLongPtr(m_windowHandle, GWL_STYLE, convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styles, GetParent(m_windowHandle)));
		SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
	AvoGUI::WindowStyleFlags getStyles() const override
	{
		return m_crossPlatformStyles;
	}

	//------------------------------

	// Internal method used to initialize the window handle at the right moment.
	void setNativeHandle(HWND p_handle)
	{
		m_windowHandle = p_handle;
	}
	void* getNativeHandle() const override
	{
		return m_windowHandle;
	}

	//------------------------------

	void setIsFullscreen(bool p_isFullscreen) override
	{
		if (m_isFullscreen == p_isFullscreen)
		{
			return;
		}
		if (p_isFullscreen)
		{
			m_wasWindowMaximizedBeforeFullscreen = false;
			if (m_state == AvoGUI::WindowState::Restored)
			{
				GetWindowRect(m_windowHandle, &m_windowRectBeforeFullscreen);
			}
			else if (m_state == AvoGUI::WindowState::Maximized)
			{
				m_wasWindowMaximizedBeforeFullscreen = true;
			}

			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_VISIBLE | WS_MAXIMIZE);
			SetWindowPos(
				m_windowHandle, 0, 
				info.rcMonitor.left, info.rcMonitor.top, 
				info.rcMonitor.right - info.rcMonitor.left, 
				info.rcMonitor.bottom - info.rcMonitor.top, 
				SWP_NOZORDER | SWP_NOOWNERZORDER
			);
		}
		else
		{
			SetWindowLongPtr(m_windowHandle, GWL_STYLE, m_wasWindowMaximizedBeforeFullscreen * WS_MAXIMIZE | m_styles & ~(WS_MAXIMIZE | WS_MINIMIZE));
			if (m_wasWindowMaximizedBeforeFullscreen)
			{
				SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
			}
			else
			{
				SetWindowPos(
					m_windowHandle, 0, 
					m_windowRectBeforeFullscreen.left, m_windowRectBeforeFullscreen.top, 
					m_windowRectBeforeFullscreen.right - m_windowRectBeforeFullscreen.left, 
					m_windowRectBeforeFullscreen.bottom - m_windowRectBeforeFullscreen.top, 
					SWP_NOZORDER | SWP_NOOWNERZORDER
				);
			}
		}
		m_isFullscreen = p_isFullscreen;
	}
	void switchFullscreen() override
	{
		setIsFullscreen(!m_isFullscreen);
	}
	bool getIsFullscreen() const override
	{
		return m_isFullscreen;
	}

	//------------------------------

	void hide() override
	{
		ShowWindow(m_windowHandle, SW_HIDE);
	}
	void show() override
	{
		ShowWindow(m_windowHandle, SW_SHOW);
	}

	void maximize() override
	{
		ShowWindow(m_windowHandle, SW_MAXIMIZE);
	}
	void minimize() override
	{
		ShowWindow(m_windowHandle, SW_MINIMIZE);
	}
	void restore() override
	{
		ShowWindow(m_windowHandle, SW_RESTORE);
	}

	void setState(AvoGUI::WindowState p_state) override
	{
		if (p_state == AvoGUI::WindowState::Maximized)
			ShowWindow(m_windowHandle, SW_MAXIMIZE);
		else if (p_state == AvoGUI::WindowState::Minimized)
			ShowWindow(m_windowHandle, SW_MINIMIZE);
		else if (p_state == AvoGUI::WindowState::Restored)
			ShowWindow(m_windowHandle, SW_RESTORE);
	}
	AvoGUI::WindowState getState() const override
	{
		return m_state;
	}

	//------------------------------

	void setPosition(AvoGUI::Point<int32> const& p_position) override
	{
		setPosition(p_position.x, p_position.y);
	}
	void setPosition(int32 p_x, int32 p_y) override
	{
		SetWindowPos(m_windowHandle, 0, p_x, p_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		m_position.set(p_x, p_y);
	}
	AvoGUI::Point<int32> getPosition() const override
	{
		return m_position;
	}
	int32 getPositionX() const override
	{
		return m_position.x;
	}
	int32 getPositionY() const override
	{
		return m_position.y;
	}

	void setSize(AvoGUI::Point<float> const& p_size) override
	{
		setSize(p_size.x, p_size.y);
	}
	void setSize(float p_width, float p_height) override
	{
		if (m_windowHandle)
		{
			if (getHasCustomBorder())
			{
				PostMessage(m_windowHandle, WM_APP_CHANGE_SIZE, std::ceil(p_width*m_dipToPixelFactor), std::ceil(p_height*m_dipToPixelFactor));
			}
			else
			{
				RECT windowRect = { 0, 0, (int)std::ceil(p_width * m_dipToPixelFactor), (int)std::ceil(p_height * m_dipToPixelFactor) };
				AdjustWindowRect(&windowRect, m_styles, 0);
			
				PostMessage(m_windowHandle, WM_APP_CHANGE_SIZE, uint32(windowRect.right - windowRect.left), uint32(windowRect.bottom - windowRect.top));
			}
		}
	}

	AvoGUI::Point<float> getSize() const override
	{
		return m_size / m_dipToPixelFactor;
	}
	float getWidth() const override
	{
		return m_size.x / m_dipToPixelFactor;
	}
	float getHeight() const override
	{
		return m_size.y / m_dipToPixelFactor;
	}

	//------------------------------

	void setMinSize(AvoGUI::Point<float> const& p_minSize) override
	{
		m_minSize = p_minSize*m_dipToPixelFactor;
	}
	void setMinSize(float p_minWidth, float p_minHeight) override
	{
		m_minSize.x = p_minWidth * m_dipToPixelFactor;
		m_minSize.y = p_minHeight * m_dipToPixelFactor;
	}
	AvoGUI::Point<float> getMinSize() const override
	{
		return m_minSize/m_dipToPixelFactor;
	}
	float getMinWidth() const override
	{
		return m_minSize.x/ m_dipToPixelFactor;
	}
	float getMinHeight() const override
	{
		return m_minSize.y/ m_dipToPixelFactor;
	}

	void setMaxSize(AvoGUI::Point<float> const& p_maxSize) override
	{
		m_maxSize = p_maxSize* m_dipToPixelFactor;
	}
	void setMaxSize(float p_maxWidth, float p_maxHeight) override
	{
		m_maxSize.x = p_maxWidth* m_dipToPixelFactor;
		m_maxSize.y = p_maxHeight* m_dipToPixelFactor;
	}
	AvoGUI::Point<float> getMaxSize() const override
	{
		return m_maxSize/ m_dipToPixelFactor;
	}
	float getMaxWidth() const override
	{
		return m_maxSize.x/ m_dipToPixelFactor;
	}
	float getMaxHeight() const override
	{
		return m_maxSize.y/ m_dipToPixelFactor;
	}

	//------------------------------

	AvoGUI::Rectangle<uint32> getMonitorBounds() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
		return AvoGUI::Rectangle<uint32>(info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom);
	}
	AvoGUI::Point<uint32> getMonitorPosition() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
		return AvoGUI::Point<uint32>(info.rcMonitor.left, info.rcMonitor.top);
	}
	AvoGUI::Point<uint32> getMonitorSize() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return AvoGUI::Point<uint32>(info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top);
	}
	uint32 getMonitorWidth() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return info.rcMonitor.right - info.rcMonitor.left;
	}
	uint32 getMonitorHeight() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return info.rcMonitor.bottom - info.rcMonitor.top;
	}

	//------------------------------

	AvoGUI::Rectangle<uint32> getWorkAreaBounds() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
		return AvoGUI::Rectangle<uint32>(info.rcWork.left, info.rcWork.top, info.rcWork.right, info.rcWork.bottom);
	}
	AvoGUI::Point<uint32> getWorkAreaPosition() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
		return AvoGUI::Point<uint32>(info.rcWork.left, info.rcWork.top);
	}
	AvoGUI::Point<uint32> getWorkAreaSize() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return AvoGUI::Point<uint32>(info.rcWork.right - info.rcWork.left, info.rcWork.bottom - info.rcWork.top);
	}
	uint32 getWorkAreaWidth() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return info.rcWork.right - info.rcWork.left;
	}
	uint32 getWorkAreaHeight() const override
	{
		MONITORINFO info = { };
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
		return info.rcWork.bottom - info.rcWork.top;
	}

	//------------------------------

	bool getIsKeyDown(AvoGUI::KeyboardKey p_key) const override
	{
		switch (p_key)
		{
			case AvoGUI::KeyboardKey::A:
				return GetAsyncKeyState(0x41) & (1 << 16);
			case AvoGUI::KeyboardKey::B:
				return GetAsyncKeyState(0x42) & (1 << 16);
			case AvoGUI::KeyboardKey::C:
				return GetAsyncKeyState(0x43) & (1 << 16);
			case AvoGUI::KeyboardKey::D:
				return GetAsyncKeyState(0x44) & (1 << 16);
			case AvoGUI::KeyboardKey::E:
				return GetAsyncKeyState(0x45) & (1 << 16);
			case AvoGUI::KeyboardKey::F:
				return GetAsyncKeyState(0x46) & (1 << 16);
			case AvoGUI::KeyboardKey::G:
				return GetAsyncKeyState(0x47) & (1 << 16);
			case AvoGUI::KeyboardKey::H:
				return GetAsyncKeyState(0x48) & (1 << 16);
			case AvoGUI::KeyboardKey::I:
				return GetAsyncKeyState(0x49) & (1 << 16);
			case AvoGUI::KeyboardKey::J:
				return GetAsyncKeyState(0x4A) & (1 << 16);
			case AvoGUI::KeyboardKey::K:
				return GetAsyncKeyState(0x4B) & (1 << 16);
			case AvoGUI::KeyboardKey::L:
				return GetAsyncKeyState(0x4C) & (1 << 16);
			case AvoGUI::KeyboardKey::M:
				return GetAsyncKeyState(0x4D) & (1 << 16);
			case AvoGUI::KeyboardKey::N:
				return GetAsyncKeyState(0x4E) & (1 << 16);
			case AvoGUI::KeyboardKey::O:
				return GetAsyncKeyState(0x4F) & (1 << 16);
			case AvoGUI::KeyboardKey::P:
				return GetAsyncKeyState(0x50) & (1 << 16);
			case AvoGUI::KeyboardKey::Q:
				return GetAsyncKeyState(0x51) & (1 << 16);
			case AvoGUI::KeyboardKey::R:
				return GetAsyncKeyState(0x52) & (1 << 16);
			case AvoGUI::KeyboardKey::S:
				return GetAsyncKeyState(0x53) & (1 << 16);
			case AvoGUI::KeyboardKey::T:
				return GetAsyncKeyState(0x54) & (1 << 16);
			case AvoGUI::KeyboardKey::U:
				return GetAsyncKeyState(0x55) & (1 << 16);
			case AvoGUI::KeyboardKey::V:
				return GetAsyncKeyState(0x56) & (1 << 16);
			case AvoGUI::KeyboardKey::W:
				return GetAsyncKeyState(0x57) & (1 << 16);
			case AvoGUI::KeyboardKey::X:
				return GetAsyncKeyState(0x58) & (1 << 16);
			case AvoGUI::KeyboardKey::Y:
				return GetAsyncKeyState(0x59) & (1 << 16);
			case AvoGUI::KeyboardKey::Z:
				return GetAsyncKeyState(0x5A) & (1 << 16);
			case AvoGUI::KeyboardKey::Alt:
				return GetAsyncKeyState(VK_MENU) & (1 << 16);
			case AvoGUI::KeyboardKey::Backspace:
				return GetAsyncKeyState(VK_BACK) & (1 << 16);
			case AvoGUI::KeyboardKey::CapsLock:
				return GetAsyncKeyState(VK_CAPITAL) & (1 << 16);
			case AvoGUI::KeyboardKey::Clear:
				return GetAsyncKeyState(VK_CLEAR) & (1 << 16);
			case AvoGUI::KeyboardKey::Control:
				return GetAsyncKeyState(VK_CONTROL) & (1 << 16);
			case AvoGUI::KeyboardKey::Decimal:
				return GetAsyncKeyState(VK_DECIMAL) & (1 << 16);
			case AvoGUI::KeyboardKey::Delete:
				return GetAsyncKeyState(VK_DELETE) & (1 << 16);
			case AvoGUI::KeyboardKey::Down:
				return GetAsyncKeyState(VK_DOWN) & (1 << 16);
			case AvoGUI::KeyboardKey::End:
				return GetAsyncKeyState(VK_END) & (1 << 16);
			case AvoGUI::KeyboardKey::Enter:
				return GetAsyncKeyState(VK_RETURN) & (1 << 16);
			case AvoGUI::KeyboardKey::Escape:
				return GetAsyncKeyState(VK_ESCAPE) & (1 << 16);
			case AvoGUI::KeyboardKey::F1:
				return GetAsyncKeyState(VK_F1) & (1 << 16);
			case AvoGUI::KeyboardKey::F2:
				return GetAsyncKeyState(VK_F2) & (1 << 16);
			case AvoGUI::KeyboardKey::F3:
				return GetAsyncKeyState(VK_F3) & (1 << 16);
			case AvoGUI::KeyboardKey::F4:
				return GetAsyncKeyState(VK_F4) & (1 << 16);
			case AvoGUI::KeyboardKey::F5:
				return GetAsyncKeyState(VK_F5) & (1 << 16);
			case AvoGUI::KeyboardKey::F6:
				return GetAsyncKeyState(VK_F6) & (1 << 16);
			case AvoGUI::KeyboardKey::F7:
				return GetAsyncKeyState(VK_F7) & (1 << 16);
			case AvoGUI::KeyboardKey::F8:
				return GetAsyncKeyState(VK_F8) & (1 << 16);
			case AvoGUI::KeyboardKey::F9:
				return GetAsyncKeyState(VK_F9) & (1 << 16);
			case AvoGUI::KeyboardKey::F10:
				return GetAsyncKeyState(VK_F10) & (1 << 16);
			case AvoGUI::KeyboardKey::F11:
				return GetAsyncKeyState(VK_F11) & (1 << 16);
			case AvoGUI::KeyboardKey::F12:
				return GetAsyncKeyState(VK_F12) & (1 << 16);
			case AvoGUI::KeyboardKey::F13:
				return GetAsyncKeyState(VK_F13) & (1 << 16);
			case AvoGUI::KeyboardKey::F14:
				return GetAsyncKeyState(VK_F14) & (1 << 16);
			case AvoGUI::KeyboardKey::F15:
				return GetAsyncKeyState(VK_F15) & (1 << 16);
			case AvoGUI::KeyboardKey::F16:
				return GetAsyncKeyState(VK_F16) & (1 << 16);
			case AvoGUI::KeyboardKey::F17:
				return GetAsyncKeyState(VK_F17) & (1 << 16);
			case AvoGUI::KeyboardKey::F18:
				return GetAsyncKeyState(VK_F18) & (1 << 16);
			case AvoGUI::KeyboardKey::F19:
				return GetAsyncKeyState(VK_F19) & (1 << 16);
			case AvoGUI::KeyboardKey::F20:
				return GetAsyncKeyState(VK_F20) & (1 << 16);
			case AvoGUI::KeyboardKey::F21:
				return GetAsyncKeyState(VK_F21) & (1 << 16);
			case AvoGUI::KeyboardKey::F22:
				return GetAsyncKeyState(VK_F22) & (1 << 16);
			case AvoGUI::KeyboardKey::F23:
				return GetAsyncKeyState(VK_F23) & (1 << 16);
			case AvoGUI::KeyboardKey::F24:
				return GetAsyncKeyState(VK_F24) & (1 << 16);
			case AvoGUI::KeyboardKey::Help:
				return GetAsyncKeyState(VK_HELP) & (1 << 16);
			case AvoGUI::KeyboardKey::Home:
				return GetAsyncKeyState(VK_HOME) & (1 << 16);
			case AvoGUI::KeyboardKey::Insert:
				return GetAsyncKeyState(VK_INSERT) & (1 << 16);
			case AvoGUI::KeyboardKey::Left:
				return GetAsyncKeyState(VK_LEFT) & (1 << 16);
			case AvoGUI::KeyboardKey::Menu:
				return GetAsyncKeyState(VK_APPS) & (1 << 16);
			case AvoGUI::KeyboardKey::Number0:
				return GetAsyncKeyState(0x30) & (1 << 16);
			case AvoGUI::KeyboardKey::Number1:
				return GetAsyncKeyState(0x31) & (1 << 16);
			case AvoGUI::KeyboardKey::Number2:
				return GetAsyncKeyState(0x32) & (1 << 16);
			case AvoGUI::KeyboardKey::Number3:
				return GetAsyncKeyState(0x33) & (1 << 16);
			case AvoGUI::KeyboardKey::Number4:
				return GetAsyncKeyState(0x34) & (1 << 16);
			case AvoGUI::KeyboardKey::Number5:
				return GetAsyncKeyState(0x35) & (1 << 16);
			case AvoGUI::KeyboardKey::Number6:
				return GetAsyncKeyState(0x36) & (1 << 16);
			case AvoGUI::KeyboardKey::Number7:
				return GetAsyncKeyState(0x37) & (1 << 16);
			case AvoGUI::KeyboardKey::Number8:
				return GetAsyncKeyState(0x38) & (1 << 16);
			case AvoGUI::KeyboardKey::Number9:
				return GetAsyncKeyState(0x39) & (1 << 16);
			case AvoGUI::KeyboardKey::NumLock:
				return GetAsyncKeyState(VK_NUMLOCK) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad0:
				return GetAsyncKeyState(VK_NUMPAD0) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad1:
				return GetAsyncKeyState(VK_NUMPAD1) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad2:
				return GetAsyncKeyState(VK_NUMPAD2) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad3:
				return GetAsyncKeyState(VK_NUMPAD3) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad4:
				return GetAsyncKeyState(VK_NUMPAD4) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad5:
				return GetAsyncKeyState(VK_NUMPAD5) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad6:
				return GetAsyncKeyState(VK_NUMPAD6) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad7:
				return GetAsyncKeyState(VK_NUMPAD7) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad8:
				return GetAsyncKeyState(VK_NUMPAD8) & (1 << 16);
			case AvoGUI::KeyboardKey::Numpad9:
				return GetAsyncKeyState(VK_NUMPAD9) & (1 << 16);
			case AvoGUI::KeyboardKey::Add:
				return GetAsyncKeyState(VK_ADD) & (1 << 16);
			case AvoGUI::KeyboardKey::Subtract:
				return GetAsyncKeyState(VK_SUBTRACT) & (1 << 16);
			case AvoGUI::KeyboardKey::Multiply:
				return GetAsyncKeyState(VK_MULTIPLY) & (1 << 16);
			case AvoGUI::KeyboardKey::Divide:
				return GetAsyncKeyState(VK_DIVIDE) & (1 << 16);
			case AvoGUI::KeyboardKey::PageDown:
				return GetAsyncKeyState(VK_NEXT) & (1 << 16);
			case AvoGUI::KeyboardKey::PageUp:
				return GetAsyncKeyState(VK_PRIOR) & (1 << 16);
			case AvoGUI::KeyboardKey::Pause:
				return GetAsyncKeyState(VK_PAUSE) & (1 << 16);
			case AvoGUI::KeyboardKey::PrintScreen:
				return GetAsyncKeyState(VK_SNAPSHOT) & (1 << 16);
			case AvoGUI::KeyboardKey::Comma:
				return GetAsyncKeyState(VK_OEM_COMMA) & (1 << 16);
			case AvoGUI::KeyboardKey::Period:
				return GetAsyncKeyState(VK_OEM_PERIOD) & (1 << 16);
			case AvoGUI::KeyboardKey::Plus:
				return GetAsyncKeyState(VK_OEM_PLUS) & (1 << 16);
			case AvoGUI::KeyboardKey::Minus:
				return GetAsyncKeyState(VK_OEM_MINUS) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional1:
				return GetAsyncKeyState(VK_OEM_1) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional2:
				return GetAsyncKeyState(VK_OEM_2) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional3:
				return GetAsyncKeyState(VK_OEM_3) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional4:
				return GetAsyncKeyState(VK_OEM_4) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional5:
				return GetAsyncKeyState(VK_OEM_5) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional6:
				return GetAsyncKeyState(VK_OEM_6) & (1 << 16);
			case AvoGUI::KeyboardKey::Regional7:
				return GetAsyncKeyState(VK_OEM_7) & (1 << 16);
			case AvoGUI::KeyboardKey::Right:
				return GetAsyncKeyState(VK_RIGHT) & (1 << 16);
			case AvoGUI::KeyboardKey::Separator:
				return GetAsyncKeyState(VK_SEPARATOR) & (1 << 16);
			case AvoGUI::KeyboardKey::Shift:
				return GetAsyncKeyState(VK_SHIFT) & (1 << 16);
			case AvoGUI::KeyboardKey::Spacebar:
				return GetAsyncKeyState(VK_SPACE) & (1 << 16);
			case AvoGUI::KeyboardKey::Tab:
				return GetAsyncKeyState(VK_TAB) & (1 << 16);
			case AvoGUI::KeyboardKey::Up:
				return GetAsyncKeyState(VK_UP) & (1 << 16);
		}
		return false;
	}
	bool getIsMouseButtonDown(AvoGUI::MouseButton p_button) const override
	{
		switch (p_button)
		{
			case AvoGUI::MouseButton::Left:
				return GetAsyncKeyState(VK_LBUTTON) & (1 << 16);
			case AvoGUI::MouseButton::Middle:
				return GetAsyncKeyState(VK_MBUTTON) & (1 << 16);
			case AvoGUI::MouseButton::Right:
				return GetAsyncKeyState(VK_RBUTTON) & (1 << 16);
			case AvoGUI::MouseButton::X0:
				return GetAsyncKeyState(VK_XBUTTON1) & (1 << 16);
			case AvoGUI::MouseButton::X1:
				return GetAsyncKeyState(VK_XBUTTON2) & (1 << 16);
		}
		return false;
	}
	AvoGUI::Point<float> getMousePosition() const override
	{
		return m_mousePosition / m_dipToPixelFactor;
	}

	//------------------------------

	void setCursor(AvoGUI::Cursor p_cursor) override
	{
		if (p_cursor == m_cursorType) return;

		wchar_t const* name = 0;
		switch (p_cursor)
		{
			case AvoGUI::Cursor::Arrow:
				name = (wchar_t const*)IDC_ARROW;
				break;
			case AvoGUI::Cursor::Blocked:
				name = (wchar_t const*)IDC_NO;
				break;
			case AvoGUI::Cursor::Hand:
				name = (wchar_t const*)IDC_HAND;
				break;
			case AvoGUI::Cursor::Ibeam:
				name = (wchar_t const*)IDC_IBEAM;
				break;
			case AvoGUI::Cursor::ResizeAll:
				name = (wchar_t const*)IDC_SIZEALL;
				break;
			case AvoGUI::Cursor::ResizeNESW:
				name = (wchar_t const*)IDC_SIZENESW;
				break;
			case AvoGUI::Cursor::ResizeNS:
				name = (wchar_t const*)IDC_SIZENS;
				break;
			case AvoGUI::Cursor::ResizeNWSE:
				name = (wchar_t const*)IDC_SIZENWSE;
				break;
			case AvoGUI::Cursor::ResizeWE:
				name = (wchar_t const*)IDC_SIZEWE;
				break;
			case AvoGUI::Cursor::Wait:
				name = (wchar_t const*)IDC_WAIT;
				break;
		}
		m_cursorType = p_cursor;
		if (m_cursorHandle)
		{
			DestroyCursor(m_cursorHandle);
		}
		m_cursorHandle = LoadCursorW(0, name);
		if (!m_isMouseOutsideClientArea)
		{
			SetCursor(m_cursorHandle);
		}
	}
	AvoGUI::Cursor getCursor() const override
	{
		return m_cursorType;
	}

	//------------------------------

	float getDipToPixelFactor() const override
	{
		return m_dipToPixelFactor;
	}

	//------------------------------

	AvoGUI::DragDropOperation dragAndDropString(std::string const& p_string, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		return dragAndDropString(p_string.data(), p_string.size(), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropString(char const* p_string, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		return dragAndDropString(p_string, strlen(p_string), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropString(char const* p_string, uint32 p_length, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		OleDataObject* dataObject = createStringOleDataObject(p_string, p_length);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}

	AvoGUI::DragDropOperation dragAndDropString(std::wstring const& p_string, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		return dragAndDropString(p_string.data(), p_string.size(), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropString(wchar_t const* p_string, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		return dragAndDropString(p_string, wcslen(p_string), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropString(wchar_t const* p_string, uint32 p_length, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		OleDataObject* dataObject = createStringOleDataObject(p_string, p_length);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}

	AvoGUI::DragDropOperation dragAndDropImage(AvoGUI::Image* p_image, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		OleDataObject* dataObject = createImageOleDataObject(p_image);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFile(char const* p_data, uint32 p_dataSize, std::string const& p_name, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data, p_dataSize, p_name);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFile(char const* p_data, uint32 p_dataSize, std::wstring const& p_name, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition) override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data, p_dataSize, p_name);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_data, std::string const& p_name, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		return dragAndDropFile(p_data.data(), p_data.size(), p_name, p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_data, std::wstring const& p_name, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		return dragAndDropFile(p_data.data(), p_data.size(), p_name, p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_path, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFileOleDataObject(p_path);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::wstring const& p_path, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFileOleDataObject(p_path);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}

	AvoGUI::DragDropOperation dragAndDropFiles(std::vector<std::string> const& p_pathStrings, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		return dragAndDropFiles((std::string*)p_pathStrings.data(), p_pathStrings.size(), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::vector<std::wstring> const& p_pathStrings, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		return dragAndDropFiles((std::wstring*)p_pathStrings.data(), p_pathStrings.size(), p_dragImage, p_dragImageCursorPosition);
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::string* p_pathStrings, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_pathStrings, p_numberOfPaths);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::wstring* p_pathStrings, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_pathStrings, p_numberOfPaths);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFiles(char const* const* p_pathStrings, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_pathStrings, p_numberOfPaths);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}
	AvoGUI::DragDropOperation dragAndDropFiles(wchar_t const* const* p_pathStrings, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage, AvoGUI::Point<float> const& p_dragImageCursorPosition)
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_pathStrings, p_numberOfPaths);
		uint32 dropOperation = doDragDrop(dataObject, p_dragImage, p_dragImageCursorPosition);
		dataObject->Release();

		switch (dropOperation)
		{
			case DROPEFFECT_COPY:
				return AvoGUI::DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return AvoGUI::DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return AvoGUI::DragDropOperation::Link;
		}
		return AvoGUI::DragDropOperation::None;
	}

	//------------------------------

	void setClipboardString(std::wstring const& p_string) const override
	{
		setClipboardString(p_string.data(), p_string.size());
	}
	void setClipboardString(wchar_t const* p_string) const override
	{
		setClipboardString(p_string, wcslen(p_string));
	}
	void setClipboardString(wchar_t const* p_string, uint32 p_length) const override
	{
		OleDataObject* dataObject = createStringOleDataObject(p_string, p_length);
		OleSetClipboard(dataObject);
	}

	void setClipboardString(std::string const& p_string) const override
	{
		setClipboardString(p_string.data(), p_string.size());
	}
	void setClipboardString(char const* p_string) const override
	{
		setClipboardString(p_string, strlen(p_string));
	}
	void setClipboardString(char const* p_string, uint32 p_length) const override
	{
		OleDataObject* dataObject = createStringOleDataObject(p_string, p_length);
		OleSetClipboard(dataObject);
	}

	void setClipboardImage(AvoGUI::Image* p_image) const override
	{
		OleDataObject* dataObject = createImageOleDataObject(p_image);
		OleSetClipboard(dataObject);
	}

	void setClipboardFile(char const* p_data, uint32 p_dataSize, std::string const& p_name) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data, p_dataSize, p_name);
		OleSetClipboard(dataObject);
	}
	void setClipboardFile(char const* p_data, uint32 p_dataSize, std::wstring const& p_name) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data, p_dataSize, p_name);
		OleSetClipboard(dataObject);
	}
	void setClipboardFile(std::string const& p_data, std::string const& p_name) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data.data(), p_data.size(), p_name);
		OleSetClipboard(dataObject);
	}
	void setClipboardFile(std::string const& p_data, std::wstring const& p_name) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_data.data(), p_data.size(), p_name);
		OleSetClipboard(dataObject);
	}
	void setClipboardFile(std::string const& p_path) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_path);
		OleSetClipboard(dataObject);
	}
	void setClipboardFile(std::wstring const& p_path) const override
	{
		OleDataObject* dataObject = createFileOleDataObject(p_path);
		OleSetClipboard(dataObject);
	}

	void setClipboardFiles(std::vector<std::string> const& p_paths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject((std::string*)p_paths.data(), p_paths.size());
		OleSetClipboard(dataObject);
	}
	void setClipboardFiles(std::vector<std::wstring> const& p_paths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject((std::wstring*)p_paths.data(), p_paths.size());
		OleSetClipboard(dataObject);
	}
	void setClipboardFiles(std::string* p_paths, uint32 p_numberOfPaths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_paths, p_numberOfPaths);
		OleSetClipboard(dataObject);
	}
	void setClipboardFiles(std::wstring* p_paths, uint32 p_numberOfPaths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_paths, p_numberOfPaths);
		OleSetClipboard(dataObject);
	}
	void setClipboardFiles(char const* const* p_paths, uint32 p_numberOfPaths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_paths, p_numberOfPaths);
		OleSetClipboard(dataObject);
	}
	void setClipboardFiles(wchar_t const* const* p_paths, uint32 p_numberOfPaths) const override
	{
		OleDataObject* dataObject = createFilesOleDataObject(p_paths, p_numberOfPaths);
		OleSetClipboard(dataObject);
	}

	AvoGUI::ClipboardData* getClipboardData() const
	{
		IDataObject* dataObject = 0;
		OleGetClipboard(&dataObject);
		OleClipboardData* clipboardData = new OleClipboardData(m_gui);
		clipboardData->setOleDataObject(dataObject);
		return clipboardData;
	}

	//------------------------------

	// Returns true if the event was handled
	long long handleEvent(UINT p_message, WPARAM p_data_a, LPARAM p_data_b)
	{
		switch (p_message)
		{
			case WM_CREATE:
			{
				OleInitialize(0);

				m_oleDropSource = new OleDropSource(m_gui);
				m_oleDropTarget = new OleDropTarget(m_gui);
				RegisterDragDrop(m_windowHandle, m_oleDropTarget);

				m_clipboardFormat_fileContents = RegisterClipboardFormatW(CFSTR_FILECONTENTS);
				m_clipboardFormat_fileGroupDescriptor = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);

				//------------------------------

				EnableNonClientDpiScaling(m_windowHandle);

				/*
					LCS_WINDOWS_COLOR_SPACE is the default colorspace, but we want the background erase 
					color to be consistent with the colors of Direct2D and other potential graphics APIs 
					so it is changed to the sRGB color space.
				*/
				LOGCOLORSPACEW colorSpaceSettings;
				colorSpaceSettings.lcsSignature = LCS_SIGNATURE;
				colorSpaceSettings.lcsVersion = 0x400;
				colorSpaceSettings.lcsSize = sizeof(colorSpaceSettings);
				colorSpaceSettings.lcsCSType = LCS_sRGB;
				colorSpaceSettings.lcsIntent = LCS_GM_ABS_COLORIMETRIC;

				HCOLORSPACE colorSpace = CreateColorSpaceW(&colorSpaceSettings);
				SetColorSpace(GetDC(m_windowHandle), colorSpace);

				m_isOpen = true;

				sendWindowCreateEvents({ this, m_size.x / m_dipToPixelFactor, m_size.y / m_dipToPixelFactor });

				return 0;
			}
			case WM_SIZE:
			{
				if (!m_hasCreatedWindow)
				{
					m_hasCreatedWindowMutex.lock();
					m_hasCreatedWindow = true;
					m_hasCreatedWindowMutex.unlock();
					m_hasCreatedWindowConditionVariable.notify_one();
					return 0;
				}

				AvoGUI::WindowEvent windowEvent;
				windowEvent.window = this;
				if (p_data_a == SIZE_MINIMIZED)
				{
					sendWindowMinimizeEvents(windowEvent);
					m_state = AvoGUI::WindowState::Minimized;
				}
				else if (m_hasGottenInitialSizeMessageForCustomBorderWindows || !getHasCustomBorder())
				{
					m_size.x = p_data_b & 0xffff;
					m_size.y = p_data_b >> 16 & 0xffff;
					windowEvent.width = m_size.x / m_dipToPixelFactor;
					windowEvent.height = m_size.y / m_dipToPixelFactor;

					if (p_data_a == SIZE_MAXIMIZED)
					{
						sendWindowMaximizeEvents(windowEvent);
						m_state = AvoGUI::WindowState::Maximized;
					}
					else if (p_data_a == SIZE_RESTORED && m_state != AvoGUI::WindowState::Restored)
					{
						sendWindowRestoreEvents(windowEvent);
						m_state = AvoGUI::WindowState::Restored;
					}
					sendWindowSizeChangeEvents(windowEvent);
				}
				m_hasGottenInitialSizeMessageForCustomBorderWindows = true;

				return 0;
			}
			case WM_APP_CHANGE_SIZE:
			{
				SetWindowPos(m_windowHandle, 0, 0, 0, (uint32)p_data_a, (uint32)p_data_b, SWP_NOMOVE | SWP_NOZORDER);

				return 0;
			}
			case WM_APP_SET_IS_ENABLED:
			{
				if (p_data_a)
				{
					EnableWindow(m_windowHandle, true);
					SetForegroundWindow(m_windowHandle);
				}
				else
				{
					HWND child = GetWindow(m_windowHandle, GW_HWNDFIRST);
					if (child)
					{
						SetForegroundWindow(child);
						//SetActiveWindow(child);
						//SetFocus(child);
					}

					EnableWindow(m_windowHandle, false);
				}

				return 0;
			}
			case WM_ERASEBKGND:
			{
				HDC deviceContext = (HDC)p_data_a;

				RECT rectangle;
				GetUpdateRect(m_windowHandle, &rectangle, false);
				AvoGUI::Color color = m_gui->getDrawingContext()->getBackgroundColor(); // Thread safe I think?
				FillRect(deviceContext, &rectangle, CreateSolidBrush(RGB(color.red * 255, color.green * 255, color.blue * 255)));

				return 1; // We erased it.
			}
			case WM_NCCALCSIZE:
			{
				if (getHasCustomBorder() && p_data_a)
				{
					if (IsMaximized(m_windowHandle))
					{
						NCCALCSIZE_PARAMS* parameters = (NCCALCSIZE_PARAMS*)p_data_b;
						MONITORINFO info = { };
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
				if (getHasCustomBorder() && GetCapture() != m_windowHandle)
				{
					POINT mousePosition = { GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b) };
					ScreenToClient(m_windowHandle, &mousePosition);

					bool wasMousePositionInsideWindow = m_mousePosition.x >= 0 && m_mousePosition.y >= 0 && m_mousePosition.x < m_size.x && m_mousePosition.y < m_size.y;
					if (!m_isMouseOutsideClientArea || m_isMouseOutsideClientArea && !wasMousePositionInsideWindow) // Is was outside of the nonclient area before this mousemove.
					{
						TRACKMOUSEEVENT trackStructure = { };
						trackStructure.dwFlags = TME_LEAVE | TME_NONCLIENT;
						trackStructure.cbSize = sizeof(TRACKMOUSEEVENT);
						trackStructure.hwndTrack = m_windowHandle;
						TrackMouseEvent(&trackStructure);
						if (m_isMouseOutsideClientArea)
						{
							// The window will recieve WM_MOUSELEAVE - no need for extra mouse events, so return.
							m_isMouseOutsideClientArea = true;
							return 0;
						}
					}

					m_isMouseOutsideClientArea = true;

					// We want the GUI to recieve mouse move events even when the mouse is inside the nonclient area of the window - 
					// because it is in this case part of the GUI (since the CustomBorder style flag is true).
					if (mousePosition.x >= 0 && mousePosition.y >= 0 && mousePosition.x < m_size.x && mousePosition.y < m_size.y)
					{
						AvoGUI::MouseEvent mouseEvent;
						mouseEvent.x = mousePosition.x;
						mouseEvent.y = mousePosition.y;
						mouseEvent.movementX = mousePosition.x - m_mousePosition.x;
						mouseEvent.movementY = mousePosition.y - m_mousePosition.y;

						m_mousePosition.x = mousePosition.x;
						m_mousePosition.y = mousePosition.y;

						m_gui->excludeAnimationThread();
						m_gui->handleGlobalMouseMove(mouseEvent);
						m_gui->includeAnimationThread();
					}
					return 0;
				}
				break;
			}
			case WM_MOUSEMOVE:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				if (x == m_mousePosition.x && y == m_mousePosition.y)
				{
					return 0;
				}

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.movementX = (x - m_mousePosition.x) / m_dipToPixelFactor;
				mouseEvent.movementY = (y - m_mousePosition.y) / m_dipToPixelFactor;

				m_mousePosition.x = x;
				m_mousePosition.y = y;

				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseMove(mouseEvent);
				m_gui->includeAnimationThread();

				if (m_isMouseOutsideClientArea)
				{
					SetCursor(m_cursorHandle);

					// This is to make the window recieve WM_MOUSELEAVE.
					TRACKMOUSEEVENT trackStructure = { };
					trackStructure.dwFlags = TME_LEAVE;
					trackStructure.cbSize = sizeof(TRACKMOUSEEVENT);
					trackStructure.hwndTrack = m_windowHandle;
					TrackMouseEvent(&trackStructure);

					m_isMouseOutsideClientArea = false;
				}

				return 0;
			}
			case WM_NCMOUSELEAVE:
			case WM_MOUSELEAVE:
			{
				if (GetCapture() != m_windowHandle)
				{
					POINT mousePosition;
					GetCursorPos(&mousePosition);

					bool isMouseOverWindow = WindowFromPoint(mousePosition) == m_windowHandle;

					ScreenToClient(m_windowHandle, &mousePosition);

					if (mousePosition.x >= 0 && mousePosition.y >= 0 && mousePosition.x < m_size.x && mousePosition.y < m_size.y && isMouseOverWindow)
					{
						// If it's a WM_MOUSELEAVE message, then it has entered the nonclient area if the new mouse position still is inside the window.
						// If it's a WM_NCMOUSELEAVE message, then it has entered the client area.
						// Note that both these cases would mean that the window has the CustomBorder style flag set.
						m_isMouseOutsideClientArea = p_message == WM_MOUSELEAVE;
						return 0;
					}

					m_isMouseOutsideClientArea = true;

					AvoGUI::MouseEvent mouseEvent;
					mouseEvent.x = mousePosition.x / m_dipToPixelFactor;
					mouseEvent.y = mousePosition.y / m_dipToPixelFactor;
					mouseEvent.movementX = (mousePosition.x - m_mousePosition.x) / m_dipToPixelFactor;
					mouseEvent.movementY = (mousePosition.y - m_mousePosition.y) / m_dipToPixelFactor;

					m_mousePosition.x = mousePosition.x;
					m_mousePosition.y = mousePosition.y;

					m_gui->excludeAnimationThread();
					m_gui->handleGlobalMouseMove(mouseEvent);
					m_gui->handleGlobalMouseLeave(mouseEvent);
					m_gui->includeAnimationThread();
				}
				return 0;
			}
			case WM_NCHITTEST:
			{
				if (getHasCustomBorder())
				{
					if (p_data_b)
					{
						POINT mousePosition = { GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b) };
						ScreenToClient(m_windowHandle, &mousePosition);

						AvoGUI::WindowBorderArea area = m_gui->getWindowBorderAreaAtPosition(mousePosition.x / m_dipToPixelFactor, mousePosition.y / m_dipToPixelFactor);
						if (IsMaximized(m_windowHandle) && area != AvoGUI::WindowBorderArea::Dragging && area != AvoGUI::WindowBorderArea::None)
						{
							return HTCLIENT;
						}
						switch (area)
						{
						case AvoGUI::WindowBorderArea::TopLeftResize:
							return HTTOPLEFT;
						case AvoGUI::WindowBorderArea::TopResize:
							return HTTOP;
						case AvoGUI::WindowBorderArea::TopRightResize:
							return HTTOPRIGHT;
						case AvoGUI::WindowBorderArea::LeftResize:
							return HTLEFT;
						case AvoGUI::WindowBorderArea::RightResize:
							return HTRIGHT;
						case AvoGUI::WindowBorderArea::BottomLeftResize:
							return HTBOTTOMLEFT;
						case AvoGUI::WindowBorderArea::BottomResize:
							return HTBOTTOM;
						case AvoGUI::WindowBorderArea::BottomRightResize:
							return HTBOTTOMRIGHT;
						case AvoGUI::WindowBorderArea::Dragging:
							return HTCAPTION;
						case AvoGUI::WindowBorderArea::None:
							return HTCLIENT;
						}
					}
					return 0;
				}
				break;
			}
			case WM_DPICHANGED:
			{
				m_gui->getDrawingContext()->setDpi(float(HIWORD(p_data_a)));
				m_dipToPixelFactor = HIWORD(p_data_a) / (float)USER_DEFAULT_SCREEN_DPI;
				RECT* newRectangle = (RECT*)p_data_b;
				SetWindowPos(m_windowHandle, 0, newRectangle->left, newRectangle->top, newRectangle->right - newRectangle->left, newRectangle->bottom - newRectangle->top, SWP_NOZORDER | SWP_NOACTIVATE);
				return 0;
			}
			case WM_GETMINMAXINFO:
			{
				MINMAXINFO* minMaxInfo = (MINMAXINFO*)p_data_b;
				RECT rect = { 0, 0, (int)m_minSize.x, (int)m_minSize.y };
				AdjustWindowRect(&rect, m_styles, 0);

				if (m_minSize.x > 0U || m_minSize.y > 0U)
				{
					minMaxInfo->ptMinTrackSize.x = rect.right - rect.left;
					minMaxInfo->ptMinTrackSize.y = rect.bottom - rect.top;
				}
				if (m_maxSize.x > 0U || m_maxSize.y > 0U)
				{
					minMaxInfo->ptMaxTrackSize.x = rect.right - rect.left;
					minMaxInfo->ptMaxTrackSize.y = rect.bottom - rect.top;
				}
				return 0;
			}
			case WM_MOVE:
			{
				RECT rect;
				GetWindowRect(m_windowHandle, &rect);
				m_position.set(rect.left, rect.top);

				return 0;
			}
			case WM_MOUSEWHEEL:
			{
				POINT mousePosition;
				mousePosition.x = GET_X_LPARAM(p_data_b);
				mousePosition.y = GET_Y_LPARAM(p_data_b);
				
				ScreenToClient(m_windowHandle, &mousePosition);

				float delta = float(GET_WHEEL_DELTA_WPARAM(p_data_a)) / 120.f;

				AvoGUI::ModifierKeyFlags modifierKeyFlags = convertWindowsKeyStateToModifierKeyFlags(GET_KEYSTATE_WPARAM(p_data_a));

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = mousePosition.x / m_dipToPixelFactor;
				mouseEvent.y = mousePosition.y / m_dipToPixelFactor;
				mouseEvent.scrollDelta = delta;
				mouseEvent.modifierKeys = modifierKeyFlags;

				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseScroll(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_LBUTTONDOWN:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;

				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDown(mouseEvent);
				m_gui->includeAnimationThread();

				SetCapture(m_windowHandle);

				return 0;
			}
			case WM_LBUTTONUP:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;

				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseUp(mouseEvent);
				m_gui->includeAnimationThread();

				ReleaseCapture();

				return 0;
			}
			case WM_LBUTTONDBLCLK:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDoubleClick(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDown(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_RBUTTONUP:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseUp(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_RBUTTONDBLCLK:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDoubleClick(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_MBUTTONDOWN:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDown(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_MBUTTONUP:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseUp(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_MBUTTONDBLCLK:
			{
				int32 x = GET_X_LPARAM(p_data_b);
				int32 y = GET_Y_LPARAM(p_data_b);

				AvoGUI::ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				AvoGUI::MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = AvoGUI::MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalMouseDoubleClick(mouseEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				bool isRepeated = p_data_b & (1 << 30);
				AvoGUI::KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				AvoGUI::KeyboardEvent keyboardEvent;
				keyboardEvent.key = key;
				keyboardEvent.isRepeated = isRepeated;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalKeyboardKeyDown(keyboardEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				AvoGUI::KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				AvoGUI::KeyboardEvent keyboardEvent;
				keyboardEvent.key = key;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalKeyboardKeyUp(keyboardEvent);
				m_gui->includeAnimationThread();

				return 0;
			}
			case WM_CHAR:
			{
				bool isRepeated = p_data_b & (1 << 30);

				// Length is 5 because 4 is the max number of bytes in a utf-8 encoded character, and the null terminator is included
				char character[5];
				AvoGUI::convertUtf16ToUtf8((wchar_t const*)&p_data_a, character, 5);

				AvoGUI::KeyboardEvent keyboardEvent;
				keyboardEvent.character = character;
				keyboardEvent.isRepeated = isRepeated;
				m_gui->excludeAnimationThread();
				m_gui->handleGlobalCharacterInput(keyboardEvent);
				m_gui->includeAnimationThread();
				return 0;
			}
			case WM_MENUCHAR:
			{
				return 1 << 16;
			}
			case WM_CLOSE:
			{
				if (m_willClose)
				{
					DeleteColorSpace(GetColorSpace(GetDC(m_windowHandle)));
					
					RevokeDragDrop(m_windowHandle);
					OleUninitialize();

					m_isOpen = false;
					DestroyWindow(m_windowHandle);
				}
				else
				{
					m_willClose = sendWindowCloseEvents({ this, m_size.x/m_dipToPixelFactor, m_size.y/m_dipToPixelFactor });
				}

				return 0;
			}
			case WM_DESTROY:
			{
				m_windowHandle = 0;
				s_numberOfWindows--;
				if (!s_numberOfWindows)
				{
					UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandle(0));
				}
				PostQuitMessage(0);

				return 0;
			}
		}
		return ~0LL;
	}

	//------------------------------
	// Static

	static LRESULT CALLBACK handleGlobalEvents(HWND p_windowHandle, UINT p_message, WPARAM p_data_a, LPARAM p_data_b)
	{
		WindowsWindow* window;
		if (p_message == WM_CREATE)
		{
			window = (WindowsWindow*)((CREATESTRUCT*)p_data_b)->lpCreateParams;
			SetWindowLongPtr(p_windowHandle, GWLP_USERDATA, (LONG_PTR)window);
			window->setNativeHandle(p_windowHandle);
		}
		else
		{
			window = (WindowsWindow*)GetWindowLongPtr(p_windowHandle, GWLP_USERDATA);
		}
		if (window)
		{
			long long result = window->handleEvent(p_message, p_data_a, p_data_b);
			if (result != ~0LL)
			{
				return result;
			}
		}
		return DefWindowProc(p_windowHandle, p_message, p_data_a, p_data_b);
	}
};
std::atomic<uint32> WindowsWindow::s_numberOfWindows;
wchar_t const* const WindowsWindow::WINDOW_CLASS_NAME = L"AvoGUI window class";
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

class LinuxWindow : public AvoGUI::Window
{
private:
	AvoGUI::Gui* m_gui{nullptr};

	Display* m_server{nullptr};
	XWindow m_windowHandle{0};
	Colormap m_colormap{0};

	XIM m_inputMethod{nullptr};
	XIC m_inputContext{nullptr};

	Atom m_windowManagerProtocolsMessageType{0};
	Atom m_windowCloseEvent{0};

	Atom m_backgroundColorMessageType{0};
	
	bool m_isOpen{false};
	AvoGUI::Point<uint32> m_size;
	AvoGUI::Point<float> m_minSize;
	AvoGUI::Point<float> m_maxSize;

	AvoGUI::WindowStyleFlags m_crossPlatformStyles{AvoGUI::WindowStyleFlags::None};

	float m_dipToPixelFactor{1.f};

	//------------------------------

	KeySym convertKeyboardKeyToKeySym(AvoGUI::KeyboardKey p_key) const
	{
		switch (p_key)
		{
			case AvoGUI::KeyboardKey::Menu:
				return XK_Menu;
			case AvoGUI::KeyboardKey::Backspace:
				return XK_BackSpace;
			case AvoGUI::KeyboardKey::Clear:
				return XK_Clear;
			case AvoGUI::KeyboardKey::Tab:
				return XK_Tab;
			case AvoGUI::KeyboardKey::Return:
				return XK_Return;
			case AvoGUI::KeyboardKey::Shift:
				return XK_Shift_L;
			case AvoGUI::KeyboardKey::Control:
				return XK_Control_L;
			case AvoGUI::KeyboardKey::Alt:
				return XK_Alt_L;
			case AvoGUI::KeyboardKey::Pause:
				return XK_Pause;
			case AvoGUI::KeyboardKey::CapsLock:
				return XK_Caps_Lock;
			case AvoGUI::KeyboardKey::Escape:
				return XK_Escape;
			case AvoGUI::KeyboardKey::Spacebar:
				return XK_space;
			case AvoGUI::KeyboardKey::PageUp:
				return XK_Page_Up;
			case AvoGUI::KeyboardKey::PageDown:
				return XK_Page_Down;
			case AvoGUI::KeyboardKey::End:
				return XK_End;
			case AvoGUI::KeyboardKey::Home:
				return XK_Home;
			case AvoGUI::KeyboardKey::Left:
				return XK_Left;
			case AvoGUI::KeyboardKey::Right:
				return XK_Right;
			case AvoGUI::KeyboardKey::Up:
				return XK_Up;
			case AvoGUI::KeyboardKey::Down:
				return XK_Down;
			case AvoGUI::KeyboardKey::PrintScreen:
				return XK_Print;
			case AvoGUI::KeyboardKey::Insert:
				return XK_Insert;
			case AvoGUI::KeyboardKey::Delete:
				return XK_Delete;
			case AvoGUI::KeyboardKey::Help:
				return XK_Help;
			case AvoGUI::KeyboardKey::Numpad0:
				return XK_KP_0;
			case AvoGUI::KeyboardKey::Numpad1:
				return XK_KP_1;
			case AvoGUI::KeyboardKey::Numpad2:
				return XK_KP_2;
			case AvoGUI::KeyboardKey::Numpad3:
				return XK_KP_3;
			case AvoGUI::KeyboardKey::Numpad4:
				return XK_KP_4;
			case AvoGUI::KeyboardKey::Numpad5:
				return XK_KP_5;
			case AvoGUI::KeyboardKey::Numpad6:
				return XK_KP_6;
			case AvoGUI::KeyboardKey::Numpad7:
				return XK_KP_7;
			case AvoGUI::KeyboardKey::Numpad8:
				return XK_KP_8;
			case AvoGUI::KeyboardKey::Numpad9:
				return XK_KP_9;
			case AvoGUI::KeyboardKey::Add:
				return XK_KP_Add;
			case AvoGUI::KeyboardKey::Subtract:
				return XK_KP_Subtract;
			case AvoGUI::KeyboardKey::Multiply:
				return XK_KP_Multiply;
			case AvoGUI::KeyboardKey::Divide:
				return XK_KP_Divide;
			case AvoGUI::KeyboardKey::F1:
				return XK_F1;
			case AvoGUI::KeyboardKey::F2:
				return XK_F2;
			case AvoGUI::KeyboardKey::F3:
				return XK_F3;
			case AvoGUI::KeyboardKey::F4:
				return XK_F4;
			case AvoGUI::KeyboardKey::F5:
				return XK_F5;
			case AvoGUI::KeyboardKey::F6:
				return XK_F6;
			case AvoGUI::KeyboardKey::F7:
				return XK_F7;
			case AvoGUI::KeyboardKey::F8:
				return XK_F8;
			case AvoGUI::KeyboardKey::F9:
				return XK_F9;
			case AvoGUI::KeyboardKey::F10:
				return XK_F10;
			case AvoGUI::KeyboardKey::F11:
				return XK_F11;
			case AvoGUI::KeyboardKey::F12:
				return XK_F12;
			case AvoGUI::KeyboardKey::F13:
				return XK_F13;
			case AvoGUI::KeyboardKey::F14:
				return XK_F14;
			case AvoGUI::KeyboardKey::F15:
				return XK_F15;
			case AvoGUI::KeyboardKey::F16:
				return XK_F16;
			case AvoGUI::KeyboardKey::F17:
				return XK_F17;
			case AvoGUI::KeyboardKey::F18:
				return XK_F18;
			case AvoGUI::KeyboardKey::F19:
				return XK_F19;
			case AvoGUI::KeyboardKey::F20:
				return XK_F20;
			case AvoGUI::KeyboardKey::F21:
				return XK_F21;
			case AvoGUI::KeyboardKey::F22:
				return XK_F22;
			case AvoGUI::KeyboardKey::F23:
				return XK_F23;
			case AvoGUI::KeyboardKey::F24:
				return XK_F24;
			case AvoGUI::KeyboardKey::NumLock:
				return XK_Num_Lock;
			case AvoGUI::KeyboardKey::Number0:
				return XK_0;
			case AvoGUI::KeyboardKey::Number1:
				return XK_1;
			case AvoGUI::KeyboardKey::Number2:
				return XK_2;
			case AvoGUI::KeyboardKey::Number3:
				return XK_3;
			case AvoGUI::KeyboardKey::Number4:
				return XK_4;
			case AvoGUI::KeyboardKey::Number5:
				return XK_5;
			case AvoGUI::KeyboardKey::Number6:
				return XK_6;
			case AvoGUI::KeyboardKey::Number7:
				return XK_7;
			case AvoGUI::KeyboardKey::Number8:
				return XK_8;
			case AvoGUI::KeyboardKey::Number9:
				return XK_9;
			case AvoGUI::KeyboardKey::A:
				return XK_A;
			case AvoGUI::KeyboardKey::B:
				return XK_B;
			case AvoGUI::KeyboardKey::C:
				return XK_C;
			case AvoGUI::KeyboardKey::D:
				return XK_D;
			case AvoGUI::KeyboardKey::E:
				return XK_E;
			case AvoGUI::KeyboardKey::F:
				return XK_F;
			case AvoGUI::KeyboardKey::G:
				return XK_G;
			case AvoGUI::KeyboardKey::H:
				return XK_H;
			case AvoGUI::KeyboardKey::I:
				return XK_I;
			case AvoGUI::KeyboardKey::J:
				return XK_J;
			case AvoGUI::KeyboardKey::K:
				return XK_K;
			case AvoGUI::KeyboardKey::L:
				return XK_L;
			case AvoGUI::KeyboardKey::M:
				return XK_M;
			case AvoGUI::KeyboardKey::N:
				return XK_N;
			case AvoGUI::KeyboardKey::O:
				return XK_O;
			case AvoGUI::KeyboardKey::P:
				return XK_P;
			case AvoGUI::KeyboardKey::Q:
				return XK_Q;
			case AvoGUI::KeyboardKey::R:
				return XK_R;
			case AvoGUI::KeyboardKey::S:
				return XK_S;
			case AvoGUI::KeyboardKey::T:
				return XK_T;
			case AvoGUI::KeyboardKey::U:
				return XK_U;
			case AvoGUI::KeyboardKey::V:
				return XK_V;
			case AvoGUI::KeyboardKey::W:
				return XK_W;
			case AvoGUI::KeyboardKey::X:
				return XK_X;
			case AvoGUI::KeyboardKey::Y:
				return XK_Y;
			case AvoGUI::KeyboardKey::Z:
				return XK_Z;
			case AvoGUI::KeyboardKey::Regional1:
				return XK_semicolon;
			case AvoGUI::KeyboardKey::Regional2:
				return XK_slash;
			case AvoGUI::KeyboardKey::Regional3:
				return XK_grave;
			case AvoGUI::KeyboardKey::Regional4:
				return XK_bracketleft;
			case AvoGUI::KeyboardKey::Regional5:
				return XK_backslash;
			case AvoGUI::KeyboardKey::Regional6:
				return XK_bracketright;
			case AvoGUI::KeyboardKey::Regional7:
				return XK_apostrophe;
		}
		return 0;
	}
	AvoGUI::KeyboardKey convertKeySymToKeyboardKey(KeySym p_keySym) const
	{
		switch (p_keySym)
		{
			case XK_Menu:
				return AvoGUI::KeyboardKey::Menu;
			case XK_BackSpace:
				return AvoGUI::KeyboardKey::Backspace;
			case XK_Clear:
				return AvoGUI::KeyboardKey::Clear;
			case XK_Tab:
				return AvoGUI::KeyboardKey::Tab;
			case XK_Return:
				return AvoGUI::KeyboardKey::Return;
			case XK_Shift_L:
			case XK_Shift_R:
				return AvoGUI::KeyboardKey::Shift;
			case XK_Control_L:
			case XK_Control_R:
				return AvoGUI::KeyboardKey::Control;
			case XK_Alt_L:
			case XK_Alt_R:
				return AvoGUI::KeyboardKey::Alt;
			case XK_Pause:
				return AvoGUI::KeyboardKey::Pause;
			case XK_Caps_Lock:
				return AvoGUI::KeyboardKey::CapsLock;
			case XK_Escape:
				return AvoGUI::KeyboardKey::Escape;
			case XK_space:
				return AvoGUI::KeyboardKey::Spacebar;
			case XK_Page_Up:
				return AvoGUI::KeyboardKey::PageUp;
			case XK_Page_Down:
				return AvoGUI::KeyboardKey::PageDown;
			case XK_End:
				return AvoGUI::KeyboardKey::End;
			case XK_Home:
				return AvoGUI::KeyboardKey::Home;
			case XK_Left:
				return AvoGUI::KeyboardKey::Left;
			case XK_Right:
				return AvoGUI::KeyboardKey::Right;
			case XK_Up:
				return AvoGUI::KeyboardKey::Up;
			case XK_Down:
				return AvoGUI::KeyboardKey::Down;
			case XK_Print:
				return AvoGUI::KeyboardKey::PrintScreen;
			case XK_Insert:
				return AvoGUI::KeyboardKey::Insert;
			case XK_Delete:
				return AvoGUI::KeyboardKey::Delete;
			case XK_Help:
				return AvoGUI::KeyboardKey::Help;
			case XK_KP_0:
				return AvoGUI::KeyboardKey::Numpad0;
			case XK_KP_1:
				return AvoGUI::KeyboardKey::Numpad1;
			case XK_KP_2:
				return AvoGUI::KeyboardKey::Numpad2;
			case XK_KP_3:
				return AvoGUI::KeyboardKey::Numpad3;
			case XK_KP_4:
				return AvoGUI::KeyboardKey::Numpad4;
			case XK_KP_5:
				return AvoGUI::KeyboardKey::Numpad5;
			case XK_KP_6:
				return AvoGUI::KeyboardKey::Numpad6;
			case XK_KP_7:
				return AvoGUI::KeyboardKey::Numpad7;
			case XK_KP_8:
				return AvoGUI::KeyboardKey::Numpad8;
			case XK_KP_9:
				return AvoGUI::KeyboardKey::Numpad9;
			case XK_KP_Add:
				return AvoGUI::KeyboardKey::Add;
			case XK_KP_Subtract:
				return AvoGUI::KeyboardKey::Subtract;
			case XK_KP_Multiply:
				return AvoGUI::KeyboardKey::Multiply;
			case XK_KP_Divide:
				return AvoGUI::KeyboardKey::Divide;
			case XK_F1:
				return AvoGUI::KeyboardKey::F1;
			case XK_F2:
				return AvoGUI::KeyboardKey::F2;
			case XK_F3:
				return AvoGUI::KeyboardKey::F3;
			case XK_F4:
				return AvoGUI::KeyboardKey::F4;
			case XK_F5:
				return AvoGUI::KeyboardKey::F5;
			case XK_F6:
				return AvoGUI::KeyboardKey::F6;
			case XK_F7:
				return AvoGUI::KeyboardKey::F7;
			case XK_F8:
				return AvoGUI::KeyboardKey::F8;
			case XK_F9:
				return AvoGUI::KeyboardKey::F9;
			case XK_F10:
				return AvoGUI::KeyboardKey::F10;
			case XK_F11:
				return AvoGUI::KeyboardKey::F11;
			case XK_F12:
				return AvoGUI::KeyboardKey::F12;
			case XK_F13:
				return AvoGUI::KeyboardKey::F13;
			case XK_F14:
				return AvoGUI::KeyboardKey::F14;
			case XK_F15:
				return AvoGUI::KeyboardKey::F15;
			case XK_F16:
				return AvoGUI::KeyboardKey::F16;
			case XK_F17:
				return AvoGUI::KeyboardKey::F17;
			case XK_F18:
				return AvoGUI::KeyboardKey::F18;
			case XK_F19:
				return AvoGUI::KeyboardKey::F19;
			case XK_F20:
				return AvoGUI::KeyboardKey::F20;
			case XK_F21:
				return AvoGUI::KeyboardKey::F21;
			case XK_F22:
				return AvoGUI::KeyboardKey::F22;
			case XK_F23:
				return AvoGUI::KeyboardKey::F23;
			case XK_F24:
				return AvoGUI::KeyboardKey::F24;
			case XK_Num_Lock:
				return AvoGUI::KeyboardKey::NumLock;
			case XK_0:
				return AvoGUI::KeyboardKey::Number0;
			case XK_1:
				return AvoGUI::KeyboardKey::Number1;
			case XK_2:
				return AvoGUI::KeyboardKey::Number2;
			case XK_3:
				return AvoGUI::KeyboardKey::Number3;
			case XK_4:
				return AvoGUI::KeyboardKey::Number4;
			case XK_5:
				return AvoGUI::KeyboardKey::Number5;
			case XK_6:
				return AvoGUI::KeyboardKey::Number6;
			case XK_7:
				return AvoGUI::KeyboardKey::Number7;
			case XK_8:
				return AvoGUI::KeyboardKey::Number8;
			case XK_9:
				return AvoGUI::KeyboardKey::Number9;
			case XK_A:
				return AvoGUI::KeyboardKey::A;
			case XK_B:
				return AvoGUI::KeyboardKey::B;
			case XK_C:
				return AvoGUI::KeyboardKey::C;
			case XK_D:
				return AvoGUI::KeyboardKey::D;
			case XK_E:
				return AvoGUI::KeyboardKey::E;
			case XK_F:
				return AvoGUI::KeyboardKey::F;
			case XK_G:
				return AvoGUI::KeyboardKey::G;
			case XK_H:
				return AvoGUI::KeyboardKey::H;
			case XK_I:
				return AvoGUI::KeyboardKey::I;
			case XK_J:
				return AvoGUI::KeyboardKey::J;
			case XK_K:
				return AvoGUI::KeyboardKey::K;
			case XK_L:
				return AvoGUI::KeyboardKey::L;
			case XK_M:
				return AvoGUI::KeyboardKey::M;
			case XK_N:
				return AvoGUI::KeyboardKey::N;
			case XK_O:
				return AvoGUI::KeyboardKey::O;
			case XK_P:
				return AvoGUI::KeyboardKey::P;
			case XK_Q:
				return AvoGUI::KeyboardKey::Q;
			case XK_R:
				return AvoGUI::KeyboardKey::R;
			case XK_S:
				return AvoGUI::KeyboardKey::S;
			case XK_T:
				return AvoGUI::KeyboardKey::T;
			case XK_U:
				return AvoGUI::KeyboardKey::U;
			case XK_V:
				return AvoGUI::KeyboardKey::V;
			case XK_W:
				return AvoGUI::KeyboardKey::W;
			case XK_X:
				return AvoGUI::KeyboardKey::X;
			case XK_Y:
				return AvoGUI::KeyboardKey::Y;
			case XK_Z:
				return AvoGUI::KeyboardKey::Z;
			case XK_semicolon:
				return AvoGUI::KeyboardKey::Regional1;
			case XK_slash:
				return AvoGUI::KeyboardKey::Regional2;
			case XK_grave:
				return AvoGUI::KeyboardKey::Regional3;
			case XK_bracketleft:
				return AvoGUI::KeyboardKey::Regional4;
			case XK_backslash:
				return AvoGUI::KeyboardKey::Regional5;
			case XK_bracketright:
				return AvoGUI::KeyboardKey::Regional6;
			case XK_apostrophe:
				return AvoGUI::KeyboardKey::Regional7;
		}
		return AvoGUI::KeyboardKey::None;
	}
	
	//------------------------------

	bool m_hasCreatedWindow = false;
	std::condition_variable m_hasCreatedWindowConditionVariable;
	std::mutex m_hasCreatedWindowMutex;
	std::thread m_messageThread;
	void thread_runEventLoop(char const* p_title, uint32 p_titleSize, float p_x, float p_y, float p_width, float p_height, AvoGUI::Window* p_parent)
	{
		XInitThreads();

		m_server = XOpenDisplay(nullptr); // Open connection to server

		auto displayWidth = (float)XDisplayWidth(m_server, 0);
		auto displayHeight = (float)XDisplayHeight(m_server, 0);
		m_dipToPixelFactor = displayWidth / (float)XDisplayWidthMM(m_server, 0) * 25.4f / NORMAL_DPI;
		m_size.set(p_width*m_dipToPixelFactor, p_height*m_dipToPixelFactor);

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
			GLX_STENCIL_SIZE, 8,
			GLX_DOUBLEBUFFER, 1,
			0 // Null terminator
		};

		int numberOfMatchingConfigurations = 0;
		GLXFBConfig* framebufferConfigurations = glXChooseFBConfig(m_server, DefaultScreen(m_server), framebufferAttributes, &numberOfMatchingConfigurations);

		XVisualInfo* visualInfo = glXGetVisualFromFBConfig(m_server, *framebufferConfigurations);

		XFree(framebufferConfigurations);

		//------------------------------
		// Create window

		m_colormap = XCreateColormap(m_server, RootWindow(m_server, visualInfo->screen), visualInfo->visual, 0);
		XSetWindowAttributes windowAttributes = { };
		windowAttributes.colormap = m_colormap;
		windowAttributes.event_mask =
			ExposureMask |
			EnterWindowMask | LeaveWindowMask |
			StructureNotifyMask |
			PointerMotionMask |
			ButtonPressMask | ButtonReleaseMask |
			ButtonMotionMask |
			KeyPressMask | KeyReleaseMask;

		m_windowHandle = XCreateWindow(
			m_server, p_parent ? (XWindow)p_parent->getNativeHandle() : RootWindow(m_server, visualInfo->screen),
			0, 0, // Initial x and y are ignored by the window manager
			(unsigned int)(p_width*m_dipToPixelFactor),
			(unsigned int)(p_height*m_dipToPixelFactor),
			0,
			visualInfo->depth,
			InputOutput,
			visualInfo->visual,
			CWEventMask | CWBorderPixel | CWColormap,
			&windowAttributes
		);

		XFree(visualInfo);

		setTitle(p_title, p_titleSize);
		XMapWindow(m_server, m_windowHandle); // Show the window
		setPosition(p_x * (displayWidth - p_width*m_dipToPixelFactor), p_y * (displayHeight - p_height*m_dipToPixelFactor));

		//------------------------------
		// Open keyboard input

		m_inputMethod = XOpenIM(m_server, nullptr, nullptr, nullptr);
		m_inputContext = XCreateIC(
			m_inputMethod,
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing, // Input style flags
			XNClientWindow, m_windowHandle,
			XNFocusWindow, m_windowHandle,
			nullptr // Null terminator
		);

		//------------------------------

		m_backgroundColorMessageType = XInternAtom(m_server, "AVOGUI_SET_BACKGROUND_COLOR", false);

		// We want the window manager to tell us when the window should be closed.
		// WM_PROTOCOLS is the atom used to identify messages sent from the window manager in a ClientMessage.
		m_windowManagerProtocolsMessageType = XInternAtom(m_server, "WM_PROTOCOLS", true);
		// This is the atom sent as the data in a ClientMessage with type WM_PROTOCOLS, to indicate the close event.
		m_windowCloseEvent = XInternAtom(m_server, "WM_DELETE_WINDOW", 0);
		// Tell the window manager that we want it to send the event through WM_PROTOCOLS.
		XSetWMProtocols(m_server, m_windowHandle, &m_windowCloseEvent, 1);

		XFlush(m_server); // Execute the command queue

		//------------------------------

		XEvent event;
		Time lastKeyPressTime = 0;
		KeyCode lastKeyPressKeyCode = 0;

		m_isOpen = true;
		while (m_isOpen)
		{
			XNextEvent(m_server, &event);

			if (XFilterEvent(&event, m_windowHandle))
			{
				continue;
			}

			switch (event.type)
			{
				case Expose:
				{
					if (!event.xexpose.count)
					{
						m_gui->invalidateRectangle(
							(float)event.xexpose.x/m_dipToPixelFactor, (float)event.xexpose.y/m_dipToPixelFactor,
							(float)event.xexpose.width/m_dipToPixelFactor, (float)event.xexpose.height/m_dipToPixelFactor
						);
					}
					break;
				}
				case ClientMessage:
				{
					if (event.xclient.message_type == m_windowManagerProtocolsMessageType)
					{
						// Sent from the window manager when the user has tried to close the window,
						// it is up to us to decide whether to actually close and exit the application.
						if (event.xclient.data.l[0] == m_windowCloseEvent)
						{
							if (m_gui->getWillClose())
							{
								m_isOpen = false;
							}
							else
							{
								m_gui->excludeAnimationThread();
								m_gui->handleWindowClose({ this, m_size.x/m_dipToPixelFactor, m_size.y/m_dipToPixelFactor });
								m_gui->includeAnimationThread();
							}
						}
					}
					else if (event.xclient.message_type == m_backgroundColorMessageType)
					{
						// Sent from drawing context
						XColor xColor;
						xColor.red = event.xclient.data.l[0];
						xColor.green = event.xclient.data.l[1];
						xColor.blue = event.xclient.data.l[2];
						xColor.flags = DoRed | DoGreen | DoBlue;
						XAllocColor(m_server, m_colormap, &xColor);
						XSetWindowBackground(m_server, m_windowHandle, xColor.pixel);
					}
					break;
				}
				case GravityNotify:
				{
					break;
				}
				case ConfigureNotify:
				{
					if (!m_hasCreatedWindow)
					{
						AvoGUI::WindowEvent windowEvent = { this, p_width, p_height };
						m_gui->excludeAnimationThread();
						m_gui->handleWindowCreate(windowEvent);
						m_gui->includeAnimationThread();

						m_gui->excludeAnimationThread();
						m_gui->handleWindowSizeChange(windowEvent);
						m_gui->includeAnimationThread();

						m_hasCreatedWindowMutex.lock();
						m_hasCreatedWindow = true;
						m_hasCreatedWindowMutex.unlock();
						m_hasCreatedWindowConditionVariable.notify_one();
					}
					else if (m_size.x != event.xconfigure.width || m_size.y != event.xconfigure.height)
					{
						m_size.set(event.xconfigure.width, event.xconfigure.height);
						m_gui->excludeAnimationThread();
						m_gui->handleWindowSizeChange({ this, m_size.x / m_dipToPixelFactor, m_size.y / m_dipToPixelFactor });
						m_gui->includeAnimationThread();
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
					int length = Xutf8LookupString(m_inputContext, &event.xkey, character, 4, &key, &characterLookupStatus);

					AvoGUI::KeyboardEvent keyboardEvent;
					keyboardEvent.isRepeated = lastKeyPressKeyCode == event.xkey.keycode && event.xkey.time < lastKeyPressTime + 2;
					if (characterLookupStatus == XLookupBoth || characterLookupStatus == XLookupChars)
					{
						keyboardEvent.character = character;
						m_gui->excludeAnimationThread();
						m_gui->handleGlobalCharacterInput(keyboardEvent);
						m_gui->includeAnimationThread();
					}
					if (characterLookupStatus == XLookupBoth || characterLookupStatus == XLookupKeySym)
					{
						keyboardEvent.character = "";
						keyboardEvent.key = convertKeySymToKeyboardKey(key);
						m_gui->excludeAnimationThread();
						m_gui->handleGlobalKeyboardKeyDown(keyboardEvent);
						m_gui->includeAnimationThread();
					}

					lastKeyPressTime = event.xkey.time;

					break;
				}
				case KeyRelease:
				{
					AvoGUI::KeyboardEvent keyboardEvent;

					// Try the four modifier groups until one matches
					for (uint32 a = 0; a < 4; a++)
					{
						if (AvoGUI::KeyboardKey::None != (keyboardEvent.key = convertKeySymToKeyboardKey(XLookupKeysym(&event.xkey, a))))
						{
							break;
						}
					}

					if (keyboardEvent.key != AvoGUI::KeyboardKey::None)
					{
						keyboardEvent.isRepeated = lastKeyPressKeyCode == event.xkey.keycode && event.xkey.time < lastKeyPressTime + 2;
						m_gui->excludeAnimationThread();
						m_gui->handleGlobalKeyboardKeyUp(keyboardEvent);
						m_gui->includeAnimationThread();
						lastKeyPressTime = event.xkey.time;
					}

					break;
				}
			} 
		}
		XDestroyIC(m_inputContext);
		XCloseIM(m_inputMethod);
		XFreeColormap(m_server, m_colormap);
		XDestroyWindow(m_server, m_windowHandle);
		XCloseDisplay(m_server);
	}

public:
	Display* getServer()
	{
		return m_server;
	}
	Colormap getColormap()
	{
		return m_colormap;
	}
	Atom getBackgroundColorMessageType()
	{
		return m_backgroundColorMessageType;
	}

	//------------------------------

	explicit LinuxWindow(AvoGUI::Gui* p_gui) :
		m_gui(p_gui)
	{

	}
	~LinuxWindow() final
	{
		if (m_messageThread.joinable())
		{
			m_messageThread.join();
		}
	}

	//------------------------------

	void create(char const* p_title, uint32 p_titleSize, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		m_crossPlatformStyles = p_styleFlags;
		m_messageThread = std::thread(&LinuxWindow::thread_runEventLoop, this, p_title, p_titleSize, p_x, p_y, p_width, p_height, p_parent);

		if (!m_hasCreatedWindow)
		{
			std::unique_lock<std::mutex> lock(m_hasCreatedWindowMutex);
			m_hasCreatedWindowConditionVariable.wait(lock, [=] { return (bool)m_hasCreatedWindow; });
		}
	}
	void create(char const* p_title, uint32 p_titleSize, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		create(p_title, p_titleSize, 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(char const* p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		create(p_title, strlen(p_title), p_x, p_y, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(char const* p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		create(p_title, strlen(p_title), 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(std::string const& p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		create(p_title.data(), p_title.size(), p_x, p_y, p_width, p_height, p_styleFlags, p_parent);
	}
	void create(std::string const& p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_styleFlags = AvoGUI::WindowStyleFlags::Default, AvoGUI::Window* p_parent = nullptr) override
	{
		create(p_title.data(), p_title.size(), 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
	}

	void close() override 
	{
		if (m_windowHandle)
		{
			XEvent event;
			event.type = ClientMessage;
			event.xclient.message_type = m_windowManagerProtocolsMessageType;
			event.xclient.data.l[0] = m_windowCloseEvent;
			event.xclient.display = m_server;
			event.xclient.window = m_windowHandle;
			event.xclient.format = 32;
			event.xclient.send_event = true;
			XSendEvent(m_server, m_windowHandle, false, 0, &event);
			XFlush(m_server);
		}
	}
	bool getIsOpen() const override
	{
		return m_isOpen;
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

	void setTitle(char const* p_title, uint32 p_size) override 
	{
		XTextProperty textProperty;
		#ifdef X_HAVE_UTF8_STRING
		textProperty.encoding = XInternAtom(m_server, "UTF8_STRING", 0);
		#else
		textProperty.encoding = XA_STRING;
		#endif
		textProperty.format = 8;
		textProperty.nitems = p_size;
		textProperty.value = (unsigned char*)p_title;
		XSetWMName(m_server, m_windowHandle, &textProperty);
		XSetWMIconName(m_server, m_windowHandle, &textProperty);
		XFlush(m_server);
	}
	void setTitle(char const* p_title) override
	{
		setTitle(p_title, strlen(p_title));
	}
	void setTitle(std::string const& p_title) override
	{
		setTitle(p_title.data(), p_title.size());
	}
	std::string getTitle() const override
	{
		XTextProperty textProperty;
		XGetWMName(m_server, m_windowHandle, &textProperty);
		return (char const*)textProperty.value;
	}

	//------------------------------

	void setStyles(AvoGUI::WindowStyleFlags p_styles) override 
	{
	}
	AvoGUI::WindowStyleFlags getStyles() const override
	{
	}

	//------------------------------

	void* getNativeHandle() const override
	{
		return (void*)m_windowHandle;
	}

	//------------------------------

	void setIsFullscreen(bool p_isFullscreen) override 
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

	void setState(AvoGUI::WindowState p_state) override 
	{
	}
	AvoGUI::WindowState getState() const override
	{
	}

	//------------------------------

	void setPosition(AvoGUI::Point<int32> const& p_position) override 
	{
		if (m_windowHandle)
		{
			XMoveWindow(m_server, m_windowHandle, p_position.x, p_position.y);
			XFlush(m_server);
		}
	}
	void setPosition(int32 p_x, int32 p_y) override 
	{
		if (m_windowHandle)
		{
			XMoveWindow(m_server, m_windowHandle, p_x, p_y);
			XFlush(m_server);
		}
	}
	AvoGUI::Point<int32> getPosition() const override
	{
		AvoGUI::Point<int32> result;
		XWindow child;
		XTranslateCoordinates(m_server, m_windowHandle, DefaultRootWindow(m_server), 0, 0, &result.x, &result.y, &child);
		return result;
	}
	int32 getPositionX() const override
	{
		int32 x;
		int32 y;
		XWindow child;
		XTranslateCoordinates(m_server, m_windowHandle, DefaultRootWindow(m_server), 0, 0, &x, &y, &child);
		return x;
	}
	int32 getPositionY() const override
	{
		int32 x;
		int32 y;
		XWindow child;
		XTranslateCoordinates(m_server, m_windowHandle, DefaultRootWindow(m_server), 0, 0, &x, &y, &child);
		return y;
	}

	void setSize(AvoGUI::Point<float> const& p_size) override 
	{
		if (m_windowHandle)
		{
			XResizeWindow(m_server, m_windowHandle, p_size.x*m_dipToPixelFactor, p_size.y*m_dipToPixelFactor);
			XFlush(m_server);
		}
	}
	void setSize(float p_width, float p_height) override 
	{
		if (m_windowHandle)
		{
			XResizeWindow(m_server, m_windowHandle, p_width*m_dipToPixelFactor, p_height*m_dipToPixelFactor);
			XFlush(m_server);
		}
	}
	AvoGUI::Point<float> getSize() const override
	{
		return m_size / m_dipToPixelFactor;
	}
	float getWidth() const override
	{
		return m_size.x / m_dipToPixelFactor;
	}
	float getHeight() const override
	{
		return m_size.y / m_dipToPixelFactor;
	}

	void setMinSize(AvoGUI::Point<float> const& p_minSize) override 
	{
		setMinSize(p_minSize.x, p_minSize.y);
	}
	void setMinSize(float p_minWidth, float p_minHeight) override 
	{
		XSizeHints sizeHints = { };
		sizeHints.flags = PMinSize;
		sizeHints.min_width = p_minWidth*m_dipToPixelFactor;
		sizeHints.min_height = p_minHeight*m_dipToPixelFactor; 
		XSetWMNormalHints(m_server, m_windowHandle, &sizeHints);
		m_minSize.set(p_minWidth, p_minHeight);
	}
	AvoGUI::Point<float> getMinSize() const override
	{
		return m_minSize;
	}
	float getMinWidth() const override
	{
		return m_minSize.x;
	}
	float getMinHeight() const override
	{
		return m_minSize.y;
	}

	void setMaxSize(AvoGUI::Point<float> const& p_maxSize) override 
	{
		setMaxSize(p_maxSize.x, p_maxSize.y);
	}
	void setMaxSize(float p_maxWidth, float p_maxHeight) override 
	{
		XSizeHints sizeHints = { };
		sizeHints.flags = PMaxSize;
		sizeHints.max_width = p_maxWidth*m_dipToPixelFactor;
		sizeHints.max_height = p_maxHeight*m_dipToPixelFactor;
		XSetWMNormalHints(m_server, m_windowHandle, &sizeHints);
		m_maxSize.set(p_maxWidth, p_maxHeight);
	}
	AvoGUI::Point<float> getMaxSize() const override
	{
		return m_maxSize;
	}
	float getMaxWidth() const override
	{
		return m_maxSize.x;
	}
	float getMaxHeight() const override
	{
		return m_maxSize.y;
	}

	//------------------------------

	AvoGUI::Rectangle<uint32> getMonitorBounds() const override
	{
	}
	AvoGUI::Point<uint32> getMonitorPosition() const override
	{
	}
	AvoGUI::Point<uint32> getMonitorSize() const override
	{
	}
	uint32 getMonitorWidth() const override
	{
	}
	uint32 getMonitorHeight() const override
	{
	}

	//------------------------------

	AvoGUI::Rectangle<uint32> getWorkAreaBounds() const override
	{
	}
	AvoGUI::Point<uint32> getWorkAreaPosition() const override
	{
	}
	AvoGUI::Point<uint32> getWorkAreaSize() const override
	{
	}
	uint32 getWorkAreaWidth() const override
	{
	}
	uint32 getWorkAreaHeight() const override
	{
	}

	//------------------------------

	bool getIsKeyDown(AvoGUI::KeyboardKey p_key) const override
	{
		KeySym keySym = convertKeyboardKeyToKeySym(p_key);
		KeyCode keyCode = XKeysymToKeycode(m_server, keySym);
		
		char keymap[32];
		XQueryKeymap(m_server, keymap);
		if (keyCode)
		{
			bool result = keymap[keyCode / 8] & 1 << keyCode % 8; // Look up operator precedence ;)
			if (!result && keySym == XK_Control_L)
			{
				return keymap[XKeysymToKeycode(m_server, XK_Control_R) / 8] & 1 << keyCode % 8;
			}
			if (!result && keySym == XK_Shift_L)
			{
				return keymap[XKeysymToKeycode(m_server, XK_Shift_R) / 8] & 1 << keyCode % 8;
			}
			if (!result && keySym == XK_Alt_L)
			{
				return keymap[XKeysymToKeycode(m_server, XK_Alt_R) / 8] & 1 << keyCode % 8;
			}
			return result;
		}
		return false;
	}
	bool getIsMouseButtonDown(AvoGUI::MouseButton p_button) const override
	{
	}
	AvoGUI::Point<float> getMousePosition() const override
	{
	}

	//------------------------------

	void setCursor(AvoGUI::Cursor p_cursor) override 
	{
		
	}
	AvoGUI::Cursor getCursor() const override
	{
	}

	//------------------------------

	float getDipToPixelFactor() const override
	{
		return m_dipToPixelFactor;
	}

	//------------------------------

	AvoGUI::DragDropOperation dragAndDropString(std::string const& p_string, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropString(char const* p_string, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropString(char const* p_string, uint32 p_length, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}

	AvoGUI::DragDropOperation dragAndDropString(std::wstring const& p_string, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropString(wchar_t const* p_string, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropString(wchar_t const* p_string, uint32 p_length, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}

	AvoGUI::DragDropOperation dragAndDropImage(AvoGUI::Image* p_image, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(char const* p_data, uint32 p_dataSize, std::string const& p_name, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(char const* p_data, uint32 p_dataSize, std::wstring const& p_name, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_data, std::string const& p_name, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_data, std::wstring const& p_name, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::string const& p_path, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFile(std::wstring const& p_path, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}

	AvoGUI::DragDropOperation dragAndDropFiles(std::vector<std::string> const& p_paths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::vector<std::wstring> const& p_paths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::string* p_paths, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFiles(std::wstring* p_paths, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFiles(char const* const* p_paths, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}
	AvoGUI::DragDropOperation dragAndDropFiles(wchar_t const* const* p_paths, uint32 p_numberOfPaths, AvoGUI::Image* p_dragImage = nullptr, AvoGUI::Point<float> const& p_dragImageCursorPosition = AvoGUI::Point<float>()) override
	{
	}

	//------------------------------

	void setClipboardString(std::wstring const& p_string) const override
	{
	}
	void setClipboardString(wchar_t const* p_string) const override
	{
	}
	void setClipboardString(wchar_t const* p_string, uint32 p_length) const override
	{
	}

	void setClipboardString(std::string const& p_string) const override
	{
	}
	void setClipboardString(char const* p_string) const override
	{
	}
	void setClipboardString(char const* p_string, uint32 p_length) const override
	{
	}

	void setClipboardImage(AvoGUI::Image* p_image) const override
	{
	}

	void setClipboardFile(char const* p_data, uint32 p_dataSize, std::string const& p_name) const override
	{
	}
	void setClipboardFile(char const* p_data, uint32 p_dataSize, std::wstring const& p_name) const override
	{
	}
	void setClipboardFile(std::string const& p_data, std::string const& p_name) const override
	{
	}
	void setClipboardFile(std::string const& p_data, std::wstring const& p_name) const override
	{
	}
	void setClipboardFile(std::string const& p_path) const override
	{
	}
	void setClipboardFile(std::wstring const& p_path) const override
	{
	}

	void setClipboardFiles(std::vector<std::string> const& p_paths) const override
	{
	}
	void setClipboardFiles(std::vector<std::wstring> const& p_paths) const override
	{
	}
	void setClipboardFiles(std::string* p_paths, uint32 p_numberOfPaths) const override
	{
	}
	void setClipboardFiles(std::wstring* p_paths, uint32 p_numberOfPaths) const override
	{
	}
	void setClipboardFiles(char const* const* p_paths, uint32 p_numberOfPaths) const override
	{
	}
	void setClipboardFiles(wchar_t const* const* p_paths, uint32 p_numberOfPaths) const override
	{
	}

	AvoGUI::ClipboardData* getClipboardData() const override
	{
	}
};
#endif
#pragma endregion

#pragma region Platform-specific drawing context implementations
#ifdef _WIN32
class Direct2DImage : public AvoGUI::Image
{
private:
	ID2D1Bitmap1* m_image;

	AvoGUI::ImageScalingMethod m_scalingMethod;
	AvoGUI::ImageBoundsSizing m_boundsSizing;
	AvoGUI::Point<float> m_boundsPositioning;

	AvoGUI::Rectangle<float> m_cropRectangle;
	float m_opacity;

public:
	Direct2DImage(ID2D1Bitmap1* p_image) :
		m_image(p_image), 
		m_scalingMethod(AvoGUI::ImageScalingMethod::Smooth), m_boundsSizing(AvoGUI::ImageBoundsSizing::Stretch), m_boundsPositioning(0.5f, 0.5f),
		m_cropRectangle(0.f, 0.f, p_image->GetSize().width, p_image->GetSize().height), m_opacity(1.f)
	{
		m_bounds = m_cropRectangle;
	}
	~Direct2DImage()
	{
		m_image->Release();
	}

	//------------------------------

	void setCropRectangle(AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		m_cropRectangle = p_rectangle;
	}
	AvoGUI::Rectangle<float> const& getCropRectangle() const override
	{
		return m_cropRectangle;
	}

	AvoGUI::Point<uint32> getOriginalSize() const override
	{
		return AvoGUI::Point<uint32>(m_image->GetSize().width, m_image->GetSize().height);
	}
	uint32 getOriginalWidth() const override
	{
		return m_image->GetSize().width;
	}
	uint32 getOriginalHeight() const override
	{
		return m_image->GetSize().height;
	}

	//------------------------------

	void setBoundsSizing(AvoGUI::ImageBoundsSizing p_sizeMode) override
	{
		m_boundsSizing = p_sizeMode;
	}
	AvoGUI::ImageBoundsSizing getBoundsSizing() const override
	{
		return m_boundsSizing;
	}

	void setBoundsPositioning(float p_x, float p_y) override
	{
		m_boundsPositioning.set(p_x, p_y);
	}
	void setBoundsPositioningX(float p_x) override
	{
		m_boundsPositioning.x = p_x;
	}
	void setBoundsPositioningY(float p_y) override
	{
		m_boundsPositioning.y = p_y;
	}
	AvoGUI::Point<float> const& getBoundsPositioning() const override
	{
		return m_boundsPositioning;
	}
	float getBoundsPositioningX() const override
	{
		return m_boundsPositioning.x;
	}
	float getBoundsPositioningY() const override
	{
		return m_boundsPositioning.x;
	}

	//------------------------------

	void setScalingMethod(AvoGUI::ImageScalingMethod p_scalingMethod) override
	{
		m_scalingMethod = p_scalingMethod;
	}
	AvoGUI::ImageScalingMethod getScalingMethod() const override
	{
		return m_scalingMethod;
	}

	//------------------------------

	void setOpacity(float p_opacity) override
	{
		m_opacity = p_opacity;
	}
	float getOpacity() const override
	{
		return m_opacity;
	}

	//------------------------------

	float getInnerWidth() const override
	{
		if (m_boundsSizing != AvoGUI::ImageBoundsSizing::Stretch &&
			m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() < m_image->GetSize().width / m_image->GetSize().height)
		{
			return m_bounds.getHeight() * m_image->GetSize().width / m_image->GetSize().height;
		}
		return m_bounds.getWidth();
	}
	float getInnerHeight() const override
	{
		if (m_boundsSizing != AvoGUI::ImageBoundsSizing::Stretch &&
			m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() > m_image->GetSize().width / m_image->GetSize().height)
		{
			return m_bounds.getWidth() * m_image->GetSize().height / m_image->GetSize().width;
		}
		return m_bounds.getHeight();
	}
	AvoGUI::Point<float> getInnerSize() const override
	{
		return AvoGUI::Point<float>(getInnerWidth(), getInnerHeight());
	}
	AvoGUI::Rectangle<float> getInnerBounds() const override
	{
		if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Stretch)
		{
			return m_bounds;
		}

		AvoGUI::Rectangle<float> innerBounds = m_bounds;

		bool areBoundsProportionallyWider = m_bounds.getWidth() / m_bounds.getHeight() > m_image->GetSize().width / m_image->GetSize().height;
		if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Fill != areBoundsProportionallyWider)
		{
			innerBounds.setWidth(m_bounds.getHeight() * m_image->GetSize().width / m_image->GetSize().height);
		}
		else if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != areBoundsProportionallyWider)
		{
			innerBounds.setHeight(m_bounds.getWidth() * m_image->GetSize().height / m_image->GetSize().width);
		}

		innerBounds.move(m_boundsPositioning.x * (m_bounds.getWidth() - innerBounds.getWidth()), m_boundsPositioning.y * (m_bounds.getHeight() - innerBounds.getHeight()));

		return innerBounds;
	}

	//------------------------------

	void* getHandle() const override
	{
		return m_image;
	}
};

//------------------------------

class DirectWriteText : public AvoGUI::Text
{
private:
	IDWriteTextLayout1* m_handle;
	std::wstring m_wideString;
	std::string m_string;
	bool m_isTopTrimmed;

	DWRITE_TEXT_RANGE createTextRange(int32 p_startPosition, int32 p_length)
	{
		DWRITE_TEXT_RANGE textRange = { };
		textRange.startPosition = p_length > 0 ? p_startPosition : AvoGUI::max(0, p_startPosition - p_length);
		textRange.length = p_length > 0 ? p_length : (p_length == 0 ? m_string.size() : -p_length);
		return textRange;
	}

public:
	DirectWriteText(IDWriteTextLayout1* p_handle, std::wstring const& p_wideString, std::string const& p_string, AvoGUI::Rectangle<float> const& p_bounds) :
		m_handle(p_handle), m_wideString(p_wideString), m_string(p_string), m_isTopTrimmed(false)
	{
		m_bounds = p_bounds;
		if (!m_bounds.right && !m_bounds.bottom)
		{
			m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
			fitSizeToText();
		}
		else
		{
			m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
		}
	}
	~DirectWriteText()
	{
		m_handle->Release();
	}

	//------------------------------

	void setWordWrapping(AvoGUI::WordWrapping p_wordWrapping) override
	{
		switch (p_wordWrapping)
		{
			case AvoGUI::WordWrapping::Always:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP);
				break;
			case AvoGUI::WordWrapping::Emergency:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
				break;
			case AvoGUI::WordWrapping::Never:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
				break;
			case AvoGUI::WordWrapping::WholeWord:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD);
		}
	}
	AvoGUI::WordWrapping getWordWrapping() override
	{
		switch (m_handle->GetWordWrapping())
		{
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP:
				return AvoGUI::WordWrapping::Always;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK:
				return AvoGUI::WordWrapping::Emergency;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP:
				return AvoGUI::WordWrapping::Never;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD:
				return AvoGUI::WordWrapping::WholeWord;
		}
		return AvoGUI::WordWrapping::Never;
	}

	void fitSizeToText() override
	{
		DWRITE_TEXT_METRICS metrics = { 0 };
		m_handle->GetMetrics(&metrics);

		DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
		m_handle->GetOverhangMetrics(&overhangMetrics);

		m_bounds.setSize(metrics.width, m_handle->GetMaxHeight() + overhangMetrics.bottom + m_isTopTrimmed*overhangMetrics.top);
	}
	void fitWidthToText() override
	{
		DWRITE_TEXT_METRICS metrics = { 0 };
		m_handle->GetMetrics(&metrics);
		m_bounds.setWidth(metrics.width);
	}
	void fitHeightToText() override
	{
		DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
		m_handle->GetOverhangMetrics(&overhangMetrics);

		m_bounds.setHeight(m_handle->GetMaxHeight() + overhangMetrics.bottom + m_isTopTrimmed * overhangMetrics.top);
	}
	AvoGUI::Point<float> getMinimumSize() override
	{
		DWRITE_TEXT_METRICS metrics = { 0 };
		m_handle->GetMetrics(&metrics);

		DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
		m_handle->GetOverhangMetrics(&overhangMetrics);

		return AvoGUI::Point<float>(metrics.width, m_handle->GetMaxHeight() + overhangMetrics.bottom + m_isTopTrimmed * overhangMetrics.top);
	}
	float getMinimumWidth() override
	{
		DWRITE_TEXT_METRICS metrics = { 0 };
		m_handle->GetMetrics(&metrics);
		return metrics.width;
	}
	float getMinimumHeight() override
	{
		DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
		m_handle->GetOverhangMetrics(&overhangMetrics);

		return m_handle->GetMaxHeight() + overhangMetrics.bottom + m_isTopTrimmed * overhangMetrics.top;
	}

	//------------------------------

	void setIsTopTrimmed(bool p_isTopTrimmed)
	{
		m_isTopTrimmed = p_isTopTrimmed;
	}
	bool getIsTopTrimmed()
	{
		return m_isTopTrimmed;
	}

	//------------------------------

	AvoGUI::Point<float> getCharacterPosition(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) override
	{
		AvoGUI::Point<float> result;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestTextPosition(AvoGUI::getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &result.x, &result.y, &metrics);
		if (p_isRelativeToOrigin)
		{
			result.x += getLeft();
			result.y += getTop();
		}
		return result;
	}
	AvoGUI::Point<float> getCharacterSize(uint32 p_characterIndex) override
	{
		float x;
		float y;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestTextPosition(AvoGUI::getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &x, &y, &metrics);
		return AvoGUI::Point<float>(metrics.width, metrics.height);
	}
	AvoGUI::Rectangle<float> getCharacterBounds(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) override
	{
		AvoGUI::Rectangle<float> result;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestTextPosition(AvoGUI::getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &result.left, &result.top, &metrics);
		if (p_isRelativeToOrigin)
		{
			result.left += getLeft();
			result.top += getTop();
		}
		result.right = result.left + metrics.width;
		result.bottom = result.top + metrics.height;
		return result;
	}
	uint32 getNearestCharacterIndex(AvoGUI::Point<float> const& p_point, bool p_isRelativeToOrigin = false) override
	{
		return getNearestCharacterIndex(p_point.x, p_point.y, p_isRelativeToOrigin);
	}
	uint32 getNearestCharacterIndex(float p_pointX, float p_pointY, bool p_isRelativeToOrigin = false) override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
		return AvoGUI::getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
	}
	void getNearestCharacterIndexAndPosition(AvoGUI::Point<float> const& p_point, uint32* p_outCharacterIndex, AvoGUI::Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) override
	{
		getNearestCharacterIndexAndPosition(p_point.x, p_point.y, p_outCharacterIndex, p_outCharacterPosition, p_isRelativeToOrigin);
	}
	void getNearestCharacterIndexAndPosition(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, AvoGUI::Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
		*p_outCharacterIndex = AvoGUI::getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
		p_outCharacterPosition->set(metrics.left + isTrailingHit * metrics.width + p_isRelativeToOrigin * getLeft(), metrics.top + p_isRelativeToOrigin * getTop());
	}
	void getNearestCharacterIndexAndBounds(AvoGUI::Point<float> const& p_point, uint32* p_outCharacterIndex, AvoGUI::Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) override
	{
		getNearestCharacterIndexAndBounds(p_point.x, p_point.y, p_outCharacterIndex, p_outCharacterBounds, p_isRelativeToOrigin);
	}
	void getNearestCharacterIndexAndBounds(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, AvoGUI::Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) override
	{
		int isTrailingHit;
		int isInside;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
		*p_outCharacterIndex = AvoGUI::getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
		p_outCharacterBounds->left = metrics.left + isTrailingHit * metrics.width + p_isRelativeToOrigin * getLeft();
		p_outCharacterBounds->top = metrics.top + p_isRelativeToOrigin * getTop();
		p_outCharacterBounds->right = p_outCharacterBounds->left + metrics.width;
		p_outCharacterBounds->bottom = p_outCharacterBounds->top + metrics.height;
	}

	//------------------------------

	void setTextAlign(AvoGUI::TextAlign p_textAlign) override
	{
		switch (p_textAlign)
		{
			case AvoGUI::TextAlign::Left:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case AvoGUI::TextAlign::Center:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case AvoGUI::TextAlign::Right:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			case AvoGUI::TextAlign::Fill:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
				break;
		}
	}
	AvoGUI::TextAlign getTextAlign() override
	{
		switch (m_handle->GetTextAlignment())
		{
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING:
				return AvoGUI::TextAlign::Left;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER:
				return AvoGUI::TextAlign::Center;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING:
				return AvoGUI::TextAlign::Right;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED:
				return AvoGUI::TextAlign::Fill;
		}
	}

	void setReadingDirection(AvoGUI::ReadingDirection p_readingDirection) override
	{
		switch (p_readingDirection)
		{
			case AvoGUI::ReadingDirection::LeftToRight:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
				break;
			case AvoGUI::ReadingDirection::RightToLeft:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
				break;
			case AvoGUI::ReadingDirection::TopToBottom:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM);
				break;
			case AvoGUI::ReadingDirection::BottomToTop:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP);
				break;
		}
	}
	AvoGUI::ReadingDirection getReadingDirection() override
	{
		switch (m_handle->GetReadingDirection())
		{
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT:
				return AvoGUI::ReadingDirection::LeftToRight;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT:
				return AvoGUI::ReadingDirection::RightToLeft;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM:
				return AvoGUI::ReadingDirection::TopToBottom;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP:
				return AvoGUI::ReadingDirection::BottomToTop;
		}
	}

	//------------------------------

	void setBounds(AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		m_bounds = p_rectangle;
		if (p_rectangle.right - p_rectangle.left != m_bounds.right - m_bounds.left ||
			p_rectangle.bottom - p_rectangle.top != m_bounds.bottom - m_bounds.top)
		{
			m_handle->SetMaxWidth(getWidth());
			m_handle->SetMaxHeight(getHeight());
		}
	}
	void setBounds(float p_left, float p_top, float p_right, float p_bottom) override
	{
		m_bounds.left = p_left;
		m_bounds.top = p_top;
		m_bounds.right = p_right;
		m_bounds.bottom = p_bottom;
		if (p_right - p_left != m_bounds.right - m_bounds.left ||
			p_bottom - p_top != m_bounds.bottom - m_bounds.top)
		{
			m_handle->SetMaxWidth(getWidth());
			m_handle->SetMaxHeight(getHeight());
		}
	}
	void setBounds(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size) override
	{
		m_bounds.left = p_position.x;
		m_bounds.top = p_position.y;
		m_bounds.right = p_position.x + p_size.x;
		m_bounds.bottom = p_position.y + p_size.y;
		if (p_size.x != m_bounds.right - m_bounds.left ||
			p_size.y != m_bounds.bottom - m_bounds.top)
		{
			m_handle->SetMaxWidth(getWidth());
			m_handle->SetMaxHeight(getHeight());
		}
	}
	AvoGUI::Rectangle<float> const& getBounds() const override
	{
		return m_bounds;
	}

	//------------------------------

	void move(AvoGUI::Point<float> const& p_offset) override
	{
		m_bounds += p_offset;
	}
	void move(float p_offsetX, float p_offsetY) override
	{
		m_bounds.move(p_offsetX, p_offsetY);
	}

	//------------------------------

	void setTopLeft(AvoGUI::Point<float> const& p_position, bool p_willKeepSize = true) override
	{
		if (p_position.x != m_bounds.left || p_position.y != m_bounds.top)
		{
			m_bounds.setTopLeft(p_position, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) override
	{
		if (p_left != m_bounds.left || p_top != m_bounds.top)
		{
			m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	AvoGUI::Point<float> getTopLeft() const override
	{
		return AvoGUI::Point<float>(m_bounds.left, m_bounds.top);
	}

	void setTopRight(AvoGUI::Point<float> const& p_position, bool p_willKeepSize = true) override
	{
		if (p_position.x != m_bounds.right || p_position.y != m_bounds.top)
		{
			m_bounds.setTopRight(p_position, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) override
	{
		if (p_right != m_bounds.right || p_top != m_bounds.top)
		{
			m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	AvoGUI::Point<float> getTopRight() const override
	{
		return AvoGUI::Point<float>(m_bounds.right, m_bounds.top);
	}

	void setBottomLeft(AvoGUI::Point<float> const& p_position, bool p_willKeepSize = true) override
	{
		if (p_position.x != m_bounds.left || p_position.y != m_bounds.bottom)
		{
			m_bounds.setBottomLeft(p_position, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) override
	{
		if (p_left != m_bounds.left || p_bottom != m_bounds.bottom)
		{
			m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	AvoGUI::Point<float> getBottomLeft() const override
	{
		return AvoGUI::Point<float>(m_bounds.left, m_bounds.bottom);
	}

	void setBottomRight(AvoGUI::Point<float> const& p_position, bool p_willKeepSize = true) override
	{
		if (p_position.x != m_bounds.right || p_position.y != m_bounds.bottom)
		{
			m_bounds.setBottomRight(p_position, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) override
	{
		if (p_right != m_bounds.right || p_bottom != m_bounds.bottom)
		{
			m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
			if (!p_willKeepSize)
			{
				m_handle->SetMaxWidth(getWidth());
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	AvoGUI::Point<float> getBottomRight() const override
	{
		return AvoGUI::Point<float>(m_bounds.right, m_bounds.bottom);
	}

	//------------------------------

	void setCenter(AvoGUI::Point<float> const& p_position) override
	{
		m_bounds.setCenter(p_position.x, p_position.y);
	}
	void setCenter(float p_x, float p_y) override
	{
		m_bounds.setCenter(p_x, p_y);
	}
	void setCenterX(float p_x) override
	{
		m_bounds.setCenterX(p_x);
	}
	void setCenterY(float p_y) override
	{
		m_bounds.setCenterY(p_y);
	}
	AvoGUI::Point<float> getCenter() const override
	{
		return m_bounds.getCenter();
	}
	float getCenterX() const override
	{
		return m_bounds.getCenterX();
	}
	float getCenterY() const override
	{
		return m_bounds.getCenterY();
	}

	//------------------------------

	void setLeft(float p_left, bool p_willKeepWidth = true) override
	{
		if (p_left != m_bounds.left)
		{
			m_bounds.setLeft(p_left, p_willKeepWidth);
			if (!p_willKeepWidth)
			{
				m_handle->SetMaxWidth(getWidth());
			}
		}
	}
	float getLeft() const override
	{
		return m_bounds.left;
	}

	void setTop(float p_top, bool p_willKeepHeight = true) override
	{
		if (p_top != m_bounds.top)
		{
			m_bounds.setTop(p_top, p_willKeepHeight);
			if (!p_willKeepHeight)
			{
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	float getTop() const override
	{
		return m_bounds.top;
	}

	void setRight(float p_right, bool p_willKeepWidth = true) override
	{
		if (p_right != m_bounds.right)
		{
			m_bounds.setRight(p_right, p_willKeepWidth);
			if (!p_willKeepWidth)
			{
				m_handle->SetMaxWidth(getWidth());
			}
		}
	}
	float getRight() const override
	{
		return m_bounds.right;
	}

	void setBottom(float p_bottom, bool p_willKeepHeight = true) override
	{
		if (p_bottom != m_bounds.bottom)
		{
			m_bounds.setBottom(p_bottom, p_willKeepHeight);
			if (!p_willKeepHeight)
			{
				m_handle->SetMaxHeight(getHeight());
			}
		}
	}
	float getBottom() const override
	{
		return m_bounds.bottom;
	}

	//------------------------------

	void setWidth(float p_width) override
	{
		if (p_width != m_bounds.right - m_bounds.left)
		{
			m_bounds.setWidth(p_width);
			m_handle->SetMaxWidth(getWidth());
		}
	}
	float getWidth() const override
	{
		return m_bounds.right - m_bounds.left;
	}

	void setHeight(float p_height) override
	{
		if (p_height != m_bounds.bottom - m_bounds.top)
		{
			m_bounds.setHeight(p_height);
			m_handle->SetMaxHeight(getHeight());
		}
	}
	float getHeight() const override
	{
		return m_bounds.bottom - m_bounds.top;
	}

	void setSize(AvoGUI::Point<float> const& p_size) override
	{
		if (p_size.x != m_bounds.right - m_bounds.left || p_size.y != m_bounds.bottom - m_bounds.top)
		{
			m_bounds.setSize(p_size);
			m_handle->SetMaxWidth(getWidth());
			m_handle->SetMaxHeight(getHeight());
		}
	}
	void setSize(float p_width, float p_height) override
	{
		if (p_width != m_bounds.right - m_bounds.left || p_height != m_bounds.bottom - m_bounds.top)
		{
			m_bounds.setSize(p_width, p_height);
			m_handle->SetMaxWidth(getWidth());
			m_handle->SetMaxHeight(getHeight());
		}
	}
	AvoGUI::Point<float> getSize() const override
	{
		return AvoGUI::Point<float>(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top);
	}

	//------------------------------

	bool getIsIntersecting(AvoGUI::Rectangle<float> const& p_rectangle) const override
	{
		return m_bounds.getIsIntersecting(p_rectangle);
	}
	bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
	{
		return m_bounds.getIsIntersecting(p_protectedRectangle->getBounds());
	}

	//------------------------------

	bool getIsContaining(AvoGUI::Rectangle<float> const& p_rectangle) const override
	{
		return m_bounds.getIsContaining(p_rectangle);
	}
	bool getIsContaining(ProtectedRectangle* p_view) const override
	{
		return m_bounds.getIsContaining(p_view->getBounds());
	}

	bool getIsContaining(float p_x, float p_y) const override
	{
		return m_bounds.getIsContaining(p_x, p_y);
	}
	bool getIsContaining(AvoGUI::Point<float> const& p_point) const override
	{
		return m_bounds.getIsContaining(p_point);
	}

	//------------------------------

	void setFontFamily(std::string const& p_name, int32 p_startPosition, int32 p_length) override
	{
		wchar_t wideName[100];
		AvoGUI::convertUtf8ToUtf16(p_name, wideName, 100);

		m_handle->SetFontFamilyName(wideName, createTextRange(p_startPosition, p_length));
	}

	//------------------------------

	void setCharacterSpacing(float p_leading, float p_trailing, int32 p_startPosition, int32 p_length) override
	{
		m_handle->SetCharacterSpacing(p_leading, p_trailing, 0.f, createTextRange(p_startPosition, p_length));
	}
	void setCharacterSpacing(float p_characterSpacing, int32 p_startPosition, int32 p_length) override
	{
		m_handle->SetCharacterSpacing(p_characterSpacing * 0.5f, p_characterSpacing * 0.5f, 0.f, createTextRange(p_startPosition, p_length));
	}
	float getLeadingCharacterSpacing(int32 p_characterIndex) override
	{
		float leadingSpacing = 0.f;
		m_handle->GetCharacterSpacing(p_characterIndex, &leadingSpacing, 0, 0);
		return leadingSpacing;
	}
	float getTrailingCharacterSpacing(int32 p_characterIndex) override
	{
		float trailingSpacing = 0.f;
		m_handle->GetCharacterSpacing(p_characterIndex, 0, &trailingSpacing, 0);
		return trailingSpacing;
	}

	//------------------------------

	void setLineHeight(float p_lineHeight)
	{
		m_handle->SetLineSpacing(DWRITE_LINE_SPACING_METHOD::DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, p_lineHeight, p_lineHeight * 0.8f);
	}
	float getLineHeight()
	{
		DWRITE_LINE_SPACING_METHOD method;
		float height;
		float baseline;
		m_handle->GetLineSpacing(&method, &height, &baseline);
		return height;
	}

	//------------------------------

	void setFontWeight(AvoGUI::FontWeight p_fontWeight, int32 p_startPosition, int32 p_length) override
	{
		m_handle->SetFontWeight((DWRITE_FONT_WEIGHT)p_fontWeight, createTextRange(p_startPosition, p_length));
	}
	AvoGUI::FontWeight getFontWeight(uint32 p_characterPosition) override
	{
		DWRITE_FONT_WEIGHT fontWeight;
		m_handle->GetFontWeight(p_characterPosition, &fontWeight);
		return (AvoGUI::FontWeight)fontWeight;
	}

	//------------------------------

	void setFontStyle(AvoGUI::FontStyle p_fontStyle, int32 p_startPosition = 0, int32 p_length = 0) override
	{
		m_handle->SetFontStyle((DWRITE_FONT_STYLE)p_fontStyle, createTextRange(p_startPosition, p_length));
	}
	AvoGUI::FontStyle getFontStyle(uint32 p_characterPosition) override
	{
		DWRITE_FONT_STYLE directwriteFontStyle;
		m_handle->GetFontStyle(p_characterPosition, &directwriteFontStyle);
		return (AvoGUI::FontStyle)directwriteFontStyle;
	}

	//------------------------------

	void setFontStretch(AvoGUI::FontStretch p_fontStretch, int32 p_startPosition = 0, int32 p_length = 0) override
	{
		m_handle->SetFontStretch((DWRITE_FONT_STRETCH)p_fontStretch, createTextRange(p_startPosition, p_length));
	}
	AvoGUI::FontStretch getFontStretch(uint32 p_characterPosition)
	{
		DWRITE_FONT_STRETCH fontStretch;
		m_handle->GetFontStretch(p_characterPosition, &fontStretch);
		return (AvoGUI::FontStretch)fontStretch;
	}

	//------------------------------

	void setFontSize(float p_fontSize, int32 p_startPosition, int32 p_length) override
	{
		m_handle->SetFontSize(p_fontSize, createTextRange(p_startPosition, p_length));
	}
	float getFontSize(uint32 p_characterPosition) override
	{
		float fontSize = 0.f;
		m_handle->GetFontSize(p_characterPosition, &fontSize);
		return fontSize;
	}

	//------------------------------

	std::string const& getString() override
	{
		return m_string;
	}

	//------------------------------

	void* getHandle() override
	{
		return m_handle;
	}
};

//class FontData
//{
//public:
//	void const* data;
//	uint32 dataSize;
//
//	FontData(void const* p_data, uint32 p_dataSize) :
//		data(p_data), dataSize(p_dataSize)
//	{ }
//};
using FontData = std::shared_ptr<std::vector<uint8>>;

class FontFileStream : public IDWriteFontFileStream
{
private:
	ULONG m_referenceCount;
	FontData m_fontData;

public:
	FontFileStream(FontData p_fontData) : 
		m_referenceCount(1), m_fontData(p_fontData)
	{ 
	}

	//------------------------------

	IUnknownDefinition(IDWriteFontFileStream)

	//------------------------------

	HRESULT __stdcall ReadFileFragment(void const** p_fragment, UINT64 p_fileOffset, UINT64 p_fragmentSize, void** p_fragmentContext) override
	{
		if (p_fileOffset + p_fragmentSize > m_fontData->size() || !p_fragmentSize)
		{
			*p_fragment = 0;
			*p_fragmentContext = 0;
			return E_FAIL;
		}

		*p_fragment = (char const*)m_fontData->data() + p_fileOffset;
		*p_fragmentContext = 0;

		return S_OK;
	}
	void __stdcall ReleaseFileFragment(void* p_fragmentContext) override { }

	HRESULT __stdcall GetFileSize(UINT64* p_fileSize) override
	{
		*p_fileSize = m_fontData->size();
		return S_OK;
	}
	HRESULT __stdcall GetLastWriteTime(UINT64* p_lastWriteTime) override
	{
		*p_lastWriteTime = 0;
		return E_NOTIMPL;
	}
};

class FontFileLoader : public IDWriteFontFileLoader
{
private:
	uint32 m_referenceCount;

public:
	FontFileLoader() :
		m_referenceCount(1)
	{ 
	}

	//------------------------------

	IUnknownDefinition(IDWriteFontFileLoader)

	//------------------------------

	HRESULT __stdcall CreateStreamFromKey(void const* p_data, UINT32 p_dataSize, IDWriteFontFileStream** p_stream)
	{
		if (p_dataSize != sizeof(FontData) || !p_data)
		{
			*p_stream = 0;
			return E_INVALIDARG;
		}
		*p_stream = new FontFileStream(*(FontData*)p_data);
		return S_OK;
	}
};
class FontFileEnumerator : public IDWriteFontFileEnumerator
{
private:
	uint32 m_referenceCount;

	//------------------------------

	IDWriteFactory* m_factory;
	FontFileLoader* m_fontFileLoader;

	std::vector<FontData>* m_fontData;
	IDWriteFontFile* m_currentFontFile = 0;
	int32 m_currentFontFileIndex;

public:
	FontFileEnumerator(IDWriteFactory* p_factory, FontFileLoader* p_fontFileLoader, std::vector<FontData>* p_data) :
		m_referenceCount(1), m_factory(p_factory), m_fontFileLoader(p_fontFileLoader), m_fontData(p_data),
		m_currentFontFileIndex(-1)
	{
	}

	//------------------------------

	IUnknownDefinition(IDWriteFontFileEnumerator)

	//------------------------------

	HRESULT __stdcall GetCurrentFontFile(IDWriteFontFile** p_fontFile) override
	{
		*p_fontFile = m_currentFontFile;
		if (m_currentFontFile)
		{
			return S_OK;
		}
		return E_FAIL;
	}
	HRESULT __stdcall MoveNext(BOOL* p_hasCurrentFile) override
	{
		m_currentFontFileIndex++;

		if (m_currentFontFileIndex >= m_fontData->size())
		{
			*p_hasCurrentFile = 0;
			m_currentFontFile = 0;
		}
		else
		{
			*p_hasCurrentFile = 1;
			m_factory->CreateCustomFontFileReference((void const*)(m_fontData->data() + m_currentFontFileIndex), sizeof(FontData), m_fontFileLoader, &m_currentFontFile);
		}
		return S_OK;
	}
};
class FontCollectionLoader : public IDWriteFontCollectionLoader
{
private:
	uint32 m_referenceCount;
	FontFileLoader* m_fontFileLoader;

public:
	FontCollectionLoader(FontFileLoader* p_fontFileLoader) :
		m_referenceCount(1), m_fontFileLoader(p_fontFileLoader)
	{
	}

	//------------------------------

	IUnknownDefinition(IDWriteFontCollectionLoader)

	//------------------------------

	HRESULT __stdcall CreateEnumeratorFromKey(IDWriteFactory* p_factory, void const* p_data, UINT32 p_dataSize, IDWriteFontFileEnumerator** p_fontFileEnumerator)
	{
		*p_fontFileEnumerator = new FontFileEnumerator(p_factory, m_fontFileLoader, (std::vector<FontData>*)p_data);
		return S_OK;
	}
};

//------------------------------

class Direct2DGeometry : 
	public AvoGUI::Geometry
{
private:
	ID2D1Geometry* m_geometry;
	ID2D1GeometryRealization* m_strokedRealization{ nullptr };
	ID2D1GeometryRealization* m_filledRealization{ nullptr };

public:
	Direct2DGeometry(ID2D1Geometry* p_geometry) :
		m_geometry(p_geometry)
	{
	}
	~Direct2DGeometry()
	{
		if (m_strokedRealization)
		{
			m_strokedRealization->Release();
		}
		if (m_filledRealization)
		{
			m_filledRealization->Release();
		}
		if (m_geometry)
		{
			m_geometry->Release();
		}
	}

	void setStrokedRealization(ID2D1GeometryRealization* p_realization)
	{
		m_strokedRealization = p_realization;
	}
	void setFilledRealization(ID2D1GeometryRealization* p_realization)
	{
		m_filledRealization = p_realization;
	}

	ID2D1Geometry* getGeometry()
	{
		return m_geometry;
	}
	ID2D1GeometryRealization* getStrokedRealization()
	{
		return m_strokedRealization;
	}
	ID2D1GeometryRealization* getFilledRealization()
	{
		return m_filledRealization;
	}
};

//------------------------------

class Direct2DLinearGradient :
	public AvoGUI::LinearGradient
{
private:
	ID2D1LinearGradientBrush* m_brush;

public:
	Direct2DLinearGradient(ID2D1LinearGradientBrush* p_brush) :
		m_brush(p_brush)
	{
	}
	~Direct2DLinearGradient()
	{
		m_brush->Release();
	}

	//------------------------------

	ID2D1LinearGradientBrush* getHandle()
	{
		return m_brush;
	}

	//------------------------------

	void setOffset(AvoGUI::Point<float> const& p_offset) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_offset.x, p_offset.y));
	}
	void setOffset(float p_x, float p_y) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, p_y));
	}
	void setOffsetX(float p_x) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, getOffsetY()));
	}
	void setOffsetY(float p_y) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(getOffsetX(), p_y));
	}
	AvoGUI::Point<float> const& getOffset() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return AvoGUI::Point<float>(matrix._31, matrix._32);
	}
	float getOffsetX() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return matrix._31;
	}
	float getOffsetY() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return matrix._32;
	}

	void setStartPosition(AvoGUI::Point<float> const& p_startPosition) override
	{
		m_brush->SetStartPoint(D2D1::Point2F(p_startPosition.x, p_startPosition.y));
	}
	void setStartPosition(float p_x, float p_y) override
	{
		m_brush->SetStartPoint(D2D1::Point2F(p_x, p_y));
	}
	AvoGUI::Point<float> const& getStartPosition() const override
	{
		return AvoGUI::Point<float>(m_brush->GetStartPoint().x, m_brush->GetStartPoint().y);
	}
	float getStartPositionX() const override
	{
		return m_brush->GetStartPoint().x;
	}
	float getStartPositionY() const override
	{
		return m_brush->GetStartPoint().y;
	}

	void setEndPosition(AvoGUI::Point<float> const& p_endPosition) override
	{
		m_brush->SetEndPoint(D2D1::Point2F(p_endPosition.x, p_endPosition.y));
	}
	void setEndPosition(float p_x, float p_y) override
	{
		m_brush->SetEndPoint(D2D1::Point2F(p_x, p_y));
	}
	AvoGUI::Point<float> const& getEndPosition() const override
	{
		return AvoGUI::Point<float>(m_brush->GetEndPoint().x, m_brush->GetEndPoint().y);
	}
	float getEndPositionX() const override
	{
		return m_brush->GetEndPoint().x;
	}
	float getEndPositionY() const override
	{
		return m_brush->GetEndPoint().y;
	}
};

//------------------------------

class Direct2DRadialGradient :
	public AvoGUI::RadialGradient
{
private:
	ID2D1RadialGradientBrush* m_brush;

public:
	Direct2DRadialGradient(ID2D1RadialGradientBrush* p_brush) :
		m_brush(p_brush)
	{
	}
	~Direct2DRadialGradient()
	{
		m_brush->Release();
	}

	//------------------------------

	ID2D1RadialGradientBrush* getHandle()
	{
		return m_brush;
	}

	//------------------------------

	void setOffset(AvoGUI::Point<float> const& p_offset) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_offset.x, p_offset.y));
	}
	void setOffset(float p_x, float p_y) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, p_y));
	}
	void setOffsetX(float p_x) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(p_x, getOffsetY()));
	}
	void setOffsetY(float p_y) override
	{
		m_brush->SetTransform(D2D1::Matrix3x2F::Translation(getOffsetX(), p_y));
	}
	AvoGUI::Point<float> const& getOffset() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return AvoGUI::Point<float>(matrix._31, matrix._32);
	}
	float getOffsetX() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return matrix._31;
	}
	float getOffsetY() const override
	{
		D2D1::Matrix3x2F matrix;
		m_brush->GetTransform(&matrix);
		return matrix._32;
	}
	void setStartPosition(AvoGUI::Point<float> const& p_startPosition) override
	{
		m_brush->SetCenter(D2D1::Point2F(p_startPosition.x, p_startPosition.y));
	}
	void setStartPosition(float p_x, float p_y) override
	{
		m_brush->SetCenter(D2D1::Point2F(p_x, p_y));
	}
	AvoGUI::Point<float> const& getStartPosition() const override
	{
		return AvoGUI::Point<float>(m_brush->GetCenter().x, m_brush->GetCenter().y);
	}
	float getStartPositionX() const override
	{
		return m_brush->GetCenter().x;
	}
	float getStartPositionY() const override
	{
		return m_brush->GetCenter().y;
	}

	void setRadius(float p_radius) override
	{
		m_brush->SetRadiusX(p_radius);
		m_brush->SetRadiusY(p_radius);
	}
	void setRadius(AvoGUI::Point<float> const& p_radius) override
	{
		m_brush->SetRadiusX(p_radius.x);
		m_brush->SetRadiusY(p_radius.y);
	}
	void setRadius(float p_radiusX, float p_radiusY) override
	{
		m_brush->SetRadiusX(p_radiusX);
		m_brush->SetRadiusY(p_radiusY);
	}
	AvoGUI::Point<float> const& getRadius() const override
	{
		return AvoGUI::Point<float>(m_brush->GetRadiusX(), m_brush->GetRadiusY());
	}
	float getRadiusX() const override
	{
		return m_brush->GetRadiusX();
	}
	float getRadiusY() const override
	{
		return m_brush->GetRadiusY();
	}
};

//------------------------------

class Direct2DDrawingState : AvoGUI::DrawingState
{
private:
	ID2D1DrawingStateBlock1* m_drawingState;

public:
	Direct2DDrawingState(ID2D1DrawingStateBlock1* p_drawingState) :
		m_drawingState(p_drawingState)
	{
	}
	~Direct2DDrawingState()
	{
		m_drawingState->Release();
	}

	ID2D1DrawingStateBlock1* getHandle()
	{
		return m_drawingState;
	}
};

//------------------------------

class Direct2DDrawingContext : 
	public AvoGUI::DrawingContext
{
public:
	static IWICImagingFactory2* s_imagingFactory;
	static ID2D1Factory2* s_direct2DFactory;
	static IDWriteFactory1* s_directWriteFactory;

	static FontCollectionLoader* s_fontCollectionLoader;
	static FontFileLoader* s_fontFileLoader;

	static void createStaticResources()
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
			options.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_INFORMATION;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &s_direct2DFactory);
#else
			D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &s_direct2DFactory);
#endif
		}

		if (!s_directWriteFactory)
		{
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(s_directWriteFactory), (IUnknown**)&s_directWriteFactory
			);

			s_fontFileLoader = new FontFileLoader();
			s_directWriteFactory->RegisterFontFileLoader(s_fontFileLoader);

			s_fontCollectionLoader = new FontCollectionLoader(s_fontFileLoader);
			s_directWriteFactory->RegisterFontCollectionLoader(s_fontCollectionLoader);
		}
	}
	static void destroyStaticResources()
	{
		if (!WindowsWindow::s_numberOfWindows)
		{
			if (s_fontCollectionLoader)
			{
				if (s_directWriteFactory)
				{
					s_directWriteFactory->UnregisterFontCollectionLoader(s_fontCollectionLoader);
				}
				s_fontCollectionLoader->Release();
				s_fontCollectionLoader = 0;
			}

			if (s_fontFileLoader)
			{
				if (s_directWriteFactory)
				{
					s_directWriteFactory->UnregisterFontFileLoader(s_fontFileLoader);
				}
				s_fontFileLoader->Release();
				s_fontFileLoader = 0;
			}

			if (s_directWriteFactory)
			{
				s_directWriteFactory->Release();
				s_directWriteFactory = 0;
			}
			if (s_direct2DFactory)
			{
				s_direct2DFactory->Release();
				s_direct2DFactory = 0;
			}
			if (s_imagingFactory)
			{
				s_imagingFactory->Release();
				s_imagingFactory = 0;
			}
		}
	}

private:
	AvoGUI::Window* m_window{ nullptr };

	ID2D1DeviceContext1* m_context{ nullptr };
	IDXGISwapChain1* m_swapChain{ nullptr };
	ID2D1Bitmap1* m_targetWindowBitmap{ nullptr };
	bool m_isVsyncEnabled{ true };

	std::stack<bool> m_clipTypeStack;

	ID2D1SolidColorBrush* m_solidColorBrush{ nullptr };
	ID2D1Brush* m_currentBrush{ nullptr };
	float m_brushOpacity{ 1.f };

	D2D1_STROKE_STYLE_PROPERTIES1 m_strokeStyleProperties;
	ID2D1StrokeStyle1* m_strokeStyle = 0;

	AvoGUI::Point<float> m_scale{ 1.f, 1.f };

	IDWriteTextFormat* m_textFormat = 0;
	IDWriteFontCollection* m_fontCollection = 0;
	std::vector<FontData> m_fontData;

	std::recursive_mutex m_targetMutex;

	//------------------------------

	void createCornerRectangleGeometry(ID2D1PathGeometry1* p_geometry, float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, bool p_isFilled)
	{
		ID2D1GeometrySink* sink;
		p_geometry->Open(&sink);

		sink->SetFillMode(D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING);
		sink->BeginFigure(D2D1::Point2F(p_left, p_top + p_corners.topLeftSizeY), p_isFilled ? D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW);

		if (p_corners.topLeftSizeX && p_corners.topLeftSizeY)
		{
			if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(p_left + p_corners.topLeftSizeX, p_top), D2D1::SizeF(p_corners.topLeftSizeX, p_corners.topLeftSizeY), 
						0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine(D2D1::Point2F(p_left + p_corners.topLeftSizeX, p_top));
			}
		}
		sink->AddLine(D2D1::Point2F(p_right - p_corners.topRightSizeX, p_top));
		if (p_corners.topRightSizeX && p_corners.topRightSizeY)
		{
			if (p_corners.topRightType == AvoGUI::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(p_right, p_top + p_corners.topRightSizeY), D2D1::SizeF(p_corners.topRightSizeX, p_corners.topRightSizeY), 
						0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine(D2D1::Point2F(p_right, p_top + p_corners.topRightSizeY));
			}
		}
		sink->AddLine(D2D1::Point2F(p_right, p_bottom - p_corners.bottomRightSizeY));
		if (p_corners.bottomRightSizeX && p_corners.bottomRightSizeY)
		{
			if (p_corners.bottomRightType == AvoGUI::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(p_right - p_corners.bottomRightSizeX, p_bottom), D2D1::SizeF(p_corners.bottomRightSizeX, p_corners.bottomRightSizeY), 
						0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine(D2D1::Point2F(p_right - p_corners.bottomRightSizeX, p_bottom));
			}
		}
		sink->AddLine(D2D1::Point2F(p_left + p_corners.bottomLeftSizeX, p_bottom));
		if (p_corners.bottomLeftSizeX && p_corners.bottomLeftSizeY)
		{
			if (p_corners.bottomLeftType == AvoGUI::RectangleCornerType::Round)
			{
				sink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(p_left, p_bottom - p_corners.bottomLeftSizeY), D2D1::SizeF(p_corners.bottomLeftSizeX, p_corners.bottomLeftSizeY), 
						0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL
					)
				);
			}
			else
			{
				sink->AddLine(D2D1::Point2F(p_left, p_bottom - p_corners.bottomLeftSizeY));
			}
		}
		sink->AddLine(D2D1::Point2F(p_left, p_top + p_corners.topLeftSizeY));

		sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
		sink->Close();
		sink->Release();
	}
	void updateStrokeStyle()
	{
		if (m_strokeStyle)
		{
			m_strokeStyle->Release();
		}
		s_direct2DFactory->CreateStrokeStyle(m_strokeStyleProperties, 0, 0, &m_strokeStyle);
	}

	void updateFontCollection()
	{
		if (m_fontCollection)
		{
			m_fontCollection->Release();
		}
		//std::vector<FontData>* fontDataPointer = &m_fontData;
		s_directWriteFactory->CreateCustomFontCollection(s_fontCollectionLoader, &m_fontData, sizeof(std::vector<FontData>), &m_fontCollection);
	}

	void realizeStrokedGeometry(Direct2DGeometry* p_geometry, float p_strokeWidth)
	{
		if (!p_geometry->getStrokedRealization())
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);

			float dpiX = USER_DEFAULT_SCREEN_DPI;
			float dpiY = USER_DEFAULT_SCREEN_DPI;
			m_context->GetDpi(&dpiX, &dpiY);

			ID2D1GeometryRealization* geometryRealization = 0;
			m_context->CreateStrokedGeometryRealization(p_geometry->getGeometry(), D2D1::ComputeFlatteningTolerance(transform, dpiX, dpiY), p_strokeWidth, m_strokeStyle, &geometryRealization);

			p_geometry->setStrokedRealization(geometryRealization);
		}
	}
	void realizeFilledGeometry(Direct2DGeometry* p_geometry)
	{
		if (!p_geometry->getFilledRealization())
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);

			float dpiX = USER_DEFAULT_SCREEN_DPI;
			float dpiY = USER_DEFAULT_SCREEN_DPI;
			m_context->GetDpi(&dpiX, &dpiY);

			ID2D1GeometryRealization* geometryRealization = 0;
			m_context->CreateFilledGeometryRealization(p_geometry->getGeometry(), D2D1::ComputeFlatteningTolerance(transform, dpiX, dpiY), &geometryRealization);

			p_geometry->setFilledRealization(geometryRealization);
		}
	}

public:
	Direct2DDrawingContext(AvoGUI::Window* p_window) :
		m_window(p_window)
	{
		// Create temporary Direct3D device

		ID3D11Device* d3dDevice;
		ID3D11DeviceContext* d3dDeviceContext;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1,
		};
		D3D_FEATURE_LEVEL featureLevel;
		D3D11CreateDevice(
			0,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT
#ifdef _DEBUG 
			| D3D11_CREATE_DEVICE_DEBUG
#endif
			,
			featureLevels,
			sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,
			&d3dDevice,
			&featureLevel,
			&d3dDeviceContext
		);

		//------------------------------
		// Get dxgi factory from the Direct3D device

		IDXGIDevice1* dxgiDevice = 0;
		d3dDevice->QueryInterface(&dxgiDevice);
		dxgiDevice->SetMaximumFrameLatency(1U);

		IDXGIAdapter* dxgiAdapter;
		dxgiDevice->GetAdapter(&dxgiAdapter);

		IDXGIFactory2* dxgiFactory;
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		//------------------------------
		// Create Direct2D device and device context.

		ID2D1Device1* direct2DDevice = 0;
		s_direct2DFactory->CreateDevice(dxgiDevice, &direct2DDevice);
		direct2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_context);

		float dpi = GetDpiForSystem();

		//------------------------------
		// Create swap chain, which holds the back buffer and is connected to the window.

		DXGI_SWAP_CHAIN_DESC1 swapChainDescription = { };
		swapChainDescription.Width = m_window->getWidth()*dpi/USER_DEFAULT_SCREEN_DPI;
		swapChainDescription.Height = m_window->getHeight()*dpi/USER_DEFAULT_SCREEN_DPI;
		swapChainDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDescription.Stereo = false;
		swapChainDescription.SampleDesc.Count = 1;
		swapChainDescription.SampleDesc.Quality = 0;
		swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescription.BufferCount = 2;
		swapChainDescription.Scaling = DXGI_SCALING::DXGI_SCALING_NONE;
		swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDescription.Flags = 0;

		dxgiFactory->CreateSwapChainForHwnd(
			d3dDevice, (HWND)p_window->getNativeHandle(),
			&swapChainDescription, 0,
			0, &m_swapChain
		);

		dxgiFactory->MakeWindowAssociation((HWND)p_window->getNativeHandle(), DXGI_MWA_NO_WINDOW_CHANGES);

		//------------------------------
		// Create a target bitmap which is connected to the back buffer of the window.

		IDXGISurface* dxgiBackBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		m_context->CreateBitmapFromDxgiSurface(
			dxgiBackBuffer,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_IGNORE),
				dpi, dpi
			),
			&m_targetWindowBitmap
		);

		m_context->SetTarget(m_targetWindowBitmap);

		m_context->SetDpi(dpi, dpi);

		DXGI_RGBA color;
		color.r = 0.5f;
		color.g = 0.5f;
		color.b = 0.5f;
		color.a = 1.f;
		m_swapChain->SetBackgroundColor(&color);

		//------------------------------

		dxgiBackBuffer->Release();
		direct2DDevice->Release();
		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();
		d3dDeviceContext->Release();
		d3dDevice->Release();

		//------------------------------

		m_context->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &m_solidColorBrush);
		m_currentBrush = m_solidColorBrush;

		m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
		m_strokeStyleProperties.dashOffset = 1.f;
		m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_SOLID;
		m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
		m_strokeStyleProperties.miterLimit = 0.f;
		m_strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
		m_strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
		m_strokeStyleProperties.transformType = D2D1_STROKE_TRANSFORM_TYPE::D2D1_STROKE_TRANSFORM_TYPE_NORMAL;

		updateStrokeStyle();

		//------------------------------
		// Create text stuff

		m_fontData.reserve(8);
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(FONT_DATA_ROBOTO_LIGHT, FONT_DATA_ROBOTO_LIGHT + FONT_DATA_SIZE_ROBOTO_LIGHT));
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(FONT_DATA_ROBOTO_REGULAR, FONT_DATA_ROBOTO_REGULAR + FONT_DATA_SIZE_ROBOTO_REGULAR));
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(FONT_DATA_ROBOTO_MEDIUM, FONT_DATA_ROBOTO_MEDIUM + FONT_DATA_SIZE_ROBOTO_MEDIUM));
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(FONT_DATA_ROBOTO_BOLD, FONT_DATA_ROBOTO_BOLD + FONT_DATA_SIZE_ROBOTO_BOLD));
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(FONT_DATA_MATERIAL_ICONS, FONT_DATA_MATERIAL_ICONS + FONT_DATA_SIZE_MATERIAL_ICONS));
		updateFontCollection();

		// Just for debugging...
		//std::vector<wchar_t*> fontFamilyNames;
		//for (uint32 a = 0; a < m_fontCollection->GetFontFamilyCount(); a++)
		//{
		//	IDWriteFontFamily* fontFamily;
		//	m_fontCollection->GetFontFamily(a, &fontFamily);
		//	IDWriteLocalizedStrings* names;
		//	fontFamily->GetFamilyNames(&names);
		//	wchar_t* buffer = new wchar_t[30];
		//	names->GetString(0, buffer, 30);
		//	fontFamilyNames.push_back(buffer);
		//}

		m_textProperties.fontFamilyName = "Roboto";
		setDefaultTextProperties(m_textProperties);

		m_context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	}

	~Direct2DDrawingContext()
	{
		if (m_solidColorBrush)
		{
			m_solidColorBrush->Release();
		}
		if (m_strokeStyle)
		{
			m_strokeStyle->Release();
		}
		if (m_targetWindowBitmap)
		{
			m_targetWindowBitmap->Release();
		}
		if (m_swapChain)
		{
			m_swapChain->Release();
		}
		if (m_context)
		{
			m_context->Release();
		}

		destroyStaticResources();
	}

	//------------------------------

	void beginDrawing() override
	{
		m_targetMutex.lock();
		m_context->BeginDraw();
	}
	void finishDrawing(std::vector<AvoGUI::Rectangle<float>> const& p_updatedRectangles) override
	{
		if (p_updatedRectangles.size())
		{
			m_context->EndDraw();

			DXGI_PRESENT_PARAMETERS presentParameters;
			presentParameters.DirtyRectsCount = p_updatedRectangles.size();

			//RECT* updatedRects = new RECT[p_updatedRectangles.size()];
			RECT updatedRects[500]; // This is more efficient than dynamic allocation... But it does feel dangerous to have an upper limit like this.

			float dpiFactor = getDpi() / USER_DEFAULT_SCREEN_DPI;

			D2D1_SIZE_U size = m_context->GetPixelSize();
			m_targetMutex.unlock();

			// If you're getting an exception below, you have three options; 
			// 1. don't invalidate so damn many rectangles
			// 2. increase the size of the static array above
			// 3. make the array above dynamic (see the commented line above there), also don't forget to free it.
			for (uint32 a = 0; a < p_updatedRectangles.size(); a++)
			{
				updatedRects[a].left = AvoGUI::max(0.f, floor(p_updatedRectangles[a].left*dpiFactor));
				updatedRects[a].top = AvoGUI::max(0.f, floor(p_updatedRectangles[a].top*dpiFactor));
				updatedRects[a].right = AvoGUI::min(size.width, (uint32)ceil(p_updatedRectangles[a].right*dpiFactor));
				updatedRects[a].bottom = AvoGUI::min(size.height, (uint32)ceil(p_updatedRectangles[a].bottom*dpiFactor));
			}

			presentParameters.pDirtyRects = updatedRects;
			presentParameters.pScrollOffset = 0;
			presentParameters.pScrollRect = 0;

			ID2D1Multithread* multithreader;
			s_direct2DFactory->QueryInterface(IID_PPV_ARGS(&multithreader));

			multithreader->Enter();
			m_swapChain->Present1(1, m_isVsyncEnabled ? 0 : (DXGI_PRESENT_DO_NOT_WAIT | DXGI_PRESENT_RESTART), &presentParameters);
			multithreader->Leave();
			multithreader->Release();

			//delete[] updatedRects;
		}
		//else
		//{
		//	// Just triggers a buffer swap.

		//	RECT rect = { 0, 0, 1, 1 };
		//	DXGI_PRESENT_PARAMETERS presentParameters;
		//	presentParameters.DirtyRectsCount = 1;
		//	presentParameters.pDirtyRects = &rect;
		//	presentParameters.pScrollOffset = 0;
		//	presentParameters.pScrollRect = 0;

		//	m_swapChain->Present1(1, m_isVsyncEnabled ? 0 : (DXGI_PRESENT_DO_NOT_WAIT | DXGI_PRESENT_RESTART), &presentParameters);
		//}

	}

	AvoGUI::DrawingState* createDrawingState() override
	{
		ID2D1DrawingStateBlock1* drawingState = 0;
		s_direct2DFactory->CreateDrawingStateBlock(&drawingState);
		return (AvoGUI::DrawingState*)(new Direct2DDrawingState(drawingState));
	}
	void saveDrawingState(AvoGUI::DrawingState* p_drawingState) override
	{
		m_context->SaveDrawingState(((Direct2DDrawingState*)p_drawingState)->getHandle());
	}
	void restoreDrawingState(AvoGUI::DrawingState* p_drawingState) override
	{
		m_context->RestoreDrawingState(((Direct2DDrawingState*)p_drawingState)->getHandle());
	}

	//------------------------------

	void setIsFullscreen(bool p_isFullscreen) override
	{
		m_window->setIsFullscreen(p_isFullscreen);
	}
	void switchFullscreen() override
	{
		m_window->switchFullscreen();
	}
	bool getIsFullscreen() override
	{
		return m_window->getIsFullscreen();
	}

	//------------------------------

	void enableVsync()
	{
		m_isVsyncEnabled = true;
	}
	void disableVsync()
	{
		m_isVsyncEnabled = false;
	}
	bool getIsVsyncEnabled()
	{
		return m_isVsyncEnabled;
	}

	//------------------------------

	void setBackgroundColor(AvoGUI::Color const& p_color) override
	{
		if (m_swapChain)
		{
			DXGI_RGBA dxgiColor;
			dxgiColor.r = p_color.red;
			dxgiColor.g = p_color.green;
			dxgiColor.b = p_color.blue;
			dxgiColor.a = p_color.alpha;

			ID2D1Multithread* multithreader;
			s_direct2DFactory->QueryInterface(IID_PPV_ARGS(&multithreader));

			multithreader->Enter();
			m_swapChain->SetBackgroundColor(&dxgiColor);
			multithreader->Leave();
			multithreader->Release();
		}
	}
	AvoGUI::Color getBackgroundColor()
	{
		if (m_swapChain)
		{
			ID2D1Multithread* multithreader;
			s_direct2DFactory->QueryInterface(IID_PPV_ARGS(&multithreader));

			multithreader->Enter();
			DXGI_RGBA dxgiColor;
			m_swapChain->GetBackgroundColor(&dxgiColor);
			multithreader->Leave();
			multithreader->Release();

			return AvoGUI::Color(dxgiColor.r, dxgiColor.g, dxgiColor.b, dxgiColor.a);
		}
		return AvoGUI::Color(0.5f);
	}

	//------------------------------

	float getDpi() override
	{
		float DPIX = 0.f;
		float DPIY = 0.f;
		m_context->GetDpi(&DPIX, &DPIY);
		return DPIX;
	}
	void setDpi(float p_dpi)
	{
		m_context->SetDpi(p_dpi, p_dpi);
	}

	//------------------------------

	void moveOrigin(AvoGUI::Point<float> const& p_offset) override
	{
		D2D1_MATRIX_3X2_F transform;
		m_context->GetTransform(&transform);
		transform.dx += p_offset.x;
		transform.dy += p_offset.y;
		m_context->SetTransform(transform);
	}
	void moveOrigin(float p_offsetX, float p_offsetY) override
	{
		D2D1_MATRIX_3X2_F transform;
		m_context->GetTransform(&transform);
		transform.dx += p_offsetX;
		transform.dy += p_offsetY;
		m_context->SetTransform(transform);
	}
	void setOrigin(AvoGUI::Point<float> const& p_origin) override
	{
		D2D1_MATRIX_3X2_F transform;
		m_context->GetTransform(&transform);
		transform.dx = p_origin.x;
		transform.dy = p_origin.y;
		m_context->SetTransform(transform);
	}
	void setOrigin(float p_x, float p_y) override
	{
		D2D1_MATRIX_3X2_F transform;
		m_context->GetTransform(&transform);
		transform.dx = p_x;
		transform.dy = p_y;
		m_context->SetTransform(transform);
	}
	AvoGUI::Point<float> getOrigin() override
	{
		D2D1_MATRIX_3X2_F transform;
		m_context->GetTransform(&transform);
		return AvoGUI::Point<float>(transform.dx, transform.dy);
	}

	//------------------------------

	void scale(float p_scale) override
	{
		scale(p_scale, p_scale);
	}
	void scale(float p_scaleX, float p_scaleY) override
	{
		D2D1::Matrix3x2F transform;
		m_context->GetTransform(&transform);
		transform.m11 *= p_scaleX;
		transform.m22 *= p_scaleY;
		transform.m21 *= p_scaleX;
		transform.m12 *= p_scaleY;
		m_scale.x *= p_scaleX;
		m_scale.y *= p_scaleY;
		m_context->SetTransform(transform);
	}
	void scale(float p_scale, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scale, p_scale, p_origin.x, p_origin.y);
	}
	void scale(float p_scaleX, float p_scaleY, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scaleX, p_scaleY, p_origin.x, p_origin.y);
	}
	void scale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale, p_scale, p_originX, p_originY);
	}
	void scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		D2D1::Matrix3x2F transform;
		m_context->GetTransform(&transform);
		transform.m11 *= p_scaleX;
		transform.m22 *= p_scaleY;
		transform.m21 *= p_scaleX;
		transform.m12 *= p_scaleY;
		m_scale.x *= p_scaleX;
		m_scale.y *= p_scaleY;
		transform.dx += (p_originX - transform.dx) * (1.f - p_scaleX);
		transform.dy += (p_originY - transform.dy) * (1.f - p_scaleY);
		m_context->SetTransform(transform);
	}
	void setScale(float p_scale) override
	{
		scale(p_scale / m_scale.x, p_scale / m_scale.x);
	}
	void setScale(float p_scaleX, float p_scaleY) override
	{
		scale(p_scaleX / m_scale.x, p_scaleY / m_scale.x);
	}
	void setScale(float p_scale, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scale / m_scale.x, p_scale / m_scale.y, p_origin.x, p_origin.y);
	}
	void setScale(float p_scaleX, float p_scaleY, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scaleX / m_scale.x, p_scaleY / m_scale.y, p_origin.x, p_origin.y);
	}
	void setScale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale / m_scale.x, p_scale / m_scale.y, p_originX, p_originY);
	}
	void setScale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		scale(p_scaleX / m_scale.x, p_scaleY / m_scale.y, p_originX, p_originY);
	}
	AvoGUI::Point<float> const& getScale() override
	{
		return m_scale;
	}
	float getScaleX() override
	{
		return m_scale.x;
	}
	float getScaleY() override
	{
		return m_scale.y;
	}

	//------------------------------

	void rotate(float p_radians) override
	{
		D2D1::Matrix3x2F transform;
		m_context->GetTransform(&transform);
		m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / AvoGUI::PI));
	}
	void rotate(float p_radians, AvoGUI::Point<float> const& p_origin) override
	{
		D2D1::Matrix3x2F transform;
		m_context->GetTransform(&transform);
		m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / AvoGUI::PI, D2D1::Point2F(p_origin.x, p_origin.y)));
	}
	void rotate(float p_radians, float p_originX, float p_originY) override
	{
		D2D1::Matrix3x2F transform;
		m_context->GetTransform(&transform);
		m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / AvoGUI::PI, D2D1::Point2F(p_originX, p_originY)));
	}

	//------------------------------

	void resetTransformations() override
	{
		m_context->SetTransform(D2D1::Matrix3x2F::Identity());
		m_scale.x = 1.f;
		m_scale.y = 1.f;
	}

	//------------------------------

	void setSize(AvoGUI::Point<float> const& p_size) override
	{
		setSize(p_size.x, p_size.y);
	}
	void setSize(float p_width, float p_height) override
	{
		D2D1_SIZE_F oldSize = m_context->GetSize();
		//if (oldSize.width >= p_width && oldSize.width < p_width + 200 &&
			//oldSize.height >= p_height && oldSize.height < p_height + 200)
		if (oldSize.width == p_width && oldSize.height == p_height)
		{
			return;
		}

		m_targetMutex.lock();
		ID2D1Image* oldTarget = 0;
		m_context->GetTarget(&oldTarget);

		bool wasOldTargetWindow = oldTarget == m_targetWindowBitmap;
		if (wasOldTargetWindow)
		{
			m_context->SetTarget(0);
		}
		oldTarget->Release();

		// Release the old window bitmap
		m_targetWindowBitmap->Release();

		float dpi = getDpi();

		AvoGUI::Point<float> newSize(p_width * dpi / USER_DEFAULT_SCREEN_DPI, p_height* dpi / USER_DEFAULT_SCREEN_DPI);

		ID2D1Multithread* multithreader;
		s_direct2DFactory->QueryInterface(IID_PPV_ARGS(&multithreader));

		multithreader->Enter();

		// Resize buffers, creating new ones
		m_swapChain->ResizeBuffers(0, p_width*dpi / USER_DEFAULT_SCREEN_DPI, p_height*dpi / USER_DEFAULT_SCREEN_DPI, DXGI_FORMAT_UNKNOWN, 0);

		// Get the new back buffer and create new bitmap connected to it
		IDXGISurface* dxgiBackBuffer = 0;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		multithreader->Leave();
		multithreader->Release();

		m_context->CreateBitmapFromDxgiSurface(
			dxgiBackBuffer,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_IGNORE),
				dpi, dpi
			),
			&m_targetWindowBitmap
		);

		dxgiBackBuffer->Release();

		if (wasOldTargetWindow)
		{
			m_context->SetTarget(m_targetWindowBitmap);
		}
		m_targetMutex.unlock();
	}
	AvoGUI::Point<float> getSize() override
	{
		return AvoGUI::Point<float>(m_context->GetSize().width, m_context->GetSize().height);
	}

	//------------------------------

	void clear(AvoGUI::Color const& p_color) override
	{
		m_context->Clear(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
	}
	void clear() override
	{
		m_context->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
	}

	//------------------------------

	void fillRectangle(AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size) override
	{
		fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
	}
	void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillRectangle(
			D2D1::RectF(
				p_left, p_top,
				p_right, p_bottom
			), m_currentBrush
		);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_size) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillRectangle(D2D1::RectF(0, 0, p_size.x, p_size.y), m_currentBrush);
	}
	void fillRectangle(float p_width, float p_height) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillRectangle(D2D1::RectF(0, 0, p_width, p_height), m_currentBrush);
	}

	void fillRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners) override
	{
		fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners) override
	{
		fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners);
	}
	void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners) override
	{
		ID2D1PathGeometry1* pathGeometry;
		s_direct2DFactory->CreatePathGeometry(&pathGeometry);
		createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, true);

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillGeometry(pathGeometry, m_currentBrush);

		pathGeometry->Release();
	}

	void fillRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners) override
	{
		fillRectangle(0.f, 0.f, p_size.x, p_size.y, p_corners);
	}
	void fillRectangle(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners) override
	{
		fillRectangle(0.f, 0.f, p_width, p_height, p_corners);
	}

	void fillRoundedRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius) override
	{
		fillRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius);
	}
	void fillRoundedRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius) override
	{
		fillRoundedRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius);
	}
	void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillRoundedRectangle(
			D2D1::RoundedRect(
				D2D1::RectF(
					p_left, p_top,
					p_right, p_bottom
				), p_radius, p_radius
			), m_currentBrush
		);
	}
	void fillRoundedRectangle(AvoGUI::Point<float> const& p_size, float p_radius) override
	{
		fillRoundedRectangle(p_size.x, p_size.y, p_radius);
	}
	void fillRoundedRectangle(float p_width, float p_height, float p_radius) override
	{
		fillRoundedRectangle(0.f, 0.f, p_width, p_height, p_radius);
	}

	//------------------------------

	void strokeRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_strokeWidth);
	}
	void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawRectangle(
			D2D1::RectF(
				p_left, p_top, p_right, p_bottom
			), m_currentBrush,
			p_strokeWidth, m_strokeStyle
		);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_size, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(p_size.x, p_size.y, p_strokeWidth);
	}
	void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(0.f, 0.f, p_width, p_height, p_strokeWidth);
	}

	void strokeRoundedRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth = 1.f) override
	{
		strokeRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_strokeWidth);
	}
	void strokeRoundedRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) override
	{
		strokeRoundedRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius, p_strokeWidth);
	}
	void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawRoundedRectangle(
			D2D1::RoundedRect(
				D2D1::RectF(
					p_left, p_top,
					p_right, p_bottom
				), p_radius, p_radius
			), m_currentBrush,
			p_strokeWidth, m_strokeStyle
		);
	}
	void strokeRoundedRectangle(AvoGUI::Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) override
	{
		strokeRoundedRectangle(p_size.x, p_size.y, p_radius, p_strokeWidth);
	}
	void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) override
	{
		strokeRoundedRectangle(0.f, 0.f, p_width, p_height, p_radius, p_strokeWidth);
	}

	void strokeRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_strokeWidth);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners, p_strokeWidth);
	}
	void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
	{
		ID2D1PathGeometry1* pathGeometry;
		s_direct2DFactory->CreatePathGeometry(&pathGeometry);
		createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, false);

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawGeometry(pathGeometry, m_currentBrush, p_strokeWidth, m_strokeStyle);

		pathGeometry->Release();
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(0.f, 0.f, p_size.x, p_size.y, p_corners, p_strokeWidth);
	}
	void strokeRectangle(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
	{
		strokeRectangle(0.f, 0.f, p_width, p_height, p_corners, p_strokeWidth);
	}

	//------------------------------

	void fillCircle(AvoGUI::Point<float> const& p_position, float p_radius) override
	{
		fillCircle(p_position.x, p_position.y, p_radius);
	}
	void fillCircle(float p_x, float p_y, float p_radius) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillEllipse(
			D2D1::Ellipse(
				D2D1::Point2F(p_x, p_y),
				p_radius, p_radius
			), m_currentBrush
		);
	}

	void strokeCircle(AvoGUI::Point<float> const& p_position, float p_radius, float p_strokeWidth = 1.f)
	{
		strokeCircle(p_position.x, p_position.y, p_radius, p_strokeWidth);
	};
	void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f)
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawEllipse(
			D2D1::Ellipse(
				D2D1::Point2F(p_x, p_y),
				p_radius, p_radius
			), m_currentBrush,
			p_strokeWidth, m_strokeStyle
		);
	}

	//------------------------------

	void drawLine(AvoGUI::Point<float> const& p_point_0, AvoGUI::Point<float> const& p_point_1, float p_thickness = 1.f) override
	{
		drawLine(p_point_0.x, p_point_0.y, p_point_1.x, p_point_1.y, p_thickness);
	}
	void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) override
	{
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawLine(
			D2D1::Point2F(p_x0, p_y0),
			D2D1::Point2F(p_x1, p_y1),
			m_currentBrush,
			p_thickness, m_strokeStyle
		);
	}

	//------------------------------

	void strokeShape(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed) override
	{
		if (!p_numberOfVertices)
		{
			return;
		}

		ID2D1PathGeometry1* path;
		s_direct2DFactory->CreatePathGeometry(&path);
		ID2D1GeometrySink* sink;
		path->Open(&sink);

		sink->BeginFigure(D2D1::Point2F(p_vertices[0].x, p_vertices[0].y), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);
		for (uint32 a = 1; a < p_numberOfVertices; a++)
		{
			sink->AddLine(D2D1::Point2F(p_vertices[a].x, p_vertices[a].y));
		}
		sink->EndFigure(p_isClosed ? D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);

		sink->Close();

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawGeometry(path, m_currentBrush, p_lineThickness, m_strokeStyle);

		sink->Release();
		path->Release();
	}
	void strokeShape(std::vector<AvoGUI::Point<float>> const& p_vertices, float p_lineThickness, bool p_isClosed) override
	{
		strokeShape(p_vertices.data(), p_vertices.size(), p_lineThickness, p_isClosed);
	}

	void fillShape(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices) override
	{
		if (!p_numberOfVertices)
		{
			return;
		}

		ID2D1PathGeometry1* path;
		s_direct2DFactory->CreatePathGeometry(&path);
		ID2D1GeometrySink* sink;
		path->Open(&sink);

		sink->BeginFigure(D2D1::Point2F(p_vertices[0].x, p_vertices[0].y), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
		for (uint32 a = 1; a < p_numberOfVertices; a++)
		{
			sink->AddLine(D2D1::Point2F(p_vertices[a].x, p_vertices[a].y));
		}
		sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);

		sink->Close();

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->FillGeometry(path, m_currentBrush);

		sink->Release();
		path->Release();
	}
	void fillShape(std::vector<AvoGUI::Point<float>> const& p_vertices) override
	{
		fillShape(p_vertices.data(), p_vertices.size());
	}

	//------------------------------

	void strokeGeometry(AvoGUI::Geometry* p_geometry, float p_strokeWidth) override
	{
		if (!((Direct2DGeometry*)p_geometry)->getStrokedRealization())
		{
			realizeStrokedGeometry((Direct2DGeometry*)p_geometry, p_strokeWidth);
		}
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawGeometryRealization(((Direct2DGeometry*)p_geometry)->getStrokedRealization(), m_currentBrush);
	}
	void fillGeometry(AvoGUI::Geometry* p_geometry) override
	{
		if (!((Direct2DGeometry*)p_geometry)->getFilledRealization())
		{
			realizeFilledGeometry((Direct2DGeometry*)p_geometry);
		}
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawGeometryRealization(((Direct2DGeometry*)p_geometry)->getFilledRealization(), m_currentBrush);
	}

	//------------------------------

	AvoGUI::Geometry* createRoundedRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius, bool p_isStroked) override
	{
		ID2D1RoundedRectangleGeometry* geometry;
		s_direct2DFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect(
				D2D1::RectF(p_left, p_top, p_right, p_bottom),
				p_radius, p_radius
			), &geometry
		);

		return new Direct2DGeometry((ID2D1Geometry*)geometry);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius, bool p_isStroked) override
	{
		return createRoundedRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius, p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, bool p_isStroked) override
	{
		return createRoundedRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(float p_width, float p_height, float p_radius, bool p_isStroked) override
	{
		return createRoundedRectangleGeometry(0.f, 0.f, p_width, p_height, p_radius, p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Point<float> const& p_size, float p_radius, bool p_isStroked) override
	{
		return createRoundedRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, p_radius, p_isStroked);
	}

	AvoGUI::Geometry* createCornerRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		ID2D1PathGeometry1* pathGeometry = 0;
		s_direct2DFactory->CreatePathGeometry(&pathGeometry);
		createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, true);

		return new Direct2DGeometry((ID2D1Geometry*)pathGeometry);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(0.f, 0.f, p_width, p_height, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, p_corners, p_isStroked);
	}

	//------------------------------

	AvoGUI::Geometry* createPolygonGeometry(std::vector<AvoGUI::Point<float>> const& p_vertices, bool p_isStroked) override
	{
		return createPolygonGeometry(p_vertices.data(), p_vertices.size(), p_isStroked);
	}
	AvoGUI::Geometry* createPolygonGeometry(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices, bool p_isStroked) override
	{
		if (!p_numberOfVertices)
		{
			return 0;
		}

		ID2D1PathGeometry1* path;
		s_direct2DFactory->CreatePathGeometry(&path);
		
		ID2D1GeometrySink* sink;
		path->Open(&sink);

		sink->BeginFigure(D2D1::Point2F(p_vertices[0].x, p_vertices[0].y), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
		for (uint32 a = 1; a < p_numberOfVertices; a++)
		{
			sink->AddLine(D2D1::Point2F(p_vertices[a].x, p_vertices[a].y));
		}
		sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);

		sink->Close();
		sink->Release();

		return new Direct2DGeometry(path);
	}

	//------------------------------

	void setLineCap(AvoGUI::LineCap p_lineCap) override
	{
		switch (p_lineCap)
		{
			case AvoGUI::LineCap::Flat:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case AvoGUI::LineCap::Round:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case AvoGUI::LineCap::Square:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case AvoGUI::LineCap::Triangle:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	void setStartLineCap(AvoGUI::LineCap p_lineCap) override
	{
		switch (p_lineCap)
		{
			case AvoGUI::LineCap::Flat:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case AvoGUI::LineCap::Round:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case AvoGUI::LineCap::Square:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case AvoGUI::LineCap::Triangle:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	void setEndLineCap(AvoGUI::LineCap p_lineCap) override
	{
		switch (p_lineCap)
		{
			case AvoGUI::LineCap::Flat:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case AvoGUI::LineCap::Round:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case AvoGUI::LineCap::Square:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case AvoGUI::LineCap::Triangle:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	AvoGUI::LineCap getStartLineCap() override
	{
		switch (m_strokeStyleProperties.startCap)
		{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return AvoGUI::LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return AvoGUI::LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return AvoGUI::LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return AvoGUI::LineCap::Triangle;
		}
		return AvoGUI::LineCap::Triangle;
	}
	AvoGUI::LineCap getEndLineCap() override
	{
		switch (m_strokeStyleProperties.endCap)
		{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return AvoGUI::LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return AvoGUI::LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return AvoGUI::LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return AvoGUI::LineCap::Triangle;
		}
		return AvoGUI::LineCap::Triangle;
	}

	//------------------------------

	void setLineJoin(AvoGUI::LineJoin p_lineJoin) override
	{
		switch (p_lineJoin)
		{
			case AvoGUI::LineJoin::Bevel:
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_BEVEL;
				break;
			case AvoGUI::LineJoin::Miter:
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER;
				break;
			case AvoGUI::LineJoin::Round:
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND;
				break;
		}
		updateStrokeStyle();
	}
	AvoGUI::LineJoin getLineJoin() override
	{
		switch (m_strokeStyleProperties.lineJoin)
		{
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_BEVEL:
				return AvoGUI::LineJoin::Bevel;
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER:
				return AvoGUI::LineJoin::Miter;
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND:
				return AvoGUI::LineJoin::Round;
		}
		return AvoGUI::LineJoin::Miter;
	}

	void setLineJoinMiterLimit(float p_miterLimit) override
	{
		m_strokeStyleProperties.miterLimit = p_miterLimit;
		updateStrokeStyle();
	}
	float getLineJoinMiterLimit() override
	{
		return m_strokeStyleProperties.miterLimit;
	}

	//------------------------------

	void setLineDashStyle(AvoGUI::LineDashStyle p_dashStyle) override
	{
		switch (p_dashStyle)
		{
			case AvoGUI::LineDashStyle::Solid:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID;
				break;
			case AvoGUI::LineDashStyle::Dash:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH;
				break;
			case AvoGUI::LineDashStyle::Dot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT;
				break;
			case AvoGUI::LineDashStyle::DashDot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT;
				break;
			case AvoGUI::LineDashStyle::DashDotDot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT_DOT;
				break;
			case AvoGUI::LineDashStyle::Custom:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_CUSTOM;
		}
		updateStrokeStyle();
	}
	AvoGUI::LineDashStyle getLineDashStyle() override
	{
		switch (m_strokeStyleProperties.dashStyle)
		{
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID:
				return AvoGUI::LineDashStyle::Solid;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH:
				return AvoGUI::LineDashStyle::Dash;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT:
				return AvoGUI::LineDashStyle::Dot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT:
				return AvoGUI::LineDashStyle::DashDot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT_DOT:
				return AvoGUI::LineDashStyle::DashDotDot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_CUSTOM:
				return AvoGUI::LineDashStyle::Custom;
		}
	}

	void setLineDashOffset(float p_dashOffset) override
	{
		m_strokeStyleProperties.dashOffset = p_dashOffset;
		updateStrokeStyle();
	}
	float getLineDashOffset() override
	{
		return m_strokeStyleProperties.dashOffset;
	}

	void setLineDashCap(AvoGUI::LineCap p_dashCap) override
	{
		switch (p_dashCap)
		{
			case AvoGUI::LineCap::Flat:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case AvoGUI::LineCap::Round:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case AvoGUI::LineCap::Square:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case AvoGUI::LineCap::Triangle:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
		}
		updateStrokeStyle();
	}
	AvoGUI::LineCap getLineDashCap() override
	{
		switch (m_strokeStyleProperties.dashCap)
		{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return AvoGUI::LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return AvoGUI::LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return AvoGUI::LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return AvoGUI::LineCap::Triangle;
		}
	}

	//------------------------------

	void pushClipGeometry(AvoGUI::Geometry* p_geometry, float p_opacity) override
	{
		m_context->PushLayer(
			D2D1::LayerParameters1(
				D2D1::InfiniteRect(), ((Direct2DGeometry*)p_geometry)->getGeometry(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, 
				D2D1::IdentityMatrix(), p_opacity, 0, D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
			), 0
		);
		m_clipTypeStack.push(true);
	}

	//------------------------------

	void pushClipShape(std::vector<AvoGUI::Point<float>> const& p_points, float p_opacity) override
	{
		pushClipShape(p_points.data(), p_points.size(), p_opacity);
	}
	void pushClipShape(AvoGUI::Point<float> const* p_points, uint32 p_numberOfPoints, float p_opacity) override
	{
		if (!p_numberOfPoints)
		{
			return;
		}

		ID2D1PathGeometry1* geometry;
		s_direct2DFactory->CreatePathGeometry(&geometry);

		ID2D1GeometrySink* sink;
		geometry->Open(&sink);

		sink->BeginFigure(D2D1::Point2F(p_points[0].x, p_points[0].y), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
		for (uint32 a = 1; a < p_numberOfPoints; a++)
		{
			sink->AddLine(D2D1::Point2F(p_points[a].x, p_points[a].y));
		}
		sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);

		sink->Close();
		sink->Release();

		m_context->PushLayer(
			D2D1::LayerParameters1(
				D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(), p_opacity, 0, D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
			), 0
		);
		m_clipTypeStack.push(true);

		geometry->Release();
	}
	void popClipShape() override
	{
		if (m_clipTypeStack.size())
		{
			if (m_clipTypeStack.top())
			{
				m_context->PopLayer();
			}
			else
			{
				m_context->PopAxisAlignedClip();
			}
			m_clipTypeStack.pop();
		}
	}

	//------------------------------

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_opacity) override
	{
		if (p_opacity > 0.99607843137f) // 254/255
		{
			m_context->PushAxisAlignedClip(
				D2D1::RectF(p_left, p_top, p_right, p_bottom), 
				D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
			);
			m_clipTypeStack.push(false);
		}
		else
		{
			ID2D1RectangleGeometry* geometry = 0;
			s_direct2DFactory->CreateRectangleGeometry(D2D1::RectF(p_left, p_top, p_right, p_bottom), &geometry);
			m_context->PushLayer(
				D2D1::LayerParameters1(
					D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
					D2D1::IdentityMatrix(), p_opacity, 0, D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
				), 0
			);
			geometry->Release();
			m_clipTypeStack.push(true);
		}
	}
	void pushClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_opacity) override
	{
		pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_opacity);
	}
	void pushClipRectangle(AvoGUI::Point<float> const& p_size, float p_opacity) override
	{
		pushClipRectangle(0.f, 0.f, p_size.x, p_size.y, p_opacity);
	}

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, float p_opacity)
	{
		ID2D1PathGeometry1* geometry;
		s_direct2DFactory->CreatePathGeometry(&geometry);

		createCornerRectangleGeometry(geometry, p_left, p_top, p_right, p_bottom, p_corners, true);

		m_context->PushLayer(
			D2D1::LayerParameters1(
				D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(), p_opacity, 0, D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
			), 0
		);
		geometry->Release();
		m_clipTypeStack.push(true);
	}
	void pushClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners, float p_opacity)
	{
		pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_opacity);
	}
	void pushClipRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_opacity)
	{
		pushClipRectangle(0.f, 0.f, p_size.x, p_size.y, p_corners, p_opacity);
	}

	//------------------------------

	void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_opacity) override
	{
		ID2D1RoundedRectangleGeometry* geometry;
		s_direct2DFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect(
				D2D1::RectF(p_left, p_top, p_right, p_bottom),
				p_radius, p_radius
			), &geometry
		);

		ID2D1Layer* layer;
		m_context->CreateLayer(&layer);
		m_context->PushLayer(
			D2D1::LayerParameters1(
				D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(), p_opacity, 0, D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND | D2D1_LAYER_OPTIONS1_IGNORE_ALPHA // Improves performance :^)
			), 0
		);
		layer->Release();
		geometry->Release();
		m_clipTypeStack.push(true);
	}
	void pushRoundedClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, float p_opacity) override
	{
		pushRoundedClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_opacity);
	}
	void pushRoundedClipRectangle(AvoGUI::Point<float> const& p_size, float p_radius, float p_opacity) override
	{
		pushRoundedClipRectangle(0.f, 0.f, p_size.x, p_size.y, p_radius, p_opacity);
	}

	//------------------------------

	AvoGUI::Image* createRectangleShadowImage(AvoGUI::Point<float> const& p_size, float p_blur, AvoGUI::Color const& p_color) override
	{
		return createRectangleShadowImage(p_size.x, p_size.y, p_blur, p_color);
	}
	AvoGUI::Image* createRectangleShadowImage(float p_width, float p_height, float p_blur, AvoGUI::Color const& p_color) override
	{
		// I am proud of this algorithm I came up with when I was
		// trying to make a GUI library using SDL, so why not use it here

		// Me in the future: because it doesn't look similar enough to shadows made with the shadow effect in direct2d,
		// which is used in the createRoundedRectangleShadowImage() method. I also don't know which method is faster (mine probably hehe)

		// Here is the code anyways, it is pretty fun:

		/*
		if (p_width*p_height == 0) return 0;

		float blurCeiled = std::ceil(p_blur);
		int32 edgeWidth = (int32)blurCeiled << 1;

		p_width += edgeWidth;
		p_height += edgeWidth;

		//------------------------------

		if (p_color & 0xff000000)
		{
			premultiplyColor(p_color);
		}
		else
		{
			p_color |= 0xff000000;
		}

		uint32* pixels = new uint32[p_width*p_height];
		for (uint32 a = 0; a < p_width*p_height; a++)
		{
			pixels[a] = p_color;
		}

		int32 x = 0, y = 0;
		float distance = (p_blur - (int32)p_blur) / (p_blur*2.f + 1.f);
		float alphaFactor = 2.f*distance*distance;

		if (p_width > p_height)
		{
			while (y < edgeWidth)
			{
				pixels[x + y * p_width] = getMultipliedColor(pixels[x + y * p_width], alphaFactor);
				pixels[x + (p_height - y - 1)*p_width] = getMultipliedColor(pixels[x + (p_height - y - 1)*p_width], alphaFactor);
				if (x < p_height)
				{
					pixels[y + x * p_width] = getMultipliedColor(pixels[y + x * p_width], alphaFactor);
					pixels[p_width - 1 - y + x * p_width] = getMultipliedColor(pixels[p_width - 1 - y + x * p_width], alphaFactor);
				}

				//------------------------------

				x++;
				if (x == p_width)
				{
					x = 0;
					y++;

					// I know this looks really weird, but this is a way of calculating
					// the appropriate distance even when p_elevation is a decimal value,
					// to achieve anti-aliasing, essentially.
					// Also, 0 < distance < 1, which means it doesn't bother with fully transparent or
					// fully opaque pixels.

					// I needed to sit down with pen and paper for like half an hour to figure this out lol

					distance = ((float)y + p_blur - (int32)p_blur) / (p_blur*2.f + 1.f);

					if (y < blurCeiled)
					{
						alphaFactor = 2.f*distance*distance;
					}
					else
					{
						alphaFactor = 4.f*distance - 2.f*distance*distance - 1.f;
					}
				}
			}
		}
		else
		{
			while (x < edgeWidth)
			{
				pixels[x + y * p_width] = getMultipliedColor(pixels[x + y * p_width], alphaFactor);
				pixels[p_width - x - 1 + y * p_width] = getMultipliedColor(pixels[p_height - x - 1 + y * p_width], alphaFactor);
				if (y < p_width)
				{
					pixels[y + x * p_width] = getMultipliedColor(pixels[y + x * p_width], alphaFactor);
					pixels[y + (p_height - x - 1) * p_width] = getMultipliedColor(pixels[y + (p_height - x - 1) * p_width], alphaFactor);
				}

				//------------------------------

				y++;
				if (y == p_height)
				{
					y = 0;
					x++;

					distance = ((float)x + p_blur - (int32)p_blur) / (p_blur*2.f + 1.f);

					if (x < blurCeiled)
					{
						alphaFactor = 2.f*distance*distance;
					}
					else
					{
						alphaFactor = 4.f*distance - 2.f*distance*distance - 1.f;
					}
				}
			}
		}

		//------------------------------

		ID2D1Bitmap1* outputBitmap;
		m_context->CreateBitmap(
			D2D1::SizeU(p_width, p_height),
			pixels, 4*p_width,
			D2D1::BitmapProperties(
				D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &outputBitmap
		);

		delete[] pixels;

		return new Direct2DImage(outputBitmap);
		*/

		if (!p_width || !p_height || !p_color.alpha) return 0;

		p_blur *= 2.f / 3.f;

		m_targetMutex.lock();

		float dpiX = 0;
		float dpiY = 0;
		m_context->GetDpi(&dpiX, &dpiY);
		float dipToPixelFactor = dpiX / USER_DEFAULT_SCREEN_DPI;

		ID2D1Image* targetBefore = 0;
		m_context->GetTarget(&targetBefore);
		m_context->SetDpi(USER_DEFAULT_SCREEN_DPI, USER_DEFAULT_SCREEN_DPI);

		ID2D1Bitmap1* inputBitmap;
		m_context->CreateBitmap(
			D2D1::SizeU(p_width, p_height),
			0, p_width * 4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &inputBitmap
		);

		m_context->SetTarget(inputBitmap);

		m_context->BeginDraw();
		clear(AvoGUI::Color(0.f));
		m_context->EndDraw();
		m_context->SetDpi(dpiX, dpiY);

		//------------------------------
		// Apply effect

		ID2D1Effect* shadowEffect;
		m_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

		shadowEffect->SetInput(0, inputBitmap);
		shadowEffect->SetValue(
			D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_COLOR,
			D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
		);
		shadowEffect->SetValue(D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur / dipToPixelFactor);

		//------------------------------
		// Convert to bitmap

		ID2D1Bitmap1* outputBitmap;
		D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f * p_blur, p_height + 6.f * p_blur);

		m_context->CreateBitmap(
			outputSize,
			0, outputSize.width * 4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CPU_READ,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &outputBitmap
		);

		m_context->SetTarget(outputBitmap);
		m_context->BeginDraw();
		clear();
		m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur * 3.f / dipToPixelFactor, p_blur * 3.f / dipToPixelFactor));
		m_context->EndDraw();
		m_context->SetTarget(targetBefore);

		m_targetMutex.unlock();

		shadowEffect->Release();
		inputBitmap->Release();
		targetBefore->Release();

		return new Direct2DImage(outputBitmap);
	}
	AvoGUI::Image* createRectangleShadowImage(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_blur, AvoGUI::Color const& p_color) override
	{
		return createRectangleShadowImage(p_size.x, p_size.y, p_corners, p_blur, p_color);
	}
	AvoGUI::Image* createRectangleShadowImage(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners, float p_blur, AvoGUI::Color const& p_color) override
	{
		if (!p_width || !p_height || !p_color.alpha) return 0;

		p_blur *= 2.f / 3.f;

		m_targetMutex.lock();

		float dpiX = 0;
		float dpiY = 0;
		m_context->GetDpi(&dpiX, &dpiY);
		float dipToPixelFactor = dpiX / USER_DEFAULT_SCREEN_DPI;

		ID2D1Image* targetBefore = 0;
		m_context->GetTarget(&targetBefore);
		m_context->SetDpi(USER_DEFAULT_SCREEN_DPI, USER_DEFAULT_SCREEN_DPI);
		
		ID2D1Bitmap1* inputBitmap;
		m_context->CreateBitmap(
			D2D1::SizeU(p_width, p_height),
			0, 0,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &inputBitmap
		);

		m_context->SetTarget(inputBitmap);

		m_context->BeginDraw();
		clear();
		setColor(AvoGUI::Color(0.f));
		fillRectangle(p_width, p_height, p_corners);
		m_context->EndDraw();
		m_context->SetDpi(dpiX, dpiY);

		//------------------------------
		// Appy effect

		ID2D1Effect* shadowEffect;
		m_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

		shadowEffect->SetInput(0, inputBitmap);
		shadowEffect->SetValue(
			D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_COLOR,
			D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
		);
		shadowEffect->SetValue(D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur / dipToPixelFactor);

		//------------------------------
		// Convert to bitmap

		ID2D1Bitmap1* outputBitmap;
		D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f*p_blur, p_height + 6.f * p_blur);

		m_context->CreateBitmap(
			outputSize,
			0, outputSize.width * 4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &outputBitmap
		);

		m_context->SetTarget(outputBitmap);
		m_context->BeginDraw();
		clear();
		m_context->DrawImage(shadowEffect, D2D1::Point2F(3.f * p_blur / dipToPixelFactor, 3.f * p_blur / dipToPixelFactor));
		m_context->EndDraw();
		m_context->SetTarget(targetBefore);

		m_targetMutex.unlock();

		shadowEffect->Release();
		inputBitmap->Release();
		targetBefore->Release();

		return new Direct2DImage(outputBitmap);
	}

	AvoGUI::Image* createRoundedRectangleShadowImage(AvoGUI::Point<float> const& p_size, float p_radius, float p_blur, AvoGUI::Color const& p_color)
	{
		return createRoundedRectangleShadowImage(p_size.x, p_size.y, p_radius, p_blur, p_color);
	}
	AvoGUI::Image* createRoundedRectangleShadowImage(float p_width, float p_height, float p_radius, float p_blur, AvoGUI::Color const& p_color)
	{
		if (!p_width || !p_height || !p_color.alpha) return 0;

		p_blur *= 2.f / 3.f;

		m_targetMutex.lock();

		float dpiX = 0;
		float dpiY = 0;
		m_context->GetDpi(&dpiX, &dpiY);
		float dipToPixelFactor = dpiX / USER_DEFAULT_SCREEN_DPI;

		ID2D1Image* targetBefore = 0;
		m_context->GetTarget(&targetBefore);
		m_context->SetDpi(USER_DEFAULT_SCREEN_DPI, USER_DEFAULT_SCREEN_DPI);

		ID2D1Bitmap1* inputBitmap;
		m_context->CreateBitmap(
			D2D1::SizeU(p_width, p_height),
			0, 0,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &inputBitmap
		);

		m_context->SetTarget(inputBitmap);

		m_context->BeginDraw();
		clear();
		setColor(AvoGUI::Color(0.f));
		fillRoundedRectangle(0, 0, p_width, p_height, p_radius);
		m_context->EndDraw();
		m_context->SetDpi(dpiX, dpiY);

		//------------------------------
		// Appy effect

		ID2D1Effect* shadowEffect;
		m_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

		shadowEffect->SetInput(0, inputBitmap);
		shadowEffect->SetValue(
			D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_COLOR,
			D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
		);
		shadowEffect->SetValue(D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur / dipToPixelFactor);

		//------------------------------
		// Convert to bitmap

		ID2D1Bitmap1* outputBitmap;
		D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f * p_blur, p_height + 6.f * p_blur);

		m_context->CreateBitmap(
			outputSize,
			0, outputSize.width * 4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &outputBitmap
		);

		m_context->SetTarget(outputBitmap);
		m_context->BeginDraw();
		clear();
		m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur * 3.f / dipToPixelFactor, p_blur * 3.f / dipToPixelFactor));
		m_context->EndDraw();
		m_context->SetTarget(targetBefore);

		m_targetMutex.unlock();

		shadowEffect->Release();
		inputBitmap->Release();
		targetBefore->Release();

		return new Direct2DImage(outputBitmap);
	}

	//------------------------------

	AvoGUI::Image* createImage(uint8 const* p_pixelData, uint32 p_width, uint32 p_height) override
	{
		ID2D1Bitmap1* bitmap = 0;
		m_context->CreateBitmap(
			D2D1::Size(p_width, p_height),
			p_pixelData, p_width * 4,
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_NONE,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			), &bitmap
		);
		return new Direct2DImage(bitmap);
	}
	AvoGUI::Image* createImage(void const* p_imageData, uint32 p_size) override
	{
		IWICStream* stream = 0;
		s_imagingFactory->CreateStream(&stream);
		stream->InitializeFromMemory((BYTE*)p_imageData, p_size);

		IWICBitmapDecoder* decoder = 0;
		s_imagingFactory->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnDemand, &decoder);

		Direct2DImage* result = 0;
		if (decoder)
		{
			IWICBitmapFrameDecode* frame = 0;
			decoder->GetFrame(0, &frame);

			if (frame)
			{
				IWICFormatConverter* formatConverter = 0;
				s_imagingFactory->CreateFormatConverter(&formatConverter);
				formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.f, WICBitmapPaletteTypeMedianCut);

				ID2D1Bitmap1* bitmap;
				m_context->CreateBitmapFromWicBitmap(formatConverter, 0, &bitmap);
				if (bitmap)
				{
					result = new Direct2DImage(bitmap);
				}

				formatConverter->Release();
				frame->Release();
			}
			decoder->Release();
		}
		stream->Release();

		return result;
	}
	AvoGUI::Image* createImage(char const* p_filePath) override
	{
		wchar_t wideFilePath[MAX_PATH];
		AvoGUI::convertUtf8ToUtf16(p_filePath, wideFilePath, MAX_PATH);

		IWICBitmapDecoder* decoder = 0;
		s_imagingFactory->CreateDecoderFromFilename(wideFilePath, 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
		if (!decoder)
		{
			return 0;
		}

		IWICBitmapFrameDecode* frame = 0;
		decoder->GetFrame(0, &frame);

		Direct2DImage* result = 0;
		if (frame)
		{
			IWICFormatConverter* formatConverter = 0;
			s_imagingFactory->CreateFormatConverter(&formatConverter);
			formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.f, WICBitmapPaletteTypeMedianCut);

			ID2D1Bitmap1* bitmap;
			m_context->CreateBitmapFromWicBitmap(formatConverter, 0, &bitmap);
			if (bitmap)
			{
				result = new Direct2DImage(bitmap);
			}

			formatConverter->Release();
			frame->Release();
		}

		decoder->Release();

		return result;
	}
	AvoGUI::Image* createImage(void* p_handle) override
	{
		IWICBitmap* wicBitmap = 0;
		HRESULT result = s_imagingFactory->CreateBitmapFromHICON((HICON)p_handle, &wicBitmap);

		if (result < 0)
		{
			if (wicBitmap)
			{
				wicBitmap->Release();
			}
			result = s_imagingFactory->CreateBitmapFromHBITMAP((HBITMAP)p_handle, 0, WICBitmapAlphaChannelOption::WICBitmapUseAlpha, &wicBitmap);
		}

		IWICFormatConverter* formatConverter = 0;
		s_imagingFactory->CreateFormatConverter(&formatConverter);
		formatConverter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.f, WICBitmapPaletteTypeMedianCut);

		ID2D1Bitmap1* bitmap;
		m_context->CreateBitmapFromWicBitmap(formatConverter, &bitmap);

		formatConverter->Release();
		wicBitmap->Release();

		return new Direct2DImage(bitmap);
	}

	//------------------------------

	void drawImage(AvoGUI::Image* p_image, float p_multiplicativeOpacity) override
	{
		if (!p_image)
		{
			return;
		}

		AvoGUI::Rectangle<float> const& cropRectangle = p_image->getCropRectangle();
		AvoGUI::Rectangle<float> innerBounds = p_image->getInnerBounds();

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawBitmap(
			(ID2D1Bitmap1*)p_image->getHandle(),
			D2D1::RectF(innerBounds.left, innerBounds.top, innerBounds.right, innerBounds.bottom),
			p_image->getOpacity()* m_currentBrush->GetOpacity() * p_multiplicativeOpacity,
			p_image->getScalingMethod() == AvoGUI::ImageScalingMethod::Pixelated ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(cropRectangle.left, cropRectangle.top, cropRectangle.right, cropRectangle.bottom)
		);
	}

	//------------------------------

	std::string createImageFileData(AvoGUI::Image* p_image, AvoGUI::ImageFormat p_format) override
	{
		ID2D1Bitmap1* direct2dBitmap = (ID2D1Bitmap1*)p_image->getHandle();

		IStream* outputStream = SHCreateMemStream(0, 0);

		GUID formatGuid;
		switch (p_format)
		{
			case AvoGUI::ImageFormat::Jpeg:
				formatGuid = GUID_ContainerFormatJpeg;
				break;
			case AvoGUI::ImageFormat::Png:
				formatGuid = GUID_ContainerFormatPng;
				break;
			case AvoGUI::ImageFormat::Bmp:
				formatGuid = GUID_ContainerFormatBmp;
				break;
			case AvoGUI::ImageFormat::Ico:
				formatGuid = GUID_ContainerFormatIco;
				break;
		}

		IWICBitmapEncoder* bitmapEncoder = 0;
		s_imagingFactory->CreateEncoder(formatGuid, 0, &bitmapEncoder);
		bitmapEncoder->Initialize(outputStream, WICBitmapEncoderNoCache);

		IWICBitmapFrameEncode* frameEncoder = 0;
		bitmapEncoder->CreateNewFrame(&frameEncoder, 0);
		frameEncoder->Initialize(0);

		ID2D1Device* device = 0;
		m_context->GetDevice(&device);

		IWICImageEncoder* imageEncoder;
		s_imagingFactory->CreateImageEncoder(device, &imageEncoder);

		imageEncoder->WriteFrame(direct2dBitmap, frameEncoder, 0);

		frameEncoder->Commit();
		bitmapEncoder->Commit();

		imageEncoder->Release();
		device->Release();
		frameEncoder->Release();
		bitmapEncoder->Release();

		//------------------------------

		STATSTG stats;
		outputStream->Stat(&stats, STATFLAG_NONAME);

		std::string buffer;
		buffer.resize(stats.cbSize.QuadPart);

		ULONG numberOfBytesWritten = 0;
		outputStream->Seek({ 0 }, SEEK_SET, 0);
		outputStream->Write(buffer.data(), buffer.size(), &numberOfBytesWritten);

		if (numberOfBytesWritten != buffer.size())
		{
			buffer.resize(numberOfBytesWritten);
		}

		return buffer;
	}
	void* createImageFileDataNativeStream(AvoGUI::Image* p_image, AvoGUI::ImageFormat p_format) override
	{
		ID2D1Bitmap1* direct2dBitmap = (ID2D1Bitmap1*)p_image->getHandle();

		IStream* outputStream = SHCreateMemStream(0, 0);

		GUID formatGuid;
		switch (p_format)
		{
			case AvoGUI::ImageFormat::Jpeg:
				formatGuid = GUID_ContainerFormatJpeg;
				break;
			case AvoGUI::ImageFormat::Png:
				formatGuid = GUID_ContainerFormatPng;
				break;
			case AvoGUI::ImageFormat::Bmp:
				formatGuid = GUID_ContainerFormatBmp;
				break;
			case AvoGUI::ImageFormat::Ico:
				formatGuid = GUID_ContainerFormatIco;
		}

		IWICBitmapEncoder* bitmapEncoder = 0;
		s_imagingFactory->CreateEncoder(formatGuid, 0, &bitmapEncoder);
		bitmapEncoder->Initialize(outputStream, WICBitmapEncoderNoCache);

		IWICBitmapFrameEncode* frameEncoder = 0;
		bitmapEncoder->CreateNewFrame(&frameEncoder, 0);
		frameEncoder->Initialize(0);

		ID2D1Device* device = 0;
		m_context->GetDevice(&device);

		IWICImageEncoder* imageEncoder;
		s_imagingFactory->CreateImageEncoder(device, &imageEncoder);

		imageEncoder->WriteFrame(direct2dBitmap, frameEncoder, 0);

		frameEncoder->Commit();
		bitmapEncoder->Commit();

		imageEncoder->Release();
		device->Release();
		frameEncoder->Release();
		bitmapEncoder->Release();

		return outputStream;
	}
	void saveImageToFile(AvoGUI::Image* p_image, std::string const& p_filePath, AvoGUI::ImageFormat p_format = AvoGUI::ImageFormat::Png) override
	{
		ID2D1Bitmap1* direct2dBitmap = (ID2D1Bitmap1*)p_image->getHandle();

		IStream* outputStream = 0;
		SHCreateStreamOnFileW(AvoGUI::convertUtf8ToUtf16(p_filePath).c_str(), STGM_CREATE | STGM_WRITE, &outputStream);

		GUID formatGuid;
		switch (p_format)
		{
			case AvoGUI::ImageFormat::Jpeg:
				formatGuid = GUID_ContainerFormatJpeg;
				break;
			case AvoGUI::ImageFormat::Png:
				formatGuid = GUID_ContainerFormatPng;
				break;
			case AvoGUI::ImageFormat::Bmp:
				formatGuid = GUID_ContainerFormatBmp;
				break;
			case AvoGUI::ImageFormat::Ico:
				formatGuid = GUID_ContainerFormatIco;
		}

		IWICBitmapEncoder* bitmapEncoder = 0;
		s_imagingFactory->CreateEncoder(formatGuid, 0, &bitmapEncoder);
		bitmapEncoder->Initialize(outputStream, WICBitmapEncoderNoCache);

		IWICBitmapFrameEncode* frameEncoder = 0;
		bitmapEncoder->CreateNewFrame(&frameEncoder, 0);
		frameEncoder->Initialize(0);

		ID2D1Device* device = 0;
		m_context->GetDevice(&device);

		IWICImageEncoder* imageEncoder;
		s_imagingFactory->CreateImageEncoder(device, &imageEncoder);

		imageEncoder->WriteFrame(direct2dBitmap, frameEncoder, 0);

		frameEncoder->Commit();
		bitmapEncoder->Commit();

		imageEncoder->Release();
		device->Release();
		frameEncoder->Release();
		bitmapEncoder->Release();

		outputStream->Release();
	}

	//------------------------------

	void* createNativeImageFromImage(AvoGUI::Image* p_image) override
	{
		ID2D1Bitmap1* sourceBitmap = (ID2D1Bitmap1*)p_image->getHandle();
		D2D1_SIZE_U size = sourceBitmap->GetPixelSize();

		ID2D1Bitmap1* cpuBitmap = 0;
		if (sourceBitmap->GetOptions() & D2D1_BITMAP_OPTIONS_CPU_READ)
		{
			cpuBitmap = sourceBitmap;
			cpuBitmap->AddRef();
		}
		else
		{
			m_context->CreateBitmap(size, 0, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, sourceBitmap->GetPixelFormat()), &cpuBitmap);
		}

		D2D1_POINT_2U destinationPoint{ 0, 0 };
		D2D1_RECT_U sourceRectangle{ 0, 0, size.width, size.height };
		cpuBitmap->CopyFromBitmap(&destinationPoint, sourceBitmap, &sourceRectangle);

		// Pixel data
		D2D1_MAPPED_RECT mappedRectangle;
		cpuBitmap->Map(D2D1_MAP_OPTIONS_READ, &mappedRectangle);
		
		BITMAPINFOHEADER bitmapInfoHeader = { 0 };
		bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader.biWidth = ceil(size.width/16.f)*16;
		bitmapInfoHeader.biHeight = -(long)size.height;
		bitmapInfoHeader.biBitCount = 32;
		bitmapInfoHeader.biCompression = BI_RGB;
		bitmapInfoHeader.biPlanes = 1;
		HBITMAP result = CreateDIBitmap(GetDC((HWND)m_window->getNativeHandle()), &bitmapInfoHeader, CBM_INIT, mappedRectangle.bits, (BITMAPINFO*)&bitmapInfoHeader, DIB_RGB_COLORS);

		cpuBitmap->Unmap();
		cpuBitmap->Release();

		return result;
	}

	//------------------------------

	AvoGUI::LinearGradient* createLinearGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_endX, float p_endY) override
	{
		D2D1_GRADIENT_STOP* gradientStops = new D2D1_GRADIENT_STOP[p_gradientStops.size()];
		for (uint32 a = 0; a < p_gradientStops.size(); a++)
		{
			AvoGUI::Color const& color = p_gradientStops[a].color;
			gradientStops[a].color = D2D1::ColorF(color.red, color.green, color.blue, color.alpha);
			gradientStops[a].position = p_gradientStops[a].position;
		}

		ID2D1GradientStopCollection* stopCollection = 0;
		m_context->CreateGradientStopCollection(gradientStops, p_gradientStops.size(), &stopCollection);
		delete[] gradientStops;

		ID2D1LinearGradientBrush* brush = 0;
		m_context->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(p_startX, p_startY), D2D1::Point2F(p_endX, p_endY)), stopCollection, &brush);

		stopCollection->Release();

		return new Direct2DLinearGradient(brush);
	}
	AvoGUI::LinearGradient* createLinearGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, AvoGUI::Point<float> const& p_endPosition) override
	{
		return createLinearGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_endPosition.x, p_endPosition.y);
	}

	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radiusX, float p_radiusY) override
	{
		D2D1_GRADIENT_STOP* gradientStops = new D2D1_GRADIENT_STOP[p_gradientStops.size()];
		for (uint32 a = 0; a < p_gradientStops.size(); a++)
		{
			AvoGUI::Color const& color = p_gradientStops[a].color;
			gradientStops[a].color = D2D1::ColorF(color.red, color.green, color.blue, color.alpha);
			gradientStops[a].position = p_gradientStops[a].position;
		}

		ID2D1GradientStopCollection* stopCollection = 0;
		m_context->CreateGradientStopCollection(gradientStops, p_gradientStops.size(), &stopCollection);
		delete[] gradientStops;

		ID2D1RadialGradientBrush* brush = 0;
		m_context->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(p_startX, p_startY), D2D1_POINT_2F(), p_radiusX, p_radiusY), stopCollection, &brush);

		stopCollection->Release();

		return new Direct2DRadialGradient(brush);
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) override
	{
		return createRadialGradient(p_gradientStops, p_startX, p_startY, p_radius, p_radius);
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, float p_radiusX, float p_radiusY) override
	{
		return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radiusX, p_radiusY);
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, float p_radius) override
	{
		return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radius, p_radius);
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, AvoGUI::Point<float> const& p_radius) override
	{
		return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radius.x, p_radius.y);
	}

	void setGradient(AvoGUI::LinearGradient* p_gradient) override
	{
		m_currentBrush = ((Direct2DLinearGradient*)p_gradient)->getHandle();
	}
	void setGradient(AvoGUI::RadialGradient* p_gradient) override
	{
		m_currentBrush = ((Direct2DRadialGradient*)p_gradient)->getHandle();
	}

	void setColor(AvoGUI::Color const& p_color) override
	{
		m_solidColorBrush->SetColor(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
		m_currentBrush = m_solidColorBrush;
	}
	void setOpacity(float p_opacity) override
	{
		m_brushOpacity = p_opacity;
	}

	//------------------------------

	void addFont(std::string const& p_filePath) override
	{
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(AvoGUI::readFile(p_filePath)));
		updateFontCollection();
	}

	void addFont(std::vector<uint8> const& p_data) override
	{
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(p_data));
		updateFontCollection();
	}
	void addFont(std::vector<uint8>&& p_data) override
	{
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(std::move(p_data)));
		updateFontCollection();
	}

	void addFont(uint8 const* p_data, uint32 p_dataSize) override
	{
		m_fontData.emplace_back(std::make_shared<std::vector<uint8>>(p_data, p_data + p_dataSize));
		updateFontCollection();
	}

	//------------------------------

	void setDefaultTextProperties(AvoGUI::TextProperties const& p_textProperties) override
	{
		wchar_t fontFamily[100];
		AvoGUI::convertUtf8ToUtf16(p_textProperties.fontFamilyName, fontFamily, 100);

		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
		if (p_textProperties.fontStyle == AvoGUI::FontStyle::Italic)
		{
			fontStyle = DWRITE_FONT_STYLE_ITALIC;
		}
		else if (p_textProperties.fontStyle == AvoGUI::FontStyle::Oblique)
		{
			fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
		}

		s_directWriteFactory->CreateTextFormat(
			fontFamily, m_fontCollection, (DWRITE_FONT_WEIGHT)p_textProperties.fontWeight,
			fontStyle, (DWRITE_FONT_STRETCH)p_textProperties.fontStretch,
			p_textProperties.fontSize, L"",
			(IDWriteTextFormat**)&m_textFormat
		);

		switch (p_textProperties.textAlign)
		{
			case AvoGUI::TextAlign::Left:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case AvoGUI::TextAlign::Center:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case AvoGUI::TextAlign::Right:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			case AvoGUI::TextAlign::Fill:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
		}

		switch (p_textProperties.readingDirection)
		{
			case AvoGUI::ReadingDirection::RightToLeft:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
				break;
			case AvoGUI::ReadingDirection::TopToBottom:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM);
				break;
			case AvoGUI::ReadingDirection::BottomToTop:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP);
		}
		m_textFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD::DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, p_textProperties.lineHeight, p_textProperties.lineHeight * 0.8f);

		m_textProperties = p_textProperties;
	}
	AvoGUI::TextProperties getDefaultTextProperties() override
	{
		return m_textProperties;
	}

	//------------------------------

	AvoGUI::Text* createText(char const* p_string, float p_fontSize, AvoGUI::Rectangle<float> p_bounds = AvoGUI::Rectangle<float>()) override
	{
		p_bounds.clipNegativeSpace();

		std::wstring wideString = AvoGUI::convertUtf8ToUtf16(p_string);

		IDWriteTextLayout1* textLayout;
		s_directWriteFactory->CreateTextLayout(wideString.data(), wideString.size(), m_textFormat, p_bounds.getWidth(), p_bounds.getHeight(), (IDWriteTextLayout**)&textLayout);

		DWRITE_TEXT_RANGE textRange;
		textRange.startPosition = 0;
		textRange.length = wideString.size();
		textLayout->SetFontSize(p_fontSize, textRange);
		textLayout->SetCharacterSpacing(m_textProperties.characterSpacing*0.5f, m_textProperties.characterSpacing*0.5f, 0.f, textRange);

		return new DirectWriteText(textLayout, wideString, p_string, p_bounds);
	}
	AvoGUI::Text* createText(std::string const& p_string, float p_fontSize, AvoGUI::Rectangle<float> p_bounds = AvoGUI::Rectangle<float>()) override
	{
		p_bounds.clipNegativeSpace();

		int32 numberOfCharacters = MultiByteToWideChar(CP_UTF8, 0, p_string.c_str(), p_string.size(), 0, 0);
		wchar_t* wideString = new wchar_t[numberOfCharacters];
		MultiByteToWideChar(CP_UTF8, 0, p_string.c_str(), p_string.size(), wideString, numberOfCharacters);

		IDWriteTextLayout1* textLayout;
		s_directWriteFactory->CreateTextLayout(wideString, numberOfCharacters, m_textFormat, p_bounds.getWidth(), p_bounds.getHeight(), (IDWriteTextLayout**)&textLayout);
		DWRITE_TEXT_RANGE textRange;
		textRange.startPosition = 0;
		textRange.length = numberOfCharacters;
		textLayout->SetFontSize(p_fontSize, textRange);
		textLayout->SetCharacterSpacing(m_textProperties.characterSpacing * 0.5f, m_textProperties.characterSpacing * 0.5f, 0.f, textRange);

		std::wstring wideStringObject = wideString;
		delete[] wideString;

		return new DirectWriteText(textLayout, wideStringObject, p_string, p_bounds);
	}
	void drawText(AvoGUI::Text* p_text) override
	{
		if (!p_text)
		{
			return;
		}
		IDWriteTextLayout1* textLayout = (IDWriteTextLayout1*)p_text->getHandle();
		DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
		if (p_text->getIsTopTrimmed())
		{
			textLayout->GetOverhangMetrics(&overhangMetrics);
		}
		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawTextLayout(
			D2D1::Point2F(p_text->getTopLeft().x, p_text->getTopLeft().y + overhangMetrics.top), 
			textLayout, m_currentBrush, D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
		);
	}
	void drawText(char const* p_string, AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		if (p_string == "") return;

		int32 numberOfCharacters = MultiByteToWideChar(CP_UTF8, 0, p_string, -1, 0, 0);
		wchar_t* wideString = new wchar_t[numberOfCharacters];
		MultiByteToWideChar(CP_UTF8, 0, p_string, -1, wideString, numberOfCharacters);

		m_currentBrush->SetOpacity(m_brushOpacity);
		m_context->DrawTextW(
			wideString, numberOfCharacters, m_textFormat,
			D2D1::RectF(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom),
			m_currentBrush, D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_NONE
		);
		delete[] wideString;
	}
	void drawText(char const* p_string, float p_left, float p_top, float p_right, float p_bottom) override
	{
		drawText(p_string, AvoGUI::Rectangle<float>(p_left, p_top, p_right, p_bottom));
	}
	void drawText(char const* p_string, AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size) override
	{
		drawText(p_string, AvoGUI::Rectangle<float>(p_position, p_size));
	}
	void drawText(char const* p_string, float p_x, float p_y) override
	{
		drawText(p_string, AvoGUI::Rectangle<float>(p_x, p_y, m_context->GetSize().width, m_context->GetSize().height));
	}
	void drawText(char const* p_string, AvoGUI::Point<float> const& p_position) override
	{
		drawText(p_string, AvoGUI::Rectangle<float>(p_position.x, p_position.y, m_context->GetSize().width * 2, m_context->GetSize().height * 2));
	}
};
ID2D1Factory2* Direct2DDrawingContext::s_direct2DFactory{nullptr};
IDWriteFactory1* Direct2DDrawingContext::s_directWriteFactory{nullptr};
FontCollectionLoader* Direct2DDrawingContext::s_fontCollectionLoader{nullptr};
FontFileLoader* Direct2DDrawingContext::s_fontFileLoader{nullptr};
IWICImagingFactory2* Direct2DDrawingContext::s_imagingFactory{nullptr};
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
	GLuint m_programID{0};
	GLuint m_vertexArrayObjectID{0};

	GLuint m_vertexBufferID{0};
	uint32 m_vertexBufferSize{0};
	uint32 m_vertexSize{0};

	GLuint m_indexBufferID{0};
	uint32 m_indexBufferSize{0};

public:
	OpenGlShader() = default;
	~OpenGlShader()
	{
		if (m_programID)
		{
			glDeleteProgram(m_programID);
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
			std::cout << "\nVertex shader compile error(s)!\n" << message << '\n';
		}

		// Check for fragment shader compile errors
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &wasSuccessful);
		if (!wasSuccessful)
		{
			GLchar message[256];
			glGetShaderInfoLog(fragmentShaderID, 256, nullptr, message);
			std::cout << "\nFragment shader compile error(s)!\n" << message << '\n';
		}
#endif

		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShaderID);
		glAttachShader(m_programID, fragmentShaderID);
		glLinkProgram(m_programID);

		glDeleteShader(fragmentShaderID);
		glDeleteShader(vertexShaderID);

		m_vertexArrayObjectID = 0;
		glGenVertexArrays(1, &m_vertexArrayObjectID);

		m_vertexBufferID = 0;
		glGenBuffers(1, &m_vertexBufferID);
	}

	void setVertexData(float* p_data, uint32 p_dataSize, uint32 p_vertexSize, GLenum p_usage = GL_STATIC_DRAW)
	{
        glBindVertexArray(m_vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, p_dataSize, p_data, p_usage);

		m_vertexBufferSize = p_dataSize;
		m_vertexSize = p_vertexSize;
	}
	/*
		Sets information about a vertex shader input.
		p_shaderInputIndex is the layout index of the "in" variable in the vertex shader.
		p_numberOfComponentsInShaderInput is the number of floats stored in the "in" variable, and is either 1, 2, 3 or 4.
		p_pointerPosition is the start position of the input in the vertex buffer, in number of floats.
	*/
	void configureVertexShaderInput(uint32 p_shaderInputIndex, uint32 p_numberOfComponentsInShaderInput, uint32 p_pointerPosition)
	{
		glBindVertexArray(m_vertexArrayObjectID);

		glVertexAttribPointer(p_shaderInputIndex, p_numberOfComponentsInShaderInput, GL_FLOAT, GL_FALSE, m_vertexSize, (void*)(p_pointerPosition*sizeof(float)));
		glEnableVertexAttribArray(p_shaderInputIndex);
	}

	void setVertexDataOrder(uint32* p_indices, uint32 p_indexCount, GLenum p_usage = GL_STATIC_DRAW)
    {
        glBindVertexArray(m_vertexArrayObjectID);

	    glGenBuffers(1, &m_indexBufferID);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_indexCount, p_indices, p_usage);

	    m_indexBufferSize = p_indexCount;
    }

    void setUniformValue(char const* p_name, float p_value)
    {
		glUniform1f(glGetUniformLocation(m_programID, p_name), p_value);
    }
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1)
	{
		glUniform2f(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1);
	}
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1, float p_value_2)
	{
		glUniform3f(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1, p_value_2);
	}
	void setUniformVector(char const* p_name, float p_value_0, float p_value_1, float p_value_2, float p_value_3)
	{
		glUniform4f(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1, p_value_2, p_value_3);
	}
	void setUniformValue(char const* p_name, int p_value)
	{
		glUniform1i(glGetUniformLocation(m_programID, p_name), p_value);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1)
	{
		glUniform2i(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1, int p_value_2)
	{
		glUniform3i(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1, p_value_2);
	}
	void setUniformVector(char const* p_name, int p_value_0, int p_value_1, int p_value_2, int p_value_3)
	{
		glUniform4i(glGetUniformLocation(m_programID, p_name), p_value_0, p_value_1, p_value_2, p_value_3);
	}
    void setUniformColor(char const* p_name, AvoGUI::Color const& p_color)
    {
		glUniform4f(glGetUniformLocation(m_programID, p_name), p_color.red, p_color.green, p_color.blue, p_color.alpha);
    }
	/*
		Sets a matrix uniform with 2 rows and 3 columns.
		The values are in column-major order.
	*/
	void setUniformMatrix3x3(char const* p_name, float* p_values)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_programID, p_name), 1, GL_FALSE, p_values);
	}

	void use()
	{
		glUseProgram(m_programID);
	}

    /*
		If p_numberOfVerticesToRender is -1, it renders all vertices or all referred to by a previous call to setVertexDataOrder.
    */
    void draw(uint32 p_startVertexIndex = 0, uint32 p_numberOfVerticesToDraw = -1, GLenum p_mode = GL_TRIANGLES)
    {
	    glBindVertexArray(m_vertexArrayObjectID);

        if (m_indexBufferID)
        {
            glDrawElements(p_mode, p_numberOfVerticesToDraw == -1 ? m_indexBufferSize : p_numberOfVerticesToDraw, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(p_mode, p_startVertexIndex, p_numberOfVerticesToDraw == -1 ? m_vertexBufferSize/m_vertexSize : p_numberOfVerticesToDraw);
        }
    }
};

//------------------------------

class ClippingShape
{
private:
	AvoGUI::Rectangle<float> m_bounds;
	
public:
	explicit ClippingShape(AvoGUI::Rectangle<float> const& p_bounds) :
		m_bounds(p_bounds)
	{
	}
	ClippingShape(float p_left, float p_top, float p_right, float p_bottom) :
		m_bounds(p_left, p_top, p_right, p_bottom)
	{
	}

	AvoGUI::Rectangle<float> getBounds()
	{
		return m_bounds;
	}
};

class OpenGlDrawCall
{
public:
	uint32 vertexBufferStartIndex{0};
	uint32 numberOfVertices{0};
	GLenum primitiveType{GL_TRIANGLES};
	AvoGUI::Color color;
	int renderMode{0};
	float lineWidth{1.f};
	GLuint texture{0u};
	float transform[9];
};

class OpenGlGeometry : public AvoGUI::Geometry
{
public:
	std::vector<float> vertexBuffer;
	bool isStroked{false};
};

class OpenGlImage : public AvoGUI::Image
{
private:
	GLuint m_textureID;
	AvoGUI::Point<uint32> m_originalSize;

	AvoGUI::ImageScalingMethod m_scalingMethod{AvoGUI::ImageScalingMethod::Pixelated};
	AvoGUI::ImageBoundsSizing m_boundsSizing{AvoGUI::ImageBoundsSizing::Contain};
	AvoGUI::Point<float> m_boundsPositioning{0.5f, 0.5f};

	AvoGUI::Rectangle<float> m_cropRectangle;
	float m_opacity{1.f};

public:
	OpenGlImage(GLuint p_textureID, uint32 p_width, uint32 p_height, float p_dipToPixelFactor) :
		m_textureID(p_textureID), m_originalSize(p_width, p_height)
	{
		m_bounds.set(0.f, 0.f, p_width*p_dipToPixelFactor, p_height*p_dipToPixelFactor);
		m_cropRectangle = m_bounds;
	}

	//------------------------------

	void setCropRectangle(AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		m_cropRectangle = p_rectangle;
	}
	AvoGUI::Rectangle<float> const& getCropRectangle() const override
	{
		return m_cropRectangle;
	}

	AvoGUI::Point<uint32> getOriginalSize() const override
	{
		return m_originalSize;
	}
	uint32 getOriginalWidth() const override
	{
		return m_originalSize.x;
	}
	uint32 getOriginalHeight() const override
	{
		return m_originalSize.y;
	}

	//------------------------------

	void setBoundsSizing(AvoGUI::ImageBoundsSizing p_sizeMode) override
	{
		m_boundsSizing = p_sizeMode;
	}
	AvoGUI::ImageBoundsSizing getBoundsSizing() const override
	{
		return m_boundsSizing;
	}

	void setBoundsPositioning(float p_x, float p_y) override
	{
		m_boundsPositioning.set(p_x, p_y);
	}
	void setBoundsPositioningX(float p_x) override
	{
		m_boundsPositioning.x = p_x;
	}
	void setBoundsPositioningY(float p_y) override
	{
		m_boundsPositioning.y = p_y;
	}
	AvoGUI::Point<float> const& getBoundsPositioning() const override
	{
		return m_boundsPositioning;
	}
	float getBoundsPositioningX() const override
	{
		return m_boundsPositioning.x;
	}
	float getBoundsPositioningY() const override
	{
		return m_boundsPositioning.x;
	}

	//------------------------------

	void setScalingMethod(AvoGUI::ImageScalingMethod p_scalingMethod) override
	{
		m_scalingMethod = p_scalingMethod;
	}
	AvoGUI::ImageScalingMethod getScalingMethod() const override
	{
		return m_scalingMethod;
	}

	//------------------------------

	void setOpacity(float p_opacity) override
	{
		m_opacity = p_opacity;
	}
	float getOpacity() const override
	{
		return m_opacity;
	}

	//------------------------------

	float getInnerWidth() const override
	{
		if (m_boundsSizing != AvoGUI::ImageBoundsSizing::Stretch &&
		    m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() < m_originalSize.x / (float)m_originalSize.y)
		{
			return m_bounds.getHeight() * m_originalSize.x / (float)m_originalSize.y;
		}
		return m_bounds.getWidth();
	}
	float getInnerHeight() const override
	{
		if (m_boundsSizing != AvoGUI::ImageBoundsSizing::Stretch &&
		    m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() > m_originalSize.x / (float)m_originalSize.y)
		{
			return m_bounds.getWidth() * m_originalSize.y / (float)m_originalSize.x;
		}
		return m_bounds.getHeight();
	}
	AvoGUI::Point<float> getInnerSize() const override
	{
		return AvoGUI::Point<float>(getInnerWidth(), getInnerHeight());
	}
	AvoGUI::Rectangle<float> getInnerBounds() const override
	{
		if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Stretch)
		{
			return m_bounds;
		}

		AvoGUI::Rectangle<float> innerBounds = m_bounds;

		bool areBoundsProportionallyWider = m_bounds.getWidth() / m_bounds.getHeight() > m_originalSize.x / (float)m_originalSize.y;
		if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Fill != areBoundsProportionallyWider)
		{
			innerBounds.setWidth(m_bounds.getHeight() * m_originalSize.x / (float)m_originalSize.y);
		}
		else if (m_boundsSizing == AvoGUI::ImageBoundsSizing::Contain != areBoundsProportionallyWider)
		{
			innerBounds.setHeight(m_bounds.getWidth() * m_originalSize.y / (float)m_originalSize.x);
		}

		innerBounds.move(m_boundsPositioning.x * (m_bounds.getWidth() - innerBounds.getWidth()), m_boundsPositioning.y * (m_bounds.getHeight() - innerBounds.getHeight()));

		return innerBounds;
	}

	void* getHandle() const override
	{
		return (void*)&m_textureID;
	}
};

class OpenGlDrawingContext :
	public AvoGUI::DrawingContext
{
private:
	static constexpr uint32 CIRCLE_RESOLUTION{50};
	static constexpr uint32 VERTEX_DATA_LENGTH{8};

	static bool s_areStaticVariablesInitialized;
	static AvoGUI::Point<float> s_unitCirclePoints[CIRCLE_RESOLUTION + 1];

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
			s_unitCirclePoints[a].setPolar((float)AvoGUI::TAU*a/(float)CIRCLE_RESOLUTION);
		}

		s_areStaticVariablesInitialized = true;
	}

private:
	LinuxWindow* m_window;
	XWindow m_windowHandle;
	Display* m_server;
	GLXContext m_context;
	AvoGUI::Color m_backgroundColor;
	
	float m_transformMatrix[9]{};
	AvoGUI::Point<float> m_scale{1.f, 1.f};

	AvoGUI::Color m_currentColor;
	OpenGlShader m_renderShader;
	std::vector<OpenGlDrawCall> m_drawCalls;
	std::vector<float> m_vertexBuffer;

	std::stack<ClippingShape> m_clippingShapeStack;
	AvoGUI::LineJoin m_lineJoin{AvoGUI::LineJoin::Miter};
	AvoGUI::LineCap m_lineCap{AvoGUI::LineCap::Round};

	float m_dipToPixelFactor{1.f};
	AvoGUI::Point<float> m_size;

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
	void addDrawCall(uint32 p_numberOfVertices, GLuint p_texture = 0u, GLenum p_primitiveType = GL_TRIANGLES)
	{
		OpenGlDrawCall drawCall;
		drawCall.vertexBufferStartIndex = uint32(m_vertexBuffer.size()/VERTEX_DATA_LENGTH - p_numberOfVertices);
		drawCall.numberOfVertices = p_numberOfVertices;
		drawCall.primitiveType = p_primitiveType;
		drawCall.color = m_currentColor;
		drawCall.renderMode = bool(p_texture) + 1;
		drawCall.texture = p_texture;
		m_drawCalls.push_back(drawCall);
		memcpy(m_drawCalls.back().transform, m_transformMatrix, sizeof(m_transformMatrix));
	}
	/*
		This is called after the vertices have been added to the buffer.
	*/
	void addDrawCall(uint32 p_numberOfVertices, float p_lineWidth, GLenum p_primitiveType = GL_TRIANGLES)
	{
		OpenGlDrawCall drawCall;
		drawCall.vertexBufferStartIndex = uint32(m_vertexBuffer.size()/VERTEX_DATA_LENGTH - p_numberOfVertices);
		drawCall.numberOfVertices = p_numberOfVertices;
		drawCall.primitiveType = p_primitiveType;
		drawCall.color = m_currentColor;
		drawCall.renderMode = 0;
		drawCall.lineWidth = p_lineWidth;
		m_drawCalls.push_back(drawCall);
		memcpy(m_drawCalls.back().transform, m_transformMatrix, sizeof(m_transformMatrix));
	}

	static uint32 getCirclePointStrideForRadiusSquared(float p_radiusSquared)
	{
		return p_radiusSquared <= 25.f*25.f ? (p_radiusSquared <= 5.f*5.f ? 4 : 2) : 1;
	}
	/*
		The primitive type is GL_TRIANGLES
	*/
	static void createStrokedCornerRectangleGeometry(std::vector<float>& p_buffer, float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners p_corners)
	{
		if (p_right - p_left < p_corners.topLeftSizeX + p_corners.topRightSizeY)
		{
			float factor = (p_right - p_left)/(p_corners.topLeftSizeX + p_corners.topRightSizeX);
			p_corners.topLeftSizeX *= factor;
			p_corners.topRightSizeX *= factor;
		}
		if (p_right - p_left < p_corners.bottomLeftSizeX + p_corners.bottomRightSizeY)
		{
			float factor = (p_right - p_left)/(p_corners.bottomLeftSizeX + p_corners.bottomRightSizeX);
			p_corners.bottomLeftSizeX *= factor;
			p_corners.bottomRightSizeX *= factor;
		}
		if (p_bottom - p_top < p_corners.topLeftSizeX + p_corners.bottomLeftSizeY)
		{
			float factor = (p_bottom - p_top)/(p_corners.topLeftSizeX + p_corners.bottomLeftSizeY);
			p_corners.topLeftSizeX *= factor;
			p_corners.bottomLeftSizeX *= factor;
		}
		if (p_bottom - p_top < p_corners.topRightSizeX + p_corners.bottomRightSizeY)
		{
			float factor = (p_bottom - p_top)/(p_corners.topRightSizeX + p_corners.bottomRightSizeY);
			p_corners.topRightSizeX *= factor;
			p_corners.bottomRightSizeX *= factor;
		}

		p_buffer.insert(
			p_buffer.end(),
			{
				p_left + p_corners.topLeftSizeX  , p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.topLeftSizeX  , p_top, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.topLeftSizeX  , p_top, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,

				p_left + p_corners.bottomLeftSizeX  , p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.bottomLeftSizeX  , p_bottom, 0.f, 1.f , 1.f, 0.f, 0.f, 0.f,
				p_left + p_corners.bottomLeftSizeX  , p_bottom, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f,

				p_left, p_top + p_corners.topLeftSizeY      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_left, p_top + p_corners.topLeftSizeY      , 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottomLeftSizeY, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottomLeftSizeY, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_left, p_bottom - p_corners.bottomLeftSizeY, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_left, p_top + p_corners.topLeftSizeY      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,

				p_right, p_top + p_corners.topRightSizeY      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_right, p_top + p_corners.topRightSizeY      , 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottomRightSizeY, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottomRightSizeY, 1.f , 0.f, 1.f, 0.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottomRightSizeY, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
				p_right, p_top + p_corners.topRightSizeY      , -1.f, 0.f, 0.f, 0.f, 0.f, 0.f
			}
		);

		if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			AvoGUI::Point<float> normal(-p_corners.topLeftSizeY, -p_corners.topLeftSizeX);
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                         , p_top + p_corners.topLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_left                         , p_top + p_corners.topLeftSizeY, -1.f         , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.topLeftSizeX, p_top                         , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.topLeftSizeX, p_top                         , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.topLeftSizeX, p_top                         , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_left                         , p_top + p_corners.topLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.topLeftSizeX * p_corners.topLeftSizeY);
			float lastX = p_corners.topLeftSizeX;
			float lastY = 0.f;
			AvoGUI::Point<float> lastNormal(-lastX, -lastY);
			lastNormal.normalizeFast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topLeftSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topLeftSizeY*s_unitCirclePoints[a].y;
				AvoGUI::Point<float> normal(-x, -y);
				normal.normalizeFast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - x    , p_top + p_corners.topLeftSizeY - y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - x    , p_top + p_corners.topLeftSizeY - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - x    , p_top + p_corners.topLeftSizeY - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
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
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x  , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX        , p_top                                 , 0.f           , -1.f         , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX        , p_top                                 , 0.f           , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX        , p_top                                 , 0.f           , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, -lastNormal.x , -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x  , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			AvoGUI::Point<float> normal(p_corners.topRightSizeY, -p_corners.topRightSizeX);
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                          , p_top + p_corners.topRightSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_right                          , p_top + p_corners.topRightSizeY, 1.f          , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.topRightSizeX, p_top                          , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.topRightSizeX, p_top                          , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.topRightSizeX, p_top                          , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_right                          , p_top + p_corners.topRightSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.topRightSizeX * p_corners.topRightSizeY);
			float lastX = p_corners.topRightSizeX;
			float lastY = 0.f;
			AvoGUI::Point<float> lastNormal(lastX, -lastY);
			lastNormal.normalizeFast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topRightSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topRightSizeY*s_unitCirclePoints[a].y;
				AvoGUI::Point<float> normal(x, -y);
				normal.normalizeFast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + x    , p_top + p_corners.topLeftSizeY - y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + x    , p_top + p_corners.topLeftSizeY - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + x    , p_top + p_corners.topLeftSizeY - y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
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
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX        , p_top                                 , 0.f          , -1.f         , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX        , p_top                                 , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX        , p_top                                 , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topLeftSizeY - lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			AvoGUI::Point<float> normal(-p_corners.bottomLeftSizeY, p_corners.bottomLeftSizeX);
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                            , p_bottom - p_corners.bottomLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_left                            , p_bottom - p_corners.bottomLeftSizeY, -1.f         , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottomLeftSizeX, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottomLeftSizeX, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_left + p_corners.bottomLeftSizeX, p_bottom                            , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_left                            , p_bottom - p_corners.bottomLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottomLeftSizeX * p_corners.bottomLeftSizeY);
			float lastX = p_corners.bottomLeftSizeX;
			float lastY = 0.f;
			AvoGUI::Point<float> lastNormal(-lastX, lastY);
			lastNormal.normalizeFast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topLeftSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topLeftSizeY*s_unitCirclePoints[a].y;
				AvoGUI::Point<float> normal(-x, y);
				normal.normalizeFast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - x    , p_bottom - p_corners.bottomLeftSizeY + y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - x    , p_bottom - p_corners.bottomLeftSizeY + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - x    , p_bottom - p_corners.bottomLeftSizeY + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
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
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX        , p_bottom                                    , 0.f          , 1.f          , 0.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX        , p_bottom                                    , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX        , p_bottom                                    , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			AvoGUI::Point<float> normal(p_corners.bottomLeftSizeY, p_corners.bottomLeftSizeX);
			normal.normalize();
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                             , p_bottom - p_corners.bottomLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f,
					p_right                             , p_bottom - p_corners.bottomLeftSizeY, 1.f          , 0.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottomRightSizeX, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottomRightSizeX, p_bottom                            , 0.f          , 1.f          , 1.f, 0.f, 0.f, 0.f,
					p_right - p_corners.bottomRightSizeX, p_bottom                            , -normal.x*2.f, -normal.y    , 0.f, 0.f, 0.f, 0.f,
					p_right                             , p_bottom - p_corners.bottomLeftSizeY, -normal.x    , -normal.y*2.f, 0.f, 0.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottomRightSizeX * p_corners.bottomRightSizeY);
			float lastX = p_corners.bottomRightSizeX;
			float lastY = 0.f;
			AvoGUI::Point<float> lastNormal(lastX, lastY);
			lastNormal.normalizeFast();
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topRightSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topRightSizeY*s_unitCirclePoints[a].y;
				AvoGUI::Point<float> normal(x, y);
				normal.normalizeFast();
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + x    , p_bottom - p_corners.bottomRightSizeY + y    , normal.x     , normal.y     , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + x    , p_bottom - p_corners.bottomRightSizeY + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + x    , p_bottom - p_corners.bottomRightSizeY + y    , -normal.x    , -normal.y    , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
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
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX        , p_bottom                                     , 0.f          , 1.f          , 0.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX        , p_bottom                                     , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX        , p_bottom                                     , 0.f          , -1.f         , 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, -lastNormal.x, -lastNormal.y, 1.f, 0.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, lastNormal.x , lastNormal.y , 0.f, 0.f, 0.f, 0.f
					}
				);
			}
		}
	}
	/*
		The primitive type is GL_TRIANGLES
	*/
	static void createFilledCornerRectangleGeometry(std::vector<float>& p_buffer, float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners p_corners)
	{
		p_left -= 0.5f;
		p_top -= 0.5f;
		p_right += 0.5f;
		p_bottom += 0.5f;
		if (p_right - p_left < p_corners.topLeftSizeX + p_corners.topRightSizeY)
		{
			float factor = (p_right - p_left)/(p_corners.topLeftSizeX + p_corners.topRightSizeX);
			p_corners.topLeftSizeX *= factor;
			p_corners.topRightSizeX *= factor;
		}
		if (p_right - p_left < p_corners.bottomLeftSizeX + p_corners.bottomRightSizeY)
		{
			float factor = (p_right - p_left)/(p_corners.bottomLeftSizeX + p_corners.bottomRightSizeX);
			p_corners.bottomLeftSizeX *= factor;
			p_corners.bottomRightSizeX *= factor;
		}
		if (p_bottom - p_top < p_corners.topLeftSizeX + p_corners.bottomLeftSizeY)
		{
			float factor = (p_bottom - p_top)/(p_corners.topLeftSizeX + p_corners.bottomLeftSizeY);
			p_corners.topLeftSizeX *= factor;
			p_corners.bottomLeftSizeX *= factor;
		}
		if (p_bottom - p_top < p_corners.topRightSizeX + p_corners.bottomRightSizeY)
		{
			float factor = (p_bottom - p_top)/(p_corners.topRightSizeX + p_corners.bottomRightSizeY);
			p_corners.topRightSizeX *= factor;
			p_corners.bottomRightSizeX *= factor;
		}

		// Create the "body"
		float width = p_right - p_left;
		p_buffer.insert(
			p_buffer.end(),
			{
				// Horizontal center bar
				p_left , p_top + p_corners.topLeftSizeY       , 1.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_top + p_corners.topRightSizeY      , 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottomRightSizeY, 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_right, p_bottom - p_corners.bottomRightSizeY, 1.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_left , p_bottom - p_corners.bottomLeftSizeY , 0.f, width, 0.5f, 2.f, 0.f, 0.f,
				p_left , p_top + p_corners.topLeftSizeY       , 0.f, width, 0.5f, 2.f, 0.f, 0.f,

				// Top center bar
				p_left + p_corners.topLeftSizeX  , p_top                          , 0.f , p_corners.topLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top                          , 0.f , p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top + p_corners.topRightSizeY, 0.5f, p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.topRightSizeX, p_top + p_corners.topRightSizeY, 0.5f, p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.topLeftSizeX  , p_top + p_corners.topLeftSizeY , 0.5f, p_corners.topLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.topLeftSizeX  , p_top                          , 0.f , p_corners.topLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f,

				// Bottom center bar
				p_left + p_corners.bottomLeftSizeX  , p_bottom                             , 0.f , p_corners.bottomLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom                             , 0.f , p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom - p_corners.bottomRightSizeY, 0.5f, p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_right - p_corners.bottomRightSizeX, p_bottom - p_corners.bottomRightSizeY, 0.5f, p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.bottomLeftSizeX  , p_bottom - p_corners.bottomLeftSizeY , 0.5f, p_corners.bottomLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f,
				p_left + p_corners.bottomLeftSizeX  , p_bottom                             , 0.f , p_corners.bottomLeftSizeY*2.f , 0.5f, 2.f, 0.f, 0.f
			}
		);

		// Create the corners
		if (p_corners.topLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.topLeftSizeX*p_corners.topLeftSizeY/(float)AvoGUI::Point<float>::getLengthFast(p_corners.topLeftSizeX, p_corners.topLeftSizeY);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                         , p_top + p_corners.topLeftSizeY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.topLeftSizeX, p_top                         , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.topLeftSizeX, p_top + p_corners.topLeftSizeY, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.topLeftSizeX * p_corners.topLeftSizeY);
			float lastX = p_corners.topLeftSizeX;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topLeftSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topLeftSizeY*s_unitCirclePoints[a].y;
				// Length of vector (0.5*(x1 + x2), 0.5*(y1 + y2)), doubled because the anti-aliasing attribute of the inner vertex is 0.5 (we don't want anti-aliasing for that vertex)
				auto distance = (float)AvoGUI::Point<float>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.topLeftSizeX        , p_top + p_corners.topLeftSizeY        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - x    , p_top + p_corners.topLeftSizeY - y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
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
						p_left + p_corners.topLeftSizeX        , p_top + p_corners.topLeftSizeY        , 0.5f, p_corners.topLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX - lastX, p_top + p_corners.topLeftSizeY - lastY, 0.f , p_corners.topLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.topLeftSizeX        , p_top                                 , 0.f , p_corners.topLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}
		if (p_corners.topRightType == AvoGUI::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.topRightSizeX*p_corners.topRightSizeY/(float)AvoGUI::Point<float>::getLengthFast(p_corners.topRightSizeX, p_corners.topRightSizeY);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                          , p_top + p_corners.topRightSizeY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.topRightSizeX, p_top                          , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.topRightSizeX, p_top + p_corners.topRightSizeY, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.topRightSizeX * p_corners.topRightSizeY);
			float lastX = p_corners.topRightSizeX;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.topRightSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.topRightSizeY*s_unitCirclePoints[a].y;
				auto distance = (float)AvoGUI::Point<float>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.topRightSizeX        , p_top + p_corners.topRightSizeY        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topRightSizeY - lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + x    , p_top + p_corners.topRightSizeY - y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
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
						p_right - p_corners.topRightSizeX        , p_top + p_corners.topRightSizeY        , 0.5f, p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX + lastX, p_top + p_corners.topRightSizeY - lastY, 0.f , p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.topRightSizeX        , p_top                                  , 0.f , p_corners.topRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}

		if (p_corners.bottomLeftType == AvoGUI::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.bottomLeftSizeX*p_corners.bottomLeftSizeY/(float)AvoGUI::Point<float>::getLengthFast(p_corners.bottomLeftSizeX, p_corners.bottomLeftSizeY);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_left                            , p_bottom - p_corners.bottomLeftSizeY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.bottomLeftSizeX, p_bottom                            , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_left + p_corners.bottomLeftSizeX, p_bottom - p_corners.bottomLeftSizeY, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottomLeftSizeX * p_corners.bottomLeftSizeY);
			float lastX = p_corners.bottomLeftSizeX;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.bottomLeftSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.bottomLeftSizeY*s_unitCirclePoints[a].y;
				auto distance = (float)AvoGUI::Point<float>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_left + p_corners.bottomLeftSizeX        , p_bottom - p_corners.bottomLeftSizeY        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - x    , p_bottom - p_corners.bottomLeftSizeY + y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
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
						p_left + p_corners.bottomLeftSizeX        , p_bottom - p_corners.bottomLeftSizeY        , 0.5f, p_corners.bottomLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX - lastX, p_bottom - p_corners.bottomLeftSizeY + lastY, 0.f , p_corners.bottomLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_left + p_corners.bottomLeftSizeX        , p_bottom                                    , 0.f , p_corners.bottomLeftSizeY*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}

		if (p_corners.bottomRightType == AvoGUI::RectangleCornerType::Cut)
		{
			float distance = 2.f*p_corners.bottomRightSizeX*p_corners.bottomRightSizeY/(float)AvoGUI::Point<float>::getLengthFast(p_corners.bottomRightSizeX, p_corners.bottomRightSizeY);
			p_buffer.insert(
				p_buffer.end(),
				{
					p_right                             , p_bottom - p_corners.bottomRightSizeY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.bottomRightSizeX, p_bottom                             , 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
					p_right - p_corners.bottomRightSizeX, p_bottom - p_corners.bottomRightSizeY, 0.5f, distance, 0.5f, 2.f, 0.f, 0.f
				}
			);
		}
		else
		{
			uint32 stride = getCirclePointStrideForRadiusSquared(p_corners.bottomRightSizeX * p_corners.bottomRightSizeY);
			float lastX = p_corners.bottomRightSizeX;
			float lastY = 0.f;
			for (uint32 a = 1; a <= CIRCLE_RESOLUTION/4; a += stride)
			{
				float x = p_corners.bottomRightSizeX*s_unitCirclePoints[a].x;
				float y = p_corners.bottomRightSizeY*s_unitCirclePoints[a].y;
				auto distance = (float)AvoGUI::Point<float>::getLengthFast(lastX + x, lastY + y);
				p_buffer.insert(
					p_buffer.end(),
					{
						p_right - p_corners.bottomRightSizeX        , p_bottom - p_corners.bottomRightSizeY        , 0.5f, distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, 0.f , distance, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + x    , p_bottom - p_corners.bottomRightSizeY + y    , 0.f , distance, 0.5f, 2.f, 0.f, 0.f
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
						p_right - p_corners.bottomRightSizeX        , p_bottom - p_corners.bottomRightSizeY        , 0.5f, p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX + lastX, p_bottom - p_corners.bottomRightSizeY + lastY, 0.f , p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f,
						p_right - p_corners.bottomRightSizeX        , p_bottom                                     , 0.f , p_corners.bottomRightSizeY*2.f, 0.5f, 2.f, 0.f, 0.f
					}
				);
			}
		}
	}

public:
	explicit OpenGlDrawingContext(AvoGUI::Window* p_window) :
		m_window((LinuxWindow*)p_window), m_windowHandle((XWindow)p_window->getNativeHandle())
	{
		loadOpenGlFunctions();

		initializeStaticVariables();

		//------------------------------
		// Create the OpenGL context.
		// Need to find the framebuffer configuration that was used to create the window, because our context should match it.

		m_server = XOpenDisplay(nullptr);

		auto* linuxWindow = (LinuxWindow*)p_window;

		m_dipToPixelFactor = linuxWindow->getDipToPixelFactor();

		XWindowAttributes windowAttributes; // Used to match the VisualID
		XGetWindowAttributes(m_server, m_windowHandle, &windowAttributes);

		int numberOfConfigurations = 0;
		GLXFBConfig* framebufferConfigurations = glXChooseFBConfig(m_server, DefaultScreen(m_server), nullptr, &numberOfConfigurations);
		GLXFBConfig framebufferConfiguration = *framebufferConfigurations;
		for (uint32 a = 0; a < numberOfConfigurations; a++)
		{
			XVisualInfo* visualInfo = glXGetVisualFromFBConfig(m_server, framebufferConfigurations[a]);
			if (!visualInfo)
			{
				continue;
			}
			if (visualInfo->visualid == windowAttributes.visual->visualid)
			{
				framebufferConfiguration = framebufferConfigurations[a];
				XFree(visualInfo);
				break;
			}
			XFree(visualInfo);
		}

		int contextAttributes[] =
		{
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
		m_context = glXCreateContextAttribsARB(m_server, framebufferConfiguration, nullptr, true, contextAttributes);

		XFree(framebufferConfigurations);

		//------------------------------

		glXMakeCurrent(m_server, m_windowHandle, m_context);
		glViewport(0, 0, linuxWindow->getWidth()*m_dipToPixelFactor, linuxWindow->getHeight()*m_dipToPixelFactor);
		m_size = linuxWindow->getSize();

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
		m_renderShader.compile(vertexShaderSource, fragmentShaderSource);
		m_renderShader.use();

		// Identity matrix
		m_transformMatrix[0] = 1.f;
		m_transformMatrix[4] = 1.f;
		m_transformMatrix[8] = 1.f;

		m_renderShader.setUniformMatrix3x3("u_transform", m_transformMatrix);

		//------------------------------

		m_drawCalls.reserve(256);
		m_vertexBuffer.reserve(8192);
	}
	~OpenGlDrawingContext() final
	{
		glXDestroyContext(m_server, m_context);
		XCloseDisplay(m_server);
	}

	//------------------------------

	void beginDrawing() override
	{
		glXMakeCurrent(m_server, m_windowHandle, m_context);
		m_drawCalls.clear();
		m_vertexBuffer.clear();

		glClear(GL_STENCIL_BUFFER_BIT);
	}
	void finishDrawing(std::vector<AvoGUI::Rectangle<float>> const& p_updatedRectangles) override
	{
		// GL_STATIC_DRAW hints that the buffer is modified once and used many times
		m_renderShader.setVertexData(m_vertexBuffer.data(), m_vertexBuffer.size()*sizeof(float), VERTEX_DATA_LENGTH*sizeof(float), GL_STATIC_DRAW);
		m_renderShader.configureVertexShaderInput(0, 2, 0);
		m_renderShader.configureVertexShaderInput(1, 4, 2);
		m_renderShader.configureVertexShaderInput(2, 2, 6);

		for (auto& drawCall : m_drawCalls)
		{
			m_renderShader.setUniformColor("u_color", drawCall.color);
			m_renderShader.setUniformMatrix3x3("u_transform", drawCall.transform);
			if (drawCall.renderMode == 0)
			{
				m_renderShader.setUniformValue("u_lineWidth", drawCall.lineWidth);
			}
			else if (drawCall.renderMode == 2)
			{
				glBindTexture(GL_TEXTURE_2D, drawCall.texture);
			}
			m_renderShader.setUniformValue("u_renderMode", drawCall.renderMode);
			m_renderShader.draw(drawCall.vertexBufferStartIndex, drawCall.numberOfVertices, drawCall.primitiveType);
		}

		//------------------------------

		glXSwapBuffers(m_server, m_windowHandle);
	}

	//------------------------------

	AvoGUI::DrawingState* createDrawingState() override
	{
		return nullptr;
	}

	void saveDrawingState(AvoGUI::DrawingState* p_drawingState) override
	{
	}

	void restoreDrawingState(AvoGUI::DrawingState* p_drawingState) override
	{
	}

	//------------------------------

	void setIsFullscreen(bool p_isFullscreen) override
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

	void setBackgroundColor(AvoGUI::Color const& p_color) override
	{
		// If we try to set the background color here directly, some methods block until XNextEvent in the event thread returns...
		// So send the color in a client event instead and let the event thread set the background color of the window.

		//Display* server = m_window->getServer();
		//XLockDisplay(server);
		//
		//XEvent event;
		//event.type = ClientMessage;
		//event.xclient.message_type = m_window->getBackgroundColorMessageType();
		//event.xclient.format = 32; // Using data.l, 32 bits per data item
		//event.xclient.data.l[0] = long(p_color.red * 65535);
		//event.xclient.data.l[1] = long(p_color.green * 65535);
		//event.xclient.data.l[2] = long(p_color.blue * 65535);
		//event.xclient.display = server;
		//event.xclient.window = m_windowHandle;
		//event.xclient.send_event = true;
		//XSendEvent(server, m_windowHandle, false, 0, &event);
		//XFlush(server);
		//
		//XUnlockDisplay(server);

		//XColor xColor;
		//xColor.red = (unsigned short)(p_color.red * 65535);
		//xColor.green = (unsigned short)(p_color.green * 65535);
		//xColor.blue = (unsigned short)(p_color.blue * 65535);
		//xColor.flags = DoRed | DoGreen | DoBlue;
		//XAllocColor(m_server, m_window->getColormap(), &xColor);
		//XSetWindowBackground(m_server, m_windowHandle, xColor.pixel);

		m_backgroundColor = p_color;
	}
	AvoGUI::Color getBackgroundColor() override
	{
		return m_backgroundColor;
	}

	//------------------------------

	float getDpi() override
	{
		return m_dipToPixelFactor*NORMAL_DPI;
	}
	void setDpi(float p_dpi) override
	{
		m_dipToPixelFactor = p_dpi/NORMAL_DPI;
	}

	//------------------------------

	void moveOrigin(AvoGUI::Point<float> const& p_offset) override
	{
		moveOrigin(p_offset.x, p_offset.y);
	}
	void moveOrigin(float p_offsetX, float p_offsetY) override
	{
		m_transformMatrix[7] -= p_offsetX;
		m_transformMatrix[8] -= p_offsetY;
	}
	void setOrigin(AvoGUI::Point<float> const& p_origin) override
	{
		setOrigin(p_origin.x, p_origin.y);
	}
	void setOrigin(float p_x, float p_y) override
	{
		m_transformMatrix[7] = -p_x;
		m_transformMatrix[8] = -p_y;
	}
	AvoGUI::Point<float> getOrigin() override
	{
		return AvoGUI::Point<float>(-m_transformMatrix[7], -m_transformMatrix[8]);
	}

	//------------------------------

	void scale(float p_scale) override
	{
		scale(p_scale, p_scale);
	}
	void scale(float p_scaleX, float p_scaleY) override
	{
		m_scale.x *= p_scaleX;
		m_scale.y *= p_scaleY;
		m_transformMatrix[0] *= p_scaleX;
		m_transformMatrix[1] *= p_scaleY;
		m_transformMatrix[3] *= p_scaleX;
		m_transformMatrix[4] *= p_scaleY;
	}
	void scale(float p_scale, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scale, p_scale, p_origin.x, p_origin.y);
	}
	void scale(float p_scaleX, float p_scaleY, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scaleX, p_scaleY, p_origin.x, p_origin.y);
	}
	void scale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale, p_scale, p_originX, p_originY);
	}
	void scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		m_scale.x *= p_scaleX;
		m_scale.y *= p_scaleY;
		m_transformMatrix[0] *= p_scaleX;
		m_transformMatrix[1] *= p_scaleX;
		m_transformMatrix[3] *= p_scaleY;
		m_transformMatrix[4] *= p_scaleY;
		m_transformMatrix[6] += (p_originX - m_transformMatrix[6])*(1.f - p_scaleX);
		m_transformMatrix[7] += (p_originY - m_transformMatrix[7])*(1.f - p_scaleY);
	}
	void setScale(float p_scale) override
	{
		scale(p_scale/m_scale.x, p_scale/m_scale.y);
	}
	void setScale(float p_scaleX, float p_scaleY) override
	{
		scale(p_scaleX/m_scale.x, p_scaleY/m_scale.y);
	}
	void setScale(float p_scale, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scale/m_scale.x, p_scale/m_scale.y, p_origin.x, p_origin.y);
	}
	void setScale(float p_scaleX, float p_scaleY, AvoGUI::Point<float> const& p_origin) override
	{
		scale(p_scaleX/m_scale.x, p_scaleY/m_scale.y, p_origin.x, p_origin.y);
	}
	void setScale(float p_scale, float p_originX, float p_originY) override
	{
		scale(p_scale/m_scale.x, p_scale/m_scale.y, p_originX, p_originY);
	}
	void setScale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) override
	{
		scale(p_scaleX/m_scale.x, p_scaleY/m_scale.y, p_originX, p_originY);
	}
	AvoGUI::Point<float> const& getScale() override
	{
		return m_scale;
	}
	float getScaleX() override
	{
		return m_scale.x;
	}
	float getScaleY() override
	{
		return m_scale.y;
	}

	//------------------------------

	void rotate(float p_radians) override
	{
		float cos = std::cos(p_radians);
		float sin = std::sin(p_radians);

		float previousMatrix[9];
		memcpy(previousMatrix, m_transformMatrix, sizeof(previousMatrix));

		/*
			Matrix multiplication:
			[cos -sin  0]   [m[0] m[3] m[6]]   [m[0]cos - m[1]sin  m[3]cos - m[4]sin  m[6]cos - m[7]sin]
			[sin  cos  0] * [m[1] m[4] m[7]] = [m[0]sin + m[1]cos  m[3]sin + m[4]cos  m[6]sin + m[7]cos]
			[0    0    1]   [0    0    1   ]   [0                  0                  1                ]
		*/
		m_transformMatrix[0] = cos*previousMatrix[0] - sin*previousMatrix[1];
		m_transformMatrix[1] = sin*previousMatrix[0] + cos*previousMatrix[1];
		m_transformMatrix[3] = cos*previousMatrix[3] - sin*previousMatrix[4];
		m_transformMatrix[4] = sin*previousMatrix[3] + cos*previousMatrix[4];
		m_transformMatrix[6] = cos*previousMatrix[6] - sin*previousMatrix[7];
		m_transformMatrix[7] = sin*previousMatrix[6] + cos*previousMatrix[7];
	}
	void rotate(float p_radians, AvoGUI::Point<float> const& p_origin) override
	{
		rotate(p_radians, p_origin.x, p_origin.y);
	}
	void rotate(float p_radians, float p_originX, float p_originY) override
	{
		float cos = std::cos(p_radians);
		float sin = std::sin(p_radians);

		float previousMatrix[9];
		memcpy(previousMatrix, m_transformMatrix, sizeof(previousMatrix));
		previousMatrix[6] -= p_originX;
		previousMatrix[7] -= p_originY;

		/*
			Matrix multiplication:
			[cos -sin  0]   [m[0] m[3] m[6]]   [m[0]cos - m[1]sin  m[3]cos - m[4]sin  m[6]cos - m[7]sin]
			[sin  cos  0] * [m[1] m[4] m[7]] = [m[0]sin + m[1]cos  m[3]sin + m[4]cos  m[6]sin + m[7]cos]
			[0    0    1]   [0    0    1   ]   [0                  0                  1                ]
		*/
		m_transformMatrix[0] = cos*previousMatrix[0] - sin*previousMatrix[1];
		m_transformMatrix[1] = sin*previousMatrix[0] + cos*previousMatrix[1];
		m_transformMatrix[3] = cos*previousMatrix[3] - sin*previousMatrix[4];
		m_transformMatrix[4] = sin*previousMatrix[3] + cos*previousMatrix[4];
		m_transformMatrix[6] = cos*previousMatrix[6] - sin*previousMatrix[7] + p_originX;
		m_transformMatrix[7] = sin*previousMatrix[6] + cos*previousMatrix[7] + p_originY;
	}

	//------------------------------

	void resetTransformations() override
	{
		m_transformMatrix[0] = 1.f;
		m_transformMatrix[1] = 0.f;
		m_transformMatrix[2] = 0.f;
		m_transformMatrix[3] = 0.f;
		m_transformMatrix[4] = 1.f;
		m_transformMatrix[5] = 0.f;
		m_transformMatrix[6] = 0.f;
		m_transformMatrix[7] = 0.f;
		m_transformMatrix[8] = 1.f;
	}

	//------------------------------

	void setSize(AvoGUI::Point<float> const& p_size) override
	{
		setSize(p_size.x, p_size.y);
	}
	void setSize(float p_width, float p_height) override
	{
		glXMakeCurrent(m_server, m_windowHandle, m_context);
		glViewport(0, 0, p_width*m_dipToPixelFactor, p_height*m_dipToPixelFactor);
		m_size.set(p_width, p_height);

		// Column-major order matrix
		float transform[9] =
		{
			2.f/m_size.x, 0.f          , 0.f,
			0.f         , -2.f/m_size.y, 0.f,
			-1.f        , 1.f          , 1.f
		};
		m_renderShader.setUniformMatrix3x3("u_viewTransform", transform);
	}
	AvoGUI::Point<float> getSize() override
	{
		return m_size;
	}

	//------------------------------

	void clear(AvoGUI::Color const& p_color) override
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

	void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) override
	{
		// Anti-aliasing requires an extra row of pixels
		p_left -= 0.5f;
		p_top -= 0.5f;
		p_right += 0.5f;
		p_bottom += 0.5f;

		float height = p_bottom - p_top;
		float width = p_right - p_left;

		m_vertexBuffer.insert(
			m_vertexBuffer.end(),
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
	void fillRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size) override
	{
		fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
	}
	void fillRectangle(AvoGUI::Rectangle<float> const& p_rectangle) override
	{
		fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
	}
	void fillRectangle(float p_width, float p_height) override
	{
		fillRectangle(0.f, 0.f, p_width, p_height);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_size) override
	{
		fillRectangle(0.f, 0.f, p_size.x, p_size.y);
	}

	void fillRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_rectangleCorners) override
	{
		fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_rectangleCorners);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_rectangleCorners) override
	{
		fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_rectangleCorners);
	}
	void fillRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_rectangleCorners) override
	{
		fillRectangle(0.f, 0.f, p_size.x, p_size.y, p_rectangleCorners);
	}
	void fillRectangle(float p_width, float p_height, AvoGUI::RectangleCorners const& p_rectangleCorners) override
	{
		fillRectangle(0.f, 0.f, p_width, p_height, p_rectangleCorners);
	}
	void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_rectangleCorners) override
	{
		size_t sizeBefore = m_vertexBuffer.size();
		createFilledCornerRectangleGeometry(m_vertexBuffer, p_left, p_top, p_right, p_bottom, p_rectangleCorners);
		addDrawCall((m_vertexBuffer.size() - sizeBefore)/VERTEX_DATA_LENGTH);
	}

	void fillRoundedRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius) override
	{
		fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, AvoGUI::RectangleCorners(p_radius));
	}
	void fillRoundedRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius) override
	{
		fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, AvoGUI::RectangleCorners(p_radius));
	}
	void fillRoundedRectangle(AvoGUI::Point<float> const& p_size, float p_radius) override
	{
		fillRectangle(0.f, 0.f, p_size.x, p_size.y, AvoGUI::RectangleCorners(p_radius));
	}
	void fillRoundedRectangle(float p_width, float p_height, float p_radius) override
	{
		fillRectangle(0.f, 0.f, p_width, p_height, AvoGUI::RectangleCorners(p_radius));
	}
	void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) override
	{
		fillRectangle(p_left, p_top, p_right, p_bottom, AvoGUI::RectangleCorners(p_radius));
	}

	//------------------------------

	void strokeRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_strokeWidth) override
	{
		strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_strokeWidth);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_strokeWidth) override
	{
		strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_strokeWidth);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_size, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_size.x, p_size.y, p_strokeWidth);
	}
	void strokeRectangle(float p_width, float p_height, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_width, p_height, p_strokeWidth);
	}
	void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth) override
	{
		// Anti-aliasing requires an extra row of pixels
		p_strokeWidth += 1.f;
		p_left -= p_strokeWidth*0.5f;
		p_top -= p_strokeWidth*0.5f;
		p_right += p_strokeWidth*0.5f;
		p_bottom += p_strokeWidth*0.5f;

		float height = p_bottom - p_top;
		float width = p_right - p_left;
		m_vertexBuffer.insert(
			m_vertexBuffer.end(),
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

	void strokeRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_rectangleCorners, p_strokeWidth);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_rectangleCorners, p_strokeWidth);
	}
	void strokeRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_size.x, p_size.y, p_rectangleCorners, p_strokeWidth);
	}
	void strokeRectangle(float p_width, float p_height, AvoGUI::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_width, p_height, p_rectangleCorners, p_strokeWidth);
	}
	void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_rectangleCorners, float p_strokeWidth) override
	{
		p_strokeWidth += 1.f;
		size_t sizeBefore = m_vertexBuffer.size();
		createStrokedCornerRectangleGeometry(m_vertexBuffer, p_left, p_top, p_right, p_bottom, p_rectangleCorners);
		addDrawCall((m_vertexBuffer.size() - sizeBefore)/VERTEX_DATA_LENGTH, p_strokeWidth);
	}

	void strokeRoundedRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth) override
	{
		strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, AvoGUI::RectangleCorners(p_radius), p_strokeWidth);
	}
	void strokeRoundedRectangle(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius, float p_strokeWidth) override
	{
		strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, AvoGUI::RectangleCorners(p_radius), p_strokeWidth);
	}
	void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth) override
	{
		strokeRectangle(p_left, p_top, p_right, p_bottom, AvoGUI::RectangleCorners(p_radius), p_strokeWidth);
	}

	void strokeRoundedRectangle(AvoGUI::Point<float> const& p_size, float p_radius, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_size.x, p_size.y, AvoGUI::RectangleCorners(p_radius), p_strokeWidth);
	}
	void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth) override
	{
		strokeRectangle(0.f, 0.f, p_width, p_height, AvoGUI::RectangleCorners(p_radius), p_strokeWidth);
	}

	//------------------------------

	void fillCircle(AvoGUI::Point<float> const& p_position, float p_radius) override
	{
		fillCircle(p_position.x, p_position.y, p_radius);
	}
	void fillCircle(float p_x, float p_y, float p_radius) override
	{
		p_radius += 0.5f;
		m_vertexBuffer.insert(m_vertexBuffer.end(), { p_x, p_y, 0.5f, p_radius*2.f, 0.5f, 2.f, 0.f, 0.f }); // Only want anti-aliasing on the outer edges of the triangles.
		for (uint32 a = 0; a <= CIRCLE_RESOLUTION; a++)
		{
			m_vertexBuffer.insert(m_vertexBuffer.end(), { s_unitCirclePoints[a].x*p_radius + p_x, s_unitCirclePoints[a].y*p_radius + p_y, 0.f, p_radius*2.f, 0.5f, 2.f, 0.f, 0.f, });
		}
		addDrawCall(CIRCLE_RESOLUTION + 2, 0u, (GLenum)GL_TRIANGLE_FAN);
	}

	void strokeCircle(AvoGUI::Point<float> const& p_position, float p_radius, float p_strokeWidth) override
	{
		strokeCircle(p_position.x, p_position.y, p_radius, p_strokeWidth);
	}
	void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth) override
	{
		p_strokeWidth += 1.f; // Anti-aliasing needs extra pixels
		p_radius += p_strokeWidth*0.5f;

		float innerAntiAliasingStart = 1.f - p_radius/p_strokeWidth;
		m_vertexBuffer.insert(m_vertexBuffer.end(), { p_x, p_y, innerAntiAliasingStart, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f });
		for (uint32 a = 0; a <= CIRCLE_RESOLUTION; a++)
		{
			m_vertexBuffer.insert(m_vertexBuffer.end(), { s_unitCirclePoints[a].x*p_radius + p_x, s_unitCirclePoints[a].y*p_radius + p_y, 1.f, p_strokeWidth, 0.5f, 2.f, 0.f, 0.f });
		}
		addDrawCall(CIRCLE_RESOLUTION + 2, 0u, (GLenum)GL_TRIANGLE_FAN);
	}

	//------------------------------

	void drawLine(AvoGUI::Point<float> const& p_point_0, AvoGUI::Point<float> const& p_point_1, float p_thickness) override
	{
		drawLine(p_point_0.x, p_point_0.y, p_point_1.x, p_point_1.y);
	}
	void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) override
	{
		float distance = AvoGUI::fastSqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
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

		m_vertexBuffer.insert(
			m_vertexBuffer.end(),
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

	void strokeShape(std::vector<AvoGUI::Point<float>> const& p_vertices, float p_lineThickness, bool p_isClosed) override
	{
	}
	void strokeShape(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed) override
	{
	}
	void fillShape(std::vector<AvoGUI::Point<float>> const& p_vertices) override
	{
	}
	void fillShape(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices) override
	{
	}

	//------------------------------

	void strokeGeometry(AvoGUI::Geometry* p_geometry, float p_strokeWidth) override
	{
		auto geometry = (OpenGlGeometry*)p_geometry;
		m_vertexBuffer.insert(m_vertexBuffer.end(), geometry->vertexBuffer.begin(), geometry->vertexBuffer.end());
		addDrawCall(geometry->vertexBuffer.size());
	}
	void fillGeometry(AvoGUI::Geometry* p_geometry) override
	{
		auto geometry = (OpenGlGeometry*)p_geometry;
		m_vertexBuffer.insert(m_vertexBuffer.end(), geometry->vertexBuffer.begin(), geometry->vertexBuffer.end());
		addDrawCall(geometry->vertexBuffer.size());
	}

	//------------------------------

	AvoGUI::Geometry* createRoundedRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius, bool p_isStroked) override
	{
		createCornerRectangleGeometry(p_left, p_top, p_right, p_bottom, AvoGUI::RectangleCorners(p_radius), p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, float p_radius, bool p_isStroked) override
	{
		createCornerRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, AvoGUI::RectangleCorners(p_radius), p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, bool p_isStroked) override
	{
		createCornerRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, AvoGUI::RectangleCorners(p_radius), p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(float p_width, float p_height, float p_radius, bool p_isStroked) override
	{
		createCornerRectangleGeometry(0.f, 0.f, p_width, p_height, AvoGUI::RectangleCorners(p_radius), p_isStroked);
	}
	AvoGUI::Geometry* createRoundedRectangleGeometry(AvoGUI::Point<float> const& p_size, float p_radius, bool p_isStroked) override
	{
		createCornerRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, AvoGUI::RectangleCorners(p_radius), p_isStroked);
	}

	AvoGUI::Geometry* createCornerRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		auto* geometry = new OpenGlGeometry();

		if (p_isStroked)
		{
			createStrokedCornerRectangleGeometry(geometry->vertexBuffer, p_left, p_top, p_right, p_bottom, p_corners);
		}
		else
		{
			createFilledCornerRectangleGeometry(geometry->vertexBuffer, p_left, p_top, p_right, p_bottom, p_corners);
		}
		geometry->isStroked = p_isStroked;

		return (AvoGUI::Geometry*)geometry;
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(0.f, 0.f, p_width, p_height, p_corners, p_isStroked);
	}
	AvoGUI::Geometry* createCornerRectangleGeometry(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, bool p_isStroked) override
	{
		return createCornerRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, p_corners, p_isStroked);
	}

	//------------------------------

	AvoGUI::Geometry* createPolygonGeometry(std::vector<AvoGUI::Point<float>> const& p_vertices, bool p_isStroked) override
	{
		return createPolygonGeometry(p_vertices.data(), p_vertices.size(), p_isStroked);
	}
	AvoGUI::Geometry* createPolygonGeometry(AvoGUI::Point<float> const* p_vertices, uint32 p_numberOfVertices, bool p_isStroked) override
	{
		auto* geometry = new OpenGlGeometry();
		return (AvoGUI::Geometry*)geometry;
	}

	//------------------------------

	void setLineCap(AvoGUI::LineCap p_lineCap) override
	{
	}
	void setStartLineCap(AvoGUI::LineCap p_lineCap) override
	{
	}
	void setEndLineCap(AvoGUI::LineCap p_lineCap) override
	{
	}
	AvoGUI::LineCap getStartLineCap() override
	{
	}
	AvoGUI::LineCap getEndLineCap() override
	{
	}

	//------------------------------

	void setLineDashStyle(AvoGUI::LineDashStyle p_dashStyle) override
	{
	}
	AvoGUI::LineDashStyle getLineDashStyle() override
	{
	}

	void setLineDashOffset(float p_dashOffset) override
	{
	}
	float getLineDashOffset() override
	{
	}

	void setLineDashCap(AvoGUI::LineCap p_dashCap) override
	{
	}
	AvoGUI::LineCap getLineDashCap() override
	{
	}

	//------------------------------

	void setLineJoin(AvoGUI::LineJoin p_lineJoin) override
	{
	}
	AvoGUI::LineJoin getLineJoin() override
	{
	}

	void setLineJoinMiterLimit(float p_miterLimit) override
	{
	}
	float getLineJoinMiterLimit() override
	{
	}

	//------------------------------

	void pushClipGeometry(AvoGUI::Geometry* p_geometry, float p_opacity) override
	{
	}

	//------------------------------

	void pushClipShape(std::vector<AvoGUI::Point<float>> const& p_points, float p_opacity) override
	{
	}
	void pushClipShape(AvoGUI::Point<float> const* p_points, uint32 p_numberOfPoints, float p_opacity) override
	{
	}

	void popClipShape() override
	{
		if (!m_clippingShapeStack.empty())
		{
			m_clippingShapeStack.pop();
			if (!m_clippingShapeStack.empty())
			{
				AvoGUI::Rectangle<float> bounds = m_clippingShapeStack.top().getBounds();
				glScissor(bounds.left, bounds.top, std::ceil(bounds.getWidth()), std::ceil(bounds.getHeight()));
			}
			else
			{
				glScissor(0, 0, m_size.x*m_dipToPixelFactor, m_size.y*m_dipToPixelFactor);
			}
		}
	}

	//------------------------------

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_opacity) override
	{
		if (!m_clippingShapeStack.empty())
		{
			AvoGUI::Rectangle<float> clippingBounds = m_clippingShapeStack.top().getBounds().bound(p_left, p_top, p_right, p_bottom);
			m_clippingShapeStack.push(ClippingShape(clippingBounds));
			glScissor(clippingBounds.left, clippingBounds.bottom, std::ceil(clippingBounds.getWidth()), std::ceil(clippingBounds.getHeight()));
		}
		else
		{
			m_clippingShapeStack.push(ClippingShape(p_left, p_bottom, p_right, p_bottom));
			glScissor(p_left, p_bottom, std::ceil(p_right - p_left), std::ceil(p_bottom - p_top));
		}
	}
	void pushClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_opacity) override
	{
		pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_opacity);
	}
	void pushClipRectangle(AvoGUI::Point<float> const& p_size, float p_opacity) override
	{
		pushClipRectangle(0, 0, p_size.x, p_size.y, p_opacity);
	}

	void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, AvoGUI::RectangleCorners const& p_corners, float p_opacity) override
	{
	}
	void pushClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, AvoGUI::RectangleCorners const& p_corners, float p_opacity) override
	{
	}
	void pushClipRectangle(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_opacity) override
	{
	}

	//------------------------------

	void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_opacity) override
	{
	}
	void pushRoundedClipRectangle(AvoGUI::Rectangle<float> const& p_rectangle, float p_radius, float p_opacity) override
	{
	}
	void pushRoundedClipRectangle(AvoGUI::Point<float> const& p_size, float p_radius, float p_opacity) override
	{
	}

	//------------------------------

	AvoGUI::Image* createRectangleShadowImage(AvoGUI::Point<float> const& p_size, float p_blur, AvoGUI::Color const& p_color) override
	{
	}
	AvoGUI::Image* createRectangleShadowImage(float p_width, float p_height, float p_blur, AvoGUI::Color const& p_color) override
	{
	}

	AvoGUI::Image* createRectangleShadowImage(AvoGUI::Point<float> const& p_size, AvoGUI::RectangleCorners const& p_corners, float p_blur, AvoGUI::Color const& p_color) override
	{
	}
	AvoGUI::Image* createRectangleShadowImage(float p_width, float p_height, AvoGUI::RectangleCorners const& p_corners, float p_blur, AvoGUI::Color const& p_color) override
	{
	}

	//------------------------------

	AvoGUI::Image* createRoundedRectangleShadowImage(AvoGUI::Point<float> const& p_size, float p_radius, float p_blur, AvoGUI::Color const& p_color) override
	{
	}
	AvoGUI::Image* createRoundedRectangleShadowImage(float p_width, float p_height, float p_radius, float p_blur, AvoGUI::Color const& p_color) override
	{
	}

	//------------------------------

	AvoGUI::Image* createImage(uint8 const* p_pixelData, uint32 p_width, uint32 p_height) override
	{
		GLuint texture{0u};
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, p_pixelData);
		glGenerateMipmap(GL_TEXTURE_2D);

		return new OpenGlImage(texture, p_width, p_height, m_dipToPixelFactor);
	}
	AvoGUI::Image* createImage(void const* p_imageData, uint32 p_size) override
	{
		png_image pngImage{};
		pngImage.version = PNG_IMAGE_VERSION;
		pngImage.format = PNG_FORMAT_BGRA;

		if (!png_image_begin_read_from_memory(&pngImage, p_imageData, p_size))
		{
			return nullptr;
		}

		png_byte* data{new png_byte[PNG_IMAGE_SIZE(pngImage)]};
		if (!png_image_finish_read(&pngImage, nullptr, data, 0, nullptr))
		{
			delete[] data;
			return nullptr;
		}

		AvoGUI::Image* image = createImage(data, pngImage.width, pngImage.height);
		delete[] data;
		return image;
	}
	AvoGUI::Image* createImage(char const* p_filePath) override
	{
		if (!filesystem::is_regular_file(p_filePath))
		{
			return nullptr;
		}
		switch (getImageFormatOfFile(p_filePath))
		{
			case AvoGUI::ImageFormat::Png:
			{
				png_image pngImage{};
				pngImage.version = PNG_IMAGE_VERSION;

				if (!png_image_begin_read_from_file(&pngImage, p_filePath))
				{
					return nullptr;
				}

				pngImage.format = PNG_FORMAT_BGRA;

				png_byte* data{new png_byte[PNG_IMAGE_SIZE(pngImage)]};
				if (!png_image_finish_read(&pngImage, nullptr, data, 0, nullptr))
				{
					delete[] data;
					return nullptr;
				}

				AvoGUI::Image *image = createImage(data, pngImage.width, pngImage.height);
				delete[] data;
				return image;
			}
			case AvoGUI::ImageFormat::Jpeg:
			{
				FILE* file{ fopen(p_filePath, "rb") };
				if (!file)
				{
					return nullptr;
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

				uint32 rowStride = decompressor.output_width * decompressor.output_components;
				JSAMPARRAY rowBuffer = (*decompressor.mem->alloc_sarray)((j_common_ptr)&decompressor, JPOOL_IMAGE, rowStride, 1);

				uint8* data = new uint8[rowStride*decompressor.output_height];

				while (decompressor.output_scanline < decompressor.output_height)
				{
					jpeg_read_scanlines(&decompressor, rowBuffer, 1);
					std::memcpy(data + (decompressor.output_scanline - 1)*rowStride, rowBuffer[0], rowStride);
				}

				AvoGUI::Image* image = createImage(data, decompressor.output_width, decompressor.output_height);

				jpeg_finish_decompress(&decompressor);
				jpeg_destroy_decompress(&decompressor);
				fclose(file);

				return image;
			}
		}
		return nullptr;
	}
	AvoGUI::Image* createImage(void* p_handle) override
	{
	}
	void drawImage(AvoGUI::Image* p_image, float p_multiplicativeOpacity) override
	{
		AvoGUI::Rectangle<float> innerBounds = p_image->getInnerBounds();
		float width = innerBounds.getWidth();
		float height = innerBounds.getHeight();

		AvoGUI::Rectangle<float> cropRectangle = p_image->getCropRectangle();
		AvoGUI::Point<float> originalSize = p_image->getOriginalSize();
		cropRectangle.left /= originalSize.x;
		cropRectangle.top /= originalSize.y;
		cropRectangle.right /= originalSize.x;
		cropRectangle.bottom /= originalSize.y;

		m_vertexBuffer.insert(
			m_vertexBuffer.end(),
			{
				innerBounds.left , innerBounds.top   , 1.f, width, 0.f, height, cropRectangle.left , cropRectangle.top,
				innerBounds.right, innerBounds.top   , 0.f, width, 0.f, height, cropRectangle.right, cropRectangle.top,
				innerBounds.right, innerBounds.bottom, 0.f, width, 1.f, height, cropRectangle.right, cropRectangle.bottom,

				innerBounds.left , innerBounds.top   , 0.f, width, 1.f, height, cropRectangle.left , cropRectangle.top,
				innerBounds.left , innerBounds.bottom, 0.f, width, 0.f, height, cropRectangle.left , cropRectangle.bottom,
				innerBounds.right, innerBounds.bottom, 1.f, width, 0.f, height, cropRectangle.right, cropRectangle.bottom,
			}
		);
		addDrawCall(6, *(GLuint*)p_image->getHandle());
	}

	//------------------------------

	std::string createImageFileData(AvoGUI::Image* p_image, AvoGUI::ImageFormat p_format) override
	{
	}
	void* createImageFileDataNativeStream(AvoGUI::Image* p_image, AvoGUI::ImageFormat p_format) override
	{
	}
	void saveImageToFile(AvoGUI::Image* p_image, std::string const& p_filePath, AvoGUI::ImageFormat p_format) override
	{
	}

	//------------------------------

	void* createNativeImageFromImage(AvoGUI::Image* p_image) override
	{
	}

	//------------------------------

	AvoGUI::LinearGradient* createLinearGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_endX, float p_endY) override
	{
	}
	AvoGUI::LinearGradient* createLinearGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, AvoGUI::Point<float> const& p_endPosition) override
	{
	}

	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radiusX, float p_radiusY) override
	{
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) override
	{
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, float p_radiusX, float p_radiusY) override
	{
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, float p_radius) override
	{
	}
	AvoGUI::RadialGradient* createRadialGradient(std::vector<AvoGUI::GradientStop> const& p_gradientStops, AvoGUI::Point<float> const& p_startPosition, AvoGUI::Point<float> const& p_radius) override
	{
	}

	void setGradient(AvoGUI::LinearGradient* p_gradient) override
	{
	}
	void setGradient(AvoGUI::RadialGradient* p_gradient) override
	{
	}
	void setColor(AvoGUI::Color const& p_color) override
	{
		m_currentColor = p_color;
	}

	void setOpacity(float p_opacity) override
	{
	}

	//------------------------------

	void addFont(void const* p_data, uint32 p_dataSize) override
	{
	}

	//------------------------------

	void setDefaultTextProperties(AvoGUI::TextProperties const& p_textProperties) override
	{
	}
	AvoGUI::TextProperties getDefaultTextProperties() override
	{
	}

	//------------------------------

	AvoGUI::Text* createText(char const* p_string, float p_fontSize, AvoGUI::Rectangle<float> const& p_bounds) override
	{
	}
	AvoGUI::Text* createText(std::string const& p_string, float p_fontSize, AvoGUI::Rectangle<float> const& p_bounds) override
	{
	}
	void drawText(AvoGUI::Text* p_text) override
	{
	}

	void drawText(char const* p_string, AvoGUI::Rectangle<float> const& p_rectangle) override
	{
	}
	void drawText(char const* p_string, float p_left, float p_top, float p_right, float p_bottom) override
	{
	}
	void drawText(char const* p_string, AvoGUI::Point<float> const& p_position, AvoGUI::Point<float> const& p_size) override
	{
	}
	void drawText(char const* p_string, float p_x, float p_y) override
	{
	}
	void drawText(char const* p_string, AvoGUI::Point<float> const& p_position) override
	{
	}
};

bool OpenGlDrawingContext::s_areStaticVariablesInitialized = false;
AvoGUI::Point<float> OpenGlDrawingContext::s_unitCirclePoints[OpenGlDrawingContext::CIRCLE_RESOLUTION + 1];

#endif
#pragma endregion

//------------------------------
// class GUI
//------------------------------

//
// Private
//

void AvoGUI::Gui::getTopMouseListenersAt(AvoGUI::Point<float> const& p_coordinates, std::vector<AvoGUI::View*>& p_result)
{
	if (getAreMouseEventsEnabled())
	{
		remember();
		p_result = { this };
	}
	else
	{
		p_result = { };
	}
	p_result.reserve(10);

	AvoGUI::View* container = this;
	int32 startIndex = getNumberOfChildren() - 1;

	bool hasFoundTopView = false;

	while (true)
	{
	loopStart:
		for (int32 a = startIndex; a >= 0; a--)
		{
			AvoGUI::View* child = container->getChild(a);
			// Invisible views and their children do not receive mouse events.
			if (child->getIsVisible() && child->getIsContainingAbsolute(p_coordinates.x, p_coordinates.y))
			{
				bool hasChildren = child->getNumberOfChildren();

				if (child->getAreMouseEventsEnabled())
				{
					child->remember();
					p_result.push_back(child);
				}
				if (hasChildren)
				{
					container = child;
					startIndex = container->getNumberOfChildren() - 1;
					goto loopStart; // I have determined this is the least messy way to do it pls don't kill me
				}
				else
				{
					// We only continue of it's an overlay view, meaning mouse events can pass through it.
					if (!child->getIsOverlay())
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
		if (!container->getIsOverlay() || hasFoundTopView || container == this)
		{
			break;
		}

		startIndex = container->getIndex() - 1;
		container = container->getParent();
	}
}
void AvoGUI::Gui::getTopMouseListenersAt(float p_x, float p_y, std::vector<AvoGUI::View*>& p_result)
{
	getTopMouseListenersAt(AvoGUI::Point<float>(p_x, p_y), p_result);
}

void AvoGUI::Gui::thread_runAnimationLoop()
{	
	int32 syncInterval = 16666667;
	auto timeBefore = std::chrono::steady_clock::now();

	bool wasLastFrameDrawn = false;

	while (!m_window->getWillClose())
	{
		//if (m_hasNewWindowSize)
		//{
			//excludeAnimationThread();
			//m_hasNewWindowSize = false;

			//AvoGUI::Point<float> sizeBefore = getBottomRight();
			//m_drawingContext->setSize(m_newWindowSize.x, m_newWindowSize.y);
			//m_bounds.set(0, 0, m_newWindowSize.x, m_newWindowSize.y);
			//m_shadowBounds = m_bounds;
			//m_lastWindowSize = m_newWindowSize;

			//sendBoundsChangeEvents(AvoGUI::Rectangle<float>(0.f, 0.f, sizeBefore.x, sizeBefore.y));

			//m_invalidRectangles.clear();
			//invalidate();
			//includeAnimationThread();
		//}

		excludeAnimationThread();
		uint32 numberOfEventsToProcess = m_animationUpdateQueue.size();
		for (uint32 a = 0; a < numberOfEventsToProcess; a++)
		{
			m_animationUpdateQueue.front()->informAboutAnimationUpdateQueueRemoval();
			m_animationUpdateQueue.front()->updateAnimations();
			m_animationUpdateQueue.front()->forget();
			m_animationUpdateQueue.pop_front();
		}
		includeAnimationThread();

		if (!m_invalidRectangles.empty())
		{
			drawViews();

			//wasLastFrameDrawn = true;

			if (!getDrawingContext()->getIsVsyncEnabled())
			{
				std::this_thread::sleep_for(std::chrono::nanoseconds(syncInterval));
			}
		}
		else
		{
			if (wasLastFrameDrawn)
			{
				// Just to force a buffer swap.
				invalidateRectangle(AvoGUI::Rectangle<float>(0, 0, 1, 1));
				drawViews();
				wasLastFrameDrawn = false;
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::nanoseconds(syncInterval));
			}
		}
		auto timeAfter = std::chrono::steady_clock::now();
		syncInterval = max(1000000, int32(syncInterval + 0.5 * (16666667 - (timeAfter - timeBefore).count())));
		timeBefore = timeAfter;
	}

	// This will cause the window to be destroyed, because getWillClose() is true.
	m_window->close();
	forget(); // To allow for cleanup if the animation thread is detached.
}

//
// Public
//

AvoGUI::Gui::Gui() :
	AvoGUI::View(nullptr)
{
#ifdef _WIN32
	Direct2DDrawingContext::createStaticResources();
	m_window = new WindowsWindow(this);
#endif
#ifdef __linux__
	m_window = new LinuxWindow(this);
#endif

	m_window->addWindowCreateListener(AvoGUI::bind(&Gui::handleWindowCreate, this));
	m_window->addWindowDestroyListener(AvoGUI::bind(&Gui::handleWindowDestroy, this));
	m_window->addWindowSizeChangeListener(AvoGUI::bind(&Gui::handleWindowSizeChange, this));

	m_gui = this;
}
AvoGUI::Gui::~Gui()
{
	if (m_window)
	{
		m_window->forget();
		m_window = nullptr;
	}
	if (m_drawingContextState)
	{
		m_drawingContextState->forget();
		m_drawingContextState = nullptr;
	}
	if (m_drawingContext)
	{
		m_drawingContext->forget();
		m_drawingContext = nullptr;
	}
	for (auto pair : m_viewsById)
	{
		pair.second->m_id = 0;
	}
}

void AvoGUI::Gui::create(char const* p_title, uint32 p_titleSize, float p_x, float p_y, float p_width, float p_height, WindowStyleFlags p_windowFlags, Gui* p_parent)
{
	if (p_parent)
	{
		m_parent = p_parent;
	}

	m_absolutePosition.set(0, 0);
	m_window->create(p_title, p_titleSize, p_x, p_y, p_width, p_height, p_windowFlags, p_parent ? p_parent->getWindow() : nullptr);

	m_animationThread = std::thread(&AvoGUI::Gui::thread_runAnimationLoop, this);
}
void AvoGUI::Gui::create(char const* p_title, uint32 p_titleSize, float p_width, float p_height, WindowStyleFlags p_windowFlags, Gui* p_parent)
{
	if (p_parent)
	{
		m_parent = p_parent;
	}

	m_absolutePosition.set(0, 0);
	m_window->create(p_title, p_titleSize, p_width, p_height, p_windowFlags, p_parent ? p_parent->getWindow() : nullptr);

	m_animationThread = std::thread(&AvoGUI::Gui::thread_runAnimationLoop, this);
}
void AvoGUI::Gui::create(char const* p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_windowFlags, AvoGUI::Gui* p_parent)
{
	create(p_title, strlen(p_title), p_x, p_y, p_width, p_height, p_windowFlags, p_parent);
}
void AvoGUI::Gui::create(char const* p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_windowFlags, AvoGUI::Gui* p_parent)
{
	create(p_title, strlen(p_title), p_width, p_height, p_windowFlags, p_parent);
}
void AvoGUI::Gui::create(std::string const& p_title, float p_x, float p_y, float p_width, float p_height, AvoGUI::WindowStyleFlags p_windowFlags, AvoGUI::Gui* p_parent)
{
	create(p_title.data(), p_title.size(), p_x, p_y, p_width, p_height, p_windowFlags, p_parent);
}
void AvoGUI::Gui::create(std::string const& p_title, float p_width, float p_height, AvoGUI::WindowStyleFlags p_windowFlags, AvoGUI::Gui* p_parent)
{
	create(p_title.data(), p_title.size(), p_width, p_height, p_windowFlags, p_parent);
}

//------------------------------

AvoGUI::View* AvoGUI::Gui::getViewAt(AvoGUI::Point<float> const& p_coordinates)
{
	AvoGUI::View* currentContainer = this;

	while (true)
	{
		for (int32 a = currentContainer->getNumberOfChildren() - 1; a >= 0; a--)
		{
			AvoGUI::View* view = currentContainer->getChild(a);
			if (view->getIsVisible() && !view->getIsOverlay() && view->getIsContainingAbsolute(p_coordinates))
			{
				if (view->getNumberOfChildren())
				{
					currentContainer = view;
					break;
				}
				else
				{
					return view;
				}
			}
			else if (!a)
			{
				return currentContainer;
			}
		}
	}
}
AvoGUI::View* AvoGUI::Gui::getViewAt(float p_x, float p_y)
{
	return getViewAt(AvoGUI::Point<float>(p_x, p_y));
}

//------------------------------

void AvoGUI::Gui::handleWindowCreate(WindowEvent const& p_event)
{
	excludeAnimationThread();
	if (m_drawingContext)
	{
		m_drawingContext->forget();
	}
#ifdef _WIN32
	m_drawingContext = new Direct2DDrawingContext(m_window);
#elif __linux__
	m_drawingContext = new OpenGlDrawingContext(m_window);
#endif
	m_drawingContextState = m_drawingContext->createDrawingState();

	m_bounds.set(0.f, 0.f, p_event.width, p_event.height);
	m_shadowBounds = m_bounds;
	m_lastUpdatedWindowSize.set(p_event.width, p_event.height);

	createContent();

	/*
		createContent might have changed the size of the GUI. 
		In that case, corresponding size event(s) will be caused later by a window size change event.
	*/
	if (getWidth() == p_event.width && getHeight() == p_event.height)
	{
		View::sendBoundsChangeEvents(AvoGUI::Rectangle<float>());
	}

	invalidate();
	includeAnimationThread();
}
void AvoGUI::Gui::handleWindowDestroy(WindowEvent const& p_event)
{
	excludeAnimationThread();
	if (!m_animationUpdateQueue.empty())
	{
		for (AvoGUI::View* view : m_animationUpdateQueue)
		{
			view->forget();
		}
		m_animationUpdateQueue.clear();
	}
	if (!m_pressedMouseEventListeners.empty())
	{
		for (AvoGUI::View* view : m_pressedMouseEventListeners)
		{
			view->forget();
		}
		m_pressedMouseEventListeners.clear();
	}
	includeAnimationThread();
}
void AvoGUI::Gui::handleWindowSizeChange(WindowEvent const& p_event)
{
	excludeAnimationThread();
	m_drawingContext->setSize(p_event.width, p_event.height);

	m_bounds.set(0, 0, p_event.width, p_event.height);
	m_shadowBounds = m_bounds;

	sendBoundsChangeEvents(AvoGUI::Rectangle<float>(0.f, 0.f, m_lastUpdatedWindowSize.x, m_lastUpdatedWindowSize.y));

	m_lastUpdatedWindowSize.set(p_event.width, p_event.height);
	m_invalidRectangles.clear();

	invalidate();
	includeAnimationThread();
}

//------------------------------

void AvoGUI::Gui::handleGlobalDragDropMove(DragDropEvent& p_event)
{
	AvoGUI::View* container = this;
	int32 startIndex = m_children.size() - 1;

	std::stack<bool> wasHoveringStack;
	wasHoveringStack.push(((AvoGUI::View*)this)->m_isDraggingOver);

	if (getIsContaining(p_event.x, p_event.y))
	{
		if (m_areDragDropEventsEnabled)
		{
			if (((AvoGUI::View*)this)->m_isDraggingOver)
			{
				handleDragDropMove(p_event);
			}
			else
			{
				handleDragDropEnter(p_event);

				if (startIndex < 0)
				{
					handleDragDropBackgroundEnter(p_event);
				}
			}
		}
		((AvoGUI::View*)this)->m_isDraggingOver = true;
	}
	else if (((AvoGUI::View*)this)->m_isDraggingOver)
	{
		if (m_areDragDropEventsEnabled)
		{
			handleDragDropLeave(p_event);

			if (startIndex < 0)
			{
				handleDragDropBackgroundLeave(p_event);
			}
		}
		((AvoGUI::View*)this)->m_isDraggingOver = false;
	}

	float absoluteX = p_event.x;
	float absoluteY = p_event.y;
	if (startIndex >= 0)
	{
		bool hasInvisibleParent = false;
		bool hasOverlayParent = false;
		bool hasFoundEnterViews = false;
		bool hasFoundLeaveViews = false;
		while (true)
		{
		loopStart:
			for (int32 a = startIndex; a >= 0; a--)
			{
				AvoGUI::View* child = container->m_children[a];

				if (container->m_isDraggingOver && child->getIsContainingAbsolute(absoluteX, absoluteY) && child->getIsVisible() && !hasInvisibleParent && !hasFoundEnterViews)
				{
					if (child->m_areDragDropEventsEnabled)
					{
						p_event.x = absoluteX - child->getAbsoluteLeft();
						p_event.y = absoluteY - child->getAbsoluteTop();
					}

					bool isContainer = !child->m_children.empty();

					if (child->m_isDraggingOver)
					{
						if (child->m_areDragDropEventsEnabled)
						{
							child->handleDragDropMove(p_event);
						}
					}
					else
					{
						if (child->m_areDragDropEventsEnabled)
						{
							child->handleDragDropEnter(p_event);
							if (!isContainer)
							{
								child->handleDragDropBackgroundEnter(p_event);
							}
						}
					}

					if (isContainer)
					{
						wasHoveringStack.push(child->m_isDraggingOver);
						child->m_isDraggingOver = true;
						if (child->getIsOverlay())
						{
							hasOverlayParent = true;
						}
						container = child;
						startIndex = child->getNumberOfChildren() - 1;
						goto loopStart;
					}
					else
					{
						if (!hasOverlayParent && !child->getIsOverlay())
						{
							hasFoundEnterViews = true;
							if (child->m_isDraggingOver)
							{
								hasFoundLeaveViews = true;
								break;
							}
							else if (hasFoundLeaveViews)
							{
								child->m_isDraggingOver = true;
								break;
							}
						}
						child->m_isDraggingOver = true;
					}
				}
				else if (child->m_isDraggingOver && !hasFoundLeaveViews)
				{
					bool isContainer = child->getNumberOfChildren();

					if (child->m_areDragDropEventsEnabled)
					{
						p_event.x = absoluteX - child->getAbsoluteLeft();
						p_event.y = absoluteY - child->getAbsoluteTop();
						child->handleDragDropLeave(p_event);
						if (!isContainer)
						{
							child->handleDragDropBackgroundLeave(p_event);
						}
					}

					if (isContainer)
					{
						wasHoveringStack.push(child->m_isDraggingOver);
						child->m_isDraggingOver = false;

						if (child->m_isOverlay)
						{
							hasOverlayParent = true;
						}
						if (!child->m_isVisible)
						{
							hasInvisibleParent = true;
						}
						container = child;
						startIndex = child->getNumberOfChildren() - 1;
						goto loopStart;
					}
					else
					{
						child->m_isDraggingOver = false;
						if (!hasOverlayParent && !child->m_isOverlay)
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

			if (wasHoveringStack.top() && container->m_isDraggingOver && hasFoundLeaveViews && !hasFoundEnterViews ||
				!wasHoveringStack.top() && container->m_isDraggingOver && !hasFoundEnterViews)
			{
				hasFoundEnterViews = true;
				if (container->m_areDragDropEventsEnabled)
				{
					p_event.x = absoluteX - container->getAbsoluteLeft();
					p_event.y = absoluteY - container->getAbsoluteTop();
					container->handleDragDropBackgroundEnter(p_event);
				}
			}
			else if (wasHoveringStack.top() && container->m_isDraggingOver && hasFoundEnterViews && !hasFoundLeaveViews ||
				wasHoveringStack.top() && !container->m_isDraggingOver && !hasFoundLeaveViews)
			{
				hasFoundLeaveViews = true;
				if (container->m_areDragDropEventsEnabled)
				{
					p_event.x = absoluteX - container->getAbsoluteLeft();
					p_event.y = absoluteY - container->getAbsoluteTop();
					container->handleDragDropBackgroundLeave(p_event);
				}
			}
			else if (wasHoveringStack.top() && container->m_isDraggingOver)
			{
				hasFoundEnterViews = true;
				hasFoundLeaveViews = true;
			}

			if (container == this)
			{
				break;
			}

			if (container->getIsOverlay())
			{
				wasHoveringStack.pop();
				hasOverlayParent = false;
				startIndex = container->getIndex() - 1;
				container = container->getParent();
			}
			else
			{
				while (container != this && wasHoveringStack.top() != container->m_isDraggingOver)
				{
					wasHoveringStack.pop();
					startIndex = (int32)container->getIndex() - 1;
					container = container->getParent();
					if (container->getIsOverlay())
					{
						hasOverlayParent = false;
					}
					if (!container->getIsVisible())
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
	p_event.x = absoluteX;
	p_event.y = absoluteY;
}
void AvoGUI::Gui::handleGlobalDragDropLeave(DragDropEvent& p_event)
{
	if (m_isDraggingOver)
	{
		if (m_areDragDropEventsEnabled)
		{
			handleDragDropLeave(p_event);
			handleDragDropBackgroundLeave(p_event);
		}
		m_isDraggingOver = false;
	}

	float absoluteX = p_event.x;
	float absoluteY = p_event.y;

	AvoGUI::View* container = this;
	int32 startIndex = m_children.size() - 1;
	int32 numberOfOverlayParents = 0;
	while (true)
	{
	loopStart:
		for (int32 a = startIndex; a >= 0; a--)
		{
			AvoGUI::View* child = container->m_children[a];

			if (child->m_isDraggingOver)
			{
				if (child->m_areDragDropEventsEnabled)
				{
					p_event.x = absoluteX - child->getAbsoluteLeft();
					p_event.y = absoluteY - child->getAbsoluteTop();
					child->handleDragDropLeave(p_event);
					child->handleDragDropBackgroundLeave(p_event);
				}
				child->m_isDraggingOver = false;

				if (!child->m_children.empty())
				{
					startIndex = child->m_children.size() - 1;
					container = child;
					if (container->m_isOverlay)
					{
						numberOfOverlayParents++;
					}
					goto loopStart;
				}
				else if (!child->m_isOverlay)
				{
					break;
				}
			}
		}
		if (container == this)
		{
			break;
		}
		else if (container->m_isOverlay)
		{
			startIndex = container->m_index - 1;
			container = container->m_parent;
			numberOfOverlayParents--;
		}
		else if (numberOfOverlayParents)
		{
			while (!container->m_isOverlay && container != this)
			{
				container = container->m_parent;
			}
			if (container == this)
			{
				break;
			}
			startIndex = container->m_index - 1;
			container = container->m_parent;
			numberOfOverlayParents--;
		}
		else
		{
			break;
		}
	}
}

//------------------------------

void AvoGUI::Gui::handleGlobalMouseMove(MouseEvent& p_event)
{
	// This is false if it's called from a view just to send mouse leave and mouse enter events,
	// if a view has been moved from the mouse for example.
	bool wasMouseReallyMoved = p_event.movementX || p_event.movementY;

	float absoluteX = p_event.x;
	float absoluteY = p_event.y;

	if (!m_pressedMouseEventListeners.empty())
	{
		if (wasMouseReallyMoved)
		{
			for (auto pressedView : m_pressedMouseEventListeners)
			{
				p_event.x = absoluteX - pressedView->getAbsoluteLeft();
				p_event.y = absoluteY - pressedView->getAbsoluteTop();
				pressedView->handleMouseMove(p_event);
			}
		}
	}
	else
	{
		AvoGUI::View* container = this;
		int32 startIndex = m_children.size() - 1;

		std::stack<bool> wasHoveringStack;
		wasHoveringStack.push(((AvoGUI::View*)this)->m_isMouseHovering);

		if (getIsContaining(p_event.x, p_event.y))
		{
			if (m_areMouseEventsEnabled)
			{
				if (((AvoGUI::View*)this)->m_isMouseHovering)
				{
					if (wasMouseReallyMoved)
					{
						handleMouseMove(p_event);
					}
				}
				else
				{
					handleMouseEnter(p_event);

					if (startIndex < 0)
					{
						handleMouseBackgroundEnter(p_event);
					}
				}
			}
			((AvoGUI::View*)this)->m_isMouseHovering = true;
		}
		else if (((AvoGUI::View*)this)->m_isMouseHovering)
		{
			if (getAreMouseEventsEnabled())
			{
				handleMouseLeave(p_event);

				if (startIndex < 0)
				{
					handleMouseBackgroundLeave(p_event);
				}
			}
			((AvoGUI::View*)this)->m_isMouseHovering = false;
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
				for (int32 a = startIndex; a >= 0; a--)
				{
					AvoGUI::View* child = container->m_children[a];

					if (container->m_isMouseHovering && child->getIsContainingAbsolute(absoluteX, absoluteY) && child->m_isVisible && !hasInvisibleParent && !hasFoundEnterViews)
					{
						if (child->m_areMouseEventsEnabled)
						{
							p_event.x = absoluteX - child->getAbsoluteLeft();
							p_event.y = absoluteY - child->getAbsoluteTop();
						}

						bool isContainer = !child->m_children.empty();

						if (child->m_isMouseHovering)
						{
							if (child->m_areMouseEventsEnabled && wasMouseReallyMoved)
							{
								child->handleMouseMove(p_event);
							}
						}
						else
						{
							if (child->m_areMouseEventsEnabled)
							{
								child->handleMouseEnter(p_event);
								if (!isContainer)
								{
									child->handleMouseBackgroundEnter(p_event);
								}
							}
						}

						if (isContainer)
						{
							wasHoveringStack.push(child->m_isMouseHovering);
							child->m_isMouseHovering = true;
							if (child->m_isOverlay)
							{
								hasOverlayParent = true;
							}
							container = child;
							startIndex = child->getNumberOfChildren() - 1;
							goto loopStart;
						}
						else
						{
							if (!hasOverlayParent && !child->m_isOverlay)
							{
								hasFoundEnterViews = true;
								if (child->m_isMouseHovering)
								{
									hasFoundLeaveViews = true;
									break;
								}
								else if (hasFoundLeaveViews)
								{
									child->m_isMouseHovering = true;
									break;
								}
							}
							child->m_isMouseHovering = true;
						}
					}
					else if (child->m_isMouseHovering && !hasFoundLeaveViews)
					{
						bool isContainer = child->getNumberOfChildren();

						if (child->m_areMouseEventsEnabled)
						{
							p_event.x = absoluteX - child->getAbsoluteLeft();
							p_event.y = absoluteY - child->getAbsoluteTop();
							child->handleMouseLeave(p_event);
							if (!isContainer)
							{
								child->handleMouseBackgroundLeave(p_event);
							}
						}

						if (isContainer)
						{
							wasHoveringStack.push(child->m_isMouseHovering);
							child->m_isMouseHovering = false;

							if (child->m_isOverlay)
							{
								hasOverlayParent = true;
							}
							if (!child->m_isVisible)
							{
								hasInvisibleParent = true;
							}
							container = child;
							startIndex = child->getNumberOfChildren() - 1;
							goto loopStart;
						}
						else
						{
							child->m_isMouseHovering = false;
							if (!hasOverlayParent && !child->m_isOverlay)
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

				if (wasHoveringStack.top() && container->m_isMouseHovering && hasFoundLeaveViews && !hasFoundEnterViews ||
					!wasHoveringStack.top() && container->m_isMouseHovering && !hasFoundEnterViews)
				{
					hasFoundEnterViews = true;
					if (container->m_areMouseEventsEnabled)
					{
						p_event.x = absoluteX - container->getAbsoluteLeft();
						p_event.y = absoluteY - container->getAbsoluteTop();
						container->handleMouseBackgroundEnter(p_event);
					}
				}
				else if (wasHoveringStack.top() && container->m_isMouseHovering && hasFoundEnterViews && !hasFoundLeaveViews ||
					wasHoveringStack.top() && !container->m_isMouseHovering && !hasFoundLeaveViews)
				{
					hasFoundLeaveViews = true;
					if (container->m_areMouseEventsEnabled)
					{
						p_event.x = absoluteX - container->getAbsoluteLeft();
						p_event.y = absoluteY - container->getAbsoluteTop();
						container->handleMouseBackgroundLeave(p_event);
					}
				}
				else if (wasHoveringStack.top() && container->m_isMouseHovering)
				{
					hasFoundEnterViews = true;
					hasFoundLeaveViews = true;
				}

				if (container == this)
				{
					break;
				}

				if (container->m_isOverlay)
				{
					wasHoveringStack.pop();
					hasOverlayParent = false;
					startIndex = container->getIndex() - 1;
					container = container->getParent();
				}
				else
				{
					while (container != this && wasHoveringStack.top() != container->m_isMouseHovering)
					{
						wasHoveringStack.pop();
						startIndex = (int32)container->getIndex() - 1;
						container = container->getParent();
						if (container->m_isOverlay)
						{
							hasOverlayParent = false;
						}
						if (!container->m_isVisible)
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
void AvoGUI::Gui::handleGlobalMouseLeave(MouseEvent& p_event)
{
	if (!m_pressedMouseEventListeners.empty())
	{
		return;
	}

	if (m_isMouseHovering)
	{
		if (m_areMouseEventsEnabled)
		{
			handleMouseLeave(p_event);
			handleMouseBackgroundLeave(p_event);
		}
		m_isMouseHovering = false;
	}

	float absoluteX = p_event.x;
	float absoluteY = p_event.y;

	AvoGUI::View* container = this;
	int32 startIndex = m_children.size() - 1;
	int32 numberOfOverlayParents = 0;
	while (true)
	{
		loopStart:
		for (int32 a = startIndex; a >= 0; a--)
		{
			AvoGUI::View* child = container->m_children[a];

			if (child->m_isMouseHovering)
			{
				if (child->m_areMouseEventsEnabled)
				{
					p_event.x = absoluteX - child->getAbsoluteLeft();
					p_event.y = absoluteY - child->getAbsoluteTop();
					child->handleMouseLeave(p_event);
					child->handleMouseBackgroundLeave(p_event);
				}
				child->m_isMouseHovering = false;

				if (child->m_children.size())
				{
					startIndex = child->m_children.size() - 1;
					container = child;
					if (container->m_isOverlay)
					{
						numberOfOverlayParents++;
					}
					goto loopStart;
				}
				else if (!child->m_isOverlay)
				{
					break;
				}
			}
		}
		if (container == this)
		{
			break;
		}
		else if (container->m_isOverlay)
		{
			startIndex = container->m_index - 1;
			container = container->m_parent;
			numberOfOverlayParents--;
		}
		else if (numberOfOverlayParents)
		{
			while (!container->m_isOverlay && container != this)
			{
				container = container->m_parent;
			}
			if (container == this)
			{
				break;
			}
			startIndex = container->m_index - 1;
			container = container->m_parent;
			numberOfOverlayParents--;
		}
		else
		{
			break;
		}
	}
}

//------------------------------

void AvoGUI::Gui::invalidateRectangle(AvoGUI::Rectangle<float> p_rectangle)
{
	p_rectangle.bound(m_bounds);

	if (p_rectangle.getWidth() == 0.f || p_rectangle.getHeight() == 0.f)
	{
		return;
	}

	//------------------------------
	// Here, we're rounding the coordinates to whole device pixels so that anti aliased clipping doesn't leave traces at the edges.

	float dipToPixelFactor = m_window->getDipToPixelFactor();
	p_rectangle.left = floor(p_rectangle.left * dipToPixelFactor) / dipToPixelFactor;
	p_rectangle.top = floor(p_rectangle.top * dipToPixelFactor) / dipToPixelFactor;
	p_rectangle.right = ceil(p_rectangle.right * dipToPixelFactor) / dipToPixelFactor;
	p_rectangle.bottom = ceil(p_rectangle.bottom * dipToPixelFactor) / dipToPixelFactor;

	//------------------------------

	int32 rectangleIndex = -1;
	AvoGUI::Rectangle<float>* rectangle = 0;

	bool willAdd = true;
	bool isDone = false;
	while (!isDone)
	{
		if (rectangle)
		{
			isDone = true;
			m_invalidRectanglesMutex.lock();
			for (uint32 a = 0; a < m_invalidRectangles.size(); a++)
			{
				if (a != rectangleIndex)
				{
					if (m_invalidRectangles[a].getIsIntersecting(*rectangle))
					{
						m_invalidRectangles[a].contain(*rectangle);
						m_invalidRectangles.erase(m_invalidRectangles.begin() + rectangleIndex);
						if (rectangleIndex < a)
						{
							a--;
						}
						rectangle = &m_invalidRectangles[a];
						rectangleIndex = a;
						isDone = false;
						break;
					}
				}
			}
			m_invalidRectanglesMutex.unlock();
		}
		else
		{
			isDone = true;
			m_invalidRectanglesMutex.lock();
			for (uint32 a = 0; a < m_invalidRectangles.size(); a++)
			{
				if (m_invalidRectangles[a].getIsIntersecting(p_rectangle))
				{
					rectangleIndex = a;
					rectangle = m_invalidRectangles.data() + a;
					rectangle->contain(p_rectangle);
					willAdd = false;
					isDone = false;
					break;
				}
			}
			m_invalidRectanglesMutex.unlock();
		}
	}
	if (willAdd)
	{
		m_invalidRectanglesMutex.lock();
		m_invalidRectangles.push_back(p_rectangle);
		m_invalidRectanglesMutex.unlock();
	}
}

void AvoGUI::Gui::drawViews()
{
	if (!m_invalidRectangles.empty())
	{
		m_invalidRectanglesMutex.lock();
		std::vector<AvoGUI::Rectangle<float>> invalidRectangles(std::move(m_invalidRectangles));
		m_invalidRectanglesMutex.unlock();

		excludeAnimationThread(); // State needs to be static during drawing.

		m_drawingContext->beginDrawing();

		for (auto const& targetRectangle : invalidRectangles)
		{
			AvoGUI::View* currentContainer = this;
			uint32 startPosition = 0;

			m_drawingContext->resetTransformations();
			m_drawingContext->setOpacity(1.f);
			m_drawingContext->pushClipRectangle(targetRectangle);

			m_drawingContext->clear(m_theme->colors[ThemeColors::background]);

			draw(m_drawingContext, targetRectangle);

			while (true)
			{
				bool isDoneWithContainer = true;
				for (uint32 a = startPosition; a < currentContainer->getNumberOfChildren(); a++)
				{
					AvoGUI::View* view = currentContainer->getChild(a);

					if (view->getWidth() > 0.f && view->getHeight() > 0.f && view->getIsVisible())
					{
						if (view->getAbsoluteBounds().getIsIntersecting(targetRectangle) && 
							view->getIsIntersecting(0.f, 0.f, currentContainer->getWidth(), currentContainer->getHeight()))
						{
							m_drawingContext->resetTransformations();
							m_drawingContext->setOrigin(view->getAbsoluteTopLeft());
							m_drawingContext->setOpacity(1.f);

							view->drawShadow(m_drawingContext);

							RectangleCorners& corners = view->getCorners();
							if (view->getHasCornerStyles())
							{
								m_drawingContext->pushClipGeometry(view->m_clipGeometry, view->m_opacity);
							}
							else
							{
								m_drawingContext->pushClipRectangle(view->getSize(), view->m_opacity);
							}

							view->draw(m_drawingContext, targetRectangle);

							if (view->getNumberOfChildren())
							{
								currentContainer = view;
								startPosition = 0;
								isDoneWithContainer = false;
								break;
							}
							else
							{
								m_drawingContext->resetTransformations();
								m_drawingContext->setOrigin(view->getAbsoluteTopLeft());
								m_drawingContext->setOpacity(1.f);
								view->drawOverlay(m_drawingContext, targetRectangle);

								m_drawingContext->popClipShape();
							}
						}
						else if (view->getAbsoluteShadowBounds().getIsIntersecting(targetRectangle))
						{
							m_drawingContext->resetTransformations();
							m_drawingContext->setOrigin(view->getAbsoluteTopLeft());
							m_drawingContext->setOpacity(1.f);
							view->drawShadow(m_drawingContext);
						}
					}
				}
				if (isDoneWithContainer)
				{
					if (currentContainer == this)
					{
						break;
					}

					m_drawingContext->resetTransformations();
					m_drawingContext->setOrigin(currentContainer->getAbsoluteTopLeft());
					m_drawingContext->setOpacity(1.f);
					currentContainer->drawOverlay(m_drawingContext, targetRectangle);

					m_drawingContext->popClipShape();

					startPosition = currentContainer->getIndex() + 1U;
					currentContainer = currentContainer->getParent();
				}
			}

			m_drawingContext->resetTransformations();
			m_drawingContext->setOpacity(1.f);
			drawOverlay(m_drawingContext, targetRectangle);
			m_drawingContext->popClipShape();
		}
		includeAnimationThread();
		m_drawingContext->restoreDrawingState(m_drawingContextState);
		m_drawingContext->finishDrawing(invalidRectangles);
	}
}

//------------------------------
// class AvoGUI::OpenFileDialog
//------------------------------

void AvoGUI::OpenFileDialog::open(std::vector<std::wstring>& p_openedFilePaths)
{
#ifdef _WIN32
	IFileOpenDialog* dialog;
	CoCreateInstance(CLSID_FileOpenDialog, 0, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&dialog));

	wchar_t wideTitle[200];
	convertUtf8ToUtf16(m_title.data(), m_title.size(), wideTitle, 200);
	dialog->SetTitle(wideTitle);

	COMDLG_FILTERSPEC* filters = new COMDLG_FILTERSPEC[m_fileExtensions.size()];
	// I made 1 big string buffer to decrease the number of allocations.
	wchar_t* filterStringBuffer = new wchar_t[100 * m_fileExtensions.size()];
	for (uint32 a = 0; a < m_fileExtensions.size(); a++)
	{
		filters[a].pszName = filterStringBuffer + a * 100;
		convertUtf8ToUtf16(m_fileExtensions[a].name, (wchar_t*)filters[a].pszName, 50);

		filters[a].pszSpec = filterStringBuffer + a * 100 + 50;
		convertUtf8ToUtf16(m_fileExtensions[a].extensions, (wchar_t*)filters[a].pszSpec, 50);

	}
	dialog->SetFileTypes(m_fileExtensions.size(), filters);

	if (m_canSelectMultipleFiles)
	{
		FILEOPENDIALOGOPTIONS options;
		dialog->GetOptions(&options);
		dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
	}

	HRESULT result = dialog->Show(HWND(m_gui ? m_gui->getWindow()->getNativeHandle() : 0));

	if (SUCCEEDED(result))
	{
		if (m_canSelectMultipleFiles)
		{
			IShellItemArray* items;
			dialog->GetResults(&items);

			DWORD numberOfResults = 0;
			items->GetCount(&numberOfResults);
			p_openedFilePaths.resize(numberOfResults);

			for (uint32 a = 0; a < numberOfResults; a++)
			{
				IShellItem* item;
				items->GetItemAt(a, &item);

				LPWSTR name;
				item->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &name);
				p_openedFilePaths[a] = name;

				item->Release();
			}
			items->Release();
		}
		else
		{
			IShellItem* item;
			dialog->GetResult(&item);

			LPWSTR name;
			item->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &name);
			p_openedFilePaths.resize(1);
			p_openedFilePaths[0] = name;

			item->Release();
		}
	}
	delete[] filterStringBuffer;
	delete[] filters;

	dialog->Release();
#endif
}
void AvoGUI::OpenFileDialog::open(std::vector<std::string>& p_openedFilePaths)
{
#ifdef _WIN32
	IFileOpenDialog* dialog;
	CoCreateInstance(CLSID_FileOpenDialog, 0, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&dialog));

	wchar_t wideTitle[200];
	convertUtf8ToUtf16(m_title, wideTitle, 200);
	dialog->SetTitle(wideTitle);

	COMDLG_FILTERSPEC* filters = new COMDLG_FILTERSPEC[m_fileExtensions.size()];
	// I made 1 big string buffer to decrease the number of allocations.
	wchar_t* filterStringBuffer = new wchar_t[100 * m_fileExtensions.size()];
	for (uint32 a = 0; a < m_fileExtensions.size(); a++)
	{
		filters[a].pszName = filterStringBuffer + a * 100LL;
		convertUtf8ToUtf16(m_fileExtensions[a].name, (wchar_t*)filters[a].pszName, 50);

		filters[a].pszSpec = filterStringBuffer + a * 100LL + 50;
		convertUtf8ToUtf16(m_fileExtensions[a].extensions, (wchar_t*)filters[a].pszSpec, 50);
	}
	dialog->SetFileTypes(m_fileExtensions.size(), filters);

	if (m_canSelectMultipleFiles)
	{
		FILEOPENDIALOGOPTIONS options;
		dialog->GetOptions(&options);
		dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
	}

	HRESULT result = dialog->Show(HWND(m_gui ? m_gui->getWindow()->getNativeHandle() : 0));

	if (SUCCEEDED(result))
	{
		if (m_canSelectMultipleFiles)
		{
			IShellItemArray* items;
			dialog->GetResults(&items);

			DWORD numberOfResults = 0;
			items->GetCount(&numberOfResults);
			p_openedFilePaths.resize(numberOfResults);

			for (uint32 a = 0; a < numberOfResults; a++)
			{
				IShellItem* item;
				items->GetItemAt(a, &item);

				LPWSTR name;
				item->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &name);
				p_openedFilePaths[a].resize(getNumberOfUnitsInUtfConvertedString(name) - 1);
				convertUtf16ToUtf8(name, (char*)p_openedFilePaths[a].data(), p_openedFilePaths[a].size() + 1);

				item->Release();
			}
			items->Release();
		}
		else
		{
			IShellItem* item;
			dialog->GetResult(&item);

			LPWSTR name;
			item->GetDisplayName(SIGDN::SIGDN_FILESYSPATH, &name);
			p_openedFilePaths.resize(1);
			p_openedFilePaths[0].resize(getNumberOfUnitsInUtfConvertedString(name) - 1);
			convertUtf16ToUtf8(name, (char*)p_openedFilePaths[0].data(), p_openedFilePaths[0].size() + 1);

			item->Release();
		}
	}
	delete[] filterStringBuffer;
	delete[] filters;

	dialog->Release();
#endif
}
