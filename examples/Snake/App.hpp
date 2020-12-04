#pragma once

#include "StartScreen.hpp"

//------------------------------

class App : public Avo::Gui
{
public:
	static constexpr auto size = Avo::Size{600.f, 700.f};

	auto setupTheme() -> void
	{
		setThemeColor(ThemeColors::background, {0.1f});
		setThemeColor(ThemeColors::onBackground, {1.f});
	}

private:
	View* m_screen{};
public:
	auto setScreen(View* p_screen) -> void
	{
		if (m_screen)
		{
			m_screen->setParent(nullptr);
		}
		m_screen = p_screen;
	}

	App()
	{
		create(Strings::title, size, Avo::WindowStyleFlags::DefaultNoResize);

		setupTheme();

		setScreen(new StartScreen{this});

		run();
	}
};
