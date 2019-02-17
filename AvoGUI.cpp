#include "AvoGUI.hpp"

//------------------------------

#include <stack>
#include <iostream>

//------------------------------

#ifdef _WIN32
#include <Windows.h>
#include <windowsx.h>
#include <d2d1effects.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#endif

//------------------------------

namespace AvoGUI
{
	wchar_t* widenString(const char* p_string)
	{
#ifdef _WIN32
		int32_t numberOfCharacters = MultiByteToWideChar(CP_ACP, 0, p_string, -1, 0, 0);
		wchar_t* wideString = new wchar_t[numberOfCharacters];
		MultiByteToWideChar(CP_ACP, 0, p_string, -1, wideString, numberOfCharacters);

		return wideString;
#endif
	}

	//------------------------------
	// class Easing
	//------------------------------

	/*
	f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

	f0(x) = 3*t*(1-t)*(1-t)*x0
	f0'(x) = (3*x0*(t+h)*(1-t-h)*(1-t-h) - 3*t*(1-t)*(1-t)*x0)/h =
	(3*x0*(t+h)*(1-t-h)*(1-t-h) - 3*t*x0 + 6*t*t*x0 - 3*t*t*t*x0)/h =
	(3*x0*(t + h)*(1 - 2*t - 2*h + t*t + h*h + 2*t*h) - 3*t*x0 + 6*t*t*x0 - 3*t*t*t*x0)/h =
	(3*x0*(t + h - 2*t*t - 2*t*h - 2*t*h - 2*h*h + t*t*t + t*t*h + t*h*h + h*h*h + 2*t*t*h + 2*t*h*h) - 3*t*x0 + 6*t*t*x0 - 3*t*t*t*x0)/h =
	(3*t*x0 + 3*h*x0 - 6*t*t*x0 - 6*t*h*x0 - 6*t*h*x0 - 6*h*h*x0 + 3*t*t*t*x0 + 3*t*t*h*x0 + 3*t*h*h*x0 + 3*h*h*h*x0 + 6*t*t*h*x0 + 6*t*h*h*x0 - 3*t*x0 + 6*t*t*x0 - 3*t*t*t*x0)/h =
	(3*h*x0 - 6*h*h*x0 + 3*h*h*h*x0 - 12*t*h*x0 + 9*t*t*h*x0 + 9*t*h*h*x0)/h =
	3*x0 - 6*h*x0 + 3*h*h*x0 - 12*t*x0 + 9*t*t*x0 + 9*t*h*x0 =
	3*x0 - 12*t*x0 + 9*t*t*x0 =
	x0*(3 - 12*t + 9*t*t)

	f1(x) = 3*t*t*(1-t)*x1
	f1'(x) = (3*(t + h)*(t + h)*(1 - t - h)*x1 - 3*t*t*(1-t)*x1)/h =
	(3*(t + h)*(t + h)*(1 - t - h)*x1 - 3*t*t*x1 + 3*t*t*t*x1)/h =
	(3*(t*t + 2*t*h + h*h)*(1 - t - h)*x1 - 3*t*t*x1 + 3*t*t*t*x1)/h =
	((3*t*t*x1 + 6*t*h*x1 + 3*h*h*x1) - (3*t*t*x1 + 6*t*h*x1 + 3*h*h*x1)*t - (3*t*t*x1 + 6*t*h*x1 + 3*h*h*x1)*h - 3*t*t*x1 + 3*t*t*t*x1)/h =
	(3*t*t*x1 + 6*t*h*x1 + 3*h*h*x1 - 3*t*t*t*x1 - 6*t*t*h*x1 - 3*t*h*h*x1 - 3*t*t*h*x1 - 6*t*h*h*x1 - 3*h*h*h*x1 - 3*t*t*x1 + 3*t*t*t*x1)/h =
	(6*t*h*x1 - 9*t*t*h*x1 - 9*t*h*h*x1 + 3*h*h*x1 - 3*h*h*h*x1)/h =
	6*t*x1 - 9*t*t*x1 - 9*t*h*x1 + 3*h*x1 - 3*h*h*x1 =
	x1(6*t - 9*t*t)

	f2(x) = t*t*t
	f2'(x) = 3*t*t

	f'(x) = x0*(3 - 12*t + 9*t*t) + x1(6*t - 9*t*t) + 3*t*t
	*/

	float Easing::easeValue(float p_value, float p_precision) const
	{

		if (p_value <= 0.f) {
			return 0.f;
		}
		if (p_value >= 1.f) {
			return 1.f;
		}

		float t = p_value < 0.5f ? 0.25f : 0.75f;

		float error = 1;
		while (abs(error) > p_precision) {
			error = p_value - t * ((1.f - t)*(3.f*(1.f - t)*x0 + 3.f*t*x1) + t * t);
			t += error / (x0*(3.f - 12.f*t + 9.f*t*t) + x1 * (6.f*t - 9.f*t*t) + 3.f*t*t);
		}

		return t * ((1.f - t)*(3.f*(1.f - t)*y0 + 3.f*t*y1) + t * t);
	}

	//------------------------------
	// class Rectangle
	//------------------------------

	template <typename RectangleType>
	bool Rectangle<RectangleType>::getIsContaining(ProtectedRectangle* p_protectedRectangle) const
	{
		return p_protectedRectangle->getLeft() >= left && p_protectedRectangle->getTop() >= top
			&& p_protectedRectangle->getRight() <= right && p_protectedRectangle->getBottom() <= bottom;
	}
	template <typename RectangleType>
	bool Rectangle<RectangleType>::getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const
	{
		return p_protectedRectangle->getRight() >= left && p_protectedRectangle->getBottom() >= top
			&& p_protectedRectangle->getLeft() <= right && p_protectedRectangle->getTop() <= bottom;
	}

	//------------------------------
	// class View
	//------------------------------

	//
	// Private
	//

	Point<float> View::calculateAbsolutePositionRelativeTo(Point<float> p_position) const
	{
		View* container = getParent();
		while (container && container != getGUI())
		{
			p_position += container->getTopLeft();
			container = container->getParent();
		}

		return p_position;

	}

	//
	// Public
	//

	View::View(View* p_parent, const Rectangle<float>& p_bounds) :
		ProtectedRectangle(p_bounds), m_isVisible(true), m_cornerRadius(0.f), m_hasShadow(true), m_elevation(0.f),
		m_hasSizeChangedSinceLastElevationChange(true), m_shadowImage(0), m_shadowBounds(p_bounds), m_userData(0)
	{
		if (p_parent)
		{
			setParent(p_parent);

			m_GUI = m_parent->getGUI();

			//------------------------------

			m_theme = m_parent->getTheme();
			m_theme->remember();
		}
		else
		{
			m_parent = 0;
			m_GUI = 0;

			m_layerIndex = 0U;
			m_index = 0U;

			m_theme = new Theme();
		}
	}
	View::~View()
	{
		m_theme->forget();
		removeAllViews();
	}

	//------------------------------

	void View::setParent(View* p_container)
	{
		if (m_parent)
		{
			m_parent->removeView(this);
		}

		m_parent = p_container;

		m_index = m_parent->getNumberOfViews();
		if (m_parent == this)
		{
			m_layerIndex = 0;
		}
		else
		{
			m_layerIndex = m_parent->getLayerIndex() + 1U;
		}
		m_parent->addView(this);
		m_parent->updateViewDrawingIndex(this);
	}

	void View::addView(View* p_view)
	{
		p_view->setIndex(m_views.size());
		m_views.push_back(p_view);
		updateViewDrawingIndex(p_view);
	}
	void View::removeView(View* p_view)
	{
		for (int32_t a = 0; a < (int32_t)m_views.size(); a++)
		{
			if (m_views[a] == p_view)
			{
				p_view->forget();
				m_views.erase(m_views.begin() + a);
				return;
			}
		}
	}
	void View::removeView(uint32_t p_viewIndex)
	{
		m_views[p_viewIndex]->forget();
		m_views.erase(m_views.begin() + p_viewIndex);
	}
	void View::removeAllViews()
	{
		if (m_views.empty()) // That function naming, ew... Why didn't they call it getIsEmpty? empty() should be emptying something >:^(
		{
			return;
		}

		for (int32_t a = 0; a < (int32_t)m_views.size(); a++)
		{
			m_views[a]->forget();
		}
		m_views.clear();
	}

	void View::updateViewDrawingIndex(View* p_view)
	{
		int32_t numberOfViews = (int32_t)m_views.size();
		if (numberOfViews <= 1) return;

		float elevation = p_view->getElevation();
		if (!p_view->getIndex() || (p_view->getIndex() < numberOfViews - 1 && m_views[p_view->getIndex() + 1]->getElevation() < elevation))
		{
			for (uint32_t a = p_view->getIndex(); a < numberOfViews; a++)
			{
				if (a == numberOfViews - 1 || m_views[a + 1]->getElevation() >= elevation)
				{
					m_views[a] = p_view;
					p_view->setIndex(a);
					return;
				}
				else
				{
					m_views[a] = m_views[a + 1];
					m_views[a]->setIndex(a);
				}
			}
		}
		else
		{
			for (int32_t a = p_view->getIndex(); a >= 0; a--)
			{
				if (!a || m_views[a - 1]->getElevation() <= elevation)
				{
					m_views[a] = p_view;
					p_view->setIndex(a);
					return;
				}
				else
				{
					m_views[a] = m_views[a - 1];
					m_views[a]->setIndex(a);
				}
			}
		}
	}

	//------------------------------

