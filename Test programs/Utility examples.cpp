#include "../AvoGUI.hpp"

/*
	Usage of Console namespace.
*/
auto example_console() -> void
{
	Console::println(u8"Write something! Maybe unicode Åå Ää Öö Ññ Üü α δ λ μ π τ")
	        .println("You wrote: \"", Console::read<std::string>(), "\".");

	Console::io << "\nNow write a number between 1 and 4: ";

	auto const number = Console::readValidated<int>(
		[](int const number) {
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

//------------------------------

/*
	Usage of Avo::Indices class.
*/
auto example_indices() -> void
{
	float someArray[5]; // Or any container that std::data and std::size can be called on, or a null-terminated string.

	// i goes from 0 to std::size(someArray) - 1.
	for (auto const i : Avo::Indices{someArray}) 
	{
		someArray[i] = i / 3.f;
	}

	// End offset, i goes from 0 to std::size(someArray) - 2.
	for (auto const i : Avo::Indices{someArray, -1}) 
	{
		someArray[i] = someArray[i + 1];
	}

	// Start offset, i goes from 2 to std::size(someArray) - 1.
	for (auto const i : Avo::Indices{2, someArray})
	{
		someArray[i] = someArray[i + 1];
	}

	// Start and end offset, i goes from 2 to std::size(someArray) - 2.
	for (auto const i : Avo::Indices{someArray, 2, -1})
	{
		someArray[i] = someArray[i + 1];
	}

	// i goes from 0 to 7.
	for (auto const i : Avo::Indices{8}) // Range{8} does the same
	{
		Console::io << i << ' ';
	}
	Console::io << '\n';

	// i goes from 4 to 89.
	for (auto const i : Avo::Indices{4, 10}) // Range{4, 10} does the same
	{
		Console::io << i << ' ';
	}
}

//------------------------------

/*
	Usage of the Avo::Range class.
*/
auto example_range() -> void
{
	/*
		Pretend this is just some API that wants a sequence of pointers. 
		Replace all pointer-and-length parameters in functions by a Range of pointers. 
		Inspired by the cpp core guidelines.
	*/
	auto const printBuffer = [](Avo::Range<std::byte const*> p_buffer) {
		for (auto byte : p_buffer)
		{
			Console::io << static_cast<char>(byte) << ' ';
		}
		Console::println();
	};

	// Static array:
	std::byte buffer[] = {std::byte{'b'}, std::byte{'o'}, std::byte{'n'}, std::byte{'k'}, std::byte{'!'}};
	printBuffer(buffer);

	// Iterators:
	printBuffer({std::begin(buffer), std::end(buffer) - 1});

	// Any object that std::data and std::size can be called on:
	printBuffer(std::array{std::byte{'h'}, std::byte{'i'}});

	// Dynamic array:
	// (Pretty bad, but may happen if data comes from some C API)

	auto const cBufferSize = std::size_t{26};
	auto* const cBuffer = new std::byte[cBufferSize];
	std::generate_n(cBuffer, cBufferSize, [c = 'a']() mutable { return std::byte{c++}; });

	printBuffer({cBuffer, cBufferSize});
	
	delete[] cBuffer;

	// An Avo::Range can also be used for ranges of integers.
}

//------------------------------

/*
	Just to make you aware of this class.
*/
auto example_cleanup() -> int
{
	Avo::Cleanup const cleanup = [] {
		Console::println("Scope ended!!");
	};

	// Just do demonstrate that cleanup happens after the return statement.
	return [] {
		Console::println("I'm doing some things");
		return 33;
	}();
}

//------------------------------

/*
	Initialization order demonstration.

	Prints:
	Base construct
	Member construct 1
	Test initializer
	Member construct 2
	Test construct
*/
namespace example_initialization
{
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
		Member(int const p_count)
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
			m_firstMember{1},
			m_secondMember{2}
		{
			Console::println("Test construct");
		}
	};
} // example_initialization


auto main() -> int
{
	// example_initialization::Test{};
	// example_range();
	example_console();
}
