#pragma once

#include <Windows.h>
#pragma comment(lib, "winmm")
#undef min
#undef max

#include <fstream>

#include "../../AvoGUI.hpp"
#include "Parameters.hpp"

//------------------------------

class SoundOpener : public AvoGUI::View, public AvoGUI::ButtonListener
{
private:
	AvoGUI::Text* m_text_sound;
	AvoGUI::Text* m_text_currentSoundFileName;
	AvoGUI::Button* m_button_open;

	std::string m_soundFilePath;

	AvoGUI::OpenFileDialog m_openFileDialog;

	void updateCurrentSoundFileNameText()
	{
		if (m_soundFilePath.size())
		{
			std::string fileName = m_soundFilePath;
			for (int32 a = fileName.size() - 1; a > 0; a--)
			{
				if (fileName[a] == '\\' || fileName[a] == '/')
				{
					fileName = fileName.substr((size_t)a + 1);
					break;
				}
			}
			if (fileName.size() > 13)
			{
				fileName = fileName.substr(0, 10) + "...";
			}
			m_text_currentSoundFileName = getGUI()->getDrawingContext()->createText(fileName.c_str(), 12.f);
		}
		else
		{
			m_text_currentSoundFileName = getGUI()->getDrawingContext()->createText("(none)", 12.f);
		}
		m_text_currentSoundFileName->setLeft(m_text_sound->getRight() + 2.f);
		m_text_currentSoundFileName->setFontWeight(AvoGUI::FontWeight::Regular);
		m_button_open->setLeft(m_text_currentSoundFileName->getRight() + 8.f);
		m_text_currentSoundFileName->setCenterY(m_button_open->getCenterY() - 0.5f);
		setSize(m_button_open->getBottomRight());
	}
	bool getIsSoundFileMp3()
	{
		return m_soundFilePath.substr(m_soundFilePath.size() - 3, 3) == "mp3";
	}

public:
	SoundOpener(AvoGUI::View* p_parent) :
		View(p_parent),
		m_text_sound(0), m_text_currentSoundFileName(0), m_button_open(0)
	{
		enableMouseEvents();

		m_openFileDialog.setFileExtensions({ { "Audio files", "*.mp3;*.wav" } });

		m_text_sound = getGUI()->getDrawingContext()->createText("Sound: ", 12.f);

		m_text_currentSoundFileName = getGUI()->getDrawingContext()->createText("(none)", 12.f);
		m_text_currentSoundFileName->setLeft(m_text_sound->getRight() + 2.f);
		m_text_currentSoundFileName->setFontWeight(AvoGUI::FontWeight::Regular);

		m_button_open = new AvoGUI::Button(this, "OPEN", AvoGUI::Button::Emphasis::Medium);
		m_button_open->setLeft(m_text_currentSoundFileName->getRight() + 8.f);
		m_button_open->addButtonListener(this);

		m_text_sound->setCenterY(m_button_open->getCenterY() - 0.5f);

		std::ifstream file;
		file.open("soundpath", std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			m_soundFilePath.resize(file.tellg());
			file.seekg(0);
			file.read((char*)m_soundFilePath.data(), m_soundFilePath.size());
		}
		file.close();

		updateCurrentSoundFileNameText();
	}
	~SoundOpener()
	{
		if (m_text_currentSoundFileName)
		{
			m_text_currentSoundFileName->forget();
		}
		if (m_text_sound)
		{
			m_text_sound->forget();
		}
	}

	void handleButtonClick(AvoGUI::Button* p_button) override
	{
		std::vector<std::string> filenames;
		m_openFileDialog.open(filenames);
		if (filenames.size())
		{
			m_soundFilePath = filenames[0];

			std::ofstream file;
			file.open("soundpath", std::ios::binary);
			file.write(m_soundFilePath.data(), m_soundFilePath.size());
			file.close();

			updateCurrentSoundFileNameText();
			invalidate();
		}
	}

	void playSound()
	{
		if (m_soundFilePath.size())
		{
			if (getIsSoundFileMp3())
			{
				mciSendString(("open \"" + m_soundFilePath + "\" type mpegvideo alias mp3").c_str(), 0, 0, 0);
				mciSendString("play mp3 from 0 repeat", 0, 0, 0);
			}
			else
			{
				PlaySoundA(m_soundFilePath.c_str(), GetModuleHandle(0), SND_LOOP | SND_ASYNC | SND_FILENAME);
			}
		}
	}

