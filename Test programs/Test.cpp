#include "../AvoGUI.hpp"
#include <iostream>

//------------------------------
// Example of a view that draws an image

class Sprite : public AvoGUI::View, public AvoGUI::ViewListener
{
private:
	AvoGUI::Image* m_image;

public:
	Sprite(AvoGUI::View* p_parent, const char* p_filePath) : View(p_parent)
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

public:
	MyGUI() :
		m_viewContainer(0)
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);
	}

	//------------------------------

	void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
	{
		setKeyboardFocus(0);
	}

	void handleButtonClick(AvoGUI::Button* p_button)
	{
		std::cout << "A button saying '" << p_button->getString() << "' was pressed!" << std::endl;
	}

	void handleKeyboardKeyDown(const AvoGUI::KeyboardEvent& p_event) override
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
	}

	//------------------------------

	void createContent() override
	{
		//m_theme->colors["background"] = AvoGUI::Color(0.1f);
		//m_theme->colors["on background"] = AvoGUI::;

		//m_theme->colors["primary"] = AvoGUI::Color(31, 115, 230);
		//m_theme->colors["primary on background"] = m_theme->colors["primary"];
		//m_theme->colors["on primary"] = AvoGUI::Color(1.f);

		//m_theme->colors["shadow"] = 0x90000000U;

		//------------------------------

		enableMouseEvents();

		m_viewContainer = new AvoGUI::View(this);
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

		AvoGUI::TextField* textField_name = new AvoGUI::TextField(m_viewContainer, AvoGUI::TextField::Type::Filled, "Name", 200.f);
		textField_name->setLeft(button_no->getRight() + 15.f);
		textField_name->setCenterY(button_readMore->getBottom()*0.5f);
		textField_name->setThemeValue("text field height", 3.2f);
		textField_name->setThemeValue("text field font size", 16.f);

		m_viewContainer->setPadding(30.f);
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
