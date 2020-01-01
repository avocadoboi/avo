#include "../AvoGUI.hpp"

//------------------------------

class DragAndDrop :
	public AvoGUI::Gui
{
public:
	DragAndDrop()
	{
		create(u8"Drag and drop", 600, 500);
		waitForFinish();
	}

	void createContent() override
	{
		setThemeColor("background", AvoGUI::Color(0.1f));
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
	}
};

//------------------------------

int main()
{
	new DragAndDrop();
}
