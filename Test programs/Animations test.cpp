#include "..//AvoGUI.hpp"

class AnimationsTest : public Avo::Gui
{
private:
	Avo::Point<> m_startPosition;
	Avo::Point<> m_targetPosition;
	float m_animationTime = 0;

public:
	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		m_startPosition = m_targetPosition;
		m_targetPosition.set(p_event.x, p_event.y);
		m_animationTime = 0.f;
		queueAnimationUpdate();
	}

	void updateAnimations()
	{
		m_animationTime += 0.04f;
		invalidate();
		queueAnimationUpdate();
	}
	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(Avo::Color(1.f));
		p_context->fillCircle(Avo::interpolate(m_startPosition, m_targetPosition, Avo::Easing{ 0.2, 0.9, 0.2, 1 }.easeValue(m_animationTime)), 25.f);
	}

	AnimationsTest()
	{
		create("Animations test", 600, 500);

		enableMouseEvents();
		setThemeColor(Avo::ThemeColors::background, { 0.9f, 0.f, 0.4f });

		run();
	}
};

int main()
{
	new AnimationsTest();
}
