#pragma once

#include "../../AvoGUI.hpp"

class IconButton : public Avo::View
{
public:
	static constexpr auto PADDING = 0.8f;

private:
	Avo::Text m_text;
	float m_size;
public:
	void setIcon(std::string const& p_icon)
	{
		m_text = getDrawingContext()->createText(p_icon, m_size);
		m_text.setFontFamily(Avo::FONT_FAMILY_MATERIAL_ICONS);
		m_text.setIsTopTrimmed(true);
		m_text.fitSizeToText();
		m_text.setCenter(getSize() * 0.5f);
		invalidate();
	}

	//------------------------------

	void draw(Avo::DrawingContext* p_context) override
	{
		if (m_text)
		{
			p_context->setColor(getThemeColor(Avo::ThemeColors::primary));
			p_context->drawText(m_text);
		}
	}
	void drawOverlay(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(Avo::Color(getThemeColor(Avo::ThemeColors::onBackground), 0.2f));
		p_context->strokeRectangle(getSize(), getCorners(), 2.f);
	}

	//------------------------------

private:
	std::string m_tooltipString;
public:
	void setTooltip(std::string const& p_string)
	{
		m_tooltipString = p_string;
	}

	void handleMouseEnter(Avo::MouseEvent const&) override;
	void handleMouseLeave(Avo::MouseEvent const&) override;

	//------------------------------

	Avo::EventListeners<void()> buttonClickListeners;

	void handleMouseUp(Avo::MouseEvent const& p_event) override
	{
		if (getSize().getIsContaining(p_event.x, p_event.y) && p_event.mouseButton == Avo::MouseButton::Left)
		{
			buttonClickListeners();
		}
	}

public:
	IconButton(Avo::View* p_parent, float p_size) :
		View{ p_parent, { 0.f, 0.f, p_size * (1.f + PADDING), p_size * (1.f + PADDING) } },
		m_size{ p_size }
	{
		new Avo::Ripple{ this, Avo::Color{ getThemeColor(Avo::ThemeColors::primaryOnBackground), 0.4f } };
		setCornerRadius(getWidth()*0.5f);
		enableMouseEvents();
		setCursor(Avo::Cursor::Hand);
	}
};
