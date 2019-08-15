#pragma once
#include "../../AvoGUI.hpp"
#include <chrono>
#include <math.h>

//------------------------------

uint32_t const TIMER_SPIRAL_RESOLUTION = 200U;
float const TIMER_SPIRAL_STEEPNESS = 20.f;
float const TIMER_SPIRAL_PADDING = 20.f;
float const TIMER_SPIRAL_RADIUS = 200.f;
float const TIMER_TEXT_FIELD_PADDING = 10.f;
float const TIMER_TEXT_FIELD_WIDTH = 60.5f;
float const TIMER_MAX_NUMBER_OF_HOURS = 24.f;

//------------------------------

class TimerApp : public AvoGUI::GUI, public AvoGUI::EditableTextListener
{
private:
	std::chrono::time_point<std::chrono::system_clock> timeStart;

	AvoGUI::Point<float>* m_spiralVertices;
	uint32_t m_numberOfSpiralVerticesInTotal;
	uint32_t m_numberOfSpiralVerticesToDraw;
	float m_timeLeft;
	bool m_isDraggingSpiral;

	AvoGUI::TextField* m_textField_hours;
	AvoGUI::TextField* m_textField_minutes;
	AvoGUI::TextField* m_textField_seconds;

public:
	TimerApp() :
		m_numberOfSpiralVerticesInTotal(0), m_numberOfSpiralVerticesToDraw(0), 
		m_timeLeft(0.f), m_isDraggingSpiral(false),
		m_textField_hours(0), m_textField_minutes(0), m_textField_seconds(0)
	{
		create("The best timer that exists", 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), 2.f*(TIMER_SPIRAL_RADIUS + TIMER_SPIRAL_PADDING), AvoGUI::WindowStyleFlags::DefaultNoResize);
		run();
	}

	//------------------------------

	void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
	{

	}
	void handleMouseMove(const AvoGUI::MouseEvent& p_event) override
	{

	}
	void handleKeyboardKeyDown(const AvoGUI::KeyboardEvent& p_event) override
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
		// Check input

		int32_t caretIndexBefore = p_newCaretIndex;
		uint32_t lastEmptyIndex = 0;
		for (uint32_t a = 0; a < p_newString.size(); a++)
		{
			if (p_newString[a] > 47 && p_newString[a] < 58)
			{
				p_newString[lastEmptyIndex] = p_newString[a];
				lastEmptyIndex++;
			}
			else if (a <= caretIndexBefore)
			{
				p_newCaretIndex--;
			}
		}
		if (lastEmptyIndex != p_newString.size())
		{
			p_newString.resize(lastEmptyIndex);
		}

		//------------------------------
		// Update timer



		return true;
	}

	//------------------------------

	void createContent() override
	{
		enableMouseEvents();

		//------------------------------

		m_theme->colors["background"] = AvoGUI::Color(0.1f);
		m_theme->colors["on background"] = AvoGUI::Color(0.97f);

		m_theme->values["text field height"] = 3.f;

		//------------------------------

		AvoGUI::View* textFieldContainer = new AvoGUI::View(this);
		textFieldContainer->enableMouseEvents();

		m_textField_hours = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH);
		m_textField_hours->setSuffixString(" H");
		m_textField_hours->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_hours->addEditableTextListener(this);

		m_textField_minutes = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH + 1.5f);
		m_textField_minutes->setLeft(m_textField_hours->getRight() + TIMER_TEXT_FIELD_PADDING);
		m_textField_minutes->setSuffixString(" M");
		m_textField_minutes->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_minutes->addEditableTextListener(this);

		m_textField_seconds = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH - 1.7f);
		m_textField_seconds->setLeft(m_textField_minutes->getRight() + TIMER_TEXT_FIELD_PADDING);
		m_textField_seconds->setSuffixString(" S");
		m_textField_seconds->setTextAlign(AvoGUI::TextAlign::Right);
		m_textField_seconds->addEditableTextListener(this);

		textFieldContainer->setPadding(1.f);
		textFieldContainer->setCenter(getCenter());

		//------------------------------
		// Generate spiral!

		//if (m_spiralAngle > 0.f)
		//{
		//	uint32_t oldSize = m_spiralVertices.size();
		//	uint32_t newSize = uint32_t(m_spiralAngle / AvoGUI::TAU * (float)TIMER_SPIRAL_RESOLUTION);
		//	m_spiralVertices.resize(newSize);
		//	if (newSize > oldSize)
		//	{
		//		for (uint32_t a = 0; a < m_spiralAngle; a += AvoGUI::TAU / TIMER_SPIRAL_RESOLUTION)
		//		{
		//			m_spiralVertices[a] 
		//		}
		//	}
		//}
	}

	//------------------------------

	void updateAnimations() override
	{
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		//p_context->strokeShape(m_spiralVertices, m_numberOfSpiralVerticesToDraw, 2.f);
	}
};
