#include <AvoGUI.hpp>

//------------------------------

class App : 
    public AvoGUI::Gui
{
public:
    App()
    {
        create(u8"Linux test! Unicode: åäöâñëV݉sZ㇡ه搶o7賍", 600, 500, AvoGUI::WindowStyleFlags::Default);
        waitForFinish();
    }

    void createContent() override
    {
        setThemeColor("background", AvoGUI::Color(0.3, 0.3, 0.3));
    }
};

//------------------------------

int main()
{
    new App();
}
