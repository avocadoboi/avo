#include <AvoGUI.hpp>

class AnimationsTest : public Avo::Gui
{
private:
	Avo::Point<> m_startPosition;
	Avo::Point<> m_targetPosition;
	Avo::Point<> m_currentPosition;

	auto initPosition() -> void
	{
		m_startPosition = getCenter();
		m_targetPosition = m_startPosition;
		m_currentPosition = m_startPosition;
	}

	Avo::Animation m_animation
	{
		this, Avo::Easing{0.2, 0.9, 0.2, 1}, 0.5s,
		[this](float p_value){
			m_currentPosition = Avo::interpolate(m_startPosition, m_targetPosition, p_value);
			invalidate();
		}
	};

	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override 
	{
		m_startPosition = m_targetPosition;
		m_targetPosition = p_event.xy;
		m_animation.replay();
	}

	auto draw(Avo::DrawingContext* const p_context) -> void override 
	{
		p_context->setColor(1.f);
		p_context->fillCircle(m_currentPosition, 25.f);
	}

public:
	AnimationsTest()
	{
		create("Animations test", {600, 500});

		enableMouseEvents();
		setThemeColor(Avo::ThemeColors::background, {0.9f, 0.f, 0.4f});
		initPosition();

		run();
	}
};

auto main() -> int
{
	new AnimationsTest;
}
