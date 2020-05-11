#include "../AvoGUI.hpp"

class Card : public AvoGUI::View
{
private:
	AvoGUI::Color m_color{ getThemeColor(AvoGUI::ThemeColors::background) };

public:
	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(m_color);
		p_context->fillRectangle(getSize());
	}

	Card(View* p_parent) :
		View(p_parent)
	{
		auto text = new AvoGUI::TextView(this, 15.f, u8"Hover or\nclick me!");
		text->getText().setTextAlign(AvoGUI::TextAlign::Center);

		auto center = [=](auto...) {
			setCenter(getParent<View>()->getCenter());
		};
		sizeChangeListeners += [=](auto...) {
			center();
			text->setCenter(getSize() * 0.5f);
		};
		getParent<View>()->sizeChangeListeners += center;

		auto hoverAnimation = createAnimation(AvoGUI::ThemeEasings::out, 500.f);
		hoverAnimation->updateListeners += [=](float p_value) {
			setSize(200.f + p_value * 20.f);
			setElevation(3.f + p_value * 20.f);
			invalidate();
		};
		hoverAnimation->updateListeners(0.f);
		enableMouseEvents();
		mouseEnterListeners += [=](auto) { hoverAnimation->play(false); };
		mouseLeaveListeners += [=](auto) { hoverAnimation->play(true); };

		auto clickAnimation = createAnimation(AvoGUI::ThemeEasings::inOut, 300.f);
		clickAnimation->updateListeners += [=](float p_value) {
			setCornerRadius(p_value * getWidth() * 0.5f);
			m_color = AvoGUI::interpolate(getThemeColor(AvoGUI::ThemeColors::background), AvoGUI::Color(1.f, 0.4f, 0.8f), p_value);
			text->setColor(AvoGUI::interpolate(getThemeColor(AvoGUI::ThemeColors::onBackground), getThemeColor(AvoGUI::ThemeColors::background), p_value));
			invalidate();
		};
		mouseDownListeners += [=](auto) { clickAnimation->play(false); };
		mouseUpListeners += [=](auto) { clickAnimation->play(true); };
	}
};

int main()
{
	auto gui = new AvoGUI::Gui();
	gui->create("New animation system!", 500, 400);

	new Card(gui);

	gui->run();
}
