#include "TimeoutScreen.hpp"
#include "Timer.hpp"

void TimeoutScreen::updateAnimations()
{
	if (m_willStopSound)
	{
		getViewById<SoundOpener>(Ids::soundOpener)->stopSound();
		getGui<TimerApp>()->showMainScreen();
		m_willStopSound = false;
	}
	else
	{
		m_backgroundHue += 0.01f;
		invalidate();
		queueAnimationUpdate();
	}
}

