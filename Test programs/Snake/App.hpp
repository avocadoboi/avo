#pragma once

#include "Strings.hpp"
#include "StartScreen.hpp"

#include "../../AvoGUI.hpp"

//------------------------------

class App : public Avo::Gui
{
public:
	static constexpr float WIDTH = 600.f;
	static constexpr float HEIGHT = 700.f;

	void setupTheme()
	{
		setThemeColor(ThemeColors::background, { 0.1f });
		setThemeColor(ThemeColors::onBackground, { 1.f });
	}

private:
	View* m_screen = nullptr;
public:
	void setScreen(View* p_screen)
	{
		if (m_screen)
		{
			m_screen->setParent(nullptr);
		}
		m_screen = p_screen;
	}

	App()
	{
		create(Strings::title, WIDTH, HEIGHT, Avo::WindowStyleFlags::DefaultNoResize);

		setupTheme();

		setScreen(new StartScreen{ this });

		run();
	}
};
