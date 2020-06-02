#include "../AvoGUI.hpp"

class IconsTest : public Avo::Gui
{
private:
	float m_animationValue = 1.f;
	Avo::Animation m_entrance{
		this, { 0.06, 0.53, 0., 1. }, 3s,
		[=](float p_value) {
			m_animationValue = p_value;
			invalidate();
		}
	};

	Avo::Text m_text;

public:
	void draw(Avo::DrawingContext* p_context) override
	{
		if (m_text)
		{
			p_context->setColor(Avo::Color{}.setHSBA(m_animationValue - 0.1f, 0.9f, 1, m_animationValue ));
			p_context->rotate(m_animationValue * Avo::PI * 4.f, getCenter());
			p_context->scale(m_animationValue, getCenter());
			p_context->drawText(m_text);
		}
	}

	IconsTest()
	{
		create("Icons!", 500.f, 400.f);

		constexpr float FONT_SIZE = 120.f;

		auto centerText = [=](auto...) {
			m_text.setCenter(getCenter());
		};
		sizeChangeListeners += centerText;

		auto updateCount = [=](Count p_count) {
			m_text = getDrawingContext()->createText(Avo::convertNumberToString(p_count), FONT_SIZE);
			m_text.fitSizeToText();
			centerText();
			invalidate();
		};
		updateCount(1);
		addTimerCallback([=] { updateCount(2); }, 1s);
		addTimerCallback([=] { updateCount(3); }, 2s);
		addTimerCallback(
			[=] {
				m_text = getDrawingContext()->createText(std::string{ MaterialIcons::BLUETOOTH } + MaterialIcons::ANDROID, FONT_SIZE);
				m_text.setFontFamily(Avo::FONT_FAMILY_MATERIAL_ICONS);
				m_text.fitSizeToText();
				centerText();
				m_entrance.play();
			}, 3s
		);

		run();
	}
};

int main()
{
	new IconsTest;
}
