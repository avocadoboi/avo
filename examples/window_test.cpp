#include <avo.hpp>

#include <magic_enum.hpp>

#include <chrono>
#include <print>

using namespace std::chrono_literals;
using magic_enum::enum_name;
namespace event = avo::window::event;

int main() {
	using namespace avo::math;
	using enum avo::window::StyleFlags;

	auto const min_max_size = avo::util::MinMax{Size{150.f, 150.f}, Size{700.f, 500.f}};

	auto parent = avo::window::create("BIG G")
		.size(Size{500.f, 400.f})
		.min_max_size(min_max_size)
		.style(CloseButton | MinimizeButton | Resizable)
		.open();

	// auto child = avo::window::create("Smol")
	// 	.size(Size{200.f, 150.f})
	// 	.with_parent(parent)
	// 	.open();

	auto event_manager = avo::window::EventManager{};

	event_manager.add_listener([](event::KeyDown const& event) {
		std::println("The key '{}' was pressed. Repeat: {}", enum_name(event.key), event.is_repeated);
	});
	event_manager.add_listener([&](event::KeyUp const& event) {
		std::println("The key '{}' was released.", enum_name(event.key));

		if (event.key == avo::window::KeyboardKey::F) {
			if (parent.is_fullscreen()) {
				parent.min_max_size(min_max_size);
			}
			else {
				parent.min_max_size({});
			}
			std::println("Fullscreen: {}", parent.toggle_fullscreen());
		}
	});
	event_manager.add_listener([](event::CharacterInput const& event) {
		std::println("The character '{}' was input. Repeat: {}", event.character, event.is_repeated);
	});
	event_manager.add_listener([](event::MouseDown const& event) {
		std::println("The mouse button '{}' was {}.", enum_name(event.button), event.is_double_click ? "double clicked" : "pressed");
	});
	event_manager.add_listener([](event::MouseUp const& event) {
		std::println("The mouse button '{}' was released.", enum_name(event.button));
	});
	event_manager.add_listener([](event::MouseMove const& event) {
		std::println("The mouse moved {} and is now at {}.", event.movement, event.position);
	});
	event_manager.add_listener([](event::MouseScroll const& event) {
		std::println("The mouse scrolled with delta {}.", event.scroll_delta);
	});
	event_manager.add_listener([](event::MouseLeave const& event) {
		std::println("The mouse left the window by moving {} and is now at {}.", event.movement, event.position);
	});
	event_manager.add_listener([](event::SizeChange const& event) {
		std::println("The window resized and now has size {}.", event.size);
	});
	event_manager.add_listener([](event::StateChange const& event) {
		std::println("The window state changed and is now '{}'.", enum_name(event.state));
	});
	event_manager.add_listener([](event::FocusGain const&) {
		std::println("The window gained focus.");
	});
	event_manager.add_listener([](event::FocusLose const&) {
		std::println("The window lost focus.");
	});
	event_manager.add_listener([](event::DpiChange const& event) {
		std::println("DPI changed and is now {}.", event.dpi);
	});
	event_manager.run(parent);

	std::println("Finished running.");
}
