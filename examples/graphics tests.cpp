#include "../AvoGUI.hpp"
#include <array>

class Card : public Avo::View
{
private:
	Avo::Text m_text;
	Avo::LinearGradient m_gradient;
	Avo::RadialGradient m_radialGradient;

public:
	void handleSizeChange()
	{
		m_text.setWidth(getWidth() - 50.f);
		m_text.fitHeightToText();

		m_text.setCenterX(getCenterX());
		m_text.setTop(18.f);
		setHeight(m_text.getBottom() + 18.f);

		m_gradient.setEndPosition(Avo::Point{getSize()});
	}

	void draw(Avo::DrawingContext* p_context)
	{
		p_context->setGradient(m_gradient);
		p_context->fillRectangle(getSize());

		p_context->setGradient(m_radialGradient);
		p_context->fillRectangle(getSize());

		p_context->setColor(1.f);
		p_context->drawText(m_text);
	}

	Card(View* p_parent) :
		View{p_parent}
	{
		setCornerCutSize(20.f, 0.f, 0.f, 20.f);
		setElevation(4.f);

		m_text = getDrawingContext()->createText("لعاصمة, ذات سكان السادس ومطالبة ٣٠. بفرض فهرست كلّ كل, دارت اليميني تلك ما, وفي سابق الحرة مع. بـ الدمج", 24.f);
		m_text.setFontWeight(Avo::FontWeight::Light);
		m_text.setWordWrapping(Avo::WordWrapping::WholeWord);
		m_text.setReadingDirection(Avo::ReadingDirection::RightToLeft);

		m_gradient = getDrawingContext()->createLinearGradient(std::array{ 
			Avo::GradientStop{Avo::Color{255, 0, 0}, 0.f}, 
			Avo::GradientStop{Avo::Color{0, 0, 255}, 1.f}
		});
		m_radialGradient = getDrawingContext()->createRadialGradient(
			std::array{
				Avo::GradientStop{Avo::Color{1.f, 1.f, 0.f, 1.f}, 0.f},
				Avo::GradientStop{Avo::Color{0.f, 0.f, 0.f, 0.f}, 1.f}
			}, getTopLeft(), 200.f
		);
	}
};

int main()
{
	auto gui = new Avo::Gui;

	gui->create("Graphics test", {800, 700});

	gui->setThemeColor(Avo::ThemeColors::shadow, {0.f, 0.5f});

	auto card = new Card{gui};
	card->setSize({400, 300});
	gui->sizeChangeListeners += [card, gui](auto) { card->setCenter(gui->getCenter()); };

	gui->run();
}
