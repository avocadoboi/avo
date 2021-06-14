#include <avo.hpp>

#include <thread>

using namespace std::chrono_literals;

int main() 
{
	while (true) 
	{
		using namespace avo::window;

		if (get_is_key_down(KeyboardKey::Left)) 
		{
			fmt::print("Left keyboard key is pressed!\n");
		}
		if (get_is_mouse_button_down(MouseButton::Left)) 
		{
			fmt::print("Left mouse button is pressed!\n");
		}

		std::this_thread::sleep_for(0.2s);
	}
}