	bool View::getIsIntersecting(View* p_view) const
	{
		Rectangle<float> viewBounds(p_view->calculateAbsoluteBounds());
		if (p_view->getParent() != getParent())
		{
			viewBounds -= getParent()->calculateAbsoluteTopLeft();
		}
		if (m_bounds.getIsIntersecting(viewBounds))
		{
			if (p_view->getCornerRadius() > 0.f)
			{
				if (m_cornerRadius > 0.f)
				{
					if (viewBounds.right - p_view->getCornerRadius() < m_bounds.left + m_cornerRadius)
					{
						if (viewBounds.bottom - p_view->getCornerRadius() < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(viewBounds.right - m_cornerRadius, viewBounds.bottom - m_cornerRadius, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
						}
						if (viewBounds.top + p_view->getCornerRadius() > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(viewBounds.right - m_cornerRadius, viewBounds.top + m_cornerRadius, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
						}
					}
					else if (viewBounds.left + p_view->getCornerRadius() > m_bounds.right - m_cornerRadius)
					{
						if (viewBounds.bottom - p_view->getCornerRadius() < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(viewBounds.left + m_cornerRadius, viewBounds.bottom - m_cornerRadius, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
						}
						if (viewBounds.top + p_view->getCornerRadius() > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(viewBounds.left + m_cornerRadius, viewBounds.top + m_cornerRadius, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
						}
					}
					return true;
				}
				float radius = p_view->getCornerRadius();
				if (m_bounds.right < viewBounds.left + radius)
				{
					if (m_bounds.bottom < viewBounds.top + radius)
					{
						return Point<>::getDistanceSquared(m_bounds.right, m_bounds.bottom, viewBounds.left + radius, viewBounds.top + radius) < radius*radius;
					}
					if (m_bounds.top < viewBounds.bottom - radius)
					{
						return Point<>::getDistanceSquared(m_bounds.right, m_bounds.top, viewBounds.left + radius, viewBounds.bottom - radius) < radius*radius;
					}
				}
				else if (m_bounds.left > viewBounds.right)
				{
					if (m_bounds.bottom < viewBounds.top + radius)
					{
						return Point<>::getDistanceSquared(m_bounds.left, m_bounds.bottom, viewBounds.right - radius, viewBounds.top + radius) < radius*radius;
					}
					if (m_bounds.top < viewBounds.bottom - radius)
					{
						return Point<>::getDistanceSquared(m_bounds.left, m_bounds.top, viewBounds.right - radius, viewBounds.bottom - radius) < radius*radius;
					}
				}
			}
			return true;
		}
		return false;
	}

	bool View::getIsContaining(View* p_view) const
	{
		Rectangle<float> viewBounds(p_view->calculateAbsoluteBounds());
		if (p_view->getParent() != getParent())
		{
			viewBounds -= getParent()->calculateAbsoluteTopLeft();
		}
		if (getIsContaining(viewBounds))
		{
			float radius = p_view->getCornerRadius();
			float maxDistance = (m_cornerRadius - radius)*(m_cornerRadius - radius);
			if (viewBounds.left < m_bounds.left + m_cornerRadius)
			{
				if (viewBounds.top < m_bounds.top + m_cornerRadius)
				{
					if (Point<>::getDistanceSquared(viewBounds.left + radius, viewBounds.top + radius, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) > maxDistance)
					{
						return false;
					}
				}
				if (viewBounds.bottom > m_bounds.bottom - m_cornerRadius)
				{
					if (Point<>::getDistanceSquared(viewBounds.left + radius, viewBounds.bottom - radius, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) > maxDistance)
					{
						return false;
					}
				}
			}
			if (viewBounds.right > m_bounds.right - m_cornerRadius)
			{
				if (viewBounds.top < m_bounds.top + m_cornerRadius)
				{
					if (Point<>::getDistanceSquared(viewBounds.right - radius, viewBounds.top + radius, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) > maxDistance)
					{
						return false;
					}
				}
				if (viewBounds.bottom > m_bounds.bottom - m_cornerRadius)
				{
					if (Point<>::getDistanceSquared(viewBounds.right - radius, viewBounds.bottom - radius, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) > maxDistance)
					{
						return false;
					}
				}
			}
			return true;
		}
		if (viewBounds.getIsContaining(m_bounds))
		{
			float radius = p_view->getCornerRadius();
			float maxDistance = (m_cornerRadius - radius)*(m_cornerRadius - radius);
			if (m_bounds.left < viewBounds.left + radius)
			{
				if (m_bounds.top < viewBounds.top + radius)
				{
					if (Point<>::getDistanceSquared(m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius, viewBounds.left + radius, viewBounds.top + radius) > maxDistance)
					{
						return false;
					}
				}
				if (m_bounds.bottom > viewBounds.bottom - radius)
				{
					if (Point<>::getDistanceSquared(m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius, viewBounds.left + radius, viewBounds.bottom - radius) > maxDistance)
					{
						return false;
					}
				}
			}
			if (m_bounds.right > viewBounds.right - radius)
			{
				if (m_bounds.top < viewBounds.top + radius)
				{
					if (Point<>::getDistanceSquared(m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius, viewBounds.right - radius, viewBounds.top + radius) > maxDistance)
					{
						return false;
					}
				}
				if (m_bounds.bottom > viewBounds.bottom - radius)
				{
					if (Point<>::getDistanceSquared(m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius, viewBounds.right - radius, viewBounds.bottom - radius) > maxDistance)
					{
						return false;
					}
				}
			}
			return true;
		}
		return false;
	}

	//------------------------------

	void View::setElevation(float p_elevation)
	{
		p_elevation = float(p_elevation < 0.f)*FLT_MAX + p_elevation;

		if (m_elevation != p_elevation || m_hasSizeChangedSinceLastElevationChange)
		{
			if (m_hasShadow && p_elevation < 400.f && p_elevation > 0.00001f)
			{
				if (m_shadowImage)
				{
					m_shadowImage->forget();
				}
				m_shadowImage = m_GUI->getDrawingContext()->createRoundedRectangleShadowImage(getSize(), m_cornerRadius, p_elevation, m_theme->colors["shadow"]);
				m_shadowBounds = Rectangle<float>(
					Point<float>(
						0.5f*(m_bounds.right - m_bounds.left - (float)m_shadowImage->getWidth()),
						0.35f*(m_bounds.bottom - m_bounds.top - (float)m_shadowImage->getHeight())
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

			if (p_elevation != m_elevation)
			{
				m_elevation = p_elevation;
				m_parent->updateViewDrawingIndex(this);
			}
			m_hasSizeChangedSinceLastElevationChange = false;
		}
	}

	void View::setHasShadow(bool p_hasShadow)
	{
		m_hasShadow = p_hasShadow;
		if (m_hasShadow)
		{
			setElevation(m_elevation);
		}
		else if (m_shadowImage)
		{
			m_shadowImage->forget();
			m_shadowImage = 0;
			m_shadowBounds = m_bounds.createCopyAtOrigin();
		}
	}

	//------------------------------

	void View::queueAnimationUpdate()
	{
		if (!m_isInAnimationUpdateQueue && m_GUI && m_isVisible)
		{
			m_GUI->getWindow()->queueAnimationUpdateForView(this);
			m_isInAnimationUpdateQueue = true;
		}
	}

	//------------------------------

	void View::invalidate()
	{
		if (m_GUI)
		{
			setElevation(m_elevation);

			Rectangle<float> shadowBounds(calculateAbsoluteShadowBounds().roundCoordinatesOutwards());
			if (shadowBounds == m_lastInvalidatedShadowBounds || (!m_lastInvalidatedShadowBounds.getWidth() && !m_lastInvalidatedShadowBounds.getHeight()))
			{
				m_GUI->invalidateRect(shadowBounds);
			}
			else if (shadowBounds.getIsIntersecting(m_lastInvalidatedShadowBounds))
			{
				m_GUI->invalidateRect(m_lastInvalidatedShadowBounds.createContainedCopy(shadowBounds));
			}
			else
			{
				m_GUI->invalidateRect(shadowBounds);
				m_GUI->invalidateRect(m_lastInvalidatedShadowBounds);
			}

			m_lastInvalidatedShadowBounds = shadowBounds;
		}
	}

	void View::drawShadow(DrawingContext* p_drawingContext)
	{
		if (m_shadowImage && m_hasShadow)
		{
			p_drawingContext->setColor(Color(1.f));
			p_drawingContext->drawImage(m_shadowImage);
		}
	}

	//------------------------------
	// class MouseEventListener
	//------------------------------

	void MouseEventListener::handleMouseEnter(const MouseEvent& p_event)
	{
		View* view = dynamic_cast<View*>(this);
		if (view)
		{
			view->getGUI()->getWindow()->setCursor(m_cursor);
		}
	}

	//------------------------------

#pragma region Platform-specific window implementations
#ifdef _WIN32
	class WindowsWindow : public Window
	{
	private:
		HWND m_windowHandle;

		bool m_isMouseOutsideWindow;

		HCURSOR m_cursorHandle;
		Cursor m_cursorType;

		ModifierKeyFlags convertWindowsKeyStateToModifierKeyFlags(unsigned short p_keyState)
		{
			ModifierKeyFlags modifierFlags = ModifierKeyFlags::None;
			if (p_keyState & MK_CONTROL)
			{
				modifierFlags |= ModifierKeyFlags::Ctrl;
			}
			if (p_keyState & MK_SHIFT)
			{
				modifierFlags |= ModifierKeyFlags::Shift;
			}
			if (p_keyState & MK_LBUTTON)
			{
				modifierFlags |= ModifierKeyFlags::LeftMouse;
			}
			if (p_keyState & MK_MBUTTON)
			{
				modifierFlags |= ModifierKeyFlags::MiddleMouse;
			}
			if (p_keyState & MK_RBUTTON)
			{
				modifierFlags |= ModifierKeyFlags::RightMouse;
			}
			if (p_keyState & MK_XBUTTON1)
			{
				modifierFlags |= ModifierKeyFlags::X0Mouse;
			}
			if (p_keyState & MK_XBUTTON2)
			{
				modifierFlags |= ModifierKeyFlags::X1Mouse;
			}
			if (GetKeyState(VK_MENU) < 0)
			{
				modifierFlags |= ModifierKeyFlags::Alt;
			}
			return modifierFlags;
		}
		KeyboardKey convertWindowsDataToKeyboardKey(uint64_t p_data)
		{
			KeyboardKey key = KeyboardKey::None;
			switch (p_data)
			{
			case VK_BACK:
				key = KeyboardKey::Backspace;
				break;
			case VK_CLEAR:
				key = KeyboardKey::Clear;
				break;
			case VK_TAB:
				key = KeyboardKey::Tab;
				break;
			case VK_RETURN:
				key = KeyboardKey::Return;
				break;
			case VK_SHIFT:
				key = KeyboardKey::Shift;
				break;
			case VK_CONTROL:
				key = KeyboardKey::Control;
				break;
			case VK_MENU:
				key = KeyboardKey::Alt;
				break;
			case VK_PAUSE:
				key = KeyboardKey::Pause;
				break;
			case VK_PLAY:
				key = KeyboardKey::Play;
				break;
			case VK_CAPITAL:
				key = KeyboardKey::CapsLock;
				break;
			case VK_ESCAPE:
				key = KeyboardKey::Escape;
				break;
			case VK_SPACE:
				key = KeyboardKey::Spacebar;
				break;
			case VK_PRIOR:
				key = KeyboardKey::PageUp;
				break;
			case VK_NEXT:
				key = KeyboardKey::PageDown;
				break;
			case VK_END:
				key = KeyboardKey::End;
				break;
			case VK_HOME:
				key = KeyboardKey::Home;
				break;
			case VK_LEFT:
				key = KeyboardKey::Left;
				break;
			case VK_RIGHT:
				key = KeyboardKey::Right;
				break;
			case VK_UP:
				key = KeyboardKey::Up;
				break;
			case VK_DOWN:
				key = KeyboardKey::Down;
				break;
			case VK_SNAPSHOT:
				key = KeyboardKey::PrintScreen;
				break;
			case VK_INSERT:
				key = KeyboardKey::Insert;
				break;
			case VK_DELETE:
				key = KeyboardKey::Delete;
				break;
			case VK_HELP:
				key = KeyboardKey::Help;
				break;
			case VK_NUMPAD0:
				key = KeyboardKey::Numpad0;
				break;
			case VK_NUMPAD1:
				key = KeyboardKey::Numpad1;
				break;
			case VK_NUMPAD2:
				key = KeyboardKey::Numpad2;
				break;
			case VK_NUMPAD3:
				key = KeyboardKey::Numpad3;
				break;
			case VK_NUMPAD4:
				key = KeyboardKey::Numpad4;
				break;
			case VK_NUMPAD5:
				key = KeyboardKey::Numpad5;
				break;
			case VK_NUMPAD6:
				key = KeyboardKey::Numpad6;
				break;
			case VK_NUMPAD7:
				key = KeyboardKey::Numpad7;
				break;
			case VK_NUMPAD8:
				key = KeyboardKey::Numpad8;
				break;
			case VK_NUMPAD9:
				key = KeyboardKey::Numpad9;
				break;
			case VK_F1:
				key = KeyboardKey::F1;
				break;
			case VK_F2:
				key = KeyboardKey::F2;
				break;
			case VK_F3:
				key = KeyboardKey::F3;
				break;
			case VK_F4:
				key = KeyboardKey::F4;
				break;
			case VK_F5:
				key = KeyboardKey::F5;
				break;
			case VK_F6:
				key = KeyboardKey::F6;
				break;
			case VK_F7:
				key = KeyboardKey::F7;
				break;
			case VK_F8:
				key = KeyboardKey::F8;
				break;
			case VK_F9:
				key = KeyboardKey::F9;
				break;
			case VK_F10:
				key = KeyboardKey::F10;
				break;
			case VK_F11:
				key = KeyboardKey::F11;
				break;
			case VK_F12:
				key = KeyboardKey::F12;
				break;
			case VK_F13:
				key = KeyboardKey::F13;
				break;
			case VK_F14:
				key = KeyboardKey::F14;
				break;
			case VK_F15:
				key = KeyboardKey::F15;
				break;
			case VK_F16:
				key = KeyboardKey::F16;
				break;
			case VK_F17:
				key = KeyboardKey::F17;
				break;
			case VK_F18:
				key = KeyboardKey::F18;
				break;
			case VK_F19:
				key = KeyboardKey::F19;
				break;
			case VK_F20:
				key = KeyboardKey::F20;
				break;
			case VK_F21:
				key = KeyboardKey::F21;
				break;
			case VK_F22:
				key = KeyboardKey::F22;
				break;
			case VK_F23:
				key = KeyboardKey::F23;
				break;
			case VK_F24:
				key = KeyboardKey::F24;
				break;
			case VK_NUMLOCK:
				key = KeyboardKey::NumLock;
				break;
			case VK_LSHIFT:
				key = KeyboardKey::ShiftLeft;
				break;
			case VK_RSHIFT:
				key = KeyboardKey::ShiftRight;
				break;
			case VK_LCONTROL:
				key = KeyboardKey::ControlLeft;
				break;
			case VK_RCONTROL:
				key = KeyboardKey::ControlRight;
				break;
			case VK_LMENU:
				key = KeyboardKey::MenuLeft;
				break;
			case VK_RMENU:
				key = KeyboardKey::MenuRight;
				break;
			case VK_MEDIA_PREV_TRACK:
				key = KeyboardKey::PreviousTrack;
				break;
			case VK_MEDIA_NEXT_TRACK:
				key = KeyboardKey::NextTrack;
				break;
			case VK_MEDIA_PLAY_PAUSE:
				key = KeyboardKey::PlayPauseTrack;
				break;
			case VK_MEDIA_STOP:
				key = KeyboardKey::StopTrack;
				break;
			case 0x30:
				key = KeyboardKey::Number0;
				break;
			case 0x31:
				key = KeyboardKey::Number1;
				break;
			case 0x32:
				key = KeyboardKey::Number2;
				break;
			case 0x33:
				key = KeyboardKey::Number3;
				break;
			case 0x34:
				key = KeyboardKey::Number4;
				break;
			case 0x35:
				key = KeyboardKey::Number5;
				break;
			case 0x36:
				key = KeyboardKey::Number6;
				break;
			case 0x37:
				key = KeyboardKey::Number7;
				break;
			case 0x38:
				key = KeyboardKey::Number8;
				break;
			case 0x39:
				key = KeyboardKey::Number9;
				break;
			case 0x41:
				key = KeyboardKey::A;
				break;
			case 0x42:
				key = KeyboardKey::B;
				break;
			case 0x43:
				key = KeyboardKey::C;
				break;
			case 0x44:
				key = KeyboardKey::D;
				break;
			case 0x45:
				key = KeyboardKey::E;
				break;
			case 0x46:
				key = KeyboardKey::F;
				break;
			case 0x47:
				key = KeyboardKey::G;
				break;
			case 0x48:
				key = KeyboardKey::H;
				break;
			case 0x49:
				key = KeyboardKey::I;
				break;
			case 0x4A:
				key = KeyboardKey::J;
				break;
			case 0x4B:
				key = KeyboardKey::K;
				break;
			case 0x4C:
				key = KeyboardKey::L;
				break;
			case 0x4D:
				key = KeyboardKey::M;
				break;
			case 0x4E:
				key = KeyboardKey::N;
				break;
			case 0x4F:
				key = KeyboardKey::O;
				break;
			case 0x50:
				key = KeyboardKey::P;
				break;
			case 0x51:
				key = KeyboardKey::Q;
				break;
			case 0x52:
				key = KeyboardKey::R;
				break;
			case 0x53:
				key = KeyboardKey::S;
				break;
			case 0x54:
				key = KeyboardKey::T;
				break;
			case 0x55:
				key = KeyboardKey::U;
				break;
			case 0x56:
				key = KeyboardKey::V;
				break;
			case 0x57:
				key = KeyboardKey::W;
				break;
			case 0x58:
				key = KeyboardKey::X;
				break;
			case 0x59:
				key = KeyboardKey::Y;
				break;
			case 0x5A:
				key = KeyboardKey::Z;
				break;
			case VK_OEM_1:
				key = KeyboardKey::Regional1;
				break;
			case VK_OEM_2:
				key = KeyboardKey::Regional2;
				break;
			case VK_OEM_3:
				key = KeyboardKey::Regional3;
				break;
			case VK_OEM_4:
				key = KeyboardKey::Regional4;
				break;
			case VK_OEM_5:
				key = KeyboardKey::Regional5;
				break;
			case VK_OEM_6:
				key = KeyboardKey::Regional6;
				break;
			case VK_OEM_7:
				key = KeyboardKey::Regional7;
				break;
			case VK_OEM_8:
				key = KeyboardKey::Regional8;
				break;
			}
			return key;
		}

	public:
		static uint32_t s_numberOfWindows;
		static const char* const WINDOW_CLASS_NAME;

		//------------------------------

		WindowsWindow(GUI* p_GUI) :
			m_windowHandle(0), m_isMouseOutsideWindow(true), m_cursorHandle(0)
		{
			m_GUI = p_GUI;
			m_isFullscreen = false;

			m_cursorType = (Cursor)-1;
			setCursor(Cursor::Arrow);
		}
		WindowsWindow(GUI* p_GUI, const char* p_title, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) :
			m_windowHandle(0), m_isMouseOutsideWindow(true), m_cursorHandle(0)
		{
			m_GUI = p_GUI;
			m_isFullscreen = false;

			m_size.x = 0;
			m_size.y = 0;

			create(p_title, p_width, p_height, p_styleFlags, p_parent);

			m_cursorType = (Cursor)-1;
			setCursor(Cursor::Arrow);
		}
		~WindowsWindow()
		{
			close();
			DestroyCursor(m_cursorHandle);
		}

		//------------------------------

		void create(const char* p_title, int32_t p_x, int32_t p_y, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, Window* p_parent = 0) override
		{
			if (m_windowHandle)
			{
				DestroyWindow(m_windowHandle);
				s_numberOfWindows--;
			}
			else if (!s_numberOfWindows)
			{
				WNDCLASS windowClass = { };
				windowClass.lpszClassName = WINDOW_CLASS_NAME;
				windowClass.hInstance = GetModuleHandle(0);
				windowClass.lpfnWndProc = handleGlobalEvents;
				windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
				windowClass.hCursor = 0;
				windowClass.style = CS_DBLCLKS;

				RegisterClass(&windowClass);
			}

			uint32_t extendedStyles = 0;
			uint32_t styles = WS_POPUP | WS_SYSMENU;
			if (uint32_t(p_styleFlags & WindowStyleFlags::Border))
			{
				styles |= WS_CAPTION;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::Visible))
			{
				styles |= WS_VISIBLE;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::Child))
			{
				styles |= WS_CHILD;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::Minimized))
			{
				styles |= WS_MINIMIZE;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::Maximized))
			{
				styles |= WS_MAXIMIZE;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::MinimizeBox))
			{
				styles |= WS_MINIMIZEBOX;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::MaximizeBox))
			{
				styles |= WS_MAXIMIZEBOX;
			}
			if (uint32_t(p_styleFlags & WindowStyleFlags::ResizeBorder))
			{
				styles |= WS_THICKFRAME;
			}

			// m_windowHandle is initialized by the WM_CREATE event,
			// before CreateWindowEx returns. This is because the GUI
			// needs to create the drawing context before the first WM_PAINT
			// message is recieved.
			CreateWindowEx(
				extendedStyles,
				WINDOW_CLASS_NAME,
				p_title,
				styles,
				p_x,
				p_y,
				p_width,
				p_height,
				p_parent ? (HWND)p_parent->getWindowHandle() : 0,
				0, // No menu
				GetModuleHandle(0),
				this // Additional window data
			);

			UpdateWindow(m_windowHandle);

			s_numberOfWindows++;
		}
		void create(const char* p_title, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, Window* p_parent = 0) override
		{
			create(p_title, (GetSystemMetrics(SM_CXSCREEN) - p_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - p_height) / 2, p_width, p_height, p_styleFlags, p_isFullscreen, p_parent);
		}

		void close() override
		{
			if (m_windowHandle)
			{
				DestroyWindow(m_windowHandle);
				m_windowHandle = 0;
				s_numberOfWindows--;

				if (!s_numberOfWindows)
				{
					UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(0));
					PostQuitMessage(0);
				}
			}
		}

		//------------------------------

		void setWindowHandle(HWND p_handle)
		{
			m_windowHandle = p_handle;
		}
		void* getWindowHandle() override
		{
			return m_windowHandle;
		}

		//------------------------------

		void setIsFullscreen(bool p_isFullscreen) override
		{
			m_isFullscreen = p_isFullscreen;
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

		//------------------------------

		void setPosition(const Point<int32_t>& p_position) override
		{
			SetWindowPos(m_windowHandle, 0, p_position.x, p_position.y, 0, 0, SWP_NOSIZE);
			m_position = p_position;
		}
		void setPosition(int32_t p_x, int32_t p_y) override
		{
			SetWindowPos(m_windowHandle, 0, p_x, p_y, 0, 0, SWP_NOSIZE);
			m_position.set(p_x, p_y);
		}

		void setSize(const Point<uint32_t>& p_size) override
		{
			SetWindowPos(m_windowHandle, 0, 0, 0, p_size.x, p_size.y, SWP_NOMOVE);
			m_size = p_size;
		}
		void setSize(uint32_t p_width, uint32_t p_height) override
		{
			SetWindowPos(m_windowHandle, 0, 0, 0, p_width, p_height, SWP_NOMOVE);
			m_size.set(p_width, p_height);
		}

		//------------------------------

		Rectangle<uint32_t> getMonitorBounds() override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Rectangle<uint32_t>(info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom);
		}
		Point<uint32_t> getMonitorPosition() override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Point<uint32_t>(info.rcMonitor.left, info.rcMonitor.top);
		}
		Point<uint32_t> getMonitorSize() override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
			return Point<uint32_t>(info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top);
		}

		//------------------------------

		bool getIsKeyDown(KeyboardKey p_key)
		{
			switch (p_key)
			{
			case KeyboardKey::A:
				return GetAsyncKeyState(0x41);
			case KeyboardKey::B:
				return GetAsyncKeyState(0x42);
			case KeyboardKey::C:
				return GetAsyncKeyState(0x43);
			case KeyboardKey::D:
				return GetAsyncKeyState(0x44);
			case KeyboardKey::E:
				return GetAsyncKeyState(0x45);
			case KeyboardKey::F:
				return GetAsyncKeyState(0x46);
			case KeyboardKey::G:
				return GetAsyncKeyState(0x47);
			case KeyboardKey::H:
				return GetAsyncKeyState(0x48);
			case KeyboardKey::I:
				return GetAsyncKeyState(0x49);
			case KeyboardKey::J:
				return GetAsyncKeyState(0x4A);
			case KeyboardKey::K:
				return GetAsyncKeyState(0x4B);
			case KeyboardKey::L:
				return GetAsyncKeyState(0x4C);
			case KeyboardKey::M:
				return GetAsyncKeyState(0x4D);
			case KeyboardKey::N:
				return GetAsyncKeyState(0x4E);
			case KeyboardKey::O:
				return GetAsyncKeyState(0x4F);
			case KeyboardKey::P:
				return GetAsyncKeyState(0x50);
			case KeyboardKey::Q:
				return GetAsyncKeyState(0x51);
			case KeyboardKey::R:
				return GetAsyncKeyState(0x52);
			case KeyboardKey::S:
				return GetAsyncKeyState(0x53);
			case KeyboardKey::T:
				return GetAsyncKeyState(0x54);
			case KeyboardKey::U:
				return GetAsyncKeyState(0x55);
			case KeyboardKey::V:
				return GetAsyncKeyState(0x56);
			case KeyboardKey::W:
				return GetAsyncKeyState(0x57);
			case KeyboardKey::X:
				return GetAsyncKeyState(0x58);
			case KeyboardKey::Y:
				return GetAsyncKeyState(0x59);
			case KeyboardKey::Z:
				return GetAsyncKeyState(0x5A);
			case KeyboardKey::Add:
				return GetAsyncKeyState(VK_ADD);
			case KeyboardKey::Alt:
				return GetAsyncKeyState(VK_MENU);
			case KeyboardKey::Backspace:
				return GetAsyncKeyState(VK_BACK);
			case KeyboardKey::CapsLock:
				return GetAsyncKeyState(VK_CAPITAL);
			case KeyboardKey::Clear:
				return GetAsyncKeyState(VK_CLEAR);
			case KeyboardKey::Comma:
				return GetAsyncKeyState(VK_OEM_COMMA);
			case KeyboardKey::Control:
				return GetAsyncKeyState(VK_CONTROL);
			case KeyboardKey::ControlLeft:
				return GetAsyncKeyState(VK_LCONTROL);
			case KeyboardKey::ControlRight:
				return GetAsyncKeyState(VK_RCONTROL);
			case KeyboardKey::Decimal:
				return GetAsyncKeyState(VK_DECIMAL);
			case KeyboardKey::Delete:
				return GetAsyncKeyState(VK_DELETE);
			case KeyboardKey::Divide:
				return GetAsyncKeyState(VK_DIVIDE);
			case KeyboardKey::Down:
				return GetAsyncKeyState(VK_DOWN);
			case KeyboardKey::End:
				return GetAsyncKeyState(VK_END);
			case KeyboardKey::Enter:
				return GetAsyncKeyState(VK_RETURN);
			case KeyboardKey::Escape:
				return GetAsyncKeyState(VK_ESCAPE);
			case KeyboardKey::F1:
				return GetAsyncKeyState(VK_F1);
			case KeyboardKey::F2:
				return GetAsyncKeyState(VK_F2);
			case KeyboardKey::F3:
				return GetAsyncKeyState(VK_F3);
			case KeyboardKey::F4:
				return GetAsyncKeyState(VK_F4);
			case KeyboardKey::F5:
				return GetAsyncKeyState(VK_F5);
			case KeyboardKey::F6:
				return GetAsyncKeyState(VK_F6);
			case KeyboardKey::F7:
				return GetAsyncKeyState(VK_F7);
			case KeyboardKey::F8:
				return GetAsyncKeyState(VK_F8);
			case KeyboardKey::F9:
				return GetAsyncKeyState(VK_F9);
			case KeyboardKey::F10:
				return GetAsyncKeyState(VK_F10);
			case KeyboardKey::F11:
				return GetAsyncKeyState(VK_F11);
			case KeyboardKey::F12:
				return GetAsyncKeyState(VK_F12);
			case KeyboardKey::F13:
				return GetAsyncKeyState(VK_F13);
			case KeyboardKey::F14:
				return GetAsyncKeyState(VK_F14);
			case KeyboardKey::F15:
				return GetAsyncKeyState(VK_F15);
			case KeyboardKey::F16:
				return GetAsyncKeyState(VK_F16);
			case KeyboardKey::F17:
				return GetAsyncKeyState(VK_F17);
			case KeyboardKey::F18:
				return GetAsyncKeyState(VK_F18);
			case KeyboardKey::F19:
				return GetAsyncKeyState(VK_F19);
			case KeyboardKey::F20:
				return GetAsyncKeyState(VK_F20);
			case KeyboardKey::F21:
				return GetAsyncKeyState(VK_F21);
			case KeyboardKey::F22:
				return GetAsyncKeyState(VK_F22);
			case KeyboardKey::F23:
				return GetAsyncKeyState(VK_F23);
			case KeyboardKey::F24:
				return GetAsyncKeyState(VK_F24);
			case KeyboardKey::Help:
				return GetAsyncKeyState(VK_HELP);
			case KeyboardKey::Home:
				return GetAsyncKeyState(VK_HOME);
			case KeyboardKey::Insert:
				return GetAsyncKeyState(VK_INSERT);
			case KeyboardKey::Left:
				return GetAsyncKeyState(VK_LEFT);
			case KeyboardKey::MenuLeft:
				return GetAsyncKeyState(VK_LMENU);
			case KeyboardKey::MenuRight:
				return GetAsyncKeyState(VK_RMENU);
			case KeyboardKey::Minus:
				return GetAsyncKeyState(VK_OEM_MINUS);
			case KeyboardKey::Multiply:
				return GetAsyncKeyState(VK_MULTIPLY);
			case KeyboardKey::NextTrack:
				return GetAsyncKeyState(VK_MEDIA_NEXT_TRACK);
			case KeyboardKey::Number0:
				return GetAsyncKeyState(0x30);
			case KeyboardKey::Number1:
				return GetAsyncKeyState(0x31);
			case KeyboardKey::Number2:
				return GetAsyncKeyState(0x32);
			case KeyboardKey::Number3:
				return GetAsyncKeyState(0x33);
			case KeyboardKey::Number4:
				return GetAsyncKeyState(0x34);
			case KeyboardKey::Number5:
				return GetAsyncKeyState(0x35);
			case KeyboardKey::Number6:
				return GetAsyncKeyState(0x36);
			case KeyboardKey::Number7:
				return GetAsyncKeyState(0x37);
			case KeyboardKey::Number8:
				return GetAsyncKeyState(0x38);
			case KeyboardKey::Number9:
				return GetAsyncKeyState(0x39);
			case KeyboardKey::NumLock:
				return GetAsyncKeyState(VK_NUMLOCK);
			case KeyboardKey::Numpad0:
				return GetAsyncKeyState(VK_NUMPAD0);
			case KeyboardKey::Numpad1:
				return GetAsyncKeyState(VK_NUMPAD1);
			case KeyboardKey::Numpad2:
				return GetAsyncKeyState(VK_NUMPAD2);
			case KeyboardKey::Numpad3:
				return GetAsyncKeyState(VK_NUMPAD3);
			case KeyboardKey::Numpad4:
				return GetAsyncKeyState(VK_NUMPAD4);
			case KeyboardKey::Numpad5:
				return GetAsyncKeyState(VK_NUMPAD5);
			case KeyboardKey::Numpad6:
				return GetAsyncKeyState(VK_NUMPAD6);
			case KeyboardKey::Numpad7:
				return GetAsyncKeyState(VK_NUMPAD7);
			case KeyboardKey::Numpad8:
				return GetAsyncKeyState(VK_NUMPAD8);
			case KeyboardKey::Numpad9:
				return GetAsyncKeyState(VK_NUMPAD9);
			case KeyboardKey::PageDown:
				return GetAsyncKeyState(VK_NEXT);
			case KeyboardKey::PageUp:
				return GetAsyncKeyState(VK_PRIOR);
			case KeyboardKey::Pause:
				return GetAsyncKeyState(VK_PAUSE);
			case KeyboardKey::Period:
				return GetAsyncKeyState(VK_OEM_PERIOD);
			case KeyboardKey::Play:
				return GetAsyncKeyState(VK_PLAY);
			case KeyboardKey::PlayPauseTrack:
				return GetAsyncKeyState(VK_MEDIA_PLAY_PAUSE);
			case KeyboardKey::Plus:
				return GetAsyncKeyState(VK_OEM_PLUS);
			case KeyboardKey::PreviousTrack:
				return GetAsyncKeyState(VK_MEDIA_PREV_TRACK);
			case KeyboardKey::PrintScreen:
				return GetAsyncKeyState(VK_SNAPSHOT);
			case KeyboardKey::Regional1:
				return GetAsyncKeyState(VK_OEM_1);
			case KeyboardKey::Regional2:
				return GetAsyncKeyState(VK_OEM_2);
			case KeyboardKey::Regional3:
				return GetAsyncKeyState(VK_OEM_3);
			case KeyboardKey::Regional4:
				return GetAsyncKeyState(VK_OEM_4);
			case KeyboardKey::Regional5:
				return GetAsyncKeyState(VK_OEM_5);
			case KeyboardKey::Regional6:
				return GetAsyncKeyState(VK_OEM_6);
			case KeyboardKey::Regional7:
				return GetAsyncKeyState(VK_OEM_7);
			case KeyboardKey::Regional8:
				return GetAsyncKeyState(VK_OEM_8);
			case KeyboardKey::Right:
				return GetAsyncKeyState(VK_RIGHT);
			case KeyboardKey::Separator:
				return GetAsyncKeyState(VK_SEPARATOR);
			case KeyboardKey::Shift:
				return GetAsyncKeyState(VK_SHIFT);
			case KeyboardKey::ShiftLeft:
				return GetAsyncKeyState(VK_LSHIFT);
			case KeyboardKey::ShiftRight:
				return GetAsyncKeyState(VK_RSHIFT);
			case KeyboardKey::Spacebar:
				return GetAsyncKeyState(VK_SPACE);
			case KeyboardKey::StopTrack:
				return GetAsyncKeyState(VK_MEDIA_STOP);
			case KeyboardKey::Subtract:
				return GetAsyncKeyState(VK_SUBTRACT);
			case KeyboardKey::Tab:
				return GetAsyncKeyState(VK_TAB);
			case KeyboardKey::Up:
				return GetAsyncKeyState(VK_UP);
			}
			return false;
		}
		bool getIsMouseButtonDown(MouseButton p_button) override
		{
			switch (p_button)
			{
			case MouseButton::Left:
				return GetAsyncKeyState(VK_LBUTTON);
			case MouseButton::Middle:
				return GetAsyncKeyState(VK_MBUTTON);
			case MouseButton::Right:
				return GetAsyncKeyState(VK_RBUTTON);
			case MouseButton::X0:
				return GetAsyncKeyState(VK_XBUTTON1);
			case MouseButton::X1:
				return GetAsyncKeyState(VK_XBUTTON2);
			}
			return false;
		}

		//------------------------------

		void setAnimationTimerInterval(uint32_t p_interval) override
		{
			SetTimer(m_windowHandle, 1, p_interval, 0);
		}
		void queueAnimationUpdateForView(View* p_view) override
		{
			m_animationUpdateQueue.push_back(p_view);
		}

		void invalidateRect(const Rectangle<float>& p_rectangle) override
		{
			RECT rect;
			rect.left = p_rectangle.left;
			rect.top = p_rectangle.top;
			rect.right = p_rectangle.right;
			rect.bottom = p_rectangle.bottom;

			InvalidateRect(m_windowHandle, &rect, false);
		}

		//------------------------------

		void setCursor(Cursor p_cursor) override
		{
			if (p_cursor == m_cursorType) return;

			const char* name = 0;
			switch (p_cursor)
			{
			case Cursor::Arrow:
				name = IDC_ARROW;
				break;
			case Cursor::Blocked:
				name = IDC_NO;
				break;
			case Cursor::Hand:
				name = IDC_HAND;
				break;
			case Cursor::Ibeam:
				name = IDC_IBEAM;
				break;
			case Cursor::ResizeAll:
				name = IDC_SIZEALL;
				break;
			case Cursor::ResizeNESW:
				name = IDC_SIZENESW;
				break;
			case Cursor::ResizeNS:
				name = IDC_SIZENS;
				break;
			case Cursor::ResizeNWSE:
				name = IDC_SIZENWSE;
				break;
			case Cursor::ResizeWE:
				name = IDC_SIZEWE;
				break;
			case Cursor::Wait:
				name = IDC_WAIT;
				break;
			}
			m_cursorType = p_cursor;
			if (m_cursorHandle)
			{
				DestroyCursor(m_cursorHandle);
			}
			m_cursorHandle = LoadCursor(0, name);
			if (!m_isMouseOutsideWindow)
			{
				SetCursor(m_cursorHandle);
			}
		}
		Cursor getCursor() override
		{
			return m_cursorType;
		}

		//------------------------------

		// Returns true if the event was handled
		bool handleEvent(UINT p_message, WPARAM p_data_a, LPARAM p_data_b)
		{
			switch (p_message)
			{
			case WM_CREATE:
			{
				SetTimer(m_windowHandle, 1, 17, 0);

				WindowEvent windowEvent;
				windowEvent.window = this;
				m_GUI->handleWindowCreated(windowEvent);

				return true;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT paintStructure;
				BeginPaint(m_windowHandle, &paintStructure);

				m_GUI->draw(
					m_GUI->getDrawingContext(),
					Rectangle<float>(
						paintStructure.rcPaint.left, paintStructure.rcPaint.top,
						paintStructure.rcPaint.right, paintStructure.rcPaint.bottom
						)
				);

				EndPaint(m_windowHandle, &paintStructure);
				return true;
			}
			case WM_SIZE:
			{
				WindowEvent windowEvent;
				windowEvent.window = this;
				if (p_data_a == SIZE_MINIMIZED)
				{
					m_GUI->handleWindowMinimized(windowEvent);
				}
				else
				{
					uint32_t width = p_data_b & 0xffff;
					uint32_t height = p_data_b >> 16 & 0xffff;
					m_size.set(width, height);

					windowEvent.width = width;
					windowEvent.height = height;

					if (p_data_a == SIZE_MAXIMIZED)
					{
						m_GUI->handleWindowMaximized(windowEvent);
					}
					else if (p_data_a == SIZE_RESTORED)
					{
						m_GUI->handleWindowRestored(windowEvent);
					}
					m_GUI->handleWindowSizeChanged(windowEvent);
				}

				return true;
			}
			case WM_MOVE:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				m_position.set(x, y);

				return true;
			}
			case WM_TIMER:
			{
				uint32_t sizeBefore = m_animationUpdateQueue.size();
				for (uint32_t a = 0; a < sizeBefore; a++)
				{
					m_animationUpdateQueue.front()->informAboutAnimationUpdateQueueRemoval(); // We do this before updateAnimation() because it should be able to queue the next animation update.
					m_animationUpdateQueue.front()->updateAnimations();
					m_animationUpdateQueue.pop_front();
				}
				return true;
			}
			case WM_MOUSEWHEEL:
			{
				POINT mousePosition;
				mousePosition.x = GET_X_LPARAM(p_data_b);
				mousePosition.y = GET_Y_LPARAM(p_data_b);

				ScreenToClient(m_windowHandle, &mousePosition);

				float delta = float(GET_WHEEL_DELTA_WPARAM(p_data_a)) / 120.f;

				ModifierKeyFlags modifierKeyFlags = convertWindowsKeyStateToModifierKeyFlags(GET_KEYSTATE_WPARAM(p_data_a));

				MouseEvent mouseEvent;
				mouseEvent.x = mousePosition.x;
				mouseEvent.y = mousePosition.y;
				mouseEvent.scrollDelta = delta;
				mouseEvent.modifierKeys = modifierKeyFlags;
				m_GUI->handleMouseScroll(mouseEvent);

				return true;
			}
			case WM_LBUTTONDOWN:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDown(mouseEvent);

				SetCapture(m_windowHandle);

				return true;
			}
			case WM_LBUTTONUP:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseUp(mouseEvent);

				ReleaseCapture();

				return true;
			}
			case WM_LBUTTONDBLCLK:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Left;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDoubleClick(mouseEvent);

				return true;
			}
			case WM_RBUTTONDOWN:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDown(mouseEvent);

				return true;
			}
			case WM_RBUTTONUP:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseUp(mouseEvent);

				return true;
			}
			case WM_RBUTTONDBLCLK:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Right;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDoubleClick(mouseEvent);

				return true;
			}
			case WM_MBUTTONDOWN:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDown(mouseEvent);

				return true;
			}
			case WM_MBUTTONUP:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseUp(mouseEvent);

				return true;
			}
			case WM_MBUTTONDBLCLK:
			{
				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.mouseButton = MouseButton::Middle;
				mouseEvent.modifierKeys = modifierFlags;
				m_GUI->handleMouseDoubleClick(mouseEvent);

				return true;
			}

			case WM_MOUSEMOVE:
			{
				if (m_isMouseOutsideWindow)
				{
					SetCursor(m_cursorHandle);

					TRACKMOUSEEVENT trackStructure = { };
					trackStructure.dwFlags = TME_LEAVE;
					trackStructure.cbSize = sizeof(TRACKMOUSEEVENT);
					trackStructure.hwndTrack = m_windowHandle;
					TrackMouseEvent(&trackStructure);

					m_isMouseOutsideWindow = false;
				}

				ModifierKeyFlags modifierKeys = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				int32_t x = GET_X_LPARAM(p_data_b);
				int32_t y = GET_Y_LPARAM(p_data_b);

				MouseEvent mouseEvent;
				mouseEvent.x = x;
				mouseEvent.y = y;
				mouseEvent.movementX = x - m_mousePosition.x;
				mouseEvent.movementY = y - m_mousePosition.y;
				mouseEvent.modifierKeys = modifierKeys;
				m_GUI->handleMouseMove(mouseEvent);

				m_mousePosition.set(x, y);

				return true;
			}
			case WM_MOUSELEAVE:
			{
				m_isMouseOutsideWindow = true;
				return true;
			}
			case WM_KEYDOWN:
			{
				bool isRepeated = p_data_b & (1 << 30);
				KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				KeyboardEvent keyboardEvent;
				keyboardEvent.key = key;
				keyboardEvent.isRepeated = isRepeated;
				m_GUI->handleKeyboardKeyDown(keyboardEvent);

				return true;
			}
			case WM_KEYUP:
			{
				KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				KeyboardEvent keyboardEvent;
				keyboardEvent.key = key;
				m_GUI->handleKeyboardKeyUp(keyboardEvent);

				return true;
			}
			case WM_CHAR:
			{
				bool isRepeated = p_data_b & (1 << 30);
				char character = p_data_a;

				KeyboardEvent keyboardEvent;
				keyboardEvent.character = character;
				keyboardEvent.isRepeated = isRepeated;
				m_GUI->handleCharacterInput(keyboardEvent);

				return true;
			}
			case WM_DESTROY:
			{
				close();

				WindowEvent windowEvent;
				windowEvent.window = this;
				m_GUI->handleWindowDestroyed(windowEvent);

				return true;
			}
			}
			return false;
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
				window->setWindowHandle(p_windowHandle);
			}
			else
			{
				window = (WindowsWindow*)GetWindowLongPtr(p_windowHandle, GWLP_USERDATA);
			}
			if (window && window->handleEvent(p_message, p_data_a, p_data_b))
			{
				return 0;
			}
			return DefWindowProc(p_windowHandle, p_message, p_data_a, p_data_b);
		}
	};
	uint32_t WindowsWindow::s_numberOfWindows;
	const char* const WindowsWindow::WINDOW_CLASS_NAME = "AvoGUI window class";

