#include <avo.hpp>

int main() 
{
	while (true) 
	{
		if (avo::window::get_is_keyboard_key_pressed(avo::window::KeyboardKey::Left)) 
		{
			fmt::print("Left keyboard key is pressed!\n");
		}
		if (avo::window::get_is_mouse_button_pressed(avo::window::MouseButton::Left)) 
		{
			fmt::print("Left mouse button is pressed!\n");
		}

		std::this_thread::sleep_for(0.2s);
	}
}
