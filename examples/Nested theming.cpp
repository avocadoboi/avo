#include <AvoGUI.hpp>

class ThemedCard : public Avo::View {
private:
	auto draw(Avo::DrawingContext* p_context) -> void override {
		p_context->clear(getThemeColor(Avo::ThemeColors::background));
	}
	
public:
	ThemedCard(View* p_parent) :
		View{p_parent, p_parent->getSize()/2}
	{
		setElevation(5.f);
	}
};

class Themed : public Avo::Gui {
private:
	auto createLayout() -> void {
		auto const addCard = [this](View* parent) -> View* {
			auto* const card = p_parent->addView<ThemedCard>();
			card->setThemeColor(
				Avo::ThemeColors::background, 
				Avo::Color::hsb(Avo::random(), 1.f, 1.f)
			);
			return card;
		};

		auto card = addCard(this);

		auto childCard = addCard(card);
	}
	
public:
	Themed() {
		create("Nested themes!", {500, 400});

		createLayout();
		
		run();
	}
};

auto main() -> int {
	Themed{};
}
