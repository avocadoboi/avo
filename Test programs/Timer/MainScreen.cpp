#include "MainScreen.hpp"
#include "Timer.hpp"

#include <Windows.h>
#pragma comment(lib, "winmm")
#undef min
#undef max

//------------------------------
// class SoundOpener
//------------------------------

void SoundOpener::playSound()
{
	if (m_soundFilePath.size())
	{
		if (getIsSoundFileMp3())
		{
			mciSendStringA(("open \"" + m_soundFilePath + "\" type mpegvideo alias mp3").c_str(), 0, 0, 0);
			mciSendStringA("play mp3 from 0 repeat", 0, 0, 0);
		}
		else
		{
			PlaySoundA(m_soundFilePath.c_str(), GetModuleHandle(0), SND_LOOP | SND_ASYNC | SND_FILENAME);
		}
	}
}

void SoundOpener::stopSound()
{
	if (m_soundFilePath.size())
	{
		if (getIsSoundFileMp3())
		{
			mciSendStringA("stop mp3", 0, 0, 0);
			mciSendStringA("close mp3", 0, 0, 0);
		}
		else
		{
			PlaySoundA(0, GetModuleHandle(0), 0);
		}
	}
}

//------------------------------
// class MainScreen
//------------------------------

MainScreen::MainScreen(View* p_app) :
	View(p_app, p_app->getBounds())
{
	enableMouseEvents();

	//------------------------------

	setThemeColor(Avo::ThemeColors::primaryOnBackground, Avo::Color(1.f, 0.4f, 0.7f, 0.9f));
	setThemeColor(Avo::ThemeColors::primary, Avo::Color(1.f, 0.3f, 0.7f, 0.9f));

	setThemeColor(Avo::ThemeColors::background, Avo::Color(0.05f));
	setThemeColor(Avo::ThemeColors::onBackground, Avo::Color(0.97f));

	setThemeColor(Avo::ThemeColors::tooltipBackground, Avo::Color(0.1f));
	setThemeValue(Avo::ThemeValues::tooltipFontSize, 10.f);

	setThemeValue(Avo::ThemeValues::textFieldHeight, 2.4f);

	setThemeValue(Avo::ThemeValues::buttonFontSize, 11.f);
	setThemeValue(Avo::ThemeValues::buttonCharacterSpacing, 0.f);

	//------------------------------

	auto textFieldContainer = new Avo::View(this);
	textFieldContainer->enableMouseEvents();

	m_textField_hours = new Avo::TextField(textFieldContainer, Avo::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH);
	m_textField_hours->setSuffixString(" H");
	m_textField_hours->setTextAlign(Avo::TextAlign::Right);
	m_textField_hours->setString("1");

	m_textField_minutes = new Avo::TextField(textFieldContainer, Avo::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH + 1.5f);
	m_textField_minutes->setLeft(m_textField_hours->getRight() + TIMER_TEXT_FIELD_PADDING);
	m_textField_minutes->setSuffixString(" M");
	m_textField_minutes->setTextAlign(Avo::TextAlign::Right);
	m_textField_minutes->setString("");

	m_textField_seconds = new Avo::TextField(textFieldContainer, Avo::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH - 1.7f);
	m_textField_seconds->setLeft(m_textField_minutes->getRight() + TIMER_TEXT_FIELD_PADDING);
	m_textField_seconds->setSuffixString(" S");
	m_textField_seconds->setTextAlign(Avo::TextAlign::Right);
	m_textField_seconds->setString("");

	auto handleEditableTextChange = [this](Avo::EditableText*, std::string& p_newString, int32& p_newCaretIndex) -> bool {
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

		m_timePlayer->willRestart = true;

		return true;
	};
	m_textField_hours->getEditableText()->editableTextChangeListeners += handleEditableTextChange;
	m_textField_minutes->getEditableText()->editableTextChangeListeners += handleEditableTextChange;
	m_textField_seconds->getEditableText()->editableTextChangeListeners += handleEditableTextChange;

	textFieldContainer->setPadding(1.f);
	textFieldContainer->setCenter(getCenterX(), getCenterY());

	m_soundOpener = new SoundOpener(this);
	m_soundOpener->setCenter(getWidth() * 0.5f, textFieldContainer->getBottom() + 30.f);

	//------------------------------

	getGui()->globalKeyboardKeyDownListeners += [this](Avo::KeyboardEvent const& p_event) {
		if (p_event.key == Avo::KeyboardKey::Tab)
		{
			if (m_textField_hours->getHasKeyboardFocus())
			{
				getGui()->setKeyboardFocus(m_textField_minutes);
			}
			else if (m_textField_minutes->getHasKeyboardFocus())
			{
				getGui()->setKeyboardFocus(m_textField_seconds);
			}
			else
			{
				getGui()->setKeyboardFocus(m_textField_hours);
			}
			((Avo::EditableText*)getGui()->getKeyboardFocus())->selectAll();
		}
	};

	//------------------------------
	// Generate spiral!

	m_numberOfSpiralVerticesInTotal = TIMER_SPIRAL_RESOLUTION * TIMER_MAX_NUMBER_OF_HOURS;
	m_spiralVertices = new Avo::Point<float>[m_numberOfSpiralVerticesInTotal];
	for (uint32 a = 0; a < m_numberOfSpiralVerticesInTotal; a++)
	{
		float angle = ((float)a / TIMER_SPIRAL_RESOLUTION - 0.25f) * Avo::TAU;
		float magnitude = TIMER_SPIRAL_RADIUS * (1.f - TIMER_SPIRAL_STEEPNESS * (float)a / m_numberOfSpiralVerticesInTotal);
		m_spiralVertices[a] = Avo::Point<float>(getWidth() * 0.5f + std::cos(angle) * magnitude, getHeight() * 0.5f + std::sin(angle) * magnitude);
	}
	m_startAngle = 1.f; // One hour, 360 degrees, TAU radians
	m_currentAngle = m_startAngle;

	queueAnimationUpdate();

	//------------------------------

	m_timePlayer = new TimePlayer(this);
	m_timePlayer->setTimeLeft(m_currentAngle);
	m_timePlayer->setBottom(textFieldContainer->getTop() - 20.f);
}

