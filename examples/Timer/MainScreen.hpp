#pragma once

#include "Parameters.hpp"
#include "IconButton.hpp"

#include "../../AvoGUI.hpp"
#include <fstream>
#include <filesystem>

//------------------------------

class SoundOpener : public Avo::View
{
private:
	auto updateCurrentSoundFileNameText() -> void
	{
		if (m_soundFilePath.empty())
		{
			m_text_currentSoundFileName = getDrawingContext()->createText("(none)", 12.f);
		}
		else
		{
			auto filename = m_soundFilePath.filename().u8string();
			if (Avo::getNumberOfCharactersInString(std::string_view{filename}) > 13)
			{
				filename = filename.substr(0, Avo::getUnitIndexFromCharacterIndex(std::string_view{filename}, 10)) + "...";
			}
			m_text_currentSoundFileName = getDrawingContext()->createText(filename, 12.f);
		}
		m_text_currentSoundFileName.setLeft(m_text_sound.getRight() + 2.f);
		m_text_currentSoundFileName.setFontWeight(Avo::FontWeight::Regular);
		m_text_currentSoundFileName.setCenterY(m_button_open->getCenterY() - 0.5f);
		m_button_open->setLeft(m_text_currentSoundFileName.getRight() + 8.f);
		setSize(Avo::Size{m_button_open->getBottomRight()});
	}	
	auto getIsSoundFileMp3() -> bool
	{
		// return m_soundFilePath.substr(m_soundFilePath.size() - 3, 3) == "mp3";
		return m_soundFilePath.extension() == ".mp3";
	}

public:
	auto playSound() -> void;
	auto stopSound() -> void;

	//------------------------------

private:
	Avo::Text m_text_sound = getDrawingContext()->createText("Sound: ", 12.f);
	Avo::Text m_text_currentSoundFileName = getDrawingContext()->createText("(none)", 12.f);
public:
	auto draw(Avo::DrawingContext* const p_drawingContext) -> void override
	{
		p_drawingContext->setColor(0.9f);
		p_drawingContext->drawText(m_text_sound);
		p_drawingContext->setColor(0.8f);
		p_drawingContext->drawText(m_text_currentSoundFileName);
	}

	//------------------------------

private:
	// Avo::Button* m_button_open = addView<Avo::Button>("OPEN", Avo::Button::Emphasis::Medium);
	Avo::Button* m_button_open = new Avo::Button{this, "OPEN", Avo::Button::Emphasis::Medium};
	auto createLayout() -> void
	{
		m_text_currentSoundFileName.setLeft(m_text_sound.getRight() + 2.f);
		m_text_currentSoundFileName.setFontWeight(Avo::FontWeight::Regular);

		m_button_open->setLeft(m_text_currentSoundFileName.getRight() + 8.f);

		m_text_sound.setCenterY(m_button_open->getCenterY() - 0.5f);

		sizeChangeListeners += [this](auto){
			setCenterX(getParent<View>()->getWidth() * 0.5f);
		};
	}

	//------------------------------

	std::filesystem::path m_soundFilePath;
	auto loadSavedFile() -> void
	{
		if (auto file = std::ifstream{"soundpath", std::ios::binary | std::ios::ate})
		{
			auto pathString = std::wstring(file.tellg()/sizeof(wchar_t), L'\0');
			file.seekg(0);
			file.read(reinterpret_cast<char*>(pathString.data()), pathString.size()*sizeof(wchar_t));
			m_soundFilePath = std::move(pathString);
		}
		updateCurrentSoundFileNameText();
	}

	//------------------------------

	Avo::OpenFileDialog m_openFileDialog;
	auto initializeSoundOpen() -> void
	{
		m_openFileDialog.setFileExtensions({{"Audio files", "*.mp3;*.wav"}});
		m_button_open->buttonClickListeners += [this](auto) {
			if (auto const filenames = m_openFileDialog.open(); !filenames.empty())
			{
				m_soundFilePath = std::filesystem::u8path(filenames[0]);
				auto pathString = m_soundFilePath.wstring();

				auto file = std::ofstream{"soundpath", std::ios::binary};
				file.write(reinterpret_cast<char const*>(pathString.data()), pathString.size()*sizeof(wchar_t));

				updateCurrentSoundFileNameText();
				invalidate();
			}
		};
	}

public:
	SoundOpener(Avo::View* const p_parent) :
		View{p_parent, Ids::soundOpener}
	{
		enableMouseEvents();
		createLayout();
		initializeSoundOpen();
		loadSavedFile();
	}
};

struct ClockTime
{
	Hours<int> hours{};
	Minutes<int> minutes{};
	Seconds<int> seconds{};

