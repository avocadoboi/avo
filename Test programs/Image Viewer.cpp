#include "../AvoGUI.hpp"

//------------------------------

float const BACKGROUND_TILE_WIDTH = 30.f;
float const ANIMATION_SPEED = 0.2f;

//------------------------------

class ImageViewer : public AvoGUI::GUI, public AvoGUI::KeyboardEventListener
{
private:
	AvoGUI::Image* m_image;
	const char* m_filePath;

	AvoGUI::Rectangle<float> m_targetImageBounds;

public:
	ImageViewer(const char* p_filePath) : 
		m_filePath(p_filePath)
	{
		create("Image viewer", 600U, 500U, AvoGUI::WindowStyleFlags::Default);
	}
	~ImageViewer()
	{
		m_image->forget();
	}

	//------------------------------

	void createContent() override
	{
		getWindow()->setMinSize(250, 200);
		enableMouseEvents();
		setKeyboardFocus(this);

		m_theme->colors["background"] = AvoGUI::Color(0.3f);

		m_image = getDrawingContext()->createImage(m_filePath);
		m_image->setCenter(getCenter());
		m_targetImageBounds = m_image->getBounds();
		queueAnimationUpdate();
	}

	void handleSizeChange() override
	{
		m_targetImageBounds.setCenter(getCenter());
		queueAnimationUpdate();
	}

	//------------------------------

	void handleKeyboardKeyDown(const AvoGUI::KeyboardEvent& p_event) override
	{
		if (p_event.isRepeated)
		{
			return;
		}
		if (p_event.key == AvoGUI::KeyboardKey::Escape)
		{
			getWindow()->setIsFullscreen(false);
		}
		else if (p_event.key == AvoGUI::KeyboardKey::F4)
		{
			getWindow()->switchFullscreen();
		}
	}

	//------------------------------

	void handleMouseScroll(const AvoGUI::MouseEvent& p_event) override
	{
		float factor = 1.f;
		if (p_event.scrollDelta > 0.f)
		{
			factor = 1.f + p_event.scrollDelta*0.1f;
		}
		else
		{
			factor = 1.f / (1.f - p_event.scrollDelta*0.1f);
		}
		m_targetImageBounds.setSize(m_targetImageBounds.getSize()*factor);
		if (m_targetImageBounds.getWidth() > getWidth() || m_targetImageBounds.getHeight() > getHeight())
		{
			m_targetImageBounds.move((p_event.x - m_targetImageBounds.left)*(1.f - factor), (p_event.y - m_targetImageBounds.top)*(1.f - factor));
		}
		else
		{
			m_targetImageBounds.setCenter(getCenter());
		}
		queueAnimationUpdate();
	}
	void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	{
		if (p_event.modifierKeys == AvoGUI::ModifierKeyFlags::LeftMouse)
		{
			m_targetImageBounds.move(p_event.movementX, p_event.movementY);
			queueAnimationUpdate();
		}
	}

	void updateAnimations() override
	{
		float offsetLeft = m_targetImageBounds.left - m_image->getLeft();
		float offsetTop = m_targetImageBounds.top - m_image->getTop();
		float offsetRight = m_targetImageBounds.right - m_image->getRight();
		float offsetBottom = m_targetImageBounds.bottom - m_image->getBottom();
		if (abs(offsetLeft) > 0.1f || abs(offsetTop) > 0.1f || abs(offsetRight) > 0.1f || abs(offsetBottom) > 0.1f)
		{
			m_image->setBounds(
				m_image->getLeft() + offsetLeft * ANIMATION_SPEED,
				m_image->getTop() + offsetTop * ANIMATION_SPEED,
				m_image->getRight() + offsetRight * ANIMATION_SPEED,
				m_image->getBottom() + offsetBottom * ANIMATION_SPEED
			);
			invalidate();
			queueAnimationUpdate();
		}
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		uint32_t width = ceil(getWidth() / BACKGROUND_TILE_WIDTH);
		uint32_t height = ceil(getHeight() / BACKGROUND_TILE_WIDTH);
		AvoGUI::Color tileColor(0.7f);
		for (uint32_t a = 0; a < width; a++)
		{
			for (uint32_t b = a & 1; b < height; b += 2)
			{
				p_context->setColor(tileColor);
				p_context->fillRectangle(AvoGUI::Point<float>(a*BACKGROUND_TILE_WIDTH, b*BACKGROUND_TILE_WIDTH), AvoGUI::Point<float>(BACKGROUND_TILE_WIDTH, BACKGROUND_TILE_WIDTH));
			}
		}
		p_context->drawImage(m_image);
	}
};

//------------------------------

class MessageBox : public AvoGUI::GUI, public AvoGUI::ButtonListener
{
private:
	AvoGUI::Text* m_message;
	AvoGUI::Button* m_button_ok;

public:
	MessageBox()
	{
		create("No image", 400U, 160U, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}

	void createContent() override
	{
		m_message = getGUI()->getDrawingContext()->createText("No image was given the image viewer. Please open an image using the viewer as the opener.", 16.f, AvoGUI::Rectangle<float>(25.f, 20.f, getWidth() - 25.f, 1000.f));
		m_message->setFontWeight(AvoGUI::FontWeight::Regular);
		m_message->setCharacterSpacing(0.3f);
		m_message->setLineHeight(1.1f);
		m_message->minimizeSize();

		m_button_ok = new AvoGUI::Button(this, "OK");
		m_button_ok->setCenter(m_message->getCenterX());
		m_button_ok->setTop(m_message->getBottom() + 20.f, true);
		m_button_ok->addButtonListener(this);

		setSize(m_message->getRight() + 25.f, m_button_ok->getBottom() + 15.f);
	}

	//------------------------------

	void handleButtonClick(AvoGUI::Button*) override
	{
		getWindow()->close();
	}
	
	//------------------------------

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(0.f));
		p_context->drawText(m_message);
	}
};

//------------------------------

int main(int p_numberOfArguments, char** p_arguments)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (p_numberOfArguments > 1)
	{
		ImageViewer* viewer = new ImageViewer(p_arguments[1]);
		viewer->run();
		viewer->forget();
	}
	else
	{
		MessageBox* message = new MessageBox();
		message->run();
		message->forget();
	}
	return 1;
}
