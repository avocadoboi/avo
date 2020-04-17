#pragma once
#include "../../AvoGUI.hpp"

//------------------------------

uint32 const MAX_NUMBER_OF_ITERATIONS_START = 100;
double const MAX_NUMBER_OF_ITERATIONS_SCALE_INCREASE = 30.;
double const GLOBAL_SCALE_X = 3.;
double const GLOBAL_SCALE_Y = 2.5;

uint32 const WIDTH = 700;
uint32 const HEIGHT_PER_THREAD = 60;
uint32 const NUMBER_OF_DRAWING_THREADS = 10;

//------------------------------

class MandelbrotViewer;

class MandelbrotRenderer
{
private:
	MandelbrotViewer* m_viewer;
	uint32 m_partIndex;
	AvoGUI::Image* m_image{ nullptr };
	std::mutex m_imageMutex;

	bool m_needsRendering{ false };
	std::condition_variable m_needsRenderingConditionVariable;
	std::mutex m_needsRenderingMutex;
	std::thread m_renderingThread;

	void render();

public:
	MandelbrotRenderer(MandelbrotViewer* p_app, uint32 p_partIndex) :
		m_viewer(p_app), m_partIndex(p_partIndex)
	{
		m_renderingThread = std::thread(&MandelbrotRenderer::render, this);
	}
	~MandelbrotRenderer()
	{
		startRender();
		m_renderingThread.join();
		if (m_image)
		{
			m_image->forget();
		}
	}

	void startRender()
	{
		m_needsRenderingMutex.lock();
		m_needsRendering = true;
		m_needsRenderingMutex.unlock();
		m_needsRenderingConditionVariable.notify_all();
	}

	void draw(AvoGUI::DrawingContext* p_context, AvoGUI::Rectangle<float> const& p_targetRectangle)
	{
		m_imageMutex.lock();
		if (m_image && m_image->getIsIntersecting(p_targetRectangle))
		{
			p_context->drawImage(m_image);
		}
		m_imageMutex.unlock();
	}
};

//------------------------------

class MandelbrotViewer :
	public AvoGUI::Gui
{
private:
	std::vector<MandelbrotRenderer*> m_renderers;
	uint8 m_pixels[WIDTH * HEIGHT_PER_THREAD * NUMBER_OF_DRAWING_THREADS * 4];
	bool m_isRunning{ true };

	uint32 m_maxNumberOfIterations{ MAX_NUMBER_OF_ITERATIONS_START };
	double m_scale{ 1. };
	double m_newScale{ 1. };
	double m_offsetX{ -2.2 };
	double m_offsetY{ -1.25 };
	double m_newOffsetX{ -2.2 };
	double m_newOffsetY{ -1.25 };

	bool m_isDragging{ false };

	//------------------------------

	AvoGUI::Text* m_infoText{ nullptr };

	void updateInfoText()
	{
		if (m_infoText)
		{
			m_infoText->forget();
		}
		m_infoText = getDrawingContext()->createText(("Max iterations: " + AvoGUI::convertNumberToString(m_maxNumberOfIterations) + " Scale: " + AvoGUI::convertNumberToString(m_scale)).c_str(), 13.f);
		m_infoText->setTopLeft(10.f, 10.f);
	}

public:
	MandelbrotViewer()
	{
		create("Mandelbrot set", WIDTH, HEIGHT_PER_THREAD * NUMBER_OF_DRAWING_THREADS, AvoGUI::WindowStyleFlags::DefaultNoResize);
		waitForFinish();
	}
	~MandelbrotViewer()
	{
		m_isRunning = false;
		for (MandelbrotRenderer* renderer : m_renderers)
		{
			delete renderer;
		}
	}

	uint8* getPixels()
	{
		return m_pixels;
	}
	void renderImage()
	{
		for (uint32 a = 0; a < m_renderers.size(); a++)
		{
			m_renderers[a]->startRender();
		}
	}

	bool getIsRunning()
	{
		return m_isRunning;
	}

	double getScale()
	{
		return m_scale;
	}
	double getOffsetX()
	{
		return m_offsetX;
	}
	double getOffsetY()
	{
		return m_offsetY;
	}
	uint32 getMaxNumberOfIterations()
	{
		return m_maxNumberOfIterations;
	}

	void createContent() override
	{
		enableMouseEvents();

		m_renderers.resize(NUMBER_OF_DRAWING_THREADS);
		for (uint32 a = 0; a < NUMBER_OF_DRAWING_THREADS; a++)
		{
			m_renderers[a] = new MandelbrotRenderer(this, a);
		}

		updateInfoText();
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
		if (abs(m_newScale / m_scale - 1.) > 0.2)
		{
			m_offsetX -= p_event.x / getWidth() * (m_newScale - m_scale) * GLOBAL_SCALE_X;
			m_offsetY -= p_event.y / getHeight() * (m_newScale - m_scale) * GLOBAL_SCALE_Y;
			m_newOffsetX = m_offsetX;
			m_newOffsetY = m_offsetY;
			m_scale = m_newScale;
			m_maxNumberOfIterations = MAX_NUMBER_OF_ITERATIONS_START * AvoGUI::max(1., std::log10(1. / m_scale)* MAX_NUMBER_OF_ITERATIONS_SCALE_INCREASE + 1.);
			updateInfoText();
			renderImage();
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
			m_newOffsetX -= p_event.movementX / getWidth() * m_scale * GLOBAL_SCALE_X;
			m_newOffsetY -= p_event.movementY / getHeight() * m_scale * GLOBAL_SCALE_Y;
			if (AvoGUI::square(m_newOffsetX - m_offsetX) + AvoGUI::square(m_newOffsetY - m_offsetY) > AvoGUI::square(m_scale * 0.2))
			{
				m_offsetX = m_newOffsetX;
				m_offsetY = m_newOffsetY;
				renderImage();
			}
		}
	}

	void draw(AvoGUI::DrawingContext* p_context, AvoGUI::Rectangle<float> const& p_targetRectangle) override
	{
		for (uint32 a = 0; a < m_renderers.size(); a++)
		{
			m_renderers[a]->draw(p_context, p_targetRectangle);
		}

		if (m_infoText && m_infoText->getIsIntersecting(p_targetRectangle))
		{
			p_context->setColor(AvoGUI::Color(1.f));
			p_context->drawText(m_infoText);
		}
	}
};

