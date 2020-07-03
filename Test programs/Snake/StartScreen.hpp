#pragma once

#include "Button.hpp"

class StartScreen : public Avo::View
{
public:
	static constexpr auto titleSize = 70.f;

private:
	auto createButtons() -> void
	{
		setThemeValue(ThemeValues::buttonSize, 40.f);

		auto* const buttons = new View{this};
		buttons->enableMouseEvents();

		auto* const button_play = new Button{buttons, Strings::play};
		auto* const button_leaderboard = new Button{buttons, Strings::leaderboard};
		button_leaderboard->setTop(button_play->getBottom() + 20.f);
		button_play->setCenterX(button_leaderboard->getWidth()/2);
		buttons->setPadding(0.f);
		buttons->setCenter({getWidth()/2, getHeight() * 0.65f});
	}

public:
	StartScreen(View* p_parent) :
		View{p_parent, p_parent->getBounds()}
	{
		enableMouseEvents();

		auto* const title = new Avo::TextView{this, titleSize, Strings::title};
		title->setCenter({getCenterX(), getHeight() * 0.3f});

		createButtons();
	}
};
