#include "MainScreen.hpp"
#include "Timer.hpp"

#include <charconv>

#include <Windows.h>
#pragma comment(lib, "winmm")
#undef min
#undef max

//------------------------------
// class SoundOpener
//------------------------------

auto SoundOpener::playSound() -> void
{
	if (!m_soundFilePath.empty())
	{
		if (getIsSoundFileMp3())
		{
			mciSendStringW((std::wstring{L"open \""} + m_soundFilePath.c_str() + L"\" type mpegvideo alias mp3").c_str(), 0, 0, 0);
			mciSendStringW(L"play mp3 from 0 repeat", nullptr, 0u, nullptr);
		}
		else
		{
			PlaySoundW(m_soundFilePath.c_str(), GetModuleHandle(0), SND_LOOP | SND_ASYNC | SND_FILENAME);
		}
	}
}

auto SoundOpener::stopSound() -> void
{
	if (!m_soundFilePath.empty())
	{
		if (getIsSoundFileMp3())
		{
			mciSendStringW(L"stop mp3", nullptr, 0, nullptr);
			mciSendStringW(L"close mp3", nullptr, 0, nullptr);
		}
		else
		{
			PlaySoundW(nullptr, GetModuleHandle(nullptr), 0);
		}
	}
}

//------------------------------
// class MainScreen
//------------------------------

auto MainScreen::updateAnimations() -> void
{
	if (!m_hasDraggedSpiral)
	{
		auto timeLeftBefore = m_timeLeft;
		if (m_timePlayer->willRestart)
		{
			m_startTime.hours = Hours<int>{m_textField_hours->getValue<int>().value_or(0)};
			m_startTime.minutes = Minutes<int>{m_textField_minutes->getValue<int>().value_or(0)};
			m_startTime.seconds = Seconds<int>{m_textField_seconds->getValue<int>().value_or(0)};
			
			if (m_startTime.getTotal<Hours<>>() > timerMaxNumberOfHours)
			{
				m_startTime.hours = std::chrono::duration_cast<Hours<int>>(timerMaxNumberOfHours);
				m_startTime.minutes = {};
				m_startTime.seconds = {};
				m_textField_hours->setValue(m_startTime.hours.count());
				m_textField_minutes->setValue(0);
				m_textField_seconds->setValue(0);
			}
			else
			{
				if (m_startTime.minutes > Minutes<int>{59})
				{
					m_startTime.minutes = Minutes<int>{59};
					m_textField_minutes->setValue(59);
				}
				if (m_startTime.seconds > Seconds<int>{59})
				{
					m_startTime.seconds = Seconds<int>{59};
					m_textField_seconds->setValue(59);
				}
			}
			m_timeLeft = m_startTime;

			m_timePlayer->reset();

			timeLeftBefore = Hours<>{-1.};

			m_timePlayer->willRestart = false;
		}
		else if (m_timePlayer->isPlaying)
		{
			m_timeLeft = Avo::max(
				Hours<>{}, 
				Hours<>{m_startTime.getTotal<Hours<>>() - (Clock::now() - m_timePlayer->getTimeStart())}
			);
		}

		if (getGui()->getWindow()->getState() != Avo::WindowState::Minimized && 
		    timeLeftBefore.getTotal<Seconds<int>>() != m_timeLeft.getTotal<Seconds<int>>())
		{
			m_timePlayer->setTimeLeft(m_timeLeft);
			invalidate();
		}
		
		if (auto sumBefore = timeLeftBefore.getTotal<Seconds<>>();
		    sumBefore.count() > 0 && sumBefore <= Seconds<>{1.} && 
		    m_timeLeft.getTotal<Seconds<>>().count() == 0)
		{
			if (getGui()->getWindow()->getState() == Avo::WindowState::Minimized)
			{
				m_timePlayer->setTimeLeft(m_timeLeft);
			}
			m_soundOpener->playSound();
			getGui<TimerApp>()->showTimeoutScreen();
			return;
		}
	}
	queueAnimationUpdate();
}

MainScreen::MainScreen(View* const p_app) :
	View{p_app, p_app->getBounds()}
{
	enableMouseEvents();

	//------------------------------

	using namespace Avo::ThemeColors;
	setThemeColors({
		{primaryOnBackground, Avo::Color{1.f, 0.4f, 0.7f, 0.9f}},
		{primary, Avo::Color{1.f, 0.3f, 0.7f, 0.9f}},
		{background, Avo::Color{0.05f}},
		{onBackground, Avo::Color{0.97f}},
		{tooltipBackground, Avo::Color{0.1f}}
	});

	using namespace Avo::ThemeValues;
	setThemeValues({
		{tooltipFontSize, 10.f},
		{textFieldHeight, 2.4f},
		{buttonFontSize, 11.f},
		{buttonCharacterSpacing, 0.f}
	});

	//------------------------------

	auto* const textFieldContainer = new Avo::View(this);
	textFieldContainer->enableMouseEvents();

	m_textField_hours = new Avo::TextField{textFieldContainer, Avo::TextField::Type::Outlined, "", timerTextFieldWidth};
	m_textField_hours->setSuffixString(" H");
	m_textField_hours->setTextAlign(Avo::TextAlign::Right);
	m_textField_hours->setString("1");

	m_textField_minutes = new Avo::TextField{textFieldContainer, Avo::TextField::Type::Outlined, "", timerTextFieldWidth + 1.5f};
	m_textField_minutes->setLeft(m_textField_hours->getRight() + timerTextFieldPadding);
	m_textField_minutes->setSuffixString(" M");
	m_textField_minutes->setTextAlign(Avo::TextAlign::Right);
	m_textField_minutes->setString("");

	m_textField_seconds = new Avo::TextField{textFieldContainer, Avo::TextField::Type::Outlined, "", timerTextFieldWidth - 1.7f};
	m_textField_seconds->setLeft(m_textField_minutes->getRight() + timerTextFieldPadding);
	m_textField_seconds->setSuffixString(" S");
	m_textField_seconds->setTextAlign(Avo::TextAlign::Right);
	m_textField_seconds->setString("");

	auto const handleEditableTextChange = 
	[this](Avo::EditableText*, std::string& p_newString, Avo::Index& p_newCaretIndex) -> bool {
		if (!std::all_of(p_newString.begin(), p_newString.end(), [](char c){ return c >= '0' && c <= '9'; }))
		{
			return false;
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
	textFieldContainer->setCenter(getCenter());

	m_soundOpener = new SoundOpener{this};
	m_soundOpener->setCenter({getWidth() * 0.5f, textFieldContainer->getBottom() + 30.f});

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
			getGui()->getKeyboardFocus<Avo::EditableText>()->selectAll();
		}
	};

	//------------------------------

	createSpiral();

	queueAnimationUpdate();

	m_timePlayer = new TimePlayer{this};
	m_timePlayer->setTimeLeft(m_timeLeft);
	m_timePlayer->setBottom(textFieldContainer->getTop() - 20.f);
}