	auto operator=(Hours<> p_hours) noexcept -> ClockTime&
	{
		auto totalSeconds = std::chrono::ceil<Seconds<int>>(p_hours);
		hours = std::chrono::floor<Hours<int>>(totalSeconds);
		minutes = std::chrono::floor<Minutes<int>>(totalSeconds - hours);
		seconds = std::chrono::floor<Seconds<int>>(totalSeconds - hours - minutes);
		
		return *this;
	}
	template<typename DurationType>
	auto getTotal() noexcept -> DurationType
	{
		return DurationType{hours + minutes + seconds};
	}
	operator bool() noexcept
	{
		return hours.count() || minutes.count() || seconds.count();
	}
	
	ClockTime() = default;
	ClockTime(Hours<> p_hours) noexcept
	{
		operator=(p_hours);
	}
};

class TimePlayer : public Avo::View
{
public:
	bool willRestart = false;
	bool isPlaying = false;

private:
	Avo::TextView* m_text_timeLeft = new Avo::TextView{this, 23.f};
public:
	auto setTimeLeft(ClockTime p_timeLeft) -> void
	{
		if (p_timeLeft)
		{
			// auto totalSeconds = std::ceil(Seconds<>{p_angle});
			// auto hours = Hours<int>{totalSeconds};
			// auto minutes = Minutes<int>{totalSeconds - hours};
			// auto seconds = Seconds<int>{totalSeconds - hours - minutes};
			// auto const timeLeft = splitHours(p_hours);
			m_text_timeLeft->setString(
				(p_timeLeft.hours.count() < 10 ? "0" : "") + Avo::numberToString(p_timeLeft.hours.count())
			  + (p_timeLeft.minutes.count() < 10 ? ":0" : ":") + Avo::numberToString(p_timeLeft.minutes.count()) 
			  + (p_timeLeft.seconds.count() < 10u ? ":0" : ":") + Avo::numberToString(p_timeLeft.seconds.count())
			);
		}
		else
		{
			m_text_timeLeft->setString("00:00:00");
		}

		m_text_timeLeft->getText().setFontWeight(Avo::FontWeight::Regular);
		m_text_timeLeft->fitSizeToText();
		m_text_timeLeft->setCenterY(getHeight() * 0.5f);

		m_button_playPause->setLeft(m_text_timeLeft->getRight() + 10.f);
		m_button_restart->setLeft(m_button_playPause->getRight() + 5.f);
		setWidth(m_button_restart->getRight());
		setCenterX(getParent<View>()->getCenterX());
	}

private:
	std::chrono::time_point<Clock> m_timeStart = Clock::now();
	std::chrono::time_point<Clock> m_lastPauseTime = Clock::now();
public:
	auto reset() -> void
	{
		m_timeStart = Clock::now();
		m_lastPauseTime = m_timeStart;
	}
	auto getTimeStart() const
	{
		return m_timeStart;
	}

	//------------------------------
private:
	static constexpr auto iconButtonSize = 14.f;

	IconButton* m_button_restart{};
	auto createRestartButton() -> void
	{
		m_button_restart = new IconButton{this, iconButtonSize};
		m_button_restart->setIcon(MaterialIcons::REPLAY);
		m_button_restart->setCenterY(getHeight()*0.5f);
		m_button_restart->setTooltip("Restart timer");
		m_button_restart->buttonClickListeners += [this]{ willRestart = true; };
	}

	//------------------------------

	IconButton* m_button_playPause{};
	auto createPlayPauseButton() -> void
	{
		m_button_playPause = new IconButton{this, iconButtonSize};
		m_button_playPause->setIcon(MaterialIcons::PLAY_ARROW);
		m_button_playPause->setCenterY(getHeight()*0.5f);
		m_button_playPause->setTooltip("Play timer");
		m_button_playPause->buttonClickListeners += [&]() {
			if (isPlaying = !isPlaying)
			{
				m_button_playPause->setIcon(MaterialIcons::PAUSE);
				m_timeStart += Clock::now() - m_lastPauseTime;
			}
			else
			{
				m_button_playPause->setIcon(MaterialIcons::PLAY_ARROW);
				m_lastPauseTime = Clock::now();
			}
		};
		getGui()->globalKeyboardKeyUpListeners += [this](Avo::KeyboardEvent const& p_event) { 
			if (p_event.key == Avo::KeyboardKey::Spacebar)
			{
				m_button_playPause->buttonClickListeners(); 
			}
		};
	}

public:
	TimePlayer(Avo::View* const p_parent) :
		View{p_parent}
	{
		enableMouseEvents();
		setHeight(35.f);

		createPlayPauseButton();
		createRestartButton();
	}
};

//------------------------------

class TimerApp;

class MainScreen : public Avo::View
{
private:
	std::array<
		Avo::Point<float>, 
		static_cast<std::size_t>(timerSpiralResolution*timerMaxNumberOfHours.count())
	> m_spiralVertices;

