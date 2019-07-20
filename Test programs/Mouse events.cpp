#include "../AvoGUI.hpp"

//------------------------------

std::string lastConsoleMessage = "";

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
		lastConsoleMessage = m_name + " mouse down";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseUp(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = m_name + " mouse up";
		std::cout << lastConsoleMessage << std::endl;
	}
	//void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	//{
	//	std::string consoleMessage = m_name + " mouse move";
	//	if (lastConsoleMessage != consoleMessage)
	//	{
	//		lastConsoleMessage = consoleMessage;
	//		std::cout << lastConsoleMessage << std::endl;
	//	}
	//}
	void handleMouseEnter(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = m_name + " mouse enter";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseLeave(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = m_name + " mouse leave";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseBackgroundEnter(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = m_name + " mouse background enter";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseBackgroundLeave(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = m_name + " mouse background leave";
		std::cout << lastConsoleMessage << "\n\n";
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
		lastConsoleMessage = "GUI mouse down";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseUp(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = "GUI mouse up";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	{
		invalidate();
	}
	void handleMouseEnter(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = "GUI mouse enter";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseLeave(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = "GUI mouse leave";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseBackgroundEnter(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = "GUI mouse background enter";
		std::cout << lastConsoleMessage << std::endl;
	}
	void handleMouseBackgroundLeave(const AvoGUI::MouseEvent& p_event) override
	{
		lastConsoleMessage = "GUI mouse background leave";
		std::cout << lastConsoleMessage << "\n\n";
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
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
	}
};

//------------------------------

int main()
{
	MouseEventsTest* application = new MouseEventsTest();
	AvoGUI::GUI::run();
	application->forget();
}
