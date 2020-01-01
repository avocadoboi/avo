#include "../AvoGUI.hpp"

//------------------------------

class DragAndDrop :
	public AvoGUI::Gui
{
public:
	DragAndDrop()
	{
		create(u8"böböbñãåö \U0001F914", 600, 500);
		waitForFinish();
	}

	void createContent() override
	{
		setThemeColor("background", AvoGUI::Color(0.1f));
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(1.f, 0.8f, 0.f));
		p_context->fillCircle(getCenter(), 50.f);
	}
};

//------------------------------

int main()
{
	new DragAndDrop();
}
