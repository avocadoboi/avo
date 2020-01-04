#include "../AvoGUI.hpp"

//------------------------------

class DragAndDrop :
	public AvoGUI::Gui
{
private:
	AvoGUI::Text* m_text_dropItems = 0;

	std::vector<AvoGUI::Text*> m_droppedTexts;
	std::vector<AvoGUI::Image*> m_droppedImages;

	//------------------------------

	void addDroppedText(std::string const& p_string, float p_x, float p_y)
	{
		AvoGUI::Text* text = getDrawingContext()->createText(p_string, 25.f);
		text->setCenter(p_x, p_y);
		text->move(AvoGUI::Point<float>().setPolar(AvoGUI::random() * AvoGUI::TAU, 80.f * AvoGUI::random()));
		text->setFontWeight(AvoGUI::FontWeight::Light);
		m_droppedTexts.push_back(text);
	}

public:
	DragAndDrop()
	{
		create(u8"Drag and drop", 800, 600);
		waitForFinish();
	}
	~DragAndDrop()
	{
		m_text_dropItems->forget();
		for (AvoGUI::Text* text : m_droppedTexts)
		{
			text->forget();
		}
		for (AvoGUI::Image* image : m_droppedImages)
		{
			image->forget();
		}
	}

	//------------------------------

	void createContent() override
	{
		enableDragDropEvents();

		setThemeColor("background", AvoGUI::Color(0.1f, 0.f, 0.1f));
		setThemeColor("on background", AvoGUI::Color(1.f));

		m_text_dropItems = getDrawingContext()->createText("Drop something here!", 50.f);
	}

	void handleSizeChange() override
	{
		m_text_dropItems->setCenter(getCenter());
	}

	//------------------------------

	AvoGUI::DragDropOperation handleDragDropMove(AvoGUI::DragDropEvent const& p_event) override
	{
		return AvoGUI::DragDropOperation::Copy;
	}

	void handleDragDropFinish(AvoGUI::DragDropEvent const& p_event) override
	{
		if (p_event.getNumberOfFiles())
		{
			std::vector<std::string> filenames = p_event.getFilenames();
			for (uint32 a = 0; a < filenames.size(); a++)
			{
				addDroppedText(filenames[a], p_event.x, p_event.y);
			}

			/*
				Add image if the first file is one - I don't think it's possible to drag more than one image.
			*/
			AvoGUI::Image* image = p_event.getImage();
			if (image)
			{
				image->setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);
				image->setSize(400.f);
				image->setCenter(p_event.x, p_event.y);
				m_droppedImages.push_back(image);
			}
			else if (!filenames.size() && p_event.getHasString())
			{
				addDroppedText(p_event.getString(), p_event.x, p_event.y);
			}
		}
		else if (p_event.getHasString())
		{
			addDroppedText(p_event.getString(), p_event.x, p_event.y);
		}
		handleSizeChange();
		invalidate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(getThemeColor("on background"), 0.4f));
		p_context->drawText(m_text_dropItems);
		p_context->setColor(getThemeColor("on background"));
		for (AvoGUI::Text* text : m_droppedTexts)
		{
			p_context->drawText(text);
		}
		for (AvoGUI::Image* image : m_droppedImages)
		{
			p_context->drawImage(image);
		}
	}
};

//------------------------------

int main()
{
	new DragAndDrop();
}
