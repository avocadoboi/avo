#pragma once

#include "MainScreen.hpp"
#include "TimeoutScreen.hpp"

#include <chrono>
#include <math.h>

//------------------------------

class TimerApp : public Avo::Gui
{
private:
	Avo::Tooltip* m_tooltip{};
public:
	auto getTooltip() const -> Avo::Tooltip*
	{
		return m_tooltip;
	}

	//------------------------------

private:
	TimeoutScreen* m_timeoutScreen{};
public:
	auto showTimeoutScreen() -> void
	{
		m_mainScreen->setIsVisible(false);
		m_timeoutScreen->setIsVisible(true);
		m_timeoutScreen->queueAnimationUpdate();
		invalidate();
	}

private:
	MainScreen* m_mainScreen{};
public:
	auto showMainScreen() -> void
	{
		m_timeoutScreen->setIsVisible(false);
		m_mainScreen->setIsVisible(true);
		m_mainScreen->queueAnimationUpdate();
		invalidate();
	}

	//------------------------------

	TimerApp()
	{
		create("The best timer that exists", {2.f*(timerSpiralRadius + timerSpiralPadding)}, Avo::WindowStyleFlags::DefaultNoResize);
		
		m_tooltip = new Avo::Tooltip{this};

		m_mainScreen = new MainScreen{this};

		m_timeoutScreen = new TimeoutScreen{this};
		m_timeoutScreen->setIsVisible(false);

		run();
	}
};
