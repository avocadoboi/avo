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
	MyGUI()
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);
	}

	//------------------------------

	void handleButtonClick(AvoGUI::Button* p_button)
	{
		std::cout << "A button saying '" << p_button->getString() << "' was pressed!" << std::endl;
	}

	void handleKeyboardKeyDown(const AvoGUI::KeyboardEvent& p_event) override
	{
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
		m_theme->colors["background"] = 0xfffefefeU;
		m_theme->colors["on background"] = 0xff202020U;

		m_theme->colors["primary"] = AvoGUI::Color(31.f/255.f, 115.f/255.f, 230.f/255.f);
		m_theme->colors["primary on background"] = m_theme->colors["primary"];
		m_theme->colors["on primary"] = AvoGUI::Color(1.f);

		m_theme->colors["shadow"] = 0x70000000U;

		//------------------------------

		m_viewContainer = new AvoGUI::View(this);

		AvoGUI::Button* button_yes = new AvoGUI::Button(m_viewContainer, "YES");
		button_yes->setTooltip("Tooltip 0");
		button_yes->addButtonListener(this);

		AvoGUI::Button* button_no = new AvoGUI::Button(m_viewContainer, "NO", AvoGUI::Button::Emphasis::Medium, button_yes->getRight() + 10.f);
		button_no->setTooltip("Tooltip 1");
		button_no->addButtonListener(this);

		AvoGUI::Button* button_readMore = new AvoGUI::Button(m_viewContainer, "READ MORE", AvoGUI::Button::Emphasis::Low, 0.f, button_no->getBottom() + 15.f);
		button_readMore->setCenterX(button_no->getRight()*0.5f);
		button_readMore->setTooltip("tooltip 2");
		button_readMore->addButtonListener(this);

		AvoGUI::TextField* textField_name = new AvoGUI::TextField(m_viewContainer, AvoGUI::TextField::Type::Filled, "Name");
		textField_name->setLeft(button_no->getRight() + 15.f, true);
		textField_name->setCenterY(button_readMore->getBottom()*0.5f);

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
