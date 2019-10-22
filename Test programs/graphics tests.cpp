#include "../AvoGUI.hpp"

class Card : public AvoGUI::View
{
private:
	AvoGUI::Text* m_text;

public:
	Card(View* p_parent) :
		View(p_parent)
	{
		//setCornerCutSize(10.f);
		setCornerCutSize(20.f, 0.f, 0.f, 20.f);
		setElevation(4.f);

		m_text = getGUI()->getDrawingContext()->createText(u8"بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا بعضهم بعضًا ", 24.f);
		m_text->setFontWeight(AvoGUI::FontWeight::Light);
		m_text->setWordWrapping(AvoGUI::WordWrapping::WholeWord);
		m_text->setReadingDirection(AvoGUI::ReadingDirection::RightToLeft);
	}

	void handleSizeChange()
	{
		m_text->setWidth(getWidth() - 50.f);
		m_text->fitHeightToText();

		m_text->setCenterX(getCenterX());
		m_text->setTop(18.f);
		setHeight(m_text->getBottom() + 18.f);
	}

	void draw(AvoGUI::DrawingContext* p_context)
	{
		p_context->setColor(AvoGUI::Color(1.f));
		p_context->fillRectangle(getSize());

		//p_context->setColor(AvoGUI::Color(0.1f, 0.9f, 0.7f));
		//p_context->fillRectangle(m_text->getBounds());

		p_context->setColor(AvoGUI::Color(0.f));
		p_context->drawText(m_text);
	}
};

class GUI : public AvoGUI::GUI
{
private:
	Card* m_card;

public:
	GUI()
	{
		create("Graphics test", 800, 700);
	}

	void createContent()
	{
		setThemeColor("shadow", AvoGUI::Color(0.f, 0.5f));

		m_card = new Card(this);
		m_card->setSize(400, 300);
		m_card->invalidate();
	}
	void handleSizeChange()
	{
		m_card->setCenter(getCenter());
	}

	void draw(AvoGUI::DrawingContext* p_context)
	{
		p_context->setColor(AvoGUI::Color(0.9f, 0.1f, 0.5f));
	}
};

int main()
{
	GUI* gui = new GUI();
	AvoGUI::GUI::run();
	gui->forget();
}
