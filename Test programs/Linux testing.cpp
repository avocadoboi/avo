#include <AvoGUI.hpp>

//------------------------------

class App :
	public AvoGUI::Gui
{
private:
	float m_time;

public:
	App() :
		m_time(0.f)
	{
		create(u8"Linux test! Unicode: åäöâñëV݉sZ㇡ه搶o7賍", 600, 500, AvoGUI::WindowStyleFlags::Default);
		waitForFinish();
	}

	void createContent() override
	{
		setThemeColor("background", AvoGUI::Color(0));

		queueAnimationUpdate();
	}
	void updateAnimations() override
	{
		m_time += 0.005f;

		invalidate();
		queueAnimationUpdate();
	}

	void draw(AvoGUI::DrawingContext *p_context) override
	{
		//p_context->setColor(AvoGUI::Color().setHSB(m_hue,1.f,1.f));
		p_context->setColor(AvoGUI::Color(1.f, 0.1f, 0.5f));
		for (uint32 a = 0; a < 10; a++)
		{
			p_context->drawLine(10.f, 10.f + a*10.f, 100.f, 50.f + a*10.f, 0.5f + a*0.5f);
		}

		p_context->setColor(AvoGUI::Color(0.1f, 1.f, 0.5f));
		p_context->scale(0.5f);
		p_context->rotate(m_time);
		p_context->fillRectangle(AvoGUI::Rectangle(-50.f, -30.f, 50.f, 30.f) += getCenter());
		p_context->rotate(-m_time);
		p_context->scale(2.f);

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
