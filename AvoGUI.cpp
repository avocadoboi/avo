#include "AvoGUI.hpp"

#include "Font data.hpp"

//------------------------------

#include <stack>
#include <random>
#include <time.h>
#include <chrono>

//------------------------------

#ifdef _WIN32
#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>
#undef DrawTextW

#include <windowsx.h>
#include <ShObjIdl.h>
#include <ShlObj_core.h>
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

#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#endif

//------------------------------

namespace AvoGUI
{
	std::default_random_engine randomEngine(time(0));
	std::uniform_real_distribution<double> uniformDistribution(0.0, 1.0);
	std::normal_distribution<double> normalDistribution(0.0, 1.0);

	double random()
	{
		return uniformDistribution(randomEngine);
	}
	double randomNormal()
	{
		return normalDistribution(randomEngine);
	}

	//------------------------------

	void convertUtf8ToUtf16(char const* p_input, wchar_t* p_output, uint32 p_numberOfUnitsInOutput)
	{
#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, p_input, -1, p_output, p_numberOfUnitsInOutput);
#endif
	}
	void convertUtf8ToUtf16(char const* p_input, uint32 p_numberOfUnitsInInput, wchar_t* p_output, uint32 p_numberOfUnitsInOutput)
	{
#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, p_output, p_numberOfUnitsInOutput);
#endif
	}
	std::wstring convertUtf8ToUtf16(std::string const& p_input)
	{
#ifdef _WIN32
		std::wstring result;
		result.resize(MultiByteToWideChar(CP_UTF8, 0, p_input.c_str(), p_input.size(), 0, 0));
		MultiByteToWideChar(CP_UTF8, 0, p_input.c_str(), p_input.size(), (wchar_t*)result.data(), result.size());
		return result;
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(char const* p_input)
	{
#ifdef _WIN32
		return MultiByteToWideChar(CP_UTF8, 0, p_input, -1, 0, 0);
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(char const* p_input, int32 p_numberOfUnitsInInput)
	{
#ifdef _WIN32
		return MultiByteToWideChar(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0);
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(std::string const& p_string)
	{
#ifdef _WIN32
		return MultiByteToWideChar(CP_UTF8, 0, p_string.data(), p_string.size() + 1, 0, 0);
#endif
	}

	void convertUtf16ToUtf8(wchar_t const* p_input, char* p_output, uint32 p_numberOfCharactersInOutput)
	{
#ifdef _WIN32
		WideCharToMultiByte(CP_UTF8, 0, p_input, -1, p_output, p_numberOfCharactersInOutput, 0, false);
#endif
	}
	void convertUtf16ToUtf8(wchar_t const* p_input, uint32 p_numberOfUnitsInInput, char* p_output, uint32 p_numberOfCharactersInResult)
	{
#ifdef _WIN32
		WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, p_output, p_numberOfCharactersInResult, 0, false);
#endif
	}
	std::string convertUtf16ToUtf8(std::wstring const& p_input)
	{
#ifdef _WIN32
		std::string result;
		result.resize(WideCharToMultiByte(CP_UTF8, 0, p_input.c_str(), p_input.size(), 0, 0, 0, false));
		WideCharToMultiByte(CP_UTF8, 0, p_input.c_str(), p_input.size(), (char*)result.data(), result.size(), 0, false);
		return result;
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input)
	{
#ifdef _WIN32
		return WideCharToMultiByte(CP_UTF8, 0, p_input, -1, 0, 0, 0, false);
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input, uint32 p_numberOfUnitsInInput)
	{
#ifdef _WIN32
		return WideCharToMultiByte(CP_UTF8, 0, p_input, p_numberOfUnitsInInput, 0, 0, 0, false);
#endif
	}
	uint32 getNumberOfUnitsInUtfConvertedString(std::wstring const& p_string)
	{
#ifdef _WIN32
		return WideCharToMultiByte(CP_UTF8, 0, p_string.data(), p_string.size() + 1, 0, 0, 0, false);
#endif
	}

	//------------------------------
	// class Easing
	//------------------------------

	/*
	f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

	f'(x) = x0*(3 - 12*t + 9*t*t) + x1*(6*t - 9*t*t) + 3*t*t
	*/

	float Easing::easeValue(float p_value, float p_precision) const
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
		while (container && container != getGui())
		{
			p_position += container->getTopLeft();
			container = container->getParent();
		}

		return p_position;
	}
	void View::addChild(View* p_view)
	{
		if (p_view)
		{
			p_view->setIndex(m_children.size());
			m_children.push_back(p_view);
			updateViewDrawingIndex(p_view);

			handleChildAttachment(p_view);
			for (auto view : m_viewEventListeners)
			{
				view->handleViewChildAttachment(this, p_view);
			}
		}
	}

	void View::updateViewDrawingIndex(View* p_view)
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
					p_view->setIndex(a);
					return;
				}
				else
				{
					m_children[a] = m_children[a + 1U];
					m_children[a]->setIndex(a);
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
					p_view->setIndex(a);
					return;
				}
				else
				{
					m_children[a] = m_children[a - 1];
					m_children[a]->setIndex(a);
				}
			}
		}
	}

	void View::updateShadow()
	{
		if (getWidth() >= 1.f && getHeight() >= 1.f && m_elevation > 0.00001f && m_hasShadow && m_elevation < 400.f)
		{
			if (m_shadowImage)
			{
				m_shadowImage->forget();
			}
			m_shadowImage = m_gui->getDrawingContext()->createRectangleShadowImage(getSize(), m_corners, m_elevation, m_theme->colors["shadow"]);
			m_shadowBounds = Rectangle<float>(
				Point<float>(
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

	void View::sendBoundsChangeEvents(Rectangle<float> const& p_previousBounds)
	{
		if (p_previousBounds.left != m_bounds.left || p_previousBounds.right != m_bounds.right ||
			p_previousBounds.top != m_bounds.top || p_previousBounds.bottom != m_bounds.bottom)
		{
			if (fabsf(p_previousBounds.getWidth() - m_bounds.getWidth()) > 0.001f || fabsf(p_previousBounds.getHeight() - m_bounds.getHeight()) > 0.001f)
			{
				updateShadow(); // This is to update the shadow bounds and image.

				handleSizeChange(p_previousBounds.getWidth(), p_previousBounds.getHeight());
				for (auto viewListener : m_viewEventListeners)
				{
					viewListener->handleViewSizeChange(this, p_previousBounds.getWidth(), p_previousBounds.getHeight());
				}

				updateClipGeometry();
			}

			if (getParent())
			{
				Point<float> const& mousePosition = getGui()->getWindow()->getMousePosition() - getParent()->getAbsoluteTopLeft();
				if (getIsContaining(mousePosition) != p_previousBounds.getIsContaining(mousePosition))
				{
					MouseEvent event;
					event.x = mousePosition.x + getParent()->getAbsoluteLeft();
					event.y = mousePosition.y + getParent()->getAbsoluteTop();
					getGui()->handleGlobalMouseMove(event);
				}
			}

			handleBoundsChange(p_previousBounds);
			for (auto viewListener : m_viewEventListeners)
			{
				viewListener->handleViewBoundsChange(this, p_previousBounds);
			}
		}
	}

	void View::updateClipGeometry()
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

	View::View(View* p_parent, Rectangle<float> const& p_bounds) :
		ProtectedRectangle(p_bounds), 
		m_isInAnimationUpdateQueue(false), m_isVisible(true), m_isOverlay(false),
		m_areDragDropEventsEnabled(false),
		m_areMouseEventsEnabled(false), m_cursor(Cursor::Arrow),
		m_opacity(1.f),
		m_shadowBounds(p_bounds), m_hasShadow(true), m_elevation(0.f),
		m_layerIndex(0), m_index(0), m_id(0),
		m_isMouseHovering(false)
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
	View::~View()
	{
		if (m_id)
		{
			if (m_gui == this)
			{
				((Gui*)this)->getParent()->m_viewsById.erase(m_id);
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
	}

	//------------------------------

	void View::setClipGeometry(Geometry* p_geometry)
	{
		if (m_clipGeometry)
		{
			m_clipGeometry->forget();
		}
		p_geometry->remember();
		m_clipGeometry = p_geometry;
	}

	//------------------------------

	void View::setParent(View* p_container)
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

	void View::removeChild(View* p_view)
	{
		if (p_view && p_view->getParent() == this)
		{
			removeChild(p_view->getIndex());
		}
	}
	void View::removeChild(uint32 p_viewIndex)
	{
		View* childToRemove = m_children[p_viewIndex];

		for (uint32 a = p_viewIndex; a < m_children.size() - 1; a++)
		{
			m_children[a] = m_children[a + 1];
			m_children[a]->setIndex(a);
		}
		m_children.pop_back();

		handleChildDetachment(childToRemove);
		for (auto view : m_viewEventListeners)
		{
			view->handleViewChildDetachment(this, childToRemove);
		}
		childToRemove->forget();
	}
	void View::removeAllChildren()
	{
		if (m_children.empty()) // That function naming, ew... Why didn't they call it getIsEmpty? empty() should be emptying something >:^(
		{
			return;
		}

		while (m_children.size())
		{
			View* child = m_children.back();
			m_children.pop_back();

			handleChildDetachment(child);
			for (auto listener : m_viewEventListeners)
			{
				listener->handleViewChildDetachment(this, child);
			}
			child->forget();
		}
	}
	void View::setId(uint64 p_id)
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

	void View::setThemeColor(char const* p_name, Color const& p_color, bool p_willAffectChildren)
	{
		if (p_willAffectChildren)
		{
			View* view = this;
			uint32 startIndex = 0;
			while (true)
			{
			loopStart:
				for (uint32 a = startIndex; a < view->getNumberOfChildren(); a++)
				{
					view->getChild(a)->setThemeColor(p_name, p_color, false);
					if (view->getChild(a)->getNumberOfChildren())
					{
						view = view->getChild(a);
						startIndex = 0;
						goto loopStart; // dont @ me
					}
				}
				if (view == this)
				{
					break;
				}
				startIndex = view->getIndex() + 1;
				view = view->getParent();
			}
		}

		// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
		// It's not the other way around because the parent lays out the children and the size of the children may change in the handler.
		if (!m_theme)
		{
			m_theme = new Theme();
		}
		else if (m_theme->getReferenceCount() > 1)
		{
			m_theme->forget();
			m_theme = new Theme(*m_theme);
		}

		if (m_theme->colors[p_name] != p_color)
		{
			m_theme->colors[p_name] = p_color;
			std::string name(p_name);
			if (getGui() == this && name == "background")
			{
				((Gui*)this)->getDrawingContext()->setBackgroundColor(p_color);
			}
			handleThemeColorChange(name, p_color);
		}
	}

	//------------------------------

	//bool View::getIsIntersecting(View* p_view) const
	//{
	//	Rectangle<float> viewBounds(p_view->getAbsoluteBounds());
	//	if (p_view->getParent() != getParent())
	//	{
	//		viewBounds -= getParent()->getAbsoluteBounds().getTopLeft();
	//	}
	//	if (m_bounds.getIsIntersecting(viewBounds))
	//	{
	//		if (p_view->getCornerRadius() > 0.f)
	//		{
	//			if (m_cornerRadius > 0.f)
	//			{
	//				if (viewBounds.right - p_view->getCornerRadius() < m_bounds.left + m_cornerRadius)
	//				{
	//					if (viewBounds.bottom - p_view->getCornerRadius() < m_bounds.top + m_cornerRadius)
	//					{
	//						return Point<>::getDistanceSquared(viewBounds.right - m_cornerRadius, viewBounds.bottom - m_cornerRadius, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
	//					}
	//					if (viewBounds.top + p_view->getCornerRadius() > m_bounds.bottom - m_cornerRadius)
	//					{
	//						return Point<>::getDistanceSquared(viewBounds.right - m_cornerRadius, viewBounds.top + m_cornerRadius, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
	//					}
	//				}
	//				else if (viewBounds.left + p_view->getCornerRadius() > m_bounds.right - m_cornerRadius)
	//				{
	//					if (viewBounds.bottom - p_view->getCornerRadius() < m_bounds.top + m_cornerRadius)
	//					{
	//						return Point<>::getDistanceSquared(viewBounds.left + m_cornerRadius, viewBounds.bottom - m_cornerRadius, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
	//					}
	//					if (viewBounds.top + p_view->getCornerRadius() > m_bounds.bottom - m_cornerRadius)
	//					{
	//						return Point<>::getDistanceSquared(viewBounds.left + m_cornerRadius, viewBounds.top + m_cornerRadius, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) < (p_view->getCornerRadius() + m_cornerRadius)*(p_view->getCornerRadius() + m_cornerRadius);
	//					}
	//				}
	//				return true;
	//			}
	//			float radius = p_view->getCornerRadius();
	//			if (m_bounds.right < viewBounds.left + radius)
	//			{
	//				if (m_bounds.bottom < viewBounds.top + radius)
	//				{
	//					return Point<>::getDistanceSquared(m_bounds.right, m_bounds.bottom, viewBounds.left + radius, viewBounds.top + radius) < radius*radius;
	//				}
	//				if (m_bounds.top < viewBounds.bottom - radius)
	//				{
	//					return Point<>::getDistanceSquared(m_bounds.right, m_bounds.top, viewBounds.left + radius, viewBounds.bottom - radius) < radius*radius;
	//				}
	//			}
	//			else if (m_bounds.left > viewBounds.right)
	//			{
	//				if (m_bounds.bottom < viewBounds.top + radius)
	//				{
	//					return Point<>::getDistanceSquared(m_bounds.left, m_bounds.bottom, viewBounds.right - radius, viewBounds.top + radius) < radius*radius;
	//				}
	//				if (m_bounds.top < viewBounds.bottom - radius)
	//				{
	//					return Point<>::getDistanceSquared(m_bounds.left, m_bounds.top, viewBounds.right - radius, viewBounds.bottom - radius) < radius*radius;
	//				}
	//			}
	//		}
	//		return true;
	//	}
	//	return false;
	//}

	//bool View::getIsContaining(View* p_view) const
	//{
	//	Rectangle<float> viewBounds(p_view->getAbsoluteBounds());
	//	if (p_view->getParent() != getParent())
	//	{
	//		viewBounds -= getParent()->getAbsoluteBounds().getTopLeft();
	//	}
	//	if (getIsContaining(viewBounds))
	//	{
	//		float radius = p_view->getCornerRadius();
	//		float maxDistance = (m_cornerRadius - radius)*(m_cornerRadius - radius);
	//		if (viewBounds.left < m_bounds.left + m_cornerRadius)
	//		{
	//			if (viewBounds.top < m_bounds.top + m_cornerRadius)
	//			{
	//				if (Point<>::getDistanceSquared(viewBounds.left + radius, viewBounds.top + radius, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//			if (viewBounds.bottom > m_bounds.bottom - m_cornerRadius)
	//			{
	//				if (Point<>::getDistanceSquared(viewBounds.left + radius, viewBounds.bottom - radius, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//		}
	//		if (viewBounds.right > m_bounds.right - m_cornerRadius)
	//		{
	//			if (viewBounds.top < m_bounds.top + m_cornerRadius)
	//			{
	//				if (Point<>::getDistanceSquared(viewBounds.right - radius, viewBounds.top + radius, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//			if (viewBounds.bottom > m_bounds.bottom - m_cornerRadius)
	//			{
	//				if (Point<>::getDistanceSquared(viewBounds.right - radius, viewBounds.bottom - radius, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//		}
	//		return true;
	//	}
	//	if (viewBounds.getIsContaining(m_bounds))
	//	{
	//		float radius = p_view->getCornerRadius();
	//		float maxDistance = (m_cornerRadius - radius)*(m_cornerRadius - radius);
	//		if (m_bounds.left < viewBounds.left + radius)
	//		{
	//			if (m_bounds.top < viewBounds.top + radius)
	//			{
	//				if (Point<>::getDistanceSquared(m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius, viewBounds.left + radius, viewBounds.top + radius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//			if (m_bounds.bottom > viewBounds.bottom - radius)
	//			{
	//				if (Point<>::getDistanceSquared(m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius, viewBounds.left + radius, viewBounds.bottom - radius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//		}
	//		if (m_bounds.right > viewBounds.right - radius)
	//		{
	//			if (m_bounds.top < viewBounds.top + radius)
	//			{
	//				if (Point<>::getDistanceSquared(m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius, viewBounds.right - radius, viewBounds.top + radius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//			if (m_bounds.bottom > viewBounds.bottom - radius)
	//			{
	//				if (Point<>::getDistanceSquared(m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius, viewBounds.right - radius, viewBounds.bottom - radius) > maxDistance)
	//				{
	//					return false;
	//				}
	//			}
	//		}
	//		return true;
	//	}
	//	return false;
	//}

	//------------------------------

	void View::setIsVisible(bool p_isVisible)
	{
		if (p_isVisible != m_isVisible)
		{
			m_isVisible = p_isVisible;

			//Point<float> const& mousePosition = getGUI()->getWindow()->getMousePosition();
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

	void View::setElevation(float p_elevation)
	{
		p_elevation = float(p_elevation < 0.f) * FLT_MAX + p_elevation;

		if (m_elevation != p_elevation)
		{
			m_elevation = p_elevation;
			updateShadow();
			m_parent->updateViewDrawingIndex(this);
		}
	}

	void View::setHasShadow(bool p_hasShadow)
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

	void View::queueAnimationUpdate()
	{
		if (!m_isInAnimationUpdateQueue && m_gui && m_isVisible)
		{
			m_gui->queueAnimationUpdateForView(this);
			m_isInAnimationUpdateQueue = true;
		}
	}

	//------------------------------

	void View::handleMouseBackgroundEnter(MouseEvent const& p_event)
	{
		getGui()->getWindow()->setCursor(m_cursor);
	}

	//------------------------------

	void View::invalidate()
	{
		if (m_gui)
		{
			Rectangle<float> shadowBounds(getAbsoluteShadowBounds().roundCoordinatesOutwards());
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

	void View::drawShadow(DrawingContext* p_drawingContext)
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

#define IUnknownDefinition(p_interfaceName)									\
ULONG __stdcall AddRef() override											\
{																			\
	return InterlockedIncrement(&m_referenceCount);							\
}																			\
ULONG __stdcall Release() override											\
{																			\
	uint32 referenceCount = InterlockedDecrement(&m_referenceCount);		\
	if (!referenceCount)													\
	{																		\
		delete this;														\
		return 0;															\
	}																		\
	return referenceCount;													\
}																			\
HRESULT __stdcall QueryInterface(IID const& p_id, void** p_object) override	\
{																			\
	if (p_id == IID_IUnknown || p_id == __uuidof(p_interfaceName))			\
	{																		\
		*p_object = this;													\
		AddRef();															\
		return S_OK;														\
	}																		\
	*p_object = 0;															\
	return E_NOINTERFACE;													\
}

	//------------------------------

	ModifierKeyFlags convertWindowsKeyStateToModifierKeyFlags(unsigned short p_keyState)
	{
		ModifierKeyFlags modifierFlags = ModifierKeyFlags::None;

		if (p_keyState & MK_CONTROL)
			modifierFlags |= ModifierKeyFlags::Control;
		if (p_keyState & MK_SHIFT)
			modifierFlags |= ModifierKeyFlags::Shift;
		if (p_keyState & MK_LBUTTON)
			modifierFlags |= ModifierKeyFlags::LeftMouse;
		if (p_keyState & MK_MBUTTON)
			modifierFlags |= ModifierKeyFlags::MiddleMouse;
		if (p_keyState & MK_RBUTTON)
			modifierFlags |= ModifierKeyFlags::RightMouse;
		if (p_keyState & MK_XBUTTON1)
			modifierFlags |= ModifierKeyFlags::X0Mouse;
		if (p_keyState & MK_XBUTTON2)
			modifierFlags |= ModifierKeyFlags::X1Mouse;
		if (GetKeyState(VK_MENU) < 0)
			modifierFlags |= ModifierKeyFlags::Alt;

		return modifierFlags;
	}

	//------------------------------

	class OleFormatEnumerator : public IEnumFORMATETC
	{
	private:
		uint32 m_referenceCount;

		FORMATETC* m_formats;
		uint32 m_numberOfFormats;
		uint32 m_currentFormatIndex;

	public:
		OleFormatEnumerator(FORMATETC* p_formats, uint32 p_numberOfFormats) :
			m_referenceCount(1), m_numberOfFormats(p_numberOfFormats)
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
				if (p_formats->ptd)
				{
					p_formats->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));
					*p_formats->ptd = *m_formats[m_currentFormatIndex].ptd;
				}

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
		uint32 m_numberOfFormats;

		FORMATETC* m_formats;
		STGMEDIUM* m_mediums;

	public:
		OleDataObject(FORMATETC* p_formats, STGMEDIUM* p_mediums, uint32 p_numberOfFormats) :
			m_referenceCount(1), m_numberOfFormats(p_numberOfFormats)
		{
			m_formats = new FORMATETC[p_numberOfFormats];
			m_mediums = new STGMEDIUM[p_numberOfFormats];

			for (uint32 a = 0; a < p_numberOfFormats; a++)
			{
				m_formats[a] = p_formats[a];
				m_mediums[a] = p_mediums[a];
			}
		}
		~OleDataObject()
		{
			delete[] m_formats;
		
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				ReleaseStgMedium(m_mediums + a);
			}
			delete[] m_mediums;
		}

		//------------------------------

		IUnknownDefinition(IDataObject)

		//------------------------------

		HRESULT __stdcall SetData(FORMATETC* p_format, STGMEDIUM* p_medium, BOOL fRelease) override
		{
			return E_NOTIMPL;
		}
		HRESULT __stdcall QueryGetData(FORMATETC* p_format) override
		{
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				if (m_formats[a].cfFormat == p_format->cfFormat &&
					m_formats[a].dwAspect == p_format->dwAspect &&
					m_formats[a].tymed & p_format->tymed)
				{
					return S_OK;
				}
			}
			return DV_E_FORMATETC;
		}
		HRESULT __stdcall GetData(FORMATETC* p_format, STGMEDIUM* p_medium) override
		{
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				if (m_formats[a].cfFormat == p_format->cfFormat &&
					m_formats[a].dwAspect == p_format->dwAspect &&
					m_formats[a].tymed & p_format->tymed)
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
						// Does not actually copy the data, only allocates a separate object with same internal state
						m_mediums[a].pstm->Clone(&p_medium->pstm); 

						ULARGE_INTEGER size;
						size.QuadPart = ULONGLONG_MAX;
						m_mediums[a].pstm->CopyTo(p_medium->pstm, size, 0, 0);
					}

					return S_OK;
				}
			}
			return DV_E_FORMATETC;
		}
		// Non-allocating version of GetData
		HRESULT __stdcall GetDataHere(FORMATETC* p_format, STGMEDIUM* p_medium) override
		{
			for (uint32 a = 0; a < m_numberOfFormats; a++)
			{
				if (m_formats[a].cfFormat == p_format->cfFormat &&
					m_formats[a].dwAspect == p_format->dwAspect &&
					m_formats[a].tymed & p_format->tymed)
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
				*p_formatEnumerator = new OleFormatEnumerator(m_formats, m_numberOfFormats);
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

	public:
		OleDropSource() :
			m_referenceCount(1)
		{

		}
		~OleDropSource()
		{

		}

		//------------------------------

		IUnknownDefinition(IDropSource)

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
			return DRAGDROP_S_USEDEFAULTCURSORS;
		}
	};

	//------------------------------

	class WindowsDragDropEvent :
		public DragDropEvent
	{
	private:
		IDataObject* m_dataObject = 0;
		FORMATETC* m_oleFormats = 0;
		uint32 m_numberOfFormats = 0;
		uint32 m_numberOfFiles = 0;

		FORMATETC* m_fileDescriptorFormat = 0;
		FORMATETC* m_itemNamesFormat = 0;
		FORMATETC* m_textFormat = 0;

		std::vector<HGLOBAL> m_globalDataToRelease;
		std::vector<char const*> m_streamBuffersToRelease;

		uint32 m_clipboardFormat_fileContents;
		uint32 m_clipboardFormat_fileGroupDescriptor;

		Gui* m_gui;

		void releaseDataObject()
		{
			m_fileDescriptorFormat = 0;
			m_itemNamesFormat = 0;
			m_textFormat = 0;
			m_numberOfFiles = 0;
			if (m_dataObject)
			{
				m_dataObject->Release();
				delete[] m_oleFormats;
			}
			for (HGLOBAL globalDataHandle : m_globalDataToRelease)
			{
				GlobalUnlock(globalDataHandle);
				GlobalFree(globalDataHandle);
			}
			for (char const* buffer : m_streamBuffersToRelease)
			{
				delete[] buffer;
			}
			m_globalDataToRelease.clear();
			formats.clear();
		}

	public:
		WindowsDragDropEvent(Gui* p_gui) :
			m_gui(p_gui)
		{
			m_clipboardFormat_fileContents = RegisterClipboardFormatW(L"FileContents");
			m_clipboardFormat_fileGroupDescriptor = RegisterClipboardFormatW(L"FileGroupDescriptorW");
		}
		~WindowsDragDropEvent()
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

				m_oleFormats = new FORMATETC[50];
				enumerator->Next(50, m_oleFormats, (ULONG*)&m_numberOfFormats);
				for (uint32 a = 0; a < m_numberOfFormats; a++)
				{
					uint32 format = m_oleFormats[a].cfFormat;
					if (format == CF_HDROP)
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

		DragDropData getDataForFormat(uint32 p_formatIndex) const override
		{
			switch (m_oleFormats[p_formatIndex].tymed)
			{
			case TYMED_FILE:
			{
				break;
			}
			case TYMED_HGLOBAL:
			{
				STGMEDIUM medium;
				HRESULT result = m_dataObject->GetData(m_oleFormats + p_formatIndex, &medium);
				if (result == S_OK)
				{
					DragDropData data { (char const*)GlobalLock(medium.hGlobal), GlobalSize(medium.hGlobal) };
					((WindowsDragDropEvent*)this)->m_globalDataToRelease.push_back(medium.hGlobal);
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
					medium.pstm->Read((void*)buffer, stats.cbSize.QuadPart, &numberOfBytesRead);
					medium.pstm->Release();
					((WindowsDragDropEvent*)this)->m_streamBuffersToRelease.push_back(buffer);

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
			return convertUtf16ToUtf8(name);
		}

		std::string getString() const override
		{
			if (m_textFormat)
			{
				STGMEDIUM medium;
				HRESULT result = m_dataObject->GetData(m_textFormat, &medium);
				if (result == S_OK)
				{
					std::string string = convertUtf16ToUtf8((wchar_t*)GlobalLock(medium.hGlobal));
					GlobalUnlock(medium.hGlobal);
					GlobalFree(medium.hGlobal);
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
					GlobalFree(medium.hGlobal);
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
				if (result == S_OK && medium.tymed == TYMED_HGLOBAL)
				{
					DROPFILES* filenameStructure = (DROPFILES*)GlobalLock(medium.hGlobal);

					wchar_t const* currentBufferPosition = (wchar_t const*)((char*)filenameStructure + filenameStructure->pFiles) + 1;
					itemNames.push_back(convertUtf16ToUtf8(currentBufferPosition - 1));
					while (true)
					{
						if (!*currentBufferPosition)
						{
							if (!*(currentBufferPosition + 1))
							{
								break;
							}
							itemNames.push_back(convertUtf16ToUtf8(currentBufferPosition + 1));
						}
						currentBufferPosition++;
					}

					GlobalUnlock(medium.hGlobal);
					GlobalFree(medium.hGlobal);
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
				if (result == S_OK && medium.tymed == TYMED_HGLOBAL)
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
					GlobalFree(medium.hGlobal);
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
				if (result == S_OK && medium.tymed == TYMED_HGLOBAL)
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
					GlobalFree(medium.hGlobal);
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
				if (result == S_OK && medium.tymed == TYMED_HGLOBAL)
				{
					FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)GlobalLock(medium.hGlobal);
					fileNames.reserve(groupDescriptor->cItems);
					for (uint32 a = 0; a < fileNames.size(); a++)
					{
						fileNames[a] = convertUtf16ToUtf8(groupDescriptor->fgd[a].cFileName);
					}
					GlobalUnlock(medium.hGlobal);
					GlobalFree(medium.hGlobal);
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
				if (result == S_OK && medium.tymed == TYMED_HGLOBAL)
				{
					FILEGROUPDESCRIPTORW* groupDescriptor = (FILEGROUPDESCRIPTORW*)GlobalLock(medium.hGlobal);
					fileNames.reserve(groupDescriptor->cItems);
					for (uint32 a = 0; a < fileNames.size(); a++)
					{
						fileNames[a] = groupDescriptor->fgd[a].cFileName;
					}
					GlobalUnlock(medium.hGlobal);
					GlobalFree(medium.hGlobal);
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
								GlobalFree(medium.hGlobal);
							}
							else if (medium.tymed == TYMED_ISTREAM)
							{
								STATSTG stats;
								medium.pstm->Stat(&stats, STATFLAG_NONAME);

								output.push_back(std::string());
								output.back().resize(stats.cbSize.QuadPart);

								ULONG bufferSize = 0;
								medium.pstm->Read((char*)output.back().data(), output.size(), &bufferSize);
								if (bufferSize != output.back().size())
								{
									output.back().resize(bufferSize);
								}

								medium.pstm->Release();
							}
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
									GlobalFree(medium.hGlobal);
								}
								else if (medium.tymed == TYMED_ISTREAM)
								{
									STATSTG stats;
									medium.pstm->Stat(&stats, STATFLAG_NONAME);

									output.resize(stats.cbSize.QuadPart);

									ULONG bufferSize = 0;
									medium.pstm->Read((char*)output.data(), output.size(), &bufferSize);
									if (bufferSize != output.size())
									{
										output.resize(bufferSize);
									}

									medium.pstm->Release();
								}
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

		Image* getImage() const override
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

		Gui* m_gui;

		WindowsDragDropEvent m_dragDropEvent;

		IDropTargetHelper* m_dropImageViewer;

	public:
		OleDropTarget(Gui* p_gui) :
			m_referenceCount(1), m_gui(p_gui), m_dragDropEvent(p_gui)
		{
			m_dragDropEvent.formats.reserve(15);

			CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_dropImageViewer));
		}
		~OleDropTarget()
		{
		}

		//------------------------------

		IUnknownDefinition(IDropTarget)

		//------------------------------

		HRESULT __stdcall DragEnter(IDataObject* p_dataObject, DWORD p_keyState, POINTL p_mousePosition, DWORD* p_effect)
		{
			POINT clientMousePosition = { p_mousePosition.x, p_mousePosition.y };
			m_dropImageViewer->DragEnter((HWND)m_gui->getWindow()->getNativeHandle(), p_dataObject, &clientMousePosition, *p_effect);

			m_dragDropEvent.setOleDataObject(p_dataObject);

			//------------------------------

			ScreenToClient((HWND)m_gui->getWindow()->getNativeHandle(), &clientMousePosition);

			float newX = clientMousePosition.x / m_gui->getWindow()->getDipToPixelFactor();
			float newY = clientMousePosition.y / m_gui->getWindow()->getDipToPixelFactor();

			m_dragDropEvent.movementX = newX - m_dragDropEvent.x;
			m_dragDropEvent.movementY = newY - m_dragDropEvent.y;
			m_dragDropEvent.x = newX;
			m_dragDropEvent.y = newY;

			m_dragDropEvent.modifierKeys = convertWindowsKeyStateToModifierKeyFlags(p_keyState);

			switch (m_gui->handleGlobalDragDropMove(m_dragDropEvent))
			{
			case DragDropOperation::Copy:
				*p_effect = DROPEFFECT_COPY;
				break;
			case DragDropOperation::Move:
				*p_effect = DROPEFFECT_MOVE;
				break;
			case DragDropOperation::Link:
				*p_effect = DROPEFFECT_LINK;
				break;
			default:
				*p_effect = DROPEFFECT_NONE;
			}

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

			switch (m_gui->handleGlobalDragDropMove(m_dragDropEvent))
			{
			case DragDropOperation::Copy:
				*p_effect = DROPEFFECT_COPY;
				break;
			case DragDropOperation::Move:
				*p_effect = DROPEFFECT_MOVE;
				break;
			case DragDropOperation::Link:
				*p_effect = DROPEFFECT_LINK;
				break;
			default:
				*p_effect = DROPEFFECT_NONE;
			}

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

			m_gui->handleGlobalDragDropLeave(m_dragDropEvent);

			m_dragDropEvent.setOleDataObject(0);
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

			m_gui->handleGlobalDragDropFinish(m_dragDropEvent);

			clientMousePosition.x = p_mousePosition.x;
			clientMousePosition.y = p_mousePosition.y;
			m_dropImageViewer->Drop(p_dataObject, &clientMousePosition, *p_effect);

			m_dragDropEvent.setOleDataObject(0);

			return S_OK;
		}
	};

	//------------------------------

	constexpr int WM_APP_CHANGE_SIZE = WM_APP;
	constexpr int WM_APP_SET_IS_ENABLED = WM_APP + 1;

	class WindowsWindow : public Window
	{
	private:
		Gui* m_gui;

		OleDropSource* m_oleDropSource;
		OleDropTarget* m_oleDropTarget;

		HWND m_windowHandle = 0;
		WindowStyleFlags m_crossPlatformStyles;
		uint32 m_styles = 0;

		bool m_isOpen = false;
		Point<int32> m_position;
		Point<uint32> m_size;
		Point<uint32> m_minSize;
		Point<uint32> m_maxSize;

		float m_dipToPixelFactor = 1.f;

		bool m_isFullscreen = false;
		RECT m_windowRectBeforeFullscreen;
		bool m_wasWindowMaximizedBeforeFullscreen = false;

		WindowState m_state = WindowState::Restored;

		bool m_isMouseOutsideClientArea = true;
		Point<int32> m_mousePosition;
		HCURSOR m_cursorHandle = 0;
		Cursor m_cursorType = (Cursor)-1;

		uint32 convertWindowStyleFlagsToWindowsWindowStyleFlags(WindowStyleFlags p_styleFlags, bool p_hasParent)
		{
			uint32 styles = 0;

			if (p_hasParent)
			{
				styles |= WS_POPUP;
			}

			if (!uint32(p_styleFlags & WindowStyleFlags::Invisible))
				styles |= WS_VISIBLE;
			if (uint32(p_styleFlags & WindowStyleFlags::Border))
				styles |= WS_CAPTION | WS_SYSMENU;

			if (uint32(p_styleFlags & WindowStyleFlags::Minimized))
				styles |= WS_MINIMIZE;
			else if (uint32(p_styleFlags & WindowStyleFlags::Maximized))
				styles |= WS_MAXIMIZE;

			if (uint32(p_styleFlags & WindowStyleFlags::MinimizeButton))
				styles |= WS_MINIMIZEBOX;
			if (uint32(p_styleFlags & WindowStyleFlags::MaximizeButton))
				styles |= WS_MAXIMIZEBOX;
			if (uint32(p_styleFlags & WindowStyleFlags::ResizeBorder))
				styles |= WS_THICKFRAME;

			return styles;
		}
		KeyboardKey convertWindowsDataToKeyboardKey(uint64 p_data)
		{
			switch (p_data)
			{
			case VK_BACK:
				return KeyboardKey::Backspace;
			case VK_CLEAR:
				return KeyboardKey::Clear;
			case VK_TAB:
				return KeyboardKey::Tab;
			case VK_RETURN:
				return KeyboardKey::Return;
			case VK_SHIFT:
				return KeyboardKey::Shift;
			case VK_CONTROL:
				return KeyboardKey::Control;
			case VK_MENU:
				return KeyboardKey::Alt;
			case VK_PAUSE:
				return KeyboardKey::Pause;
			case VK_PLAY:
				return KeyboardKey::Play;
			case VK_CAPITAL:
				return KeyboardKey::CapsLock;
			case VK_ESCAPE:
				return KeyboardKey::Escape;
			case VK_SPACE:
				return KeyboardKey::Spacebar;
			case VK_PRIOR:
				return KeyboardKey::PageUp;
			case VK_NEXT:
				return KeyboardKey::PageDown;
			case VK_END:
				return KeyboardKey::End;
			case VK_HOME:
				return KeyboardKey::Home;
			case VK_LEFT:
				return KeyboardKey::Left;
			case VK_RIGHT:
				return KeyboardKey::Right;
			case VK_UP:
				return KeyboardKey::Up;
			case VK_DOWN:
				return KeyboardKey::Down;
			case VK_SNAPSHOT:
				return KeyboardKey::PrintScreen;
			case VK_INSERT:
				return KeyboardKey::Insert;
			case VK_DELETE:
				return KeyboardKey::Delete;
			case VK_HELP:
				return KeyboardKey::Help;
			case VK_NUMPAD0:
				return KeyboardKey::Numpad0;
			case VK_NUMPAD1:
				return KeyboardKey::Numpad1;
			case VK_NUMPAD2:
				return KeyboardKey::Numpad2;
			case VK_NUMPAD3:
				return KeyboardKey::Numpad3;
			case VK_NUMPAD4:
				return KeyboardKey::Numpad4;
			case VK_NUMPAD5:
				return KeyboardKey::Numpad5;
			case VK_NUMPAD6:
				return KeyboardKey::Numpad6;
			case VK_NUMPAD7:
				return KeyboardKey::Numpad7;
			case VK_NUMPAD8:
				return KeyboardKey::Numpad8;
			case VK_NUMPAD9:
				return KeyboardKey::Numpad9;
			case VK_F1:
				return KeyboardKey::F1;
			case VK_F2:
				return KeyboardKey::F2;
			case VK_F3:
				return KeyboardKey::F3;
			case VK_F4:
				return KeyboardKey::F4;
			case VK_F5:
				return KeyboardKey::F5;
			case VK_F6:
				return KeyboardKey::F6;
			case VK_F7:
				return KeyboardKey::F7;
			case VK_F8:
				return KeyboardKey::F8;
			case VK_F9:
				return KeyboardKey::F9;
			case VK_F10:
				return KeyboardKey::F10;
			case VK_F11:
				return KeyboardKey::F11;
			case VK_F12:
				return KeyboardKey::F12;
			case VK_F13:
				return KeyboardKey::F13;
			case VK_F14:
				return KeyboardKey::F14;
			case VK_F15:
				return KeyboardKey::F15;
			case VK_F16:
				return KeyboardKey::F16;
			case VK_F17:
				return KeyboardKey::F17;
			case VK_F18:
				return KeyboardKey::F18;
			case VK_F19:
				return KeyboardKey::F19;
			case VK_F20:
				return KeyboardKey::F20;
			case VK_F21:
				return KeyboardKey::F21;
			case VK_F22:
				return KeyboardKey::F22;
			case VK_F23:
				return KeyboardKey::F23;
			case VK_F24:
				return KeyboardKey::F24;
			case VK_NUMLOCK:
				return KeyboardKey::NumLock;
			case VK_LSHIFT:
				return KeyboardKey::ShiftLeft;
			case VK_RSHIFT:
				return KeyboardKey::ShiftRight;
			case VK_LCONTROL:
				return KeyboardKey::ControlLeft;
			case VK_RCONTROL:
				return KeyboardKey::ControlRight;
			case VK_LMENU:
				return KeyboardKey::MenuLeft;
			case VK_RMENU:
				return KeyboardKey::MenuRight;
			case VK_MEDIA_PREV_TRACK:
				return KeyboardKey::PreviousTrack;
			case VK_MEDIA_NEXT_TRACK:
				return KeyboardKey::NextTrack;
			case VK_MEDIA_PLAY_PAUSE:
				return KeyboardKey::PlayPauseTrack;
			case VK_MEDIA_STOP:
				return KeyboardKey::StopTrack;
			case 0x30:
				return KeyboardKey::Number0;
			case 0x31:
				return KeyboardKey::Number1;
			case 0x32:
				return KeyboardKey::Number2;
			case 0x33:
				return KeyboardKey::Number3;
			case 0x34:
				return KeyboardKey::Number4;
			case 0x35:
				return KeyboardKey::Number5;
			case 0x36:
				return KeyboardKey::Number6;
			case 0x37:
				return KeyboardKey::Number7;
			case 0x38:
				return KeyboardKey::Number8;
			case 0x39:
				return KeyboardKey::Number9;
			case 0x41:
				return KeyboardKey::A;
			case 0x42:
				return KeyboardKey::B;
			case 0x43:
				return KeyboardKey::C;
			case 0x44:
				return KeyboardKey::D;
			case 0x45:
				return KeyboardKey::E;
			case 0x46:
				return KeyboardKey::F;
			case 0x47:
				return KeyboardKey::G;
			case 0x48:
				return KeyboardKey::H;
			case 0x49:
				return KeyboardKey::I;
			case 0x4A:
				return KeyboardKey::J;
			case 0x4B:
				return KeyboardKey::K;
			case 0x4C:
				return KeyboardKey::L;
			case 0x4D:
				return KeyboardKey::M;
			case 0x4E:
				return KeyboardKey::N;
			case 0x4F:
				return KeyboardKey::O;
			case 0x50:
				return KeyboardKey::P;
			case 0x51:
				return KeyboardKey::Q;
			case 0x52:
				return KeyboardKey::R;
			case 0x53:
				return KeyboardKey::S;
			case 0x54:
				return KeyboardKey::T;
			case 0x55:
				return KeyboardKey::U;
			case 0x56:
				return KeyboardKey::V;
			case 0x57:
				return KeyboardKey::W;
			case 0x58:
				return KeyboardKey::X;
			case 0x59:
				return KeyboardKey::Y;
			case 0x5A:
				return KeyboardKey::Z;
			case VK_OEM_1:
				return KeyboardKey::Regional1;
			case VK_OEM_2:
				return KeyboardKey::Regional2;
			case VK_OEM_3:
				return KeyboardKey::Regional3;
			case VK_OEM_4:
				return KeyboardKey::Regional4;
			case VK_OEM_5:
				return KeyboardKey::Regional5;
			case VK_OEM_6:
				return KeyboardKey::Regional6;
			case VK_OEM_7:
				return KeyboardKey::Regional7;
			case VK_OEM_8:
				return KeyboardKey::Regional8;
			default:
				return KeyboardKey::None;
			}
		}

		bool m_hasCreatedWindow = false;
		std::condition_variable m_hasCreatedWindowConditionVariable;
		std::mutex m_hasCreatedWindowMutex;
		std::thread m_messageThread;
		void thread_createAndRun(char const* p_title, float p_x, float p_y, float p_width, float p_height, Window* p_parent)
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
			
			std::wstring wideTitle = convertUtf8ToUtf16(p_title);

			// m_windowHandle is initialized by the WM_CREATE event, before CreateWindow returns.
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

			if (uint32(m_crossPlatformStyles & WindowStyleFlags::CustomBorder))
			{
				SetWindowPos(
					m_windowHandle, 0,
					parentRect.left + p_x * (parentRect.right - parentRect.left - m_size.x),
					parentRect.top + p_y * (parentRect.bottom - parentRect.top - m_size.y),
					m_size.x, m_size.y,
					SWP_NOZORDER
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

		WindowsWindow(Gui* p_gui) :
			m_gui(p_gui), m_crossPlatformStyles((WindowStyleFlags)0),
			m_mousePosition(-1, -1)
		{
			setCursor(Cursor::Arrow);
		}
		WindowsWindow(Gui* p_gui, char const* p_title, uint32 p_width, uint32 p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) :
			m_gui(p_gui), m_crossPlatformStyles(p_styleFlags), 
			m_mousePosition(-1, -1)
		{
			create(p_title, p_width, p_height, p_styleFlags, p_parent);

			setCursor(Cursor::Arrow);
		}
		~WindowsWindow()
		{
			m_messageThread.join();
			DestroyCursor(m_cursorHandle);
		}

		//------------------------------

		void create(char const* p_title, float p_x, float p_y, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) override
		{
			m_crossPlatformStyles = p_styleFlags;
			m_styles = convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styleFlags, p_parent);
			
			m_messageThread = std::thread(&WindowsWindow::thread_createAndRun, this, p_title, p_x, p_y, p_width, p_height, p_parent);
			if (!m_hasCreatedWindow)
			{
				std::unique_lock<std::mutex> lock(m_hasCreatedWindowMutex);
				m_hasCreatedWindowConditionVariable.wait(lock, [=] { return (bool)m_hasCreatedWindow; });
			}
		}
		void create(char const* p_title, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) override
		{
			create(p_title, 0.5f, 0.5f, p_width, p_height, p_styleFlags, p_parent);
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

		void setTitle(char const* p_title) override
		{
			std::wstring wideTitle = convertUtf8ToUtf16(p_title);
			SetWindowTextW(m_windowHandle, wideTitle.c_str());
		}
		std::string getTitle() const override
		{
			std::wstring wideTitle;
			wideTitle.resize(150);
			GetWindowTextW(m_windowHandle, (wchar_t*)wideTitle.data(), wideTitle.size());
			return convertUtf16ToUtf8(wideTitle);
		}

		//------------------------------

		void setStyles(WindowStyleFlags p_styles) override
		{
			SetWindowLongPtr(m_windowHandle, GWL_STYLE, convertWindowStyleFlagsToWindowsWindowStyleFlags(p_styles, GetParent(m_windowHandle)));
			SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
		}
		WindowStyleFlags getStyles() const override
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
				if (m_state == WindowState::Restored)
				{
					GetWindowRect(m_windowHandle, &m_windowRectBeforeFullscreen);
				}
				else if (m_state == WindowState::Maximized)
				{
					m_wasWindowMaximizedBeforeFullscreen = true;
				}

				MONITORINFO info = { };
				info.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
				SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_VISIBLE | WS_MAXIMIZE);
				SetWindowPos(m_windowHandle, 0, info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top, SWP_NOZORDER | SWP_NOOWNERZORDER);
			}
			else
			{
				SetWindowLongPtr(m_windowHandle, GWL_STYLE, (m_wasWindowMaximizedBeforeFullscreen * WS_MAXIMIZE) | (m_styles & ~(WS_MAXIMIZE | WS_MINIMIZE)));
				if (m_wasWindowMaximizedBeforeFullscreen)
				{
					SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
				}
				else
				{
					SetWindowPos(m_windowHandle, 0, m_windowRectBeforeFullscreen.left, m_windowRectBeforeFullscreen.top, m_windowRectBeforeFullscreen.right - m_windowRectBeforeFullscreen.left, m_windowRectBeforeFullscreen.bottom - m_windowRectBeforeFullscreen.top, SWP_NOZORDER | SWP_NOOWNERZORDER);
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

		void setState(WindowState p_state) override
		{
			if (p_state == WindowState::Maximized)
				ShowWindow(m_windowHandle, SW_MAXIMIZE);
			else if (p_state == WindowState::Minimized)
				ShowWindow(m_windowHandle, SW_MINIMIZE);
			else if (p_state == WindowState::Restored)
				ShowWindow(m_windowHandle, SW_RESTORE);
		}
		WindowState getState() const override
		{
			return m_state;
		}

		//------------------------------

		void setPosition(Point<int32> const& p_position) override
		{
			setPosition(p_position.x, p_position.y);
		}
		void setPosition(int32 p_x, int32 p_y) override
		{
			SetWindowPos(m_windowHandle, 0, p_x, p_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			m_position.set(p_x, p_y);
		}
		Point<int32> const& getPosition() const override
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

		void setSize(Point<float> const& p_size) override
		{
			setSize(p_size.x, p_size.y);
		}
		void setSize(float p_width, float p_height) override
		{
			if (m_windowHandle)
			{
				RECT windowRect = { 0, 0, (int)std::ceil(p_width * m_dipToPixelFactor), (int)std::ceil(p_height * m_dipToPixelFactor) };
				AdjustWindowRect(&windowRect, m_styles, 0);
				
				PostMessage(m_windowHandle, WM_APP_CHANGE_SIZE, uint32(windowRect.right - windowRect.left), uint32(windowRect.bottom - windowRect.top));
			}
		}

		Point<float> const& getSize() const override
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

		void setMinSize(Point<float> const& p_minSize) override
		{
			m_minSize = p_minSize*m_dipToPixelFactor;
		}
		void setMinSize(float p_minWidth, float p_minHeight) override
		{
			m_minSize.x = p_minWidth * m_dipToPixelFactor;
			m_minSize.y = p_minHeight * m_dipToPixelFactor;
		}
		Point<float> getMinSize() const override
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

		void setMaxSize(Point<float> const& p_maxSize) override
		{
			m_maxSize = p_maxSize* m_dipToPixelFactor;
		}
		void setMaxSize(float p_maxWidth, float p_maxHeight) override
		{
			m_maxSize.x = p_maxWidth* m_dipToPixelFactor;
			m_maxSize.y = p_maxHeight* m_dipToPixelFactor;
		}
		Point<float> getMaxSize() const override
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

		Rectangle<uint32> getMonitorBounds() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Rectangle<uint32>(info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom);
		}
		Point<uint32> getMonitorPosition() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Point<uint32>(info.rcMonitor.left, info.rcMonitor.top);
		}
		Point<uint32> getMonitorSize() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
			return Point<uint32>(info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top);
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

		Rectangle<uint32> getWorkAreaBounds() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Rectangle<uint32>(info.rcWork.left, info.rcWork.top, info.rcWork.right, info.rcWork.bottom);
		}
		Point<uint32> getWorkAreaPosition() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST), &info);
			return Point<uint32>(info.rcWork.left, info.rcWork.top);
		}
		Point<uint32> getWorkAreaSize() const override
		{
			MONITORINFO info = { };
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTOPRIMARY), &info);
			return Point<uint32>(info.rcWork.right - info.rcWork.left, info.rcWork.bottom - info.rcWork.top);
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

		bool getIsKeyDown(KeyboardKey p_key) const override
		{
			switch (p_key)
			{
			case KeyboardKey::A:
				return GetAsyncKeyState(0x41) & (1 << 16);
			case KeyboardKey::B:
				return GetAsyncKeyState(0x42) & (1 << 16);
			case KeyboardKey::C:
				return GetAsyncKeyState(0x43) & (1 << 16);
			case KeyboardKey::D:
				return GetAsyncKeyState(0x44) & (1 << 16);
			case KeyboardKey::E:
				return GetAsyncKeyState(0x45) & (1 << 16);
			case KeyboardKey::F:
				return GetAsyncKeyState(0x46) & (1 << 16);
			case KeyboardKey::G:
				return GetAsyncKeyState(0x47) & (1 << 16);
			case KeyboardKey::H:
				return GetAsyncKeyState(0x48) & (1 << 16);
			case KeyboardKey::I:
				return GetAsyncKeyState(0x49) & (1 << 16);
			case KeyboardKey::J:
				return GetAsyncKeyState(0x4A) & (1 << 16);
			case KeyboardKey::K:
				return GetAsyncKeyState(0x4B) & (1 << 16);
			case KeyboardKey::L:
				return GetAsyncKeyState(0x4C) & (1 << 16);
			case KeyboardKey::M:
				return GetAsyncKeyState(0x4D) & (1 << 16);
			case KeyboardKey::N:
				return GetAsyncKeyState(0x4E) & (1 << 16);
			case KeyboardKey::O:
				return GetAsyncKeyState(0x4F) & (1 << 16);
			case KeyboardKey::P:
				return GetAsyncKeyState(0x50) & (1 << 16);
			case KeyboardKey::Q:
				return GetAsyncKeyState(0x51) & (1 << 16);
			case KeyboardKey::R:
				return GetAsyncKeyState(0x52) & (1 << 16);
			case KeyboardKey::S:
				return GetAsyncKeyState(0x53) & (1 << 16);
			case KeyboardKey::T:
				return GetAsyncKeyState(0x54) & (1 << 16);
			case KeyboardKey::U:
				return GetAsyncKeyState(0x55) & (1 << 16);
			case KeyboardKey::V:
				return GetAsyncKeyState(0x56) & (1 << 16);
			case KeyboardKey::W:
				return GetAsyncKeyState(0x57) & (1 << 16);
			case KeyboardKey::X:
				return GetAsyncKeyState(0x58) & (1 << 16);
			case KeyboardKey::Y:
				return GetAsyncKeyState(0x59) & (1 << 16);
			case KeyboardKey::Z:
				return GetAsyncKeyState(0x5A) & (1 << 16);
			case KeyboardKey::Add:
				return GetAsyncKeyState(VK_ADD) & (1 << 16);
			case KeyboardKey::Alt:
				return GetAsyncKeyState(VK_MENU) & (1 << 16);
			case KeyboardKey::Backspace:
				return GetAsyncKeyState(VK_BACK) & (1 << 16);
			case KeyboardKey::CapsLock:
				return GetAsyncKeyState(VK_CAPITAL) & (1 << 16);
			case KeyboardKey::Clear:
				return GetAsyncKeyState(VK_CLEAR) & (1 << 16);
			case KeyboardKey::Comma:
				return GetAsyncKeyState(VK_OEM_COMMA) & (1 << 16);
			case KeyboardKey::Control:
				return GetAsyncKeyState(VK_CONTROL) & (1 << 16);
			case KeyboardKey::ControlLeft:
				return GetAsyncKeyState(VK_LCONTROL) & (1 << 16);
			case KeyboardKey::ControlRight:
				return GetAsyncKeyState(VK_RCONTROL) & (1 << 16);
			case KeyboardKey::Decimal:
				return GetAsyncKeyState(VK_DECIMAL) & (1 << 16);
			case KeyboardKey::Delete:
				return GetAsyncKeyState(VK_DELETE) & (1 << 16);
			case KeyboardKey::Divide:
				return GetAsyncKeyState(VK_DIVIDE) & (1 << 16);
			case KeyboardKey::Down:
				return GetAsyncKeyState(VK_DOWN) & (1 << 16);
			case KeyboardKey::End:
				return GetAsyncKeyState(VK_END) & (1 << 16);
			case KeyboardKey::Enter:
				return GetAsyncKeyState(VK_RETURN) & (1 << 16);
			case KeyboardKey::Escape:
				return GetAsyncKeyState(VK_ESCAPE) & (1 << 16);
			case KeyboardKey::F1:
				return GetAsyncKeyState(VK_F1) & (1 << 16);
			case KeyboardKey::F2:
				return GetAsyncKeyState(VK_F2) & (1 << 16);
			case KeyboardKey::F3:
				return GetAsyncKeyState(VK_F3) & (1 << 16);
			case KeyboardKey::F4:
				return GetAsyncKeyState(VK_F4) & (1 << 16);
			case KeyboardKey::F5:
				return GetAsyncKeyState(VK_F5) & (1 << 16);
			case KeyboardKey::F6:
				return GetAsyncKeyState(VK_F6) & (1 << 16);
			case KeyboardKey::F7:
				return GetAsyncKeyState(VK_F7) & (1 << 16);
			case KeyboardKey::F8:
				return GetAsyncKeyState(VK_F8) & (1 << 16);
			case KeyboardKey::F9:
				return GetAsyncKeyState(VK_F9) & (1 << 16);
			case KeyboardKey::F10:
				return GetAsyncKeyState(VK_F10) & (1 << 16);
			case KeyboardKey::F11:
				return GetAsyncKeyState(VK_F11) & (1 << 16);
			case KeyboardKey::F12:
				return GetAsyncKeyState(VK_F12) & (1 << 16);
			case KeyboardKey::F13:
				return GetAsyncKeyState(VK_F13) & (1 << 16);
			case KeyboardKey::F14:
				return GetAsyncKeyState(VK_F14) & (1 << 16);
			case KeyboardKey::F15:
				return GetAsyncKeyState(VK_F15) & (1 << 16);
			case KeyboardKey::F16:
				return GetAsyncKeyState(VK_F16) & (1 << 16);
			case KeyboardKey::F17:
				return GetAsyncKeyState(VK_F17) & (1 << 16);
			case KeyboardKey::F18:
				return GetAsyncKeyState(VK_F18) & (1 << 16);
			case KeyboardKey::F19:
				return GetAsyncKeyState(VK_F19) & (1 << 16);
			case KeyboardKey::F20:
				return GetAsyncKeyState(VK_F20) & (1 << 16);
			case KeyboardKey::F21:
				return GetAsyncKeyState(VK_F21) & (1 << 16);
			case KeyboardKey::F22:
				return GetAsyncKeyState(VK_F22) & (1 << 16);
			case KeyboardKey::F23:
				return GetAsyncKeyState(VK_F23) & (1 << 16);
			case KeyboardKey::F24:
				return GetAsyncKeyState(VK_F24) & (1 << 16);
			case KeyboardKey::Help:
				return GetAsyncKeyState(VK_HELP) & (1 << 16);
			case KeyboardKey::Home:
				return GetAsyncKeyState(VK_HOME) & (1 << 16);
			case KeyboardKey::Insert:
				return GetAsyncKeyState(VK_INSERT) & (1 << 16);
			case KeyboardKey::Left:
				return GetAsyncKeyState(VK_LEFT) & (1 << 16);
			case KeyboardKey::MenuLeft:
				return GetAsyncKeyState(VK_LMENU) & (1 << 16);
			case KeyboardKey::MenuRight:
				return GetAsyncKeyState(VK_RMENU) & (1 << 16);
			case KeyboardKey::Minus:
				return GetAsyncKeyState(VK_OEM_MINUS) & (1 << 16);
			case KeyboardKey::Multiply:
				return GetAsyncKeyState(VK_MULTIPLY) & (1 << 16);
			case KeyboardKey::NextTrack:
				return GetAsyncKeyState(VK_MEDIA_NEXT_TRACK) & (1 << 16);
			case KeyboardKey::Number0:
				return GetAsyncKeyState(0x30) & (1 << 16);
			case KeyboardKey::Number1:
				return GetAsyncKeyState(0x31) & (1 << 16);
			case KeyboardKey::Number2:
				return GetAsyncKeyState(0x32) & (1 << 16);
			case KeyboardKey::Number3:
				return GetAsyncKeyState(0x33) & (1 << 16);
			case KeyboardKey::Number4:
				return GetAsyncKeyState(0x34) & (1 << 16);
			case KeyboardKey::Number5:
				return GetAsyncKeyState(0x35) & (1 << 16);
			case KeyboardKey::Number6:
				return GetAsyncKeyState(0x36) & (1 << 16);
			case KeyboardKey::Number7:
				return GetAsyncKeyState(0x37) & (1 << 16);
			case KeyboardKey::Number8:
				return GetAsyncKeyState(0x38) & (1 << 16);
			case KeyboardKey::Number9:
				return GetAsyncKeyState(0x39) & (1 << 16);
			case KeyboardKey::NumLock:
				return GetAsyncKeyState(VK_NUMLOCK) & (1 << 16);
			case KeyboardKey::Numpad0:
				return GetAsyncKeyState(VK_NUMPAD0) & (1 << 16);
			case KeyboardKey::Numpad1:
				return GetAsyncKeyState(VK_NUMPAD1) & (1 << 16);
			case KeyboardKey::Numpad2:
				return GetAsyncKeyState(VK_NUMPAD2) & (1 << 16);
			case KeyboardKey::Numpad3:
				return GetAsyncKeyState(VK_NUMPAD3) & (1 << 16);
			case KeyboardKey::Numpad4:
				return GetAsyncKeyState(VK_NUMPAD4) & (1 << 16);
			case KeyboardKey::Numpad5:
				return GetAsyncKeyState(VK_NUMPAD5) & (1 << 16);
			case KeyboardKey::Numpad6:
				return GetAsyncKeyState(VK_NUMPAD6) & (1 << 16);
			case KeyboardKey::Numpad7:
				return GetAsyncKeyState(VK_NUMPAD7) & (1 << 16);
			case KeyboardKey::Numpad8:
				return GetAsyncKeyState(VK_NUMPAD8) & (1 << 16);
			case KeyboardKey::Numpad9:
				return GetAsyncKeyState(VK_NUMPAD9) & (1 << 16);
			case KeyboardKey::PageDown:
				return GetAsyncKeyState(VK_NEXT) & (1 << 16);
			case KeyboardKey::PageUp:
				return GetAsyncKeyState(VK_PRIOR) & (1 << 16);
			case KeyboardKey::Pause:
				return GetAsyncKeyState(VK_PAUSE) & (1 << 16);
			case KeyboardKey::Period:
				return GetAsyncKeyState(VK_OEM_PERIOD) & (1 << 16);
			case KeyboardKey::Play:
				return GetAsyncKeyState(VK_PLAY) & (1 << 16);
			case KeyboardKey::PlayPauseTrack:
				return GetAsyncKeyState(VK_MEDIA_PLAY_PAUSE) & (1 << 16);
			case KeyboardKey::Plus:
				return GetAsyncKeyState(VK_OEM_PLUS) & (1 << 16);
			case KeyboardKey::PreviousTrack:
				return GetAsyncKeyState(VK_MEDIA_PREV_TRACK) & (1 << 16);
			case KeyboardKey::PrintScreen:
				return GetAsyncKeyState(VK_SNAPSHOT) & (1 << 16);
			case KeyboardKey::Regional1:
				return GetAsyncKeyState(VK_OEM_1) & (1 << 16);
			case KeyboardKey::Regional2:
				return GetAsyncKeyState(VK_OEM_2) & (1 << 16);
			case KeyboardKey::Regional3:
				return GetAsyncKeyState(VK_OEM_3) & (1 << 16);
			case KeyboardKey::Regional4:
				return GetAsyncKeyState(VK_OEM_4) & (1 << 16);
			case KeyboardKey::Regional5:
				return GetAsyncKeyState(VK_OEM_5) & (1 << 16);
			case KeyboardKey::Regional6:
				return GetAsyncKeyState(VK_OEM_6) & (1 << 16);
			case KeyboardKey::Regional7:
				return GetAsyncKeyState(VK_OEM_7) & (1 << 16);
			case KeyboardKey::Regional8:
				return GetAsyncKeyState(VK_OEM_8) & (1 << 16);
			case KeyboardKey::Right:
				return GetAsyncKeyState(VK_RIGHT) & (1 << 16);
			case KeyboardKey::Separator:
				return GetAsyncKeyState(VK_SEPARATOR) & (1 << 16);
			case KeyboardKey::Shift:
				return GetAsyncKeyState(VK_SHIFT) & (1 << 16);
			case KeyboardKey::ShiftLeft:
				return GetAsyncKeyState(VK_LSHIFT) & (1 << 16);
			case KeyboardKey::ShiftRight:
				return GetAsyncKeyState(VK_RSHIFT) & (1 << 16);
			case KeyboardKey::Spacebar:
				return GetAsyncKeyState(VK_SPACE) & (1 << 16);
			case KeyboardKey::StopTrack:
				return GetAsyncKeyState(VK_MEDIA_STOP) & (1 << 16);
			case KeyboardKey::Subtract:
				return GetAsyncKeyState(VK_SUBTRACT) & (1 << 16);
			case KeyboardKey::Tab:
				return GetAsyncKeyState(VK_TAB) & (1 << 16);
			case KeyboardKey::Up:
				return GetAsyncKeyState(VK_UP) & (1 << 16);
			}
			return false;
		}
		bool getIsMouseButtonDown(MouseButton p_button) const override
		{
			switch (p_button)
			{
			case MouseButton::Left:
				return GetAsyncKeyState(VK_LBUTTON) & (1 << 16);
			case MouseButton::Middle:
				return GetAsyncKeyState(VK_MBUTTON) & (1 << 16);
			case MouseButton::Right:
				return GetAsyncKeyState(VK_RBUTTON) & (1 << 16);
			case MouseButton::X0:
				return GetAsyncKeyState(VK_XBUTTON1) & (1 << 16);
			case MouseButton::X1:
				return GetAsyncKeyState(VK_XBUTTON2) & (1 << 16);
			}
			return false;
		}
		Point<float> getMousePosition() const override
		{
			return m_mousePosition / m_dipToPixelFactor;
		}

		//------------------------------

		void setCursor(Cursor p_cursor) override
		{
			if (p_cursor == m_cursorType) return;

			wchar_t const* name = 0;
			switch (p_cursor)
			{
			case Cursor::Arrow:
				name = (wchar_t const*)IDC_ARROW;
				break;
			case Cursor::Blocked:
				name = (wchar_t const*)IDC_NO;
				break;
			case Cursor::Hand:
				name = (wchar_t const*)IDC_HAND;
				break;
			case Cursor::Ibeam:
				name = (wchar_t const*)IDC_IBEAM;
				break;
			case Cursor::ResizeAll:
				name = (wchar_t const*)IDC_SIZEALL;
				break;
			case Cursor::ResizeNESW:
				name = (wchar_t const*)IDC_SIZENESW;
				break;
			case Cursor::ResizeNS:
				name = (wchar_t const*)IDC_SIZENS;
				break;
			case Cursor::ResizeNWSE:
				name = (wchar_t const*)IDC_SIZENWSE;
				break;
			case Cursor::ResizeWE:
				name = (wchar_t const*)IDC_SIZEWE;
				break;
			case Cursor::Wait:
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
		Cursor getCursor() const override
		{
			return m_cursorType;
		}

		//------------------------------

		float getDipToPixelFactor() const override
		{
			return m_dipToPixelFactor;
		}

		//------------------------------

		DragDropOperation dragAndDropString(std::string const& p_string) override
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
			
			uint32 wideStringLength = getNumberOfUnitsInUtfConvertedString(p_string);
			medium.hGlobal = GlobalAlloc(GMEM_FIXED, wideStringLength*sizeof(wchar_t));
			convertUtf8ToUtf16(p_string.data(), p_string.size() + 1, (wchar_t*)medium.hGlobal, wideStringLength);

			OleDataObject* dataObject = new OleDataObject(&format, &medium, 1);

			DWORD dropOperation = DROPEFFECT_NONE;
			DoDragDrop(dataObject, m_oleDropSource, DROPEFFECT_COPY | DROPEFFECT_LINK, &dropOperation);
			switch (dropOperation)
			{
			case DROPEFFECT_COPY:
				return DragDropOperation::Copy;
			case DROPEFFECT_MOVE:
				return DragDropOperation::Move;
			case DROPEFFECT_LINK:
				return DragDropOperation::Link;
			}
			return DragDropOperation::None;
		}

		//------------------------------

		void setClipboardString(std::wstring const& p_string) override
		{
			HGLOBAL clipboardMemory = GlobalAlloc(GMEM_MOVEABLE, (p_string.size() + 1)*sizeof(wchar_t));
			if (!clipboardMemory)
			{
				return;
			}
			wchar_t* data = (wchar_t*)GlobalLock(clipboardMemory);
			memcpy(data, (char*)p_string.data(), p_string.size()*sizeof(wchar_t));
			data[p_string.size()] = 0;
			GlobalUnlock(clipboardMemory);

			OpenClipboard(m_windowHandle);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, clipboardMemory);
			CloseClipboard();
		}
		void setClipboardString(wchar_t const* p_string, int32 p_length) override
		{
			size_t size = p_length >= 0 ? p_length : wcslen(p_string);
			HGLOBAL clipboardMemory = GlobalAlloc(GMEM_MOVEABLE, (size + 1) * sizeof(wchar_t));
			if (!clipboardMemory)
			{
				return;
			}
			wchar_t* data = (wchar_t*)GlobalLock(clipboardMemory);
			memcpy(data, (char*)p_string, size * sizeof(wchar_t));
			data[size] = 0;
			GlobalUnlock(clipboardMemory);

			OpenClipboard(m_windowHandle);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, clipboardMemory);
			CloseClipboard();
		}

		void setClipboardString(std::string const& p_string) override
		{
			setClipboardString(convertUtf8ToUtf16(p_string));
		}
		void setClipboardString(char const* p_string, int32 p_length) override
		{
			uint32 size = p_length >= 0 ? p_length : strlen(p_string);

			uint32 wideSize = getNumberOfUnitsInUtfConvertedString(p_string, size);
			wchar_t* wideString = new wchar_t[wideSize];
			convertUtf8ToUtf16(p_string, size, wideString, wideSize);

			HGLOBAL clipboardMemory = GlobalAlloc(GMEM_MOVEABLE, (wideSize + 1) * sizeof(wchar_t));
			if (!clipboardMemory)
			{
				return;
			}
			wchar_t* data = (wchar_t*)GlobalLock(clipboardMemory);
			memcpy(data, (char*)p_string, wideSize * sizeof(wchar_t));
			data[wideSize] = 0;
			GlobalUnlock(clipboardMemory);

			OpenClipboard(m_windowHandle);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, clipboardMemory);
			CloseClipboard();

			delete[] wideString;
		}

		std::wstring getClipboardUtf16String() const override
		{
			std::wstring string = L"";

			OpenClipboard(m_windowHandle);

			void* handle = GetClipboardData(CF_UNICODETEXT);
			if (handle)
			{
				string = (wchar_t*)GlobalLock(handle);
				GlobalUnlock(handle);
			}

			CloseClipboard();

			return string;
		}
		std::string getClipboardString() const override
		{
			std::string string = "";

			OpenClipboard(m_windowHandle);

			void* handle = GetClipboardData(CF_UNICODETEXT);
			if (handle)
			{
				wchar_t* wideString = (wchar_t*)GlobalLock(handle);
				uint32 wideStringSize = GlobalSize(handle) >> 1;
				string.resize(getNumberOfUnitsInUtfConvertedString(wideString, wideStringSize));
				convertUtf16ToUtf8(wideString, wideStringSize, (char*)string.data(), string.size());
				GlobalUnlock(handle);
			}

			CloseClipboard();

			return string;
		}
		ClipboardDataType getClipboardDataType() const override
		{
			OpenClipboard(m_windowHandle);
			uint32 format = 0;
			while (format = EnumClipboardFormats(format))
			{
				if (format == CF_UNICODETEXT)
				{
					CloseClipboard();
					return ClipboardDataType::UnicodeString;
				}
			}
			CloseClipboard();

			if (format == CF_UNICODETEXT)
			{
				return ClipboardDataType::UnicodeString;
			}
			return ClipboardDataType::Unknown;
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

				m_oleDropSource = new OleDropSource();
				m_oleDropTarget = new OleDropTarget(m_gui);
				RegisterDragDrop(m_windowHandle, m_oleDropTarget);

				//------------------------------

				m_isOpen = true;
				WindowEvent event;
				event.window = this;

				m_gui->excludeAnimationThread();
				m_gui->handleWindowCreate(event);
				m_gui->includeAnimationThread();

				//------------------------------

				EnableNonClientDpiScaling(m_windowHandle);

				/*
					LCS_WINDOWS_COLOR_SPACE is the default colorspace, but we want the background erase 
					color to be consistent with the colors of Direct2D and other potential graphics APIs 
					so it is changed to the sRGB color space.
				*/
				LOGCOLORSPACEA colorSpaceSettings;
				colorSpaceSettings.lcsSignature = LCS_SIGNATURE;
				colorSpaceSettings.lcsVersion = 0x400;
				colorSpaceSettings.lcsSize = sizeof(colorSpaceSettings);
				colorSpaceSettings.lcsCSType = LCS_sRGB;
				colorSpaceSettings.lcsIntent = LCS_GM_ABS_COLORIMETRIC;

				HCOLORSPACE colorSpace = CreateColorSpaceA(&colorSpaceSettings);
				SetColorSpace(GetDC(m_windowHandle), colorSpace);

				return 0;
			}
			case WM_ACTIVATE:
			{
				if (uint32(m_crossPlatformStyles & WindowStyleFlags::CustomBorder))
				{
					MARGINS margins = { 0, 0, 1, 0 };
					DwmExtendFrameIntoClientArea(m_windowHandle, &margins);

					DWORD flags = DWMNCRP_ENABLED;
					DwmSetWindowAttribute(m_windowHandle, DWMWA_NCRENDERING_POLICY, &flags, sizeof(DWORD));
					return 0;
				}
				break;
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
				Color color = m_gui->getDrawingContext()->getBackgroundColor(); // Thread safe I think?
				FillRect(deviceContext, &rectangle, CreateSolidBrush(RGB(color.red * 255, color.green * 255, color.blue * 255)));

				return 1; // We erased it.
			}
			case WM_NCCALCSIZE:
			{
				if (uint32(m_crossPlatformStyles & WindowStyleFlags::CustomBorder) && p_data_a)
				{
					NCCALCSIZE_PARAMS* parameters = (NCCALCSIZE_PARAMS*)p_data_b;

					if (IsMaximized(m_windowHandle))
					{
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
				if (uint32(m_crossPlatformStyles & WindowStyleFlags::CustomBorder) && GetCapture() != m_windowHandle)
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
						MouseEvent mouseEvent;
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

				MouseEvent mouseEvent;
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

					MouseEvent mouseEvent;
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
				if (uint32(m_crossPlatformStyles & WindowStyleFlags::CustomBorder) && p_data_b)
				{
					POINT mousePosition = { GET_X_LPARAM(p_data_b), GET_Y_LPARAM(p_data_b) };
					ScreenToClient(m_windowHandle, &mousePosition);

					WindowBorderArea area = m_gui->getWindowBorderAreaAtPosition(mousePosition.x, mousePosition.y);
					if (IsMaximized(m_windowHandle) && area != WindowBorderArea::Dragging && area != WindowBorderArea::None)
					{
						return HTCLIENT;
					}
					switch (area)
					{
					case WindowBorderArea::TopLeftResize:
						return HTTOPLEFT;
					case WindowBorderArea::TopResize:
						return HTTOP;
					case WindowBorderArea::TopRightResize:
						return HTTOPRIGHT;
					case WindowBorderArea::LeftResize:
						return HTLEFT;
					case WindowBorderArea::RightResize:
						return HTRIGHT;
					case WindowBorderArea::BottomLeftResize:
						return HTBOTTOMLEFT;
					case WindowBorderArea::BottomResize:
						return HTBOTTOM;
					case WindowBorderArea::BottomRightResize:
						return HTBOTTOMRIGHT;
					case WindowBorderArea::Dragging:
						return HTCAPTION;
					case WindowBorderArea::None:
						return HTCLIENT;
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
			case WM_SIZE:
			{
				WindowEvent windowEvent;
				windowEvent.window = this;
				if (p_data_a == SIZE_MINIMIZED)
				{
					m_gui->excludeAnimationThread();
					m_gui->handleWindowMinimize(windowEvent);
					m_gui->includeAnimationThread();
					m_state = WindowState::Minimized;
				}
				else
				{
					uint32 width = p_data_b & 0xffff;
					uint32 height = p_data_b >> 16 & 0xffff;
					m_size.x = width;
					m_size.y = height;
					windowEvent.width = width / m_dipToPixelFactor;
					windowEvent.height = height / m_dipToPixelFactor;

					m_gui->excludeAnimationThread();
					if (p_data_a == SIZE_MAXIMIZED)
					{
						m_gui->handleWindowMaximize(windowEvent);
						m_state = WindowState::Maximized;
					}
					else if (p_data_a == SIZE_RESTORED && m_state != WindowState::Restored)
					{
						m_gui->handleWindowRestore(windowEvent);
						m_state = WindowState::Restored;
					}
					m_gui->handleWindowSizeChange(windowEvent);
					m_gui->includeAnimationThread();
				}

				if (!m_hasCreatedWindow)
				{
					m_hasCreatedWindowMutex.lock();
					m_hasCreatedWindow = true;
					m_hasCreatedWindowMutex.unlock();
					m_hasCreatedWindowConditionVariable.notify_one();
				}
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

				ModifierKeyFlags modifierKeyFlags = convertWindowsKeyStateToModifierKeyFlags(GET_KEYSTATE_WPARAM(p_data_a));

				MouseEvent mouseEvent;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Left;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Left;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Left;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Right;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Right;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Right;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Middle;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Middle;
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

				ModifierKeyFlags modifierFlags = convertWindowsKeyStateToModifierKeyFlags(p_data_a);

				MouseEvent mouseEvent;
				mouseEvent.x = x / m_dipToPixelFactor;
				mouseEvent.y = y / m_dipToPixelFactor;
				mouseEvent.mouseButton = MouseButton::Middle;
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
				KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				KeyboardEvent keyboardEvent;
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
				KeyboardKey key = convertWindowsDataToKeyboardKey(p_data_a);

				KeyboardEvent keyboardEvent;
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
				convertUtf16ToUtf8((wchar_t const*)&p_data_a, character, 5);

				KeyboardEvent keyboardEvent;
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
				if (m_gui->getWillClose())
				{
					DeleteColorSpace(GetColorSpace(GetDC(m_windowHandle)));
					
					RevokeDragDrop(m_windowHandle);
					OleUninitialize();

					m_isOpen = false;
					DestroyWindow(m_windowHandle);
				}
				else
				{
					WindowEvent windowEvent;
					windowEvent.window = this;
					m_gui->excludeAnimationThread();
					m_gui->handleWindowClose(windowEvent);
					m_gui->includeAnimationThread();
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
#pragma endregion

	class Direct2DImage : public Image
	{
	private:
		ID2D1Bitmap* m_image;

		ImageScalingMethod m_scalingMethod;
		ImageBoundsSizing m_boundsSizing;
		Point<float> m_boundsPositioning;

		Rectangle<float> m_cropRectangle;
		float m_opacity;

	public:
		Direct2DImage(ID2D1Bitmap* p_image) :
			m_image(p_image), 
			m_scalingMethod(ImageScalingMethod::Smooth), m_boundsSizing(ImageBoundsSizing::Stretch), m_boundsPositioning(0.5f, 0.5f), 
			m_cropRectangle(0.f, 0.f, p_image->GetSize().width, p_image->GetSize().height), m_opacity(1.f)
		{
			m_bounds = m_cropRectangle;
		}
		~Direct2DImage()
		{
			m_image->Release();
		}

		//------------------------------

		void setCropRectangle(Rectangle<float> const& p_rectangle) override
		{
			m_cropRectangle = p_rectangle;
		}
		Rectangle<float> const& getCropRectangle() const override
		{
			return m_cropRectangle;
		}

		Point<uint32> getOriginalSize() const override
		{
			return Point<uint32>(m_image->GetSize().width, m_image->GetSize().height);
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
		Point<float> const& getBoundsPositioning() const override
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

		void setScalingMethod(ImageScalingMethod p_scalingMethod) override
		{
			m_scalingMethod = p_scalingMethod;
		}
		ImageScalingMethod getScalingMethod() const override
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
			if (m_boundsSizing != ImageBoundsSizing::Stretch && m_boundsSizing == ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() < m_image->GetSize().width / m_image->GetSize().height)
			{
				return m_bounds.getHeight() * m_image->GetSize().width / m_image->GetSize().height;
			}
			return m_bounds.getWidth();
		}
		float getInnerHeight() const override
		{
			if (m_boundsSizing != ImageBoundsSizing::Stretch && m_boundsSizing == ImageBoundsSizing::Contain != m_bounds.getWidth() / m_bounds.getHeight() > m_image->GetSize().width / m_image->GetSize().height)
			{
				return m_bounds.getWidth() * m_image->GetSize().height / m_image->GetSize().width;
			}
			return m_bounds.getHeight();
		}
		Point<float> getInnerSize() const override
		{
			return Point<float>(getInnerWidth(), getInnerHeight());
		}
		Rectangle<float> getInnerBounds() const override
		{
			if (m_boundsSizing == ImageBoundsSizing::Stretch)
			{
				return m_bounds;
			}

			Rectangle<float> innerBounds = m_bounds;

			bool areBoundsProportionallyWider = m_bounds.getWidth() / m_bounds.getHeight() > m_image->GetSize().width / m_image->GetSize().height;
			if (m_boundsSizing == ImageBoundsSizing::Fill != areBoundsProportionallyWider)
			{
				innerBounds.setWidth(m_bounds.getHeight() * m_image->GetSize().width / m_image->GetSize().height);
			}
			else if (m_boundsSizing == ImageBoundsSizing::Contain != areBoundsProportionallyWider)
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

	class DirectWriteText : public Text
	{
	private:
		IDWriteTextLayout1* m_handle;
		std::wstring m_wideString;
		std::string m_string;
		bool m_isTopTrimmed;

		DWRITE_TEXT_RANGE createTextRange(int32 p_startPosition, int32 p_length)
		{
			DWRITE_TEXT_RANGE textRange = { };
			textRange.startPosition = p_length > 0 ? p_startPosition : max(0, p_startPosition - p_length);
			textRange.length = p_length > 0 ? p_length : (p_length == 0 ? m_string.size() : -p_length);
			return textRange;
		}

	public:
		DirectWriteText(IDWriteTextLayout1* p_handle, std::wstring const& p_wideString, std::string const& p_string, Rectangle<float> const& p_bounds) :
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
		Point<float> getMinimumSize() override
		{
			DWRITE_TEXT_METRICS metrics = { 0 };
			m_handle->GetMetrics(&metrics);

			DWRITE_OVERHANG_METRICS overhangMetrics = { 0 };
			m_handle->GetOverhangMetrics(&overhangMetrics);

			return Point<float>(metrics.width, m_handle->GetMaxHeight() + overhangMetrics.bottom + m_isTopTrimmed * overhangMetrics.top);
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

		Point<float> getCharacterPosition(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) override
		{
			Point<float> result;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestTextPosition(getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &result.x, &result.y, &metrics);
			if (p_isRelativeToOrigin)
			{
				result.x += getLeft();
				result.y += getTop();
			}
			return result;
		}
		Point<float> getCharacterSize(uint32 p_characterIndex) override
		{
			float x;
			float y;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestTextPosition(getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &x, &y, &metrics);
			return Point<float>(metrics.width, metrics.height);
		}
		Rectangle<float> getCharacterBounds(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) override
		{
			Rectangle<float> result;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestTextPosition(getUtf16UnitIndexFromCharacterIndex(m_wideString, p_characterIndex), false, &result.left, &result.top, &metrics);
			if (p_isRelativeToOrigin)
			{
				result.left += getLeft();
				result.top += getTop();
			}
			result.right = result.left + metrics.width;
			result.bottom = result.top + metrics.height;
			return result;
		}
		uint32 getNearestCharacterIndex(Point<float> const& p_point, bool p_isRelativeToOrigin = false) override
		{
			return getNearestCharacterIndex(p_point.x, p_point.y, p_isRelativeToOrigin);
		}
		uint32 getNearestCharacterIndex(float p_pointX, float p_pointY, bool p_isRelativeToOrigin = false) override
		{
			int isTrailingHit;
			int isInside;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
			return getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
		}
		void getNearestCharacterIndexAndPosition(Point<float> const& p_point, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) override
		{
			getNearestCharacterIndexAndPosition(p_point.x, p_point.y, p_outCharacterIndex, p_outCharacterPosition, p_isRelativeToOrigin);
		}
		void getNearestCharacterIndexAndPosition(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) override
		{
			int isTrailingHit;
			int isInside;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
			*p_outCharacterIndex = getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
			p_outCharacterPosition->set(metrics.left + isTrailingHit * metrics.width + p_isRelativeToOrigin * getLeft(), metrics.top + p_isRelativeToOrigin * getTop());
		}
		void getNearestCharacterIndexAndBounds(Point<float> const& p_point, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) override
		{
			getNearestCharacterIndexAndBounds(p_point.x, p_point.y, p_outCharacterIndex, p_outCharacterBounds, p_isRelativeToOrigin);
		}
		void getNearestCharacterIndexAndBounds(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) override
		{
			int isTrailingHit;
			int isInside;
			DWRITE_HIT_TEST_METRICS metrics = { 0 };
			m_handle->HitTestPoint(p_pointX - p_isRelativeToOrigin * getLeft(), p_pointY - p_isRelativeToOrigin * getTop(), &isTrailingHit, &isInside, &metrics);
			*p_outCharacterIndex = getCharacterIndexFromUtf16UnitIndex(m_wideString, metrics.textPosition + isTrailingHit * isInside);
			p_outCharacterBounds->left = metrics.left + isTrailingHit * metrics.width + p_isRelativeToOrigin * getLeft();
			p_outCharacterBounds->top = metrics.top + p_isRelativeToOrigin * getTop();
			p_outCharacterBounds->right = p_outCharacterBounds->left + metrics.width;
			p_outCharacterBounds->bottom = p_outCharacterBounds->top + metrics.height;
		}

		//------------------------------

		void setTextAlign(TextAlign p_textAlign) override
		{
			switch (p_textAlign)
			{
			case TextAlign::Left:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case TextAlign::Center:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case TextAlign::Right:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			case TextAlign::Fill:
				m_handle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
				break;
			}
		}
		TextAlign getTextAlign() override
		{
			switch (m_handle->GetTextAlignment())
			{
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING:
				return TextAlign::Left;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER:
				return TextAlign::Center;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING:
				return TextAlign::Right;
			case DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED:
				return TextAlign::Fill;
			}
		}

		void setReadingDirection(ReadingDirection p_readingDirection) override
		{
			switch (p_readingDirection)
			{
			case ReadingDirection::LeftToRight:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
				break;
			case ReadingDirection::RightToLeft:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
				break;
			case ReadingDirection::TopToBottom:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM);
				break;
			case ReadingDirection::BottomToTop:
				m_handle->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP);
				break;
			}
		}
		ReadingDirection getReadingDirection() override
		{
			switch (m_handle->GetReadingDirection())
			{
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT:
				return ReadingDirection::LeftToRight;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT:
				return ReadingDirection::RightToLeft;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM:
				return ReadingDirection::TopToBottom;
			case DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP:
				return ReadingDirection::BottomToTop;
			}
		}

		//------------------------------

		void setBounds(Rectangle<float> const& p_rectangle) override
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
		void setBounds(Point<float> const& p_position, Point<float> const& p_size) override
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
		Rectangle<float> const& getBounds() const override
		{
			return m_bounds;
		}

		//------------------------------

		void move(Point<float> const& p_offset) override
		{
			m_bounds += p_offset;
		}
		void move(float p_offsetX, float p_offsetY) override
		{
			m_bounds.move(p_offsetX, p_offsetY);
		}

		//------------------------------

		void setTopLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
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

		void setTopRight(Point<float> const& p_position, bool p_willKeepSize = true) override
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

		void setBottomLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
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

		void setBottomRight(Point<float> const& p_position, bool p_willKeepSize = true) override
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

		void setCenter(Point<float> const& p_position) override
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

		void setSize(Point<float> const& p_size) override
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

		bool getIsIntersecting(Rectangle<float> const& p_rectangle) const override
		{
			return m_bounds.getIsIntersecting(p_rectangle);
		}
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
		{
			return m_bounds.getIsIntersecting(p_protectedRectangle->getBounds());
		}

		//------------------------------

		bool getIsContaining(Rectangle<float> const& p_rectangle) const override
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
		bool getIsContaining(Point<float> const& p_point) const override
		{
			return m_bounds.getIsContaining(p_point);
		}

		//------------------------------

		void setFontFamily(char const* p_name, int32 p_startPosition, int32 p_length) override
		{
			wchar_t wideName[100];
			convertUtf8ToUtf16(p_name, wideName, 100);

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

		void setFontWeight(FontWeight p_fontWeight, int32 p_startPosition, int32 p_length) override
		{
			m_handle->SetFontWeight((DWRITE_FONT_WEIGHT)p_fontWeight, createTextRange(p_startPosition, p_length));
		}
		FontWeight getFontWeight(uint32 p_characterPosition) override
		{
			DWRITE_FONT_WEIGHT fontWeight;
			m_handle->GetFontWeight(p_characterPosition, &fontWeight);
			return (FontWeight)fontWeight;
		}

		//------------------------------

		void setFontStyle(FontStyle p_fontStyle, int32 p_startPosition = 0, int32 p_length = 0) override
		{
			m_handle->SetFontStyle((DWRITE_FONT_STYLE)p_fontStyle, createTextRange(p_startPosition, p_length));
		}
		FontStyle getFontStyle(uint32 p_characterPosition) override
		{
			DWRITE_FONT_STYLE directwriteFontStyle;
			m_handle->GetFontStyle(p_characterPosition, &directwriteFontStyle);
			return (FontStyle)directwriteFontStyle;
		}

		//------------------------------

		void setFontStretch(FontStretch p_fontStretch, int32 p_startPosition = 0, int32 p_length = 0) override
		{
			m_handle->SetFontStretch((DWRITE_FONT_STRETCH)p_fontStretch, createTextRange(p_startPosition, p_length));
		}
		FontStretch getFontStretch(uint32 p_characterPosition)
		{
			DWRITE_FONT_STRETCH fontStretch;
			m_handle->GetFontStretch(p_characterPosition, &fontStretch);
			return (FontStretch)fontStretch;
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

#pragma region Platform-specific drawing context implementations
#ifdef _WIN32
	class FontData
	{
	public:
		void const* data;
		uint32 dataSize;

		FontData(void const* p_data, uint32 p_dataSize) :
			data(p_data), dataSize(p_dataSize)
		{ }
	};

	class FontFileStream : public IDWriteFontFileStream
	{
	private:
		ULONG m_referenceCount = 0;
		FontData m_fontData;

	public:
		FontFileStream(FontData* p_fontData) : 
			m_fontData(*p_fontData)
		{ 
		}

		//------------------------------

		IUnknownDefinition(IDWriteFontFileStream)

		//------------------------------

		HRESULT __stdcall ReadFileFragment(void const** p_fragment, UINT64 p_fileOffset, UINT64 p_fragmentSize, void** p_fragmentContext) override
		{
			if (p_fileOffset + p_fragmentSize > m_fontData.dataSize || !p_fragmentSize)
			{
				*p_fragment = 0;
				*p_fragmentContext = 0;
				return E_FAIL;
			}

			*p_fragment = (char const*)m_fontData.data + p_fileOffset;
			*p_fragmentContext = 0;

			return S_OK;
		}
		void __stdcall ReleaseFileFragment(void* p_fragmentContext) override { }

		HRESULT __stdcall GetFileSize(UINT64* p_fileSize) override
		{
			*p_fileSize = m_fontData.dataSize;
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
		uint32 m_referenceCount = 0;

	public:
		FontFileLoader()
		{ 
		}

		//------------------------------

		IUnknownDefinition(IDWriteFontFileLoader)

		//------------------------------

		HRESULT __stdcall CreateStreamFromKey(void const* p_data, UINT32 p_dataSize, IDWriteFontFileStream** p_stream)
		{
			if (p_dataSize != sizeof(FontData**) || !p_data)
			{
				*p_stream = 0;
				return E_INVALIDARG;
			}
			*p_stream = new FontFileStream(*(FontData**)p_data);
			(*p_stream)->AddRef();
			return S_OK;
		}
	};
	class FontFileEnumerator : public IDWriteFontFileEnumerator
	{
	private:
		uint32 m_referenceCount = 0;

		//------------------------------

		IDWriteFactory* m_factory;
		FontFileLoader* m_fontFileLoader;

		std::vector<FontData*>* m_fontData;
		IDWriteFontFile* m_currentFontFile = 0;
		int32 m_currentFontFileIndex;

	public:
		FontFileEnumerator(IDWriteFactory* p_factory, FontFileLoader* p_fontFileLoader, std::vector<FontData*>* p_data) :
			m_factory(p_factory), m_fontFileLoader(p_fontFileLoader), m_fontData(p_data),
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
				m_factory->CreateCustomFontFileReference((void const*)(m_fontData->data() + m_currentFontFileIndex), sizeof(FontData**), m_fontFileLoader, &m_currentFontFile);
			}
			return S_OK;
		}
	};
	class FontCollectionLoader : public IDWriteFontCollectionLoader
	{
	private:
		uint32 m_referenceCount = 0;
		FontFileLoader* m_fontFileLoader;

	public:
		FontCollectionLoader(FontFileLoader* p_fontFileLoader) :
			m_fontFileLoader(p_fontFileLoader)
		{
		}

		//------------------------------

		IUnknownDefinition(IDWriteFontCollectionLoader)

		//------------------------------

		HRESULT __stdcall CreateEnumeratorFromKey(IDWriteFactory* p_factory, void const* p_data, UINT32 p_dataSize, IDWriteFontFileEnumerator** p_fontFileEnumerator)
		{
			*p_fontFileEnumerator = new FontFileEnumerator(p_factory, m_fontFileLoader, *((std::vector<FontData*>**)p_data));
			(*p_fontFileEnumerator)->AddRef();
			return S_OK;
		}
	};

	//------------------------------

	class Direct2DGeometry : 
		public Geometry
	{
	private:
		ID2D1Geometry* m_geometry;
		ID2D1GeometryRealization* m_strokedRealization = 0;
		ID2D1GeometryRealization* m_filledRealization = 0;

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
		public LinearGradient
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

		void setOffset(Point<float> const& p_offset) override
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
		Point<float> const& getOffset() const override
		{
			D2D1::Matrix3x2F matrix;
			m_brush->GetTransform(&matrix);
			return Point<float>(matrix._31, matrix._32);
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

		void setStartPosition(Point<float> const& p_startPosition) override
		{
			m_brush->SetStartPoint(D2D1::Point2F(p_startPosition.x, p_startPosition.y));
		}
		void setStartPosition(float p_x, float p_y) override
		{
			m_brush->SetStartPoint(D2D1::Point2F(p_x, p_y));
		}
		Point<float> const& getStartPosition() const override
		{
			return Point<float>(m_brush->GetStartPoint().x, m_brush->GetStartPoint().y);
		}
		float getStartPositionX() const override
		{
			return m_brush->GetStartPoint().x;
		}
		float getStartPositionY() const override
		{
			return m_brush->GetStartPoint().y;
		}

		void setEndPosition(Point<float> const& p_endPosition) override
		{
			m_brush->SetEndPoint(D2D1::Point2F(p_endPosition.x, p_endPosition.y));
		}
		void setEndPosition(float p_x, float p_y) override
		{
			m_brush->SetEndPoint(D2D1::Point2F(p_x, p_y));
		}
		Point<float> const& getEndPosition() const override
		{
			return Point<float>(m_brush->GetEndPoint().x, m_brush->GetEndPoint().y);
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
		public RadialGradient
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

		void setOffset(Point<float> const& p_offset) override
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
		Point<float> const& getOffset() const override
		{
			D2D1::Matrix3x2F matrix;
			m_brush->GetTransform(&matrix);
			return Point<float>(matrix._31, matrix._32);
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
		void setStartPosition(Point<float> const& p_startPosition) override
		{
			m_brush->SetCenter(D2D1::Point2F(p_startPosition.x, p_startPosition.y));
		}
		void setStartPosition(float p_x, float p_y) override
		{
			m_brush->SetCenter(D2D1::Point2F(p_x, p_y));
		}
		Point<float> const& getStartPosition() const override
		{
			return Point<float>(m_brush->GetCenter().x, m_brush->GetCenter().y);
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
		void setRadius(Point<float> const& p_radius) override
		{
			m_brush->SetRadiusX(p_radius.x);
			m_brush->SetRadiusY(p_radius.y);
		}
		void setRadius(float p_radiusX, float p_radiusY) override
		{
			m_brush->SetRadiusX(p_radiusX);
			m_brush->SetRadiusY(p_radiusY);
		}
		Point<float> const& getRadius() const override
		{
			return Point<float>(m_brush->GetRadiusX(), m_brush->GetRadiusY());
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

	class Direct2DDrawingState : DrawingState
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
		public DrawingContext
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
				s_fontFileLoader->AddRef();
				s_directWriteFactory->RegisterFontFileLoader(s_fontFileLoader);

				s_fontCollectionLoader = new FontCollectionLoader(s_fontFileLoader);
				s_fontCollectionLoader->AddRef();
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
		Window* m_window = 0;

		ID2D1DeviceContext1* m_context = 0;
		IDXGISwapChain1* m_swapChain = 0;
		ID2D1Bitmap1* m_targetWindowBitmap = 0;
		bool m_isVsyncEnabled;

		std::stack<bool> m_clipTypeStack;

		ID2D1SolidColorBrush* m_solidColorBrush = 0;
		ID2D1Brush* m_currentBrush = 0;
		float m_brushOpacity;

		D2D1_STROKE_STYLE_PROPERTIES1 m_strokeStyleProperties;
		ID2D1StrokeStyle1* m_strokeStyle = 0;

		Point<float> m_scale;

		IDWriteTextFormat* m_textFormat = 0;
		IDWriteFontCollection* m_fontCollection = 0;
		std::vector<FontData*> m_fontData;

		//------------------------------

		void createCornerRectangleGeometry(ID2D1PathGeometry1* p_geometry, float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, bool p_isFilled)
		{
			ID2D1GeometrySink* sink;
			p_geometry->Open(&sink);

			sink->SetFillMode(D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING);
			sink->BeginFigure(D2D1::Point2F(p_left, p_top + p_corners.topLeftSizeY), p_isFilled ? D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW);

			if (p_corners.topLeftSizeX && p_corners.topLeftSizeY)
			{
				if (p_corners.topLeftType == RectangleCornerType::Round)
				{
					sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(p_left + p_corners.topLeftSizeX, p_top), D2D1::SizeF(p_corners.topLeftSizeX, p_corners.topLeftSizeY), 0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL));
				}
				else
				{
					sink->AddLine(D2D1::Point2F(p_left + p_corners.topLeftSizeX, p_top));
				}
			}
			sink->AddLine(D2D1::Point2F(p_right - p_corners.topRightSizeX, p_top));
			if (p_corners.topRightSizeX && p_corners.topRightSizeY)
			{
				if (p_corners.topRightType == RectangleCornerType::Round)
				{
					sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(p_right, p_top + p_corners.topRightSizeY), D2D1::SizeF(p_corners.topRightSizeX, p_corners.topRightSizeY), 0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL));
				}
				else
				{
					sink->AddLine(D2D1::Point2F(p_right, p_top + p_corners.topRightSizeY));
				}
			}
			sink->AddLine(D2D1::Point2F(p_right, p_bottom - p_corners.bottomRightSizeY));
			if (p_corners.bottomRightSizeX && p_corners.bottomRightSizeY)
			{
				if (p_corners.bottomRightType == RectangleCornerType::Round)
				{
					sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(p_right - p_corners.bottomRightSizeX, p_bottom), D2D1::SizeF(p_corners.bottomRightSizeX, p_corners.bottomRightSizeY), 0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL));
				}
				else
				{
					sink->AddLine(D2D1::Point2F(p_right - p_corners.bottomRightSizeX, p_bottom));
				}
			}
			sink->AddLine(D2D1::Point2F(p_left + p_corners.bottomLeftSizeX, p_bottom));
			if (p_corners.bottomLeftSizeX && p_corners.bottomLeftSizeY)
			{
				if (p_corners.bottomLeftType == RectangleCornerType::Round)
				{
					sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(p_left, p_bottom - p_corners.bottomLeftSizeY), D2D1::SizeF(p_corners.bottomLeftSizeX, p_corners.bottomLeftSizeY), 0.f, D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL));
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
			std::vector<FontData*>* fontDataPointer = &m_fontData;
			s_directWriteFactory->CreateCustomFontCollection(s_fontCollectionLoader, &fontDataPointer, sizeof(std::vector<FontData*>**), &m_fontCollection);
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
		Direct2DDrawingContext(Window* p_window) :
			m_window(p_window), 
			m_isVsyncEnabled(true), 
			m_brushOpacity(1.f), 
			m_scale(1.f, 1.f)
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
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_LIGHT, FONT_DATA_SIZE_ROBOTO_LIGHT));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_REGULAR, FONT_DATA_SIZE_ROBOTO_REGULAR));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_MEDIUM, FONT_DATA_SIZE_ROBOTO_MEDIUM));
			m_fontData.push_back(new FontData(FONT_DATA_ROBOTO_BOLD, FONT_DATA_SIZE_ROBOTO_BOLD));
			m_fontData.push_back(new FontData(FONT_DATA_MATERIAL_ICONS, FONT_DATA_SIZE_MATERIAL_ICONS));
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
			for (uint32 a = 0; a < m_fontData.size(); a++)
			{
				delete m_fontData[a];
			}

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
			m_context->BeginDraw();
		}
		void finishDrawing(std::vector<Rectangle<float>> const& p_updatedRectangles) override
		{
			if (p_updatedRectangles.size())
			{
				m_context->EndDraw();

				DXGI_PRESENT_PARAMETERS presentParameters;
				presentParameters.DirtyRectsCount = p_updatedRectangles.size();

				//RECT* updatedRects = new RECT[p_updatedRectangles.size()];
				RECT updatedRects[500]; // This is more efficient than dynamic allocation... But it does feel dangerous to have an upper limit like this.

				float dpiFactor = getDpi() / USER_DEFAULT_SCREEN_DPI;

				// If you're getting an exception below, you have three options; 
				// 1. don't invalidate so damn many rectangles
				// 2. increase the size of the static array above
				// 3. make the array above dynamic (see the commented line above there), also don't forget to free it.
				for (uint32 a = 0; a < p_updatedRectangles.size(); a++)
				{
					updatedRects[a].left = p_updatedRectangles[a].left*dpiFactor;
					updatedRects[a].top = p_updatedRectangles[a].top*dpiFactor;
					updatedRects[a].right = p_updatedRectangles[a].right*dpiFactor;
					updatedRects[a].bottom = p_updatedRectangles[a].bottom*dpiFactor;
				}

				presentParameters.pDirtyRects = updatedRects;
				presentParameters.pScrollOffset = 0;
				presentParameters.pScrollRect = 0;

				m_swapChain->Present1(1, m_isVsyncEnabled ? 0 : (DXGI_PRESENT_DO_NOT_WAIT | DXGI_PRESENT_RESTART), &presentParameters);
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

		DrawingState* createDrawingState() override
		{
			ID2D1DrawingStateBlock1* drawingState = 0;
			s_direct2DFactory->CreateDrawingStateBlock(&drawingState);
			return (DrawingState*)(new Direct2DDrawingState(drawingState));
		}
		void saveDrawingState(DrawingState* p_drawingState) override
		{
			m_context->SaveDrawingState(((Direct2DDrawingState*)p_drawingState)->getHandle());
		}
		void restoreDrawingState(DrawingState* p_drawingState) override
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

		void setBackgroundColor(Color const& p_color) override
		{
			if (m_swapChain)
			{
				DXGI_RGBA dxgiColor;
				dxgiColor.r = p_color.red;
				dxgiColor.g = p_color.green;
				dxgiColor.b = p_color.blue;
				dxgiColor.a = p_color.alpha;

				m_swapChain->SetBackgroundColor(&dxgiColor);
			}
		}
		Color getBackgroundColor()
		{
			if (m_swapChain)
			{
				DXGI_RGBA dxgiColor;
				m_swapChain->GetBackgroundColor(&dxgiColor);

				return Color(dxgiColor.r, dxgiColor.g, dxgiColor.b, dxgiColor.a);
			}
			return Color(0.5f);
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

		void moveOrigin(Point<float> const& p_offset) override
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
		void setOrigin(Point<float> const& p_origin) override
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
		Point<float> getOrigin() override
		{
			D2D1_MATRIX_3X2_F transform;
			m_context->GetTransform(&transform);
			return Point<float>(transform.dx, transform.dy);
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
		void scale(float p_scale, Point<float> const& p_origin) override
		{
			scale(p_scale, p_scale, p_origin.x, p_origin.y);
		}
		void scale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) override
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
		void setScale(float p_scale, Point<float> const& p_origin) override
		{
			scale(p_scale / m_scale.x, p_scale / m_scale.y, p_origin.x, p_origin.y);
		}
		void setScale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) override
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
		Point<float> const& getScale() override
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
			m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / PI));
		}
		void rotate(float p_radians, Point<float> const& p_origin) override
		{
			D2D1::Matrix3x2F transform;
			m_context->GetTransform(&transform);
			m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / PI, D2D1::Point2F(p_origin.x, p_origin.y)));
		}
		void rotate(float p_radians, float p_originX, float p_originY) override
		{
			D2D1::Matrix3x2F transform;
			m_context->GetTransform(&transform);
			m_context->SetTransform(transform * D2D1::Matrix3x2F::Rotation(p_radians * 180.f / PI, D2D1::Point2F(p_originX, p_originY)));
		}

		//------------------------------

		void resetTransformations() override
		{
			m_context->SetTransform(D2D1::Matrix3x2F::Identity());
			m_scale.x = 1.f;
			m_scale.y = 1.f;
		}

		//------------------------------

		void setSize(Point<uint32> const& p_size) override
		{
			setSize(p_size.x, p_size.y);
		}
		void setSize(uint32 p_width, uint32 p_height) override
		{
			D2D1_SIZE_F oldSize = m_context->GetSize();
			//if (oldSize.width >= p_width && oldSize.width < p_width + 200 &&
				//oldSize.height >= p_height && oldSize.height < p_height + 200)
			if (oldSize.width == p_width && oldSize.height == p_height)
			{
				return;
			}

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

			// Resize buffers, creating new ones
			m_swapChain->ResizeBuffers(0, p_width*dpi / USER_DEFAULT_SCREEN_DPI, p_height*dpi / USER_DEFAULT_SCREEN_DPI, DXGI_FORMAT_UNKNOWN, 0);

			// Get the new back buffer and create new bitmap connected to it
			IDXGISurface* dxgiBackBuffer = 0;
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

			dxgiBackBuffer->Release();

			if (wasOldTargetWindow)
			{
				m_context->SetTarget(m_targetWindowBitmap);
			}
		}
		Point<uint32> getSize() override
		{
			IDXGISurface* dxgiBackBuffer;
			m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
			DXGI_SURFACE_DESC description;
			dxgiBackBuffer->GetDesc(&description);
			dxgiBackBuffer->Release();
			return Point<uint32>(description.Width, description.Height);
		}

		//------------------------------

		void clear(Color const& p_color) override
		{
			m_context->Clear(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
		}
		void clear() override
		{
			m_context->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));
		}

		//------------------------------

		void fillRectangle(Rectangle<float> const& p_rectangle) override
		{
			fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		void fillRectangle(Point<float> const& p_position, Point<float> const& p_size) override
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
		void fillRectangle(Point<float> const& p_size) override
		{
			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->FillRectangle(D2D1::RectF(0, 0, p_size.x, p_size.y), m_currentBrush);
		}
		void fillRectangle(float p_width, float p_height) override
		{
			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->FillRectangle(D2D1::RectF(0, 0, p_width, p_height), m_currentBrush);
		}

		void fillRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners) override
		{
			fillRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners);
		}
		void fillRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_corners) override
		{
			fillRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners);
		}
		void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners) override
		{
			ID2D1PathGeometry1* pathGeometry;
			s_direct2DFactory->CreatePathGeometry(&pathGeometry);
			createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, true);

			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->FillGeometry(pathGeometry, m_currentBrush);

			pathGeometry->Release();
		}

		void fillRectangle(Point<float> const& p_size, RectangleCorners const& p_corners) override
		{
			fillRectangle(0.f, 0.f, p_size.x, p_size.y, p_corners);
		}
		void fillRectangle(float p_width, float p_height, RectangleCorners const& p_corners) override
		{
			fillRectangle(0.f, 0.f, p_width, p_height, p_corners);
		}

		void fillRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius) override
		{
			fillRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius);
		}
		void fillRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius) override
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
		void fillRoundedRectangle(Point<float> const& p_size, float p_radius) override
		{
			fillRoundedRectangle(p_size.x, p_size.y, p_radius);
		}
		void fillRoundedRectangle(float p_width, float p_height, float p_radius) override
		{
			fillRoundedRectangle(0.f, 0.f, p_width, p_height, p_radius);
		}

		//------------------------------

		void strokeRectangle(Rectangle<float> const& p_rectangle, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_strokeWidth = 1.f) override
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
		void strokeRectangle(Point<float> const& p_size, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_size.x, p_size.y, p_strokeWidth);
		}
		void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(0.f, 0.f, p_width, p_height, p_strokeWidth);
		}

		void strokeRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_strokeWidth);
		}
		void strokeRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) override
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
		void strokeRoundedRectangle(Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(p_size.x, p_size.y, p_radius, p_strokeWidth);
		}
		void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) override
		{
			strokeRoundedRectangle(0.f, 0.f, p_width, p_height, p_radius, p_strokeWidth);
		}

		void strokeRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_strokeWidth);
		}
		void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners, p_strokeWidth);
		}
		void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
		{
			ID2D1PathGeometry1* pathGeometry;
			s_direct2DFactory->CreatePathGeometry(&pathGeometry);
			createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, false);

			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->DrawGeometry(pathGeometry, m_currentBrush, p_strokeWidth, m_strokeStyle);

			pathGeometry->Release();
		}
		void strokeRectangle(Point<float> const& p_size, RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(0.f, 0.f, p_size.x, p_size.y, p_corners, p_strokeWidth);
		}
		void strokeRectangle(float p_width, float p_height, RectangleCorners const& p_corners, float p_strokeWidth = 1.f) override
		{
			strokeRectangle(0.f, 0.f, p_width, p_height, p_corners, p_strokeWidth);
		}

		//------------------------------

		void fillCircle(Point<float> const& p_position, float p_radius) override
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

		void strokeCircle(Point<float> const& p_position, float p_radius, float p_strokeWidth = 1.f)
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

		void drawLine(Point<float> const& p_point_0, Point<float> const& p_point_1, float p_thickness = 1.f) override
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

		void strokeShape(Point<float> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed) override
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
		void strokeShape(std::vector<Point<float>> const& p_vertices, float p_lineThickness, bool p_isClosed) override
		{
			strokeShape(p_vertices.data(), p_vertices.size(), p_lineThickness, p_isClosed);
		}

		void fillShape(Point<float> const* p_vertices, uint32 p_numberOfVertices) override
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
		void fillShape(std::vector<Point<float>> const& p_vertices) override
		{
			fillShape(p_vertices.data(), p_vertices.size());
		}

		//------------------------------

		void strokeGeometry(Geometry* p_geometry, float p_strokeWidth) override
		{
			if (!((Direct2DGeometry*)p_geometry)->getStrokedRealization())
			{
				realizeStrokedGeometry((Direct2DGeometry*)p_geometry, p_strokeWidth);
			}
			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->DrawGeometryRealization(((Direct2DGeometry*)p_geometry)->getStrokedRealization(), m_currentBrush);
		}
		void fillGeometry(Geometry* p_geometry) override
		{
			if (!((Direct2DGeometry*)p_geometry)->getFilledRealization())
			{
				realizeFilledGeometry((Direct2DGeometry*)p_geometry);
			}
			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->DrawGeometryRealization(((Direct2DGeometry*)p_geometry)->getFilledRealization(), m_currentBrush);
		}

		//------------------------------

		Geometry* createRoundedRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius) override
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
		Geometry* createRoundedRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, float p_radius) override
		{
			return createRoundedRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_radius);
		}
		Geometry* createRoundedRectangleGeometry(Rectangle<float> const& p_rectangle, float p_radius) override
		{
			return createRoundedRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius);
		}
		Geometry* createRoundedRectangleGeometry(float p_width, float p_height, float p_radius) override
		{
			return createRoundedRectangleGeometry(0.f, 0.f, p_width, p_height, p_radius);
		}
		Geometry* createRoundedRectangleGeometry(Point<float> const& p_size, float p_radius) override
		{
			return createRoundedRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, p_radius);
		}

		Geometry* createCornerRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners) override
		{
			ID2D1PathGeometry1* pathGeometry = 0;
			s_direct2DFactory->CreatePathGeometry(&pathGeometry);
			createCornerRectangleGeometry(pathGeometry, p_left, p_top, p_right, p_bottom, p_corners, true);

			return new Direct2DGeometry((ID2D1Geometry*)pathGeometry);
		}
		Geometry* createCornerRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_corners)
		{
			return createCornerRectangleGeometry(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y, p_corners);
		}
		Geometry* createCornerRectangleGeometry(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners)
		{
			return createCornerRectangleGeometry(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners);
		}
		Geometry* createCornerRectangleGeometry(float p_width, float p_height, RectangleCorners const& p_corners)
		{
			return createCornerRectangleGeometry(0.f, 0.f, p_width, p_height, p_corners);
		}
		Geometry* createCornerRectangleGeometry(Point<float> const& p_size, RectangleCorners const& p_corners)
		{
			return createCornerRectangleGeometry(0.f, 0.f, p_size.x, p_size.y, p_corners);
		}

		//------------------------------

		Geometry* createPolygonGeometry(std::vector<Point<float>> const& p_vertices) override
		{
			return createPolygonGeometry(p_vertices.data(), p_vertices.size());
		}
		Geometry* createPolygonGeometry(Point<float> const* p_vertices, uint32 p_numberOfVertices) override
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

		void setLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case LineCap::Round:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case LineCap::Square:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case LineCap::Triangle:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
			updateStrokeStyle();
		}
		void setStartLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case LineCap::Round:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case LineCap::Square:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case LineCap::Triangle:
				m_strokeStyleProperties.startCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
			updateStrokeStyle();
		}
		void setEndLineCap(LineCap p_lineCap) override
		{
			switch (p_lineCap)
			{
			case LineCap::Flat:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case LineCap::Round:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case LineCap::Square:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case LineCap::Triangle:
				m_strokeStyleProperties.endCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
			updateStrokeStyle();
		}
		LineCap getStartLineCap() override
		{
			switch (m_strokeStyleProperties.startCap)
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
			switch (m_strokeStyleProperties.endCap)
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
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_BEVEL;
				break;
			case LineJoin::Miter:
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER;
				break;
			case LineJoin::Round:
				m_strokeStyleProperties.lineJoin = D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND;
				break;
			}
			updateStrokeStyle();
		}
		LineJoin getLineJoin() override
		{
			switch (m_strokeStyleProperties.lineJoin)
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
			m_strokeStyleProperties.miterLimit = p_miterLimit;
			updateStrokeStyle();
		}
		float getLineJoinMiterLimit() override
		{
			return m_strokeStyleProperties.miterLimit;
		}

		//------------------------------

		void setLineDashStyle(LineDashStyle p_dashStyle) override
		{
			switch (p_dashStyle)
			{
			case LineDashStyle::Solid:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID;
				break;
			case LineDashStyle::Dash:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH;
				break;
			case LineDashStyle::Dot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT;
				break;
			case LineDashStyle::DashDot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT;
				break;
			case LineDashStyle::DashDotDot:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_DASH_DOT_DOT;
				break;
			case LineDashStyle::Custom:
				m_strokeStyleProperties.dashStyle = D2D1_DASH_STYLE::D2D1_DASH_STYLE_CUSTOM;
			}
			updateStrokeStyle();
		}
		LineDashStyle getLineDashStyle() override
		{
			switch (m_strokeStyleProperties.dashStyle)
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
			m_strokeStyleProperties.dashOffset = p_dashOffset;
			updateStrokeStyle();
		}
		float getLineDashOffset() override
		{
			return m_strokeStyleProperties.dashOffset;
		}

		void setLineDashCap(LineCap p_dashCap) override
		{
			switch (p_dashCap)
			{
			case LineCap::Flat:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT;
				break;
			case LineCap::Round:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND;
				break;
			case LineCap::Square:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_SQUARE;
				break;
			case LineCap::Triangle:
				m_strokeStyleProperties.dashCap = D2D1_CAP_STYLE::D2D1_CAP_STYLE_TRIANGLE;
			}
			updateStrokeStyle();
		}
		LineCap getLineDashCap() override
		{
			switch (m_strokeStyleProperties.dashCap)
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

		void pushClipGeometry(Geometry* p_geometry, float p_opacity) override
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

		void pushClipShape(std::vector<Point<float>> const& p_points, float p_opacity) override
		{
			pushClipShape(p_points.data(), p_points.size(), p_opacity);
		}
		void pushClipShape(Point<float> const* p_points, uint32 p_numberOfPoints, float p_opacity) override
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
		void pushClipRectangle(Rectangle<float> const& p_rectangle, float p_opacity) override
		{
			pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_opacity);
		}
		void pushClipRectangle(Point<float> const& p_size, float p_opacity) override
		{
			pushClipRectangle(0.f, 0.f, p_size.x, p_size.y, p_opacity);
		}

		void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, float p_opacity)
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
		void pushClipRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners, float p_opacity)
		{
			pushClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_corners, p_opacity);
		}
		void pushClipRectangle(Point<float> const& p_size, RectangleCorners const& p_corners, float p_opacity)
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
		void pushRoundedClipRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_opacity) override
		{
			pushRoundedClipRectangle(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom, p_radius, p_opacity);
		}
		void pushRoundedClipRectangle(Point<float> const& p_size, float p_radius, float p_opacity) override
		{
			pushRoundedClipRectangle(0.f, 0.f, p_size.x, p_size.y, p_radius, p_opacity);
		}

		//------------------------------

		Image* createRectangleShadowImage(Point<uint32> const& p_size, float p_blur, Color const& p_color) override
		{
			return createRectangleShadowImage(p_size.x, p_size.y, p_blur, p_color);
		}
		Image* createRectangleShadowImage(uint32 p_width, uint32 p_height, float p_blur, Color const& p_color) override
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

			ID2D1Bitmap* outputBitmap;
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

			ID2D1Image* targetBefore = 0;
			m_context->GetTarget(&targetBefore);

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
			D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f * p_blur * dpiX / USER_DEFAULT_SCREEN_DPI, p_height + 6.f * p_blur * dpiY / USER_DEFAULT_SCREEN_DPI);

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
			m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur * 3.f * dpiX / USER_DEFAULT_SCREEN_DPI, p_blur * 3.f * dpiY / USER_DEFAULT_SCREEN_DPI));
			m_context->EndDraw();
			m_context->SetTarget(targetBefore);

			shadowEffect->Release();
			inputBitmap->Release();
			targetBefore->Release();

			return new Direct2DImage(outputBitmap);
		}
		Image* createRectangleShadowImage(Point<uint32> const& p_size, RectangleCorners const& p_corners, float p_blur, Color const& p_color) override
		{
			return createRectangleShadowImage(p_size.x, p_size.y, p_corners, p_blur, p_color);
		}
		Image* createRectangleShadowImage(uint32 p_width, uint32 p_height, RectangleCorners const& p_corners, float p_blur, Color const& p_color) override
		{
			if (!p_width || !p_height || !p_color.alpha) return 0;

			p_blur *= 2.f / 3.f;

			ID2D1Image* targetBefore = 0;
			m_context->GetTarget(&targetBefore);

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
			fillRectangle(p_width, p_height, p_corners);
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
			D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f * p_blur * dpiX / USER_DEFAULT_SCREEN_DPI, p_height + 6.f * p_blur * dpiY / USER_DEFAULT_SCREEN_DPI);

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
			m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur * 3.f * dpiX / USER_DEFAULT_SCREEN_DPI, p_blur * 3.f * dpiY / USER_DEFAULT_SCREEN_DPI));
			m_context->EndDraw();
			m_context->SetTarget(targetBefore);

			shadowEffect->Release();
			inputBitmap->Release();
			targetBefore->Release();

			return new Direct2DImage(outputBitmap);
		}

		Image* createRoundedRectangleShadowImage(Point<uint32> const& p_size, float p_radius, float p_blur, Color const& p_color)
		{
			return createRoundedRectangleShadowImage(p_size.x, p_size.y, p_radius, p_blur, p_color);
		}
		Image* createRoundedRectangleShadowImage(uint32 p_width, uint32 p_height, float p_radius, float p_blur, Color const& p_color)
		{
			if (!p_width || !p_height || !p_color.alpha) return 0;

			p_blur *= 2.f / 3.f;

			ID2D1Image* targetBefore = 0;
			m_context->GetTarget(&targetBefore);

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
			D2D1_SIZE_U outputSize = D2D1::SizeU(p_width + 6.f * p_blur * dpiX / USER_DEFAULT_SCREEN_DPI, p_height + 6.f * p_blur * dpiY / USER_DEFAULT_SCREEN_DPI);

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
			m_context->DrawImage(shadowEffect, D2D1::Point2F(p_blur * 3.f * dpiX / USER_DEFAULT_SCREEN_DPI, p_blur * 3.f * dpiY / USER_DEFAULT_SCREEN_DPI));
			m_context->EndDraw();
			m_context->SetTarget(targetBefore);

			shadowEffect->Release();
			inputBitmap->Release();
			targetBefore->Release();

			return new Direct2DImage(outputBitmap);
		}

		//------------------------------

		Image* createImage(void const* p_pixelData, uint32 p_width, uint32 p_height) override
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
		Image* createImage(void const* p_imageData, uint32 p_size) override
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

					ID2D1Bitmap* bitmap;
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
		Image* createImage(char const* p_filePath) override
		{
			wchar_t wideFilePath[200];
			convertUtf8ToUtf16(p_filePath, wideFilePath, 200);

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

				ID2D1Bitmap* bitmap;
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
		Image* createImage(void* p_handle) override
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

			ID2D1Bitmap* bitmap;
			m_context->CreateBitmapFromWicBitmap(formatConverter, &bitmap);

			formatConverter->Release();
			wicBitmap->Release();

			return new Direct2DImage(bitmap);
		}

		//------------------------------

		void drawImage(Image* p_image, float p_multiplicativeOpacity) override
		{
			if (!p_image)
			{
				return;
			}

			Rectangle<float> const& cropRectangle = p_image->getCropRectangle();
			Rectangle<float> innerBounds = p_image->getInnerBounds();

			m_currentBrush->SetOpacity(m_brushOpacity);
			m_context->DrawBitmap(
				(ID2D1Bitmap*)p_image->getHandle(),
				D2D1::RectF(innerBounds.left, innerBounds.top, innerBounds.right, innerBounds.bottom),
				p_image->getOpacity()* m_currentBrush->GetOpacity() * p_multiplicativeOpacity,
				p_image->getScalingMethod() == ImageScalingMethod::Pixelated ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(cropRectangle.left, cropRectangle.top, cropRectangle.right, cropRectangle.bottom)
			);
		}

		//------------------------------

		LinearGradient* createLinearGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_endX, float p_endY) override
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
		LinearGradient* createLinearGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_endPosition) override
		{
			return createLinearGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_endPosition.x, p_endPosition.y);
		}

		RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radiusX, float p_radiusY) override
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
		RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) override
		{
			return createRadialGradient(p_gradientStops, p_startX, p_startY, p_radius, p_radius);
		}
		RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radiusX, float p_radiusY) override
		{
			return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radiusX, p_radiusY);
		}
		RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radius) override
		{
			return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radius, p_radius);
		}
		RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_radius) override
		{
			return createRadialGradient(p_gradientStops, p_startPosition.x, p_startPosition.y, p_radius.x, p_radius.y);
		}

		void setGradient(LinearGradient* p_gradient) override
		{
			m_currentBrush = ((Direct2DLinearGradient*)p_gradient)->getHandle();
		}
		void setGradient(RadialGradient* p_gradient) override
		{
			m_currentBrush = ((Direct2DRadialGradient*)p_gradient)->getHandle();
		}

		void setColor(Color const& p_color) override
		{
			m_solidColorBrush->SetColor(D2D1::ColorF(p_color.red, p_color.green, p_color.blue, p_color.alpha));
			m_currentBrush = m_solidColorBrush;
		}
		void setOpacity(float p_opacity) override
		{
			m_brushOpacity = p_opacity;
		}

		//------------------------------

		void addFont(void const* p_data, uint32 p_dataSize)
		{
			m_fontData.push_back(new FontData(p_data, p_dataSize));
			updateFontCollection();
		}

		//------------------------------

		void setDefaultTextProperties(TextProperties const& p_textProperties) override
		{
			wchar_t fontFamily[100];
			convertUtf8ToUtf16(p_textProperties.fontFamilyName, fontFamily, 100);

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
				p_textProperties.fontSize, L"",
				(IDWriteTextFormat**)&m_textFormat
			);

			switch (p_textProperties.textAlign)
			{
			case TextAlign::Left:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case TextAlign::Center:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case TextAlign::Right:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			case TextAlign::Fill:
				m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
			}

			switch (p_textProperties.readingDirection)
			{
			case ReadingDirection::RightToLeft:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
				break;
			case ReadingDirection::TopToBottom:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_TOP_TO_BOTTOM);
				break;
			case ReadingDirection::BottomToTop:
				m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_BOTTOM_TO_TOP);
				break;
			}
			m_textFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD::DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, p_textProperties.lineHeight, p_textProperties.lineHeight * 0.8f);

			m_textProperties = p_textProperties;
		}
		TextProperties getDefaultTextProperties() override
		{
			return m_textProperties;
		}

		//------------------------------

		Text* createText(char const* p_string, float p_fontSize, Rectangle<float> const& p_bounds = Rectangle<float>()) override
		{
			int32 numberOfCharacters = MultiByteToWideChar(CP_UTF8, 0, p_string, -1, 0, 0);
			wchar_t* wideString = new wchar_t[numberOfCharacters];
			MultiByteToWideChar(CP_UTF8, 0, p_string, -1, wideString, numberOfCharacters);

			IDWriteTextLayout1* textLayout;
			s_directWriteFactory->CreateTextLayout(wideString, numberOfCharacters, m_textFormat, p_bounds.getWidth(), p_bounds.getHeight(), (IDWriteTextLayout**)&textLayout);
			DWRITE_TEXT_RANGE textRange;
			textRange.startPosition = 0;
			textRange.length = numberOfCharacters;
			textLayout->SetFontSize(p_fontSize, textRange);
			textLayout->SetCharacterSpacing(m_textProperties.characterSpacing*0.5f, m_textProperties.characterSpacing*0.5f, 0.f, textRange);

			std::wstring wideStringObject = wideString;
			delete[] wideString;

			return new DirectWriteText(textLayout, wideStringObject, p_string, p_bounds);
		}
		Text* createText(std::string const& p_string, float p_fontSize, Rectangle<float> const& p_bounds = Rectangle<float>()) override
		{
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
		void drawText(Text* p_text) override
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
			m_context->DrawTextLayout(D2D1::Point2F(p_text->getTopLeft().x, p_text->getTopLeft().y + overhangMetrics.top), textLayout, m_currentBrush, D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
		}
		void drawText(char const* p_string, Rectangle<float> const& p_rectangle) override
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
			drawText(p_string, Rectangle<float>(p_left, p_top, p_right, p_bottom));
		}
		void drawText(char const* p_string, Point<float> const& p_position, Point<float> const& p_size) override
		{
			drawText(p_string, Rectangle<float>(p_position, p_size));
		}
		void drawText(char const* p_string, float p_x, float p_y) override
		{
			drawText(p_string, Rectangle<float>(p_x, p_y, m_context->GetSize().width, m_context->GetSize().height));
		}
		void drawText(char const* p_string, Point<float> const& p_position) override
		{
			drawText(p_string, Rectangle<float>(p_position.x, p_position.y, m_context->GetSize().width * 2, m_context->GetSize().height * 2));
		}
	};
	ID2D1Factory2* Direct2DDrawingContext::s_direct2DFactory = 0;
	IDWriteFactory1* Direct2DDrawingContext::s_directWriteFactory = 0;
	FontCollectionLoader* Direct2DDrawingContext::s_fontCollectionLoader = 0;
	FontFileLoader* Direct2DDrawingContext::s_fontFileLoader = 0;
	IWICImagingFactory2* Direct2DDrawingContext::s_imagingFactory = 0;
