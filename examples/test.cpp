#include <AvoGUI.hpp>

int main() {
    using namespace avo::math;
    
    auto window = avo::window("Hello AvoGUI!")
        .position(Vector2d{0.5f, 0.5f})
        .size(Size{500.f, 400.f})
        .open();

    auto update = [&, i = 0]() mutable {
        window.title(fmt::format("{}. Size: {}.", i, window.size()));
        ++i;
    };

    update();
    
    window.size(Size{300.f, 300.f});
    window.position(Point{300, 200});

    update();
    
    while (window.is_open()) {
        std::this_thread::sleep_for(1s);
        update();
    }
}
