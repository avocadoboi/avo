#include "../AvoGUI.hpp"
#include <iostream>

//------------------------------

/*
	Example of a view that draws an image that fills its parent.
	Usage example:
	auto sprite = new Sprite(this, "path/to/image.png");
*/
class Sprite : public Avo::View
{
private:
	Avo::Image m_image;

public:
	void draw(Avo::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->drawImage(m_image);
	}

	Sprite(Avo::View* p_parent, std::string_view p_filePath) : 
		View{p_parent},
		m_image{getGui()->getDrawingContext()->createImage(p_filePath)}
	{
		m_image.setBoundsPositioning(0.5f);
		m_image.setBoundsSizing(Avo::ImageBoundsSizing::Contain);

		auto sizeChangeListener = [this](auto) {
			auto const size = getParent<View>()->getSize();
			m_image.setSize(size);
			setSize(size);
		};
		p_parent->sizeChangeListeners += sizeChangeListener;
		p_parent->childDetachmentListeners += [=](Component* p_child) {
			if (p_child == this)
			{
				p_parent->sizeChangeListeners -= sizeChangeListener;
			}
		};
	}
};

//------------------------------

int main()
{
	auto gui = new Avo::Gui;

	gui->create(u8"My GUI", {450, 300}, Avo::WindowStyleFlags::Default);

	{
		using namespace Avo::ThemeColors;
		gui->setThemeColors({
			{background, 0.1f},
			{onBackground, 0.98f},
			{primary, {31, 115, 230}},
			{primaryOnBackground, {50, 130, 250}},
			{onPrimary, 1.f},
			{shadow, {0.f, 0.9f}}
		});
	}

	{
		using namespace Avo::ThemeValues;
		gui->setThemeValues({
			{textFieldHeight, 2.4f},
			{textFieldFontSize, 13.f},
			{textFieldPaddingLeft, 15.f}
		});
	}

	auto tooltip = new Avo::Tooltip{gui};

	auto viewContainer = new Avo::View{gui};
	viewContainer->enableMouseEvents();

	auto handleButtonClick = [](Avo::Button* p_button) {
		Console::println("A button saying '", p_button->getString(), "' was pressed!");
	};

	auto button_yes = new Avo::Button{viewContainer, "YES"};
	button_yes->setTooltip(tooltip, "Tooltip 0");
	button_yes->buttonClickListeners += handleButtonClick;

	auto button_no = new Avo::Button{viewContainer, "NO", Avo::Button::Emphasis::Medium};
	button_no->setLeft(button_yes->getRight() + 10.f);
	button_no->setTooltip(tooltip, "Tooltip 1");
	button_no->buttonClickListeners += handleButtonClick;

	auto button_readMore = new Avo::Button{viewContainer, "READ MORE", Avo::Button::Emphasis::Low};
	button_readMore->setCenterX(button_no->getRight() * 0.5f);
	button_readMore->setTop(button_no->getBottom() + 15.f);
	button_readMore->setTooltip(tooltip, "Tooltip 2");
	button_readMore->buttonClickListeners += handleButtonClick;

	auto textField_firstName = new Avo::TextField{viewContainer, Avo::TextField::Type::Outlined, "First name", 150.f};
	textField_firstName->setLeft(button_no->getRight() + 15.f);
	textField_firstName->setCenterY(button_readMore->getBottom() * 0.5f - 22.f - Avo::TextField::OUTLINED_PADDING_LABEL * 0.5f);

	// auto textField_lastName = viewContainer->addView<Avo::TextField>(Avo::TextField::Type::Outlined, "Last name", 150.f);
	auto textField_lastName = new Avo::TextField{viewContainer, Avo::TextField::Type::Outlined, "Last name", 150.f};
	textField_lastName->setLeft(button_no->getRight() + 15.f);
	textField_lastName->setCenterY(button_readMore->getBottom() * 0.5f + 22.f - Avo::TextField::OUTLINED_PADDING_LABEL * 0.5f);

	viewContainer->setPadding(5.f);

	gui->globalKeyboardKeyDownListeners += [=](Avo::KeyboardEvent const& p_event) {
		if (p_event.key == Avo::KeyboardKey::F4)
		{
			gui->getWindow()->switchFullscreen();
		}
		else if (p_event.key == Avo::KeyboardKey::Escape)
		{
			gui->getWindow()->setIsFullscreen(false);
		}
		else if (p_event.key == Avo::KeyboardKey::Tab)
		{
			gui->setKeyboardFocus(textField_firstName->getHasKeyboardFocus() ? textField_lastName : textField_firstName);
		}
	};

	gui->sizeChangeListeners += [=](auto) {
		viewContainer->setCenter(gui->getCenter());
	};

	gui->enableMouseEvents();
	gui->mouseDownListeners += [=](auto) {
		gui->setKeyboardFocus(0);
	};

	gui->run();
}
