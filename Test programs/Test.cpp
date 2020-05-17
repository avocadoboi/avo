#include "../AvoGUI.hpp"
#include <iostream>

//------------------------------

/*
	Example of a view that draws an image that fills its parent.
	Usage example:
	auto sprite = new Sprite(this, "path/to/image.png");
*/
class Sprite : public AvoGUI::View
{
private:
	AvoGUI::Image m_image;

public:
	Sprite(AvoGUI::View* p_parent, std::string const& p_filePath) : 
		View{ p_parent },
		m_image{ getGui()->getDrawingContext()->createImage(p_filePath) }
	{
		m_image.setBoundsPositioning(0.5f, 0.5f);
		m_image.setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);

		auto sizeChangeListener = [this](float, float) {
			m_image.setSize(getParent<View>()->getSize());
			setSize(getParent<View>()->getSize());
		};
		p_parent->sizeChangeListeners += sizeChangeListener;
		p_parent->childDetachmentListeners += [=](Component* p_child) {
			if (p_child == this)
			{
				p_parent->sizeChangeListeners -= sizeChangeListener;
			}
		};
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->drawImage(m_image);
	}
};

//------------------------------

int main()
{
	auto gui = new AvoGUI::Gui;

	gui->create(u8"My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);

	{
		using namespace AvoGUI::ThemeColors;
		gui->setThemeColors({
			{ background, 0.1f },
			{ onBackground, 0.98f },
			{ primary, { 31, 115, 230 } },
			{ primaryOnBackground, { 50, 130, 250 } },
			{ onPrimary, 1.f },
			{ shadow, { 0.f, 0.9f } }
		});
	}

	{
		using namespace AvoGUI::ThemeValues;
		//gui->setThemeValue()
		gui->setThemeValue(textFieldHeight, 2.4f);
		gui->setThemeValue(textFieldFontSize, 13.f);
		gui->setThemeValue(textFieldPaddingLeft, 15.f);
	}

	auto tooltip = new AvoGUI::Tooltip{ gui };

	auto viewContainer = new AvoGUI::View{ gui };
	viewContainer->enableMouseEvents();

	auto handleButtonClick = [](AvoGUI::Button* p_button) {
		std::cout << "A button saying '" << p_button->getString() << "' was pressed!\n";
	};

	auto button_yes = new AvoGUI::Button{ viewContainer, "YES" };
	button_yes->setTooltip(tooltip, "Tooltip 0");
	button_yes->buttonClickListeners += handleButtonClick;

	auto button_no = new AvoGUI::Button{ viewContainer, "NO", AvoGUI::Button::Emphasis::Medium };
	button_no->setLeft(button_yes->getRight() + 10.f);
	button_no->setTooltip(tooltip, "Tooltip 1");
	button_no->buttonClickListeners += handleButtonClick;

	auto button_readMore = new AvoGUI::Button{ viewContainer, "READ MORE", AvoGUI::Button::Emphasis::Low };
	button_readMore->setCenterX(button_no->getRight() * 0.5f);
	button_readMore->setTop(button_no->getBottom() + 15.f);
	button_readMore->setTooltip(tooltip, "Tooltip 2");
	button_readMore->buttonClickListeners += handleButtonClick;

	auto textField_firstName = new AvoGUI::TextField{ viewContainer, AvoGUI::TextField::Type::Outlined, "First name", 150.f };
	textField_firstName->setLeft(button_no->getRight() + 15.f);
	textField_firstName->setCenterY(button_readMore->getBottom() * 0.5f - 22.f - AvoGUI::TextField::OUTLINED_PADDING_LABEL * 0.5f);

	auto textField_lastName = new AvoGUI::TextField{ viewContainer, AvoGUI::TextField::Type::Outlined, "Last name", 150.f };
	textField_lastName->setLeft(button_no->getRight() + 15.f);
	textField_lastName->setCenterY(button_readMore->getBottom() * 0.5f + 22.f - AvoGUI::TextField::OUTLINED_PADDING_LABEL * 0.5f);

	viewContainer->setPadding(5.f);

	gui->globalKeyboardKeyDownListeners += [=](AvoGUI::KeyboardEvent const& p_event) {
		if (p_event.key == AvoGUI::KeyboardKey::F4)
		{
			gui->getWindow()->switchFullscreen();
		}
		else if (p_event.key == AvoGUI::KeyboardKey::Escape)
		{
			gui->getWindow()->setIsFullscreen(false);
		}
		else if (p_event.key == AvoGUI::KeyboardKey::Tab)
		{
			gui->setKeyboardFocus(textField_firstName->getHasKeyboardFocus() ? textField_lastName : textField_firstName);
		}
	};

	gui->sizeChangeListeners += [=](auto...) {
		viewContainer->setCenter(gui->getCenterX(), gui->getCenterY());
	};

	gui->enableMouseEvents();
	gui->mouseDownListeners += [=](auto) {
		gui->setKeyboardFocus(0);
	};
	gui->invalidate();

	gui->run();
}
