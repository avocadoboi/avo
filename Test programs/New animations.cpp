#include "../AvoGUI.hpp"

class Card : public Avo::View
{
private:
	Avo::Color m_color{ getThemeColor(Avo::ThemeColors::background) };

public:
	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(m_color);
		p_context->fillRectangle(getSize());
	}

	Card(View* p_parent) :
		View(p_parent)
	{
		auto text = new Avo::TextView(this, 15.f, u8"Hover or\nclick me!");
		text->getText().setTextAlign(Avo::TextAlign::Center);

		auto center = [=](auto...) {
			setCenter(getParent<View>()->getCenter());
		};
		sizeChangeListeners += [=](auto...) {
			center();
			text->setCenter(getSize() * 0.5f);
		};
		getParent<View>()->sizeChangeListeners += center;

		auto hoverAnimation = createAnimation(Avo::ThemeEasings::out, 500.f);
		hoverAnimation->updateListeners += [=](float p_value) {
			setSize(200.f + p_value * 20.f);
			setElevation(3.f + p_value * 20.f);
			invalidate();
		};
		hoverAnimation->updateListeners(0.f);
		enableMouseEvents();
		mouseEnterListeners += [=](auto) { hoverAnimation->play(false); };
		mouseLeaveListeners += [=](auto) { hoverAnimation->play(true); };

		auto clickAnimation = createAnimation(Avo::ThemeEasings::inOut, 300.f);
		clickAnimation->updateListeners += [=](float p_value) {
			setCornerRadius(p_value * getWidth() * 0.5f);
			m_color = Avo::interpolate(getThemeColor(Avo::ThemeColors::background), Avo::Color(1.f, 0.4f, 0.8f), p_value);
			text->setColor(Avo::interpolate(getThemeColor(Avo::ThemeColors::onBackground), getThemeColor(Avo::ThemeColors::background), p_value));
			invalidate();
		};
		mouseDownListeners += [=](auto) { clickAnimation->play(false); };
		mouseUpListeners += [=](auto) { clickAnimation->play(true); };
	}
};

int main()
{
	auto gui = new Avo::Gui();
	gui->create("New animation system!", 500, 400);

	new Card(gui);

	gui->run();
}