	void stopSound()
	{
		if (m_soundFilePath.size())
		{
			if (getIsSoundFileMp3())
			{
				mciSendString("stop mp3", 0, 0, 0);
				mciSendString("close mp3", 0, 0, 0);
			}
			else
			{
				PlaySoundA(0, GetModuleHandle(0), 0);
			}
		}
	}

	//------------------------------

	void handleSizeChange() override
	{
		setCenterX(getParent()->getWidth() * 0.5f);
	}

	//------------------------------

	void draw(AvoGUI::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->setColor(AvoGUI::Color(0.9f));
		p_drawingContext->drawText(m_text_sound);
		p_drawingContext->setColor(AvoGUI::Color(0.8f));
		p_drawingContext->drawText(m_text_currentSoundFileName);
	}
};

//------------------------------

class TimerApp;

class MainScreen : 
	public AvoGUI::View, 
	public AvoGUI::KeyboardListener, 
	public AvoGUI::EditableTextListener, 
	public AvoGUI::ButtonListener
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_timeStart;
	bool m_willRestart;

	AvoGUI::Point<float>* m_spiralVertices;
	uint32 m_numberOfSpiralVerticesInTotal;
	float m_startAngle;
	float m_currentAngle;
	bool m_isDraggingSpiral;
	bool m_hasDraggedSpiral;

	AvoGUI::Text* m_text_timeLeft;
	AvoGUI::Button* m_button_restart;

	AvoGUI::TextField* m_textField_hours;
	AvoGUI::TextField* m_textField_minutes;
	AvoGUI::TextField* m_textField_seconds;

	SoundOpener* m_soundOpener;

	//------------------------------

	void updateTimeLeftText()
	{
		if (m_text_timeLeft)
		{
			m_text_timeLeft->forget();
		}
		if (m_currentAngle)
		{
			float angle = std::ceil(m_currentAngle * 3600.f) / 3600.f;
			float minutes = (angle - (uint32)angle) * 60.f;
			uint32 seconds = std::round((minutes - (uint32)minutes) * 60.f);
			m_text_timeLeft = getGUI()->getDrawingContext()->createText(((angle < 10 ? "0" + AvoGUI::convertNumberToString((uint32)angle) : AvoGUI::convertNumberToString((uint32)angle)) + (minutes < 10 ? ":0" : ":") + AvoGUI::convertNumberToString((uint32)minutes) + (seconds < 10u ? ":0" : ":") + AvoGUI::convertNumberToString(seconds)).c_str(), 23.f);
		}
		else
		{
			m_text_timeLeft = getGUI()->getDrawingContext()->createText("00:00:00", 23.f);
		}

		m_text_timeLeft->setFontWeight(AvoGUI::FontWeight::Regular);
		m_text_timeLeft->setCenter(getWidth() * 0.5f - 16.f, getHeight() * 0.5f - 50.f);
	}

