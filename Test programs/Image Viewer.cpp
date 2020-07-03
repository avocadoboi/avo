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
	Avo::Rectangle<float> m_targetImageBounds;
public:
	auto handleSizeChange() -> void override
	{
		m_targetImageBounds.setCenter(getCenter());
		queueAnimationUpdate();
	}

	//------------------------------

	auto handleMouseScroll(Avo::MouseEvent const& p_event) -> void override
	{
		auto factor = 1.f;
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
			m_targetImageBounds += (p_event.xy - m_targetImageBounds.getTopLeft())*(1.f - factor);
		}
		else
		{
			m_targetImageBounds.setCenter(getCenter());
		}
		queueAnimationUpdate();
	}
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		if (getWindow()->getIsMouseButtonDown(Avo::MouseButton::Left))
		{
			m_targetImageBounds += p_event.movement;
			queueAnimationUpdate();
		}
	}

	auto updateAnimations() -> void override
	{
		if (auto const offset = m_targetImageBounds - m_image.getBounds();
		    std::abs(offset.left) > 0.1f || std::abs(offset.top) > 0.1f || 
		    std::abs(offset.right) > 0.1f || std::abs(offset.bottom) > 0.1f)
		{
			m_image.setBounds(m_image.getBounds() + offset*ANIMATION_SPEED);
			invalidate();
			queueAnimationUpdate();
		}
	}

	//------------------------------

	auto draw(Avo::DrawingContext* p_context) -> void override
	{
		constexpr auto tileColor = Avo::Color{0.7f};
		auto const width = static_cast<Avo::Count>(std::ceil(getWidth() / BACKGROUND_TILE_WIDTH));
		auto const height = static_cast<Avo::Count>(std::ceil(getHeight() / BACKGROUND_TILE_WIDTH));
		for (auto const a : Avo::Indices{width})
		{
			for (Avo::Index b = a & 1; b < height; b += 2)
			{
				p_context->setColor(tileColor);
				p_context->fillRectangle({
					Avo::Point{static_cast<float>(a), static_cast<float>(b)}*BACKGROUND_TILE_WIDTH, 
					Avo::Size{BACKGROUND_TILE_WIDTH}
				});
			}
		}
		p_context->drawImage(m_image);
	}

	//------------------------------

	auto handleKeyboardKeyDown(Avo::KeyboardEvent const& p_event) -> void override
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

	ImageViewer(std::string_view p_filePath)
	{
		create("Image viewer", {600, 500}, Avo::WindowStyleFlags::Default);

		getWindow()->setMinSize({250, 200});
		enableMouseEvents();
		setKeyboardFocus(this);

		setThemeColor(Avo::ThemeColors::background, 0.3f);

		m_image = getDrawingContext()->createImage(m_filePath);
		m_image.setCenter(getCenter());
		m_targetImageBounds = m_image.getBounds();

		queueAnimationUpdate();

		run();
	}
};

//------------------------------

auto main(int p_numberOfArguments, char** p_arguments) -> int
{
	if (p_numberOfArguments > 1)
	{
		new ImageViewer{p_arguments[1]};
	}
	else
	{
		auto* const messageBox = new Avo::Gui;
		messageBox->create("No image!", {400, 0}, Avo::WindowStyleFlags::DefaultNoResize);
		messageBox->enableMouseEvents();

		auto* const messageText = new Avo::TextView{
			messageBox, 16.f, 
			"No image was given the image viewer. Please open an image using the viewer as the opener."
		};
		messageText->getText().setWordWrapping(Avo::WordWrapping::WholeWord);
		messageText->getText().setFontWeight(Avo::FontWeight::Regular);
		messageText->getText().setCharacterSpacing(0.3f);
		messageText->getText().setLineHeight(1.1f);

		messageText->setWidth(messageBox->getWidth() - 50.f);
		messageText->fitSizeToText();
		messageText->setCenterX(messageBox->getWidth()/2);
		messageText->setTop(20.f);
		
		auto* const okButton = new Avo::Button{messageBox, "OK"};
		okButton->setCenterX(messageText->getCenterX());
		okButton->setTop(messageText->getBottom() + 20.f, true);
		okButton->buttonClickListeners += [messageBox](auto) { messageBox->getWindow()->close(); };

		messageBox->setHeight(okButton->getBottom() + 15.f);

		messageBox->run();
	}
	return 1;
}