#endif
#pragma endregion

	class WindowsImage : public Image
	{
	private:
		ID2D1Bitmap* m_image;

		ImageBoundsSizing m_boundsSizing;
		Point<float> m_boundsPositioning;

		Rectangle<float> m_cropRectangle;
		float m_opacity;

	public:
		WindowsImage(ID2D1Bitmap* p_image) :
			m_image(p_image), m_boundsSizing(ImageBoundsSizing::Stretch), m_boundsPositioning(0.5f, 0.5f),
			m_cropRectangle(0.f, 0.f, p_image->GetSize().width, p_image->GetSize().height),
			m_opacity(1.f)
		{
			m_bounds = m_cropRectangle;
		}
		~WindowsImage()
		{
			m_image->Release();
		}

		//------------------------------

		void setCropRectangle(const Rectangle<float>& p_rectangle) override
		{
			m_cropRectangle = p_rectangle;
		}
		const Rectangle<float>& getCropRectangle() const override
		{
			return m_cropRectangle;
		}

		Point<uint32_t> getOriginalSize() const override
		{
			return Point<uint32_t>(m_image->GetSize().width, m_image->GetSize().height);
		}
		uint32_t getOriginalWidth() const override
		{
			return m_image->GetSize().width;
		}
		uint32_t getOriginalHeight() const override
		{
			return m_image->GetSize().height;
		}

		//------------------------------

		void setBoundsSizing(ImageBoundsSizing p_sizeMode) override
		{
			m_boundsSizing = p_sizeMode;
		}
		ImageBoundsSizing getBoundsSizing() const override
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
		const Point<float>& getBoundsPositioning() const override
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

		void setOpacity(float p_opacity) override
		{
			m_opacity = p_opacity;
		}
		float getOpacity() const override
		{
			return m_opacity;
		}

		//------------------------------

		void* getHandle() const override
		{
			return m_image;
		}
	};

	//------------------------------

	class WindowsText : public Text
	{
	private:
		IDWriteTextLayout1* m_handle;
		std::string m_string;

		DWRITE_TEXT_RANGE createTextRange(int32_t p_startPosition, int32_t p_length)
		{
			DWRITE_TEXT_RANGE textRange = { };
			textRange.startPosition = p_length > 0 ? p_startPosition : max(0, p_startPosition - p_length);
			textRange.length = p_length > 0 ? p_length : (p_length == 0 ? m_string.size() : -p_length);
			return textRange;
		}

	public:
		WindowsText(IDWriteTextLayout1* p_handle, const std::string& p_string) :
			m_handle(p_handle), m_string(p_string)
		{
			m_bounds.right = p_handle->GetMaxWidth();
			m_bounds.bottom = p_handle->GetMaxHeight();
			if (!m_bounds.right && !m_bounds.bottom)
			{
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
				minimizeSize();
			}
			else
			{
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
			}
		}

		//------------------------------

		void setWordWrapping(WordWrapping p_wordWrapping) override
		{
			switch (p_wordWrapping)
			{
			case WordWrapping::Always:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP);
				break;
			case WordWrapping::Emergency:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
				break;
			case WordWrapping::Never:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
				break;
			case WordWrapping::WholeWord:
				m_handle->SetWordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD);
			}
		}
		WordWrapping getWordWrapping() override
		{
			switch (m_handle->GetWordWrapping())
			{
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WRAP:
				return WordWrapping::Always;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_EMERGENCY_BREAK:
				return WordWrapping::Emergency;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP:
				return WordWrapping::Never;
			case DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD:
				return WordWrapping::WholeWord;
			}
			return WordWrapping::Never;
		}

		void minimizeSize() override
		{
			DWRITE_TEXT_METRICS metrics;
			HRESULT result = m_handle->GetMetrics(&metrics);
			m_bounds.setSize(metrics.width, metrics.height);
		}

		//------------------------------

		void setBounds(const Rectangle<float>& p_rectangle) override
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
		void setBounds(const Point<float>& p_position, const Point<float>& p_size) override
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
		const Rectangle<float>& getBounds() const override
		{
			return m_bounds;
		}

		//------------------------------

		void move(const Point<float>& p_offset) override
		{
			m_bounds += p_offset;
		}
		void move(float p_offsetX, float p_offsetY) override
		{
			m_bounds.move(p_offsetX, p_offsetY);
		}

		//------------------------------

		void setTopLeft(const Point<float>& p_position, bool p_willKeepSize = true) override
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
		Point<float> getTopLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.top);
		}

		void setTopRight(const Point<float>& p_position, bool p_willKeepSize = true) override
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
		Point<float> getTopRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.top);
		}

		void setBottomLeft(const Point<float>& p_position, bool p_willKeepSize = true) override
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
		Point<float> getBottomLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.bottom);
		}

		void setBottomRight(const Point<float>& p_position, bool p_willKeepSize = true) override
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
		Point<float> getBottomRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.bottom);
		}

		//------------------------------

		void setCenter(const Point<float>& p_position) override
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
		Point<float> getCenter() const override
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

		void setLeft(float p_left, bool p_willKeepWidth = false) override
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

		void setTop(float p_top, bool p_willKeepHeight = false) override
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

		void setRight(float p_right, bool p_willKeepWidth = false) override
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

		void setBottom(float p_bottom, bool p_willKeepHeight = false) override
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

		void setSize(const Point<float>& p_size) override
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
		Point<float> getSize() const override
		{
			return Point<float>(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		bool getIsIntersecting(const Rectangle<float>& p_rectangle) const override
		{
			return m_bounds.getIsIntersecting(p_rectangle);
		}
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
		{
			return m_bounds.getIsIntersecting(p_protectedRectangle->getBounds());
		}

		//------------------------------

		bool getIsContaining(const Rectangle<float>& p_rectangle) const override
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
		bool getIsContaining(const Point<float>& p_point) const override
		{
			return m_bounds.getIsContaining(p_point);
		}

		//------------------------------

		void setFontFamily(const char* p_name, int32_t p_startPosition, int32_t p_length) override
		{
			wchar_t* wideName = widenString(p_name);

			m_handle->SetFontFamilyName(wideName, createTextRange(p_startPosition, p_length));

			delete[] wideName;
		}

		//------------------------------

		void setCharacterSpacing(float p_leading, float p_trailing, int32_t p_startPosition, int32_t p_length) override
		{
			m_handle->SetCharacterSpacing(p_leading, p_trailing, 0.f, createTextRange(p_startPosition, p_length));
		}
		void setCharacterSpacing(float p_characterSpacing, int32_t p_startPosition, int32_t p_length) override
		{
			m_handle->SetCharacterSpacing(p_characterSpacing*0.5f, p_characterSpacing*0.5f, 0.f, createTextRange(p_startPosition, p_length));
		}
		float getLeadingCharacterSpacing(int32_t p_characterIndex) override
		{
			float leadingSpacing = 0.f;
			m_handle->GetCharacterSpacing(p_characterIndex, &leadingSpacing, 0, 0);
			return leadingSpacing;
		}
		float getTrailingCharacterSpacing(int32_t p_characterIndex) override
		{
			float trailingSpacing = 0.f;
			m_handle->GetCharacterSpacing(p_characterIndex, 0, &trailingSpacing, 0);
			return trailingSpacing;
		}

		//------------------------------

		void setFontWeight(FontWeight p_fontWeight, int32_t p_startPosition, int32_t p_length) override
		{
			m_handle->SetFontWeight((DWRITE_FONT_WEIGHT)p_fontWeight, createTextRange(p_startPosition, p_length));
		}
		FontWeight getFontWeight(uint32_t p_characterPosition) override
		{
			DWRITE_FONT_WEIGHT fontWeight;
			m_handle->GetFontWeight(p_characterPosition, &fontWeight);
			return (FontWeight)fontWeight;
		}

		//------------------------------

		void setFontStyle(FontStyle p_fontStyle, int32_t p_startPosition = 0, int32_t p_length = 0) override
		{
			m_handle->SetFontStyle((DWRITE_FONT_STYLE)p_fontStyle, createTextRange(p_startPosition, p_length));
		}
		FontStyle getFontStyle(uint32_t p_characterPosition) override
		{
			DWRITE_FONT_STYLE directwriteFontStyle;
			m_handle->GetFontStyle(p_characterPosition, &directwriteFontStyle);
			return (FontStyle)directwriteFontStyle;
		}

		//------------------------------

		void setFontStretch(FontStretch p_fontStretch, int32_t p_startPosition = 0, int32_t p_length = 0) override
		{
			m_handle->SetFontStretch((DWRITE_FONT_STRETCH)p_fontStretch, createTextRange(p_startPosition, p_length));
		}
		FontStretch getFontStretch(uint32_t p_characterPosition)
		{
			DWRITE_FONT_STRETCH fontStretch;
			m_handle->GetFontStretch(p_characterPosition, &fontStretch);
			return (FontStretch)fontStretch;
		}

		//------------------------------

		void setFontSize(float p_fontSize, int32_t p_startPosition, int32_t p_length) override
		{
			m_handle->SetFontSize(p_fontSize, createTextRange(p_startPosition, p_length));
		}
		float getFontSize(uint32_t p_characterPosition) override
		{
			float fontSize = 0.f;
			m_handle->GetFontSize(p_characterPosition, &fontSize);
			return fontSize;
		}

		//------------------------------

		const std::string& getString() override
		{
			return m_string;
		}

		//------------------------------

		void* getHandle() override
		{
			return m_handle;
		}
	};

#pragma region Platform-specific drawing context implementations
#ifdef _WIN32
	class FontData
	{
	public:
		const void* data;
		uint32_t dataSize;

		FontData(const void* p_data, uint32_t p_dataSize) :
			data(p_data), dataSize(p_dataSize)
		{ }
	};

	class FontFileStream : public IDWriteFontFileStream
	{
	private:
		FontData* m_fontData;

		//------------------------------

		ULONG m_referenceCount;

	public:
		FontFileStream(FontData* p_fontData) : m_referenceCount(0), m_fontData(p_fontData) { }

		//------------------------------
		// The IUnknown methods...

		ULONG __stdcall AddRef() override
		{
			return InterlockedIncrement(&m_referenceCount);
		}
		ULONG __stdcall Release() override
		{
			InterlockedDecrement(&m_referenceCount);
			if (!m_referenceCount)
			{
				delete this;
			}
			return m_referenceCount;
		}
		HRESULT __stdcall QueryInterface(const IID& p_id, void** p_object) override
		{
			if (p_id == IID_IUnknown || p_id == __uuidof(IDWriteFontFileStream))
			{
				*p_object = this;
				AddRef();
				return S_OK;
			}
			*p_object = 0;
			return E_NOINTERFACE;
		}

		//------------------------------

		HRESULT __stdcall ReadFileFragment(const void** p_fragment, UINT64 p_fileOffset, UINT64 p_fragmentSize, void** p_fragmentContext) override
		{
			if (p_fileOffset + p_fragmentSize > m_fontData->dataSize || !p_fragmentSize)
			{
				*p_fragment = 0;
				*p_fragmentContext = 0;
				return E_FAIL;
			}

			*p_fragment = (const char*)m_fontData->data + p_fileOffset;
			*p_fragmentContext = 0;
			return S_OK;
		}
		void __stdcall ReleaseFileFragment(void* p_fragmentContext) override { }

		HRESULT __stdcall GetFileSize(UINT64* p_fileSize) override
		{
			*p_fileSize = m_fontData->dataSize;
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
		uint32_t m_referenceCount;

	public:
		FontFileLoader() : m_referenceCount(0) { }

		//------------------------------
		// The IUnknown methods...

		ULONG __stdcall AddRef() override
		{
			return InterlockedIncrement(&m_referenceCount);
		}
		ULONG __stdcall Release() override
		{
			InterlockedDecrement(&m_referenceCount);
			if (!m_referenceCount)
			{
				delete this;
			}
			return m_referenceCount;
		}
		HRESULT __stdcall QueryInterface(const IID& p_id, void** p_object)
		{
			if (p_id == IID_IUnknown || p_id == __uuidof(IDWriteFontFileLoader))
			{
				*p_object = this;
				AddRef();
				return S_OK;
			}
			*p_object = 0;
			return E_NOINTERFACE;
		}

		//------------------------------

		HRESULT __stdcall CreateStreamFromKey(const void* p_data, UINT32 p_dataSize, IDWriteFontFileStream** p_stream)
		{
			if (p_dataSize != sizeof(FontData*) || !p_data)
			{
				*p_stream = 0;
				return E_INVALIDARG;
			}
			*p_stream = new FontFileStream(*((FontData**)p_data));
			(*p_stream)->AddRef();
			return S_OK;
		}
	};
	class FontFileEnumerator : public IDWriteFontFileEnumerator
	{
	private:
		uint32_t m_referenceCount;

		//------------------------------

		IDWriteFactory* m_factory;
		FontFileLoader* m_fontFileLoader;

		std::vector<FontData*>* m_fontData;
		IDWriteFontFile* m_currentFontFile;
		int32_t m_currentFontFileIndex;

	public:
		FontFileEnumerator(IDWriteFactory* p_factory, FontFileLoader* p_fontFileLoader, std::vector<FontData*>* p_data) :
			m_referenceCount(0), m_factory(p_factory), m_fontFileLoader(p_fontFileLoader), m_fontData(p_data),
			m_currentFontFileIndex(-1), m_currentFontFile(0)
		{
		}

		//------------------------------
		// The IUnknown methods...

		ULONG __stdcall AddRef() override
		{
			return InterlockedIncrement(&m_referenceCount);
		}
		ULONG __stdcall Release() override
		{
			InterlockedDecrement(&m_referenceCount);
			if (!m_referenceCount)
			{
				delete this;
			}
			return m_referenceCount;
		}
		HRESULT __stdcall QueryInterface(const IID& p_id, void** p_object) override
		{
			if (p_id == IID_IUnknown || p_id == __uuidof(IDWriteFontFileEnumerator))
			{
				*p_object = this;
				AddRef();
				return S_OK;
			}
			*p_object = 0;
			return E_NOINTERFACE;
		}

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
				m_factory->CreateCustomFontFileReference((const void*)&(*m_fontData)[m_currentFontFileIndex], sizeof(FontData*), m_fontFileLoader, &m_currentFontFile);
			}
			return S_OK;
		}
	};
	class FontCollectionLoader : public IDWriteFontCollectionLoader
	{
	private:
		uint32_t m_referenceCount;
		FontFileLoader* m_fontFileLoader;

	public:
		FontCollectionLoader(FontFileLoader* p_fontFileLoader) :
			m_referenceCount(0), m_fontFileLoader(p_fontFileLoader)
		{
		}

		//------------------------------
		// The IUnknown methods...

		ULONG __stdcall AddRef() override
		{
			return InterlockedIncrement(&m_referenceCount);
		}
		ULONG __stdcall Release() override
		{
			InterlockedDecrement(&m_referenceCount);
			if (!m_referenceCount)
			{
				delete this;
			}
			return m_referenceCount;
		}
		HRESULT __stdcall QueryInterface(const IID& p_id, void** p_object) override
		{
			if (p_id == IID_IUnknown || p_id == __uuidof(IDWriteFontCollectionLoader))
			{
				*p_object = this;
				AddRef();
				return S_OK;
			}
			*p_object = 0;
			return E_NOINTERFACE;
		}

		//------------------------------

		HRESULT __stdcall CreateEnumeratorFromKey(IDWriteFactory* p_factory, const void* p_data, UINT32 p_dataSize, IDWriteFontFileEnumerator** p_fontFileEnumerator)
		{
			*p_fontFileEnumerator = new FontFileEnumerator(p_factory, m_fontFileLoader, *((std::vector<FontData*>**)p_data));
			(*p_fontFileEnumerator)->AddRef();
			return S_OK;
		}
	};

	class WindowsDrawingContext : public DrawingContext
	{
	private:
		static ID2D1Factory1* s_direct2DFactory;
		static IDWriteFactory1* s_directWriteFactory;
		static FontCollectionLoader* s_fontCollectionLoader;
		static FontFileLoader* s_fontFileLoader;
		static IWICImagingFactory2* s_imagingFactory;

		Window* m_window;

		ID2D1DeviceContext* m_context;
		IDXGISwapChain1* m_swapChain;
		ID2D1Bitmap1* m_targetWindowBitmap;

		ID2D1SolidColorBrush* m_solidColorBrush;
		D2D1_STROKE_STYLE_PROPERTIES m_strokeStyle;

		IDWriteTextFormat* m_textFormat;
		IDWriteFontCollection* m_fontCollection;
		std::vector<FontData*> m_fontData;

		//------------------------------

		void updateFontCollection()
		{
			if (m_fontCollection)
			{
				m_fontCollection->Release();
			}
			std::vector<FontData*>* fontDataPointer = &m_fontData;
			s_directWriteFactory->CreateCustomFontCollection(s_fontCollectionLoader, &fontDataPointer, sizeof(std::vector<FontData*>*), &m_fontCollection);
		}

	public:
		WindowsDrawingContext(Window* p_window) :
			m_window(p_window), m_textFormat(0), m_fontCollection(0)
		{
			if (!s_imagingFactory)
			{
				CoInitialize(0);
				CoCreateInstance(CLSID_WICImagingFactory2, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&s_imagingFactory));
			}

			//------------------------------

			if (!s_direct2DFactory)
			{
				D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &s_direct2DFactory);
			}

			//------------------------------
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
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				0,
				D3D11_CREATE_DEVICE_BGRA_SUPPORT,
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

			ID2D1Device* direct2DDevice = 0;
			s_direct2DFactory->CreateDevice(dxgiDevice, &direct2DDevice);
			direct2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_context);

			//------------------------------
			// Create swap chain, which holds the back buffer and is connected to the window.

			DXGI_SWAP_CHAIN_DESC1 swapChainDescription = { };
			swapChainDescription.Width = 0; // Automatic width.
			swapChainDescription.Height = 0; // Automatic height.
			swapChainDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			swapChainDescription.Stereo = false;
			swapChainDescription.SampleDesc.Count = 1;
			swapChainDescription.SampleDesc.Quality = 0;
			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescription.BufferCount = 2;
			swapChainDescription.Scaling = DXGI_SCALING_NONE;
			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapChainDescription.Flags = 0;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenSwapChainDescription = { };
			// The documentation says the refresh rate is expressed in hertz, so I guess it's just 60/1 = 60 hertz?
			// Why is this a rational object then? Wouldn't it be more logical to express it in seconds? Then it would be 1/60
			fullscreenSwapChainDescription.RefreshRate.Numerator = 60;
			fullscreenSwapChainDescription.RefreshRate.Denominator = 1;
			fullscreenSwapChainDescription.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
			fullscreenSwapChainDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
			fullscreenSwapChainDescription.Windowed = !p_window->getIsFullscreen();

			dxgiFactory->CreateSwapChainForHwnd(
				d3dDevice, (HWND)p_window->getWindowHandle(),
				&swapChainDescription, &fullscreenSwapChainDescription,
				0, &m_swapChain
			);

			//------------------------------
			// Create a target bitmap which is connected to the back buffer of the window.

			IDXGISurface* dxgiBackBuffer;
			m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

			m_context->CreateBitmapFromDxgiSurface(
				dxgiBackBuffer,
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_IGNORE)
				),
				&m_targetWindowBitmap
			);

			m_context->SetTarget(m_targetWindowBitmap);

			//------------------------------

			m_context->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &m_solidColorBrush);

			//------------------------------

			dxgiBackBuffer->Release();
			dxgiFactory->Release();
			direct2DDevice->Release();
			dxgiAdapter->Release();
			dxgiDevice->Release();
			d3dDeviceContext->Release();
			d3dDevice->Release();

			//------------------------------
			// Create text stuff

			if (!s_directWriteFactory)
			{
				DWriteCreateFactory(
					DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED,
					__uuidof(s_directWriteFactory), (IUnknown**)&s_directWriteFactory
				);

				s_fontFileLoader = new FontFileLoader();
				s_fontFileLoader->AddRef();
				s_directWriteFactory->RegisterFontFileLoader(s_fontFileLoader);

				s_fontCollectionLoader = new FontCollectionLoader(s_fontFileLoader);
				s_fontCollectionLoader->AddRef();
				s_directWriteFactory->RegisterFontCollectionLoader(s_fontCollectionLoader);
			}

			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_LIGHT, FONT_DATA_SIZE_ROBOTO_LIGHT));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_REGULAR, FONT_DATA_SIZE_ROBOTO_REGULAR));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_MEDIUM, FONT_DATA_SIZE_ROBOTO_MEDIUM));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_BOLD, FONT_DATA_SIZE_ROBOTO_BOLD));
			updateFontCollection();

			// Just for debugging...
			//std::vector<wchar_t*> fontFamilyNames;
			//for (uint32_t a = 0; a < m_fontCollection->GetFontFamilyCount(); a++)
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
		}

		~WindowsDrawingContext()
		{
			for (uint32_t a = 0; a < m_fontData.size(); a++)
			{
				delete m_fontData[a];
			}

			m_solidColorBrush->Release();
			m_targetWindowBitmap->Release();
			m_swapChain->Release();
			m_context->Release();

			if (!WindowsWindow::s_numberOfWindows)
			{
				s_directWriteFactory->UnregisterFontCollectionLoader(s_fontCollectionLoader);
				s_fontCollectionLoader->Release();

				s_directWriteFactory->UnregisterFontFileLoader(s_fontFileLoader);
				s_fontFileLoader->Release();

				s_directWriteFactory->Release();

				s_direct2DFactory->Release();

				s_imagingFactory->Release();
			}
		}

		//------------------------------

		void beginDrawing() override
		{
			m_context->BeginDraw();
		}
		void finishDrawing(const Rectangle<float>& p_updatedRectangle) override
		{
			m_context->EndDraw();

			DXGI_PRESENT_PARAMETERS presentParameters = { };
			presentParameters.DirtyRectsCount = 1;

			RECT updatedRectangle;
			updatedRectangle.left = p_updatedRectangle.left;
			updatedRectangle.right = p_updatedRectangle.right;
			updatedRectangle.top = p_updatedRectangle.top;
			updatedRectangle.bottom = p_updatedRectangle.bottom;
			presentParameters.pDirtyRects = &updatedRectangle;

			m_swapChain->Present1(1, 0, &presentParameters);
		}

		//------------------------------

		float convertPixelsToDeviceIndependentPixels(float p_pixels) override
		{
			float DPIX = 0.f;
			float DPIY = 0.f;
			m_context->GetDpi(&DPIX, &DPIY);
			return p_pixels * 96.f / DPIX;
		}
		float convertDeviceIndependentPixelsToPixels(float p_deviceIndependentPixels) override
		{
			float DPIX = 0.f;
			float DPIY = 0.f;
			m_context->GetDpi(&DPIX, &DPIY);
			return p_deviceIndependentPixels * DPIX / 96.f;
		}

		//------------------------------

		void moveOrigin(const Point<float>& p_offset) override
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);
			transform._31 += p_offset.x;
			transform._32 += p_offset.y;
			m_context->SetTransform(transform);
		}
		void moveOrigin(float p_offsetX, float p_offsetY) override
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);
			transform._31 += p_offsetX;
			transform._32 += p_offsetY;
			m_context->SetTransform(transform);
		}
		void setOrigin(const Point<float>& p_origin) override
		{
			m_context->SetTransform(D2D1::Matrix3x2F::Translation(p_origin.x, p_origin.y));
		}
		void setOrigin(float p_x, float p_y) override
		{
			m_context->SetTransform(D2D1::Matrix3x2F::Translation(p_x, p_y));
		}
		Point<float> getOrigin() override
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);
			return Point<float>(transform._31, transform._32);
		}

		//------------------------------

		void setSize(const Point<uint32_t>& p_size) override
		{
			setSize(p_size.x, p_size.y);
		}
		void setSize(uint32_t p_width, uint32_t p_height) override
		{
			// Release the old target bitmap
			m_context->SetTarget(0);
			m_targetWindowBitmap->Release();

			// Resize buffers, creating new ones
			HRESULT result = m_swapChain->ResizeBuffers(0, p_width, p_height, DXGI_FORMAT_UNKNOWN, 0);

			// Get the new back buffer and create new bitmap connected to it
			IDXGISurface* dxgiBackBuffer;
			m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
			m_context->CreateBitmapFromDxgiSurface(
				dxgiBackBuffer,
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_IGNORE)
				),
				&m_targetWindowBitmap
			);
			dxgiBackBuffer->Release();

			m_context->SetTarget(m_targetWindowBitmap);

			return;
		}

		//------------------------------

		void clear(const Color& p_color) override
		{
			m_context->Clear(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
		}
		void clear() override
		{
			m_context->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
		}

		//------------------------------

		void fillRectangle(const Rectangle<float>& p_rectangle) override
		{
			fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		void fillRectangle(const Point<float>& p_position, const Point<float>& p_size) override
		{
			fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) override
		{
			m_context->FillRectangle(
				D2D1::RectF(
					p_left, p_top,
					p_right, p_bottom
				), m_solidColorBrush
			);
		}

		void fillRectangle(const Point<float>& p_size) override
		{
			m_context->FillRectangle(D2D1::RectF(0, 0, p_size.x, p_size.y), m_solidColorBrush);
		}
		void fillRectangle(float p_width, float p_height) override
		{
			m_context->FillRectangle(D2D1::RectF(0, 0, p_width, p_height), m_solidColorBrush);
		}

		//------------------------------

		void strokeRectangle(const Rectangle<float>& p_rectangle, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		void strokeRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_strokeWidth);
		}
		void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) override
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawRectangle(
				D2D1::RectF(
					p_left, p_top, p_right, p_bottom
				), m_solidColorBrush,
				p_strokeWidth, strokeStyle
			);

			strokeStyle->Release();
		}

		void strokeRectangle(const Point<float>& p_size, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_size.x, p_size.y, p_strokeWidth);
		}
		void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) override
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawRectangle(
				D2D1::RectF(0, 0, p_width, p_height),
				m_solidColorBrush, p_strokeWidth, strokeStyle
			);

			strokeStyle->Release();
		}

		//------------------------------

		void fillRoundedRectangle(const Rectangle<float>& p_rectangle, float p_radius) override
		{
			fillRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius);
		}
		void fillRoundedRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_radius) override
		{
			fillRoundedRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius);
		}
		void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) override
		{
			m_context->FillRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(
						p_left, p_top,
						p_right, p_bottom
					), p_radius, p_radius
				), m_solidColorBrush
			);
		}

		void fillRoundedRectangle(const Point<float>& p_size, float p_radius) override
		{
			fillRoundedRectangle(p_size.x, p_size.y, p_radius);
		}
		void fillRoundedRectangle(float p_width, float p_height, float p_radius) override
		{
			m_context->FillRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(0, 0, p_width, p_height),
					p_radius, p_radius
				), m_solidColorBrush
			);
		}

		//------------------------------

		void strokeRoundedRectangle(const Rectangle<float>& p_rectangle, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_strokeWidth);
		}
		void strokeRoundedRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius, p_strokeWidth);
		}
		void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) override
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(
						p_left, p_top,
						p_right, p_bottom
					), p_radius, p_radius
				), m_solidColorBrush,
				p_strokeWidth, strokeStyle
			);

			strokeStyle->Release();
		}

		void strokeRoundedRectangle(const Point<float>& p_size, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(p_size.x, p_size.y, p_radius, p_strokeWidth);
		}
		void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) override
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawRoundedRectangle(
				D2D1::RoundedRect(
					D2D1::RectF(0, 0, p_width, p_height), p_radius, p_radius
				), m_solidColorBrush,
				p_strokeWidth, strokeStyle
			);

			strokeStyle->Release();
		}

		//------------------------------

		void fillCircle(const Point<float>& p_position, float p_radius) override
		{
			fillCircle(p_position.x, p_position.y, p_radius);
		}
		void fillCircle(float p_x, float p_y, float p_radius) override
		{
			m_context->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(p_x, p_y),
					p_radius, p_radius
				), m_solidColorBrush
			);
		}

		void strokeCircle(const Point<float>& p_position, float p_radius, float p_strokeWidth = 1.f)
		{
			strokeCircle(p_position.x, p_position.y, p_radius, p_strokeWidth);
		};
		void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f)
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(p_x, p_y),
					p_radius, p_radius
				), m_solidColorBrush,
				p_strokeWidth, strokeStyle
			);

			strokeStyle->Release();
		}

		//------------------------------

		void drawLine(const Point<float>& p_point_0, const Point<float>& p_point_1, float p_thickness = 1.f) override
		{
			drawLine(p_point_0.x, p_point_0.y, p_point_1.x, p_point_1.y, p_thickness);
		}
		void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) override
		{
			ID2D1StrokeStyle* strokeStyle;
			s_direct2DFactory->CreateStrokeStyle(m_strokeStyle, 0, 0, &strokeStyle);

			m_context->DrawLine(
				D2D1::Point2F(p_x0, p_y0),
				D2D1::Point2F(p_x1, p_y1),
				m_solidColorBrush,
				p_thickness, strokeStyle
			);

			strokeStyle->Release();
		}

		//------------------------------

		void setLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				return;
			case LineCap::Round:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				return;
			case LineCap::Square:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				return;
			case LineCap::Triangle:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
		}
		void setStartLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				return;
			case LineCap::Round:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				return;
			case LineCap::Square:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				return;
			case LineCap::Triangle:
				m_strokeStyle.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
		}
		void setEndLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				return;
			case LineCap::Round:
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				return;
			case LineCap::Square:
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				return;
			case LineCap::Triangle:
				m_strokeStyle.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
		}
		LineCap getStartLineCap() override
		{
			switch (m_strokeStyle.startCap)
			{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return LineCap::Triangle;
			}
			return LineCap::Triangle;
		}
		LineCap getEndLineCap() override
		{
			switch (m_strokeStyle.endCap)
			{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return LineCap::Triangle;
			}
			return LineCap::Triangle;
		}

		//------------------------------

		void setLineJoin(LineJoin p_lineJoin) override
		{
			switch (p_lineJoin)
			{
			case LineJoin::Bevel:
				m_strokeStyle.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_BEVEL;
				break;
			case LineJoin::Miter:
				m_strokeStyle.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER;
				break;
			case LineJoin::Round:
				m_strokeStyle.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND;
				break;
			}
		}
		LineJoin getLineJoin() override
		{
			switch (m_strokeStyle.lineJoin)
			{
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_BEVEL:
				return LineJoin::Bevel;
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER:
				return LineJoin::Miter;
			case D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND:
				return LineJoin::Round;
			}
			return LineJoin::Miter;
		}

		void setLineJoinMiterLimit(float p_miterLimit) override
		{
			m_strokeStyle.miterLimit = p_miterLimit;
		}
		float getLineJoinMiterLimit() override
		{
			return m_strokeStyle.miterLimit;
		}

		//------------------------------

		void setLineDashStyle(LineDashStyle p_dashStyle) override
		{
			switch (p_dashStyle)
			{
			case LineDashStyle::Solid:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID;
				break;
			case LineDashStyle::Dash:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH;
				break;
			case LineDashStyle::Dot:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT;
				break;
			case LineDashStyle::DashDot:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT;
				break;
			case LineDashStyle::DashDotDot:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT_DOT;
				break;
			case LineDashStyle::Custom:
				m_strokeStyle.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_CUSTOM;
			}
		}
		LineDashStyle getLineDashStyle() override
		{
			switch (m_strokeStyle.dashStyle)
			{
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID:
				return LineDashStyle::Solid;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH:
				return LineDashStyle::Dash;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT:
				return LineDashStyle::Dot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT:
				return LineDashStyle::DashDot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT_DOT:
				return LineDashStyle::DashDotDot;
			case D2D1_DASH_STYLE::D2D1_DASH_STYLE_CUSTOM:
				return LineDashStyle::Custom;
			}
		}

		void setLineDashOffset(float p_dashOffset) override
		{
			m_strokeStyle.dashOffset = p_dashOffset;
		}
		float getLineDashOffset() override
		{
			return m_strokeStyle.dashOffset;
		}

		void setLineDashCap(LineCap p_dashCap) override
		{
			switch (p_dashCap)
			{
			case LineCap::Flat:
				m_strokeStyle.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case LineCap::Round:
				m_strokeStyle.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case LineCap::Square:
				m_strokeStyle.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case LineCap::Triangle:
				m_strokeStyle.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
		}
		LineCap getLineDashCap() override
		{
			switch (m_strokeStyle.dashCap)
			{
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT:
				return LineCap::Flat;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND:
				return LineCap::Round;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE:
				return LineCap::Square;
			case D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE:
				return LineCap::Triangle;
			}
		}

		//------------------------------

		void pushClipRectangle(const Rectangle<float>& p_rectangle) override
		{
			m_context->PushAxisAlignedClip(
				D2D1::RectF(
					p_rectangle.left, p_rectangle.top,
					p_rectangle.right, p_rectangle.bottom
				), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
			);
		}
		void pushClipRectangle(const Point<float>& p_size) override
		{
			m_context->PushAxisAlignedClip(
				D2D1::RectF(
					0, 0, p_size.x, p_size.y
				), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
			);
		}
		void popClipRectangle() override
		{
			m_context->PopAxisAlignedClip();
		}

		//------------------------------

		void pushRoundedClipRectangle(const Rectangle<float>& p_rectangle, float p_radius) override
		{
			ID2D1RoundedRectangleGeometry* geometry;
			s_direct2DFactory->CreateRoundedRectangleGeometry(
				D2D1::RoundedRect(
					D2D1::RectF(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom),
					p_radius, p_radius
				), &geometry
			);

			ID2D1Layer* layer;
			m_context->CreateLayer(&layer);
			m_context->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), layer);

			layer->Release();
			geometry->Release();
		}
		void pushRoundedClipRectangle(const Point<float>& p_size, float p_radius) override
		{
			ID2D1RoundedRectangleGeometry* geometry;
			s_direct2DFactory->CreateRoundedRectangleGeometry(
				D2D1::RoundedRect(
					D2D1::RectF(0, 0, p_size.x, p_size.y),
					p_radius, p_radius
				), &geometry
			);

			ID2D1Layer* layer;
			m_context->CreateLayer(&layer);
			m_context->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geometry, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), layer);

			layer->Release();
			geometry->Release();
		}
		void popRoundedClipRectangle() override
		{
			m_context->PopLayer();
		}

		//------------------------------

		Image* createRectangleShadowImage(const Point<uint32_t>& p_size, float p_blur, const Color& p_color) override
		{
			return createRectangleShadowImage(p_size.x, p_size.y, p_blur, p_color);
		}
		Image* createRectangleShadowImage(uint32_t p_width, uint32_t p_height, float p_blur, const Color& p_color) override
		{
			// I am proud of this algorithm I came up with when I was
			// trying to make a GUI library using SDL, so why not use it here

			// Me in the future: because it doesn't look similar enough to shadows made with the shadow effect in direct2d,
			// which is used in the createRoundedRectangleShadowImage() method. I also don't know which method is faster (mine probably hehe)

			// Here is the code anyways, it is pretty fun:

			/*
			if (p_width*p_height == 0) return 0;

			float blurCeiled = std::ceil(p_blur);
			int32_t edgeWidth = (int32_t)blurCeiled << 1;

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

			uint32_t* pixels = new uint32_t[p_width*p_height];
			for (uint32_t a = 0; a < p_width*p_height; a++)
			{
				pixels[a] = p_color;
			}

			int32_t x = 0, y = 0;
			float distance = (p_blur - (int32_t)p_blur) / (p_blur*2.f + 1.f);
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

						distance = ((float)y + p_blur - (int32_t)p_blur) / (p_blur*2.f + 1.f);

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

						distance = ((float)x + p_blur - (int32_t)p_blur) / (p_blur*2.f + 1.f);

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

			ID2D1Bitmap* outputBitmap;
			m_context->CreateBitmap(
				D2D1::SizeU(p_width, p_height),
				pixels, 4*p_width,
				D2D1::BitmapProperties(
					D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED)
				), &outputBitmap
			);

			delete[] pixels;

			return new WindowsImage(outputBitmap);
			*/

			if (!p_width || !p_height || !p_color.alpha) return 0;

			p_blur *= 2.f / 3.f;

			// Create input bitmap
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
			clear(Color(0.f));
			m_context->EndDraw();

			//------------------------------
			// Apply effect

			ID2D1Effect* shadowEffect;
			m_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

			shadowEffect->SetInput(0, inputBitmap);
			shadowEffect->SetValue(
				D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_COLOR,
				D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
			);
			shadowEffect->SetValue(D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur);

			//------------------------------
			// Convert to bitmap

			float dpiX = 0;
			float dpiY = 0;
			m_context->GetDpi(&dpiX, &dpiY);

			ID2D1Bitmap1* outputBitmap;
			D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f*p_blur * dpiX / 96.f, p_height + 6.f*p_blur * dpiY / 96.f);

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
			m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur*3.f*dpiX / 96.f, p_blur*3.f*dpiY / 96.f));
			m_context->EndDraw();
			m_context->SetTarget(m_targetWindowBitmap);

			shadowEffect->Release();
			inputBitmap->Release();

			return new WindowsImage(outputBitmap);
		}

		Image* createRoundedRectangleShadowImage(const Point<uint32_t>& p_size, float p_radius, float p_blur, const Color& p_color)
		{
			return createRoundedRectangleShadowImage(p_size.x, p_size.y, p_radius, p_blur, p_color);
		}
		Image* createRoundedRectangleShadowImage(uint32_t p_width, uint32_t p_height, float p_radius, float p_blur, const Color& p_color)
		{
			if (!p_width || !p_height || !p_color.alpha) return 0;

			p_blur *= 2.f / 3.f;

			// Create input bitmap
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
			clear();
			setColor(Color(0.f));
			fillRoundedRectangle(0, 0, p_width, p_height, p_radius);
			m_context->EndDraw();

			//------------------------------
			// Appy effect

			ID2D1Effect* shadowEffect;
			m_context->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);

			shadowEffect->SetInput(0, inputBitmap);
			shadowEffect->SetValue(
				D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_COLOR,
				D2D1::Vector4F(p_color.red, p_color.green, p_color.blue, p_color.alpha)
			);
			shadowEffect->SetValue(D2D1_SHADOW_PROP::D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, p_blur);

			//------------------------------
			// Convert to bitmap

			float dpiX = 0;
			float dpiY = 0;
			m_context->GetDpi(&dpiX, &dpiY);

			ID2D1Bitmap1* outputBitmap;
			D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f*p_blur * dpiX / 96.f, p_height + 6.f*p_blur * dpiY / 96.f);

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
			m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur*3.f*dpiX / 96.f, p_blur*3.f*dpiY / 96.f));
			m_context->EndDraw();
			m_context->SetTarget(m_targetWindowBitmap);

			shadowEffect->Release();
			inputBitmap->Release();

			return new WindowsImage(outputBitmap);
		}

		//------------------------------

		Image* createImage(const void* p_pixelData, uint32_t p_width, uint32_t p_height) override
		{
			ID2D1Bitmap1* bitmap;
			m_context->CreateBitmap(
				D2D1::Size(p_width, p_height),
				p_pixelData, p_width * 4,
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_NONE,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
				), &bitmap
			);
			return new WindowsImage(bitmap);
		}
		Image* createImage(const char* p_filePath) override
		{
			wchar_t* wideFilePath = widenString(p_filePath);

			IWICBitmapDecoder* decoder = 0;
			s_imagingFactory->CreateDecoderFromFilename(wideFilePath, 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

			delete[] wideFilePath;

			IWICBitmapFrameDecode* frame = 0;
			decoder->GetFrame(0, &frame);

			IWICFormatConverter* formatConverter = 0;
			s_imagingFactory->CreateFormatConverter(&formatConverter);
			formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0.f, WICBitmapPaletteTypeMedianCut);

			ID2D1Bitmap* bitmap;
			m_context->CreateBitmapFromWicBitmap(formatConverter, 0, &bitmap);

			formatConverter->Release();
			frame->Release();
			decoder->Release();

			return new WindowsImage(bitmap);
		}

		//------------------------------

		void drawImage(Image* p_image) override
		{
			const Rectangle<float>& cropRectangle = p_image->getCropRectangle();
			const Point<float>& imageSize = cropRectangle.getSize();
			const Point<float>& boundsSize = p_image->getSize();

			float left = p_image->getLeft();
			float top = p_image->getTop();
			float width = boundsSize.x;
			float height = boundsSize.y;

			if (p_image->getBoundsSizing() != ImageBoundsSizing::Stretch)
			{
				if (p_image->getBoundsSizing() == ImageBoundsSizing::Fill)
				{
					if (boundsSize.x / boundsSize.y > imageSize.x / imageSize.y)
					{
						height = imageSize.y * width / imageSize.x;
					}
					else
					{
						width = imageSize.x * height / imageSize.y;
					}
				}
				else if (p_image->getBoundsSizing() == ImageBoundsSizing::Contain)
				{
					if (boundsSize.x / boundsSize.y > imageSize.x / imageSize.y)
					{
						width = imageSize.x * height / imageSize.y;
					}
					else
					{
						height = imageSize.y * width / imageSize.x;
					}
				}

				left += p_image->getBoundsPositioningX()*(boundsSize.x - width);
				top += p_image->getBoundsPositioningY()*(boundsSize.y - height);
			}

			m_context->DrawBitmap(
				(ID2D1Bitmap*)p_image->getHandle(),
				D2D1::RectF(left, top, left + width, top + height),
				p_image->getOpacity(),
				D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(cropRectangle.left, cropRectangle.top, cropRectangle.right, cropRectangle.bottom)
			);
		}

		//------------------------------

		void setColor(const Color& p_color) override
		{
			m_solidColorBrush->SetColor(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
		}

		//------------------------------

		void addFont(const void* p_data, uint32_t p_dataSize)
		{
			m_fontData.push_back(new FontData(p_data, p_dataSize));
			updateFontCollection();
		}

		//------------------------------

		void setDefaultTextProperties(const TextProperties& p_textProperties) override
		{
			wchar_t* fontFamily = widenString(p_textProperties.fontFamilyName);
			wchar_t* fontLocale = widenString(p_textProperties.fontLocaleName);

			DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
			if (p_textProperties.fontStyle == FontStyle::Italic)
			{
				fontStyle = DWRITE_FONT_STYLE_ITALIC;
			}
			else if (p_textProperties.fontStyle == FontStyle::Oblique)
			{
				fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
			}

			s_directWriteFactory->CreateTextFormat(
				fontFamily, m_fontCollection, (DWRITE_FONT_WEIGHT)p_textProperties.fontWeight,
				fontStyle, (DWRITE_FONT_STRETCH)p_textProperties.fontStretch,
				p_textProperties.fontSize, fontLocale,
				(IDWriteTextFormat**)&m_textFormat
			);

			delete[] fontLocale;
			delete[] fontFamily;
		}
		TextProperties getDefaultTextProperties() override
		{
			return m_textProperties;
		}

		//------------------------------

		Text* createText(const char* p_string, float p_fontSize, const Rectangle<float>& p_bounds = Rectangle<float>()) override
		{
			int32_t numberOfCharacters = MultiByteToWideChar(CP_ACP, 0, p_string, -1, 0, 0);
			wchar_t* wideString = new wchar_t[numberOfCharacters];
			MultiByteToWideChar(CP_ACP, 0, p_string, -1, wideString, numberOfCharacters);

			IDWriteTextLayout1* textLayout;
			s_directWriteFactory->CreateTextLayout(wideString, numberOfCharacters, m_textFormat, p_bounds.getWidth(), p_bounds.getHeight(), (IDWriteTextLayout**)&textLayout);
			DWRITE_TEXT_RANGE textRange;
			textRange.startPosition = 0;
			textRange.length = numberOfCharacters;
			textLayout->SetFontSize(p_fontSize, textRange);

			delete[] wideString;

			return new WindowsText(textLayout, p_string);
		}
		void drawText(Text* p_text) override
		{
			m_context->DrawTextLayout(D2D1::Point2F(p_text->getTopLeft().x, p_text->getTopLeft().y), (IDWriteTextLayout*)p_text->getHandle(), m_solidColorBrush);
		}
		void drawText(const char* p_string, const Rectangle<float>& p_rectangle) override
		{
			int32_t numberOfCharacters = MultiByteToWideChar(CP_ACP, 0, p_string, -1, 0, 0);
			wchar_t* wideString = new wchar_t[numberOfCharacters];
			MultiByteToWideChar(CP_ACP, 0, p_string, -1, wideString, numberOfCharacters);

			m_context->DrawTextA(
				wideString, numberOfCharacters, m_textFormat,
				D2D1::RectF(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom),
				m_solidColorBrush
			);

			delete[] wideString;
		}
		void drawText(const char* p_string, float p_left, float p_top, float p_right, float p_bottom) override
		{
			drawText(p_string, Rectangle<float>(p_left, p_top, p_right, p_bottom));
		}
		void drawText(const char* p_string, const Point<float>& p_position, const Point<float>& p_size) override
		{
			drawText(p_string, Rectangle<float>(p_position, p_size));
		}
		void drawText(const char* p_string, float p_x, float p_y) override
		{
			drawText(p_string, Rectangle<float>(p_x, p_y, p_x, p_y));
		}
		void drawText(const char* p_string, const Point<float>& p_position) override
		{
			drawText(p_string, Rectangle<float>(p_position.x, p_position.y, p_position.x, p_position.y));
		}
	};
	ID2D1Factory1* WindowsDrawingContext::s_direct2DFactory = 0;
	IDWriteFactory1* WindowsDrawingContext::s_directWriteFactory = 0;
	FontCollectionLoader* WindowsDrawingContext::s_fontCollectionLoader = 0;
	FontFileLoader* WindowsDrawingContext::s_fontFileLoader = 0;
	IWICImagingFactory2* WindowsDrawingContext::s_imagingFactory = 0;

