#include <avo.hpp>

#include <thread>

using namespace std::chrono_literals;

int main() 
{
	while (true) 
	{
		if (avo::window::get_is_key_down(avo::window::KeyboardKey::Left)) 
		{
			std::println("Left keyboard key is pressed!");
		}
		if (avo::window::get_is_mouse_button_down(avo::window::MouseButton::Left)) 
		{
			std::println("Left mouse button is pressed!");
		}

		std::this_thread::sleep_for(0.2s);
	}
}
