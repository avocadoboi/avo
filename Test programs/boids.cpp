#include "../AvoGUI.hpp"

//------------------------------

float const BOID_SCALE = 15.f;
float const BOID_VIEW_RADIUS = 40.f;
float const BOID_SPEED = 3.f;
float const BOID_POSITIONING_SPEED = 2.f;
float const BOID_ALIGNMENT_SPEED = 3.f;
float const BOID_AVOIDANCE_SPEED = 1.5f;

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
		m_boids.push_back(Boid());
		m_boids.back().position.set(p_event.x, p_event.y);
	}
	void handleMouseDoubleClick(AvoGUI::MouseEvent const& p_event) override
	{
		handleMouseDown(p_event);
	}

	//------------------------------

	void updateAnimations() override
	{
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

						m_boids[a].acceleration -= BOID_AVOIDANCE_SPEED*0.05f*delta / delta.getLengthSquared();
						m_boids[a].hue += 0.001f*(m_boids[b].hue - m_boids[a].hue);
					}
				}
			}
			if (numberOfBoidsWithinRadius)
			{
				averagePosition /= numberOfBoidsWithinRadius;
				averageVelocity /= numberOfBoidsWithinRadius;
				m_boids[a].position += BOID_POSITIONING_SPEED * 0.001f * (averagePosition - m_boids[a].position);
				m_boids[a].acceleration += BOID_ALIGNMENT_SPEED * 0.005f*(averageVelocity.normalizeFast()* BOID_SPEED - m_boids[a].velocity);
			}
			//m_boids[a].acceleration += AvoGUI::Point<float>().setPolar(AvoGUI::random()*AvoGUI::TAU, BOID_SPEED*0.01f);

			m_boids[a].velocity += m_boids[a].acceleration;
			m_boids[a].position += m_boids[a].velocity;
			m_boids[a].acceleration = 0.f;

			if (m_boids[a].position.x < -BOID_SCALE)
			{
				m_boids[a].position.x += getWidth() + BOID_SCALE;
			}
			if (m_boids[a].position.y < -BOID_SCALE)
			{
				m_boids[a].position.y += getHeight() + BOID_SCALE;
			}
			if (m_boids[a].position.x >= getWidth() + BOID_SCALE)
			{
				m_boids[a].position.x -= getWidth() + BOID_SCALE;
			}
			if (m_boids[a].position.y >= getHeight() + BOID_SCALE)
			{
				m_boids[a].position.y -= getHeight() + BOID_SCALE;
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
	}
};

//------------------------------

int main()
{
	new Boids();
	AvoGUI::GUI::run();
}
