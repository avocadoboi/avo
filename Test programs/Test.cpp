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
	AvoGUI::Image* m_image;

public:
	Sprite(AvoGUI::View* p_parent, std::string const& p_filePath) : 
		View(p_parent),
		m_image(getGui()->getDrawingContext()->createImage(p_filePath))
	{
		m_image->setBoundsPositioning(0.5f, 0.5f);
		m_image->setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);

		auto sizeChangeListener = [this](View*, float, float) {
			m_image->setSize(getParent()->getSize());
			setSize(getParent()->getSize());
		};
		p_parent->addSizeChangeListener(sizeChangeListener);
		p_parent->addChildDetachmentListener([=](View*, View* p_child) {
			if (p_child == this)
			{
				p_parent->removeSizeChangeListener(sizeChangeListener);
			}
		});
	}
	~Sprite()
	{
		m_image->forget();
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->drawImage(m_image);
	}
};

//------------------------------

class MyGui : 
	public AvoGUI::Gui
{
public:
	MyGui()
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);
		waitForFinish();
	}

	//------------------------------

	void createContent() override
	{
		setThemeColor(AvoGUI::ThemeColors::background, AvoGUI::Color(0.1f));
		setThemeColor(AvoGUI::ThemeColors::onBackground, AvoGUI::Color(0.98f));

		setThemeColor(AvoGUI::ThemeColors::primary, AvoGUI::Color(31, 115, 230));
		setThemeColor(AvoGUI::ThemeColors::primaryOnBackground, AvoGUI::Color(50, 130, 250));
		setThemeColor(AvoGUI::ThemeColors::onPrimary, AvoGUI::Color(1.f));
		setThemeColor(AvoGUI::ThemeColors::shadow, AvoGUI::Color(0.f, 0.9f));

		setThemeValue(AvoGUI::ThemeValues::textFieldHeight, 2.4f);
		setThemeValue(AvoGUI::ThemeValues::textFieldFontSize, 13.f);
		setThemeValue(AvoGUI::ThemeValues::textFieldPaddingLeft, 15.f);

		//------------------------------

		auto tooltip = new AvoGUI::Tooltip(this);

		auto viewContainer = new AvoGUI::View(this);
		viewContainer->enableMouseEvents();

		auto handleButtonClick = [](AvoGUI::Button* p_button) {
			std::cout << "A button saying '" << p_button->getString() << "' was pressed!\n";
		};

		auto button_yes = new AvoGUI::Button(viewContainer, "YES");
		button_yes->setTooltip(tooltip, "Tooltip 0");
		button_yes->addButtonClickListener(handleButtonClick);

		auto button_no = new AvoGUI::Button(viewContainer, "NO", AvoGUI::Button::Emphasis::Medium);
		button_no->setLeft(button_yes->getRight() + 10.f);
		button_no->setTooltip(tooltip, "Tooltip 1");
		button_no->addButtonClickListener(handleButtonClick);

		auto button_readMore = new AvoGUI::Button(viewContainer, "READ MORE", AvoGUI::Button::Emphasis::Low);
		button_readMore->setCenterX(button_no->getRight()*0.5f);
		button_readMore->setTop(button_no->getBottom() + 15.f);
		button_readMore->setTooltip(tooltip, "Tooltip 2");
		button_readMore->addButtonClickListener(handleButtonClick);

		auto textField_firstName = new AvoGUI::TextField(viewContainer, AvoGUI::TextField::Type::Outlined, "First name", 150.f);
		textField_firstName->setLeft(button_no->getRight() + 15.f);
		textField_firstName->setCenterY(button_readMore->getBottom()*0.5f - 22.f - AvoGUI::TextField::OUTLINED_PADDING_LABEL*0.5f);

		auto textField_lastName = new AvoGUI::TextField(viewContainer, AvoGUI::TextField::Type::Outlined, "Last name", 150.f);
		textField_lastName->setLeft(button_no->getRight() + 15.f);
		textField_lastName->setCenterY(button_readMore->getBottom() * 0.5f + 22.f - AvoGUI::TextField::OUTLINED_PADDING_LABEL*0.5f);

		viewContainer->setPadding(5.f);

		addGlobalKeyboardKeyDownListener([=](AvoGUI::KeyboardEvent const& p_event) {
			if (p_event.key == AvoGUI::KeyboardKey::F4)
			{
				getWindow()->switchFullscreen();
			}
			else if (p_event.key == AvoGUI::KeyboardKey::Escape)
			{
				getWindow()->setIsFullscreen(false);
			}
			else if (p_event.key == AvoGUI::KeyboardKey::Tab)
			{
				setKeyboardFocus(textField_firstName->getHasKeyboardFocus() ? textField_lastName : textField_firstName);
			}
		});

		addSizeChangeListener([=](View*, float, float) {
			viewContainer->setCenter(getCenterX(), getCenterY());
		});

		enableMouseEvents();
		addMouseDownListener([this](auto) {
			setKeyboardFocus(0);
		});
	}
};

//------------------------------

int main()
{
	new MyGui();
}
