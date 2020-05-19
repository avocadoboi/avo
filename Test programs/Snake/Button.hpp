#pragma once

#include "Theme.hpp"

namespace Avo::ThemeValues
{
	Id buttonSize;
}

class Button : public Avo::View
{
public:
	static constexpr float THICKNESS = 5.f;
	static constexpr float CORNER_RADIUS = 5.f;
	static constexpr float WIDTH = 1.f;
	static constexpr float HEIGHT = 2.f;

	Avo::EventListeners<void()> clickListeners;

private:
	Avo::Text m_text;

	float m_hoverValue = 0.f;
	float m_pressValue = 0.f;

public:
	void draw(Avo::DrawingContext* p_context) override
	{
		Avo::Color primary = getThemeColor(ThemeColors::primary);
		p_context->setColor(primary * 0.7f);
		p_context->fillRoundedRectangle({ 0.f, THICKNESS, getWidth(), getHeight() }, CORNER_RADIUS);

		p_context->moveOrigin(0.f, m_pressValue*THICKNESS);
		p_context->setColor(primary);
		p_context->fillRoundedRectangle(getWidth(), getHeight() - THICKNESS, CORNER_RADIUS);

		p_context->setColor(getThemeColor(ThemeColors::onPrimary));
		p_context->drawText(m_text);

		p_context->setColor({ getThemeColor(ThemeColors::onBackground), 0.2f*m_hoverValue });
		p_context->fillRoundedRectangle(getSize(), CORNER_RADIUS);
		p_context->moveOrigin(0.f, -m_pressValue * THICKNESS);
	}

	Button(View* p_parent, std::string const& p_string) :
		View{ p_parent }
	{
		initializeThemeValue(ThemeValues::buttonSize, 12.f);

		setCornerRadius(CORNER_RADIUS);

		auto size = getThemeValue(ThemeValues::buttonSize);
		m_text = getDrawingContext()->createText(p_string, size);

		setSize(m_text.getWidth() + size * WIDTH, size * HEIGHT);

		m_text.setCenter(getSize() * 0.5f);
		
		auto pressAnimation = createAnimation(ThemeEasings::out, 200.f);
		pressAnimation->updateListeners += [=](float p_value) {
			m_pressValue = p_value;
			invalidate();
		};

		enableMouseEvents();
		setCursor(Avo::Cursor::Hand);
		mouseDownListeners += [=](Avo::MouseEvent const&) {
			pressAnimation->play(false);
		};
		mouseUpListeners += [=](Avo::MouseEvent const& p_event) {
			pressAnimation->play(true);

			if (getSize().getIsContaining(p_event.x, p_event.y) && p_event.mouseButton == Avo::MouseButton::Left)
			{
				clickListeners();
			}
		};

		auto hoverAnimation = createAnimation(ThemeEasings::out, 200.f);
		hoverAnimation->updateListeners += [=](float p_value) {
			m_hoverValue = p_value;
			invalidate();
		};

		mouseEnterListeners += [=](Avo::MouseEvent const&) {
			hoverAnimation->play(false);
		};
		mouseLeaveListeners += [=](Avo::MouseEvent const&) {
			hoverAnimation->play(true);
		};
	}
};
