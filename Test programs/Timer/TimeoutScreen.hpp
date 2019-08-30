#pragma once
#include "../../AvoGUI.hpp"

class TimerApp;

class TimeoutScreen : public AvoGUI::View
{
private:
	AvoGUI::Text* m_text_message;
	AvoGUI::Text* m_text_dismiss;
	float m_backgroundHue;
	bool m_willStopSound;

public:
	TimeoutScreen(TimerApp* p_timerApp);
	~TimeoutScreen()
	{
		m_text_message->forget();
		m_text_dismiss->forget();
	}

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		m_willStopSound = true;
	}

	void updateAnimations() override;

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->clear(AvoGUI::Color().setHSBA(m_backgroundHue, 1.f, 0.8f));

		p_context->setColor(~0u);
		p_context->drawText(m_text_message);

		p_context->setColor(AvoGUI::Color(0.8f));
		p_context->drawText(m_text_dismiss);
	}
};
