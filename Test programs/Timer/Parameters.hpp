#pragma once

#include "../../AvoGUI.hpp"

// using Hours = std::chrono::duration<double, std::ratio<3600>>;
template<typename T = double>
using Hours = std::chrono::duration<T, std::ratio<3600>>;

// using Minutes = std::chrono::duration<double, std::ratio<60>>;
template<typename T = double>
using Minutes = std::chrono::duration<T, std::ratio<60>>;

// using Seconds = std::chrono::duration<double>;
template<typename T = double>
using Seconds = std::chrono::duration<T>;

using Clock = std::chrono::steady_clock;

constexpr auto timerSpiralResolution = 360u;
constexpr auto timerSpiralSteepness = 0.4f;
constexpr auto timerSpiralPadding = 20.f;
constexpr auto timerSpiralRadius = 200.f;
constexpr auto timerTextFieldPadding = 10.f;
constexpr auto timerTextFieldWidth = 61.f;
constexpr auto timerMaxNumberOfHours = Hours<>{12.};

namespace Ids
{
	inline Avo::Id const soundOpener;
}
