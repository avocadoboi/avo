#pragma once
#include "../../AvoGUI.hpp"
#include <chrono>
#include <math.h>
#include <Windows.h>
#include <ShObjIdl.h>

#undef min
#undef max

//------------------------------

uint32_t const TIMER_SPIRAL_RESOLUTION = 360U;
float const TIMER_SPIRAL_STEEPNESS = 0.4f;
float const TIMER_SPIRAL_PADDING = 20.f;
float const TIMER_SPIRAL_RADIUS = 200.f;
float const TIMER_TEXT_FIELD_PADDING = 10.f;
float const TIMER_TEXT_FIELD_WIDTH = 60.5f;
float const TIMER_MAX_NUMBER_OF_HOURS = 12.f;

//------------------------------

class TimerApp : public AvoGUI::GUI, public AvoGUI::EditableTextListener, public AvoGUI::ButtonListener
{
private:
	std::chrono::time_point<std::chrono::system_clock> m_timeStart;

	AvoGUI::Point<float>* m_spiralVertices;
	uint32_t m_numberOfSpiralVerticesInTotal;
	float m_startAngle;
	float m_currentAngle;
	bool m_isHoveringSpiral;
	bool m_isDraggingSpiral;

	AvoGUI::Text* m_text_timeLeft;
	AvoGUI::Button* m_button_restart;

	AvoGUI::TextField* m_textField_hours;
	AvoGUI::TextField* m_textField_minutes;
	AvoGUI::TextField* m_textField_seconds;
	AvoGUI::Button* m_button_openSound;

