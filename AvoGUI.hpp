/*
	GUI framework by Björn Sundin. 

             --
       \   --  --   /
        \/     \ /
         |   /\   |
         \   \/   /
          \      /
           ------
            | |
	
	Info about documentation: a class that has virtual methods starts the documentation of methods with either 
	"LIBRARY IMPLEMENTED" or "USER IMPLEMENTED". If a method is "USER IMPLEMENTED", it means that the method 
	has no default library implementation and is meant to be implemented by you, the user of the library. A 
	method that is "LIBRARY IMPLEMENTED" can sometimes be overridden (if it is virtual) and in that case you 
	can choose to keep the library implementation and only add to it (by calling Parent::method(...) from
	inside your overridden method), or you can completely override it - although that can sometimes remove
	important functionality.

	Submit issues on GitHub!
	https://github.com/avocadoboi/AvoGUI

	Started development in summer 2018.

	MIT license, see LICENSE file.
*/

#pragma once

//------------------------------

#include <cstdint> // Fixed-size integer typedefs
#include <cmath>
#include <cstring>

// I/O
#include <fstream>
#include <iostream>

// Data structures
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <deque>
#include <set>
#include <unordered_map>

// Utilities
#include <functional>
#include <optional>

// Algorithms
#include <numeric>
#include <algorithm>
#include <charconv>

// Threading
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

// Debugging
#include <cassert>

//------------------------------
// I don't like the t

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using char8 = char; // char8_t Will have to wait for c++20 :(
using char16 = char16_t;

//------------------------------
// This is strongly recommended. 
// Remove it if it causes you any problems.

using namespace std::chrono_literals;

//------------------------------

/*
	Contains a Console class, an instance of it and some proxy methods 
	to simplify cross-platform UTF-8 console I/O.

	Separate repository at: https://github.com/avocadoboi/Unicode-console 
*/
namespace Console
{
	/*
		A console i/o class that supports UTF-8 (8-bit unicode) input and output.
		This allows the usage of all characters that are supported by the console font.
		It also provides a simpler interface and better error handling.
	*/
	class Console
	{
	public:
		/*
			This is the same as std::cout.operator<<.
		*/
		template<typename T>
		auto operator<<(T const& p_type) -> Console&
		{
			std::cout << p_type;
			return *this;
		}
		/*
			Prints p_arguments to the console and appends a new line afterwards.
		*/
		template<typename ... Argument>
		auto println(Argument&& ... p_arguments) -> Console&
		{
			(std::cout << ... << std::forward<Argument>(p_arguments)) << '\n';
			return *this;
		}

	private:
		bool m_wasLastInputValid = true;
	public:
		/*
			Returns whether the last input read from the console was the correct type.
		*/
		auto getWasLastInputValid() noexcept -> bool
		{
			return m_wasLastInputValid;
		}
		/*
			Same as getWasLastInputValid().
		*/
		operator bool() noexcept
		{
			return m_wasLastInputValid;
		}
		/*
			Same as !getWasLastInputValid().
		*/
		auto operator!() noexcept -> bool
		{
			return !m_wasLastInputValid;
		}

		/*
			This uses std::cin.operator>> but clears newline characters if user wrote invalid input.
			If the user did provide invalid input, getWasLastInputValid() or operator bool() will 
			return false until the next time input is correctly read.
		*/
		template<typename T>
		auto operator>>(T& p_type) -> Console&
		{
			std::cin >> p_type;
			m_wasLastInputValid = (bool)std::cin;
			if (!m_wasLastInputValid)
			{
				// A number was expected but the user wrote characters, so we need to ignore the 
				// trailing newline character and clear the error state of std::cin.
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cin.clear();
			}
			return *this;
		}

	private:
#ifdef _WIN32
		void* m_inputHandle; // Used to read unicode from console in the method below
#endif
		auto readString(std::string& p_string) -> void;

	public:
		/*
			This is the same as for example:
				auto string = std::string{};
				console >> string;
			but a simpler interface.
		*/
		template<typename T>
		auto read() -> T
		{
			T output;
			operator>>(output);
			return output;
		}

		/*
			Reads input from the console and prints p_errorMessage if the input did not
			correspond to the datatype T and tries again until it does.
		*/
		template<typename T>
		auto readValidated(std::string_view const p_errorMessage) -> T
		{
			T result;
			while (true)
			{
				operator>>(result);
				if (operator bool())
				{
					return result;
				}
				else
				{
					println(p_errorMessage);
				}
			}
		}
		/*
			Reads input from the console and prints p_errorMessage if p_getIsValid returns
			false and tries again until p_getIsValid returns true.
		*/
		template<typename T, typename ValidatorType>
		auto readValidated(ValidatorType const& p_getIsValid, std::string_view const p_errorMessage) -> T
		{
			T result;
			while (true)
			{
				operator>>(result);

				if (m_wasLastInputValid && p_getIsValid(result))
				{
					return result;
				}
				else
				{
					println(p_errorMessage);
				}
			}
		}
		/*
			Reads input from the console and prints p_typeValidationErrorMessage if the input did not correspond to the datatype T.
			If it was correctly read as T then it prints p_customValidationErrorMessage if p_getIsValid returns false.
			If the input was invalid in any of these two ways it tries to read input again until the input is valid.
		*/
		template<typename T, typename ValidatorType>
		auto readValidated(ValidatorType const& p_getIsValid, 
		                   std::string_view const p_customValidationErrorMessage, 
						   std::string_view const p_typeValidationErrorMessage) -> T
		{
			T result;
			while (true)
			{
				operator>>(result);

				if (operator!())
				{
					println(p_typeValidationErrorMessage);
				}
				else if (!p_getIsValid(result))
				{
					println(p_customValidationErrorMessage);
				}
				else
				{
					return result;
				}
			}
		}

	public:
		Console();
	};

	/*
		Specialization of the input operator template, to support UTF-8 input on Windows.
	*/
	template<>
	inline auto Console::operator>><std::string>(std::string& p_string) -> Console&
	{
		readString(p_string);
		return *this;
	}

	/*
		An instance of Console.
	*/
	inline Console io;

	/*
		Prints p_arguments to the console and appends a new line afterwards.
	*/
	template<typename ... Argument>
	auto println(Argument&& ... p_arguments) -> Console&
	{
		return io.println(std::forward<Argument>(p_arguments)...);
	}
	/*
		This is the same as for example:
			auto string = std::string{};
			Console::io >> string;
		but a simpler interface.
	*/
	template<typename T>
	auto read() -> T
	{
		return io.read<T>();
	}
	/*
		Reads input from the console and prints p_errorMessage if the input did not
		correspond to the datatype T and tries again until it does.
	*/
	template<typename T>
	auto readValidated(std::string_view const p_errorMessage) -> T
	{
		return io.readValidated<T>(p_errorMessage);
	}
	/*
		Reads input from the console and prints p_errorMessage if p_getIsValid returns
		false and tries again until p_getIsValid returns true.
	*/
	template<typename T, typename ValidatorType>
	auto readValidated(ValidatorType const& p_getIsValid, std::string_view const p_errorMessage) -> T
	{
		return io.readValidated<T>(p_getIsValid, p_errorMessage);
	}
	/*
		Reads input from the console and prints p_typeValidationErrorMessage if the input did not correspond to the datatype T.
		If it was correctly read as T then it prints p_customValidationErrorMessage if p_getIsValid returns false.
		If the input was invalid in any of these two ways it tries to read input again until the input is valid.
	*/
	template<typename T, typename ValidatorType>
	auto readValidated(ValidatorType const& p_getIsValid, 
	                   std::string_view const p_customValidationErrorMessage, 
					   std::string_view const p_typeValidationErrorMessage) -> T
	{
		return io.readValidated<T>(p_getIsValid, p_customValidationErrorMessage, p_typeValidationErrorMessage);
	}
}

namespace Avo
{
	namespace chrono = std::chrono;

	// Just give me concepts already smh
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	using IntegerType = T;
	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	using FloatingPointType = T;
	template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
	using ClassType = T;
	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	using ArithmeticType = T;

	// Represents an index, position in some sort of list or array.
	using Index = int64;

	// Represents a length, size or count
	using Count = int64;

	// Physical screen pixels
	using Pixels = int32;

	// Density independent pixels
	using Dip = float;

	// Represents a proportion of something. Something that will be multiplied with a value.
	using Factor = float;

	/*
		Represents a generic, iterable range of pointers or integers.
		If the range is over pointers, dereferencing an iterator in the Range also dereferences the pointer it holds.
		If the range is over integers, dereferencing an iterator in the Range only returns the integer.

		Example 1:
			auto someArray = std::array{3, 1, 4, 1, 5, 9};
			auto rangeOverArray = Range{someArray};
			// The resulting type is Range<int*>.

		Example 2:
			auto rangeOverIntegers = Range{3, 8};
			// The resulting type is Range<int>.
			// The range includes values 3 to 7.
	*/
	template<typename _Value>
	class Range
	{
	public:
		class Iterator
		{
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = std::remove_pointer_t<_Value>;
			using pointer = std::conditional_t<std::is_pointer_v<_Value>, _Value, _Value*>;
			using reference = std::add_lvalue_reference_t<value_type>;
			using iterator_category = std::random_access_iterator_tag;
			
		private:
			_Value m_value{};
		public:
			constexpr auto getValue() const -> _Value
			{
				return m_value;
			}
			constexpr operator _Value() const 
			{
				return m_value;
			}

			constexpr auto operator=(Iterator const p_other) noexcept -> Iterator&
			{
				m_value = p_other.m_value;
				return *this;
			}
			constexpr auto operator=(_Value const p_value) noexcept -> Iterator&
			{
				m_value = p_value;
				return *this;
			}

			constexpr auto operator==(Iterator const p_other) const noexcept -> bool
			{
				return m_value == p_other.m_value;
			}
			constexpr auto operator!=(Iterator const p_other) const noexcept -> bool
			{
				return m_value != p_other.m_value;
			}
			constexpr auto operator<(Iterator const p_other) const noexcept -> bool
			{
				return m_value < p_other.m_value;
			}
			constexpr auto operator>(Iterator const p_other) const noexcept -> bool
			{
				return m_value > p_other.m_value;
			}

			constexpr auto operator*() const noexcept -> auto&
			{
				if constexpr (std::is_pointer_v<_Value>)
				{
					return *m_value;
				}
				else
				{
					return m_value;
				}
			}
			
			constexpr auto operator++() noexcept -> Iterator&
			{
				++m_value;
				return *this;
			}
			constexpr auto operator--() noexcept -> Iterator&
			{
				--m_value;
				return *this;
			}

			constexpr auto operator+(Iterator const p_other) const noexcept -> Iterator
			{
				return {m_value + p_other.m_value};
			}
			constexpr auto operator+(std::size_t const p_offset) const noexcept -> Iterator
			{
				return {m_value + p_offset};
			}
			constexpr auto operator-(Iterator const p_other) const noexcept -> std::ptrdiff_t
			{
				return m_value - p_other.m_value;
			}
			constexpr auto operator-(std::size_t const p_offset) const noexcept -> Iterator
			{
				return {m_value - p_offset};
			}

			constexpr Iterator(Iterator const&) noexcept = default;
			constexpr Iterator(Iterator&&) noexcept = default;
			constexpr Iterator() noexcept = default;
			constexpr Iterator(_Value p_value) noexcept :
				m_value{p_value}
			{}
		};

	private:
	
		Iterator m_start;
	public:
		/*
			Returns the first iterator in the range.
		*/
		[[nodiscard]] constexpr auto begin() const noexcept -> Iterator
		{
			return m_start;
		}

		/*
			Returns the first pointer in the range or the first number in the range.
		*/
		[[nodiscard]] constexpr auto data() const noexcept -> _Value
		{
			return m_start.getValue();
		}

		/*
			Returns the value in the range at index p_position.
		*/
		constexpr auto operator[](std::size_t const p_position) const noexcept -> auto&
		{
			assert(("Range error: index out of bounds.", p_position < size()));
			return *(m_start + p_position);
		}
		
	private:
		Iterator m_end;
	public:
		/*
			Returns the last iterator in the range.
		*/
		[[nodiscard]] constexpr auto end() const noexcept -> Iterator
		{
			return m_end;
		}

		/*
			Returns the number of values in the range.
		*/
		[[nodiscard]] constexpr auto size() const noexcept -> std::size_t
		{
			// m_end is always >= m_start.
			return static_cast<std::size_t>(m_end - m_start);
		}

		/*
			Constructs a range including values p_start to p_end - 1.
		*/
		constexpr Range(_Value const p_start, _Value const p_end) noexcept :
			m_start{p_start},
			m_end{p_end}
		{
			checkBounds();
		}
		/*
			Constructs a range from begin and end random access iterators.
		*/
		template<typename _Iterator, typename = std::enable_if_t<std::is_class_v<_Iterator>>>
		constexpr Range(_Iterator const p_start, _Iterator const p_end) noexcept :
			m_start{&*p_start},
			m_end{&*p_end}
		{

			checkBounds();
		}
		/*
			Constructs a range from a start value and a length.
		*/
		template<typename _Integer, typename = std::enable_if_t<!std::is_integral_v<_Value> && std::is_integral_v<_Integer>>>
		constexpr Range(_Value const p_start, _Integer const p_length) noexcept :
			m_start{p_start},
			m_end{p_start + p_length}
		{
			checkBounds();
		}
		/*
			Constructs a range from a static array.
		*/
		template<typename T, std::size_t size>
		constexpr Range(T (&p_array)[size]) noexcept :
			m_start{p_array},
			m_end{p_array + size}
		{
		}
		/*
			Constructs a range from an integer, where 
			the range goes from 0 to p_value - 1
		*/
		template<typename T>
		constexpr Range(IntegerType<T> const p_value) noexcept :
			m_end{p_value}
		{
			checkBounds();
		}
		/*
			Constructs a range from any container that std::data and std::size can be called on.
		*/
		template<typename _Container, typename = std::enable_if_t<std::is_class_v<_Container>>>
		constexpr Range(_Container& p_container) noexcept :
			m_start{std::data(p_container)},
			m_end{m_start + std::size(p_container)}
		{
			static_assert(
				std::is_convertible_v<decltype(std::data(p_container)), _Value>, 
				"Range error: Container value type is not convertible to the value type of the range."
			);
		}

	private:
		constexpr auto checkBounds() noexcept -> void
		{
			if (m_end < m_start)
			{
				m_end = m_start;
			}
		}
	};

	template<typename _Iterator, typename = std::enable_if_t<std::is_class_v<_Iterator>>>
	Range(_Iterator p_start, _Iterator p_end) -> Range<decltype(&*p_start)>;

	template<typename T, std::size_t size>
	Range(T (&p_array)[size]) -> Range<T*>;
	template<typename T, typename = std::enable_if_t<std::is_pointer_v<T> || std::is_integral_v<T>>>
	Range(T p_value) -> Range<T>;
	template<typename _Container>
	Range(_Container& p_container) -> Range<decltype(std::data(p_container))>;

	/*
		Used to iterate over the indices of some container or array, or iterate through a range of integers.
		Use it together with range-based for loops like this:
			for (auto i : Indices{array})
		or this:
			for (auto i : Indices{100})
		This is a zero runtime-overhead abstraction and yields identical assembly to an equivalent regular for loop. (with optimizations turned on)
	*/
	class Indices : public Range<std::size_t>
	{
	public:
		template<typename _Container, typename = std::enable_if_t<!std::is_integral_v<_Container>>>
		constexpr Indices(_Container& p_container) noexcept :
			Range{std::size(p_container)}
		{}
		template<
			typename _Container, typename _Integer, 
			typename = std::enable_if_t<!std::is_integral_v<_Container> && std::is_integral_v<_Integer>>
		>
		constexpr Indices(_Integer const p_startOffset, _Container& p_container) noexcept :
			Range{p_startOffset, std::size(p_container)}
		{}
		template<
			typename _Container, typename _Integer, 
			typename = std::enable_if_t<!std::is_integral_v<_Container> && std::is_integral_v<_Integer>>
		>
		constexpr Indices(_Container& p_container, _Integer const p_endOffset) noexcept :
			Range{0, std::size(p_container) + p_endOffset}
		{}
		template<
			typename _Container, typename _Integer, 
			typename = std::enable_if_t<!std::is_integral_v<_Container> && std::is_integral_v<_Integer>>
		>
		constexpr Indices(_Container& p_container, _Integer const startOffset, _Integer const p_endOffset) noexcept :
			Range{startOffset, std::size(p_container) + p_endOffset}
		{}
		template<typename _Integer, typename = std::enable_if_t<std::is_integral_v<_Integer>>>
		constexpr Indices(_Integer const p_n) noexcept :
			Range{p_n}
		{}
		template<typename _Integer, typename = std::enable_if_t<std::is_integral_v<_Integer>>>
		constexpr Indices(_Integer const p_start, _Integer const p_end) noexcept :
			Range{p_start, p_end}
		{}
	};

	template<typename T>
	constexpr T E =       static_cast<T>(2.71828182845904523l);
	template<typename T>
	constexpr T HALF_PI = static_cast<T>(1.57079632679489661l);
	template<typename T>
	constexpr T PI =      static_cast<T>(3.14159265358979323l);
	template<typename T>
	constexpr T TWO_PI =  static_cast<T>(6.28318530717958647l);
	template<typename T>
	constexpr T TAU = TWO_PI<T>;

	/*
		Returns a number multiplied by itself (x to the 2nd power, meaning x^2, meaning x*x).
		Can be useful if you want to quickly square a longer expression.
	*/
	template<typename T>
	constexpr auto square(T const p_x) noexcept -> T
	{
		return p_x*p_x;
	}

	/*
		Returns the inverse square root of a float using a faster but less accurate algorithm.
		It is about 8% to 15% faster than 1.f/std::sqrt(x) with gcc -O3 on my computer.
	*/
	inline auto fastInverseSqrt(float const p_input) noexcept -> float
	{
		static_assert(std::numeric_limits<float>::is_iec559, "fastInverseSqrt error: float type must follow IEEE 754-1985 standard.");
		static_assert(sizeof(float) == 4, "fastInverseSqrt error: sizeof(float) must be 4.");

		// Only way to do type punning without undefined behavior it seems.
		uint32 bits;
		std::memcpy(&bits, &p_input, 4);

		bits = 0x5f3759df - bits/2;

		float approximation;
		std::memcpy(&approximation, &bits, 4);

		return approximation*(1.5f - 0.5f*p_input*approximation*approximation);
	}

	/*
		Returns a random double between 0 and 1 from a uniform distribution.
		It just uses the standard library random header. Convenient function.
	*/
	auto random() -> long double;
	/*
		Returns a random double between 0 and 1 from a normal distribution with standard deviation 1 and mean 0.
		It just uses the standard library random header. Convenient function.
	*/
	auto randomNormal() -> long double;

	//------------------------------

	/*
		Returns a value somewhere within p_range depending on p_position. This is linear interpolation.
		If p_position is below 0 or above 1, the returned value will not be within p_range.
	*/
	template<typename T>
	constexpr auto interpolate(Range<T> const p_range, Factor const p_position) noexcept -> T
	{
		if constexpr (std::is_integral_v<T>)
		{
			return static_cast<T>(p_range.begin().getValue()*(1.f - p_position) + p_range.end().getValue()*p_position + 0.5f);
		}
		return static_cast<T>(p_range.begin().getValue()*(1.f - p_position) + p_range.end().getValue()*p_position);
	}
	/*
		Clips p_value so that the returned value is never outside p_range.
	*/
	template<typename T0, typename T1>
	constexpr auto constrain(T0 const p_value, Range<T1> const p_range) noexcept -> T0
	{
		// If this is true then we need to round "inwards" in case the value is outside of the bounds.
		auto const min = p_range.begin().getValue(),
		           max = p_range.end().getValue();
		if constexpr (std::is_integral_v<T0> && std::is_floating_point_v<T1>)
		{
			return p_value < min ? std::ceil(min) : 
			       p_value > max ? std::floor(max) : p_value;
		}
		return p_value < min ? min : p_value > max ? max : p_value;
	}
	/*
		Clips p_value so that the returned value is never below 0 or above 1.
		If 0 <= p_value <= 1, then the returned value is equal to p_value.
	*/
	template<typename T>
	constexpr auto constrain(T const p_value) noexcept -> T
	{
		return constrain<T>(p_value, Range{static_cast<T>(0), static_cast<T>(1)});
	}

	//------------------------------

	/*
		Returns -1 if the number is negative, 0 if it's 0 and 1 if it's positive.
	*/
	template<typename T>
	constexpr auto sign(T const p_number) noexcept -> T
	{
		return (p_number > 0) - (p_number < 0);
	}

	//------------------------------

	template<typename T>
	constexpr auto max(T&& p_value) noexcept
	{
		return std::forward<T>(p_value);
	}
	template<typename T0, typename T1, typename ... T2>
	constexpr auto max(T0&& p_first, T1&& p_second, T2&& ... p_arguments) noexcept
	{
		return (p_first > p_second) ? 
			max(std::forward<T0>(p_first), std::forward<T2>(p_arguments)...) : 
			max(std::forward<T1>(p_second), std::forward<T2>(p_arguments)...);
	}

	template<typename T>
	constexpr auto min(T&& p_value) noexcept
	{
		return std::forward<T>(p_value);
	}
	template<typename T0, typename T1, typename ... T2>
	constexpr auto min(T0&& p_first, T1&& p_second, T2&& ... p_arguments) noexcept
	{
		return (p_first < p_second) ? 
			min(std::forward<T0>(p_first), std::forward<T2>(p_arguments)...) : 
			min(std::forward<T1>(p_second), std::forward<T2>(p_arguments)...);
	}

	//------------------------------

	/*
		Removes an element from a vector. The function returns true if the element existed in the vector and was removed.
	*/
	template<typename T>
	auto removeVectorElementWhileKeepingOrder(std::vector<T>& p_vector, T const& p_element) -> bool
	{
		if (auto const position = std::find(p_vector.cbegin(), p_vector.cend(), p_element);
			position != p_vector.cend())
		{
			p_vector.erase(position);
			return true;
		}
		return false;
	}

	/*
		Removes an element from a vector without keeping the order of the elements in the vector, making it more efficient.
		The function returns true if the element existed in the vector and was removed (replaced by the last element).
	*/
	template<typename T>
	auto removeVectorElementWithoutKeepingOrder(std::vector<T>& p_vector, T const& p_element) -> bool
	{
		if (auto const position = std::find(p_vector.begin(), p_vector.end(), p_element);
			position != p_vector.end())
		{
			*position = p_vector.back();
			p_vector.pop_back();
			return true;
		}
		return false;
	}

	//------------------------------

	// TODO: add String class with built in UTF-8 support and more operations, ICU library used if available for toLowercase/toUppercase support.
	// TODO: add Character class for UTF-8 characters with operators and stuff

	/*
		Converts a UTF-8 encoded char string to a UTF-16 encoded char16 string.
	*/
	auto convertUtf8ToUtf16(std::string_view p_input, Range<char16*> p_output) -> void;
	/*
		Converts a UTF-8 encoded string to a UTF-16 encoded std::u16string.
	*/
	auto convertUtf8ToUtf16(std::string_view p_input) -> std::u16string;
	/*
		Returns the number of UTF-16 encoded char16 units that would be used to represent the same characters in a UTF-8 encoded char string.
	*/
	auto getNumberOfUnitsInUtfConvertedString(std::string_view p_input) -> Count;

	/*
		Converts a UTF-16 encoded char16 string to a UTF-8 encoded char string.
	*/
	void convertUtf16ToUtf8(std::u16string_view p_input, Range<char8*> p_output);
	/*
		Converts a UTF-16 char16 string to a UTF-8 encoded std::string.
	*/
	auto convertUtf16ToUtf8(std::u16string_view p_input) -> std::string;
	/*
		Returns the number of UTF-8 encoded char units that would be used to represent the same characters in a UTF-16 encoded char16 string.
	*/
	auto getNumberOfUnitsInUtfConvertedString(std::u16string_view p_input) -> Count;

	//------------------------------

	/*
		If p_startByte is the first byte in a utf-8 encoded character, the function returns the number of bytes that the character is made up of, which can be 1-4.
		If p_startByte is not the first byte in the character, the function returns 0.
		If p_startByte is an invalid UTF-8 byte, -1 is returned.
	*/
	constexpr auto getNumberOfUnitsInCharacter(char8 const p_startByte) noexcept -> int8
	{
		// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 126
		if (!(p_startByte & 0x80)) // 0xxxxxxx
			return 1;
		if ((p_startByte & 0xc0) == 0x80) // 10??????
			return 0;
		if ((p_startByte & 0xe0) == 0xc0) // 110yyyyy
			return 2;
		if ((p_startByte & 0xf0) == 0xe0) // 1110zzzz
			return 3;
		if ((p_startByte & 0xf8) == 0xf0) // 11110uuu
			return 4;
		return -1;
	}
	/*
		If p_startUnit is the first unit in a UTF-16 encoded character, the function returns the number of units that the character is made up of, which can only be 1 or 2.
		If p_startUnit is not the first unit in the character, the function returns 0.
	*/
	constexpr auto getNumberOfUnitsInCharacter(char16 const p_startUnit) noexcept -> int8
	{
		// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 125
		if ((p_startUnit & 0xfc00) == 0xd800) // 110110wwwwxxxxxx
			return 2;
		if ((p_startUnit & 0xfc00) == 0xdc00) // 110111xxxxxxxxxx
			return 0;
		return 1; // xxxxxxxxxxxxxxxx
	}
	/*
		Returns whether p_byte is the start of a UTF-8 encoded character
	*/
	constexpr auto getIsUnitStartOfCharacter(char8 const p_byte) noexcept -> bool
	{
		return (p_byte & 0xc0) != 0x80;
	}
	/*
		Returns whether p_unit is the start of a UTF-8 encoded character
	*/
	constexpr auto getIsUnitStartOfCharacter(char16 const p_unit) noexcept -> bool
	{
		return (p_unit & 0xfc00) != 0xdc00;
	}
	/*
		Returns the index of the unit at a certain character index in a UTF-8 or UTF-16 encoded string.
		If p_characterIndex is outside of the string, the size of the string is returned.
	*/
	template<typename T>
	auto getUnitIndexFromCharacterIndex(std::basic_string_view<T> const p_string, Index const p_characterIndex) -> Index
	{
		if (!p_characterIndex)
		{
			return 0;
		}
		if (p_characterIndex >= p_string.size())
		{
			return p_string.size();
		}
		
		auto position = std::find_if(
			p_string.begin(), p_string.end(), 
			[&, numberOfCharactersCounted = Count{}](auto character) mutable {
				return getIsUnitStartOfCharacter(character) && numberOfCharactersCounted++ == p_characterIndex;
			}
		);
		return position - p_string.begin();
	}
	/*
		Returns the index of the character that the unit at p_unitIndex in the UTF-8 or UTF-16 encoded p_string belongs to.
		If p_unitIndex is outside of the string, the last character index is returned.
		If p_unitIndex is past the start of a character but before the next one, it returns the index of the character it is part of.
	*/
	template<typename T>
	auto getCharacterIndexFromUnitIndex(std::basic_string_view<T> const p_string, Index p_unitIndex) -> Index
	{
		if (p_unitIndex < 1)
		{
			return 0;
		}
		if (p_unitIndex >= p_string.size())
		{
			p_unitIndex = p_string.size() - 1;
		}

		return std::count_if(
			p_string.begin() + 1, p_string.begin() + p_unitIndex + 1,
			[](T const unit) { return getIsUnitStartOfCharacter(unit); }
		);
	}
	template<typename T>
	inline auto getNumberOfCharactersInString(std::basic_string_view<T> const p_string) -> Index
	{
		return getCharacterIndexFromUnitIndex(p_string, p_string.size()) + 1;
	}

	//------------------------------

	enum class RoundingType
	{
		Down,
		Up,
		Nearest
	};

	// TODO: write our own stringToNumber function because only MSVC has implemented std::from_chars for floating point numbers...

	/*
		Converts a string to a number if possible.
	*/
	template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	auto stringToNumber(std::string_view const p_string) -> std::optional<T>
	{
		auto value = T{};
		if (auto error = std::from_chars(p_string.data(), p_string.data() + p_string.size(), value).ec;
			error == std::errc::invalid_argument || error == std::errc::result_out_of_range)
		{
			return {};
		}
		return value;
	}

	/*
		Converts a number to a string, using . (dot) for the decimal point.
		This overload takes a buffer as input and returns a string_view pointing 
		to the range within the input buffer where the output was written.
	*/
	template<int precision = 6, typename T>
	[[nodiscard]] auto numberToString(Range<char8*> p_buffer, ArithmeticType<T> p_number) -> std::string_view
	{
		if (!p_number)
		{
			return "0";
		}

		constexpr auto maxNumberOfIntegerDigits = 39;
		if constexpr (std::is_integral_v<T>)
		{
			auto position = p_buffer.end();
			
			auto const writeTheDigits = [&]{
				while (p_number)
				{
					*--position = '0' + p_number % 10;
					p_number /= 10;
				}
			};

			if constexpr (std::is_signed_v<T>)
			{
				auto const isNegative = p_number < 0;
				p_number = std::abs(p_number);

				writeTheDigits();

				if (isNegative)
				{
					*--position = '-';
				}
			}
			else
			{
				writeTheDigits();
			}

			return {position, static_cast<std::string_view::size_type>(p_buffer.end() - position)};
		}
		else
		{
			auto position = p_buffer.begin() + maxNumberOfIntegerDigits;

			auto const isNegative = p_number < 0;
			auto decimalPart = static_cast<long double>(std::abs(p_number));

			long double integerPart_double;
			decimalPart = std::modf(decimalPart, &integerPart_double);

			auto integerPart_int = static_cast<int64>(integerPart_double);
			do
			{
				*--position = '0' + integerPart_int % 10;
				integerPart_int /= 10;
			} while (integerPart_int);

			auto startPosition = position;
			if (isNegative)
			{
				*--startPosition = '-';
			}

			*(position = p_buffer.begin() + maxNumberOfIntegerDigits) = '.';

			auto const last = p_buffer.end() - 1;
			while (position != last)
			{
				decimalPart = std::modf(decimalPart*10.l, &integerPart_double);
				*++position = '0' + static_cast<int64>(integerPart_double) % 10;
			}

			if (*position == '9')
			{
				while (*--position == '9') {}
				if (*position == '.')
				{
					--position;
				}
				++*position;
			}
			else
			{
				while (*--position == '0') {}
				if (*position == '.')
				{
					--position;
				}
				else
				{
					*position += *position != '9' && *(position + 1) >= '5';
				}
			}
			return std::string_view{startPosition, static_cast<std::string_view::size_type>(position + 1 - startPosition)};
		}
	}
	/*
		Converts a number to a string, using . (dot) for the decimal point.
		It's about 8x faster than using std::ostringstream for floating point numbers
		and 10x faster for integers.
	*/
	template<int precision = 5, typename T>
	[[nodiscard]] auto numberToString(ArithmeticType<T> p_number) -> std::string
	{
		constexpr auto maxNumberOfIntegerDigits = 39;
		auto buffer = std::array<char8, maxNumberOfIntegerDigits + 1 + (std::is_floating_point_v<T> ? precision + 1 : 0)>();
		return std::string{numberToString<precision>(buffer, p_number)};
	}

	/*
		Converts a number rounded at a certain digit to a string, using . (dot) for the decimal point.
		If p_roundingIndex is 0, only all decimals are rounded off and it becomes an integer.
		Positive goes to the right and negative goes to the left.
	*/
	template<typename T>
	[[nodiscard]] auto numberToString(
		T const p_value, 
		Index const p_roundingIndex, 
		RoundingType const p_roundingType = RoundingType::Nearest
	) -> std::string
	{
		auto const roundingFactor = std::pow(10., p_roundingIndex);
		if (p_roundingType == RoundingType::Nearest)
		{
			return numberToString(std::round(p_value*roundingFactor)/roundingFactor);
		}
		else if (p_roundingType == RoundingType::Down)
		{
			return numberToString(std::floor(p_value*roundingFactor)/roundingFactor);
		}
		else
		{
			return numberToString(std::ceil(p_value*roundingFactor)/roundingFactor);
		}
	}

	/*
		TODO: rewrite createFormattedString to avoid creating a vector of std::ostringstream. 
		Instead split p_format into array of std::string_view and use parameter pack expansion 
		to directly write alternating format string views and p_objects to one string.
	*/

	template<typename T>
	inline auto toString(ArithmeticType<T> p_number) -> std::string
	{
		return numberToString(p_number);
	}
	inline auto toString(std::string const& p_string) -> std::string const&
	{
		return p_string;
	}
	inline auto toString(std::string&& p_string) -> std::string&&
	{
		return std::move(p_string);
	}
	inline auto toString(std::string_view p_string) -> std::string
	{
		return std::string{p_string};
	}
    template<std::size_t size>
    auto toString(char8 const (&p_array)[size]) -> std::string
    {
        return p_array;
    }
	inline auto toString(std::u16string const& p_string) -> std::string
	{
		return convertUtf16ToUtf8(p_string);
	}
	inline auto toString(std::u16string_view p_string) -> std::string
	{
		return convertUtf16ToUtf8(p_string);
	}
	inline auto toString(std::wstring const& p_string) -> std::string
	{
		static_assert(
			sizeof(wchar_t) == sizeof(char16), 
			"A std::wstring can only be converted to a std::string if the size of wchar_t" 
			"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
			"AvoGUI does not support the UTF-32 encoding."
		);
		return convertUtf16ToUtf8({reinterpret_cast<char16 const*>(p_string.data()), p_string.size()});
	}
	inline auto toString(std::wstring_view const& p_string) -> std::string
	{
		static_assert(
			sizeof(wchar_t) == sizeof(char16), 
			"A std::wstring can only be converted to a std::string if the size of wchar_t" 
			"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
			"AvoGUI does not support the UTF-32 encoding."
		);
		return convertUtf16ToUtf8({reinterpret_cast<char16 const*>(p_string.data()), p_string.size()});
	}

	/*
		Simple function to format a string by replacing placeholders in p_format with p_objects.
		p_objects can be any objects which have a std::ostring << operator defined.

		The placeholders are in the form of {index} where index is the index of the argument to be inserted.
		Writing {0} will insert the first item, {1} will insert the second item.
		Only values of [0, 9] are allowed as the indicies, meaning max 10 objects can be inserted in one call.

		Example:
		std::string formattedString = createFormattedString(
			"I have {1} {2} and {3} {4} and {5} {6}. Pi: {0}",
			PI, 2, "cats", 0, "dogs", 10, "fingers"
		);
	*/
	template<typename ... FormattableType>
	[[nodiscard]] auto createFormattedString(std::string_view const p_format, FormattableType&& ... p_objects)
		-> std::string
	{
		auto const objectStrings = std::array{toString(std::forward<FormattableType>(p_objects))...};
		auto result = std::string();
		
		auto position = Index{};
		auto objectIndex = 0;
		while (true)
		{
			/*
				For every iteration we will first append a slice of 
				text from p_format onto the result, then an object string.
			*/
			
			// This is the position of the first text character in p_format to be appended.
			auto const formatSliceStart = position;

			// Find where to insert the next object

			auto const openBracePosition = p_format.find('{', formatSliceStart);
			if (openBracePosition == std::string_view::npos)
			{
				// We're done with formatting.
				break;
			}
			
			auto const closeBracePosition = p_format.find('}', openBracePosition + 1);
			if (closeBracePosition == std::string_view::npos)
			{
				// We're done with formatting.
				break;
			}
			
			// Choose the object string to append from what is between the '{' and the '}'.
			// There are some different possible cases.

			auto* objectString = static_cast<std::string const*>(nullptr);
			if (closeBracePosition == openBracePosition + 1)
			{
				// There was a {} sequence. Just insert the next object.
				if (++objectIndex == objectStrings.size())
				{
					// We're done with formatting.
					break;
				}
				else
				{
					objectString = &objectStrings[objectIndex];
				}
			}
			else if (p_format[openBracePosition + 1] == '{')
			{
				// Two { were after each other. 
				// Insert single { and start the next iteration after the {{.
				result += p_format.substr(formatSliceStart, openBracePosition + 1 - formatSliceStart);
				position = openBracePosition + 2; // Here we put the next position after the {{
				continue;
			}
			else
			{
				if (
					std::optional<Index> const index = stringToNumber<Index>(
						p_format.substr(
							openBracePosition + 1, 
							closeBracePosition - openBracePosition - 1
						)
					)
				)
				{
					objectString = &objectStrings[*index];
					objectIndex = *index;
				}
				else
				{
					// There was no valid index between '{' and '}'.
					// Just write the slice including the { and continue after the { in the next iteration.
					position = openBracePosition + 1;
					result += p_format.substr(formatSliceStart, position - formatSliceStart);
					continue;
				}
			}
			
			// Append both the text slice and the object string.
			(result += p_format.substr(formatSliceStart, openBracePosition - formatSliceStart)) += *objectString;

			// Will become the start of the next text slice.
			position = closeBracePosition + 1;
		}
		if (position < p_format.size() - 1)
		{
			result += p_format.substr(position);
		}
		return result;
	}

	//------------------------------

	using DataVector = std::vector<std::byte>;
	using DataView = Range<std::byte const*>;
	using DataRange = Range<std::byte*>;

	[[nodiscard]] inline auto readFile(std::string_view const p_path) -> DataVector
	{
		auto file = std::ifstream{p_path.data(), std::ios::ate | std::ios::binary};

		if (!file)
		{
			return {};
		}

		auto result = DataVector(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(result.data()), result.size());

		return result;
	}

	//------------------------------

	/*
		Represents an ID.
		To generate a new unique ID, use the default constructor like this:
			Id id;
		To create an ID with a specific value (not guaranteed to be unique), just assign:
			ID id = 1234;
		An ID which converts to 0 is considered invalid, and can be created like this:
			Id id = 0;
	*/
	class Id
	{
	private:
		static Count s_counter;
		Count m_count;

	public:
		constexpr operator Count() const noexcept
		{
			return m_count;
		}
		constexpr auto operator==(Id p_id) const noexcept -> bool
		{
			return p_id.m_count == m_count;
		}
		constexpr auto operator!=(Id p_id) const noexcept -> bool
		{
			return p_id.m_count != m_count;
		}

		constexpr auto operator=(Id const& p_id) noexcept -> Id&
		{
			m_count = p_id;
			return *this;
		}

		constexpr Id(Count p_id) noexcept :
			m_count{p_id}
		{}
		constexpr Id(Id const& p_id) noexcept :
			m_count{p_id}
		{}
		constexpr Id() noexcept :
			m_count{++s_counter}
		{}
	};

	//------------------------------

	/*
		Binds an object to a member method of its class, so that the returned function can be called without providing the instance.
	*/
	template<typename ReturnType, typename Class, typename ... Arguments>
	[[nodiscard]] auto bind(auto (Class::* p_function)(Arguments...) -> ReturnType, Class* const p_instance)
		-> std::function<ReturnType(Arguments...)>
	{
		return [p_instance, p_function](Arguments... arguments) { return (p_instance->*p_function)(arguments...); };
	}

	/*
		This is a class used to easily manage event listeners. Any type of functional can be a listener.
		The return type and arguments have to be the same for all listeners added to one instance of EventListeners.
	*/
	template<typename FunctionalType>
	class EventListeners
	{
	private:
		std::recursive_mutex m_mutex;
		std::vector<std::function<FunctionalType>> m_listeners;

	public:
		auto begin() noexcept
		{
			return m_listeners.begin();
		}
		auto end() noexcept
		{
			return m_listeners.end();
		}

		template<typename Callable>
		auto add(Callable const& p_listener) -> void
		{
			std::scoped_lock const lock{m_mutex};
			m_listeners.emplace_back(p_listener);
		}
		template<typename Callable>
		auto operator+=(Callable const& p_listener) -> EventListeners&
		{
			add(p_listener);
			return *this;
		}

		auto remove(std::function<FunctionalType> const& p_listener) -> void
		{
			auto const lock = std::scoped_lock{m_mutex};
			auto const& listenerType = p_listener.target_type();
			for (auto& listener : m_listeners)
			{
				if (listenerType == listener.target_type())
				{
					// template keyword is used to expicitly tell the compiler that target is a template method for
					// std::function<FunctionalType> and < shouldn't be parsed as the less-than operator
					if (*(p_listener.template target<FunctionalType>()) == *(listener.template target<FunctionalType>()))
					{
						listener = m_listeners.back();
						m_listeners.pop_back();
						break;
					}
				}
			}
		}
		auto operator-=(std::function<FunctionalType> const& p_listener) -> EventListeners&
		{
			remove(p_listener);
			return *this;
		}

		auto clear() -> void
		{
			std::scoped_lock const lock{m_mutex};
			m_listeners.clear();
		}

		/*
			Calls all of the listeners with p_eventArguments as the arguments.
		*/
		template<typename ... T>
		auto notifyAll(T&& ... p_eventArguments) -> void
		{
			// The explicit template argument is required here.
			auto const lock = std::scoped_lock<std::recursive_mutex>{m_mutex};
			auto listenersCopy = m_listeners;
			for (auto& listener : listenersCopy)
			{
				listener(std::forward<T>(p_eventArguments)...);
			}
		}
		template<typename ... T>
		auto operator()(T&& ... p_eventArguments) -> void
		{
			notifyAll(std::forward<T>(p_eventArguments)...);
		}

		auto operator=(EventListeners&& p_other) noexcept -> EventListeners&
		{
			m_listeners = std::move(p_other.m_listeners);
			return *this;
		}
		auto operator=(EventListeners const& p_other) -> EventListeners& = delete;

		EventListeners() = default;
		EventListeners(EventListeners&& p_listeners) noexcept
		{
			m_listeners = std::move(p_listeners.m_listeners);
		}
		EventListeners(EventListeners const&) = delete;
	};

	/*
		Calls a callable when it goes out of scope.
	*/
	template<typename T>
	class Cleanup
	{
	public:
		Cleanup(Cleanup const&) = delete;
		Cleanup(Cleanup&&) = delete;
		auto& operator=(Cleanup const&) = delete;
		auto& operator=(Cleanup&&) = delete;

	private:
		T m_callback;

	public:
		Cleanup(T const& p_callback) :
			m_callback{p_callback}
		{}
		Cleanup(T&& p_callback) :
			m_callback{std::move(p_callback)}
		{}
		~Cleanup()
		{
			m_callback();
		}
	};

	//------------------------------

	/*
		A TimerThread is used for timer callbacks. 
		The first time a callback is added, it spawns a thread that sleeps until the next callback should be called.
	*/
	template<typename MutexType = std::mutex>
	class TimerThread
	{
		using _Clock = chrono::steady_clock;

		class Timeout
		{
			std::function<void()> callback;
		public:
			auto operator()() const -> void
			{
				callback();
			}
			
			_Clock::time_point endTime;
			Id id = 0;

			auto operator=(Timeout const& p_other) -> Timeout&
			{
				callback = p_other.callback;
				endTime = p_other.endTime;
				id = p_other.id;
				return *this;
			}
			auto operator=(Timeout&& p_other) noexcept -> Timeout&
			{
				callback = std::move(p_other.callback);
				endTime = p_other.endTime;
				id = p_other.id;
				return *this;
			}

			template<typename Callable, typename DurationType, typename DurationPeriod>
			Timeout(Callable const& p_callback, chrono::duration<DurationType, DurationPeriod> p_duration, Id p_id) :
				callback{p_callback},
				endTime{_Clock::now() + chrono::duration_cast<_Clock::duration>(p_duration)},
				id{p_id}
			{
				static_assert(std::is_invocable_r_v<void, Callable>, "TimerThread::Timeout error: Timeout callback must be of type void().");
			}
			Timeout(Timeout const& p_other) = default;
			Timeout(Timeout&& p_other) noexcept = default;
		};

		std::vector<Timeout> m_timeouts;

		// Protects the m_timeouts vector because it is modified in different threads
		std::mutex m_timeoutsMutex;

		// This is a pointer to a user-provided mutex that should be locked during all timeout callbacks.
		MutexType* m_callbackMutex = nullptr;

		// The timeouts have their own IDs and this is used to generate new ones
		std::atomic<Count> m_idCounter = 1;

		std::atomic<bool> m_needsToWake = false;
		std::condition_variable m_wakeConditionVariable;
		std::mutex m_wakeMutex;

		std::atomic<bool> m_isRunning = false;
		std::thread m_thread;

		auto thread_run() -> void
		{
			m_isRunning = true;
			while (m_isRunning)
			{
				if (m_timeouts.empty())
				{
					// Just wait until a timeout is added.

					m_idCounter = 0;
					if (!m_needsToWake)
					{
						auto lock = std::unique_lock{m_wakeMutex};
						m_wakeConditionVariable.wait(lock, [&] { return static_cast<bool>(m_needsToWake); });
					}
					m_needsToWake = false;
				}
				else
				{
					// Wait until either a timeout is added or the timeout nearest in the future has ended.
					if (!m_needsToWake)
					{
						auto lock = std::unique_lock{m_wakeMutex};
						m_wakeConditionVariable.wait_until(lock, m_timeouts.begin()->endTime, [&] { return static_cast<bool>(m_needsToWake); });
					}
					m_needsToWake = false;

					if (m_timeouts.empty())
					{
						continue;
					}

					// Clear all of the timeouts that have ended and invoke their callbacks.

					auto const nextTimeout = std::find_if(
						m_timeouts.begin(), m_timeouts.end(), 
						[&](Timeout& timeout) {
							if (timeout.endTime >= _Clock::now())
							{
								return true;
							}
							if (m_callbackMutex)
							{
								auto const lock = std::scoped_lock{*m_callbackMutex};
								timeout();
							}
							else
							{
								timeout();
							}
							return false;
						}
					);
					auto const lock = std::scoped_lock{m_timeoutsMutex};
					m_timeouts.erase(m_timeouts.begin(), nextTimeout);

					// auto timeout = m_timeouts.begin();
					
					// while (timeout->endTime < _Clock::now())
					// {
					// 	if (m_callbackMutex)
					// 	{
					// 		auto const lock = std::scoped_lock{*m_callbackMutex};
					// 		(*timeout)();
					// 	}
					// 	else
					// 	{
					// 		(*timeout)();
					// 	}

					// 	if (++timeout == m_timeouts.end())
					// 	{
					// 		break;
					// 	}
					// }

				}
			}
		}

		auto wake() -> void
		{
			if (!m_needsToWake)
			{
				m_wakeMutex.lock();
				m_needsToWake = true;
				m_wakeMutex.unlock();
				m_wakeConditionVariable.notify_one();
			}
		}

		auto run() -> void
		{
			m_thread = std::thread{&TimerThread::thread_run, this};
			m_isRunning = true;
		}

	public:
		/*
			Adds a function that will be called in p_duration from now.
		*/
		template<typename Callable, typename DurationType, typename DurationPeriod>
		auto addCallback(Callable const& p_callback, chrono::duration<DurationType, DurationPeriod> p_duration) 
			-> Id
		{
			if (!m_isRunning)
			{
				run();
			}

			auto timeout = Timeout{p_callback, p_duration, m_idCounter++};
			{
				auto const lock = std::scoped_lock{m_timeoutsMutex};
				// Find the right position to keep the vector sorted.
				auto const position = std::lower_bound(
					m_timeouts.cbegin(), m_timeouts.cend(), timeout, 
					[](Timeout const& p_a, Timeout const& p_b) {
						return p_a.endTime < p_b.endTime; 
					}
				);
				m_timeouts.insert(position, timeout);
			}
			wake();
			return timeout.id;
		}
		template<typename Callable>
		auto addCallback(Callable const& p_callback, float p_milliseconds)
			-> Id
		{
			return addCallback(p_callback, chrono::duration<float, std::milli>{p_milliseconds});
		}

		auto cancelCallback(Id p_id) -> void
		{
			auto const lock = std::scoped_lock{m_timeoutsMutex};

			auto const position = std::find_if(
				m_timeouts.cbegin(), m_timeouts.cend(), 
				[=](Timeout const& p_timeout) {
					return p_timeout.id == p_id; 
				}
			);
			if (position != m_timeouts.end())
			{
				m_timeouts.erase(position);
			}
		}

		TimerThread() = default;
		/*
			p_callbackMutex is a mutex that is locked every time a timer callback is called.
		*/
		TimerThread(MutexType& p_callbackMutex) :
			m_callbackMutex{&p_callbackMutex}
		{
		}
		~TimerThread()
		{
			if (m_isRunning)
			{
				m_isRunning = false;
				wake();
				m_thread.join();
			}
		}
	};

	//------------------------------

	/*
		An object that inherits RefernceCounted doesn't get deleted until every remember() has a forget().
		The constructor is the first remember(), meaning m_referenceCount is initialized with 1.
		Don't use the delete operator with objects that are ReferenceCounted, use forget() instead.

		The use of this interface is 100% optional in AvoGUI, and should only be used in rare cases. 
		The idea is that if you want your own control over the lifetime of for example a view, 
		then you can use remember() and forget(). Everything allocated by the framework is managed by 
		the framework otherwise.
	*/
	class ReferenceCounted
	{
	private:
		std::atomic<Count> m_referenceCount = 1;

	public:
		ReferenceCounted() {}
		virtual ~ReferenceCounted() = default;

		/*
			Increments the reference count and returns the new reference count. Remembers a pointer reference.
		*/
		auto remember() -> Count
		{
			return ++m_referenceCount;
		}

		/*
			Decrements the reference count, returns the new reference count and deletes the object if the reference
			count has reached 0. Forgets a pointer reference.
		*/
		auto forget() -> Count
		{
			if (!--m_referenceCount)
			{
				delete this;
				return 0;
			}
			return m_referenceCount;
		}

		/*
			Returns the number of pointer references to the dynamically allocated object that have been remembered.
		*/
		auto getReferenceCount() -> Count
		{
			return m_referenceCount;
		}
	};

	/*
		Only used in the framework.
	*/
	// template<typename T>
	// class ProtectedReferenceCounted : protected ReferenceCounted
	// {
	// public:
	// 	virtual auto operator=(ProtectedReferenceCounted<T> const& p_other) -> ProtectedReferenceCounted<T>&
	// 	{
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->forget();
	// 		}
	// 		m_implementation = p_other.m_implementation;
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->remember();
	// 		}
	// 		return *this;
	// 	}
	// 	virtual auto operator=(ProtectedReferenceCounted<T>&& p_other) -> ProtectedReferenceCounted<T>&
	// 	{
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->forget();
	// 		}
	// 		m_implementation = p_other.m_implementation;
	// 		p_other.m_implementation = nullptr;
	// 		return *this;
	// 	}

	// 	auto operator==(ProtectedReferenceCounted<T> const& p_other) const -> bool
	// 	{
	// 		return m_implementation == p_other.m_implementation;
	// 	}
	// 	auto operator!=(ProtectedReferenceCounted<T> const& p_other) const -> bool
	// 	{
	// 		return m_implementation != p_other.m_implementation;
	// 	}

	// 	auto getIsValid() const -> bool
	// 	{
	// 		return m_implementation;
	// 	}
	// 	operator bool() const
	// 	{
	// 		return m_implementation;
	// 	}

	// 	auto destroy() -> void
	// 	{
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->forget();
	// 			m_implementation = nullptr;
	// 		}
	// 	}

	// protected:
	// 	T* m_implementation = nullptr;
	// public:
	// 	auto getImplementation() const noexcept -> T*
	// 	{
	// 		return m_implementation;
	// 	}

	// protected:
	// 	ProtectedReferenceCounted(T* p_implementation) noexcept :
	// 		m_implementation{p_implementation}
	// 	{
	// 	}

	// public:
	// 	ProtectedReferenceCounted() = default;
	// 	ProtectedReferenceCounted(ProtectedReferenceCounted<T> const& p_other) :
	// 		m_implementation{p_other.m_implementation}
	// 	{
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->remember();
	// 		}
	// 	}
	// 	ProtectedReferenceCounted(ProtectedReferenceCounted<T>&& p_other) noexcept :
	// 		m_implementation{p_other.m_implementation}
	// 	{
	// 		p_other.m_implementation = nullptr;
	// 	}
	// 	~ProtectedReferenceCounted()
	// 	{
	// 		if (m_implementation)
	// 		{
	// 			m_implementation->forget();
	// 		}
	// 	}
	// };

	//------------------------------

	// TODO: Remove manual reference counting from Component and make parent own a unique_ptr for every child instead.

	/*
		A component is an essential building block in an application.
		An application consists of a hierarchy of components.
		These components can be Views, however they don't need to be.
		Every component has its own responsibility, and using non-view components as well as view 
		components can help separate the concerns of an application.
		
		All components which have a parent require being dynamically allocated with the new keyword,
		and their memory is automatically managed by their parent so there is no need to call forget.
		The root component, however, doesn't have a parent and could be allocated on the stack.

		See View and Gui for more information.
	*/
	class Component : public ReferenceCounted
	{
	private:
		Component* m_root;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getRoot() const -> Component*
		{
			return m_root;
		}

	private:
		Component* m_parent = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto setParent(Component* p_parent) -> void
		{
			if (m_parent == p_parent)
			{
				return;
			}

			if (m_parent)
			{
				remember();
				m_parent->removeChild(this);
			}

			m_parent = p_parent;
			if (p_parent)
			{
				m_root = m_parent->getRoot();
				m_parent->m_children.push_back(this);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getParent() const -> Component*
		{
			return m_parent;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		template<typename T>
		auto getParent() const -> T*
		{
			return dynamic_cast<T*>(m_parent);
		}

	private:
		std::vector<Component*> m_children;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getChildren() const -> auto const&
		{
			return m_children;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getNumberOfChildren() const
		{
			return m_children.size();
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		template<typename T>
		auto getChild(Index p_index) const -> T*
		{
			return dynamic_cast<T*>(m_children[p_index]);
		}

		/*
			LIBRARY IMPLEMENTED
		*/
		auto addChild(Component* p_child) -> void
		{
			p_child->setParent(this);
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto removeChild(Index p_index) -> void
		{
			auto& child = m_children[p_index];
			child->m_parent = nullptr;
			child->parentChangeListeners(this);
			child->forget();

			child = m_children.back();
			m_children.pop_back();
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto removeChild(Component* p_child) -> void
		{
			if (auto const position = std::find(m_children.begin(), m_children.end(), p_child);
			    position != m_children.end())
			{
				removeChild(position - m_children.begin());
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto removeAllChildren() -> void
		{
			while (!m_children.empty())
			{
				auto const child = m_children.back();
				child->m_parent = nullptr;
				child->parentChangeListeners(this);
				child->forget();
				m_children.pop_back();
			}
		}

		/*
			Listener signature:
				void (Component* attachedChild)
			See Component::handleChildAttachment for more info.
		*/
		EventListeners<void(Component*)> childAttachmentListeners;
		/*
			USER IMPLEMENTED
			Gets called when a child component has been added to this component.
		*/
		virtual auto handleChildAttachment(Component* p_attachedChild) -> void {}

		/*
			Listener signature:
				void (Component* attachedChild)
			See Component::handleChildDetachment for more info.
		*/
		EventListeners<void(Component*)> childDetachmentListeners;
		/*
			USER IMPLEMENTED
			Gets called when a child component has been removed from this component.
		*/
		virtual auto handleChildDetachment(Component* p_detachedChild) -> void {}

		/*
			Listener signature:
				void (Component* oldParent)
			See Component::handleParentChange for more info.
		*/
		EventListeners<void(Component*)> parentChangeListeners;
		/*
			USER IMPLEMENTED
			Gets called when this component has been attached to a new component.
		*/
		virtual auto handleParentChange(Component* p_oldParent) -> void {}

	private:
		std::unordered_map<Count, Component*> m_componentsById;
		Component* m_idScope = nullptr;
		Id m_id = 0;
	public:
		/*
			Sets an ID that can be used to retrieve the component from the hierarchy.
			If p_id is 0, it is only removed from the scope.
			p_scope is the component that manages the ID of this component and is the topmost component 
			from which the ID of this component can be retrieved.
		*/
		auto setId(Id p_id, Component* p_scope) -> void
		{
			if (m_idScope)
			{
				if (auto const componentIterator = m_idScope->m_componentsById.find(m_id);
				    componentIterator != m_idScope->m_componentsById.end())
				{
					m_idScope->m_componentsById.erase(componentIterator);
				}
			}
			if (p_id)
			{
				p_scope->m_componentsById[p_id] = this;
				m_idScope = p_scope;
			}
		}
		auto setId(Id p_id) -> void
		{
			setId(p_id, getRoot());
		}
		/*
			Returns the ID that can be used to retrieve the component from the component hierarchy.
			The ID is invalid by default and converts to 0.
		*/
		auto getId() const -> Id
		{
			return m_id;
		}

		auto getComponentById(Id p_id) -> Component*
		{
			auto parent = this;
			while (parent)
			{
				if (auto componentIterator = parent->m_componentsById.find(p_id);
				    componentIterator == parent->m_componentsById.end())
				{
					parent = parent->m_parent;
				}
				else
				{
					return componentIterator->second;
				}
			}
			return nullptr;
		}
		template<typename T>
		auto getComponentById(Id p_id) -> T*
		{
			return dynamic_cast<T*>(getComponentById(p_id));
		}

	public:
		Component() :
			m_root{this}
		{
		}
		Component(Component* p_parent) :
			m_root{p_parent ? p_parent->getRoot() : this}
		{
			if (p_parent && p_parent != this)
			{
				setParent(p_parent);
			}
		}
		~Component()
		{
			removeAllChildren();

			setId(0);

			if (m_parent)
			{
				remember();
				setParent(nullptr);
			}
		}
	};

	//------------------------------

	template<typename _Type, typename _Class>
	struct Arithmetic
	{
		_Type value = 0;

        constexpr Arithmetic(_Class p_value) noexcept :
            value{static_cast<_Type>(p_value)}
        {}
		constexpr Arithmetic(_Type p_value) noexcept :
			value{p_value}
		{}

		constexpr explicit operator _Type() const noexcept
		{
			return value;
		}
		constexpr auto operator-() const noexcept -> Arithmetic<_Type, _Class>
		{
			return {-value};
		}

		constexpr auto operator=(_Class p_value) noexcept -> auto&
		{
			value = static_cast<_Type>(p_value);
			return *this;
		}
		constexpr auto operator+(_Class p_value) const noexcept -> Arithmetic<_Type, _Class>
		{
			return value + static_cast<_Type>(p_value);
		}
		constexpr auto operator+=(_Class const& p_value) noexcept -> auto&
		{
			value += static_cast<_Type>(p_value);
			return *this;
		}
		constexpr auto operator-(_Class p_value) const noexcept -> Arithmetic<_Type, _Class>
		{
			return value - static_cast<_Type>(p_value);
		}
		constexpr auto operator-=(_Class p_value) noexcept -> auto&
		{
			value -= static_cast<_Type>(p_value);
			return *this;
		}
		constexpr auto operator*(_Class p_value) const noexcept -> Arithmetic<_Type, _Class>
		{
			return value*static_cast<_Type>(p_value);
		}
		constexpr auto operator*=(_Class p_value) noexcept -> auto&
		{
			value *= static_cast<_Type>(p_value);
			return *this;
		}
		constexpr auto operator/(_Class p_value) const noexcept -> Arithmetic<_Type, _Class>
		{
			return value/static_cast<_Type>(p_value);
		}
		constexpr auto operator/=(_Class p_value) noexcept -> auto&
		{
			value /= static_cast<_Type>(p_value);
			return *this;
		}

		constexpr auto operator==(_Class p_value) const noexcept -> bool
		{
			return value == static_cast<_Type>(p_value);
		}
		constexpr auto operator!=(_Class p_value) const noexcept -> bool
		{
			return value != static_cast<_Type>(p_value);
		}

		constexpr auto operator<(_Class p_value) const noexcept -> bool
		{
			return value < static_cast<_Type>(p_value);
		}
		constexpr auto operator>(_Class p_value) const noexcept -> bool
		{
			return value > static_cast<_Type>(p_value);
		}
		constexpr auto operator<=(_Class p_value) const noexcept -> bool
		{
			return value <= static_cast<_Type>(p_value);
		}
		constexpr auto operator>=(_Class p_value) const noexcept -> bool
		{
			return value >= static_cast<_Type>(p_value);
		}
	};

	struct Degrees;
	struct Radians : Arithmetic<float, Radians>
	{
		/*
			Returns the angle in normalized units where 360 degrees = tau radians = 1 normalized.
		*/
		constexpr auto getNormalized() noexcept -> float
		{
			return value/TAU<float>;
		}
		/*
			Sets the angle in normalized units where 360 degrees = tau radians = 1 normalized.
		*/
		constexpr auto setNormalized(float p_value) noexcept -> void
		{
			value = p_value*TAU<float>;
		}
		
        template<typename _Type, typename _Class>
        constexpr Radians(Arithmetic<_Type, _Class> p_arithmetic) noexcept :
            Arithmetic{static_cast<float>(p_arithmetic)}
        {}
        template<typename _Type>
        constexpr Radians(Arithmetic<_Type, Degrees> p_arithmetic) noexcept;
		template<typename T>
        constexpr Radians(FloatingPointType<T> p_radians) noexcept :
            Arithmetic{static_cast<float>(p_radians)}
        {}
	};
	struct Degrees : Arithmetic<float, Degrees>
	{
		/*
			Returns the angle in normalized units where 360 degrees = tau radians = 1 normalized.
		*/
		constexpr auto getNormalized() noexcept -> float
		{
			return value/360.f;
		}
		/*
			Sets the angle in normalized units where 360 degrees = tau radians = 1 normalized.
		*/
		constexpr auto setNormalized(float p_value) noexcept -> void
		{
			value = p_value*360.f;
		}
		
        template<typename _Type, typename _Class>
        constexpr Degrees(Arithmetic<_Type, _Class> p_arithmetic) noexcept :
            Arithmetic{static_cast<float>(p_arithmetic)}
        {}
        template<typename _Type>
		constexpr Degrees(Arithmetic<_Type, Radians> p_radians) noexcept :
			Arithmetic{p_radians.value*180.f/PI<float>}
		{}
		template<typename T>
        constexpr Degrees(FloatingPointType<T> p_degrees) noexcept :
            Arithmetic{static_cast<float>(p_degrees)}
        {}
	};
	template<typename _Type>
	constexpr Radians::Radians(Arithmetic<_Type, Degrees> p_degrees) noexcept :
		Arithmetic{p_degrees.value/180.f*PI<float>}
	{}

    template<typename T, typename = std::enable_if_t<std::is_same_v<T, Degrees> || std::is_same_v<T, Radians>>>
    using Angle = T;

	inline namespace literals
	{
    constexpr auto operator"" _deg(long double p_value) noexcept -> Degrees
    {
        return Degrees{static_cast<float>(p_value)};
	}
    constexpr auto operator"" _deg(unsigned long long p_value) noexcept -> Degrees
    {
        return Degrees{static_cast<float>(p_value)};
	}
    constexpr auto operator"" _rad(long double p_value) noexcept -> Radians
    {
        return Radians{static_cast<float>(p_value)};
	}
    constexpr auto operator"" _rad(unsigned long long p_value) noexcept -> Radians
    {
        return Radians{static_cast<float>(p_value)};
	}
	}

	//------------------------------

	template<typename _Type, template<typename> typename _Class>
	struct Vector2dBase
	{
		static_assert(std::is_arithmetic_v<_Type>, "Vector2dBase error: type must be arithmetic.");

		_Type x = 0, y = 0;

		constexpr Vector2dBase() noexcept = default;
		// template<typename T>
		// constexpr Vector2dBase(Vector2dBase<T, _Class> p_vector) noexcept :
		// 	x{static_cast<_Type>(p_vector.x)},
		// 	y{static_cast<_Type>(p_vector.y)}
		// {}
		template<typename T, template<typename>typename C>
		constexpr explicit Vector2dBase(Vector2dBase<T, C> p_vector) noexcept :
			x{static_cast<_Type>(p_vector.x)},
			y{static_cast<_Type>(p_vector.y)}
		{}
		constexpr Vector2dBase(_Type p_x, _Type p_y) noexcept :
			x{p_x},
			y{p_y}
		{}
		constexpr Vector2dBase(_Type p_coordinate) noexcept :
			x{p_coordinate},
			y{p_coordinate}
		{}

		//------------------------------

		template<typename T>
		constexpr operator _Class<T>() const noexcept
		{
			return {static_cast<T>(x), static_cast<T>(y)};
		}

		constexpr explicit operator bool() const noexcept
		{
			return x || y;
		}

		constexpr auto operator-() const noexcept -> _Class<_Type>
		{
			return {-x, -y};
		}

		//------------------------------

		/*
			Sets the polar coordinates of the vector.
			p_angle is the angle between the ray to the vector and the x-axis on the right-hand side, clockwise in our coordinate system.
			p_length is the distance from the origin of the coordinates.
		*/
		template<typename _AngleType>
		static auto fromPolar(Angle<_AngleType> p_angle, double p_length) noexcept -> _Class<_Type>
		{
			auto radians = Radians{p_angle}.value;
			return {static_cast<_Type>(std::cos(radians)*p_length), static_cast<_Type>(std::sin(radians)*p_length)};
		}
		/*
			Sets the polar coordinates of the vector, with length of 1.
			p_angle is the angle between the ray to the vector and the x-axis on the right-hand side, clockwise in our coordinate system.
		*/
		template<typename _AngleType>
		static auto fromPolar(Angle<_AngleType> p_angle) noexcept -> _Class<_Type>
		{
			auto radians = Radians{p_angle}.value;
			return {static_cast<_Type>(std::cos(radians)), static_cast<_Type>(std::sin(radians))};
		}

		//------------------------------
		// Spaceship operator please come

		template<typename T, template<typename>typename C>
		constexpr auto operator==(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x == p_vector.x && y == p_vector.y;
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator!=(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x != p_vector.x || y != p_vector.y;
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator>=(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x >= p_vector.x && y >= p_vector.y;
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator>(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x > p_vector.x && y > p_vector.y;
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator<=(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x <= p_vector.x && y <= p_vector.y;
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator<(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return x < p_vector.x && y < p_vector.y;
		}

		template<typename T>
		constexpr auto operator==(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x == p_value && y == p_value;
		}
		template<typename T>
		constexpr auto operator!=(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x != p_value || y != p_value;
		}
		template<typename T>
		constexpr auto operator>=(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x >= p_value && y >= p_value;
		}
		template<typename T>
		constexpr auto operator>(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x > p_value && y > p_value;
		}
		template<typename T>
		constexpr auto operator<=(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x <= p_value && y <= p_value;
		}
		template<typename T>
		constexpr auto operator<(ArithmeticType<T> p_value) const noexcept -> bool
		{
			return x < p_value && y < p_value;
		}

		//------------------------------

		template<typename T, template<typename>typename C>
		constexpr auto operator+(Vector2dBase<T, C> p_value) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x + p_value.x, y + p_value.y};
		}
		constexpr auto operator+(_Type p_value) const noexcept -> _Class<_Type>
		{
			return {x + p_value, y + p_value};
		}
		template<typename T, template<typename>typename C>
		constexpr auto operator+=(Vector2dBase<T, C> p_value) noexcept -> _Class<_Type>&
		{
			x += p_value.x;
			y += p_value.y;
			return static_cast<_Class<_Type>&>(*this);
		}
		constexpr auto operator+=(_Type p_value) noexcept -> _Class<_Type>&
		{
			x += p_value;
			y += p_value;
			return static_cast<_Class<_Type>&>(*this);
		}

		//------------------------------

		/*
			Returns a version of this vector that is offset negatively by the same amount on the x- and y-axis.
		*/
		template<typename T, template<typename>typename C>
		constexpr auto operator-(Vector2dBase<T, C> p_value) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x - p_value.x, y - p_value.y};
		}
		constexpr auto operator-(_Type p_value) const noexcept -> _Class<_Type>
		{
			return {x - p_value, y - p_value};
		}

		template<typename T, template<typename>typename C>
		constexpr auto operator-=(Vector2dBase<T, C> p_value) noexcept -> _Class<_Type>&
		{
			x -= p_value.x;
			y -= p_value.y;
			return static_cast<_Class<_Type>&>(*this);
		}
		constexpr auto operator-=(_Type p_value) noexcept -> _Class<_Type>&
		{
			x -= p_value;
			y -= p_value;
			return static_cast<_Class<_Type>&>(*this);
		}

		//------------------------------

		template<typename T, template<typename>typename C>
		constexpr auto operator*(Vector2dBase<T, C> p_factor) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x*p_factor.x, y*p_factor.y};
		}
		template<typename T>
		constexpr auto operator*(ArithmeticType<T> p_factor) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x*p_factor, y*p_factor};
		}

		template<typename T, template<typename>typename C>
		constexpr auto operator*=(Vector2dBase<T, C> p_factor) noexcept -> _Class<_Type>&
		{
			x *= p_factor.x;
			y *= p_factor.y;
			return static_cast<_Class<_Type>&>(*this);
		}
		template<typename T>
		constexpr auto operator*=(ArithmeticType<T> p_factor) noexcept -> _Class<_Type>&
		{
			x *= p_factor;
			y *= p_factor;
			return static_cast<_Class<_Type>&>(*this);
		}

		//------------------------------

		template<typename T, template<typename>typename C>
		constexpr auto operator/(Vector2dBase<T, C> p_divisor) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x/p_divisor.x, y/p_divisor.y};
		}
		template<typename T>
		constexpr auto operator/(ArithmeticType<T> p_divisor) const noexcept -> _Class<std::common_type_t<_Type, T>>
		{
			return {x/p_divisor, y/p_divisor};
		}

		template<typename T, template<typename>typename C>
		constexpr auto operator/=(Vector2dBase<T, C> p_divisor) noexcept -> _Class<_Type>&
		{
			x /= p_divisor.x;
			y /= p_divisor.y;
			return static_cast<_Class<_Type>&>(*this);
		}
		template<typename T>
		constexpr auto operator/=(ArithmeticType<T> p_divisor) noexcept -> _Class<_Type>&
		{
			x /= p_divisor;
			y /= p_divisor;
			return static_cast<_Class<_Type>&>(*this);
		}

		//------------------------------

		template<typename T, template<typename>typename C>
		constexpr auto getDotProduct(Vector2dBase<T, C> p_vector) const noexcept
		{
			return x*p_vector.x + y*p_vector.y;
		}

		template<typename T, template<typename>typename C>
		constexpr auto getCrossProduct(Vector2dBase<T, C> p_vector) const noexcept
		{
			return x*p_vector.x - y*p_vector.x;
		}

		//------------------------------

		/*
			Calculates the length of the 2d vector with pythagorean theorem.
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		constexpr auto getLengthSquared() const noexcept -> _Type
		{
			return x*x + y*y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of the 2d vector with pythagorean teorem.
		*/
		auto getLength() const noexcept -> double
		{
			return std::hypot(x, y);
		}

		/*
			Calculates the distance between this vector and another vector with pythagorean theorem.
			This is faster than getDistance() since no square root is needed, so use this one when you can!
		*/
		template<typename T, template<typename>typename C>
		constexpr auto getDistanceSquared(Vector2dBase<T, C> p_vector) const noexcept
		{
			return (x - p_vector.x)*(x - p_vector.x) + (y - p_vector.y)*(y - p_vector.y);
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between this vector and another vector with pythagorean theorem.
		*/
		template<typename T, template<typename>typename C>
		auto getDistance(Vector2dBase<T, C> p_vector) const noexcept -> double
		{
			return std::hypot(x - p_vector.x, y - p_vector.y);
		}

		//------------------------------

		/*
			Calculates the length of a 2d vector with pythagorean theorem.
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1>
		static constexpr auto getLengthSquared(T0 p_x, T1 p_y) noexcept
		{
			return p_x*p_x + p_y*p_y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of a 2d vector with pythagorean teorem.
		*/
		template<typename T0, typename T1>
		static auto getLength(T0 p_x, T1 p_y) noexcept -> double
		{
			return std::hypot(p_x, p_y);
		}

		/*
			Calculates the distance between two vectors with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1>
		static constexpr auto getDistanceSquared(_Class<T0> p_vector_0, _Class<T1> p_vector_1) noexcept
		{
			return (p_vector_1.x - p_vector_0.x)*(p_vector_1.x - p_vector_0.x) + (p_vector_1.y - p_vector_0.y)*(p_vector_1.y - p_vector_0.y);
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between two vectors with pytagorean theorem.
		*/
		template<typename T0, typename T1>
		static auto getDistance(_Class<T0> p_vector_0, _Class<T1> p_vector_1) noexcept -> double
		{
			return std::hypot(p_vector_1.x - p_vector_0.x, p_vector_1.y - p_vector_0.y);
		}

		//------------------------------

		/*
			Rotates the vector clockwise (our coordinate system) by p_angle so that it keeps its length.
		*/
		template<typename _AngleType>
		auto rotate(Angle<_AngleType> p_angle) noexcept -> _Class<_Type>&
		{
			_Type xBefore = x;
			auto radians = Radians{p_angle}.value;
			double cos = std::cos(radians);
			double sin = std::sin(radians);
			x = x*cos - y*sin;
			y = y*cos + xBefore*sin;
			return static_cast<_Class<_Type>&>(*this);
		}
		/*
			Rotates the vector clockwise (our coordinate system) relative to p_origin by p_angle so that it keeps its distance from p_origin.
		*/
		template<typename _AngleType, typename T, template<typename>typename C>
		auto rotate(Angle<_AngleType> p_angle, Vector2dBase<T, C> p_origin) noexcept -> _Class<_Type>&
		{
			_Type xBefore = x;
			auto radians = Radians{p_angle}.value;
			double cos = std::cos(radians);
			double sin = std::sin(radians);
			x = (x - p_origin.x)*cos - (y - p_origin.y)*sin + p_origin.x;
			y = (y - p_origin.y)*cos + (xBefore - p_origin.x)*sin + p_origin.y;
		}

		/*
			Rotates the vector so that its angle is equal to p_angle, clockwise and relative to the x-axis on the right-hand side.
		*/
		template<typename _AngleType>
		auto setAngle(Angle<_AngleType> p_angle) noexcept -> _Class<_Type>&
		{
			double length = getLength();
			auto radians = Radians{p_angle}.value;
			x = std::cos(radians)*length;
			y = std::sin(radians)*length;
			return static_cast<_Class<_Type>&>(*this);
		}
		/*
			Rotates the vector so that its angle relative to p_origin is p_angle.
			The angle is clockwise in our coordinate system and relative to the x-axis on the right-hand side.
		*/
		template<typename _AngleType, typename T, template<typename>typename C>
		auto setAngle(Angle<_AngleType> p_angle, Vector2dBase<T, C> p_origin) noexcept -> _Class<_Type>&
		{
			double length = getDistance(p_origin.x, p_origin.y);
			auto radians = Radians{p_angle}.value;
			x = std::cos(radians)*length + p_origin.x;
			y = std::sin(radians)*length + p_origin.y;
			return static_cast<_Class<_Type>&>(*this);
		}

		/*
			Returns the clockwise angle between the ray to the vector and the x-axis 
			on the right-hand side, in the range [0, 2pi].
		*/
		template<typename _AngleType>
		auto getAngle() const noexcept -> Angle<_AngleType>
		{
			if (!x && !y)
			{
				return 0.;
			}
			double atan2 = std::atan2(y, x);
			if (atan2 < 0.)
			{
				return Radians{atan2 + TAU<float>};
			}
			return Radians{atan2};
		}
		/*
			Returns the clockwise angle between the ray to the vector and the x-axis on the right hand side, relative to p_origin.
			Angle is in the range [0, 2pi].
		*/
		template<typename _AngleType, typename T, template<typename>typename C>
		auto getAngle(Vector2dBase<T, C> p_origin) const noexcept -> Angle<_AngleType>
		{
			if (x == p_origin.x && y == p_origin.y)
			{
				return 0.;
			}
			auto atan2 = std::atan2(y - p_origin.y, x - p_origin.x);
			if (atan2 < 0.)
			{
				return Radians{atan2 + TAU};
			}
			return Radians{atan2};
		}

		//------------------------------

		/*
			Uses an accurate but slower method to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		auto normalize() noexcept -> _Class<_Type>&
		{
			auto length = sqrt(x*x + y*y);
			x /= length;
			y /= length;
			return static_cast<_Class<_Type>&>(*this);
		}
		/*
			Uses a fast but less accurate method to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		auto normalizeFast() -> _Class<_Type>&
		{
			auto inverseLength = fastInverseSqrt(x*x + y*y);
			x *= inverseLength;
			y *= inverseLength;
			return static_cast<_Class<_Type>&>(*this);
		}

		//------------------------------

		/*
			Returns whether p_vector is inside the rectangle formed from the origin to this vector.
		*/
		template<typename T, template<typename>typename C>
		constexpr auto getIsContaining(Vector2dBase<T, C> p_vector) const noexcept -> bool
		{
			return p_vector.x >= 0 && p_vector.y >= 0 && p_vector.x < x && p_vector.y < y;
		}
	};

	template<
		typename T0, typename T1, 
		template<typename> typename _Vector, 
		typename = std::enable_if_t<std::is_base_of_v<Vector2dBase<T1, _Vector>, _Vector<T1>>>
	>
	constexpr auto operator*(ArithmeticType<T0> p_factor, _Vector<T1> p_vector) noexcept -> _Vector<std::common_type_t<T0, T1>>
	{
		return {p_vector.x*p_factor, p_vector.y*p_factor};
	}
	template<
		typename T0, typename T1, 
		template<typename> typename _Vector, 
		typename = std::enable_if_t<std::is_base_of_v<Vector2dBase<T1, _Vector>, _Vector<T1>>>
	>
	constexpr auto operator/(ArithmeticType<T0> p_dividend, _Vector<T1> p_vector) noexcept -> _Vector<std::common_type_t<T0, T1>>
	{
		return {p_dividend/p_vector.x, p_dividend/p_vector.y};
	}

	template<
		typename T0, typename T1, 
		template<typename> typename _Vector, 
		typename = std::enable_if_t<std::is_base_of_v<Vector2dBase<T1, _Vector>, _Vector<T1>>>
	>
	constexpr auto operator<(ArithmeticType<T0> p_coordinate, _Vector<T1> p_vector) noexcept -> bool
	{
		return p_coordinate < p_vector.x && p_coordinate < p_vector.y;
	}
	template<
		typename T0, typename T1, 
		template<typename> typename _Vector, 
		typename = std::enable_if_t<std::is_base_of_v<Vector2dBase<T1, _Vector>, _Vector<T1>>>
	>
	constexpr auto operator>(ArithmeticType<T0> p_coordinate, _Vector<T1> p_vector) noexcept -> bool
	{
		return p_coordinate > p_vector.x && p_coordinate > p_vector.y;
	}
	/*
		Linearly interpolates between p_start and p_end. This means we are calculating a vector on the line segment between the two vectors.
		If p_progress is 0, p_start is returned. If p_progress is 1, p_end is returned.
		If p_progress is outside the range of [0, 1] then a vector on the line that is defined by the two vectors will still be returned,
		but outside of the line segment between them.
	*/
	template<
		typename T0, typename T1, typename T2,
		template<typename> typename _Vector, 
		typename = std::enable_if_t<std::is_base_of_v<Vector2dBase<T0, _Vector>, _Vector<T0>>>
	>
	constexpr auto interpolate(_Vector<T0> p_start, _Vector<T1> p_end, ArithmeticType<T2> p_progress) noexcept
		-> _Vector<std::common_type_t<T0, T1, T2>>
	{
		return p_start*(1 - p_progress) + p_end*p_progress;
	}

	//------------------------------

	/*
		Represents a 2D vector. 
		Use this type for anything you'd visualize with an arrow; speed, acceleration and so on.
		Can also be used for any two-dimensional number.
		Use Point and Size instead of Vector2d if you need 2d coordinates or a size.
	*/
	template<typename _Type = Dip>
	struct Vector2d : Vector2dBase<_Type, Vector2d>
	{		
		constexpr Vector2d() noexcept = default;
		constexpr Vector2d(_Type p_x, _Type p_y) noexcept :
			Vector2dBase<_Type, Vector2d>{p_x, p_y}
		{}
		/*
			Initializes the vector with the same x and y.
		*/
		constexpr Vector2d(_Type p_value) noexcept :
			Vector2dBase<_Type, Vector2d>{p_value}
		{}
		// template<typename T>
		// constexpr Vector2d(Vector2d<T> p_vector) noexcept :
		// 	Vector2dBase<T, Vector2d>{p_vector}
		// {}
        template<typename T, template<typename>typename C>
        constexpr explicit Vector2d(C<T> p_other) :
            Vector2dBase<_Type, Vector2d>{p_other}
        {}
	};
	
	//------------------------------

	/*
		Represents a 2D point where x is the horizontal component and y is the 
		vertical component if you were to think of it graphically.
		Used it for coordinates, not general 2D vectors or sizes.
		The coordinate system used throughout AvoGUI is one where the positive 
		y-direction is downwards and the positive x-direction is to the right.
	*/
	template<typename _Type = Dip>
	struct Point : Vector2dBase<_Type, Point>
	{
		constexpr Point() noexcept = default;
		constexpr Point(_Type p_x, _Type p_y) noexcept :
			Vector2dBase<_Type, Point>{p_x, p_y}
		{}
		/*
			Initializes the point with the same x and y coordinates.
		*/
		constexpr Point(_Type p_coordinate) noexcept :
			Vector2dBase<_Type, Point>{p_coordinate}
		{}
		// template<typename T>
		// constexpr Point(Point<T> p_point) noexcept :
		// 	Vector2dBase<T, Vector2d>{p_point}
		// {}
        template<typename T, template<typename>typename C>
        constexpr explicit Point(C<T> p_other) :
            Vector2dBase<_Type, Point>{p_other}
        {}
	};


	//------------------------------

	/*
		Represents a 2D size with a width and height.
		Use this instead of Vector2d or Point when it should represent a 
		size and not point coordinates or a general 2D vector.
	*/
	template<typename _Type = Dip>
	struct Size : Vector2dBase<_Type, Size>
	{
		constexpr Size() noexcept = default;
		constexpr Size(_Type p_width, _Type p_height) noexcept :
			Vector2dBase<_Type, Size>{p_width, p_height}
		{}
		/*
			Initializes the size with the same width and height.
			Creates a square size.
		*/
		constexpr Size(_Type p_side) noexcept :
			Vector2dBase<_Type, Size>{p_side}
		{}
        template<typename T, template<typename>typename C>
        constexpr explicit Size(C<T> p_other) :
            Vector2dBase<_Type, Size>{p_other}
        {}

        constexpr auto operator=(Size<_Type> p_other) noexcept
        {
            width = p_other.x;
            height = p_other.y;
            return *this;
		}

		_Type& width = Vector2dBase<_Type, Size>::x;
		_Type& height = Vector2dBase<_Type, Size>::y;
	};

	//------------------------------

	struct Transform
	{
		/*
			These are the transform coefficients.
		*/
		Factor xToX = 1.f, yToX = 0.f, offsetX = 0.f,
		       xToY = 0.f, yToY = 1.f, offsetY = 0.f;

		//------------------------------

		/*
			Applies the transform to a vector.
		*/
		template<typename T, template<typename> typename C>
		constexpr auto operator*(Vector2dBase<T, C> p_vector) const noexcept -> Vector2dBase<T, C>
		{
			return {
				xToX*p_point.x + yToX*p_point.y + offsetX, 
				yToY*p_point.y + xToY*p_point.x + offsetY
			};
		}
		/*
			Returns p_transform applied to this transform.
			Note that the order of multiplication matters. 
			The right hand side is applied afterwards, meaning that the multiplication a*b is actually mathematically b*a.
		*/
		constexpr auto operator*(Transform const& p_transform) const noexcept -> Transform
		{
			return {
				p_transform.xToX*xToX + p_transform.yToX*xToY,
				p_transform.xToX*yToX + p_transform.yToX*yToY,
				p_transform.xToX*offsetX + p_transform.yToX*offsetY,
				p_transform.xToY*xToX + p_transform.yToY*xToY,
				p_transform.xToY*yToX + p_transform.yToY*yToY,
				p_transform.xToY*offsetX + p_transform.yToY*offsetY
			};
		}
		/*
			Applies another transform to this one.
			Note that the order of multiplication matters.
			The right hand side is applied afterwards, meaning that the multiplication a *= b is actually mathematically a2 = b*a.
		*/
		constexpr auto operator*=(Transform const& p_transform) noexcept -> Transform&
		{
			return *this = *this*p_transform;
		}

		//------------------------------

		/*
			Rotates transformed points around the origin by an angle.
		*/
		template<typename T>
		auto rotate(Angle<T> p_angle) noexcept -> Transform&
		{
			/*
				[cos -sin  0]   [xToX yToX offsetX]
				[sin  cos  0] * [xToY yToY offsetY]
				[0    0    1]   [0    0    0      ]
			*/
			auto radians = Radians{p_angle}.value;
			float cos = std::cos(radians);
			float sin = std::sin(radians);

			// Previous transformation
			auto p = *this;

			xToX = cos*p.xToX - sin*p.xToY;
			yToX = cos*p.yToX - sin*p.yToY;
			offsetX = cos*p.offsetX - sin*p.offsetY;
			xToY = sin*p.xToX + cos*p.xToY;
			yToY = sin*p.yToX + cos*p.yToY;
			offsetY = sin*p.offsetX + cos*p.offsetY;

			return *this;
		}
		/*
			Rotates transformed points around p_origin by an angle.
		*/
		template<typename T>
		auto rotate(Angle<T> p_angle, Point<> p_origin) noexcept -> Transform&
		{
			offsetX -= p_origin.x;
			offsetY -= p_origin.y;
			rotate(p_angle);
			offsetX += p_origin.x;
			offsetY += p_origin.y;
			return *this;
		}

		/*
			Moves the translation by (p_dx, p_dy).
		*/
		constexpr auto translate(Vector2d<> p_offset) noexcept -> Transform&
		{
			offsetX += p_offset.x, offsetY += p_offset.y;
			return *this;
		}
		/*
			Sets the absolute offset in coordinates caused by the transform.
		*/
		constexpr auto setTranslation(Point<> p_point) noexcept -> Transform&
		{
			offsetX = p_point.x, offsetY = p_point.y;
			return *this;
		}

		/*
			Scales the transform by a horizontal and vertical factor.
		*/
		constexpr auto scale(Vector2d<Factor> p_scaleFactor) noexcept -> Transform&
		{
			xToX *= p_scaleFactor.x;
			yToX *= p_scaleFactor.x;
			yToY *= p_scaleFactor.y;
			xToY *= p_scaleFactor.y;
			return *this;
		}
		constexpr auto scaleX(float p_scaleFactor) noexcept -> Transform&
		{
			xToX *= p_scaleFactor;
			yToX *= p_scaleFactor;
			return *this;
		}
		constexpr auto scaleY(float p_scaleFactor) noexcept -> Transform&
		{
			yToY *= p_scaleFactor;
			xToY *= p_scaleFactor;
			return *this;
		}
	};

	//------------------------------

	class ProtectedRectangle;

	/*
		A 2D axis-aligned rectangle. right > left and bottom > top.
		Increasingly positive values for bottom and top will move the rectangle downwards and increasingly positive
		values for left and right will move the rectangle to the right (when used in the AvoGUI framework).
	*/
	template<typename _Type = Dip>
	struct Rectangle
	{
		_Type left = 0, top = 0, right = 0, bottom = 0;

		constexpr Rectangle() = default;
		constexpr Rectangle(_Type p_left, _Type p_top, 
		                    _Type p_right, _Type p_bottom) noexcept :
			left{p_left},
			top{p_top},
			right{p_right},
			bottom{p_bottom}
		{}
		/*
			Initializes a rectangle from a width and a height.
		*/
		constexpr Rectangle(_Type p_width, _Type p_height) noexcept :
			right{p_width},
			bottom{p_height}
		{}
		constexpr Rectangle(Point<_Type> p_position) noexcept :
			left{p_position.x},
			top{p_position.y},
			right{p_position.x},
			bottom{p_position.y}
		{}
		constexpr Rectangle(Size<_Type> p_size) noexcept :
			right{p_size.width},
			bottom{p_size.height}
		{}
		constexpr Rectangle(Point<_Type> p_position, Size<_Type> p_size) noexcept :
			left{p_position.x},
			top{p_position.y},
			right{p_position.x + p_size.width},
			bottom{p_position.y + p_size.height}
		{}
		template<typename T>
		constexpr Rectangle(Rectangle<T> p_rectangle) noexcept :
			left{static_cast<_Type>(p_rectangle.left)},
			top{static_cast<_Type>(p_rectangle.top)},
			right{static_cast<_Type>(p_rectangle.right)},
			bottom{static_cast<_Type>(p_rectangle.bottom)}
		{
		}

		//------------------------------

		constexpr auto operator=(Rectangle<_Type> p_rectangle) noexcept -> auto&
		{
			left = p_rectangle.left;
			top = p_rectangle.top;
			right = p_rectangle.right;
			bottom = p_rectangle.bottom;
			return *this;
		}

		/*
			Makes the size >= 0 like this:
			If right < left, right = left.
			If bottom < top, bottom = top.
		*/
		constexpr auto clipNegativeSpace() noexcept -> Rectangle<_Type>&
		{
			if (right < left) right = left;
			if (bottom < top) bottom = top;
			return *this;
		}

		//------------------------------

		/*
			Offsets the position of the rectangle.
		*/
		constexpr auto operator+=(Vector2d<_Type> p_offset) noexcept -> auto&
		{
			left += p_offset.x;
			top += p_offset.y;
			right += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		constexpr auto operator+=(Point<_Type> p_offset) noexcept -> auto&
		{
			return operator+=(Vector2d<_Type>{p_offset});
		}
		/*
			Offsets the position of the rectangle negatively.
		*/
		constexpr auto operator-=(Vector2d<_Type> p_offset) noexcept -> auto&
		{
			left -= p_offset.x;
			top -= p_offset.y;
			right -= p_offset.x;
			bottom -= p_offset.y;
			return *this;
		}
		constexpr auto operator-=(Point<_Type> p_offset) noexcept -> auto&
		{
			return operator-=(Vector2d<_Type>{p_offset});
		}

		template<typename T>
		constexpr auto operator+=(Rectangle<T> p_other) noexcept -> auto&
		{
			left += p_other.left;
			top += p_other.top;
			right += p_other.right;
			bottom += p_other.bottom;
			return *this;
		}
		template<typename T>
		constexpr auto operator-=(Rectangle<T> p_other) noexcept -> auto&
		{
			left -= p_other.left;
			top -= p_other.top;
			right -= p_other.right;
			bottom -= p_other.bottom;
			return *this;
		}

		//------------------------------

		// Note: am not providing addition/subtraction operators for 
		// Sizes because it would not be obvious to a user what they do.
		// Offset only the size or interpret it as a vector and offset the position?

		template<typename T>
		constexpr auto operator+(Vector2d<T> p_offset) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left + p_offset.x, top + p_offset.y, right + p_offset.x, bottom + p_offset.y};
		}
		template<typename T>
		constexpr auto operator+(Point<T> p_offset) const noexcept
		{
			return operator+(Vector2d<T>{p_offset});
		}
		template<typename T>
		constexpr auto operator-(Vector2d<T> p_offset) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left - p_offset.x, top - p_offset.y, right - p_offset.x, bottom - p_offset.y};
		}
		template<typename T>
		constexpr auto operator-(Point<T> p_offset) const noexcept
		{
			return operator-(Vector2d<T>{p_offset});
		}

		template<typename T>
		constexpr auto operator+(Rectangle<T> p_other) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left + p_other.left, top + p_other.top, right + p_other.right, bottom + p_other.bottom};
		}
		template<typename T>
		constexpr auto operator-(Rectangle<T> p_other) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left - p_other.left, top - p_other.top, right - p_other.right, bottom - p_other.bottom};
		}

		//------------------------------

		template<typename T>
		constexpr auto operator*=(ArithmeticType<T> p_factor) const noexcept -> auto&
		{
			left *= p_factor;
			top *= p_factor;
			right *= p_factor;
			bottom *= p_factor;
			return *this;
		}
		template<typename T>
		constexpr auto operator/=(ArithmeticType<T> p_divisor) const noexcept -> auto&
		{
			left /= p_divisor;
			top /= p_divisor;
			right /= p_divisor;
			bottom /= p_divisor;
			return *this;
		}
		
		template<typename T>
		constexpr auto operator*=(Vector2d<T> p_factor) const noexcept -> auto&
		{
			left *= p_factor.x;
			top *= p_factor.y;
			right *= p_factor.x;
			bottom *= p_factor.y;
			return *this;
		}		
		template<typename T>
		constexpr auto operator/=(Vector2d<T> p_divisor) const noexcept -> auto&
		{
			left /= p_divisor.x;
			top /= p_divisor.y;
			right /= p_divisor.x;
			bottom /= p_divisor.y;
			return *this;
		}

		//------------------------------

		template<typename T>
		constexpr auto operator*(ArithmeticType<T> p_factor) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left*p_factor, top*p_factor, right*p_factor, bottom*p_factor};
		}
		template<typename T>
		constexpr auto operator/(ArithmeticType<T> p_divisor) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left/p_divisor, top/p_divisor, right/p_divisor, bottom/p_divisor};
		}
		
		template<typename T>
		constexpr auto operator*(Vector2d<T> p_factor) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left*p_factor.x, top*p_factor.y, right*p_factor.x, bottom*p_factor.y};
		}		
		template<typename T>
		constexpr auto operator/(Vector2d<T> p_divisor) const noexcept -> Rectangle<std::common_type_t<_Type, T>>
		{
			return {left/p_divisor.x, top/p_divisor.y, right/p_divisor.x, bottom/p_divisor.y};
		}
		
		//------------------------------

		template<typename T>
		constexpr auto operator==(Rectangle<T> p_rectangle) const noexcept -> bool
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top   && bottom == p_rectangle.bottom;
		}

		template<typename T>
		constexpr auto operator!=(Rectangle<T> p_rectangle) const noexcept -> bool
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top   || bottom != p_rectangle.bottom;
		}

		//------------------------------

		/*
			Used to copy a point in an rvalue expression.
			instead of this:
				auto newRectangle = rectangle;
				newRectangle.setTopLeft(2, 3);
			or this:
				auto newRectangle = Rectangle{rectangle}.setTopLeft(2, 3);
			write this:
				auto newRectangle = rectangle.copy().setTopLeft(2, 3);
		*/
		constexpr auto copy() const noexcept -> Rectangle<_Type>
		{
			return *this;
		}

		//------------------------------

		/*
			Sets the top left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		constexpr auto setTopLeft(Point<_Type> p_position, bool p_willKeepSize = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepSize)
			{
				right += p_position.x - left;
				bottom += p_position.y - top;
			}
			left = p_position.x;
			top = p_position.y;
			return *this;
		}
		/*
			Returns the top left coordinates of the rectangle.
		*/
		constexpr auto getTopLeft() const noexcept -> Point<_Type>
		{
			return {left, top};
		}

		//------------------------------

		/*
			Sets the top right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		constexpr auto setTopRight(Point<_Type> p_position, bool p_willKeepSize = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepSize)
			{
				left += p_position.x - right;
				bottom += p_position.y - top;
			}
			right = p_position.x;
			top = p_position.y;
			return *this;
		}
		/*
			Returns the top right coordinates of the rectangle.
		*/
		constexpr auto getTopRight() const noexcept -> Point<_Type>
		{
			return {right, top};
		}

		//------------------------------

		/*
			Sets the bottom left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		constexpr auto setBottomLeft(Point<_Type> p_position, bool p_willKeepSize = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepSize)
			{
				right += p_position.x - left;
				top += p_position.y - bottom;
			}
			left = p_position.x;
			bottom = p_position.y;
			return *this;
		}
		/*
			Returns the bottom left coordinates of the rectangle.
		*/
		constexpr auto getBottomLeft() const noexcept -> Point<_Type>
		{
			return {left, bottom};
		}

		//------------------------------

		/*
			Sets the bottom right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		constexpr auto setBottomRight(Point<_Type> p_position, bool p_willKeepSize = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepSize)
			{
				left += p_position.x - right;
				top += p_position.y - bottom;
			}
			right = p_position.x;
			bottom = p_position.y;
			return *this;
		}
		/*
			Returns the bottom right coordinates of the rectangle.
		*/
		constexpr auto getBottomRight() const noexcept -> Point<_Type>
		{
			return {right, bottom};
		}

		//------------------------------

		/*
			Sets the left coordinate of the rectangle.
			If p_willKeepWidth is true, the right coordinate will be changed so that the width stays the same.
		*/
		constexpr auto setLeft(_Type p_left, bool p_willKeepWidth = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepWidth)
			{
				right += p_left - left;
			}
			left = p_left;
			return *this;
		}
		/*
			Sets the top coordinate of the rectangle.
			If p_willKeepHeight is true, the bottom coordinate will be changed so that the height stays the same.
		*/
		constexpr auto setTop(_Type p_top, bool p_willKeepHeight = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepHeight)
			{
				bottom += p_top - top;
			}
			top = p_top;
			return *this;
		}
		/*
			Sets the right coordinate of the rectangle.
			If p_willKeepWidth is true, the left coordinate will be changed so that the width stays the same.
		*/
		constexpr auto setRight(_Type p_right, bool p_willKeepWidth = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepWidth)
			{
				left += p_right - right;
			}
			right = p_right;
			return *this;
		}
		/*
			Sets the bottom coordinate of the rectangle.
			If p_willKeepWidth is true, the top coordinate will be changed so that the height stays the same.
		*/
		constexpr auto setBottom(_Type p_bottom, bool p_willKeepHeight = true) noexcept -> Rectangle<_Type>&
		{
			if (p_willKeepHeight)
			{
				top += p_bottom - bottom;
			}
			bottom = p_bottom;
			return *this;
		}

		//------------------------------

		/*
			Rounds the coordinates of the rectangle in the directions that expand the rectangle.
		*/
		constexpr auto roundCoordinatesOutwards() noexcept -> Rectangle<_Type>&
		{
			left = floor(left);
			top = floor(top);
			right = ceil(right);
			bottom = ceil(bottom);
			return *this;
		}

		//------------------------------

		/*
			Sets the center coordinates of the rectangle by moving it.
		*/
		template<typename T>
		constexpr auto setCenter(Point<T> p_position) noexcept -> Rectangle<_Type>&
		{
			auto halfSize = Point<std::common_type_t<_Type, T>>{getSize()}/2;
			left = p_position.x - halfSize.x;
			top = p_position.y - halfSize.y;
			right = p_position.x + halfSize.x;
			bottom = p_position.y + halfSize.y;
			return *this;
		}

		/*
			Sets the horizontal center coordinate of the rectangle by moving it.
		*/
		template<typename T>
		constexpr auto setCenterX(T p_x) noexcept -> Rectangle<_Type>&
		{
			auto halfWidth = static_cast<std::common_type_t<_Type, T>>(getWidth())/2;
			left = p_x - halfWidth;
			right = p_x + halfWidth;
			return *this;
		}
		/*
			Sets the vertical center coordinate of the rectangle by moving it.
		*/
		template<typename T>
		constexpr auto setCenterY(T p_y) noexcept -> Rectangle<_Type>&
		{
			auto halfHeight = static_cast<std::common_type_t<_Type, T>>(getHeight())/2;
			top = p_y - halfHeight;
			bottom = p_y + halfHeight;
			return *this;
		}
		/*
			Returns the center coordinates of the rectangle.
		*/
		constexpr auto getCenter() const noexcept -> Point<_Type>
		{
			return {(left + right)/2, (top + bottom)/2};
		}
		/*
			Returns the x-axis center coordinate of the rectangle.
		*/
		constexpr auto getCenterX() const noexcept -> _Type
		{
			return (left + right)/2;
		}
		/*
			Returns the y-axis center coordinate of the rectangle.
		*/
		constexpr auto getCenterY() const noexcept -> _Type
		{
			return (top + bottom)/2;
		}

		//------------------------------

		/*
			Moves the left and top coordinates of the rectangle without affecting the other two.
		*/
		constexpr auto moveTopLeft(Vector2d<_Type> p_offset) noexcept -> Rectangle<_Type>&
		{
			left += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		/*
			Moves the right and top coordinates of the rectangle without affecting the other two.
		*/
		constexpr auto moveTopRight(Vector2d<_Type> p_offset) noexcept -> Rectangle<_Type>&
		{
			right += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		/*
			Moves the left and bottom coordinates of the rectangle without affecting the other two.
		*/
		constexpr auto moveBottomLeft(Vector2d<_Type> p_offset) noexcept -> Rectangle<_Type>&
		{
			left += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		/*
			Moves the right and bottom coordinates of the rectangle without affecting the other two.
		*/
		constexpr auto moveBottomRight(Vector2d<_Type> p_offset) noexcept -> Rectangle<_Type>&
		{
			right += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}

		//------------------------------

		/*
			Offsets the left and right coordinates by an amount.
		*/
		constexpr auto moveX(_Type p_offsetX) noexcept -> Rectangle<_Type>&
		{
			left += p_offsetX;
			right += p_offsetX;
			return *this;
		}
		/*
			Offsets the top and bottom coordinates by an amount.
		*/
		constexpr auto moveY(_Type p_offsetY) noexcept -> Rectangle<_Type>&
		{
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/*
			Sets the width and height of the rectangle, changing only the right and bottom coordinates.
		*/
		constexpr auto setSize(Size<_Type> p_size) noexcept -> Rectangle<_Type>&
		{
			right = left + p_size.x;
			bottom = top + p_size.y;
			return *this;
		}
		/*
			Returns a point representing the size of the rectangle, where the x component is width and y is height.
		*/
		constexpr auto getSize() const noexcept -> Size<_Type>
		{
			return {right - left, bottom - top};
		}

		/*
			Sets the width of the rectangle, changing only the right coordinate.
		*/
		constexpr auto setWidth(_Type p_width) noexcept -> Rectangle<_Type>&
		{
			right = left + max(static_cast<_Type>(0), p_width);
			return *this;
		}
		/*
			Returns the distance between right and left coordinates of the rectangle.
		*/
		constexpr auto getWidth() const noexcept -> _Type
		{
			return right - left;
		}

		/*
			Sets the height of the rectangle, changing only the bottom coordinate.
		*/
		constexpr auto setHeight(_Type p_height) noexcept -> Rectangle<_Type>&
		{
			bottom = top + max(static_cast<_Type>(0), p_height);
			return *this;
		}
		/*
			Returns the distance between bottom and top coordinates of the rectangle.
		*/
		constexpr auto getHeight() const noexcept -> _Type
		{
			return bottom - top;
		}

		//------------------------------

		/*
			Clips this rectangle to fit into the parameter rectangle.
		*/
		template<typename T>
		constexpr auto bound(Rectangle<T> p_bounds) noexcept -> Rectangle<_Type>&
		{
			left = constrain(left, Range{p_bounds.left, p_bounds.right});
			top = constrain(top, Range{p_bounds.top, p_bounds.bottom});
			right = constrain(right, Range{p_bounds.left, p_bounds.right});
			bottom = constrain(bottom, Range{p_bounds.top, p_bounds.bottom});

			return *this;
		}

		/*
			Extends the rectangle so that it contains the parameter rectangle.
		*/
		template<typename T>
		constexpr auto contain(Rectangle<T> p_rectangle) noexcept -> Rectangle<_Type>&
		{
			/*
				If this is true then we need to round "outwards" so that this 
				rectangle also contains the other rectangle's fractional part.
			*/
			if constexpr (std::is_integral_v<_Type> && std::is_floating_point_v<T>)
			{
				if (p_rectangle.left < left) left = std::floor(p_rectangle.left);
				if (p_rectangle.top < top) top = std::floor(p_rectangle.top);
				if (p_rectangle.right > right) right = std::ceil(p_rectangle.right);
				if (p_rectangle.bottom > bottom) bottom = std::ceil(p_rectangle.bottom);
			}
			else
			{
				if (p_rectangle.left < left) left = p_rectangle.left;
				if (p_rectangle.top < top) top = p_rectangle.top;
				if (p_rectangle.right > right) right = p_rectangle.right;
				if (p_rectangle.bottom > bottom) bottom = p_rectangle.bottom;
			}

			return *this;
		}

		//------------------------------

		/*
			Returns whether a point lies within this rectangle.
		*/
		template<typename T>
		constexpr auto getIsContaining(Point<T> p_point) const noexcept -> bool
		{
			return p_point.x >= left && p_point.x < right
			    && p_point.y >= top && p_point.y < bottom;
		}
		/*
			Returns whether another rectangle is fully inside this rectangle.
		*/
		template<typename T>
		constexpr auto getIsContaining(Rectangle<T> p_rectangle) const noexcept -> bool
		{
			return p_rectangle.left >= left && p_rectangle.right < right
				&& p_rectangle.top >= top && p_rectangle.bottom < bottom;
		}
		/*
			Returns whether this rectangle intersects/overlaps/touches another rectangle.
		*/
		template<typename T>
		constexpr auto getIsIntersecting(Rectangle<T> p_rectangle) const noexcept -> bool
		{
			return p_rectangle.right >= left && p_rectangle.bottom >= top
				&& p_rectangle.left <= right && p_rectangle.top <= bottom;
		}
	};

	/*
		An abstract class for objects that have 2d bounds for which changes by the user of the object can be responded to.
	*/
	class ProtectedRectangle
	{
	protected:
		Rectangle<> m_bounds;

		virtual auto handleProtectedRectangleChange(Rectangle<> p_oldRectangle) -> void {};

	public:
		ProtectedRectangle() = default;
		ProtectedRectangle(Rectangle<> p_bounds) : 
			m_bounds{p_bounds} 
		{}

		virtual auto setBounds(Rectangle<> p_rectangle) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds = p_rectangle;
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getBounds() const noexcept -> Rectangle<>
		{
			return m_bounds;
		}

		//------------------------------

		virtual auto move(Vector2d<> p_offset) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds += p_offset;
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto moveX(Dip p_offsetX) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.moveX(p_offsetX);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto moveY(Dip p_offsetY) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.moveY(p_offsetY);
			handleProtectedRectangleChange(oldRectangle);
		}

		//------------------------------

		virtual auto setTopLeft(Point<> p_position, bool p_willKeepSize = true) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTopLeft(p_position, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getTopLeft() const noexcept -> Point<>
		{
			return m_bounds.getTopLeft();
		}

		virtual auto setTopRight(Point<> p_topRight, bool p_willKeepSize = true) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTopRight(p_topRight, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getTopRight() const noexcept -> Point<>
		{
			return m_bounds.getTopRight();
		}

		virtual void setBottomLeft(Point<> p_bottomLeft, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottomLeft(p_bottomLeft, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getBottomLeft() const noexcept -> Point<>
		{
			return m_bounds.getBottomLeft();
		}

		virtual void setBottomRight(Point<> p_bottomRight, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottomRight(p_bottomRight, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getBottomRight() const noexcept -> Point<>
		{
			return m_bounds.getBottomRight();
		}

		//------------------------------

		virtual auto setCenter(Point<> p_position)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenter(p_position);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto setCenterX(Dip p_x)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenterX(p_x);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto setCenterY(Dip p_y)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenterY(p_y);
			handleProtectedRectangleChange(oldRectangle);
		}

		virtual auto getCenter() const noexcept -> Point<>
		{
			return m_bounds.getCenter();
		}
		virtual auto getCenterX() const noexcept -> Dip
		{
			return m_bounds.getCenterX();
		}
		virtual auto getCenterY() const noexcept -> Dip
		{
			return m_bounds.getCenterY();
		}

		//------------------------------

		virtual auto setLeft(Dip p_left, bool p_willKeepWidth = true)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setLeft(p_left, p_willKeepWidth);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getLeft() const noexcept -> Dip
		{
			return m_bounds.left;
		}

		virtual auto setTop(Dip p_top, bool p_willKeepHeight = true)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTop(p_top, p_willKeepHeight);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getTop() const noexcept -> Dip
		{
			return m_bounds.top;
		}

		virtual auto setRight(Dip p_right, bool p_willKeepWidth = true)  -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setRight(p_right, p_willKeepWidth);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getRight() const noexcept -> Dip
		{
			return m_bounds.right;
		}

		virtual auto setBottom(Dip p_bottom, bool p_willKeepHeight = true) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottom(p_bottom, p_willKeepHeight);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getBottom() const noexcept -> Dip
		{
			return m_bounds.bottom;
		}

		//------------------------------

		virtual auto setWidth(Dip p_width) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setWidth(p_width);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getWidth() const noexcept -> Dip
		{
			return m_bounds.getWidth();
		}

		virtual auto setHeight(Dip p_height) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setHeight(p_height);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getHeight() const noexcept -> Dip
		{
			return m_bounds.getHeight();
		}

		virtual auto setSize(Size<> p_size) -> void
		{
			auto oldRectangle = m_bounds;
			m_bounds.setSize(p_size);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual auto getSize() const noexcept -> Size<>
		{
			return m_bounds.getSize();
		}

		//------------------------------

		virtual auto getIsIntersecting(Rectangle<> p_rectangle) const noexcept -> bool
		{
			return m_bounds.getIsIntersecting(p_rectangle);
		}

		virtual auto getIsContaining(Rectangle<> p_rectangle) const noexcept -> bool
		{
			return m_bounds.getIsContaining(p_rectangle);
		}

		virtual auto getIsContaining(Point<> p_point) const -> bool
		{
			return m_bounds.getIsContaining(p_point);
		}
	};

	//------------------------------

	enum class RectangleCornerType
	{
		Round,
		Cut
	};
	struct RectangleCorners
	{
		RectangleCornerType topLeftType;
		RectangleCornerType topRightType;
		RectangleCornerType bottomLeftType;
		RectangleCornerType bottomRightType;

		Dip topLeftSizeX;
		Dip topLeftSizeY;

		Dip topRightSizeX;
		Dip topRightSizeY;

		Dip bottomLeftSizeX;
		Dip bottomLeftSizeY;

		Dip bottomRightSizeX;
		Dip bottomRightSizeY;

		RectangleCorners() :
			topLeftSizeX{0.f}, topLeftSizeY{0.f}, topRightSizeX{0.f}, topRightSizeY{0.f},
			bottomLeftSizeX{0.f}, bottomLeftSizeY{0.f}, bottomRightSizeX{0.f}, bottomRightSizeY{0.f},
			topLeftType{RectangleCornerType::Round}, topRightType{RectangleCornerType::Round},
			bottomLeftType{RectangleCornerType::Round}, bottomRightType{RectangleCornerType::Round}
		{
		}
		explicit RectangleCorners(Dip p_cornerSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX{p_cornerSize}, topLeftSizeY{p_cornerSize},
			topRightSizeX{p_cornerSize}, topRightSizeY{p_cornerSize},
			bottomLeftSizeX{p_cornerSize}, bottomLeftSizeY{p_cornerSize},
			bottomRightSizeX{p_cornerSize}, bottomRightSizeY{p_cornerSize},
			topLeftType{p_cornerType}, topRightType{p_cornerType},
			bottomLeftType{p_cornerType}, bottomRightType{p_cornerType}
		{
		}
		RectangleCorners(Dip p_cornerSizeX, Dip p_cornerSizeY, RectangleCornerType p_cornerType = RectangleCornerType::Cut) :
			topLeftSizeX{p_cornerSizeX}, topLeftSizeY{p_cornerSizeY},
			topRightSizeX{p_cornerSizeX}, topRightSizeY{p_cornerSizeY},
			bottomLeftSizeX{p_cornerSizeX}, bottomLeftSizeY{p_cornerSizeY},
			bottomRightSizeX{p_cornerSizeX}, bottomRightSizeY{p_cornerSizeY},
			topLeftType{p_cornerType}, topRightType{p_cornerType},
			bottomLeftType{p_cornerType}, bottomRightType{p_cornerType}
		{
		}

		RectangleCorners(Dip p_topLeftSize, Dip p_topRightSize, Dip p_bottomLeftSize, Dip p_bottomRightSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX{p_topLeftSize}, topLeftSizeY{p_topLeftSize},
			topRightSizeX{p_topRightSize}, topRightSizeY{p_topRightSize},
			bottomLeftSizeX{p_bottomLeftSize}, bottomLeftSizeY{p_bottomLeftSize},
			bottomRightSizeX{p_bottomRightSize}, bottomRightSizeY{p_bottomRightSize},
			topLeftType{p_cornerType}, topRightType{p_cornerType},
			bottomLeftType{p_cornerType}, bottomRightType{p_cornerType}
		{
		}
	};

	//------------------------------

	/*
		Cubic bezier animation easing.
		Try out this, can be useful for making your easing curves:
		http://bjornsundin.com/projects/cubic-bezier-easing

		Ease in example:
		Easing(0.7, 0, 1, 1).easeValue(x)
		Ease out example:
		Easing(0, 0, 0.3, 1).easeValue(x)
		See the constructors and easeValue() for more info.

		Storing Easing objects in a Theme can be a good idea because you can use the same easings within your whole
		application, or different parts of it.
	*/
	struct Easing
	{
		float x0 = 0, y0 = 0, x1 = 0, y1 = 0;

		//------------------------------

		constexpr Easing() noexcept = default;
		constexpr Easing(Easing const& p_easing) = default;
		/*
			Initializes the control points of the bezier curve easing.
			The parameters are the coordinates of the first and second control points, respectively.
		*/
		constexpr Easing(float p_x0, float p_y0, float p_x1, float p_y1) noexcept :
			x0{p_x0}, y0{p_y0}, x1{p_x1}, y1{p_y1}
		{}

		//------------------------------

		constexpr auto operator==(Easing p_easing) const noexcept -> bool
		{
			return x0 == p_easing.x0 && y0 == p_easing.y0 && x1 == p_easing.x1 && y1 == p_easing.y1;
		}
		constexpr auto operator!=(Easing p_easing) const noexcept -> bool
		{
			return x0 != p_easing.x0 || y0 != p_easing.y0 || x1 != p_easing.x1 || y1 != p_easing.y1;
		}

		//------------------------------

		/*
			Transforms a normalized value according to a cubic bezier curve.
			(p_x0, p_y0) is the first control point and (p_x1, p_y1) is the second one.
			p_precision is the maximum amount of error in the output value.

			It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
			f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (p_value is x) and not t. This why we have a precision parameter.
		*/
		static auto easeValue(float p_x0, float p_y0, float p_x1, float p_y1, float p_value, float p_precision = 0.005f) noexcept -> float
		{

			if (p_value <= 0.00001f)
			{
				return 0.f;
			}
			if (p_value >= 0.99999f)
			{
				return 1.f;
			}

			float t = p_value < 0.5f ? 0.25f : 0.75f;

			/*
				f(x) = 3*t*(1-t)*(1-t)*x0 + 3*t*t*(1-t)*x1 + t*t*t

				f'(x) = x0*(3 - 12*t + 9*t*t) + x1*(6*t - 9*t*t) + 3*t*t
				      = x0*9*(t - 1)*(t - 1/3) + t*(x1*(6 - 9*t) + 3*t)
			*/

			constexpr float ONE_THIRD = 1.f/3.f;

			float error = 1;
			while (std::abs(error) > p_precision)
			{
				error = p_value - t*((1.f - t)*(3.f*(1.f - t)*p_x0 + 3.f*t*p_x1) + t*t);
				//t += error/(p_x0*(3.f - 12.f*t + 9.f*t*t) + p_x1*(6.f*t - 9.f*t*t) + 3.f*t*t);
				t += error/(p_x0*9.f*(t - 1.f)*(t - ONE_THIRD) + t*(p_x1*(6.f - 9.f*t) + 3.f*t));
			}

			return t*((1.f - t)*(3.f*(1.f - t)*p_y0 + 3.f*t*p_y1) + t*t);
		}
		/*
			Transforms a normalized value according to a cubic bezier curve.
			p_precision is the maximum amount of error in the output value.

			It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
			f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (p_value is x) and not t. This why we have a precision parameter.
		*/
		auto easeValue(float p_value, float p_precision = 0.005f) const noexcept -> float
		{
			return easeValue(x0, y0, x1, y1, p_value, p_precision);
		}
		/*
			Transforms a normalized value according to the inverse of this cubic bezier curve,
			so that easeValueInverse(easeValue(x)) = x (approximately)
			p_precision is the maximum amount of error in the output value.
		*/
		auto easeValueInverse(float p_value, float p_precision = 0.005f) const noexcept -> float
		{
			return easeValue(y0, x0, y1, x1, p_value, p_precision);
		}
	};

	class View;

	/*
		Class used for making animations.
		Preferrablý use the constructor directly, but there is also View::addAnimation 
		methods to dynamically create animations that have the same lifetime as a view.
	*/
	class Animation
	{
		friend class Gui;

	private:
		using _Clock = chrono::steady_clock;
		using _Duration = chrono::duration<float, std::milli>;

		_Duration m_duration;
	public:
		auto setDuration(float p_milliseconds) -> void
		{
			m_duration = _Duration{p_milliseconds};
		}
		/*
			Returns the duration of the animation in milliseconds.
		*/
		auto getDuration() noexcept -> float
		{
			return m_duration.count();
		}
		/*
			Sets the duration of the animation in any type from the standard chrono library.
			Examples:
				animation.setDuration(1min/5); // Minutes
				animation.setDuration(2s); // Seconds
				animation.setDuration(400ms); // Milliseconds
		*/
		template<typename DurationType, typename DurationPeriod>
		auto setDuration(chrono::duration<DurationType, DurationPeriod> p_duration) -> void
		{
			m_duration = chrono::duration_cast<_Duration>(p_duration);
		}
		/*
			Returns the duration of the animation in any duration type from the standard chrono library.
			Example:
				auto seconds = animation.getDuration<std::chrono::seconds>();
		*/
		template<typename DurationType>
		auto getDuration() -> DurationType
		{
			return chrono::duration_cast<DurationType>(m_duration);
		}

	private:
		bool m_isReversed = false;
	public:
		auto setIsReversed(bool p_isReversed) -> void
		{
			if (p_isReversed != m_isReversed)
			{
				float value = m_easing.easeValue(chrono::duration_cast<_Duration>(_Clock::now() - m_startTime)/m_duration, m_easingPrecision);
				m_startTime = _Clock::now() - chrono::duration_cast<_Clock::duration>(m_easing.easeValueInverse(1.f - value)*m_duration);
				m_isReversed = p_isReversed;
			}
		}
		auto getIsReversed() noexcept -> bool
		{
			return m_isReversed;
		}

	private:
		bool m_isDone = true;
	public:
		auto getIsDone() noexcept -> bool
		{
			return m_isDone;
		}

	private:
		Easing m_easing;
	public:
		auto setEasing(Easing p_easing) noexcept -> void
		{
			m_easing = p_easing;
		}
		auto getEasing() noexcept -> Easing
		{
			return m_easing;
		}

	private:
		float m_easingPrecision = 0.005f;
	public:
		auto setEasingPrecision(float p_easingPrecision) -> void
		{
			m_easingPrecision = p_easingPrecision;
		}
		auto getEasingPrecision() -> float
		{
			return m_easingPrecision;
		}

	private:
		Gui* m_gui = nullptr;
		bool m_isInUpdateQueue = false;
		auto queueUpdate() -> void;

		bool m_areUpdatesCancelled = false;
		auto update() -> void
		{
			if (m_areUpdatesCancelled)
			{
				m_isInUpdateQueue = false;
				return;
			}
			float value = m_easing.easeValue(chrono::duration_cast<_Duration>(_Clock::now() - m_startTime)/m_duration, m_easingPrecision);
			if (value >= 1.f)
			{
				m_isDone = true;
				value = 1.f;
			}
			if (m_isReversed)
			{
				value = 1.f - value;
			}

			updateListeners(value);

			m_isInUpdateQueue = false;
			if (!m_isDone)
			{
				queueUpdate();
			}
		}
		auto cancelAllUpdates() -> void;

	public:
		/*
			Listener signature:
				void(float p_value)
			p_value is between 0 and 1.
			At first the animation goes forward, but if you call reverse() the direction is switched.
		*/
		EventListeners<void(float)> updateListeners;

	private:
		bool m_isPaused = false;
		_Clock::time_point m_startTime;
		_Clock::time_point m_pauseTime;
	public:
		auto play(bool p_isReversed) -> void
		{
			setIsReversed(p_isReversed);
			if (m_isPaused)
			{
				m_startTime += _Clock::now() - m_pauseTime;
			}
			else if (m_isDone)
			{
				m_startTime = _Clock::now();
			}
			else return;
			m_isDone = false;
			queueUpdate();
		}
		auto play() -> void
		{
			play(m_isReversed);
		}
		/*
			If the animation is reversed then the animation value will start at 1 if p_startProgress is 0.
		*/
		auto play(float p_startProgress) -> void
		{
			m_isDone = false;
			if (m_isReversed)
			{
				m_startTime = _Clock::now() - chrono::duration_cast<_Clock::duration>((1.f - p_startProgress)*m_duration);
			}
			else
			{
				m_startTime = _Clock::now() - chrono::duration_cast<_Clock::duration>(p_startProgress*m_duration);
			}
		}
		auto pause() -> void
		{
			m_isPaused = true;
			m_isDone = true;
		}
		auto stop() -> void
		{
			m_isPaused = false;
			m_isDone = true;
		}
		auto replay() -> void
		{
			stop();
			play();
		}

		//------------------------------

		auto operator=(Animation const&) -> Animation& = default;
		auto operator=(Animation&& p_other) noexcept -> Animation& = default;

		Animation() = default;
		Animation(Animation const&) = default;
		Animation(Animation&&) noexcept = default;

		Animation(Gui* p_gui, Easing p_easing, float p_milliseconds) :
			m_gui{p_gui},
			m_easing{p_easing},
			m_duration{p_milliseconds}
		{
		}
		template<typename Callback>
		Animation(Gui* p_gui, Easing p_easing, float p_milliseconds, Callback const& p_callback) :
			m_gui{p_gui},
			m_easing{p_easing},
			m_duration{p_milliseconds}
		{
			static_assert(std::is_invocable_r_v<void, Callback, float>, "Animation error: callback must be of type void (float).");
			updateListeners += p_callback;
		}
		template<typename DurationType, typename DurationPeriod>
		Animation(Gui* p_gui, Easing p_easing, chrono::duration<DurationType, DurationPeriod> const& p_duration) :
			Animation{p_gui, p_easing, chrono::duration_cast<_Duration>(p_duration).count()}
		{
		}
		template<typename DurationType, typename DurationPeriod, typename Callback>
		Animation(Gui* p_gui, Easing p_easing, chrono::duration<DurationType, DurationPeriod> const& p_duration, Callback const& p_callback) :
			Animation{p_gui, p_easing, chrono::duration_cast<_Duration>(p_duration).count(), p_callback}
		{
		}
		~Animation()
		{
			cancelAllUpdates();
		}
	};

	//------------------------------

	/*
		Runs a callable on construction.
	*/
	struct Initializer
	{
		template<typename Lambda>
		Initializer(Lambda p_initializer)
		{
			p_initializer();
		}
	};

	//------------------------------
	// Color stuff

	/*
		ARGB formatted 32-bit packed color, where every channel has 8 bits.
	*/
	using colorInt = uint32;

	constexpr inline auto getRedChannel(colorInt p_color) noexcept -> uint8
	{
		return p_color >> 16 & 0xff;
	}
	constexpr inline auto getGreenChannel(colorInt p_color) noexcept -> uint8
	{
		return p_color >> 8 & 0xff;
	}
	constexpr inline auto getBlueChannel(colorInt p_color) noexcept -> uint8
	{
		return p_color & 0xff;
	}
	constexpr inline auto getAlphaChannel(colorInt p_color) noexcept -> uint8
	{
		return p_color >> 24 & 0xff;
	}

	/*
		Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
		This means that a Color object is 4 times as big as a packed 32-bit color.
	*/
	struct Color
	{
		float red = 0.f, 
		      green = 0.f, 
			  blue = 0.f, 
			  alpha = 1.f;

		//------------------------------

		constexpr Color() noexcept = default;
		/*
			The channels are floats in the range [0, 1].
		*/
		constexpr Color(float p_red, float p_green, float p_blue, float p_alpha = 1.f) noexcept :
			red{constrain(p_red)}, 
			green{constrain(p_green)}, 
			blue{constrain(p_blue)}, 
			alpha{constrain(p_alpha)}
		{}
		/*
			The channels are in the range [0, 255]
		*/
		constexpr Color(uint8 p_red, uint8 p_green, uint8 p_blue, uint8 p_alpha = static_cast<uint8>(255)) noexcept :
			red{p_red/255.f},
			green{p_green/255.f},
			blue{p_blue/255.f},
			alpha{p_alpha/255.f}
		{}
		template<typename T>
		constexpr Color(
			IntegerType<T> p_red, IntegerType<T> p_green, IntegerType<T> p_blue, 
			IntegerType<T> p_alpha = static_cast<T>(255)
		) noexcept :
			red{constrain(p_red/255.f)},
			green{constrain(p_green/255.f)},
			blue{constrain(p_blue/255.f)},
			alpha{constrain(p_alpha/255.f)}
		{}
		/*
			Initializes the color with a grayscale value. The values are floats in the range [0, 1].
		*/
		constexpr Color(float p_lightness, float p_alpha = 1.f) noexcept :
			red{constrain(p_lightness)},
			green{constrain(p_lightness)},
			blue{constrain(p_lightness)},
			alpha{constrain(p_alpha)}
		{
		}
		/*
			Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
		*/
		constexpr Color(uint8 p_lightness, uint8 p_alpha = 255u) noexcept :
			red{p_lightness/255.f},
			green{red},
			blue{red},
			alpha{p_alpha/255.f}
		{
		}
		/*
			Initializes the color with a grayscale value. The values are in the range [0, 255].
		*/
		template<typename T>
		constexpr Color(IntegerType<T> p_lightness, IntegerType<T> p_alpha = static_cast<T>(255)) noexcept :
			red{constrain(p_lightness/255.f)},
			green{red},
			blue{red},
			alpha{constrain(p_alpha/255.f)}
		{
		}
		/*
			Creates a copy of another color but with a new alpha.
		*/
		constexpr Color(Color p_color, float p_alpha) noexcept :
			red{p_color.red}, 
			green{p_color.green}, 
			blue{p_color.blue}, 
			alpha{constrain(p_alpha)}
		{}
		constexpr Color(Color p_color, uint8 p_alpha) noexcept :
			red{p_color.red/255.f},
			green{p_color.green/255.f},
			blue{p_color.blue/255.f},
			alpha{p_alpha/255.f}
		{}
		template<typename T>
		constexpr Color(Color p_color, IntegerType<T> p_alpha) noexcept :
			red{p_color.red},
			green{p_color.green},
			blue{p_color.blue},
			alpha{constrain(p_alpha/255.f)}
		{}
		
		/*
			Initializes with a 4-byte packed RGBA color.
		*/
		constexpr Color(colorInt p_color) noexcept :
			alpha{(p_color >> 24u)/255.f},
			red{(p_color >> 16u & 0xffu)/255.f},
			green{(p_color >> 8u & 0xffu)/255.f},
			blue{(p_color & 0xffu)/255.f}
		{
		}

		constexpr auto operator=(colorInt p_color) noexcept -> Color&
		{
			alpha = (p_color >> 24u)/255.f;
			red = (p_color >> 16u & 0xffu)/255.f;
			green = (p_color >> 8u & 0xffu)/255.f;
			blue = (p_color & 0xffu)/255.f;
			return *this;
		}

		constexpr auto operator*(float p_factor) const noexcept -> Color
		{
			return {red*p_factor, green*p_factor, blue*p_factor};
		}
		constexpr auto operator*=(float p_factor) noexcept -> Color&
		{
			red *= p_factor;
			green *= p_factor;
			blue *= p_factor;
			return *this;
		}
		constexpr auto operator/(float p_divisor) const noexcept -> Color
		{
			return {red/p_divisor, green/p_divisor, blue/p_divisor};
		}
		constexpr auto operator/=(float p_divisor) noexcept -> Color&
		{
			red /= p_divisor;
			green /= p_divisor;
			blue /= p_divisor;
			return *this;
		}

		constexpr auto operator+(float p_delta) const noexcept -> Color
		{
			return {red + p_delta, green + p_delta, blue + p_delta};
		}
		constexpr auto operator+=(float p_delta) noexcept -> Color&
		{
			red += p_delta;
			green += p_delta;
			blue += p_delta;
			return *this;
		}

		constexpr auto operator-(float p_delta) const noexcept -> Color
		{
			return {red - p_delta, green - p_delta, blue - p_delta};
		}
		constexpr auto operator-=(float p_delta) noexcept -> Color&
		{
			red -= p_delta;
			green -= p_delta;
			blue -= p_delta;
			return *this;
		}

		//------------------------------

		constexpr auto operator==(Color p_color) const noexcept -> bool
		{
			return red == p_color.red && green == p_color.green && blue == p_color.blue && alpha == p_color.alpha;
		}
		constexpr auto operator!=(Color p_color) const noexcept -> bool
		{
			return red != p_color.red || green != p_color.green || blue != p_color.blue || alpha != p_color.alpha;
		}

		//------------------------------

		static constexpr auto rgba(float p_red, float p_green, float p_blue, float p_alpha = 1.f) noexcept -> Color
		{
			return Color{p_red, p_green, p_blue, p_alpha};
		}
		static constexpr auto rgb(float p_red, float p_green, float p_blue) noexcept -> Color
		{
			return Color{p_red, p_green, p_blue};
		}

		/*
			Creates a color from hue, saturation, brightness and alpha values.
			They are all floats in the range [0, 1].
			The difference between HSB and HSL is that the lightness value goes from black to white 
			while brightness goes from black to full color brightness. 
			HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
		*/
		static auto hsba(float p_hue, float p_saturation, float p_brightness, float p_alpha = 1.f) noexcept -> Color
		{
			p_hue -= floor(p_hue);
			p_brightness = constrain(p_brightness);
			float factor = p_brightness*constrain(p_saturation);

			return Color{
				p_brightness + factor*(constrain(1.f - (p_hue - 1.f/6.f)*6.f) + constrain((p_hue - 4.f/6.f)*6.f) - 1.f),
				p_brightness + factor*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f/6.f)*6.f) - 1.f),
				p_brightness + factor*(constrain((p_hue - 2.f/6.f)*6.f) - constrain((p_hue - 5.f/6.f)*6.f) - 1.f)
			};
		}
		/*
			Calls Color::hsba.
		*/
		static auto hsb(float p_hue, float p_saturation, float p_brightness) noexcept -> Color
		{
			return hsba(p_hue, p_saturation, p_brightness);
		}
		template<typename T>
		static auto hsba(Angle<T> p_hue, float p_saturation, float p_brightness, float p_alpha = 1.f) noexcept -> Color
		{
			if constexpr (std::is_convertible_v<Angle<T>, Degrees>)
			{
				return hsba(p_hue.value/360.f, p_saturation, p_brightness, p_alpha);
			}
			else
			{
				return hsba(p_hue.value/TAU<float>, p_saturation, p_brightness, p_alpha);
			}
		}
		template<typename T>
		static auto hsb(Angle<T> p_hue, float p_saturation, float p_brightness) noexcept -> Color
		{
			return hsba(p_hue, p_saturation, p_brightness);
		}

		/*
			Creates a color from hue, saturation, lightness and alpha values.
			They are all floats in the range [0, 1].
			The difference between HSB and HSL is that the lightness value goes from black to white 
			while brightness goes from black to full color brightness. 
			HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
		*/
		static auto hsla(float p_hue, float p_saturation, float p_lightness, float p_alpha = 1.f) noexcept -> Color
		{
			p_hue -= floor(p_hue);
			p_lightness = constrain(p_lightness);
			float factor = 2.f*constrain(p_saturation)*(p_lightness < 0.5f ? p_lightness : (1.f - p_lightness));

			return Color{
				p_lightness + factor*(constrain(1.f - (p_hue - 1.f/6.f)*6.f) + constrain((p_hue - 4.f/6.f)*6.f) - 0.5f),
				p_lightness + factor*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f/6.f)*6.f) - 0.5f),
				p_lightness + factor*(constrain((p_hue - 2.f/6.f)*6.f) - constrain((p_hue - 5.f/6.f)*6.f) - 0.5f)
			};
		}
		/*
			Calls Color::hsla.
		*/
		static auto hsl(float p_hue, float p_saturation, float p_lightness) noexcept -> Color
		{
			return hsla(p_hue, p_saturation, p_lightness);
		}
		/*
			Creates a color from hue, saturation, lightness and alpha values.
			The hue is expressed as an angle and the rest are values in the range [0, 1].
			The difference between HSB and HSL is that the lightness value goes from black to white 
			while brightness goes from black to full color brightness. 
			HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
		*/
		template<typename T>
		static auto hsla(Angle<T> p_hue, float p_saturation, float p_lightness, float p_alpha = 1.f) noexcept -> Color
		{
			if constexpr (std::is_convertible_v<Angle<T>, Degrees>)
			{
				return hsla(p_hue.value/360.f, p_saturation, p_lightness, p_alpha);
			}
			else
			{
				return hsla(p_hue.value/TAU<float>, p_saturation, p_lightness, p_alpha);
			}
		}
		/*
			Calls Color::hsla.
		*/
		template<typename T>
		static auto hsl(Angle<T> p_hue, float p_saturation, float p_lightness) noexcept -> Color
		{
			return hsla(p_hue, p_saturation, p_lightness);
		}

		//------------------------------


		/*
			Changes the hue of the color. The hue is a float in the range [0, 1].
		*/
		auto setHue(float p_hue) noexcept -> Color&
		{
			p_hue -= floor(p_hue);
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			red = minColor + (maxColor - minColor)*(constrain(1.f - (p_hue - 1.f/6.f)*6.f) + constrain((p_hue - 4.f/6.f)*6.f));
			green = minColor + (maxColor - minColor)*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f/6.f)*6.f));
			blue = minColor + (maxColor - minColor)*(constrain((p_hue - 2.f/6.f)*6.f) - constrain((p_hue - 5.f/6.f)*6.f));
			return *this;
		}
		/*
			Returns the hue of the color. The hue is a float in the range [0, 1].
		*/
		constexpr auto getHue() const noexcept -> float
		{
			if (red + green + blue == 0.f)
			{
				return 0.f;
			}

			if (red > green)
			{
				if (red > blue)
				{
					if (green > blue)
					{
						// (1, 0, 0) -> (1, 1, 0) : 0/6 < hue < 1/6, max = red, min = blue
						return (green - blue)/(red - blue)/6.f;
					}
					else
					{
						// (1, 0, 1) -> (1, 0, 0) : 5/6 < hue < 6/6, max = red, min = green
						return 1.f - (blue - green)/(red - green)/6.f;
					}
				}
				else
				{
					// (0, 0, 1) -> (1, 0, 1) : 4/6 < hue < 5/6, max = blue, min = green
					return (4.f + (red - green)/(blue - green))/6.f;
				}
			}
			else
			{
				if (green > blue)
				{
					if (red > blue)
					{
						// (1, 1, 0) -> (0, 1, 0) : 1/6 < hue < 2/6, max = green, min = blue
						return (2.f - (red - blue)/(green - blue))/6.f;
					}
					else
					{
						// (0, 1, 0) -> (0, 1, 1) : 2/6 < hue < 3/6, max = green, min = red
						return (2.f + (blue - red)/(green - red))/6.f;
					}
				}
				else
				{
					// (0, 1, 1) -> (0, 0, 1) : 3/6 < hue < 4/6, max = blue, min = red
					return (4.f - (green - red)/(blue - red))/6.f;
				}
			}
		}

		/*
			Sets the HSB saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is grayscale, because only RGBA values are stored.
		*/
		constexpr auto setSaturationHSB(float p_saturation) noexcept -> Color&
		{
			if (red == green && red == blue)
			{
				return *this;
			}

			p_saturation = constrain(p_saturation);
			float factor = p_saturation/getSaturationHSB();

			float brightness = max(red, green, blue);
			red = brightness + factor*(red - brightness);
			green = brightness + factor*(green - brightness);
			blue = brightness + factor*(blue - brightness);

			return *this;
		}
		/*
			Returns the HSB saturation of the color. The saturation is a float in the range [0, 1].
		*/
		constexpr auto getSaturationHSB() const noexcept -> float
		{
			if (auto brightness = getBrightness())
			{
				return 1.f - min(red, green, blue)/brightness;
			}
			return 0.f;
		}

		/*
			Sets the HSL saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is gray, since only RGBA values are stored.
		*/
		constexpr auto setSaturationHSL(float p_saturation) noexcept -> Color&
		{
			p_saturation = constrain(p_saturation);

			float factor = p_saturation/getSaturationHSL();
			if (factor == p_saturation/0.f)
			{
				return *this;
			}
			
			float lightness = getLightness();
			red = lightness + factor*(red - lightness);
			green = lightness + factor*(green - lightness);
			blue = lightness + factor*(blue - lightness);

			return *this;
		}
		/*
			Returns the HSL saturation of the color. The saturation is a float in the range [0, 1].
		*/
		constexpr auto getSaturationHSL() const noexcept -> float
		{
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			if (minColor == maxColor)
			{
				return 0.f;
			}
			return max((maxColor - minColor)/(maxColor + minColor), (maxColor - minColor)/(2.f - maxColor - minColor));
		}

		/*
			Sets the brightness of the color. The brightness is a float in the range [0, 1]. A brightness of 0 makes the
			color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
		*/
		constexpr auto setBrightness(float p_brightness) noexcept -> Color&
		{
			p_brightness = constrain(p_brightness);

			if (red == green && red == blue)
			{
				red = p_brightness;
				green = p_brightness;
				blue = p_brightness;
				return *this;
			}

			float brightness = max(red, green, blue);
			red *= p_brightness/brightness;
			green *= p_brightness/brightness;
			blue *= p_brightness/brightness;

			return *this;
		}
		/*
			Returns the brightness of the color. The brightness is a float in the range [0, 1].
		*/
		constexpr auto getBrightness() const noexcept -> float
		{
			return max(red, green, blue);
		}

		/*
			Changes the lightness of the color. The lightness a float in the range [0, 1]. A lightness of 0 makes the
			color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
		*/
		constexpr auto setLightness(float p_lightness) noexcept -> Color&
		{
			p_lightness = constrain(p_lightness);

			if (red == green && red == blue)
			{
				red = p_lightness;
				green = p_lightness;
				blue = p_lightness;
				return *this;
			}

			if (auto const lightness = getLightness();
			    lightness <= 0.5f)
			{
				if (p_lightness <= 0.5f)
				{
					red = red*p_lightness/lightness;
					green = green*p_lightness/lightness;
					blue = blue*p_lightness/lightness;
				}
				else
				{
					red = (red - lightness)*(1.f - p_lightness)/lightness + p_lightness;
					green = (green - lightness)*(1.f - p_lightness)/lightness + p_lightness;
					blue = (blue - lightness)*(1.f - p_lightness)/lightness + p_lightness;
				}
			}
			else
			{
				if (p_lightness <= 0.5)
				{
					red = (red - lightness)*p_lightness/(1.f - lightness) + p_lightness;
					green = (green - lightness)*p_lightness/(1.f - lightness) + p_lightness;
					blue = (blue - lightness)*p_lightness/(1.f - lightness) + p_lightness;
				}
				else
				{
					red = (red - lightness)*(1.f - p_lightness)/(1.f - lightness) + p_lightness;
					green = (green - lightness)*(1.f - p_lightness)/(1.f - lightness) + p_lightness;
					blue = (blue - lightness)*(1.f - p_lightness)/(1.f - lightness) + p_lightness;
				}
			}
			return *this;
		}
		/*
			Returns the lightness of the color. The lightness is a float in the range [0, 1].
		*/
		constexpr auto getLightness() const noexcept -> float
		{
			return 0.5f*(min(red, green, blue) + max(red, green, blue));
		}

		//------------------------------

		/*
			A contrast of 0 makes the color gray, 0.5 leaves it unchanged and 1 is maximum contrast.
		*/
		constexpr auto setContrast(float p_contrast) noexcept -> void
		{
			if (p_contrast == 0.5)
			{
				return;
			}
			if (p_contrast < 0.5f)
			{
				red = (red - 0.5f)*p_contrast*2.f + 0.5f;
				green = (green - 0.5f)*p_contrast*2.f + 0.5f;
				blue = (blue - 0.5f)*p_contrast*2.f + 0.5f;
			}
			else
			{
				red = (static_cast<float>(red >= 0.5f) - red)*(p_contrast*2.f - 1.f) + red;
				green = (static_cast<float>(green >= 0.5f) - green)*(p_contrast*2.f - 1.f) + green;
				blue = (static_cast<float>(blue >= 0.5f) - blue)*(p_contrast*2.f - 1.f) + blue;
			}
		}

		//------------------------------

		/*
			Packs the color into a 32-bit integer in ARGB format.
		*/
		constexpr auto getPacked() const noexcept -> colorInt
		{
			return (colorInt(alpha*0xff) << 24) | (colorInt(red*0xff) << 16) | (colorInt(green*0xff) << 8) | (colorInt(blue*0xff));
		}
	};

	/*
		Linearly interpolates a color between p_start and p_end. Each channel is faded individually.
		If p_progress is 0, p_start is returned. If p_progress is 1, p_end is returned.
	*/
	inline auto interpolate(Color p_start, Color p_end, float p_progress) -> Color
	{
		return {
			p_start.red*(1.f - p_progress) + p_end.red*p_progress,
			p_start.green*(1.f - p_progress) + p_end.green*p_progress,
			p_start.blue*(1.f - p_progress) + p_end.blue*p_progress,
			p_start.alpha*(1.f - p_progress) + p_end.alpha*p_progress
		};
	}

	//
	// Font family names
	//

	inline constexpr std::string_view FONT_FAMILY_ROBOTO = "Roboto";
	inline constexpr std::string_view FONT_FAMILY_MATERIAL_ICONS = "Material Icons";

	//------------------------------

	/*
		Default theme color names.
	*/
	namespace ThemeColors
	{
		inline Id const background;
		inline Id const onBackground;

		inline Id const primary;
		inline Id const primaryOnBackground;
		inline Id const onPrimary;

		inline Id const secondary;
		inline Id const secondaryOnBackground;
		inline Id const onSecondary;

		inline Id const selection;
		inline Id const shadow;
	};

	/*
		Default theme easing names.
	*/
	namespace ThemeEasings
	{
		inline Id const in;
		inline Id const out;
		inline Id const inOut;
		inline Id const symmetricalInOut;
	}

	/*
		Default theme value names.
	*/
	namespace ThemeValues
	{
		inline Id const hoverAnimationSpeed;
		inline Id const hoverAnimationDuration;
	}

	/*
		A theme consists of different variables that change the look and feel of the parts of the GUI that are using the theme.
		Can be used for changing and accessing any values, colors and easings.
		All the default IDs are in ThemeColors, ThemeEasings and ThemeValues.
	*/
	struct Theme : public ReferenceCounted
	{
		std::unordered_map<uint64, Color> colors;
		std::unordered_map<uint64, Easing> easings;
		std::unordered_map<uint64, float> values;

		Theme(Theme const& p_theme)
		{
			colors = p_theme.colors;
			easings = p_theme.easings;
			values = p_theme.values;
		}

		/*
			This initializes the default global theme.
		*/
		Theme()
		{
			// Colors

			colors[ThemeColors::background] = 0xfffefefe;
			colors[ThemeColors::onBackground] = 0xff070707;

			colors[ThemeColors::primary] = 0xff6200ea; // MaterialColors::DEEP_PURPLE_A700
			colors[ThemeColors::primaryOnBackground] = 0xff512da8; // MaterialColors::DEEP_PURPLE_700
			colors[ThemeColors::onPrimary] = ~0U;

			colors[ThemeColors::secondary] = 0xff1de9b6; // MaterialColors::TEAL_A400
			colors[ThemeColors::secondaryOnBackground] = 0xff00bfa5; // MaterialColors::TEAL_A700
			colors[ThemeColors::onSecondary] = 0xff070707;

			colors[ThemeColors::selection] = 0x90488db5;

			colors[ThemeColors::shadow] = 0x68000000;

			//------------------------------
			// Easings

			easings[ThemeEasings::in] = {0.6, 0.0, 0.8, 0.2};
			easings[ThemeEasings::out] = {0.1, 0.9, 0.2, 1.0};
			easings[ThemeEasings::inOut] = {0.4, 0.0, 0.0, 1.0};
			easings[ThemeEasings::symmetricalInOut] = {0.6, 0.0, 0.4, 1.0};

			//------------------------------
			// Values

			values[ThemeValues::hoverAnimationSpeed] = 1.f/6.f; // 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.
			values[ThemeValues::hoverAnimationDuration] = 60.f;
		}
	};

	//------------------------------

	template<typename T>
	class DrawingContextObject
	{
	public:
		auto operator==(DrawingContextObject<T> const& p_other) const -> bool
		{
			return m_implementation == p_other.m_implementation;
		}
		auto operator!=(DrawingContextObject<T> const& p_other) const -> bool
		{
			return m_implementation != p_other.m_implementation;
		}

		auto getIsValid() const -> bool
		{
			return m_implementation;
		}
		operator bool() const
		{
			return m_implementation.operator bool();
		}
		auto destroy() -> void
		{
			m_implementation = nullptr;
		}

	protected:
		std::shared_ptr<T> m_implementation;
		DrawingContextObject(std::shared_ptr<T>&& p_implementation) :
			m_implementation{std::move(p_implementation)}
		{}
		DrawingContextObject(std::shared_ptr<T> const& p_implementation) :
			m_implementation{p_implementation}
		{}
	public:
		auto getImplementation() const -> T*
		{
			return m_implementation.get();
		}

		DrawingContextObject() = default;
	};
	
	//------------------------------

	/*
		This specifies what is done to fit the image within its bounds.
	*/
	enum class ImageBoundsSizing
	{
		Stretch, // This stretches the image so that it fills its bounds.
		Contain, // This makes sure the image is as big as possible while still keeping the image within its bounds. Aspect ratio is kept.
		Fill, // This makes sure the image is so big that it fills its bounds while keeping aspect ratio. Edges may be clipped.
		Unknown = -1
	};

	/*
		This specifies how the pixels of an image are interpolated when it is scaled.
	*/
	enum class ImageScalingMethod
	{
		Pixelated, // Uses nearest neighbor interpolation
		Smooth, // Uses linear interpolation
		Unknown = -1
	};

	/*
		The formats that an image can be encoded to and decoded from.
	*/
	enum class ImageFormat
	{
		Png,
		Jpeg,
		Bmp, // Only on Windows.
		Ico, // Only on Windows.
		Unknown = -1
	};

	/*
		Represents an image on the GPU which can be created and drawn by a DrawingContext.
		Notice that this is not a view but should be treated as a drawable object.
		The memory of an image is automatically managed and reference counted.
		Here:
			Image image_0 = getDrawingContext()->createImage("image.png");
			Image image_1 = image_0;
		image_1 and image_0 are referring to the same image, and the internal image object
		is released once all references have been destroyed.
	*/
	class Image : public ProtectedRectangle, public DrawingContextObject<Image>
	{
	public:
		/*
			Sets a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual auto setCropRectangle(Rectangle<> p_rectangle) -> void
		{
			if (m_implementation)
			{
				m_implementation->setCropRectangle(p_rectangle);
			}
		}
		/*
			Returns a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual auto getCropRectangle() const -> Rectangle<>
		{
			if (m_implementation)
			{
				return m_implementation->getCropRectangle();
			}
			return {};
		}

		/*
			Returns the DIP size of the actual image.
		*/
		virtual auto getOriginalSize() const -> Size<>
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalSize();
			}
			return {};
		}
		/*
			Returns the DIP width of the actual image.
		*/
		virtual auto getOriginalWidth() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalWidth();
			}
			return 0;
		}
		/*
			Returns the DIP height of the actual image.
		*/
		virtual auto getOriginalHeight() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalHeight();
			}
			return 0;
		}

		virtual auto getOriginalPixelSize() const -> Size<Pixels>
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalPixelSize();
			}
			return {};
		}
		virtual auto getOriginalPixelWidth() const -> Pixels
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalPixelWidth();
			}
			return {};
		}
		virtual auto getOriginalPixelHeight() const -> Pixels
		{
			if (m_implementation)
			{
				return m_implementation->getOriginalPixelHeight();
			}
			return {};
		}

		//------------------------------

		/*
			Sets the way the image is fit within its bounds.
		*/
		virtual auto setBoundsSizing(ImageBoundsSizing p_sizeMode) -> void
		{
			if (m_implementation)
			{
				m_implementation->setBoundsSizing(p_sizeMode);
			}
		}
		/*
			Returns the way the image is fit within its bounds.
		*/
		virtual auto getBoundsSizing() const -> ImageBoundsSizing
		{
			if (m_implementation)
			{
				return m_implementation->getBoundsSizing();
			}
			return ImageBoundsSizing::Unknown;
		}

		/*
			Sets the way the image is positioned within its bounds.

			p_factor represents the coordinates of the point on the image that aligns with the same point but relative to the bounds.
			It is expressed as a factor of the size of the image. For example, if p_factor is (1, 1), the bottom right corner of the image will be
			aligned with the bottom right corner of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual auto setBoundsPositioning(Point<Factor> p_factor) -> void
		{
			if (m_implementation)
			{
				m_implementation->setBoundsPositioning(p_factor);
			}
		}
		/*
			Sets the way the image is positioned within its bounds on the x-axis.

			p_x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_x is expressed as a factor of the width of the image. For example, if p_x is 1, the right edge of the image will be
			aligned with the right edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual auto setBoundsPositioningX(Factor p_x) -> void
		{
			if (m_implementation)
			{
				m_implementation->setBoundsPositioningX(p_x);
			}
		}
		/*
			Sets the way the image is positioned within its bounds on the y-axis.

			p_y represents the y coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_y is expressed as a factor of the height of the image. For example, if p_y is 1, the bottom edge of the image will be
			aligned with the bottom edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual auto setBoundsPositioningY(Factor p_y) -> void
		{
			if (m_implementation)
			{
				m_implementation->setBoundsPositioningY(p_y);
			}
		}
		/*
			Returns the way the image is positioned within its bounds. See setBoundsPositioning for more info.
		*/
		virtual auto getBoundsPositioning() const -> Point<Factor>
		{
			if (m_implementation)
			{
				return m_implementation->getBoundsPositioning();
			}
			return {};
		}
		/*
			Returns the way the image is positioned within its bounds on the x-axis. See setBoundsPositioningX for more info.
		*/
		virtual auto getBoundsPositioningX() const -> Factor
		{
			if (m_implementation)
			{
				return m_implementation->getBoundsPositioningX();
			}
			return 0.f;
		}
		/*
			Returns the way the image is positioned within its bounds on the y-axis. See setBoundsPositioningY for more info.
		*/
		virtual auto getBoundsPositioningY() const -> Factor
		{
			if (m_implementation)
			{
				return m_implementation->getBoundsPositioningY();
			}
			return 0.f;
		}

		//------------------------------

		/*
			Sets how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual auto setScalingMethod(ImageScalingMethod p_scalingMethod) -> void
		{
			if (m_implementation)
			{
				m_implementation->setScalingMethod(p_scalingMethod);
			}
		}
		/*
			Returns how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual auto getScalingMethod() const -> ImageScalingMethod
		{
			if (m_implementation)
			{
				return m_implementation->getScalingMethod();
			}
			return ImageScalingMethod::Unknown;
		}

		//------------------------------

		/*
			Sets how opaque the image is being drawn.
		*/
		virtual auto setOpacity(float p_opacity) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOpacity(p_opacity);
			}
		}
		/*
			Returns how opaque the image is being drawn.
		*/
		virtual auto getOpacity() const -> float
		{
			if (m_implementation)
			{
				return m_implementation->getOpacity();
			}
			return 0;
		}

		//------------------------------

		/*
			Returns the drawn width of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual auto getInnerWidth() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getInnerWidth();
			}
			return 0;
		}
		/*
			Returns the drawn height of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual auto getInnerHeight() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getInnerHeight();
			}
			return 0;
		}
		/*
			Returns the drawn size of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual auto getInnerSize() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getInnerSize();
			}
			return {};
		}
		/*
			Returns the drawn inner bounds of the image within the outer bounds, calculated using the positioning options, sizing options and the crop rectangle.
		*/
		virtual auto getInnerBounds() const -> Rectangle<>
		{
			if (m_implementation)
			{
				return m_implementation->getInnerBounds();
			}
			return {};
		}

		//------------------------------

	protected:
		auto handleProtectedRectangleChange(Rectangle<> p_old) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->setBounds(m_bounds);
			}
		}
	
	private:
		void initializeBoundsFromImplementation()
		{
			if (m_implementation)
			{
				m_bounds = m_implementation->m_bounds;
			}
		}
	public:
		auto operator=(Image&& p_image) -> Image&
		{
			m_implementation = std::move(p_image.m_implementation);
			initializeBoundsFromImplementation();
			return *this;
		}
		auto operator=(Image const& p_image) -> Image&
		{
			m_implementation = p_image.m_implementation;
			initializeBoundsFromImplementation();
		}
	
	protected:
		friend class DrawingContext;
		Image(std::shared_ptr<Image>&& p_implementation) :
			DrawingContextObject{std::move(p_implementation)}
		{
			initializeBoundsFromImplementation();
		}
		Image(std::shared_ptr<Image> const& p_implementation) :
			DrawingContextObject{p_implementation}
		{
			initializeBoundsFromImplementation();
		}
	public:
		Image() = default;
		Image(Image&& p_image) :
			DrawingContextObject{std::move(p_image.m_implementation)}
		{
			initializeBoundsFromImplementation();
		}
		Image(Image const& p_image) :
			DrawingContextObject{p_image.m_implementation}
		{
			initializeBoundsFromImplementation();
		}
	};

	// TODO: Make ImageView class

	//------------------------------

	enum class WordWrapping
	{
		Emergency, // Wraps between words unless a word is wider than the maximum width.
		WholeWord, // Only wraps between words to prevent overflow.
		Always, // Always wraps to the next line to prevent overflow.
		Never, // Allows overflow, never wraps.
		Unknown = -1
	};

	enum class FontWeight
	{
		Thin = 100, // Thinnest option
		UltraLight = 200,
		Light = 300,
		SemiLight = 350,
		Regular = 400, // Normal thickness
		Medium = 500, // A bit thicker than regular
		SemiBold = 600,
		Bold = 700,
		UltraBold = 800,
		Black = 900, // Second most thick option
		UltraBlack = 950, // Most thick option
		Unknown = -1
	};

	enum class FontStyle
	{
		Normal,
		Oblique,
		Italic,
		Unknown = -1
	};

	/*
		The horizontal stretch of the font.
		Only some fonts have different horizontal stretch options.
	*/
	enum class FontStretch
	{
		Undefined = 0,
		UltraCondensed = 1, // Most condensed
		ExtraCondensed = 2, // Second most condensed
		Condensed = 3,
		SemiCondensed = 4,
		Medium = 5,
		SemiStretched = 6,
		Stretched = 7,
		ExtraStretched = 8, // Second most stretched
		UltraStretched = 9, // Most stretched
		Unknown = -1
	};

	enum class TextAlign
	{
		Left,
		Center,
		Right,
		Fill, // Stretches the spaces of the text to make the left and right edges of the text line up with the bounds of the text.
		Unknown = -1
	};
	enum class ReadingDirection
	{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop,
		Unknown = -1
	};

	/*
		Represents a text block which can be calculated once and drawn any number of times by a DrawingContext.
		Notice that this is not a view, but should be treated as a drawable object created by a DrawingContext.
		The memory of a text object is automatically managed and reference counted.
		Here:
			Text text_0 = getDrawingContext()->createText("some text", 12.f);
			Text text_1 = text_0;
		text_1 and text_0 are referring to the same text, and the internal text object
		is released once all references have been destroyed.
	*/
	class Text : public ProtectedRectangle, public DrawingContextObject<Text>
	{
	public:
		/*
			Sets the rules for inserting line breaks in the text to avoid overflow.
		*/
		virtual auto setWordWrapping(WordWrapping p_wordWrapping) -> void
		{
			if (m_implementation)
			{
				m_implementation->setWordWrapping(p_wordWrapping);
			}
		}
		/*
			Returns the type of rules used for inserting line breaks in the text to avoid overflow.
		*/
		virtual auto getWordWrapping() const -> WordWrapping
		{
			if (m_implementation)
			{
				return m_implementation->getWordWrapping();
			}
			return WordWrapping::Unknown;
		}

		/*
			Sets the size of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual auto fitSizeToText() -> void
		{
			if (m_implementation)
			{
				m_implementation->fitSizeToText();
			}
		}
		/*
			Sets the width of the bounding box to fit the text.
		*/
		virtual auto fitWidthToText() -> void
		{
			if (m_implementation)
			{
				m_implementation->fitWidthToText();
			}
		}
		/*
			Sets the height of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual auto fitHeightToText() -> void
		{
			if (m_implementation)
			{
				m_implementation->fitHeightToText();
			}
		}
		/*
			Returns the smallest size to contain the actual text.
			If getIsTopTrimmed() == false, the height includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual auto getMinimumSize() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getMinimumSize();
			}
			return {};
		}
		/*
			Returns the smallest width to contain the actual text.
		*/
		virtual auto getMinimumWidth() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getMinimumWidth();
			}
			return 0.f;
		}
		/*
			Returns the smallest height to contain the actual text.
			If getIsTopTrimmed() == false, this includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual auto getMinimumHeight() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getMinimumHeight();
			}
			return 0.f;
		}

		//------------------------------

		/*
			Sets whether the top of the text is trimmed so that there is no space between the top of the tallest
			character of the text and the top edge of the bounds.

			If this is false, the text is positioned within the bounds so that the baseline is at a fixed position,
			and there may be space above the characters in the text to allow this. This is the default.

			Setting this to true can be useful when you want to perfectly center text vertically.
		*/
		virtual auto setIsTopTrimmed(bool p_isTopTrimmed) -> void
		{
			if (m_implementation)
			{
				m_implementation->setIsTopTrimmed(p_isTopTrimmed);
			}
		}
		/*
			Returns whether the top of the text is trimmed so that there is no space between the top of the tallest
			character of the text and the top edge of the bounds. This is false by default.
		*/
		virtual auto getIsTopTrimmed() const -> bool
		{
			if (m_implementation)
			{
				return m_implementation->getIsTopTrimmed();
			}
			return false;
		}

		//------------------------------

		/*
			Returns the 2d position of a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position returned is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual auto getCharacterPosition(Index p_characterIndex, bool p_isRelativeToOrigin = false) const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getCharacterPosition(p_characterIndex, p_isRelativeToOrigin);
			}
			return {};
		}
		/*
			Returns the width and height of a character in the text, specified by its index in the string.
		*/
		virtual auto getCharacterSize(Index p_characterIndex) const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getCharacterSize(p_characterIndex);
			}
			return {};
		}
		/*
			Returns a rectangle enclosing a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position of the bounds returned is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual auto getCharacterBounds(Index p_characterIndex, bool p_isRelativeToOrigin = false) const -> Rectangle<>
		{
			if (m_implementation)
			{
				return m_implementation->getCharacterBounds(p_characterIndex, p_isRelativeToOrigin);
			}
			return {};
		}

		/*
			Returns the index of the character which is nearest to a point.
			p_isRelativeToOrigin is whether the position given is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual auto getNearestCharacterIndex(Point<> p_point, bool p_isRelativeToOrigin = false) const -> Index
		{
			if (m_implementation)
			{
				return m_implementation->getNearestCharacterIndex(p_point, p_isRelativeToOrigin);
			}
			return {};
		}
		/*
			Returns the index and position of the character which is nearest to a point.

			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context.
			If not, they are relative to the bounds of the text.
		*/
		virtual auto getNearestCharacterIndexAndPosition(Point<> p_point, bool p_isRelativeToOrigin = false) const 
			-> std::pair<Index, Point<>>
		{
			if (m_implementation)
			{
				return m_implementation->getNearestCharacterIndexAndPosition(p_point, p_isRelativeToOrigin);
			}
			return {};
		}
		/*
			Returns the index and bounds of the character which is nearest to a point.

			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. 
			If not, they are relative to the bounds of the text.
		*/
		virtual auto getNearestCharacterIndexAndBounds(Point<> p_point, bool p_isRelativeToOrigin = false) const 
			-> std::pair<Index, Rectangle<>>
		{
			if (m_implementation)
			{
				return m_implementation->getNearestCharacterIndexAndBounds(p_point, p_isRelativeToOrigin);
			}
			return {};
		}

		//------------------------------

		/*
			Sets how the text is placed within the bounds.
		*/
		virtual auto setTextAlign(TextAlign p_textAlign) -> void
		{
			if (m_implementation)
			{
				m_implementation->setTextAlign(p_textAlign);
			}
		}
		/*
			Returns how the text is placed within the bounds.
		*/
		virtual auto getTextAlign() const -> TextAlign
		{
			if (m_implementation)
			{
				return m_implementation->getTextAlign();
			}
			return TextAlign::Unknown;
		}

		//------------------------------

		/*
			Sets the layout direction of the text.
		*/
		virtual auto setReadingDirection(ReadingDirection p_readingDirection) -> void
		{
			if (m_implementation)
			{
				m_implementation->setReadingDirection(p_readingDirection);
			}
		}
		/*
			Returns the layout direction of the text.
		*/
		virtual auto getReadingDirection() const -> ReadingDirection
		{
			if (m_implementation)
			{
				return m_implementation->getReadingDirection();
			}
			return ReadingDirection::Unknown;
		}

		//------------------------------

		/*
			Represents a part of the text that a property-changing method will affect.
		*/
		struct TextRange
		{
			/*
				The position of the first character that a property is set on.
				If this is negative, it is relative to the end of the text.
			*/
			Index startPosition = 0;
			/*
				The number of characters that a property is set on.
				If this is negative, it goes to the left of the start position.
				If it is 0, everything after the start position is affected.
			*/
			Count length = 0;
		};

		//------------------------------

		/*
			Sets the font family to be used in a section of the text.

			p_name is the name of the font family.
		*/
		virtual auto setFontFamily(std::string_view p_name, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setFontFamily(p_name, p_range);
			}
		}

		//------------------------------

		/*
			Sets the spacing between characters in a section of the text.
		*/
		auto setCharacterSpacing(float p_characterSpacing, TextRange p_range = {}) -> void
		{
			setCharacterSpacing(p_characterSpacing*0.5f, p_characterSpacing*0.5f, p_range);
		}
		/*
			Sets the leading and trailing spacing of the characters in a section of the text.

			p_leading is the spacing before the characters of the text.
			p_trailing is the spacing after the characters of the text.
		*/
		virtual auto setCharacterSpacing(float p_leading, float p_trailing, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setCharacterSpacing(p_leading, p_trailing, p_range);
			}
		}
		/*
			Returns the spacing before one of the characters.
		*/
		virtual auto getLeadingCharacterSpacing(Index p_characterIndex = 0) const -> float
		{
			if (m_implementation)
			{
				return m_implementation->getLeadingCharacterSpacing(p_characterIndex);
			}
			return 0.f;
		}
		/*
			Returns the spacing after one of the characters.
		*/
		virtual auto getTrailingCharacterSpacing(Index p_characterIndex = 0) const -> float
		{
			if (m_implementation)
			{
				return m_implementation->getTrailingCharacterSpacing(p_characterIndex);
			}
			return 0.f;
		}

		//------------------------------

		/*
			Sets the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual auto setLineHeight(Factor p_lineHeight) -> void
		{
			if (m_implementation)
			{
				m_implementation->setLineHeight(p_lineHeight);
			}
		}
		/*
			Returns the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual auto getLineHeight() const -> Factor
		{
			if (m_implementation)
			{
				return m_implementation->getLineHeight();
			}
			return 0.f;
		}

		//------------------------------

		/*
			Sets the thickness of characters in a section of the text.
		*/
		virtual auto setFontWeight(FontWeight p_fontWeight, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setFontWeight(p_fontWeight, p_range);
			}
		}
		/*
			Returns the weight/thickness of a character in the text.
		*/
		virtual auto getFontWeight(Index p_characterPosition = 0) const -> FontWeight
		{
			if (m_implementation)
			{
				return m_implementation->getFontWeight(p_characterPosition);
			}
			return FontWeight::Unknown;
		}

		//------------------------------

		/*
			Sets the font style in a section of the text.
		*/
		virtual auto setFontStyle(FontStyle p_fontStyle, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setFontStyle(p_fontStyle, p_range);
			}
		}
		/*
			Returns the style of a character in the text.
		*/
		virtual auto getFontStyle(Index p_characterPosition = 0) const -> FontStyle
		{
			if (m_implementation)
			{
				return m_implementation->getFontStyle(p_characterPosition);
			}
			return FontStyle::Unknown;
		}

		//------------------------------

		/*
			Sets the font stretch in a section of the text. Not all fonts support this.
		*/
		virtual auto setFontStretch(FontStretch p_fontStretch, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setFontStretch(p_fontStretch, p_range);
			}
		}
		/*
			Returns the font stretch of a character in the text.
		*/
		virtual auto getFontStretch(Index p_characterPosition = 0) const -> FontStretch
		{
			if (m_implementation)
			{
				return m_implementation->getFontStretch(p_characterPosition);
			}
			return FontStretch::Unknown;
		}

		//------------------------------

		/*
			Sets the font size in a section of the text.
		*/
		virtual auto setFontSize(float p_fontSize, TextRange p_range = {}) -> void
		{
			if (m_implementation)
			{
				m_implementation->setFontSize(p_fontSize, p_range);
			}
		}
		/*
			Returns the size (height) of a character in the text.
		*/
		virtual auto getFontSize(Index p_characterPosition = 0) const -> float
		{
			if (m_implementation)
			{
				return m_implementation->getFontSize(p_characterPosition);
			}
			return 0.f;
		}

		//------------------------------

		virtual auto getString() const -> std::string_view
		{
			if (m_implementation)
			{
				return m_implementation->getString();
			}
			return "";
		}

		//------------------------------

		using ProtectedRectangle::setBounds;
		auto setBounds(Rectangle<> p_rectangle) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setBounds(p_rectangle);
			}
		}
		auto getBounds() const noexcept -> Rectangle<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getBounds();
			}
			return {};
		}

		using ProtectedRectangle::move;
		auto move(Vector2d<> p_offset) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::move(p_offset);
			}
		}
		auto moveX(Dip p_offsetX) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::moveX(p_offsetX);
			}
		}
		auto moveY(Dip p_offsetY) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::moveY(p_offsetY);
			}
		}

		using ProtectedRectangle::setTopLeft;
		auto setTopLeft(Point<> p_topLeft, bool p_willKeepSize = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setTopLeft(p_topLeft, p_willKeepSize);
			}
		}
		auto getTopLeft() const noexcept -> Point<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getTopLeft();
			}
			return {};
		}

		using ProtectedRectangle::setTopRight;
		auto setTopRight(Point<> p_topRight, bool p_willKeepSize = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setTopRight(p_topRight, p_willKeepSize);
			}
		}
		auto getTopRight() const noexcept -> Point<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getTopRight();
			}
			return {};
		}

		using ProtectedRectangle::setBottomLeft;
		auto setBottomLeft(Point<> p_bottomLeft, bool p_willKeepSize = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setBottomLeft(p_bottomLeft, p_willKeepSize);
			}
		}
		auto getBottomLeft() const noexcept -> Point<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getBottomLeft();
			}
			return {};
		}

		using ProtectedRectangle::setBottomRight;
		auto setBottomRight(Point<> p_bottomRight, bool p_willKeepSize = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setBottomRight(p_bottomRight, p_willKeepSize);
			}
		}
		auto getBottomRight() const noexcept -> Point<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getBottomRight();
			}
			return {};
		}

		using ProtectedRectangle::setCenter;
		auto setCenter(Point<> p_center) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setCenter(p_center);
			}
		}
		auto getCenter() const noexcept -> Point<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getCenter();
			}
			return {};
		}
		auto setCenterX(Dip p_x) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setCenterX(p_x);
			}
		}
		auto getCenterX() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getCenterX();
			}
			return 0.f;
		}
		auto setCenterY(Dip p_y) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setCenterY(p_y);
			}
		}
		auto getCenterY() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getCenterY();
			}
			return 0.f;
		}
		auto setLeft(Dip p_left, bool p_willKeepWidth = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setLeft(p_left, p_willKeepWidth);
			}
		}
		auto getLeft() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getLeft();
			}
			return 0.f;
		}
		auto setTop(Dip p_top, bool p_willKeepHeight = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setTop(p_top, p_willKeepHeight);
			}
		}
		auto getTop() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getTop();
			}
			return 0.f;
		}
		auto setRight(Dip p_right, bool p_willKeepWidth = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setRight(p_right, p_willKeepWidth);
			}
		}
		auto getRight() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getRight();
			}
			return 0.f;
		}
		auto setBottom(Dip p_bottom, bool p_willKeepHeight = true) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setBottom(p_bottom, p_willKeepHeight);
			}
		}
		auto getBottom() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getBottom();
			}
			return 0.f;
		}
		auto setWidth(Dip p_width) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setWidth(p_width);
			}
		}
		auto getWidth() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getWidth();
			}
			return 0.f;
		}
		auto setHeight(Dip p_height) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setHeight(p_height);
			}
		}
		auto getHeight() const noexcept -> Dip override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getHeight();
			}
			return 0.f;
		}

		using ProtectedRectangle::setSize;
		auto setSize(Size<> p_size) -> void override 
		{
			if (m_implementation)
			{
				m_implementation->ProtectedRectangle::setSize(p_size);
			}
		}
		auto getSize() const noexcept -> Size<> override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getSize();
			}
			return {};
		}

		using ProtectedRectangle::getIsIntersecting;
		auto getIsIntersecting(Rectangle<> p_rectangle) const noexcept -> bool override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getIsIntersecting(p_rectangle);
			}
			return false;
		}
		auto getIsContaining(Rectangle<> p_rectangle) const noexcept -> bool override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getIsContaining(p_rectangle);
			}
			return false;
		}

		using ProtectedRectangle::getIsContaining;
		auto getIsContaining(Point<> p_point) const noexcept -> bool override 
		{
			if (m_implementation)
			{
				return m_implementation->ProtectedRectangle::getIsContaining(p_point);
			}
			return false;
		}

		friend class DrawingContext;
		using DrawingContextObject<Text>::DrawingContextObject;
	};
	
	class LinearGradient : public DrawingContextObject<LinearGradient>
	{
	public:
		/*
			Sets an offset in the start and end positions.
		*/
		virtual auto setOffset(Point<> p_offset) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffset(p_offset);
			}
		}
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual auto setOffsetX(Dip p_x) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffsetX(p_x);
			}
		}
		/*
			Sets the vertical offset in the start position.
		*/
		virtual auto setOffsetY(Dip p_y) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffsetY(p_y);
			}
		}

		/*
			Returns the offset in the start and end positions.
		*/
		virtual auto getOffset() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getOffset();
			}
			return {};
		}
		/*
			Returns the horizontal offset in the start and end positions.
		*/
		virtual auto getOffsetX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOffsetX();
			}
			return 0.f;
		}
		/*
			Returns the vertical offset in the start and end positions.
		*/
		virtual auto getOffsetY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOffsetY();
			}
			return 0.f;
		}

		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual auto setStartPosition(Point<> p_startPosition) -> void
		{
			if (m_implementation)
			{
				m_implementation->setStartPosition(p_startPosition);
			}
		}
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual auto getStartPosition() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getStartPosition();
			}
			return {};
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual auto getStartPositionX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getStartPositionX();
			}
			return 0.f;
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual auto getStartPositionY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getStartPositionY();
			}
			return 0.f;
		}

		/*
			Sets the coordinates relative to the origin where the gradient will end.
		*/
		virtual auto setEndPosition(Point<> p_endPosition) -> void
		{
			if (m_implementation)
			{
				m_implementation->setEndPosition(p_endPosition);
			}
		}
		/*
			Returns the coordinates relative to the origin where the gradient will end.
		*/
		virtual auto getEndPosition() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getEndPosition();
			}
			return {};
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will end.
		*/
		virtual auto getEndPositionX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getEndPositionX();
			}
			return 0.f;
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will end.
		*/
		virtual auto getEndPositionY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getEndPositionY();
			}
			return 0.f;
		}

		friend class DrawingContext;
		using DrawingContextObject<LinearGradient>::DrawingContextObject;
	};

	class RadialGradient : public DrawingContextObject<RadialGradient>
	{
	public:
		/*
			Sets an offset in the start position.
		*/
		virtual auto setOffset(Point<> p_offset) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffset(p_offset);
			}
		}
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual auto setOffsetX(Dip p_x) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffsetX(p_x);
			}
		}
		/*
			Sets the vertical offset in the start position.
		*/
		virtual auto setOffsetY(Dip p_y) -> void
		{
			if (m_implementation)
			{
				m_implementation->setOffsetY(p_y);
			}
		}
		/*
			Returns the offset in the start position.
		*/
		virtual auto getOffset() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getOffset();
			}
			return {};
		}
		/*
			Returns the horizontal offset in the start position.
		*/
		virtual auto getOffsetX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOffsetX();
			}
			return 0.f;
		}
		/*
			Returns the vertical offset in the start position.
		*/
		virtual auto getOffsetY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getOffsetY();
			}
			return 0.f;
		}

		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual auto setStartPosition(Point<> p_startPosition) -> void
		{
			if (m_implementation)
			{
				m_implementation->setStartPosition(p_startPosition);
			}
		}
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual auto getStartPosition() const -> Point<>
		{
			if (m_implementation)
			{
				return m_implementation->getStartPosition();
			}
			return Point<>{};
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual auto getStartPositionX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getStartPositionX();
			}
			return 0.f;
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual auto getStartPositionY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getStartPositionY();
			}
			return 0.f;
		}

		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual auto setRadius(Size<> p_radius) -> void
		{
			if (m_implementation)
			{
				m_implementation->setRadius(p_radius);
			}
		}
		/*
			Returns the horizontal and vertical size of the gradient.
		*/
		virtual auto getRadius() const -> Size<>
		{
			if (m_implementation)
			{
				return m_implementation->getRadius();
			}
			return {};
		}
		/*
			Returns the horizontal size of the gradient.
		*/
		virtual auto getRadiusX() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getRadiusX();
			}
			return 0.f;
		}
		/*
			Returns the vertical size of the gradient.
		*/
		virtual auto getRadiusY() const -> Dip
		{
			if (m_implementation)
			{
				return m_implementation->getRadiusY();
			}
			return 0.f;
		}

		friend class DrawingContext;
		using DrawingContextObject<RadialGradient>::DrawingContextObject;
	};

	/*
		A class representing a position and a color, used to make a gradient.
		The position between 0 and 1 and is relative to the start and end positions if it's linear,
		and relative to the start position and radius if it's radial.
	*/
	struct GradientStop
	{
		Color color;
		Factor position;
	};

	/*
		Platform-specific interface for cached geometry that can be created and drawn by a DrawingContext.
		Used to draw more efficiently.
	*/
	class Geometry : public DrawingContextObject<Geometry>
	{
	public:
		friend class DrawingContext;
		using DrawingContextObject<Geometry>::DrawingContextObject;
	};

	/*
		Used to store the drawing state of a DrawingContext, which includes the current transformations.
		Create one with DrawingContext::createDrawingState().
	*/
	class DrawingState : public DrawingContextObject<DrawingState>
	{
	public:
		friend class DrawingContext;
		using DrawingContextObject<DrawingState>::DrawingContextObject;
	};

	struct TextProperties
	{
		std::string fontFamilyName = std::string{FONT_FAMILY_ROBOTO};

		FontWeight fontWeight = FontWeight::Medium;
		FontStyle fontStyle = FontStyle::Normal;
		FontStretch fontStretch = FontStretch::Medium;
		TextAlign textAlign = TextAlign::Left;
		ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		float characterSpacing = 0.f; // Only supported for text objects.
		float lineHeight = 1.f;
		float fontSize = 22.f;
	};

	enum class LineCap
	{
		Flat,
		Round,
		Square,
		Triangle,
		Unknown = -1
	};

	enum class LineJoin
	{
		Bevel,
		Miter,
		Round,
		Unknown = -1
	};

	enum class LineDashStyle
	{
		Solid,
		Dash,
		Dot,
		DashDot,
		DashDotDot,
		Custom,
		Unknown = -1
	};
	
	// TODO: Remove invalid rectangle system because it does not make sense with an OpenGL backend.
	// Always redrawing the whole screen should not be that bad for the GPU.

	/*
		A drawing context interface, created by a GUI to be used to create objects
		like text and images (and more) as well as to draw graphics in views.
	*/
	class DrawingContext
	{
	protected:
		TextProperties m_textProperties;

		static auto createImageFromImplementation(std::shared_ptr<Image>&& p_implementation) -> Image
		{
			return {std::move(p_implementation)};
		}
		static auto createTextFromImplementation(std::shared_ptr<Text>&& p_implementation) -> Text
		{
			return {std::move(p_implementation)};
		}
		static auto createLinearGradientFromImplementation(std::shared_ptr<LinearGradient>&& p_implementation) -> LinearGradient
		{
			return {std::move(p_implementation)};
		}
		static auto createRadialGradientFromImplementation(std::shared_ptr<RadialGradient>&& p_implementation) -> RadialGradient
		{
			return {std::move(p_implementation)};
		}
		static auto createGeometryFromImplementation(std::shared_ptr<Geometry>&& p_geometry) -> Geometry
		{
			return {std::move(p_geometry)};
		}
		static auto createDrawingStateFromImplementation(std::shared_ptr<DrawingState>&& p_state) -> DrawingState
		{
			return {std::move(p_state)};
		}

	public:
		/*
			Returns the image format of the given image file.
			Only the first 8 bytes of the file is needed.
		*/
		static auto getImageFormatOfFile(DataRange p_fileData) -> ImageFormat
		{
			if (p_fileData.size() < 8)
			{
				return ImageFormat::Unknown;
			}
			if (!std::memcmp(p_fileData.data(), "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8))
			{
				return ImageFormat::Png;
			}
			else if (!std::memcmp(p_fileData.data(), "\xFF\xD8\xFF", 3))
			{
				return ImageFormat::Jpeg;
			}
			else if (!std::memcmp(p_fileData.data(), "\x00\x00\x01\x00", 4))
			{
				return ImageFormat::Ico;
			}
			else if (!std::memcmp(p_fileData.data(), "\x42\x4D", 2))
			{
				return ImageFormat::Bmp;
			}
			return ImageFormat::Unknown;
		}
		/*
			Returns the image format of the given image file.
		*/
		static auto getImageFormatOfFile(std::string_view p_filePath) -> ImageFormat
		{
			if (auto fileStream = std::ifstream{p_filePath.data()})
			{
				auto signatureBytes = std::array<std::byte, 8>{};
				fileStream.read(reinterpret_cast<char*>(signatureBytes.data()), signatureBytes.size());
				return getImageFormatOfFile(signatureBytes);
			}
			return ImageFormat::Unknown;
		}

		//------------------------------

		/*
			Initializes drawing. The GUI calls this for you.
		*/
		virtual auto beginDrawing() -> void = 0;
		/*
			Finishes the drawing and shows it. The GUI calls this for you.
		*/
		virtual auto finishDrawing(Range<Rectangle<>*> p_updatedRectangles) -> void = 0;

		//------------------------------

		/*
			Creates a drawing state object.
			It can be re-used and you can call saveDrawingState and restoreDrawingState as many times as you want.
		*/
		virtual auto createDrawingState() -> DrawingState = 0;
		/*
			Saves the internal drawing state of the drawing context in a DrawingState object.
		*/
		virtual auto saveDrawingState(DrawingState const& p_drawingState) -> void = 0;
		/*
			Loads the internal drawing state of the drawing context from a DrawingState object.
		*/
		virtual auto restoreDrawingState(DrawingState const& p_drawingState) -> void = 0;

		//------------------------------

		/*
			Sets whether the target is fullscreen or windowed.
		*/
		virtual auto setIsFullscreen(bool p_isFullscreen) -> void = 0;
		/*
			Switches between windowed and fullscreen mode.
			If it is currently windowed, it switches to fullscreen, and the other way around.
		*/
		virtual auto switchFullscreen() -> void = 0;
		/*
			Returns whether the target is fullscreen or windowed.
		*/
		virtual auto getIsFullscreen() -> bool = 0;

		//------------------------------

		/*
			Enables synchronization with the monitor.
		*/
		virtual auto enableVsync() -> void = 0;
		/*
			Disables synchronization with the monitor.
		*/
		virtual auto disableVsync() -> void = 0;
		/*
			Returns whether presentation is synchronized with the monitor.
		*/
		virtual auto getIsVsyncEnabled() -> bool = 0;

		//------------------------------

		/*
			Sets the color that the target is filled with before any drawing.
		*/
		virtual auto setBackgroundColor(Color p_color) -> void = 0;
		/*
			Returns the color that the target is filled with before any drawing.
		*/
		virtual auto getBackgroundColor() -> Color = 0;

		//------------------------------

		/*
			Returns the DPI that the DrawingContext is scaling all DIP units to.
			All coordinates that the DrawingContext works with are in DIPs.
		*/
		virtual auto getDpi() -> float = 0;
		/*
			Sets the DPI that the DrawingContext is scaling all DIP units to.
			It is not recommended to call this manually, since any DPI changes are updated with this method automatically from the corresponding window.
			All coordinates that the DrawingContext works with are in DIPs.
		*/
		virtual auto setDpi(float p_dpi) -> void = 0;

		//------------------------------

		/*
			Moves the screen position of the coordinate (0, 0).
		*/
		virtual auto moveOrigin(Vector2d<> p_offset) -> void = 0;
		/*
			Sets the screen position of the coordinate (0, 0).
		*/
		virtual auto setOrigin(Point<> p_origin) -> void = 0;
		/*
			Returns the screen position of the coordinate (0, 0).
		*/
		virtual auto getOrigin() -> Point<> = 0;

		//------------------------------

		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin.
		*/
		virtual auto scale(Vector2d<Factor> p_scale) -> void = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual auto scale(Vector2d<Factor> p_scale, Point<> p_origin) -> void = 0;

		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller than normal. Everything will be scaled towards the origin.
		*/
		virtual auto setScale(Vector2d<Factor> p_scale) -> void = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics drawing so that
			it is bigger or smaller than normal. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual auto setScale(Vector2d<Factor> p_scale, Point<> p_origin) -> void = 0;
		/*
			Returns the sizing factor which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
		*/
		virtual auto getScale() -> Vector2d<Factor> = 0;
		/*
			Returns the sizing factor for the x-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
		*/
		virtual auto getScaleX() -> Factor = 0;
		/*
			Returns the sizing factor for the y-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
		*/
		virtual auto getScaleY() -> Factor = 0;

		//------------------------------

		/*
			Rotates all future graphics drawing with an angle. Graphics will be rotated relative to the origin.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual auto rotate(Arithmetic<float, Radians> p_angle) -> void = 0;
		/*
			Rotates all future graphics drawing with an angle. Graphics will be rotated relative to the origin.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual auto rotate(Arithmetic<float, Degrees> p_angle) -> void = 0;
		/*
			Rotates all future graphics drawing with an angle.
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual auto rotate(Arithmetic<float, Radians> p_angle, Point<> p_origin) -> void = 0;
		/*
			Rotates all future graphics drawing with an angle.
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual auto rotate(Arithmetic<float, Degrees> p_angle, Point<> p_origin) -> void = 0;

		//------------------------------

		/*
			Resets all graphics drawing transformations, so that every coordinate used in any drawing 
			operation is unaltered, and relative to the top-left corner of the target.
		*/
		virtual auto resetTransformations() -> void = 0;

		//------------------------------

		/*
			Resizes the drawing buffers for the window. The GUI calls this for you when it is being resized.
			The size is expressed in dips.
		*/
		virtual auto setSize(Size<> p_size) -> void = 0;
		/*
			Returns the size of the drawing buffers for the window, in dips.
		*/
		virtual auto getSize() -> Size<> = 0;

		//------------------------------

		/*
			Clears the whole buffer with the specified color.
		*/
		virtual auto clear(Color p_color) -> void = 0;
		/*
			Clears the whole buffer with a transparent background.
		*/
		virtual auto clear() -> void = 0;

		//------------------------------

		/*
			Draws a filled rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual auto fillRectangle(Rectangle<> p_rectangle) -> void = 0;

		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual auto fillRectangle(Rectangle<> p_rectangle, RectangleCorners const& p_rectangleCorners) -> void = 0;

		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual auto fillRoundedRectangle(Rectangle<> p_rectangle, Size<> p_radius) -> void = 0;

		//------------------------------

		/*
			Draws a rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual auto strokeRectangle(Rectangle<> p_rectangle, Dip p_strokeWidth) -> void = 0;

		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual auto strokeRectangle(
			Rectangle<> p_rectangle, RectangleCorners const& p_rectangleCorners, Dip p_strokeWidth
		) -> void = 0;

		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual auto strokeRoundedRectangle(Rectangle<> p_rectangle, Size<> p_radius, Dip p_strokeWidth) -> void = 0;

		//------------------------------

		/*
			Draws a filled circle using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_center is the center position of the circle.
		*/
		virtual auto fillCircle(Point<> p_center, Dip p_radius) -> void = 0;

		/*
			Draws a circle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_center is the center position of the circle.
		*/
		virtual auto strokeCircle(Point<> p_center, Dip p_radius, Dip p_strokeWidth) -> void = 0;

		virtual auto fillEllipse(Point<> p_center, Size<> p_radius) -> void = 0;

		virtual auto strokeEllipse(Point<> p_center, Size<> p_radius, Dip p_strokeWidth) -> void = 0;

		//------------------------------

		/*
			Draws a straight line between two points using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual auto drawLine(Point<> p_point_0, Point<> p_point_1, float p_thickness = 1.f) -> void = 0;

		//------------------------------

		/*
			Draws the edge of a custom shape.

			p_vertices is a vector of the points that make up the shape.
			p_lineThickness is how thicc the edges of the shape are.
			p_isClosed is whether the last vertex will be connected to the first one to close the shape.
		*/
		virtual auto strokeShape(
			Range<Point<>*> p_vertices, float p_lineThickness, bool p_isClosed = false
		) -> void = 0;
		/*
			Fills a custom shape with the current color or gradient.

			p_shape is a vector of points that make up the shape.
		*/
		virtual auto fillShape(Range<Point<>*> p_vertices) -> void = 0;

		//------------------------------

		/*
			Draws a stroked cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the stroke color with setColor().
		*/
		virtual auto strokeGeometry(Geometry const& p_geometry, float p_strokeWidth = 1.f) -> void = 0;
		/*
			Draws a filled cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the fill color with setColor().
		*/
		virtual auto fillGeometry(Geometry const& p_geometry) -> void = 0;

		//------------------------------

		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual auto createRoundedRectangleGeometry(
			Rectangle<> p_rectangle, float p_radius, bool p_isStroked = false
		) -> Geometry = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual auto createCornerRectangleGeometry(
			Rectangle<> p_rectangle, RectangleCorners const& p_corners, bool p_isStroked = false
		) -> Geometry = 0;

		//------------------------------

		/*
			Creates a geometry object that represents a polygon.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual auto createPolygonGeometry(
			Range<Point<> const*> p_vertices, bool p_isStroked = false, bool p_isClosed = true
		) -> Geometry = 0;

		//------------------------------

		/*
			Changes the way both start- and endpoints of lines are drawn.
		*/
		virtual auto setLineCap(LineCap p_lineCap) -> void = 0;
		/*
			Changes the way startpoints of lines are drawn.
		*/
		virtual auto setStartLineCap(LineCap p_lineCap) -> void = 0;
		/*
			Changes the way endpoints of lines are drawn.
		*/
		virtual auto setEndLineCap(LineCap p_lineCap) -> void = 0;
		/*
			Returns the way startpoints of lines are drawn.
		*/
		virtual auto getStartLineCap() -> LineCap = 0;
		/*
			Returns the way endpoints of lines are drawn.
		*/
		virtual auto getEndLineCap() -> LineCap = 0;

		//------------------------------

		/*
			Sets how and if lines are dashed/dotted.
		*/
		virtual auto setLineDashStyle(LineDashStyle p_dashStyle) -> void = 0;
		/*
			Returns how and if lines are dashed/dotted.
		*/
		virtual auto getLineDashStyle() -> LineDashStyle = 0;

		/*
			Sets the offset of line dashing/dotting.
		*/
		virtual auto setLineDashOffset(float p_dashOffset) -> void = 0;
		/*
			Returns the offset of line dashing/dotting.
		*/
		virtual auto getLineDashOffset() -> float = 0;

		/*
			This changes the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual auto setLineDashCap(LineCap p_dashCap) -> void = 0;
		/*
			Returns the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual auto getLineDashCap() -> LineCap = 0;

		//------------------------------

		/*
			Sets the way line joints are drawn.
		*/
		virtual auto setLineJoin(LineJoin p_lineJoin) -> void = 0;
		/*
			Returns the way line joints are drawn.
		*/
		virtual auto getLineJoin() -> LineJoin = 0;

		/*
			Sets the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual auto setLineJoinMiterLimit(float p_miterLimit) -> void = 0;
		/*
			Returns the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual auto getLineJoinMiterLimit() -> float = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the cached geometry will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip geometry.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual auto pushClipGeometry(Geometry const& p_geometry, float p_opacity = 1.f) -> void = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual auto pushClipShape(Range<Point<> const*> p_points, float p_opacity = 1.f) -> void = 0;

		/*
			This removes the last added clipping shape.
		*/
		virtual auto popClipShape() -> void = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip rectangle.
		*/
		virtual auto pushClipRectangle(Rectangle<> p_rectangle, float p_opacity = 1.f) -> void = 0;

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual auto pushClipRectangle(
			Rectangle<> p_rectangle, RectangleCorners const& p_corners, float p_opacity = 1.f
		) -> void = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual auto pushRoundedClipRectangle(
			Rectangle<> p_rectangle, float p_radius, float p_opacity = 1.f
		) -> void = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rectangle.

			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual auto createRectangleShadowImage(Size<> p_size, float p_blur, Color p_color) -> Image = 0;

		/*
			Generates an image of a shadow that is cast by a rectangle with custom corners.

			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual auto createRectangleShadowImage(
			Size<> p_size, RectangleCorners const& p_corners, float p_blur, Color p_color
		) -> Image = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rounded rectangle.

			p_size is the size of the rounded rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
			p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual auto createRoundedRectangleShadowImage(
			Size<> p_size, float p_radius, float p_blur, Color p_color
		) -> Image = 0;

		//------------------------------

		/*
			Loads an image from pixel data in BGRA format.

			p_pixelData is an array which is 4*width*height bytes in size.
			It contains the color values for every pixel in the image, row-by-row. One byte for every color channel.
		*/
		virtual auto createImage(std::byte const* p_pixelData, Size<Pixels> p_size) -> Image = 0;
		/*
			Loads an image from the data of an image file.
		*/
		virtual auto createImage(DataView p_imageData) -> Image = 0;
		/*
			Loads an image from a file. Most standard image formats/codecs are supported.
			p_filePath is the path, relative or absolute, to the image file to be loaded.
			If this returns an invalid image, then the file path is probably incorrect.
		*/
		virtual auto createImage(std::string_view p_filePath) -> Image = 0;
		/*
			Creates an image from an OS-specific handle.

			On Windows, it is an HICON or HBITMAP.
		*/
		virtual auto createImageFromHandle(void* p_handle) -> Image = 0;
		/*
			Draws an image, placed according to the image's bounds and positioning/scaling options.
		*/
		virtual auto drawImage(Image const& p_image, float p_multiplicativeOpacity = 1.f) -> void = 0;

		//------------------------------

		/*
			Creates a buffer that contains the file data of an image, encoded in the format p_format.
		*/
		virtual auto createImageFileData(
			Image const& p_image, ImageFormat p_format = ImageFormat::Png
		) -> DataVector = 0;
		/*
			Creates a stream that contains the file data of an image, encoded in the format p_format.
			On Windows, the return pointer type is IStream.
		*/
		virtual auto createImageFileDataNativeStream(
			Image const& p_image, ImageFormat p_format = ImageFormat::Png
		) -> void* = 0;
		/*
			Saves an image to a file, encoded in the format p_format.
			Returns true if it succeeded and false if it failed.
		*/
		virtual auto saveImageToFile(
			Image const& p_image, std::string_view p_filePath, ImageFormat p_format = ImageFormat::Png
		) -> bool = 0;

		//------------------------------

		/*
			Creates an OS API native image from an AvoGUI image.
			On Windows, it returns an HBITMAP.
		*/
		virtual auto createNativeImageFromImage(Image const& p_image) -> void* = 0;

		//------------------------------

		/*
			Creates a linear gradient that can be used as a brush when drawing things.
		*/
		virtual auto createLinearGradient(
			Range<GradientStop*> p_gradientStops, Point<> p_startPosition = {}, Point<> p_endPosition = {}
		) -> LinearGradient = 0;

		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual auto createRadialGradient(
			Range<GradientStop*> p_gradientStops, Point<> p_startPosition = {}, Point<> p_radius = {}
		) -> RadialGradient = 0;

		/*
			Sets a linear gradient to be used as the brush when drawing things.
		*/
		virtual auto setGradient(LinearGradient const& p_gradient) -> void = 0;
		/*
			Sets a radial gradient to be used as the brush when drawing things.
		*/
		virtual auto setGradient(RadialGradient const& p_gradient) -> void = 0;
		/*
			Sets a color to be used when drawing things.
		*/
		virtual auto setColor(Color p_color) -> void = 0;

		/*
			Sets the transparency of all graphics that will be drawn.
		*/
		virtual auto setOpacity(float p_opacity) -> void = 0;

		//------------------------------

		/*
			Adds a new font family that can be used by text.
			p_filePath is a path to a font file with a common format.
		*/
		virtual auto addFont(std::string_view p_filePath) -> void = 0;

		/*
			Adds a new font to a font family that can be used by text.
			p_data is the data that would be in a font file with a common format.
			The data is moved from p_data.
		*/
		virtual auto addFont(DataVector&& p_data) -> void = 0;
		/*
			Adds a new font to a font family that can be used by text.
			p_data is the data that would be in a font file with a common format.
		*/
		virtual auto addFont(DataView p_data) -> void = 0;

		//------------------------------

		/*
			Sets the default properties of text created with this drawing context.
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual auto setDefaultTextProperties(TextProperties const& p_textProperties) -> void = 0;
		/*
			Returns the default properties of text created with this drawing context.
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual auto getDefaultTextProperties() -> TextProperties = 0;

		//------------------------------

		/*
			Creates a new Text object which represents a pre-calculated text layout, using the current text properties.
			p_bounds is the maximum bounds of the text. If it's (0, 0, 0, 0) then the bounds will be calculated to fit the text.
		*/
		virtual auto createText(std::string_view p_string, float p_fontSize, Rectangle<> p_bounds = {}) -> Text = 0;
		/*
			Draws pre-calculated text created with the createText method.
		*/
		virtual auto drawText(Text const& p_text) -> void = 0;

		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText).
		*/
		virtual auto drawText(std::string_view p_string, Rectangle<> p_rectangle) -> void = 0;

		/*
			Lays out and draws a string at a position usinng the current text properties.
			If you're drawing the same text repeatedly, use a Text object (created with createText()).
		*/
		virtual auto drawText(std::string_view p_string, Point<> p_position) -> void = 0;

		virtual ~DrawingContext() = default;
	};

	enum class Cursor
	{
		Arrow,
		Blocked,
		Hand,
		Ibeam,
		Wait,
		ResizeAll,
		ResizeWE,
		ResizeNS,
		ResizeNESW,
		ResizeNWSE,
		Unknown = -1
	};

	//------------------------------

	enum class ModifierKeyFlags
	{
		None = 0UL,
		Control = 0x1UL,
		Alt = 0x2UL,
		Shift = 0x4UL,
		LeftMouse = 0x8UL,
		MiddleMouse = 0x10UL,
		RightMouse = 0x20UL,
		X0Mouse = 0x40UL,
		X1Mouse = 0x80UL
	};
	constexpr auto operator&(ModifierKeyFlags p_left, ModifierKeyFlags p_right) noexcept -> bool
	{
		return static_cast<uint32>(p_left) & static_cast<uint32>(p_right);
	}
	constexpr auto operator|(ModifierKeyFlags p_left, ModifierKeyFlags p_right) noexcept -> ModifierKeyFlags
	{
		return static_cast<ModifierKeyFlags>(static_cast<uint32>(p_left) | static_cast<uint32>(p_right));
	}
	constexpr auto operator|=(ModifierKeyFlags& p_left, ModifierKeyFlags p_right) noexcept -> ModifierKeyFlags&
	{
		return p_left = p_left | p_right;
	}

	enum class MouseButton
	{
		None = 0,
		Left,
		Middle,
		Right,
		X0,
		X1
	};

	class View;
	struct MouseEvent
	{
		/*
			The view that the mouse interacted with.
		*/
		View* target = nullptr;

		/*
			Coordinates of the mouse pointer.
		*/
		Point<> xy;
		/*
			X coordinate of the mouse pointer.
		*/
		Dip& x = xy.x;
		/*
			Y coordinate of the mouse pointer.
		*/
		Dip& y = xy.y;
		/*
			The movement of the mouse pointer since the last move event.
			If it is positive it has moved down and right and if it is negative it has moved left and up.
		*/
		Vector2d<> movement;
		/*
			How much the mouse wheel has been moved.
			If it is positive, the wheel has been moved away from the user, if it negative it has moved towards the user.
			It represents the number of ticks the wheel has been moved, but can be a fraction if the mouse has smooth scrolling.
		*/
		float scrollDelta = 0.f;
		/*
			The mouse button that has been pressed, released or double clicked (depending on the mouse event).
		*/
		MouseButton mouseButton = MouseButton::None;
		/*
			The modifier keys and mouse buttons that were down when the event occurred.
		*/
		ModifierKeyFlags modifierKeys = ModifierKeyFlags::None;
		/*
			This is valid for both mouse down and up events, and tells whether the event is part of a double click.
		*/
		bool isDoubleClick{};

		constexpr MouseEvent() noexcept = default;
		/*
			WARNING: VERY IMPORTANT NOT TO USE DEFAULT CONSTRUCTOR!!!
			KEEP THIS ONE!!!
			the default constructor would make the x and y reference members point 
			to the object being copied, which can result in stack corruption if
			the copied object then gets destroyed. 
		*/
		constexpr MouseEvent(MouseEvent const& p_other) noexcept :
			target{p_other.target},
			xy{p_other.xy},
			movement{p_other.movement},
			scrollDelta{p_other.scrollDelta},
			mouseButton{p_other.mouseButton},
			modifierKeys{p_other.modifierKeys},
			isDoubleClick{p_other.isDoubleClick}
		{}
	};

	//------------------------------

	enum class KeyboardKey
	{
		None = 0,
		Backspace,
		Clear,
		Tab,
		Return, // Enter and return have the same value.
		Enter = Return, // Enter and return have the same value.
		Shift,
		Control,
		Menu,
		Alt,
		CapsLock,
		Escape,
		Spacebar,
		PageUp, PageDown, Home, End,
		PrintScreen,
		Insert,
		Delete,
		Pause,
		Help,
		Separator,
		Left, Right, Up, Down,
		NumLock,
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		Add, Subtract, Multiply, Divide, Decimal,
		Number0, Number1, Number2, Number3, Number4, Number5, Number6, Number7, Number8, Number9,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
		Comma, Period, Plus, Minus,
		// These keys vary by country/region.
		Regional1, Regional2, Regional3, Regional4, Regional5, Regional6, Regional7, Regional8
	};

	struct KeyboardEvent
	{
		/*
			The keyboard key that was pressed or released. This is not valid for character press events.
		*/
		KeyboardKey key = KeyboardKey::None;
		/*
			If this is true, this character/key press event is generated after the initial attack because the key is being held down.
		*/
		bool isRepeated = false;
		/*
			The character that was pressed. This is only valid for character press events.
			Since the multibyte UTF-8 encoding is used, this is a string that could be up to 4 8-bit chars.
		*/
		std::string_view character;
		/*
			A pointer to the view that the event is directed towards.
		*/
		View* target = nullptr;

		constexpr KeyboardEvent(KeyboardKey p_key, bool p_isRepeated = false) noexcept :
			key{p_key},
			isRepeated{p_isRepeated}
		{}
		constexpr KeyboardEvent(std::string_view p_character, bool p_isRepeated = false) noexcept :
			character{p_character},
			isRepeated{p_isRepeated}
		{}
	};

	//------------------------------

	enum class DragDropOperation
	{
		Copy,
		Move,
		Link,
		None
	};

	class ClipboardData
	{
	public:
		/*
			A list of platform-specific format values where every index represents a different format with the type value at that index.
		*/
		std::vector<uint32> formats;
		/*
			Provides a more advanced, platform-specific interface for accessing dragged data.
			When data is dragged from an application, many data formats may be given which tell 
			different things about the data or represent it in different ways.
			There may be more than 1 data format with the value formats[p_formatIndex].
		*/
		virtual auto getDataForFormat(Index p_formatIndex) const -> DataView = 0;
		/*
			p_format is one of the values in the "formats" vector.
		*/
		virtual auto getFormatName(uint32 p_format) const -> std::string = 0;

		/*
			Returns the text of what is to be dropped, in UTF-8 encoding.
		*/
		virtual auto getString() const -> std::string = 0;
		/*
			Returns the text of what is to be dropped, in UTF-16 encoding.
		*/
		virtual auto getUtf16String() const -> std::u16string = 0;
		/*
			Returns whether the item to be dropped has any text.
		*/
		virtual auto getHasString() const -> bool = 0;

		/*
			Return the names of what is to be dropped, in UTF-8 encoding.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual auto getItemNames() const -> std::vector<std::string> = 0;
		/*
			Returns the names of what is to be dropped, in UTF-16 encoding.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual auto getUtf16ItemNames() const -> std::vector<std::u16string> = 0;
		/*
			Returns the number of items that have a name.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual auto getNumberOfItemNames() const -> Count = 0;

		/*
			Returns the file names of the respective file contents, in UTF-8 encoding.
		*/
		virtual auto getFileNames() const -> std::vector<std::string> = 0;
		/*
			Returns the file names of the respective file contents, in UTF-16 encoding.
		*/
		virtual auto getUtf16FileNames() const -> std::vector<std::u16string> = 0;
		/*
			Returns the file contents of every file that is being dragged.
		*/
		virtual auto getFileContents() const -> std::vector<DataVector> = 0;
		/*
			Returns the file contents of an item that is being dragged, by its index.
		*/
		virtual auto getFileContents(Index p_index) const -> DataVector = 0;
		/*
			Returns the number of dragged items that have file contents.
		*/
		virtual auto getNumberOfFiles() const -> Count = 0;

		/*
			Returns the additional data that has been assigned by an AvoGUI application.
		*/
		virtual auto getAdditionalData() const -> uint64 = 0;

		/*
			If an image is being dragged, this creates and returns an Image object representing the image that was dragged.
			If no image is being dragged, it returns 0.
		*/
		virtual auto getImage() const -> Image = 0;
	};

	struct DragDropEvent
	{
		/*
			The view that the event is directed towards.
		*/
		View* target{};
		/*
			The modifier keys that were pressed when the event fired.
		*/
		ModifierKeyFlags modifierKeys = ModifierKeyFlags::None;

		/*
			The position of the cursor in DIP view coordinates.
		*/
		Point<> xy;
		/*
			The horizontal position of the cursor in DIP view coordinates.
		*/
		Dip& x = xy.x;
		/*
			The vertical position of the cursor in DIP view coordinates.
		*/
		Dip& y = xy.y;
		/*
			The movement of the cursor in DIP view coordinates.
		*/
		Vector2d<> movement;

		/*
			Contains the data that is being dragged.
		*/
		ClipboardData* data{};

		DragDropEvent() = default;
		// DO NOT REMOVE
		DragDropEvent(DragDropEvent const& p_other) :
			target{p_other.target},
			modifierKeys{p_other.modifierKeys},
			xy{p_other.xy},
			movement{p_other.movement},
			data{p_other.data}
		{}
	};

	//------------------------------

	class Window;
	struct WindowEvent
	{
		/*
			The window that has received the event from the OS.
		*/
		Window* window = nullptr;
		/*
			The new size of the window if it has changed size (includes sizeChange/maximize/restore events).
		*/
		Size<> size;
	};

	//------------------------------

	enum class WindowStyleFlags
	{
		None = 0x0UL, // Borderless window.
		CloseButton = 0x1UL,
		Invisible = 0x2UL, // Makes the window invisible at first. You can make it visible afterwards.
		Minimized = 0x8UL,
		Maximized = 0x10UL,
		MinimizeButton = 0x20UL,
		MaximizeButton = 0x40UL,
		Resizable = 0x80UL,
		CustomBorder = 0x100UL, // This makes the client area take up the full window, and the GUI determines which areas are for resizing and moving the window.
		DefaultCustom = CustomBorder | MaximizeButton | MinimizeButton | Resizable,
		Default = CloseButton | MinimizeButton | MaximizeButton | Resizable,
		DefaultNoResize = CloseButton | MinimizeButton
	};

	constexpr auto operator&(WindowStyleFlags p_left, WindowStyleFlags p_right) noexcept -> WindowStyleFlags
	{
		return static_cast<WindowStyleFlags>(static_cast<uint32>(p_left) & static_cast<uint32>(p_right));
	}
	constexpr auto operator|(WindowStyleFlags p_left, WindowStyleFlags p_right) noexcept -> WindowStyleFlags
	{
		return static_cast<WindowStyleFlags>(static_cast<uint32>(p_left) | static_cast<uint32>(p_right));
	}
	constexpr auto operator|=(WindowStyleFlags& p_left, WindowStyleFlags p_right) noexcept -> WindowStyleFlags&
	{
		p_left = p_left | p_right;
		return p_left;
	}

	enum class WindowBorderArea
	{
		None = 0, // The area of the window is not part of the window border, meaning any mouse events are handled only by the GUI.
		TopLeftResize,
		TopResize,
		TopRightResize,
		LeftResize,
		RightResize,
		BottomLeftResize,
		BottomResize,
		BottomRightResize,
		Dragging // The area of the window is used for dragging the window, normally the title bar.
	};

	enum class WindowState
	{
		Minimized,
		Maximized,
		Restored
	};

	/*
		An abstract window, which has an OS-specific implementation.
		The window is responsible for receiving events from the OS and sending them to the GUI.
		It's like a portal between your application and the operating system.
		It is only intended to be created by a GUI, and you can access and use it from there.
	*/
	class Window
	{
	public:
		/*
			Creates the window. To close it, use close().

			p_title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.

			p_positionFactor is the position of the window, expressed as a factor between 0 and 1, where 0 means the left/top edge
			of the primary monitor and the left/top edge of the window are aligned, and 1 means the right/bottom edges are aligned.

			p_size is the size of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual auto create(
			std::string_view p_title, 
			Point<Factor> p_positionFactor, Size<> p_size, 
			WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, 
			Window* p_parent = nullptr
		) -> void = 0;
		/*
			Creates the window in the center of the screen. To close it, use close().

			p_title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.
			p_size is the size of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual auto create(
			std::string_view p_title, 
			Size<> p_size, 
			WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, 
			Window* p_parent = nullptr
		) -> void
		{
			create(p_title, {0.5f}, p_size, p_styleFlags, p_parent);
		}

	protected:
		bool m_isRunning = false;
		std::mutex m_isRunningMutex;
		std::condition_variable m_isRunningConditionVariable;

		friend class Gui;

		/*
			Makes the window start receiving events.
		*/
		auto run() -> void
		{
			m_isRunningMutex.lock();
			m_isRunning = true;
			m_isRunningMutex.unlock();
			m_isRunningConditionVariable.notify_one();
		}
	public:

		/*
			Closes the window, if the GUI associated with the window allows it.
			The animation thread will then safely destroy the window when it has terminated.
		*/
		virtual auto close() -> void = 0;
		/*
			Returns whether the OS window has been created and exists.
		*/
		virtual auto getIsOpen() const -> bool = 0;

	protected:
		bool m_willClose = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns whether the GUI and its window is awaiting being closed by the animation/drawing thread.
		*/
		auto getWillClose() -> bool
		{
			return m_willClose;
		}

		//------------------------------

		/*
			Usually used for the parent of a popup window when the popup is closed.
			Makes the window receive mouse and keyboard events again.
		*/
		virtual auto enableUserInteraction() -> void = 0;
		/*
			Usually used for the parent of a popup window when the popup is opened.
			Makes the window not receive any mouse and keyboard events, until enableUserInteraction is called.
		*/
		virtual auto disableUserInteraction() -> void = 0;
		/*
			Returns whether the window receives mouse and keyboard events.
		*/
		virtual auto getIsUserInteractionEnabled() -> bool = 0;

		//------------------------------

		/*
			Sets the text shown in the titlebar.
		*/
		virtual auto setTitle(std::string_view p_title) -> void = 0;
		/*
			Returns the text shown in the titlebar.
		*/
		virtual auto getTitle() const -> std::string = 0;

		//------------------------------

		/*
			Changes the styles that determine how the window is drawn by the OS.
			These are set when the window is created, and you can change them afterwards here.
		*/
		virtual auto setStyles(WindowStyleFlags p_styles) -> void = 0;
		/*
			Returns the current styles that determine how the window is drawn by the OS.
		*/
		virtual auto getStyles() const -> WindowStyleFlags = 0;

		//------------------------------

		/*
			Returns the OS-specific window object associated with this window.
		*/
		virtual auto getNativeHandle() const -> void* = 0;
		/*
			Returns the OS-specific window object associated with this window.
		*/
		template<typename T>
		auto getNativeHandle() const -> T
		{
			return static_cast<T>(getNativeHandle());
		}

		//------------------------------

		/*
			Changes whether the client area of the window fills the whole screen.
		*/
		virtual auto setIsFullscreen(bool p_isFullscreen) -> void = 0;
		/*
			Switches between fullscreen and windowed mode.
			If the window is currently windowed, it will become fullscreen, and the other way around.
		*/
		virtual auto switchFullscreen() -> void = 0;
		/*
			Returns whether the client area of the window fills the whole screen.
		*/
		virtual auto getIsFullscreen() const -> bool = 0;

		//------------------------------

		/*
			Makes the window invisible and disabled.
		*/
		virtual auto hide() -> void = 0;
		/*
			Shows the window and enables it.
		*/
		virtual auto show() -> void = 0;

		/*
			Makes the size of the window as big as possible so that the border still shows.
		*/
		virtual auto maximize() -> void = 0;
		/*
			Hides the window temporarily in a taskbar where it can be restored again.
		*/
		virtual auto minimize() -> void = 0;
		/*
			Shows/opens the window as it was before it was minimized.
		*/
		virtual auto restore() -> void = 0;

		/*
			Changes the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
			Methods maximize(), minimize() and restore() do the same thing.
		*/
		virtual auto setState(WindowState p_state) -> void = 0;
		/*
			Returns the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
		*/
		virtual auto getState() const -> WindowState = 0;

		//------------------------------

		/*
			Sets the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual auto setPosition(Point<Pixels> p_position) -> void = 0;
		/*
			Returns the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual auto getPosition() const -> Point<Pixels> = 0;
		/*
			Returns the position of the left edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual auto getPositionX() const -> Pixels = 0;
		/*
			Returns the position of the top edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual auto getPositionY() const -> Pixels = 0;

		/*
			Sets the size of the client area of the window, in dip units.
		*/
		auto setSize(Size<> p_size) -> void 
		{
			setPixelSize(dipsToPixels(p_size));
		}
		/*
			Returns the size of the client area of the window, in dip units.
		*/
		auto getSize() const -> Size<> 
		{
			return pixelsToDips(getPixelSize());
		}
		/*
			Returns the width of the client area of the window, in dip units.
		*/
		auto getWidth() const -> Dip 
		{
			return pixelsToDips(getPixelWidth());
		}
		/*
			Returns the height of the client area of the window, in dip units.
		*/
		auto getHeight() const -> Dip 
		{
			return pixelsToDips(getPixelHeight());
		}

		/*
			Sets the size of the client area of the window, in pixel units.
		*/
		virtual auto setPixelSize(Size<Pixels> p_size) -> void = 0;
		/*
			Returns the size of the client area of the window, in pixel units.
		*/
		virtual auto getPixelSize() const -> Size<Pixels> = 0;
		/*
			Returns the width of the client area of the window, in pixel units.
		*/
		virtual auto getPixelWidth() const -> Pixels = 0;
		/*
			Returns the height of the client area of the window, in pixel units.
		*/
		virtual auto getPixelHeight() const -> Pixels = 0;
		
		/*
			Sets the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		auto setMinSize(Size<> p_minSize) -> void
		{
			setMinPixelSize(dipsToPixels(p_minSize));
		}
		/*
			Returns the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		auto getMinSize() const -> Size<>
		{
			return pixelsToDips(getMinPixelSize());
		}
		/*
			Returns the smallest allowed width for the window when the user is resizing it, in dip units.
		*/
		auto getMinWidth() const -> Dip
		{
			return pixelsToDips(getMinPixelWidth());
		}
		/*
			Returns the smallest allowed height for the window when the user is resizing it, in dip units.
		*/
		auto getMinHeight() const -> Dip
		{
			return pixelsToDips(getMinPixelHeight());
		}

		/*
			Sets the smallest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual auto setMinPixelSize(Size<Pixels> p_minSize) -> void = 0;
		/*
			Returns the smallest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMinPixelSize() const -> Size<Pixels> = 0;
		/*
			Returns the smallest allowed width for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMinPixelWidth() const -> Pixels = 0;
		/*
			Returns the smallest allowed height for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMinPixelHeight() const -> Pixels = 0;

		/*
			Sets the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		auto setMaxSize(Size<> p_maxSize) -> void
		{
			setMaxPixelSize(dipsToPixels(p_maxSize));
		}
		/*
			Returns the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		auto getMaxSize() const -> Size<>
		{
			return pixelsToDips(getMaxPixelSize());
		}
		/*
			Returns the biggest allowed width for the window when the user is resizing it, in dip units.
		*/
		auto getMaxWidth() const -> Dip
		{
			return pixelsToDips(getMaxPixelWidth());
		}
		/*
			Returns the biggest allowed height for the window when the user is resizing it, in dip units.
		*/
		auto getMaxHeight() const -> Dip
		{
			return pixelsToDips(getMaxPixelHeight());
		}

		/*
			Sets the biggest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual auto setMaxPixelSize(Size<Pixels> p_maxSize) -> void = 0;
		/*
			Returns the biggest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMaxPixelSize() const -> Size<Pixels> = 0;
		/*
			Returns the biggest allowed width for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMaxPixelWidth() const -> Pixels = 0;
		/*
			Returns the biggest allowed height for the window when the user is resizing it, in pixel units.
		*/
		virtual auto getMaxPixelHeight() const -> Pixels = 0;

		//------------------------------

		/*
			Returns the bounds of the current monitor used by the window, in pixel units.
		*/
		virtual auto getMonitorBounds() const -> Rectangle<Pixels> = 0;
		/*
			Returns the virtual position of the current monitor used by the window relative to other monitors, in pixel units.
		*/
		virtual auto getMonitorPosition() const -> Point<Pixels> = 0;
		/*
			Returns the size of the current monitor used by the window, in pixel units.
		*/
		virtual auto getMonitorSize() const -> Size<Pixels> = 0;
		/*
			Returns the width of the current monitor used by the window, in pixel units.
		*/
		virtual auto getMonitorWidth() const -> Pixels = 0;
		/*
			Returns the height of the current monitor used by the window, in pixel units.
		*/
		virtual auto getMonitorHeight() const -> Pixels = 0;

		//------------------------------

		/*
			Returns the bounds of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual auto getWorkAreaBounds() const -> Rectangle<Pixels> = 0;
		/*
			Returns the virtual position of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual auto getWorkAreaPosition() const -> Point<Pixels> = 0;
		/*
			Returns the size of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual auto getWorkAreaSize() const -> Size<Pixels> = 0;
		/*
			Returns the width of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual auto getWorkAreaWidth() const -> Pixels = 0;
		/*
			Returns the height of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual auto getWorkAreaHeight() const -> Pixels = 0;

		//------------------------------

		/*
			Returns whether a key is currently pressed down.
		*/
		virtual auto getIsKeyDown(KeyboardKey p_key) const -> bool = 0;
		/*
			Returns whether a mouse button is currently pressed down.
		*/
		virtual auto getIsMouseButtonDown(MouseButton p_button) const -> bool = 0;
		/*
			Returns the position of the mouse cursor, relative to the top-left corner of the window.
		*/
		virtual auto getMousePosition() const -> Point<> = 0;

		//------------------------------

		/*
			Changes what the mouse cursor looks like.
		*/
		virtual auto setCursor(Cursor p_cursor) -> void = 0;
		/*
			Returns the current mouse cursor.
		*/
		virtual auto getCursor() const -> Cursor = 0;

		//------------------------------

	protected:
		Factor m_dipToPixelFactor = 1.f;
	public:
		/*
			Returns the factor that is used to convert DIP units to pixel units in the window.
			This can change during the lifetime of the window, if the user drags it to a monitor with a different DPI for example.
		*/
		auto getDipToPixelFactor() const -> Factor
		{
			return m_dipToPixelFactor;
		}
		/*
			Converts device independent units to pixel units.
			This conversion depends on the DPI of the monitor.
			1 dip will convert to more pixels on a high DPI display.
		*/
		auto dipsToPixels(Dip p_dip) const -> Pixels
		{
			return p_dip*m_dipToPixelFactor;
		}
		/*
			Converts device independent units to pixel units.
			This conversion depends on the DPI of the monitor.
			1 dip will convert to more pixels on a high DPI display.
		*/
		template<template<typename>typename C>
		auto dipsToPixels(Vector2dBase<Dip, C> p_container) const -> Vector2dBase<Pixels, C>
		{
			return Vector2dBase<Pixels, C>{p_container*m_dipToPixelFactor};
		}
		/*
			Converts pixel units to device independent units.
			This conversion depends on the DPI of the monitor.
			1 pixel will convert to fewer dips on a high DPI display.
		*/
		auto pixelsToDips(Pixels p_pixels) const -> Dip
		{
			return p_pixels/m_dipToPixelFactor;
		}
		/*
			Converts pixel units to device independent units.
			This conversion depends on the DPI of the monitor.
			1 pixel will convert to fewer dips on a high DPI display.
		*/
		template<template<typename>typename C>
		auto pixelsToDips(Vector2dBase<Pixels, C> p_container) const -> Vector2dBase<Dip, C>
		{
			return Vector2dBase<Dip, C>{p_container/m_dipToPixelFactor};
		}

		//------------------------------

		/*
			Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropString(
			std::string_view p_string, Image const& p_dragImage = {},
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;
		/*
			Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropString(
			std::u16string_view p_string, Image const& p_dragImage = {},
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;

		/*
			Runs a blocking loop that allows the user to drag image data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropImage(
			Image const& p_image, Image const& p_dragImage = {},
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;

		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFile(
			DataView p_data, std::string_view p_name, 
			Image const& p_dragImage = {}, Point<> p_dragImageCursorPosition = {},
			uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;
		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFile(
			DataView p_data, std::u16string_view p_name, 
			Image const& p_dragImage = {}, Point<> p_dragImageCursorPosition = {}, 
			uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;

		/*
			Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFile(
			std::string_view p_path, Image const& p_dragImage = {}, 
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;
		/*
			Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFile(
			std::u16string_view p_path, Image const& p_dragImage = {}, 
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;

		/*
			Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFiles(
			Range<std::string*> p_paths, Image const& p_dragImage = {},
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;
		/*
			Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual auto dragAndDropFiles(
			Range<std::u16string*> p_paths, Image const& p_dragImage = {},
			Point<> p_dragImageCursorPosition = {}, uint64 p_additionalData = 0u
		) -> DragDropOperation = 0;

		//------------------------------

		/*
			Gives a UTF-8 encoded string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual auto setClipboardString(std::string_view p_string, uint64 p_additionalData = 0u) const -> void = 0;
		/*
			Gives a UTF-16 encoded string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual auto setClipboardString(std::u16string_view p_string, uint64 p_additionalData = 0u) const -> void = 0;

		/*
			Gives an image for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardImage(Image const& p_image, uint64 p_additionalData = 0u) const -> void = 0;

		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFile(DataView p_data, std::string_view p_name, uint64 p_additionalData = 0u) const -> void = 0;
		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFile(DataView p_data, std::u16string_view p_name, uint64 p_additionalData = 0u) const -> void = 0;
		/*
			Gives a UTF-8 file path for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFile(std::string_view p_path, uint64 p_additionalData = 0u) const -> void = 0;
		/*
			Gives a UTF-16 file path for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFile(std::u16string_view p_path, uint64 p_additionalData = 0u) const -> void = 0;

		/*
			Gives UTF-8 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFiles(Range<std::string*> p_paths, uint64 p_additionalData = 0u) const -> void = 0;
		/*
			Gives UTF-16 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual auto setClipboardFiles(Range<std::u16string*> p_paths, uint64 p_additionalData = 0u) const -> void = 0;

		/*
			Returns the data that is currently stored on the clipboard.
		*/
		virtual auto getClipboardData() const -> std::unique_ptr<ClipboardData> = 0;

		//------------------------------
		// Window events

	protected:
		auto sendWindowCloseEvents(WindowEvent const& p_event) -> bool
		{
			return std::all_of(
				windowCloseListeners.begin(), windowCloseListeners.end(), 
				[&](auto const& listener){ return listener(p_event); }
			);
		}

	public:
		using WindowListeners = EventListeners<void(WindowEvent const&)>;
		EventListeners<bool(WindowEvent const&)> windowCloseListeners;
		WindowListeners windowCreateListeners;
		WindowListeners windowDestroyListeners;
		WindowListeners windowMinimizeListeners;
		WindowListeners windowMaximizeListeners;
		WindowListeners windowRestoreListeners;
		WindowListeners windowSizeChangeListeners;
		WindowListeners windowFocusGainListeners;
		WindowListeners windowFocusLoseListeners;
	};

	class Gui;

	/*
		A rectangle that can draw itself and receive events. Used for GUI components and stuff.
	*/
	class View : public Component, public ProtectedRectangle
	{
	private:
		friend class Gui;

	public:
		View(View* p_parent, Rectangle<> p_bounds = {});
		View(View* p_parent, Id p_id, Rectangle<> p_bounds = {});
		~View() override;

	protected:
		Geometry m_clipGeometry;
		/*
			LIBRARY IMPLEMENTED
			This is called whenever the clipping geometry of the view needs to be updated.
			You can override this if you want a custom clipping geometry, just replace m_clipGeometry.
		*/
		virtual auto updateClipGeometry() -> void;

	public:
		/*
			Sets the geometry being used to clip the view's contents.
			The clip geometry of the view is by default updated automatically in the updateGeometry method when the size has changed.
			Note that hit testing is not by default affected by this, override getIsContaining(Point<>) if you want custom hit testing.
		*/
		auto setClipGeometry(Geometry const& p_geometry) -> void
		{
			m_clipGeometry = p_geometry;
		}
		/*
			Returns the geometry being used to clip the view's contents.
		*/
		auto getClipGeometry() const -> Geometry const&
		{
			return m_clipGeometry;
		}

		//------------------------------

	private:
		bool m_isOverlay = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			If you set this to true, this view will not block any mouse events from reaching views below this one.
			Whether this view is a mouse listener has no effect on this.
		*/
		auto setIsOverlay(bool p_isOverlay) -> void
		{
			m_isOverlay = p_isOverlay;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view blocks mouse events from reaching views below this one.
			False means it blocks, true means it does not.
		*/
		auto getIsOverlay() const -> bool
		{
			return m_isOverlay;
		}

		//------------------------------

	private:
		bool m_isVisible = true;
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets whether the view is visible and can receive events.
		*/
		auto setIsVisible(bool p_isVisible) -> void
		{
			if (p_isVisible != m_isVisible)
			{
				m_isVisible = p_isVisible;
				invalidate();
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether the view is visible and can receive events.
		*/
		auto getIsVisible() const -> bool
		{
			return m_isVisible;
		}

		//------------------------------

	private:
		Factor m_opacity = 1.f;
	public:
		/*
			Sets how opaque the view and its children are (multiplied with parent opacity).
		*/
		auto setOpacity(Factor p_opacity) -> void
		{
			m_opacity = p_opacity;
		}
		/*
			Returns how opaque the view and its children are (multiplied with parent opacity).
		*/
		auto getOpacity() const -> Factor
		{
			return m_opacity;
		}

		//------------------------------

		// TODO: Add view transforms

		//------------------------------

	private:
		Cursor m_cursor = Cursor::Arrow;
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the cursor that will by default be shown when the mouse enters the view.
			The default implementation of handleMouseBackgroundEnter sets the cursor to this one, and you can override this behaviour.
			This method also calls enableMouseEvents().
		*/
		auto setCursor(Cursor p_cursor) -> void
		{
			m_cursor = p_cursor;
			enableMouseEvents();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the cursor that will by default be shown when the mouse enters the view.
		*/
		auto getCursor() const -> Cursor
		{
			return m_cursor;
		}

		//------------------------------

	private:
		Gui* m_gui = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the highest view in the hierarchy, the GUI.
		*/
		auto getGui() const -> Gui*
		{
			return m_gui;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the highest view in the hierarchy, the GUI.
			It is cast to another pointer type.
		*/
		template<typename T>
		auto getGui() const -> T*
		{
			return dynamic_cast<T*>(m_gui);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the object used for drawing.
			The same as calling getGui()->getDrawingContext(), but more convenient.
		*/
		auto getDrawingContext() const -> DrawingContext*;
		/*
			LIBRARY IMPLEMENTED
			Returns the window that is attached to the GUI.
		*/
		auto getWindow() const -> Window*;

		//------------------------------

	private:
		std::vector<std::unique_ptr<Animation>> m_animations;

	public:

		/*
			Creates an animation that is released by this view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
		*/
		auto addAnimation(Easing p_easing, float p_milliseconds) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), p_easing, p_milliseconds)
			).get();
		}
		/*
			Creates an animation.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
		*/
		auto addAnimation(Id p_easingId, float p_milliseconds) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), getThemeEasing(p_easingId), p_milliseconds)
			).get();
		}
		/*
			Creates an animation that is released by this view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes 
			the current animation value as a parameter.
		*/
		template<typename Callable>
		auto addAnimation(
			Easing p_easing, float p_milliseconds, 
			Callable const& p_callback
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), p_easing, p_milliseconds, p_callback)
			).get();
		}
		/*
			Creates an animation that is released by the view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes 
			the current animation value as a parameter.
		*/
		template<typename Callable>
		auto addAnimation(
			Id p_easingId, float p_milliseconds,
			Callable const& p_callback
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), getThemeEasing(p_easingId), p_milliseconds, p_callback)
			).get();
		}

		/*
			Creates an animation that is released by this view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_duration is the duration of the animation, can be changed later on the returned object.
		*/
		template<typename DurationType, typename DurationPeriod>
		auto addAnimation(
			Easing p_easing, 
			chrono::duration<DurationType, DurationPeriod> p_duration
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), p_easing, p_milliseconds)
			).get();
		}
		/*
			Creates an animation that is released by the view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_duration is the duration of the animation, can be changed later on the returned object.
		*/
		template<typename DurationType, typename DurationPeriod>
		auto addAnimation(
			Id p_easingId, 
			chrono::duration<DurationType, DurationPeriod> p_duration
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), getThemeEasing(p_easingId), p_milliseconds)
			).get();
		}
		/*
			Creates an animation that is released by this view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_duration is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes 
			the current animation value as a parameter.
		*/
		template<typename DurationType, typename DurationPeriod, typename Callable>
		auto addAnimation(
			Easing p_easing, 
		    chrono::duration<DurationType, DurationPeriod> const& p_duration, 
			Callable const& p_callback
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), p_easing, p_duration, p_callback)
			).get();
		}
		/*
			Creates an animation that is released by the view when it is destroyed.
			It is recommended to create Animation objects as stack-allocated member variables of the view 
			instead of using these methods if you can.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_duration is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes 
			the current animation value as a parameter.
		*/
		template<typename DurationType, typename DurationPeriod, typename Callable>
		auto addAnimation(
			Id p_easingId, 
			chrono::duration<DurationType, DurationPeriod> const& p_duration, 
			Callable const& p_callback
		) -> Animation*
		{
			return m_animations.emplace_back(
				std::make_unique<Animation>(getGui(), getThemeEasing(p_easingId), p_duration, p_callback)
			).get();
		}

	private:
		/*
			LIBRARY IMPLEMENTED
			Makes sure the view is drawn at the correct time, according to elevation.
		*/
		auto updateViewDrawingIndex(View* p_view) -> void;

		Index m_index = 0;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the index of this view relative to its siblings.
		*/
		auto getIndex() const -> Index
		{
			return m_index;
		}

	private:
		Index m_layerIndex = 0;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the layer index of the view, how deep down the view hierarchy it is.
			The GUI view has a layer index of 0.
		*/
		auto getLayerIndex() const -> Index
		{
			return m_layerIndex;
		}

	private:
		View* m_parent = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
			Attaches this view to a new parent, which will manage the lifetime of the view unless you've called remember() on it.
			If the parameter is 0, the view is only detached from its old parent, and is left alone with no parents :^(.
		*/
		auto setParent(View* const p_container) -> void
		{
			if (p_container == m_parent)
			{
				return;
			}

			Component::setParent(p_container);

			if (m_parent)
			{
				m_parent->removeChildView(this);
			}

			m_parent = p_container;
			if (p_container)
			{
				m_gui = m_parent->m_gui;

				m_index = m_parent->m_childViews.size();
				if (reinterpret_cast<View*>(m_gui) == this)
				{
					m_layerIndex = 0;
				}
				else
				{
					m_layerIndex = m_parent->m_layerIndex + 1U;
				}
				m_absolutePosition = m_parent->getAbsoluteTopLeft() + m_bounds.getTopLeft();

				m_parent->m_childViews.push_back(this);

				m_parent->childViewAttachmentListeners(this);
				m_parent->updateViewDrawingIndex(this);
			}
			else
			{
				m_layerIndex = 0;
				m_index = 0;
			}
		}

	private:
		std::vector<View*> m_childViews;

		/*
			Applies an operation to all child views within this view, 
			and to the child views of those child views and so on.
			This is done in determined sequential order, the same
			order as the child views appear in their vector container.
		*/
		template<typename Callable>
		auto applyToAllChildViewsRecursively(Callable&& p_apply) -> void
		{
			auto const* currentContainer = this;
			auto startIndex = Index{0};
			while (true)
			{
				auto const nextContainer = std::find_if(
					currentContainer->begin() + startIndex, currentContainer->end(),
					[&](View* const childView){
						std::forward<Callable>(p_apply)(childView);
						return childView->getHasChildViews();
					}
				);
				if (nextContainer == currentContainer->end())
				{
					if (currentContainer == this)
					{
						break;
					}
					else
					{
						startIndex = currentContainer->getIndex() + 1;
						currentContainer = currentContainer->getParent<View>();
					}
				}
				else
				{
					currentContainer = *nextContainer;
					startIndex = 0;
				}
			}
		}

	public:
		/*
			LIBRARY IMPLEMENTED
			Removes a child view from this view. This forgets the view being removed.
			If you haven't remembered it yourself, it will get deleted.
		*/
		auto removeChildView(View* const p_view) -> void
		{
			if (p_view && p_view->m_parent == this)
			{
				removeChildView(p_view->m_index);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Removes a child view from this view. This forgets the view being removed.
			If you haven't remembered it yourself, it will get deleted.
		*/
		auto removeChildView(Index const p_viewIndex) -> void
		{
			auto const childToRemove = m_childViews[p_viewIndex];
			childViewDetachmentListeners(childToRemove);

			childToRemove->m_parent = nullptr;

			for (auto const a : Indices{p_viewIndex, m_childViews})
			{
				m_childViews[a] = m_childViews[a + 1];
				m_childViews[a]->m_index = a;
			}
			m_childViews.pop_back();

			removeChild(childToRemove);
		}
		/*
			LIBRARY IMPLEMENTED
			Forgets the children views and empties this view from children.
		*/
		auto removeAllChildViews() -> void
		{
			while (!m_childViews.empty()) // That function naming, ew... Why didn't they call it getIsEmpty? empty() should be emptying something >:^(
			{
				auto const child = m_childViews.back();
				childViewDetachmentListeners(child);
				child->m_parent = nullptr;
				m_childViews.pop_back();

				removeChild(child);
			}
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the child view at an index.
		*/
		auto getChildView(Index const p_viewIndex) const -> View*
		{
			return m_childViews[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the child view at an index, casted to a pointer of another type.
		*/
		template<typename T>
		auto getChildView(Index const p_viewIndex) const -> T*
		{
			return dynamic_cast<T*>(m_childViews[p_viewIndex]);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a vector containing the child views that are attached to this view.
		*/
		auto getChildViews() const -> std::vector<View*> const&
		{
			return m_childViews;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the number of child views that are attached to this view.
		*/
		auto getNumberOfChildViews() const -> Count
		{
			return static_cast<Count>(m_childViews.size());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether the view has any child views.
		*/
		auto getHasChildViews() const noexcept -> bool
		{
			return !m_childViews.empty();
		}

		using iterator = std::vector<View*>::iterator;
		/*
			LIBRARY IMPLEMENTED
			Returns the begin iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		auto begin() noexcept -> iterator
		{
			return m_childViews.begin();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the end iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		auto end() noexcept -> iterator
		{
			return m_childViews.end();
		}
		
		using const_iterator = std::vector<View*>::const_iterator;
		/*
			LIBRARY IMPLEMENTED
			Returns the begin iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		auto begin() const noexcept -> const_iterator
		{
			return m_childViews.begin();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the end iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		auto end() const noexcept -> const_iterator
		{
			return m_childViews.end();
		}

		using reverse_iterator = std::vector<View*>::reverse_iterator;
		/*
			LIBRARY IMPLEMENTED
			Returns a reverse iterator pointing to the last child view.
			Incrementing this iterator moves it backwards towards the first child view.
		*/
		auto rbegin() noexcept -> reverse_iterator
		{
			return m_childViews.rbegin();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a reverse iterator pointing to a non-existing child view preceding the first one.
			Incrementing a reverse_iterator moves it backwards towards the first element in the container.
		*/
		auto rend() noexcept -> reverse_iterator
		{
			return m_childViews.rend();
		}

		using const_reverse_iterator = std::vector<View*>::const_reverse_iterator;
		/*
			LIBRARY IMPLEMENTED
			Returns a reverse iterator pointing to the last child view.
			Incrementing this iterator moves it backwards towards the first child view.
		*/
		auto rbegin() const noexcept -> const_reverse_iterator
		{
			return m_childViews.rbegin();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a reverse iterator pointing to a non-existing child view preceding the first one.
			Incrementing a const_reverse_iterator moves it backwards towards the first element in the container.
		*/
		auto rend() const noexcept -> const_reverse_iterator
		{
			return m_childViews.rend();
		}

		//------------------------------

	private:
		Image m_shadowImage;
		float m_elevation = 0.f;

	public:
		/*
			Updates the shadow bounds and the shadow image.
		*/
		auto updateShadow() -> void;

		/*
			LIBRARY IMPLEMENTED
			Sets the elevation of the view. This both changes its shadow (if the view has shadow) and drawing order.
			The higher the elevation is, the later it will get drawn.
			If p_elevation is negative, it is set from the top of the elevation space.
		*/
		auto setElevation(float p_elevation) -> void
		{
			p_elevation = static_cast<float>(p_elevation < 0.f)*std::numeric_limits<float>::max() + p_elevation;

			if (m_elevation != p_elevation)
			{
				m_elevation = p_elevation;
				updateShadow();
				m_parent->updateViewDrawingIndex(this);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the elevation of the view. See the setElevation method.
		*/
		auto getElevation() const -> float
		{
			return m_elevation;
		}

	private:
		bool m_hasShadow = true;
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets whether the elevation is shown with a shadow.
		*/
		auto setHasShadow(bool p_hasShadow) -> void;
		/*
			LIBRARY IMPLEMENTED
			Returns whether the elevation is shown with a shadow.
		*/
		auto getHasShadow() const -> bool
		{
			return m_hasShadow;
		}

	private:
		Rectangle<> m_shadowBounds;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the rectangle that represents the area where the shadow is drawn, relative to the view position.
			The view is always contained within the shadow bounds.
		*/
		auto getShadowBounds() const -> Rectangle<>
		{
			return m_shadowBounds;
		}

		//------------------------------

	private:
		bool m_isInAnimationUpdateQueue = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			Queues an animation update for the next frame.
		*/
		auto queueAnimationUpdate() -> void;
		/*
			USER IMPLEMENTED
			Updates things like animations and does anything that you never want to happen more than once every frame.
			Call queueAnimationUpdate() when you want this method to be called in the next interval.
			This system allows for animations to only get updated when they have to.
		*/
		virtual auto updateAnimations() -> void {}

		//------------------------------

	private:
		Rectangle<> m_lastInvalidatedShadowBounds;
		/*
			Draws the shadow of the view.
		*/
		auto drawShadow(DrawingContext* p_drawingContext) -> void;

	public:
		/*
			LIBRARY IMPLEMENTED
			Call this if you want the view to get redrawn. Adds an invalid rectangle to the window or two if the view has been moved.
		*/
		auto invalidate() -> void;

		/*
			USER IMPLEMENTED
			Draws the contents of the view.
			This method is called by default from the other draw method that also takes the target rectangle as input.
			You often don't need to use that parameter.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
		*/
		virtual auto draw(DrawingContext* p_drawingContext) -> void {}
		/*
			USER IMPLEMENTED
			Draws the content of the view. Override this method if you want the target rectangle, override the overloaded
			method that only takes the drawing context otherwise.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI.
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual auto draw(DrawingContext* p_drawingContext, Rectangle<> p_targetRectangle) -> void
		{
			draw(p_drawingContext);
		}

		/*
			USER IMPLEMENTED
			Draws on top of child views.
			This method is called by default from the other drawOverlay method that also takes the target rectangle as input.
			You do not often care about that parameter.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
		*/
		virtual auto drawOverlay(DrawingContext* p_drawingContext) -> void {}

		/*
			USER IMPLEMENTED
			Draws on top of child views. Override this method if you want the target rectangle, override the overloaded
			method that only takes the drawing context otherwise.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI.
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual auto drawOverlay(DrawingContext* p_drawingContext, Rectangle<> p_targetRectangle) -> void
		{
			drawOverlay(p_drawingContext);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the smallest possible rectangle that contains all child views belonging to this View.
			The rectangle is relative to the position of this view.
		*/
		auto calculateContentBounds() const -> Rectangle<>
		{
			if (m_childViews.empty())
			{
				return {};
			}

			return {
				calculateContentLeft(), 
				calculateContentTop(), 
				calculateContentRight(), 
				calculateContentBottom()
			};
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the width of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		auto calculateContentWidth() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}
			return calculateContentRight() - calculateContentLeft();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the height of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		auto calculateContentHeight() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}
			return calculateContentBottom() - calculateContentTop();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the size of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		auto calculateContentSize() const -> Size<>
		{
			return calculateContentBounds().getSize();
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the leftmost edge of all child views belonging to this View.
			The returned offset is relative to the left edge of this view.
		*/
		auto calculateContentLeft() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			auto view = *std::min_element(m_childViews.begin(), m_childViews.end(), [](View* a, View* b) { return a->getLeft() < b->getLeft(); });
			return view->getLeft();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the rightmost edge of all child views belonging to this View.
			The returned offset is relative to the left edge of this view.
		*/
		auto calculateContentRight() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			auto view = *std::max_element(m_childViews.begin(), m_childViews.end(), [](View* a, View* b) { return a->getRight() < b->getRight(); });
			return view->getRight();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost edge of all child views belonging to this View.
			The returned offset is relative to the top edge of this view.
		*/
		auto calculateContentTop() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			auto const view = *std::min_element(m_childViews.begin(), m_childViews.end(), [](View* a, View* b) { return a->getTop() < b->getTop(); });
			return view->getTop();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bottommost edge of all child views belonging to this View.
			The returned offset is relative to the top edge of this view.
		*/
		auto calculateContentBottom() const -> Dip
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			auto const view = *std::max_element(m_childViews.begin(), m_childViews.end(), [](View* a, View* b) { return a->getBottom() < b->getBottom(); });
			return view->getBottom();
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and/or change the size of this view.
		*/
		auto setPadding(Dip const p_padding) -> void
		{
			setPadding(p_padding, p_padding, p_padding, p_padding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and/or change the size of this view.

			p_horizontalPadding is the spacing at the left and right edges
			p_verticalPadding is the spacing at the top and bottom edges
		*/
		auto setPadding(Dip const p_horizontalPadding, Dip const p_verticalPadding) -> void
		{
			setPadding(p_horizontalPadding, p_horizontalPadding, p_verticalPadding, p_verticalPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and change the size of this view.
		*/
		auto setPadding(
			Dip const p_leftPadding, 
			Dip const p_topPadding, 
			Dip const p_rightPadding, 
			Dip const p_bottomPadding
		) -> void
		{
			auto const contentBounds = calculateContentBounds();
			Vector2d const offset = {
				p_leftPadding - contentBounds.left,
				p_topPadding - contentBounds.top
			};
			for (auto* const child : m_childViews)
			{
				child->move(offset);
			}
			setSize({
				contentBounds.getWidth() + p_leftPadding + p_rightPadding, 
				contentBounds.getHeight() + p_topPadding + p_bottomPadding
			});
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the leftmost edge of the contents and the left edge of this View.
			This may move the child views with a uniform offset and change the width of this view.
		*/
		auto setLeftPadding(Dip const p_leftPadding) -> void
		{
			auto const offset = p_leftPadding - calculateContentLeft();
			for (auto* const child : m_childViews)
			{
				child->moveX(offset);
			}
			setWidth(getWidth() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the rightmost edge of the contents and the right edge of this View.
			This changes the width of this view.
		*/
		auto setRightPadding(Dip const p_rightPadding) -> void
		{
			setWidth(calculateContentRight() + p_rightPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the topmost edge of the contents and the top edge of this View.
			This may move the child views with a uniform offset and change the height of this view.
		*/
		auto setTopPadding(Dip const p_topPadding) -> void
		{
			auto const offset = p_topPadding - calculateContentTop();
			for (auto* const child : m_childViews)
			{
				child->moveY(offset);
			}
			setHeight(getHeight() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the bottommost edge of the contents and the bottom edge of this View.
			This changes the height of this view.
		*/
		auto setBottomPadding(Dip const p_bottomPadding) -> void
		{
			setHeight(calculateContentBottom() + p_bottomPadding);
		}

		//------------------------------

		/*
			Listener signature:
				void (Id id, Color color)
			See View::handleThemeColorChange for more information.
		*/
		EventListeners<void(Id, Color)> themeColorChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme color has changed, not including initialization.
		*/
		virtual auto handleThemeColorChange(Id p_id, Color p_newColor) -> void {}

		/*
			Listener signature:
				void (Id id, Easing easing)
			See View::handleThemeEasingChange for more information.
		*/
		EventListeners<void(Id, Easing)> themeEasingChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme easing has changed, not including initialization.
		*/
		virtual auto handleThemeEasingChange(Id p_id, Easing p_newEasing) -> void {};

		/*
			Listener signature:
				void (Id id, float value)
			See View::handleThemeValueChange for more information.
		*/
		EventListeners<void(Id, float)> themeValueChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme value has changed, not including initialization.
		*/
		virtual auto handleThemeValueChange(Id p_id, float p_newValue) -> void {};

		//------------------------------

	private:
		Theme* m_theme = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the theme that is used by this view.
		*/
		auto getTheme() const -> Theme const*
		{
			return m_theme;
		}

	private:
		template<typename T, typename U>
		auto propagateThemePropertyChange(
			auto (View::* p_function)(Id, T, bool) -> void, 
			Id p_id, 
			U&& p_property, 
			bool p_willAffectChildren
		) -> void
		{
			if (p_willAffectChildren)
			{
				applyToAllChildViewsRecursively([&](View* const view){
					(childView->*p_function)(p_id, std::forward<U>(p_property), false);
				});
			}

			// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
			// It's not the other way around because the parent lays out the children and the size of the children may changed in the handler.
			if (!m_theme)
			{
				m_theme = new Theme;
			}
			else if (m_theme->getReferenceCount() > 1)
			{
				m_theme->forget();
				m_theme = new Theme{*m_theme};
			}
		}

	public:
		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default color that can be changed.
			These colors may be used by views that come with the library, but you can use them yourself too.
			The default color IDs are in the ThemeColors namespace.
			If p_id is anything else, the color is kept in the theme and you can use it yourself.

			If p_willAffectChildren is true, all children and views below those too will change this color in their themes.
		*/
		auto setThemeColor(Id const p_id, Color const p_color, bool const p_willAffectChildren = true) -> void
		{
			propagateThemePropertyChange(&View::setThemeColor, p_id, p_color, p_willAffectChildren);

			if (auto& color = m_theme->colors[p_id];
			    color != p_color)
			{
				color = p_color;
				themeColorChangeListeners(p_id, p_color);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme colors.
			Example usage:
				using namespace ThemeColors;
				setThemeColors({
					{background, 0.f},
					{onBackground, {1.f, 0.f, 0.2f}},
				});

			See setThemeColor for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, Color>>>
		auto setThemeColors(Pairs const& p_pairs, bool const p_willAffectChildren = true) -> void
		{
			for (auto const& [id, color] : p_pairs)
			{
				setThemeColor(id, color, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getThemeColor(Id const p_id) const -> Color
		{
			return m_theme->colors[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_color is inserted into the theme with the id p_id if it doesn't already have a value.
		*/
		auto initializeThemeColor(Id const p_id, Color const p_color) -> void
		{
			m_theme->colors.insert({p_id, p_color});
		}

		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default easing that can be changed.
			These easings may be used by views that come with the library, but you can use them yourself too.
			The default easing IDs are in the ThemeEasings namespace.
			If p_id is anything else, the easing is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this easing in their themes.
		*/
		auto setThemeEasing(Id const p_id, Easing const p_easing, bool const p_willAffectChildren = true) -> void
		{
			propagateThemePropertyChange(&View::setThemeEasing, p_id, p_easing, p_willAffectChildren);

			if (auto& easing = m_theme->easings[p_id];
			    easing != p_easing)
			{
				easing = p_easing;
				themeEasingChangeListeners(p_id, p_easing);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme easings.
			Example usage:
				using namespace ThemeEasings;
				setThemeEasings({
					{in, {1.f, 0.f, 1.f, 1.f}},
					{inOut, {1.f, 0.f, 0.f, 1.f}},
				});

			See setThemeEasing for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, Easing>>>
		auto setThemeEasings(Pairs const& p_pairs, bool const p_willAffectChildren = true) -> void
		{
			for (auto const& [id, easing] pair : p_pairs)
			{
				setThemeEasing(id, easing, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getThemeEasing(Id const p_id) const -> Easing
		{
			return m_theme->easings[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_easing is inserted into the theme with the ID p_id if it doesn't already have a value.
		*/
		auto initializeThemeEasing(Id const p_id, Easing const p_easing) -> void
		{
			m_theme->easings.insert({p_id, p_easing});
		}

		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default value that can be changed.
			These values may be used by views that come with the library, but you can use them yourself too.
			The default value IDs are in the ThemeValues namespace.
			If p_id is anything else, the value is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this value in their themes.
		*/
		auto setThemeValue(Id const p_id, float const p_value, bool const p_willAffectChildren = true) -> void
		{
			propagateThemePropertyChange(&View::setThemeValue, p_id, p_value, p_willAffectChildren);

			if (auto& value = m_theme->values[p_id];
			    value != p_value)
			{
				value = p_value;
				themeValueChangeListeners(p_id, p_value);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme values.
			Example usage:
				using namespace ThemeValues;
				setThemeValues({
					{hoverAnimationDuration, 100},
					{tooltipFontSize, 13.f},
				});

			See setThemeValue for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, float>>>
		auto setThemeValues(Pairs const& p_pairs, bool const p_willAffectChildren = true) -> void
		{
			for (auto const& [id, value] : p_pairs)
			{
				setThemeValue(id, value, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getThemeValue(Id const p_id) const -> float
		{
			return m_theme->values[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_value is inserted into the theme with the ID p_id if it doesn't already have a value.
		*/
		auto initializeThemeValue(Id const p_id, float const p_value) -> void
		{
			m_theme->values.insert({p_id, p_value});
		}

		//------------------------------

	private:
		Point<> m_absolutePosition;

		/*
			Moves the point(s) representing the absolute position(s) of this view and/or all children of this view (recursively).
			The absolute positions of views are used often for mouse event targeting, among other things.
			Because of this, it is pre-calculated in this way only when this view or a parent view has moved.
		*/
		auto moveAbsolutePositions(Vector2d<> const p_offset, bool const p_willUpdateChildren = true) -> void
		{
			m_absolutePosition += p_offset;

			if (p_willUpdateChildren && !m_childViews.empty())
			{
				applyToAllChildViewsRecursively([&](View* const view){
					view->moveAbsolutePositions(p_offset, false);
				});
			}
		}
		auto calculateAbsolutePositionRelativeTo(Point<> p_position) const -> Point<>;

		auto createBoundsChange()
		{
			struct _BoundsChange
			{
				View* const view;
				Rectangle<> const boundsBefore;

				_BoundsChange(View* p_view) :
					view{p_view},
					boundsBefore{p_view->m_bounds}
				{}
				~_BoundsChange()
				{
					view->sendBoundsChangeEvents(boundsBefore);
				}
			};
			return _BoundsChange{this};
		}
		
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		auto setAbsoluteBounds(Rectangle<> const p_rectangle) -> void
		{
			auto const change = createBoundsChange();

			if (auto const offset = Vector2d{p_rectangle.getTopLeft() - m_absolutePosition})
			{
				moveAbsolutePositions(offset);
				m_bounds.moveTopLeft(offset);
			}

			m_bounds.setBottomRight(m_bounds.getTopLeft() + p_rectangle.getSize(), false);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteBounds() const -> Rectangle<>
		{
			return {m_absolutePosition, m_bounds.getSize()};
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view shadow relative to the top left corner of the GUI.
		*/
		auto getAbsoluteShadowBounds() const -> Rectangle<>
		{
			// Rectangle<> bounds;
			// bounds.left = m_absolutePosition.x + (m_bounds.getWidth() - m_shadowBounds.getWidth())*0.5f;
			// bounds.top = m_absolutePosition.y + (m_bounds.getHeight() - m_shadowBounds.getHeight())*0.5f;
			// bounds.right = bounds.left + m_shadowBounds.getWidth();
			// bounds.bottom = bounds.top + m_shadowBounds.getHeight();
			// return bounds;

			return {Point{m_absolutePosition + (m_bounds.getSize() - m_shadowBounds.getSize())/2}, m_shadowBounds.getSize()};
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		auto setAbsoluteTopLeft(Point<> const p_position, bool const p_willKeepSize = true) -> void
		{
			if (auto const offset = Vector2d{p_position - m_absolutePosition})
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(offset);
				m_bounds.setTopLeft(m_bounds.getTopLeft() + offset, p_willKeepSize);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top left corner of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteTopLeft() const -> Point<>
		{
			return m_absolutePosition;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		auto setAbsoluteTopRight(Point<> const p_position, bool const p_willKeepSize = true) -> void
		{
			if (auto const offset = Vector2d{
					p_position.x - m_absolutePosition.x + m_bounds.left - m_bounds.right,
					p_position.y - m_absolutePosition.y
				})
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions({p_willKeepSize ? offset.x : 0, offset.y});
				m_bounds.setTopRight(m_bounds.getTopRight() + offset, p_willKeepSize);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top right corner of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteTopRight() const -> Point<>
		{
			return {m_absolutePosition.x + m_bounds.getWidth(), m_absolutePosition.y};
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		auto setAbsoluteBottomLeft(Point<> const p_position, bool const p_willKeepSize = true) -> void
		{
			if (auto const offset = Vector2d{
					p_position.x - m_absolutePosition.x,
					p_position.y - m_absolutePosition.y + m_bounds.top - m_bounds.bottom
				})
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(offset.x, p_willKeepSize ? offset.y : 0.f);
				m_bounds.setBottomLeft(m_bounds.getBottomLeft() + offset, p_willKeepSize);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom left corner of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteBottomLeft() const -> Point<>
		{
			return {m_absolutePosition.x, m_absolutePosition.y + m_bounds.bottom - m_bounds.top};
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		auto setAbsoluteBottomRight(Point<> const p_position, bool const p_willKeepSize = true) -> void
		{
			if (auto const offset = Vector2d{
					p_position.x - m_absolutePosition.x + m_bounds.left - m_bounds.right,
					p_position.y - m_absolutePosition.y + m_bounds.top - m_bounds.bottom
			    })
			{
				auto const change = createBoundsChange();
				if (p_willKeepSize)
				{
					moveAbsolutePositions(offset);
					m_bounds += offset;
				}
				else
				{
					m_bounds.moveBottomRight(offset);
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom right corner of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteBottomRight() const -> Point<>
		{
			return {m_absolutePosition.x + m_bounds.right - m_bounds.left, m_absolutePosition.y + m_bounds.bottom - m_bounds.top};
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the GUI.
		*/
		auto setAbsoluteCenter(Point<> const p_position) -> void
		{
			if (auto const offset = Vector2d{p_position - m_absolutePosition - getSize()/2})
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(offset);
				m_bounds += offset;
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the horizontal center coordinate of the view relative to the left edge of the GUI.
		*/
		auto setAbsoluteCenterX(float const p_x) -> void
		{
			if (auto const offsetX = p_x - m_absolutePosition.x - getWidth()*0.5f)
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(offsetX, 0);
				m_bounds.moveX(offsetX);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the vertical center coordinate of the view relative to the top edge of the GUI.
		*/
		auto setAbsoluteCenterY(float const p_y) -> void
		{
			if (auto const offsetY = p_y - m_absolutePosition.y - getHeight()*0.5f)
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(0.f, offsetY);
				m_bounds.moveY(offsetY);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the center coordinates of the view relative to the top left corner of the GUI.
		*/
		auto getAbsoluteCenter() const noexcept -> Point<>
		{
			return m_absolutePosition + getSize()/2;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the x-axis center coordinate of the view relative to the left edge of the GUI.
		*/
		auto getAbsoluteCenterX() const noexcept -> float
		{
			return m_absolutePosition.x + getWidth()*0.5f;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the y-axis center coordinate of the view relative to the top edge of the GUI.
		*/
		auto getAbsoluteCenterY() const noexcept -> float
		{
			return m_absolutePosition.y + getHeight()*0.5f;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the left coordinate of this view and updates the layout relative to the left edge of the GUI.
			If p_willKeepWidth is true, the right coordinate will also be changed so that the width of the view stays the same.
		*/
		auto setAbsoluteLeft(float const p_left, bool const p_willKeepWidth = true) -> void
		{
			if (auto const offset = p_left - m_absolutePosition.x)
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(offset, 0);
				m_bounds.setLeft(m_bounds.left + offset, p_willKeepWidth);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the left coordinate of this view relative to the left edge of the GUI.
		*/
		auto getAbsoluteLeft() const noexcept -> float
		{
			return m_absolutePosition.x;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top coordinate of this view relative to the top edge of the GUI.
			If p_willKeepHeight is true, the bottom coordinate will also be changed so that the height of the view stays the same.
		*/
		auto setAbsoluteTop(float const p_top, bool const p_willKeepHeight = true) -> void
		{
			if (auto const offset = p_top - m_absolutePosition.y)
			{
				auto const change = createBoundsChange();
				moveAbsolutePositions(0, offset);
				m_bounds.setTop(m_bounds.top + offset, p_willKeepHeight);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the top coordinate of this view relative to the top edge of the GUI.
		*/
		auto getAbsoluteTop() const noexcept -> float
		{
			return m_absolutePosition.y;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the right coordinate of this view relative to the left edge of the GUI.
			If p_willKeepWidth is true, the left coordinate will also be changed so that the width of the view stays the same.
		*/
		auto setAbsoluteRight(float const p_right, bool const p_willKeepWidth = true) -> void
		{
			if (auto const offset = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right)
			{
				auto const change = createBoundsChange();
				if (p_willKeepWidth)
				{
					moveAbsolutePositions(offset, 0);
					m_bounds.moveX(offset);
				}
				else
				{
					m_bounds.right += offset;
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the right edge of this view relative to the left edge of the GUI.
		*/
		auto getAbsoluteRight() const noexcept -> float
		{
			return m_absolutePosition.x + m_bounds.right - m_bounds.left;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom coordinate of this view relative to the top edge of the GUI and updates the layout.
			If p_willKeepHeight is true, the top coordinate will also be changed so that the height of the view stays the same.
		*/
		auto setAbsoluteBottom(float const p_bottom, bool const p_willKeepHeight = true) -> void
		{
			if (auto const offset = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom)
			{
				auto const change = createBoundsChange();
				if (p_willKeepHeight)
				{
					m_bounds.moveY(offset);
					moveAbsolutePositions(0, offset);
				}
				else
				{
					m_bounds.bottom += offset;
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the bottom edge of this view relative to the top edge of the GUI.
		*/
		auto getAbsoluteBottom() const noexcept -> float
		{
			return m_absolutePosition.y + m_bounds.bottom - m_bounds.top;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps a rectangle that is relative to the top left corner of the parent.
			If you have a custom clipping geometry, you could override this.
		*/
		auto getIsIntersecting(Rectangle<> const p_rectangle) const noexcept -> bool override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || 
			    m_corners.topRightSizeX && m_corners.topRightSizeY ||
			    m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || 
			    m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsIntersecting(p_rectangle))
				{
					if (p_rectangle.right < m_bounds.left + m_corners.topLeftSizeX && 
					    p_rectangle.bottom < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.left + m_corners.topLeftSizeX - p_rectangle.right, 
								(m_bounds.top + m_corners.topLeftSizeY - p_rectangle.bottom)*
								m_corners.topLeftSizeX/m_corners.topLeftSizeY
							) < m_corners.topLeftSizeX*m_corners.topLeftSizeX;
						}
						return p_rectangle.bottom > m_bounds.top + m_corners.topLeftSizeY - 
							(p_rectangle.right - m_bounds.left)*m_corners.topLeftSizeY/m_corners.topLeftSizeX;
					}
					else if (p_rectangle.right < m_bounds.left + m_corners.bottomLeftSizeX && 
					         p_rectangle.top > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.left + m_corners.bottomLeftSizeX - p_rectangle.right, 
								(m_bounds.bottom - m_corners.bottomLeftSizeY - p_rectangle.top)*
								m_corners.bottomLeftSizeX/m_corners.bottomLeftSizeY
							) < m_corners.bottomLeftSizeX*m_corners.bottomLeftSizeX;
						}
						return p_rectangle.top < m_bounds.bottom - m_corners.bottomLeftSizeY + 
							(p_rectangle.right - m_bounds.left)*m_corners.bottomLeftSizeY/m_corners.bottomLeftSizeX;
					}
					else if (p_rectangle.left > m_bounds.right - m_corners.topRightSizeX && 
					         p_rectangle.bottom < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.right - m_corners.topRightSizeX - p_rectangle.left, 
								(m_bounds.top + m_corners.topRightSizeY - p_rectangle.bottom)*
								m_corners.topRightSizeX/m_corners.topRightSizeY
							) < m_corners.topRightSizeX*m_corners.topRightSizeX;
						}
						return p_rectangle.bottom > m_bounds.top + (m_bounds.right - p_rectangle.left)*
							m_corners.topRightSizeY/m_corners.topRightSizeX;
					}
					else if (p_rectangle.left > m_bounds.right - m_corners.bottomRightSizeX && 
					         p_rectangle.top > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.right - m_corners.bottomRightSizeX - p_rectangle.left, 
								(m_bounds.bottom - m_corners.bottomRightSizeY - p_rectangle.top)*
								m_corners.bottomRightSizeX/m_corners.bottomRightSizeY
							) < m_corners.bottomRightSizeX*m_corners.bottomRightSizeX;
						}
						return p_rectangle.top < m_bounds.bottom - (m_bounds.right - p_rectangle.left)*
							m_corners.bottomRightSizeY/m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsIntersecting(p_rectangle);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether a rectangle can be contained within this view. The rectangle is relative to the parent of this view.
			If you have a custom clipping geometry, you could override this.
		*/
		auto getIsContaining(Rectangle<> const p_rectangle) const noexcept -> bool override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_rectangle))
				{
					if (p_rectangle.left < m_bounds.left + m_corners.topLeftSizeX && 
					    p_rectangle.top < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Vector2d<>::getLengthSquared(
									m_bounds.left + m_corners.topLeftSizeX - p_rectangle.left, 
									(m_bounds.top + m_corners.topLeftSizeY - p_rectangle.top)*m_corners.topLeftSizeX/m_corners.topLeftSizeY
								) > m_corners.topLeftSizeX*m_corners.topLeftSizeX)
							{
								return false;
							}
						}
						else if (p_rectangle.top > m_bounds.top + m_corners.topLeftSizeY - (p_rectangle.left - m_bounds.left)*
						         m_corners.topLeftSizeY/m_corners.topLeftSizeX)
						{
							return false;
						}
					}
					else if (p_rectangle.left < m_bounds.left + m_corners.bottomLeftSizeX && 
					         p_rectangle.bottom > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Vector2d<>::getLengthSquared(
									m_bounds.left + m_corners.bottomLeftSizeX - p_rectangle.left, 
									(m_bounds.bottom - m_corners.bottomLeftSizeY - p_rectangle.bottom)*
									m_corners.bottomLeftSizeX/m_corners.bottomLeftSizeY
								) > m_corners.bottomLeftSizeX*m_corners.bottomLeftSizeX)
							{
								return false;
							}
						}
						else if (p_rectangle.bottom < m_bounds.bottom - m_corners.bottomLeftSizeY + 
							(m_bounds.right - p_rectangle.left)*m_corners.topRightSizeY/m_corners.topRightSizeX)
						{
							return false;
						}
					}
					else if (p_rectangle.right > m_bounds.right - m_corners.topRightSizeX && 
					         p_rectangle.top < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Vector2d<>::getLengthSquared(
									m_bounds.right - m_corners.topRightSizeX - p_rectangle.right, 
									(m_bounds.top + m_corners.topRightSizeY - p_rectangle.top)*
									m_corners.topRightSizeX/m_corners.topRightSizeY
								) > m_corners.topRightSizeX*m_corners.topRightSizeX)
							{
								return false;
							}
						}
						else if (p_rectangle.top > m_bounds.top + (m_bounds.right - p_rectangle.right)*
							m_corners.topRightSizeY/m_corners.topRightSizeY)
						{
							return false;
						}
					}
					else if (p_rectangle.right > m_bounds.right - m_corners.bottomRightSizeX && 
					         p_rectangle.bottom > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Vector2d<>::getLengthSquared(
									m_bounds.right - m_corners.bottomRightSizeX - p_rectangle.right, 
									(m_bounds.bottom - m_corners.bottomRightSizeY - p_rectangle.bottom)*
									m_corners.bottomRightSizeX/m_corners.bottomRightSizeY
								) > m_corners.bottomRightSizeX*m_corners.bottomRightSizeX)
							{
								return false;
							}
						}
						else if (p_rectangle.bottom < m_bounds.bottom - (m_bounds.right - p_rectangle.right)*
							m_corners.bottomRightSizeY/m_corners.bottomRightSizeX)
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_rectangle);
		}

		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the parent of this view.
			If you have a custom clipping geometry, you could override this.
		*/
		auto getIsContaining(Point<> const p_point) const noexcept -> bool override
		{
			auto const x = p_point.x, 
			           y = p_point.y;
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_point))
				{
					if (x < m_bounds.left + m_corners.topLeftSizeX && y < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.left + m_corners.topLeftSizeX - x, 
								(m_bounds.top + m_corners.topLeftSizeY - y)*m_corners.topLeftSizeX/m_corners.topLeftSizeY
							) < m_corners.topLeftSizeX*m_corners.topLeftSizeX;
						}
						return y > m_bounds.top + m_corners.topLeftSizeY - (x - m_bounds.left)*m_corners.topLeftSizeY/m_corners.topLeftSizeX;
					}
					else if (x > m_bounds.right - m_corners.topRightSizeX && y < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topRightType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.right - m_corners.topRightSizeX - x, 
								(m_bounds.top + m_corners.topRightSizeY - y)*m_corners.topRightSizeX/m_corners.topRightSizeY
							) < m_corners.topRightSizeX*m_corners.topRightSizeX;
						}
						return y > m_bounds.top + (m_bounds.right - x)*m_corners.topRightSizeY/m_corners.topRightSizeX;
					}
					else if (x < m_bounds.left + m_corners.bottomLeftSizeX && y > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.bottomLeftType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.left + m_corners.bottomLeftSizeX - x, 
								(m_bounds.bottom - m_corners.bottomLeftSizeY - y)*m_corners.bottomLeftSizeX/m_corners.bottomLeftSizeY
							) < m_corners.bottomLeftSizeX*m_corners.bottomLeftSizeX;
						}
						return y < m_bounds.bottom - m_corners.bottomLeftSizeY + 
							(x - m_bounds.left)*m_corners.bottomLeftSizeY/m_corners.bottomLeftSizeX;
					}
					else if (x > m_bounds.right - m_corners.bottomRightSizeX && y > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.bottomRightType == RectangleCornerType::Round)
						{
							return Vector2d<>::getLengthSquared(
								m_bounds.right - m_corners.bottomRightSizeX - x, 
								(m_bounds.bottom - m_corners.bottomRightSizeY - y)*m_corners.bottomRightSizeX/m_corners.bottomRightSizeY
							) < m_corners.bottomRightSizeX*m_corners.bottomRightSizeX;
						}
						return y < m_bounds.bottom - (m_bounds.right - x)*m_corners.bottomRightSizeY/m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_point);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
		*/
		auto getIsContainingAbsolute(Point<> const p_point) const noexcept -> bool
		{
			if (m_parent)
			{
				return getIsContaining(p_point - m_parent->getAbsoluteTopLeft());
			}
			return getIsContaining(p_point);
		}

		//------------------------------
		// Size change events

	private:
		auto handleProtectedRectangleChange(Rectangle<> const p_oldRectangle) -> void override 
		{
			if (p_oldRectangle != m_bounds)
			{
				if (auto const newTopLeft = m_bounds.getTopLeft(), oldTopLeft = p_oldRectangle.getTopLeft(); 
				    oldTopLeft != newTopLeft)
				{
					moveAbsolutePositions(Vector2d{newTopLeft - oldTopLeft});
				}
				sendBoundsChangeEvents(p_oldRectangle);
			}
		}

		virtual auto sendBoundsChangeEvents(Rectangle<> p_previousBounds) -> void;
	public:
		EventListeners<void(Rectangle<>)> boundsChangeListeners;
		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the bounds of the view have been changed.
		*/
		virtual auto handleBoundsChange(Rectangle<> p_previousBounds) -> void {}

	public:
		/*
			Listener signature:
				void (View* target, float previousWidth, float previousHeight)
			target is a pointer to the view that changed size.
			See View::handleSizeChange for more info.
		*/
		EventListeners<void(Size<>)> sizeChangeListeners;
		/*
			LIBRARY IMPLEMENTED
			This calls handleSizeChange() by default. Override this method if you need to know the previous size of the view.
		*/
		virtual auto handleSizeChange(Size<> const) -> void
		{
			handleSizeChange();
		}
		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the size of the view has been changed.
		*/
		virtual auto handleSizeChange() -> void {}

		//------------------------------

	private:
		RectangleCorners m_corners;

	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the corners of the view. p_radius is the radius of the corner circles.
		*/
		auto setCornerRadius(float const p_radius) -> void
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_radius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_radius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_radius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_radius;
			m_corners.topLeftType = m_corners.topRightType = 
				m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the different corners of the view, as the radius of a quarter circle.
		*/
		auto setCornerRadius(
			float const p_topLeftRadius, float const p_topRightRadius, 
		    float const p_bottomLeftRadius, float const p_bottomRightRadius
		) -> void
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_topLeftRadius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_topRightRadius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_bottomLeftRadius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_bottomRightRadius;
			m_corners.topLeftType = m_corners.topRightType = 
				m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
			updateShadow();
		}

		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		auto setCornerCutSize(float const p_cutSize) -> void
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_cutSize;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_cutSize;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_cutSize;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_cutSize;
			m_corners.topLeftType = m_corners.topRightType = 
				m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Cut;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		auto setCornerCutSize(
			float const p_topLeftSize, float const p_topRightSize, 
			float const p_bottomLeftSize, float const p_bottomRightSize
		) -> void
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_topLeftSize;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_topRightSize;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_bottomLeftSize;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_bottomRightSize;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Cut;
			updateClipGeometry();
			updateShadow();
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the shapes of the corners of the view.
		*/
		auto setCorners(RectangleCorners const& p_corners) -> void
		{
			m_corners = p_corners;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the shapes of the corners of the view.
		*/
		auto getCorners() -> RectangleCorners&
		{
			return m_corners;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether any of the corners of the view are non-rectangular.
		*/
		auto getHasCornerStyles() const -> bool
		{
			return m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY;
		}

		//------------------------------

		EventListeners<void(KeyboardEvent const&)> characterInputListeners;
		/*
			USER IMPLEMENTED
			This method is called when a character key has been pressed.
			Only p_event.character and p_event.isRepeated are valid for this event type.
		*/
		virtual auto handleCharacterInput(KeyboardEvent const& p_event) -> void {}

		EventListeners<void(KeyboardEvent const&)> keyboardKeyDownListeners;
		/*
			USER IMPLEMENTED
			This method is called when a keyboard key has been pressed.
			Only p_event.key and p_event.isRepeated are valid for this event type.
		*/
		virtual auto handleKeyboardKeyDown(KeyboardEvent const& p_event) -> void {}

		EventListeners<void(KeyboardEvent const&)> keyboardKeyUpListeners;
		/*
			USER IMPLEMENTED
			This method is called when a keyboard key has been released.
			Only p_event.key is valid for this event type.
		*/
		virtual auto handleKeyboardKeyUp(KeyboardEvent const& p_event) -> void {}

		EventListeners<void()> keyboardFocusLoseListeners;
		/*
			USER IMPLEMENTED
			Gets called when another keyboard event listener becomes the target of keyboard events.
		*/
		virtual auto handleKeyboardFocusLose() -> void {}

		EventListeners<void()> keyboardFocusGainListeners;
		/*
			USER IMPLEMENTED
			Gets called when this keyboard event listener becomes the target of keyboard events.
		*/
		virtual auto handleKeyboardFocusGain() -> void {}

		//------------------------------

	private:
		bool m_areDragDropEventsEnabled = false;
		bool m_isDraggingOver = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		auto enableDragDropEvents() -> void
		{
			m_areDragDropEventsEnabled = true;
		}
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		auto disableDragDropEvents() -> void
		{
			m_areDragDropEventsEnabled = false;
		}
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		auto getAreDragDropEventsEnabled() const -> bool
		{
			return m_areDragDropEventsEnabled;
		}

		/*
			LIBRARY IMPLEMENTED (only default behavior)
			This is used to tell the OS what type of operation the application supports for the dragged data when dropped at the mouse location specified by p_event.x and p_event.y.
			The coordinates are relative to the top-left corner of this view.
		*/
		virtual auto getDragDropOperation(DragDropEvent const& p_event) const -> DragDropOperation
		{
			return DragDropOperation::None;
		}

		EventListeners<void(DragDropEvent const&)> dragDropEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor enters the bounds of the view during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual auto handleDragDropEnter(DragDropEvent const& p_event) -> void {}

		EventListeners<void(DragDropEvent const&)> dragDropBackgroundEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor enters the parts of the view that are not occupied by children, during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual auto handleDragDropBackgroundEnter(DragDropEvent const& p_event) -> void {}

		EventListeners<void(DragDropEvent const&)> dragDropMoveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor moves over the view during a drag and drop operation.
		*/
		virtual auto handleDragDropMove(DragDropEvent const& p_event) -> void {}

		EventListeners<void(DragDropEvent const&)> dragDropLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor leaves the bounds of the view during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual auto handleDragDropLeave(DragDropEvent const& p_event) -> void {}

		EventListeners<void(DragDropEvent const&)> dragDropBackgroundLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor leaves the parts of the view that are not occupied by children, during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual auto handleDragDropBackgroundLeave(DragDropEvent const& p_event) -> void {}

		EventListeners<void(DragDropEvent const&)> dragDropFinishListeners;
		/*
			USER IMPLEMENTED
			Gets called when the user drops data above the view, finishing a drag and drop operation.
			p_event contains information about the event, including the data and data type of what was dropped.
		*/
		virtual auto handleDragDropFinish(DragDropEvent const& p_event) -> void {}

		//------------------------------

	private:
		bool m_areMouseEventsEnabled = false;
		bool m_isMouseHovering = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		auto enableMouseEvents() -> void
		{
			m_areMouseEventsEnabled = true;
		}
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		auto disableMouseEvents() -> void
		{
			m_areMouseEventsEnabled = false;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getAreMouseEventsEnabled() const -> bool
		{
			return m_areMouseEventsEnabled;
		}

		using MouseListener = void(MouseEvent const&);

		EventListeners<MouseListener> mouseDownListeners;
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been pressed down while the pointer is above the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseDown(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseUpListeners;
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been released after having been pressed down when the mouse pointer was above the view.
			The mouse cursor may have left the view during the time the button is pressed, but it will still receive the event.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseUp(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseMoveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has been moved within the bounds of the view.
			This can be blocked by non-overlay views which have the same parent and are overlapping this one.
			The event is never blocked by children of this view.
			If it has entered the view, a mouse enter event is sent, and if it has left the view, a mouse leave event is sent.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseMove(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has entered the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
			that the mouse has entered gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseEnter(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse cursor has left the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
			that the mouse has left gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseLeave(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseBackgroundEnterListeners;
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when the mouse pointer has entered any part of the view that is not occupied by children of this view.
			By default, this changes the mouse cursor to the cursor that is set with setCursor on the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseBackgroundEnter(MouseEvent const& p_event) -> void;

		EventListeners<MouseListener> mouseBackgroundLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has left any part of the view that is not occupied by children of this view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseBackgroundLeave(MouseEvent const& p_event) -> void {}

		EventListeners<MouseListener> mouseScrollListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse wheel has been moved/scrolled while the mouse pointer is above the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual auto handleMouseScroll(MouseEvent const& p_event) -> void {}

		//------------------------------

		/*
			Listener signature:
				void (View* attachedChild)
			See View::handleChildViewAttachment for more info.
		*/
		EventListeners<void(View*)> childViewAttachmentListeners;
		/*
			USER IMPLEMENTED
			Gets called when a child view has been added to this view.
		*/
		virtual auto handleChildViewAttachment(View* p_attachedChild) -> void {}

		/*
			Listener signature:
				void (View* attachedChild)
			See View::handleChildViewDetachment for more info.
		*/
		EventListeners<void(View*)> childViewDetachmentListeners;
		/*
			USER IMPLEMENTED
			Gets called when a child view has been removed from this view.
		*/
		virtual auto handleChildViewDetachment(View* p_detachedChild) -> void {}
	};

	//------------------------------

	// TODO: continue switch to trailing return types

	/*
		The highest, "root" view in the view hierarchy.
		It is connected to a window which it holds and receives events from.
		When the window has been closed and destroyed, forget() is called on the GUI.
	*/
	class Gui : public View
	{
	private:
		friend View;
		friend Animation;

		static Count s_numberOfInstances;

		//------------------------------

		void handleThemeColorChange(Id p_id, Color p_newColor) override
		{
			if (p_id == ThemeColors::background)
			{
				getDrawingContext()->setBackgroundColor(p_newColor);
			}
		}

	public:
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_size is the size of the client area in DIPS (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string_view p_title, Size<Dip> p_size, 
		            WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr)
		{
			create(p_title, {0.5f}, p_size, p_windowFlags, p_parent);
		}
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_positionFactor is the position of the window, expressed as horizontal and vertical factors between 0 and 1, 
			where (0, 0) means the top left corner of the primary monitor and the top left edge of the window are aligned, 
			and (1, 1) means the bottom right corners are aligned.
			p_size is the size of the client area in DIPS (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string_view p_title, Point<Factor> p_positionFactor, Size<Dip> p_size, 
		            WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr);

	private:
		Gui* m_parent = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the GUI that owns the parent window of the window of this GUI.
			If the window does not have a parent, it returns 0.
		*/
		Gui* getParent() const
		{
			return m_parent;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given.
		*/
		View* getViewAt(Point<> p_coordinates);

		//------------------------------

	private:
		TimerThread<std::recursive_mutex> m_timerThread{m_sharedStateMutex};
	public:
		/*
			Adds a function that will be called in p_duration from now
			and returns an ID that identifies the timer callback.
		*/
		template<typename Callable, typename DurationTime, typename DurationPeriod>
		Id addTimerCallback(Callable const& p_callback, chrono::duration<DurationTime, DurationPeriod> const p_duration)
		{
			return m_timerThread.addCallback(p_callback, p_duration);
		}
		/*
			Adds a function that will be called in p_milliseconds milliseconds from now
			and returns an ID that identifies the timer callback.
		*/
		template<typename Callable>
		Id addTimerCallback(Callable const& p_callback, float const p_milliseconds)
		{
			return m_timerThread.addCallback(p_callback, p_milliseconds);
		}
		void cancelTimerCallback(Id const p_id)
		{
			m_timerThread.cancelCallback(p_id);
		}

		//------------------------------

	private:
		std::deque<View*> m_viewAnimationUpdateQueue;
		std::deque<Animation*> m_animationUpdateQueue;

		bool m_hasAnimationLoopStarted = false;
		std::thread m_animationThread;

		void thread_runAnimationLoop();
		
	public:
		/*
			LIBRARY IMPLEMENTED
			
			This method starts allowing events to be received, and starts drawing. 
			The content of the GUI should be created between a create and a run call.
			If this is the last GUI instance among the currently existing ones to run, the method waits for all GUIs to finish before returning.
			The GUI is forgotten when the window closes.
		*/
		void run()
		{
			/*
				The size of the GUI might have changed.
				In that case, corresponding size event(s) will be caused later by a window size change event.
			*/
			if (getSize() == m_window->getSize())
			{
				View::sendBoundsChangeEvents({});
			}
			invalidate();

			m_window->run();
			m_animationThread = std::thread{&Gui::thread_runAnimationLoop, this};

			static std::vector<Gui*> s_instancesToJoin;
			static std::mutex s_instancesToJoinMutex;
			static bool s_isWaitingForInstancesToFinish = false;

			if (s_isWaitingForInstancesToFinish)
			{
				m_animationThread.detach();
			}
			else
			{
				auto lock = std::scoped_lock{s_instancesToJoinMutex};
				s_instancesToJoin.push_back(this);
			}
			if (s_numberOfInstances == s_instancesToJoin.size() && !s_isWaitingForInstancesToFinish)
			{
				s_isWaitingForInstancesToFinish = true;

				s_instancesToJoinMutex.lock();
				auto instancesToJoin = std::move(s_instancesToJoin);
				s_instancesToJoin = {};
				s_instancesToJoinMutex.unlock();

				for (auto& instance : instancesToJoin)
				{
					instance->remember();
					instance->m_animationThread.join();
					instance->forget();
				}
				s_isWaitingForInstancesToFinish = false;
			}
		}

		//------------------------------

	private:
		std::recursive_mutex m_sharedStateMutex;
	public:
		/*
			LIBRARY IMPLEMENTED
			Gives the running thread exclusive access to modify any state that is shared by the event thread and animation thread.
		*/
		auto lockThreads() -> void
		{
			m_sharedStateMutex.lock();
		}
		/*
			LIBRARY IMPLEMENTED
			Gives back the other threads access to modify any state that is shared by the event thread and animation thread.
		*/
		auto unlockThreads() -> void
		{
			m_sharedStateMutex.unlock();
		}
		/*
			LIBRARY IMPLEMENTED

		*/
		auto createThreadLock()
		{
			return std::scoped_lock{m_sharedStateMutex};
		}

		//------------------------------

	private:
		void handleWindowCreate(WindowEvent const& p_event);
		void handleWindowDestroy(WindowEvent const& p_event);

		Size<> m_lastUpdatedWindowSize;
		void handleWindowSizeChange(WindowEvent const& p_event);

		auto sendBoundsChangeEvents(Rectangle<> const p_previousBounds) -> void override
		{
			if (auto const size = getSize();
			    m_window->dipsToPixels(size) != m_window->dipsToPixels(m_window->getSize()))
			{
				m_window->setSize(size);
			}
			else
			{
				View::sendBoundsChangeEvents(p_previousBounds);
			}
		}

		//------------------------------

	public:
		/*
			This is used to tell the OS what type of operation is supported for the dragged data.
			Queries the targeted views.
		*/
		auto getGlobalDragDropOperation(DragDropEvent& p_event) -> DragDropOperation
		{
			auto const targets = getTopMouseListenersAt(p_event.xy);

			auto const absolute = p_event.xy;

			auto result = DragDropOperation::None;
			for (View* const target : targets)
			{
				p_event.xy = absolute - target->getAbsoluteTopLeft();
				if (auto const operation = target->getDragDropOperation(p_event);
				    operation != DragDropOperation::None)
				{
					result = operation;
					break;
				}
			}
			for (View* const target : targets)
			{
				target->forget();
			}
			p_event.xy = absolute;

			return result;
		}

		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalDragDropEnter(DragDropEvent& p_event) -> void
		{
			handleGlobalDragDropMove(p_event);
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalDragDropMove(DragDropEvent& p_event) -> void;
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalDragDropLeave(DragDropEvent& p_event) -> void;
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalDragDropFinish(DragDropEvent& p_event) -> void
		{
			if (m_areDragDropEventsEnabled)
			{
				handleDragDropFinish(p_event);
			}

			auto const absolute = p_event.xy;

			View* container = this;
			Index startIndex = getNumberOfChildViews() - 1;

			bool hasFoundTopView = false;

			while (true)
			{
			loopStart:
				for (Index a = startIndex; a >= 0; a--)
				{
					if (auto const child = container->getChildView(a);
					    child->getIsVisible() && child->getIsContainingAbsolute(absolute))
					{
						auto const sendEventsIfTheyAreEnabled = [&]{
							if (child->getAreDragDropEventsEnabled())
							{
								p_event.xy = absolute - child->getAbsoluteTopLeft();
								child->dragDropFinishListeners(p_event);
							}
						};
						if (auto const childCount = child->getNumberOfChildViews())
						{
							sendEventsIfTheyAreEnabled();
							container = child;
							startIndex = childCount - 1;
							goto loopStart;
						}
						else
						{
							sendEventsIfTheyAreEnabled();
							if (!child->getIsOverlay())
							{
								// This is only used to determine if the outer loop should be exited afterwards.
								hasFoundTopView = true;
								break;
							}
						}
					}
				}

				if (!container->getIsOverlay() || hasFoundTopView || container == this)
				{
					break;
				}

				startIndex = container->getIndex() - 1;
				container = container->getParent<View>();
			}
		}

		EventListeners<void(DragDropOperation)> dragDropOperationChangeListeners;

		//------------------------------

	private:
		/*
			Returns the topmost non-overlay view which contains the coordinates given, as well as any overlay views which are above the non-overlay view.
		*/
		std::vector<View*> getTopMouseListenersAt(Point<> p_coordinates);

		std::vector<View*> m_pressedMouseEventListeners;
		Point<> m_mouseDownPosition;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalMouseDown(MouseEvent& p_event) -> void
		{
			auto const targets = getTopMouseListenersAt(p_event.xy);

			auto const absolute = p_event.xy;

			if (targets.size())
			{
				for (auto const view : targets)
				{
					p_event.xy = absolute - view->getAbsoluteTopLeft();

					view->mouseDownListeners(p_event);
					m_pressedMouseEventListeners.push_back(view);
				}
			}

			m_mouseDownPosition = absolute;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalMouseUp(MouseEvent& p_event) -> void
		{
			auto const absolute = p_event.xy;
			if (!m_pressedMouseEventListeners.empty())
			{
				for (auto const view : m_pressedMouseEventListeners)
				{
					p_event.xy = absolute - view->getAbsoluteTopLeft();
					view->mouseUpListeners(p_event);
				}
				for (auto const view : m_pressedMouseEventListeners)
				{
					view->forget();
				}
				m_pressedMouseEventListeners.clear();

				if (absolute != m_mouseDownPosition)
				{
					p_event.xy = absolute;
					p_event.movement = Vector2d{absolute - m_mouseDownPosition};
					handleGlobalMouseMove(p_event); // This is so that any views that the mouse has entered while pressed get their events.
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalMouseMove(MouseEvent& p_event) -> void;
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalMouseLeave(MouseEvent& p_event) -> void;
		/*
			LIBRARY IMPLEMENTED
		*/
		auto handleGlobalMouseScroll(MouseEvent& p_event) -> void
		{
			auto const targets = getTopMouseListenersAt(p_event.xy);

			auto const absolute = p_event.xy;

			if (!targets.empty())
			{
				for (auto const view : targets)
				{
					p_event.xy = absolute - view->getAbsoluteTopLeft();

					view->mouseScrollListeners(p_event);
				}
				for (auto const view : targets)
				{
					view->forget();
				}
			}

			p_event.xy = absolute;
			handleGlobalMouseMove(p_event);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			This is only called if the window of the GUI has the custom border style.
			It returns what part of the window border is under the given coordinates.

			The default implementation only handles the window resizing border areas, at the edges of the GUI.
			You can override this and either keep the default implementation and only add handling of window dragging areas
			(when the standard implementation returns WindowBorderArea::None), or detect all resize areas yourself too.
		*/
		virtual auto getWindowBorderAreaAtPosition(Point<> const p) const -> WindowBorderArea
		{
			constexpr float borderWidth = 5.f;
			constexpr float diagonalBorderWidth = 7.f;

			auto const size = getSize();

			if (p.y < diagonalBorderWidth)
			{
				if (p.x < diagonalBorderWidth)
				{
					return WindowBorderArea::TopLeftResize;
				}
				if (p.x >= size.width - diagonalBorderWidth)
				{
					return WindowBorderArea::TopRightResize;
				}
				if (p.y < borderWidth)
				{
					return WindowBorderArea::TopResize;
				}
			}
			if (p.y >= size.height - diagonalBorderWidth)
			{
				if (p.x < diagonalBorderWidth)
				{
					return WindowBorderArea::BottomLeftResize;
				}
				if (p.x >= size.width - diagonalBorderWidth)
				{
					return WindowBorderArea::BottomRightResize;
				}
				if (p.y >= size.height - borderWidth)
				{
					return WindowBorderArea::BottomResize;
				}
			}
			if (p.x < borderWidth)
			{
				return WindowBorderArea::LeftResize;
			}
			if (p.x >= size.width - borderWidth)
			{
				return WindowBorderArea::RightResize;
			}
			return WindowBorderArea::None;
		}

		//------------------------------

	private:
		View* m_keyboardFocus = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the keyboard event listener that keyboard events are sent to.
		*/
		auto setKeyboardFocus(View* const p_view) -> void
		{
			if (m_keyboardFocus == p_view)
			{
				return;
			}

			auto const focusBefore = m_keyboardFocus;

			m_keyboardFocus = p_view;

			if (focusBefore)
			{
				focusBefore->keyboardFocusLoseListeners();
			}

			if (p_view)
			{
				p_view->keyboardFocusGainListeners();
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the keyboard event listener that keyboard events are sent to.
		*/
		auto getKeyboardFocus() const noexcept -> View*
		{
			return m_keyboardFocus;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the keyboard event listener that keyboard events are sent to.
		*/
		template<typename T>
		auto getKeyboardFocus() const noexcept -> T*
		{
			return dynamic_cast<T*>(m_keyboardFocus);
		}

		auto sendGlobalCharacterInputEvents(KeyboardEvent const& p_event) -> void
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->characterInputListeners(p_event);
			}
			globalCharacterInputListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalCharacterInputListeners;

		auto sendGlobalKeyboardKeyDownEvents(KeyboardEvent const& p_event) -> void
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->keyboardKeyDownListeners(p_event);
			}
			globalKeyboardKeyDownListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalKeyboardKeyDownListeners;

		auto sendGlobalKeyboardKeyUpEvents(KeyboardEvent const& p_event) -> void
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->keyboardKeyUpListeners(p_event);
			}
			globalKeyboardKeyUpListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalKeyboardKeyUpListeners;

		//------------------------------

	private:
		std::unique_ptr<Window> m_window;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the window used by this GUI.
		*/
		auto getWindow() const noexcept -> Window*
		{
			return m_window.get();
		}

	private:
		std::unique_ptr<DrawingContext> m_drawingContext;
		DrawingState m_drawingContextState;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the drawing context used by this GUI.
		*/
		auto getDrawingContext() const noexcept -> DrawingContext*
		{
			return m_drawingContext.get();
		}

		//------------------------------

	private:
		std::mutex m_invalidRectanglesMutex;
		std::vector<Rectangle<>> m_invalidRectangles;
	public:
		/*
			LIBRARY IMPLEMENTED
			Invalidates a part of the GUI that has been changed, and therefore needs to be redrawn.
			Views that intersect with any invalid rectangles will be drawn in the next call to drawViews() (which is made internally) automatically.
		*/
		auto invalidateRectangle(Rectangle<> p_rectangle) -> void;
		/*
			LIBRARY IMPLEMENTED
			Draws the invalid rectangles of the GUI. This method should only be called internally by the library. Use draw() to draw things directly on the GUI.
		*/
		auto drawViews() -> void;

		//------------------------------

		Gui();
		Gui(Component* p_parent);
		~Gui() override;
	};

	//------------------------------

	namespace ThemeColors
	{
		inline Id const tooltipBackground;
		inline Id const tooltipOnBackground;
	}
	namespace ThemeValues
	{
		inline Id const tooltipFontSize;
		/*
			In milliseconds.
		*/
		inline Id const tooltipDelay;
	}

	/*
		Shows a short info message about a view.
		The parent of a tooltip is the GUI.
	*/
	class Tooltip : public View
	{
	private:
		Animation m_showAnimation
		{
			getGui(), getThemeEasing(ThemeEasings::out), 100ms, 
			[=](float p_value) {
				m_opacity = p_value;
				invalidate();
			} 
		};
		bool m_isShowing = false;

		Id m_timerId = 0;

	public:
		/*
			Makes the tooltip appear
			p_string is the string to be displayed on the tooltip.
			p_targetBounds is the area that the tooltip points to and is relative to the parent of this tooltip. The tooltip decides the exact positioning.
		*/
		virtual void show(std::string_view p_string, Rectangle<> p_targetRectangle)
		{
			if (!m_isShowing)
			{
				if (!m_text || p_string != m_text.getString())
				{
					auto fontSize = getThemeValue(ThemeValues::tooltipFontSize);
					m_text = getGui()->getDrawingContext()->createText(p_string, fontSize);
					m_text.fitSizeToText();
					setSize(m_text.getSize() + Size{2.2f*fontSize, 1.8f*fontSize});
					m_text.setCenter(Point{getSize()/2});
				}

				if (p_targetRectangle.bottom + 7.f + getHeight() >= getGui()->getHeight())
				{
					setBottom(max(1.f, p_targetRectangle.top - 7.f), true);
				}
				else
				{
					setTop(p_targetRectangle.bottom + 7.f, true);
				}
				setCenterX(max(1.f + getWidth()/2, min(getGui()->getWidth() - getWidth()/2 - 1.f, p_targetRectangle.getCenterX())));

				m_isShowing = true;
				m_timerId = getGui()->addTimerCallback([=] {
					m_opacity = 0.f;
					m_showAnimation.play(false);
				}, getThemeValue(ThemeValues::tooltipDelay));
			}
		}

		/*
			Makes the tooltip disappear.
		*/
		virtual void hide()
		{
			if (m_isShowing)
			{
				getGui()->cancelTimerCallback(m_timerId);
				m_isShowing = false;
				if (m_opacity)
				{
					m_showAnimation.play(true);
				}
			}
		}

		//------------------------------

	private:
		Text m_text;
		float m_opacity = 0.f;
	public:
		void draw(DrawingContext* p_drawingContext) override
		{
			if (m_text)
			{
				p_drawingContext->scale(m_opacity*0.3f + 0.7f, getAbsoluteCenter());
				p_drawingContext->setColor({getThemeColor(ThemeColors::tooltipBackground), m_opacity});
				p_drawingContext->fillRectangle(getSize());
				p_drawingContext->setColor(Color(getThemeColor(ThemeColors::tooltipOnBackground), m_opacity));
				p_drawingContext->drawText(m_text);
				p_drawingContext->scale(1.f/(m_opacity*0.3f + 0.7f), getAbsoluteCenter());
			}
		}

		//------------------------------

		Tooltip(View* p_parent) :
			View{p_parent}
		{
			initializeThemeColor(ThemeColors::tooltipBackground, {0.2f, 0.8f});
			initializeThemeColor(ThemeColors::tooltipOnBackground, {1.f, 0.95f});
			initializeThemeValue(ThemeValues::tooltipFontSize, 12.f);
			initializeThemeValue(ThemeValues::tooltipDelay, 400.f);

			setHasShadow(false);
			setElevation(-1.f);
			setCornerRadius(2.f);
			setIsOverlay(true); // Don't want to block any events from reaching views below the tooltip, especially not when it has faded away.
		}
	};

	//------------------------------

	class OpenFileDialog
	{
	public:
		struct FileExtensionFilter
		{
			/*
				This is the name that will be shown for the file extension filter.
			*/
			std::string name;
			/*
				This is the file extension(s) that the user can open when this filter is selected.
				If you want more than 1 file extension for this file extension name, you can seperate the extensions with ";".
				Wildcards are used to specify what part of the file name is filtered.
				For example: "*.png;*.jpg"
			*/
			std::string extensions;
		};

	private:
		bool m_canSelectMultipleFiles = false;
	public:
		/*
			If this is true, the user can select more than 1 file to open.
		*/
		void setCanSelectMultipleFiles(bool p_canSelectMultipleFiles)
		{
			m_canSelectMultipleFiles = p_canSelectMultipleFiles;
		}
		bool getCanSelectMultipleFiles()
		{
			return m_canSelectMultipleFiles;
		}

	private:
		std::string m_title = "Open file...";
	public:
		/*
			Sets the title shown in the top border of the open file dialog.
		*/
		void setTitle(std::string_view p_title)
		{
			m_title = p_title;
		}
		/*
			Returns the title shown in the thop border of the open file dialog.
		*/
		std::string_view getTitle()
		{
			return m_title;
		}

	private:
		std::vector<FileExtensionFilter> m_fileExtensions;
	public:
		/*
			Sets the file extensions of the files that the user can open with the dialog.
			See the properties of FileExtensionFilter for details.

			You can initialize the vector like this:

			{
				{"Images", "*.jpg;*.png"}
				{"Sound files", "*.mp3;*.wav;*.ogg"}
			}
		*/
		void setFileExtensions(std::vector<FileExtensionFilter>& p_fileExtensions)
		{
			m_fileExtensions = std::move(p_fileExtensions);
		}
		/*
			These are the file extensions of the files that the user can open with the dialog.
			See the properties of FileExtensionFilter for details.

			You can initialize the vector like this:

			{
				{"Images", "*.jpg;*.png"}
				{"Sound files", "*.mp3;*.wav;*.ogg"}
			}
		*/
		void setFileExtensions(std::vector<FileExtensionFilter>&& p_fileExtensions)
		{
			m_fileExtensions = p_fileExtensions;
		}
		/*
			Returns the file extension filters that can be selected in the dialog.
		*/
		std::vector<FileExtensionFilter> const& getFileExtensions()
		{
			return m_fileExtensions;
		}

		/*
			Opens the dialog and returns when the user has selected the files or closed the window.
			Returns a vector which contains the file paths in UTF-8 format of the files the user has selected to open.
			It can be empty if the user closed the window without selecting any files.
		*/
		std::vector<std::string> open();

	private:
		Gui* m_gui;
	public:
		OpenFileDialog() :
			m_gui{nullptr}
		{}
		OpenFileDialog(Gui* p_gui) :
			m_gui{p_gui}
		{}
	};

	//------------------------------

	/*
		A view that displays text.
	*/
	class TextView : public View
	{
	private:
		Color m_color = getThemeColor(ThemeColors::onBackground);
	public:
		void setColor(Color p_color)
		{
			m_color = p_color;
		}
		Color getColor()
		{
			return m_color;
		}

	private:
		float m_fontSize;
	public:
		void setFontSize(float p_fontSize)
		{
			m_fontSize = p_fontSize;
			if (m_text)
			{
				m_text.setFontSize(p_fontSize);
			}
		}
		float getFontSize()
		{
			return m_fontSize;
		}

	private:
		Text m_text;
	public:
		void setString(std::string_view p_string)
		{
			if (p_string.empty())
			{
				return;
			}
			m_text = getDrawingContext()->createText(p_string, m_fontSize);
			m_text.setIsTopTrimmed(true);
			//m_text.setSize(getSize());
			setSize(m_text.getSize() + 1.f);
		}
		void setText(Text p_text)
		{
			m_text = p_text;
		}
		Text& getText()
		{
			return m_text;
		}

		void fitSizeToText()
		{
			if (m_text)
			{
				m_text.fitSizeToText();
				setSize(m_text.getSize());
			}
		}
		void fitWidthToText()
		{
			if (m_text)
			{
				m_text.fitWidthToText();
				setWidth(m_text.getWidth());
			}
		}
		void fitHeightToText()
		{
			if (m_text)
			{
				m_text.fitHeightToText();
				setHeight(m_text.getHeight());
			}
		}

		void handleSizeChange() override
		{
			if (m_text)
			{
				m_text.setSize(getSize());
			}
		}

		void draw(DrawingContext* p_context) override
		{
			if (m_text)
			{
				p_context->setColor(m_color);
				p_context->drawText(m_text);
			}
		}

		//------------------------------

		TextView(View* p_parent, float p_fontSize, std::string_view p_string = "") :
			View{p_parent},
			m_fontSize{p_fontSize}
		{
			setString(p_string);
		}
	};

	//------------------------------

	namespace ThemeEasings
	{
		inline Id const ripple;
	}
	namespace ThemeValues
	{
		/*
			In milliseconds
		*/
		inline Id const rippleDuration;
	}

	/*
		A view that shows a ripple effect when you click it and optionally shows a hover effect when the mouse hovers over it.
		It is a mouse event overlay which means views behind this view are targeted as if this view didn't exist.
	*/
	class Ripple : public View
	{
	private:
		bool m_isEnabled = true;
	public:
		/*
			Disables the ripple and hover effects.
		*/
		void disable()
		{
			m_isEnabled = false;
		}
		/*
			Enables the ripple and hover effects.
		*/
		void enable()
		{
			m_isEnabled = true;
		}
		/*
			Returns whether the ripple and hover effects are enabled.
		*/
		bool getIsEnabled()
		{
			return m_isEnabled;
		}

		//------------------------------

	private:
		Color m_color;
	public:
		/*
			Sets the color that is used by the ripple and hover effects.
		*/
		void setColor(Color p_color)
		{
			m_color = p_color;
		}
		/*
			Returns the color that is used by the ripple and hover effects.
		*/
		Color getColor()
		{
			return m_color;
		}

		//------------------------------

	private:
		bool m_hasHoverEffect = true;
	public:
		/*
			Sets whether the view will be lightly highlighted when the mouse hovers over it.
			This is true by default and is recommended since it indicates that the view can be pressed.
		*/
		void setHasHoverEffect(bool p_hasHoverEffect)
		{
			m_hasHoverEffect = p_hasHoverEffect;
		}
		/*
			Returns whether the view will be lightly highlighted when the mouse hovers over it.
		*/
		bool getHasHoverEffect()
		{
			return m_hasHoverEffect;
		}

	private:
		Point<> m_position;
		float m_maxSize = 0.f;
	public:
		void updateMaxSize()
		{
			m_maxSize = 2.f*Point<>::getDistance(m_position, Point{
				m_position.x < getWidth()*0.5f ? getWidth() : 0.f,
				m_position.y < getHeight()*0.5f ? getHeight() : 0.f
			});
		}

	private:
		// This might be a bad idea but i'm experimenting haha

		Initializer init_theme = [=] {
			initializeThemeEasing(ThemeEasings::ripple, {0.1, 0.8, 0.2, 0.95});
			initializeThemeValue(ThemeValues::rippleDuration, 300);
		};

		float m_size = 0.f;
		Animation m_rippleAnimation
		{
			getGui(), getThemeEasing(ThemeEasings::ripple), getThemeValue(ThemeValues::rippleDuration),
			[=](float p_value) {
				m_size = interpolate(Range{m_maxSize*0.4f, m_maxSize}, p_value);
				m_alphaFactor = 1.f;
				if (!m_isMouseDown && p_value == 1.f)
				{
					m_rippleFadeAnimation.replay();
				}
				invalidate();
			}
		};

		bool m_isMouseDown = false;
		Initializer init_press = [=] {
			auto mouseDownListener = [=](MouseEvent const& p_event) {
				if (m_isEnabled && p_event.mouseButton == MouseButton::Left)
				{
					m_rippleFadeAnimation.stop();

					m_isMouseDown = true;
					m_position = p_event.xy;
					m_alphaFactor = 1.f;
					updateMaxSize();

					m_rippleAnimation.replay();
				}
			};
			mouseDownListeners += mouseDownListener;
		};

		float m_alphaFactor = 0.f;
		Animation m_rippleFadeAnimation
		{
			getGui(), getThemeEasing(ThemeEasings::inOut), 400ms,
			[=](float p_value) {
				m_alphaFactor = 1.f - p_value;
				invalidate();
			}
		};

		Initializer init_release = [=] {
			mouseUpListeners += [=](MouseEvent const& p_event) {
				if (m_isMouseDown && p_event.mouseButton == MouseButton::Left)
				{
					m_isMouseDown = false;
					if (m_size == m_maxSize && m_alphaFactor == 1.f)
					{
						m_rippleAnimation.stop();
						m_rippleFadeAnimation.replay();
					}
				}
			};
		};

		float m_overlayAlphaFactor = 0.f;
		Animation m_hoverAnimation
		{
			getGui(), getThemeEasing(ThemeEasings::inOut), getThemeValue(ThemeValues::hoverAnimationDuration),
			[=](float p_value) {
				m_overlayAlphaFactor = p_value;
				invalidate();
			}
		};

		Initializer init_hover = [=] {
			mouseBackgroundEnterListeners += [=](auto const&) {
				m_hoverAnimation.play(false);
			};
			mouseBackgroundLeaveListeners += [=](auto const&) {
				m_hoverAnimation.play(true);
			};
		};

		//------------------------------

	public:
		void draw(DrawingContext* p_drawingContext) override
		{
			if (m_isEnabled)
			{
				p_drawingContext->setColor({m_color, m_color.alpha*m_overlayAlphaFactor*0.3f});
				p_drawingContext->fillRectangle(getSize());

				if (m_color.alpha*m_alphaFactor >= 0.f)
				{
					p_drawingContext->setColor({m_color, m_color.alpha*m_alphaFactor*0.8f});
					p_drawingContext->fillCircle(m_position, m_size*0.5f);
				}
			}
		}

		Ripple(View* p_parent, Color p_color = {1.f, 0.45f}) :
			View{p_parent, p_parent->getSize()},
			m_color{p_color}
		{
			setIsOverlay(true); // Mouse events should be sent through
			setHasShadow(false);
			setElevation(std::numeric_limits<float>::max());
			enableMouseEvents();

			p_parent->sizeChangeListeners += [=](auto) {
				setSize(getParent<View>()->getSize());
				updateMaxSize();
			};
		}
	};

	//------------------------------

	namespace ThemeValues
	{
		inline Id const buttonFontSize;
		inline Id const buttonCharacterSpacing;
	}

	// TOOD: Update/remake Button class

	class Button : public View
	{
	public:
		enum class Emphasis
		{
			Low, // Text
			Medium, // Outlined
			High // Raised
		};

	private:
		Text m_text;

		Tooltip* m_tooltipView = nullptr;
		std::string m_tooltipString;

		Image m_icon;

		float m_pressAnimationTime = 1.f;
		bool m_isPressed = false;
		bool m_isRaising = false;
		Emphasis m_emphasis;

		bool m_isEnabled = true;
		Color m_currentColor;
		float m_colorAnimationTime = 1.f;
		bool m_isAccent = false;

		bool m_isMouseHovering = false;

		Ripple* m_ripple = nullptr;

	protected:
		void handleThemeValueChange(Id p_id, float p_newValue) override
		{
			if (p_id == ThemeValues::buttonFontSize)
			{
				m_text.setFontSize(p_newValue);
				if (p_id == ThemeValues::buttonCharacterSpacing)
				{
					m_text.setCharacterSpacing(p_newValue);
				}
				updateSize();
			}
			else if (p_id == ThemeValues::buttonCharacterSpacing)
			{
				m_text.setCharacterSpacing(p_newValue);
				updateSize();
			}
		}
		void handleThemeColorChange(Id p_id, Color p_newColor) override
		{
			if (m_emphasis == Emphasis::High)
			{
				if (p_id == (m_isAccent ? ThemeColors::secondary : ThemeColors::primary) ||
					p_id == (m_isAccent ? ThemeColors::onSecondary : ThemeColors::onPrimary))
				{
					m_currentColor = p_newColor;
				}
			}
			else if (p_id == (m_isAccent ? ThemeColors::secondaryOnBackground : ThemeColors::primaryOnBackground))
			{
				m_currentColor = p_newColor;
				m_ripple->setColor(Color(p_newColor, 0.3f));
			}
		}

	public:
		explicit Button(View* p_parent, std::string_view p_text = "", Emphasis p_emphasis = Emphasis::High, bool p_isAccent = false) :
			View{p_parent},
			m_emphasis{p_emphasis}
		{
			initializeThemeValue(ThemeValues::buttonFontSize, 14.f);
			initializeThemeValue(ThemeValues::buttonCharacterSpacing, 1.f);

			setString(p_text);

			setCornerRadius(4.f);

			m_ripple = new Ripple{this};
			m_ripple->setCursor(Cursor::Hand);

			setIsAccent(p_isAccent);
			if (p_emphasis == Emphasis::High)
			{
				setElevation(2.f);
			}

			enableMouseEvents();
		}

		//------------------------------

		void updateSize()
		{
			if (m_text)
			{
				float sizeFactor = getThemeValue(ThemeValues::buttonFontSize)/14.f;
				if (m_icon)
				{
					m_icon.setSize(16.f*sizeFactor);
					m_icon.setCenter({sizeFactor*38.f*0.5f, getHeight()*0.5f});

					m_text.setLeft(38.f*sizeFactor);
					setSize({std::round(m_text.getWidth()) + sizeFactor*(16.f + 38.f), 36.f*sizeFactor});
				}
				else
				{
					if (m_text.getWidth() >= 32.f*sizeFactor)
					{
						setSize(Size{std::round(m_text.getWidth()) + 32.f, 36.f}*sizeFactor);
					}
					else
					{
						setSize(Size{64.f, 36.f}*sizeFactor);
					}
					m_text.setCenter(getCenter() - getTopLeft());
				}
			}
			else if (m_icon)
			{
				m_icon.setCenter(getCenter() - getTopLeft());
			}
		}

		//------------------------------

		/*
			Makes the user unable to use the button and makes it gray.
		*/
		void disable()
		{
			if (m_isEnabled)
			{
				m_isEnabled = false;
				m_colorAnimationTime = 1.f;
				queueAnimationUpdate();

				m_ripple->disable();

				//if (m_isMouseHovering)
				//{
				//	getGui()->getWindow()->setCursor(Cursor::Arrow);
				//}
			}
		}

		/*
			Makes the user able to use the button.
		*/
		void enable()
		{
			if (!m_isEnabled)
			{
				m_isEnabled = true;
				m_colorAnimationTime = 0.f;
				queueAnimationUpdate();

				m_ripple->enable();

				//if (m_isMouseHovering)
				//{
				//	getGui()->getWindow()->setCursor(getCursor());
				//}
			}
		}

		/*
			Returns whether the user can use the button.
		*/
		bool getIsEnabled()
		{
			return m_isEnabled;
		}

		//------------------------------

		/*
			Sets whether the button uses the secondary/accent color. If not, it uses the primary color. The button uses primary color by default.
		*/
		void setIsAccent(bool p_isAccent)
		{
			m_isAccent = p_isAccent;
			if (m_emphasis == Emphasis::High)
			{
				m_currentColor = m_isAccent ? getThemeColor(ThemeColors::secondary) : getThemeColor(ThemeColors::primary);
				m_ripple->setColor(Color(m_isAccent ? getThemeColor(ThemeColors::onSecondary) : getThemeColor(ThemeColors::onPrimary), 0.3f));
			}
			else
			{
				m_currentColor = m_isAccent ? getThemeColor(ThemeColors::secondaryOnBackground) : getThemeColor(ThemeColors::primaryOnBackground);
				m_ripple->setColor(Color(m_isAccent ? getThemeColor(ThemeColors::secondaryOnBackground) : getThemeColor(ThemeColors::primaryOnBackground), 0.3f));
			}
		}
		/*
			Returns whether the button uses the secondary/accent color. If not, it uses the primary color. The button uses primary color by default.
		*/
		bool getIsAccent()
		{
			return m_isAccent;
		}

		//------------------------------

		/*
			Sets the string that the button displays.
		*/
		void setString(std::string_view p_string)
		{
			if (p_string[0])
			{
				m_text = getGui()->getDrawingContext()->createText(p_string, getThemeValue(ThemeValues::buttonFontSize));
				m_text.setWordWrapping(WordWrapping::Never);
				m_text.setCharacterSpacing(getThemeValue(ThemeValues::buttonCharacterSpacing));
				m_text.setFontWeight(FontWeight::Medium);
				//m_text.setIsTopTrimmed(true);
				m_text.fitSizeToText();
			}
			else
			{
				m_text.destroy();
			}
			updateSize();
		}

		/*
			Returns the string that the button displays.
		*/
		std::string_view getString()
		{
			if (m_text)
			{
				return m_text.getString();
			}
			return "";
		}

		/*
			Returns the text object that is used to display the button label.
		*/
		Text getText()
		{
			return m_text;
		}

		//------------------------------

		/*
			Sets an image to be shown together with the text. Unless you have remembered the image yourself, your are transferring ownership of the image to the button.
			It is best to keep a text label with the icon, unless it is very clear to all users what the button does with the icon alone, or if you have set a tooltip.
			If p_icon is 0, the icon is removed.
		*/
		void setIcon(Image const& p_icon)
		{
			if (p_icon != m_icon)
			{
				if (p_icon)
				{
					m_icon = p_icon;
					m_icon.setBoundsSizing(ImageBoundsSizing::Contain);
				}
				else
				{
					m_icon.destroy();
				}
				updateSize();
				invalidate();
			}
		}

		/*
			Returns the image that is shown together with the button text.
		*/
		Image getIcon()
		{
			return m_icon;
		}

		//------------------------------

		/*
			Sets a string to be shown as a tooltip when the mouse hovers over the button.
			Should give the user additional information about the button's purpose.
			An empty string disables the tooltip.
		*/
		void setTooltip(Tooltip* p_tooltipView, std::string_view p_info)
		{
			m_tooltipView = p_tooltipView;
			m_tooltipString = p_info;
		}

		//------------------------------

		EventListeners<void(Button*)> buttonClickListeners;

		//------------------------------

		void handleMouseBackgroundEnter(MouseEvent const& p_event) override
		{
			if (m_tooltipView && !m_tooltipString.empty())
			{
				m_tooltipView->show(m_tooltipString, getAbsoluteBounds());
			}
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			m_isMouseHovering = true;
		}
		void handleMouseBackgroundLeave(MouseEvent const& p_event) override
		{
			if (m_tooltipView && !m_tooltipString.empty())
			{
				m_tooltipView->hide();
			}
			m_isMouseHovering = false;
		}
		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (p_event.mouseButton == MouseButton::Left && m_isEnabled && m_emphasis == Emphasis::High)
			{
				m_isPressed = true;
				m_isRaising = true;
				m_pressAnimationTime = 0.f;
				queueAnimationUpdate();
			}
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			if (p_event.mouseButton == MouseButton::Left)
			{
				if (m_emphasis == Emphasis::High)
				{
					m_isPressed = false;
					queueAnimationUpdate();
				}
				if (m_isEnabled && getIsContaining(Point{p_event.x + getLeft(), p_event.y + getTop()}))
				{
					buttonClickListeners(this);
				}
			}
		}

		//------------------------------

		void updateAnimations() override
		{
			if ((m_colorAnimationTime != 1.f && m_isEnabled) || (m_colorAnimationTime != 0.f && !m_isEnabled))
			{
				float colorAnimationValue = getThemeEasing(ThemeEasings::symmetricalInOut).easeValue(m_colorAnimationTime);
				if (m_emphasis == Emphasis::High)
				{
					m_currentColor = m_isAccent ? getThemeColor(ThemeColors::secondary) : getThemeColor(ThemeColors::primary);
				}
				else
				{
					m_currentColor = m_isAccent ? getThemeColor(ThemeColors::secondaryOnBackground) : getThemeColor(ThemeColors::primaryOnBackground);
				}
				m_currentColor.setSaturationHSL(colorAnimationValue);

				if (m_isEnabled)
				{
					if (m_colorAnimationTime < 1.f)
					{
						m_colorAnimationTime = min(1.f, m_colorAnimationTime + 0.1f);
						queueAnimationUpdate();
					}
				}
				else
				{
					if (m_colorAnimationTime > 0.f)
					{
						m_colorAnimationTime = max(0.f, m_colorAnimationTime - 0.1f);
						queueAnimationUpdate();
					}
				}
			}

			if (m_emphasis == Emphasis::High)
			{
				float pressAnimationValue = getThemeEasing(ThemeEasings::inOut).easeValue(m_pressAnimationTime);
				m_pressAnimationTime += 0.06f;

				if (m_isRaising || m_isPressed)
				{
					setElevation(2.f + pressAnimationValue*4.f);
					if (!m_isPressed && pressAnimationValue == 1.f)
					{
						m_pressAnimationTime = 0.f;
						m_isRaising = false;
						queueAnimationUpdate();
					}
				}
				else
				{
					setElevation(2.f + (1.f - pressAnimationValue)*4.f);
				}

				if (pressAnimationValue < 1.f)
				{
					queueAnimationUpdate();
				}
			}

			invalidate();
		}


		//------------------------------

		void drawOverlay(DrawingContext* p_drawingContext, Rectangle<> p_targetRectangle) override
		{
			if (m_emphasis == Emphasis::Medium)
			{
				p_drawingContext->setColor(Color(getThemeColor(ThemeColors::onBackground), 0.25f));
				p_drawingContext->strokeRoundedRectangle(Rectangle<>(0.5f, 0.5f, getWidth() - 0.5f, getHeight() - 0.5f), getCorners().topLeftSizeX, 1.f);
			}
		}

		void draw(DrawingContext* p_drawingContext, Rectangle<> p_targetRectangle) override
		{
			if (m_emphasis == Emphasis::High)
			{
				p_drawingContext->clear(m_currentColor);
				p_drawingContext->setColor(m_isAccent ? getThemeColor(ThemeColors::onSecondary) : getThemeColor(ThemeColors::onPrimary));
			}
			else
			{
				p_drawingContext->setColor(m_currentColor);
			}

			if (m_icon)
			{
				p_drawingContext->drawImage(m_icon);
			}

			if (m_text)
			{
				p_drawingContext->drawText(m_text);
			}
		}
	};

	//------------------------------

	namespace ThemeValues
	{
		inline Id const editableTextCaretBlinkRate;
	}

	// TODO: Update/remake EditableText

	/*
		A view that only consists of text that can be edited by the user.
	*/
	class EditableText : public View
	{
	private:
		Text m_text;
		float m_textDrawingOffsetX = 0.f;
		float m_fontSize;
		TextAlign m_textAlign = TextAlign::Left;

		Index m_caretCharacterIndex = 0;
		Index m_caretByteIndex = 0;
		Point<> m_caretPosition;
		bool m_isCaretVisible = false;
		Count m_caretFrameCount = 0;

		Index m_selectionEndCharacterIndex = 0;
		Index m_selectionEndByteIndex = 0;
		Point<> m_selectionEndPosition;
		bool m_isSelectingWithMouse = false;
		bool m_isSelectionVisible = false;

		//------------------------------

		void updateCaretTracking()
		{
			if (!m_text)
			{
				return;
			}

			if (m_caretPosition.x + m_textDrawingOffsetX > getWidth())
			{
				m_textDrawingOffsetX = getWidth() - m_caretPosition.x;
			}
			else if (m_caretPosition.x + m_textDrawingOffsetX < 0.f)
			{
				m_textDrawingOffsetX = -m_caretPosition.x;
			}

			if (m_textAlign == TextAlign::Left)
			{
				if (m_text.getMinimumWidth() > getWidth())
				{
					if (m_text.getMinimumWidth() + m_textDrawingOffsetX < getWidth())
					{
						m_textDrawingOffsetX = getWidth() - m_text.getMinimumWidth();
					}
				}
				else
				{
					m_textDrawingOffsetX = 0.f;
				}
			}
			else if (m_textAlign == TextAlign::Right)
			{
				if (m_text.getMinimumWidth() > getWidth())
				{
					if (getWidth() - m_text.getMinimumWidth() + m_textDrawingOffsetX > 0.f)
					{
						m_textDrawingOffsetX = m_text.getMinimumWidth() - getWidth();
					}
				}
				else
				{
					m_textDrawingOffsetX = 0;
				}
			}
		}
		void updateSelectionEndTracking()
		{
			if (m_selectionEndPosition.x + m_textDrawingOffsetX > getWidth())
			{
				m_textDrawingOffsetX = getWidth() - m_selectionEndPosition.x;
			}
			else if (m_selectionEndPosition.x + m_textDrawingOffsetX < 0.f)
			{
				m_textDrawingOffsetX = -m_selectionEndPosition.x;
			}
		}

	public:
		explicit EditableText(View* p_parent, float p_width = 0.f, float p_fontSize = 12.f) :
			View(p_parent, Rectangle<>(0.f, 0.f, p_width, p_fontSize*1.2f)),
			m_fontSize(p_fontSize)
		{
			initializeThemeValue(ThemeValues::editableTextCaretBlinkRate, 20);

			setCursor(Cursor::Ibeam);
			enableMouseEvents();
		}

		//------------------------------

		/*
			Listeners that get called when the text is about to be changed, either by the user or programmatically.
			
			Listener signature:
				bool (EditableText* target, std::string& newString, newCaretCharacterIndex)
			newString is the string that will be set if all listeners return true from this handler. Otherwise, the string is left unchanged.
			newString can be modified, and the contents of the string after all listeners have handled the event is what will be set as the new text.
			newCaretCharacterIndex works in a similar way, and it is the index of the cursor showing where new user input is inserted.
			This index can be equal to the size of the new string, and in that case the cursor ends up at the end of the text.
		*/
		EventListeners<bool(EditableText*, std::string&, Index&)> editableTextChangeListeners;

		/*
			Listeners that get called when the user has pressed the enter/return key while p_editableText has keyboard focus.
		*/
		EventListeners<void(EditableText*)> editableTextEnterListeners;

		//------------------------------

	private:
		auto handleDoubleClick(MouseEvent const& p_event) -> void
		{
			if (m_text)
			{	
				auto const string = m_text.getString();

				auto const clickCharacterIndex = m_text.getNearestCharacterIndex({p_event.x - m_textDrawingOffsetX, p_event.y}, true);
				auto const clickUnitIndex = getUnitIndexFromCharacterIndex(string, clickCharacterIndex);
				
				auto leftBound = string.rfind(' ', clickUnitIndex);
				if (leftBound == std::string_view::npos)
				{
					leftBound = 0;
				}
				m_caretCharacterIndex = getCharacterIndexFromUnitIndex(string, leftBound);
				m_caretByteIndex = leftBound;
				m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
				
				if (auto const rightBound = string.find(' ', clickUnitIndex);
				    rightBound == std::string_view::npos)
				{
					m_selectionEndCharacterIndex = getNumberOfCharactersInString(string);
					m_selectionEndByteIndex = string.size();
				}
				else
				{
					m_selectionEndCharacterIndex = getCharacterIndexFromUnitIndex(string, rightBound);
					m_selectionEndByteIndex = rightBound;
				}
				m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex);

				// m_caretCharacterIndex = 0;
				// m_caretByteIndex = 0;

				// auto characterIndex = Index{};
				// for (auto byteIndex : Indices{string, 1})
				// {
				// 	if (byteIndex == string.size() || getIsUnitStartOfCharacter(string[byteIndex]))
				// 	{
				// 		if (byteIndex == string.size() || string[byteIndex] == ' ')
				// 		{
				// 			if (characterIndex >= clickCharacterIndex)
				// 			{
				// 				m_selectionEndCharacterIndex = characterIndex;
				// 				m_selectionEndByteIndex = byteIndex;
				// 				m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex);
				// 				m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
				// 				updateCaretTracking();
				// 				break;
				// 			}
				// 			else
				// 			{
				// 				m_caretCharacterIndex = characterIndex + 1;
				// 				m_caretByteIndex = byteIndex + 1;
				// 			}
				// 		}
				// 		++characterIndex;
				// 	}
				// }
				if (m_caretCharacterIndex != m_selectionEndCharacterIndex)
				{
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}
	public:
		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (p_event.isDoubleClick)
			{
				handleDoubleClick(p_event);
			}
			else
			{
				if (m_text)
				{
					if (p_event.modifierKeys & ModifierKeyFlags::Shift)
					{
						std::tie(m_selectionEndCharacterIndex, m_selectionEndPosition) = 
							m_text.getNearestCharacterIndexAndPosition({p_event.x - m_textDrawingOffsetX, p_event.y}, true);
						
						m_selectionEndByteIndex = getUnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);

						if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
						{
							m_caretFrameCount = 1;
							m_isCaretVisible = true;
							m_isSelectionVisible = false;
						}
						else
						{
							updateSelectionEndTracking();
							m_isSelectionVisible = true;
						}
						m_isSelectingWithMouse = true;
					}
					else
					{
						std::tie(m_caretCharacterIndex, m_caretPosition) = 
							m_text.getNearestCharacterIndexAndPosition({p_event.x - m_textDrawingOffsetX, p_event.y}, true);

						m_caretByteIndex = getUnitIndexFromCharacterIndex(m_text.getString(), m_caretCharacterIndex);
						updateCaretTracking();

						m_isCaretVisible = true;
						m_caretFrameCount = 1;
						m_isSelectingWithMouse = true;
						m_isSelectionVisible = false;
					}
				}
				else
				{
					//setString("");
				}

				getGui()->setKeyboardFocus(this);

				invalidate();
				queueAnimationUpdate();
			}
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			if (m_isSelectingWithMouse)
			{
				std::tie(m_selectionEndCharacterIndex, m_selectionEndPosition) = 
					m_text.getNearestCharacterIndexAndPosition({p_event.x - m_textDrawingOffsetX, 0}, true);

				m_selectionEndByteIndex = getUnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);
				updateSelectionEndTracking();
				m_isSelectionVisible = m_selectionEndCharacterIndex != m_caretCharacterIndex;
				m_isCaretVisible = true;
				m_caretFrameCount = 1;
				invalidate();
			}
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			m_isSelectingWithMouse = false;
		}

		void handleKeyboardFocusGain() override
		{
			m_caretFrameCount = 1;
			m_isCaretVisible = true;

			queueAnimationUpdate();
			invalidate();
		}
		void handleKeyboardFocusLose() override
		{
			m_caretFrameCount = 1;
			m_isCaretVisible = false;
			m_isSelectionVisible = false;

			invalidate();
		}
		void handleCharacterInput(KeyboardEvent const& p_event) override
		{
			if (p_event.character > u8"\u001f" && (p_event.character < u8"\u007f" || p_event.character > u8"\u009f"))
			{
				auto string = std::string{m_text ? m_text.getString() : ""};
				if (m_isSelectionVisible)
				{
					if (m_caretCharacterIndex <= m_selectionEndCharacterIndex)
					{
						string.erase(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex);
					}
					else
					{
						string.erase(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex);
						m_caretCharacterIndex = m_selectionEndCharacterIndex;
						m_caretByteIndex = m_selectionEndByteIndex;
					}
					m_isSelectionVisible = false;
				}

				string.insert(m_caretByteIndex, p_event.character);

				setString(string, m_caretCharacterIndex + 1);

				updateCaretTracking();

				m_caretFrameCount = 1;
				m_isCaretVisible = true;

				invalidate();
			}
		}
		void handleKeyboardKeyDown(KeyboardEvent const& p_event) override
		{
			Window* window = getWindow();

			auto string = m_text ? std::string{m_text.getString()} : "";

			if (m_isSelectionVisible && 
				(p_event.key == KeyboardKey::Backspace || p_event.key == KeyboardKey::Delete) && 
				m_caretCharacterIndex != m_selectionEndCharacterIndex)
			{
				if (m_caretCharacterIndex <= m_selectionEndCharacterIndex)
				{
					string.erase(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex);
					m_isSelectionVisible = false;
					setString(string);
					updateCaretTracking();
				}
				else
				{
					string.erase(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex);
					m_isSelectionVisible = false;
					setString(string, m_selectionEndCharacterIndex);
					updateCaretTracking();
				}
				if (m_textAlign == TextAlign::Center && m_text)
				{
					m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex);
					updateCaretTracking();
				}

				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				invalidate();
				return;
			}
			switch (p_event.key)
			{
				case KeyboardKey::Backspace:
				{
					if (!m_text)
					{
						return;
					}
					if (!m_isSelectionVisible && m_caretCharacterIndex > 0)
					{
						if (window->getIsKeyDown(KeyboardKey::Control))
						{
							Index characterIndex = m_caretCharacterIndex - 1;
							for (Index byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfCharacter(string[byteIndex]))
								{
									if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
									{
										string.erase(byteIndex, m_caretByteIndex - byteIndex);
										setString(string, characterIndex);
										break;
									}
									characterIndex--;
								}
							}
						}
						else
						{
							for (Index byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								int8 numberOfBytesInCharacter = getNumberOfUnitsInCharacter(string[byteIndex]);
								if (numberOfBytesInCharacter)
								{
									setString(string.erase(byteIndex, numberOfBytesInCharacter), m_caretCharacterIndex - 1);
									break;
								}
							}
						}
					}
					m_caretFrameCount = 1;
					m_isCaretVisible = true;
					m_isSelectionVisible = false;
					break;
				}
				case KeyboardKey::Delete:
				{
					if (!m_text)
					{
						return;
					}
					if (!m_isSelectionVisible && m_caretByteIndex < string.size())
					{
						if (window->getIsKeyDown(KeyboardKey::Control))
						{
							for (auto byteIndex : Indices{m_caretByteIndex, string})
							{
								if (byteIndex == string.size() - 1 || (string[byteIndex + 1] == ' ' && string[byteIndex] != ' '))
								{
									string.erase(m_caretByteIndex, byteIndex - m_caretByteIndex + 1);
									setString(string);
									break;
								}
							}
						}
						else
						{
							setString(string.erase(m_caretByteIndex, getNumberOfUnitsInCharacter(string[m_caretByteIndex])));
						}
					}
					m_caretFrameCount = 1;
					m_isCaretVisible = true;
					m_isSelectionVisible = false;
					break;
				}
				case KeyboardKey::Left:
				{
					if (!m_text)
					{
						return;
					}
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						if (window->getIsKeyDown(KeyboardKey::Shift))
						{
							if (!m_isSelectionVisible)
							{
								m_selectionEndCharacterIndex = m_caretCharacterIndex;
								m_selectionEndByteIndex = m_caretByteIndex;
							}
							Index characterIndex = m_selectionEndCharacterIndex - 1;
							for (Index byteIndex = m_selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfCharacter(string[byteIndex]))
								{
									if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
									{
										m_selectionEndByteIndex = byteIndex;
										m_selectionEndCharacterIndex = characterIndex;
										if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
										{
											m_isSelectionVisible = false;
										}
										else
										{
											m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
											updateSelectionEndTracking();
											m_isSelectionVisible = true;
										}
										break;
									}
									characterIndex--;
								}
							}
						}
						else
						{
							Index characterIndex = m_caretCharacterIndex - 1;
							for (Index byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfCharacter(string[byteIndex]))
								{
									if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
									{
										m_caretByteIndex = byteIndex;
										m_caretCharacterIndex = characterIndex;
										m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
										updateCaretTracking();
										m_isSelectionVisible = false;
										break;
									}
									characterIndex--;
								}
							}
						}
					}
					else if (window->getIsKeyDown(KeyboardKey::Shift))
					{
						if (!m_isSelectionVisible)
						{
							m_selectionEndCharacterIndex = m_caretCharacterIndex;
							m_selectionEndByteIndex = m_caretByteIndex;
						}
						if (m_selectionEndCharacterIndex > 0)
						{
							for (Index byteIndex = m_selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfCharacter(string[byteIndex]))
								{
									m_selectionEndCharacterIndex--;
									m_selectionEndByteIndex = byteIndex;
									if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
									{
										m_isSelectionVisible = false;
									}
									else
									{
										m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
										updateSelectionEndTracking();
										m_isSelectionVisible = true;
									}
									break;
								}
							}
						}
					}
					else
					{
						if (m_isSelectionVisible)
						{
							if (m_caretCharacterIndex > m_selectionEndCharacterIndex)
							{
								m_caretCharacterIndex = m_selectionEndCharacterIndex;
								m_caretByteIndex = m_selectionEndByteIndex;
								m_caretPosition = m_selectionEndPosition;
							}
							updateCaretTracking();
							m_isSelectionVisible = false;
						}
						else
						{
							if (m_caretCharacterIndex > 0)
							{
								for (Index byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
								{
									if (getIsUnitStartOfCharacter(string[byteIndex]))
									{
										m_caretCharacterIndex--;
										m_caretByteIndex = byteIndex;
										m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
										updateCaretTracking();
										break;
									}
								}
							}
						}
					}
					m_caretFrameCount = 1;
					m_isCaretVisible = true;
					invalidate();
					break;
				}
				case KeyboardKey::Right:
				{
					if (!m_text)
					{
						return;
					}
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						if (window->getIsKeyDown(KeyboardKey::Shift))
						{
							if (!m_isSelectionVisible)
							{
								m_selectionEndCharacterIndex = m_caretCharacterIndex;
								m_selectionEndByteIndex = m_caretByteIndex;
							}
							Index characterIndex = m_selectionEndCharacterIndex;
							for (Index byteIndex = m_selectionEndByteIndex + 1; byteIndex <= string.size(); byteIndex++)
							{
								if (byteIndex == string.size() || getIsUnitStartOfCharacter(string[byteIndex]))
								{
									characterIndex++;
									if (byteIndex == string.size() || string[byteIndex] == ' ' && string[byteIndex - 1] != ' ')
									{
										m_selectionEndByteIndex = byteIndex;
										m_selectionEndCharacterIndex = characterIndex;
										if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
										{
											m_isSelectionVisible = false;
										}
										else
										{
											m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
											updateSelectionEndTracking();
											m_isSelectionVisible = true;
										}
										break;
									}
								}
							}
						}
						else
						{
							Index characterIndex = m_caretCharacterIndex;
							for (Index byteIndex = m_caretByteIndex + 1; byteIndex <= string.size(); byteIndex++)
							{
								if (byteIndex == string.size() || getIsUnitStartOfCharacter(string[byteIndex]))
								{
									characterIndex++;
									if (byteIndex == string.size() || string[byteIndex] == ' ' && string[byteIndex - 1] != ' ')
									{
										m_caretByteIndex = byteIndex;
										m_caretCharacterIndex = characterIndex;
										m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
										updateCaretTracking();
										m_isSelectionVisible = false;
										break;
									}
								}
							}
						}
					}
					else if (window->getIsKeyDown(KeyboardKey::Shift))
					{
						if (!m_isSelectionVisible)
						{
							m_selectionEndCharacterIndex = m_caretCharacterIndex;
						}
						if (m_selectionEndByteIndex < string.size())
						{
							m_selectionEndByteIndex += getNumberOfUnitsInCharacter(string[m_selectionEndByteIndex]);
							m_selectionEndCharacterIndex++;
							if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
							{
								m_isSelectionVisible = false;
							}
							else
							{
								m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
								updateSelectionEndTracking();
								m_isSelectionVisible = true;
							}
						}
					}
					else
					{
						if (m_isSelectionVisible)
						{
							if (m_caretCharacterIndex < m_selectionEndCharacterIndex)
							{
								m_caretCharacterIndex = m_selectionEndCharacterIndex;
								m_caretByteIndex = m_selectionEndByteIndex;
								m_caretPosition = m_selectionEndPosition;
								updateCaretTracking();
							}
							m_isSelectionVisible = false;
						}
						else
						{
							if (m_caretByteIndex < string.size())
							{
								m_caretByteIndex += getNumberOfUnitsInCharacter(string[m_caretByteIndex]);
								m_caretCharacterIndex++;
								m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
								updateCaretTracking();
							}
						}
					}
					m_caretFrameCount = 1;
					m_isCaretVisible = true;
					invalidate();
					break;
				}
				case KeyboardKey::C:
				{
					if (!m_text)
					{
						return;
					}
					if (window->getIsKeyDown(KeyboardKey::Control) && m_isSelectionVisible)
					{
						if (m_caretCharacterIndex < m_selectionEndCharacterIndex)
						{
							window->setClipboardString(string.substr(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex));
						}
						else
						{
							window->setClipboardString(string.substr(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex));
						}
					}
					break;
				}
				case KeyboardKey::X:
				{
					if (!m_text)
					{
						return;
					}
					if (window->getIsKeyDown(KeyboardKey::Control) && m_isSelectionVisible)
					{
						if (m_caretCharacterIndex < m_selectionEndCharacterIndex)
						{
							window->setClipboardString(string.substr(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex));
							string.erase(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex);
							setString(string);
						}
						else {
							window->setClipboardString(string.substr(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex));
							string.erase(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex);
							setString(string, m_selectionEndCharacterIndex);
						}

						m_isSelectionVisible = false;

						m_caretFrameCount = 1;
						m_isCaretVisible = true;
					}
					break;
				}
				case KeyboardKey::V:
				{
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						Index caretCharacterIndex = m_caretCharacterIndex;
						Index caretByteIndex = m_caretByteIndex;
						if (m_isSelectionVisible)
						{
							if (caretCharacterIndex < m_selectionEndCharacterIndex)
							{
								string.erase(m_caretByteIndex, m_selectionEndByteIndex - m_caretByteIndex);
								m_selectionEndCharacterIndex = m_caretCharacterIndex;
								m_selectionEndByteIndex = m_caretByteIndex;
							}
							else
							{
								string.erase(m_selectionEndByteIndex, m_caretByteIndex - m_selectionEndByteIndex);
								caretCharacterIndex = m_selectionEndCharacterIndex;
								caretByteIndex = m_selectionEndByteIndex;
							}
							m_isSelectionVisible = false;
						}
						auto clipboardData = window->getClipboardData();
						auto clipboardString = clipboardData->getString();
						string.insert(caretByteIndex, clipboardString);
						setString(string, caretCharacterIndex + getNumberOfCharactersInString<char>(clipboardString));

						m_caretFrameCount = 1;
						m_isCaretVisible = true;
					}
					break;
				}
				case KeyboardKey::A:
				{
					if (!m_text)
					{
						return;
					}
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						selectAll();
						return;
					}
					break;
				}
				case KeyboardKey::Enter:
				{
					editableTextEnterListeners(this);
					break;
				}
			}
		}

		//------------------------------

		/*
			p_startIndex is the index of the first character to be selected and p_endIndex is the index of the character after the last selected character.
		*/
		auto setSelection(Index p_startIndex, Index p_endIndex) -> void
		{
			if (m_text)
			{
				Index numberOfCharactersInString = getNumberOfCharactersInString(m_text.getString());
				p_startIndex = min(numberOfCharactersInString, p_startIndex);
				p_endIndex = min(numberOfCharactersInString, max(p_startIndex, p_endIndex));
				if (p_startIndex != p_endIndex)
				{
					if (p_startIndex != m_caretCharacterIndex)
					{
						m_caretCharacterIndex = p_startIndex;
						m_caretByteIndex = getUnitIndexFromCharacterIndex(m_text.getString(), m_caretCharacterIndex);
						m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
					}

					if (p_endIndex != m_selectionEndCharacterIndex)
					{
						m_selectionEndCharacterIndex = p_endIndex;
						m_selectionEndByteIndex = getUnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);
						m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
					}
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}
		/*
			Selects all of the text.
		*/
		auto selectAll() -> void
		{
			if (m_text)
			{
				if (auto const stringLength = m_text.getString().size())
				{
					if (m_caretCharacterIndex != 0)
					{
						m_caretCharacterIndex = 0;
						m_caretByteIndex = 0;
						m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
					}

					if (m_selectionEndCharacterIndex != stringLength)
					{
						m_selectionEndCharacterIndex = getNumberOfCharactersInString(m_text.getString());
						m_selectionEndByteIndex = stringLength;
						m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
					}
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}

		//------------------------------

		/*
			Changes the content of the editable text.
			p_newCaretCharacterIndex determines the caret index that will be set if no event listeners cancel the change.
			This is needed because the old caret index will be kept in case any event listener returns false.
			Note that it is a character index and not a byte index, the string is utf-8 encoded.
		*/
		auto setString(std::string_view p_string, Index p_newCaretCharacterIndex = -1) -> void
		{
			if (m_text && m_text.getString() == p_string)
			{
				return;
			}
			if (p_newCaretCharacterIndex == -1)
			{
				p_newCaretCharacterIndex = m_caretCharacterIndex;
			}

			auto newString = std::string{p_string};

			for (auto& listener : editableTextChangeListeners)
			{
				if (!listener(this, newString, p_newCaretCharacterIndex))
				{
					return;
				}
			}

			if (!newString.size())
			{
				m_text.destroy();
				m_caretCharacterIndex = 0;
				m_caretByteIndex = 0;
				m_caretPosition.y = 0;
				if (m_textAlign == TextAlign::Left)
				{
					m_caretPosition.x = 0;
				}
				else if (m_textAlign == TextAlign::Right)
				{
					m_caretPosition.x = getWidth();
				}
				else if (m_textAlign == TextAlign::Center)
				{
					m_caretPosition.x = getWidth()*0.5f;
				}
				m_textDrawingOffsetX = 0.f;
				m_isSelectionVisible = false;
				invalidate();
				return;
			}

			m_text = getDrawingContext()->createText(newString, m_fontSize);
			m_text.setFontWeight(FontWeight::Regular);
			m_text.setTextAlign(m_textAlign);
			m_text.setWidth(getWidth());
			m_text.setTop(2.f);
			m_text.setBottom(getHeight(), false);

			auto const characterCount = getNumberOfCharactersInString<char>(newString);
			if (p_newCaretCharacterIndex > characterCount)
			{
				m_caretByteIndex = newString.size();
				m_caretCharacterIndex = characterCount;
			}
			else if (p_newCaretCharacterIndex != m_caretCharacterIndex)
			{
				if (p_newCaretCharacterIndex < 0)
				{
					m_caretCharacterIndex = 0;
					m_caretByteIndex = 0;
				}
				else
				{
					m_caretCharacterIndex = p_newCaretCharacterIndex;
					m_caretByteIndex = getUnitIndexFromCharacterIndex<char>(newString, p_newCaretCharacterIndex);
				}
			}
			m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
			updateCaretTracking();

			if (m_isSelectionVisible)
			{
				if (m_selectionEndByteIndex > newString.size())
				{
					m_selectionEndByteIndex = newString.size();
					m_selectionEndCharacterIndex = characterCount;
					if (m_selectionEndCharacterIndex == m_caretCharacterIndex)
					{
						m_isSelectionVisible = false;
					}
					else
					{
						m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex, true);
					}
				}
			}
			invalidate();
		}
		/*
			Returns the content of the editable text.
		*/
		auto getString() const -> std::string_view
		{
			if (m_text)
			{
				return m_text.getString();
			}
			return "";
		}

		/*
			Sets the content of the editable text as a value.
		*/
		template<typename T>
		auto setValue(T p_value) -> void
		{
			setString(numberToString(p_value));
		}
		/*
			Sets the content of the editable text as a value, rounded at a certain digit.
		*/
		template<typename T>
		auto setValue(T p_value, Index p_roundingDigit, RoundingType p_roundingType = RoundingType::Nearest) -> void
		{
			setString(numberToString(p_value, p_roundingDigit, p_roundingType));
		}
		/*
			Returns the content of the editable text as an arithmetic value.
		*/
		template<typename T>
		auto getValue() -> std::optional<T>
		{
			return stringToNumber<T>(getString());
		}

		/*
			Returns the internal text graphics object.
		*/
		auto getText() const -> Text
		{
			return m_text;
		}

		//------------------------------

		/*
			Sets the way text is horizontally aligned within the bounds of the editable text.
		*/
		auto setTextAlign(TextAlign p_textAlign) -> void
		{
			m_textAlign = p_textAlign;
			if (m_text)
			{
				m_text.setTextAlign(m_textAlign);
				invalidate();
			}
		}
		/*
			Returns the way text is horizontally aligned within the bounds of the editable text.
		*/
		auto getTextAlign() const -> TextAlign
		{
			return m_textAlign;
		}

		//------------------------------

		auto setFontSize(float p_fontSize) -> void
		{
			m_fontSize = p_fontSize;
			if (m_text)
			{
				m_text.setFontSize(p_fontSize);
			}
			setHeight(p_fontSize*1.2f);
			invalidate();
		}
		auto getFontSize() const -> float
		{
			return m_fontSize;
		}

		//------------------------------

		auto handleSizeChange() -> void override
		{
			updateCaretTracking();
			if (m_text)
			{
				m_text.setWidth(getWidth());
			}
		}

		//------------------------------

		auto updateAnimations() -> void override
		{
			if (getGui()->getKeyboardFocus() == this)
			{
				if (m_caretFrameCount % (uint32)getThemeValue(ThemeValues::editableTextCaretBlinkRate) == 0 && !m_isSelectionVisible)
				{
					m_isCaretVisible = !m_isCaretVisible;
					invalidate();
				}
				m_caretFrameCount++;
				queueAnimationUpdate();
			}
		}

		auto draw(DrawingContext* p_context) -> void override
		{
			//p_context->setColor(Color(0.f));
			//p_context->strokeRectangle(getSize(), 1.f);
			p_context->moveOrigin({m_textDrawingOffsetX, 0.f});
			p_context->setColor(getThemeColor(ThemeColors::onBackground));
			if (m_text)
			{
				p_context->drawText(m_text);
				if (m_isSelectionVisible)
				{
					p_context->setColor(getThemeColor(ThemeColors::selection));
					p_context->fillRectangle({m_caretPosition.x, 0.f, m_selectionEndPosition.x, getHeight()});
				}
			}
			if (m_isCaretVisible && !m_isSelectionVisible)
			{
				p_context->drawLine({m_caretPosition.x, 0.f}, {m_caretPosition.x, getHeight()}, 1.f);
			}
			p_context->moveOrigin({-m_textDrawingOffsetX, 0.f});
		}
	};

	//------------------------------

	namespace ThemeValues
	{
		inline Id const textFieldFontSize;
		inline Id const textFieldHeight;
		inline Id const textFieldPaddingLeft;
		inline Id const textFieldPaddingRight;
		inline Id const textFieldFilledPaddingBottom;
	}

	// TODO: Update/remake TextField

	class TextField : public View
	{
	public:
		enum Type
		{
			Outlined,
			Filled
		};

	private:
		static constexpr float OUTLINED_PADDING_LABEL = 5.f;
		Type m_type;

		EditableText* m_editableText = new EditableText{this};
	public:
		auto getEditableText() -> EditableText*
		{
			return m_editableText;
		}

	protected:
		auto handleThemeValueChange(Id const p_id, float const p_newValue) -> void override
		{
			if (p_id == ThemeValues::textFieldFontSize)
			{
				if (m_labelText)
				{
					m_labelText.setFontSize(p_newValue);
					m_labelText.fitSizeToText();
				}
				if (m_prefixText)
				{
					m_prefixText.setFontSize(p_newValue);
					m_prefixText.fitSizeToText();
				}
				if (m_suffixText)
				{
					m_suffixText.setFontSize(p_newValue);
					m_suffixText.fitSizeToText();
				}
				m_editableText->setFontSize(p_newValue);
			}
			if (p_id == ThemeValues::textFieldFontSize || p_id == ThemeValues::textFieldHeight)
			{
				// Text positions will be updated in handleSizeChange()
				setHeight(getThemeValue(ThemeValues::textFieldFontSize)*1.2f*getThemeValue(ThemeValues::textFieldHeight) + OUTLINED_PADDING_LABEL*(m_type == Type::Outlined));
			}
			if (p_id == ThemeValues::textFieldPaddingLeft)
			{
				if (m_labelText)
				{
					m_labelText.setLeft(p_newValue);
				}
				if (m_prefixText)
				{
					m_prefixText.setLeft(p_newValue);
					m_editableText->setLeft(m_prefixText.getRight() + 1.f, false);
				}
				else
				{
					m_editableText->setLeft(p_newValue, false);
				}
			}
			else if (p_id == ThemeValues::textFieldPaddingRight)
			{
				if (m_suffixText)
				{
					m_suffixText.setRight(getWidth() - p_newValue);
					m_editableText->setRight(m_suffixText.getLeft() - 1.f, false);
				}
				else
				{
					m_editableText->setRight(getWidth() - p_newValue, false);
				}
			}
			else if (p_id == ThemeValues::textFieldFilledPaddingBottom)
			{
				if (m_prefixText)
				{
					m_prefixText.setBottom(getHeight() - p_newValue);
				}
				if (m_suffixText)
				{
					m_suffixText.setBottom(getHeight() - p_newValue);
				}
				m_editableText->setBottom(getHeight() - p_newValue);
			}
		}

	public:
		auto handleSizeChange() -> void override
		{
			if (m_suffixText)
			{
				m_suffixText.setRight(getWidth() - getThemeValue(ThemeValues::textFieldPaddingRight));
				m_editableText->setRight(m_suffixText.getLeft() - 1.f, false);
			}
			else
			{
				m_editableText->setRight(getWidth() - getThemeValue(ThemeValues::textFieldPaddingRight), false);
			}

			if (m_type == Type::Filled)
			{
				auto const bottom = getHeight() - getThemeValue(ThemeValues::textFieldFilledPaddingBottom);
				if (m_labelText)
				{
					m_labelText.setCenterY(getHeight()*0.5f);
				}
				if (m_prefixText)
				{
					m_prefixText.setBottom(bottom);
				}
				if (m_suffixText)
				{
					m_suffixText.setBottom(bottom);
				}
				m_editableText->setBottom(bottom);
			}
			else
			{
				auto const centerY = OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL)*0.5f;
				if (m_labelText)
				{
					m_labelText.setCenterY(centerY);
				}
				if (m_prefixText)
				{
					m_prefixText.setCenterY(centerY);
				}
				if (m_suffixText)
				{
					m_suffixText.setCenterY(centerY);
				}
				m_editableText->setCenterY(centerY);
			}
		}

	private:
		Text m_labelText;
		Color m_labelColor;

	public:
		auto setLabel(std::string_view const p_label) -> void
		{
			if (m_labelText)
			{
				if (p_label == m_labelText.getString())
				{
					return;
				}
			}
			if (p_label.empty())
			{
				m_labelText.destroy();
			}
			else
			{
				m_labelText = getGui()->getDrawingContext()->createText(p_label, getThemeValue(ThemeValues::textFieldFontSize));
				m_labelText.setFontWeight(FontWeight::Regular);
				m_labelText.fitSizeToText();
				if (m_type == Type::Filled)
				{
					m_labelText.setCenterY(getHeight()*0.5f);
				}
				else if (m_type == Type::Outlined)
				{
					m_labelText.setCenterY(OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL)*0.5f);
				}
				queueAnimationUpdate();
			}
		}
		auto getLabel() const -> std::string_view
		{
			if (m_labelText)
			{
				return m_labelText.getString();
			}
			return "";
		}

		//------------------------------

	private:
		Text m_prefixText;
		Text m_suffixText;

		auto setAffixString(std::string_view const p_string, Text& p_affixText) -> bool
		{
			if (p_affixText)
			{
				if (p_affixText.getString() == p_string)
				{
					return false;
				}
			}
			if (p_string.empty())
			{
				p_affixText.destroy();
				return false;
			}
			p_affixText = getDrawingContext()->createText(p_string, getThemeValue(ThemeValues::textFieldFontSize));
			p_affixText.setFontWeight(FontWeight::Regular);
			p_affixText.setHeight(p_affixText.getFontSize()*1.2f);
			if (m_type == Type::Filled)
			{
				p_affixText.setBottom(getThemeValue(ThemeValues::textFieldFilledPaddingBottom));
			}
			else
			{
				p_affixText.setTop(m_editableText->getTop() + 2.f);
			}
			return true;
		}
	public:
		auto setPrefixString(std::string_view const p_string) -> void
		{
			if (setAffixString(p_string, m_prefixText))
			{
				m_prefixText.setLeft(getThemeValue(ThemeValues::textFieldPaddingLeft));
				m_editableText->setLeft(m_prefixText.getRight() + 1.f, false);
				if (m_labelText)
				{
					m_labelText.setLeft(m_prefixText.getRight() + 1.f);
				}
			}
		}
		auto getPrefixString() const -> std::string_view
		{
			if (m_suffixText)
			{
				return m_suffixText.getString();
			}
			return "";
		}

		auto setSuffixString(std::string_view const p_string) -> void
		{
			if (setAffixString(p_string, m_suffixText))
			{
				m_suffixText.setRight(getWidth() - getThemeValue(ThemeValues::textFieldPaddingRight));
				m_editableText->setRight(m_suffixText.getLeft() - 1.f, false);
			}
		}
		auto getSuffixString() const -> std::string_view
		{
			if (m_suffixText)
			{
				return m_suffixText.getString();
			}
			return "";
		}

		//------------------------------

		auto setString(std::string_view const p_string) -> void
		{
			m_editableText->setString(p_string);
			if (m_type == Type::Filled)
			{
				m_editableText->setBottom(getHeight() - getThemeValue(ThemeValues::textFieldFilledPaddingBottom));
			}
			else if (m_type == Type::Outlined)
			{
				m_editableText->setCenterY(OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL)*0.5f);
			}
		}
		auto getString() const -> std::string_view
		{
			return m_editableText->getString();
		}
		
		/*
			Sets the content of the text field as a value.
		*/
		template<typename T>
		auto setValue(T const p_value) -> void
		{
			setString(numberToString(p_value));
		}
		/*
			Sets the content of the text field as a value, rounded at a certain digit.
		*/
		template<typename T>
		auto setValue(
			T const p_value, Index const p_roundingDigit, 
			RoundingType const p_roundingType = RoundingType::Nearest
		) -> void
		{
			setString(numberToString(p_value, p_roundingDigit, p_roundingType));
		}
		template<typename T>
		auto getValue() -> std::optional<T>
		{
			return m_editableText->getValue<T>();
		}

		//------------------------------

		auto setTextAlign(TextAlign p_textAlign) -> void
		{
			m_editableText->setTextAlign(p_textAlign);
		}
		auto getTextAlign() const -> TextAlign
		{
			return m_editableText->getTextAlign();
		}

		//------------------------------

		void handleMouseDown(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseDown(event);
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseUp(event);
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseMove(event);
		}
		void handleMouseEnter(MouseEvent const& p_event) override
		{
			View::handleMouseBackgroundEnter(p_event);
			m_isMouseHovering = true;
			queueAnimationUpdate();
		}
		void handleMouseLeave(MouseEvent const& p_event) override
		{
			m_isMouseHovering = false;
			queueAnimationUpdate();
		}

		void handleKeyboardFocusGain() override
		{
			getGui()->setKeyboardFocus(m_editableText);
		}

		//------------------------------

		/*
			Returns whether the EditableText child of this text field has keyboard focus.
		*/
		bool getHasKeyboardFocus()
		{
			return m_editableText == getGui()->getKeyboardFocus();
		}

		//------------------------------

	private:
		float m_focusAnimationTime = 0.f;
		Factor m_focusAnimationValue = 0.f;

		bool m_isMouseHovering = false;
		float m_hoverAnimationTime = 0.f;
		Factor m_hoverAnimationValue = 0.f;

	public:
		void updateAnimations() override
		{
			if (getGui()->getKeyboardFocus() == m_editableText)
			{
				if (m_focusAnimationValue < 1.f)
				{
					m_focusAnimationValue = getThemeEasing(ThemeEasings::inOut).easeValue(m_focusAnimationTime);
					m_focusAnimationTime = min(1.f, m_focusAnimationTime + 0.09f);
					invalidate();
					queueAnimationUpdate();
				}
			}
			else if (m_focusAnimationValue > 0.f)
			{
				m_focusAnimationValue = 1.f - getThemeEasing(ThemeEasings::inOut).easeValue(1.f - m_focusAnimationTime);
				m_focusAnimationTime = max(0.f, m_focusAnimationTime - 0.09f);
				invalidate();
				queueAnimationUpdate();
			}
			if (m_isMouseHovering)
			{
				if (m_hoverAnimationValue < 1.f)
				{
					m_hoverAnimationValue = getThemeEasing(ThemeEasings::symmetricalInOut).easeValue(m_hoverAnimationTime);
					m_hoverAnimationTime = min(1.f, m_hoverAnimationTime + getThemeValue(ThemeValues::hoverAnimationSpeed));
					invalidate();
					queueAnimationUpdate();
				}
			}
			else if (m_hoverAnimationValue > 0.f)
			{
				m_hoverAnimationValue = 1.f - getThemeEasing(ThemeEasings::symmetricalInOut).easeValue(1.f - m_hoverAnimationTime);
				m_hoverAnimationTime = max(0.f, m_hoverAnimationTime - getThemeValue(ThemeValues::hoverAnimationSpeed));
				invalidate();
				queueAnimationUpdate();
			}
			m_labelColor = interpolate(
				interpolate(
					getThemeColor(ThemeColors::background), 
					getThemeColor(ThemeColors::onBackground), 
					(1.f - m_focusAnimationValue)*m_hoverAnimationValue*0.3f + 0.4f
				), 
				getThemeColor(ThemeColors::primaryOnBackground), 
				m_focusAnimationValue
			);
		}

		void draw(DrawingContext* p_context) override
		{
			if (m_type == Type::Filled)
			{
				p_context->setColor({
					interpolate(
						getThemeColor(ThemeColors::background), getThemeColor(ThemeColors::onBackground), 
						0.05f + 0.05f*min(m_hoverAnimationValue*0.3f + m_focusAnimationValue, 1.f)
					), 1.f 
				});
				p_context->fillRectangle(getSize());
				p_context->setColor(Color{getThemeColor(ThemeColors::onBackground), 0.4f});
				p_context->drawLine({0.f, getHeight() - 1.f}, {getWidth(), getHeight() - 0.5f}, 1.f);
				if (m_focusAnimationValue > 0.01f)
				{
					p_context->setColor(getThemeColor(ThemeColors::primaryOnBackground));
					p_context->drawLine(
						{(1.f - m_focusAnimationValue)*getWidth()*0.5f, getHeight() - 1.f}, 
						{(1.f + m_focusAnimationValue)*getWidth()*0.5f, getHeight() - 1.f}, 
						2.f
					);
				}
				if (m_labelText)
				{
					Factor labelAnimationValue = m_editableText->getString().empty() ? m_focusAnimationValue : 1.f;
					Dip leftPadding = getThemeValue(ThemeValues::textFieldPaddingLeft);
					p_context->moveOrigin({
						leftPadding + 2.f*labelAnimationValue, 
						-0.17f*(getHeight() - m_labelText.getHeight() - leftPadding)*labelAnimationValue
					});
					p_context->setScale(1.f - labelAnimationValue*0.3f);
					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);
					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}
			else if (m_type == Type::Outlined)
			{
				p_context->setColor(m_labelColor);
				p_context->strokeRectangle({1.f, 1.f + OUTLINED_PADDING_LABEL, getWidth() - 1.f, getHeight() - 1.f}, getCorners(), m_focusAnimationValue + 1.f);

				if (m_labelText)
				{
					Factor labelAnimationValue = m_editableText->getString().empty() ? m_focusAnimationValue : 1.f;
					p_context->moveOrigin({
						getThemeValue(ThemeValues::textFieldPaddingLeft) + 2.f*labelAnimationValue, 
						-(getHeight() - OUTLINED_PADDING_LABEL)*0.3f*labelAnimationValue
					});
					p_context->setScale(1.f - labelAnimationValue*0.3f);

					p_context->setColor(getThemeColor(ThemeColors::background));
					p_context->fillRoundedRectangle(
						{
							m_labelText.getLeft() - 4.f, m_labelText.getTop(), 
							m_labelText.getRight() + 4.f, m_labelText.getBottom() 
						}, 2.f
					);

					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);

					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}

			if (m_prefixText)
			{
				p_context->setColor({getThemeColor(ThemeColors::onBackground), 0.5f});
				p_context->drawText(m_prefixText);
			}
			if (m_suffixText)
			{
				p_context->setColor({getThemeColor(ThemeColors::onBackground), 0.5f});
				p_context->drawText(m_suffixText);
			}
		}

		//------------------------------

		TextField(
			View* const p_parent, 
			Type const p_type = Type::Filled, 
			std::string_view const p_label = "", 
			Dip const p_width = 120.f
		) :
			View{p_parent},
			m_type{p_type}
		{
			initializeThemeValue(ThemeValues::textFieldFontSize, 15.f);
			initializeThemeValue(ThemeValues::textFieldHeight, 3.f);
			initializeThemeValue(ThemeValues::textFieldPaddingLeft, 14.f);
			initializeThemeValue(ThemeValues::textFieldPaddingRight, 14.f);
			initializeThemeValue(ThemeValues::textFieldFilledPaddingBottom, 9.f);

			setLabel(p_label);
			setCursor(Cursor::Ibeam);
			enableMouseEvents();

			m_editableText->setFontSize(getThemeValue(ThemeValues::textFieldFontSize));
			m_editableText->setLeft(getThemeValue(ThemeValues::textFieldPaddingLeft));
			m_editableText->setRight(p_width - getThemeValue(ThemeValues::textFieldPaddingRight), false);

			auto const handleEditableTextFocusChange = [this]() {
				queueAnimationUpdate();
			};
			m_editableText->keyboardFocusGainListeners += handleEditableTextFocusChange;
			m_editableText->keyboardFocusLoseListeners += handleEditableTextFocusChange;

			setSize({
				p_width, 
				getThemeValue(ThemeValues::textFieldFontSize)*1.2f*getThemeValue(ThemeValues::textFieldHeight) + 
				OUTLINED_PADDING_LABEL*(m_type == Type::Outlined)
			});

			if (p_type == Type::Filled)
			{
				setCorners({5.f, 5.f, 0.f, 0.f});
			}
			else
			{
				setCornerRadius(5.f);
			}

			setString("");

			queueAnimationUpdate();
		}
	};
}

/*
	Material design 2014 colors
*/
namespace MaterialColors
{
	inline Avo::colorInt const
		RED_50 = 0xFFFFEBEE,
		RED_100 = 0xFFFFCDD2,
		RED_200 = 0xFFEF9A9A,
		RED_300 = 0xFFE57373,
		RED_400 = 0xFFEF5350,
		RED_500 = 0xFFF44336,
		RED_600 = 0xFFE53935,
		RED_700 = 0xFFD32F2F,
		RED_800 = 0xFFC62828,
		RED_900 = 0xFFB71C1C,
		RED_A100 = 0xFFFF8A80,
		RED_A200 = 0xFFFF5252,
		RED_A400 = 0xFFFF1744,
		RED_A700 = 0xFFD50000,

		PINK_50 = 0xFFFCE4EC,
		PINK_100 = 0xFFF8BBD0,
		PINK_200 = 0xFFF48FB1,
		PINK_300 = 0xFFF06292,
		PINK_400 = 0xFFEC407A,
		PINK_500 = 0xFFE91E63,
		PINK_600 = 0xFFD81B60,
		PINK_700 = 0xFFC2185B,
		PINK_800 = 0xFFAD1457,
		PINK_900 = 0xFF880E4F,
		PINK_A100 = 0xFFFF80AB,
		PINK_A200 = 0xFFFF4081,
		PINK_A400 = 0xFFF50057,
		PINK_A700 = 0xFFC51162,

		PURPLE_50 = 0xFFF3E5F5,
		PURPLE_100 = 0xFFE1BEE7,
		PURPLE_200 = 0xFFCE93D8,
		PURPLE_300 = 0xFFBA68C8,
		PURPLE_400 = 0xFFAB47BC,
		PURPLE_500 = 0xFF9C27B0,
		PURPLE_600 = 0xFF8E24AA,
		PURPLE_700 = 0xFF7B1FA2,
		PURPLE_800 = 0xFF6A1B9A,
		PURPLE_900 = 0xFF4A148C,
		PURPLE_A100 = 0xFFEA80FC,
		PURPLE_A200 = 0xFFE040FB,
		PURPLE_A400 = 0xFFD500F9,
		PURPLE_A700 = 0xFFAA00FF,

		DEEP_PURPLE_50 = 0xFFEDE7F6,
		DEEP_PURPLE_100 = 0xFFD1C4E9,
		DEEP_PURPLE_200 = 0xFFB39DDB,
		DEEP_PURPLE_300 = 0xFF9579CD,
		DEEP_PURPLE_400 = 0xFF7E57C2,
		DEEP_PURPLE_500 = 0xFF673AB7,
		DEEP_PURPLE_600 = 0xFF5E35B1,
		DEEP_PURPLE_700 = 0xFF512DA8,
		DEEP_PURPLE_800 = 0xFF4527A0,
		DEEP_PURPLE_900 = 0xFF311B92,
		DEEP_PURPLE_A100 = 0xFFB388FF,
		DEEP_PURPLE_A200 = 0xFF7C4DFF,
		DEEP_PURPLE_A400 = 0xFF651FFF,
		DEEP_PURPLE_A700 = 0xFF6200EA,

		INDIGO_50 = 0xFFE8EAF6,
		INDIGO_100 = 0xFFC5CAE9,
		INDIGO_200 = 0xFF9FA8DA,
		INDIGO_300 = 0xFF7986CB,
		INDIGO_400 = 0xFF5C6BC0,
		INDIGO_500 = 0xFF3F51B5,
		INDIGO_600 = 0xFF3949AB,
		INDIGO_700 = 0xFF303F9F,
		INDIGO_800 = 0xFF283593,
		INDIGO_900 = 0xFF1A237E,
		INDIGO_A100 = 0xFF8C9EFF,
		INDIGO_A200 = 0xFF536DFE,
		INDIGO_A400 = 0xFF3D5AFE,
		INDIGO_A700 = 0xFF304FFE,

		BLUE_50 = 0xFFE3F2FD,
		BLUE_100 = 0xFFBBDEFB,
		BLUE_200 = 0xFF90CAF9,
		BLUE_300 = 0xFF64B5F6,
		BLUE_400 = 0xFF42A5F5,
		BLUE_500 = 0xFF2196F3,
		BLUE_600 = 0xFF1E88E5,
		BLUE_700 = 0xFF1976D2,
		BLUE_800 = 0xFF1565C0,
		BLUE_900 = 0xFF0D47A1,
		BLUE_A100 = 0xFF82B1FF,
		BLUE_A200 = 0xFF448AFF,
		BLUE_A400 = 0xFF2979FF,
		BLUE_A700 = 0xFF2962FF,

		LIGHT_BLUE_50 = 0xFFE1F5FE,
		LIGHT_BLUE_100 = 0xFFB3E5FC,
		LIGHT_BLUE_200 = 0xFF81D4FA,
		LIGHT_BLUE_300 = 0xFF4FC3F7,
		LIGHT_BLUE_400 = 0xFF29B6F6,
		LIGHT_BLUE_500 = 0xFF03A9F4,
		LIGHT_BLUE_600 = 0xFF039BE5,
		LIGHT_BLUE_700 = 0xFF0288D1,
		LIGHT_BLUE_800 = 0xFF0277BD,
		LIGHT_BLUE_900 = 0xFF01579B,
		LIGHT_BLUE_A100 = 0xFF80D8FF,
		LIGHT_BLUE_A200 = 0xFF40C4FF,
		LIGHT_BLUE_A400 = 0xFF00B0FF,
		LIGHT_BLUE_A700 = 0xFF0091EA,

		CYAN_50 = 0xFFE0F7FA,
		CYAN_100 = 0xFFB2EBF2,
		CYAN_200 = 0xFF80DEEA,
		CYAN_300 = 0xFF4DD0E1,
		CYAN_400 = 0xFF26C6DA,
		CYAN_500 = 0xFF00BCD4,
		CYAN_600 = 0xFF00ACC1,
		CYAN_700 = 0xFF0097A7,
		CYAN_800 = 0xFF00838F,
		CYAN_900 = 0xFF006064,
		CYAN_A100 = 0xFF84FFFF,
		CYAN_A200 = 0xFF18FFFF,
		CYAN_A400 = 0xFF00E5FF,
		CYAN_A700 = 0xFF00B8D4,

		TEAL_50 = 0xFFE0F2F1,
		TEAL_100 = 0xFFB2DFDB,
		TEAL_200 = 0xFF80CBC4,
		TEAL_300 = 0xFF4DB6AC,
		TEAL_400 = 0xFF26A69A,
		TEAL_500 = 0xFF009688,
		TEAL_600 = 0xFF00897B,
		TEAL_700 = 0xFF00796B,
		TEAL_800 = 0xFF00695C,
		TEAL_900 = 0xFF004D40,
		TEAL_A100 = 0xFFA7FFEB,
		TEAL_A200 = 0xFF64FFDA,
		TEAL_A400 = 0xFF1DE9B6,
		TEAL_A700 = 0xFF00BFA5,

		GREEN_50 = 0xFFE8F5E9,
		GREEN_100 = 0xFFC8E6C9,
		GREEN_200 = 0xFFA5D6A7,
		GREEN_300 = 0xFF81C784,
		GREEN_400 = 0xFF66BB6A,
		GREEN_500 = 0xFF4CAF50,
		GREEN_600 = 0xFF43A047,
		GREEN_700 = 0xFFE88E3C,
		GREEN_800 = 0xFF2E7D32,
		GREEN_900 = 0xFF1B5E20,
		GREEN_A100 = 0xFFB9F6CA,
		GREEN_A200 = 0xFF69F0AE,
		GREEN_A400 = 0xFF00E676,
		GREEN_A700 = 0xFF00C853,

		LIGHT_GREEN_50 = 0xFFF1F8E9,
		LIGHT_GREEN_100 = 0xFFDCEDC8,
		LIGHT_GREEN_200 = 0xFFC5E1A5,
		LIGHT_GREEN_300 = 0xFFAED581,
		LIGHT_GREEN_400 = 0xFF9CCC65,
		LIGHT_GREEN_500 = 0xFF8BC34A,
		LIGHT_GREEN_600 = 0xFF7CB342,
		LIGHT_GREEN_700 = 0xFF689F38,
		LIGHT_GREEN_800 = 0xFF558B2F,
		LIGHT_GREEN_900 = 0xFF33691E,
		LIGHT_GREEN_A100 = 0xFFCCFF90,
		LIGHT_GREEN_A200 = 0xFFB2FF59,
		LIGHT_GREEN_A400 = 0xFF76FF03,
		LIGHT_GREEN_A700 = 0xFF64DD17,

		LIME_50 = 0xFFF9FBE7,
		LIME_100 = 0xFFF0F4C3,
		LIME_200 = 0xFFE6EE9C,
		LIME_300 = 0xFFDCE775,
		LIME_400 = 0xFFD4E157,
		LIME_500 = 0xFFCDDC39,
		LIME_600 = 0xFFC0CA33,
		LIME_700 = 0xFFAFB42B,
		LIME_800 = 0xFF9E9D24,
		LIME_900 = 0xFF827717,
		LIME_A100 = 0xFFF4FF81,
		LIME_A200 = 0xFFEEFF41,
		LIME_A400 = 0xFFC6FF00,
		LIME_A700 = 0xFFAEEA00,

		YELLOW_50 = 0xFFFFFDE7,
		YELLOW_100 = 0xFFFFF9C4,
		YELLOW_200 = 0xFFFFF59D,
		YELLOW_300 = 0xFFFFF176,
		YELLOW_400 = 0xFFFFEE58,
		YELLOW_500 = 0xFFFFEB3B,
		YELLOW_600 = 0xFFFDD835,
		YELLOW_700 = 0xFFFBC02D,
		YELLOW_800 = 0xFFF9A825,
		YELLOW_900 = 0xFFF57F17,
		YELLOW_A100 = 0xFFFFFF8D,
		YELLOW_A200 = 0xFFFFFF00,
		YELLOW_A400 = 0xFFFFEA00,
		YELLOW_A700 = 0xFFFFD600,

		AMBER_50 = 0xFFFFF8E1,
		AMBER_100 = 0xFFFFECB3,
		AMBER_200 = 0xFFFFE082,
		AMBER_300 = 0xFFFFD54F,
		AMBER_400 = 0xFFFFCA28,
		AMBER_500 = 0xFFFFC107,
		AMBER_600 = 0xFFFFB300,
		AMBER_700 = 0xFFFFA000,
		AMBER_800 = 0xFFFF8F00,
		AMBER_900 = 0xFFFF7F00,
		AMBER_A100 = 0xFFFFE57F,
		AMBER_A200 = 0xFFFFD740,
		AMBER_A400 = 0xFFFFC400,
		AMBER_A700 = 0xFFFFAB00,

		ORANGE_50 = 0xFFFFF3E0,
		ORANGE_100 = 0xFFFFE0B2,
		ORANGE_200 = 0xFFFFCC80,
		ORANGE_300 = 0xFFFFB74D,
		ORANGE_400 = 0xFFFFA726,
		ORANGE_500 = 0xFFFF9800,
		ORANGE_600 = 0xFFFB8C00,
		ORANGE_700 = 0xFFF57C00,
		ORANGE_800 = 0xFFEF6C00,
		ORANGE_900 = 0xFFE65100,
		ORANGE_A100 = 0xFFFFD180,
		ORANGE_A200 = 0xFFFFAB40,
		ORANGE_A400 = 0xFFFF9100,
		ORANGE_A700 = 0xFFFF6D00,

		DEEP_ORANGE_50 = 0xFFFBE9E7,
		DEEP_ORANGE_100 = 0xFFFFCCBC,
		DEEP_ORANGE_200 = 0xFFFFAB91,
		DEEP_ORANGE_300 = 0xFFFF8A65,
		DEEP_ORANGE_400 = 0xFFFF7043,
		DEEP_ORANGE_500 = 0xFFFF5722,
		DEEP_ORANGE_600 = 0xFFF4511E,
		DEEP_ORANGE_700 = 0xFFE64A19,
		DEEP_ORANGE_800 = 0xFFD84315,
		DEEP_ORANGE_900 = 0xFFBF360C,
		DEEP_ORANGE_A100 = 0xFFFF9E80,
		DEEP_ORANGE_A200 = 0xFFFF6E40,
		DEEP_ORANGE_A400 = 0xFFFF3D00,
		DEEP_ORANGE_A700 = 0xFFDD2C00,

		BROWN_50 = 0xFFEFEBE9,
		BROWN_100 = 0xFFD7CCC8,
		BROWN_200 = 0xFFBCAAA4,
		BROWN_300 = 0xFFA1887F,
		BROWN_400 = 0xFF8D6E63,
		BROWN_500 = 0xFF795548,
		BROWN_600 = 0xFF6D4C41,
		BROWN_700 = 0xFF5D4037,
		BROWN_800 = 0xFF4E342E,
		BROWN_900 = 0xFF3E2723,

		GRAY_50 = 0xFFFAFAFA,
		GRAY_100 = 0xFFF5F5F5,
		GRAY_200 = 0xFFEEEEEE,
		GRAY_300 = 0xFFE0E0E0,
		GRAY_400 = 0xFFBDBDBD,
		GRAY_500 = 0xFF9E9E9E,
		GRAY_600 = 0xFF757575,
		GRAY_700 = 0xFF616161,
		GRAY_800 = 0xFF424242,
		GRAY_900 = 0xFF212121,

		BLUE_GRAY_50 = 0xFFECEFF1,
		BLUE_GRAY_100 = 0xFFCFD8DC,
		BLUE_GRAY_200 = 0xFFB0BEC5,
		BLUE_GRAY_300 = 0xFF90A4AE,
		BLUE_GRAY_400 = 0xFF78909C,
		BLUE_GRAY_500 = 0xFF607D8B,
		BLUE_GRAY_600 = 0xFF546E7A,
		BLUE_GRAY_700 = 0xFF455A64,
		BLUE_GRAY_800 = 0xFF37474F,
		BLUE_GRAY_900 = 0xFF263238;
}

/*
	These are the codepoints for the material icon font!
*/
namespace MaterialIcons
{
	constexpr auto THREED_ROTATION{u8"\ue84d"};
	constexpr auto AC_UNIT{u8"\ueb3b"};
	constexpr auto ACCESS_ALARM{u8"\ue190"};
	constexpr auto ACCESS_ALARMS{u8"\ue191"};
	constexpr auto ACCESS_TIME{u8"\ue192"};
	constexpr auto ACCESSIBILITY{u8"\ue84e"};
	constexpr auto ACCESSIBLE{u8"\ue914"};
	constexpr auto ACCOUNT_BALANCE{u8"\ue84f"};
	constexpr auto ACCOUNT_BALANCE_WALLET{u8"\ue850"};
	constexpr auto ACCOUNT_BOX{u8"\ue851"};
	constexpr auto ACCOUNT_CIRCLE{u8"\ue853"};
	constexpr auto ADB{u8"\ue60e"};
	constexpr auto ADD{u8"\ue145"};
	constexpr auto ADD_A_PHOTO{u8"\ue439"};
	constexpr auto ADD_ALARM{u8"\ue193"};
	constexpr auto ADD_ALERT{u8"\ue003"};
	constexpr auto ADD_BOX{u8"\ue146"};
	constexpr auto ADD_CIRCLE{u8"\ue147"};
	constexpr auto ADD_CIRCLE_OUTLINE{u8"\ue148"};
	constexpr auto ADD_LOCATION{u8"\ue567"};
	constexpr auto ADD_SHOPPING_CART{u8"\ue854"};
	constexpr auto ADD_TO_PHOTOS{u8"\ue39d"};
	constexpr auto ADD_TO_QUEUE{u8"\ue05c"};
	constexpr auto ADJUST{u8"\ue39e"};
	constexpr auto AIRLINE_SEAT_FLAT{u8"\ue630"};
	constexpr auto AIRLINE_SEAT_FLAT_ANGLED{u8"\ue631"};
	constexpr auto AIRLINE_SEAT_INDIVIDUAL_SUITE{u8"\ue632"};
	constexpr auto AIRLINE_SEAT_LEGROOM_EXTRA{u8"\ue633"};
	constexpr auto AIRLINE_SEAT_LEGROOM_NORMAL{u8"\ue634"};
	constexpr auto AIRLINE_SEAT_LEGROOM_REDUCED{u8"\ue635"};
	constexpr auto AIRLINE_SEAT_RECLINE_EXTRA{u8"\ue636"};
	constexpr auto AIRLINE_SEAT_RECLINE_NORMAL{u8"\ue637"};
	constexpr auto AIRPLANEMODE_ACTIVE{u8"\ue195"};
	constexpr auto AIRPLANEMODE_INACTIVE{u8"\ue194"};
	constexpr auto AIRPLAY{u8"\ue055"};
	constexpr auto AIRPORT_SHUTTLE{u8"\ueb3c"};
	constexpr auto ALARM{u8"\ue855"};
	constexpr auto ALARM_ADD{u8"\ue856"};
	constexpr auto ALARM_OFF{u8"\ue857"};
	constexpr auto ALARM_ON{u8"\ue858"};
	constexpr auto ALBUM{u8"\ue019"};
	constexpr auto ALL_INCLUSIVE{u8"\ueb3d"};
	constexpr auto ALL_OUT{u8"\ue90b"};
	constexpr auto ANDROID{u8"\ue859"};
	constexpr auto ANNOUNCEMENT{u8"\ue85a"};
	constexpr auto APPS{u8"\ue5c3"};
	constexpr auto ARCHIVE{u8"\ue149"};
	constexpr auto ARROW_BACK{u8"\ue5c4"};
	constexpr auto ARROW_DOWNWARD{u8"\ue5db"};
	constexpr auto ARROW_DROP_DOWN{u8"\ue5c5"};
	constexpr auto ARROW_DROP_DOWN_CIRCLE{u8"\ue5c6"};
	constexpr auto ARROW_DROP_UP{u8"\ue5c7"};
	constexpr auto ARROW_FORWARD{u8"\ue5c8"};
	constexpr auto ARROW_UPWARD{u8"\ue5d8"};
	constexpr auto ART_TRACK{u8"\ue060"};
	constexpr auto ASPECT_RATIO{u8"\ue85b"};
	constexpr auto ASSESSMENT{u8"\ue85c"};
	constexpr auto ASSIGNMENT{u8"\ue85d"};
	constexpr auto ASSIGNMENT_IND{u8"\ue85e"};
	constexpr auto ASSIGNMENT_LATE{u8"\ue85f"};
	constexpr auto ASSIGNMENT_RETURN{u8"\ue860"};
	constexpr auto ASSIGNMENT_RETURNED{u8"\ue861"};
	constexpr auto ASSIGNMENT_TURNED_IN{u8"\ue862"};
	constexpr auto ASSISTANT{u8"\ue39f"};
	constexpr auto ASSISTANT_PHOTO{u8"\ue3a0"};
	constexpr auto ATTACH_FILE{u8"\ue226"};
	constexpr auto ATTACH_MONEY{u8"\ue227"};
	constexpr auto ATTACHMENT{u8"\ue2bc"};
	constexpr auto AUDIOTRACK{u8"\ue3a1"};
	constexpr auto AUTORENEW{u8"\ue863"};
	constexpr auto AV_TIMER{u8"\ue01b"};
	constexpr auto BACKSPACE{u8"\ue14a"};
	constexpr auto BACKUP{u8"\ue864"};
	constexpr auto BATTERY_ALERT{u8"\ue19c"};
	constexpr auto BATTERY_CHARGING_FULL{u8"\ue1a3"};
	constexpr auto BATTERY_FULL{u8"\ue1a4"};
	constexpr auto BATTERY_STD{u8"\ue1a5"};
	constexpr auto BATTERY_UNKNOWN{u8"\ue1a6"};
	constexpr auto BEACH_ACCESS{u8"\ueb3e"};
	constexpr auto BEENHERE{u8"\ue52d"};
	constexpr auto BLOCK{u8"\ue14b"};
	constexpr auto BLUETOOTH{u8"\ue1a7"};
	constexpr auto BLUETOOTH_AUDIO{u8"\ue60f"};
	constexpr auto BLUETOOTH_CONNECTED{u8"\ue1a8"};
	constexpr auto BLUETOOTH_DISABLED{u8"\ue1a9"};
	constexpr auto BLUETOOTH_SEARCHING{u8"\ue1aa"};
	constexpr auto BLUR_CIRCULAR{u8"\ue3a2"};
	constexpr auto BLUR_LINEAR{u8"\ue3a3"};
	constexpr auto BLUR_OFF{u8"\ue3a4"};
	constexpr auto BLUR_ON{u8"\ue3a5"};
	constexpr auto BOOK{u8"\ue865"};
	constexpr auto BOOKMARK{u8"\ue866"};
	constexpr auto BOOKMARK_BORDER{u8"\ue867"};
	constexpr auto BORDER_ALL{u8"\ue228"};
	constexpr auto BORDER_BOTTOM{u8"\ue229"};
	constexpr auto BORDER_CLEAR{u8"\ue22a"};
	constexpr auto BORDER_COLOR{u8"\ue22b"};
	constexpr auto BORDER_HORIZONTAL{u8"\ue22c"};
	constexpr auto BORDER_INNER{u8"\ue22d"};
	constexpr auto BORDER_LEFT{u8"\ue22e"};
	constexpr auto BORDER_OUTER{u8"\ue22f"};
	constexpr auto BORDER_RIGHT{u8"\ue230"};
	constexpr auto BORDER_STYLE{u8"\ue231"};
	constexpr auto BORDER_TOP{u8"\ue232"};
	constexpr auto BORDER_VERTICAL{u8"\ue233"};
	constexpr auto BRANDING_WATERMARK{u8"\ue06b"};
	constexpr auto BRIGHTNESS_1{u8"\ue3a6"};
	constexpr auto BRIGHTNESS_2{u8"\ue3a7"};
	constexpr auto BRIGHTNESS_3{u8"\ue3a8"};
	constexpr auto BRIGHTNESS_4{u8"\ue3a9"};
	constexpr auto BRIGHTNESS_5{u8"\ue3aa"};
	constexpr auto BRIGHTNESS_6{u8"\ue3ab"};
	constexpr auto BRIGHTNESS_7{u8"\ue3ac"};
	constexpr auto BRIGHTNESS_AUTO{u8"\ue1ab"};
	constexpr auto BRIGHTNESS_HIGH{u8"\ue1ac"};
	constexpr auto BRIGHTNESS_LOW{u8"\ue1ad"};
	constexpr auto BRIGHTNESS_MEDIUM{u8"\ue1ae"};
	constexpr auto BROKEN_IMAGE{u8"\ue3ad"};
	constexpr auto BRUSH{u8"\ue3ae"};
	constexpr auto BUBBLE_CHART{u8"\ue6dd"};
	constexpr auto BUG_REPORT{u8"\ue868"};
	constexpr auto BUILD{u8"\ue869"};
	constexpr auto BURST_MODE{u8"\ue43c"};
	constexpr auto BUSINESS{u8"\ue0af"};
	constexpr auto BUSINESS_CENTER{u8"\ueb3f"};
	constexpr auto CACHED{u8"\ue86a"};
	constexpr auto CAKE{u8"\ue7e9"};
	constexpr auto CALL{u8"\ue0b0"};
	constexpr auto CALL_END{u8"\ue0b1"};
	constexpr auto CALL_MADE{u8"\ue0b2"};
	constexpr auto CALL_MERGE{u8"\ue0b3"};
	constexpr auto CALL_MISSED{u8"\ue0b4"};
	constexpr auto CALL_MISSED_OUTGOING{u8"\ue0e4"};
	constexpr auto CALL_RECEIVED{u8"\ue0b5"};
	constexpr auto CALL_SPLIT{u8"\ue0b6"};
	constexpr auto CALL_TO_ACTION{u8"\ue06c"};
	constexpr auto CAMERA{u8"\ue3af"};
	constexpr auto CAMERA_ALT{u8"\ue3b0"};
	constexpr auto CAMERA_ENHANCE{u8"\ue8fc"};
	constexpr auto CAMERA_FRONT{u8"\ue3b1"};
	constexpr auto CAMERA_REAR{u8"\ue3b2"};
	constexpr auto CAMERA_ROLL{u8"\ue3b3"};
	constexpr auto CANCEL{u8"\ue5c9"};
	constexpr auto CARD_GIFTCARD{u8"\ue8f6"};
	constexpr auto CARD_MEMBERSHIP{u8"\ue8f7"};
	constexpr auto CARD_TRAVEL{u8"\ue8f8"};
	constexpr auto CASINO{u8"\ueb40"};
	constexpr auto CAST{u8"\ue307"};
	constexpr auto CAST_CONNECTED{u8"\ue308"};
	constexpr auto CENTER_FOCUS_STRONG{u8"\ue3b4"};
	constexpr auto CENTER_FOCUS_WEAK{u8"\ue3b5"};
	constexpr auto CHANGE_HISTORY{u8"\ue86b"};
	constexpr auto CHAT{u8"\ue0b7"};
	constexpr auto CHAT_BUBBLE{u8"\ue0ca"};
	constexpr auto CHAT_BUBBLE_OUTLINE{u8"\ue0cb"};
	constexpr auto CHECK{u8"\ue5ca"};
	constexpr auto CHECK_BOX{u8"\ue834"};
	constexpr auto CHECK_BOX_OUTLINE_BLANK{u8"\ue835"};
	constexpr auto CHECK_CIRCLE{u8"\ue86c"};
	constexpr auto CHEVRON_LEFT{u8"\ue5cb"};
	constexpr auto CHEVRON_RIGHT{u8"\ue5cc"};
	constexpr auto CHILD_CARE{u8"\ueb41"};
	constexpr auto CHILD_FRIENDLY{u8"\ueb42"};
	constexpr auto CHROME_READER_MODE{u8"\ue86d"};
	constexpr auto CLASS{u8"\ue86e"};
	constexpr auto CLEAR{u8"\ue14c"};
	constexpr auto CLEAR_ALL{u8"\ue0b8"};
	constexpr auto CLOSE{u8"\ue5cd"};
	constexpr auto CLOSED_CAPTION{u8"\ue01c"};
	constexpr auto CLOUD{u8"\ue2bd"};
	constexpr auto CLOUD_CIRCLE{u8"\ue2be"};
	constexpr auto CLOUD_DONE{u8"\ue2bf"};
	constexpr auto CLOUD_DOWNLOAD{u8"\ue2c0"};
	constexpr auto CLOUD_OFF{u8"\ue2c1"};
	constexpr auto CLOUD_QUEUE{u8"\ue2c2"};
	constexpr auto CLOUD_UPLOAD{u8"\ue2c3"};
	constexpr auto CODE{u8"\ue86f"};
	constexpr auto COLLECTIONS{u8"\ue3b6"};
	constexpr auto COLLECTIONS_BOOKMARK{u8"\ue431"};
	constexpr auto COLOR_LENS{u8"\ue3b7"};
	constexpr auto COLORIZE{u8"\ue3b8"};
	constexpr auto COMMENT{u8"\ue0b9"};
	constexpr auto COMPARE{u8"\ue3b9"};
	constexpr auto COMPARE_ARROWS{u8"\ue915"};
	constexpr auto COMPUTER{u8"\ue30a"};
	constexpr auto CONFIRMATION_NUMBER{u8"\ue638"};
	constexpr auto CONTACT_MAIL{u8"\ue0d0"};
	constexpr auto CONTACT_PHONE{u8"\ue0cf"};
	constexpr auto CONTACTS{u8"\ue0ba"};
	constexpr auto CONTENT_COPY{u8"\ue14d"};
	constexpr auto CONTENT_CUT{u8"\ue14e"};
	constexpr auto CONTENT_PASTE{u8"\ue14f"};
	constexpr auto CONTROL_POINT{u8"\ue3ba"};
	constexpr auto CONTROL_POINT_DUPLICATE{u8"\ue3bb"};
	constexpr auto COPYRIGHT{u8"\ue90c"};
	constexpr auto CREATE{u8"\ue150"};
	constexpr auto CREATE_NEW_FOLDER{u8"\ue2cc"};
	constexpr auto CREDIT_CARD{u8"\ue870"};
	constexpr auto CROP{u8"\ue3be"};
	constexpr auto CROP_16_9{u8"\ue3bc"};
	constexpr auto CROP_3_2{u8"\ue3bd"};
	constexpr auto CROP_5_4{u8"\ue3bf"};
	constexpr auto CROP_7_5{u8"\ue3c0"};
	constexpr auto CROP_DIN{u8"\ue3c1"};
	constexpr auto CROP_FREE{u8"\ue3c2"};
	constexpr auto CROP_LANDSCAPE{u8"\ue3c3"};
	constexpr auto CROP_ORIGINAL{u8"\ue3c4"};
	constexpr auto CROP_PORTRAIT{u8"\ue3c5"};
	constexpr auto CROP_ROTATE{u8"\ue437"};
	constexpr auto CROP_SQUARE{u8"\ue3c6"};
	constexpr auto DASHBOARD{u8"\ue871"};
	constexpr auto DATA_USAGE{u8"\ue1af"};
	constexpr auto DATE_RANGE{u8"\ue916"};
	constexpr auto DEHAZE{u8"\ue3c7"};
#undef DELETE
	constexpr auto DELETE{u8"\ue872"};
	constexpr auto DELETE_FOREVER{u8"\ue92b"};
	constexpr auto DELETE_SWEEP{u8"\ue16c"};
	constexpr auto DESCRIPTION{u8"\ue873"};
	constexpr auto DESKTOP_MAC{u8"\ue30b"};
	constexpr auto DESKTOP_WINDOWS{u8"\ue30c"};
	constexpr auto DETAILS{u8"\ue3c8"};
	constexpr auto DEVELOPER_BOARD{u8"\ue30d"};
	constexpr auto DEVELOPER_MODE{u8"\ue1b0"};
	constexpr auto DEVICE_HUB{u8"\ue335"};
	constexpr auto DEVICES{u8"\ue1b1"};
	constexpr auto DEVICES_OTHER{u8"\ue337"};
	constexpr auto DIALER_SIP{u8"\ue0bb"};
	constexpr auto DIALPAD{u8"\ue0bc"};
	constexpr auto DIRECTIONS{u8"\ue52e"};
	constexpr auto DIRECTIONS_BIKE{u8"\ue52f"};
	constexpr auto DIRECTIONS_BOAT{u8"\ue532"};
	constexpr auto DIRECTIONS_BUS{u8"\ue530"};
	constexpr auto DIRECTIONS_CAR{u8"\ue531"};
	constexpr auto DIRECTIONS_RAILWAY{u8"\ue534"};
	constexpr auto DIRECTIONS_RUN{u8"\ue566"};
	constexpr auto DIRECTIONS_SUBWAY{u8"\ue533"};
	constexpr auto DIRECTIONS_TRANSIT{u8"\ue535"};
	constexpr auto DIRECTIONS_WALK{u8"\ue536"};
	constexpr auto DISC_FULL{u8"\ue610"};
	constexpr auto DNS{u8"\ue875"};
	constexpr auto DO_NOT_DISTURB{u8"\ue612"};
	constexpr auto DO_NOT_DISTURB_ALT{u8"\ue611"};
	constexpr auto DO_NOT_DISTURB_OFF{u8"\ue643"};
	constexpr auto DO_NOT_DISTURB_ON{u8"\ue644"};
	constexpr auto DOCK{u8"\ue30e"};
#undef DOMAIN
	constexpr auto DOMAIN{u8"\ue7ee"};
	constexpr auto DONE{u8"\ue876"};
	constexpr auto DONE_ALL{u8"\ue877"};
	constexpr auto DONUT_LARGE{u8"\ue917"};
	constexpr auto DONUT_SMALL{u8"\ue918"};
	constexpr auto DRAFTS{u8"\ue151"};
	constexpr auto DRAG_HANDLE{u8"\ue25d"};
	constexpr auto DRIVE_ETA{u8"\ue613"};
	constexpr auto DVR{u8"\ue1b2"};
	constexpr auto EDIT{u8"\ue3c9"};
	constexpr auto EDIT_LOCATION{u8"\ue568"};
	constexpr auto EJECT{u8"\ue8fb"};
	constexpr auto EMAIL{u8"\ue0be"};
	constexpr auto ENHANCED_ENCRYPTION{u8"\ue63f"};
	constexpr auto EQUALIZER{u8"\ue01d"};
	constexpr auto ERROR{u8"\ue000"};
	constexpr auto ERROR_OUTLINE{u8"\ue001"};
	constexpr auto EURO_SYMBOL{u8"\ue926"};
	constexpr auto EV_STATION{u8"\ue56d"};
	constexpr auto EVENT{u8"\ue878"};
	constexpr auto EVENT_AVAILABLE{u8"\ue614"};
	constexpr auto EVENT_BUSY{u8"\ue615"};
	constexpr auto EVENT_NOTE{u8"\ue616"};
	constexpr auto EVENT_SEAT{u8"\ue903"};
	constexpr auto EXIT_TO_APP{u8"\ue879"};
	constexpr auto EXPAND_LESS{u8"\ue5ce"};
	constexpr auto EXPAND_MORE{u8"\ue5cf"};
	constexpr auto EXPLICIT{u8"\ue01e"};
	constexpr auto EXPLORE{u8"\ue87a"};
	constexpr auto EXPOSURE{u8"\ue3ca"};
	constexpr auto EXPOSURE_NEG_1{u8"\ue3cb"};
	constexpr auto EXPOSURE_NEG_2{u8"\ue3cc"};
	constexpr auto EXPOSURE_PLUS_1{u8"\ue3cd"};
	constexpr auto EXPOSURE_PLUS_2{u8"\ue3ce"};
	constexpr auto EXPOSURE_ZERO{u8"\ue3cf"};
	constexpr auto EXTENSION{u8"\ue87b"};
	constexpr auto FACE{u8"\ue87c"};
	constexpr auto FAST_FORWARD{u8"\ue01f"};
	constexpr auto FAST_REWIND{u8"\ue020"};
	constexpr auto FAVORITE{u8"\ue87d"};
	constexpr auto FAVORITE_BORDER{u8"\ue87e"};
	constexpr auto FEATURED_PLAY_LIST{u8"\ue06d"};
	constexpr auto FEATURED_VIDEO{u8"\ue06e"};
	constexpr auto FEEDBACK{u8"\ue87f"};
	constexpr auto FIBER_DVR{u8"\ue05d"};
	constexpr auto FIBER_MANUAL_RECORD{u8"\ue061"};
	constexpr auto FIBER_NEW{u8"\ue05e"};
	constexpr auto FIBER_PIN{u8"\ue06a"};
	constexpr auto FIBER_SMART_RECORD{u8"\ue062"};
	constexpr auto FILE_DOWNLOAD{u8"\ue2c4"};
	constexpr auto FILE_UPLOAD{u8"\ue2c6"};
	constexpr auto FILTER{u8"\ue3d3"};
	constexpr auto FILTER_1{u8"\ue3d0"};
	constexpr auto FILTER_2{u8"\ue3d1"};
	constexpr auto FILTER_3{u8"\ue3d2"};
	constexpr auto FILTER_4{u8"\ue3d4"};
	constexpr auto FILTER_5{u8"\ue3d5"};
	constexpr auto FILTER_6{u8"\ue3d6"};
	constexpr auto FILTER_7{u8"\ue3d7"};
	constexpr auto FILTER_8{u8"\ue3d8"};
	constexpr auto FILTER_9{u8"\ue3d9"};
	constexpr auto FILTER_9_PLUS{u8"\ue3da"};
	constexpr auto FILTER_B_AND_W{u8"\ue3db"};
	constexpr auto FILTER_CENTER_FOCUS{u8"\ue3dc"};
	constexpr auto FILTER_DRAMA{u8"\ue3dd"};
	constexpr auto FILTER_FRAMES{u8"\ue3de"};
	constexpr auto FILTER_HDR{u8"\ue3df"};
	constexpr auto FILTER_LIST{u8"\ue152"};
	constexpr auto FILTER_NONE{u8"\ue3e0"};
	constexpr auto FILTER_TILT_SHIFT{u8"\ue3e2"};
	constexpr auto FILTER_VINTAGE{u8"\ue3e3"};
	constexpr auto FIND_IN_PAGE{u8"\ue880"};
	constexpr auto FIND_REPLACE{u8"\ue881"};
	constexpr auto FINGERPRINT{u8"\ue90d"};
	constexpr auto FIRST_PAGE{u8"\ue5dc"};
	constexpr auto FITNESS_CENTER{u8"\ueb43"};
	constexpr auto FLAG{u8"\ue153"};
	constexpr auto FLARE{u8"\ue3e4"};
	constexpr auto FLASH_AUTO{u8"\ue3e5"};
	constexpr auto FLASH_OFF{u8"\ue3e6"};
	constexpr auto FLASH_ON{u8"\ue3e7"};
	constexpr auto FLIGHT{u8"\ue539"};
	constexpr auto FLIGHT_LAND{u8"\ue904"};
	constexpr auto FLIGHT_TAKEOFF{u8"\ue905"};
	constexpr auto FLIP{u8"\ue3e8"};
	constexpr auto FLIP_TO_BACK{u8"\ue882"};
	constexpr auto FLIP_TO_FRONT{u8"\ue883"};
	constexpr auto FOLDER{u8"\ue2c7"};
	constexpr auto FOLDER_OPEN{u8"\ue2c8"};
	constexpr auto FOLDER_SHARED{u8"\ue2c9"};
	constexpr auto FOLDER_SPECIAL{u8"\ue617"};
	constexpr auto FONT_DOWNLOAD{u8"\ue167"};
	constexpr auto FORMAT_ALIGN_CENTER{u8"\ue234"};
	constexpr auto FORMAT_ALIGN_JUSTIFY{u8"\ue235"};
	constexpr auto FORMAT_ALIGN_LEFT{u8"\ue236"};
	constexpr auto FORMAT_ALIGN_RIGHT{u8"\ue237"};
	constexpr auto FORMAT_BOLD{u8"\ue238"};
	constexpr auto FORMAT_CLEAR{u8"\ue239"};
	constexpr auto FORMAT_COLOR_FILL{u8"\ue23a"};
	constexpr auto FORMAT_COLOR_RESET{u8"\ue23b"};
	constexpr auto FORMAT_COLOR_TEXT{u8"\ue23c"};
	constexpr auto FORMAT_INDENT_DECREASE{u8"\ue23d"};
	constexpr auto FORMAT_INDENT_INCREASE{u8"\ue23e"};
	constexpr auto FORMAT_ITALIC{u8"\ue23f"};
	constexpr auto FORMAT_LINE_SPACING{u8"\ue240"};
	constexpr auto FORMAT_LIST_BULLETED{u8"\ue241"};
	constexpr auto FORMAT_LIST_NUMBERED{u8"\ue242"};
	constexpr auto FORMAT_PAINT{u8"\ue243"};
	constexpr auto FORMAT_QUOTE{u8"\ue244"};
	constexpr auto FORMAT_SHAPES{u8"\ue25e"};
	constexpr auto FORMAT_SIZE{u8"\ue245"};
	constexpr auto FORMAT_STRIKETHROUGH{u8"\ue246"};
	constexpr auto FORMAT_TEXTDIRECTION_L_TO_R{u8"\ue247"};
	constexpr auto FORMAT_TEXTDIRECTION_R_TO_L{u8"\ue248"};
	constexpr auto FORMAT_UNDERLINED{u8"\ue249"};
	constexpr auto FORUM{u8"\ue0bf"};
	constexpr auto FORWARD{u8"\ue154"};
	constexpr auto FORWARD_10{u8"\ue056"};
	constexpr auto FORWARD_30{u8"\ue057"};
	constexpr auto FORWARD_5{u8"\ue058"};
	constexpr auto FREE_BREAKFAST{u8"\ueb44"};
	constexpr auto FULLSCREEN{u8"\ue5d0"};
	constexpr auto FULLSCREEN_EXIT{u8"\ue5d1"};
	constexpr auto FUNCTIONS{u8"\ue24a"};
	constexpr auto G_TRANSLATE{u8"\ue927"};
	constexpr auto GAMEPAD{u8"\ue30f"};
	constexpr auto GAMES{u8"\ue021"};
	constexpr auto GAVEL{u8"\ue90e"};
	constexpr auto GESTURE{u8"\ue155"};
	constexpr auto GET_APP{u8"\ue884"};
	constexpr auto GIF{u8"\ue908"};
	constexpr auto GOLF_COURSE{u8"\ueb45"};
	constexpr auto GPS_FIXED{u8"\ue1b3"};
	constexpr auto GPS_NOT_FIXED{u8"\ue1b4"};
	constexpr auto GPS_OFF{u8"\ue1b5"};
	constexpr auto GRADE{u8"\ue885"};
	constexpr auto GRADIENT{u8"\ue3e9"};
	constexpr auto GRAIN{u8"\ue3ea"};
	constexpr auto GRAPHIC_EQ{u8"\ue1b8"};
	constexpr auto GRID_OFF{u8"\ue3eb"};
	constexpr auto GRID_ON{u8"\ue3ec"};
	constexpr auto GROUP{u8"\ue7ef"};
	constexpr auto GROUP_ADD{u8"\ue7f0"};
	constexpr auto GROUP_WORK{u8"\ue886"};
	constexpr auto HD{u8"\ue052"};
	constexpr auto HDR_OFF{u8"\ue3ed"};
	constexpr auto HDR_ON{u8"\ue3ee"};
	constexpr auto HDR_STRONG{u8"\ue3f1"};
	constexpr auto HDR_WEAK{u8"\ue3f2"};
	constexpr auto HEADSET{u8"\ue310"};
	constexpr auto HEADSET_MIC{u8"\ue311"};
	constexpr auto HEALING{u8"\ue3f3"};
	constexpr auto HEARING{u8"\ue023"};
	constexpr auto HELP{u8"\ue887"};
	constexpr auto HELP_OUTLINE{u8"\ue8fd"};
	constexpr auto HIGH_QUALITY{u8"\ue024"};
	constexpr auto HIGHLIGHT{u8"\ue25f"};
	constexpr auto HIGHLIGHT_OFF{u8"\ue888"};
	constexpr auto HISTORY{u8"\ue889"};
	constexpr auto HOME{u8"\ue88a"};
	constexpr auto HOT_TUB{u8"\ueb46"};
	constexpr auto HOTEL{u8"\ue53a"};
	constexpr auto HOURGLASS_EMPTY{u8"\ue88b"};
	constexpr auto HOURGLASS_FULL{u8"\ue88c"};
	constexpr auto HTTP{u8"\ue902"};
	constexpr auto HTTPS{u8"\ue88d"};
	constexpr auto IMAGE{u8"\ue3f4"};
	constexpr auto IMAGE_ASPECT_RATIO{u8"\ue3f5"};
	constexpr auto IMPORT_CONTACTS{u8"\ue0e0"};
	constexpr auto IMPORT_EXPORT{u8"\ue0c3"};
	constexpr auto IMPORTANT_DEVICES{u8"\ue912"};
	constexpr auto INBOX{u8"\ue156"};
	constexpr auto INDETERMINATE_CHECK_BOX{u8"\ue909"};
	constexpr auto INFO{u8"\ue88e"};
	constexpr auto INFO_OUTLINE{u8"\ue88f"};
	constexpr auto INPUT{u8"\ue890"};
	constexpr auto INSERT_CHART{u8"\ue24b"};
	constexpr auto INSERT_COMMENT{u8"\ue24c"};
	constexpr auto INSERT_DRIVE_FILE{u8"\ue24d"};
	constexpr auto INSERT_EMOTICON{u8"\ue24e"};
	constexpr auto INSERT_INVITATION{u8"\ue24f"};
	constexpr auto INSERT_LINK{u8"\ue250"};
	constexpr auto INSERT_PHOTO{u8"\ue251"};
	constexpr auto INVERT_COLORS{u8"\ue891"};
	constexpr auto INVERT_COLORS_OFF{u8"\ue0c4"};
	constexpr auto ISO{u8"\ue3f6"};
	constexpr auto KEYBOARD{u8"\ue312"};
	constexpr auto KEYBOARD_ARROW_DOWN{u8"\ue313"};
	constexpr auto KEYBOARD_ARROW_LEFT{u8"\ue314"};
	constexpr auto KEYBOARD_ARROW_RIGHT{u8"\ue315"};
	constexpr auto KEYBOARD_ARROW_UP{u8"\ue316"};
	constexpr auto KEYBOARD_BACKSPACE{u8"\ue317"};
	constexpr auto KEYBOARD_CAPSLOCK{u8"\ue318"};
	constexpr auto KEYBOARD_HIDE{u8"\ue31a"};
	constexpr auto KEYBOARD_RETURN{u8"\ue31b"};
	constexpr auto KEYBOARD_TAB{u8"\ue31c"};
	constexpr auto KEYBOARD_VOICE{u8"\ue31d"};
	constexpr auto KITCHEN{u8"\ueb47"};
	constexpr auto LABEL{u8"\ue892"};
	constexpr auto LABEL_OUTLINE{u8"\ue893"};
	constexpr auto LANDSCAPE{u8"\ue3f7"};
	constexpr auto LANGUAGE{u8"\ue894"};
	constexpr auto LAPTOP{u8"\ue31e"};
	constexpr auto LAPTOP_CHROMEBOOK{u8"\ue31f"};
	constexpr auto LAPTOP_MAC{u8"\ue320"};
	constexpr auto LAPTOP_WINDOWS{u8"\ue321"};
	constexpr auto LAST_PAGE{u8"\ue5dd"};
	constexpr auto LAUNCH{u8"\ue895"};
	constexpr auto LAYERS{u8"\ue53b"};
	constexpr auto LAYERS_CLEAR{u8"\ue53c"};
	constexpr auto LEAK_ADD{u8"\ue3f8"};
	constexpr auto LEAK_REMOVE{u8"\ue3f9"};
	constexpr auto LENS{u8"\ue3fa"};
	constexpr auto LIBRARY_ADD{u8"\ue02e"};
	constexpr auto LIBRARY_BOOKS{u8"\ue02f"};
	constexpr auto LIBRARY_MUSIC{u8"\ue030"};
	constexpr auto LIGHTBULB_OUTLINE{u8"\ue90f"};
	constexpr auto LINE_STYLE{u8"\ue919"};
	constexpr auto LINE_WEIGHT{u8"\ue91a"};
	constexpr auto LINEAR_SCALE{u8"\ue260"};
	constexpr auto LINK{u8"\ue157"};
	constexpr auto LINKED_CAMERA{u8"\ue438"};
	constexpr auto LIST{u8"\ue896"};
	constexpr auto LIVE_HELP{u8"\ue0c6"};
	constexpr auto LIVE_TV{u8"\ue639"};
	constexpr auto LOCAL_ACTIVITY{u8"\ue53f"};
	constexpr auto LOCAL_AIRPORT{u8"\ue53d"};
	constexpr auto LOCAL_ATM{u8"\ue53e"};
	constexpr auto LOCAL_BAR{u8"\ue540"};
	constexpr auto LOCAL_CAFE{u8"\ue541"};
	constexpr auto LOCAL_CAR_WASH{u8"\ue542"};
	constexpr auto LOCAL_CONVENIENCE_STORE{u8"\ue543"};
	constexpr auto LOCAL_DINING{u8"\ue556"};
	constexpr auto LOCAL_DRINK{u8"\ue544"};
	constexpr auto LOCAL_FLORIST{u8"\ue545"};
	constexpr auto LOCAL_GAS_STATION{u8"\ue546"};
	constexpr auto LOCAL_GROCERY_STORE{u8"\ue547"};
	constexpr auto LOCAL_HOSPITAL{u8"\ue548"};
	constexpr auto LOCAL_HOTEL{u8"\ue549"};
	constexpr auto LOCAL_LAUNDRY_SERVICE{u8"\ue54a"};
	constexpr auto LOCAL_LIBRARY{u8"\ue54b"};
	constexpr auto LOCAL_MALL{u8"\ue54c"};
	constexpr auto LOCAL_MOVIES{u8"\ue54d"};
	constexpr auto LOCAL_OFFER{u8"\ue54e"};
	constexpr auto LOCAL_PARKING{u8"\ue54f"};
	constexpr auto LOCAL_PHARMACY{u8"\ue550"};
	constexpr auto LOCAL_PHONE{u8"\ue551"};
	constexpr auto LOCAL_PIZZA{u8"\ue552"};
	constexpr auto LOCAL_PLAY{u8"\ue553"};
	constexpr auto LOCAL_POST_OFFICE{u8"\ue554"};
	constexpr auto LOCAL_PRINTSHOP{u8"\ue555"};
	constexpr auto LOCAL_SEE{u8"\ue557"};
	constexpr auto LOCAL_SHIPPING{u8"\ue558"};
	constexpr auto LOCAL_TAXI{u8"\ue559"};
	constexpr auto LOCATION_CITY{u8"\ue7f1"};
	constexpr auto LOCATION_DISABLED{u8"\ue1b6"};
	constexpr auto LOCATION_OFF{u8"\ue0c7"};
	constexpr auto LOCATION_ON{u8"\ue0c8"};
	constexpr auto LOCATION_SEARCHING{u8"\ue1b7"};
	constexpr auto LOCK{u8"\ue897"};
	constexpr auto LOCK_OPEN{u8"\ue898"};
	constexpr auto LOCK_OUTLINE{u8"\ue899"};
	constexpr auto LOOKS{u8"\ue3fc"};
	constexpr auto LOOKS_3{u8"\ue3fb"};
	constexpr auto LOOKS_4{u8"\ue3fd"};
	constexpr auto LOOKS_5{u8"\ue3fe"};
	constexpr auto LOOKS_6{u8"\ue3ff"};
	constexpr auto LOOKS_ONE{u8"\ue400"};
	constexpr auto LOOKS_TWO{u8"\ue401"};
	constexpr auto LOOP{u8"\ue028"};
	constexpr auto LOUPE{u8"\ue402"};
	constexpr auto LOW_PRIORITY{u8"\ue16d"};
	constexpr auto LOYALTY{u8"\ue89a"};
	constexpr auto MAIL{u8"\ue158"};
	constexpr auto MAIL_OUTLINE{u8"\ue0e1"};
	constexpr auto MAP{u8"\ue55b"};
	constexpr auto MARKUNREAD{u8"\ue159"};
	constexpr auto MARKUNREAD_MAILBOX{u8"\ue89b"};
	constexpr auto MEMORY{u8"\ue322"};
	constexpr auto MENU{u8"\ue5d2"};
	constexpr auto MERGE_TYPE{u8"\ue252"};
	constexpr auto MESSAGE{u8"\ue0c9"};
	constexpr auto MIC{u8"\ue029"};
	constexpr auto MIC_NONE{u8"\ue02a"};
	constexpr auto MIC_OFF{u8"\ue02b"};
	constexpr auto MMS{u8"\ue618"};
	constexpr auto MODE_COMMENT{u8"\ue253"};
	constexpr auto MODE_EDIT{u8"\ue254"};
	constexpr auto MONETIZATION_ON{u8"\ue263"};
	constexpr auto MONEY_OFF{u8"\ue25c"};
	constexpr auto MONOCHROME_PHOTOS{u8"\ue403"};
	constexpr auto MOOD{u8"\ue7f2"};
	constexpr auto MOOD_BAD{u8"\ue7f3"};
	constexpr auto MORE{u8"\ue619"};
	constexpr auto MORE_HORIZ{u8"\ue5d3"};
	constexpr auto MORE_VERT{u8"\ue5d4"};
	constexpr auto MOTORCYCLE{u8"\ue91b"};
	constexpr auto MOUSE{u8"\ue323"};
	constexpr auto MOVE_TO_INBOX{u8"\ue168"};
	constexpr auto MOVIE{u8"\ue02c"};
	constexpr auto MOVIE_CREATION{u8"\ue404"};
	constexpr auto MOVIE_FILTER{u8"\ue43a"};
	constexpr auto MULTILINE_CHART{u8"\ue6df"};
	constexpr auto MUSIC_NOTE{u8"\ue405"};
	constexpr auto MUSIC_VIDEO{u8"\ue063"};
	constexpr auto MY_LOCATION{u8"\ue55c"};
	constexpr auto NATURE{u8"\ue406"};
	constexpr auto NATURE_PEOPLE{u8"\ue407"};
	constexpr auto NAVIGATE_BEFORE{u8"\ue408"};
	constexpr auto NAVIGATE_NEXT{u8"\ue409"};
	constexpr auto NAVIGATION{u8"\ue55d"};
	constexpr auto NEAR_ME{u8"\ue569"};
	constexpr auto NETWORK_CELL{u8"\ue1b9"};
	constexpr auto NETWORK_CHECK{u8"\ue640"};
	constexpr auto NETWORK_LOCKED{u8"\ue61a"};
	constexpr auto NETWORK_WIFI{u8"\ue1ba"};
	constexpr auto NEW_RELEASES{u8"\ue031"};
	constexpr auto NEXT_WEEK{u8"\ue16a"};
	constexpr auto NFC{u8"\ue1bb"};
	constexpr auto NO_ENCRYPTION{u8"\ue641"};
	constexpr auto NO_SIM{u8"\ue0cc"};
	constexpr auto NOT_INTERESTED{u8"\ue033"};
	constexpr auto NOTE{u8"\ue06f"};
	constexpr auto NOTE_ADD{u8"\ue89c"};
	constexpr auto NOTIFICATIONS{u8"\ue7f4"};
	constexpr auto NOTIFICATIONS_ACTIVE{u8"\ue7f7"};
	constexpr auto NOTIFICATIONS_NONE{u8"\ue7f5"};
	constexpr auto NOTIFICATIONS_OFF{u8"\ue7f6"};
	constexpr auto NOTIFICATIONS_PAUSED{u8"\ue7f8"};
	constexpr auto OFFLINE_PIN{u8"\ue90a"};
	constexpr auto ONDEMAND_VIDEO{u8"\ue63a"};
	constexpr auto OPACITY{u8"\ue91c"};
	constexpr auto OPEN_IN_BROWSER{u8"\ue89d"};
	constexpr auto OPEN_IN_NEW{u8"\ue89e"};
	constexpr auto OPEN_WITH{u8"\ue89f"};
	constexpr auto PAGES{u8"\ue7f9"};
	constexpr auto PAGEVIEW{u8"\ue8a0"};
	constexpr auto PALETTE{u8"\ue40a"};
	constexpr auto PAN_TOOL{u8"\ue925"};
	constexpr auto PANORAMA{u8"\ue40b"};
	constexpr auto PANORAMA_FISH_EYE{u8"\ue40c"};
	constexpr auto PANORAMA_HORIZONTAL{u8"\ue40d"};
	constexpr auto PANORAMA_VERTICAL{u8"\ue40e"};
	constexpr auto PANORAMA_WIDE_ANGLE{u8"\ue40f"};
	constexpr auto PARTY_MODE{u8"\ue7fa"};
	constexpr auto PAUSE{u8"\ue034"};
	constexpr auto PAUSE_CIRCLE_FILLED{u8"\ue035"};
	constexpr auto PAUSE_CIRCLE_OUTLINE{u8"\ue036"};
	constexpr auto PAYMENT{u8"\ue8a1"};
	constexpr auto PEOPLE{u8"\ue7fb"};
	constexpr auto PEOPLE_OUTLINE{u8"\ue7fc"};
	constexpr auto PERM_CAMERA_MIC{u8"\ue8a2"};
	constexpr auto PERM_CONTACT_CALENDAR{u8"\ue8a3"};
	constexpr auto PERM_DATA_SETTING{u8"\ue8a4"};
	constexpr auto PERM_DEVICE_INFORMATION{u8"\ue8a5"};
	constexpr auto PERM_IDENTITY{u8"\ue8a6"};
	constexpr auto PERM_MEDIA{u8"\ue8a7"};
	constexpr auto PERM_PHONE_MSG{u8"\ue8a8"};
	constexpr auto PERM_SCAN_WIFI{u8"\ue8a9"};
	constexpr auto PERSON{u8"\ue7fd"};
	constexpr auto PERSON_ADD{u8"\ue7fe"};
	constexpr auto PERSON_OUTLINE{u8"\ue7ff"};
	constexpr auto PERSON_PIN{u8"\ue55a"};
	constexpr auto PERSON_PIN_CIRCLE{u8"\ue56a"};
	constexpr auto PERSONAL_VIDEO{u8"\ue63b"};
	constexpr auto PETS{u8"\ue91d"};
	constexpr auto PHONE{u8"\ue0cd"};
	constexpr auto PHONE_ANDROID{u8"\ue324"};
	constexpr auto PHONE_BLUETOOTH_SPEAKER{u8"\ue61b"};
	constexpr auto PHONE_FORWARDED{u8"\ue61c"};
	constexpr auto PHONE_IN_TALK{u8"\ue61d"};
	constexpr auto PHONE_IPHONE{u8"\ue325"};
	constexpr auto PHONE_LOCKED{u8"\ue61e"};
	constexpr auto PHONE_MISSED{u8"\ue61f"};
	constexpr auto PHONE_PAUSED{u8"\ue620"};
	constexpr auto PHONELINK{u8"\ue326"};
	constexpr auto PHONELINK_ERASE{u8"\ue0db"};
	constexpr auto PHONELINK_LOCK{u8"\ue0dc"};
	constexpr auto PHONELINK_OFF{u8"\ue327"};
	constexpr auto PHONELINK_RING{u8"\ue0dd"};
	constexpr auto PHONELINK_SETUP{u8"\ue0de"};
	constexpr auto PHOTO{u8"\ue410"};
	constexpr auto PHOTO_ALBUM{u8"\ue411"};
	constexpr auto PHOTO_CAMERA{u8"\ue412"};
	constexpr auto PHOTO_FILTER{u8"\ue43b"};
	constexpr auto PHOTO_LIBRARY{u8"\ue413"};
	constexpr auto PHOTO_SIZE_SELECT_ACTUAL{u8"\ue432"};
	constexpr auto PHOTO_SIZE_SELECT_LARGE{u8"\ue433"};
	constexpr auto PHOTO_SIZE_SELECT_SMALL{u8"\ue434"};
	constexpr auto PICTURE_AS_PDF{u8"\ue415"};
	constexpr auto PICTURE_IN_PICTURE{u8"\ue8aa"};
	constexpr auto PICTURE_IN_PICTURE_ALT{u8"\ue911"};
	constexpr auto PIE_CHART{u8"\ue6c4"};
	constexpr auto PIE_CHART_OUTLINED{u8"\ue6c5"};
	constexpr auto PIN_DROP{u8"\ue55e"};
	constexpr auto PLACE{u8"\ue55f"};
	constexpr auto PLAY_ARROW{u8"\ue037"};
	constexpr auto PLAY_CIRCLE_FILLED{u8"\ue038"};
	constexpr auto PLAY_CIRCLE_OUTLINE{u8"\ue039"};
	constexpr auto PLAY_FOR_WORK{u8"\ue906"};
	constexpr auto PLAYLIST_ADD{u8"\ue03b"};
	constexpr auto PLAYLIST_ADD_CHECK{u8"\ue065"};
	constexpr auto PLAYLIST_PLAY{u8"\ue05f"};
	constexpr auto PLUS_ONE{u8"\ue800"};
	constexpr auto POLL{u8"\ue801"};
	constexpr auto POLYMER{u8"\ue8ab"};
	constexpr auto POOL{u8"\ueb48"};
	constexpr auto PORTABLE_WIFI_OFF{u8"\ue0ce"};
	constexpr auto PORTRAIT{u8"\ue416"};
	constexpr auto POWER{u8"\ue63c"};
	constexpr auto POWER_INPUT{u8"\ue336"};
	constexpr auto POWER_SETTINGS_NEW{u8"\ue8ac"};
	constexpr auto PREGNANT_WOMAN{u8"\ue91e"};
	constexpr auto PRESENT_TO_ALL{u8"\ue0df"};
	constexpr auto PRINT{u8"\ue8ad"};
	constexpr auto PRIORITY_HIGH{u8"\ue645"};
	constexpr auto PUBLIC{u8"\ue80b"};
	constexpr auto PUBLISH{u8"\ue255"};
	constexpr auto QUERY_BUILDER{u8"\ue8ae"};
	constexpr auto QUESTION_ANSWER{u8"\ue8af"};
	constexpr auto QUEUE{u8"\ue03c"};
	constexpr auto QUEUE_MUSIC{u8"\ue03d"};
	constexpr auto QUEUE_PLAY_NEXT{u8"\ue066"};
	constexpr auto RADIO{u8"\ue03e"};
	constexpr auto RADIO_BUTTON_CHECKED{u8"\ue837"};
	constexpr auto RADIO_BUTTON_UNCHECKED{u8"\ue836"};
	constexpr auto RATE_REVIEW{u8"\ue560"};
	constexpr auto RECEIPT{u8"\ue8b0"};
	constexpr auto RECENT_ACTORS{u8"\ue03f"};
	constexpr auto RECORD_VOICE_OVER{u8"\ue91f"};
	constexpr auto REDEEM{u8"\ue8b1"};
	constexpr auto REDO{u8"\ue15a"};
	constexpr auto REFRESH{u8"\ue5d5"};
	constexpr auto REMOVE{u8"\ue15b"};
	constexpr auto REMOVE_CIRCLE{u8"\ue15c"};
	constexpr auto REMOVE_CIRCLE_OUTLINE{u8"\ue15d"};
	constexpr auto REMOVE_FROM_QUEUE{u8"\ue067"};
	constexpr auto REMOVE_RED_EYE{u8"\ue417"};
	constexpr auto REMOVE_SHOPPING_CART{u8"\ue928"};
	constexpr auto REORDER{u8"\ue8fe"};
	constexpr auto REPEAT{u8"\ue040"};
	constexpr auto REPEAT_ONE{u8"\ue041"};
	constexpr auto REPLAY{u8"\ue042"};
	constexpr auto REPLAY_10{u8"\ue059"};
	constexpr auto REPLAY_30{u8"\ue05a"};
	constexpr auto REPLAY_5{u8"\ue05b"};
	constexpr auto REPLY{u8"\ue15e"};
	constexpr auto REPLY_ALL{u8"\ue15f"};
	constexpr auto REPORT{u8"\ue160"};
	constexpr auto REPORT_PROBLEM{u8"\ue8b2"};
	constexpr auto RESTAURANT{u8"\ue56c"};
	constexpr auto RESTAURANT_MENU{u8"\ue561"};
	constexpr auto RESTORE{u8"\ue8b3"};
	constexpr auto RESTORE_PAGE{u8"\ue929"};
	constexpr auto RING_VOLUME{u8"\ue0d1"};
	constexpr auto ROOM{u8"\ue8b4"};
	constexpr auto ROOM_SERVICE{u8"\ueb49"};
	constexpr auto ROTATE_90_DEGREES_CCW{u8"\ue418"};
	constexpr auto ROTATE_LEFT{u8"\ue419"};
	constexpr auto ROTATE_RIGHT{u8"\ue41a"};
	constexpr auto ROUNDED_CORNER{u8"\ue920"};
	constexpr auto ROUTER{u8"\ue328"};
	constexpr auto ROWING{u8"\ue921"};
	constexpr auto RSS_FEED{u8"\ue0e5"};
	constexpr auto RV_HOOKUP{u8"\ue642"};
	constexpr auto SATELLITE{u8"\ue562"};
	constexpr auto SAVE{u8"\ue161"};
	constexpr auto SCANNER{u8"\ue329"};
	constexpr auto SCHEDULE{u8"\ue8b5"};
	constexpr auto SCHOOL{u8"\ue80c"};
	constexpr auto SCREEN_LOCK_LANDSCAPE{u8"\ue1be"};
	constexpr auto SCREEN_LOCK_PORTRAIT{u8"\ue1bf"};
	constexpr auto SCREEN_LOCK_ROTATION{u8"\ue1c0"};
	constexpr auto SCREEN_ROTATION{u8"\ue1c1"};
	constexpr auto SCREEN_SHARE{u8"\ue0e2"};
	constexpr auto SD_CARD{u8"\ue623"};
	constexpr auto SD_STORAGE{u8"\ue1c2"};
	constexpr auto SEARCH{u8"\ue8b6"};
	constexpr auto SECURITY{u8"\ue32a"};
	constexpr auto SELECT_ALL{u8"\ue162"};
	constexpr auto SEND{u8"\ue163"};
	constexpr auto SENTIMENT_DISSATISFIED{u8"\ue811"};
	constexpr auto SENTIMENT_NEUTRAL{u8"\ue812"};
	constexpr auto SENTIMENT_SATISFIED{u8"\ue813"};
	constexpr auto SENTIMENT_VERY_DISSATISFIED{u8"\ue814"};
	constexpr auto SENTIMENT_VERY_SATISFIED{u8"\ue815"};
	constexpr auto SETTINGS{u8"\ue8b8"};
	constexpr auto SETTINGS_APPLICATIONS{u8"\ue8b9"};
	constexpr auto SETTINGS_BACKUP_RESTORE{u8"\ue8ba"};
	constexpr auto SETTINGS_BLUETOOTH{u8"\ue8bb"};
	constexpr auto SETTINGS_BRIGHTNESS{u8"\ue8bd"};
	constexpr auto SETTINGS_CELL{u8"\ue8bc"};
	constexpr auto SETTINGS_ETHERNET{u8"\ue8be"};
	constexpr auto SETTINGS_INPUT_ANTENNA{u8"\ue8bf"};
	constexpr auto SETTINGS_INPUT_COMPONENT{u8"\ue8c0"};
	constexpr auto SETTINGS_INPUT_COMPOSITE{u8"\ue8c1"};
	constexpr auto SETTINGS_INPUT_HDMI{u8"\ue8c2"};
	constexpr auto SETTINGS_INPUT_SVIDEO{u8"\ue8c3"};
	constexpr auto SETTINGS_OVERSCAN{u8"\ue8c4"};
	constexpr auto SETTINGS_PHONE{u8"\ue8c5"};
	constexpr auto SETTINGS_POWER{u8"\ue8c6"};
	constexpr auto SETTINGS_REMOTE{u8"\ue8c7"};
	constexpr auto SETTINGS_SYSTEM_DAYDREAM{u8"\ue1c3"};
	constexpr auto SETTINGS_VOICE{u8"\ue8c8"};
	constexpr auto SHARE{u8"\ue80d"};
	constexpr auto SHOP{u8"\ue8c9"};
	constexpr auto SHOP_TWO{u8"\ue8ca"};
	constexpr auto SHOPPING_BASKET{u8"\ue8cb"};
	constexpr auto SHOPPING_CART{u8"\ue8cc"};
	constexpr auto SHORT_TEXT{u8"\ue261"};
	constexpr auto SHOW_CHART{u8"\ue6e1"};
	constexpr auto SHUFFLE{u8"\ue043"};
	constexpr auto SIGNAL_CELLULAR_4_BAR{u8"\ue1c8"};
	constexpr auto SIGNAL_CELLULAR_CONNECTED_NO_INTERNET_4_BAR{u8"\ue1cd"};
	constexpr auto SIGNAL_CELLULAR_NO_SIM{u8"\ue1ce"};
	constexpr auto SIGNAL_CELLULAR_NULL{u8"\ue1cf"};
	constexpr auto SIGNAL_CELLULAR_OFF{u8"\ue1d0"};
	constexpr auto SIGNAL_WIFI_4_BAR{u8"\ue1d8"};
	constexpr auto SIGNAL_WIFI_4_BAR_LOCK{u8"\ue1d9"};
	constexpr auto SIGNAL_WIFI_OFF{u8"\ue1da"};
	constexpr auto SIM_CARD{u8"\ue32b"};
	constexpr auto SIM_CARD_ALERT{u8"\ue624"};
	constexpr auto SKIP_NEXT{u8"\ue044"};
	constexpr auto SKIP_PREVIOUS{u8"\ue045"};
	constexpr auto SLIDESHOW{u8"\ue41b"};
	constexpr auto SLOW_MOTION_VIDEO{u8"\ue068"};
	constexpr auto SMARTPHONE{u8"\ue32c"};
	constexpr auto SMOKE_FREE{u8"\ueb4a"};
	constexpr auto SMOKING_ROOMS{u8"\ueb4b"};
	constexpr auto SMS{u8"\ue625"};
	constexpr auto SMS_FAILED{u8"\ue626"};
	constexpr auto SNOOZE{u8"\ue046"};
	constexpr auto SORT{u8"\ue164"};
	constexpr auto SORT_BY_ALPHA{u8"\ue053"};
	constexpr auto SPA{u8"\ueb4c"};
	constexpr auto SPACE_BAR{u8"\ue256"};
	constexpr auto SPEAKER{u8"\ue32d"};
	constexpr auto SPEAKER_GROUP{u8"\ue32e"};
	constexpr auto SPEAKER_NOTES{u8"\ue8cd"};
	constexpr auto SPEAKER_NOTES_OFF{u8"\ue92a"};
	constexpr auto SPEAKER_PHONE{u8"\ue0d2"};
	constexpr auto SPELLCHECK{u8"\ue8ce"};
	constexpr auto STAR{u8"\ue838"};
	constexpr auto STAR_BORDER{u8"\ue83a"};
	constexpr auto STAR_HALF{u8"\ue839"};
	constexpr auto STARS{u8"\ue8d0"};
	constexpr auto STAY_CURRENT_LANDSCAPE{u8"\ue0d3"};
	constexpr auto STAY_CURRENT_PORTRAIT{u8"\ue0d4"};
	constexpr auto STAY_PRIMARY_LANDSCAPE{u8"\ue0d5"};
	constexpr auto STAY_PRIMARY_PORTRAIT{u8"\ue0d6"};
	constexpr auto STOP{u8"\ue047"};
	constexpr auto STOP_SCREEN_SHARE{u8"\ue0e3"};
	constexpr auto STORAGE{u8"\ue1db"};
	constexpr auto STORE{u8"\ue8d1"};
	constexpr auto STORE_MALL_DIRECTORY{u8"\ue563"};
	constexpr auto STRAIGHTEN{u8"\ue41c"};
	constexpr auto STREETVIEW{u8"\ue56e"};
	constexpr auto STRIKETHROUGH_S{u8"\ue257"};
	constexpr auto STYLE{u8"\ue41d"};
	constexpr auto SUBDIRECTORY_ARROW_LEFT{u8"\ue5d9"};
	constexpr auto SUBDIRECTORY_ARROW_RIGHT{u8"\ue5da"};
	constexpr auto SUBJECT{u8"\ue8d2"};
	constexpr auto SUBSCRIPTIONS{u8"\ue064"};
	constexpr auto SUBTITLES{u8"\ue048"};
	constexpr auto SUBWAY{u8"\ue56f"};
	constexpr auto SUPERVISOR_ACCOUNT{u8"\ue8d3"};
	constexpr auto SURROUND_SOUND{u8"\ue049"};
	constexpr auto SWAP_CALLS{u8"\ue0d7"};
	constexpr auto SWAP_HORIZ{u8"\ue8d4"};
	constexpr auto SWAP_VERT{u8"\ue8d5"};
	constexpr auto SWAP_VERTICAL_CIRCLE{u8"\ue8d6"};
	constexpr auto SWITCH_CAMERA{u8"\ue41e"};
	constexpr auto SWITCH_VIDEO{u8"\ue41f"};
	constexpr auto SYNC{u8"\ue627"};
	constexpr auto SYNC_DISABLED{u8"\ue628"};
	constexpr auto SYNC_PROBLEM{u8"\ue629"};
	constexpr auto SYSTEM_UPDATE{u8"\ue62a"};
	constexpr auto SYSTEM_UPDATE_ALT{u8"\ue8d7"};
	constexpr auto TAB{u8"\ue8d8"};
	constexpr auto TAB_UNSELECTED{u8"\ue8d9"};
	constexpr auto TABLET{u8"\ue32f"};
	constexpr auto TABLET_ANDROID{u8"\ue330"};
	constexpr auto TABLET_MAC{u8"\ue331"};
	constexpr auto TAG_FACES{u8"\ue420"};
	constexpr auto TAP_AND_PLAY{u8"\ue62b"};
	constexpr auto TERRAIN{u8"\ue564"};
	constexpr auto TEXT_FIELDS{u8"\ue262"};
	constexpr auto TEXT_FORMAT{u8"\ue165"};
	constexpr auto TEXTSMS{u8"\ue0d8"};
	constexpr auto TEXTURE{u8"\ue421"};
	constexpr auto THEATERS{u8"\ue8da"};
	constexpr auto THUMB_DOWN{u8"\ue8db"};
	constexpr auto THUMB_UP{u8"\ue8dc"};
	constexpr auto THUMBS_UP_DOWN{u8"\ue8dd"};
	constexpr auto TIME_TO_LEAVE{u8"\ue62c"};
	constexpr auto TIMELAPSE{u8"\ue422"};
	constexpr auto TIMELINE{u8"\ue922"};
	constexpr auto TIMER{u8"\ue425"};
	constexpr auto TIMER_10{u8"\ue423"};
	constexpr auto TIMER_3{u8"\ue424"};
	constexpr auto TIMER_OFF{u8"\ue426"};
	constexpr auto TITLE{u8"\ue264"};
	constexpr auto TOC{u8"\ue8de"};
	constexpr auto TODAY{u8"\ue8df"};
	constexpr auto TOLL{u8"\ue8e0"};
	constexpr auto TONALITY{u8"\ue427"};
	constexpr auto TOUCH_APP{u8"\ue913"};
	constexpr auto TOYS{u8"\ue332"};
	constexpr auto TRACK_CHANGES{u8"\ue8e1"};
	constexpr auto TRAFFIC{u8"\ue565"};
	constexpr auto TRAIN{u8"\ue570"};
	constexpr auto TRAM{u8"\ue571"};
	constexpr auto TRANSFER_WITHIN_A_STATION{u8"\ue572"};
	constexpr auto TRANSFORM{u8"\ue428"};
	constexpr auto TRANSLATE{u8"\ue8e2"};
	constexpr auto TRENDING_DOWN{u8"\ue8e3"};
	constexpr auto TRENDING_FLAT{u8"\ue8e4"};
	constexpr auto TRENDING_UP{u8"\ue8e5"};
	constexpr auto TUNE{u8"\ue429"};
	constexpr auto TURNED_IN{u8"\ue8e6"};
	constexpr auto TURNED_IN_NOT{u8"\ue8e7"};
	constexpr auto TV{u8"\ue333"};
	constexpr auto UNARCHIVE{u8"\ue169"};
	constexpr auto UNDO{u8"\ue166"};
	constexpr auto UNFOLD_LESS{u8"\ue5d6"};
	constexpr auto UNFOLD_MORE{u8"\ue5d7"};
	constexpr auto UPDATE{u8"\ue923"};
	constexpr auto USB{u8"\ue1e0"};
	constexpr auto VERIFIED_USER{u8"\ue8e8"};
	constexpr auto VERTICAL_ALIGN_BOTTOM{u8"\ue258"};
	constexpr auto VERTICAL_ALIGN_CENTER{u8"\ue259"};
	constexpr auto VERTICAL_ALIGN_TOP{u8"\ue25a"};
	constexpr auto VIBRATION{u8"\ue62d"};
	constexpr auto VIDEO_CALL{u8"\ue070"};
	constexpr auto VIDEO_LABEL{u8"\ue071"};
	constexpr auto VIDEO_LIBRARY{u8"\ue04a"};
	constexpr auto VIDEOCAM{u8"\ue04b"};
	constexpr auto VIDEOCAM_OFF{u8"\ue04c"};
	constexpr auto VIDEOGAME_ASSET{u8"\ue338"};
	constexpr auto VIEW_AGENDA{u8"\ue8e9"};
	constexpr auto VIEW_ARRAY{u8"\ue8ea"};
	constexpr auto VIEW_CAROUSEL{u8"\ue8eb"};
	constexpr auto VIEW_COLUMN{u8"\ue8ec"};
	constexpr auto VIEW_COMFY{u8"\ue42a"};
	constexpr auto VIEW_COMPACT{u8"\ue42b"};
	constexpr auto VIEW_DAY{u8"\ue8ed"};
	constexpr auto VIEW_HEADLINE{u8"\ue8ee"};
	constexpr auto VIEW_LIST{u8"\ue8ef"};
	constexpr auto VIEW_MODULE{u8"\ue8f0"};
	constexpr auto VIEW_QUILT{u8"\ue8f1"};
	constexpr auto VIEW_STREAM{u8"\ue8f2"};
	constexpr auto VIEW_WEEK{u8"\ue8f3"};
	constexpr auto VIGNETTE{u8"\ue435"};
	constexpr auto VISIBILITY{u8"\ue8f4"};
	constexpr auto VISIBILITY_OFF{u8"\ue8f5"};
	constexpr auto VOICE_CHAT{u8"\ue62e"};
	constexpr auto VOICEMAIL{u8"\ue0d9"};
	constexpr auto VOLUME_DOWN{u8"\ue04d"};
	constexpr auto VOLUME_MUTE{u8"\ue04e"};
	constexpr auto VOLUME_OFF{u8"\ue04f"};
	constexpr auto VOLUME_UP{u8"\ue050"};
	constexpr auto VPN_KEY{u8"\ue0da"};
	constexpr auto VPN_LOCK{u8"\ue62f"};
	constexpr auto WALLPAPER{u8"\ue1bc"};
	constexpr auto WARNING{u8"\ue002"};
	constexpr auto WATCH{u8"\ue334"};
	constexpr auto WATCH_LATER{u8"\ue924"};
	constexpr auto WB_AUTO{u8"\ue42c"};
	constexpr auto WB_CLOUDY{u8"\ue42d"};
	constexpr auto WB_INCANDESCENT{u8"\ue42e"};
	constexpr auto WB_IRIDESCENT{u8"\ue436"};
	constexpr auto WB_SUNNY{u8"\ue430"};
	constexpr auto WC{u8"\ue63d"};
	constexpr auto WEB{u8"\ue051"};
	constexpr auto WEB_ASSET{u8"\ue069"};
	constexpr auto WEEKEND{u8"\ue16b"};
	constexpr auto WHATSHOT{u8"\ue80e"};
	constexpr auto WIDGETS{u8"\ue1bd"};
	constexpr auto WIFI{u8"\ue63e"};
	constexpr auto WIFI_LOCK{u8"\ue1e1"};
	constexpr auto WIFI_TETHERING{u8"\ue1e2"};
	constexpr auto WORK{u8"\ue8f9"};
	constexpr auto WRAP_TEXT{u8"\ue25b"};
	constexpr auto YOUTUBE_SEARCHED_FOR{u8"\ue8fa"};
	constexpr auto ZOOM_IN{u8"\ue8ff"};
	constexpr auto ZOOM_OUT{u8"\ue900"};
	constexpr auto ZOOM_OUT_MAP{u8"\ue56b"};
}
