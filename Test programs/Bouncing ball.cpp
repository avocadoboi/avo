#include "../AvoGUI.hpp"
#include <chrono>

//------------------------------

constexpr auto ballRadius = 60.;
constexpr auto ballRestitution = 0.8;
constexpr auto gravitationalAcceleration = 9.8;
constexpr auto pixelsPerMeter = 400.;
constexpr auto wallColor = Avo::Color{0.2f};

//------------------------------

class Ball
{
public:
	Avo::Color color = Avo::Color::hsba(Avo::random(), 1.f, 1.f);
	double radius = 0.;

	Avo::Point<double> position;
	Avo::Vector2d<double> velocity;
	double acceleration = 0.; // There's only acceleration on the y-axis.

	Avo::Vector2d<double> draggingVelocity;
	bool isDragged = false;

	auto isPointInside(Avo::Point<double> p_point) -> bool
	{
		return position.getDistanceSquared(p_point) < radius*radius;
	}

	auto draw(Avo::DrawingContext* p_context) -> void
	{
		p_context->setColor(color);
		p_context->fillCircle(position, radius);
	}
};

//------------------------------

namespace chrono = std::chrono;
using _Clock = chrono::steady_clock;

class Application : public Avo::Gui
{
private:
	Ball m_ball;
	Avo::Rectangle<double> m_wallLeft;
	Avo::Rectangle<double> m_wallRight;
	Avo::Rectangle<double> m_wallBottom;

	_Clock::time_point m_lastTimeMeasurementPoint;
	Avo::Count m_frameCount = 0;

public:
	Application()
	{
		create("Bouncing ball!", {800, 700});

		setThemeColor(Avo::ThemeColors::background, {1.f, 1.f, 1.f});

		m_ball.radius = ballRadius;
		m_ball.position = getCenter();
		m_ball.acceleration = gravitationalAcceleration * pixelsPerMeter / 3600.0;
		getWindow()->setMinSize({static_cast<float>(m_ball.radius * 2 + 101.), 0.f});
		enableMouseEvents();

		m_lastTimeMeasurementPoint = chrono::steady_clock::now();
		queueAnimationUpdate();

		run();
	}

	//------------------------------

	void handleMouseMove(Avo::MouseEvent const& p_event) override
	{
		if (m_ball.isDragged)
		{
			m_ball.draggingVelocity += 0.6*(p_event.movement - m_ball.draggingVelocity);
			m_ball.velocity += p_event.movement;
		}
	}
	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		if (m_ball.isPointInside(p_event.xy))
		{
			m_ball.isDragged = true;
		}
	}
	void handleMouseUp(Avo::MouseEvent const& p_event) override
	{
		m_ball.isDragged = false;
		m_ball.velocity = m_ball.draggingVelocity;
	}

	//------------------------------

	void handleSizeChange() override
	{
		m_wallLeft = {0., 0., 50., getHeight()};
		m_wallRight = {getWidth() - 50., 0., getWidth(), getHeight()};
		m_wallBottom = {0., getHeight() - 50., getWidth(), getHeight()};
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
			m_ball.position += Avo::Vector2d{m_ball.velocity.x, m_ball.velocity.y + m_ball.acceleration*0.5};
			m_ball.velocity.y += m_ball.acceleration;

			while (true)
			{
				if (m_ball.position.y + m_ball.radius > m_wallBottom.top)
				{
					auto const ballBottom = m_ball.position.y + m_ball.radius;
					if (auto const collisionTime = -(std::sqrt(Avo::square(m_ball.velocity.y) - 
					    2.*m_ball.acceleration*(ballBottom - m_wallBottom.top)) - 
						m_ball.velocity.y)/m_ball.acceleration; !isnan(collisionTime))
					{
						m_ball.velocity.y -= m_ball.acceleration*collisionTime;
						m_ball.velocity.y *= -ballRestitution;
						m_ball.velocity.y += m_ball.acceleration*collisionTime;
						m_ball.position.y = m_wallBottom.top - m_ball.radius + m_ball.acceleration*Avo::square(collisionTime)*0.5 + 
							m_ball.velocity.y*collisionTime;
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
					auto const collisionTime = -(m_wallLeft.right + m_ball.radius - m_ball.position.x) / m_ball.velocity.x;
					m_ball.velocity.x *= -ballRestitution;
					m_ball.position.x = m_wallLeft.right + m_ball.radius + m_ball.velocity.x*collisionTime;
				}
				else if (m_ball.position.x + m_ball.radius > m_wallRight.left)
				{
					auto const collisionTime = -(m_wallRight.left - m_ball.radius - m_ball.position.x) / m_ball.velocity.x;
					m_ball.velocity.x *= -ballRestitution;
					m_ball.position.x = m_wallRight.left - m_ball.radius + collisionTime * m_ball.velocity.x;
				}
				else break;
			}
		}

		queueAnimationUpdate();
		invalidate();

		if (++m_frameCount == 5*60)
		{
			auto const now = _Clock::now();
			auto const duration = chrono::duration<float>{now - m_lastTimeMeasurementPoint};
			Console::println("FPS: ", m_frameCount/duration.count());
			m_lastTimeMeasurementPoint = now;
			m_frameCount = 0u;
		}
	}

	auto draw(Avo::DrawingContext* p_context) -> void override
	{
		p_context->setColor(wallColor);
		p_context->fillRectangle(m_wallLeft);
		p_context->fillRectangle(m_wallRight);
		p_context->fillRectangle(m_wallBottom);
		m_ball.draw(p_context);
	}
};

//------------------------------

int main()
{
	new Application;
}
