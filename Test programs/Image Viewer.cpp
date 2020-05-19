#include "../AvoGUI.hpp"
#include <chrono>

//------------------------------

float const BACKGROUND_TILE_WIDTH = 30.f;
float const ANIMATION_SPEED = 0.2f;

//------------------------------

class ImageViewer : public Avo::Gui
{
private:
	Avo::Image m_image;
	std::string m_filePath;

	Avo::Rectangle<float> m_targetImageBounds;

public:
	ImageViewer(char const* p_filePath) :
		m_filePath(p_filePath)
	{
		create("Image viewer", 600, 500, Avo::WindowStyleFlags::Default);

		getWindow()->setMinSize(250, 200);
		enableMouseEvents();
		setKeyboardFocus(this);

		setThemeColor(Avo::ThemeColors::background, Avo::Color(0.3f));

		m_image = getDrawingContext()->createImage(m_filePath);
		m_image.setCenter(getCenter());
		m_targetImageBounds = m_image.getBounds();

		queueAnimationUpdate();

		run();
	}

	//------------------------------

	void handleSizeChange() override
	{
		m_targetImageBounds.setCenter(getCenter());
		queueAnimationUpdate();
	}

	//------------------------------

	void handleKeyboardKeyDown(Avo::KeyboardEvent const& p_event) override
	{
		if (p_event.isRepeated)
		{
			return;
		}
		if (p_event.key == Avo::KeyboardKey::Escape)
		{
			getWindow()->setIsFullscreen(false);
		}
		else if (p_event.key == Avo::KeyboardKey::F4)
		{
			getWindow()->switchFullscreen();
		}
	}

	//------------------------------

	void handleMouseScroll(Avo::MouseEvent const& p_event) override
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
	void handleMouseMove(Avo::MouseEvent const& p_event) override
	{
		if (getWindow()->getIsMouseButtonDown(Avo::MouseButton::Left))
		{
			m_targetImageBounds.move(p_event.movementX, p_event.movementY);
			queueAnimationUpdate();
		}
	}

	void updateAnimations() override
	{
		float offsetLeft = m_targetImageBounds.left - m_image.getLeft();
		float offsetTop = m_targetImageBounds.top - m_image.getTop();
		float offsetRight = m_targetImageBounds.right - m_image.getRight();
		float offsetBottom = m_targetImageBounds.bottom - m_image.getBottom();
		if (abs(offsetLeft) > 0.1f || abs(offsetTop) > 0.1f || abs(offsetRight) > 0.1f || abs(offsetBottom) > 0.1f)
		{
			m_image.setBounds(
				m_image.getLeft() + offsetLeft * ANIMATION_SPEED,
				m_image.getTop() + offsetTop * ANIMATION_SPEED,
				m_image.getRight() + offsetRight * ANIMATION_SPEED,
				m_image.getBottom() + offsetBottom * ANIMATION_SPEED
			);
			invalidate();
			queueAnimationUpdate();
		}
	}

	//------------------------------

	void draw(Avo::DrawingContext* p_context) override
	{
		uint32_t width = ceil(getWidth() / BACKGROUND_TILE_WIDTH);
		uint32_t height = ceil(getHeight() / BACKGROUND_TILE_WIDTH);
		Avo::Color tileColor(0.7f);
		for (uint32_t a = 0; a < width; a++)
		{
			for (uint32_t b = a & 1; b < height; b += 2)
			{
				p_context->setColor(tileColor);
				p_context->fillRectangle(Avo::Point<float>(a*BACKGROUND_TILE_WIDTH, b*BACKGROUND_TILE_WIDTH), Avo::Point<float>(BACKGROUND_TILE_WIDTH, BACKGROUND_TILE_WIDTH));
			}
		}
		p_context->drawImage(m_image);
	}
};

//------------------------------

int main(int p_numberOfArguments, char** p_arguments)
{
	if (p_numberOfArguments > 1)
	{
		new ImageViewer(p_arguments[1]);
	}
	else
	{
		auto messageBox = new Avo::Gui();
		messageBox->create("No image!", 400, 0, Avo::WindowStyleFlags::DefaultNoResize);

		messageBox->enableMouseEvents();

		auto messageText = new Avo::TextView(messageBox, 16.f, u8"No image was given the image viewer. Please open an image using the viewer as the opener.");
		messageText->getText().setWordWrapping(Avo::WordWrapping::WholeWord);
		messageText->getText().setFontWeight(Avo::FontWeight::Regular);
		messageText->getText().setCharacterSpacing(0.3f);
		messageText->getText().setLineHeight(1.1f);

		messageText->setWidth(messageBox->getWidth() - 50.f);
		messageText->fitSizeToText();
		messageText->setCenterX(messageBox->getWidth()/2);
		messageText->setTop(20.f);
		
		auto okButton = new Avo::Button(messageBox, u8"OK");
		okButton->setCenterX(messageText->getCenterX());
		okButton->setTop(messageText->getBottom() + 20.f, true);
		okButton->buttonClickListeners += [=](auto) { messageBox->getWindow()->close(); };

		messageBox->setHeight(okButton->getBottom() + 15.f);

		messageBox->run();
	}
	return 1;
}
