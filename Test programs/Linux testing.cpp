#include <AvoGUI.hpp>

int main()
{
    AvoGUI::Gui* gui = new AvoGUI::Gui();
    gui->create(u8"Linux test! Unicode: åäöâñëﻄ൷ـUV݉sZ㇡ه搶o7賍", 600, 500, AvoGUI::WindowStyleFlags::Default);
    gui->waitForFinish();
}
