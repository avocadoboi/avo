#pragma once

#include "Parameters.hpp"
#include "IconButton.hpp"

#include "../../AvoGUI.hpp"
#include <fstream>

//------------------------------

class SoundOpener : public AvoGUI::View
{
private:
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
		m_text_currentSoundFileName.setLeft(m_text_sound.getRight() + 2.f);
		m_text_currentSoundFileName.setFontWeight(AvoGUI::FontWeight::Regular);
		m_button_open->setLeft(m_text_currentSoundFileName.getRight() + 8.f);
		m_text_currentSoundFileName.setCenterY(m_button_open->getCenterY() - 0.5f);
		setSize(m_button_open->getBottomRight());
	}	
	bool getIsSoundFileMp3()
	{
		return m_soundFilePath.substr(m_soundFilePath.size() - 3, 3) == "mp3";
	}

public:
	void playSound();
	void stopSound();

	//------------------------------

	void handleSizeChange() override
	{
		setCenterX(getParent<View>()->getWidth() * 0.5f);
	}

	//------------------------------

private:
	AvoGUI::Text m_text_sound{ getDrawingContext()->createText("Sound: ", 12.f) };
	AvoGUI::Text m_text_currentSoundFileName{ getDrawingContext()->createText("(none)", 12.f) };
public:
	void draw(AvoGUI::DrawingContext* p_drawingContext) override
	{
		p_drawingContext->setColor(AvoGUI::Color(0.9f));
		p_drawingContext->drawText(m_text_sound);
		p_drawingContext->setColor(AvoGUI::Color(0.8f));
		p_drawingContext->drawText(m_text_currentSoundFileName);
	}

	//------------------------------

private:
	AvoGUI::Button* m_button_open{ new AvoGUI::Button(this, "OPEN", AvoGUI::Button::Emphasis::Medium) };
	AvoGUI::OpenFileDialog m_openFileDialog;
	std::string m_soundFilePath;
public:
	SoundOpener(AvoGUI::View* p_parent) :
		View(p_parent, Ids::soundOpener)
	{
		enableMouseEvents();

		m_openFileDialog.setFileExtensions({ { "Audio files", "*.mp3;*.wav" } });

		m_text_currentSoundFileName.setLeft(m_text_sound.getRight() + 2.f);
		m_text_currentSoundFileName.setFontWeight(AvoGUI::FontWeight::Regular);

		m_button_open->setLeft(m_text_currentSoundFileName.getRight() + 8.f);
		m_button_open->buttonClickListeners += [this](auto) {
			std::vector<std::string> filenames = m_openFileDialog.open();
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

		m_text_sound.setCenterY(m_button_open->getCenterY() - 0.5f);

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
};

class TimePlayer : public AvoGUI::View
{
public:
	bool willRestart{ false };
	bool isPlaying{ false };

private:
	AvoGUI::TextView* m_text_timeLeft{ new AvoGUI::TextView(this, 23.f) };
	IconButton* m_button_restart;
	IconButton* m_button_playPause;
public:
	void setTimeLeft(double p_angle)
	{
		if (p_angle)
		{
			double totalSeconds = std::ceil(p_angle * 3600.);
			uint32 hours = totalSeconds/3600.;
			uint32 minutes = totalSeconds/60. - hours*60.;
			uint32 seconds = totalSeconds - hours*3600. - minutes*60.;
			m_text_timeLeft->setString((hours < 10 ? "0" : "") + AvoGUI::convertNumberToString(hours)
				                     + (minutes < 10 ? ":0" : ":") + AvoGUI::convertNumberToString(minutes) 
				                     + (seconds < 10u ? ":0" : ":") + AvoGUI::convertNumberToString(seconds));
		}
		else
		{
			m_text_timeLeft->setString("00:00:00");
		}

		m_text_timeLeft->getText().setFontWeight(AvoGUI::FontWeight::Regular);
		m_text_timeLeft->fitSizeToText();
		m_text_timeLeft->setCenterY(getHeight() * 0.5f);

		m_button_playPause->setLeft(m_text_timeLeft->getRight() + 10.f);
		m_button_restart->setLeft(m_button_playPause->getRight() + 5.f);
		setWidth(m_button_restart->getRight());
		setCenterX(getParent<View>()->getCenterX());
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> m_timeStart{ std::chrono::steady_clock::now() };
	std::chrono::time_point<std::chrono::steady_clock> m_lastPauseTime{ std::chrono::steady_clock::now() };
public:
	void reset()
	{
		m_timeStart = std::chrono::steady_clock::now();
		m_lastPauseTime = m_timeStart;
	}
	auto getTimeStart()
	{
		return m_timeStart;
	}

	TimePlayer(AvoGUI::View* p_parent) :
		View(p_parent)
	{
		enableMouseEvents();
		setHeight(35.f);

		auto const ICON_BUTTON_SIZE = 14.f;

		m_button_playPause = new IconButton(this, ICON_BUTTON_SIZE);
		m_button_playPause->setIcon(MaterialIcons::PLAY_ARROW);
		m_button_playPause->setCenterY(getHeight()*0.5f);
		m_button_playPause->setTooltip("Play timer");
		m_button_playPause->buttonClickListeners += [&]() {
			isPlaying = !isPlaying;
			if (isPlaying)
			{
				m_button_playPause->setIcon(MaterialIcons::PAUSE);
				m_timeStart += std::chrono::steady_clock::now() - m_lastPauseTime;
			}
			else
			{
				m_button_playPause->setIcon(MaterialIcons::PLAY_ARROW);
				m_lastPauseTime = std::chrono::steady_clock::now();
			}
		};

		m_button_restart = new IconButton(this, ICON_BUTTON_SIZE);
		m_button_restart->setIcon(MaterialIcons::REPLAY);
		m_button_restart->setCenterY(getHeight()*0.5f);
		m_button_restart->setTooltip("Restart timer");
		m_button_restart->buttonClickListeners += [&]() { willRestart = true; };

		getGui()->globalKeyboardKeyUpListeners += [&](AvoGUI::KeyboardEvent p_event) { 
			if (p_event.key == AvoGUI::KeyboardKey::Spacebar)
			{
				m_button_playPause->buttonClickListeners(); 
			}
		};
	}
};

//------------------------------

class TimerApp;

class MainScreen : public AvoGUI::View
{
private:
	AvoGUI::Point<float>* m_spiralVertices{ nullptr };
	uint32 m_numberOfSpiralVerticesInTotal{ 0 };
	double m_startAngle{ 0.f };
	double m_currentAngle{ 0.f };
	bool m_isDraggingSpiral{ false };
	bool m_hasDraggedSpiral{ false };

	AvoGUI::TextField* m_textField_hours{ nullptr };
	AvoGUI::TextField* m_textField_minutes{ nullptr };
	AvoGUI::TextField* m_textField_seconds{ nullptr };

	TimePlayer* m_timePlayer{ nullptr };
	SoundOpener* m_soundOpener{ nullptr };

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
			m_timePlayer->willRestart = true;
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
		}
	}
};