#endif
#pragma endregion

	//------------------------------
	// class GUI
	//------------------------------

	//
	// Private
	//

	void Gui::getTopMouseListenersAt(Point<float> const& p_coordinates, std::vector<View*>& p_result)
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

		View* container = this;
		int32 startIndex = getNumberOfChildren() - 1;

		bool hasFoundTopView = false;

		while (true)
		{
		loopStart:
			for (int32 a = startIndex; a >= 0; a--)
			{
				View* child = container->getChild(a);
				// Invisible views and their children do not recieve mouse events.
				if (child->getIsVisible() && child->getIsContainingAbsolute(p_coordinates.x, p_coordinates.y))
				{
					bool hasChildren = child->getNumberOfChildren();

					if (hasChildren)
					{
						if (child->getAreMouseEventsEnabled())
						{
							child->remember();
							p_result.push_back(child);
						}
						container = child;
						startIndex = container->getNumberOfChildren() - 1;
						goto loopStart; // I have determined this is the least messy way to do it pls don't kill me
					}
					else
					{
						if (child->getAreMouseEventsEnabled())
						{
							child->remember();
							p_result.push_back(child);
						}

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
	void Gui::getTopMouseListenersAt(float p_x, float p_y, std::vector<View*>& p_result)
	{
		getTopMouseListenersAt(Point<float>(p_x, p_y), p_result);
	}

	void Gui::thread_runAnimationLoop()
	{
		int32 syncInterval = 16666667;
		auto timeBefore = std::chrono::steady_clock::now();

		bool wasLastFrameDrawn = false;

		while (!m_willClose)
		{
			updateQueuedAnimations();

			if (m_invalidRectangles.size())
			{
				drawViews();

				wasLastFrameDrawn = true;

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
					invalidateRectangle(Rectangle<float>(0, 0, 1, 1));
					drawViews();
					wasLastFrameDrawn = false;
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::nanoseconds(syncInterval));
				}
			}
			auto timeAfter = std::chrono::steady_clock::now();
			syncInterval = max(1000000, syncInterval + 0.5 * (16666667 - (timeAfter - timeBefore).count()));
			timeBefore = timeAfter;
		}

		// This will cause the window to be destroyed, because getWillClose() is true.
		m_window->close();
		forget(); // To allow for cleanup if the animation thread is detached.
	}

	//
	// Public
	//

	Gui::Gui() :
		View(0), 
		m_hasNewWindowSize(false), m_hasAnimationLoopStarted(false), m_willClose(false)
	{
#ifdef _WIN32
		Direct2DDrawingContext::createStaticResources();
		m_window = new WindowsWindow(this);
#endif

		m_gui = this;

		//------------------------------

		m_windowEventListeners.reserve(5);
		m_globalKeyboardEventListeners.reserve(20);
	}
	Gui::~Gui()
	{
		if (m_window)
		{
			m_window->forget();
			m_window = 0;
		}
		if (m_drawingContextState)
		{
			m_drawingContextState->forget();
			m_drawingContextState = 0;
		}
		if (m_drawingContext)
		{
			m_drawingContext->forget();
			m_drawingContext = 0;
		}
		for (auto pair : m_viewsById)
		{
			pair.second->m_id = 0;
		}
	}

	void Gui::create(char const* p_title, float p_x, float p_y, float p_width, float p_height, WindowStyleFlags p_windowFlags, Gui* p_parent)
	{
		if (p_parent)
		{
			m_parent = p_parent;
		}

		m_bounds = Rectangle<float>(0, 0, p_width, p_height);
		setAbsoluteBounds(m_bounds);
		m_window->create(p_title, p_x, p_y, p_width, p_height, p_windowFlags, p_parent ? p_parent->getWindow() : 0);

		m_animationThread = std::thread(&Gui::thread_runAnimationLoop, this);
	}
	void Gui::create(char const* p_title, float p_width, float p_height, WindowStyleFlags p_windowFlags, Gui* p_parent)
	{
		if (p_parent)
		{
			m_parent = p_parent;
		}

		m_bounds = Rectangle<float>(0, 0, p_width, p_height);
		setAbsoluteBounds(m_bounds);
		m_window->create(p_title, p_width, p_height, p_windowFlags, p_parent ? p_parent->getWindow() : 0);

		m_animationThread = std::thread(&Gui::thread_runAnimationLoop, this);
	}

	//------------------------------

	View* Gui::getViewAt(Point<float> const& p_coordinates)
	{
		View* currentContainer = this;

		while (true)
		{
			for (int32 a = currentContainer->getNumberOfChildren() - 1; a >= 0; a--)
			{
				View* view = currentContainer->getChild(a);
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
	View* Gui::getViewAt(float p_x, float p_y)
	{
		return getViewAt(Point<float>(p_x, p_y));
	}

	//------------------------------

	void Gui::handleWindowCreate(WindowEvent const& p_event)
	{
		if (m_drawingContext)
		{
			m_drawingContext->forget();
		}
#ifdef _WIN32
		m_drawingContext = new Direct2DDrawingContext(m_window);
#endif
		m_drawingContextState = m_drawingContext->createDrawingState();

		m_lastWindowSize = m_window->getSize();
		createContent();

		for (auto listener : m_windowEventListeners)
		{
			listener->handleWindowCreate(p_event);
		}
	}
	bool Gui::handleWindowClose(WindowEvent const& p_event)
	{
		bool willClose = true;
		for (auto listener : m_windowEventListeners)
		{
			if (!listener->handleWindowClose(p_event))
			{
				willClose = false;
			}
		}
		m_willClose = willClose;
		if (willClose)
		{
			if (m_animationUpdateQueue.size())
			{
				for (View* view : m_animationUpdateQueue)
				{
					view->forget();
				}
				m_animationUpdateQueue.clear();
			}
			if (m_pressedMouseEventListeners.size())
			{
				for (View* view : m_pressedMouseEventListeners)
				{
					view->forget();
				}
				m_pressedMouseEventListeners.clear();
			}
		}
		return willClose;
	}
	void Gui::handleWindowMinimize(WindowEvent const& p_event)
	{
		for (WindowListener* listener : m_windowEventListeners)
		{
			listener->handleWindowMinimize(p_event);
		}
	}
	void Gui::handleWindowMaximize(WindowEvent const& p_event)
	{
		for (WindowListener* listener : m_windowEventListeners)
		{
			listener->handleWindowMaximize(p_event);
		}
	}
	void Gui::handleWindowRestore(WindowEvent const& p_event)
	{
		for (WindowListener* listener : m_windowEventListeners)
		{
			listener->handleWindowRestore(p_event);
		}
	}
	void Gui::handleWindowSizeChange(WindowEvent const& p_event)
	{
		m_newWindowSize.set(p_event.width, p_event.height);
		m_hasNewWindowSize = true;

		for (auto listener : m_windowEventListeners)
		{
			listener->handleWindowSizeChange(p_event);
		}

		//if (!m_hasAnimationLoopStarted)
		//{
		//	m_hasAnimationLoopStarted = true;
		//}
	}
	void Gui::handleWindowFocus(WindowEvent const& p_event)
	{
		for (auto listener : m_windowEventListeners)
		{
			listener->handleWindowFocus(p_event);
		}
	}
	void Gui::handleWindowUnfocus(WindowEvent const& p_event)
	{
		for (auto listener : m_windowEventListeners)
		{
			listener->handleWindowUnfocus(p_event);
		}
	}

	//------------------------------

	DragDropOperation Gui::handleGlobalDragDropMove(DragDropEvent& p_event)
	{
		DragDropOperation finalOperation(DragDropOperation::None);
		uint32 finalOperationLayerIndex = 0; // The layer index of the view that updated finalOperation the last time.

		View* container = this;
		int32 startIndex = m_children.size() - 1;

		std::stack<bool> wasHoveringStack;
		wasHoveringStack.push(((View*)this)->m_isMouseHovering);

		if (getIsContaining(p_event.x, p_event.y))
		{
			if (m_areDragDropEventsEnabled)
			{
				if (((View*)this)->m_isMouseHovering)
				{
					finalOperation = handleDragDropMove(p_event);
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
			((View*)this)->m_isMouseHovering = true;
		}
		else if (((View*)this)->m_isMouseHovering)
		{
			if (m_areDragDropEventsEnabled)
			{
				handleDragDropLeave(p_event);

				if (startIndex < 0)
				{
					handleDragDropBackgroundLeave(p_event);
				}
			}
			((View*)this)->m_isMouseHovering = false;
		}

		if (startIndex >= 0)
		{
			float absoluteX = p_event.x;
			float absoluteY = p_event.y;

			bool hasInvisibleParent = false;
			bool hasOverlayParent = false;
			bool hasFoundEnterViews = false;
			bool hasFoundLeaveViews = false;
			while (true)
			{
			loopStart:
				for (int32 a = startIndex; a >= 0; a--)
				{
					View* child = container->m_children[a];

					if (container->m_isMouseHovering && child->getIsContainingAbsolute(absoluteX, absoluteY) && child->getIsVisible() && !hasInvisibleParent && !hasFoundEnterViews)
					{
						if (child->m_areDragDropEventsEnabled)
						{
							p_event.x = absoluteX - child->getAbsoluteLeft();
							p_event.y = absoluteY - child->getAbsoluteTop();
						}

						bool isContainer = child->m_children.size();

						if (child->m_isMouseHovering)
						{
							if (child->m_areDragDropEventsEnabled)
							{
								DragDropOperation operation = child->handleDragDropMove(p_event);
								if (child->m_layerIndex >= finalOperationLayerIndex)
								{
									finalOperation = operation;
									finalOperationLayerIndex = child->m_layerIndex;
								}
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
							wasHoveringStack.push(child->m_isMouseHovering);
							child->m_isMouseHovering = true;
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
					if (container->m_areDragDropEventsEnabled)
					{
						p_event.x = absoluteX - container->getAbsoluteLeft();
						p_event.y = absoluteY - container->getAbsoluteTop();
						container->handleDragDropBackgroundEnter(p_event);
					}
				}
				else if (wasHoveringStack.top() && container->m_isMouseHovering && hasFoundEnterViews && !hasFoundLeaveViews ||
					wasHoveringStack.top() && !container->m_isMouseHovering && !hasFoundLeaveViews)
				{
					hasFoundLeaveViews = true;
					if (container->m_areDragDropEventsEnabled)
					{
						p_event.x = absoluteX - container->getAbsoluteLeft();
						p_event.y = absoluteY - container->getAbsoluteTop();
						container->handleDragDropBackgroundLeave(p_event);
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

				if (container->getIsOverlay())
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
		return finalOperation;
	}
	void Gui::handleGlobalDragDropLeave(DragDropEvent& p_event)
	{
		if (m_isMouseHovering)
		{
			if (m_areDragDropEventsEnabled)
			{
				handleDragDropLeave(p_event);
				handleDragDropBackgroundLeave(p_event);
			}
			m_isMouseHovering = false;
		}

		float absoluteX = p_event.x;
		float absoluteY = p_event.y;

		View* container = this;
		int32 startIndex = m_children.size() - 1;
		int32 numberOfOverlayParents = 0;
		while (true)
		{
		loopStart:
			for (int32 a = startIndex; a >= 0; a--)
			{
				View* child = container->m_children[a];

				if (child->m_isMouseHovering)
				{
					if (child->m_areDragDropEventsEnabled)
					{
						p_event.x = absoluteX - child->getAbsoluteLeft();
						p_event.y = absoluteY - child->getAbsoluteTop();
						child->handleDragDropLeave(p_event);
						child->handleDragDropBackgroundLeave(p_event);
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

	void Gui::handleGlobalMouseMove(MouseEvent& p_event)
	{
		// This is false if it's called from a view just to send mouse leave and mouse enter events,
		// if a view has been moved from the mouse for example.
		bool wasMouseReallyMoved = p_event.movementX || p_event.movementY;

		float absoluteX = p_event.x;
		float absoluteY = p_event.y;

		if (m_pressedMouseEventListeners.size())
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
			View* container = this;
			int32 startIndex = m_children.size() - 1;

			std::stack<bool> wasHoveringStack;
			wasHoveringStack.push(((View*)this)->m_isMouseHovering);

			if (getIsContaining(p_event.x, p_event.y))
			{
				if (m_areMouseEventsEnabled)
				{
					if (((View*)this)->m_isMouseHovering)
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
				((View*)this)->m_isMouseHovering = true;
			}
			else if (((View*)this)->m_isMouseHovering)
			{
				if (getAreMouseEventsEnabled())
				{
					handleMouseLeave(p_event);

					if (startIndex < 0)
					{
						handleMouseBackgroundLeave(p_event);
					}
				}
				((View*)this)->m_isMouseHovering = false;
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
						View* child = container->m_children[a];

						if (container->m_isMouseHovering && child->getIsContainingAbsolute(absoluteX, absoluteY) && child->m_isVisible && !hasInvisibleParent && !hasFoundEnterViews)
						{
							if (child->m_areMouseEventsEnabled)
							{
								p_event.x = absoluteX - child->getAbsoluteLeft();
								p_event.y = absoluteY - child->getAbsoluteTop();
							}

							bool isContainer = child->m_children.size();

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

		if (m_globalMouseEventListeners.size() && wasMouseReallyMoved)
		{
			p_event.x = absoluteX;
			p_event.y = absoluteY;
			for (auto listener : m_globalMouseEventListeners)
			{
				listener->handleGlobalMouseMove(p_event);
			}
		}
	}
	void Gui::handleGlobalMouseLeave(MouseEvent& p_event)
	{
		if (m_pressedMouseEventListeners.size())
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

		View* container = this;
		int32 startIndex = m_children.size() - 1;
		int32 numberOfOverlayParents = 0;
		while (true)
		{
			loopStart:
			for (int32 a = startIndex; a >= 0; a--)
			{
				View* child = container->m_children[a];

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

	void Gui::invalidateRectangle(Rectangle<float> p_rectangle)
	{
		p_rectangle.bound(m_bounds);

		if (p_rectangle.getWidth() == 0.f || p_rectangle.getHeight() == 0.f)
		{
			return;
		}

		int32 rectangleIndex = -1;
		Rectangle<float>* rectangle = 0;

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

	void Gui::drawViews()
	{
		if (m_invalidRectangles.size())
		{
			m_invalidRectanglesMutex.lock();
			std::vector<Rectangle<float>> invalidRectangles = std::move(m_invalidRectangles);
			m_invalidRectanglesMutex.unlock();
			//std::cout << "Invalid rectangles:\n";
			//for (auto rect : invalidRectangles)
			//{
			//	std::cout << "(" << rect.left << ", " << rect.top << ", " << rect.right << ", " << rect.bottom << ")\n";
			//}
			//std::cout << "\n\n";
			//includeAnimationThread();

			excludeAnimationThread(); // State needs to be static during drawing.

			m_drawingContext->beginDrawing();

			for (auto const& targetRectangle : invalidRectangles)
			{
				View* currentContainer = this;
				uint32 startPosition = 0;

				m_drawingContext->resetTransformations();
				m_drawingContext->setOpacity(1.f);
				m_drawingContext->pushClipRectangle(targetRectangle);
				m_drawingContext->clear(m_theme->colors["background"]);

				draw(m_drawingContext, targetRectangle);

				while (true)
				{
					bool isDoneWithContainer = true;
					for (uint32 a = startPosition; a < currentContainer->getNumberOfChildren(); a++)
					{
						View* view = currentContainer->getChild(a);

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
			m_drawingContext->restoreDrawingState(m_drawingContextState);
			includeAnimationThread();
			m_drawingContext->finishDrawing(invalidRectangles);
		}
	}

	//------------------------------
	// class OpenFileDialog
	//------------------------------

	void OpenFileDialog::open(std::vector<std::wstring>& p_openedFilePaths)
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
	void OpenFileDialog::open(std::vector<std::string>& p_openedFilePaths)
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
};
