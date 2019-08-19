#include "../AvoGUI.hpp"
#include <chrono>

//------------------------------

double const BALL_RADIUS = 60.0;
double const BALL_RESTITUTION = 0.8;
double const GRAVITATIONAL_ACCELERATION = 9.8;
double const PIXELS_PER_METER = 400.0;
AvoGUI::Color const WALL_COLOR(0.2f);

//------------------------------

class Ball
{
public:
	AvoGUI::Color color;
	double radius;

	AvoGUI::Point<double> position;
	AvoGUI::Point<double> velocity;
	double acceleration; // There's only acceleration on the y-axis.

	AvoGUI::Point<double> draggingVelocity;
	bool isDragged;

	Ball()
	{
		color.setHSBA(AvoGUI::random(), 1.f, 1.f);
	}

	bool isPointInside(double p_x, double p_y)
	{
		return position.getDistanceSquared(p_x, p_y) < radius*radius;
	}

	void draw(AvoGUI::DrawingContext* p_context)
	{
		p_context->setColor(color);
		p_context->fillCircle(position, radius);
	}
};

//------------------------------

class Application : public AvoGUI::GUI
{
private:
	Ball m_ball;
	AvoGUI::Rectangle<double> m_wallLeft;
	AvoGUI::Rectangle<double> m_wallRight;
	AvoGUI::Rectangle<double> m_wallBottom;

	std::chrono::time_point<std::chrono::steady_clock> m_lastTimeMeasurementPoint;
	float m_frameCount;

public:
	Application()
	{
		create("Bouncing ball!", 800, 700);
	}

	//------------------------------

	void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	{
		if (m_ball.isDragged)
		{
			m_ball.draggingVelocity.x += 0.6*(p_event.movementX - m_ball.draggingVelocity.x);
			m_ball.draggingVelocity.y += 0.6*(p_event.movementY - m_ball.draggingVelocity.y);
			m_ball.velocity.move(p_event.movementX, p_event.movementY);
		}
	}
	void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
	{
		if (m_ball.isPointInside(p_event.x, p_event.y))
		{
			m_ball.isDragged = true;
		}
	}
	void handleMouseUp(const AvoGUI::MouseEvent& p_event) override
	{
		m_ball.isDragged = false;
		m_ball.velocity = m_ball.draggingVelocity;
	}

	//------------------------------

	void createContent()
	{
		m_theme->colors["background"] = AvoGUI::Color(1.f, 1.f, 1.f);

		m_ball.radius = BALL_RADIUS;
		m_ball.position = getCenter();
		m_ball.acceleration = GRAVITATIONAL_ACCELERATION * PIXELS_PER_METER / 3600.0;
		getWindow()->setMinSize(m_ball.radius * 2 + 101.0, 0);
		enableMouseEvents();

		m_lastTimeMeasurementPoint = std::chrono::steady_clock::now();
		queueAnimationUpdate();
	}

	void handleSizeChange() override
	{
		m_wallLeft.set(0.0, 0.0, 50.0, getHeight());
		m_wallRight.set(getWidth() - 50.0, 0.0, getWidth(), getHeight());
		m_wallBottom.set(0.0, getHeight() - 50.0, getWidth(), getHeight());
	}

	//------------------------------

	void updateAnimations() override
	{
		if (m_ball.isDragged)
		{
			m_ball.position += m_ball.velocity;
			m_ball.velocity *= 0.0;
		}
		else
		{
			m_ball.position.move(m_ball.velocity.x, m_ball.velocity.y + m_ball.acceleration*0.5);
			m_ball.velocity.y += m_ball.acceleration;

			while (true)
			{
				if (m_ball.position.y + m_ball.radius > m_wallBottom.top)
				{
					double collisionTime = -(sqrt(m_ball.velocity.y*m_ball.velocity.y - 2.0*m_ball.acceleration*(m_ball.position.y + m_ball.radius - m_wallBottom.top)) - m_ball.velocity.y) / m_ball.acceleration;
					if (!isnan(collisionTime))
					{
						m_ball.velocity.y -= m_ball.acceleration * collisionTime;
						m_ball.velocity.y *= -BALL_RESTITUTION;
						m_ball.velocity.y += m_ball.acceleration * collisionTime;
						m_ball.position.y = m_wallBottom.top - m_ball.radius + m_ball.acceleration*collisionTime*collisionTime*0.5 + m_ball.velocity.y*collisionTime;
					}
					if (m_ball.position.y + m_ball.radius > m_wallBottom.top)
					{
						// This means the bounce was shorter than a timestep, very unnecessary to calculate every single bounce within in a timestep...
						// The number of bounces within a timestep would in theory go towards infinity as they would get infinitely small.
						m_ball.position.y = m_wallBottom.top - m_ball.radius;
					}
				}
				else if (m_ball.position.x - m_ball.radius < m_wallLeft.right)
				{
					double collisionTime = -(m_wallLeft.right + m_ball.radius - m_ball.position.x) / m_ball.velocity.x;
					m_ball.velocity.x *= -BALL_RESTITUTION;
					m_ball.position.x = m_wallLeft.right + m_ball.radius + m_ball.velocity.x*collisionTime;
				}
				else if (m_ball.position.x + m_ball.radius > m_wallRight.left)
				{
					double collisionTime = -(m_wallRight.left - m_ball.radius - m_ball.position.x) / m_ball.velocity.x;
					m_ball.velocity.x *= -BALL_RESTITUTION;
					m_ball.position.x = m_wallRight.left - m_ball.radius + collisionTime * m_ball.velocity.x;
				}
				else break;
			}
		}

		queueAnimationUpdate();
		invalidate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		m_frameCount++;
		if (m_frameCount == 5.f*60.f)
		{
			std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastTimeMeasurementPoint);
			std::cout << "FPS: " << (m_frameCount * 1000.f / (float)duration.count()) << std::endl;
			m_lastTimeMeasurementPoint = now;
			m_frameCount = 0;
		}

		p_context->setColor(WALL_COLOR);
		p_context->fillRectangle(m_wallLeft);
		p_context->fillRectangle(m_wallRight);
		p_context->fillRectangle(m_wallBottom);
		m_ball.draw(p_context);
	}
};

//------------------------------

int main()
{
	Application* application = new Application();
	AvoGUI::GUI::run();
	application->forget();
}
