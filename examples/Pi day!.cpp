#include "../AvoGUI.hpp"

#include <algorithm>

//------------------------------

constexpr auto NUMBER_OF_DIGITS = 7.0L;
constexpr auto START_VELOCITY = 1.0L;
constexpr auto windowWidth = 800.f;
constexpr auto windowHeight = 350.f;

//------------------------------

using Unit = long double;

class Block
{
public:
	Unit position{};
	Unit velocity{};
	Unit inverseMass{};
	Unit width{};
	Avo::Color color = Avo::Color::hsba(Avo::random(), 0.95f, 0.8f);

	auto draw(Avo::DrawingContext* p_context) -> void
	{
		p_context->setColor(color);
		p_context->fillRectangle({
			static_cast<float>(position), static_cast<float>(windowHeight - width), 
			static_cast<float>(position + width), static_cast<float>(windowHeight)
		});
	}

	Block() = default;
	Block(Unit p_position, Unit p_velocity, Unit p_inverseMass, Unit p_width) :
		position{p_position}, 
		velocity{p_velocity}, 
		inverseMass{p_inverseMass}, 
		width{p_width}
	{
	}
};

//------------------------------

struct PiDay : public Avo::Gui
{
private:
	Block m_firstBlock;
	Block m_secondBlock;
	
	// Avo::Text m_text_numberOfDigits;
	Avo::Text m_text_numberOfCollisions;
	Count m_numberOfCollisions{};
public:
	auto startSimulation() -> void
	{
		m_firstBlock = Block{500.0L, 0.0L, 1.0L, 70.0L};
		m_secondBlock = Block{600.0L, -START_VELOCITY, m_secondBlock.inverseMass, 200.0L};
		m_numberOfCollisions = 0U;
		queueAnimationUpdate();
		invalidate();
	}

	auto updateAnimations() -> void override
	{
		m_firstBlock.position += m_firstBlock.velocity;
		m_secondBlock.position += m_secondBlock.velocity;

		Count const numberOfCollisionsBefore = m_numberOfCollisions;

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
				auto const blockCollisionTime = m_firstBlock.velocity == m_secondBlock.velocity ? 0.0L : 
					((m_firstBlock.position + m_firstBlock.width - m_secondBlock.position) / (m_secondBlock.velocity - m_firstBlock.velocity));
				auto const impulse = 2.0L*(m_secondBlock.velocity - m_firstBlock.velocity) / (m_firstBlock.inverseMass + m_secondBlock.inverseMass);
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
			createCollisionsText();

			auto const bounds = m_text_numberOfCollisions.getBounds();
			invalidateRectangle({bounds.left, bounds.top, 300.f, bounds.bottom + 1.f});
		}

		if (m_firstBlock.position < getWidth())
		{
			auto const size = getSize();
			invalidateRectangle({0, static_cast<float>(size.height - m_secondBlock.width), size.width, size.height});
			queueAnimationUpdate();
		}
	}

	auto draw(Avo::DrawingContext* p_context) -> void override
	{
		m_firstBlock.draw(p_context);
		m_secondBlock.draw(p_context);
		p_context->setColor(17);
		p_context->drawText(m_text_numberOfCollisions);
		// p_context->drawText(m_text_numberOfDigits);
	}

	auto createCollisionsText() -> void
	{
		m_text_numberOfCollisions = getDrawingContext()->createText(Avo::numberToString(m_numberOfCollisions), 35.f);
		m_text_numberOfCollisions.setTopLeft({20.f, 10.f});
	}

	auto createControls() -> void
	{
		auto const restartButton = new Avo::Button{this, "RESTART", Avo::Button::Emphasis::High};
		restartButton->setTopRight({getRight() - 10.f, 10.f});
		restartButton->buttonClickListeners += [this](auto) { startSimulation(); };

		//------------------------------
		
		setThemeValue(Avo::ThemeValues::textFieldHeight, 2.f);
		setThemeValue(Avo::ThemeValues::textFieldFontSize, 16.f);

		auto const textField_numberOfDigits = new Avo::TextField(this, Avo::TextField::Type::Outlined);
		textField_numberOfDigits->setTextAlign(Avo::TextAlign::Center);
		textField_numberOfDigits->setWidth(40.f);
		textField_numberOfDigits->setTopRight({restartButton->getLeft() - 20.f, 5.f});
		textField_numberOfDigits->getEditableText()->editableTextChangeListeners += 
		[this](Avo::EditableText* p_editableText, std::string& p_string, Index& p_caretIndex) -> bool {
			if (!std::all_of(p_string.begin(), p_string.end(), [](char character) { return character >= '0' && character <= '9'; }))
			{
				return false;
			}

			if (!p_string.empty())
			{
				p_caretIndex = p_caretIndex != 0;
				if (p_string.size() > 1)
				{
					p_string[0] = Avo::max(49, p_string.back());
					p_string.resize(1);
				}
				m_secondBlock.inverseMass = pow(100.0L, -static_cast<Unit>(p_string[0] - 48) + 1.0L);
			}
			startSimulation();
			return true;
		};
		textField_numberOfDigits->setString(Avo::numberToString(NUMBER_OF_DIGITS));

		mouseDownListeners += [this](auto){ setKeyboardFocus(nullptr); };

		//------------------------------

		auto const text_numberOfDigits = new Avo::TextView{this, 18.f, "PI digits:"};
		// m_text_numberOfDigits = getDrawingContext()->createText("PI digits:", 18.f);
		text_numberOfDigits->setRight(textField_numberOfDigits->getLeft() - 7.f);
		text_numberOfDigits->setCenterY(textField_numberOfDigits->getCenterY() + 2.f);
		text_numberOfDigits->getText().setFontWeight(Avo::FontWeight::Regular);
	}

	PiDay()
	{
		create("Pi day!", {windowWidth, windowHeight}, Avo::WindowStyleFlags::DefaultNoResize);
		enableMouseEvents();

		createControls();
		createCollisionsText();
		startSimulation();

		run();
	}
	~PiDay()
	{
		
	}
};

//------------------------------

int main()
{ 
	new PiDay;
}
