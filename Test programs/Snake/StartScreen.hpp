#pragma once

#include "Button.hpp"

class StartScreen : public AvoGUI::View
{
public:
	static constexpr float TITLE_SIZE{ 70.f };

	StartScreen(View* p_parent) :
		View(p_parent, p_parent->getBounds())
	{
		enableMouseEvents();

		auto title = new AvoGUI::TextView(this, TITLE_SIZE, Strings::title);
		title->setCenter(getCenterX(), getHeight() * 0.3f);

		setThemeValue(ThemeValues::buttonSize, 40.f);

		auto buttons = new View(this);
		buttons->enableMouseEvents();

		auto button_play = new Button(buttons, Strings::play);
		auto button_leaderboard = new Button(buttons, Strings::leaderboard);
		button_leaderboard->setTop(button_play->getBottom() + 20.f);
		button_play->setCenterX(button_leaderboard->getWidth() * 0.5f);
		buttons->setPadding(0.f);
		buttons->setCenter(getCenterX(), getHeight() * 0.65f);
	}
};
