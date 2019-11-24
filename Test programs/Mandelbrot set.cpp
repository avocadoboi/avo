#include "../AvoGUI.hpp"

uint32 const MAX_NUMBER_OF_ITERATIONS = 1000;
double const GLOBAL_SCALE_X = 3.;
double const GLOBAL_SCALE_Y = 2.5;

class App :
	public AvoGUI::GUI
{
private:
	AvoGUI::Image* m_image;
	double m_scale;
	double m_newScale;
	double m_offsetX;
	double m_offsetY;
	double m_newOffsetX;
	double m_newOffsetY;
	char* m_pixels;

	bool m_isDragging;

public:
	App() :
		m_image(0),
		m_scale(1.), m_newScale(1.), m_offsetX(-2.2), m_offsetY(-1.25), m_newOffsetX(-2.2), m_newOffsetY(-1.25), m_isDragging(false)
	{
		create("Mandelbrot set", 700, 600, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}
	~App()
	{
		m_image->forget();
		delete[] m_pixels;
	}

	void renderImage()
	{
		if (m_image)
		{
			m_image->forget();
		}

		for (uint32 x = 0; x < getWidth(); x++)
		{
			for (uint32 y = 0; y < getHeight(); y++)
			{
				char* pixel = m_pixels + 4 * (x + (uint32)getWidth() * y);

				double translatedX = x / getWidth() * GLOBAL_SCALE_X * m_scale + m_offsetX;
				double translatedY = y / getHeight() * GLOBAL_SCALE_Y * m_scale + m_offsetY;

				double real = 0;
				double imaginary = 0;

				uint32 iteration = 0;
				for (; iteration < MAX_NUMBER_OF_ITERATIONS && real * real + imaginary * imaginary <= 4.; iteration++)
				{
					// z^2 + c = (a + ib)^2 + c = (a^2 - b^2 + 2iab) + (x + iy)
					double realBefore = real;
					real = translatedX + real * real - imaginary * imaginary;
					imaginary = translatedY + 2. * realBefore * imaginary;
				}

				AvoGUI::Color color;
				color.setHSB(iteration / (double)MAX_NUMBER_OF_ITERATIONS, 1.f, iteration == MAX_NUMBER_OF_ITERATIONS ? 0.f : 1.f);//(1.f - AvoGUI::square(1.f - 2.f * iteration / (double)MAX_NUMBER_OF_ITERATIONS)));

				// Red
				pixel[2] = color.red * 255;
				// Green
				pixel[1] = color.green * 255;
				// Blue
				pixel[0] = color.blue * 255;
				// Alpha
				pixel[3] = 255;
			}
		}

		m_image = getDrawingContext()->createImage(m_pixels, getWidth(), getHeight());
	}

	void createContent() override
	{
		enableMouseEvents();
		m_pixels = new char[getWidth() * getHeight() * 4];
		renderImage();
	}
	void handleSizeChange() override
	{

	}

	void handleMouseScroll(AvoGUI::MouseEvent const& p_event) override
	{
		if (p_event.scrollDelta > 0.)
		{
			m_newScale /= 1. + p_event.scrollDelta * 0.1;
		}
		else
		{
			m_newScale *= 1. - p_event.scrollDelta * 0.1;
		}
		if (abs(m_newScale / m_scale - 1.) > 0.3)
		{ 
			m_offsetX -= p_event.x / getWidth() * (m_newScale - m_scale)* GLOBAL_SCALE_X;
			m_offsetY -= p_event.y / getHeight() * (m_newScale - m_scale)* GLOBAL_SCALE_Y;
			m_newOffsetX = m_offsetX;
			m_newOffsetY = m_offsetY;
			m_scale = m_newScale;
			renderImage();
			invalidate();
		}
	}
	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		m_isDragging = true;
	}
	void handleMouseUp(AvoGUI::MouseEvent const& p_event) override
	{
		m_isDragging = false;
	}
	void handleMouseMove(AvoGUI::MouseEvent const& p_event) override
	{
		if (m_isDragging)
		{
			m_newOffsetX -= p_event.movementX/getWidth() * m_scale* GLOBAL_SCALE_X;
			m_newOffsetY -= p_event.movementY/getHeight() * m_scale* GLOBAL_SCALE_Y;
			if (AvoGUI::square(m_newOffsetX - m_offsetX) + AvoGUI::square(m_newOffsetY - m_offsetY) > AvoGUI::square(m_scale * 0.3))
			{
				m_offsetX = m_newOffsetX;
				m_offsetY = m_newOffsetY;
				renderImage();
				invalidate();
			}
		}
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->drawImage(m_image);
	}
};

//------------------------------

int main()
{
	App* app = new App();
	//App* app_1 = new App();
	AvoGUI::GUI::run();
}
