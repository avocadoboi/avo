#include "../AvoGUI.hpp"

//------------------------------

class App :
	public AvoGUI::Gui
{
private:
	float m_time{ 0.f };
	AvoGUI::Image m_image;

public:
	App()
	{
		create(u8"Linux test! Unicode: åäöâñëV݉sZ㇡ه搶o7賍", 600, 500, AvoGUI::WindowStyleFlags::Default);
		
		setThemeColor(AvoGUI::ThemeColors::background, AvoGUI::Color(0));

		queueAnimationUpdate();

		m_image = getDrawingContext()->createImage("/home/bjorn/Pictures/test.png");
		m_image.setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);

		run();
	}

	void handleSizeChange() override
	{
		m_image.setBounds(getBounds());
	}
	void updateAnimations() override
	{
		m_time += 0.005f;

		invalidate();
		queueAnimationUpdate();
	}

	void draw(AvoGUI::DrawingContext *p_context) override
	{
		p_context->rotate(m_time*0.1, getCenter());
		p_context->drawImage(m_image);
		p_context->rotate(-m_time*0.1, getCenter());

		p_context->setColor(AvoGUI::Color(1.f, 0.1f, 0.5f));
		for (uint32 a = 0; a < 10; a++)
		{
			p_context->drawLine(10.f, 10.5f + a*10.f, 100.f, 50.5f + a*10.f, 0.5f + a*0.5f);
		}

		p_context->setColor(AvoGUI::Color(0.2f, 0.5f, 0.3f));
		p_context->fillRectangle(getCenterX() - 80.f, 10.f, getCenterX() + 80.f, 100.f, AvoGUI::RectangleCorners(10.f, AvoGUI::RectangleCornerType::Round));
		p_context->fillRectangle(getCenterX() - 80.f, getBottom() - 100.f, getCenterX() + 80.f, getBottom() - 10.f, AvoGUI::RectangleCorners(10.f, AvoGUI::RectangleCornerType::Cut));

		p_context->strokeRectangle(10.f, getCenterY() - 80.f, 100.f, getCenterY() + 80.f, AvoGUI::RectangleCorners(40.f, AvoGUI::RectangleCornerType::Round), 10.f);
		p_context->strokeRectangle(getRight() - 100.f, getCenterY() - 80.f, getRight() - 10.f, getCenterY() + 80.f, AvoGUI::RectangleCorners(30.f, 20.f, AvoGUI::RectangleCornerType::Cut), 5.f);

		p_context->setColor(AvoGUI::Color(0.1f, 1.f, 0.5f));
		p_context->rotate(-m_time, getCenter());
		p_context->strokeRectangle(AvoGUI::Rectangle(-50.f, -30.f, 50.f, 30.f) += getCenter(), 2.f);
		p_context->fillRectangle(AvoGUI::Rectangle(-40.f, -20.f, 40.f, 20.f) += getCenter());
		p_context->rotate(m_time, getCenter());

		p_context->setColor(AvoGUI::Color(0.8f, 0.8f, 0.1f));
		p_context->strokeCircle(AvoGUI::Point<float>().setPolar(m_time, 100.f) += getCenter(), std::sin(m_time)*30.f + 30.f, 2.f);
		p_context->fillCircle(AvoGUI::Point<float>().setPolar(m_time + AvoGUI::PI, 100.f) += getCenter(), std::sin(m_time + AvoGUI::PI)*30.f + 30.f);
	}
};

//------------------------------

int main()
{
	new App();
}
