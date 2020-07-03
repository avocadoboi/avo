#include <AvoGUI.hpp>

class Card : public Avo::View
{
private:
	Avo::TextView* m_text = new Avo::TextView{this, 15.f, u8"Hover or\nclick me!"};

	Avo::Initializer m_init_layout = [this]{
		m_text->getText().setTextAlign(Avo::TextAlign::Center);
		auto const center = [this](auto) {
			setCenter(getParent<View>()->getCenter());
		};
		sizeChangeListeners += [=](auto) {
			center(Avo::Size<>{});
			m_text->setCenter(Avo::Point{getSize() * 0.5f});
		};
		getParent<View>()->sizeChangeListeners += center;
	};

	//------------------------------

	Avo::Animation m_hoverAnimation{
		getGui(), getThemeEasing(Avo::ThemeEasings::out), 500ms,
		[this](float const p_value){
			setSize(200.f + p_value * 20.f);
			setElevation(3.f + p_value * 20.f);
			invalidate();			
		}
	};
	Avo::Initializer m_init_hover = [this]{
		m_hoverAnimation.updateListeners(0.f);
		mouseEnterListeners += [this](auto const&) { m_hoverAnimation.play(false); };
		mouseLeaveListeners += [this](auto const&) { m_hoverAnimation.play(true); };
	};

	//------------------------------

	Avo::Animation m_clickAnimation{
		getGui(), getThemeEasing(Avo::ThemeEasings::inOut), 300ms,
		[this](float const p_value) {
			setCornerRadius(p_value * getWidth() * 0.5f);
			m_color = Avo::interpolate(getThemeColor(Avo::ThemeColors::background), Avo::Color(1.f, 0.4f, 0.8f), p_value);
			m_text->setColor(Avo::interpolate(
				getThemeColor(Avo::ThemeColors::onBackground), 
				getThemeColor(Avo::ThemeColors::background), 
				p_value
			));
			invalidate();
		}
	};
	Avo::Initializer m_init_click = [this]{
		mouseDownListeners += [this](auto const&) { m_clickAnimation.play(false); };
		mouseUpListeners += [this](auto const&) { m_clickAnimation.play(true); };
	};

	//------------------------------

	Avo::Color m_color = getThemeColor(Avo::ThemeColors::background);

public:
	void draw(Avo::DrawingContext* const p_context) override
	{
		p_context->setColor(m_color);
		p_context->fillRectangle(getSize());
	}

	Card(View* const p_parent) :
		View{p_parent}
	{
		enableMouseEvents();
	}
};

int main()
{
	auto* const gui = new Avo::Gui;
	gui->create("New animation system!", {500, 400});

	new Card{gui};

	gui->run();
}
