#include "../AvoGUI.hpp"

class Card : public AvoGUI::View
{
private:
	AvoGUI::Text m_text;
	AvoGUI::LinearGradient m_gradient;
	AvoGUI::RadialGradient m_radialGradient;

public:
	Card(View* p_parent) :
		View(p_parent)
	{
		setCornerCutSize(20.f, 0.f, 0.f, 20.f);
		setElevation(4.f);

		m_text = getDrawingContext()->createText(u8"بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا ", 24.f);
		m_text.setFontWeight(AvoGUI::FontWeight::Light);
		m_text.setWordWrapping(AvoGUI::WordWrapping::WholeWord);
		m_text.setReadingDirection(AvoGUI::ReadingDirection::RightToLeft);

		m_gradient = getDrawingContext()->createLinearGradient({ 
			{ AvoGUI::Color(255, 0, 0), 0.f }, { AvoGUI::Color(0, 0, 255), 1.f } 
		});
		m_radialGradient = getDrawingContext()->createRadialGradient(
			{
				{ { 1.f, 1.f, 0.f, 1.f }, 0.f },
				{ { 0.f, 0.f, 0.f, 0.f }, 1.f }
			}, getTopLeft(), 200.f
		);
	}

	void handleSizeChange()
	{
		m_text.setWidth(getWidth() - 50.f);
		m_text.fitHeightToText();

		m_text.setCenterX(getCenterX());
		m_text.setTop(18.f);
		setHeight(m_text.getBottom() + 18.f);

		m_gradient.setEndPosition(getSize());
	}

	void draw(AvoGUI::DrawingContext* p_context)
	{
		p_context->setGradient(m_gradient);
		p_context->fillRectangle(getSize());

		p_context->setGradient(m_radialGradient);
		p_context->fillRectangle(getSize());

		p_context->setColor(AvoGUI::Color(1.f));
		p_context->drawText(m_text);
	}
};

int main()
{
	auto gui = new AvoGUI::Gui();

	gui->create("Graphics test", 800, 700);

	gui->setThemeColor(AvoGUI::ThemeColors::shadow, AvoGUI::Color(0.f, 0.5f));

	auto card = new Card(gui);
	card->setSize(400, 300);
	gui->sizeChangeListeners += [=](auto...) { card->setCenter(gui->getCenter()); };

	gui->run();
}