	auto createSpiral() -> void
	{
		for (auto const a : Avo::Indices{m_spiralVertices})
		{
			auto const angle = Avo::Radians{(static_cast<float>(a)/timerSpiralResolution - 0.25f)*Avo::TAU<float>};
			auto const magnitude = timerSpiralRadius*(1.f - timerSpiralSteepness*a/m_spiralVertices.size());
			m_spiralVertices[a] = Avo::Point{getSize()/2 + Avo::Vector2d<>::fromPolar(angle, magnitude)};
		}
		m_startTime = Hours<>{1.};
		m_timeLeft = m_startTime;
	}

	ClockTime m_startTime{};
	ClockTime m_timeLeft{};
	bool m_isDraggingSpiral{};
	bool m_hasDraggedSpiral{};

	Avo::TextField* m_textField_hours{};
	Avo::TextField* m_textField_minutes{};
	Avo::TextField* m_textField_seconds{};

	TimePlayer* m_timePlayer{};
	SoundOpener* m_soundOpener{};

public:
	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		getGui()->setKeyboardFocus(0);
		if ((p_event.xy - getSize()/2).getLengthSquared() >
		    (m_spiralVertices.back() - getSize()/2).getLengthSquared())
		{
			m_isDraggingSpiral = true;
			m_hasDraggedSpiral = false;
		}
	}
	auto handleMouseUp(Avo::MouseEvent const& p_event) -> void override
	{
		if (m_hasDraggedSpiral)
		{
			m_hasDraggedSpiral = false;
			m_timePlayer->willRestart = true;
		}
		m_isDraggingSpiral = false;
	}
	auto handleMouseMove(Avo::MouseEvent const& p_event) -> void override
	{
		if (m_isDraggingSpiral && p_event.xy != getSize()/2)
		{
			m_hasDraggedSpiral = true;

			auto normalizedHoursLeft = Hours<>{m_timeLeft.minutes + m_timeLeft.seconds};
			// normalizedHoursLeft -= std::chrono::floor<Hours<int>>(normalizedHoursLeft);

			auto normalizedCursorHours = Hours<>{
				std::atan2(p_event.y - getHeight()/2, p_event.x - getWidth()/2) / Avo::TAU<double> + 0.25
			};
			normalizedCursorHours -= std::chrono::floor<Hours<int>>(normalizedCursorHours);

			auto nextHours = m_timeLeft.getTotal<Hours<>>();
			if (normalizedHoursLeft > normalizedCursorHours)
			{
				if (normalizedHoursLeft - normalizedCursorHours < Hours<>{0.5})
				{
					nextHours += normalizedCursorHours - normalizedHoursLeft;
				}
				else
				{
					nextHours += Hours<>{1.} - (normalizedHoursLeft - normalizedCursorHours);
				}
			}
			else
			{
				if (normalizedCursorHours - normalizedHoursLeft < Hours<>{0.5})
				{
					nextHours += normalizedCursorHours - normalizedHoursLeft;
				}
				else
				{
					nextHours -= Hours<>{1.} - (normalizedCursorHours - normalizedHoursLeft);
				}
			}
			nextHours = Avo::constrain(nextHours, Avo::Range{Hours<>{0.}, timerMaxNumberOfHours});

			auto const newTime = ClockTime{nextHours};
			m_textField_hours->setValue(newTime.hours.count());
			m_textField_minutes->setValue(newTime.minutes.count());
			m_textField_seconds->setValue(newTime.seconds.count());

			m_startTime = newTime;
			m_timeLeft = newTime;

			invalidate();
		}
	}

	//------------------------------

	auto updateAnimations() -> void override;

	auto draw(Avo::DrawingContext* const p_context, Avo::Rectangle<> const p_targetRectangle) -> void override
	{
		p_context->clear(getThemeColor(Avo::ThemeColors::background));
		if (p_targetRectangle == getBounds())
		{
			auto const hoursLeft = m_timeLeft.getTotal<Hours<>>().count();
			
			p_context->setColor({1.f, 0.5f, 0.7f, 0.9f});
			p_context->strokeShape({m_spiralVertices.data(), static_cast<std::size_t>(round(hoursLeft * timerSpiralResolution))}, 3.f);

			auto const angle = Avo::Radians{(hoursLeft - 0.25f) * Avo::TAU<float>};
			auto const magnitude = timerSpiralRadius * (1.f - timerSpiralSteepness * hoursLeft / timerMaxNumberOfHours.count());
			auto const point = Avo::Point{getSize()/2 + Avo::Vector2d<>::fromPolar(angle, magnitude)};
			p_context->fillCircle(point, 5);
			p_context->setColor(0.f);
			p_context->strokeCircle(point, 5, 2.f);
		}
	}

	//------------------------------

	MainScreen(View* p_app);
};
