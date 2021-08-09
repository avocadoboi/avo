#include <avo.hpp>

#include <magic_enum.hpp>

#include <thread>

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

	auto child = avo::window::create("Smol")
		.size(Size{200.f, 150.f})
		.with_parent(parent)
		.open();

	auto event_manager = avo::window::EventManager{};

	event_manager.add_listener([](event::KeyDown const& event) {
		fmt::print("The key '{}' was pressed. Repeat: {}\n", enum_name(event.key), event.is_repeated);
	});
	event_manager.add_listener([&](event::KeyUp const& event) {
		fmt::print("The key '{}' was released.\n", enum_name(event.key));

		if (event.key == avo::window::KeyboardKey::F) {
			if (parent.is_fullscreen()) {
				parent.min_max_size(min_max_size);
			}
			else {
				parent.min_max_size({});
			}
			fmt::print("Fullscreen: {}\n", parent.toggle_fullscreen());
		}
	});
	event_manager.add_listener([](event::CharacterInput const& event) {
		fmt::print("The character '{}' was input. Repeat: {}\n", event.character, event.is_repeated);
	});
	event_manager.add_listener([](event::MouseDown const& event) {
		fmt::print("The mouse button '{}' was {}.\n", enum_name(event.button), event.is_double_click ? "double clicked" : "pressed");
	});
	event_manager.add_listener([](event::MouseUp const& event) {
		fmt::print("The mouse button '{}' was released.\n", enum_name(event.button));
	});
	event_manager.add_listener([](event::MouseMove const& event) {
		fmt::print("The mouse moved {} and is now at {}.\n", event.movement, event.position);
	});
	event_manager.add_listener([](event::MouseScroll const& event) {
		fmt::print("The mouse scrolled with delta {}.\n", event.scroll_delta);
	});
	event_manager.add_listener([](event::MouseLeave const& event) {
		fmt::print("The mouse left the window by moving {} and is now at {}.\n", event.movement, event.position);
	});
	event_manager.add_listener([](event::SizeChange const& event) {
		fmt::print("The window resized and now has size {}.\n", event.size);
	});
	event_manager.add_listener([](event::StateChange const& event) {
		fmt::print("The window state changed and is now '{}'.\n", enum_name(event.state));
	});
	event_manager.add_listener([](event::FocusGain const&) {
		fmt::print("The window gained focus.\n");
	});
	event_manager.add_listener([](event::FocusLose const&) {
		fmt::print("The window lost focus.\n");
	});
	event_manager.add_listener([](event::DpiChange const& event) {
		fmt::print("DPI changed and is now {}.\n", event.dpi);
	});
	event_manager.run(parent);

	fmt::print("Finished running.\n");
}
