#include "../AvoGUI.hpp"

class IconsTest : public Avo::Gui
{
private:
	float m_animationValue = 1.f;
	Avo::Animation m_entrance{
		this, {0.06, 0.53, 0., 1.}, 3s,
		[this](float const p_value) {
			m_animationValue = p_value;
			invalidate();
		}
	};

	Avo::Text m_text;

public:
	auto draw(Avo::DrawingContext* const p_context) -> void override
	{
		if (m_text)
		{
			p_context->setColor(Avo::Color::hsba(m_animationValue - 0.1f, 0.9f, 1, m_animationValue ));
			p_context->rotate(Avo::Radians{m_animationValue * Avo::PI<float> * 4.f}, getCenter());
			p_context->scale(m_animationValue, getCenter());
			p_context->drawText(m_text);
		}
	}

	IconsTest()
	{
		create("Icons!", {500.f, 400.f});

		constexpr float fontSize = 120.f;

		auto const centerText = [=](auto...) {
			m_text.setCenter(getCenter());
		};
		sizeChangeListeners += centerText;

		auto const updateCount = [=](Avo::Count const p_count) {
			m_text = getDrawingContext()->createText(Avo::numberToString(p_count), fontSize);
			m_text.fitSizeToText();
			centerText();
			invalidate();
		};
		updateCount(1);
		addTimerCallback([=] { updateCount(2); }, 1s);
		addTimerCallback([=] { updateCount(3); }, 2s);
		addTimerCallback(
			[=] {
				m_text = getDrawingContext()->createText(std::string{MaterialIcons::BLUETOOTH} + MaterialIcons::ANDROID, fontSize);
				m_text.setFontFamily(Avo::FONT_FAMILY_MATERIAL_ICONS);
				m_text.fitSizeToText();
				centerText();
				m_entrance.play();
			}, 3s
		);

		run();
	}
};

auto main() -> int
{
	new IconsTest;
}
