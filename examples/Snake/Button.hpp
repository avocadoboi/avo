#pragma once

#include "Theme.hpp"
#include "Strings.hpp"

namespace Avo::ThemeValues
{
	Id buttonSize;
}

class Button : public Avo::View
{
public:
	static constexpr auto thickness = 5.f;
	static constexpr auto cornerRadius = 5.f;
	static constexpr auto sizeFactor = Avo::Size<>{1.f, 2.f};

	Avo::EventListeners<void()> clickListeners;

private:
	float m_hoverValue{};
	Avo::Animation m_hoverAnimation{
		getGui(), getThemeEasing(ThemeEasings::out), 200ms,
		[this](float const p_value) {
			m_hoverValue = p_value;
			invalidate();
		}
	};
	Avo::Initializer m_init_hover = [this]{
		mouseEnterListeners += [this](auto const&){ m_hoverAnimation.play(false); };
		mouseLeaveListeners += [this](auto const&){ m_hoverAnimation.play(true); };
	};
	
	//------------------------------

	float m_pressValue{};
	Avo::Animation m_pressAnimation{
		getGui(), getThemeEasing(ThemeEasings::out), 200ms,
		[this](float const p_value) {
			m_pressValue = p_value;
			invalidate();
		}
	};
	auto handleMouseDown(Avo::MouseEvent const&) -> void
	{
		m_pressAnimation.play(false);
	}
	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void
	{
		m_pressAnimation.play(true);

		if (getSize().getIsContaining(p_event.xy) && p_event.mouseButton == Avo::MouseButton::Left) 
		{
			clickListeners();
		}
	}

	//------------------------------

	Avo::Text m_text;

public:
	auto draw(Avo::DrawingContext* const p_context) -> void override
	{
		auto const primary = getThemeColor(ThemeColors::primary);

		// Background button
		p_context->setColor(primary * 0.7f);
		p_context->fillRoundedRectangle({0.f, thickness, getWidth(), getHeight()}, cornerRadius);

		// Foreground button
		p_context->moveOrigin({0.f, m_pressValue*thickness});
		p_context->setColor(primary);
		p_context->fillRoundedRectangle({getWidth(), getHeight() - thickness}, cornerRadius);

		// Button text
		p_context->setColor(getThemeColor(ThemeColors::onPrimary));
		p_context->drawText(m_text);

		// Hover highlight
		p_context->setColor({getThemeColor(ThemeColors::onBackground), 0.2f*m_hoverValue});
		p_context->fillRoundedRectangle(getSize(), cornerRadius);
	}

	Button(View* const p_parent, std::string_view const p_string) :
		View{p_parent}
	{
		initializeThemeValue(ThemeValues::buttonSize, 12.f);

		setCornerRadius(cornerRadius);
		enableMouseEvents();
		setCursor(Avo::Cursor::Hand);

		auto const size = getThemeValue(ThemeValues::buttonSize);
		m_text = getDrawingContext()->createText(p_string, size);
		setSize(Avo::Size{m_text.getWidth(), 0.f} + size * sizeFactor);
		m_text.setCenter(Avo::Point{getSize()/2});
	}
};
