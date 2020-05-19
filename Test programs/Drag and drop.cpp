#include "../AvoGUI.hpp"

#include <filesystem>

//------------------------------

class DragAndDrop : public Avo::Gui
{
private:
	std::vector<Avo::Text> m_droppedTexts;
	std::vector<Avo::Image> m_droppedImages;

	//------------------------------

	void addDroppedText(std::string const& p_string, float p_x, float p_y)
	{
		auto text = getDrawingContext()->createText(p_string, 25.f);
		text.setFontWeight(Avo::FontWeight::Light);
		if (text.getWidth() > 550.f)
		{
			text.setWidth(550.f);
			text.setWordWrapping(Avo::WordWrapping::WholeWord);
		}
		text.fitHeightToText();
		text.setCenter(p_x, p_y);
		text.move(Avo::Point<float>().setPolar(Avo::random() * Avo::TAU, 20.f * Avo::random()));
		m_droppedTexts.push_back(text);
	}

public:
	void handleMouseMove(Avo::MouseEvent const& p_event) override
	{
		if (getWindow()->getIsMouseButtonDown(Avo::MouseButton::Left))
		{
			for (uint32 a = 0; a < m_droppedImages.size(); a++)
			{
				if (m_droppedImages[a].getInnerBounds().getIsContaining(p_event.x, p_event.y))
				{
					Avo::Image image = m_droppedImages[a];
					m_droppedImages.erase(m_droppedImages.begin() + a);
					invalidateRectangle(image.getBounds());
					getWindow()->dragAndDropImage(image);
					return;
				}
			}
			for (uint32 a = 0; a < m_droppedTexts.size(); a++)
			{
				if (m_droppedTexts[a].getIsContaining(p_event.x, p_event.y))
				{
					std::string string = m_droppedTexts[a].getString();

					invalidateRectangle(m_droppedTexts[a].getBounds());
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

	Avo::DragDropOperation getDragDropOperation(Avo::DragDropEvent const& p_event) override
	{
		return Avo::DragDropOperation::Copy;
	}

	void handleDragDropFinish(Avo::DragDropEvent const& p_event) override
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
		Avo::Image image;
		if (image = p_event.data->getImage())
		{
			image.setBoundsSizing(Avo::ImageBoundsSizing::Contain);
			image.setSize(350.f);
			image.setCenter(p_event.x, p_event.y);
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

	void drawOverlay(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(getThemeColor(Avo::ThemeColors::onBackground));
		for (auto& text : m_droppedTexts)
		{
			p_context->drawText(text);
		}
		for (auto& image : m_droppedImages)
		{
			p_context->drawImage(image);
		}
	}

	DragAndDrop()
	{
		create(u8"Drag and drop", 800, 600);

		enableDragDropEvents();
		enableMouseEvents();

		setThemeColor(Avo::ThemeColors::background, Avo::Color(0.1f, 0.f, 0.1f));
		setThemeColor(Avo::ThemeColors::onBackground, Avo::Color(1.f));

		auto text_dropItems = new Avo::TextView(this, 50.f, "Drop something here!");
		text_dropItems->setColor(Avo::Color(getThemeColor(Avo::ThemeColors::onBackground), 0.4f));
		sizeChangeListeners += [=](auto...) { text_dropItems->setCenter(getSize() * 0.5f); };

		run();
	}
};

//------------------------------

int main()
{
	new DragAndDrop();
}
