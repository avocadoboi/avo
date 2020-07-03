#include "Mandelbrot set.hpp"
#include <complex>

//------------------------------

void MandelbrotRenderer::render()
{
	auto const verticalRange = Avo::Range{m_partIndex * heightPerThread, (m_partIndex + 1) * heightPerThread};
	while (m_viewer->getIsRunning())
	{
		m_needsRendering = false;
		for (auto x : Avo::Range{width})
		{
			for (auto y : verticalRange)
			{
				if (!m_viewer->getIsRunning())
				{
					return;
				}
				auto* const pixel = m_viewer->getPixels().data() + static_cast<std::size_t>(4 * (x + width * y));

				auto const translated = 
					Avo::Point<Unit>{static_cast<Unit>(x), static_cast<Unit>(y)} / 
					m_viewer->getSize() * globalScale * m_viewer->getScale() + m_viewer->getOffset();
				auto const c = std::complex{translated.x, translated.y};

				auto z = std::complex{static_cast<Unit>(0), static_cast<Unit>(0)};

				auto iteration = Avo::Index{};
				for (auto const maxIterations = m_viewer->getMaxNumberOfIterations();
				     iteration < maxIterations && std::norm(z) <= static_cast<Unit>(4);
					 ++iteration)
				{
					// z^2 + c = (a + ib)^2 + c = (a^2 - b^2 + 2iab) + (x + iy)
					z = z*z + c;
					// auto const realBefore = real;
					// real = translated.x + real * real - imaginary * imaginary;
					// imaginary = translated.y + 2. * realBefore * imaginary;
				}

				auto const color = Avo::Color::hsb(
					static_cast<float>(iteration) / m_viewer->getMaxNumberOfIterations() * 1.5f, 
					1.f, 
					iteration == m_viewer->getMaxNumberOfIterations() ? 0.f : 1.f
				);

				// Red
				pixel[2] = static_cast<std::byte>(color.red * 255);
				// Green
				pixel[1] = static_cast<std::byte>(color.green * 255);
				// Blue
				pixel[0] = static_cast<std::byte>(color.blue * 255);
				// Alpha
				pixel[3] = static_cast<std::byte>(255);
			}
		}

		m_imageMutex.lock();
		m_image = m_viewer->getDrawingContext()->createImage(
			m_viewer->getPixels().data() + static_cast<std::size_t>(m_partIndex * heightPerThread * width * 4),
			{static_cast<Avo::Pixels>(m_viewer->getWidth()), heightPerThread}
		);
		m_image.setTop(m_partIndex * heightPerThread);
		m_imageMutex.unlock();

		m_viewer->invalidateRectangle(m_image.getBounds());

		//invalidate();

		if (!m_viewer->getIsRunning())
		{
			return;
		}

		if (!m_needsRendering)
		{
			auto lock = std::unique_lock<std::mutex>{m_needsRenderingMutex};
			m_needsRenderingConditionVariable.wait(lock, [this]() { return m_needsRendering; });
		}
	}

}

//------------------------------

int main()
{
	new MandelbrotViewer;
}
