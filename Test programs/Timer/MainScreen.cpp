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

	setThemeColor(AvoGUI::ThemeColors::primaryOnBackground, AvoGUI::Color(1.f, 0.4f, 0.7f, 0.9f));
	setThemeColor(AvoGUI::ThemeColors::primary, AvoGUI::Color(1.f, 0.3f, 0.7f, 0.9f));

	setThemeColor(AvoGUI::ThemeColors::background, AvoGUI::Color(0.05f));
	setThemeColor(AvoGUI::ThemeColors::onBackground, AvoGUI::Color(0.97f));

	setThemeColor(AvoGUI::ThemeColors::tooltipBackground, AvoGUI::Color(0.1f));
	setThemeValue(AvoGUI::ThemeValues::tooltipFontSize, 10.f);

	setThemeValue(AvoGUI::ThemeValues::textFieldHeight, 2.4f);

	setThemeValue(AvoGUI::ThemeValues::buttonFontSize, 11.f);
	setThemeValue(AvoGUI::ThemeValues::buttonCharacterSpacing, 0.f);

	//------------------------------

	auto textFieldContainer = new AvoGUI::View(this);
	textFieldContainer->enableMouseEvents();

	m_textField_hours = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH);
	m_textField_hours->setSuffixString(" H");
	m_textField_hours->setTextAlign(AvoGUI::TextAlign::Right);
	m_textField_hours->setString("1");

	m_textField_minutes = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH + 1.5f);
	m_textField_minutes->setLeft(m_textField_hours->getRight() + TIMER_TEXT_FIELD_PADDING);
	m_textField_minutes->setSuffixString(" M");
	m_textField_minutes->setTextAlign(AvoGUI::TextAlign::Right);
	m_textField_minutes->setString("");

	m_textField_seconds = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH - 1.7f);
	m_textField_seconds->setLeft(m_textField_minutes->getRight() + TIMER_TEXT_FIELD_PADDING);
	m_textField_seconds->setSuffixString(" S");
	m_textField_seconds->setTextAlign(AvoGUI::TextAlign::Right);
	m_textField_seconds->setString("");

	auto handleEditableTextChange = [this](AvoGUI::EditableText*, std::string& p_newString, int32& p_newCaretIndex) -> bool {
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
	};
	m_textField_hours->addEditableTextChangeListener(handleEditableTextChange);
	m_textField_minutes->addEditableTextChangeListener(handleEditableTextChange);
	m_textField_seconds->addEditableTextChangeListener(handleEditableTextChange);

	textFieldContainer->setPadding(1.f);
	textFieldContainer->setCenter(getCenterX(), getCenterY());

	m_soundOpener = new SoundOpener(this);
	m_soundOpener->setCenter(getWidth() * 0.5f, textFieldContainer->getBottom() + 30.f);

	//------------------------------

	getGui()->globalKeyboardKeyDownListeners += [this](AvoGUI::KeyboardEvent const& p_event) {
		if (p_event.key == AvoGUI::KeyboardKey::Tab)
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
			((AvoGUI::EditableText*)getGui()->getKeyboardFocus())->selectAll();
		}
	};

	//------------------------------
	// Generate spiral!

	m_numberOfSpiralVerticesInTotal = TIMER_SPIRAL_RESOLUTION * TIMER_MAX_NUMBER_OF_HOURS;
	m_spiralVertices = new AvoGUI::Point<float>[m_numberOfSpiralVerticesInTotal];
	for (uint32 a = 0; a < m_numberOfSpiralVerticesInTotal; a++)
	{
		float angle = ((float)a / TIMER_SPIRAL_RESOLUTION - 0.25f) * AvoGUI::TAU;
		float magnitude = TIMER_SPIRAL_RADIUS * (1.f - TIMER_SPIRAL_STEEPNESS * (float)a / m_numberOfSpiralVerticesInTotal);
		m_spiralVertices[a] = AvoGUI::Point<float>(getWidth() * 0.5f + std::cos(angle) * magnitude, getHeight() * 0.5f + std::sin(angle) * magnitude);
	}
	m_startAngle = 1.f; // One hour, 360 degrees, TAU radians
	m_timeStart = std::chrono::steady_clock::now();

	queueAnimationUpdate();

	//------------------------------

	updateTimeLeftText();

	auto button_restart = new AvoGUI::Button(this, MaterialIcons::REPLAY, AvoGUI::Button::Emphasis::Medium);
	button_restart->getText()->setFontFamily(AvoGUI::FONT_FAMILY_MATERIAL_ICONS);
	button_restart->getText()->setFontSize(18.f);
	button_restart->getText()->setIsTopTrimmed(true);
	button_restart->getText()->setSize(17.5f);
	button_restart->getText()->setCenter(27.f * 0.5f);
	button_restart->setWidth(27.f);
	button_restart->setHeight(27.f);
	button_restart->setCornerRadius(button_restart->getWidth() * 0.5f);
	button_restart->setLeft(m_text_timeLeft->getRight() + 5.f);
	button_restart->setCenterY(m_text_timeLeft->getCenterY());
	button_restart->setTooltip(getGui<TimerApp>()->getTooltip(), "Restart timer");
	button_restart->buttonClickListeners += [this](auto) { m_willRestart = true; };
}

void MainScreen::updateAnimations() 
{
	if (!m_hasDraggedSpiral)
	{
		float angleBefore = m_currentAngle;
		if (m_willRestart)
		{
			uint32 hours = m_textField_hours->getString()[0] ? std::stoi(m_textField_hours->getString()) : 0;
			uint32 minutes = m_textField_minutes->getString()[0] ? std::stoi(m_textField_minutes->getString()) : 0;
			uint32 seconds = m_textField_seconds->getString()[0] ? std::stoi(m_textField_seconds->getString()) : 0;
			if ((float)hours + minutes / 60.f + seconds / 3600.f > TIMER_MAX_NUMBER_OF_HOURS)
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

			m_timeStart = std::chrono::steady_clock::now();
			m_startAngle = (float)hours + minutes / 60.f + seconds / 3600.f;
			m_currentAngle = m_startAngle;

			angleBefore = -1;

			m_willRestart = false;
		}
		else
		{
			m_currentAngle = AvoGUI::max(0.f, m_startAngle - std::chrono::duration<float>(std::chrono::steady_clock::now() - m_timeStart).count() / 3600.f);
		}

		if (getGui()->getWindow()->getState() != AvoGUI::WindowState::Minimized && std::ceil(angleBefore * 3600) != std::ceil(m_currentAngle * 3600))
		{
			updateTimeLeftText();
			invalidate();
		}
		if (angleBefore > 0.f && angleBefore <= 1.f / 3600.f && !m_currentAngle)
		{
			if (getGui()->getWindow()->getState() == AvoGUI::WindowState::Minimized)
			{
				updateTimeLeftText();
			}
			m_soundOpener->playSound();
			getGui<TimerApp>()->showTimeoutScreen();
			return;
		}
	}
	queueAnimationUpdate();
}