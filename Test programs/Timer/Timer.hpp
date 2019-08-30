#pragma once
#include <chrono>
#include <math.h>

#include "../../AvoGUI.hpp"
#include "MainScreen.hpp"
#include "TimeoutScreen.hpp"

//------------------------------

class TimerApp : public AvoGUI::GUI
{
private:
	MainScreen* m_mainScreen;
	TimeoutScreen* m_timeoutScreen;

public:
	TimerApp() : 
		m_mainScreen(0), m_timeoutScreen(0)
	{
		create("The best timer that exists", 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), AvoGUI::WindowStyleFlags::DefaultNoResize);
		run();
	}

	//------------------------------

	void createContent() override
	{
		m_mainScreen = new MainScreen(this);
		m_timeoutScreen = new TimeoutScreen(this);
		m_timeoutScreen->setIsVisible(false);
	}

	void showTimeoutScreen()
	{
		m_mainScreen->setIsVisible(false);
		m_timeoutScreen->setIsVisible(true);
		m_timeoutScreen->queueAnimationUpdate();
		invalidate();
	}
	void showMainScreen()
	{
		m_timeoutScreen->setIsVisible(false);
		m_mainScreen->setIsVisible(true);
		m_mainScreen->queueAnimationUpdate();
		invalidate();
	}
	void stopTimerSound()
	{
		m_mainScreen->stopTimerSound();
	}
};