void MainScreen::updateAnimations() 
{
	if (!m_hasDraggedSpiral)
	{
		auto angleBefore = m_currentAngle;
		if (m_timePlayer->willRestart)
		{
			uint32 hours = m_textField_hours->getString()[0] ? std::stoi(m_textField_hours->getString()) : 0;
			uint32 minutes = m_textField_minutes->getString()[0] ? std::stoi(m_textField_minutes->getString()) : 0;
			uint32 seconds = m_textField_seconds->getString()[0] ? std::stoi(m_textField_seconds->getString()) : 0;
			if ((double)hours + minutes / 60. + seconds / 3600. > TIMER_MAX_NUMBER_OF_HOURS)
			{
				hours = (uint32)TIMER_MAX_NUMBER_OF_HOURS;
				minutes = 0u;
				seconds = 0u;
				m_textField_hours->setValue(hours);
				m_textField_minutes->setValue(0);
				m_textField_seconds->setValue(0);
			}
			else
			{
				if (minutes > 59)
				{
					minutes = 59u;
					m_textField_minutes->setValue(59);
				}
				if (seconds > 59)
				{
					seconds = 59u;
					m_textField_seconds->setValue(59);
				}
			}

			m_timePlayer->reset();
			m_startAngle = (double)hours + minutes / 60. + seconds / 3600.;
			m_currentAngle = m_startAngle;

			angleBefore = -1;

			m_timePlayer->willRestart = false;
		}
		else if (m_timePlayer->isPlaying)
		{
			m_currentAngle = Avo::max(0., m_startAngle - (std::chrono::steady_clock::now() - m_timePlayer->getTimeStart()).count() / 3600'000'000'000.);
		}

		if (getGui()->getWindow()->getState() != Avo::WindowState::Minimized && std::ceil(angleBefore * 3600.) != std::ceil(m_currentAngle * 3600.))
		{
			m_timePlayer->setTimeLeft(m_currentAngle);
			invalidate();
		}
		if (angleBefore > 0.f && angleBefore <= 1.f / 3600.f && !m_currentAngle)
		{
			if (getGui()->getWindow()->getState() == Avo::WindowState::Minimized)
			{
				m_timePlayer->setTimeLeft(m_currentAngle);
			}
			m_soundOpener->playSound();
			getGui<TimerApp>()->showTimeoutScreen();
			return;
		}
	}
	queueAnimationUpdate();
}