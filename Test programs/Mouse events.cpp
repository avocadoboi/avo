/*
	This little program was created to help with debugging and building the mouse event system.
	It can also be useful for library users who want to understand the mouse event system better!
*/

#include <AvoGUI.hpp>

//------------------------------

auto frameCount = Avo::Count{};

class MouseView : public Avo::View
{
private:
	std::string m_name;
public:
	auto setName(char const* const p_name) -> void
	{
		m_name = p_name;
	}

	//------------------------------

	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse down" << std::endl;
	}
	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse up" << std::endl;
	}
	// Commented out because this spams the console...
	//auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	//{
	//	std::cout << m_name + "mouse move" << std::endl;
	//}
	auto handleMouseEnter(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse enter" << std::endl;
	}
	auto handleMouseLeave(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse leave" << std::endl;
	}
	auto handleMouseBackgroundEnter(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse background enter" << std::endl;
	}
	auto handleMouseBackgroundLeave(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << m_name + " mouse background leave" << "\n\n";
		frameCount = 0;
	}

	//------------------------------

	auto draw(Avo::DrawingContext* const p_context) -> void override
	{
		p_context->setColor(getThemeColor(Avo::ThemeColors::background));
		p_context->fillRectangle(getSize());
	}

	//------------------------------

	MouseView(View* const p_parent, Avo::Rectangle<float> const& p_bounds) :
		View{p_parent, p_bounds}
	{
		enableMouseEvents();
	}
};

//------------------------------

class MouseEventsTest : public Avo::Gui
{
public:
	MouseEventsTest()
	{
		create("Mouse events test", {800, 600}, Avo::WindowStyleFlags::DefaultNoResize);

		enableMouseEvents();

		auto* const container_0 = new MouseView{this, {100, 100, 400, 300}};
		container_0->setName("Red");
		container_0->setThemeColor(Avo::ThemeColors::background, 0xffff0000);

		auto* child = new MouseView{container_0, {Avo::Point{10.f}, container_0->getSize()}};
		child->setName("Green");
		child->setThemeColor(Avo::ThemeColors::background, 0xff00ff00);

		auto* const container_1 = new MouseView{this, {300, 200, 500, 400}};
		container_1->setName("Yellow");
		container_1->setThemeColor(Avo::ThemeColors::background, 0xffffff00);

		child = new MouseView{container_1, {70, 50, 150, 120}};
		child->setName("Cyan");
		child->setThemeColor(Avo::ThemeColors::background, 0xff00ffff);

		child = new MouseView{container_1, {100, 100, 180, 180}};
		child->setName("Blue");
		child->setThemeColor(Avo::ThemeColors::background, 0xff0000ff);

		auto* const overlay = new MouseView{this, {0, 0, getWidth(), getHeight()}};
		overlay->setName("Overlay");
		overlay->setThemeColor(Avo::ThemeColors::background, 0U);
		overlay->setIsOverlay(true);

		queueAnimationUpdate();

		run();
	}

	//------------------------------

	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse down" << std::endl;
	}
	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse up" << std::endl;
	}
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		invalidate();
	}
	auto handleMouseEnter(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse enter" << std::endl;
	}
	auto handleMouseLeave(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse leave" << std::endl;
	}
	auto handleMouseBackgroundEnter(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse background enter" << std::endl;
	}
	auto handleMouseBackgroundLeave(Avo::MouseEvent const& p_event) -> void override
	{
		std::cout << "GUI mouse background leave" << "\n\n";
		frameCount = 0;
	}

	//------------------------------

	auto updateAnimations() -> void override
	{
		if (frameCount == 15 * 60)
		{
			std::system("cls");
		}
		frameCount++;
		queueAnimationUpdate();
	}
};

//------------------------------

auto main() -> int
{
	new MouseEventsTest;
}
