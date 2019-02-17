#include "AvoGUI.hpp"
#include <iostream>

//------------------------------

class Sprite : public AvoGUI::View, public AvoGUI::ViewEventListener
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

class MyGUI : public AvoGUI::GUI
{
private:
	AvoGUI::View* m_buttonContainer;
	AvoGUI::Button* m_button_readMore;
	Sprite* m_sprite;

	bool m_areButtonsEnabled = true;

public:
	MyGUI() : m_areButtonsEnabled(true)
	{
		create("My GUI", 450, 300, AvoGUI::WindowStyleFlags::Default);
	}

	//------------------------------

	void handleKeyboardKeyDown(const AvoGUI::KeyboardEvent& p_event) override
	{
		GUI::handleKeyboardKeyDown(p_event);

		if (p_event.key == AvoGUI::KeyboardKey::Spacebar)
		{
			if (m_areButtonsEnabled)
			{
				((AvoGUI::Button*)m_buttonContainer->getView(0))->disable();
				((AvoGUI::Button*)m_buttonContainer->getView(1))->disable();
				m_button_readMore->disable();
			}
			else
			{
				((AvoGUI::Button*)m_buttonContainer->getView(0))->enable();
				((AvoGUI::Button*)m_buttonContainer->getView(1))->enable();
				m_button_readMore->enable();
			}
			m_areButtonsEnabled = !m_areButtonsEnabled;
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

		m_sprite = new Sprite(this, "test image.png");

		m_buttonContainer = new AvoGUI::View(this, AvoGUI::Rectangle<float>());
		AvoGUI::Button* button_press = new AvoGUI::Button(m_buttonContainer, "YES");
		new AvoGUI::Button(m_buttonContainer, "NO", AvoGUI::Button::Emphasis::Medium, button_press->getWidth() + 10.f);
		m_buttonContainer->setPadding(30.f);

		m_button_readMore = new AvoGUI::Button(this, "READ MORE", AvoGUI::Button::Emphasis::Low);
	}
	void handleSizeChange() override
	{
		m_buttonContainer->setCenter(getCenterX(), getCenterY() - 30.f);
		m_button_readMore->setCenter(getCenterX(), getCenterY() + 30.f);
	}
};

//------------------------------

int main()
{
	MyGUI* gui = new MyGUI();
	AvoGUI::GUI::run();
}
