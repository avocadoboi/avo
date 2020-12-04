#include "../AvoGUI.hpp"

#include <filesystem>

//------------------------------

class DragAndDrop : public Avo::Gui
{
private:
	std::vector<Avo::Text> m_droppedTexts;
	std::vector<Avo::Image> m_droppedImages;

	//------------------------------

	auto addDroppedText(std::string const& p_string, Avo::Point<> const p_position) -> void
	{
		auto text = getDrawingContext()->createText(p_string, 25.f);
		text.setFontWeight(Avo::FontWeight::Light);
		if (text.getWidth() > 550.f)
		{
			text.setWidth(550.f);
			text.setWordWrapping(Avo::WordWrapping::WholeWord);
		}
		text.fitHeightToText();
		text.setCenter(p_position);
		text.move(Avo::Vector2d<>::fromPolar(Avo::Radians{Avo::random() * Avo::TAU<float>}, 20.f * Avo::random()));
		m_droppedTexts.push_back(std::move(text));
	}

	bool m_startedDragging{};
public:
	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		if (p_event.mouseButton == Avo::MouseButton::Left)
		{
			m_startedDragging = true;
		}
	}
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		if (getWindow()->getIsMouseButtonDown(Avo::MouseButton::Left) && m_startedDragging)
		{
			m_startedDragging = false;

			for (auto const a : Avo::Indices{m_droppedImages})
			{
				if (m_droppedImages[a].getInnerBounds().getIsContaining(p_event.xy))
				{
					auto const image = m_droppedImages[a];
					m_droppedImages.erase(m_droppedImages.begin() + a);
					getWindow()->dragAndDropImage(image);
					invalidate();
					return;
				}
			}

			for (auto const a : Avo::Indices{m_droppedTexts})
			{
				if (m_droppedTexts[a].getIsContaining(p_event.xy))
				{
					auto const string = m_droppedTexts[a].getString();

					if (std::filesystem::exists(std::filesystem::u8path(string)))
					{
						getWindow()->dragAndDropFile(string);
					}
					else
					{
						getWindow()->dragAndDropString(string);
					}
					m_droppedTexts.erase(m_droppedTexts.begin() + a);
					invalidate();
					return;
				}
			}
		}
	}

	//------------------------------

	auto getDragDropOperation(Avo::DragDropEvent const& p_event) const -> Avo::DragDropOperation override
	{
		return Avo::DragDropOperation::Copy;
	}

	void handleDragDropFinish(Avo::DragDropEvent const& p_event) override
	{
		/*
			Add names of dropped directories and files, if any items were dropped.
		*/
		auto const itemNames = p_event.data->getItemNames();
		for (auto const& name : itemNames)
		{
			addDroppedText(name, p_event.xy);
		}

		/*
			Add image if the first file is one - I don't think it's possible to drag more than one image.
		*/
		Avo::Image image;
		if (image = p_event.data->getImage())
		{
			image.setBoundsSizing(Avo::ImageBoundsSizing::Contain);
			image.setSize(350.f);
			image.setCenter(p_event.xy);
			m_droppedImages.push_back(image);
		}
		
		/*
			Add dropped text, if any text was dropped.
		*/
		if (!image && !itemNames.size() && p_event.data->getHasString())
		{
			addDroppedText(p_event.data->getString(), p_event.xy);
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
		create(u8"Drag and drop", {800, 600});

		enableDragDropEvents();
		enableMouseEvents();

		setThemeColor(Avo::ThemeColors::background, {0.1f, 0.f, 0.1f});
		setThemeColor(Avo::ThemeColors::onBackground, {1.f});

		auto text_dropItems = new Avo::TextView{this, 50.f, "Drop something here!"};
		text_dropItems->setColor({getThemeColor(Avo::ThemeColors::onBackground), 0.4f});
		sizeChangeListeners += [=](auto) { 
			text_dropItems->setCenter(Avo::Point{getSize()/2}); 
		};

		run();
	}
};

//------------------------------

int main()
{
	new DragAndDrop{};
}
