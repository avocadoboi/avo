#pragma once

#include "../../AvoGUI.hpp"

class IconButton : public Avo::View
{
public:
	static constexpr auto PADDING = 0.8f;

private:
	Avo::Text m_text;
	float m_size{};
public:
	auto setIcon(std::string_view const p_icon) -> void
	{
		m_text = getDrawingContext()->createText(p_icon, m_size);
		m_text.setFontFamily(Avo::FONT_FAMILY_MATERIAL_ICONS);
		m_text.setIsTopTrimmed(true);
		m_text.fitSizeToText();
		m_text.setCenter(Avo::Point{getSize() * 0.5f});
		invalidate();
	}

	//------------------------------

	auto draw(Avo::DrawingContext* const p_context) -> void override
	{
		if (m_text)
		{
			p_context->setColor(getThemeColor(Avo::ThemeColors::primary));
			p_context->drawText(m_text);
		}
	}
	auto drawOverlay(Avo::DrawingContext* const p_context) -> void override
	{
		p_context->setColor({getThemeColor(Avo::ThemeColors::onBackground), 0.2f});
		p_context->strokeRectangle(getSize(), getCorners(), 2.f);
	}

	//------------------------------

private:
	std::string m_tooltipString;
public:
	auto setTooltip(std::string_view const p_string) -> void
	{
		m_tooltipString = p_string;
	}

	auto handleMouseEnter(Avo::MouseEvent const&) -> void override;
	auto handleMouseLeave(Avo::MouseEvent const&) -> void override;

	//------------------------------

	Avo::EventListeners<void()> buttonClickListeners;

	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void override
	{
		if (getSize().getIsContaining(p_event.xy) && p_event.mouseButton == Avo::MouseButton::Left)
		{
			buttonClickListeners();
		}
	}

public:
	IconButton(Avo::View* const p_parent, float const p_size) :
		View{p_parent, Avo::Size{p_size * (1.f + PADDING)}},
		m_size{p_size}
	{
		enableMouseEvents();
		setCornerRadius(getWidth()*0.5f);
		setCursor(Avo::Cursor::Hand);

		new Avo::Ripple{this, Avo::Color{getThemeColor(Avo::ThemeColors::primaryOnBackground), 0.4f}};
	}
};
