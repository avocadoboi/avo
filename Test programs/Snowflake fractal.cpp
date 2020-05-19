#include "../AvoGUI.hpp"

//------------------------------

class SnowflakeFractal : public Avo::Gui
{
private:
	static constexpr uint32 NUMBER_OF_ITERATIONS = 5;
	static constexpr double TRIANGLE_HEIGHT = 0.2886751346; // sqrt(1/12)

	Avo::Geometry m_fractalGeometry;

public:
	SnowflakeFractal()
	{
		create("Snowflake", 800, 600);

		setThemeColor(Avo::ThemeColors::background, Avo::Color(0.f));

		std::vector<Avo::Point<float>> points;
		points.push_back(Avo::Point<float>(0.f, 0.f));
		points.push_back(Avo::Point<float>(1.f, 0.f));
		points.push_back(Avo::Point<float>(0.5f, TRIANGLE_HEIGHT * 3.f));
		points.push_back(Avo::Point<float>(0.f, 0.f));

		for (uint32 a = 0; a < NUMBER_OF_ITERATIONS; a++)
		{
			for (uint32 b = 1; b < points.size(); b += 4)
			{
				Avo::Point<float> point_0 = points[b - 1];
				Avo::Point<float> point_1 = points[b];
				points.insert(points.begin() + b, Avo::interpolate(point_0, point_1, 1.f / 3.f));
				points.insert(points.begin() + (b + 1), 0.5f * (point_0 + point_1) + (point_1 - point_0).rotate(-Avo::PI * 0.5f) * TRIANGLE_HEIGHT);
				points.insert(points.begin() + (b + 2), Avo::interpolate(point_0, point_1, 2.f / 3.f));
			}
		}

		m_fractalGeometry = getDrawingContext()->createPolygonGeometry(points);

		run();
	}

	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(Avo::Color(0.8f, 0.9f, 1.f));
		p_context->setOrigin(getCenterX()*0.5f, 0.5f*(getHeight() - getWidth()*TRIANGLE_HEIGHT));
		p_context->setScale(getWidth()*0.5f);
		p_context->strokeGeometry(m_fractalGeometry, 0.002f);
	}
};

//------------------------------

int main()
{
	new SnowflakeFractal();
}
