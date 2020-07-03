#include "IconButton.hpp"
#include "Timer.hpp"

auto IconButton::handleMouseEnter(Avo::MouseEvent const&) -> void
{
	getGui<TimerApp>()->getTooltip()->show(m_tooltipString, getAbsoluteBounds());
}
auto IconButton::handleMouseLeave(Avo::MouseEvent const&) -> void
{
	getGui<TimerApp>()->getTooltip()->hide();
}