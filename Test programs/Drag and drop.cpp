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
		enableDragDropEvents();

		setThemeColor("background", AvoGUI::Color(0.1f));
	}

	AvoGUI::DragDropOperation handleDragDropMove(AvoGUI::DragDropEvent const& p_event) override
	{
		return AvoGUI::DragDropOperation::Copy;
	}
	void handleDragDropFinish(AvoGUI::DragDropEvent const& p_event) override
	{
		p_event.getFilenames();
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
