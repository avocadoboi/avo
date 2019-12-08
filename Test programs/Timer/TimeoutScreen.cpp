#include "TimeoutScreen.hpp"
#include "Timer.hpp"

TimeoutScreen::TimeoutScreen(TimerApp* p_timerApp) :
	View(p_timerApp, p_timerApp->getBounds()),
	m_text_message(0), m_text_dismiss(0), m_backgroundHue(0.f),
	m_willStopSound(false)
{
	enableMouseEvents();

	getGui()->getWindow()->setCursor(AvoGUI::Cursor::Arrow);

	m_text_message = getGui()->getDrawingContext()->createText("Time's up!", 50.f);
	m_text_message->setCenter(getCenterX(), getCenterY() - 20.f);
	
	m_text_dismiss = getGui()->getDrawingContext()->createText("(press anywhere to dismiss)", 12.f);
	m_text_dismiss->setCenter(getCenterX(), getCenterY() + 40.f);
}

void TimeoutScreen::updateAnimations()
{
	if (m_willStopSound)
	{
		((TimerApp*)getGui())->stopTimerSound();
		((TimerApp*)getGui())->showMainScreen();
		m_willStopSound = false;
	}
	else
	{
		m_backgroundHue += 0.01f;
		invalidate();
		queueAnimationUpdate();
	}
}

