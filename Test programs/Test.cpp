#include "../AvoGUI.hpp"
#include <iostream>

//------------------------------
// Example of a view that draws an image

class Sprite : public AvoGUI::View, public AvoGUI::ViewListener
{
private:
	AvoGUI::Image* m_image;

public:
	Sprite(AvoGUI::View* p_parent, char const* p_filePath) : View(p_parent)
	{
		p_parent->addEventListener(this);

		m_image = getGUI()->getDrawingContext()->createImage(p_filePath);
		m_image->setBoundsPositioning(0.5f, 0.5f);
		m_image->setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);
	}
	~Sprite()
	{
		m_image->forget();
	}

	//------------------------------

	void handleViewSizeChange(View* p_view) override
	{
		m_image->setSize(getParent()->getSize());
		setSize(getParent()->getSize());
	}

	void draw(AvoGUI::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->drawImage(m_image);
	}
};

//------------------------------

class MyGUI : public AvoGUI::GUI, public AvoGUI::ButtonListener
{
private:
	AvoGUI::View* m_viewContainer;
	AvoGUI::TextField* m_textField_firstName;
	AvoGUI::TextField* m_textField_lastName;

public:
	MyGUI() :
		m_viewContainer(0)
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);
	}

	//------------------------------

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		setKeyboardFocus(0);
	}

	void handleButtonClick(AvoGUI::Button* p_button)
	{
		std::cout << "A button saying '" << p_button->getString() << "' was pressed!" << std::endl;
	}

	void handleKeyboardKeyDown(AvoGUI::KeyboardEvent const& p_event) override
	{
		GUI::handleKeyboardKeyDown(p_event);

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
			setKeyboardFocus(m_textField_firstName->getHasKeyboardFocus() ? m_textField_lastName : m_textField_firstName);
		}
	}

	//------------------------------

	void createContent() override
	{
		m_theme->colors["background"] = AvoGUI::Color(0.1f);
		m_theme->colors["on background"] = AvoGUI::Color(0.98f);

		m_theme->colors["primary"] = AvoGUI::Color(31, 115, 230);
		m_theme->colors["primary on background"] = AvoGUI::Color(50, 130, 250);
		m_theme->colors["on primary"] = AvoGUI::Color(1.f);

		m_theme->values["text field height"] = 2.4f;
		m_theme->values["text field font size"] = 13.f;
		m_theme->values["text field padding left"] = 15.f;
		//m_theme->colors["shadow"] = 0x90000000U;

		//------------------------------

		enableMouseEvents();

		m_viewContainer = new AvoGUI::View(this);
		m_viewContainer->enableMouseEvents();

		AvoGUI::Button* button_yes = new AvoGUI::Button(m_viewContainer, "YES");
		button_yes->setTooltip("Tooltip 0");
		button_yes->addButtonListener(this);

		AvoGUI::Button* button_no = new AvoGUI::Button(m_viewContainer, "NO", AvoGUI::Button::Emphasis::Medium);
		button_no->setLeft(button_yes->getRight() + 10.f);
		button_no->setTooltip("Tooltip 1");
		button_no->addButtonListener(this);

		AvoGUI::Button* button_readMore = new AvoGUI::Button(m_viewContainer, "READ MORE", AvoGUI::Button::Emphasis::Low);
		button_readMore->setCenterX(button_no->getRight()*0.5f);
		button_readMore->setTop(button_no->getBottom() + 15.f);
		button_readMore->setTooltip("tooltip 2");
		button_readMore->addButtonListener(this);

		m_textField_firstName = new AvoGUI::TextField(m_viewContainer, AvoGUI::TextField::Type::Outlined, "First name", 150.f);
		m_textField_firstName->setLeft(button_no->getRight() + 15.f);
		m_textField_firstName->setCenterY(button_readMore->getBottom()*0.5f - 22.f - AvoGUI::TEXT_FIELD_OUTLINED_PADDING_LABEL*0.5f);

		m_textField_lastName = new AvoGUI::TextField(m_viewContainer, AvoGUI::TextField::Type::Outlined, "Last name", 150.f);
		m_textField_lastName->setLeft(button_no->getRight() + 15.f);
		m_textField_lastName->setCenterY(button_readMore->getBottom() * 0.5f + 22.f - AvoGUI::TEXT_FIELD_OUTLINED_PADDING_LABEL*0.5f);

		m_viewContainer->setPadding(5.f);
	}
	void handleSizeChange() override
	{
		m_viewContainer->setCenter(getCenterX(), getCenterY());
	}
};

//------------------------------

int main()
{
	MyGUI* gui = new MyGUI();
	AvoGUI::GUI::run();
	gui->forget();
}
