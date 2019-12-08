#include "..//AvoGUI.hpp"

class AnimationsTest : public AvoGUI::Gui
{
private:
	AvoGUI::Point<float> m_startPosition;
	AvoGUI::Point<float> m_targetPosition;
	float m_animationTime;

public:
	AnimationsTest() :
		m_animationTime(0.f)
	{
		create("Animations test", 600, 500);
	}

	void createContent() override
	{
		enableMouseEvents();

		setThemeColor("background", AvoGUI::Color(0.9f, 0.f, 0.4f));
		getDrawingContext()->setBackgroundColor(getThemeColor("background"));
	}

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
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
	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(1.f));
		p_context->fillCircle(AvoGUI::interpolate(m_startPosition, m_targetPosition, AvoGUI::Easing(0.2, 0.9, 0.2, 1).easeValue(m_animationTime)), 25.f);
	}
};

int main()
{
	AnimationsTest* test = new AnimationsTest();
	test->waitForFinish();
}
