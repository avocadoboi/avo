#include "../AvoGUI.hpp"

#include <algorithm>

//------------------------------

long double const NUMBER_OF_DIGITS = 7.0L;
long double const START_VELOCITY = 1.0L;
float windowWidth = 800.f;
float windowHeight = 350.f;

//------------------------------

class Block
{
public:
	long double position{ 0.L };
	long double velocity{ 0.L };
	long double inverseMass{ 0.L };
	long double width{ 0.L };
	Avo::Color color;

	Block()
	{
		color.setHSBA(Avo::random(), 0.95f, 0.8f);
	}
	Block(long double p_position, long double p_velocity, long double p_inverseMass, long double p_width) :
		position(p_position), velocity(p_velocity), inverseMass(p_inverseMass), width(p_width)
	{
		color.setHSBA(Avo::random(), 0.95f, 0.8f);
	}

	void draw(Avo::DrawingContext* p_context)
	{
		p_context->setColor(color);
		p_context->fillRectangle((float)position, float(windowHeight - width), float(position + width), (float)windowHeight);
	}
};

//------------------------------

class PiDay : public Avo::Gui
{
private:
	Block m_firstBlock;
	Block m_secondBlock;
	
	Avo::Text m_text_numberOfCollisions;
	uint32_t m_numberOfCollisions{ 0u };

	Avo::Text m_text_numberOfDigits;
	Avo::TextField* m_textField_numberOfDigits{ nullptr };

	Avo::Button* m_restartButton{ nullptr };

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
		m_text_numberOfCollisions = getDrawingContext()->createText(std::to_string(m_numberOfCollisions), 35.f);
		m_text_numberOfCollisions.setTopLeft(20.f, 10.f);
	}

public:
	PiDay()
	{
		create("Pi day!", (uint32_t)windowWidth, (uint32_t)windowHeight, Avo::WindowStyleFlags::DefaultNoResize);

		m_restartButton = new Avo::Button(this, "RESTART", Avo::Button::Emphasis::High);
		m_restartButton->setTopRight(getRight() - 10.f, 10.f);
		m_restartButton->buttonClickListeners += [this](auto) { startSimulation(); };

		setThemeValue(Avo::ThemeValues::textFieldHeight, 2.f);
		setThemeValue(Avo::ThemeValues::textFieldFontSize, 16.f);
		m_textField_numberOfDigits = new Avo::TextField(this, Avo::TextField::Type::Outlined);
		m_textField_numberOfDigits->setTextAlign(Avo::TextAlign::Center);
		m_textField_numberOfDigits->setWidth(40.f);
		m_textField_numberOfDigits->setTopRight(m_restartButton->getLeft() - 20.f, 5.f);
		m_textField_numberOfDigits->getEditableText()->editableTextChangeListeners += [this](Avo::EditableText* p_editableText, std::string& p_string, int32_t& p_caretIndex) -> bool {
			if (std::any_of(p_string.begin(), p_string.end(), [](char character) { return character < 48 || character > 57; }))
				return false;

			if (p_string.size())
			{
				p_caretIndex = p_caretIndex != 0;
				if (p_string.size() > 1)
				{
					p_string[0] = Avo::max((char)49, p_string.back());
					p_string.resize(1);
				}
				m_secondBlock.inverseMass = pow(100.0L, -(long double)(p_string[0] - 48) + 1.0L);
			}
			startSimulation();
			return true;
		};
		m_textField_numberOfDigits->setString(std::to_string((uint32_t)NUMBER_OF_DIGITS));

		m_text_numberOfDigits = getDrawingContext()->createText("PI digits:", 18.f);
		m_text_numberOfDigits.setRight(m_textField_numberOfDigits->getLeft() - 7.f);
		m_text_numberOfDigits.setCenterY(m_textField_numberOfDigits->getCenterY() + 2.f);
		m_text_numberOfDigits.setFontWeight(Avo::FontWeight::Regular);

		enableMouseEvents();
		createText();
		startSimulation();

		run();
	}

	//------------------------------

	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		setKeyboardFocus(0);
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
			invalidateRectangle(m_text_numberOfCollisions.getLeft(), m_text_numberOfCollisions.getTop(), 300.f, m_text_numberOfCollisions.getBottom() + 1.f);
		}

		if (m_firstBlock.position < getWidth())
		{
			invalidateRectangle(0, getHeight() - m_secondBlock.width, getWidth(), getHeight());
			queueAnimationUpdate();
		}
	}

	void draw(Avo::DrawingContext* p_context) override
	{
		m_firstBlock.draw(p_context);
		m_secondBlock.draw(p_context);
		p_context->setColor(Avo::Color(17));
		p_context->drawText(m_text_numberOfCollisions);
		p_context->drawText(m_text_numberOfDigits);
	}
};

//------------------------------

int main()
{ 
	new PiDay();
}
