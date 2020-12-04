#include "../AvoGUI.hpp"

//------------------------------

class SnowflakeFractal : public Avo::Gui
{
public:
	static constexpr auto numberOfIterations = Avo::Count{5};
	static constexpr auto triangleHeight = 0.2886751346f; // sqrt(1/12)

private:
	Avo::Geometry m_fractalGeometry;
public:
	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor({0.8f, 0.9f, 1.f});
		p_context->setOrigin({getCenterX()*0.5f, 0.5f*(getHeight() - getWidth()*triangleHeight)});
		p_context->setScale(getWidth()*0.5f);
		p_context->strokeGeometry(m_fractalGeometry, 0.002f);
	}

	SnowflakeFractal()
	{
		create("Snowflake", {800, 600});

		setThemeColor(Avo::ThemeColors::background, 0.f);

		auto points = std::vector<Avo::Point<>>{
			{0.f, 0.f},
			{1.f, 0.f},
			{0.5f, triangleHeight * 3.f},
			{0.f, 0.f}
		};

		for (auto a : Avo::Range{numberOfIterations})
		{
			for (Avo::Index b = 1; b < points.size(); b += 4)
			{
				auto point_0 = points[b - 1];
				auto point_1 = points[b];
				points.insert(points.begin() + b, {
					Avo::interpolate(point_0, point_1, 1.f / 3.f),
					0.5f * (point_0 + point_1) + (point_1 - point_0).rotate(Avo::Radians{-Avo::PI<float> * 0.5f}) * triangleHeight,
					Avo::interpolate(point_0, point_1, 2.f / 3.f)
				});
			}
		}

		m_fractalGeometry = getDrawingContext()->createPolygonGeometry(points);

		run();
	}
};

//------------------------------

int main()
{
	new SnowflakeFractal;
}
