#pragma once

#include <fstream>

#include "../../AvoGUI.hpp"
#include "Parameters.hpp"

//------------------------------

class SoundOpener : public AvoGUI::View
{
private:
	AvoGUI::Text* m_text_sound{ getDrawingContext()->createText("Sound: ", 12.f) };
	AvoGUI::Text* m_text_currentSoundFileName{ getDrawingContext()->createText("(none)", 12.f) };
	AvoGUI::Button* m_button_open{ new AvoGUI::Button(this, "OPEN", AvoGUI::Button::Emphasis::Medium) };

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
			m_text_currentSoundFileName = getDrawingContext()->createText(fileName.c_str(), 12.f);
		}
		else
		{
			m_text_currentSoundFileName = getDrawingContext()->createText("(none)", 12.f);
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
		View(p_parent, Ids::soundOpener)
	{
		enableMouseEvents();

		m_openFileDialog.setFileExtensions({ { "Audio files", "*.mp3;*.wav" } });

		m_text_currentSoundFileName->setLeft(m_text_sound->getRight() + 2.f);
		m_text_currentSoundFileName->setFontWeight(AvoGUI::FontWeight::Regular);

		m_button_open->setLeft(m_text_currentSoundFileName->getRight() + 8.f);
		m_button_open->buttonClickListeners += [this](auto) {
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
		};

		m_text_sound->setCenterY(m_button_open->getCenterY() - 0.5f);

		//------------------------------

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

	void playSound();

	void stopSound();

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

class MainScreen : public AvoGUI::View
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_timeStart;
	bool m_willRestart{ false };

	AvoGUI::Point<float>* m_spiralVertices{ nullptr };
	uint32 m_numberOfSpiralVerticesInTotal{ 0 };
	float m_startAngle{ 0.f };
	float m_currentAngle{ 0.f };
	bool m_isDraggingSpiral{ false };
	bool m_hasDraggedSpiral{ false };

	AvoGUI::Text* m_text_timeLeft{ nullptr };

	AvoGUI::TextField* m_textField_hours{ nullptr };
	AvoGUI::TextField* m_textField_minutes{ nullptr };
	AvoGUI::TextField* m_textField_seconds{ nullptr };

	SoundOpener* m_soundOpener{ nullptr };

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
			m_text_timeLeft = getGui()->getDrawingContext()->createText(((angle < 10 ? "0" + AvoGUI::convertNumberToString((uint32)angle) : AvoGUI::convertNumberToString((uint32)angle)) + (minutes < 10 ? ":0" : ":") + AvoGUI::convertNumberToString((uint32)minutes) + (seconds < 10u ? ":0" : ":") + AvoGUI::convertNumberToString(seconds)).c_str(), 23.f);
		}
		else
		{
			m_text_timeLeft = getGui()->getDrawingContext()->createText("00:00:00", 23.f);
		}

		m_text_timeLeft->setFontWeight(AvoGUI::FontWeight::Regular);
		m_text_timeLeft->setCenter(getWidth() * 0.5f - 16.f, getHeight() * 0.5f - 50.f);
	}

public:
	MainScreen(View* p_app);

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		getGui()->setKeyboardFocus(0);
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

	//------------------------------

	//void stopTimerSound()
	//{
	//	m_soundOpener->stopSound();
	//}

	//------------------------------

	void updateAnimations() override;

	void draw(AvoGUI::DrawingContext* p_context, AvoGUI::Rectangle<float> const& p_targetRectangle) override
	{
		p_context->clear(getThemeColor(AvoGUI::ThemeColors::background));
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
