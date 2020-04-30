#pragma once

#include "../../AvoGUI.hpp"

class IconButton : public AvoGUI::View
{
public:
	static constexpr auto PADDING = 0.8f;

private:
	AvoGUI::Text* m_text{ nullptr };
	float m_size;
public:
	void setIcon(std::string const& p_icon)
	{
		if (m_text)
		{
			m_text->forget();
		}
		m_text = getDrawingContext()->createText(p_icon, m_size);
		m_text->setFontFamily(AvoGUI::FONT_FAMILY_MATERIAL_ICONS);
		m_text->setIsTopTrimmed(true);
		m_text->fitSizeToText();
		m_text->setCenter(getSize() * 0.5f);
		invalidate();
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		if (m_text)
		{
			p_context->setColor(getThemeColor(AvoGUI::ThemeColors::primary));
			p_context->drawText(m_text);
		}
	}
	void drawOverlay(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(getThemeColor(AvoGUI::ThemeColors::onBackground), 0.5f));
		p_context->strokeRectangle(getSize(), getCorners());
	}

	//------------------------------

private:
	std::string m_tooltipString;
public:
	void setTooltip(std::string const& p_string)
	{
		m_tooltipString = p_string;
	}

	void handleMouseEnter(AvoGUI::MouseEvent const&) override;
	void handleMouseLeave(AvoGUI::MouseEvent const&) override;

	//------------------------------

	AvoGUI::EventListeners<void()> buttonClickListeners;

	void handleMouseUp(AvoGUI::MouseEvent const& p_event) override
	{
		if (getSize().getIsContaining(p_event.x, p_event.y) && p_event.mouseButton == AvoGUI::MouseButton::Left)
		{
			buttonClickListeners();
		}
	}

public:
	IconButton(AvoGUI::View* p_parent, float p_size) :
		View(p_parent, { 0.f, 0.f, p_size * (1.f + PADDING), p_size * (1.f + PADDING) }),
		m_size(p_size)
	{
		new AvoGUI::Ripple(this, AvoGUI::Color(getThemeColor(AvoGUI::ThemeColors::primaryOnBackground), 0.4f));
		setCornerRadius(getWidth()*0.5f);
		enableMouseEvents();
		setCursor(AvoGUI::Cursor::Hand);
	}
};
