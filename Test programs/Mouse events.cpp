/*
	This little program was created to help with debugging and building the mouse event system.
	It can also be useful for library users who want to understand the mouse event system better!
*/

#include "../AvoGUI.hpp"

//------------------------------

class MouseView : public AvoGUI::View
{
private:
	std::string m_name;

public:
	MouseView(View* p_parent, const AvoGUI::Rectangle<float>& p_bounds) : 
		View(p_parent, p_bounds), m_name("")
	{
		enableMouseEvents();
	}

	//------------------------------

	void setName(const char* p_name)
	{
		m_name = p_name;
	}

	//------------------------------

	void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse down" << std::endl;
	}
	void handleMouseUp(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse up" << std::endl;
	}
	// Commented out because this spams the console...
	//void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	//{
	//	std::cout << m_name + "mouse move" << std::endl;
	//}
	void handleMouseEnter(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse enter" << std::endl;
	}
	void handleMouseLeave(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse leave" << std::endl;
	}
	void handleMouseBackgroundEnter(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse background enter" << std::endl;
	}
	void handleMouseBackgroundLeave(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << m_name + " mouse background leave" << "\n\n";
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(getThemeColor("background"));
		p_context->fillRectangle(getSize());
	}
};

//------------------------------

class MouseEventsTest : public AvoGUI::GUI
{
public:
	MouseEventsTest()
	{
		create("Mouse events test", 800, 600, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}

	//------------------------------

	void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse down" << std::endl;
	}
	void handleMouseUp(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse up" << std::endl;
	}
	void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	{
		invalidate();
	}
	void handleMouseEnter(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse enter" << std::endl;
	}
	void handleMouseLeave(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse leave" << std::endl;
	}
	void handleMouseBackgroundEnter(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse background enter" << std::endl;
	}
	void handleMouseBackgroundLeave(const AvoGUI::MouseEvent& p_event) override
	{
		std::cout << "GUI mouse background leave" << "\n\n";
	}

	//------------------------------

	void createContent() override
	{
		enableMouseEvents();

		MouseView* container_0 = new MouseView(this, AvoGUI::Rectangle<float>(100, 100, 400, 300));
		container_0->setName("Red");
		container_0->setThemeColor("background", 0xffff0000);

		MouseView* child = new MouseView(container_0, AvoGUI::Rectangle<float>(10, 10, container_0->getWidth() - 10, container_0->getHeight() - 10));
		child->setName("Green");
		child->setThemeColor("background", 0xff00ff00);

		MouseView* container_1 = new MouseView(this, AvoGUI::Rectangle<float>(300, 200, 500, 400));
		container_1->setName("Yellow");
		container_1->setThemeColor("background", 0xffffff00);

		child = new MouseView(container_1, AvoGUI::Rectangle<float>(70, 50, 150, 120));
		child->setName("Cyan");
		child->setThemeColor("background", 0xff00ffff);

		child = new MouseView(container_1, AvoGUI::Rectangle<float>(100, 100, 180, 180));
		child->setName("Blue");
		child->setThemeColor("background", 0xff0000ff);

		MouseView* overlay = new MouseView(this, AvoGUI::Rectangle<float>(0, 0, getWidth(), getHeight()));
		overlay->setName("Overlay");
		overlay->setThemeColor("background", 0U);
		overlay->setIsOverlay(true);
	}
};

//------------------------------

int main()
{
	MouseEventsTest* application = new MouseEventsTest();
	application->run();
	application->forget();
}
