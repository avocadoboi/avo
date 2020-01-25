#include "../AvoGUI.hpp"

#include <filesystem>

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
		text->setFontWeight(AvoGUI::FontWeight::Light);
		if (text->getWidth() > 550.f)
		{
			text->setWidth(550.f);
			text->setWordWrapping(AvoGUI::WordWrapping::WholeWord);
		}
		text->fitHeightToText();
		text->setCenter(p_x, p_y);
		text->move(AvoGUI::Point<float>().setPolar(AvoGUI::random() * AvoGUI::TAU, 20.f * AvoGUI::random()));
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
		enableMouseEvents();

		setThemeColor("background", AvoGUI::Color(0.1f, 0.f, 0.1f));
		setThemeColor("on background", AvoGUI::Color(1.f));

		m_text_dropItems = getDrawingContext()->createText("Drop something here!", 50.f);
	}

	void handleSizeChange() override
	{
		m_text_dropItems->setCenter(getCenter());
	}

	//------------------------------

	void handleMouseMove(AvoGUI::MouseEvent const& p_event) override
	{
		if (getWindow()->getIsMouseButtonDown(AvoGUI::MouseButton::Left))
		{
			for (uint32 a = 0; a < m_droppedImages.size(); a++)
			{
				if (m_droppedImages[a]->getInnerBounds().getIsContaining(p_event.x, p_event.y))
				{
					AvoGUI::Image* image = m_droppedImages[a];
					m_droppedImages.erase(m_droppedImages.begin() + a);
					invalidateRectangle(image->getBounds());
					getWindow()->dragAndDropImage(image);
					image->forget();
					return;
				}
			}
			for (uint32 a = 0; a < m_droppedTexts.size(); a++)
			{
				if (m_droppedTexts[a]->getIsContaining(p_event.x, p_event.y))
				{
					std::string string = m_droppedTexts[a]->getString();

					invalidateRectangle(m_droppedTexts[a]->getBounds());
					m_droppedTexts[a]->forget();
					m_droppedTexts.erase(m_droppedTexts.begin() + a);
					if (std::filesystem::exists(std::filesystem::u8path(string)))
					{
						getWindow()->dragAndDropFile(string);
					}
					else
					{
						getWindow()->dragAndDropString(string);
					}
					return;
				}
			}
		}
	}

	//------------------------------

	AvoGUI::DragDropOperation getDragDropOperation(AvoGUI::DragDropEvent const& p_event) override
	{
		return AvoGUI::DragDropOperation::Copy;
	}

	void handleDragDropFinish(AvoGUI::DragDropEvent const& p_event) override
	{
		/*
			Add names of dropped directories and files, if any items were dropped.
		*/
		std::vector<std::string> itemNames = p_event.data->getItemNames();
		for (uint32 a = 0; a < itemNames.size(); a++)
		{
			addDroppedText(itemNames[a], p_event.x, p_event.y);
		}

		/*
			Add image if the first file is one - I don't think it's possible to drag more than one image.
		*/
		AvoGUI::Image* image = 0;
		if (image = p_event.data->getImage())
		{
			image->setBoundsSizing(AvoGUI::ImageBoundsSizing::Contain);
			image->setSize(350.f);
			image->setCenter(p_event.x, p_event.y);
			m_droppedImages.push_back(image);
		}
		
		/*
			Add dropped text, if any text was dropped.
		*/
		if (!image && !itemNames.size() && p_event.data->getHasString())
		{
			addDroppedText(p_event.data->getString(), p_event.x, p_event.y);
		}

		handleSizeChange();
		invalidate();
	}

	void draw(AvoGUI::DrawingContext* p_context, AvoGUI::Rectangle<float> const& p_target) override
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
