#include "IconButton.hpp"
#include "Timer.hpp"

void IconButton::handleMouseEnter(AvoGUI::MouseEvent const&)
{
	getGui<TimerApp>()->getTooltip()->show(m_tooltipString, getAbsoluteBounds());
}
void IconButton::handleMouseLeave(AvoGUI::MouseEvent const&)
{
	getGui<TimerApp>()->getTooltip()->hide();
}