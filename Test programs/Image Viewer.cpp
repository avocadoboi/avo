#include "../AvoGUI.hpp"

//------------------------------

class ImageViewer : public AvoGUI::GUI
{
private:
	AvoGUI::Image* m_image;
	const char* m_filePath;

public:
	ImageViewer(const char* p_filePath) : 
		m_filePath(p_filePath)
	{
		create("Image viewer", 600U, 500U, AvoGUI::WindowStyleFlags::Default);
	}

	//------------------------------

	void createContent() override
	{
		m_image = getDrawingContext()->createImage(m_filePath);
	}

	void handleSizeChange() override
	{
		m_image->setCenter(getCenter());
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_context) override
	{
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
