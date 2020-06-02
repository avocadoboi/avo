#include "../AvoGUI.hpp"

/*
	Usage of Console namespace.
*/
void example_console()
{
	Console::println(u8"Write something! Maybe unicode Åå Ää Öö Ññ Üü α δ λ μ π τ")
	        .println("You wrote: \"", Console::read<std::string>(), "\".");

	Console::io << "\nNow write a number between 1 and 4: ";

	auto number = Console::readValidated<int>(
		[](int number) {
			return number >= 1 && number <= 4;
		},
		"That's outside of the range. Try again.",
		"That's not a number. Try again."
	);

	Console::println("Thanks, you wrote ", number, ".");

	std::string firstInput, secondInput;
	Console::io << "\nNow write something interesting: " >> firstInput
	            << "Yes, something more please: " >> secondInput
	            << "This is what you wrote, but the other way around: "
	            << secondInput << ' ' << firstInput << '\n';

	Console::println("How nice!");
}

/*
	Usage of Avo::Indices class.
*/
void example_indices()
{
	float someArray[5]; // Or any container that std::data and std::size can be called on, or a null-terminated string.

	// i goes from 0 to std::size(someArray) - 1.
	for (auto i : Avo::Indices{ someArray }) 
	{
		someArray[i] = i / 3.f;
	}

	// End offset, i goes from 0 to std::size(someArray) - 2.
	for (auto i : Avo::Indices{ someArray, -1 }) 
	{
		someArray[i] = someArray[i + 1];
	}

	// Start offset, i goes from 2 to std::size(someArray) - 1.
	for (auto i : Avo::Indices{ 2, someArray })
	{
		someArray[i] = someArray[i + 1];
	}

	// Start and end offset, i goes from 2 to std::size(someArray) - 2.
	for (auto i : Avo::Indices{ someArray, 2, -1 })
	{
		someArray[i] = someArray[i + 1];
	}

	// i goes from 0 to 7.
	for (auto i : Avo::Indices{ 8 })
	{
		Console::io << i << ' ';
	}
	Console::io << '\n';

	// i goes from 4 to 89.
	for (auto i : Avo::Indices{ 4, 10 })
	{
		Console::io << i << ' ';
	}
}

/*
	Usage of the Avo::PointerRange class.
*/
void example_pointerRange()
{
	// Pretend this is just some API that wants a sequence of pointers. 
	// Replace all data-and-length parameters in functions by a PointerRange. Inspired by the cpp core guidelines.
	auto printBuffer = [](Avo::PointerRange<char> const& p_buffer) {
		for (auto byte : p_buffer)
		{
			std::cout << byte << ' ';
		}
	};

	// String literal:
	printBuffer("Pointer range");

	// Static array:
	char buffer[] = "Pointer range";
	printBuffer(buffer);

	// Any object that std::data and std::size can be called on:
	printBuffer(std::vector{ 'P', 'o', 'i', 'n', 't', 'e', 'r', 's', '!' });
	printBuffer(std::string{ "Pointer range" });

	// Dynamic array:
	// (Pretty bad, but may happen if data comes from some C API)

	Avo::Count cBufferSize = 30;
	auto* cBuffer = new char[cBufferSize];

	printBuffer({ cBuffer, cBufferSize });
	
	delete[] cBuffer;
}

/*
	Initialization order demonstration.

	Prints:
	Base construct
	Member construct 1
	Test initializer
	Member construct 2
	Test construct
*/

class Base
{
public:
	Base()
	{
		Console::println("Base construct");
	}
};
class Member
{
public:
	Member(int p_count)
	{
		Console::println("Member construct ", p_count);
	}
};
class Test : public Base
{
private:
	Member m_firstMember;

	Avo::Initializer init_test = [=] {
		Console::println("Test initializer");
	};

	Member m_secondMember = 3; // Does nothing because it is initialized in the constructor initializer list.

public:
	Test() :
		m_firstMember{ 1 },
		m_secondMember{ 2 }
	{
		Console::println("Test construct");
	}
};

int main()
{
}
