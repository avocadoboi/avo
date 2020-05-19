#pragma once

#include "MainScreen.hpp"
#include "TimeoutScreen.hpp"

#include <chrono>
#include <math.h>

//------------------------------

class TimerApp : public Avo::Gui
{
private:
	MainScreen* m_mainScreen{ nullptr };
	TimeoutScreen* m_timeoutScreen{ nullptr };
	Avo::Tooltip* m_tooltip{ nullptr };

public:
	TimerApp()
	{
		create("The best timer that exists", 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), Avo::WindowStyleFlags::DefaultNoResize);
		
		m_tooltip = new Avo::Tooltip(this);

		m_mainScreen = new MainScreen(this);
		m_timeoutScreen = new TimeoutScreen(this);
		m_timeoutScreen->setIsVisible(false);

		run();
	}

	//------------------------------

	Avo::Tooltip* getTooltip()
	{
		return m_tooltip;
	}

	//------------------------------

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
};
