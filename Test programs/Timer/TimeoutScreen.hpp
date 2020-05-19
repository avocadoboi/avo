#pragma once
#include "../../AvoGUI.hpp"

class TimeoutScreen : public Avo::View
{
private:
	Avo::Text m_text_message{ getDrawingContext()->createText("Time's up!", 50.f) };
	Avo::Text m_text_dismiss{ getDrawingContext()->createText("(press anywhere to dismiss)", 12.f) };
	float m_backgroundHue{ 0.f };
	bool m_willStopSound{ false };

public:
	TimeoutScreen(Avo::View* p_timerApp) :
		View(p_timerApp, p_timerApp->getBounds())
	{
		enableMouseEvents();
		getWindow()->setCursor(Avo::Cursor::Arrow);

		m_text_message.setCenter(getCenterX(), getCenterY() - 20.f);
		m_text_dismiss.setCenter(getCenterX(), getCenterY() + 40.f);
	}

	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		m_willStopSound = true;
	}

	void updateAnimations() override;

	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->clear(Avo::Color().setHSBA(m_backgroundHue, 1.f, 0.8f));
		p_context->setColor(~0u);
		p_context->drawText(m_text_message);

		p_context->setColor(0.8f);
		p_context->drawText(m_text_dismiss);
	}
};
