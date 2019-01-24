#include "AvoGUI winapi.hpp"
#include <iostream>

//------------------------------

class MyGUI : public AvoGUI::GUI
{
private:
	AvoGUI::ViewContainer* m_buttonContainer;
	AvoGUI::Button* m_button_readMore;

public:
	MyGUI()
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);

		run();
	}

	//------------------------------

	void createContent() override
	{
		m_theme->colors["background"] = 0xfffefefeU;
		m_theme->colors["shadow"] = 0x70000000U;

		m_buttonContainer = new AvoGUI::ViewContainer(this, AvoGUI::Rectangle<float>());
		AvoGUI::Button* button_press = new AvoGUI::Button(m_buttonContainer, "PRESS");
		new AvoGUI::Button(m_buttonContainer, "DON'T PRESS PLEASE", AvoGUI::Button::Emphasis::Medium, button_press->getWidth() + 10.f);
		m_buttonContainer->setPadding(10.f);

		m_button_readMore = new AvoGUI::Button(this, "READ MORE", AvoGUI::Button::Emphasis::Low);
	}
	void handleSizeChanged() override
	{
		m_buttonContainer->setCenter(getCenterX(), getCenterY() - 30.f);
		m_button_readMore->setCenter(getCenterX(), getCenterY() + 30.f);
	}
};

//------------------------------

int main()
{
	MyGUI();
}
