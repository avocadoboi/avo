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

	bool m_needsRendering = false;
	std::condition_variable m_needsRenderingConditionVariable;
	std::mutex m_needsRenderingMutex;
	std::thread m_renderingThread;

	void render();

public:
	void startRender()
	{
		m_needsRenderingMutex.lock();
		m_needsRendering = true;
		m_needsRenderingMutex.unlock();
		m_needsRenderingConditionVariable.notify_all();
	}

private:
	Avo::Image m_image;
	std::mutex m_imageMutex;
public:
	void draw(Avo::DrawingContext* p_context, Avo::Rectangle<float> const& p_targetRectangle)
	{
		m_imageMutex.lock();
		if (m_image && m_image.getIsIntersecting(p_targetRectangle))
		{
			p_context->drawImage(m_image);
		}
		m_imageMutex.unlock();
	}

	MandelbrotRenderer(MandelbrotViewer* p_app, uint32 p_partIndex) :
		m_viewer{ p_app }, 
		m_partIndex{ p_partIndex }
	{
		m_renderingThread = std::thread{ &MandelbrotRenderer::render, this };
	}
	~MandelbrotRenderer()
	{
		startRender();
		m_renderingThread.join();
	}
};

//------------------------------

class MandelbrotViewer : public Avo::Gui
{
private:
	Avo::Text m_infoText;

	void updateInfoText()
	{
		m_infoText = getDrawingContext()->createText("Max iterations: " + Avo::convertNumberToString(m_maxNumberOfIterations) + " Scale: " + Avo::convertNumberToString(m_scale), 13.f);
		m_infoText.setTopLeft(10.f, 10.f);
	}

	uint8 m_pixels[WIDTH * HEIGHT_PER_THREAD * NUMBER_OF_DRAWING_THREADS * 4];
public:
	uint8* getPixels()
	{
		return m_pixels;
	}

private:
	std::vector<std::unique_ptr<MandelbrotRenderer>> m_renderers;
public:
	void renderImage()
	{
		for (uint32 a = 0; a < m_renderers.size(); a++)
		{
			m_renderers[a]->startRender();
		}
	}

private:
	bool m_isRunning = true;
public:
	bool getIsRunning()
	{
		return m_isRunning;
	}

	double m_scale = 1.;

private:
public:
	double getScale()
	{
		return m_scale;
	}
private:
	double m_offsetX = -2.2;
public:
	double getOffsetX()
	{
		return m_offsetX;
	}
private:
	double m_offsetY = -1.25;
public:
	double getOffsetY()
	{
		return m_offsetY;
	}

private:
	uint32 m_maxNumberOfIterations = MAX_NUMBER_OF_ITERATIONS_START;
public:
	uint32 getMaxNumberOfIterations()
	{
		return m_maxNumberOfIterations;
	}

private:
	double m_newScale = 1.;
public:
	void handleMouseScroll(Avo::MouseEvent const& p_event) override
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
			m_maxNumberOfIterations = MAX_NUMBER_OF_ITERATIONS_START * Avo::max(1., std::log10(1. / m_scale)* MAX_NUMBER_OF_ITERATIONS_SCALE_INCREASE + 1.);
			updateInfoText();
			renderImage();
		}
	}

private:
	bool m_isDragging = false;
public:
	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		m_isDragging = true;
	}
	void handleMouseUp(Avo::MouseEvent const& p_event) override
	{
		m_isDragging = false;
	}

private:
	double m_newOffsetX = -2.2;
	double m_newOffsetY = -1.25;
public:
	void handleMouseMove(Avo::MouseEvent const& p_event) override
	{
		if (m_isDragging)
		{
			m_newOffsetX -= p_event.movementX / getWidth() * m_scale * GLOBAL_SCALE_X;
			m_newOffsetY -= p_event.movementY / getHeight() * m_scale * GLOBAL_SCALE_Y;
			if (Avo::square(m_newOffsetX - m_offsetX) + Avo::square(m_newOffsetY - m_offsetY) > Avo::square(m_scale * 0.2))
			{
				m_offsetX = m_newOffsetX;
				m_offsetY = m_newOffsetY;
				renderImage();
			}
		}
	}

	void draw(Avo::DrawingContext* p_context, Avo::Rectangle<float> p_targetRectangle) override
	{
		for (uint32 a = 0; a < m_renderers.size(); a++)
		{
			m_renderers[a]->draw(p_context, p_targetRectangle);
		}

		if (m_infoText && m_infoText.getIsIntersecting(p_targetRectangle))
		{
			p_context->setColor(1.f);
			p_context->drawText(m_infoText);
		}
	}

	MandelbrotViewer()
	{
		create("Mandelbrot set", WIDTH, HEIGHT_PER_THREAD * NUMBER_OF_DRAWING_THREADS, Avo::WindowStyleFlags::DefaultNoResize);

		enableMouseEvents();

		m_renderers.resize(NUMBER_OF_DRAWING_THREADS);
		for (uint32 a = 0; a < NUMBER_OF_DRAWING_THREADS; a++)
		{
			m_renderers[a] = std::make_unique<MandelbrotRenderer>(this, a);
		}

		updateInfoText();

		run();
	}
	~MandelbrotViewer()
	{
		m_isRunning = false;
	}
};

