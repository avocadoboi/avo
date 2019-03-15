#include "AvoGUI.hpp"

//------------------------------

long double const NUMBER_OF_DIGITS = 8.0L;
long double const START_VELOCITY = 0.4L;
float windowWidth = 800.f;
float windowHeight = 350.f;

//------------------------------

class Block
{
public:
	long double position;
	long double velocity;
	long double inverseMass;
	long double width;
	AvoGUI::Color color;

	Block(long double p_position, long double p_velocity, long double p_inverseMass, long double p_width) :
		position(p_position), velocity(p_velocity), inverseMass(p_inverseMass), width(p_width)
	{
		color.setHSBA(AvoGUI::random(), 0.95f, 0.8f);
	}

	void draw(AvoGUI::DrawingContext* p_context, long double p_min)
	{
		p_context->setColor(color);
		p_context->fillRectangle(AvoGUI::max(p_min, position), windowHeight - width, AvoGUI::max(p_min, position) + width, windowHeight);
	}
};

//------------------------------

class Canvas : public AvoGUI::View
{
private:
	Block m_firstBlock;
	Block m_secondBlock;
	uint32_t m_numberOfCollisions;

public:
	Canvas(View* p_parent) : 
		View(p_parent, p_parent->getBounds()),
		m_firstBlock(500.0L, 0.0L, 1.0L, 70.0L), 
		m_secondBlock(600.0L, -START_VELOCITY, pow(100.0L, -NUMBER_OF_DIGITS + 1.0L), 200.0L)
	{
		windowWidth = getWidth();
		windowHeight = getHeight();

		AvoGUI::TextProperties textProperties;
		textProperties.fontSize = 35.f;
		getGUI()->getDrawingContext()->setDefaultTextProperties(textProperties);
		queueAnimationUpdate();
	}

	void updateAnimations() override
	{
		m_firstBlock.position += m_firstBlock.velocity;
		m_secondBlock.position += m_secondBlock.velocity;

		// Trace back and resolve all collisions that have happened in the last timestep.
		while (true)
		{
			// We can only predict the earliest collision in the last timestep using the current velocity and position. It's an iterative process.
			if (m_firstBlock.position < 0.0L)
			{
				m_firstBlock.velocity *= -1.0L;
				m_firstBlock.position *= -1.0L; // The block has moved since the collision, in the opposite direction.
				m_numberOfCollisions++;
			}
			else if (m_secondBlock.position < m_firstBlock.position + m_firstBlock.width)
			{
				long double blockCollisionTime = m_firstBlock.velocity == m_secondBlock.velocity ? 0.0L : ((m_firstBlock.position + m_firstBlock.width - m_secondBlock.position) / (m_secondBlock.velocity - m_firstBlock.velocity));
				long double impulse = 2.0L*(m_secondBlock.velocity - m_firstBlock.velocity) / (m_firstBlock.inverseMass + m_secondBlock.inverseMass);
				m_firstBlock.position += m_firstBlock.velocity*blockCollisionTime; // Move the position back to where it collided
				m_firstBlock.velocity += m_firstBlock.inverseMass*impulse;
				m_firstBlock.position -= m_firstBlock.velocity*blockCollisionTime; // The block has moved since the collision

				m_secondBlock.position += m_secondBlock.velocity*blockCollisionTime;
				m_secondBlock.velocity -= m_secondBlock.inverseMass*impulse;
				m_secondBlock.position -= m_secondBlock.velocity*blockCollisionTime;
				m_numberOfCollisions++;
			}
			else break;
		}

		invalidate();
		queueAnimationUpdate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		m_firstBlock.draw(p_context, 0.0L);
		m_secondBlock.draw(p_context, m_firstBlock.width);
		p_context->setColor(0xff111111);
		p_context->drawText(std::to_string(m_numberOfCollisions).c_str(), 20.f, 10.f);
	}
};

//------------------------------

int main()
{
	AvoGUI::GUI* gui = new AvoGUI::GUI();
	gui->create("Pi day!", windowWidth, windowHeight, AvoGUI::WindowStyleFlags::DefaultNoResize);
	new Canvas(gui); // I know this looks scary but it's 100% valid.
	AvoGUI::GUI::run();
	gui->forget();
}
