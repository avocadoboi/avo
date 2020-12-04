#include "../AvoGUI.hpp"

//------------------------------

float const boidScale = 15.f;
float const boidViewRadius = 50.f;
float const boidSpeed = 2.f;
float const boidCohesionSpeed = 3.3f;
float const boidAlignmentSpeed = 2.4f;
float const boidAvoidanceSpeed = 1.f;

float const edgeRepeatPadding = 500.f;

//------------------------------

struct Boid
{
	Avo::Point<> position;
	Avo::Point<> velocity = Avo::Point<>::fromPolar(Avo::Degrees{Avo::random()*360}, boidSpeed);
	Avo::Point<> acceleration;
	float hue = Avo::random()*2.f - 1.f;
};

//------------------------------

class Boids : public Avo::Gui
{
private:
	std::vector<Boid> m_boids;
	auto addBoid(Avo::Point<> p_position) -> void
	{
		m_boids.emplace_back();
		m_boids.back().position = p_position;
	}
	
public:
	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		if (p_event.mouseButton == Avo::MouseButton::Left)
		{
			addBoid(p_event.xy);
		}
	}
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		if (getWindow()->getIsMouseButtonDown(Avo::MouseButton::Left) && 
		    p_event.movement.getLengthSquared() > Avo::square(5))
		{
			addBoid(p_event.xy);
		}
	}

	//------------------------------

	auto updateAnimations() -> void override
	{
		auto const mousePosition = getWindow()->getMousePosition();
		auto const willAvoidMouse = getWindow()->getIsMouseButtonDown(Avo::MouseButton::Right);
		for (auto const a : Avo::Indices{m_boids})
		{
			auto averagePosition = Avo::Point<>{};
			auto averageVelocity = Avo::Vector2d<>{};
			auto numberOfBoidsWithinRadius = Avo::Count{};
			for (auto const b : Avo::Indices{m_boids})
			{
				if (b != a)
				{
					auto const delta = m_boids[b].position - m_boids[a].position;
					if (auto const lengthSquared = delta.getLengthSquared();
					    lengthSquared < Avo::square(boidViewRadius))
					{
						averagePosition += m_boids[b].position;
						averageVelocity += m_boids[b].velocity;
						++numberOfBoidsWithinRadius;

						m_boids[a].acceleration -= boidAvoidanceSpeed*0.05f*delta / Avo::max(lengthSquared, 0.01f);
						m_boids[a].hue += 0.0001f*(m_boids[b].hue - m_boids[a].hue);
					}
				}
			}

			if (willAvoidMouse)
			{
				auto const delta = mousePosition - m_boids[a].position;
				auto const lengthSquared = Avo::max(delta.getLengthSquared(), 10.f);
				m_boids[a].acceleration -= 10.f*delta/lengthSquared;

				m_boids[a].hue += 50.f/lengthSquared;
			}

			if (numberOfBoidsWithinRadius)
			{
				averagePosition /= numberOfBoidsWithinRadius;
				averageVelocity /= numberOfBoidsWithinRadius;
				m_boids[a].position += boidCohesionSpeed*0.001f*(averagePosition - m_boids[a].position);
				m_boids[a].acceleration += boidAlignmentSpeed*0.005f*(averageVelocity.normalizeFast()*boidSpeed - m_boids[a].velocity);
			}

			m_boids[a].velocity += m_boids[a].acceleration;
			m_boids[a].position += m_boids[a].velocity;
			m_boids[a].acceleration = 0.f;

			if (m_boids[a].position.x < -edgeRepeatPadding)
			{
				m_boids[a].position.x += getWidth() + edgeRepeatPadding;
			}
			if (m_boids[a].position.y < -edgeRepeatPadding)
			{
				m_boids[a].position.y += getHeight() + edgeRepeatPadding;
			}
			if (m_boids[a].position.x >= getWidth() + edgeRepeatPadding)
			{
				m_boids[a].position.x -= getWidth() + edgeRepeatPadding;
			}
			if (m_boids[a].position.y >= getHeight() + edgeRepeatPadding)
			{
				m_boids[a].position.y -= getHeight() + edgeRepeatPadding;
			}
		}

		invalidate();
		queueAnimationUpdate();
	}

private:
	Avo::Geometry m_boidGeometry;
public:
	auto draw(Avo::DrawingContext* p_context) -> void override
	{
		for (auto& boid : m_boids)
		{
			p_context->setColor(Avo::Color::hsb(boid.hue, 0.7f, 0.8f));
			p_context->scale(boidScale);
			p_context->rotate(Avo::HALF_PI<Avo::Radians> + boid.velocity.getAngle<Avo::Radians>());
			p_context->setOrigin(boid.position);
			p_context->fillGeometry(m_boidGeometry);
			p_context->resetTransformations();
		}
	}

	Boids()
	{
		create("Boids", {600, 400});

		setThemeColor(Avo::ThemeColors::background, 0.05f);

		//------------------------------

		auto const triangle = std::array{
			Avo::Point{0.f, -0.5f},
			Avo::Point{0.4f, 0.5f},
			Avo::Point{0.f, 0.4f},
			Avo::Point{-0.4f, 0.5f}
		};
		m_boidGeometry = getDrawingContext()->createPolygonGeometry(triangle);

		addBoid(Avo::Point{Avo::random()*getSize()});

		//------------------------------

		enableMouseEvents();
		queueAnimationUpdate();

		run();
	}
};

//------------------------------

auto main() -> int
{
	new Boids{};
}