	std::string m_soundFilePath;

public:
	TimerApp() :
		m_spiralVertices(0), m_numberOfSpiralVerticesInTotal(0), 
		m_startAngle(0.f), m_currentAngle(0.f), m_isHoveringSpiral(false), m_isDraggingSpiral(false),
		m_textField_hours(0), m_textField_minutes(0), m_textField_seconds(0),
		m_button_openSound(0)
	{
		create("The best timer that exists", 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), AvoGUI::WindowStyleFlags::DefaultNoResize);
		run();
	}

	//------------------------------

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		if (m_isHoveringSpiral)
		{
			m_isDraggingSpiral = true;
		}
	}
	void handleMouseUp(AvoGUI::MouseEvent const& p_event) override
	{
		m_isDraggingSpiral = false;
	}
	void handleMouseMove(AvoGUI::MouseEvent const& p_event) override
	{
		m_isHoveringSpiral = 
			AvoGUI::Point<>::getLengthSquared(p_event.x - getWidth() * 0.5f, p_event.y - getHeight() * 0.5f) >
			AvoGUI::Point<>::getLengthSquared(m_spiralVertices[m_numberOfSpiralVerticesInTotal - 1].x - getWidth() * 0.5f, m_spiralVertices[m_numberOfSpiralVerticesInTotal - 1].y - getHeight() * 0.5f);

		if (m_isDraggingSpiral && (p_event.x != getWidth()*0.5f || p_event.y != getHeight()*0.5f))
		{
			float normalizedStartAngle = m_startAngle - floor(m_startAngle);
			float normalizedCursorAngle = std::atan2(p_event.y - getHeight() * 0.5f, p_event.x - getWidth() * 0.5f) / AvoGUI::TAU + 0.25;
			normalizedCursorAngle -= floor(normalizedCursorAngle);

			float nextAngle = m_startAngle;
			if (normalizedStartAngle > normalizedCursorAngle)
			{
				if (normalizedStartAngle - normalizedCursorAngle < 0.5f)
				{
					nextAngle += normalizedCursorAngle - normalizedStartAngle;
				}
				else
				{
					nextAngle += 1.f - (normalizedStartAngle - normalizedCursorAngle);
				}
			}
			else
			{
				if (normalizedCursorAngle - normalizedStartAngle < 0.5f)
				{
					nextAngle += normalizedCursorAngle - normalizedStartAngle;
				}
				else
				{
					nextAngle -= 1.f - (normalizedCursorAngle - normalizedStartAngle);
				}
			}
			nextAngle = AvoGUI::constrain(nextAngle, 0.f, TIMER_MAX_NUMBER_OF_HOURS);

			m_textField_hours->setString(std::to_string((int)nextAngle));
			float minutes = (nextAngle - (int)nextAngle) * 60.f;
			m_textField_minutes->setString(std::to_string((int)minutes));
			m_textField_seconds->setString(std::to_string(int((minutes - (int)minutes) * 60.f)));
		}
	}
	void handleKeyboardKeyDown(AvoGUI::KeyboardEvent const& p_event) override
	{
		GUI::handleKeyboardKeyDown(p_event);
		if (p_event.key == AvoGUI::KeyboardKey::Tab)
		{
			if (m_textField_hours->getHasKeyboardFocus())
			{
				setKeyboardFocus(m_textField_minutes);
			}
			else if (m_textField_minutes->getHasKeyboardFocus())
			{
				setKeyboardFocus(m_textField_seconds);
			}
			else if (m_textField_seconds->getHasKeyboardFocus())
			{
				setKeyboardFocus(m_textField_hours);
			}
		}
	}

	//------------------------------

	bool handleEditableTextChange(AvoGUI::EditableText* p_editableText, std::string& p_newString, int32_t& p_newCaretIndex) override
	{
		for (uint32_t a = 0; a < p_newString.size(); a++)
		{
			if (p_newString[a] < 48 || p_newString[a] > 57)
			{
				return false;
			}
		}

		// Update text fields and m_startAngle
		if (p_newString.size())
		{
			if (p_editableText->getParent() == m_textField_hours)
			{
				int hours = AvoGUI::min((int)TIMER_MAX_NUMBER_OF_HOURS, std::stoi(p_newString));
				p_newString = std::to_string(hours);
				if (hours == TIMER_MAX_NUMBER_OF_HOURS)
				{
					m_startAngle = TIMER_MAX_NUMBER_OF_HOURS;
					m_textField_minutes->setString("0");
					m_textField_seconds->setString("0");
				}
				else
				{
					m_startAngle += float((p_newString.size() ? std::stoi(p_newString) : 0) - (m_textField_hours->getString()[0] ? std::stoi(m_textField_hours->getString()) : 0));
				}
			}
			else if (p_editableText->getParent() == m_textField_minutes)
			{
				if (m_startAngle >= TIMER_MAX_NUMBER_OF_HOURS)
				{
					p_newString = "0";
					m_startAngle = TIMER_MAX_NUMBER_OF_HOURS;
				}
				else
				{
					p_newString = std::to_string(AvoGUI::min(59, std::stoi(p_newString)));
					m_startAngle += ((p_newString.size() ? std::stoi(p_newString) : 0) - (m_textField_minutes->getString()[0] ? std::stoi(m_textField_minutes->getString()) : 0)) / 60.f;
				}
			}
			else
			{
				if (m_startAngle >= TIMER_MAX_NUMBER_OF_HOURS)
				{
					p_newString = "0";
					m_startAngle = TIMER_MAX_NUMBER_OF_HOURS;
				}
				else
				{
					p_newString = std::to_string(AvoGUI::min(59, std::stoi(p_newString)));
					m_startAngle += ((p_newString.size() ? std::stoi(p_newString) : 0) - (m_textField_seconds->getString()[0] ? std::stoi(m_textField_seconds->getString()) : 0)) / 3600.f;
				}
			}
		}
		else
		{
			if (p_editableText->getParent() == m_textField_hours)
			{
				m_startAngle -= float(m_textField_hours->getString()[0] ? std::stoi(m_textField_hours->getString()) : 0);
			}
			else if (p_editableText->getParent() == m_textField_minutes)
			{
				m_startAngle -= (m_textField_minutes->getString()[0] ? std::stoi(m_textField_minutes->getString()) : 0) / 60.f;
			}
			else
			{
				m_startAngle -= (m_textField_seconds->getString()[0] ? std::stoi(m_textField_seconds->getString()) : 0) / 3600.f;
			}
		}

		m_timeStart = std::chrono::system_clock::now();

		return true;
	}

	//------------------------------

	void handleButtonClick(AvoGUI::Button* p_button) override
	{
	}

	//------------------------------

	void createContent() override
	{
		enableMouseEvents();

		//------------------------------

		m_theme->colors["primary on background"] = AvoGUI::Color(1.f, 0.5f, 0.7f, 0.9f);
		m_theme->colors["primary"] = AvoGUI::Color(1.f, 0.3f, 0.7f, 0.9f);

		m_theme->colors["background"] = AvoGUI::Color(0.1f);
		m_theme->colors["on background"] = AvoGUI::Color(0.97f);

		m_theme->values["text field height"] = 2.4f;

		m_theme->values["button font size"] = 12.f;
		m_theme->values["button character spacing"] = 0.f;

		//------------------------------

		AvoGUI::View* textFieldContainer = new AvoGUI::View(this);
		textFieldContainer->enableMouseEvents();

		m_textField_hours = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH);
		m_textField_hours->setSuffixString(" H");
		m_textField_hours->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_hours->setString("1");

		m_textField_minutes = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH + 1.5f);
		m_textField_minutes->setLeft(m_textField_hours->getRight() + TIMER_TEXT_FIELD_PADDING);
		m_textField_minutes->setSuffixString(" M");
		m_textField_minutes->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_minutes->setString("0");

		m_textField_seconds = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH - 1.7f);
		m_textField_seconds->setLeft(m_textField_minutes->getRight() + TIMER_TEXT_FIELD_PADDING);
		m_textField_seconds->setSuffixString(" S");
		m_textField_seconds->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_seconds->setString("0");

		m_textField_hours->addEditableTextListener(this);
		m_textField_minutes->addEditableTextListener(this);
		m_textField_seconds->addEditableTextListener(this);

		m_button_openSound = new AvoGUI::Button(textFieldContainer, "Open sound...", AvoGUI::Button::Emphasis::Low);
		m_button_openSound->setTop(m_textField_hours->getBottom() + 20.f);
		m_button_openSound->setCenterX(m_textField_minutes->getCenterX());
		m_button_openSound->addButtonListener(this);

		textFieldContainer->setPadding(1.f);
		textFieldContainer->setCenter(getCenterX(), getCenterY() - 1.f);

		//------------------------------
		// Generate spiral!

		m_numberOfSpiralVerticesInTotal = TIMER_SPIRAL_RESOLUTION * TIMER_MAX_NUMBER_OF_HOURS;
		m_spiralVertices = new AvoGUI::Point<float>[m_numberOfSpiralVerticesInTotal];
		for (uint32_t a = 0; a < m_numberOfSpiralVerticesInTotal; a ++)
		{
			float angle = ((float)a / TIMER_SPIRAL_RESOLUTION - 0.25f) * AvoGUI::TAU;
			float magnitude = TIMER_SPIRAL_RADIUS * (1.f - TIMER_SPIRAL_STEEPNESS*(float)a / m_numberOfSpiralVerticesInTotal);
			m_spiralVertices[a] = AvoGUI::Point<float>(getWidth()*0.5f + std::cos(angle)*magnitude, getHeight()*0.5f + std::sin(angle)*magnitude);
		}
		m_startAngle = 1.f; // One hour, 360 degrees, TAU radians
		m_timeStart = std::chrono::system_clock::now();

		queueAnimationUpdate();
	}

	//------------------------------

	void updateAnimations() override
	{
		float angleBefore = m_currentAngle;
		m_currentAngle = AvoGUI::max(0.f, m_startAngle - std::chrono::duration<float>(std::chrono::system_clock::now() - m_timeStart).count()/3600.f);
		if (angleBefore && !m_currentAngle && m_soundFilePath.size())
		{
			//PlaySound(m_soundFilePath.c_str(), GetModuleHandle(0), SND_ASYNC | SND_FILENAME);
		}
		if (getWindow()->getState() != AvoGUI::WindowState::Minimized)
		{
			invalidate();
		}
		queueAnimationUpdate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(1.f, 0.5f, 0.7f, 0.9f));
		p_context->strokeShape(m_spiralVertices, (uint32_t)round(m_currentAngle * TIMER_SPIRAL_RESOLUTION), 3.f);

		float magnitude = TIMER_SPIRAL_RADIUS * (1.f - TIMER_SPIRAL_STEEPNESS * m_currentAngle/TIMER_MAX_NUMBER_OF_HOURS);
		AvoGUI::Point<float> point(getWidth() * 0.5f + std::cos((m_currentAngle - 0.25f)*AvoGUI::TAU) * magnitude, getHeight() * 0.5f + std::sin((m_currentAngle - 0.25f)*AvoGUI::TAU) * magnitude);
		p_context->fillCircle(point, 5);
		p_context->setColor(AvoGUI::Color(0.f));
		p_context->strokeCircle(point, 5, 2.f);
	}
};
