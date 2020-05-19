#include "IconButton.hpp"
#include "Timer.hpp"

void IconButton::handleMouseEnter(Avo::MouseEvent const&)
{
	getGui<TimerApp>()->getTooltip()->show(m_tooltipString, getAbsoluteBounds());
}
void IconButton::handleMouseLeave(Avo::MouseEvent const&)
{
	getGui<TimerApp>()->getTooltip()->hide();
}