#endif
#pragma endregion

	//------------------------------
	// class GUI
	//------------------------------

	//
	// Private
	//

	std::vector<MouseEventListener*> GUI::getTopMouseListenersAt(const Point<float>& p_coordinates)
	{
		std::vector<MouseEventListener*> results;

		View* currentContainer = this;
		uint32_t startPosition = getNumberOfViews() - 1U;

		Point<float> viewOffset;

		bool willContinue = true;
		while (willContinue)
		{
			for (int32_t a = startPosition; a >= -1; a--)
			{
				if (a == -1)
				{
					MouseEventListener* mouseEventListener = dynamic_cast<MouseEventListener*>(currentContainer);
					if (mouseEventListener)
					{
						results.push_back(mouseEventListener);
						if (mouseEventListener->getIsOverlay())
						{
							if (currentContainer->getParent() == this)
							{
								willContinue = false;
							}
							else
							{
								startPosition = currentContainer->getIndex() - 1;
								currentContainer = currentContainer->getParent();
								viewOffset -= currentContainer->getTopLeft();
							}
						}
						else
						{
							willContinue = false;
						}
					}
					else
					{
						willContinue = false;
					}
				}
				else
				{
					View* view = currentContainer->getView(a);
					if (view->getIsContaining(p_coordinates - viewOffset))
					{
						if (view->getNumberOfViews())
						{
							currentContainer = view;
							startPosition = currentContainer->getNumberOfViews() - 1;
							viewOffset += currentContainer->getTopLeft();
							break;
						}
						else
						{
							MouseEventListener* mouseEventListener = dynamic_cast<MouseEventListener*>(view);
							if (mouseEventListener)
							{
								results.push_back(mouseEventListener);
								if (!mouseEventListener->getIsOverlay())
								{
									willContinue = false;
									break;
								}
							}
							else
							{
								willContinue = false;
								break;
							}
						}
					}
				}
			}
		}
		return results;
	}
	std::vector<MouseEventListener*> GUI::getTopMouseListenersAt(float p_x, float p_y)
	{
		return getTopMouseListenersAt(Point<float>(p_x, p_y));
	}

	//
	// Public
	//

	GUI::GUI() :
		View(0, Rectangle<float>(0, 0, 0, 0)), m_drawingContext(0),
		m_keyboardFocus(0),
		m_areIndirectKeyboardEventsEnabled(false), m_areIndirectMouseEventsEnabled(false)
	{
#ifdef _WIN32
		m_window = new WindowsWindow(this);
#endif

		m_GUI = this;
		m_theme = new Theme();

		//------------------------------

		m_windowEventListeners.reserve(5);
		m_mouseEventListeners.reserve(20);
		m_keyboardEventListeners.reserve(20);
	}
	GUI::~GUI()
	{
		if (m_window)
		{
			m_window->forget();
		}
		if (m_drawingContext)
		{
			m_drawingContext->forget();
		}
	}

	void GUI::create(const char* p_title, uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_windowFlags, bool p_isFullscreen, GUI* p_parent)
	{
		m_bounds = Rectangle<float>(0, 0, p_width, p_height);
		m_window->create(p_title, p_width, p_height, p_windowFlags, p_isFullscreen, p_parent ? p_parent->getWindow() : 0);
	}
	void GUI::create(const char* p_title, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_windowFlags, bool p_isFullscreen, GUI* p_parent)
	{
		m_bounds = Rectangle<float>(0, 0, p_width, p_height);
		m_window->create(p_title, p_width, p_height, p_windowFlags, p_isFullscreen, p_parent ? p_parent->getWindow() : 0);
	}

	//------------------------------

	View* GUI::getViewAt(const Point<float>& p_coordinates)
	{
		View* currentContainer = this;

		View* result = 0;
		while (!result)
		{
			for (int32_t a = currentContainer->getNumberOfViews() - 1; a >= 0; a--)
			{
				View* view = currentContainer->getView(a);
				if (view->getIsContaining(p_coordinates))
				{
					if (view->getNumberOfViews())
					{
						currentContainer = view;
						break;
					}
					else
					{
						result = view;
						break;
					}
				}
				else if (a == 0)
				{
					result = currentContainer;
					break;
				}
			}
		}
		return result;
	}
	View* GUI::getViewAt(float p_x, float p_y)
	{
		return getViewAt(Point<float>(p_x, p_y));
	}

	//------------------------------

	void GUI::handleWindowCreated(const WindowEvent& p_event)
	{
#ifdef _WIN32
		if (m_drawingContext)
		{
			m_drawingContext->forget();
		}
		m_drawingContext = new WindowsDrawingContext(m_window);
#endif
		createContent();
	}
	void GUI::handleWindowSizeChanged(const WindowEvent& p_event)
	{
		m_drawingContext->setSize(p_event.width, p_event.height);
		setSize(p_event.width, p_event.height);
		invalidateRect(getBounds().createCopyAtOrigin());
	}

	//------------------------------

	void GUI::handleMouseDown(const MouseEvent& p_event)
	{
		std::vector<MouseEventListener*> targets = getTopMouseListenersAt(p_event.x, p_event.y);

		m_pressedMouseEventListeners.clear();

		MouseEvent event = p_event;
		if (m_areIndirectMouseEventsEnabled)
		{
			for (uint32_t a = 0; a < m_mouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_mouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				event.isTarget = false;
				for (uint32_t b = 0; b < targets.size(); b++)
				{
					if (targets[b] == m_mouseEventListeners[a])
					{
						event.isTarget = true;

						m_pressedMouseEventListeners.push_back(targets[b]);

						break;
					}
				}
				m_mouseEventListeners[a]->handleMouseDown(event);
			}
		}
		else
		{
			event.isTarget = true;
			for (uint32_t a = 0; a < targets.size(); a++)
			{
				View* view = dynamic_cast<View*>(targets[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				targets[a]->handleMouseDown(event);
				m_pressedMouseEventListeners.push_back(targets[a]);
			}
		}
	}
	void GUI::handleMouseUp(const MouseEvent& p_event)
	{
		MouseEvent event = p_event;
		if (m_areIndirectMouseEventsEnabled)
		{
			for (uint32_t a = 0; a < m_mouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_mouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				event.isTarget = false;
				for (uint32_t b = 0; b < m_pressedMouseEventListeners.size(); b++)
				{
					if (m_mouseEventListeners[a] == m_pressedMouseEventListeners[b])
					{
						event.isTarget = true;
						break;
					}
				}
				m_mouseEventListeners[a]->handleMouseUp(event);
			}
		}
		else
		{
			event.isTarget = true;
			for (uint32_t a = 0; a < m_pressedMouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_pressedMouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				m_pressedMouseEventListeners[a]->handleMouseUp(event);
			}
		}
	}
	void GUI::handleMouseDoubleClick(const MouseEvent& p_event)
	{
		std::vector<MouseEventListener*> targets = getTopMouseListenersAt(p_event.x, p_event.y);

		MouseEvent event = p_event;
		if (m_areIndirectMouseEventsEnabled)
		{
			for (uint32_t a = 0; a < m_mouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_mouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				event.isTarget = false;
				for (uint32_t b = 0; b < targets.size(); b++)
				{
					if (targets[b] == m_mouseEventListeners[a])
					{
						event.isTarget = true;
						break;
					}
				}
				m_mouseEventListeners[a]->handleMouseDoubleClick(event);
			}
		}
		else
		{
			event.isTarget = true;
			for (uint32_t a = 0; a < targets.size(); a++)
			{
				View* view = dynamic_cast<View*>(targets[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}
				targets[a]->handleMouseDoubleClick(event);
			}
		}
	}

	void GUI::handleMouseMove(const MouseEvent& p_event)
	{
		// These vectors usually don't have more than 1 or 2 elements in them, so the loops aren't as slow as it looks like.

		std::vector<MouseEventListener*> oldTargets = getTopMouseListenersAt(p_event.x - p_event.movementX, p_event.y - p_event.movementY);
		std::vector<MouseEventListener*> newTargets = getTopMouseListenersAt(p_event.x, p_event.y);

		if (!newTargets.size() && oldTargets.size())
		{
			m_window->setCursor(Cursor::Arrow);
		}

		MouseEvent mouseEvent = p_event;

		if (m_areIndirectMouseEventsEnabled)
		{
			for (uint32_t a = 0; a < m_mouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_mouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					mouseEvent.x = p_event.x - position.x;
					mouseEvent.y = p_event.y - position.y;
				}

				bool isOldTarget = false;
				for (uint32_t b = 0; b < oldTargets.size(); b++)
				{
					if (m_mouseEventListeners[a] == oldTargets[b])
					{
						isOldTarget = true;
						break;
					}
				}
				bool isNewTarget = false;
				for (uint32_t b = 0; b < newTargets.size(); b++)
				{
					if (m_mouseEventListeners[a] == newTargets[b])
					{
						isNewTarget = true;
						break;
					}
				}
				if (isOldTarget && isNewTarget)
				{
					mouseEvent.isTarget = true;
					m_mouseEventListeners[a]->handleMouseMove(mouseEvent);
				}
				else if (isOldTarget)
				{
					mouseEvent.isTarget = true;
					m_mouseEventListeners[a]->handleMouseLeave(mouseEvent);
				}
				else if (isNewTarget)
				{
					mouseEvent.isTarget = true;
					m_mouseEventListeners[a]->handleMouseEnter(mouseEvent);
				}
				else
				{
					mouseEvent.isTarget = false;
					m_mouseEventListeners[a]->handleMouseMove(mouseEvent);
				}
			}
		}
		else
		{
			mouseEvent.isTarget = true;
			std::vector<bool> isNewTargetOldTarget(newTargets.size());
			for (uint32_t a = 0; a < oldTargets.size(); a++)
			{
				bool hasLeftTarget = true;
				for (uint32_t b = 0; b < newTargets.size(); b++)
				{
					if (oldTargets[a] == newTargets[b])
					{
						View* view = dynamic_cast<View*>(newTargets[b]);
						if (view)
						{
							Point<float> position = view->calculateAbsoluteTopLeft();
							mouseEvent.x = p_event.x - position.x;
							mouseEvent.y = p_event.y - position.y;
						}
						else {
							mouseEvent.x = p_event.x;
							mouseEvent.y = p_event.y;
						}
						newTargets[b]->handleMouseMove(mouseEvent);
						isNewTargetOldTarget[b] = true;
						hasLeftTarget = false;
						break;
					}
				}
				if (hasLeftTarget)
				{
					View* view = dynamic_cast<View*>(oldTargets[a]);
					if (view)
					{
						Point<float> position = view->calculateAbsoluteTopLeft();
						mouseEvent.x = p_event.x - position.x;
						mouseEvent.y = p_event.y - position.y;
					}
					else
					{
						mouseEvent.x = p_event.x;
						mouseEvent.y = p_event.y;
					}
					oldTargets[a]->handleMouseLeave(mouseEvent);
				}
			}
			for (uint32_t a = 0; a < newTargets.size(); a++)
			{
				if (!isNewTargetOldTarget[a])
				{
					View* view = dynamic_cast<View*>(newTargets[a]);
					if (view)
					{
						Point<float> position = view->calculateAbsoluteTopLeft();
						mouseEvent.x = p_event.x - position.x;
						mouseEvent.y = p_event.y - position.y;
					}
					else
					{
						mouseEvent.x = p_event.x;
						mouseEvent.y = p_event.y;
					}
					newTargets[a]->handleMouseEnter(mouseEvent);
				}
			}
		}
	}
	void GUI::handleMouseScroll(const MouseEvent& p_event)
	{
		std::vector<MouseEventListener*> targets = getTopMouseListenersAt(p_event.x, p_event.y);

		MouseEvent event = p_event;
		if (m_areIndirectMouseEventsEnabled)
		{
			for (uint32_t a = 0; a < m_mouseEventListeners.size(); a++)
			{
				View* view = dynamic_cast<View*>(m_mouseEventListeners[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}

				event.isTarget = false;
				for (uint32_t b = 0; b < targets.size(); b++)
				{
					if (targets[b] == m_mouseEventListeners[a])
					{
						event.isTarget = true;
						break;
					}
				}
				m_mouseEventListeners[a]->handleMouseScroll(event);
			}
		}
		else
		{
			event.isTarget = true;
			for (uint32_t a = 0; a < targets.size(); a++)
			{
				View* view = dynamic_cast<View*>(targets[a]);
				if (view)
				{
					Point<float> position = view->calculateAbsoluteTopLeft();
					event.x = p_event.x - position.x;
					event.y = p_event.y - position.y;
				}
				targets[a]->handleMouseScroll(event);
			}
		}
	}

	//------------------------------

	void GUI::handleCharacterInput(const KeyboardEvent& p_event)
	{
		KeyboardEvent event = p_event;
		for (uint32_t a = 0; a < m_keyboardEventListeners.size(); a++)
		{
			event.isTarget = m_keyboardEventListeners[a] == m_keyboardFocus;
			m_keyboardEventListeners[a]->handleCharacterInput(p_event);
		}
	}
	void GUI::handleKeyboardKeyDown(const KeyboardEvent& p_event)
	{
		KeyboardEvent event = p_event;
		for (uint32_t a = 0; a < m_keyboardEventListeners.size(); a++)
		{
			event.isTarget = m_keyboardEventListeners[a] == m_keyboardFocus;
			m_keyboardEventListeners[a]->handleKeyboardKeyDown(event);
		}
	}
	void GUI::handleKeyboardKeyUp(const KeyboardEvent& p_event)
	{
		KeyboardEvent event = p_event;
		for (uint32_t a = 0; a < m_keyboardEventListeners.size(); a++)
		{
			event.isTarget = m_keyboardEventListeners[a] == m_keyboardFocus;
			m_keyboardEventListeners[a]->handleKeyboardKeyUp(event);
		}
	}

	//------------------------------

	void GUI::addWindowEventListener(WindowEventListener* p_listener)
	{
		m_windowEventListeners.push_back(p_listener);
	}
	void GUI::addKeyboardEventListener(KeyboardEventListener* p_listener)
	{
		m_keyboardEventListeners.push_back(p_listener);
	}
	void GUI::addMouseEventListener(MouseEventListener* p_listener)
	{
		m_mouseEventListeners.push_back(p_listener);
	}

	//------------------------------

	void GUI::invalidateRect(const Rectangle<float>& p_rectangle)
	{
		m_window->invalidateRect(p_rectangle);
	}

	void GUI::draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle)
	{
		View* currentContainer = this;
		uint32_t startPosition = 0;

		Rectangle<float> movedTargetRectangle(p_targetRectangle);

		m_drawingContext->beginDrawing();

		m_drawingContext->setOrigin(0, 0);
		m_drawingContext->pushClipRectangle(p_targetRectangle);
		m_drawingContext->clear(m_theme->colors["background"]);

		while (true)
		{
			bool isDoneWithContainer = true;
			for (uint32_t a = startPosition; a < currentContainer->getNumberOfViews(); a++)
			{
				View* view = currentContainer->getView(a);

				if (view->getIsIntersecting(movedTargetRectangle) && view->getIsVisible())
				{
					m_drawingContext->moveOrigin(view->getTopLeft());
					movedTargetRectangle -= view->getTopLeft();

					view->drawShadow(m_drawingContext);

					if (view->getCornerRadius())
					{
						m_drawingContext->pushRoundedClipRectangle(view->getSize(), view->getCornerRadius());
					}
					else
					{
						m_drawingContext->pushClipRectangle(view->getSize());
					}

					view->draw(m_drawingContext, movedTargetRectangle);

					if (view->getNumberOfViews())
					{
						currentContainer = view;
						startPosition = 0;
						isDoneWithContainer = false;
						break;
					}
					else
					{
						if (view->getCornerRadius())
						{
							m_drawingContext->popRoundedClipRectangle();
						}
						else
						{
							m_drawingContext->popClipRectangle();
						}

						view->drawUnclipped(m_drawingContext, movedTargetRectangle);

						m_drawingContext->moveOrigin(-view->getTopLeft());
						movedTargetRectangle += view->getTopLeft();
					}
				}
				else if (view->getShadowBounds().getIsIntersecting(movedTargetRectangle))
				{
					m_drawingContext->moveOrigin(view->getTopLeft());
					view->drawShadow(m_drawingContext);
					m_drawingContext->moveOrigin(-view->getTopLeft());
				}
			}
			if (isDoneWithContainer)
			{
				if (currentContainer == this)
				{
					break;
				}

				if (currentContainer->getCornerRadius())
				{
					m_drawingContext->popRoundedClipRectangle();
				}
				else
				{
					m_drawingContext->popClipRectangle();
				}

				currentContainer->drawUnclipped(m_drawingContext, movedTargetRectangle);

				m_drawingContext->moveOrigin(-currentContainer->getTopLeft());
				movedTargetRectangle += currentContainer->getTopLeft();

				startPosition = currentContainer->getIndex() + 1;
				currentContainer = currentContainer->getParent();
			}
		}

		m_drawingContext->popClipRectangle();

		m_drawingContext->finishDrawing(p_targetRectangle);
	}

	//------------------------------
	// Static

	void GUI::run()
	{
#ifdef _WIN32
		MSG message = { };
		while (GetMessage(&message, 0, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
#endif
	}

	//------------------------------
	// class Ripple
	//------------------------------

	Ripple::Ripple(View* p_parent, const Color& p_color) :
		View(p_parent, p_parent->getBounds().createCopyAtOrigin()), m_color(0.f, 0.45f),
		m_isEnabled(true), m_isMouseDown(false), m_isMouseHovering(false), m_hasHoverEffect(true)
	{
		setIsOverlay(true); // Mouse events should be sent through
		setHasShadow(false);
		setElevation(FLT_MAX); // Nothing can be above a ripple...
		getGUI()->addMouseEventListener(this);
		p_parent->addEventListener(this);
	}
	Ripple::~Ripple()
	{
	}

	//------------------------------

	void Ripple::handleViewSizeChange(View* p_view)
	{
		setSize(p_view->getSize());
		m_maxSize = 2.f*Point<>::getDistanceFast(m_position, Point<float>(m_position.x < getWidth()*0.5 ? getWidth() : 0, m_position.y < getHeight()*0.5 ? getHeight() : 0));
	}

	void Ripple::handleMouseDown(const MouseEvent& p_event)
	{
		if (m_isEnabled)
		{
			m_position.set(p_event.x - getLeft(), p_event.y - getTop());
			m_circleAnimationTime = 0.f;
			m_alphaFactor = 1.f;
			m_isMouseDown = true;

			m_maxSize = 2.f*Point<>::getDistanceFast(m_position, Point<float>(m_position.x < getWidth()*0.5 ? getWidth() : 0, m_position.y < getHeight()*0.5 ? getHeight() : 0));

			queueAnimationUpdate();
		}
	}
	void Ripple::handleMouseUp(const MouseEvent& p_event)
	{
		if (m_isMouseDown)
		{
			m_isMouseDown = false;
			m_alphaAnimationTime = 0.f;
			queueAnimationUpdate();
		}
	}
	void Ripple::handleMouseEnter(const MouseEvent& p_event)
	{
		if (m_isEnabled)
		{
			getGUI()->getWindow()->setCursor(Cursor::Hand);
			m_isMouseHovering = true;
			queueAnimationUpdate();
		}
	}
	void Ripple::handleMouseLeave(const MouseEvent& p_event)
	{
		if (m_isMouseHovering)
		{
			m_isMouseHovering = false;
			queueAnimationUpdate();
		}
	}

	void Ripple::updateAnimations()
	{
		if (m_hasHoverEffect)
		{
			m_overlayAlphaFactor = m_theme->easings["symmetrical in out"].easeValue(m_overlayAnimationTime);

			if (m_isMouseHovering)
			{
				if (m_overlayAlphaFactor < 1.f)
				{
					m_overlayAnimationTime = min(m_overlayAnimationTime + 0.21f, 1.f);
					queueAnimationUpdate();
				}
			}
			else if (m_overlayAlphaFactor > 0.f)
			{
				m_overlayAnimationTime = max(m_overlayAnimationTime - 0.21f, 0.f);
				queueAnimationUpdate();
			}
		}

		float circleAnimationValue = 1.f;
		if (m_circleAnimationTime < 1.f)
		{
			circleAnimationValue = m_theme->easings["ripple"].easeValue(m_circleAnimationTime);
			m_circleAnimationTime += 0.07f;
			m_size = interpolate(m_maxSize * 0.4f, m_maxSize, circleAnimationValue);
		}

		if (m_isMouseDown)
		{
			if (circleAnimationValue < 1.f)
			{
				queueAnimationUpdate();
			}
		}
		else if (circleAnimationValue >= 1.f)
		{
			if (m_alphaAnimationTime < 1.f)
			{
				m_alphaFactor = 1.f - m_theme->easings["symmetrical in out"].easeValue(m_alphaAnimationTime);
				m_alphaAnimationTime = min(1.f, m_alphaAnimationTime + 0.06f);

				queueAnimationUpdate();
			}
		}
		else
		{
			queueAnimationUpdate();
		}

		invalidate();
	}

	//------------------------------

	void Ripple::draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle)
	{
		if (m_isEnabled)
		{
			p_drawingContext->setColor(Color(m_color, m_color.alpha*m_overlayAlphaFactor*0.4f));
			p_drawingContext->fillRectangle(getSize());

			if (m_color.alpha*m_alphaFactor >= 0.f)
			{
				p_drawingContext->setColor(Color(m_color, m_color.alpha*m_alphaFactor));
				p_drawingContext->fillCircle(m_position, m_size*0.5f);
			}
		}
	}

	//------------------------------
	// class Button
	//------------------------------

	Button::Button(View* p_parent, const char* p_text, Emphasis p_emphasis, float p_x, float p_y) :
		View(p_parent, Rectangle<float>(p_x, p_y, p_x, p_y)), m_text(0), m_fontSize(14.f), m_icon(0),
		m_pressAnimationTime(1.f), m_isPressed(false), m_emphasis(p_emphasis), m_isEnabled(true), m_colorAnimationTime(1.f),
		m_isMouseHovering(false)
	{
		setText(p_text);

		setCornerRadius(4.f);

		m_ripple = new Ripple(this);

		if (p_emphasis == Emphasis::High)
		{
			setElevation(2.f);
			m_ripple->setColor(Color(m_theme->colors["on primary"], 0.3f));
			m_currentColor = m_theme->colors["primary"];
		}
		else
		{
			m_ripple->setColor(Color(m_theme->colors["primary on background"], 0.3f));
			m_currentColor = m_theme->colors["primary on background"];
		}

		m_GUI->addMouseEventListener(this);
	}
	Button::~Button()
	{
		m_text->forget();
	}

	//------------------------------

	void Button::addButtonListener(ButtonListener* p_buttonListener)
	{
		m_buttonListeners.push_back(p_buttonListener);
	}

	//------------------------------

	void Button::disable()
	{
		if (m_isEnabled)
		{
			m_isEnabled = false;
			m_colorAnimationTime = 1.f;
			queueAnimationUpdate();

			m_ripple->disable();

			if (m_isMouseHovering)
			{
				getGUI()->getWindow()->setCursor(Cursor::Arrow);
			}
		}
	}
	void Button::enable()
	{
		if (!m_isEnabled)
		{
			m_isEnabled = true;
			m_colorAnimationTime = 0.f;
			queueAnimationUpdate();

			m_ripple->enable();

			if (m_isMouseHovering)
			{
				getGUI()->getWindow()->setCursor(Cursor::Hand);
			}
		}
	}

	//------------------------------

	void Button::setText(const char* p_text)
	{
		if (m_text)
		{
			m_text->forget();
		}

		m_text = getGUI()->getDrawingContext()->createText(p_text, m_fontSize);
		m_text->setWordWrapping(WordWrapping::Never);
		m_text->setCharacterSpacing(1.2f);
		m_text->setFontWeight(FontWeight::Medium);
		m_text->minimizeSize();

		if (m_text->getWidth() >= 32.f)
		{
			setSize(round(m_text->getWidth()) + 32.f, round(m_text->getHeight()) + 17.f);
		}
		else
		{
			setSize(64.f, round(m_text->getHeight()) + 17.f);
		}
		m_text->setCenter(getWidth()*0.5f, getHeight()*0.5f);
	}
	const char* Button::getText()
	{
		return m_text->getString().c_str();
	}

	//------------------------------

	void Button::setIcon(Image* p_icon)
	{
		if (p_icon != m_icon)
		{
			if (p_icon)
			{
				if (!m_icon)
				{
					m_icon = p_icon;
					m_text->setLeft(38.f, true);
					setWidth(round(m_text->getWidth()) + 16.f + 38.f);
				}

			}
			invalidate();
		}
	}

	//------------------------------

	void Button::handleMouseDown(const MouseEvent& p_event)
	{
		if (m_isEnabled && m_emphasis == Emphasis::High)
		{
			m_isPressed = true;
			m_isRaising = true;
			m_pressAnimationTime = 0.f;
			queueAnimationUpdate();
		}
	}
	void Button::handleMouseUp(const MouseEvent& p_event)
	{
		if (m_emphasis == Emphasis::High)
		{
			m_isPressed = false;
			queueAnimationUpdate();
		}
		if (m_isEnabled)
		{
			for (uint32_t a = 0; a < m_buttonListeners.size(); a++)
			{
				m_buttonListeners[a]->handleButtonClick(this);
			}
		}
	}

	//------------------------------

	void Button::updateAnimations()
	{
		if ((m_colorAnimationTime != 1.f && m_isEnabled) || (m_colorAnimationTime != 0.f && !m_isEnabled))
		{
			float colorAnimationValue = m_theme->easings["symmetrical in out"].easeValue(m_colorAnimationTime);
			if (m_emphasis == Emphasis::High)
			{
				m_currentColor = m_theme->colors["primary"];
			}
			else
			{
				m_currentColor = m_theme->colors["primary on background"];
			}
			m_currentColor.setSaturationHSL(colorAnimationValue);

			if (m_isEnabled)
			{
				if (m_colorAnimationTime < 1.f)
				{
					m_colorAnimationTime = min(1.f, m_colorAnimationTime + 0.1f);
					queueAnimationUpdate();
				}
			}
			else
			{
				if (m_colorAnimationTime > 0.f)
				{
					m_colorAnimationTime = max(0.f, m_colorAnimationTime - 0.1f);
					queueAnimationUpdate();
				}
			}
		}

		if (m_emphasis == Emphasis::High)
		{
			float pressAnimationValue = m_theme->easings["in out"].easeValue(m_pressAnimationTime);
			m_pressAnimationTime += 0.08f;

			if (m_isRaising || m_isPressed)
			{
				setElevation(2.f + pressAnimationValue * 4.f);
				if (!m_isPressed && pressAnimationValue == 1.f)
				{
					m_pressAnimationTime = 0.f;
					m_isRaising = false;
					queueAnimationUpdate();
				}
			}
			else
			{
				setElevation(2.f + (1.f - pressAnimationValue) * 4.f);
			}

			if (pressAnimationValue < 1.f)
			{
				queueAnimationUpdate();
			}
		}

		invalidate();
	}

	//------------------------------

	void Button::drawUnclipped(DrawingContext* p_context, const Rectangle<float>& p_invalidRectangle)
	{
		if (m_emphasis == Emphasis::Medium)
		{
			p_context->setColor(Color(m_theme->colors["on background"], 0.25f));
			p_context->strokeRoundedRectangle(Rectangle<float>(0.f, 0.f, getWidth(), getHeight()), getCornerRadius(), 1.f);
		}
	}

	void Button::draw(DrawingContext* p_context, const Rectangle<float>& p_invalidRectangle)
	{
		if (m_emphasis == Emphasis::High)
		{
			p_context->clear(m_currentColor);
			p_context->setColor(m_theme->colors["on primary"]);
		}
		else
		{
			p_context->setColor(m_currentColor);
		}

		p_context->drawText(m_text);
	}
};