public:
	MainScreen(TimerApp* p_app);

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		getGUI()->setKeyboardFocus(0);
		if (AvoGUI::Point<>::getLengthSquared(p_event.x - getWidth() * 0.5f, p_event.y - getHeight() * 0.5f) >
			AvoGUI::Point<>::getLengthSquared(m_spiralVertices[m_numberOfSpiralVerticesInTotal - 1].x - getWidth() * 0.5f, m_spiralVertices[m_numberOfSpiralVerticesInTotal - 1].y - getHeight() * 0.5f))
		{
			m_isDraggingSpiral = true;
			m_hasDraggedSpiral = false;
		}
	}
	void handleMouseUp(AvoGUI::MouseEvent const& p_event) override
	{
		if (m_hasDraggedSpiral)
		{
			m_hasDraggedSpiral = false;
			m_willRestart = true;
		}
		m_isDraggingSpiral = false;
	}
	void handleMouseMove(AvoGUI::MouseEvent const& p_event) override
	{
		if (m_isDraggingSpiral && (p_event.x != getWidth() * 0.5f || p_event.y != getHeight() * 0.5f))
		{
			m_hasDraggedSpiral = true;

			float normalizedCurrentAngle = m_currentAngle - floor(m_currentAngle);
			float normalizedCursorAngle = std::atan2(p_event.y - getHeight() * 0.5f, p_event.x - getWidth() * 0.5f) / AvoGUI::TAU + 0.25;
			normalizedCursorAngle -= floor(normalizedCursorAngle);

			float nextAngle = m_currentAngle;
			if (normalizedCurrentAngle > normalizedCursorAngle)
			{
				if (normalizedCurrentAngle - normalizedCursorAngle < 0.5f)
				{
					nextAngle += normalizedCursorAngle - normalizedCurrentAngle;
				}
				else
				{
					nextAngle += 1.f - (normalizedCurrentAngle - normalizedCursorAngle);
				}
			}
			else
			{
				if (normalizedCursorAngle - normalizedCurrentAngle < 0.5f)
				{
					nextAngle += normalizedCursorAngle - normalizedCurrentAngle;
				}
				else
				{
					nextAngle -= 1.f - (normalizedCursorAngle - normalizedCurrentAngle);
				}
			}
			nextAngle = AvoGUI::constrain(nextAngle, 0.f, TIMER_MAX_NUMBER_OF_HOURS);

			m_textField_hours->setValue((int)nextAngle);
			float minutes = (nextAngle - (int)nextAngle) * 60.f;
			m_textField_minutes->setValue((int)minutes);
			m_textField_seconds->setValue(std::round((minutes - (int)minutes) * 60.f));

			m_startAngle = nextAngle;
			m_currentAngle = nextAngle;

			invalidate();
		}
	}
	void handleKeyboardKeyDown(AvoGUI::KeyboardEvent const& p_event) override
	{
		if (p_event.key == AvoGUI::KeyboardKey::Tab)
		{
			if (m_textField_hours->getHasKeyboardFocus())
			{
				getGUI()->setKeyboardFocus(m_textField_minutes);
			}
			else if (m_textField_minutes->getHasKeyboardFocus())
			{
				getGUI()->setKeyboardFocus(m_textField_seconds);
			}
			else
			{
				getGUI()->setKeyboardFocus(m_textField_hours);
			}
			((AvoGUI::EditableText*)getGUI()->getKeyboardFocus())->selectAll();
		}
	}

	//------------------------------

	bool handleEditableTextChange(AvoGUI::EditableText* p_editableText, std::string& p_newString, int32& p_newCaretIndex) override
	{
		for (uint32 a = 0; a < p_newString.size(); a++)
		{
			if (p_newString[a] < 48 || p_newString[a] > 57)
			{
				return false;
			}
		}

		if (p_newString == "0")
		{
			p_newString = "";
			p_newCaretIndex = 0;
		}

		m_willRestart = true;

		return true;
	}

	//------------------------------

	void handleButtonClick(AvoGUI::Button* p_button) override
	{
		m_willRestart = true;
	}

	//------------------------------

	void stopTimerSound()
	{
		m_soundOpener->stopSound();
	}

	//------------------------------

	void updateAnimations() override;

	void draw(AvoGUI::DrawingContext* p_context, AvoGUI::Rectangle<float> const& p_targetRectangle) override
	{
		p_context->clear(getThemeColor("background"));
		if (p_targetRectangle == getBounds())
		{
			p_context->setColor(AvoGUI::Color(1.f, 0.5f, 0.7f, 0.9f));
			p_context->strokeShape(m_spiralVertices, (uint32)round(m_currentAngle * TIMER_SPIRAL_RESOLUTION), 3.f);

			float magnitude = TIMER_SPIRAL_RADIUS * (1.f - TIMER_SPIRAL_STEEPNESS * m_currentAngle / TIMER_MAX_NUMBER_OF_HOURS);
			AvoGUI::Point<float> point(getWidth() * 0.5f + std::cos((m_currentAngle - 0.25f) * AvoGUI::TAU) * magnitude, getHeight() * 0.5f + std::sin((m_currentAngle - 0.25f) * AvoGUI::TAU) * magnitude);
			p_context->fillCircle(point, 5);
			p_context->setColor(AvoGUI::Color(0.f));
			p_context->strokeCircle(point, 5, 2.f);

			p_context->setColor(~0U);
			p_context->drawText(m_text_timeLeft);
		}
	}

};
