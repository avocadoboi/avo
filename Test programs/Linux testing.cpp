#include "../AvoGUI.hpp"

//------------------------------

class App : public Avo::Gui
{
private:
	float m_time = 0.f;
	Avo::Image m_image;

public:
	App()
	{
		create(u8"Linux test! Unicode: åäöâñëV݉sZ㇡ه搶o7賍", { 600, 500 }, Avo::WindowStyleFlags::Default);
		
		setThemeColor(Avo::ThemeColors::background, 0);

		queueAnimationUpdate();

		m_image = getDrawingContext()->createImage(u8"/home/bjorn/Pictures/test.jpg");
		m_image.setBoundsSizing(Avo::ImageBoundsSizing::Contain);

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

	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->rotate(m_time*0.1, getCenter());
		p_context->drawImage(m_image, 0.3f);
		p_context->rotate(-m_time*0.1, getCenter());

		p_context->setColor({ 1.f, 0.1f, 0.5f });
		for (Avo::Index a = 0; a < 10; a++)
		{
			p_context->drawLine(10.f, 10.5f + a*10.f, 100.f, 50.5f + a*10.f, 0.5f + a*0.5f);
		}

		p_context->setColor({ 0.2f, 0.5f, 0.3f });
		p_context->fillRectangle(getCenterX() - 80.f, 10.f, getCenterX() + 80.f, 100.f, Avo::RectangleCorners{ 10.f, Avo::RectangleCornerType::Round });
		p_context->fillRectangle(getCenterX() - 80.f, getBottom() - 100.f, getCenterX() + 80.f, getBottom() - 10.f, Avo::RectangleCorners(10.f, Avo::RectangleCornerType::Cut));

		p_context->strokeRectangle(10.f, getCenterY() - 80.f, 100.f, getCenterY() + 80.f, Avo::RectangleCorners{ 40.f, Avo::RectangleCornerType::Round }, 10.f);
		p_context->strokeRectangle(getRight() - 100.f, getCenterY() - 80.f, getRight() - 10.f, getCenterY() + 80.f, Avo::RectangleCorners{ 30.f, 20.f, Avo::RectangleCornerType::Cut }, 5.f);

		p_context->setColor({ 0.1f, 1.f, 0.5f });
		p_context->rotate(-m_time, getCenter());
		p_context->strokeRectangle(Avo::Rectangle<>{-50.f, -30.f, 50.f, 30.f} += getCenter(), 2.f);
		p_context->fillRectangle(Avo::Rectangle<>{-40.f, -20.f, 40.f, 20.f} += getCenter());
		p_context->rotate(m_time, getCenter());

		p_context->setColor({ 0.8f, 0.8f, 0.1f });
		p_context->strokeCircle(Avo::Point<>{}.setPolar(m_time, 100.f) += getCenter(), std::sin(m_time)*30.f + 30.f, 2.f);
		p_context->fillCircle(Avo::Point<>{}.setPolar(m_time + Avo::PI, 100.f) += getCenter(), std::sin(m_time + Avo::PI)*30.f + 30.f);
	}
};

//------------------------------

int main()
{
	new App;
}
