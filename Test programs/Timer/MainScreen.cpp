#include "MainScreen.hpp"
#include "Timer.hpp"

MainScreen::MainScreen(TimerApp* p_timerApp) :
	View(p_timerApp, p_timerApp->getBounds()),
	m_willRestart(false),
	m_spiralVertices(0), m_numberOfSpiralVerticesInTotal(0),
	m_startAngle(0.f), m_currentAngle(0.f), m_isDraggingSpiral(false), m_hasDraggedSpiral(false),
	m_text_timeLeft(0), m_button_restart(0),
	m_textField_hours(0), m_textField_minutes(0), m_textField_seconds(0),
	m_soundOpener(0)
{
	enableMouseEvents();
	getGUI()->addGlobalKeyboardListener(this);

	//------------------------------

	setThemeColor("primary on background", AvoGUI::Color(1.f, 0.4f, 0.7f, 0.9f));
	setThemeColor("primary", AvoGUI::Color(1.f, 0.3f, 0.7f, 0.9f));

	setThemeColor("background", AvoGUI::Color(0.05f));
	setThemeColor("on background", AvoGUI::Color(0.97f));

	setThemeColor("tooltip background", AvoGUI::Color(0.1f));
	setThemeValue("tooltip font size", 10.f);

	setThemeValue("text field height", 2.4f);

	setThemeValue("button font size", 11.f);
	setThemeValue("button character spacing", 0.f);

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
	m_textField_minutes->setString("");

	m_textField_seconds = new AvoGUI::TextField(textFieldContainer, AvoGUI::TextField::Type::Outlined, "", TIMER_TEXT_FIELD_WIDTH - 1.7f);
	m_textField_seconds->setLeft(m_textField_minutes->getRight() + TIMER_TEXT_FIELD_PADDING);
	m_textField_seconds->setSuffixString(" S");
	m_textField_seconds->setTextAlign(AvoGUI::TextAlign::Right);
	m_textField_seconds->setString("");

	m_textField_hours->addEditableTextListener(this);
	m_textField_minutes->addEditableTextListener(this);
	m_textField_seconds->addEditableTextListener(this);

	textFieldContainer->setPadding(1.f);
	textFieldContainer->setCenter(getCenterX(), getCenterY());

	m_soundOpener = new SoundOpener(this);
	m_soundOpener->setCenter(getWidth() * 0.5f, textFieldContainer->getBottom() + 30.f);

	//------------------------------
	// Generate spiral!

	m_numberOfSpiralVerticesInTotal = TIMER_SPIRAL_RESOLUTION * TIMER_MAX_NUMBER_OF_HOURS;
	m_spiralVertices = new AvoGUI::Point<float>[m_numberOfSpiralVerticesInTotal];
	for (uint32_t a = 0; a < m_numberOfSpiralVerticesInTotal; a++)
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

	m_button_restart = new AvoGUI::Button(this, "", AvoGUI::Button::Emphasis::Medium);
	m_button_restart->setWidth(27.f);
	m_button_restart->setHeight(27.f);
	AvoGUI::Image* icon = getGUI()->getDrawingContext()->createImage(ICON_RESTART_PATH);
	icon->setSize(18.f, 18.f);
	m_button_restart->setIcon(icon);
	m_button_restart->setCornerRadius(m_button_restart->getWidth() * 0.5f);
	m_button_restart->setLeft(m_text_timeLeft->getRight() + 5.f);
	m_button_restart->setCenterY(m_text_timeLeft->getCenterY());
	m_button_restart->setTooltip("Restart timer");
	m_button_restart->addButtonListener(this);
}

void MainScreen::updateAnimations() 
{
	if (!m_hasDraggedSpiral)
	{
		float angleBefore = m_currentAngle;
		if (m_willRestart)
		{
			uint32_t hours = m_textField_hours->getString()[0] ? std::stoi(m_textField_hours->getString()) : 0;
			uint32_t minutes = m_textField_minutes->getString()[0] ? std::stoi(m_textField_minutes->getString()) : 0;
			uint32_t seconds = m_textField_seconds->getString()[0] ? std::stoi(m_textField_seconds->getString()) : 0;
			if (hours > TIMER_MAX_NUMBER_OF_HOURS)
			{
				hours = (uint32_t)TIMER_MAX_NUMBER_OF_HOURS;
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

		if (getGUI()->getWindow()->getState() != AvoGUI::WindowState::Minimized && std::ceil(angleBefore * 3600) != std::ceil(m_currentAngle * 3600))
		{
			updateTimeLeftText();
			invalidate();
		}
		if (angleBefore > 0.f && angleBefore <= 1.f / 3600.f && !m_currentAngle)
		{
			if (getGUI()->getWindow()->getState() == AvoGUI::WindowState::Minimized)
			{
				updateTimeLeftText();
			}
			m_soundOpener->playSound();
			((TimerApp*)getGUI())->showTimeoutScreen();
			return;
		}
	}
	queueAnimationUpdate();
}