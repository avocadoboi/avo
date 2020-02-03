#include <AvoGUI.hpp>

//------------------------------

class App : 
    public AvoGUI::Gui
{
private:
    AvoGUI::Color m_color;
    float m_hue;
    
public:
    App() :
        m_hue(0.f)
    {
        create(u8"Linux test! Unicode: åäöâñëV݉sZ㇡ه搶o7賍", 600, 500, AvoGUI::WindowStyleFlags::Default);
        waitForFinish();
    }

    void createContent() override
    {
        setThemeColor("background", AvoGUI::Color(1.f, 0.f, 0.4f));
        queueAnimationUpdate();
    }
    void updateAnimations() override
    {
        // setThemeColor("background", m_color.setHSB(m_hue += 0.01f, 1.f, 1.f));

        invalidate();
        queueAnimationUpdate();
    }
    
    void draw(AvoGUI::DrawingContext* p_context) override
    {
        p_context->setColor(AvoGUI::Color(1.f, 1.f, 1.f));
        p_context->fillRectangle(50.f, 50.f, 200.f, 200.f);
    }
};

//------------------------------

int main()
{
    new App();
}
