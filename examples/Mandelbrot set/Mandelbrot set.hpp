#pragma once
#include "../../AvoGUI.hpp"

//------------------------------

inline constexpr auto maxNumberOfIterationsStart = 100;
inline constexpr auto maxNumberOfIterationsScaleIncrease = 30.;
inline constexpr auto globalScale = Avo::Vector2d{3., 2.5};

inline constexpr auto width = 700ll;
inline constexpr auto heightPerThread = 60ll;
inline constexpr auto numberOfDrawingThreads = 10;

//------------------------------

using Unit = long double;

//------------------------------

class MandelbrotViewer;

class MandelbrotRenderer
{
private:
	MandelbrotViewer* m_viewer{};
	Avo::Index m_partIndex{};

	bool m_needsRendering{};
	std::condition_variable m_needsRenderingConditionVariable;
	std::mutex m_needsRenderingMutex;
	std::thread m_renderingThread;

	auto render() -> void;

public:
	auto startRender() -> void
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
	auto draw(Avo::DrawingContext* const p_context, Avo::Rectangle<> p_targetRectangle) -> void
	{
		m_imageMutex.lock();
		if (m_image && m_image.getIsIntersecting(p_targetRectangle))
		{
			p_context->drawImage(m_image);
		}
		m_imageMutex.unlock();
	}

	MandelbrotRenderer(MandelbrotViewer* const p_app, Avo::Index const p_partIndex) :
		m_viewer{p_app}, 
		m_partIndex{p_partIndex}
	{
		m_renderingThread = std::thread{&MandelbrotRenderer::render, this};
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

	auto updateInfoText() -> void
	{
		m_infoText = getDrawingContext()->createText(
			"Max iterations: " + Avo::numberToString(m_maxNumberOfIterations) + 
			" Scale: " + Avo::numberToString(m_scale), 
			13.f
		);
		m_infoText.setTopLeft(10.f);
	}

	std::array<std::byte, width * heightPerThread * numberOfDrawingThreads * 4> m_pixels;
public:
	auto getPixels() -> auto&
	{
		return m_pixels;
	}

private:
	std::array<std::shared_ptr<MandelbrotRenderer>, numberOfDrawingThreads> m_renderers;
public:
	auto renderImage() -> void
	{
		for (auto const& renderer : m_renderers)
		{
			renderer->startRender();
		}
	}

private:
	bool m_isRunning = true;
public:
	auto getIsRunning() const -> bool 
	{
		return m_isRunning;
	}

private:
	Unit m_scale = 1.;
public:
	auto getScale() const -> Unit
	{
		return m_scale;
	}

private:
	Avo::Vector2d<Unit> m_offset{-2.2, -1.25};
public:
	auto getOffset() const -> Avo::Vector2d<Unit>
	{
		return m_offset;
	}

private:
	Avo::Count m_maxNumberOfIterations = maxNumberOfIterationsStart;
public:
	auto getMaxNumberOfIterations() -> Avo::Count
	{
		return m_maxNumberOfIterations;
	}

private:
	Unit m_newScale = 1.l;
public:
	auto handleMouseScroll(Avo::MouseEvent const& p_event) -> void override
	{
		if (p_event.scrollDelta > 0.)
		{
			m_newScale /= 1. + p_event.scrollDelta * 0.1;
		}
		else
		{
			m_newScale *= 1. - p_event.scrollDelta * 0.1;
		}
		if (std::abs(m_newScale / m_scale - 1.) > 0.2)
		{
			m_offset -= p_event.xy / getSize() * (m_newScale - m_scale) * globalScale;
			m_newOffset = m_offset;
			m_scale = m_newScale;
			m_maxNumberOfIterations = maxNumberOfIterationsStart * Avo::max(1., std::log10(1. / m_scale)* maxNumberOfIterationsScaleIncrease + 1.);
			updateInfoText();
			renderImage();
		}
	}

private:
	bool m_isDragging = false;
public:
	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		m_isDragging = true;
	}
	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void override
	{
		m_isDragging = false;
	}

private:
	Avo::Vector2d<Unit> m_newOffset{-2.2, -1.25};
public:
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		if (m_isDragging)
		{
			m_newOffset -= p_event.movement / getSize() * m_scale * globalScale;
			if ((m_newOffset - m_offset).getLengthSquared() > Avo::square(m_scale * 0.2))
			{
				m_offset = m_newOffset;
				renderImage();
			}
		}
	}

	auto draw(Avo::DrawingContext* const p_context, Avo::Rectangle<> const p_targetRectangle) -> void override
	{
		for (auto const& renderer : m_renderers)
		{
			renderer->draw(p_context, p_targetRectangle);
		}

		if (m_infoText && m_infoText.getIsIntersecting(p_targetRectangle))
		{
			p_context->setColor(1.f);
			p_context->drawText(m_infoText);
		}
	}

	MandelbrotViewer()
	{
		create("Mandelbrot set", {width, heightPerThread * numberOfDrawingThreads}, Avo::WindowStyleFlags::DefaultNoResize);

		enableMouseEvents();

		for (auto a : Avo::Indices{m_renderers})
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

