#include <AvoGUI.hpp>

int main() {
    using namespace avo::math;
    
    auto const window = avo::window("Hello AvoGUI!")
        .position(Vector2d{0.5f, 0.5f})
        .size(Size{500.f, 400.f})
        .open();
}
