#include <AvoGUI.hpp>
#include <fmt/printf.h>

int main() {
    using namespace avo::math;
    
    auto window = avo::window("Hello AvoGUI!")
        .position(Vector2d{0.5f, 0.5f})
        .size(Size{500.f, 400.f})
        .open();
    window.position({100, 200});
    fmt::print("{}\n", window.position());
    std::this_thread::sleep_for(0.1s);
    fmt::print("{}\n", window.position());
}
