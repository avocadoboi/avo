#include <avo.hpp>

#include <thread>

using namespace std::chrono_literals;

int main() 
{
	while (true) 
	{
		if (avo::window::get_is_key_down(avo::window::KeyboardKey::Left)) 
		{
			fmt::print("Left keyboard key is pressed!\n");
		}
		if (avo::window::get_is_mouse_button_down(avo::window::MouseButton::Left)) 
		{
			fmt::print("Left mouse button is pressed!\n");
		}

		std::this_thread::sleep_for(0.2s);
	}
}
