#include "..//AvoGUI.hpp"

class AnimationsTest : public Avo::Gui
{
private:
	Avo::Point<float> m_startPosition;
	Avo::Point<float> m_targetPosition;
	float m_animationTime;

public:
	AnimationsTest() :
		m_animationTime(0.f)
	{
		create("Animations test", 600, 500);
		waitForFinish();
	}

	void createContent() override
	{
		enableMouseEvents();

		setThemeColor(Avo::ThemeColors::background, Avo::Color(0.9f, 0.f, 0.4f));
	}

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
		p_context->fillCircle(Avo::interpolate(m_startPosition, m_targetPosition, Avo::Easing(0.2, 0.9, 0.2, 1).easeValue(m_animationTime)), 25.f);
	}
};

int main()
{
	new AnimationsTest();
}
