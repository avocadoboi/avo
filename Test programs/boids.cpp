#include "../AvoGUI.hpp"

//------------------------------

float const BOID_SCALE = 15.f;
float const BOID_VIEW_RADIUS = 50.f;
float const BOID_SPEED = 2.f;
float const BOID_COHESION_SPEED = 3.3f;
float const BOID_ALIGNMENT_SPEED = 2.4f;
float const BOID_AVOIDANCE_SPEED = 1.f;

float const EDGE_REPEAT_PADDING = 500.f;
//float const MOUSE_OBSTACLE_RADIUS = 50.f;

//------------------------------

class Boid
{
public:
	AvoGUI::Point<float> position;
	AvoGUI::Point<float> velocity;
	AvoGUI::Point<float> acceleration;
	float hue;

	Boid() :
		hue(AvoGUI::random()*2.f - 1.f)
	{
		velocity.setPolar(AvoGUI::random() * AvoGUI::TAU, BOID_SPEED);
	}
};

//------------------------------

class Boids :
	AvoGUI::GUI
{
private:
	std::vector<Boid> m_boids;
	AvoGUI::Geometry* m_boidGeometry;

public:
	Boids()
	{
		create("Boids", 600, 400);
	}
	~Boids()
	{
		m_boidGeometry->forget();
	}

	//------------------------------

	void createContent() override
	{
		setThemeColor("background", AvoGUI::Color(0.05f));

		//------------------------------

		std::vector<AvoGUI::Point<float>> triangle =
		{
			AvoGUI::Point<float>(0.f, -0.5f),
			AvoGUI::Point<float>(0.4f, 0.5f),
			AvoGUI::Point<float>(0.f, 0.4f),
			AvoGUI::Point<float>(-0.4f, 0.5f)
		};
		m_boidGeometry = getDrawingContext()->createPolygonGeometry(triangle);

		m_boids.push_back(Boid());
		m_boids.back().position.set(AvoGUI::random() * getWidth(), AvoGUI::random() * getHeight());

		//------------------------------

		enableMouseEvents();
		queueAnimationUpdate();
	}

	//------------------------------

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		if (p_event.mouseButton == AvoGUI::MouseButton::Left)
		{
			m_boids.push_back(Boid());
			m_boids.back().position.set(p_event.x, p_event.y);
		}
	}
	void handleMouseMove(AvoGUI::MouseEvent const& p_event) override
	{
		if (getWindow()->getIsMouseButtonDown(AvoGUI::MouseButton::Left) && AvoGUI::Point<float>::getLengthSquared(p_event.movementX, p_event.movementY) > 5*5)
		{
			m_boids.push_back(Boid());
			m_boids.back().position.set(p_event.x, p_event.y);
		}
	}
	void handleMouseDoubleClick(AvoGUI::MouseEvent const& p_event) override
	{
		handleMouseDown(p_event);
	}

	//------------------------------

	void updateAnimations() override
	{
		AvoGUI::Point<float> mousePosition = getWindow()->getMousePosition();
		bool willAvoidMouse = getWindow()->getIsMouseButtonDown(AvoGUI::MouseButton::Right);
		for (uint32 a = 0; a < m_boids.size(); a++)
		{
			AvoGUI::Point<float> averagePosition;
			AvoGUI::Point<float> averageVelocity;
			uint32 numberOfBoidsWithinRadius = 0;
			for (uint32 b = 0; b < m_boids.size(); b++)
			{
				if (b != a)
				{
					AvoGUI::Point<float> delta = m_boids[b].position - m_boids[a].position;
					double lengthSquared = delta.getLengthSquared();
					if (lengthSquared < BOID_VIEW_RADIUS * BOID_VIEW_RADIUS)
					{
						averagePosition += m_boids[b].position;
						averageVelocity += m_boids[b].velocity;
						numberOfBoidsWithinRadius++;

						m_boids[a].acceleration -= BOID_AVOIDANCE_SPEED*0.05f*delta / AvoGUI::max(delta.getLengthSquared(), 0.01f);
						m_boids[a].hue += 0.0001f*(m_boids[b].hue - m_boids[a].hue);
					}
				}
			}

			if (willAvoidMouse)
			{
				AvoGUI::Point<float> delta = mousePosition - m_boids[a].position;
				float lengthSquared = AvoGUI::max(delta.getLengthSquared(), 10.f);
				m_boids[a].acceleration -= 10.f*delta / lengthSquared;

				m_boids[a].hue += 50.f/lengthSquared;

				//m_boids[a].acceleration -= delta.normalizeFast()*0.1f;
			}

			if (numberOfBoidsWithinRadius)
			{
				averagePosition /= numberOfBoidsWithinRadius;
				averageVelocity /= numberOfBoidsWithinRadius;
				m_boids[a].position += BOID_COHESION_SPEED * 0.001f * (averagePosition - m_boids[a].position);
				m_boids[a].acceleration += BOID_ALIGNMENT_SPEED * 0.005f*(averageVelocity.normalizeFast()* BOID_SPEED - m_boids[a].velocity);
			}

			//m_boids[a].acceleration += AvoGUI::Point<float>().setPolar(AvoGUI::random()*AvoGUI::TAU, BOID_SPEED*0.01f);

			m_boids[a].velocity += m_boids[a].acceleration;
			m_boids[a].position += m_boids[a].velocity;
			m_boids[a].acceleration = 0.f;

			if (m_boids[a].position.x < -EDGE_REPEAT_PADDING)
			{
				m_boids[a].position.x += getWidth() + EDGE_REPEAT_PADDING;
			}
			if (m_boids[a].position.y < -EDGE_REPEAT_PADDING)
			{
				m_boids[a].position.y += getHeight() + EDGE_REPEAT_PADDING;
			}
			if (m_boids[a].position.x >= getWidth() + EDGE_REPEAT_PADDING)
			{
				m_boids[a].position.x -= getWidth() + EDGE_REPEAT_PADDING;
			}
			if (m_boids[a].position.y >= getHeight() + EDGE_REPEAT_PADDING)
			{
				m_boids[a].position.y -= getHeight() + EDGE_REPEAT_PADDING;
			}
		}

		invalidate();
		queueAnimationUpdate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		for (Boid const& boid : m_boids)
		{
			p_context->setColor(AvoGUI::Color().setHSB(boid.hue, 0.7f, 0.8f));
			p_context->scale(BOID_SCALE);
			p_context->rotate(AvoGUI::HALF_PI + boid.velocity.getAngle());
			p_context->setOrigin(boid.position);
			p_context->fillGeometry(m_boidGeometry);
			p_context->resetTransformations();
		}

		//if (getWindow()->getIsMouseButtonDown(AvoGUI::MouseButton::Right))
		//{
		//	p_context->setColor(AvoGUI::Color(1.f, 1.f, 1.f, 0.05f));
		//	p_context->fillCircle(getWindow()->getMousePosition(), MOUSE_OBSTACLE_RADIUS);
		//}
	}
};

//------------------------------

int main()
{
	new Boids();
	AvoGUI::GUI::run();
}
