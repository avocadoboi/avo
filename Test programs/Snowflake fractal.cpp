#include "../AvoGUI.hpp"

//------------------------------

class SnowflakeFractal :
	public AvoGUI::Gui
{
private:
	static constexpr uint32 NUMBER_OF_ITERATIONS = 5;
	static constexpr double TRIANGLE_HEIGHT = 0.2886751346; // sqrt(1/12)

	AvoGUI::Geometry* m_fractalGeometry{ nullptr };

public:
	SnowflakeFractal()
	{
		create("Snowflake", 800, 600);
		waitForFinish();
	}
	~SnowflakeFractal()
	{
		m_fractalGeometry->forget();
	}

	void createContent() override
	{
		setThemeColor(AvoGUI::ThemeColors::background, AvoGUI::Color(0.f));

		std::vector<AvoGUI::Point<float>> points;
		points.push_back(AvoGUI::Point<float>(0.f, 0.f));
		points.push_back(AvoGUI::Point<float>(1.f, 0.f));
		points.push_back(AvoGUI::Point<float>(0.5f, TRIANGLE_HEIGHT*3.f));
		points.push_back(AvoGUI::Point<float>(0.f, 0.f));

		for (uint32 a = 0; a < NUMBER_OF_ITERATIONS; a++)
		{
			for (uint32 b = 1; b < points.size(); b += 4)
			{
				AvoGUI::Point<float> point_0 = points[b - 1];
				AvoGUI::Point<float> point_1 = points[b];
				points.insert(points.begin() + b, AvoGUI::interpolate(point_0, point_1, 1.f / 3.f));
				points.insert(points.begin() + (b + 1), 0.5f * (point_0 + point_1) + (point_1 - point_0).rotate(-AvoGUI::PI*0.5f) * TRIANGLE_HEIGHT);
				points.insert(points.begin() + (b + 2), AvoGUI::interpolate(point_0, point_1, 2.f / 3.f));
			}
		}

		m_fractalGeometry = getDrawingContext()->createPolygonGeometry(points);
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(0.8f, 0.9f, 1.f));
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
