#include "../AvoGUI.hpp"

//------------------------------

long double const NUMBER_OF_DIGITS = 7.0L;
long double const START_VELOCITY = 1.0L;
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

	Block()
	{
		color.setHSBA(AvoGUI::random(), 0.95f, 0.8f);
	}
	Block(long double p_position, long double p_velocity, long double p_inverseMass, long double p_width) :
		position(p_position), velocity(p_velocity), inverseMass(p_inverseMass), width(p_width)
	{
		color.setHSBA(AvoGUI::random(), 0.95f, 0.8f);
	}

	void draw(AvoGUI::DrawingContext* p_context)
	{
		p_context->setColor(color);
		p_context->fillRectangle(position, windowHeight - width, position + width, windowHeight);
	}
};

//------------------------------

class PiDay : public AvoGUI::GUI, public AvoGUI::ButtonListener
{
private:
	Block m_firstBlock;
	Block m_secondBlock;
	uint32_t m_numberOfCollisions;
	AvoGUI::Button* m_restartButton;
	AvoGUI::TextField* m_testTextField;

	void startSimulation()
	{
		m_firstBlock = Block(500.0L, 0.0L, 1.0L, 70.0L);
		m_secondBlock = Block(600.0L, -START_VELOCITY, pow(100.0L, -NUMBER_OF_DIGITS + 1.0L), 200.0L);
		m_numberOfCollisions = 0U;
	}

public:
	PiDay()
	{
		create("Pi day!", windowWidth, windowHeight, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}

	//------------------------------

	void createContent()
	{
		m_restartButton = new AvoGUI::Button(this, "RESTART", AvoGUI::Button::Emphasis::High);
		m_restartButton->setIcon(getGUI()->getDrawingContext()->createImage("test icon.png"));
		m_restartButton->setTopRight(getRight() - 10.f, 10.f);
		m_restartButton->addButtonListener(this);

		m_testTextField = new AvoGUI::TextField(this, AvoGUI::TextField::Type::Filled, "Test!");
		m_testTextField->setTopRight(getRight() - 10.f, m_restartButton->getBottom() + 10.f);

		AvoGUI::TextProperties textProperties;
		textProperties.fontSize = 35.f;
		getGUI()->getDrawingContext()->setDefaultTextProperties(textProperties);

		startSimulation();

		queueAnimationUpdate();
	}

	//------------------------------

	void handleButtonClick(AvoGUI::Button* p_button) override
	{
		startSimulation();
	}

	void updateAnimations() override
	{
		m_firstBlock.position += m_firstBlock.velocity;
		m_secondBlock.position += m_secondBlock.velocity;

		// Trace back and resolve all collisions that have happened in the last timestep/frame.
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
		m_firstBlock.draw(p_context);
		m_secondBlock.draw(p_context);
		p_context->setColor(0xff111111);
		p_context->drawText(std::to_string(m_numberOfCollisions).c_str(), 20.f, 10.f);
	}
};

//------------------------------

int main()
{
	PiDay* application = new PiDay();
	application->run();
	application->forget();
}
