#include <avo.hpp>

#include <magic_enum.hpp>

#include <thread>

using namespace std::chrono_literals;

int main() {
	using namespace avo::math;

	namespace event = avo::window::event;

	auto window = avo::window::create("Hello AvoGUI!")
		.position(Vector2d{0.5f, 0.5f})
		.size(Size{500.f, 400.f})
		.min_max_size({Size{200.f, 300.f}, Size{700.f, 500.f}})
		.open();

	auto update = [&, i = 0]() mutable {
		window.title(fmt::format("{}. Size: {}.", i++, window.size()));
	};

	update();
	
	window.size(Size{300.f, 300.f});
	window.position(Point{300, 200});

	update();

	auto event_manager = avo::window::EventManager{window};
	event_manager.add_listener([](event::KeyDown const& event) {
		fmt::print("The key '{}' was pressed.\n", event.key);
	});
	event_manager.add_listener([](event::KeyUp const& event) {
		fmt::print("The key '{}' was released.\n", event.key);
	});
	event_manager.add_listener([](event::MouseMove const& event) {
		fmt::print("The mouse moved {} and is now at {}.\n", event.movement, event.position);
	});
	event_manager.add_listener([&](event::SizeChange const& event) {
		update();
		fmt::print("The window resized and now has size {}.\n", event.size);
	});
	event_manager.add_listener([](event::StateChange const& event) {
		fmt::print("The window state changed and is now '{}'.\n", magic_enum::enum_name(event.state));
	});
	event_manager.run();
	fmt::print("Finished running.\n");
}
