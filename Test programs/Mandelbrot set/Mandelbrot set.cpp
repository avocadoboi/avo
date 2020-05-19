#include "Mandelbrot set.hpp"

//------------------------------

void MandelbrotRenderer::render()
{
	while (m_viewer->getIsRunning())
	{
		m_needsRendering = false;
		for (uint32 x = 0; x < WIDTH; x++)
		{
			for (uint32 y = m_partIndex * HEIGHT_PER_THREAD; y < m_partIndex * HEIGHT_PER_THREAD + HEIGHT_PER_THREAD; y++)
			{
				if (!m_viewer->getPixels() || !m_viewer->getIsRunning())
				{
					return;
				}
				uint8* pixel = m_viewer->getPixels() + 4 * (x + (uint32)WIDTH * y);

				double translatedX = x / m_viewer->getWidth() * GLOBAL_SCALE_X * m_viewer->getScale() + m_viewer->getOffsetX();
				double translatedY = y / m_viewer->getHeight() * GLOBAL_SCALE_Y * m_viewer->getScale() + m_viewer->getOffsetY();

				double real = 0.;
				double imaginary = 0.;

				uint32 iteration = 0;
				while (iteration < m_viewer->getMaxNumberOfIterations() && real * real + imaginary * imaginary <= 4.)
				{
					// z^2 + c = (a + ib)^2 + c = (a^2 - b^2 + 2iab) + (x + iy)
					double realBefore = real;
					real = translatedX + real * real - imaginary * imaginary;
					imaginary = translatedY + 2. * realBefore * imaginary;
					iteration++;
				}

				Avo::Color color;
				color.setHSB(iteration / (float)m_viewer->getMaxNumberOfIterations() * 1.5f, 1.f, iteration == m_viewer->getMaxNumberOfIterations() ? 0.f : 1.f);//(1.f - Avo::square(1.f - 2.f * iteration / (double)MAX_NUMBER_OF_ITERATIONS)));

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

		m_imageMutex.lock();
		m_image = m_viewer->getDrawingContext()->createImage(m_viewer->getPixels() + m_partIndex * HEIGHT_PER_THREAD * WIDTH * 4, m_viewer->getWidth(), HEIGHT_PER_THREAD);
		m_image.setTop(m_partIndex * HEIGHT_PER_THREAD);
		m_imageMutex.unlock();

		m_viewer->invalidateRectangle(m_image.getBounds());

		//invalidate();

		if (!m_viewer->getIsRunning())
		{
			return;
		}

		if (!m_needsRendering)
		{
			std::unique_lock<std::mutex> lock{ m_needsRenderingMutex };
			m_needsRenderingConditionVariable.wait(lock, [this]() { return m_needsRendering; });
		}
	}

}

//------------------------------

int main()
{
	new MandelbrotViewer;
}
