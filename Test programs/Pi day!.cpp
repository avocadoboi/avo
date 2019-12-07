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

	Block() :
		position(0.), velocity(0.), inverseMass(0.), width(0.)
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
		p_context->fillRectangle((float)position, float(windowHeight - width), float(position + width), (float)windowHeight);
	}
};

//------------------------------

class PiDay : public AvoGUI::GUI, public AvoGUI::ButtonListener, public AvoGUI::EditableTextListener
{
private:
	Block m_firstBlock;
	Block m_secondBlock;
	
	AvoGUI::Text* m_text_numberOfCollisions;
	uint32_t m_numberOfCollisions;

	AvoGUI::Text* m_text_numberOfDigits;
	AvoGUI::TextField* m_textField_numberOfDigits;

	AvoGUI::Button* m_restartButton;

	void startSimulation()
	{
		m_firstBlock = Block(500.0L, 0.0L, 1.0L, 70.0L);
		m_secondBlock = Block(600.0L, -START_VELOCITY, m_secondBlock.inverseMass, 200.0L);
		m_numberOfCollisions = 0U;
		queueAnimationUpdate();
		invalidate();
	}
	void createText()
	{
		m_text_numberOfCollisions = getGUI()->getDrawingContext()->createText(std::to_string(m_numberOfCollisions).c_str(), 35.f);
		m_text_numberOfCollisions->setTopLeft(20.f, 10.f);
	}

public:
	PiDay() :
		m_text_numberOfCollisions(0), m_numberOfCollisions(0), m_text_numberOfDigits(0), m_textField_numberOfDigits(0), m_restartButton(0)
	{
		create("Pi day!", (uint32_t)windowWidth, (uint32_t)windowHeight, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}

	void createContent()
	{
		m_restartButton = new AvoGUI::Button(this, "RESTART", AvoGUI::Button::Emphasis::High);
		m_restartButton->setTopRight(getRight() - 10.f, 10.f);
		m_restartButton->addButtonListener(this);

		setThemeValue("text field height", 2.f);
		setThemeValue("text field font size", 16.f);
		m_textField_numberOfDigits = new AvoGUI::TextField(this, AvoGUI::TextField::Type::Outlined);
		m_textField_numberOfDigits->setTextAlign(AvoGUI::TextAlign::Center);
		m_textField_numberOfDigits->setWidth(40.f);
		m_textField_numberOfDigits->setTopRight(m_restartButton->getLeft() - 20.f, 5.f);
		m_textField_numberOfDigits->addEditableTextListener(this);
		m_textField_numberOfDigits->setString(std::to_string((uint32_t)NUMBER_OF_DIGITS));

		m_text_numberOfDigits = getGUI()->getDrawingContext()->createText("PI digits:", 18.f);
		m_text_numberOfDigits->setRight(m_textField_numberOfDigits->getLeft() - 7.f);
		m_text_numberOfDigits->setCenterY(m_textField_numberOfDigits->getCenterY() + 2.f);
		m_text_numberOfDigits->setFontWeight(AvoGUI::FontWeight::Regular);

		enableMouseEvents();
		createText();
		startSimulation();
	}

	//------------------------------

	bool handleEditableTextChange(AvoGUI::EditableText* p_editableText, std::string& p_string, int32_t& p_caretIndex) override
	{
		for (uint32_t a = 0; a < p_string.size(); a++)
		{
			if (p_string[a] < 48 || p_string[a] > 57)
			{
				return false;
			}
		}

		if (p_string.size())
		{
			p_caretIndex = p_caretIndex != 0;
			if (p_string.size() > 1)
			{
				p_string[0] = AvoGUI::max((char)49, p_string.back());
				p_string.resize(1);
			}
			m_secondBlock.inverseMass = pow(100.0L, -(long double)(p_string[0] - 48) + 1.0L);
		}
		startSimulation();
		return true;
	}
	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		setKeyboardFocus(0);
	}
	void handleButtonClick(AvoGUI::Button* p_button) override
	{
		startSimulation();
	}

	void updateAnimations() override
	{
		m_firstBlock.position += m_firstBlock.velocity;
		m_secondBlock.position += m_secondBlock.velocity;

		uint32_t numberOfCollisionsBefore = m_numberOfCollisions;

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
		if (numberOfCollisionsBefore != m_numberOfCollisions)
		{
			createText();
			invalidateRectangle(m_text_numberOfCollisions->getLeft(), m_text_numberOfCollisions->getTop(), 300.f, m_text_numberOfCollisions->getBottom() + 1.f);
		}

		if (m_firstBlock.position < getWidth())
		{
			invalidateRectangle(0, getHeight() - m_secondBlock.width, getWidth(), getHeight());
			queueAnimationUpdate();
		}
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		m_firstBlock.draw(p_context);
		m_secondBlock.draw(p_context);
		p_context->setColor(0xff111111);
		p_context->drawText(m_text_numberOfCollisions);
		p_context->drawText(m_text_numberOfDigits);
	}
};

//------------------------------

int main()
{ 
	PiDay* app = new PiDay();
	app->waitForFinish();
}
