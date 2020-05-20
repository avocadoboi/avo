/*
	GUI framework by Björn Sundin. 

             --
       \   --  --   /
        \ /      \ /
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
#include <cstdint>
#include <cfloat> // Range defines for float
#include <cmath>
#include <cstring>
#include <fstream>

// Data structures
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
#include <functional>

// Threading
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

// Debugging
#include <iostream>

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

//------------------------------

namespace Avo
{
	constexpr double E =       2.71828182845904523;
	constexpr double HALF_PI = 1.57079632679489661;
	constexpr double PI =      3.14159265358979323;
	constexpr double TWO_PI =  6.28318530717958647;
	constexpr double TAU =     TWO_PI;

	/*
		Returns a number multiplied by itself (x to the 2nd power, meaning x^2, meaning x*x).
		Can be useful if you want to quickly square a longer expression.
	*/
	template<typename T>
	T square(T p_x)
	{
		return p_x * p_x;
	}

	/*
		Returns the square root of a float using a fast but less accurate algorithm.
	*/
	inline float fastSqrt(float p_x)
	{
		int32 bits = (*(int32*)&p_x - (1 << 23) >> 1) + (1 << 29);
		float approximation = *(float*)&bits;
		return (p_x / approximation + approximation) * 0.5f;
	}
	/*
		Returns the inverse square root of a float using a fast but less accurate algorithm.
	*/
	inline float fastInverseSqrt(float p_x)
	{
		int32 bits = 0x5f375a86 - (*(int32*)&p_x >> 1);
		float approximation = *(float*)&bits;
		return approximation * (1.5f - 0.5f*p_x*approximation*approximation);
	}

	/*
		Returns a random double between 0 and 1 from a uniform distribution.
		It just uses the standard library random header. Convenient function.
	*/
	long double random();
	/*
		Returns a random double between 0 and 1 from a normal distribution with standard deviation 1 and mean 0.
		It just uses the standard library random header. Convenient function.
	*/
	long double randomNormal();

	/*
		Returns the biggest of two numbers.
	*/
	template<typename T>
	T max(T p_a, T p_b)
	{
		return p_a > p_b ? p_a : p_b;
	}
	/*
		Returns the biggest of three numbers.
	*/
	template<typename T>
	T max(T p_a, T p_b, T p_c)
	{
		return p_a > p_b ? (p_a > p_c ? p_a : p_c) : (p_b > p_c ? p_b : p_c);
	}

	/*
		Returns the smallest of two numbers.
	*/
	template<typename T>
	T min(T p_a, T p_b)
	{
		return p_a < p_b ? p_a : p_b;
	}
	/*
		Returns the smallest of three numbers.
	*/
	template<typename T>
	T min(T p_a, T p_b, T p_c)
	{
		return p_a < p_b ? (p_a < p_c ? p_a : p_c) : (p_b < p_c ? p_b : p_c);
	}

	//------------------------------

	/*
		Returns a value between p_start and p_end depending on p_progress. This is linear interpolation.
		If p_progress is below 0 or above 1, the returned value will not be within the start and end position.
	*/
	inline float interpolate(float p_start, float p_end, float p_progress)
	{
		return p_start * (1.f - p_progress) + p_end * p_progress;
	}
	/*
		Returns a value between p_start and p_end depending on p_progress. This is linear interpolation.
		If p_progress is below 0 or above 1, the returned value will not be within the start and end position.
	*/
	inline double interpolate(double p_start, double p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * p_progress;
	}
	/*
		Clips p_value so that the returned value is never below p_min or above p_max.
		If p_min <= p_value <= p_max, then the returned value is equal to p_value.
	*/
	template<typename T>
	T constrain(T p_value, T p_min = 0, T p_max = 1)
	{
		return p_value < p_min ? p_min : (p_value > p_max ? p_max : p_value);
	}

	//------------------------------

	/*
		Returns -1 if the number is negative, 0 if it's 0 and 1 if it's positive.
	*/
	template<typename T>
	T sign(T p_number)
	{
		return (p_number > (T)0) - (p_number < (T)0);
	}

	//------------------------------

	/*
		Removes an element from a vector. The function returns true if the element existed in the vector and was removed.
	*/
	template<typename T>
	bool removeVectorElementWhileKeepingOrder(std::vector<T>& p_vector, T p_value)
	{
		for (auto element = p_vector.begin(); element != p_vector.end(); element++)
		{
			if (*element == p_value)
			{
				p_vector.erase(element);
				return true;
			}
		}
		return false;
	}

	/*
		Removes an element from a vector without keeping the order of the elements in the vector, making it more efficient.
		The function returns true if the element existed in the vector and was removed (replaced by the last element).
	*/
	template<typename T>
	bool removeVectorElementWithoutKeepingOrder(std::vector<T>& p_vector, T p_element)
	{
		for (auto element = p_vector.begin(); element != p_vector.end(); element++)
		{
			if (*element == p_element)
			{
				*element = p_vector.back();
				p_vector.pop_back();
				return true;
			}
		}
		return false;
	}

	//------------------------------

	/*
		Converts a UTF-8 encoded char string to a UTF-16 encoded wchar_t string.
		It is assumed that p_input is null-terminated.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator.
	*/
	void convertUtf8ToUtf16(char const* p_input, wchar_t* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-8 encoded string to a UTF-16 encoded wchar_t string.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator.
	*/
	void convertUtf8ToUtf16(std::string const& p_input, wchar_t* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-8 encoded char string to a UTF-16 encoded wchar_t string.
		p_numberOfUnitsInInput is the size in bytes of p_input.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator if the input includes the null terminator.
	*/
	void convertUtf8ToUtf16(char const* p_input, uint32 p_numberOfUnitsInInput, wchar_t* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-8 encoded std::string to a UTF-16 encoded std::wstring.
	*/
	std::wstring convertUtf8ToUtf16(std::string const& p_input);
	/*
		Converts a UTF-8 encoded null-terminated string to a UTF-16 encoded std::wstring.
	*/
	std::wstring convertUtf8ToUtf16(char const* p_input);
	/*
		Converts a UTF-8 encoded null-terminated string to a UTF-16 encoded std::wstring.
		p_numberOfUnitsInInput is the size in bytes of p_input.
	*/
	std::wstring convertUtf8ToUtf16(char const* p_input, uint32 p_numberOfUnitsInInput);
	/*
		Returns the number of UTF-16 encoded wchar_t units that would be used to represent the same characters in a UTF-8 encoded char string.
		It is assumed that p_input is null-terminated.
		The output includes the null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(char const* p_input);
	/*
		Returns the number of UTF-16 encoded wchar_t units that would be used to represent the same characters in a UTF-8 encoded char string.
		if p_numberOfUnitsInInput is the size of p_input in bytes.
		The output includes the null terminator if the input includes a null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(char const* p_input, int32 p_numberOfUnitsInInput);
	/*
		Returns the number of UTF-16 encoded wchar_t units that would be used to represent the same characters in a UTF-8 encoded char string.
		The output includes the null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(std::string const& p_input);

	/*
		Converts a UTF-16 encoded wchar_t string to a UTF-8 encoded char string.
		It is assumed that p_input is null-terminated.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator.
	*/
	void convertUtf16ToUtf8(wchar_t const* p_input, char* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-16 encoded wstring to a UTF-8 encoded char string.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator.
	*/
	void convertUtf16ToUtf8(std::wstring const& p_input, char* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-16 encoded wchar_t string to a UTF-8 encoded char string.
		p_numberOfUnitsInInput is the size of p_input, in wchar_t units.
		p_output should be allocated with p_numberOfUnitsInOutput number of wchar_t units.
		The output includes the null terminator.
	*/
	void convertUtf16ToUtf8(wchar_t const* p_input, uint32 p_numberOfUnitsInInput, char* p_output, uint32 p_numberOfUnitsInOutput);
	/*
		Converts a UTF-16 std::wstring to a UTF-8 encoded std::string.
	*/
	std::string convertUtf16ToUtf8(std::wstring const& p_input);
	/*
		Converts a UTF-16 encoded null-terminated string to a UTF-8 encoded std::string.
	*/
	std::string convertUtf16ToUtf8(wchar_t const* p_input);
	/*
		Converts a UTF-16 encoded null-terminated string to a UTF-8 encoded std::string.
		p_numberOfUnitsInInput is the size of p_input in wchar_t units.
	*/
	std::string convertUtf16ToUtf8(wchar_t const* p_input, uint32 p_numberOfUnitsInInput);
	/*
		Returns the number of UTF-8 encoded char units that would be used to represent the same characters in a UTF-16 encoded wchar_t string.
		It is assumed that p_input is null terminated.
		The output includes the null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input);
	/*
		Returns the number of UTF-8 encoded char units that would be used to represent the same characters in a UTF-16 encoded wchar_t string.
		p_numberOfUnitsInInput is the size of p_input in wchar_t units.
		The output includes the null terminator if the input includes a null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(wchar_t const* p_input, uint32 p_numberOfUnitsInInput);
	/*
		Returns the number of UTF-8 encoded char units that would be used to represent the same characters in a UTF-16 encoded wchar_t string.
		The output includes the null terminator.
	*/
	uint32 getNumberOfUnitsInUtfConvertedString(std::wstring const& p_input);

	//------------------------------

	/*
		If p_startByte is the first byte in a utf-8 encoded character, the function returns the number of bytes that the character is made up of, which can be 1-4.
		If p_startByte is not the first byte in the character, the function returns 0.
		If p_startByte is an invalid UTF-8 byte, -1 is returned.
	*/
	inline int8 getNumberOfUnitsInUtf8Character(int8 p_startByte)
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
		Returns whether p_byte is the start of a UTF-8 encoded character
	*/
	inline bool getIsUnitStartOfUtf8Character(int8 p_byte)
	{
		return (p_byte & 0xc0) != 0x80;
	}
	/*
		If p_startUnit is the first unit in a UTF-16 encoded character, the function returns the number of units that the character is made up of, which can only be 1 or 2.
		If p_startUnit is not the first unit in the character, the function returns 0.
	*/
	inline int8 getNumberOfUnitsInUtf16Character(int16 p_startUnit)
	{
		// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 125
		if ((p_startUnit & 0xfc00) == 0xd800) // 110110wwwwxxxxxx
			return 2;
		if ((p_startUnit & 0xfc00) == 0xdc00) // 110111xxxxxxxxxx
			return 0;
		return 1; // xxxxxxxxxxxxxxxx
	}
	/*
		Returns whether p_unit is the start of a UTF-8 encoded character
	*/
	inline bool getIsUnitStartOfUtf16Character(int8 p_unit)
	{
		return (p_unit & 0xfc00) != 0xdc00;
	}
	/*
		Returns the index of the byte at a certain character index in a UTF-8 encoded string (where a character can be 1-4 bytes).
		If p_characterIndex is outside of the string, the size of the string is returned.
	*/
	inline int32 getUtf8UnitIndexFromCharacterIndex(std::string const& p_string, uint32 p_characterIndex)
	{
		if (!p_characterIndex)
		{
			return 0;
		}
		if (p_characterIndex >= p_string.size())
		{
			return p_string.size();
		}

		uint32 numberOfCharactersCounted = 0;
		for (uint32 a = 0; a < p_string.size(); a++)
		{
			// If the byte is at the start of a new character, meaning if the byte doesn't start with (bits) 10.
			if ((p_string[a] & 0xc0) != 0x80)
			{
				if (numberOfCharactersCounted == p_characterIndex)
				{
					return a;
				}
				numberOfCharactersCounted++;
			}
		}
		return p_string.size();
	}
	/*
		Returns the index of the character that the byte at p_unitIndex in the UTF-8 encoded p_string belongs to (where a character can be 1-4 bytes).
		If p_unitIndex is outside of the string, the number of characters in the string is returned.
	*/
	inline int32 getCharacterIndexFromUtf8UnitIndex(std::string const& p_string, uint32 p_unitIndex)
	{
		if (!p_unitIndex)
		{
			return 0;
		}

		int32 numberOfCharactersCounted = 0;
		for (uint32 a = 0; a < p_string.size(); a++)
		{
			// If the byte is at the start of a new character, meaning if the byte doesn't start with (bits) 10.
			if ((p_string[a] & 0xc0) != 0x80)
			{
				if (p_unitIndex < a)
				{
					return numberOfCharactersCounted - 1;
				}
				numberOfCharactersCounted++;
			}
		}
		if (p_unitIndex < p_string.size())
		{
			return numberOfCharactersCounted - 1;
		}
		return numberOfCharactersCounted;
	}
	inline int32 getNumberOfCharactersInUtf8String(std::string const& p_string)
	{
		return getCharacterIndexFromUtf8UnitIndex(p_string, p_string.size());
	}
	/*
		Returns the index of the unit at a certain character index in a UTF-8 encoded string (where a character can be 1-2 units).
		If p_characterIndex is outside of the string, the size of the string in code units is returned.
	*/
	inline int32 getUtf16UnitIndexFromCharacterIndex(std::wstring const& p_string, uint32 p_characterIndex)
	{
		if (!p_characterIndex)
		{
			return 0;
		}
		if (p_characterIndex >= p_string.size())
		{
			return p_string.size();
		}

		uint32 numberOfCharactersCounted = 0;
		for (uint32 a = 0; a < p_string.size(); a++)
		{
			// If the unit is at the start of a new character, meaning if the unit doesn't start with (bits) 110111.
			if ((p_string[a] & 0xfc00) != 0xdc00)
			{
				if (numberOfCharactersCounted == p_characterIndex)
				{
					return a;
				}
				numberOfCharactersCounted++;
			}
		}
		return p_string.size();
	}
	/*
		Returns the index of the character that the code unit at p_unitIndex in the utf-16 encoded p_string belongs to (where a character can be 1-2 units).
		If p_unitIndex is outside of the string, the number of characters in the string is returned.
	*/
	inline int32 getCharacterIndexFromUtf16UnitIndex(std::wstring const& p_string, uint32 p_unitIndex)
	{
		if (!p_unitIndex)
		{
			return 0;
		}

		int32 numberOfCharactersCounted = 0;
		for (uint32 a = 0; a < p_string.size(); a++)
		{
			// If the byte is at the start of a new character, meaning if the byte doesn't start with (bits) 10.
			if ((p_string[a] & 0xfc00) != 0xdc00)
			{
				if (p_unitIndex < a)
				{
					return numberOfCharactersCounted - 1;
				}
				numberOfCharactersCounted++;
			}
		}
		if (p_unitIndex < p_string.size())
		{
			return numberOfCharactersCounted - 1;
		}
		return numberOfCharactersCounted;
	}
	inline int32 getNumberOfCharactersInUtf16String(std::wstring const& p_string)
	{
		return getCharacterIndexFromUtf16UnitIndex(p_string, p_string.size());
	}

	//------------------------------

	enum class RoundingType
	{
		Down,
		Up,
		Nearest
	};

	/*
		Converts a number to a string, using . (dot) for the decimal point.
	*/
	template<typename T>
	std::string convertNumberToString(T p_value)
	{
		std::ostringstream stream;
		stream.precision(10);
		stream << p_value;
		return stream.str();
	}

	/*
		Converts a number rounded at a certain digit to a string, using . (dot) for the decimal point.
		If p_numberOfDigitsToRound is 0, only all decimals are rounded off and it becomes an integer.
		Positive goes to the right and negative goes to the left.
	*/
	template<typename T>
	std::string convertNumberToString(T p_value, int32 p_roundingIndex, RoundingType p_roundingType = RoundingType::Nearest)
	{
		double roundingFactor = std::pow(10., p_roundingIndex);
		std::ostringstream stream;
		stream.precision(10);
		if (p_roundingType == RoundingType::Nearest)
		{
			stream << std::round(p_value*roundingFactor)/roundingFactor;
		}
		else if (p_roundingType == RoundingType::Down)
		{
			stream << std::floor(p_value * roundingFactor) / roundingFactor;
		}
		else
		{
			stream << std::ceil(p_value * roundingFactor) / roundingFactor;
		}
		return stream.str();
	}

	/*
		Simple function to format a string by replacing placeholders in p_format with p_objects.
		p_objects can be any objects which have a std::ostring << operator defined.

		The placeholders are in the form of {index} where index is the index of the argument to be inserted.
		Writing {0} will insert the first item, {1} will insert the second item.
		Only values of [0, 9] are allowed as the indicies, meaning max 10 objects can be inserted in one call.

		Example:
		std::string formattedString = Avo::createFormattedString(
			"I have {1} {2} and {3} {4} and {5} {6}. Pi: {0}",
			Avo::PI, 2, "cats", 0, "dogs", 10, "fingers"
		);
	*/
	template<typename ... FormattableType>
	inline std::string createFormattedString(std::string p_format, FormattableType&& ... p_objects)
	{
		// c++ is amazing
		std::vector<std::ostringstream> stringifiedObjects(sizeof...(p_objects));
		uint32 objectIndex = 0u; 
		((stringifiedObjects[objectIndex++] << p_objects), ...);

		std::ostringstream stream;
		stream.precision(10);

		uint32 lastPlaceholderEndIndex = 0;
		for (uint32 a = 0u; a < p_format.size() - 2; a++)
		{
			// Utf-8 is backwards-compatible with ASCII so this should work fine.
			if (p_format[a] == '{' && p_format[++a] >= '0' && p_format[a] <= '9' && p_format[a + 1] == '}')
			{
				stream.write(p_format.data() + lastPlaceholderEndIndex, a - 1 - lastPlaceholderEndIndex);
				stream << stringifiedObjects[p_format[a] - '0'].str();
				lastPlaceholderEndIndex = a += 2;
			}
		}
		stream.write(p_format.data() + lastPlaceholderEndIndex, p_format.size() - lastPlaceholderEndIndex);

		return stream.str();
	}

	//------------------------------

	inline std::vector<uint8> readFile(std::string const& p_path)
	{
		std::ifstream file{ p_path, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			return std::vector<uint8>();
		}

		std::vector<uint8> result(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read((char*)result.data(), result.size());

		return result;
	}

	//------------------------------

	/*
		Represents an ID.
		To generate a new unique ID, use the default constructor like this:
			Id id;
		To create an ID with a specific value, just assign:
			ID id = 1234;
		An ID which converts to 0 is considered invalid, and can be created like this:
			Id id = 0;
	*/
	class Id
	{
	private:
		static uint64 s_counter;
		uint64 m_count;

	public:
		operator uint64() const
		{
			return m_count;
		}
		bool operator==(Id const& p_id) const
		{
			return (uint64)p_id == m_count;
		}

		Id(uint64 p_id)
		{
			m_count = p_id;
		}
		Id(Id const& p_id)
		{
			m_count = (uint64)p_id;
		}
		Id()
		{
			m_count = ++s_counter;
		}
	};

	//------------------------------

	/*
		Binds an object to a member method of its class, so that the returned function can be called without providing the instance.
	*/
	template<typename ReturnType, typename Class, typename ... Arguments>
	std::function<ReturnType(Arguments...)> bind(ReturnType(Class::* p_function)(Arguments...), Class* p_instance)
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
		std::mutex m_mutex;
		std::vector<std::function<FunctionalType>> m_listeners;

	public:
		void add(std::function<FunctionalType> const& p_listener)
		{
			std::scoped_lock lock{ m_mutex };
			m_listeners.push_back(p_listener);
		}
		EventListeners& operator+=(std::function<FunctionalType> const& p_listener)
		{
			add(p_listener);
			return *this;
		}

		void remove(std::function<FunctionalType> const& p_listener)
		{
			std::scoped_lock lock{ m_mutex };
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
		EventListeners& operator-=(std::function<FunctionalType> const& p_listener)
		{
			remove(p_listener);
			return *this;
		}

		void clear()
		{
			std::scoped_lock lock{ m_mutex };
			m_listeners.clear();
		}

		/*
			Calls all of the listeners with p_eventArguments as the arguments.
		*/
		template<typename ... T>
		void notifyAll(T&& ... p_eventArguments)
		{
			std::scoped_lock lock{ m_mutex };
			auto listenersCopy = m_listeners;
			for (auto listener : listenersCopy)
			{
				listener(std::forward<T>(p_eventArguments)...);
			}
		}
		template<typename ... T>
		void operator()(T&& ... p_eventArguments)
		{
			notifyAll(std::forward<T>(p_eventArguments)...);
		}
	};

	//------------------------------

	/*
		A TimerThread is used for timer callbacks. 
		The first time a callback is added, it spawns a thread that sleeps until the next callback should be called.
	*/
	class TimerThread
	{
		class Timeout
		{
			std::function<void()> callback;
			float duration;
		public:
			std::chrono::steady_clock::time_point endTime;
			Id id = 0;
			
			Timeout(std::function<void()> const& p_callback, float p_duration, Id p_id) :
				callback{ p_callback },
				duration{ p_duration },
				endTime{ std::chrono::steady_clock::now() + std::chrono::steady_clock::duration{ int64(p_duration) * 1'000'000ll } },
				id{ p_id }
			{
			}
			Timeout(Timeout const& p_other) = default;
			Timeout(Timeout&& p_other)
			{
				operator=(std::move(p_other));
			}

			void operator=(Timeout const& p_other)
			{
				callback = p_other.callback;
				duration = p_other.duration;
				endTime = p_other.endTime;
				id = p_other.id;
			}
			void operator=(Timeout&& p_other)
			{
				callback = std::move(p_other.callback);
				duration = p_other.duration;
				endTime = std::move(p_other.endTime);
				id = p_other.id;
			}

			void operator()() const
			{
				callback();
			}
		};

		std::atomic<uint64> m_idCounter = 1u;
		std::vector<Timeout> m_timeouts;
		std::mutex m_timeoutsMutex;
		std::_Mutex_base* m_callbackMutex = nullptr;

		std::atomic<bool> m_needsToWake = false;
		std::condition_variable m_wakeConditionVariable;
		std::mutex m_wakeMutex;

		std::atomic<bool> m_isRunning = false;
		std::thread m_thread;
		void thread_run()
		{
			m_isRunning = true;
			while (m_isRunning)
			{
				if (m_timeouts.empty())
				{
					m_idCounter = 0;
					if (!m_needsToWake)
					{
						std::unique_lock<std::mutex> lock{ m_wakeMutex };
						m_wakeConditionVariable.wait(lock, [&] { return (bool)m_needsToWake; });
					}
					m_needsToWake = false;
				}
				else
				{
					if (!m_needsToWake)
					{
						std::unique_lock<std::mutex> lock{ m_wakeMutex };
						m_wakeConditionVariable.wait_until(lock, m_timeouts.begin()->endTime, [&] { return (bool)m_needsToWake; });
					}
					m_needsToWake = false;

					std::scoped_lock lock{ m_timeoutsMutex };

					if (m_timeouts.empty())
					{
						continue;
					}
					auto timeout = m_timeouts.begin();
					while (timeout->endTime < std::chrono::steady_clock::now())
					{
						m_callbackMutex->lock();
						(*timeout)();
						m_callbackMutex->unlock();

						timeout++;
						if (timeout == m_timeouts.end())
						{
							break;
						}
					}
					m_timeouts.erase(m_timeouts.begin(), timeout);
				}
			}
		}

		void wake()
		{
			if (!m_needsToWake)
			{
				m_wakeMutex.lock();
				m_needsToWake = true;
				m_wakeMutex.unlock();
				m_wakeConditionVariable.notify_one();
			}
		}

		void run()
		{
			m_thread = std::thread{ &TimerThread::thread_run, this };
		}

	public:
		TimerThread() = default;
		/*
			p_callbackMutex is a mutex that is locked every time a timer callback is called.
		*/
		TimerThread(std::_Mutex_base& p_callbackMutex) :
			m_callbackMutex(&p_callbackMutex)
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

		/*
			Adds a function that will be called in p_milliseconds milliseconds from now.
		*/
		Id addCallback(std::function<void()>& p_callback, float p_milliseconds)
		{
			if (!m_isRunning)
			{
				run();
			}

			Timeout timeout{ p_callback, p_milliseconds, m_idCounter++ };
			{
				std::scoped_lock lock{ m_timeoutsMutex };
				auto position = std::lower_bound(m_timeouts.begin(), m_timeouts.end(), timeout, [](Timeout const& p_a, Timeout const& p_b) { return p_a.endTime < p_b.endTime; });
				m_timeouts.insert(position, timeout);
			}
			wake();
			return timeout.id;
		}
		void cancelCallback(Id const& p_id)
		{
			std::scoped_lock lock{ m_timeoutsMutex };
			auto position = std::find_if(m_timeouts.begin(), m_timeouts.end(), [=](Timeout const& p_timeout) { return p_timeout.id == p_id; });
			if (position != m_timeouts.end())
			{
				m_timeouts.erase(position);
			}
		}
	};

	//------------------------------

	/*
		This is very useful when storing pointers to dynamically allocated objects in multiple places.
		The object doesn't get deleted until every remember() has a forget().
		The constructor is the first remember(), meaning m_referenceCount is initialized with 1.
		Don't use the delete operator with objects that are ReferenceCounted, use forget() instead.
	*/
	class ReferenceCounted
	{
	private:
		std::atomic<uint32> m_referenceCount;

	public:
		ReferenceCounted() : m_referenceCount(1U) { }
		virtual ~ReferenceCounted() = default;

		/*
			Increments the reference count and returns the new reference count. Remembers a pointer reference.
		*/
		uint32 remember()
		{
			return ++m_referenceCount;
		}

		/*
			Decrements the reference count, returns the new reference count and deletes the object if the reference
			count has reached 0. Forgets a pointer reference.
		*/
		uint32 forget()
		{
			m_referenceCount--;
			if (!m_referenceCount)
			{
				delete this;
				return 0;
			}
			return m_referenceCount;
		}

		/*
			Returns the number of pointer references to the dynamically allocated object that have been remembered.
		*/
		uint32 getReferenceCount()
		{
			return m_referenceCount;
		}
	};

	/*
		Only used in the framework.
	*/
	template<typename T>
	class ProtectedReferenceCounted : protected ReferenceCounted
	{
	protected:
		T* m_implementation;

		ProtectedReferenceCounted(T* p_implementation) :
			m_implementation{ p_implementation }
		{
		}

	public:
		ProtectedReferenceCounted() :
			m_implementation{ nullptr }
		{
		}
		ProtectedReferenceCounted(T const& p_other) :
			m_implementation{ p_other.m_implementation }
		{
			if (m_implementation)
			{
				m_implementation->remember();
			}
		}
		~ProtectedReferenceCounted()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
		}

		virtual ProtectedReferenceCounted<T>& operator=(ProtectedReferenceCounted<T> const& p_other)
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
			m_implementation = p_other.m_implementation;
			if (m_implementation)
			{
				m_implementation->remember();
			}
			return *this;
		}

		bool operator==(T const& p_other)
		{
			return m_implementation == p_other.m_implementation;
		}
		bool operator!=(ProtectedReferenceCounted<T> const& p_other)
		{
			return m_implementation != p_other.m_implementation;
		}

		bool getIsValid()
		{
			return m_implementation;
		}
		operator bool()
		{
			return m_implementation;
		}

		void destroy()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
		}
	};

	//------------------------------

	/*
		A component is an essential building block in an application.
		An application consists of a hierarchy of components.
		These components can be Views, however they don't need to be.
		Every component has its own responsibility, and using non-view components as well as view 
		components can help separate the concerns of an application.
		
		All components which have a parent require being dynamically allocated with the new keyword,
		and their memory is automatically managed by their parent so there is no need to call forget.
		The root component, however, doesn't have a parent and could be allocated on the stack.

		See Avo::View and Avo::Gui for more information.
	*/
	class Component : public ReferenceCounted
	{
	private:
		Component* m_root;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto getRoot() const
		{
			return m_root;
		}

	private:
		Component* m_parent = nullptr;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		void setParent(Component* p_parent)
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
		auto getParent() const
		{
			return m_parent;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		template<typename T>
		T* getParent() const
		{
			return (T*)m_parent;
		}

	private:
		std::vector<Component*> m_children;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		auto const& getChildren() const
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
		T* getChild(uint32 p_index) const
		{
			return (T*)m_children[p_index];
		}

		/*
			LIBRARY IMPLEMENTED
		*/
		void addChild(Component* p_child)
		{
			p_child->setParent(this);
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void removeChild(uint32 p_index)
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
		void removeChild(Component* p_child)
		{
			auto position = std::find(m_children.begin(), m_children.end(), p_child);
			if (position != m_children.end())
			{
				removeChild(position - m_children.begin());
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void removeAllChildren()
		{
			while (!m_children.empty())
			{
				auto child = m_children.back();
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
		virtual void handleChildAttachment(Component* p_attachedChild) { }

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
		virtual void handleChildDetachment(Component* p_detachedChild) { }

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
		virtual void handleParentChange(Component* p_oldParent) { }

	private:
		std::unordered_map<uint64, Component*> m_componentsById;
		Component* m_idScope = nullptr;
		Id m_id = 0;
	public:
		/*
			Sets an ID that can be used to retrieve the component from the hierarchy.
			If p_id is 0, it is only removed from the scope.
			p_scope is the component that manages the ID of this component and is the topmost component 
			from which the ID of this component can be retrieved.
		*/
		void setId(Id const& p_id, Component* p_scope)
		{
			if (m_idScope)
			{
				auto componentIterator = m_idScope->m_componentsById.find(m_id);
				if (componentIterator != m_idScope->m_componentsById.end())
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
		void setId(Id const& p_id)
		{
			setId(p_id, getRoot());
		}
		/*
			Returns the ID that can be used to retrieve the component from the component hierarchy.
			The ID is invalid by default and converts to 0.
		*/
		Id getId() const
		{
			return m_id;
		}

		Component* getComponentById(Id const& p_id)
		{
			Component* parent = this;
			while (parent)
			{
				auto componentIterator = parent->m_componentsById.find(p_id);
				if (componentIterator == parent->m_componentsById.end())
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
		T* getComponentById(Id const& p_id)
		{
			return (T*)getComponentById(p_id);
		}

	public:
		Component() :
			m_root{ this }
		{
		}
		Component(Component* p_parent) :
			m_root{ p_parent ? p_parent->getRoot() : this }
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

	/*
		A 2D point/vector where x is the horizontal component and y is the vertical component if you were to think of it graphically.
		The coordinate system used throughout AvoGUI is one where the positive y-direction is downwards and the positive x-direction is to the right.
	*/
	template<typename PointType = float>
	class Point
	{
	public:
		PointType x, y;

		Point()
		{
			x = y = 0;
		}
		Point(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
		}
		/*
			Initializes the point with the same x and y coordinates.
		*/
		Point(PointType p_coordinate)
		{
			x = p_coordinate;
			y = p_coordinate;
		}
		template<typename T>
		Point(Point<T> const& p_point)
		{
			x = p_point.x;
			y = p_point.y;
		}
		template<typename T>
		Point(Point<T>&& p_point)
		{
			x = p_point.x;
			y = p_point.y;
		}

		//------------------------------

		Point<PointType> operator-() const
		{
			return { -x, -y };
		}

		//------------------------------

		template<typename T>
		Point<PointType>& operator=(Point<T> const& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}
		template<typename T>
		Point<PointType>& operator=(Point<T>&& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}

		/*
			Sets the same value for the x and the y coordinates.
		*/
		Point<PointType>& operator=(PointType p_coordinate)
		{
			x = y = p_coordinate;
			return *this;
		}

		//------------------------------

		Point<PointType>& set(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
			return *this;
		}
		/*
			Sets the polar coordinates of the point.
			p_angle is the angle in radians between the ray to the point and the x-axis on the right-hand side, clockwise in our coordinate system.
			p_length is the distance from the origin of the coordinates.
		*/
		Point<PointType>& setPolar(double p_angle, double p_length)
		{
			x = std::cos(p_angle) * p_length;
			y = std::sin(p_angle) * p_length;
			return *this;
		}
		/*
			Sets the polar coordinates of the point, with length of 1.
			p_angle is the angle in radians between the ray to the point and the x-axis on the right-hand side, clockwise in our coordinate system.
		*/
		Point<PointType>& setPolar(double p_angle)
		{
			x = std::cos(p_angle);
			y = std::sin(p_angle);
			return *this;
		}

		//------------------------------

		bool operator==(Point<PointType> const& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		bool operator==(Point<PointType>&& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		bool operator==(PointType p_coordinate) const
		{
			return x == p_coordinate && y == p_coordinate;
		}

		//------------------------------

		bool operator!=(Point<PointType> const& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		bool operator!=(Point<PointType>&& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		bool operator!=(PointType p_coordinate) const
		{
			return x != p_coordinate || y != p_coordinate;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator+(Point<T> const& p_point) const
		{
			return { PointType(x + p_point.x), PointType(y + p_point.y) };
		}
		template<typename T>
		Point<PointType> operator+(Point<T>&& p_point) const
		{
			return { PointType(x + p_point.x), PointType(y + p_point.y) };
		}
		/*
			Returns a version of this point that is offset by an equal amount on the x- and y-axis.
		*/
		Point<PointType> operator+(PointType p_offset) const
		{
			return { PointType(x + p_offset), PointType(y + p_offset) };
		}
		Point<PointType> createAddedCopy(PointType p_x, PointType p_y) const
		{
			return { PointType(x + p_x), PointType(y + p_y) };
		}

		template<typename T>
		Point<PointType>& operator+=(Point<T> const& p_offset)
		{
			x += p_offset.x;
			y += p_offset.y;
			return *this;
		}
		Point<PointType>& operator+=(PointType p_offset)
		{
			x += p_offset;
			y += p_offset;
			return *this;
		}
		Point<PointType>& move(PointType p_x, PointType p_y)
		{
			x += p_x;
			y += p_y;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator-(Point<T> const& p_point) const
		{
			return { PointType(x - p_point.x), PointType(y - p_point.y) };
		}
		template<typename T>
		Point<PointType> operator-(Point<T>&& p_point) const
		{
			return { PointType(x - p_point.x), PointType(y - p_point.y) };
		}
		/*
			Returns a version of this point that is offset negatively by the same amount on the x- and y-axis.
		*/
		Point<PointType> operator-(PointType p_offset) const
		{
			return { PointType(x - p_offset), PointType(y - p_offset) };
		}
		Point<PointType> createSubtractedCopy(PointType p_x, PointType p_y) const
		{
			return { PointType(x - p_x), PointType(y - p_y) };
		}

		template<typename T>
		Point<PointType>& operator-=(Point<T> const& p_offset)
		{
			x -= p_offset.x;
			y -= p_offset.y;
			return *this;
		}
		Point<PointType>& operator-=(PointType p_offset)
		{
			x -= p_offset;
			y -= p_offset;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator*(Point<T> const& p_point) const
		{
			return { PointType(x * p_point.x), PointType(y * p_point.y) };
		}
		template<typename T>
		Point<PointType> operator*(Point<T>&& p_point) const
		{
			return { PointType(x * p_point.x), PointType(y * p_point.y) };
		}
		template<typename T>
		Point<PointType> operator*(T p_factor) const
		{
			return { PointType(x * p_factor), PointType(y * p_factor) };
		}
		template<typename T0, typename T1>
		Point<PointType> createMultipliedCopy(T0 p_x, T1 p_y) const
		{
			return { PointType(x * p_x), PointType(y * p_y) };
		}

		template<typename T>
		Point<PointType>& operator*=(Point<T> const& p_point)
		{
			x *= p_point.x;
			y *= p_point.y;
			return *this;
		}
		template<typename T>
		Point<PointType>& operator*=(T p_factor)
		{
			x *= p_factor;
			y *= p_factor;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator/(Point<T> const& p_point) const
		{
			return { PointType(x / p_point.x), PointType(y / p_point.y)};
		}
		template<typename T>
		Point<PointType> operator/(T p_divisor) const
		{
			return { PointType(x / p_divisor), PointType(y / p_divisor) };
		}
		template<typename T0, typename T1>
		Point<PointType> createDividedCopy(T0 p_x, T1 p_y) const
		{
			return { PointType(x / p_x), PointType(y / p_y) };
		}

		template<typename T>
		Point<PointType>& operator/=(Point<T> const& p_point)
		{
			x /= p_point.x;
			y /= p_point.y;
			return *this;
		}
		template<typename T>
		Point<PointType>& operator/=(T p_divisor)
		{
			x /= p_divisor;
			y /= p_divisor;
			return *this;
		}

		//------------------------------

		template<typename T>
		double getDotProduct(Point<T> const& p_point) const
		{
			return x * p_point.x + y * p_point.y;
		}
		template<typename T0, typename T1>
		double getDotProduct(T0 p_x, T1 p_y) const
		{
			return x * p_x + y * p_y;
		}

		template<typename T>
		double getCrossProduct(Point<T> const& p_point) const
		{
			return x * p_point.x - y * p_point.x;
		}
		template<typename T0, typename T1>
		double getCrossProduct(T0 p_x, T1 p_y) const
		{
			return x * p_y - y * p_x;
		}

		//------------------------------

		/*
			Calculates the length of the 2d vector with pythagorean theorem.
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		PointType getLengthSquared() const
		{
			return x * x + y * y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of the 2d vector with pythagorean teorem.
		*/
		double getLength() const
		{
			return sqrt(x*x + y * y);
		}
		/*
			Uses a fast but less accurate algorithm to calculate the length of the 2d vector with pythagorean teorem.
		*/
		double getLengthFast() const
		{
			return fastSqrt(x*x + y * y);
		}

		/*
			Calculates the distance between this point and another point with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T>
		double getDistanceSquared(Point<T> const& p_point)
		{
			return (x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y);
		}
		/*
			Calculates the distance between this point and another point with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1>
		double getDistanceSquared(T0 p_x, T1 p_y)
		{
			return (x - p_x)*(x - p_x) + (y - p_y)*(y - p_y);
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T>
		double getDistance(Point<T> const& p_point)
		{
			return sqrt((x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T0, typename T1>
		double getDistance(T0 p_x, T1 p_y)
		{
			return sqrt((x - p_x)*(x - p_x) + (y - p_y)*(y - p_y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T>
		double getDistanceFast(Point<T> const& p_point)
		{
			return sqrtFast((x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T0, typename T1>
		double getDistanceFast(T0 p_x, T1 p_y)
		{
			return sqrtFast((x - p_x)*(x - p_x) + (y - p_y)*(y - p_y));
		}

		//------------------------------

		/*
			Calculates the length of a 2d vector with pythagorean theorem.
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1>
		static double getLengthSquared(T0 p_x, T1 p_y)
		{
			return p_x * p_x + p_y * p_y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of a 2d vector with pythagorean teorem.
		*/
		template<typename T0, typename T1>
		static double getLength(T0 p_x, T1 p_y)
		{
			return sqrt(p_x * p_x + p_y * p_y);
		}
		/*
			Uses a fast but less accurate algorithm to calculate the length of a 2d vector with pythagorean teorem.
		*/
		template<typename T0, typename T1>
		static double getLengthFast(T0 p_x, T1 p_y)
		{
			return fastSqrt(p_x * p_x + p_y * p_y);
		}

		/*
			Calculates the distance between two points with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1>
		static double getDistanceSquared(Point<T0> const& p_point_0, Point<T1> const& p_point_1)
		{
			return (p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y);
		}
		/*
			Calculates the distance between two points with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T0, typename T1, typename T2, typename T3>
		static double getDistanceSquared(T0 p_x0, T1 p_y0, T2 p_x1, T3 p_y1)
		{
			return double((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1>
		static double getDistance(Point<T0> const& p_point_0, Point<T1> const& p_point_1)
		{
			return sqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1, typename T2, typename T3>
		static double getDistance(T0 p_x0, T1 p_y0, T2 p_x1, T3 p_y1)
		{
			return sqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1>
		static float getDistanceFast(Point<T0> const& p_point_0, Point<T1> const& p_point_1)
		{
			return fastSqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1, typename T2, typename T3>
		static float getDistanceFast(T0 p_x0, T1 p_y0, T2 p_x1, T3 p_y1)
		{
			return fastSqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}

		//------------------------------

		/*
			Rotates the vector clockwise (our coordinate system) by p_angle radians so that it keeps its length.
		*/
		Point<PointType>& rotate(double p_angle)
		{
			PointType xBefore = x;
			double cos = std::cos(p_angle);
			double sin = std::sin(p_angle);
			x = x * cos - y * sin;
			y = y * cos + xBefore * sin;
			return *this;
		}
		/*
			Rotates the point clockwise (our coordinate system) relative to (p_originX, p_originY) by p_angle radians so that it keeps its distance from that origin point.
		*/
		template<typename T0, typename T1>
		Point<PointType>& rotate(double p_angle, T0 p_originX, T1 p_originY)
		{
			PointType xBefore = x;
			double cos = std::cos(p_angle);
			double sin = std::sin(p_angle);
			x = (x - p_originX) * cos - (y - p_originY) * sin + p_originX;
			y = (y - p_originY) * cos + (xBefore - p_originX) * sin + p_originY;
			return *this;
		}
		/*
			Rotates the point clockwise (our coordinate system) relative to p_origin by p_angle radians so that it keeps its distance from p_origin.
		*/
		template<typename T>
		Point<PointType>& rotate(double p_angle, Point<T> const& p_origin)
		{
			rotate(p_angle, p_origin.x, p_origin.y);
			return *this;
		}

		/*
			Rotates the vector so that its angle is equal to p_angle radians, clockwise and relative to the x-axis on the right-hand side.
		*/
		Point<PointType>& setAngle(double p_angle)
		{
			double length = getLength();
			x = std::cos(p_angle) * length;
			y = std::sin(p_angle) * length;
			return *this;
		}
		/*
			Rotates the vector so that its angle relative to (p_originX, p_originY) is p_angle radians.
			The angle is clockwise in our coordinate system and relative to the x-axis on the right-hand side.
		*/
		template<typename T0, typename T1>
		Point<PointType>& setAngle(double p_angle, T0 p_originX, T1 p_originY)
		{
			double length = getDistance(p_originX, p_originY);
			x = std::cos(p_angle) * length + p_originX;
			y = std::sin(p_angle) * length + p_originY;
			return *this;
		}
		/*
			Rotates the vector so that its angle relative to p_origin is p_angle radians.
			The angle is clockwise in our coordinate system and relative to the x-axis on the right-hand side.
		*/
		template<typename T>
		Point<PointType>& setAngle(double p_angle, Point<T> const& p_origin)
		{
			setAngle(p_angle, p_origin.x, p_origin.y);
			return *this;
		}

		/*
			Returns the clockwise angle between the ray to the point and the x-axis on the right-hand side, in radians and in the range [0, 2pi].
		*/
		double getAngle() const
		{
			if (!x && !y)
			{
				return 0.;
			}
			double atan2 = std::atan2(y, x);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}
		/*
			Returns the clockwise angle between the ray to the point and the x-axis on the right-hand side, relative to (p_originX, p_originY).
			Angle is in radians and in the range [0, 2pi].
		*/
		template<typename T0, typename T1>
		double getAngle(T0 p_originX, T1 p_originY) const
		{
			if (x == p_originX && y == p_originY)
			{
				return 0.;
			}
			double atan2 = std::atan2(y - p_originY, x - p_originX);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}
		/*
			Returns the clockwise angle between the ray to the point and the x-axis on the right hand side, relative to p_origin.
			Angle is in radians and in the range [0, 2pi].
		*/
		template<typename T>
		double getAngle(Point<T> const& p_origin) const
		{
			if (x == p_origin.x && y == p_origin.y)
			{
				return 0.;
			}
			double atan2 = std::atan2(y - p_origin.y, x - p_origin.x);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}

		//------------------------------

		/*
			Uses an accurate but slower method to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		Point<PointType>& normalize()
		{
			float length = sqrt(x*x + y*y);
			x /= length;
			y /= length;
			return *this;
		}
		/*
			Uses a fast but less accurate method to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		Point<PointType>& normalizeFast()
		{
			float inverseLength = fastInverseSqrt(x*x + y*y);
			x *= inverseLength;
			y *= inverseLength;
			return *this;
		}

		//------------------------------

		/*
			Returns whether p_point is inside the rectangle formed from the origin to this point.
		*/
		template<typename T>
		bool getIsContaining(Point<T> const& p_point)
		{
			return p_point.x >= 0 && p_point.y >= 0 && p_point.x < x && p_point.y < y;
		}
		/*
			Returns whether the point (p_x, p_y) is inside the rectangle formed from the origin to this point.
		*/
		template<typename T0, typename T1>
		bool getIsContaining(T0 p_x, T1 p_y)
		{
			return p_x >= 0 && p_y >= 0 && p_x < x && p_y < y;
		}
		/*
			Returns whether the point (p_coordinate, p_coordinate) is inside the rectangle formed from the origin to this point.
		*/
		template<typename T>
		bool getIsContaining(T p_coordinate)
		{
			return p_coordinate >= 0 && p_coordinate < x && p_coordinate < y;
		}

		template<typename T>
		bool operator<(Point<T> const& p_point)
		{
			return x < p_point.x && y < p_point.y;
		}
		template<typename T>
		bool operator<(T p_coordinate)
		{
			return x < p_coordinate && y < p_coordinate;
		}
		template<typename T>
		bool operator>(Point<T> const& p_point)
		{
			return x > p_point.x && y > p_point.y;
		}
		template<typename T>
		bool operator>(T p_coordinate)
		{
			return x > p_coordinate && y > p_coordinate;
		}
	};

	template<typename T0, typename T1>
	Point<double> operator*(T0 p_factor, Point<T1> const& p_point)
	{
		return { p_point.x * p_factor, p_point.y * p_factor };
	}
	template<typename T0, typename T1>
	Point<double> operator/(T0 p_dividend, Point<T1> const& p_point)
	{
		return { p_dividend / p_point.x, p_dividend / p_point.y };
	}

	template<typename T0, typename T1>
	bool operator<(T0 p_coordinate, Point<T1> const& p_point)
	{
		return p_coordinate < p_point.x && p_coordinate < p_point.y;
	}
	template<typename T0, typename T1>
	bool operator>(T0 p_coordinate, Point<T1> const& p_point)
	{
		return p_coordinate > p_point.x && p_coordinate > p_point.y;
	}

	/*
		Linearly interpolates between p_start and p_end. This means we are calculating a point on the line segment between the two points.
		If p_progress is 0, p_start is returned. If p_progress is 1, p_end is returned.
		If p_progress is outside the range of [0, 1] then a point on the line that is defined by the two points will still be returned,
		but outside of the line segment between them.
	*/
	template<typename T>
	Point<T> interpolate(Point<T> const& p_start, Point<T> const& p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * p_progress;
	}

	//------------------------------

	class Transform
	{
	public:
		/*
			These are the transform coefficients.
		*/
		float xToX = 1.f, yToX = 0.f, offsetX = 0.f,
			  xToY = 0.f, yToY = 1.f, offsetY = 0.f;

		//------------------------------

		/*
			Applies the transform to a point.
		*/
		Point<float> operator*(Point<float> const& p_point) const
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
		Transform operator*(Transform const& p_transform) const
		{
			return {
				p_transform.xToX * xToX + p_transform.yToX * xToY,
				p_transform.xToX * yToX + p_transform.yToX * yToY,
				p_transform.xToX * offsetX + p_transform.yToX * offsetY,
				p_transform.xToY * xToX + p_transform.yToY * xToY,
				p_transform.xToY * yToX + p_transform.yToY * yToY,
				p_transform.xToY * offsetX + p_transform.yToY * offsetY
			};
		}
		/*
			Applies another transform to this one.
			Note that the order of multiplication matters.
			The right hand side is applied afterwards, meaning that the multiplication a *= b is actually mathematically a2 = b*a.
		*/
		Transform& operator*=(Transform const& p_transform)
		{
			xToX = xToX * p_transform.xToX + p_transform.yToX * xToY;
			yToX = p_transform.xToX * yToX + p_transform.yToX * yToY;
			offsetX = p_transform.xToX * offsetX + p_transform.yToX * offsetY;
			xToY = p_transform.xToY * xToX + p_transform.yToY * xToY;
			yToY = p_transform.xToY * yToX + p_transform.yToY * yToY;
			offsetY = p_transform.xToY * offsetX + p_transform.yToY * offsetY;
			return *this;
		}

		//------------------------------

		/*
			Rotates transformed points around the origin by an angle expressed in radians.
		*/
		Transform rotate(float p_radians)
		{
			/*
				[cos -sin  0]   [xToX yToX offsetX]
				[sin  cos  0] * [xToY yToY offsetY]
				[0    0    1]   [0    0    0      ]
			*/
			float cos = std::cos(p_radians);
			float sin = std::sin(p_radians);

			// Previous transformation
			Transform p(*this);

			xToX = cos * p.xToX - sin * p.xToY;
			yToX = cos * p.yToX - sin * p.yToY;
			offsetX = cos * p.offsetX - sin * p.offsetY;
			xToY = sin * p.xToX + cos * p.xToY;
			yToY = sin * p.yToX + cos * p.yToY;
			offsetY = sin * p.offsetX + cos * p.offsetY;

			return *this;
		}
		/*
			Rotates transformed points around (p_originX, p_originY) by an angle expressed in radians.
		*/
		Transform& rotate(float p_radians, float p_originX, float p_originY)
		{
			offsetX -= p_originX;
			offsetY -= p_originY;
			rotate(p_radians);
			offsetX += p_originX;
			offsetY += p_originY;
			return *this;
		}
		/*
			Rotates transformed points around p_origin by an angle expressed in radians.
		*/
		Transform& rotate(float p_radians, Avo::Point<float> const& p_origin)
		{
			return rotate(p_radians, p_origin.x, p_origin.y);
		}

		/*
			Moves the translation by (p_dx, p_dy).
		*/
		Transform& translate(float p_dx, float p_dy)
		{
			offsetX += p_dx, offsetY += p_dy;
			return *this;
		}
		/*
			Moves the translation by (p_dx, p_dy).
		*/
		Transform& translate(Point<float> const& p_offset)
		{
			offsetX += p_offset.x, offsetY += p_offset.y;
			return *this;
		}
		/*
			Sets the absolute offset in coordinates caused by the transform.
		*/
		Transform& setTranslation(float p_x, float p_y)
		{
			offsetX = p_x, offsetY = p_y;
			return *this;
		}
		/*
			Sets the absolute offset in coordinates caused by the transform.
		*/
		Transform& setTranslation(Point<float> const& p_point)
		{
			offsetX = p_point.x, offsetY = p_point.y;
			return *this;
		}

		/*
			Scales the transform by a factor.
		*/
		Transform& scale(float p_scaleFactor)
		{
			xToX *= p_scaleFactor;
			xToY *= p_scaleFactor;
			yToY *= p_scaleFactor;
			yToX *= p_scaleFactor;
			return *this;
		}
		/*
			Scales the transform by a horizontal and vertical factor.
		*/
		Transform& scale(float p_scaleFactorX, float p_scaleFactorY)
		{
			xToX *= p_scaleFactorX;
			yToX *= p_scaleFactorX;
			yToY *= p_scaleFactorY;
			xToY *= p_scaleFactorY;
			return *this;
		}
		/*
			Scales the transform by a horizontal and vertical factor.
		*/
		Transform& scale(Point<float> const& p_scaleFactor)
		{
			xToX *= p_scaleFactor.x;
			yToX *= p_scaleFactor.x;
			yToY *= p_scaleFactor.y;
			xToY *= p_scaleFactor.y;
			return *this;
		}
		Transform& scaleX(float p_scaleFactor)
		{
			xToX *= p_scaleFactor;
			yToX *= p_scaleFactor;
			return *this;
		}
		Transform& scaleY(float p_scaleFactor)
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
	template<typename RectangleType = float>
	class Rectangle
	{
	public:
		RectangleType left, top, right, bottom;

		Rectangle()
		{
			left = top = right = bottom = 0;
		}
		Rectangle(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			set(p_left, p_top, p_right, p_bottom);
		}
		template<typename T0, typename T1>
		Rectangle(Point<T0> const& p_position, Point<T1> const& p_size)
		{
			set(p_position, p_size);
		}
		template<typename T>
		explicit Rectangle(Rectangle<T> const& p_rectangle)
		{
			*this = p_rectangle;
		}

		//------------------------------

		void set(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			left = p_left;
			top = p_top;
			right = p_right;
			bottom = p_bottom;
		}
		template<typename T0, typename T1>
		void set(Point<T0> const& p_position, Point<T1> const& p_size)
		{
			left = p_position.x;
			top = p_position.y;
			right = left + p_size.x;
			bottom = top + p_size.y;
		}

		//------------------------------

		template<typename T>
		Rectangle<RectangleType>& operator=(Rectangle<T> const& p_rectangle)
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
		Rectangle<RectangleType>& clipNegativeSpace()
		{
			if (right < left) right = left;
			if (bottom < top) bottom = top;
			return *this;
		}

		//------------------------------

		/*
			Offsets the position of the rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType>& operator+=(Point<T> const& p_offset)
		{
			left += p_offset.x;
			top += p_offset.y;
			right += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		/*
			Offsets the position of the rectangle negatively.
		*/
		template<typename T>
		Rectangle<RectangleType>& operator-=(Point<T> const& p_offset)
		{
			left -= p_offset.x;
			top -= p_offset.y;
			right -= p_offset.x;
			bottom -= p_offset.y;
			return *this;
		}

		//------------------------------

		template<typename T>
		Rectangle<RectangleType> operator+(Point<T> const& p_offset)
		{
			return { left + p_offset.x, top + p_offset.y, right + p_offset.x, bottom + p_offset.y };
		}
		template<typename T>
		Rectangle<RectangleType> operator-(Point<T> const& p_offset)
		{
			return { left - p_offset.x, top - p_offset.y, right - p_offset.x, bottom - p_offset.y };
		}

		//------------------------------

		template<typename T>
		bool operator==(Rectangle<T> const& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}
		template<typename T>
		bool operator==(Rectangle<T>&& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}

		template<typename T>
		bool operator!=(Rectangle<T> const& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}
		template<typename T>
		bool operator!=(Rectangle<T>&& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}

		//------------------------------

		Rectangle<RectangleType> createCopyAtOrigin() const
		{
			return { 0, 0, right - left, bottom - top };
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new top and left edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopLeft(RectangleType p_topAndLeft, bool p_willKeepSize = true) const
		{
			return createCopyWithTopLeft(p_topAndLeft, p_topAndLeft, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new top-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithTopLeft(Point<T> const& p_position, bool p_willKeepSize = true) const
		{
			return createCopyWithTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new top-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return {
				p_left, p_top,
				p_willKeepSize * (p_left - left) + right,
				p_willKeepSize * (p_top - top) + bottom
			};
		}

		/*
			Sets the same position for the top and left edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopLeft(RectangleType p_topAndLeft, bool p_willKeepSize = true)
		{
			return setTopLeft(p_topAndLeft, p_topAndLeft, p_willKeepSize);
		}
		/*
			Sets the top left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType>& setTopLeft(Point<T> const& p_position, bool p_willKeepSize = true)
		{
			return setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the top left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				right += p_left - left;
				bottom += p_top - top;
			}
			left = p_left;
			top = p_top;
			return *this;
		}
		/*
			Returns the top left coordinates of the rectangle.
		*/
		Point<RectangleType> getTopLeft() const
		{
			return { left, top };
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new top and right edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopRight(RectangleType p_topAndRight, bool p_willKeepSize = true) const
		{
			return createCopyWithTopRight(p_topAndRight, p_topAndRight, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new top-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithTopRight(Point<T> const& p_position, bool p_willKeepSize = true) const
		{
			return createCopyWithTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new top-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return {
				p_willKeepSize * (p_right - right) + left,
				p_top, p_right,
				p_willKeepSize * (p_top - top) + bottom
			};
		}
		/*
			Sets the same position for the top and right edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopRight(RectangleType p_topAndRight, bool p_willKeepSize = true)
		{
			return setTopRight(p_topAndRight, p_topAndRight, p_willKeepSize);
		}
		/*
			Sets the top right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType>& setTopRight(Point<T> const& p_position, bool p_willKeepSize = true)
		{
			return setTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the top right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				left += p_right - right;
				bottom += p_top - top;
			}
			right = p_right;
			top = p_top;
			return *this;
		}
		/*
			Returns the top right coordinates of the rectangle.
		*/
		Point<RectangleType> getTopRight() const
		{
			return { right, top };
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new bottom and left edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomLeft(RectangleType p_bottomAndLeft, bool p_willKeepSize = true) const
		{
			return createCopyWithBottomLeft(p_bottomAndLeft, p_bottomAndLeft, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithBottomLeft(Point<T> const& p_position, bool p_willKeepSize = true) const
		{
			return createCopyWithBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return {
				p_left,
				p_willKeepSize * (p_bottom - bottom) + top,
				(p_left - left) + right,
				p_bottom
			};
		}
		/*
			Sets the same position for the bottom and left edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomLeft(RectangleType p_bottomAndLeft, bool p_willKeepSize = true)
		{
			return setBottomLeft(p_bottomAndLeft, p_bottomAndLeft, p_willKeepSize);
		}
		/*
			Sets the bottom left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType>& setBottomLeft(Point<T> const& p_position, bool p_willKeepSize = true)
		{
			return setBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the bottom left coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				right += p_left - left;
				top += p_bottom - bottom;
			}
			left = p_left;
			bottom = p_bottom;
			return *this;
		}
		/*
			Returns the bottom left coordinates of the rectangle.
		*/
		Point<RectangleType> getBottomLeft() const
		{
			return { left, bottom };
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new bottom and right edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_bottomAndRight, bool p_willKeepSize = true) const
		{
			return createCopyWithBottomRight(p_bottomAndRight, p_bottomAndRight, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithBottomRight(Point<T> const& p_position, bool p_willKeepSize = true) const
		{
			return createCopyWithBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return {
				p_willKeepSize * (p_right - right) + left,
				p_willKeepSize * (p_bottom - bottom) + top,
				p_right, p_bottom
			};
		}
		/*
			Sets the same position for the bottom and right edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomRight(RectangleType p_bottomAndRight, bool p_willKeepSize = true)
		{
			return setBottomRight(p_bottomAndRight, p_bottomAndRight, p_willKeepSize);
		}
		/*
			Sets the bottom right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename T>
		Rectangle<RectangleType>& setBottomRight(Point<T> const& p_position, bool p_willKeepSize = true)
		{
			return setBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the bottom right coordinates of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				left += p_right - right;
				top += p_bottom - bottom;
			}
			right = p_right;
			bottom = p_bottom;
			return *this;
		}
		/*
			Returns the bottom right coordinates of the rectangle.
		*/
		Point<RectangleType> getBottomRight() const
		{
			return Point<RectangleType>(right, bottom);
		}

		//------------------------------

		/*
			Sets the left coordinate of the rectangle.
			If p_willKeepWidth is true, the right coordinate will be changed so that the width stays the same.
		*/
		Rectangle<RectangleType>& setLeft(RectangleType p_left, bool p_willKeepWidth = true)
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
		Rectangle<RectangleType>& setTop(RectangleType p_top, bool p_willKeepHeight = true)
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
		Rectangle<RectangleType>& setRight(RectangleType p_right, bool p_willKeepWidth = true)
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
		Rectangle<RectangleType>& setBottom(RectangleType p_bottom, bool p_willKeepHeight = true)
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
			Returns a copy of the rectangle where the coordinates are rounded in the directions that expand the rectangle.
		*/
		Rectangle<RectangleType> createCopyWithOutwardsRoundedCoordinates() const
		{
			return { floor(left), floor(top), ceil(right), ceil(bottom) };
		}
		/*
			Rounds the coordinates of the rectangle in the directions that expand the rectangle.
		*/
		Rectangle<RectangleType>& roundCoordinatesOutwards()
		{
			left = floor(left);
			top = floor(top);
			right = ceil(right);
			bottom = ceil(bottom);
			return *this;
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new center position for both the x-axis and the y-axis.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithCenter(T p_centerXY) const
		{
			return createCopyWithCenter(p_centerXY, p_centerXY);
		}
		/*
			Creates a copy of this rectangle, with a new center position.
		*/
		template<typename T>
		Rectangle<RectangleType> createCopyWithCenter(Point<T> const& p_position) const
		{
			return createCopyWithCenter(p_position.x, p_position.y);
		}
		/*
			Creates a copy of this rectangle, with a new center position.
		*/
		template<typename T0, typename T1>
		Rectangle<RectangleType> createCopyWithCenter(T0 p_centerX, T1 p_centerY) const
		{
			RectangleType offsetX = p_centerX - (left + right)/2;
			RectangleType offsetY = p_centerY - (top + bottom)/2;
			return { offsetX + left, offsetY + top, offsetX + right, offsetY + bottom };
		}
		/*
			Sets the same center coordinates of the rectangle for the x-axis and the y-axis.
		*/
		template<typename T>
		Rectangle<RectangleType>& setCenter(T p_centerXY)
		{
			return setCenter(p_centerXY, p_centerXY);
		}
		/*
			Sets the center coordinates of the rectangle by moving it.
		*/
		template<typename T>
		Rectangle<RectangleType>& setCenter(Point<T> const& p_position)
		{
			return setCenter(p_position.x, p_position.y);
		}
		/*
			Sets the center coordinates of the rectangle by moving it.
		*/
		template<typename T0, typename T1>
		Rectangle<RectangleType>& setCenter(T0 p_x, T1 p_y)
		{
			RectangleType halfWidth = (right - left) / 2;
			RectangleType halfHeight = (bottom - top) / 2;
			left = p_x - halfWidth;
			top = p_y - halfHeight;
			right = p_x + halfWidth;
			bottom = p_y + halfHeight;
			return *this;
		}
		/*
			Sets the horizontal center coordinate of the rectangle by moving it.
		*/
		template<typename T>
		Rectangle<RectangleType>& setCenterX(T p_x)
		{
			RectangleType halfWidth = (right - left) / 2;
			left = p_x - halfWidth;
			right = p_x + halfWidth;
			return *this;
		}
		/*
			Sets the vertical center coordinate of the rectangle by moving it.
		*/
		template<typename T>
		Rectangle<RectangleType>& setCenterY(T p_y)
		{
			RectangleType halfHeight = (bottom - top) / 2;
			top = p_y - halfHeight;
			bottom = p_y + halfHeight;
			return *this;
		}
		/*
			Returns the center coordinates of the rectangle.
		*/
		Point<RectangleType> getCenter() const
		{
			return { (left + right) / 2, (top + bottom) / 2 };
		}
		/*
			Returns the x-axis center coordinate of the rectangle.
		*/
		RectangleType getCenterX() const
		{
			return (left + right) / 2;
		}
		/*
			Returns the y-axis center coordinate of the rectangle.
		*/
		RectangleType getCenterY() const
		{
			return (top + bottom) / 2;
		}

		//------------------------------

		/*
			Moves the left and top coordinates of the rectangle without affecting the other two.
		*/
		template<typename T>
		Rectangle<RectangleType>& moveTopLeft(Point<T> const& p_offset)
		{
			return moveTopLeft(p_offset.x, p_offset.y);
		}
		/*
			Moves the left and top coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveTopLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		/*
			Moves the right and top coordinates of the rectangle without affecting the other two.
		*/
		template<typename T>
		Rectangle<RectangleType>& moveTopRight(Point<T> const& p_offset)
		{
			return moveTopRight(p_offset.x, p_offset.y);
		}
		/*
			Moves the right and top coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveTopRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		/*
			Moves the left and bottom coordinates of the rectangle without affecting the other two.
		*/
		template<typename T>
		Rectangle<RectangleType>& moveBottomLeft(Point<T> const& p_offset)
		{
			return moveBottomLeft(p_offset.x, p_offset.y);
		}
		/*
			Moves the left and bottom coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveBottomLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		/*
			Moves the right and bottom coordinates of the rectangle without affecting the other two.
		*/
		template<typename T>
		Rectangle<RectangleType>& moveBottomRight(Point<T> const& p_offset)
		{
			return moveBottomRight(p_offset.x, p_offset.y);
		}
		/*
			Moves the right and bottom coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveBottomRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, offseted by an amount.
		*/
		template<typename T>
		Rectangle<RectangleType> createMovedCopy(Point<T> const& p_offset) const
		{
			return createMovedCopy(p_offset.x, p_offset.y);
		}
		/*
			Creates a copy of this rectangle, offseted by an amount.
		*/
		Rectangle<RectangleType> createMovedCopy(RectangleType p_offsetX, RectangleType p_offsetY) const
		{
			return { left + p_offsetX, top + p_offsetY, right + p_offsetX, bottom + p_offsetY };
		}
		/*
			Does the same as the += operator, offsets the whole rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType>& move(Point<T> const& p_offset)
		{
			return move(p_offset.x, p_offset.y);
		}
		/*
			Does the same as the += operator, offsets the whole rectangle.
		*/
		Rectangle<RectangleType>& move(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			right += p_offsetX;
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}
		/*
			Offsets the left and right coordinates by an amount.
		*/
		Rectangle<RectangleType>& moveX(RectangleType p_offsetX)
		{
			left += p_offsetX;
			right += p_offsetX;
			return *this;
		}
		/*
			Offsets the top and bottom coordinates by an amount.
		*/
		Rectangle<RectangleType>& moveY(RectangleType p_offsetY)
		{
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/*
			Sets the width and height of the rectangle, changing only the right and bottom coordinates.
		*/
		template<typename T>
		Rectangle<RectangleType>& setSize(Point<T> const& p_size)
		{
			return setSize(p_size.x, p_size.y);
		}
		/*
			Sets the width and height of the rectangle, changing only the right and bottom coordinates.
		*/
		Rectangle<RectangleType>& setSize(RectangleType p_width, RectangleType p_height)
		{
			right = left + p_width;
			bottom = top + p_height;
			return *this;
		}
		/*
			Returns a point representing the size of the rectangle, where the x component is width and y is height.
		*/
		Point<RectangleType> getSize() const
		{
			return Point<RectangleType>(right - left, bottom - top);
		}

		/*
			Sets the width of the rectangle, changing only the right coordinate.
		*/
		Rectangle<RectangleType>& setWidth(RectangleType p_width)
		{
			right = left + max(RectangleType(0), p_width);
			return *this;
		}
		/*
			Returns the distance between right and left coordinates of the rectangle.
		*/
		RectangleType getWidth() const
		{
			return right - left;
		}

		/*
			Sets the height of the rectangle, changing only the bottom coordinate.
		*/
		Rectangle<RectangleType>& setHeight(RectangleType p_height)
		{
			bottom = top + max(RectangleType(0), p_height);
			return *this;
		}
		/*
			Returns the distance between bottom and top coordinates of the rectangle.
		*/
		RectangleType getHeight() const
		{
			return bottom - top;
		}

		//------------------------------

		/*
			Returns a new copy of this rectangle, that is clipped to fit into the parameter rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType> createBoundedCopy(Rectangle<T> const& p_bounds) const
		{
			return createBoundedCopy(p_bounds.left, p_bounds.top, p_bounds.right, p_bounds.bottom);
		}
		/*
			Returns a new copy of this rectangle, that is clipped to fit into the parameter rectangle.
		*/
		Rectangle<RectangleType> createBoundedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			return {
				constrain(left, p_left, p_right),
				constrain(top, p_top, p_bottom),
				constrain(right, p_left, p_right),
				constrain(bottom, p_top, p_bottom)
			};
		}

		/*
			Clips this rectangle to fit into the parameter rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType>& bound(Rectangle<T> const& p_bounds)
		{
			return bound(p_bounds.left, p_bounds.top, p_bounds.right, p_bounds.bottom);
		}
		/*
			Clips this rectangle to fit into the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType>& bound(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			left = constrain(left, p_left, p_right);
			top = constrain(top, p_top, p_bottom);
			right = constrain(right, p_left, p_right);
			bottom = constrain(bottom, p_top, p_bottom);

			return *this;
		}

		//------------------------------

		/*
			Returns a copy of this rectangle that is extended so that it contains the parameter rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType> createContainedCopy(Rectangle<T> const& p_rectangle) const
		{
			return createContainedCopy(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			Returns a copy of this rectangle that is extended so that it contains the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType> createContainedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			return {
				min(left, p_left),
				min(top, p_top),
				max(right, p_right),
				max(bottom, p_bottom)
			};
		}

		/*
			Extends the rectangle so that it contains the parameter rectangle.
		*/
		template<typename T>
		Rectangle<RectangleType>& contain(Rectangle<T> const& p_rectangle)
		{
			return contain(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}

		/*
			Extends the rectangle so that it contains the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType>& contain(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			if (p_left < left)
				left = p_left;
			if (p_top < top)
				top = p_top;
			if (p_right > right)
				right = p_right;
			if (p_bottom > bottom)
				bottom = p_bottom;

			return *this;
		}

		//------------------------------

		/*
			Returns whether a point lies within this rectangle.
		*/
		template<typename T0, typename T1>
		bool getIsContaining(T0 p_x, T1 p_y) const
		{
			return p_x >= left && p_x < right
				&& p_y >= top && p_y < bottom;
		}
		/*
			Returns whether a point lies within this rectangle.
		*/
		template<typename T>
		bool getIsContaining(Point<T> const& p_point) const
		{
			return getIsContaining(p_point.x, p_point.y);
		}

		/*
			Returns whether another rectangle is fully inside this rectangle.
		*/
		template<typename T0, typename T1, typename T2, typename T3>
		bool getIsContaining(T0 p_left, T1 p_top, T2 p_right, T3 p_bottom) const
		{
			return p_left >= left && p_right < right
				&& p_top >= top && p_bottom < bottom;
		}
		/*
			Returns whether another rectangle is fully inside this rectangle.
		*/
		template<typename T>
		bool getIsContaining(Rectangle<T> const& p_rectangle) const
		{
			return getIsContaining(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			Returns whether a protected rectangle is fully inside this rectangle.
		*/
		bool getIsContaining(ProtectedRectangle* p_protectedRectangle) const;

		/*
			Returns whether this rectangle intersects/overlaps/touches another rectangle.
		*/
		template<typename T0, typename T1, typename T2, typename T3>
		bool getIsIntersecting(T0 p_left, T1 p_top, T2 p_right, T3 p_bottom) const
		{
			return p_right >= left && p_bottom >= top
				&& p_left <= right && p_top <= bottom;
		}
		/*
			Returns whether this rectangle intersects/overlaps/touches another rectangle.
		*/
		template<typename T>
		bool getIsIntersecting(Rectangle<T> const& p_rectangle) const
		{
			return getIsIntersecting(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			Returns whether this rectangle intersects/overlaps/touches a protected rectangle.
		*/
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const;
	};

	// It would be useless and too much job to make ProtectedRectangle templated... floats are pretty nice anyways

	/*
		An abstract class for objects that have 2d bounds for which changes by the user of the object can be responded to.
	*/
	class ProtectedRectangle
	{
	protected:
		Rectangle<float> m_bounds;

		virtual void handleProtectedRectangleChange(Rectangle<float> const& p_oldRectangle) { }

	public:
		ProtectedRectangle() = default;
		explicit ProtectedRectangle(Rectangle<float> const& p_bounds) : m_bounds(p_bounds) { }
		explicit ProtectedRectangle(Rectangle<float>&& p_bounds) : m_bounds(p_bounds) { }

		virtual void setBounds(float p_left, float p_top, float p_right, float p_bottom)
		{
			auto oldRectangle = m_bounds;
			m_bounds.set(p_left, p_top, p_right, p_bottom);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setBounds(Rectangle<float> const& p_rectangle)
		{
			setBounds(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		virtual void setBounds(Point<float> const& p_position, Point<float> const& p_size)
		{
			setBounds(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		virtual Rectangle<float> const& getBounds() const
		{
			return m_bounds;
		}

		//------------------------------

		virtual void move(float p_offsetX, float p_offsetY)
		{
			auto oldRectangle = m_bounds;
			m_bounds.move(p_offsetX, p_offsetY);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void move(Point<float> const& p_offset)
		{
			move(p_offset.x, p_offset.y);
		}
		virtual void moveX(float p_offsetX)
		{
			auto oldRectangle = m_bounds;
			m_bounds.moveX(p_offsetX);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void moveY(float p_offsetY)
		{
			auto oldRectangle = m_bounds;
			m_bounds.moveY(p_offsetY);
			handleProtectedRectangleChange(oldRectangle);
		}

		//------------------------------

		virtual void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setTopLeft(float p_topAndLeft, bool p_willKeepSize = true)
		{
			setTopLeft(p_topAndLeft, p_topAndLeft, p_willKeepSize);
		}
		virtual void setTopLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		virtual Point<float> getTopLeft() const
		{
			return m_bounds.getTopLeft();
		}

		virtual void setTopRight(float p_right, float p_top, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setTopRight(float p_topAndRight, bool p_willKeepSize = true)
		{
			setTopRight(p_topAndRight, p_topAndRight, p_willKeepSize);
		}
		virtual void setTopRight(Point<float> const& p_topRight, bool p_willKeepSize = true)
		{
			setTopRight(p_topRight.x, p_topRight.y, p_willKeepSize);
		}
		virtual Point<float> getTopRight() const
		{
			return m_bounds.getTopRight();
		}

		virtual void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setBottomLeft(float p_bottomAndLeft, bool p_willKeepSize = true)
		{
			setBottomLeft(p_bottomAndLeft, p_bottomAndLeft, p_willKeepSize);
		}
		virtual void setBottomLeft(Point<float> const& p_bottomLeft, bool p_willKeepSize = true)
		{
			setBottomLeft(p_bottomLeft.x, p_bottomLeft.y, p_willKeepSize);
		}
		virtual Point<float> getBottomLeft() const
		{
			return m_bounds.getBottomLeft();
		}

		virtual void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setBottomRight(float p_bottomAndRight, bool p_willKeepSize = true)
		{
			setBottomRight(p_bottomAndRight, p_bottomAndRight, p_willKeepSize);
		}
		virtual void setBottomRight(Point<float> const& p_bottomRight, bool p_willKeepSize = true)
		{
			setBottomRight(p_bottomRight.x, p_bottomRight.y, p_willKeepSize);
		}
		virtual Point<float> getBottomRight() const
		{
			return m_bounds.getBottomRight();
		}

		//------------------------------

		virtual void setCenter(float p_x, float p_y)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenter(p_x, p_y);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setCenter(float p_centerXY)
		{
			setCenter(p_centerXY, p_centerXY);
		}
		virtual void setCenter(Point<float> const& p_position)
		{
			setCenter(p_position.x, p_position.y);
		}
		virtual void setCenterX(float p_x)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenterX(p_x);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setCenterY(float p_y)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setCenterY(p_y);
			handleProtectedRectangleChange(oldRectangle);
		}

		virtual Point<float> getCenter() const
		{
			return m_bounds.getCenter();
		}
		virtual float getCenterX() const
		{
			return m_bounds.getCenterX();
		}
		virtual float getCenterY() const
		{
			return m_bounds.getCenterY();
		}

		//------------------------------

		virtual void setLeft(float p_left, bool p_willKeepWidth = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setLeft(p_left, p_willKeepWidth);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getLeft() const
		{
			return m_bounds.left;
		}

		virtual void setTop(float p_top, bool p_willKeepHeight = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setTop(p_top, p_willKeepHeight);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getTop() const
		{
			return m_bounds.top;
		}

		virtual void setRight(float p_right, bool p_willKeepWidth = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setRight(p_right, p_willKeepWidth);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getRight() const
		{
			return m_bounds.right;
		}

		virtual void setBottom(float p_bottom, bool p_willKeepHeight = true)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setBottom(p_bottom, p_willKeepHeight);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getBottom() const
		{
			return m_bounds.bottom;
		}

		//------------------------------

		virtual void setWidth(float p_width)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setWidth(p_width);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getWidth() const
		{
			return m_bounds.getWidth();
		}

		virtual void setHeight(float p_height)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setHeight(p_height);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual float getHeight() const
		{
			return m_bounds.getHeight();
		}

		virtual void setSize(float p_width, float p_height)
		{
			auto oldRectangle = m_bounds;
			m_bounds.setSize(p_width, p_height);
			handleProtectedRectangleChange(oldRectangle);
		}
		virtual void setSize(Point<float> const& p_size)
		{
			setSize(p_size.x, p_size.y);
		}
		virtual Point<float> getSize() const
		{
			return m_bounds.getSize();
		}

		//------------------------------

		virtual bool getIsIntersecting(float p_left, float p_top, float p_right, float p_bottom) const
		{
			return m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom);
		}
		virtual bool getIsIntersecting(Rectangle<float> const& p_rectangle) const
		{
			return getIsIntersecting(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		virtual bool getIsIntersecting(ProtectedRectangle* p_rectangle) const
		{
			return getIsIntersecting(p_rectangle->getLeft(), p_rectangle->getTop(), p_rectangle->getRight(), p_rectangle->getBottom());
		}

		virtual bool getIsContaining(float p_left, float p_top, float p_right, float p_bottom) const
		{
			return m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom);
		}
		virtual bool getIsContaining(Rectangle<float> const& p_rectangle) const
		{
			return getIsContaining(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		virtual bool getIsContaining(ProtectedRectangle* p_rectangle) const
		{
			return getIsContaining(p_rectangle->getLeft(), p_rectangle->getTop(), p_rectangle->getRight(), p_rectangle->getBottom());
		}

		virtual bool getIsContaining(float p_x, float p_y) const
		{
			return m_bounds.getIsContaining(p_x, p_y);
		}
		virtual bool getIsContaining(Point<float> const& p_point) const
		{
			return getIsContaining(p_point.x, p_point.y);
		}
	};

	//------------------------------

	enum class RectangleCornerType
	{
		Round,
		Cut
	};
	class RectangleCorners
	{
	public:
		RectangleCornerType topLeftType;
		RectangleCornerType topRightType;
		RectangleCornerType bottomLeftType;
		RectangleCornerType bottomRightType;

		float topLeftSizeX;
		float topLeftSizeY;

		float topRightSizeX;
		float topRightSizeY;

		float bottomLeftSizeX;
		float bottomLeftSizeY;

		float bottomRightSizeX;
		float bottomRightSizeY;

		RectangleCorners() :
			topLeftSizeX{ 0.f }, topLeftSizeY{ 0.f }, topRightSizeX{ 0.f }, topRightSizeY{ 0.f },
			bottomLeftSizeX{ 0.f }, bottomLeftSizeY{ 0.f }, bottomRightSizeX{ 0.f }, bottomRightSizeY{ 0.f },
			topLeftType{ RectangleCornerType::Round }, topRightType{ RectangleCornerType::Round },
			bottomLeftType{ RectangleCornerType::Round }, bottomRightType{ RectangleCornerType::Round }
		{
		}
		explicit RectangleCorners(float p_cornerSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX{ p_cornerSize }, topLeftSizeY{ p_cornerSize },
			topRightSizeX{ p_cornerSize }, topRightSizeY{ p_cornerSize },
			bottomLeftSizeX{ p_cornerSize }, bottomLeftSizeY{ p_cornerSize },
			bottomRightSizeX{ p_cornerSize }, bottomRightSizeY{ p_cornerSize },
			topLeftType{ p_cornerType }, topRightType{ p_cornerType },
			bottomLeftType{ p_cornerType }, bottomRightType{ p_cornerType }
		{
		}
		RectangleCorners(float p_cornerSizeX, float p_cornerSizeY, RectangleCornerType p_cornerType = RectangleCornerType::Cut) :
			topLeftSizeX{ p_cornerSizeX }, topLeftSizeY{ p_cornerSizeY },
			topRightSizeX{ p_cornerSizeX }, topRightSizeY{ p_cornerSizeY },
			bottomLeftSizeX{ p_cornerSizeX }, bottomLeftSizeY{ p_cornerSizeY },
			bottomRightSizeX{ p_cornerSizeX }, bottomRightSizeY{ p_cornerSizeY },
			topLeftType{ p_cornerType }, topRightType{ p_cornerType },
			bottomLeftType{ p_cornerType }, bottomRightType{ p_cornerType }
		{
		}

		RectangleCorners(float p_topLeftSize, float p_topRightSize, float p_bottomLeftSize, float p_bottomRightSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX{ p_topLeftSize }, topLeftSizeY{ p_topLeftSize },
			topRightSizeX{ p_topRightSize }, topRightSizeY{ p_topRightSize },
			bottomLeftSizeX{ p_bottomLeftSize }, bottomLeftSizeY{ p_bottomLeftSize },
			bottomRightSizeX{ p_bottomRightSize }, bottomRightSizeY{ p_bottomRightSize },
			topLeftType{ p_cornerType }, topRightType{ p_cornerType },
			bottomLeftType{ p_cornerType }, bottomRightType{ p_cornerType }
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
		float x0, y0, x1, y1;

		//------------------------------

		Easing() : x0{ 0.f }, y0{ 0.f }, x1{ 1.f }, y1{ 1.f } { }
		Easing(Easing const& p_easing)
		{
			*this = p_easing;
		}
		Easing(Easing&& p_easing) noexcept
		{
			*this = p_easing;
		}
		/*
			Initializes the control points of the bezier curve easing.
			The parameters are the coordinates of the first and second control points, respectively.
		*/
		Easing(float p_x0, float p_y0, float p_x1, float p_y1) :
			x0{ p_x0 }, y0{ p_y0 }, x1{ p_x1 }, y1{ p_y1 }
		{ }
		/*
			Initializes the control points of the bezier curve easing.
			The parameters are the coordinates of the first and second control points, respectively.
		*/
		Easing(double p_x0, double p_y0, double p_x1, double p_y1) :
			x0{ (float)p_x0 }, y0{ (float)p_y0 }, x1{ (float)p_x1 }, y1{ (float)p_y1 }
		{ }

		Easing& operator=(Easing const& p_easing)
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}
		Easing& operator=(Easing&& p_easing) noexcept
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}

		//------------------------------

		bool operator==(Easing const& p_easing) const
		{
			return x0 == p_easing.x0 && y0 == p_easing.y0 && x1 == p_easing.x1 && y1 == p_easing.y1;
		}
		bool operator!=(Easing const& p_easing) const
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
		static float easeValue(float p_x0, float p_y0, float p_x1, float p_y1, float p_value, float p_precision = 0.005f)
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
			*/

			float error = 1;
			while (std::abs(error) > p_precision)
			{
				error = p_value - t * ((1.f - t) * (3.f * (1.f - t) * p_x0 + 3.f * t * p_x1) + t * t);
				t += error / (p_x0 * (3.f - 12.f * t + 9.f * t * t) + p_x1 * (6.f * t - 9.f * t * t) + 3.f * t * t);
			}

			return t * ((1.f - t) * (3.f * (1.f - t) * p_y0 + 3.f * t * p_y1) + t * t);
		}
		/*
			Transforms a normalized value according to a cubic bezier curve.
			p_precision is the maximum amount of error in the output value.

			It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points;
			f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (p_value is x) and not t. This why we have a precision parameter.
		*/
		float easeValue(float p_value, float p_precision = 0.005f) const
		{
			return easeValue(x0, y0, x1, y1, p_value, p_precision);
		}
		/*
			Transforms a normalized value according to the inverse of this cubic bezier curve,
			so that easeValueInverse(easeValue(x)) = x (approximately)
			p_precision is the maximum amount of error in the output value.
		*/
		float easeValueInverse(float p_value, float p_precision = 0.005f) const
		{
			return easeValue(y0, x0, y1, x1, p_value, p_precision);
		}
	};

	class View;

	/*
		Class used for making animations.
		To make an animation, call View::createAnimation from a view.
		It is then cleaned up when the view destroys.
	*/
	class Animation : public ReferenceCounted
	{
	private:
		bool m_isReversed = false;
	public:
		void setIsReversed(bool p_isReversed)
		{
			if (p_isReversed != m_isReversed)
			{
				float value = m_easing.easeValue(std::chrono::duration<float, std::milli>{std::chrono::steady_clock::now() - m_startTime}.count() / m_milliseconds, m_easingPrecision);
				m_startTime = std::chrono::steady_clock::now() - std::chrono::steady_clock::duration{ uint64(m_easing.easeValueInverse(1.f - value) * m_milliseconds * 1'000'000) };
				m_isReversed = p_isReversed;
			}
		}
		bool getIsReversed()
		{
			return m_isReversed;
		}

	private:
		bool m_isDone = true;
	public:
		bool getIsDone()
		{
			return m_isDone;
		}

	private:
		float m_milliseconds;
	public:
		void setDuration(float p_milliseconds)
		{
			m_milliseconds = p_milliseconds;
		}
		/*
			Returns the duration of the animation in milliseconds.
		*/
		float getDuration()
		{
			return m_milliseconds;
		}

	private:
		Easing m_easing;
	public:
		void setEasing(Easing const& p_easing)
		{
			m_easing = p_easing;
		}
		Easing getEasing()
		{
			return m_easing;
		}

	private:
		float m_easingPrecision = 0.005f;
	public:
		void setEasingPrecision(float p_easingPrecision)
		{
			m_easingPrecision = p_easingPrecision;
		}
		float getEasingPrecision()
		{
			return m_easingPrecision;
		}

	private:
		View* m_view;
		bool m_isInUpdateQueue = false;
		void queueUpdate();

		bool m_areUpdatesCancelled = false;

		void update()
		{
			if (m_areUpdatesCancelled)
			{
				m_isInUpdateQueue = false;
				return;
			}
			float value = m_easing.easeValue(std::chrono::duration<float, std::milli>{std::chrono::steady_clock::now() - m_startTime}.count() / m_milliseconds, m_easingPrecision);
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
		void cancelAllUpdates()
		{
			m_areUpdatesCancelled = true;
		}

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
		std::chrono::steady_clock::time_point m_startTime;
		std::chrono::steady_clock::time_point m_pauseTime;
	public:
		void play(bool p_isReversed)
		{
			setIsReversed(p_isReversed);
			if (m_isPaused)
			{
				m_startTime += std::chrono::steady_clock::now() - m_pauseTime;
			}
			else if (m_isDone)
			{
				m_startTime = std::chrono::steady_clock::now();
			}
			else return;
			m_isDone = false;
			queueUpdate();
		}
		void play()
		{
			play(m_isReversed);
		}
		/*
			If the animation is reversed then the animation value will start at 1 if p_startProgress is 0.
		*/
		void play(float p_startProgress)
		{
			m_isDone = false;
			if (m_isReversed)
			{
				m_startTime = std::chrono::steady_clock::now() - std::chrono::steady_clock::duration{ uint64((1.f - p_startProgress) * m_milliseconds * 1'000'000) };
			}
			else
			{
				m_startTime = std::chrono::steady_clock::now() - std::chrono::steady_clock::duration{ uint64(p_startProgress * m_milliseconds * 1'000'000) };
			}
		}
		void pause()
		{
			m_isPaused = true;
			m_isDone = true;
		}
		void stop()
		{
			m_isPaused = false;
			m_isDone = true;
		}
		void replay()
		{
			stop();
			play();
		}

	private:
		friend class Gui;
		friend class View;

		Animation(View* p_view, Easing const& p_easing, float p_milliseconds) :
			m_view(p_view),
			m_easing(p_easing),
			m_milliseconds(p_milliseconds)
		{
		}
		~Animation()
		{

		}
	};

	//------------------------------
	// Color stuff

	/*
		ARGB formatted 32-bit packed color, where every channel has 8 bits.
	*/
	using colorInt = uint32;

	inline uint8 getRedChannel(colorInt p_color)
	{
		return p_color >> 16 & 0xff;
	}
	inline uint8 getGreenChannel(colorInt p_color)
	{
		return p_color >> 8 & 0xff;
	}
	inline uint8 getBlueChannel(colorInt p_color)
	{
		return p_color & 0xff;
	}
	inline uint8 getAlphaChannel(colorInt p_color)
	{
		return p_color >> 24 & 0xff;
	}

	/*
		Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
		This means that a Color object is 4 times as big as a packed 32-bit color.
	*/
	class Color
	{
	public:
		float red;
		float green;
		float blue;
		float alpha;

		//------------------------------

		Color() :
			red(0.f), green(0.f), blue(0.f), alpha(1.f)
		{ }
		/*
			The channels are floats in the range [0, 1].
		*/
		Color(float p_red, float p_green, float p_blue, float p_alpha = 1.f) :
			red(constrain(p_red)), green(constrain(p_green)), blue(constrain(p_blue)), alpha(constrain(p_alpha))
		{ }
		/*
			The channels are doubles in the range [0, 1].
		*/
		Color(double p_red, double p_green, double p_blue, double p_alpha = 1.f) :
			red(constrain(p_red)), green(constrain(p_green)), blue(constrain(p_blue)), alpha(constrain(p_alpha))
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(uint8 p_red, uint8 p_green, uint8 p_blue, uint8 p_alpha = (uint8)255) :
			red(float(p_red) / 255.f), green(float(p_green) / 255.f), blue(float(p_blue) / 255.f), alpha(float(p_alpha) / 255.f)
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(uint32 p_red, uint32 p_green, uint32 p_blue, uint32 p_alpha = (uint32)255) :
			red(constrain(float(p_red) / 255.f)), green(constrain(float(p_green) / 255.f)), blue(constrain(float(p_blue) / 255.f)), alpha(constrain(float(p_alpha) / 255.f))
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(int32 p_red, int32 p_green, int32 p_blue, int32 p_alpha = (int32)255) :
			red(constrain(float(p_red) / 255.f)), green(constrain(float(p_green) / 255.f)), blue(constrain(float(p_blue) / 255.f)), alpha(constrain(float(p_alpha) / 255.f))
		{ }
		/*
			Initializes the color with a grayscale value. The values are floats in the range [0, 1].
		*/
		Color(float p_lightness, float p_alpha = 1.f)
		{
			red = green = blue = constrain(p_lightness);
			alpha = constrain(p_alpha);
		}
		/*
			Initializes the color with a grayscale value. The values are doubles in the range [0, 1].
		*/
		Color(double p_lightness, double p_alpha = 1.)
		{
			red = green = blue = constrain(p_lightness);
			alpha = constrain(p_alpha);
		}
		/*
			Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
		*/
		Color(uint8 p_lightness, uint8 p_alpha = (uint8)255)
		{
			red = green = blue = float(p_lightness) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Initializes the color with a grayscale value. The values are in the range [0, 255].
		*/
		Color(uint32 p_lightness, uint32 p_alpha)
		{
			red = green = blue = constrain(float(p_lightness) / 255.f);
			alpha = constrain(float(p_alpha) / 255.f);
		}
		/*
			Initializes the color with a grayscale value. The values are in the range [0, 255].
		*/
		Color(int32 p_lightness, int32 p_alpha = 255)
		{
			red = green = blue = constrain(float(p_lightness) / 255.f);
			alpha = constrain(float(p_alpha) / 255.f);
		}
		/*
			Creates a copy of another color but with a new alpha.
		*/
		Color(Color const& p_color, float p_alpha) :
			red(p_color.red), green(p_color.green), blue(p_color.blue), alpha(p_alpha)
		{ }
		/*
			Creates a copy of another color but with a new alpha.
		*/
		Color(Color const& p_color, double p_alpha) :
			red(p_color.red), green(p_color.green), blue(p_color.blue), alpha(p_alpha)
		{ }
		/*
			Initializes with a 4-byte packed RGBA color.
		*/
		Color(colorInt p_color)
		{
			operator=(p_color);
		}

		Color& operator=(colorInt p_color)
		{
			alpha = float(p_color >> 24u) / 255.f;
			red = float(p_color >> 16u & 0xffu) / 255.f;
			green = float(p_color >> 8u & 0xffu) / 255.f;
			blue = float(p_color & 0xffu) / 255.f;
			return *this;
		}

		Color operator*(float p_factor)
		{
			return { red * p_factor, green * p_factor, blue * p_factor };
		}
		Color& operator*=(float p_factor)
		{
			red *= p_factor;
			green *= p_factor;
			blue *= p_factor;
			return *this;
		}
		Color operator/(float p_divisor)
		{
			return { red / p_divisor, green / p_divisor, blue / p_divisor };
		}
		Color& operator/=(float p_divisor)
		{
			red /= p_divisor;
			green /= p_divisor;
			blue /= p_divisor;
			return *this;
		}

		Color operator+(float p_delta)
		{
			return { red + p_delta, green + p_delta, blue + p_delta };
		}
		Color& operator+=(float p_delta)
		{
			red += p_delta;
			green += p_delta;
			blue += p_delta;
			return *this;
		}

		Color operator-(float p_delta)
		{
			return { red - p_delta, green - p_delta, blue - p_delta };
		}
		Color& operator-=(float p_delta)
		{
			red -= p_delta;
			green -= p_delta;
			blue -= p_delta;
			return *this;
		}

		//------------------------------

		bool operator==(Color const& p_color) const
		{
			return red == p_color.red && green == p_color.green && blue == p_color.blue && alpha == p_color.alpha;
		}
		bool operator!=(Color const& p_color) const
		{
			return red != p_color.red || green != p_color.green || blue != p_color.blue || alpha != p_color.alpha;
		}

		//------------------------------

		/*
			Sets the values for the red, green, blue and alpha channels. They are all floats in the range of [0, 1].
		*/
		void setRGBA(float p_red, float p_green, float p_blue, float p_alpha = 1.f)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. They are floats in the range of [0, 1].
		*/
		void setRGBA(float p_grayscale, float p_alpha = 1.f)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the values for the red, green and blue channels. They are all floats in the range of [0, 1].
		*/
		void setRGB(float p_red, float p_green, float p_blue)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
		}
		/*
			Sets the same values for the red, green and blue channels. They are floats in the range of [0, 1].
		*/
		void setRGB(float p_grayscale)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
		}
		/*
			Sets the values for the red, green, blue and alpha channels. They are all doubles in the range of [0, 1].
		*/
		void setRGBA(double p_red, double p_green, double p_blue, double p_alpha = 1.f)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. They are doubles in the range of [0, 1].
		*/
		void setRGBA(double p_grayscale, double p_alpha = 1.f)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the values for the red, green and blue channels. They are all doubles in the range of [0, 1].
		*/
		void setRGB(double p_red, double p_green, double p_blue)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
		}
		/*
			Sets the same values for the red, green and blue channels. They are doubles in the range of [0, 1].
		*/
		void setRGB(double p_grayscale)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGBA(uint8 p_red, uint8 p_green, uint8 p_blue, uint8 p_alpha = (uint8)255)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGBA(uint8 p_grayscale, uint8 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGB(uint8 p_red, uint8 p_green, uint8 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGB(uint8 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(uint32 p_red, uint32 p_green, uint32 p_blue, uint32 p_alpha = 255U)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(uint32 p_grayscale, uint32 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(uint32 p_red, uint32 p_green, uint32 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(uint32 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(int32 p_red, int32 p_green, int32 p_blue, int32 p_alpha = 255U)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(int32 p_grayscale, int32 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(int32 p_red, int32 p_green, int32 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(int32 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}

		//------------------------------

		/*
			Sets the hue, saturation and brightness values of the color. They are all floats in the range [0, 1].
		*/
		Color& setHSB(float p_hue, float p_saturation, float p_brightness)
		{
			p_hue -= floor(p_hue);
			p_brightness = constrain(p_brightness);
			float factor = p_brightness * constrain(p_saturation);
			red = p_brightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 1.f);
			green = p_brightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 1.f);
			blue = p_brightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 1.f);
			return *this;
		}
		/*
			Sets the hue, saturation, brightness and alpha values of the color. They are all floats in the range [0, 1].
		*/
		Color& setHSBA(float p_hue, float p_saturation, float p_brightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSB(p_hue, p_saturation, p_brightness);
		}
		/*
			Sets the hue, saturation and lightness values of the color. They are all floats in the range [0, 1]
		*/
		Color& setHSL(float p_hue, float p_saturation, float p_lightness)
		{
			p_hue -= floor(p_hue);
			p_lightness = constrain(p_lightness);
			float factor = 2.f*constrain(p_saturation)*(p_lightness < 0.5f ? p_lightness : (1.f - p_lightness));
			red = p_lightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 0.5f);
			green = p_lightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 0.5f);
			blue = p_lightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 0.5f);
			return *this;
		}
		/*
			Sets the hue, saturation, lightness and alpha values of the color. They are all floats in the range [0, 1]
		*/
		Color& setHSLA(float p_hue, float p_saturation, float p_lightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSL(p_hue, p_saturation, p_lightness);
		}

		/*
			Changes the hue of the color. The hue is a float in the range [0, 1].
		*/
		Color& setHue(float p_hue)
		{
			p_hue -= floor(p_hue);
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			red = minColor + (maxColor - minColor)*(constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f));
			green = minColor + (maxColor - minColor)*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f));
			blue = minColor + (maxColor - minColor)*(constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f));
			return *this;
		}
		/*
			Returns the hue of the color. The hue is a float in the range [0, 1].
		*/
		float getHue() const
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
						return (green - blue) / (red - blue) / 6.f;
					}
					else
					{
						// (1, 0, 1) -> (1, 0, 0) : 5/6 < hue < 6/6, max = red, min = green
						return 1.f - (blue - green) / (red - green) / 6.f;
					}
				}
				else
				{
					// (0, 0, 1) -> (1, 0, 1) : 4/6 < hue < 5/6, max = blue, min = green
					return (4.f + (red - green) / (blue - green)) / 6.f;
				}
			}
			else
			{
				if (green > blue)
				{
					if (red > blue)
					{
						// (1, 1, 0) -> (0, 1, 0) : 1/6 < hue < 2/6, max = green, min = blue
						return (2.f - (red - blue) / (green - blue)) / 6.f;
					}
					else
					{
						// (0, 1, 0) -> (0, 1, 1) : 2/6 < hue < 3/6, max = green, min = red
						return (2.f + (blue - red) / (green - red)) / 6.f;
					}
				}
				else
				{
					// (0, 1, 1) -> (0, 0, 1) : 3/6 < hue < 4/6, max = blue, min = red
					return (4.f - (green - red) / (blue - red)) / 6.f;
				}
			}
		}

		/*
			Sets the HSB saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is grayscale, because only RGBA values are stored.
		*/
		Color& setSaturationHSB(float p_saturation)
		{
			if (red == green && red == blue)
			{
				return *this;
			}

			p_saturation = constrain(p_saturation);
			float factor = p_saturation / getSaturationHSB();

			float brightness = max(red, green, blue);
			red = brightness + factor * (red - brightness);
			green = brightness + factor * (green - brightness);
			blue = brightness + factor * (blue - brightness);

			return *this;
		}
		/*
			Returns the HSB saturation of the color. The saturation is a float in the range [0, 1].
		*/
		float getSaturationHSB() const
		{
			float brightness = getBrightness();
			if (brightness)
			{
				return 1.f - min(red, green, blue) / brightness;
			}
			return 0.f;
		}

		/*
			Sets the HSL saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is gray, since only RGBA values are stored.
		*/
		Color& setSaturationHSL(float p_saturation)
		{
			p_saturation = constrain(p_saturation);

			float lightness = getLightness();
			float factor = p_saturation / getSaturationHSL();
			if (factor == p_saturation / 0.f)
			{
				return *this;
			}
			red = lightness + factor * (red - lightness);
			green = lightness + factor * (green - lightness);
			blue = lightness + factor * (blue - lightness);

			return *this;
		}
		/*
			Returns the HSL saturation of the color. The saturation is a float in the range [0, 1].
		*/
		float getSaturationHSL() const
		{
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			if (minColor == maxColor)
			{
				return 0.f;
			}
			return max((maxColor - minColor) / (maxColor + minColor), (maxColor - minColor) / (2.f - maxColor - minColor));
		}

		/*
			Sets the brightness of the color. The brightness is a float in the range [0, 1]. A brightness of 0 makes the
			color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
		*/
		Color& setBrightness(float p_brightness)
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
			red *= p_brightness / brightness;
			green *= p_brightness / brightness;
			blue *= p_brightness / brightness;

			return *this;
		}
		/*
			Returns the brightness of the color. The brightness is a float in the range [0, 1].
		*/
		float getBrightness() const
		{
			return max(red, green, blue);
		}

		/*
			Changes the lightness of the color. The lightness a float in the range [0, 1]. A lightness of 0 makes the
			color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
		*/
		Color& setLightness(float p_lightness)
		{
			p_lightness = constrain(p_lightness);

			if (red == green && red == blue)
			{
				red = p_lightness;
				green = p_lightness;
				blue = p_lightness;
				return *this;
			}

			float lightness = getLightness();
			if (lightness <= 0.5f)
			{
				if (p_lightness <= 0.5f)
				{
					red = red * p_lightness / lightness;
					green = green * p_lightness / lightness;
					blue = blue * p_lightness / lightness;
				}
				else
				{
					red = (red - lightness) * (1.f - p_lightness) / lightness + p_lightness;
					green = (green - lightness) * (1.f - p_lightness) / lightness + p_lightness;
					blue = (blue - lightness) * (1.f - p_lightness) / lightness + p_lightness;
				}
			}
			else
			{
				if (p_lightness <= 0.5)
				{
					red = (red - lightness)*p_lightness / (1.f - lightness) + p_lightness;
					green = (green - lightness)*p_lightness / (1.f - lightness) + p_lightness;
					blue = (blue - lightness)*p_lightness / (1.f - lightness) + p_lightness;
				}
				else
				{
					red = (red - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
					green = (green - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
					blue = (blue - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
				}
			}
			return *this;
		}
		/*
			Returns the lightness of the color. The lightness is a float in the range [0, 1].
		*/
		float getLightness() const
		{
			return 0.5f*(min(red, green, blue) + max(red, green, blue));
		}

		//------------------------------

		/*
			A contrast of 0 makes the color gray, 0.5 leaves it unchanged and 1 is maximum contrast.
		*/
		void setContrast(float p_contrast)
		{
			if (p_contrast == 0.5)
			{
				return;
			}
			if (p_contrast < 0.5f)
			{
				red = (red - 0.5f) * p_contrast*2.f + 0.5f;
				green = (green - 0.5f) * p_contrast*2.f + 0.5f;
				blue = (blue - 0.5f) * p_contrast*2.f + 0.5f;
			}
			else
			{
				red = (float(red >= 0.5f) - red)*(p_contrast * 2.f - 1.f) + red;
				green = (float(green >= 0.5f) - green) * (p_contrast * 2.f - 1.f) + green;
				blue = (float(blue >= 0.5f) - blue) * (p_contrast * 2.f - 1.f) + blue;
			}
		}

		//------------------------------

		/*
			Packs the color into a 32-bit integer in ARGB format.
		*/
		colorInt getPacked() const
		{
			return (colorInt(alpha * 0xff) << 24) | (colorInt(red * 0xff) << 16) | (colorInt(green * 0xff) << 8) | (colorInt(blue * 0xff));
		}
	};

	/*
		Linearly interpolates a color between p_start and p_end. Each channel is faded individually.
		If p_progress is 0, p_start is returned. If p_progress is 1, p_end is returned.
	*/
	inline Color interpolate(Color const& p_start, Color const& p_end, float p_progress)
	{
		return Color(
			p_start.red * (1.f - p_progress) + p_end.red*p_progress,
			p_start.green * (1.f - p_progress) + p_end.green*p_progress,
			p_start.blue * (1.f - p_progress) + p_end.blue*p_progress,
			p_start.alpha * (1.f - p_progress) + p_end.alpha*p_progress
		);
	}

	//
	// Font family names
	//

	inline std::string const FONT_FAMILY_ROBOTO{ "Roboto" };
	inline std::string const FONT_FAMILY_MATERIAL_ICONS{ "Material Icons" };

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
		All the default IDs are in Avo::ThemeColors, Avo::ThemeEasings and Avo::ThemeValues.
	*/
	class Theme : public ReferenceCounted
	{
	public:
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

			easings[ThemeEasings::in] = Easing(0.6, 0.0, 0.8, 0.2);
			easings[ThemeEasings::out] = Easing(0.1, 0.9, 0.2, 1.0);
			easings[ThemeEasings::inOut] = Easing(0.4, 0.0, 0.0, 1.0);
			easings[ThemeEasings::symmetricalInOut] = Easing(0.6, 0.0, 0.4, 1.0);

			//------------------------------
			// Values

			values[ThemeValues::hoverAnimationSpeed] = 1.f/6.f; // 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.
			values[ThemeValues::hoverAnimationDuration] = 60.f;
		}
		~Theme() override = default;
	};

	//------------------------------

	/*
		This specifies what is done to fit the image within its bounds.
	*/
	enum class ImageBoundsSizing
	{
		Stretch, // This stretches the image so that it fills its bounds.
		Contain, // This makes sure the image is as big as possible while still keeping the image within its bounds. Aspect ratio is kept.
		Fill // This makes sure the image is so big that it fills its bounds while keeping aspect ratio. Edges may be clipped.
	};

	/*
		This specifies how the pixels of an image are interpolated when it is scaled.
	*/
	enum class ImageScalingMethod
	{
		Pixelated, // Uses nearest neighbor interpolation
		Smooth // Uses linear interpolation
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
		Unknown
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
	class Image : public ProtectedRectangle, protected ReferenceCounted
	{
	private:
		friend class DrawingContext;

		Image* m_implementation;

		Image(Image* p_implementation) :
			m_implementation(p_implementation)
		{
			if (m_implementation)
			{
				m_bounds = m_implementation->m_bounds;
			}
		}

	protected:
		void handleProtectedRectangleChange(Rectangle<float> const& p_old) override
		{
			if (m_implementation)
			{
				m_implementation->setBounds(m_bounds);
			}
		}

	public:
		Image() :
			m_implementation(nullptr)
		{
		}
		Image(Image const& p_image) :
			m_implementation(p_image.m_implementation)
		{
			if (m_implementation)
			{
				m_implementation->remember();
				m_bounds = m_implementation->m_bounds;
			}
		}
		~Image()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
		}

		Image& operator=(Image const& p_image)
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
			m_implementation = p_image.m_implementation;
			if (m_implementation)
			{
				m_implementation->remember();
				m_bounds = m_implementation->m_bounds;
			}
			return *this;
		}
		bool operator==(Image const& p_image) const
		{
			return p_image.m_implementation == m_implementation;
		}
		bool operator!=(Image const& p_image) const
		{
			return p_image.m_implementation != m_implementation;
		}

		/*
			Returns whether the image has been created.
			For example, an image created with Image() is invalid while one that has been created using a DrawingContext is valid.
		*/
		bool getIsValid() const
		{
			return m_implementation;
		}
		/*
			Same as getIsValid.
		*/
		operator bool() const
		{
			return m_implementation;
		}

		/*
			Makes the image invalid and releases the image if it isn't used anywhere else.
		*/
		void destroy()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
			m_implementation = nullptr;
		}

		/*
			Sets a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual void setCropRectangle(Rectangle<float> const& p_rectangle)
		{
			m_implementation->setCropRectangle(p_rectangle);
		}
		/*
			Returns a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual Rectangle<float> const& getCropRectangle() const
		{
			return m_implementation->getCropRectangle();
		}

		/*
			Returns the DIP size of the actual image.
		*/
		virtual Point<uint32> getOriginalSize() const
		{
			return m_implementation->getOriginalSize();
		}
		/*
			Returns the DIP width of the actual image.
		*/
		virtual uint32 getOriginalWidth() const
		{
			return m_implementation->getOriginalWidth();
		}
		/*
			Returns the DIP height of the actual image.
		*/
		virtual uint32 getOriginalHeight() const
		{
			return m_implementation->getOriginalHeight();
		}

		//------------------------------

		/*
			Sets the way the image is fit within its bounds.
		*/
		virtual void setBoundsSizing(ImageBoundsSizing p_sizeMode)
		{
			m_implementation->setBoundsSizing(p_sizeMode);
		}
		/*
			Returns the way the image is fit within its bounds.
		*/
		virtual ImageBoundsSizing getBoundsSizing() const
		{
			return m_implementation->getBoundsSizing();
		}

		/*
			Sets the way the image is positioned within its bounds.

			p_factor represents the coordinates of the point on the image that aligns with the same point but relative to the bounds.
			It is expressed as a factor of the size of the image. For example, if p_factor is (1, 1), the bottom right corner of the image will be
			aligned with the bottom right corner of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual void setBoundsPositioning(Point<float> const& p_factor)
		{
			setBoundsPositioning(p_factor.x, p_factor.y);
		}
		/*
			Sets the way the image is positioned within its bounds.

			p_x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_x is expressed as a factor of the width of the image. For example, if p_x is 1, the right edge of the image will be
			aligned with the right edge of the bounds. 0.5 means the centers will be aligned.
			Same for p_y but vertical coordinates.
		*/
		virtual void setBoundsPositioning(float p_x, float p_y)
		{
			m_implementation->setBoundsPositioning(p_x, p_y);
		}
		/*
			Sets the way the image is positioned within its bounds on the x-axis.

			p_x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_x is expressed as a factor of the width of the image. For example, if p_x is 1, the right edge of the image will be
			aligned with the right edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual void setBoundsPositioningX(float p_x)
		{
			m_implementation->setBoundsPositioningX(p_x);
		}
		/*
			Sets the way the image is positioned within its bounds on the y-axis.

			p_y represents the y coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_y is expressed as a factor of the height of the image. For example, if p_y is 1, the bottom edge of the image will be
			aligned with the bottom edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual void setBoundsPositioningY(float p_y)
		{
			m_implementation->setBoundsPositioningY(p_y);
		}
		/*
			Returns the way the image is positioned within its bounds. See setBoundsPositioning for more info.
		*/
		virtual Point<float> const& getBoundsPositioning() const
		{
			return m_implementation->getBoundsPositioning();
		}
		/*
			Returns the way the image is positioned within its bounds on the x-axis. See setBoundsPositioningX for more info.
		*/
		virtual float getBoundsPositioningX() const
		{
			return m_implementation->getBoundsPositioningX();
		}
		/*
			Returns the way the image is positioned within its bounds on the y-axis. See setBoundsPositioningY for more info.
		*/
		virtual float getBoundsPositioningY() const
		{
			return m_implementation->getBoundsPositioningY();
		}

		//------------------------------

		/*
			Sets how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual void setScalingMethod(ImageScalingMethod p_scalingMethod)
		{
			m_implementation->setScalingMethod(p_scalingMethod);
		}
		/*
			Returns how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual ImageScalingMethod getScalingMethod() const
		{
			return m_implementation->getScalingMethod();
		}

		//------------------------------

		/*
			Sets how opaque the image is being drawn.
		*/
		virtual void setOpacity(float p_opacity)
		{
			m_implementation->setOpacity(p_opacity);
		}
		/*
			Returns how opaque the image is being drawn.
		*/
		virtual float getOpacity() const
		{
			return m_implementation->getOpacity();
		}

		//------------------------------

		/*
			Returns the drawn width of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual float getInnerWidth() const
		{
			return m_implementation->getInnerWidth();
		}
		/*
			Returns the drawn height of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual float getInnerHeight() const
		{
			return m_implementation->getInnerHeight();
		}
		/*
			Returns the drawn size of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual Point<float> getInnerSize() const
		{
			return m_implementation->getInnerSize();
		}
		/*
			Returns the drawn inner bounds of the image within the outer bounds, calculated using the positioning options, sizing options and the crop rectangle.
		*/
		virtual Rectangle<float> getInnerBounds() const
		{
			return m_implementation->getInnerBounds();
		}

		//------------------------------

		/*
			Returns a pointer to the os-specific image object.
		*/
		virtual void* getHandle() const
		{
			return m_implementation->getHandle();
		}
	};

	//------------------------------

	enum class WordWrapping
	{
		Emergency, // Keeps words whole unless a word is wider than the maximum width.
		WholeWord, // Only wraps between words to prevent overflow.
		Always, // Always wraps to the next line to prevent overflow.
		Never // Allows overflow, never wraps.
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
		UltraBlack = 950 // Most thick option
	};

	enum class FontStyle
	{
		Normal,
		Oblique,
		Italic
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
		UltraStretched = 9 // Most stretched
	};

	enum class TextAlign
	{
		Left,
		Center,
		Right,
		Fill // Stretches the spaces of the text to make the left and right edges of the text line up with the bounds of the text.
	};
	enum class ReadingDirection
	{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop
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
	class Text : public ProtectedRectangle, protected ReferenceCounted
	{
	private:
		friend class DrawingContext;

		Text* m_implementation;

		Text(Text* p_implementation) :
			m_implementation(p_implementation)
		{
		}

	public:
		Text() :
			m_implementation(nullptr)
		{
		}
		Text(Text const& p_text) :
			m_implementation(p_text.m_implementation)
		{
			if (m_implementation)
			{
				m_implementation->remember();
			}
		}
		~Text()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
		}

		Text& operator=(Text const& p_text)
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
			m_implementation = p_text.m_implementation;
			if (m_implementation)
			{
				m_implementation->remember();
			}
			return *this;
		}
		bool operator==(Text const& p_text) const
		{
			return m_implementation == p_text.m_implementation;
		}
		bool operator!=(Text const& p_text) const
		{
			return m_implementation != p_text.m_implementation;
		}

		bool getIsValid() const
		{
			return m_implementation;
		}
		operator bool() const
		{
			return m_implementation;
		}
		/*
			Returns whether the text has been created and can be used.
			For example, an image created with Image() is invalid while one that has been created using a DrawingContext is valid.
		*/
		void destroy()
		{
			if (m_implementation)
			{
				m_implementation->forget();
			}
			m_implementation = nullptr;
		}

		/*
			Sets the rules for inserting line breaks in the text to avoid overflow.
		*/
		virtual void setWordWrapping(WordWrapping p_wordWrapping)
		{
			m_implementation->setWordWrapping(p_wordWrapping);
		}
		/*
			Returns the type of rules used for inserting line breaks in the text to avoid overflow.
		*/
		virtual WordWrapping getWordWrapping()
		{
			return m_implementation->getWordWrapping();
		}

		/*
			Sets the size of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual void fitSizeToText()
		{
			m_implementation->fitSizeToText();
		}
		/*
			Sets the width of the bounding box to fit the text.
		*/
		virtual void fitWidthToText()
		{
			m_implementation->fitWidthToText();
		}
		/*
			Sets the height of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual void fitHeightToText()
		{
			m_implementation->fitHeightToText();
		}
		/*
			Returns the smallest size to contain the actual text.
			If getIsTopTrimmed() == false, the height includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual Point<float> getMinimumSize()
		{
			return m_implementation->getMinimumSize();
		}
		/*
			Returns the smallest width to contain the actual text.
		*/
		virtual float getMinimumWidth()
		{
			return m_implementation->getMinimumWidth();
		}
		/*
			Returns the smallest height to contain the actual text.
			If getIsTopTrimmed() == false, this includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual float getMinimumHeight()
		{
			return m_implementation->getMinimumHeight();
		}

		//------------------------------

		/*
			Sets whether the top of the text is trimmed so that there is no space between the top of the tallest
			character of the text and the top edge of the bounds.

			If this is false, the text is positioned within the bounds so that the baseline is at a fixed position,
			and there may be space above the characters in the text to allow this. This is the default.

			Setting this to true can be useful when you want to perfectly center text vertically.
		*/
		virtual void setIsTopTrimmed(bool p_isTopTrimmed)
		{
			m_implementation->setIsTopTrimmed(p_isTopTrimmed);
		}
		/*
			Returns whether the top of the text is trimmed so that there is no space between the top of the tallest
			character of the text and the top edge of the bounds. This is false by default.
		*/
		virtual bool getIsTopTrimmed()
		{
			return m_implementation->getIsTopTrimmed();
		}

		//------------------------------

		/*
			Returns the 2d position of a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position returned is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual Point<float> getCharacterPosition(uint32 p_characterIndex, bool p_isRelativeToOrigin = false)
		{
			return m_implementation->getCharacterPosition(p_characterIndex, p_isRelativeToOrigin);
		}
		/*
			Returns the width and height of a character in the text, specified by its index in the string.
		*/
		virtual Point<float> getCharacterSize(uint32 p_characterIndex)
		{
			return m_implementation->getCharacterSize(p_characterIndex);
		}
		/*
			Returns a rectangle enclosing a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position of the bounds returned is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual Rectangle<float> getCharacterBounds(uint32 p_characterIndex, bool p_isRelativeToOrigin = false)
		{
			return m_implementation->getCharacterBounds(p_characterIndex, p_isRelativeToOrigin);
		}

		/*
			Returns the index of the character which is nearest to a point.
			p_isRelativeToOrigin is whether the position given is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual uint32 getNearestCharacterIndex(Point<float> const& p_point, bool p_isRelativeToOrigin = false)
		{
			return m_implementation->getNearestCharacterIndex(p_point, p_isRelativeToOrigin);
		}
		/*
			Returns the index of the character which is nearest to a point.

			p_isRelativeToOrigin is whether the position given is relative to the origin of the drawing context.
			If not, it is relative to the bounds of the text.
		*/
		virtual uint32 getNearestCharacterIndex(float p_pointX, float p_pointY, bool p_isRelativeToOrigin = false)
		{
			return m_implementation->getNearestCharacterIndex(p_pointX, p_pointY, p_isRelativeToOrigin);
		}
		/*
			Returns the index and position of the character which is nearest to a point.

			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterPosition is a pointer to the 2d position to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context.
			If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndPosition(Point<float> const& p_point, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false)
		{
			return getNearestCharacterIndexAndPosition(p_point, p_outCharacterIndex, p_outCharacterPosition, p_isRelativeToOrigin);
		}
		/*
			Returns the index and position of the character which is nearest to a point.

			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterPosition is a pointer to the 2d position to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context.
			If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndPosition(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false)
		{
			m_implementation->getNearestCharacterIndexAndPosition(p_pointX, p_pointY, p_outCharacterIndex, p_outCharacterPosition, p_isRelativeToOrigin);
		}
		/*
			Returns the index and bounds of the character which is nearest to a point.

			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterBounds is a pointer to the bounding rectangle to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndBounds(Point<float> const& p_point, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false)
		{
			getNearestCharacterIndexAndBounds(p_point, p_outCharacterIndex, p_outCharacterBounds, p_isRelativeToOrigin);
		}
		/*
			Returns the index and bounds of the character which is nearest to a point.

			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterBounds is a pointer to the bounding rectangle to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndBounds(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false)
		{
			m_implementation->getNearestCharacterIndexAndBounds(p_pointX, p_pointY, p_outCharacterIndex, p_outCharacterBounds, p_isRelativeToOrigin);
		}

		//------------------------------

		/*
			Sets how the text is placed within the bounds.
		*/
		virtual void setTextAlign(TextAlign p_textAlign)
		{
			m_implementation->setTextAlign(p_textAlign);
		}
		/*
			Returns how the text is placed within the bounds.
		*/
		virtual TextAlign getTextAlign()
		{
			return m_implementation->getTextAlign();
		}

		//------------------------------

		/*
			Sets the layout direction of the text.
		*/
		virtual void setReadingDirection(ReadingDirection p_readingDirection)
		{
			m_implementation->setReadingDirection(p_readingDirection);
		}
		/*
			Returns the layout direction of the text.
		*/
		virtual ReadingDirection getReadingDirection()
		{
			return m_implementation->getReadingDirection();
		}

		//------------------------------

		/*
			Sets the font family to be used in a section of the text.

			p_name is the name of the font family.

			p_startPosition is the position of the first character to use this font.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this font.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontFamily(std::string const& p_name, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setFontFamily(p_name, p_startPosition, p_length);
		}

		//------------------------------

		/*
			Sets the spacing between characters in a section of the text.

			p_startPosition is the position of the first character to use this spacing.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this spacing.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setCharacterSpacing(float p_characterSpacing, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setCharacterSpacing(p_characterSpacing, p_startPosition, p_length);
		}
		/*
			Sets the leading and trailing spacing of the characters in a section of the text.

			p_leading is the spacing before the characters of the text.
			p_trailing is the spacing after the characters of the text.
			p_startPosition is the position of the first character to use this spacing.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this spacing.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setCharacterSpacing(float p_leading, float p_trailing, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setCharacterSpacing(p_leading, p_trailing, p_startPosition, p_length);
		}
		/*
			Returns the spacing before one of the characters.
		*/
		virtual float getLeadingCharacterSpacing(int32 p_characterIndex = 0)
		{
			return m_implementation->getLeadingCharacterSpacing(p_characterIndex);
		}
		/*
			Returns the spacing after one of the characters.
		*/
		virtual float getTrailingCharacterSpacing(int32 p_characterIndex = 0)
		{
			return m_implementation->getTrailingCharacterSpacing(p_characterIndex);
		}

		//------------------------------

		/*
			Sets the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual void setLineHeight(float p_lineHeight)
		{
			m_implementation->setLineHeight(p_lineHeight);
		}
		/*
			Returns the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual float getLineHeight()
		{
			return m_implementation->getLineHeight();
		}

		//------------------------------

		/*
			Sets the thickness of characters in a section of the text.

			p_startPosition is the position of the first character to use this font weight.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this font weight.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontWeight(FontWeight p_fontWeight, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setFontWeight(p_fontWeight, p_startPosition, p_length);
		}
		/*
			Returns the weight/thickness of a character in the text.
		*/
		virtual FontWeight getFontWeight(uint32 p_characterPosition = 0)
		{
			return m_implementation->getFontWeight(p_characterPosition);
		}

		//------------------------------

		/*
			Sets the font style in a section of the text.

			p_startPosition is the position of the first character to use this font style.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this font style.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontStyle(FontStyle p_fontStyle, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setFontStyle(p_fontStyle, p_startPosition, p_length);
		}
		/*
			Returns the style of a character in the text.
		*/
		virtual FontStyle getFontStyle(uint32 p_characterPosition = 0)
		{
			return m_implementation->getFontStyle(p_characterPosition);
		}

		//------------------------------

		/*
			Sets the font stretch in a section of the text. Not all fonts support this.

			p_startPosition is the position of the first character to use this font stretch.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this font stretch.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontStretch(FontStretch p_fontStretch, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setFontStretch(p_fontStretch, p_startPosition, p_length);
		}
		/*
			Returns the font stretch of a character in the text.
		*/
		virtual FontStretch getFontStretch(uint32 p_characterPosition = 0)
		{
			return m_implementation->getFontStretch(p_characterPosition);
		}

		//------------------------------

		/*
			Sets the font size in a section of the text.

			p_startPosition is the position of the first character to use this font size.
			If this is negative, it is relative to the end of the text.

			p_length is the number of characters to use this font size.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontSize(float p_fontSize, int32 p_startPosition = 0, int32 p_length = 0)
		{
			m_implementation->setFontSize(p_fontSize, p_startPosition, p_length);
		}
		/*
			Returns the size (height) of a character in the text.
		*/
		virtual float getFontSize(uint32 p_characterPosition = 0)
		{
			return m_implementation->getFontSize(p_characterPosition);
		}

		//------------------------------

		virtual std::string const& getString()
		{
			return m_implementation->getString();
		}

		//------------------------------

		/*
			Returns a pointer to an OS-specific text object.
		*/
		virtual void* getHandle()
		{
			return m_implementation->getHandle();
		}

		//------------------------------

		using ProtectedRectangle::setBounds;
		void setBounds(float p_left, float p_top, float p_right, float p_bottom) override
		{
			m_implementation->ProtectedRectangle::setBounds(p_left, p_top, p_right, p_bottom);
		}
		Rectangle<float> const& getBounds() const override
		{
			return m_implementation->ProtectedRectangle::getBounds();
		}

		using ProtectedRectangle::move;
		void move(float p_offsetX, float p_offsetY) override
		{
			m_implementation->ProtectedRectangle::move(p_offsetX, p_offsetY);
		}
		void moveX(float p_offsetX) override
		{
			m_implementation->ProtectedRectangle::moveX(p_offsetX);
		}
		void moveY(float p_offsetY) override
		{
			m_implementation->ProtectedRectangle::moveY(p_offsetY);
		}

		using ProtectedRectangle::setTopLeft;
		void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) override
		{
			m_implementation->ProtectedRectangle::setTopLeft(p_left, p_top, p_willKeepSize);
		}
		Point<float> getTopLeft() const override
		{
			return m_implementation->ProtectedRectangle::getTopLeft();
		}

		using ProtectedRectangle::setTopRight;
		void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) override
		{
			m_implementation->ProtectedRectangle::setTopRight(p_right, p_top, p_willKeepSize);
		}
		Point<float> getTopRight() const override
		{
			return m_implementation->ProtectedRectangle::getTopRight();
		}

		using ProtectedRectangle::setBottomLeft;
		void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) override
		{
			m_implementation->ProtectedRectangle::setBottomLeft(p_left, p_bottom, p_willKeepSize);
		}
		Point<float> getBottomLeft() const override
		{
			return m_implementation->ProtectedRectangle::getBottomLeft();
		}

		using ProtectedRectangle::setBottomRight;
		void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) override
		{
			m_implementation->ProtectedRectangle::setBottomRight(p_right, p_bottom, p_willKeepSize);
		}
		Point<float> getBottomRight() const override
		{
			return m_implementation->ProtectedRectangle::getBottomRight();
		}

		using ProtectedRectangle::setCenter;
		void setCenter(float p_x, float p_y) override
		{
			m_implementation->ProtectedRectangle::setCenter(p_x, p_y);
		}
		Point<float> getCenter() const override
		{
			return m_implementation->ProtectedRectangle::getCenter();
		}
		void setCenterX(float p_x) override
		{
			m_implementation->ProtectedRectangle::setCenterX(p_x);
		}
		float getCenterX() const override
		{
			return m_implementation->ProtectedRectangle::getCenterX();
		}
		void setCenterY(float p_y) override
		{
			m_implementation->ProtectedRectangle::setCenterY(p_y);
		}
		float getCenterY() const override
		{
			return m_implementation->ProtectedRectangle::getCenterY();
		}
		void setLeft(float p_left, bool p_willKeepWidth = true) override
		{
			m_implementation->ProtectedRectangle::setLeft(p_left, p_willKeepWidth);
		}
		float getLeft() const override
		{
			return m_implementation->ProtectedRectangle::getLeft();
		}
		void setTop(float p_top, bool p_willKeepHeight = true) override
		{
			m_implementation->ProtectedRectangle::setTop(p_top, p_willKeepHeight);
		}
		float getTop() const override
		{
			return m_implementation->ProtectedRectangle::getTop();
		}
		void setRight(float p_right, bool p_willKeepWidth = true) override
		{
			m_implementation->ProtectedRectangle::setRight(p_right, p_willKeepWidth);
		}
		float getRight() const override
		{
			return m_implementation->ProtectedRectangle::getRight();
		}
		void setBottom(float p_bottom, bool p_willKeepHeight = true) override
		{
			m_implementation->ProtectedRectangle::setBottom(p_bottom, p_willKeepHeight);
		}
		float getBottom() const override
		{
			return m_implementation->ProtectedRectangle::getBottom();
		}
		void setWidth(float p_width) override
		{
			m_implementation->ProtectedRectangle::setWidth(p_width);
		}
		float getWidth() const override
		{
			return m_implementation->ProtectedRectangle::getWidth();
		}
		void setHeight(float p_height) override
		{
			m_implementation->ProtectedRectangle::setHeight(p_height);
		}
		float getHeight() const override
		{
			return m_implementation->ProtectedRectangle::getHeight();
		}

		using ProtectedRectangle::setSize;
		void setSize(float p_width, float p_height) override
		{
			m_implementation->ProtectedRectangle::setSize(p_width, p_height);
		}
		Point<float> getSize() const override
		{
			return m_implementation->ProtectedRectangle::getSize();
		}

		using ProtectedRectangle::getIsIntersecting;
		bool getIsIntersecting(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			return m_implementation->ProtectedRectangle::getIsIntersecting(p_left, p_top, p_right, p_bottom);
		}
		bool getIsContaining(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			return m_implementation->ProtectedRectangle::getIsContaining(p_left, p_top, p_right, p_bottom);
		}

		using ProtectedRectangle::getIsContaining;
		bool getIsContaining(float p_x, float p_y) const override
		{
			return m_implementation->ProtectedRectangle::getIsContaining(p_x, p_y);
		}
	};

	class LinearGradient : public ProtectedReferenceCounted<LinearGradient>
	{
	private:
		friend class DrawingContext;

		LinearGradient(LinearGradient* p_implementation) :
			ProtectedReferenceCounted(p_implementation)
		{
		}

	public:
		LinearGradient()
		{
		}
		LinearGradient(LinearGradient const& p_gradient) :
			ProtectedReferenceCounted(p_gradient)
		{
		}

		virtual void* getHandle() const
		{
			return m_implementation->getHandle();
		}

		/*
			Sets an offset in the start and end positions.
		*/
		virtual void setOffset(Point<float> const& p_offset)
		{
			setOffset(p_offset.x, p_offset.y);
		}
		/*
			Sets an offset in the start and end positions.
		*/
		virtual void setOffset(float p_x, float p_y)
		{
			m_implementation->setOffset(p_x, p_y);
		}
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual void setOffsetX(float p_x)
		{
			m_implementation->setOffsetX(p_x);
		}
		/*
			Sets the vertical offset in the start position.
		*/
		virtual void setOffsetY(float p_y)
		{
			m_implementation->setOffsetY(p_y);
		}

		/*
			Returns the offset in the start and end positions.
		*/
		virtual Point<float> const& getOffset() const
		{
			return m_implementation->getOffset();
		}
		/*
			Returns the horizontal offset in the start and end positions.
		*/
		virtual float getOffsetX() const
		{
			return m_implementation->getOffsetX();
		}
		/*
			Returns the vertical offset in the start and end positions.
		*/
		virtual float getOffsetY() const
		{
			return m_implementation->getOffsetY();
		}

		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual void setStartPosition(Point<float> const& p_startPosition)
		{
			setStartPosition(p_startPosition.x, p_startPosition.y);
		}
		/*
			Sets the coordinates relative to the origin where the gradient will start.
		*/
		virtual void setStartPosition(float p_x, float p_y)
		{
			m_implementation->setStartPosition(p_x, p_y);
		}
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual Point<float> const& getStartPosition() const
		{
			return m_implementation->getStartPosition();
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionX() const
		{
			return m_implementation->getStartPositionX();
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionY() const
		{
			return m_implementation->getStartPositionY();
		}

		/*
			Sets the coordinates relative to the origin where the gradient will end.
		*/
		virtual void setEndPosition(Point<float> const& p_endPosition)
		{
			setEndPosition(p_endPosition.x, p_endPosition.y);
		}
		/*
			Sets the coordinates relative to the origin where the gradient will end.
		*/
		virtual void setEndPosition(float p_x, float p_y)
		{
			m_implementation->setEndPosition(p_x, p_y);
		}
		/*
			Returns the coordinates relative to the origin where the gradient will end.
		*/
		virtual Point<float> const& getEndPosition() const
		{
			return m_implementation->getEndPosition();
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will end.
		*/
		virtual float getEndPositionX() const
		{
			return m_implementation->getEndPositionX();
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will end.
		*/
		virtual float getEndPositionY() const
		{
			return m_implementation->getEndPositionY();
		}
	};

	class RadialGradient : public ProtectedReferenceCounted<RadialGradient>
	{
	private:
		friend class DrawingContext;

		RadialGradient(RadialGradient* p_implementation) :
			ProtectedReferenceCounted(p_implementation)
		{
		}

	public:
		RadialGradient()
		{
		}
		RadialGradient(RadialGradient const& p_gradient) :
			ProtectedReferenceCounted(p_gradient)
		{
		}

		virtual void* getHandle() const
		{
			return m_implementation->getHandle();
		}

		/*
			Sets an offset in the start position.
		*/
		virtual void setOffset(float p_x, float p_y)
		{
			m_implementation->setOffset(p_x, p_y);
		}
		/*
			Sets an offset in the start position.
		*/
		virtual void setOffset(Point<float> const& p_offset)
		{
			setOffset(p_offset.x, p_offset.y);
		}
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual void setOffsetX(float p_x)
		{
			m_implementation->setOffsetX(p_x);
		}
		/*
			Sets the vertical offset in the start position.
		*/
		virtual void setOffsetY(float p_y)
		{
			m_implementation->setOffsetY(p_y);
		}
		/*
			Returns the offset in the start position.
		*/
		virtual Point<float> const& getOffset() const
		{
			return m_implementation->getOffset();
		}
		/*
			Returns the horizontal offset in the start position.
		*/
		virtual float getOffsetX() const
		{
			return m_implementation->getOffsetX();
		}
		/*
			Returns the vertical offset in the start position.
		*/
		virtual float getOffsetY() const
		{
			return m_implementation->getOffsetY();
		}

		/*
			Sets the coordinates relative to the origin where the gradient will start.
		*/
		virtual void setStartPosition(float p_x, float p_y)
		{
			m_implementation->setStartPosition(p_x, p_y);
		}
		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual void setStartPosition(Point<float> const& p_startPosition)
		{
			setStartPosition(p_startPosition.x, p_startPosition.y);
		}
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual Point<float> const& getStartPosition() const
		{
			return m_implementation->getStartPosition();
		}
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionX() const
		{
			return m_implementation->getStartPositionX();
		}
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionY() const
		{
			return m_implementation->getStartPositionY();
		}

		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(float p_radiusX, float p_radiusY)
		{
			m_implementation->setRadius(p_radiusX, p_radiusY);
		}
		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(float p_radius)
		{
			setRadius(p_radius, p_radius);
		}
		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(Point<float> const& p_radius)
		{
			setRadius(p_radius.x, p_radius.y);
		}
		/*
			Returns the horizontal and vertical size of the gradient.
		*/
		virtual Point<float> const& getRadius() const
		{
			return m_implementation->getRadius();
		}
		/*
			Returns the horizontal size of the gradient.
		*/
		virtual float getRadiusX() const
		{
			return m_implementation->getRadiusX();
		}
		/*
			Returns the vertical size of the gradient.
		*/
		virtual float getRadiusY() const
		{
			return m_implementation->getRadiusY();
		}
	};

	/*
		A class representing a position and a color, used to make a gradient.
		The position between 0 and 1 and is relative to the start and end positions if it's linear,
		and relative to the start position and radius if it's radial.
	*/
	class GradientStop
	{
	public:
		Avo::Color color;
		float position;

		GradientStop(Avo::Color const& p_color, float p_position) :
			color(p_color), position(p_position)
		{
		}
	};

	/*
		Platform-specific interface for cached geometry that can be created and drawn by a DrawingContext.
		Used to draw more efficiently.
	*/
	class Geometry : public ProtectedReferenceCounted<Geometry> 
	{ 
	private:
		friend class DrawingContext;

		Geometry(Geometry* p_implementation) :
			ProtectedReferenceCounted(p_implementation)
		{
		}

	public:
		Geometry()
		{
		}
		Geometry(Geometry const& p_geometry) :
			ProtectedReferenceCounted(p_geometry)
		{
		}
	};

	/*
		Used to store the drawing state of a DrawingContext, which includes the current transformations.
		Create one with DrawingContext::createDrawingState().
	*/
	class DrawingState : public ReferenceCounted { };

	class TextProperties
	{
	public:
		std::string fontFamilyName{ FONT_FAMILY_ROBOTO };

		FontWeight fontWeight{ FontWeight::Medium };
		FontStyle fontStyle{ FontStyle::Normal };
		FontStretch fontStretch{ FontStretch::Medium };
		TextAlign textAlign{ TextAlign::Left };
		ReadingDirection readingDirection{ ReadingDirection::LeftToRight };

		float characterSpacing{ 0.f }; // Only supported for text objects.
		float lineHeight{ 1.f };
		float fontSize{ 22.f };
	};

	enum class LineCap
	{
		Flat,
		Round,
		Square,
		Triangle
	};

	enum class LineJoin
	{
		Bevel,
		Miter,
		Round
	};

	enum class LineDashStyle
	{
		Solid,
		Dash,
		Dot,
		DashDot,
		DashDotDot,
		Custom
	};

	/*
		A drawing context interface, created by a GUI to be used to create objects
		like text and images (and more) as well as to draw graphics in views.
	*/
	class DrawingContext : public ReferenceCounted
	{
	protected:
		TextProperties m_textProperties;

		static Image createImageFromImplementation(Image* p_implementation)
		{
			return Image(p_implementation);
		}
		static Text createTextFromImplementation(Text* p_implementation)
		{
			return Text(p_implementation);
		}
		static LinearGradient createLinearGradientFromImplementation(LinearGradient* p_implementation)
		{
			return LinearGradient(p_implementation);
		}
		static RadialGradient createRadialGradientFromImplementation(RadialGradient* p_implementation)
		{
			return RadialGradient(p_implementation);
		}
		static Geometry createGeometryFromImplementation(Geometry* p_geometry)
		{
			return Geometry(p_geometry);
		}
		static Geometry* getGeometryImplementation(Geometry const& p_geometry)
		{
			return p_geometry.m_implementation;
		}

	public:
		/*
			Returns the image format of the given image file.
			Only the first 8 bytes of the file is needed.
		*/
		static auto getImageFormatOfFile(uint64 p_fileData)
		{
			if (!std::strncmp((char const*)&p_fileData, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8))
			{
				return Avo::ImageFormat::Png;
			}
			else if (!std::strncmp((char const*)&p_fileData, "\xFF\xD8\xFF", 3))
			{
				return Avo::ImageFormat::Jpeg;
			}
			else if (!std::strncmp((char const*)&p_fileData, "\x00\x00\x01\x00", 4) ||
			         !std::strncmp((char const*)&p_fileData, "\x00\x00\x02\x00", 4))
			{
				return Avo::ImageFormat::Ico;
			}
			return Avo::ImageFormat::Unknown;
		}
		/*
			Returns the image format of the given image file.
		*/
		static Avo::ImageFormat getImageFormatOfFile(std::string const& p_filePath)
		{
			char signatureBytes[8];

			std::ifstream fileStream(p_filePath);
			fileStream.read(signatureBytes, 8);
			fileStream.close();

			return getImageFormatOfFile(*(uint64*)signatureBytes);
		}
		/*
			Returns the image format of the given image file.
		*/
		static Avo::ImageFormat getImageFormatOfFile(char const* p_filePath)
		{
			char signatureBytes[8];

			std::ifstream fileStream(p_filePath);
			fileStream.read(signatureBytes, 8);
			fileStream.close();

			return getImageFormatOfFile(*(uint64*)signatureBytes);
		}

		//------------------------------

		/*
			Initializes drawing. The GUI calls this for you.
		*/
		virtual void beginDrawing() = 0;
		/*
			Finishes the drawing and shows it. The GUI calls this for you.
		*/
		virtual void finishDrawing(std::vector<Rectangle<float>> const& p_updatedRectangles) = 0;

		//------------------------------

		/*
			Creates a drawing state object.
			It can be re-used and you can call saveDrawingState and restoreDrawingState as many times as you want.
		*/
		virtual DrawingState* createDrawingState() = 0;
		/*
			Saves the internal drawing state of the drawing context in a DrawingState object.
		*/
		virtual void saveDrawingState(DrawingState* p_drawingState) = 0;
		/*
			Loads the internal drawing state of the drawing context from a DrawingState object.
		*/
		virtual void restoreDrawingState(DrawingState* p_drawingState) = 0;

		//------------------------------

		/*
			Sets whether the target is fullscreen or windowed.
		*/
		virtual void setIsFullscreen(bool p_isFullscreen) = 0;
		/*
			Switches between windowed and fullscreen mode.
			If it is currently windowed, it switches to fullscreen, and the other way around.
		*/
		virtual void switchFullscreen() = 0;
		/*
			Returns whether the target is fullscreen or windowed.
		*/
		virtual bool getIsFullscreen() = 0;

		//------------------------------

		/*
			Enables synchronization with the monitor.
		*/
		virtual void enableVsync() = 0;
		/*
			Disables synchronization with the monitor.
		*/
		virtual void disableVsync() = 0;
		/*
			Returns whether presentation is synchronized with the monitor.
		*/
		virtual bool getIsVsyncEnabled() = 0;

		//------------------------------

		/*
			Sets the color that the target is filled with before any drawing.
		*/
		virtual void setBackgroundColor(Color const& p_color) = 0;
		/*
			Returns the color that the target is filled with before any drawing.
		*/
		virtual Color getBackgroundColor() = 0;

		//------------------------------

		/*
			Returns the DPI that the DrawingContext is scaling all DIP units to.
			All coordinates that the DrawingContext works with are in DIPs.
		*/
		virtual float getDpi() = 0;
		/*
			Sets the DPI that the DrawingContext is scaling all DIP units to.
			It is not recommended to call this manually, since any DPI changes are updated with this method automatically from the corresponding window.
			All coordinates that the DrawingContext works with are in DIPs.
		*/
		virtual void setDpi(float p_dpi) = 0;

		//------------------------------

		/*
			Moves the screen position of the coordinate (0, 0).
		*/
		virtual void moveOrigin(Point<float> const& p_offset) = 0;
		/*
			Moves the screen position of the coordinate (0, 0).
		*/
		virtual void moveOrigin(float p_offsetX, float p_offsetY) = 0;
		/*
			Sets the screen position of the coordinate (0, 0).
		*/
		virtual void setOrigin(Point<float> const& p_origin) = 0;
		/*
			Sets the screen position of the coordinate (0, 0).
		*/
		virtual void setOrigin(float p_x, float p_y) = 0;
		/*
			Returns the screen position of the coordinate (0, 0).
		*/
		virtual Point<float> getOrigin() = 0;

		//------------------------------

		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin.
		*/
		virtual void scale(float p_scale) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin.
		*/
		virtual void scale(float p_scaleX, float p_scaleY) = 0;
		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scale, Point<float> const& p_origin) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual void scale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) = 0;
		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scale, float p_originX, float p_originY) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. The origin is shifted so that everything is scaled towards the origin
			parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) = 0;
		/*
			sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin.
		*/
		virtual void setScale(float p_scale) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller than normal. Everything will be scaled towards the origin.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY) = 0;
		/*
			Sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scale, Point<float> const& p_origin) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics drawing so that
			it is bigger or smaller than normal. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) = 0;
		/*
			Sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scale, float p_originX, float p_originY) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. The origin is shifted so that everything is scaled towards the origin
			parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) = 0;
		/*
			Returns the sizing factor which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual Point<float> const& getScale() = 0;
		/*
			Returns the sizing factor for the x-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual float getScaleX() = 0;
		/*
			Returns the sizing factor for the y-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual float getScaleY() = 0;

		//------------------------------

		/*
			Rotates all future graphics drawing, with an angle in radians. Graphics will be rotated relative to the origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual void rotate(float p_radians) = 0;
		/*
			Rotates all future graphics drawing, with an angle in radians.
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual void rotate(float p_radians, Point<float> const& p_origin) = 0;
		/*
			Rotates all future graphics drawing, with an angle in radians.
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
		*/
		virtual void rotate(float p_radians, float p_originX, float p_originY) = 0;

		//------------------------------

		/*
			Resets all graphics drawing transformations, so that every coordinate used in any drawing operation is unaltered, and relative to the top-left corner of the target.
		*/
		virtual void resetTransformations() = 0;

		//------------------------------

		/*
			Resizes the drawing buffers for the window. The GUI calls this for you when it is being resized.
			The size is expressed in dips.
		*/
		virtual void setSize(Point<float> const& p_size) = 0;
		/*
			Resizes the drawing buffers for the window. The GUI calls this for you when it is being resized.
			The size is expressed in dips.
		*/
		virtual void setSize(float p_width, float p_height) = 0;
		/*
			Returns the size of the drawing buffers for the window, in dips.
		*/
		virtual Point<float> getSize() = 0;

		//------------------------------

		/*
			Clears the whole buffer with the specified color.
		*/
		virtual void clear(Color const& p_color) = 0;
		/*
			Clears the whole buffer with a transparent background.
		*/
		virtual void clear() = 0;

		//------------------------------

		/*
			Draws a filled rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle) = 0;
		/*
			Draws a filled rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			Draws a filled rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_width, float p_height) = 0;

		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rectangle with custom corners at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(float p_width, float p_height, float p_radius) = 0;

		//------------------------------

		/*
			Draws a rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a filled circle using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_position is the center position of the circle.
		*/
		virtual void fillCircle(Point<float> const& p_position, float p_radius) = 0;
		/*
			Draws a filled circle using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_x is the horizontal center position of the circle.
			p_y is the vertical center position of the circle.
		*/
		virtual void fillCircle(float p_x, float p_y, float p_radius) = 0;

		/*
			Draws a circle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(Point<float> const& p_position, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a circle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.

			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a straight line between two points using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void drawLine(Point<float> const& p_point_0, Point<float> const& p_point_1, float p_thickness = 1.f) = 0;
		/*
			Draws a straight line between two points using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) = 0;

		//------------------------------

		/*
			Draws the edge of a custom shape.

			p_vertices is a vector of the points that make up the shape.
			p_lineThickness is how thicc the edges of the shape are.
			p_isClosed is whether the last vertex will be connected to the first one to close the shape.
		*/
		virtual void strokeShape(std::vector<Point<float>> const& p_vertices, float p_lineThickness, bool p_isClosed = false) = 0;
		/*
			Draws the edge of a custom shape.

			p_vertices is an array of points that make up the shape.
			p_numberOfVertices is the number of points that make up the shape.
			p_lineThickness is how thicc the edges of the shape are.
			p_isClosed is whether the last vertex will be connected to the first one to close the shape.
		*/
		virtual void strokeShape(Point<float> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed = false) = 0;
		/*
			Fills a custom shape with the current color or gradient.

			p_shape is a vector of points that make up the shape.
		*/
		virtual void fillShape(std::vector<Point<float>> const& p_vertices) = 0;
		/*
			Fills a custom shape with the current color or gradient.

			p_vertices is an array of points that make up the shape.
			p_numberOfVertices is he number of points that make up the shape.
		*/
		virtual void fillShape(Point<float> const* p_vertices, uint32 p_numberOfVertices) = 0;

		//------------------------------

		/*
			Draws a stroked cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the stroke color with setColor().
		*/
		virtual void strokeGeometry(Geometry const& p_geometry, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a filled cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the fill color with setColor().
		*/
		virtual void fillGeometry(Geometry const& p_geometry) = 0;

		//------------------------------

		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createRoundedRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createRoundedRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, float p_radius, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createRoundedRectangleGeometry(Rectangle<float> const& p_rectangle, float p_radius, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createRoundedRectangleGeometry(float p_width, float p_height, float p_radius, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createRoundedRectangleGeometry(Point<float> const& p_size, float p_radius, bool p_isStroked = false) = 0;

		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createCornerRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createCornerRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_corners, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createCornerRectangleGeometry(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createCornerRectangleGeometry(float p_width, float p_height, RectangleCorners const& p_corners, bool p_isStroked = false) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createCornerRectangleGeometry(Point<float> const& p_size, RectangleCorners const& p_corners, bool p_isStroked = false) = 0;

		//------------------------------

		/*
			Creates a geometry object which represents a polygon.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createPolygonGeometry(std::vector<Point<float>> const& p_vertices, bool p_isStroked = false, bool p_isClosed = true) = 0;
		/*
			Creates a geometry object which represents a polygon.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry createPolygonGeometry(Point<float> const* p_vertices, uint32 p_numberOfVertices, bool p_isStroked = false, bool p_isClosed = true) = 0;

		//------------------------------

		/*
			Changes the way both start- and endpoints of lines are drawn.
		*/
		virtual void setLineCap(LineCap p_lineCap) = 0;
		/*
			Changes the way startpoints of lines are drawn.
		*/
		virtual void setStartLineCap(LineCap p_lineCap) = 0;
		/*
			Changes the way endpoints of lines are drawn.
		*/
		virtual void setEndLineCap(LineCap p_lineCap) = 0;
		/*
			Returns the way startpoints of lines are drawn.
		*/
		virtual LineCap getStartLineCap() = 0;
		/*
			Returns the way endpoints of lines are drawn.
		*/
		virtual LineCap getEndLineCap() = 0;

		//------------------------------

		/*
			Sets how and if lines are dashed/dotted.
		*/
		virtual void setLineDashStyle(LineDashStyle p_dashStyle) = 0;
		/*
			Returns how and if lines are dashed/dotted.
		*/
		virtual LineDashStyle getLineDashStyle() = 0;

		/*
			Sets the offset of line dashing/dotting.
		*/
		virtual void setLineDashOffset(float p_dashOffset) = 0;
		/*
			Returns the offset of line dashing/dotting.
		*/
		virtual float getLineDashOffset() = 0;

		/*
			This changes the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual void setLineDashCap(LineCap p_dashCap) = 0;
		/*
			Returns the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual LineCap getLineDashCap() = 0;

		//------------------------------

		/*
			Sets the way line joints are drawn.
		*/
		virtual void setLineJoin(LineJoin p_lineJoin) = 0;
		/*
			Returns the way line joints are drawn.
		*/
		virtual LineJoin getLineJoin() = 0;

		/*
			Sets the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual void setLineJoinMiterLimit(float p_miterLimit) = 0;
		/*
			Returns the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual float getLineJoinMiterLimit() = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the cached geometry will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip geometry.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipGeometry(Geometry const& p_geometry, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipShape(std::vector<Point<float>> const& p_points, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipShape(Point<float> const* p_points, uint32 p_numberOfPoints, float p_opacity = 1.f) = 0;

		/*
			This removes the last added clipping shape.
		*/
		virtual void popClipShape() = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size, float p_opacity = 1.f) = 0;

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rounded rectangle at the origin with the given size and radius will be invisible, on pixel level.
			p_size is the size of the rounded clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(Point<float> const& p_size, float p_radius, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rectangle.

			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRectangleShadowImage(Point<float> const& p_size, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rectangle.

			p_width is the width of the rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRectangleShadowImage(float p_width, float p_height, float p_blur, Color const& p_color) = 0;

		/*
			Generates an image of a shadow that is cast by a rectangle with custom corners.

			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRectangleShadowImage(Point<float> const& p_size, RectangleCorners const& p_corners, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rectangle with custom corners.

			p_width is the width of the rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRectangleShadowImage(float p_width, float p_height, RectangleCorners const& p_corners, float p_blur, Color const& p_color) = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rounded rectangle.

			p_size is the size of the rounded rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
			p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRoundedRectangleShadowImage(Point<float> const& p_size, float p_radius, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rounded rectangle.

			p_width is the width of the rounded rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rounded rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
			p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image createRoundedRectangleShadowImage(float p_width, float p_height, float p_radius, float p_blur, Color const& p_color) = 0;

		//------------------------------

		/*
			Loads an image from pixel data in BGRA format.

			p_pixelData is an array which is 4*width*height bytes in size.
			It contains the color values for every pixel in the image, row-by-row. One byte for every color channel.

			p_width and p_height are in pixels.
		*/
		virtual Image createImage(uint8 const* p_pixelData, uint32 p_width, uint32 p_height) = 0;
		/*
			Loads an image from the data of an image file.
			p_imageData is a memory block which is p_size bytes in size.
		*/
		virtual Image createImage(uint8 const* p_imageData, uint32 p_size) = 0;
		virtual Image createImage(std::vector<uint8> const& p_imageData)
		{
			return createImage(p_imageData.data(), p_imageData.size());
		}
		/*
			Loads an image from a file. Most standard image formats/codecs are supported.
			p_filePath is the path, relative or absolute, to the image file to be loaded.
			If this returns an invalid image, then the file path is probably incorrect.
		*/
		virtual Image createImage(std::string const& p_filePath) = 0;
		/*
			Creates an image from an OS-specific handle.

			On Windows, it is an HICON or HBITMAP.
		*/
		virtual Image createImageFromHandle(void* p_handle) = 0;
		/*
			Draws an image, placed according to the image's bounds and positioning/scaling options.
		*/
		virtual void drawImage(Image const& p_image, float p_multiplicativeOpacity = 1.f) = 0;

		//------------------------------

		/*
			Creates a buffer that contains the file data of an image, encoded in the format p_format.
		*/
		virtual std::vector<uint8> createImageFileData(Image const& p_image, ImageFormat p_format = ImageFormat::Png) = 0;
		/*
			Creates a stream that contains the file data of an image, encoded in the format p_format.
			On Windows, the return pointer type is IStream.
		*/
		virtual void* createImageFileDataNativeStream(Image const& p_image, ImageFormat p_format = ImageFormat::Png) = 0;
		/*
			Saves an image to a file, encoded in the format p_format.
		*/
		virtual void saveImageToFile(Image const& p_image, std::string const& p_filePath, ImageFormat p_format = ImageFormat::Png) = 0;

		//------------------------------

		/*
			Creates an OS API native image from an AvoGUI image.
			On Windows, it returns an HBITMAP.
		*/
		virtual void* createNativeImageFromImage(Image const& p_image) = 0;

		//------------------------------

		/*
			Creates a linear gradient that can be used as a brush when drawing things.
		*/
		virtual LinearGradient createLinearGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX = 0.f, float p_startY = 0.f, float p_endX = 0.f, float p_endY = 0.f) = 0;
		/*
			Creates a linear gradient that can be used as a brush when drawing things.
		*/
		virtual LinearGradient createLinearGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_endPosition) = 0;

		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX = 0.f, float p_startY = 0.f, float p_radiusX = 0.f, float p_radiusY = 0.f) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radiusX, float p_radiusY) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radius) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_radius) = 0;

		/*
			Sets a linear gradient to be used as the brush when drawing things.
		*/
		virtual void setGradient(LinearGradient const& p_gradient) = 0;
		/*
			Sets a radial gradient to be used as the brush when drawing things.
		*/
		virtual void setGradient(RadialGradient const& p_gradient) = 0;
		/*
			Sets a color to be used when drawing things.
		*/
		virtual void setColor(Color const& p_color) = 0;

		/*
			Sets the transparency of all graphics that will be drawn.
		*/
		virtual void setOpacity(float p_opacity) = 0;

		//------------------------------

		/*
			Adds a new font family that can be used by text.
			p_filePath is a path to a font file with a common format.
		*/
		virtual void addFont(std::string const& p_filePath) = 0;

		/*
			Adds a new font to a font family that can be used by text.
			p_data is the data that would be in a font file with a common format.
		*/
		virtual void addFont(std::vector<uint8> const& p_data) = 0;
		/*
			Adds a new font to a font family that can be used by text.
			p_data is the data that would be in a font file with a common format.
		*/
		virtual void addFont(std::vector<uint8>&& p_data) = 0;
		/*
			Adds a new font to a font family that can be used by text.
			p_data is the data that would be in a font file with a common format.
			p_size is the size of the data in bytes.
		*/
		virtual void addFont(uint8 const* p_data, uint32 p_size) = 0;

		//------------------------------

		/*
			Sets the default properties of text created with this drawing context.
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual void setDefaultTextProperties(TextProperties const& p_textProperties) = 0;
		/*
			Returns the default properties of text created with this drawing context.
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual TextProperties getDefaultTextProperties() = 0;

		//------------------------------

		/*
			Creates a new Text object which represents a pre-calculated text layout, using the current text properties.
			p_bounds is the maximum bounds of the text. If it's (0, 0, 0, 0) then the bounds will be calculated to fit the text.
		*/
		virtual Text createText(std::string const& p_string, float p_fontSize, Rectangle<float> p_bounds = Rectangle<float>()) = 0;
		/*
			Draws pre-calculated text created with the createText method.
		*/
		virtual void drawText(Text p_text) = 0;

		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText).
		*/
		virtual void drawText(std::string const& p_string, Rectangle<float> const& p_rectangle) = 0;
		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText()).
		*/
		virtual void drawText(std::string const& p_string, float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText()).
		*/
		virtual void drawText(std::string const& p_string, Point<float> const& p_position, Point<float> const& p_size) = 0;
		/*
			Lays out and draws a string at a position.
			If you're drawing the same text repeatedly, use a Text object (created with createText()).
		*/
		virtual void drawText(std::string const& p_string, float p_x, float p_y) = 0;
		/*
			Lays out and draws a string at a position.
			If you're drawing the same text repeatedly, use a Text object (created with createText()).
		*/
		virtual void drawText(std::string const& p_string, Point<float> const& p_position) = 0;
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
		ResizeNWSE
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
	constexpr bool operator&(ModifierKeyFlags p_left, ModifierKeyFlags p_right)
	{
		return (uint32)p_left & (uint32)p_right;
	}
	constexpr ModifierKeyFlags operator|(ModifierKeyFlags p_left, ModifierKeyFlags p_right)
	{
		return ModifierKeyFlags((uint32)p_left | (uint32)p_right);
	}
	constexpr ModifierKeyFlags& operator|=(ModifierKeyFlags& p_left, ModifierKeyFlags p_right)
	{
		p_left = p_left | p_right;
		return p_left;
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
	class MouseEvent
	{
	public:
		/*
			The view that the mouse interacted with.
		*/
		View* target{ nullptr };

		/*
			X coordinate of the mouse pointer.
		*/
		float x{ 0.f };
		/*
			Y coordinate of the mouse pointer.
		*/
		float y{ 0.f };
		/*
			The movement of the mouse pointer in the x-axis.
			If it is positive it has moved to the right and if it is negative it has moved to the left.
		*/
		float movementX{ 0.f };
		/*
			The movement of the mouse pointer in the y-axis.
			If it is positive it has moved down and if it is negative it has moved up.
		*/
		float movementY{ 0.f };
		/*
			How much the mouse wheel has been moved.
			If it is positive, the wheel has been moved away from the user, if it negative it has moved towards the user.
			It represents the number of ticks the wheel has been moved, but can be a fraction if the mouse has smooth scrolling.
		*/
		float scrollDelta{ 0.f };
		/*
			The mouse button that has been pressed, released or double clicked (depending on the mouse event).
		*/
		MouseButton mouseButton{ MouseButton::None };
		/*
			The modifier keys and mouse buttons that were down when the event occurred.
		*/
		ModifierKeyFlags modifierKeys{ ModifierKeyFlags::None };
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

	class KeyboardEvent
	{
	public:
		/*
			A pointer to the view that the event is directed towards.
		*/
		View* target{ nullptr };
		/*
			The character that was pressed. This is only valid for character press events.
			Since the multibyte UTF-8 encoding is used, this is a string that could be up to 4 8-bit chars.
		*/
		std::string character;
		/*
			The keyboard key that was pressed or released. This is not valid for character press events.
		*/
		KeyboardKey key{ KeyboardKey::None };
		/*
			If this is true, this character/key press event is generated after the initial attack because the key is being held down.
		*/
		bool isRepeated{ false };
	};

	//------------------------------

	enum class DragDropOperation
	{
		Copy,
		Move,
		Link,
		None
	};

	class DragDropFormatData
	{
	public:
		char const* buffer;
		uint32 size;
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
			When data is dragged from an application, many data formats may be given which tell different things about the data or represent it in different ways.
			There may be more than 1 data format with the value formats[p_formatIndex].
		*/
		virtual DragDropFormatData getDataForFormat(uint32 p_formatIndex) const = 0;
		/*
			p_format is one of the values in the "formats" vector.
		*/
		virtual std::string getFormatName(uint32 p_format) const = 0;

		/*
			Returns the text of what is to be dropped, in UTF-8 encoding.
		*/
		virtual std::string getString() const = 0;
		/*
			Returns the text of what is to be dropped, in UTF-16 encoding.
		*/
		virtual std::wstring getUtf16String() const = 0;
		/*
			Returns whether the item to be dropped has any text.
		*/
		virtual bool getHasString() const = 0;

		/*
			Return the names of what is to be dropped, in UTF-8 encoding.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual std::vector<std::string> getItemNames() const = 0;
		/*
			Returns the names of what is to be dropped, in UTF-16 encoding.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual std::vector<std::wstring> getUtf16ItemNames() const = 0;
		/*
			Returns the number of items that have a name.
			Keep in mind that this includes both dragged files and directories.
		*/
		virtual uint32 getNumberOfItemNames() const = 0;

		/*
			Returns the file names of the respective file contents, in UTF-8 encoding.
		*/
		virtual std::vector<std::string> getFileNames() const = 0;
		/*
			Returns the file names of the respective file contents, in UTF-16 encoding.
		*/
		virtual std::vector<std::wstring> getUtf16FileNames() const = 0;
		/*
			Returns the file contents of every file that is being dragged.
		*/
		virtual std::vector<std::string> getFileContents() const = 0;
		/*
			Returns the file contents of an item that is being dragged, by its index.
		*/
		virtual std::string getFileContents(uint32 p_index) const = 0;
		/*
			Returns the number of dragged items that have file contents.
		*/
		virtual uint32 getNumberOfFiles() const = 0;

		/*
			Returns the additional data that has been assigned by an AvoGUI application.
		*/
		virtual uint64 getAdditionalData() const = 0;

		/*
			If an image is being dragged, this creates and returns an Image object representing the image that was dragged.
			If no image is being dragged, it returns 0.
		*/
		virtual Image getImage() const = 0;
	};

	class DragDropEvent
	{
	public:
		/*
			The view that the event is directed towards.
		*/
		View* target;
		/*
			The modifier keys that were pressed when the event fired.
		*/
		ModifierKeyFlags modifierKeys = ModifierKeyFlags::None;

		/*
			The horizontal position of the cursor in DIP view coordinates.
		*/
		float x = 0.f;
		/*
			The vertical position of the cursor in DIP view coordinates.
		*/
		float y = 0.f;
		/*
			The horizontal movement of the cursor in DIP view coordinates.
		*/
		float movementX = 0.f;
		/*
			The vertical movement of the cursor in DIP view coordinates.
		*/
		float movementY = 0.f;

		/*
			Contains the data that is being dragged.
		*/
		std::unique_ptr<ClipboardData> data;
	};

	//------------------------------

	class Window;
	class WindowEvent
	{
	public:
		/*
			The window that has received the event from the OS.
		*/
		Window* window = nullptr;
		/*
			The new width of the window if it has changed size (includes sizeChange/maximize/restore events).
			Expressed in dips.
		*/
		float width = 0.f;
		/*
			The new height of the window if it has changed size (includes sizeChange/maximize/restore events).
			Expressed in dips.
		*/
		float height = 0.f;
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

	constexpr WindowStyleFlags operator&(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32)p_left & (uint32)p_right);
	}
	constexpr WindowStyleFlags operator|(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32)p_left | (uint32)p_right);
	}
	constexpr WindowStyleFlags& operator|=(WindowStyleFlags& p_left, WindowStyleFlags p_right)
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
	class Window : public ReferenceCounted
	{
	public:
		/*
			Creates the window. To close it, use close().

			p_title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.

			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.

			p_positionFactorY is the vertical equivalent to p_positionFactorX.

			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual void create(std::string const& p_title, float p_positionFactorX, float p_positionFactorY, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = nullptr) = 0;
		/*
			Creates the window in the center of the screen. To close it, use close().

			p_title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual void create(std::string const& p_title, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = nullptr) = 0;

	protected:
		bool m_isRunning = false;
		std::mutex m_isRunningMutex;
		std::condition_variable m_isRunningConditionVariable;

		friend class Gui;

		/*
			Makes the window start receiving events.
		*/
		void run()
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
		virtual void close() = 0;
		/*
			Returns whether the OS window has been created and exists.
		*/
		virtual bool getIsOpen() const = 0;

	protected:
		bool m_willClose = false;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns whether the GUI and its window is awaiting being closed by the animation/drawing thread.
		*/
		bool getWillClose()
		{
			return m_willClose;
		}

		//------------------------------

		/*
			Usually used for the parent of a popup window when the popup is closed.
			Makes the window receive mouse and keyboard events again.
		*/
		virtual void enableUserInteraction() = 0;
		/*
			Usually used for the parent of a popup window when the popup is opened.
			Makes the window not receive any mouse and keyboard events, until enableUserInteraction is called.
		*/
		virtual void disableUserInteraction() = 0;
		/*
			Returns whether the window receives mouse and keyboard events.
		*/
		virtual bool getIsUserInteractionEnabled() = 0;

		//------------------------------

		/*
			Sets the text shown in the titlebar.
		*/
		virtual void setTitle(std::string const& p_title) = 0;
		/*
			Returns the text shown in the titlebar.
		*/
		virtual std::string getTitle() const = 0;

		//------------------------------

		/*
			Changes the styles that determine how the window is drawn by the OS.
			These are set when the window is created, and you can change them afterwards here.
		*/
		virtual void setStyles(WindowStyleFlags p_styles) = 0;
		/*
			Returns the current styles that determine how the window is drawn by the OS.
		*/
		virtual WindowStyleFlags getStyles() const = 0;

		//------------------------------

		/*
			Returns the OS-specific window object associated with this window.
		*/
		virtual void* getNativeHandle() const = 0;

		//------------------------------

		/*
			Changes whether the client area of the window fills the whole screen.
		*/
		virtual void setIsFullscreen(bool p_isFullscreen) = 0;
		/*
			Switches between fullscreen and windowed mode.
			If the window is currently windowed, it will become fullscreen, and the other way around.
		*/
		virtual void switchFullscreen() = 0;
		/*
			Returns whether the client area of the window fills the whole screen.
		*/
		virtual bool getIsFullscreen() const = 0;

		//------------------------------

		/*
			Makes the window invisible and disabled.
		*/
		virtual void hide() = 0;
		/*
			Shows the window and enables it.
		*/
		virtual void show() = 0;

		/*
			Makes the size of the window as big as possible so that the border still shows.
		*/
		virtual void maximize() = 0;
		/*
			Hides the window temporarily in a taskbar where it can be restored again.
		*/
		virtual void minimize() = 0;
		/*
			Shows/opens the window as it was before it was minimized.
		*/
		virtual void restore() = 0;

		/*
			Changes the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
			Methods maximize(), minimize() and restore() do the same thing.
		*/
		virtual void setState(WindowState p_state) = 0;
		/*
			Returns the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
		*/
		virtual WindowState getState() const = 0;

		//------------------------------

		/*
			Sets the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		void setPosition(Point<int32> const& p_position)
		{
			setPosition(p_position.x, p_position.y);
		}
		/*
			Sets the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual void setPosition(int32 p_x, int32 p_y) = 0;
		/*
			Returns the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual Point<int32> getPosition() const = 0;
		/*
			Returns the position of the left edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual int32 getPositionX() const = 0;
		/*
			Returns the position of the top edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual int32 getPositionY() const = 0;

		/*
			Sets the size of the client area of the window, in dip units.
		*/
		void setSize(Point<float> const& p_size)
		{
			setSize(p_size.x, p_size.y);
		}
		/*
			Sets the size of the client area of the window, in dip units.
		*/
		virtual void setSize(float p_width, float p_height) = 0;
		/*
			Returns the size of the client area of the window, in dip units.
		*/
		virtual Point<float> getSize() const = 0;
		/*
			Returns the width of the client area of the window, in dip units.
		*/
		virtual float getWidth() const = 0;
		/*
			Returns the height of the client area of the window, in dip units.
		*/
		virtual float getHeight() const = 0;

		/*
			Sets the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		void setMinSize(Point<float> const& p_minSize)
		{
			setMinSize(p_minSize.x, p_minSize.y);
		}
		/*
			Sets the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMinSize(float p_minWidth, float p_minHeight) = 0;
		/*
			Returns the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual Point<float> getMinSize() const = 0;
		/*
			Returns the smallest allowed width for the window when the user is resizing it, in dip units.
		*/
		virtual float getMinWidth() const = 0;
		/*
			Returns the smallest allowed height for the window when the user is resizing it, in dip units.
		*/
		virtual float getMinHeight() const = 0;

		/*
			Sets the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		void setMaxSize(Point<float> const& p_maxSize)
		{
			setMaxSize(p_maxSize.x, p_maxSize.y);
		}
		/*
			Sets the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMaxSize(float p_maxWidth, float p_maxHeight) = 0;
		/*
			Returns the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual Point<float> getMaxSize() const = 0;
		/*
			Returns the biggest allowed width for the window when the user is resizing it, in dip units.
		*/
		virtual float getMaxWidth() const = 0;
		/*
			Returns the biggest allowed height for the window when the user is resizing it, in dip units.
		*/
		virtual float getMaxHeight() const = 0;

		//------------------------------

		/*
			Returns the bounds of the current monitor used by the window, in pixel units.
		*/
		virtual Rectangle<uint32> getMonitorBounds() const = 0;
		/*
			Returns the virtual position of the current monitor used by the window relative to other monitors, in pixel units.
		*/
		virtual Point<uint32> getMonitorPosition() const = 0;
		/*
			Returns the size of the current monitor used by the window, in pixel units.
		*/
		virtual Point<uint32> getMonitorSize() const = 0;
		/*
			Returns the width of the current monitor used by the window, in pixel units.
		*/
		virtual uint32 getMonitorWidth() const = 0;
		/*
			Returns the height of the current monitor used by the window, in pixel units.
		*/
		virtual uint32 getMonitorHeight() const = 0;

		//------------------------------

		/*
			Returns the bounds of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Rectangle<uint32> getWorkAreaBounds() const = 0;
		/*
			Returns the virtual position of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Point<uint32> getWorkAreaPosition() const = 0;
		/*
			Returns the size of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Point<uint32> getWorkAreaSize() const = 0;
		/*
			Returns the width of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual uint32 getWorkAreaWidth() const = 0;
		/*
			Returns the height of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual uint32 getWorkAreaHeight() const = 0;

		//------------------------------

		/*
			Returns whether a key is currently pressed down.
		*/
		virtual bool getIsKeyDown(KeyboardKey p_key) const = 0;
		/*
			Returns whether a mouse button is currently pressed down.
		*/
		virtual bool getIsMouseButtonDown(MouseButton p_button) const = 0;
		/*
			Returns the position of the mouse cursor, relative to the top-left corner of the window.
		*/
		virtual Point<float> getMousePosition() const = 0;

		//------------------------------

		/*
			Changes what the mouse cursor looks like.
		*/
		virtual void setCursor(Cursor p_cursor) = 0;
		/*
			Returns the current mouse cursor.
		*/
		virtual Cursor getCursor() const = 0;

		//------------------------------

		/*
			Returns the factor that is used to convert DIP units to pixel units in the window.
			This can change during the lifetime of the window, if the user drags it to a monitor with a different DPI for example.
		*/
		virtual float getDipToPixelFactor() const = 0;

		//------------------------------

		/*
			Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropString(std::string const& p_string, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;

		/*
			Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropString(std::wstring const& p_string, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;


		/*
			Runs a blocking loop that allows the user to drag image data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropImage(Image const& p_image, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(uint8 const* p_data, uint32 p_dataSize, std::string const& p_name, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(uint8 const* p_data, uint32 p_dataSize, std::wstring const& p_name, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(std::vector<uint8> const& p_data, std::string const& p_name, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(std::vector<uint8> const& p_data, std::wstring const& p_name, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(std::string const& p_path, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFile(std::wstring const& p_path, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;

		/*
			Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFiles(std::vector<std::string> const& p_paths, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;
		/*
			Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
			This method sends events to the drop target(s).
			The return value indicates what operation was made after the drop.
		*/
		virtual DragDropOperation dragAndDropFiles(std::vector<std::wstring> const& p_paths, Image const& p_dragImage = Image(), Point<float> const& p_dragImageCursorPosition = Point<float>(), uint64 p_additionalData = 0u) = 0;

		//------------------------------

		/*
			Gives a UTF-16 encoded string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual void setClipboardString(std::wstring const& p_string, uint64 p_additionalData = 0u) const = 0;

		/*
			Gives a UTF-8 encoded string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual void setClipboardString(std::string const& p_string, uint64 p_additionalData = 0u) const = 0;

		/*
			Gives an image for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardImage(Image const& p_image, uint64 p_additionalData = 0u) const = 0;

		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(uint8 const* p_data, uint32 p_dataSize, std::string const& p_name, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(uint8 const* p_data, uint32 p_dataSize, std::wstring const& p_name, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(std::vector<uint8> const& p_data, std::string const& p_name, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives file data for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(std::vector<uint8> const& p_data, std::wstring const& p_name, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives a UTF-8 file path for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(std::string const& p_path, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives a UTF-16 file path for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFile(std::wstring const& p_path, uint64 p_additionalData = 0u) const = 0;

		/*
			Gives UTF-8 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFiles(std::vector<std::string> const& p_paths, uint64 p_additionalData = 0u) const = 0;
		/*
			Gives UTF-16 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this data.
		*/
		virtual void setClipboardFiles(std::vector<std::wstring> const& p_paths, uint64 p_additionalData = 0u) const = 0;

		/*
			Returns the data that is currently stored on the clipboard.
		*/
		[[nodiscard]] virtual std::unique_ptr<ClipboardData> getClipboardData() const = 0;

		//------------------------------
		// Window events

	protected:
		bool sendWindowCloseEvents(WindowEvent const& p_event)
		{
			bool willClose = true;
			for (auto& listener : windowCloseListeners.listeners)
			{
				if (!listener(p_event))
				{
					willClose = false;
				}
			}
			return willClose;
		}

	public:
		EventListeners<bool(WindowEvent const&)> windowCloseListeners;
		EventListeners<void(WindowEvent const&)> windowCreateListeners;
		EventListeners<void(WindowEvent const&)> windowDestroyListeners;
		EventListeners<void(WindowEvent const&)> windowMinimizeListeners;
		EventListeners<void(WindowEvent const&)> windowMaximizeListeners;
		EventListeners<void(WindowEvent const&)> windowRestoreListeners;
		EventListeners<void(WindowEvent const&)> windowSizeChangeListeners;
		EventListeners<void(WindowEvent const&)> windowFocusGainListeners;
		EventListeners<void(WindowEvent const&)> windowFocusLoseListeners;
	};

	/*
		A rectangle that can draw itself and receive events. Used for GUI components and stuff.
	*/
	class View : public Component, public ProtectedRectangle
	{
	private:
		friend class Gui;

	public:
		View(View* p_parent, Rectangle<float> const& p_bounds = Rectangle<float>(0.f, 0.f, 0.f, 0.f));
		template<typename T>
		View(View* p_parent, T p_id, Rectangle<float> const& p_bounds = Rectangle<float>(0.f, 0.f, 0.f, 0.f)) :
			View(p_parent, p_bounds)
		{
			setId(p_id, getGui());
		}
		~View() override;

	protected:
		Geometry m_clipGeometry;
		/*
			LIBRARY IMPLEMENTED
			This is called whenever the clipping geometry of the view needs to be updated.
			You can override this if you want a custom clipping geometry, just replace m_clipGeometry.
		*/
		virtual void updateClipGeometry();

	public:
		/*
			Sets the geometry being used to clip the view's contents.
			The clip geometry of the view is by default updated automatically in the updateGeometry method when the size has changed.
			Note that hit testing is not by default affected by this, override getIsContaining(float p_x, float p_y) if you want custom hit testing.
		*/
		void setClipGeometry(Geometry const& p_geometry)
		{
			m_clipGeometry = p_geometry;
		}
		/*
			Returns the geometry being used to clip the view's contents.
		*/
		Geometry const& getClipGeometry()
		{
			return m_clipGeometry;
		}

		//------------------------------

	private:
		bool m_isOverlay{ false };
	public:
		/*
			LIBRARY IMPLEMENTED
			If you set this to true, this view will not block any mouse events from reaching views below this one.
			Whether this view is a mouse listener has no effect on this.
		*/
		void setIsOverlay(bool p_isOverlay)
		{
			m_isOverlay = p_isOverlay;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view blocks mouse events from reaching views below this one.
			False means it blocks, true means it does not.
		*/
		bool getIsOverlay()
		{
			return m_isOverlay;
		}

		//------------------------------

	private:
		bool m_isVisible{ true };
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets whether the view is visible and can receive events.
		*/
		void setIsVisible(bool p_isVisible)
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
		bool getIsVisible() const
		{
			return m_isVisible;
		}

		//------------------------------

	private:
		float m_opacity{ 1.f };
	public:
		/*
			Sets how opaque the view and its children are (multiplied with parent opacity).
		*/
		void setOpacity(float p_opacity)
		{
			m_opacity = p_opacity;
		}
		/*
			Returns how opaque the view and its children are (multiplied with parent opacity).
		*/
		float getOpacity()
		{
			return m_opacity;
		}

		//------------------------------

		// TODO: Add view transforms

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the cursor that will by default be shown when the mouse enters the view.
			The default implementation of handleMouseBackgroundEnter sets the cursor to this one, and you can override this behaviour.
			This method also calls enableMouseEvents().
		*/
		void setCursor(Cursor p_cursor)
		{
			m_cursor = p_cursor;
			enableMouseEvents();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the cursor that will by default be shown when the mouse enters the view.
		*/
		Cursor getCursor()
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
		Gui* getGui() const
		{
			return m_gui;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the highest view in the hierarchy, the GUI.
			It is cast to another pointer type.
		*/
		template<typename T>
		T* getGui() const
		{
			return (T*)m_gui;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the object used for drawing.
			The same as calling getGui()->getDrawingContext(), but more convenient.
		*/
		DrawingContext* getDrawingContext();
		/*
			LIBRARY IMPLEMENTED
			Returns the window that is attached to the GUI.
		*/
		Window* getWindow();

	private:
		std::vector<Animation*> m_animations;

	public:
		/*
			Creates an animation that is released by this view when it is destroyed.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
		*/
		[[nodiscard]] Animation* createAnimation(Easing const& p_easing, float p_milliseconds)
		{
			Animation* animation = new Animation(this, p_easing, p_milliseconds);
			m_animations.push_back(animation);
			return animation;
		}
		/*
			Creates an animation that is released by the view when it is destroyed.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
		*/
		[[nodiscard]] Animation* createAnimation(Id const& p_easingId, float p_milliseconds)
		{
			return createAnimation(getThemeEasing(p_easingId), p_milliseconds);
		}
		/*
			Creates an animation that is released by this view when it is destroyed.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes the current animation value as a parameter.
		*/
		[[nodiscard]] Animation* createAnimation(Easing const& p_easing, float p_milliseconds, std::function<void(float)> p_callback)
		{
			Animation* animation = new Animation(this, p_easing, p_milliseconds);
			animation->updateListeners += p_callback;
			m_animations.push_back(animation);
			return animation;
		}
		/*
			Creates an animation that is released by the view when it is destroyed.
			p_easingId is the theme easing ID of the animation easing to be used.
			p_milliseconds is the duration of the animation, can be changed later on the returned object.
			p_callback is a function that will be called every time the animation has been updated, it takes the current animation value as a parameter.
		*/
		[[nodiscard]] Animation* createAnimation(Id const& p_easingId, float p_milliseconds, std::function<void(float)> p_callback)
		{
			return createAnimation(getThemeEasing(p_easingId), p_milliseconds, p_callback);
		}

	private:
		/*
			LIBRARY IMPLEMENTED
			Makes sure the view is drawn at the correct time, according to elevation.
		*/
		void updateViewDrawingIndex(View* p_view);

		uint32 m_index{ 0 };
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the index of this view relative to its siblings.
		*/
		uint32 getIndex() const
		{
			return m_index;
		}

	private:
		uint32 m_layerIndex{ 0 };
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the layer index of the view, how deep down the view hierarchy it is.
			The GUI view has a layer index of 0.
		*/
		uint32 getLayerIndex() const
		{
			return m_layerIndex;
		}

	private:
		View* m_parent{ nullptr };
	public:
		/*
			LIBRARY IMPLEMENTED
			Attaches this view to a new parent, which will manage the lifetime of the view unless you've called remember() on it.
			If the parameter is 0, the view is only detached from its old parent, and is left alone with no parents :^(.
		*/
		void setParent(View* p_container)
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
				if ((View*)m_gui == this)
				{
					m_layerIndex = 0;
				}
				else
				{
					m_layerIndex = m_parent->m_layerIndex + 1U;
				}
				m_absolutePosition.x = m_parent->getAbsoluteLeft() + m_bounds.left;
				m_absolutePosition.y = m_parent->getAbsoluteTop() + m_bounds.top;

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
	public:
		/*
			LIBRARY IMPLEMENTED
			Removes a child view from this view. This forgets the view being removed.
			If you haven't remembered it yourself, it will get deleted.
		*/
		void removeChildView(View* p_view)
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
		void removeChildView(uint32 p_viewIndex)
		{
			auto childToRemove = m_childViews[p_viewIndex];
			childViewDetachmentListeners(childToRemove);

			childToRemove->m_parent = nullptr;

			for (uint32 a = p_viewIndex; a < m_childViews.size() - 1; a++)
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
		void removeAllChildViews()
		{
			while (!m_childViews.empty()) // That function naming, ew... Why didn't they call it getIsEmpty? empty() should be emptying something >:^(
			{
				Avo::View* child = m_childViews.back();
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
		View* getChildView(uint32 p_viewIndex) const
		{
			return m_childViews[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the child view at an index, casted to a pointer of another type.
		*/
		template<typename T>
		T* getChildView(uint32 p_viewIndex) const
		{
			return (T*)m_childViews[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the number of child views that are attached to this view.
		*/
		uint32 getNumberOfChildViews() const
		{
			return m_childViews.size();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a vector containing the child views that are attached to this view.
		*/
		auto const& getChildViews() const
		{
			return m_childViews;
		}

		using iterator = std::vector<View*>::iterator;
		/*
			LIBRARY IMPLEMENTED
			Returns the begin iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		iterator begin() noexcept
		{
			return m_childViews.begin();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the end iterator for the view's child views.
			This allows writing range-based for loops over a view's children with simpler syntax.
		*/
		iterator end() noexcept
		{
			return m_childViews.end();
		}

		//------------------------------

	private:
		Image m_shadowImage;
		float m_elevation{ 0.f };

		/*
			Updates the shadow bounds and the shadow image.
		*/
		void updateShadow();

	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the elevation of the view. This both changes its shadow (if the view has shadow) and drawing order.
			The higher the elevation is, the later it will get drawn.
			If p_elevation is negative, it is set from the top of the elevation space.
		*/
		void setElevation(float p_elevation)
		{
			p_elevation = float(p_elevation < 0.f) * FLT_MAX + p_elevation;

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
		float getElevation() const
		{
			return m_elevation;
		}

	private:
		bool m_hasShadow{ true };
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets whether the elevation is shown with a shadow.
		*/
		void setHasShadow(bool p_hasShadow);
		/*
			LIBRARY IMPLEMENTED
			Returns whether the elevation is shown with a shadow.
		*/
		bool getHasShadow() const
		{
			return m_hasShadow;
		}

	private:
		Rectangle<float> m_shadowBounds;
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns the rectangle that represents the area where the shadow is drawn, relative to the view position.
			The view is always contained within the shadow bounds.
		*/
		Rectangle<float> getShadowBounds() const
		{
			return m_shadowBounds;
		}

		//------------------------------

	private:
		bool m_isInAnimationUpdateQueue{ false };
	public:
		/*
			LIBRARY IMPLEMENTED
			Queues an animation update for the next frame.
		*/
		void queueAnimationUpdate();
		/*
			USER IMPLEMENTED
			Updates things like animations and does anything that you never want to happen more than once every frame.
			Call queueAnimationUpdate() when you want this method to be called in the next interval.
			This system allows for animations to only get updated when they have to.
		*/
		virtual void updateAnimations() { }

		//------------------------------

	private:
		Rectangle<float> m_lastInvalidatedShadowBounds;
		/*
			Draws the shadow of the view.
		*/
		void drawShadow(DrawingContext* p_drawingContext);

	public:
		/*
			LIBRARY IMPLEMENTED
			Call this if you want the view to get redrawn. Adds an invalid rectangle to the window or two if the view has been moved.
		*/
		void invalidate();

		/*
			USER IMPLEMENTED
			Draws the contents of the view.
			This method is called by default from the other draw method that also takes the target rectangle as input.
			You often don't need to use that parameter.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
		*/
		virtual void draw(DrawingContext* p_drawingContext) { }
		/*
			USER IMPLEMENTED
			Draws the content of the view. Override this method if you want the target rectangle, override the overloaded
			method that only takes the drawing context otherwise.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI.
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle)
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
		virtual void drawOverlay(DrawingContext* p_drawingContext) { }

		/*
			USER IMPLEMENTED
			Draws on top of child views. Override this method if you want the target rectangle, override the overloaded
			method that only takes the drawing context otherwise.

			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI.
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual void drawOverlay(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle)
		{
			drawOverlay(p_drawingContext);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the smallest possible rectangle that contains all child views belonging to this View.
			The rectangle is relative to the position of this view.
		*/
		Rectangle<float> calculateContentBounds() const
		{
			if (m_childViews.empty())
			{
				return Rectangle<float>();
			}

			float left = m_childViews[0]->getLeft();
			float right = m_childViews[0]->getRight();
			float top = m_childViews[0]->getTop();
			float bottom = m_childViews[0]->getBottom();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getLeft() < left)
				{
					left = m_childViews[a]->getLeft();
				}
				if (m_childViews[a]->getTop() < top)
				{
					top = m_childViews[a]->getTop();
				}
				if (m_childViews[a]->getRight() > right)
				{
					right = m_childViews[a]->getRight();
				}
				if (m_childViews[a]->getBottom() > bottom)
				{
					bottom = m_childViews[a]->getBottom();
				}
			}

			return Rectangle<float>(left, top, right, bottom);
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the width of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		float calculateContentWidth() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float left = m_childViews[0]->getLeft();
			float right = m_childViews[0]->getRight();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getLeft() < left)
				{
					left = m_childViews[a]->getLeft();
				}
				if (m_childViews[a]->getRight() > right)
				{
					right = m_childViews[a]->getRight();
				}
			}
			return right - left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the height of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		float calculateContentHeight() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float top = m_childViews[0]->getTop();
			float bottom = m_childViews[0]->getBottom();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getTop() < top)
				{
					top = m_childViews[a]->getTop();
				}
				if (m_childViews[a]->getBottom() > bottom)
				{
					bottom = m_childViews[a]->getBottom();
				}
			}
			return bottom - top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the size of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		Point<float> calculateContentSize() const
		{
			return calculateContentBounds().getSize();
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the leftmost edge of all child views belonging to this View.
			The returned offset is relative to the left edge of this view.
		*/
		float calculateContentLeft() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float left = m_childViews[0]->getLeft();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getLeft() < left)
				{
					left = m_childViews[a]->getLeft();
				}
			}
			return left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the rightmost edge of all child views belonging to this View.
			The returned offset is relative to the left edge of this view.
		*/
		float calculateContentRight() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float right = m_childViews[0]->getRight();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getRight() > right)
				{
					right = m_childViews[a]->getRight();
				}
			}
			return right;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost edge of all child views belonging to this View.
			The returned offset is relative to the top edge of this view.
		*/
		float calculateContentTop() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float top = m_childViews[0]->getTop();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getTop() < top)
				{
					top = m_childViews[a]->getTop();
				}
			}
			return top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bottommost edge of all child views belonging to this View.
			The returned offset is relative to the top edge of this view.
		*/
		float calculateContentBottom() const
		{
			if (m_childViews.empty())
			{
				return 0.f;
			}

			float bottom = m_childViews[0]->getBottom();
			for (uint32 a = 1; a < m_childViews.size(); a++)
			{
				if (m_childViews[a]->getBottom() > bottom)
				{
					bottom = m_childViews[a]->getBottom();
				}
			}
			return bottom;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and/or change the size of this view.
		*/
		void setPadding(float p_padding)
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
		void setPadding(float p_horizontalPadding, float p_verticalPadding)
		{
			setPadding(p_horizontalPadding, p_horizontalPadding, p_verticalPadding, p_verticalPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and change the size of this view.
		*/
		void setPadding(float p_leftPadding, float p_rightPadding, float p_topPadding, float p_bottomPadding)
		{
			Rectangle<float> contentBounds(calculateContentBounds());
			float offsetX = p_leftPadding - contentBounds.left;
			float offsetY = p_topPadding - contentBounds.top;
			for (auto& child : m_childViews)
			{
				child->move(offsetX, offsetY);
			}
			setSize(contentBounds.getWidth() + p_leftPadding + p_rightPadding, contentBounds.getHeight() + p_topPadding + p_bottomPadding);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the leftmost edge of the contents and the left edge of this View.
			This may move the child views with a uniform offset and change the width of this view.
		*/
		void setLeftPadding(float p_leftPadding)
		{
			float left = calculateContentLeft();
			float offset = p_leftPadding - left;
			for (auto& child : m_childViews)
			{
				child->move(offset, 0.f);
			}
			setWidth(getWidth() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the rightmost edge of the contents and the right edge of this View.
			This changes the width of this view.
		*/
		void setRightPadding(float p_rightPadding)
		{
			setWidth(calculateContentRight() + p_rightPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the topmost edge of the contents and the top edge of this View.
			This may move the child views with a uniform offset and change the height of this view.
		*/
		void setTopPadding(float p_topPadding)
		{
			float top = calculateContentTop();
			float offset = p_topPadding - top;
			for (auto& child : m_childViews)
			{
				child->move(offset, 0.f);
			}
			setHeight(getHeight() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the bottommost edge of the contents and the bottom edge of this View.
			This changes the height of this view.
		*/
		void setBottomPadding(float p_bottomPadding)
		{
			setHeight(calculateContentBottom() + p_bottomPadding);
		}

		//------------------------------

		/*
			Listener signature:
				void (Id const& id, Color const& color)
			See View::handleThemeColorChange for more information.
		*/
		EventListeners<void(Id const&, Color const&)> themeColorChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme color has changed, not including initialization.
		*/
		virtual void handleThemeColorChange(Id const& p_id, Color const& p_newColor) { }

		/*
			Listener signature:
				void (Id const& id, Easing const& easing)
			See View::handleThemeEasingChange for more information.
		*/
		EventListeners<void(Id const&, Easing const&)> themeEasingChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme easing has changed, not including initialization.
		*/
		virtual void handleThemeEasingChange(Id const& p_id, Easing const& p_newEasing) { };

		/*
			Listener signature:
				void (Id const& id, float value)
			See View::handleThemeValueChange for more information.
		*/
		EventListeners<void(Id const&, float)> themeValueChangeListeners;
		/*
			USER IMPLEMENTED
			This gets called whenever a theme value has changed, not including initialization.
		*/
		virtual void handleThemeValueChange(Id const& p_id, float p_newValue) { };

		//------------------------------

	private:
		Theme* m_theme{ nullptr };
	public:
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the theme that is used by this view.
		*/
		Theme const* getTheme() const
		{
			return m_theme;
		}

	private:
		template<typename T, typename U>
		void propagateThemePropertyChange(void(View::* p_function)(Id const&, T, bool), Id const& p_id, U&& p_property, bool p_willAffectChildren)
		{
			if (p_willAffectChildren)
			{
				View* view = this;
				uint32 startIndex = 0;
				while (true)
				{
				loopStart:
					for (uint32 a = startIndex; a < view->m_childViews.size(); a++)
					{
						(view->m_childViews[a]->*p_function)(p_id, std::forward<U>(p_property), false);
						if (view->m_childViews[a]->m_childViews.size())
						{
							view = view->m_childViews[a];
							startIndex = 0;
							goto loopStart;
						}
					}
					if (view == this)
					{
						break;
					}
					startIndex = view->m_index + 1;
					view = view->m_parent;
				}
			}

			// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
			// It's not the other way around because the parent lays out the children and the size of the children may changed in the handler.
			if (!m_theme)
			{
				m_theme = new Theme();
			}
			else if (m_theme->getReferenceCount() > 1)
			{
				m_theme->forget();
				m_theme = new Theme(*m_theme);
			}
		}

	public:
		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default color that can be changed.
			These colors may be used by views that come with the library, but you can use them yourself too.
			The default color IDs are in the Avo::ThemeColors namespace.
			If p_id is anything else, the color is kept in the theme and you can use it yourself.

			If p_willAffectChildren is true, all children and views below those too will change this color in their themes.
		*/
		void setThemeColor(Id const& p_id, Color const& p_color, bool p_willAffectChildren = true)
		{
			propagateThemePropertyChange(&View::setThemeColor, p_id, p_color, p_willAffectChildren);

			if (m_theme->colors[p_id] != p_color)
			{
				m_theme->colors[p_id] = p_color;
				themeColorChangeListeners(p_id, p_color);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme colors.
			Example usage:
				using namespace Avo::ThemeColors;
				setThemeColors({
					{ background, 0.f },
					{ onBackground, {1.f, 0.f, 0.2f} },
				});

			See setThemeColor for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, Color>>>
		void setThemeColors(Pairs const& p_pairs, bool p_willAffectChildren = true)
		{
			for (auto pair : p_pairs)
			{
				setThemeColor(pair.first, pair.second, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		Color getThemeColor(Id const& p_id) const
		{
			return m_theme->colors[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_color is inserted into the theme with the id p_id if it doesn't already have a value.
		*/
		void initializeThemeColor(Id const& p_id, Color const& p_color)
		{
			m_theme->colors.insert({ p_id, p_color });
		}

		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default easing that can be changed.
			These easings may be used by views that come with the library, but you can use them yourself too.
			The default easing IDs are in the Avo::ThemeEasings namespace.
			If p_id is anything else, the easing is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this easing in their themes.
		*/
		void setThemeEasing(Id const& p_id, Easing const& p_easing, bool p_willAffectChildren = true)
		{
			propagateThemePropertyChange(&View::setThemeEasing, p_id, p_easing, p_willAffectChildren);

			if (m_theme->easings[p_id] != p_easing)
			{
				m_theme->easings[p_id] = p_easing;
				themeEasingChangeListeners(p_id, p_easing);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme easings.
			Example usage:
				using namespace Avo::ThemeEasings;
				setThemeEasings({
					{ in, {1.f, 0.f, 1.f, 1.f} },
					{ inOut, {1.f, 0.f, 0.f, 1.f} },
				});

			See setThemeEasing for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, Easing>>>
		void setThemeEasings(Pairs const& p_pairs, bool p_willAffectChildren = true)
		{
			for (auto pair : p_pairs)
			{
				setThemeEasing(pair.first, pair.second, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		Easing const& getThemeEasing(Id const& p_id) const
		{
			return m_theme->easings[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_easing is inserted into the theme with the ID p_id if it doesn't already have a value.
		*/
		void initializeThemeEasing(Id const& p_id, Easing const& p_easing)
		{
			m_theme->easings.insert({ p_id, p_easing });
		}

		/*
			LIBRARY IMPLEMENTED

			Some IDs have a default value that can be changed.
			These values may be used by views that come with the library, but you can use them yourself too.
			The default value IDs are in the Avo::ThemeValues namespace.
			If p_id is anything else, the value is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this value in their themes.
		*/
		void setThemeValue(Id const& p_id, float p_value, bool p_willAffectChildren = true)
		{
			propagateThemePropertyChange(&View::setThemeValue, p_id, p_value, p_willAffectChildren);

			if (m_theme->values[p_id] != p_value)
			{
				m_theme->values[p_id] = p_value;
				themeValueChangeListeners(p_id, p_value);
			}
		}
		/*
			LIBRARY IMPLEMENTED

			Sets multiple theme values.
			Example usage:
				using namespace Avo::ThemeValues;
				setThemeValues({
					{ hoverAnimationDuration, 100 },
					{ tooltipFontSize, 13.f },
				});

			See setThemeValue for more details.
		*/
		template<typename Pairs = std::initializer_list<std::pair<Id, float>>>
		void setThemeValues(Pairs const& p_pairs, bool p_willAffectChildren = true)
		{
			for (auto pair : p_pairs)
			{
				setThemeValue(pair.first, pair.second, p_willAffectChildren);
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		float getThemeValue(Id const& p_id) const
		{
			return m_theme->values[p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			p_value is inserted into the theme with the ID p_id if it doesn't already have a value.
		*/
		void initializeThemeValue(Id const& p_id, float p_value)
		{
			m_theme->values.insert({ p_id, p_value });
		}

		//------------------------------

	private:
		Point<float> m_absolutePosition;

		/*
			Moves the point(s) representing the absolute position(s) of this view and/or all children of this view (recursively).
			The absolute positions of views are used often for mouse event targeting, among other things.
			Because of this, it is pre-calculated in this way only when this view or a parent view has moved.
		*/
		void moveAbsolutePositions(float p_offsetX, float p_offsetY, bool p_willUpdateChildren = true)
		{
			m_absolutePosition.move(p_offsetX, p_offsetY);

			if (p_willUpdateChildren && !m_childViews.empty())
			{
				View* currentContainer = this;
				View* child = nullptr;
				uint32 startIndex = 0;
				while (true)
				{
				loopBody:
					for (uint32 a = startIndex; a < currentContainer->getNumberOfChildViews(); a++)
					{
						auto child = currentContainer->getChildView(a);
						child->moveAbsolutePositions(p_offsetX, p_offsetY, false);
						if (child->getNumberOfChildViews())
						{
							currentContainer = child;
							startIndex = 0;
							goto loopBody; // THIS IS GOOD USE OF GOTO OK
						}
					}
					if (currentContainer == this)
					{
						return;
					}
					startIndex = currentContainer->getIndex() + 1;
					currentContainer = currentContainer->getParent<View>();
				}
			}
		}
		Point<float> calculateAbsolutePositionRelativeTo(Point<float> p_position) const;
	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(float p_left, float p_top, float p_right, float p_bottom) override
		{
			Rectangle<float> boundsBefore = m_bounds;

			float offsetX = p_left - m_bounds.left;
			float offsetY = p_top - m_bounds.top;
			if (offsetX || offsetY)
			{
				moveAbsolutePositions(offsetX, offsetY);
			}

			m_bounds.left = p_left;
			m_bounds.top = p_top;
			m_bounds.right = p_right;
			m_bounds.bottom = p_bottom;

			sendBoundsChangeEvents(boundsBefore);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(float p_left, float p_top, float p_right, float p_bottom)
		{
			Rectangle<float> boundsBefore = m_bounds;

			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.left += offsetX;
				m_bounds.top += offsetY;
			}

			m_bounds.right = m_bounds.left + p_right - p_left;
			m_bounds.bottom = m_bounds.top + p_bottom - p_top;

			sendBoundsChangeEvents(boundsBefore);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(Point<float> const& p_position, Point<float> const& p_size) override
		{
			setBounds(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(Point<float> const& p_position, Point<float> const& p_size)
		{
			setAbsoluteBounds(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(Rectangle<float> const& p_rectangle) override
		{
			setBounds(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(Rectangle<float> const& p_rectangle)
		{
			setAbsoluteBounds(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		Rectangle<float> const& getBounds() const override
		{
			return m_bounds;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view relative to the top left corner of the GUI.
		*/
		Rectangle<float> getAbsoluteBounds() const
		{
			return Rectangle<float>(m_absolutePosition, m_bounds.getSize());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view shadow relative to the top left corner of the GUI.
		*/
		Rectangle<float> getAbsoluteShadowBounds() const
		{
			Rectangle<float> bounds;
			bounds.left = m_absolutePosition.x + (m_bounds.getWidth() - m_shadowBounds.getWidth()) * 0.5f;
			bounds.top = m_absolutePosition.y + (m_bounds.getHeight() - m_shadowBounds.getHeight()) * 0.5f;
			bounds.right = bounds.left + m_shadowBounds.getWidth();
			bounds.bottom = bounds.top + m_shadowBounds.getHeight();
			return bounds;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Moves the whole view.
		*/
		void move(Point<float> const& p_offset) override
		{
			move(p_offset.x, p_offset.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Moves the whole view.
		*/
		void move(float p_offsetX, float p_offsetY) override
		{
			if (p_offsetX || p_offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_offsetX, p_offsetY);
				m_bounds.move(p_offsetX, p_offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_top != m_bounds.top)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, p_top - m_bounds.top);
				m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopLeft(float p_left, float p_top, bool p_willKeepSize = true)
		{
			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.setTopLeft(m_bounds.left + offsetX, m_bounds.top + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top left corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getTopLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.top);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top left corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> const& getAbsoluteTopLeft() const
		{
			return m_absolutePosition;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopRight(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopRight(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_top != m_bounds.top)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_willKeepSize ? p_right - m_bounds.right : 0, p_top - m_bounds.top);
				m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopRight(float p_right, float p_top, bool p_willKeepSize = true)
		{
			float offsetX = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_willKeepSize ? offsetX : 0, offsetY);
				m_bounds.setTopRight(m_bounds.right + offsetX, m_bounds.top + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top right corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getTopRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.top);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top right corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteTopRight() const
		{
			return Point<float>(m_absolutePosition.x + m_bounds.right - m_bounds.left, m_absolutePosition.y);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_bottom != m_bounds.bottom)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, p_willKeepSize ? p_bottom - m_bounds.bottom : 0);
				m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true)
		{
			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offsetX || offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, p_willKeepSize ? offsetY : 0.f);
				m_bounds.setBottomLeft(m_bounds.left + offsetX, m_bounds.bottom + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom left corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getBottomLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom left corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteBottomLeft() const
		{
			return Point<float>(m_absolutePosition.x, m_absolutePosition.y + m_bounds.bottom - m_bounds.top);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomRight(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomRight(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the parent.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_bottom != m_bounds.bottom)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepSize)
				{
					moveAbsolutePositions(p_right - m_bounds.right, p_bottom - m_bounds.bottom);
				}
				m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the GUI.
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true)
		{
			float offsetX = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			float offsetY = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offsetX || offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepSize)
				{
					moveAbsolutePositions(offsetX, offsetY);
					m_bounds.move(offsetX, offsetY);
				}
				else
				{
					m_bounds.moveBottomRight(offsetX, offsetY);
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom right corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getBottomRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom right corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteBottomRight() const
		{
			return Point<float>(m_absolutePosition.x + m_bounds.right - m_bounds.left, m_absolutePosition.y + m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the parent.
		*/
		void setCenter(Point<float> const& p_position) override
		{
			setCenter(p_position.x, p_position.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the GUI.
		*/
		void setAbsoluteCenter(Point<float> const& p_position)
		{
			setAbsoluteCenter(p_position.x, p_position.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the parent.
		*/
		void setCenter(float p_x, float p_y) override
		{
			if (p_x != m_bounds.getCenterX() || p_y != m_bounds.getCenterY())
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_x - m_bounds.getCenterX(), p_y - m_bounds.getCenterY());
				m_bounds.setCenter(p_x, p_y);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the GUI.
		*/
		void setAbsoluteCenter(float p_x, float p_y)
		{
			float offsetX = p_x - m_absolutePosition.x - getWidth() * 0.5f;
			float offsetY = p_y - m_absolutePosition.y - getHeight() * 0.5f;
			if (offsetX || offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.move(offsetX, offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the horizontal center coordinate of the view relative to the left edge of the parent.
		*/
		void setCenterX(float p_x) override
		{
			if (p_x != m_bounds.getCenterX())
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_x - m_bounds.getCenterX(), 0);
				m_bounds.setCenterX(p_x);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the horizontal center coordinate of the view relative to the left edge of the GUI.
		*/
		void setAbsoluteCenterX(float p_x)
		{
			float offsetX = p_x - m_absolutePosition.x - getWidth() * 0.5f;
			if (offsetX)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, 0);
				m_bounds.moveX(offsetX);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the vertical center coordinate of the view relative to the top edge of the parent.
		*/
		void setCenterY(float p_y) override
		{
			if (p_y != m_bounds.getCenterY())
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_y - m_bounds.getCenterY());
				m_bounds.setCenterY(p_y);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the vertical center coordinate of the view relative to the top edge of the GUI.
		*/
		void setAbsoluteCenterY(float p_y)
		{
			float offsetY = p_y - m_absolutePosition.y - getHeight() * 0.5f;
			if (offsetY)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0.f, offsetY);
				m_bounds.moveX(offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the center coordinates of the view relative to the top left corner of the parent.
		*/
		Point<float> getCenter() const override
		{
			return m_bounds.getCenter();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the center coordinates of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteCenter() const
		{
			return Point<float>(m_absolutePosition.x + getWidth() * 0.5f, m_absolutePosition.y + getHeight() * 0.5f);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the x-axis center coordinate of the view relative to the left edge of the parent.
		*/
		float getCenterX() const override
		{
			return m_bounds.getCenterX();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the x-axis center coordinate of the view relative to the left edge of the GUI.
		*/
		float getAbsoluteCenterX() const
		{
			return m_absolutePosition.x + getWidth() * 0.5f;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the y-axis center coordinate of the view relative to the top edge of the parent.
		*/
		float getCenterY() const override
		{
			return m_bounds.getCenterY();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the y-axis center coordinate of the view relative to the top edge of the GUI.
		*/
		float getAbsoluteCenterY() const
		{
			return m_absolutePosition.y + getHeight() * 0.5f;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the left coordinate of this view relative to the left edge of the parent.
			If p_willKeepWidth is true, the right coordinate will also be changed so that the width of the view stays the same.
		*/
		void setLeft(float p_left, bool p_willKeepWidth = true) override
		{
			if (p_left != m_bounds.left)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, 0);
				m_bounds.setLeft(p_left, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the left coordinate of this view and updates the layout relative to the left edge of the GUI.
			If p_willKeepWidth is true, the right coordinate will also be changed so that the width of the view stays the same.
		*/
		void setAbsoluteLeft(float p_left, bool p_willKeepWidth = true)
		{
			if (p_left != m_absolutePosition.x)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_absolutePosition.x, 0);
				m_bounds.setLeft(p_left - m_absolutePosition.x + m_bounds.left, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the left coordinate of this view relative to the left edge of the parent.
		*/
		float getLeft() const override
		{
			return m_bounds.left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the left coordinate of this view relative to the left edge of the GUI.
		*/
		float getAbsoluteLeft() const
		{
			return m_absolutePosition.x;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top coordinate of this view relative to the top edge of the parent.
			If p_willKeepHeight is true, the bottom coordinate will also be changed so that the height of the view stays the same.
		*/
		void setTop(float p_top, bool p_willKeepHeight = true) override
		{
			if (p_top != m_bounds.top)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_top - m_bounds.top);
				m_bounds.setTop(p_top, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top coordinate of this view relative to the top edge of the GUI.
			If p_willKeepHeight is true, the bottom coordinate will also be changed so that the height of the view stays the same.
		*/
		void setAbsoluteTop(float p_top, bool p_willKeepHeight = true)
		{
			if (p_top != m_absolutePosition.y)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_top - m_absolutePosition.y);
				m_bounds.setTop(p_top - m_absolutePosition.y + m_bounds.top, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the top coordinate of this view relative to the top edge of the parent.
		*/
		float getTop() const override
		{
			return m_bounds.top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the top coordinate of this view relative to the top edge of the GUI.
		*/
		float getAbsoluteTop() const
		{
			return m_absolutePosition.y;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the right coordinate of this view relative to the left edge of the parent.
			If p_willKeepWidth is true, the left coordinate will also be changed so that the width of the view stays the same.
		*/
		void setRight(float p_right, bool p_willKeepWidth = true) override
		{
			if (p_right != m_bounds.right)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepWidth)
				{
					moveAbsolutePositions(p_right - m_bounds.right, 0);
				}
				m_bounds.setRight(p_right, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the right coordinate of this view relative to the left edge of the GUI.
			If p_willKeepWidth is true, the left coordinate will also be changed so that the width of the view stays the same.
		*/
		void setAbsoluteRight(float p_right, bool p_willKeepWidth = true)
		{
			float offset = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			if (offset)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepWidth)
				{
					moveAbsolutePositions(offset, 0);
					m_bounds.moveX(offset);
				}
				else
				{
					m_bounds.right += offset;
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the right edge of this view relative to the left edge of the parent.
		*/
		float getRight() const override
		{
			return m_bounds.right;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the right edge of this view relative to the left edge of the GUI.
		*/
		float getAbsoluteRight() const
		{
			return m_absolutePosition.x + m_bounds.right - m_bounds.left;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom coordinate of this view relative to the top edge of the parent and updates the layout.
			If p_willKeepHeight is true, the top coordinate will also be changed so that the height of the view stays the same.
		*/
		void setBottom(float p_bottom, bool p_willKeepHeight = true) override
		{
			if (p_bottom != m_bounds.bottom)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepHeight)
				{
					moveAbsolutePositions(0, p_bottom - m_bounds.bottom);
				}
				m_bounds.setBottom(p_bottom, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom coordinate of this view relative to the top edge of the GUI and updates the layout.
			If p_willKeepHeight is true, the top coordinate will also be changed so that the height of the view stays the same.
		*/
		void setAbsoluteBottom(float p_bottom, bool p_willKeepHeight = true)
		{
			float offset = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offset)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepHeight)
				{
					m_bounds.moveY(offset);
					moveAbsolutePositions(0, offset);
				}
				else
				{
					m_bounds.bottom += offset;
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the bottom edge of this view relative to the top edge of the parent.
		*/
		float getBottom() const override
		{
			return m_bounds.bottom;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the bottom edge of this view relative to the top edge of the GUI.
		*/
		float getAbsoluteBottom() const
		{
			return m_absolutePosition.y + m_bounds.bottom - m_bounds.top;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the width of this view by changing the right coordinate and updates the layout.
		*/
		void setWidth(float p_width) override
		{
			if (p_width != m_bounds.right - m_bounds.left)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setWidth(p_width);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the distance between the left and right edges of this view.
		*/
		float getWidth() const override
		{
			return m_bounds.right - m_bounds.left;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the height of this view by changing the bottom coordinate and updates the layout.
		*/
		void setHeight(float p_height) override
		{
			if (p_height != m_bounds.bottom - m_bounds.top)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setHeight(p_height);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the distance between the top and bottom edges of this view.
		*/
		float getHeight() const override
		{
			return m_bounds.bottom - m_bounds.top;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the size of this view by changing the right and bottom coordinates and updates the layout.
		*/
		void setSize(Point<float> const& p_size) override
		{
			setSize(p_size.x, p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the size of this view by changing the right and bottom coordinates and updates the layout.
		*/
		void setSize(float p_width, float p_height) override
		{
			if (p_width != m_bounds.right - m_bounds.left || p_height != m_bounds.bottom - m_bounds.top)
			{
				Avo::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setSize(p_width, p_height);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			x is width and y is height in the returned point.
		*/
		Point<float> getSize() const override
		{
			return Point<float>(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps a rectangle that is relative to the top left corner of the parent.
			If you have a custom clipping geometry, you could override this.
		*/
		bool getIsIntersecting(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom))
				{
					if (p_right < m_bounds.left + m_corners.topLeftSizeX && p_bottom < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_right, (m_bounds.top + m_corners.topLeftSizeY - p_bottom) * m_corners.topLeftSizeX / m_corners.topLeftSizeY) < m_corners.topLeftSizeX * m_corners.topLeftSizeX;
						}
						return p_bottom > m_bounds.top + m_corners.topLeftSizeY - (p_right - m_bounds.left) * m_corners.topLeftSizeY / m_corners.topLeftSizeX;
					}
					else if (p_right < m_bounds.left + m_corners.bottomLeftSizeX && p_top > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_right, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_top) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) < m_corners.bottomLeftSizeX * m_corners.bottomLeftSizeX;
						}
						return p_top < m_bounds.bottom - m_corners.bottomLeftSizeY + (p_right - m_bounds.left) * m_corners.bottomLeftSizeY / m_corners.bottomLeftSizeX;
					}
					else if (p_left > m_bounds.right - m_corners.topRightSizeX && p_bottom < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_left, (m_bounds.top + m_corners.topRightSizeY - p_bottom) * m_corners.topRightSizeX / m_corners.topRightSizeY) < m_corners.topRightSizeX * m_corners.topRightSizeX;
						}
						return p_bottom > m_bounds.top + (m_bounds.right - p_left) * m_corners.topRightSizeY / m_corners.topRightSizeX;
					}
					else if (p_left > m_bounds.right - m_corners.bottomRightSizeX && p_top > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_left, (m_bounds.bottom - m_corners.bottomRightSizeY - p_top) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) < m_corners.bottomRightSizeX * m_corners.bottomRightSizeX;
						}
						return p_top < m_bounds.bottom - (m_bounds.right - p_left) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps a rectangle that is relative to the top left corner of the parent.
		*/
		bool getIsIntersecting(Rectangle<float> const& p_rectangle) const override
		{
			return getIsIntersecting(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps another protected rectangle that is relative to the top left corner of the parent.
		*/
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
		{
			return getIsIntersecting(p_protectedRectangle->getLeft(), p_protectedRectangle->getTop(), p_protectedRectangle->getRight(), p_protectedRectangle->getBottom());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps another view. Takes rounded corners of both views into account.
		*/
		//bool getIsIntersecting(View* p_view) const;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether a rectangle can be contained within this view. The rectangle is relative to the parent of this view.
			If you have a custom clipping geometry, you could override this.
		*/
		bool getIsContaining(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom))
				{
					if (p_left < m_bounds.left + m_corners.topLeftSizeX && p_top < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_left, (m_bounds.top + m_corners.topLeftSizeY - p_top) * m_corners.topLeftSizeX / m_corners.topLeftSizeY) > m_corners.topLeftSizeX * m_corners.topLeftSizeX)
							{
								return false;
							}
						}
						else if (p_top > m_bounds.top + m_corners.topLeftSizeY - (p_left - m_bounds.left) * m_corners.topLeftSizeY / m_corners.topLeftSizeX)
						{
							return false;
						}
					}
					else if (p_left < m_bounds.left + m_corners.bottomLeftSizeX && p_bottom > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_left, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_bottom) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) > m_corners.bottomLeftSizeX * m_corners.bottomLeftSizeX)
							{
								return false;
							}
						}
						else if (p_bottom < m_bounds.bottom - m_corners.bottomLeftSizeY + (m_bounds.right - p_left) * m_corners.topRightSizeY / m_corners.topRightSizeX)
						{
							return false;
						}
					}
					else if (p_right > m_bounds.right - m_corners.topRightSizeX && p_top < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_right, (m_bounds.top + m_corners.topRightSizeY - p_top) * m_corners.topRightSizeX / m_corners.topRightSizeY) > m_corners.topRightSizeX * m_corners.topRightSizeX)
							{
								return false;
							}
						}
						else if (p_top > m_bounds.top + (m_bounds.right - p_right) * m_corners.topRightSizeY / m_corners.topRightSizeY)
						{
							return false;
						}
					}
					else if (p_right > m_bounds.right - m_corners.bottomRightSizeX && p_bottom > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_right, (m_bounds.bottom - m_corners.bottomRightSizeY - p_bottom) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) > m_corners.bottomRightSizeX * m_corners.bottomRightSizeX)
							{
								return false;
							}
						}
						else if (p_bottom < m_bounds.bottom - (m_bounds.right - p_right) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX)
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a rectangle can be contained within this view. The rectangle is relative to the parent of this view.
		*/
		bool getIsContaining(Rectangle<float> const& p_rectangle)
		{
			return getIsContaining(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether another protected rectangle can be contained within this view. The rectangle is relative to the parent of this view.
		*/
		bool getIsContaining(ProtectedRectangle* p_rectangle) const override
		{
			return getIsContaining(p_rectangle->getLeft(), p_rectangle->getTop(), p_rectangle->getRight(), p_rectangle->getBottom());
		}

		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the parent of this view.
			If you have a custom clipping geometry, you could override this.
		*/
		bool getIsContaining(float p_x, float p_y) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_x, p_y))
				{
					if (p_x < m_bounds.left + m_corners.topLeftSizeX && p_y < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_x, (m_bounds.top + m_corners.topLeftSizeY - p_y) * m_corners.topLeftSizeX / m_corners.topLeftSizeY) < m_corners.topLeftSizeX * m_corners.topLeftSizeX;
						}
						return p_y > m_bounds.top + m_corners.topLeftSizeY - (p_x - m_bounds.left) * m_corners.topLeftSizeY / m_corners.topLeftSizeX;
					}
					else if (p_x > m_bounds.right - m_corners.topRightSizeX && p_y < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topRightType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_x, (m_bounds.top + m_corners.topRightSizeY - p_y) * m_corners.topRightSizeX / m_corners.topRightSizeY) < m_corners.topRightSizeX * m_corners.topRightSizeX;
						}
						return p_y > m_bounds.top + (m_bounds.right - p_x) * m_corners.topRightSizeY / m_corners.topRightSizeX;
					}
					else if (p_x < m_bounds.left + m_corners.bottomLeftSizeX && p_y > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.bottomLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_x, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_y) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) < m_corners.bottomLeftSizeX * m_corners.bottomLeftSizeX;
						}
						return p_y < m_bounds.bottom - m_corners.bottomLeftSizeY + (p_x - m_bounds.left) * m_corners.bottomLeftSizeY / m_corners.bottomLeftSizeX;
					}
					else if (p_x > m_bounds.right - m_corners.bottomRightSizeX && p_y > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.bottomRightType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_x, (m_bounds.bottom - m_corners.bottomRightSizeY - p_y) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) < m_corners.bottomRightSizeX * m_corners.bottomRightSizeX;
						}
						return p_y < m_bounds.bottom - (m_bounds.right - p_x) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_x, p_y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the parent of the view.
		*/
		bool getIsContaining(Point<float> const& p_point) const override
		{
			return getIsContaining(p_point.x, p_point.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
		*/
		bool getIsContainingAbsolute(float p_x, float p_y) const
		{
			if (m_parent)
			{
				return getIsContaining(p_x - m_parent->getAbsoluteLeft(), p_y - m_parent->getAbsoluteTop());
			}
			return getIsContaining(p_x, p_y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
		*/
		bool getIsContainingAbsolute(Point<float> const& p_point) const
		{
			if (m_parent)
			{
				return getIsContaining(p_point.x - m_parent->getAbsoluteLeft(), p_point.y - m_parent->getAbsoluteTop());
			}
			return getIsContaining(p_point.x, p_point.y);
		}

		//------------------------------

	private:
		RectangleCorners m_corners;

	public:
		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the corners of the view. p_radius is the radius of the corner circles.
		*/
		void setCornerRadius(float p_radius)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_radius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_radius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_radius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_radius;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the different corners of the view, as the radius of a quarter circle.
		*/
		void setCornerRadius(float p_topLeftRadius, float p_topRightRadius, float p_bottomLeftRadius, float p_bottomRightRadius)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_topLeftRadius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_topRightRadius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_bottomLeftRadius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_bottomRightRadius;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
			updateShadow();
		}

		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		void setCornerCutSize(float p_cutSize)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_cutSize;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_cutSize;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_cutSize;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_cutSize;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Cut;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		void setCornerCutSize(float p_topLeftSize, float p_topRightSize, float p_bottomLeftSize, float p_bottomRightSize)
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
		void setCorners(RectangleCorners const& p_corners)
		{
			m_corners = p_corners;
			updateClipGeometry();
			updateShadow();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the shapes of the corners of the view.
		*/
		RectangleCorners& getCorners()
		{
			return m_corners;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether any of the corners of the view are non-rectangular.
		*/
		bool getHasCornerStyles()
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
		virtual void handleCharacterInput(KeyboardEvent const& p_event) { }

		EventListeners<void(KeyboardEvent const&)> keyboardKeyDownListeners;
		/*
			USER IMPLEMENTED
			This method is called when a keyboard key has been pressed.
			Only p_event.key and p_event.isRepeated are valid for this event type.
		*/
		virtual void handleKeyboardKeyDown(KeyboardEvent const& p_event) { }

		EventListeners<void(KeyboardEvent const&)> keyboardKeyUpListeners;
		/*
			USER IMPLEMENTED
			This method is called when a keyboard key has been released.
			Only p_event.key is valid for this event type.
		*/
		virtual void handleKeyboardKeyUp(KeyboardEvent const& p_event) { }

		EventListeners<void()> keyboardFocusLoseListeners;
		/*
			USER IMPLEMENTED
			Gets called when another keyboard event listener becomes the target of keyboard events.
		*/
		virtual void handleKeyboardFocusLose() { }

		EventListeners<void()> keyboardFocusGainListeners;
		/*
			USER IMPLEMENTED
			Gets called when this keyboard event listener becomes the target of keyboard events.
		*/
		virtual void handleKeyboardFocusGain() { }

		//------------------------------

	private:
		bool m_areDragDropEventsEnabled{ false };
		bool m_isDraggingOver{ false };
	public:
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		void enableDragDropEvents()
		{
			m_areDragDropEventsEnabled = true;
		}
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		void disableDragDropEvents()
		{
			m_areDragDropEventsEnabled = false;
		}
		/*
			LIBRARY IMPLEMENTED
			Drag drop events are disabled by default.
		*/
		bool getAreDragDropEventsEnabled() const
		{
			return m_areDragDropEventsEnabled;
		}

		/*
			LIBRARY IMPLEMENTED (only default behavior)
			This is used to tell the OS what type of operation the application supports for the dragged data when dropped at the mouse location specified by p_event.x and p_event.y.
			The coordinates are relative to the top-left corner of this view.
		*/
		virtual DragDropOperation getDragDropOperation(DragDropEvent const& p_event)
		{
			return DragDropOperation::None;
		}

		EventListeners<void(DragDropEvent const&)> dragDropEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor enters the bounds of the view during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual void handleDragDropEnter(DragDropEvent const& p_event) { }

		EventListeners<void(DragDropEvent const&)> dragDropBackgroundEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor enters the parts of the view that are not occupied by children, during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual void handleDragDropBackgroundEnter(DragDropEvent const& p_event) { }

		EventListeners<void(DragDropEvent const&)> dragDropMoveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor moves over the view during a drag and drop operation.
		*/
		virtual void handleDragDropMove(DragDropEvent const& p_event) { }

		EventListeners<void(DragDropEvent const&)> dragDropLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor leaves the bounds of the view during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual void handleDragDropLeave(DragDropEvent const& p_event) { }

		EventListeners<void(DragDropEvent const&)> dragDropBackgroundLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the cursor leaves the parts of the view that are not occupied by children, during a drag and drop operation.
			p_event contains information about the event, including the data and data type of what is to be dropped.
		*/
		virtual void handleDragDropBackgroundLeave(DragDropEvent const& p_event) { }

		EventListeners<void(DragDropEvent const&)> dragDropFinishListeners;
		/*
			USER IMPLEMENTED
			Gets called when the user drops data above the view, finishing a drag and drop operation.
			p_event contains information about the event, including the data and data type of what was dropped.
		*/
		virtual void handleDragDropFinish(DragDropEvent const& p_event) { }

		//------------------------------

	private:
		bool m_areMouseEventsEnabled{ false };
		bool m_isMouseHovering{ false };
		Cursor m_cursor{ Cursor::Arrow };
	public:
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		void enableMouseEvents()
		{
			m_areMouseEventsEnabled = true;
		}
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		void disableMouseEvents()
		{
			m_areMouseEventsEnabled = false;
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		bool getAreMouseEventsEnabled()
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
		virtual void handleMouseDown(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseUpListeners;
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been released after having been pressed down when the mouse pointer was above the view.
			The mouse cursor may have left the view during the time the button is pressed, but it will still receive the event.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseUp(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseDoubleClickListeners;
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been double clicked while the mouse pointer is above the view.
			The default implementation calls handleMouseUp.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseDoubleClick(MouseEvent const& p_event)
		{
			handleMouseUp(p_event);
		}

		EventListeners<MouseListener> mouseMoveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has been moved within the bounds of the view.
			This can be blocked by non-overlay views which have the same parent and are overlapping this one.
			The event is never blocked by children of this view.
			If it has entered the view, a mouse enter event is sent, and if it has left the view, a mouse leave event is sent.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseMove(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseEnterListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has entered the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
			that the mouse has entered gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseEnter(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse cursor has left the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
			that the mouse has left gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseLeave(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseBackgroundEnterListeners;
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when the mouse pointer has entered any part of the view that is not occupied by children of this view.
			By default, this changes the mouse cursor to the cursor that is set with setCursor on the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseBackgroundEnter(MouseEvent const& p_event);

		EventListeners<MouseListener> mouseBackgroundLeaveListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has left any part of the view that is not occupied by children of this view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseBackgroundLeave(MouseEvent const& p_event) { }

		EventListeners<MouseListener> mouseScrollListeners;
		/*
			USER IMPLEMENTED
			Gets called when the mouse wheel has been moved/scrolled while the mouse pointer is above the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseScroll(MouseEvent const& p_event) { }

		//------------------------------
		// Size change events

	private:
		virtual void sendBoundsChangeEvents(Rectangle<float> const& p_previousBounds);
	public:
		EventListeners<void(Rectangle<float> const&)> boundsChangeListeners;
		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the bounds of the view have been changed.
		*/
		virtual void handleBoundsChange(Rectangle<float> const& p_previousBounds) { }

	public:
		/*
			Listener signature:
				void (View* target, float previousWidth, float previousHeight)
			target is a pointer to the view that changed size.
			See View::handleSizeChange for more info.
		*/
		EventListeners<void(float, float)> sizeChangeListeners;
		/*
			LIBRARY IMPLEMENTED
			This calls handleSizeChange() by default. Override this method if you need to know the previous size of the view.
		*/
		virtual void handleSizeChange(float p_previousWidth, float p_previousHeight)
		{
			handleSizeChange();
		}
		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the size of the view has been changed.
		*/
		virtual void handleSizeChange() { }

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
		virtual void handleChildViewAttachment(View* p_attachedChild) { }

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
		virtual void handleChildViewDetachment(View* p_detachedChild) { }
	};

	//------------------------------

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

		static uint32 s_numberOfInstances;

		Gui* m_parent = nullptr;
		Window* m_window = nullptr;
		DrawingContext* m_drawingContext = nullptr;
		DrawingState* m_drawingContextState = nullptr;

		//------------------------------

		void handleThemeColorChange(Id const& p_id, Color const& p_newColor) override
		{
			if (p_id == ThemeColors::background)
			{
				getDrawingContext()->setBackgroundColor(p_newColor);
			}
		}

	public:
		Gui();
		Gui(Component* p_parent);
		~Gui() override;

		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to Avo::GUI::createContent will be made when these objects have been created.
			After that, an initial call to Avo::GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.
			p_positionFactorY is the vertical equivalent to p_positionFactorX.
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string const& p_title, float p_positionFactorX, float p_positionFactorY, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr);
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to Avo::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to Avo::GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string const& p_title, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr);
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to Avo::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to Avo::GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_size is the size of the client area in DIPS (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string const& p_title, Point<float> const& p_size, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr)
		{
			create(p_title, p_size.x, p_size.y, p_windowFlags, p_parent);
		}
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to Avo::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to Avo::GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_positionFactor is the position of the window, expressed as horizontal and vertical factors between 0 and 1, 
			where (0, 0) means the top left corner of the primary monitor and the top left edge of the window are aligned, 
			and (1, 1) means the bottom right corners are aligned.
			p_size is the size of the client area in DIPS (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string const& p_title, Point<float> const& p_positionFactor, Point<float> const& p_size, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr)
		{
			create(p_title, p_positionFactor.x, p_positionFactor.y, p_size.x, p_size.y, p_windowFlags, p_parent);
		}
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to Avo::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to Avo::GUI::handleSizeChange will also be made.

			run must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.
			p_positionFactorY is the vertical equivalent to p_positionFactorX.
			p_size is the size of the client area in DIPS (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(std::string const& p_title, float p_positionFactorX, float p_positionFactorY, Point<float> const& p_size, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = nullptr)
		{
			create(p_title, p_positionFactorX, p_positionFactorY, p_size.x, p_size.y, p_windowFlags, p_parent);
		}

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
			if (getWidth() == m_window->getWidth() && getHeight() == m_window->getHeight())
			{
				View::sendBoundsChangeEvents(Avo::Rectangle<float>());
			}
			invalidate();

			m_window->run();
			m_animationThread = std::thread(&Avo::Gui::thread_runAnimationLoop, this);

			static std::vector<Gui*> s_instancesToJoin;
			static std::mutex s_instancesToJoinMutex;
			static bool s_isWaitingForInstancesToFinish{ false };

			if (s_isWaitingForInstancesToFinish)
			{
				m_animationThread.detach();
			}
			else
			{
				s_instancesToJoinMutex.lock();
				s_instancesToJoin.push_back(this);
				s_instancesToJoinMutex.unlock();
			}
			if (s_numberOfInstances == s_instancesToJoin.size() && !s_isWaitingForInstancesToFinish)
			{
				s_isWaitingForInstancesToFinish = true;

				s_instancesToJoinMutex.lock();
				auto instancesToJoin(std::move(s_instancesToJoin));
				s_instancesToJoin = std::vector<Gui*>();
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

		/*
			LIBRARY IMPLEMENTED
			Returns the GUI that owns the parent window of the window of this GUI.
			If the window does not have a parent, it returns 0.
		*/
		Gui* getParent()
		{
			return m_parent;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given.
		*/
		View* getViewAt(Point<float> const& p_coordinates);
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given.
		*/
		View* getViewAt(float p_x, float p_y);

		//------------------------------

	private:
		TimerThread m_timerThread;
	public:
		/*
			Adds a function that will be called in p_milliseconds milliseconds from now
			and returns an ID that identifies the timer callback.
		*/
		Id addTimerCallback(std::function<void()> p_callback, float p_milliseconds)
		{
			return m_timerThread.addCallback(p_callback, p_milliseconds);
		}
		void cancelTimerCallback(Id const& p_id)
		{
			m_timerThread.cancelCallback(p_id);
		}

		//------------------------------

	private:
		std::recursive_mutex m_sharedStateMutex;

		std::deque<View*> m_viewAnimationUpdateQueue;
		std::deque<Animation*> m_animationUpdateQueue;

		bool m_hasAnimationLoopStarted = false;
		std::thread m_animationThread;

		void thread_runAnimationLoop();
	public:
		/*
			LIBRARY IMPLEMENTED
			Gives the running thread exclusive access to modify any state that is shared by the event thread and animation thread.
		*/
		void lockThreads()
		{
			m_sharedStateMutex.lock();
		}
		/*
			LIBRARY IMPLEMENTED
			Gives back the other threads access to modify any state that is shared by the event thread and animation thread.
		*/
		void unlockThreads()
		{
			m_sharedStateMutex.unlock();
		}
		/*
			LIBRARY IMPLEMENTED

		*/
		auto createThreadLock()
		{
			return std::scoped_lock(m_sharedStateMutex);
		}

		//------------------------------

	private:
		void handleWindowCreate(WindowEvent const& p_event);
		void handleWindowDestroy(WindowEvent const& p_event);

		Point<float> m_lastUpdatedWindowSize;
		void handleWindowSizeChange(WindowEvent const& p_event);

		void sendBoundsChangeEvents(Avo::Rectangle<float> const& p_previousBounds) override
		{
			if ((uint32)getWidth() != (uint32)m_window->getSize().x || (uint32)getHeight() != (uint32)m_window->getSize().y)
			{
				m_window->setSize(getSize());
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
		DragDropOperation getGlobalDragDropOperation(DragDropEvent& p_event)
		{
			std::vector<View*> targets = getTopMouseListenersAt(p_event.x, p_event.y);

			float absoluteX = p_event.x, absoluteY = p_event.y;

			auto result = DragDropOperation::None;
			for (auto target : targets)
			{
				p_event.x = absoluteX - target->getAbsoluteLeft();
				p_event.y = absoluteY - target->getAbsoluteTop();
				auto operation = target->getDragDropOperation(p_event);
				if (operation != DragDropOperation::None)
				{
					result = operation;
					break;
				}
			}
			for (auto target : targets)
			{
				target->forget();
			}
			p_event.x = absoluteX;
			p_event.y = absoluteY;

			return result;
		}

		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalDragDropEnter(DragDropEvent& p_event)
		{
			handleGlobalDragDropMove(p_event);
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalDragDropMove(DragDropEvent& p_event);
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalDragDropLeave(DragDropEvent& p_event);
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalDragDropFinish(DragDropEvent& p_event)
		{
			if (m_areDragDropEventsEnabled)
			{
				handleDragDropFinish(p_event);
			}

			float absoluteX = p_event.x;
			float absoluteY = p_event.y;

			View* container = this;
			int32 startIndex = getNumberOfChildViews() - 1;

			bool hasFoundTopView = false;

			while (true)
			{
			loopStart:
				for (int32 a = startIndex; a >= 0; a--)
				{
					View* child = container->getChildView(a);
					if (child->getIsVisible() && child->getIsContainingAbsolute(absoluteX, absoluteY))
					{
						bool hasChildren = child->getNumberOfChildViews();

						if (hasChildren)
						{
							if (child->m_areDragDropEventsEnabled)
							{
								p_event.x = absoluteX - child->getAbsoluteLeft();
								p_event.y = absoluteY - child->getAbsoluteTop();
								child->dragDropFinishListeners(p_event);
							}
							container = child;
							startIndex = container->getNumberOfChildViews() - 1;
							goto loopStart;
						}
						else
						{
							if (child->m_areDragDropEventsEnabled)
							{
								p_event.x = absoluteX - child->getAbsoluteLeft();
								p_event.y = absoluteY - child->getAbsoluteTop();
								child->dragDropFinishListeners(p_event);
							}

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
		std::vector<View*> getTopMouseListenersAt(Point<float> const& p_coordinates);
		/*
			Returns the topmost non-overlay view which contains the coordinates given, as well as any overlay views which are above the non-overlay view.
		*/
		std::vector<View*> getTopMouseListenersAt(float p_x, float p_y);

		std::vector<View*> m_pressedMouseEventListeners;
		Point<float> m_mouseDownPosition;
	public:
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseDown(MouseEvent& p_event)
		{
			auto targets = getTopMouseListenersAt(p_event.x, p_event.y);

			float absoluteX = p_event.x;
			float absoluteY = p_event.y;

			if (targets.size())
			{
				for (View* view : targets)
				{
					p_event.x = absoluteX - view->getAbsoluteLeft();
					p_event.y = absoluteY - view->getAbsoluteTop();

					view->mouseDownListeners(p_event);
					m_pressedMouseEventListeners.push_back(view);
				}
			}

			m_mouseDownPosition.set(absoluteX, absoluteY);
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseUp(MouseEvent& p_event)
		{
			float absoluteX = p_event.x;
			float absoluteY = p_event.y;
			if (!m_pressedMouseEventListeners.empty())
			{
				for (auto view : m_pressedMouseEventListeners)
				{
					p_event.x = absoluteX - view->getAbsoluteLeft();
					p_event.y = absoluteY - view->getAbsoluteTop();
					view->mouseUpListeners(p_event);
				}
				for (auto view : m_pressedMouseEventListeners)
				{
					view->forget();
				}
				m_pressedMouseEventListeners.clear();

				if (absoluteX != m_mouseDownPosition.x || absoluteY != m_mouseDownPosition.y)
				{
					p_event.x = absoluteX;
					p_event.y = absoluteY;
					p_event.movementX = absoluteX - m_mouseDownPosition.x;
					p_event.movementY = absoluteY - m_mouseDownPosition.y;
					handleGlobalMouseMove(p_event); // This is so that any views that the mouse has entered while pressed get their events.
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseDoubleClick(MouseEvent& p_event)
		{
			auto targets = getTopMouseListenersAt(p_event.x, p_event.y);

			float absoluteX = p_event.x;
			float absoluteY = p_event.y;

			if (!targets.empty())
			{
				for (auto view : targets)
				{
					p_event.x = absoluteX - view->getAbsoluteLeft();
					p_event.y = absoluteY - view->getAbsoluteTop();

					view->mouseDoubleClickListeners(p_event);
				}
				for (auto view : targets)
				{
					view->forget();
				}
			}
		}
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseMove(MouseEvent& p_event);
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseLeave(MouseEvent& p_event);
		/*
			LIBRARY IMPLEMENTED
		*/
		void handleGlobalMouseScroll(MouseEvent& p_event)
		{
			auto targets = getTopMouseListenersAt(p_event.x, p_event.y);

			float absoluteX = p_event.x;
			float absoluteY = p_event.y;

			if (!targets.empty())
			{
				for (auto view : targets)
				{
					p_event.x = absoluteX - view->getAbsoluteLeft();
					p_event.y = absoluteY - view->getAbsoluteTop();

					view->mouseScrollListeners(p_event);
				}
				for (auto view : targets)
				{
					view->forget();
				}
			}

			p_event.x = absoluteX;
			p_event.y = absoluteY;
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
		virtual WindowBorderArea getWindowBorderAreaAtPosition(float p_x, float p_y)
		{
			constexpr float borderWidth = 5.f;
			constexpr float diagonalBorderWidth = 7.f;

			if (p_y < diagonalBorderWidth)
			{
				if (p_x < diagonalBorderWidth)
				{
					return WindowBorderArea::TopLeftResize;
				}
				if (p_x >= getWidth() - diagonalBorderWidth)
				{
					return WindowBorderArea::TopRightResize;
				}
				if (p_y < borderWidth)
				{
					return WindowBorderArea::TopResize;
				}
			}
			if (p_y >= getHeight() - diagonalBorderWidth)
			{
				if (p_x < diagonalBorderWidth)
				{
					return WindowBorderArea::BottomLeftResize;
				}
				if (p_x >= getWidth() - diagonalBorderWidth)
				{
					return WindowBorderArea::BottomRightResize;
				}
				if (p_y >= getHeight() - borderWidth)
				{
					return WindowBorderArea::BottomResize;
				}
			}
			if (p_x < borderWidth)
			{
				return WindowBorderArea::LeftResize;
			}
			if (p_x >= getWidth() - borderWidth)
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
		void setKeyboardFocus(View* p_view)
		{
			if (m_keyboardFocus == p_view)
			{
				return;
			}

			auto focusBefore = m_keyboardFocus;

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
		View* getKeyboardFocus()
		{
			return m_keyboardFocus;
		}

		void sendGlobalCharacterInputEvents(KeyboardEvent const& p_event)
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->characterInputListeners(p_event);
			}
			globalCharacterInputListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalCharacterInputListeners;

		void sendGlobalKeyboardKeyDownEvents(KeyboardEvent const& p_event)
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->keyboardKeyDownListeners(p_event);
			}
			globalKeyboardKeyDownListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalKeyboardKeyDownListeners;

		void sendGlobalKeyboardKeyUpEvents(KeyboardEvent const& p_event)
		{
			if (m_keyboardFocus)
			{
				m_keyboardFocus->keyboardKeyUpListeners(p_event);
			}
			globalKeyboardKeyUpListeners(p_event);
		}
		EventListeners<void(KeyboardEvent const&)> globalKeyboardKeyUpListeners;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the window used by this GUI.
		*/
		Window* getWindow()
		{
			return m_window;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the drawing context used by this GUI.
		*/
		DrawingContext* getDrawingContext()
		{
			return m_drawingContext;
		}

		//------------------------------

	private:
		std::mutex m_invalidRectanglesMutex;
		std::vector<Rectangle<float>> m_invalidRectangles;
	public:
		/*
			LIBRARY IMPLEMENTED
			Invalidates a part of the GUI that has been changed, and therefore needs to be redrawn.
			Views that intersect with any invalid rectangles will be drawn in the next call to drawViews() (which is made internally) automatically.
		*/
		void invalidateRectangle(Rectangle<float> p_rectangle);
		/*
			LIBRARY IMPLEMENTED
			Invalidates a part of the GUI that has been changed, and therefore needs to be redrawn.
			Views that intersect with any invalid rectangles will be drawn in the next call to drawViews() (which is made internally) automatically.
		*/
		void invalidateRectangle(float p_left, float p_top, float p_right, float p_bottom)
		{
			invalidateRectangle(Rectangle<float>(p_left, p_top, p_right, p_bottom));
		}
		/*
			LIBRARY IMPLEMENTED
			Draws the invalid rectangles of the GUI. This method should only be called internally by the library. Use draw() to draw things directly on the GUI.
		*/
		void drawViews();
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
		Animation* m_showAnimation = createAnimation(ThemeEasings::out, 100, [=](float p_value) {
			m_opacity = p_value;
			invalidate();
		});
		bool m_isShowing = false;
		Id m_timerId = 0;
	public:
		/*
			Makes the tooltip appear
			p_string is the string to be displayed on the tooltip.
			p_targetBounds is the area that the tooltip points to and is relative to the parent of this tooltip. The tooltip decides the exact positioning.
		*/
		virtual void show(std::string const& p_string, Rectangle<float> const& p_targetRectangle)
		{
			if (!m_isShowing)
			{
				if (!m_text || p_string != m_text.getString())
				{
					auto fontSize = getThemeValue(ThemeValues::tooltipFontSize);
					m_text = getGui()->getDrawingContext()->createText(p_string, fontSize);
					m_text.fitSizeToText();
					setSize(m_text.getWidth() + 2.2f * fontSize, m_text.getHeight() + fontSize*1.8f);
					m_text.setCenter(getSize()/2);
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
					m_showAnimation->play(false);
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
					m_showAnimation->play(true);
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
				p_drawingContext->scale(m_opacity * 0.3f + 0.7f, getAbsoluteCenter());
				p_drawingContext->setColor({ getThemeColor(ThemeColors::tooltipBackground), m_opacity });
				p_drawingContext->fillRectangle(getSize());
				p_drawingContext->setColor(Color(getThemeColor(ThemeColors::tooltipOnBackground), m_opacity));
				p_drawingContext->drawText(m_text);
				p_drawingContext->scale(1.f / (m_opacity * 0.3f + 0.7f), getAbsoluteCenter());
			}
		}

		//------------------------------

		Tooltip(View* p_parent) :
			View{ p_parent }
		{
			initializeThemeColor(ThemeColors::tooltipBackground, { 0.2f, 0.8f });
			initializeThemeColor(ThemeColors::tooltipOnBackground, { 1.f, 0.95f });
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
		void setTitle(char const* p_title)
		{
			m_title = p_title;
		}
		/*
			Sets the title shown in the top border of the open file dialog.
		*/
		void setTitle(std::string const& p_title)
		{
			m_title = p_title;
		}
		/*
			Returns the title shown in the thop border of the open file dialog.
		*/
		std::string const& getTitle()
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
				{ "Images", "*.jpg;*.png" }
				{ "Sound files", "*.mp3;*.wav;*.ogg" }
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
				{ "Images", "*.jpg;*.png" }
				{ "Sound files", "*.mp3;*.wav;*.ogg" }
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
			m_gui{ nullptr }
		{ }
		OpenFileDialog(Gui* p_gui) :
			m_gui{ p_gui }
		{ }
	};

	//------------------------------

	/*
		A view that displays text.
	*/
	class TextView : public View
	{
	private:
		Avo::Color m_color{ getThemeColor(ThemeColors::onBackground) };
	public:
		void setColor(Avo::Color const& p_color)
		{
			m_color = p_color;
		}
		Avo::Color getColor()
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
		void setString(std::string const& p_string)
		{
			if (p_string.empty())
			{
				return;
			}
			m_text = getDrawingContext()->createText(p_string, m_fontSize);
			//m_text.setSize(getSize());
			setSize(m_text.getSize());
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

		void draw(Avo::DrawingContext* p_context) override
		{
			if (m_text)
			{
				p_context->setColor(m_color);
				p_context->drawText(m_text);
			}
		}

		//------------------------------

		TextView(View* p_parent, float p_fontSize, std::string const& p_string = "") :
			View{ p_parent },
			m_fontSize{ p_fontSize }
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
		void setColor(Color const& p_color)
		{
			m_color = p_color;
		}
		/*
			Returns the color that is used by the ripple and hover effects.
		*/
		Color const& getColor()
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

		//------------------------------

	private:
		Point<float> m_position;
		float m_size = 0.f;

		float m_alphaFactor = 0.f;
		float m_overlayAlphaFactor = 0.f;

	public:
		void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
		{
			if (m_isEnabled)
			{
				p_drawingContext->setColor(Color(m_color, m_color.alpha * m_overlayAlphaFactor * 0.3f));
				p_drawingContext->fillRectangle(getSize());

				if (m_color.alpha * m_alphaFactor >= 0.f)
				{
					p_drawingContext->setColor(Color(m_color, m_color.alpha * m_alphaFactor * 0.8f));
					p_drawingContext->fillCircle(m_position, m_size * 0.5f);
				}
			}
		}

	private:
		float m_maxSize = 0.f;
	public:
		void updateMaxSize()
		{
			m_maxSize = 2.f * Point<>::getDistanceFast(m_position, Point<>{
				m_position.x < getWidth() * 0.5f ? getWidth() : 0.f,
				m_position.y < getHeight() * 0.5f ? getHeight() : 0.f
			});
		}

	private:
		bool m_isMouseDown = false;
	public:
		Ripple(View* p_parent, Color const& p_color = { 1.f, 0.45f }) :
			View{ p_parent, p_parent->getBounds().createCopyAtOrigin() },
			m_color{ p_color }
		{
			initializeThemeEasing(ThemeEasings::ripple, { 0.1, 0.8, 0.2, 0.95 });
			initializeThemeValue(ThemeValues::rippleDuration, 300);

			setIsOverlay(true); // Mouse events should be sent through
			setHasShadow(false);
			setElevation(FLT_MAX); // Nothing can be above a ripple...
			enableMouseEvents();

			p_parent->sizeChangeListeners += [=](auto...) {
				setSize(getParent<View>()->getSize());
				updateMaxSize();
			};

			auto rippleFadeAnimation = createAnimation(ThemeEasings::inOut, 400, [=](float p_value) {
				m_alphaFactor = 1.f - p_value;
				invalidate();
			});
			auto rippleAnimation = createAnimation(ThemeEasings::ripple, getThemeValue(ThemeValues::rippleDuration), [=](float p_value) {
				m_size = interpolate(m_maxSize * 0.4f, m_maxSize, p_value);
				m_alphaFactor = 1.f;
				if (!m_isMouseDown && p_value == 1.f)
				{
					rippleFadeAnimation->replay();
				}
				invalidate();
			});

			auto mouseDownListener = [=](MouseEvent const& p_event) {
				if (m_isEnabled && p_event.mouseButton == MouseButton::Left)
				{
					rippleFadeAnimation->stop();

					m_isMouseDown = true;
					m_position.set(p_event.x - getLeft(), p_event.y - getTop());
					m_alphaFactor = 1.f;
					updateMaxSize();

					rippleAnimation->replay();
				}
			};
			mouseDownListeners += mouseDownListener;
			mouseDoubleClickListeners += [=](auto p_event) { 
				mouseDownListener(p_event);  
				m_isMouseDown = false;
			};
			mouseUpListeners += [=](MouseEvent const& p_event) {
				if (m_isMouseDown && p_event.mouseButton == MouseButton::Left)
				{
					m_isMouseDown = false;
					if (m_size == m_maxSize && m_alphaFactor == 1.f)
					{
						rippleAnimation->stop();
						rippleFadeAnimation->replay();
					}
				}
			};

			auto hoverAnimation = createAnimation(ThemeEasings::inOut, getThemeValue(ThemeValues::hoverAnimationDuration), [=](float p_value) {
				m_overlayAlphaFactor = p_value;
				invalidate();
			});
			mouseBackgroundEnterListeners += [=](auto) {
				hoverAnimation->play(false);
			};
			mouseBackgroundLeaveListeners += [=](auto) {
				hoverAnimation->play(true);
			};
		}
	};

	//------------------------------

	namespace ThemeValues
	{
		inline Id const buttonFontSize;
		inline Id const buttonCharacterSpacing;
	}

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

		Tooltip* m_tooltipView{ nullptr };
		std::string m_tooltipString;

		Image m_icon;

		float m_pressAnimationTime{ 1.f };
		bool m_isPressed{ false };
		bool m_isRaising{ false };
		Emphasis m_emphasis;

		bool m_isEnabled{ true };
		Color m_currentColor;
		float m_colorAnimationTime{ 1.f };
		bool m_isAccent{ false };

		bool m_isMouseHovering{ false };

		Ripple* m_ripple{ nullptr };

	protected:
		void handleThemeValueChange(Id const& p_id, float p_newValue) override
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
		void handleThemeColorChange(Id const& p_id, Color const& p_newColor) override
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
				m_ripple->setColor(Avo::Color(p_newColor, 0.3f));
			}
		}

	public:
		explicit Button(View* p_parent, std::string const& p_text = "", Emphasis p_emphasis = Emphasis::High, bool p_isAccent = false) :
			View(p_parent),
			m_emphasis(p_emphasis)
		{
			initializeThemeValue(ThemeValues::buttonFontSize, 14.f);
			initializeThemeValue(ThemeValues::buttonCharacterSpacing, 1.f);

			setString(p_text);

			setCornerRadius(4.f);

			m_ripple = new Ripple(this);
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
				float sizeFactor = getThemeValue(ThemeValues::buttonFontSize) / 14.f;
				if (m_icon)
				{
					m_icon.setSize(16.f * sizeFactor, 16.f * sizeFactor);
					m_icon.setCenter(sizeFactor * 38.f * 0.5f, getHeight() * 0.5f);

					m_text.setLeft(38.f * sizeFactor);
					setSize(std::round(m_text.getWidth()) + sizeFactor * (16.f + 38.f), 36.f * sizeFactor);
				}
				else
				{
					if (m_text.getWidth() >= 32.f * sizeFactor)
					{
						setSize(std::round(m_text.getWidth()) + 32.f * sizeFactor, 36.f * sizeFactor);
					}
					else
					{
						setSize(64.f * sizeFactor, 36.f * sizeFactor);
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
		void setString(std::string const& p_string)
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
		std::string getString()
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
		void setTooltip(Tooltip* p_tooltipView, std::string const& p_info)
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
				if (m_isEnabled && getIsContaining(p_event.x + getLeft(), p_event.y + getTop()))
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
					setElevation(2.f + pressAnimationValue * 4.f);
					if (!m_isPressed && pressAnimationValue == 1.f)
					{
						m_pressAnimationTime = 0.f;
						m_isRaising = false;
						queueAnimationUpdate();
					}
				}
				else
				{
					setElevation(2.f + (1.f - pressAnimationValue) * 4.f);
				}

				if (pressAnimationValue < 1.f)
				{
					queueAnimationUpdate();
				}
			}

			invalidate();
		}


		//------------------------------

		void drawOverlay(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
		{
			if (m_emphasis == Emphasis::Medium)
			{
				p_drawingContext->setColor(Color(getThemeColor(ThemeColors::onBackground), 0.25f));
				p_drawingContext->strokeRoundedRectangle(Rectangle<float>(0.5f, 0.5f, getWidth() - 0.5f, getHeight() - 0.5f), getCorners().topLeftSizeX, 1.f);
			}
		}

		void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
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

	/*
		A view that only consists of text that can be edited by the user.
	*/
	class EditableText : public View
	{
	private:
		Text m_text;
		float m_textDrawingOffsetX{ 0.f };
		float m_fontSize;
		TextAlign m_textAlign{ TextAlign::Left };

		uint32 m_caretCharacterIndex{ 0 };
		uint32 m_caretByteIndex{ 0 };
		Point<float> m_caretPosition;
		bool m_isCaretVisible{ false };
		uint32 m_caretFrameCount{ 0 };

		uint32 m_selectionEndCharacterIndex{ 0 };
		uint32 m_selectionEndByteIndex{ 0 };
		Point<float> m_selectionEndPosition;
		bool m_isSelectingWithMouse{ false };
		bool m_isSelectionVisible{ false };

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
			View(p_parent, Rectangle<float>(0.f, 0.f, p_width, p_fontSize*1.2f)),
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
		EventListeners<bool(EditableText*, std::string&, int32&)> editableTextChangeListeners;

		/*
			Listeners that get called when the user has pressed the enter/return key while p_editableText has keyboard focus.
		*/
		EventListeners<void(EditableText*)> editableTextEnterListeners;

		//------------------------------

		void handleMouseDoubleClick(MouseEvent const& p_event) override
		{
			if (m_text)
			{
				std::string const& string = m_text.getString();
				uint32 clickCharacterIndex = m_text.getNearestCharacterIndex(p_event.x - m_textDrawingOffsetX, p_event.y, true);
				int32 clickByteIndex = getUtf8UnitIndexFromCharacterIndex(string, clickCharacterIndex);

				m_caretCharacterIndex = 0;
				m_caretByteIndex = 0;

				uint32 characterIndex = 0;
				for (uint32 byteIndex = 0; byteIndex <= string.size(); byteIndex++)
				{
					if (byteIndex == string.size() || getIsUnitStartOfUtf8Character(string[byteIndex]))
					{
						if (byteIndex == string.size() || string[byteIndex] == ' ')
						{
							if (characterIndex >= clickCharacterIndex)
							{
								m_selectionEndCharacterIndex = characterIndex;
								m_selectionEndByteIndex = byteIndex;
								m_selectionEndPosition = m_text.getCharacterPosition(m_selectionEndCharacterIndex);
								m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
								updateCaretTracking();
								break;
							}
							else
							{
								m_caretCharacterIndex = characterIndex + 1;
								m_caretByteIndex = byteIndex + 1;
							}
						}
						characterIndex++;
					}
				}
				if (m_caretCharacterIndex != m_selectionEndCharacterIndex)
				{
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}
		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (m_text)
			{
				if (p_event.modifierKeys & ModifierKeyFlags::Shift)
				{
					m_text.getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, p_event.y, &m_selectionEndCharacterIndex, &m_selectionEndPosition, true);
					m_selectionEndByteIndex = getUtf8UnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);

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
					m_text.getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, p_event.y, &m_caretCharacterIndex, &m_caretPosition, true);
					m_caretByteIndex = getUtf8UnitIndexFromCharacterIndex(m_text.getString(), m_caretCharacterIndex);
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
		void handleMouseMove(MouseEvent const& p_event) override
		{
			if (m_isSelectingWithMouse)
			{
				m_text.getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, 0, &m_selectionEndCharacterIndex, &m_selectionEndPosition, true);
				m_selectionEndByteIndex = getUtf8UnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);
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
				std::string string(m_text ? m_text.getString() : "");
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
			Window* window = getGui()->getWindow();

			std::string string = m_text ? m_text.getString() : "";

			if (m_isSelectionVisible && (p_event.key == KeyboardKey::Backspace || p_event.key == KeyboardKey::Delete) && m_caretCharacterIndex != m_selectionEndCharacterIndex)
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
							int32 characterIndex = m_caretCharacterIndex - 1;
							for (int32 byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfUtf8Character(string[byteIndex]))
								{
									if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
									{
										string.erase(byteIndex, (int32)m_caretByteIndex - byteIndex);
										setString(string, characterIndex);
										break;
									}
									characterIndex--;
								}
							}
						}
						else
						{
							for (int32 byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								int8 numberOfBytesInCharacter = getNumberOfUnitsInUtf8Character(string[byteIndex]);
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
							for (int32 byteIndex = m_caretByteIndex; byteIndex < string.size(); byteIndex++)
							{
								if (byteIndex == string.size() - 1 || (string[byteIndex + 1U] == ' ' && string[byteIndex] != ' '))
								{
									string.erase(m_caretByteIndex, byteIndex - (int32)m_caretByteIndex + 1);
									setString(string);
									break;
								}
							}
						}
						else
						{
							setString(string.erase(m_caretByteIndex, getNumberOfUnitsInUtf8Character(string[m_caretByteIndex])));
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
							int32 characterIndex = m_selectionEndCharacterIndex - 1;
							for (int32 byteIndex = m_selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfUtf8Character(string[byteIndex]))
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
							int32 characterIndex = m_caretCharacterIndex - 1;
							for (int32 byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfUtf8Character(string[byteIndex]))
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
							for (int32 byteIndex = m_selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
							{
								if (getIsUnitStartOfUtf8Character(string[byteIndex]))
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
								for (int32 byteIndex = m_caretByteIndex - 1; byteIndex >= 0; byteIndex--)
								{
									if (getIsUnitStartOfUtf8Character(string[byteIndex]))
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
							uint32 characterIndex = m_selectionEndCharacterIndex;
							for (uint32 byteIndex = m_selectionEndByteIndex + 1; byteIndex <= string.size(); byteIndex++)
							{
								if (byteIndex == string.size() || getIsUnitStartOfUtf8Character(string[byteIndex]))
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
							uint32 characterIndex = m_caretCharacterIndex;
							for (uint32 byteIndex = m_caretByteIndex + 1; byteIndex <= string.size(); byteIndex++)
							{
								if (byteIndex == string.size() || getIsUnitStartOfUtf8Character(string[byteIndex]))
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
							m_selectionEndByteIndex += getNumberOfUnitsInUtf8Character(string[m_selectionEndByteIndex]);
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
								m_caretByteIndex += getNumberOfUnitsInUtf8Character(string[m_caretByteIndex]);
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
						uint32 caretCharacterIndex = m_caretCharacterIndex;
						uint32 caretByteIndex = m_caretByteIndex;
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
						std::string clipboardString = clipboardData->getString();
						string.insert(caretByteIndex, clipboardString);
						setString(string, caretCharacterIndex + getCharacterIndexFromUtf8UnitIndex(clipboardString, clipboardString.size()));

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
		void setSelection(uint32 p_startIndex, uint32 p_endIndex)
		{
			if (m_text)
			{
				uint32 numberOfCharactersInString = getCharacterIndexFromUtf8UnitIndex(m_text.getString(), m_text.getString().size());
				p_startIndex = min(numberOfCharactersInString, p_startIndex);
				p_endIndex = min(numberOfCharactersInString, max(p_startIndex, p_endIndex));
				if (p_startIndex != p_endIndex)
				{
					if (p_startIndex != m_caretCharacterIndex)
					{
						m_caretCharacterIndex = p_startIndex;
						m_caretByteIndex = getUtf8UnitIndexFromCharacterIndex(m_text.getString(), m_caretCharacterIndex);
						m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
					}

					if (p_endIndex != m_selectionEndCharacterIndex)
					{
						m_selectionEndCharacterIndex = p_endIndex;
						m_selectionEndByteIndex = getUtf8UnitIndexFromCharacterIndex(m_text.getString(), m_selectionEndCharacterIndex);
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
		void selectAll()
		{
			if (m_text)
			{
				uint32 stringLength = m_text.getString().size();
				if (stringLength)
				{
					if (m_caretCharacterIndex != 0)
					{
						m_caretCharacterIndex = 0;
						m_caretByteIndex = 0;
						m_caretPosition = m_text.getCharacterPosition(m_caretCharacterIndex, true);
					}

					if (m_selectionEndCharacterIndex != stringLength)
					{
						m_selectionEndCharacterIndex = getCharacterIndexFromUtf8UnitIndex(m_text.getString(), stringLength);
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
		void setString(std::string const& p_string, int32 p_newCaretCharacterIndex = -1)
		{
			if (m_text && m_text.getString() == p_string)
			{
				return;
			}
			if (p_newCaretCharacterIndex == -1)
			{
				p_newCaretCharacterIndex = m_caretCharacterIndex;
			}

			auto newString = p_string;

			for (auto& listener : editableTextChangeListeners.listeners)
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
					m_caretPosition.x = getWidth() * 0.5f;
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

			auto characterCount = getNumberOfCharactersInUtf8String(newString);
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
					m_caretByteIndex = getUtf8UnitIndexFromCharacterIndex(newString, p_newCaretCharacterIndex);
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
			Sets the content of the editable text as a value.
		*/
		template<typename T>
		void setValue(T p_value)
		{
			setString(convertNumberToString(p_value));
		}
		/*
			Sets the content of the editable text as a value, rounded at a certain digit.
		*/
		template<typename T>
		void setValue(T p_value, int32 p_roundingDigit, RoundingType p_roundingType = RoundingType::Nearest)
		{
			setString(convertNumberToString(p_value, p_roundingDigit, p_roundingType));
		}
		/*
			Returns the content of the editable text.
		*/
		std::string getString()
		{
			if (m_text)
			{
				return m_text.getString();
			}
			return "";
		}

		/*
			Returns the internal text graphics object.
		*/
		Text getText()
		{
			return m_text;
		}

		//------------------------------

		/*
			Sets the way text is horizontally aligned within the bounds of the editable text.
		*/
		void setTextAlign(TextAlign p_textAlign)
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
		TextAlign getTextAlign()
		{
			return m_textAlign;
		}

		//------------------------------

		void setFontSize(float p_fontSize)
		{
			m_fontSize = p_fontSize;
			if (m_text)
			{
				m_text.setFontSize(p_fontSize);
			}
			setHeight(p_fontSize * 1.2f);
			invalidate();
		}
		float getFontSize()
		{
			return m_fontSize;
		}

		//------------------------------

		void handleSizeChange() override
		{
			updateCaretTracking();
			if (m_text)
			{
				m_text.setWidth(getWidth());
			}
		}

		//------------------------------

		void updateAnimations() override
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

		void draw(DrawingContext* p_context) override
		{
			//p_context->setColor(Color(0.f));
			//p_context->strokeRectangle(getSize(), 1.f);
			p_context->moveOrigin(m_textDrawingOffsetX, 0.f);
			p_context->setColor(getThemeColor(ThemeColors::onBackground));
			if (m_text)
			{
				p_context->drawText(m_text);
				if (m_isSelectionVisible)
				{
					p_context->setColor(getThemeColor(ThemeColors::selection));
					p_context->fillRectangle(m_caretPosition.x, 0.f, m_selectionEndPosition.x, getHeight());
				}
			}
			if (m_isCaretVisible && !m_isSelectionVisible)
			{
				p_context->drawLine(m_caretPosition.x, 0.f, m_caretPosition.x, getHeight(), 1.f);
			}
			p_context->moveOrigin(-m_textDrawingOffsetX, 0.f);
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

	class TextField : public View
	{
	public:
		enum Type
		{
			Outlined,
			Filled
		};

		static constexpr float OUTLINED_PADDING_LABEL = 5.f;

		Type m_type;

	public:
		TextField(View* p_parent, Type p_type = Type::Filled, std::string const& p_label = "", float p_width = 120.f) :
			View(p_parent),
			m_type(p_type)
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

			auto handleEditableTextFocusChange = [this]() {
				queueAnimationUpdate();
			};
			m_editableText->keyboardFocusGainListeners += handleEditableTextFocusChange;
			m_editableText->keyboardFocusLoseListeners += handleEditableTextFocusChange;

			setSize(p_width, getThemeValue(ThemeValues::textFieldFontSize) * 1.2f * getThemeValue(ThemeValues::textFieldHeight) + OUTLINED_PADDING_LABEL * (m_type == Type::Outlined));

			if (p_type == Type::Filled)
			{
				setCorners(Avo::RectangleCorners(5.f, 5.f, 0.f, 0.f));
			}
			else
			{
				setCornerRadius(5.f);
			}

			setString("");

			queueAnimationUpdate();
		}

		//------------------------------

	private:
		EditableText* m_editableText{ new EditableText(this) };

	public:
		EditableText* getEditableText()
		{
			return m_editableText;
		}

	protected:
		void handleThemeValueChange(Id const& p_id, float p_newValue) override
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
				setHeight(getThemeValue(ThemeValues::textFieldFontSize) * 1.2f * getThemeValue(ThemeValues::textFieldHeight) + OUTLINED_PADDING_LABEL * (m_type == Type::Outlined));
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
		void handleSizeChange() override
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
				float bottom = getHeight() - getThemeValue(ThemeValues::textFieldFilledPaddingBottom);
				if (m_labelText)
				{
					m_labelText.setCenterY(getHeight() * 0.5f);
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
				float centerY = OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL) * 0.5f;
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
		void setLabel(std::string const& p_label)
		{
			if (m_labelText)
			{
				if (p_label == m_labelText.getString())
				{
					return;
				}
			}
			if (p_label[0] == 0)
			{
				m_labelText.destroy();
			}
			else
			{
				m_labelText = getGui()->getDrawingContext()->createText(p_label, getThemeValue(ThemeValues::textFieldFontSize));
				m_labelText.setFontWeight(Avo::FontWeight::Regular);
				m_labelText.fitSizeToText();
				if (m_type == Type::Filled)
				{
					m_labelText.setCenterY(getHeight() * 0.5f);
				}
				else if (m_type == Type::Outlined)
				{
					m_labelText.setCenterY(OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL) * 0.5f);
				}
				queueAnimationUpdate();
			}
		}
		std::string getLabel()
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

		bool setAffixString(std::string const& p_string, Text& p_affixText)
		{
			if (p_affixText)
			{
				if (p_affixText.getString() == p_string)
				{
					return false;
				}
			}
			if (p_string[0] == 0)
			{
				p_affixText.destroy();
				return false;
			}
			p_affixText = getDrawingContext()->createText(p_string, getThemeValue(ThemeValues::textFieldFontSize));
			p_affixText.setFontWeight(Avo::FontWeight::Regular);
			p_affixText.setHeight(p_affixText.getFontSize() * 1.2f);
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
		void setPrefixString(std::string const& p_string)
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
		std::string getPrefixString()
		{
			if (m_suffixText)
			{
				return m_suffixText.getString();
			}
			return "";
		}

		//------------------------------

	public:
		void setSuffixString(std::string const& p_string)
		{
			if (setAffixString(p_string, m_suffixText))
			{
				m_suffixText.setRight(getWidth() - getThemeValue(ThemeValues::textFieldPaddingRight));
				m_editableText->setRight(m_suffixText.getLeft() - 1.f, false);
			}
		}
		std::string getSuffixString()
		{
			if (m_suffixText)
			{
				return m_suffixText.getString();
			}
			return "";
		}

		//------------------------------

		void setString(std::string const& p_string)
		{
			m_editableText->setString(p_string);
			if (m_type == Type::Filled)
			{
				m_editableText->setBottom(getHeight() - getThemeValue(ThemeValues::textFieldFilledPaddingBottom));
			}
			else if (m_type == Type::Outlined)
			{
				m_editableText->setCenterY(OUTLINED_PADDING_LABEL + (getHeight() - OUTLINED_PADDING_LABEL) * 0.5f);
			}
		}
		/*
			Sets the content of the text field as a value.
		*/
		template<typename T>
		void setValue(T p_value)
		{
			setString(convertNumberToString(p_value));
		}
		/*
			Sets the content of the text field as a value, rounded at a certain digit.
		*/
		template<typename T>
		void setValue(T p_value, int32 p_roundingDigit, RoundingType p_roundingType = RoundingType::Nearest)
		{
			setString(convertNumberToString(p_value, p_roundingDigit, p_roundingType));
		}
		std::string getString()
		{
			return m_editableText->getString();
		}

		//------------------------------

		void setTextAlign(TextAlign p_textAlign)
		{
			m_editableText->setTextAlign(p_textAlign);
		}
		TextAlign getTextAlign()
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
		float m_focusAnimationTime{ 0.f };
		float m_focusAnimationValue{ 0.f };

		bool m_isMouseHovering{ false };
		float m_hoverAnimationTime{ 0.f };
		float m_hoverAnimationValue{ 0.f };

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
			m_labelColor = interpolate(interpolate(getThemeColor(ThemeColors::background), getThemeColor(ThemeColors::onBackground), (1.f - m_focusAnimationValue) * m_hoverAnimationValue * 0.3f + 0.4f), getThemeColor(ThemeColors::primaryOnBackground), m_focusAnimationValue);
		}

		void draw(DrawingContext* p_context) override
		{
			if (m_type == Type::Filled)
			{
				p_context->setColor(Color(interpolate(getThemeColor(ThemeColors::background), getThemeColor(ThemeColors::onBackground), 0.05f + 0.05f * min(m_hoverAnimationValue*0.3f + m_focusAnimationValue, 1.f)), 1.f));
				p_context->fillRectangle(getSize());
				p_context->setColor(Color(getThemeColor(ThemeColors::onBackground), 0.4));
				p_context->drawLine(0.f, getHeight() - 1.f, getWidth(), getHeight() - 0.5f, 1.f);
				if (m_focusAnimationValue > 0.01f)
				{
					p_context->setColor(getThemeColor(ThemeColors::primaryOnBackground));
					p_context->drawLine((1.f - m_focusAnimationValue) * getWidth() * 0.5f, getHeight() - 1.f, (1.f + m_focusAnimationValue) * getWidth() * 0.5f, getHeight() - 1.f, 2.f);
				}
				if (m_labelText)
				{
					float labelAnimationValue = m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f;
					float leftPadding = getThemeValue(ThemeValues::textFieldPaddingLeft);
					p_context->moveOrigin(leftPadding + 2.f*labelAnimationValue, -0.17f*(getHeight() - m_labelText.getHeight() - leftPadding) * labelAnimationValue);
					p_context->setScale(1.f - labelAnimationValue * 0.3f);
					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);
					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}
			else if (m_type == Type::Outlined)
			{
				p_context->setColor(m_labelColor);
				p_context->strokeRectangle(Rectangle<float>(1.f, 1.f + OUTLINED_PADDING_LABEL, getWidth() - 1.f, getHeight() - 1.f), getCorners(), m_focusAnimationValue + 1.f);

				if (m_labelText)
				{
					float labelAnimationValue = m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f;
					p_context->moveOrigin(getThemeValue(ThemeValues::textFieldPaddingLeft) + 2.f * labelAnimationValue, -(getHeight() - OUTLINED_PADDING_LABEL) * 0.3f * labelAnimationValue);
					p_context->setScale(1.f - labelAnimationValue * 0.3f);

					p_context->setColor(getThemeColor(ThemeColors::background));
					p_context->fillRoundedRectangle(Rectangle<float>(m_labelText.getLeft() - 4.f, m_labelText.getTop(), m_labelText.getRight() + 4.f, m_labelText.getBottom()), 2.f);

					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);

					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}

			if (m_prefixText)
			{
				p_context->setColor(Color(getThemeColor(ThemeColors::onBackground), 0.5f));
				p_context->drawText(m_prefixText);
			}
			if (m_suffixText)
			{
				p_context->setColor(Color(getThemeColor(ThemeColors::onBackground), 0.5f));
				p_context->drawText(m_suffixText);
			}
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
	constexpr auto THREED_ROTATION{ u8"\ue84d" };
	constexpr auto AC_UNIT{ u8"\ueb3b" };
	constexpr auto ACCESS_ALARM{ u8"\ue190" };
	constexpr auto ACCESS_ALARMS{ u8"\ue191" };
	constexpr auto ACCESS_TIME{ u8"\ue192" };
	constexpr auto ACCESSIBILITY{ u8"\ue84e" };
	constexpr auto ACCESSIBLE{ u8"\ue914" };
	constexpr auto ACCOUNT_BALANCE{ u8"\ue84f" };
	constexpr auto ACCOUNT_BALANCE_WALLET{ u8"\ue850" };
	constexpr auto ACCOUNT_BOX{ u8"\ue851" };
	constexpr auto ACCOUNT_CIRCLE{ u8"\ue853" };
	constexpr auto ADB{ u8"\ue60e" };
	constexpr auto ADD{ u8"\ue145" };
	constexpr auto ADD_A_PHOTO{ u8"\ue439" };
	constexpr auto ADD_ALARM{ u8"\ue193" };
	constexpr auto ADD_ALERT{ u8"\ue003" };
	constexpr auto ADD_BOX{ u8"\ue146" };
	constexpr auto ADD_CIRCLE{ u8"\ue147" };
	constexpr auto ADD_CIRCLE_OUTLINE{ u8"\ue148" };
	constexpr auto ADD_LOCATION{ u8"\ue567" };
	constexpr auto ADD_SHOPPING_CART{ u8"\ue854" };
	constexpr auto ADD_TO_PHOTOS{ u8"\ue39d" };
	constexpr auto ADD_TO_QUEUE{ u8"\ue05c" };
	constexpr auto ADJUST{ u8"\ue39e" };
	constexpr auto AIRLINE_SEAT_FLAT{ u8"\ue630" };
	constexpr auto AIRLINE_SEAT_FLAT_ANGLED{ u8"\ue631" };
	constexpr auto AIRLINE_SEAT_INDIVIDUAL_SUITE{ u8"\ue632" };
	constexpr auto AIRLINE_SEAT_LEGROOM_EXTRA{ u8"\ue633" };
	constexpr auto AIRLINE_SEAT_LEGROOM_NORMAL{ u8"\ue634" };
	constexpr auto AIRLINE_SEAT_LEGROOM_REDUCED{ u8"\ue635" };
	constexpr auto AIRLINE_SEAT_RECLINE_EXTRA{ u8"\ue636" };
	constexpr auto AIRLINE_SEAT_RECLINE_NORMAL{ u8"\ue637" };
	constexpr auto AIRPLANEMODE_ACTIVE{ u8"\ue195" };
	constexpr auto AIRPLANEMODE_INACTIVE{ u8"\ue194" };
	constexpr auto AIRPLAY{ u8"\ue055" };
	constexpr auto AIRPORT_SHUTTLE{ u8"\ueb3c" };
	constexpr auto ALARM{ u8"\ue855" };
	constexpr auto ALARM_ADD{ u8"\ue856" };
	constexpr auto ALARM_OFF{ u8"\ue857" };
	constexpr auto ALARM_ON{ u8"\ue858" };
	constexpr auto ALBUM{ u8"\ue019" };
	constexpr auto ALL_INCLUSIVE{ u8"\ueb3d" };
	constexpr auto ALL_OUT{ u8"\ue90b" };
	constexpr auto ANDROID{ u8"\ue859" };
	constexpr auto ANNOUNCEMENT{ u8"\ue85a" };
	constexpr auto APPS{ u8"\ue5c3" };
	constexpr auto ARCHIVE{ u8"\ue149" };
	constexpr auto ARROW_BACK{ u8"\ue5c4" };
	constexpr auto ARROW_DOWNWARD{ u8"\ue5db" };
	constexpr auto ARROW_DROP_DOWN{ u8"\ue5c5" };
	constexpr auto ARROW_DROP_DOWN_CIRCLE{ u8"\ue5c6" };
	constexpr auto ARROW_DROP_UP{ u8"\ue5c7" };
	constexpr auto ARROW_FORWARD{ u8"\ue5c8" };
	constexpr auto ARROW_UPWARD{ u8"\ue5d8" };
	constexpr auto ART_TRACK{ u8"\ue060" };
	constexpr auto ASPECT_RATIO{ u8"\ue85b" };
	constexpr auto ASSESSMENT{ u8"\ue85c" };
	constexpr auto ASSIGNMENT{ u8"\ue85d" };
	constexpr auto ASSIGNMENT_IND{ u8"\ue85e" };
	constexpr auto ASSIGNMENT_LATE{ u8"\ue85f" };
	constexpr auto ASSIGNMENT_RETURN{ u8"\ue860" };
	constexpr auto ASSIGNMENT_RETURNED{ u8"\ue861" };
	constexpr auto ASSIGNMENT_TURNED_IN{ u8"\ue862" };
	constexpr auto ASSISTANT{ u8"\ue39f" };
	constexpr auto ASSISTANT_PHOTO{ u8"\ue3a0" };
	constexpr auto ATTACH_FILE{ u8"\ue226" };
	constexpr auto ATTACH_MONEY{ u8"\ue227" };
	constexpr auto ATTACHMENT{ u8"\ue2bc" };
	constexpr auto AUDIOTRACK{ u8"\ue3a1" };
	constexpr auto AUTORENEW{ u8"\ue863" };
	constexpr auto AV_TIMER{ u8"\ue01b" };
	constexpr auto BACKSPACE{ u8"\ue14a" };
	constexpr auto BACKUP{ u8"\ue864" };
	constexpr auto BATTERY_ALERT{ u8"\ue19c" };
	constexpr auto BATTERY_CHARGING_FULL{ u8"\ue1a3" };
	constexpr auto BATTERY_FULL{ u8"\ue1a4" };
	constexpr auto BATTERY_STD{ u8"\ue1a5" };
	constexpr auto BATTERY_UNKNOWN{ u8"\ue1a6" };
	constexpr auto BEACH_ACCESS{ u8"\ueb3e" };
	constexpr auto BEENHERE{ u8"\ue52d" };
	constexpr auto BLOCK{ u8"\ue14b" };
	constexpr auto BLUETOOTH{ u8"\ue1a7" };
	constexpr auto BLUETOOTH_AUDIO{ u8"\ue60f" };
	constexpr auto BLUETOOTH_CONNECTED{ u8"\ue1a8" };
	constexpr auto BLUETOOTH_DISABLED{ u8"\ue1a9" };
	constexpr auto BLUETOOTH_SEARCHING{ u8"\ue1aa" };
	constexpr auto BLUR_CIRCULAR{ u8"\ue3a2" };
	constexpr auto BLUR_LINEAR{ u8"\ue3a3" };
	constexpr auto BLUR_OFF{ u8"\ue3a4" };
	constexpr auto BLUR_ON{ u8"\ue3a5" };
	constexpr auto BOOK{ u8"\ue865" };
	constexpr auto BOOKMARK{ u8"\ue866" };
	constexpr auto BOOKMARK_BORDER{ u8"\ue867" };
	constexpr auto BORDER_ALL{ u8"\ue228" };
	constexpr auto BORDER_BOTTOM{ u8"\ue229" };
	constexpr auto BORDER_CLEAR{ u8"\ue22a" };
	constexpr auto BORDER_COLOR{ u8"\ue22b" };
	constexpr auto BORDER_HORIZONTAL{ u8"\ue22c" };
	constexpr auto BORDER_INNER{ u8"\ue22d" };
	constexpr auto BORDER_LEFT{ u8"\ue22e" };
	constexpr auto BORDER_OUTER{ u8"\ue22f" };
	constexpr auto BORDER_RIGHT{ u8"\ue230" };
	constexpr auto BORDER_STYLE{ u8"\ue231" };
	constexpr auto BORDER_TOP{ u8"\ue232" };
	constexpr auto BORDER_VERTICAL{ u8"\ue233" };
	constexpr auto BRANDING_WATERMARK{ u8"\ue06b" };
	constexpr auto BRIGHTNESS_1{ u8"\ue3a6" };
	constexpr auto BRIGHTNESS_2{ u8"\ue3a7" };
	constexpr auto BRIGHTNESS_3{ u8"\ue3a8" };
	constexpr auto BRIGHTNESS_4{ u8"\ue3a9" };
	constexpr auto BRIGHTNESS_5{ u8"\ue3aa" };
	constexpr auto BRIGHTNESS_6{ u8"\ue3ab" };
	constexpr auto BRIGHTNESS_7{ u8"\ue3ac" };
	constexpr auto BRIGHTNESS_AUTO{ u8"\ue1ab" };
	constexpr auto BRIGHTNESS_HIGH{ u8"\ue1ac" };
	constexpr auto BRIGHTNESS_LOW{ u8"\ue1ad" };
	constexpr auto BRIGHTNESS_MEDIUM{ u8"\ue1ae" };
	constexpr auto BROKEN_IMAGE{ u8"\ue3ad" };
	constexpr auto BRUSH{ u8"\ue3ae" };
	constexpr auto BUBBLE_CHART{ u8"\ue6dd" };
	constexpr auto BUG_REPORT{ u8"\ue868" };
	constexpr auto BUILD{ u8"\ue869" };
	constexpr auto BURST_MODE{ u8"\ue43c" };
	constexpr auto BUSINESS{ u8"\ue0af" };
	constexpr auto BUSINESS_CENTER{ u8"\ueb3f" };
	constexpr auto CACHED{ u8"\ue86a" };
	constexpr auto CAKE{ u8"\ue7e9" };
	constexpr auto CALL{ u8"\ue0b0" };
	constexpr auto CALL_END{ u8"\ue0b1" };
	constexpr auto CALL_MADE{ u8"\ue0b2" };
	constexpr auto CALL_MERGE{ u8"\ue0b3" };
	constexpr auto CALL_MISSED{ u8"\ue0b4" };
	constexpr auto CALL_MISSED_OUTGOING{ u8"\ue0e4" };
	constexpr auto CALL_RECEIVED{ u8"\ue0b5" };
	constexpr auto CALL_SPLIT{ u8"\ue0b6" };
	constexpr auto CALL_TO_ACTION{ u8"\ue06c" };
	constexpr auto CAMERA{ u8"\ue3af" };
	constexpr auto CAMERA_ALT{ u8"\ue3b0" };
	constexpr auto CAMERA_ENHANCE{ u8"\ue8fc" };
	constexpr auto CAMERA_FRONT{ u8"\ue3b1" };
	constexpr auto CAMERA_REAR{ u8"\ue3b2" };
	constexpr auto CAMERA_ROLL{ u8"\ue3b3" };
	constexpr auto CANCEL{ u8"\ue5c9" };
	constexpr auto CARD_GIFTCARD{ u8"\ue8f6" };
	constexpr auto CARD_MEMBERSHIP{ u8"\ue8f7" };
	constexpr auto CARD_TRAVEL{ u8"\ue8f8" };
	constexpr auto CASINO{ u8"\ueb40" };
	constexpr auto CAST{ u8"\ue307" };
	constexpr auto CAST_CONNECTED{ u8"\ue308" };
	constexpr auto CENTER_FOCUS_STRONG{ u8"\ue3b4" };
	constexpr auto CENTER_FOCUS_WEAK{ u8"\ue3b5" };
	constexpr auto CHANGE_HISTORY{ u8"\ue86b" };
	constexpr auto CHAT{ u8"\ue0b7" };
	constexpr auto CHAT_BUBBLE{ u8"\ue0ca" };
	constexpr auto CHAT_BUBBLE_OUTLINE{ u8"\ue0cb" };
	constexpr auto CHECK{ u8"\ue5ca" };
	constexpr auto CHECK_BOX{ u8"\ue834" };
	constexpr auto CHECK_BOX_OUTLINE_BLANK{ u8"\ue835" };
	constexpr auto CHECK_CIRCLE{ u8"\ue86c" };
	constexpr auto CHEVRON_LEFT{ u8"\ue5cb" };
	constexpr auto CHEVRON_RIGHT{ u8"\ue5cc" };
	constexpr auto CHILD_CARE{ u8"\ueb41" };
	constexpr auto CHILD_FRIENDLY{ u8"\ueb42" };
	constexpr auto CHROME_READER_MODE{ u8"\ue86d" };
	constexpr auto CLASS{ u8"\ue86e" };
	constexpr auto CLEAR{ u8"\ue14c" };
	constexpr auto CLEAR_ALL{ u8"\ue0b8" };
	constexpr auto CLOSE{ u8"\ue5cd" };
	constexpr auto CLOSED_CAPTION{ u8"\ue01c" };
	constexpr auto CLOUD{ u8"\ue2bd" };
	constexpr auto CLOUD_CIRCLE{ u8"\ue2be" };
	constexpr auto CLOUD_DONE{ u8"\ue2bf" };
	constexpr auto CLOUD_DOWNLOAD{ u8"\ue2c0" };
	constexpr auto CLOUD_OFF{ u8"\ue2c1" };
	constexpr auto CLOUD_QUEUE{ u8"\ue2c2" };
	constexpr auto CLOUD_UPLOAD{ u8"\ue2c3" };
	constexpr auto CODE{ u8"\ue86f" };
	constexpr auto COLLECTIONS{ u8"\ue3b6" };
	constexpr auto COLLECTIONS_BOOKMARK{ u8"\ue431" };
	constexpr auto COLOR_LENS{ u8"\ue3b7" };
	constexpr auto COLORIZE{ u8"\ue3b8" };
	constexpr auto COMMENT{ u8"\ue0b9" };
	constexpr auto COMPARE{ u8"\ue3b9" };
	constexpr auto COMPARE_ARROWS{ u8"\ue915" };
	constexpr auto COMPUTER{ u8"\ue30a" };
	constexpr auto CONFIRMATION_NUMBER{ u8"\ue638" };
	constexpr auto CONTACT_MAIL{ u8"\ue0d0" };
	constexpr auto CONTACT_PHONE{ u8"\ue0cf" };
	constexpr auto CONTACTS{ u8"\ue0ba" };
	constexpr auto CONTENT_COPY{ u8"\ue14d" };
	constexpr auto CONTENT_CUT{ u8"\ue14e" };
	constexpr auto CONTENT_PASTE{ u8"\ue14f" };
	constexpr auto CONTROL_POINT{ u8"\ue3ba" };
	constexpr auto CONTROL_POINT_DUPLICATE{ u8"\ue3bb" };
	constexpr auto COPYRIGHT{ u8"\ue90c" };
	constexpr auto CREATE{ u8"\ue150" };
	constexpr auto CREATE_NEW_FOLDER{ u8"\ue2cc" };
	constexpr auto CREDIT_CARD{ u8"\ue870" };
	constexpr auto CROP{ u8"\ue3be" };
	constexpr auto CROP_16_9{ u8"\ue3bc" };
	constexpr auto CROP_3_2{ u8"\ue3bd" };
	constexpr auto CROP_5_4{ u8"\ue3bf" };
	constexpr auto CROP_7_5{ u8"\ue3c0" };
	constexpr auto CROP_DIN{ u8"\ue3c1" };
	constexpr auto CROP_FREE{ u8"\ue3c2" };
	constexpr auto CROP_LANDSCAPE{ u8"\ue3c3" };
	constexpr auto CROP_ORIGINAL{ u8"\ue3c4" };
	constexpr auto CROP_PORTRAIT{ u8"\ue3c5" };
	constexpr auto CROP_ROTATE{ u8"\ue437" };
	constexpr auto CROP_SQUARE{ u8"\ue3c6" };
	constexpr auto DASHBOARD{ u8"\ue871" };
	constexpr auto DATA_USAGE{ u8"\ue1af" };
	constexpr auto DATE_RANGE{ u8"\ue916" };
	constexpr auto DEHAZE{ u8"\ue3c7" };
#undef DELETE
	constexpr auto DELETE{ u8"\ue872" };
	constexpr auto DELETE_FOREVER{ u8"\ue92b" };
	constexpr auto DELETE_SWEEP{ u8"\ue16c" };
	constexpr auto DESCRIPTION{ u8"\ue873" };
	constexpr auto DESKTOP_MAC{ u8"\ue30b" };
	constexpr auto DESKTOP_WINDOWS{ u8"\ue30c" };
	constexpr auto DETAILS{ u8"\ue3c8" };
	constexpr auto DEVELOPER_BOARD{ u8"\ue30d" };
	constexpr auto DEVELOPER_MODE{ u8"\ue1b0" };
	constexpr auto DEVICE_HUB{ u8"\ue335" };
	constexpr auto DEVICES{ u8"\ue1b1" };
	constexpr auto DEVICES_OTHER{ u8"\ue337" };
	constexpr auto DIALER_SIP{ u8"\ue0bb" };
	constexpr auto DIALPAD{ u8"\ue0bc" };
	constexpr auto DIRECTIONS{ u8"\ue52e" };
	constexpr auto DIRECTIONS_BIKE{ u8"\ue52f" };
	constexpr auto DIRECTIONS_BOAT{ u8"\ue532" };
	constexpr auto DIRECTIONS_BUS{ u8"\ue530" };
	constexpr auto DIRECTIONS_CAR{ u8"\ue531" };
	constexpr auto DIRECTIONS_RAILWAY{ u8"\ue534" };
	constexpr auto DIRECTIONS_RUN{ u8"\ue566" };
	constexpr auto DIRECTIONS_SUBWAY{ u8"\ue533" };
	constexpr auto DIRECTIONS_TRANSIT{ u8"\ue535" };
	constexpr auto DIRECTIONS_WALK{ u8"\ue536" };
	constexpr auto DISC_FULL{ u8"\ue610" };
	constexpr auto DNS{ u8"\ue875" };
	constexpr auto DO_NOT_DISTURB{ u8"\ue612" };
	constexpr auto DO_NOT_DISTURB_ALT{ u8"\ue611" };
	constexpr auto DO_NOT_DISTURB_OFF{ u8"\ue643" };
	constexpr auto DO_NOT_DISTURB_ON{ u8"\ue644" };
	constexpr auto DOCK{ u8"\ue30e" };
#undef DOMAIN
	constexpr auto DOMAIN{ u8"\ue7ee" };
	constexpr auto DONE{ u8"\ue876" };
	constexpr auto DONE_ALL{ u8"\ue877" };
	constexpr auto DONUT_LARGE{ u8"\ue917" };
	constexpr auto DONUT_SMALL{ u8"\ue918" };
	constexpr auto DRAFTS{ u8"\ue151" };
	constexpr auto DRAG_HANDLE{ u8"\ue25d" };
	constexpr auto DRIVE_ETA{ u8"\ue613" };
	constexpr auto DVR{ u8"\ue1b2" };
	constexpr auto EDIT{ u8"\ue3c9" };
	constexpr auto EDIT_LOCATION{ u8"\ue568" };
	constexpr auto EJECT{ u8"\ue8fb" };
	constexpr auto EMAIL{ u8"\ue0be" };
	constexpr auto ENHANCED_ENCRYPTION{ u8"\ue63f" };
	constexpr auto EQUALIZER{ u8"\ue01d" };
	constexpr auto ERROR{ u8"\ue000" };
	constexpr auto ERROR_OUTLINE{ u8"\ue001" };
	constexpr auto EURO_SYMBOL{ u8"\ue926" };
	constexpr auto EV_STATION{ u8"\ue56d" };
	constexpr auto EVENT{ u8"\ue878" };
	constexpr auto EVENT_AVAILABLE{ u8"\ue614" };
	constexpr auto EVENT_BUSY{ u8"\ue615" };
	constexpr auto EVENT_NOTE{ u8"\ue616" };
	constexpr auto EVENT_SEAT{ u8"\ue903" };
	constexpr auto EXIT_TO_APP{ u8"\ue879" };
	constexpr auto EXPAND_LESS{ u8"\ue5ce" };
	constexpr auto EXPAND_MORE{ u8"\ue5cf" };
	constexpr auto EXPLICIT{ u8"\ue01e" };
	constexpr auto EXPLORE{ u8"\ue87a" };
	constexpr auto EXPOSURE{ u8"\ue3ca" };
	constexpr auto EXPOSURE_NEG_1{ u8"\ue3cb" };
	constexpr auto EXPOSURE_NEG_2{ u8"\ue3cc" };
	constexpr auto EXPOSURE_PLUS_1{ u8"\ue3cd" };
	constexpr auto EXPOSURE_PLUS_2{ u8"\ue3ce" };
	constexpr auto EXPOSURE_ZERO{ u8"\ue3cf" };
	constexpr auto EXTENSION{ u8"\ue87b" };
	constexpr auto FACE{ u8"\ue87c" };
	constexpr auto FAST_FORWARD{ u8"\ue01f" };
	constexpr auto FAST_REWIND{ u8"\ue020" };
	constexpr auto FAVORITE{ u8"\ue87d" };
	constexpr auto FAVORITE_BORDER{ u8"\ue87e" };
	constexpr auto FEATURED_PLAY_LIST{ u8"\ue06d" };
	constexpr auto FEATURED_VIDEO{ u8"\ue06e" };
	constexpr auto FEEDBACK{ u8"\ue87f" };
	constexpr auto FIBER_DVR{ u8"\ue05d" };
	constexpr auto FIBER_MANUAL_RECORD{ u8"\ue061" };
	constexpr auto FIBER_NEW{ u8"\ue05e" };
	constexpr auto FIBER_PIN{ u8"\ue06a" };
	constexpr auto FIBER_SMART_RECORD{ u8"\ue062" };
	constexpr auto FILE_DOWNLOAD{ u8"\ue2c4" };
	constexpr auto FILE_UPLOAD{ u8"\ue2c6" };
	constexpr auto FILTER{ u8"\ue3d3" };
	constexpr auto FILTER_1{ u8"\ue3d0" };
	constexpr auto FILTER_2{ u8"\ue3d1" };
	constexpr auto FILTER_3{ u8"\ue3d2" };
	constexpr auto FILTER_4{ u8"\ue3d4" };
	constexpr auto FILTER_5{ u8"\ue3d5" };
	constexpr auto FILTER_6{ u8"\ue3d6" };
	constexpr auto FILTER_7{ u8"\ue3d7" };
	constexpr auto FILTER_8{ u8"\ue3d8" };
	constexpr auto FILTER_9{ u8"\ue3d9" };
	constexpr auto FILTER_9_PLUS{ u8"\ue3da" };
	constexpr auto FILTER_B_AND_W{ u8"\ue3db" };
	constexpr auto FILTER_CENTER_FOCUS{ u8"\ue3dc" };
	constexpr auto FILTER_DRAMA{ u8"\ue3dd" };
	constexpr auto FILTER_FRAMES{ u8"\ue3de" };
	constexpr auto FILTER_HDR{ u8"\ue3df" };
	constexpr auto FILTER_LIST{ u8"\ue152" };
	constexpr auto FILTER_NONE{ u8"\ue3e0" };
	constexpr auto FILTER_TILT_SHIFT{ u8"\ue3e2" };
	constexpr auto FILTER_VINTAGE{ u8"\ue3e3" };
	constexpr auto FIND_IN_PAGE{ u8"\ue880" };
	constexpr auto FIND_REPLACE{ u8"\ue881" };
	constexpr auto FINGERPRINT{ u8"\ue90d" };
	constexpr auto FIRST_PAGE{ u8"\ue5dc" };
	constexpr auto FITNESS_CENTER{ u8"\ueb43" };
	constexpr auto FLAG{ u8"\ue153" };
	constexpr auto FLARE{ u8"\ue3e4" };
	constexpr auto FLASH_AUTO{ u8"\ue3e5" };
	constexpr auto FLASH_OFF{ u8"\ue3e6" };
	constexpr auto FLASH_ON{ u8"\ue3e7" };
	constexpr auto FLIGHT{ u8"\ue539" };
	constexpr auto FLIGHT_LAND{ u8"\ue904" };
	constexpr auto FLIGHT_TAKEOFF{ u8"\ue905" };
	constexpr auto FLIP{ u8"\ue3e8" };
	constexpr auto FLIP_TO_BACK{ u8"\ue882" };
	constexpr auto FLIP_TO_FRONT{ u8"\ue883" };
	constexpr auto FOLDER{ u8"\ue2c7" };
	constexpr auto FOLDER_OPEN{ u8"\ue2c8" };
	constexpr auto FOLDER_SHARED{ u8"\ue2c9" };
	constexpr auto FOLDER_SPECIAL{ u8"\ue617" };
	constexpr auto FONT_DOWNLOAD{ u8"\ue167" };
	constexpr auto FORMAT_ALIGN_CENTER{ u8"\ue234" };
	constexpr auto FORMAT_ALIGN_JUSTIFY{ u8"\ue235" };
	constexpr auto FORMAT_ALIGN_LEFT{ u8"\ue236" };
	constexpr auto FORMAT_ALIGN_RIGHT{ u8"\ue237" };
	constexpr auto FORMAT_BOLD{ u8"\ue238" };
	constexpr auto FORMAT_CLEAR{ u8"\ue239" };
	constexpr auto FORMAT_COLOR_FILL{ u8"\ue23a" };
	constexpr auto FORMAT_COLOR_RESET{ u8"\ue23b" };
	constexpr auto FORMAT_COLOR_TEXT{ u8"\ue23c" };
	constexpr auto FORMAT_INDENT_DECREASE{ u8"\ue23d" };
	constexpr auto FORMAT_INDENT_INCREASE{ u8"\ue23e" };
	constexpr auto FORMAT_ITALIC{ u8"\ue23f" };
	constexpr auto FORMAT_LINE_SPACING{ u8"\ue240" };
	constexpr auto FORMAT_LIST_BULLETED{ u8"\ue241" };
	constexpr auto FORMAT_LIST_NUMBERED{ u8"\ue242" };
	constexpr auto FORMAT_PAINT{ u8"\ue243" };
	constexpr auto FORMAT_QUOTE{ u8"\ue244" };
	constexpr auto FORMAT_SHAPES{ u8"\ue25e" };
	constexpr auto FORMAT_SIZE{ u8"\ue245" };
	constexpr auto FORMAT_STRIKETHROUGH{ u8"\ue246" };
	constexpr auto FORMAT_TEXTDIRECTION_L_TO_R{ u8"\ue247" };
	constexpr auto FORMAT_TEXTDIRECTION_R_TO_L{ u8"\ue248" };
	constexpr auto FORMAT_UNDERLINED{ u8"\ue249" };
	constexpr auto FORUM{ u8"\ue0bf" };
	constexpr auto FORWARD{ u8"\ue154" };
	constexpr auto FORWARD_10{ u8"\ue056" };
	constexpr auto FORWARD_30{ u8"\ue057" };
	constexpr auto FORWARD_5{ u8"\ue058" };
	constexpr auto FREE_BREAKFAST{ u8"\ueb44" };
	constexpr auto FULLSCREEN{ u8"\ue5d0" };
	constexpr auto FULLSCREEN_EXIT{ u8"\ue5d1" };
	constexpr auto FUNCTIONS{ u8"\ue24a" };
	constexpr auto G_TRANSLATE{ u8"\ue927" };
	constexpr auto GAMEPAD{ u8"\ue30f" };
	constexpr auto GAMES{ u8"\ue021" };
	constexpr auto GAVEL{ u8"\ue90e" };
	constexpr auto GESTURE{ u8"\ue155" };
	constexpr auto GET_APP{ u8"\ue884" };
	constexpr auto GIF{ u8"\ue908" };
	constexpr auto GOLF_COURSE{ u8"\ueb45" };
	constexpr auto GPS_FIXED{ u8"\ue1b3" };
	constexpr auto GPS_NOT_FIXED{ u8"\ue1b4" };
	constexpr auto GPS_OFF{ u8"\ue1b5" };
	constexpr auto GRADE{ u8"\ue885" };
	constexpr auto GRADIENT{ u8"\ue3e9" };
	constexpr auto GRAIN{ u8"\ue3ea" };
	constexpr auto GRAPHIC_EQ{ u8"\ue1b8" };
	constexpr auto GRID_OFF{ u8"\ue3eb" };
	constexpr auto GRID_ON{ u8"\ue3ec" };
	constexpr auto GROUP{ u8"\ue7ef" };
	constexpr auto GROUP_ADD{ u8"\ue7f0" };
	constexpr auto GROUP_WORK{ u8"\ue886" };
	constexpr auto HD{ u8"\ue052" };
	constexpr auto HDR_OFF{ u8"\ue3ed" };
	constexpr auto HDR_ON{ u8"\ue3ee" };
	constexpr auto HDR_STRONG{ u8"\ue3f1" };
	constexpr auto HDR_WEAK{ u8"\ue3f2" };
	constexpr auto HEADSET{ u8"\ue310" };
	constexpr auto HEADSET_MIC{ u8"\ue311" };
	constexpr auto HEALING{ u8"\ue3f3" };
	constexpr auto HEARING{ u8"\ue023" };
	constexpr auto HELP{ u8"\ue887" };
	constexpr auto HELP_OUTLINE{ u8"\ue8fd" };
	constexpr auto HIGH_QUALITY{ u8"\ue024" };
	constexpr auto HIGHLIGHT{ u8"\ue25f" };
	constexpr auto HIGHLIGHT_OFF{ u8"\ue888" };
	constexpr auto HISTORY{ u8"\ue889" };
	constexpr auto HOME{ u8"\ue88a" };
	constexpr auto HOT_TUB{ u8"\ueb46" };
	constexpr auto HOTEL{ u8"\ue53a" };
	constexpr auto HOURGLASS_EMPTY{ u8"\ue88b" };
	constexpr auto HOURGLASS_FULL{ u8"\ue88c" };
	constexpr auto HTTP{ u8"\ue902" };
	constexpr auto HTTPS{ u8"\ue88d" };
	constexpr auto IMAGE{ u8"\ue3f4" };
	constexpr auto IMAGE_ASPECT_RATIO{ u8"\ue3f5" };
	constexpr auto IMPORT_CONTACTS{ u8"\ue0e0" };
	constexpr auto IMPORT_EXPORT{ u8"\ue0c3" };
	constexpr auto IMPORTANT_DEVICES{ u8"\ue912" };
	constexpr auto INBOX{ u8"\ue156" };
	constexpr auto INDETERMINATE_CHECK_BOX{ u8"\ue909" };
	constexpr auto INFO{ u8"\ue88e" };
	constexpr auto INFO_OUTLINE{ u8"\ue88f" };
	constexpr auto INPUT{ u8"\ue890" };
	constexpr auto INSERT_CHART{ u8"\ue24b" };
	constexpr auto INSERT_COMMENT{ u8"\ue24c" };
	constexpr auto INSERT_DRIVE_FILE{ u8"\ue24d" };
	constexpr auto INSERT_EMOTICON{ u8"\ue24e" };
	constexpr auto INSERT_INVITATION{ u8"\ue24f" };
	constexpr auto INSERT_LINK{ u8"\ue250" };
	constexpr auto INSERT_PHOTO{ u8"\ue251" };
	constexpr auto INVERT_COLORS{ u8"\ue891" };
	constexpr auto INVERT_COLORS_OFF{ u8"\ue0c4" };
	constexpr auto ISO{ u8"\ue3f6" };
	constexpr auto KEYBOARD{ u8"\ue312" };
	constexpr auto KEYBOARD_ARROW_DOWN{ u8"\ue313" };
	constexpr auto KEYBOARD_ARROW_LEFT{ u8"\ue314" };
	constexpr auto KEYBOARD_ARROW_RIGHT{ u8"\ue315" };
	constexpr auto KEYBOARD_ARROW_UP{ u8"\ue316" };
	constexpr auto KEYBOARD_BACKSPACE{ u8"\ue317" };
	constexpr auto KEYBOARD_CAPSLOCK{ u8"\ue318" };
	constexpr auto KEYBOARD_HIDE{ u8"\ue31a" };
	constexpr auto KEYBOARD_RETURN{ u8"\ue31b" };
	constexpr auto KEYBOARD_TAB{ u8"\ue31c" };
	constexpr auto KEYBOARD_VOICE{ u8"\ue31d" };
	constexpr auto KITCHEN{ u8"\ueb47" };
	constexpr auto LABEL{ u8"\ue892" };
	constexpr auto LABEL_OUTLINE{ u8"\ue893" };
	constexpr auto LANDSCAPE{ u8"\ue3f7" };
	constexpr auto LANGUAGE{ u8"\ue894" };
	constexpr auto LAPTOP{ u8"\ue31e" };
	constexpr auto LAPTOP_CHROMEBOOK{ u8"\ue31f" };
	constexpr auto LAPTOP_MAC{ u8"\ue320" };
	constexpr auto LAPTOP_WINDOWS{ u8"\ue321" };
	constexpr auto LAST_PAGE{ u8"\ue5dd" };
	constexpr auto LAUNCH{ u8"\ue895" };
	constexpr auto LAYERS{ u8"\ue53b" };
	constexpr auto LAYERS_CLEAR{ u8"\ue53c" };
	constexpr auto LEAK_ADD{ u8"\ue3f8" };
	constexpr auto LEAK_REMOVE{ u8"\ue3f9" };
	constexpr auto LENS{ u8"\ue3fa" };
	constexpr auto LIBRARY_ADD{ u8"\ue02e" };
	constexpr auto LIBRARY_BOOKS{ u8"\ue02f" };
	constexpr auto LIBRARY_MUSIC{ u8"\ue030" };
	constexpr auto LIGHTBULB_OUTLINE{ u8"\ue90f" };
	constexpr auto LINE_STYLE{ u8"\ue919" };
	constexpr auto LINE_WEIGHT{ u8"\ue91a" };
	constexpr auto LINEAR_SCALE{ u8"\ue260" };
	constexpr auto LINK{ u8"\ue157" };
	constexpr auto LINKED_CAMERA{ u8"\ue438" };
	constexpr auto LIST{ u8"\ue896" };
	constexpr auto LIVE_HELP{ u8"\ue0c6" };
	constexpr auto LIVE_TV{ u8"\ue639" };
	constexpr auto LOCAL_ACTIVITY{ u8"\ue53f" };
	constexpr auto LOCAL_AIRPORT{ u8"\ue53d" };
	constexpr auto LOCAL_ATM{ u8"\ue53e" };
	constexpr auto LOCAL_BAR{ u8"\ue540" };
	constexpr auto LOCAL_CAFE{ u8"\ue541" };
	constexpr auto LOCAL_CAR_WASH{ u8"\ue542" };
	constexpr auto LOCAL_CONVENIENCE_STORE{ u8"\ue543" };
	constexpr auto LOCAL_DINING{ u8"\ue556" };
	constexpr auto LOCAL_DRINK{ u8"\ue544" };
	constexpr auto LOCAL_FLORIST{ u8"\ue545" };
	constexpr auto LOCAL_GAS_STATION{ u8"\ue546" };
	constexpr auto LOCAL_GROCERY_STORE{ u8"\ue547" };
	constexpr auto LOCAL_HOSPITAL{ u8"\ue548" };
	constexpr auto LOCAL_HOTEL{ u8"\ue549" };
	constexpr auto LOCAL_LAUNDRY_SERVICE{ u8"\ue54a" };
	constexpr auto LOCAL_LIBRARY{ u8"\ue54b" };
	constexpr auto LOCAL_MALL{ u8"\ue54c" };
	constexpr auto LOCAL_MOVIES{ u8"\ue54d" };
	constexpr auto LOCAL_OFFER{ u8"\ue54e" };
	constexpr auto LOCAL_PARKING{ u8"\ue54f" };
	constexpr auto LOCAL_PHARMACY{ u8"\ue550" };
	constexpr auto LOCAL_PHONE{ u8"\ue551" };
	constexpr auto LOCAL_PIZZA{ u8"\ue552" };
	constexpr auto LOCAL_PLAY{ u8"\ue553" };
	constexpr auto LOCAL_POST_OFFICE{ u8"\ue554" };
	constexpr auto LOCAL_PRINTSHOP{ u8"\ue555" };
	constexpr auto LOCAL_SEE{ u8"\ue557" };
	constexpr auto LOCAL_SHIPPING{ u8"\ue558" };
	constexpr auto LOCAL_TAXI{ u8"\ue559" };
	constexpr auto LOCATION_CITY{ u8"\ue7f1" };
	constexpr auto LOCATION_DISABLED{ u8"\ue1b6" };
	constexpr auto LOCATION_OFF{ u8"\ue0c7" };
	constexpr auto LOCATION_ON{ u8"\ue0c8" };
	constexpr auto LOCATION_SEARCHING{ u8"\ue1b7" };
	constexpr auto LOCK{ u8"\ue897" };
	constexpr auto LOCK_OPEN{ u8"\ue898" };
	constexpr auto LOCK_OUTLINE{ u8"\ue899" };
	constexpr auto LOOKS{ u8"\ue3fc" };
	constexpr auto LOOKS_3{ u8"\ue3fb" };
	constexpr auto LOOKS_4{ u8"\ue3fd" };
	constexpr auto LOOKS_5{ u8"\ue3fe" };
	constexpr auto LOOKS_6{ u8"\ue3ff" };
	constexpr auto LOOKS_ONE{ u8"\ue400" };
	constexpr auto LOOKS_TWO{ u8"\ue401" };
	constexpr auto LOOP{ u8"\ue028" };
	constexpr auto LOUPE{ u8"\ue402" };
	constexpr auto LOW_PRIORITY{ u8"\ue16d" };
	constexpr auto LOYALTY{ u8"\ue89a" };
	constexpr auto MAIL{ u8"\ue158" };
	constexpr auto MAIL_OUTLINE{ u8"\ue0e1" };
	constexpr auto MAP{ u8"\ue55b" };
	constexpr auto MARKUNREAD{ u8"\ue159" };
	constexpr auto MARKUNREAD_MAILBOX{ u8"\ue89b" };
	constexpr auto MEMORY{ u8"\ue322" };
	constexpr auto MENU{ u8"\ue5d2" };
	constexpr auto MERGE_TYPE{ u8"\ue252" };
	constexpr auto MESSAGE{ u8"\ue0c9" };
	constexpr auto MIC{ u8"\ue029" };
	constexpr auto MIC_NONE{ u8"\ue02a" };
	constexpr auto MIC_OFF{ u8"\ue02b" };
	constexpr auto MMS{ u8"\ue618" };
	constexpr auto MODE_COMMENT{ u8"\ue253" };
	constexpr auto MODE_EDIT{ u8"\ue254" };
	constexpr auto MONETIZATION_ON{ u8"\ue263" };
	constexpr auto MONEY_OFF{ u8"\ue25c" };
	constexpr auto MONOCHROME_PHOTOS{ u8"\ue403" };
	constexpr auto MOOD{ u8"\ue7f2" };
	constexpr auto MOOD_BAD{ u8"\ue7f3" };
	constexpr auto MORE{ u8"\ue619" };
	constexpr auto MORE_HORIZ{ u8"\ue5d3" };
	constexpr auto MORE_VERT{ u8"\ue5d4" };
	constexpr auto MOTORCYCLE{ u8"\ue91b" };
	constexpr auto MOUSE{ u8"\ue323" };
	constexpr auto MOVE_TO_INBOX{ u8"\ue168" };
	constexpr auto MOVIE{ u8"\ue02c" };
	constexpr auto MOVIE_CREATION{ u8"\ue404" };
	constexpr auto MOVIE_FILTER{ u8"\ue43a" };
	constexpr auto MULTILINE_CHART{ u8"\ue6df" };
	constexpr auto MUSIC_NOTE{ u8"\ue405" };
	constexpr auto MUSIC_VIDEO{ u8"\ue063" };
	constexpr auto MY_LOCATION{ u8"\ue55c" };
	constexpr auto NATURE{ u8"\ue406" };
	constexpr auto NATURE_PEOPLE{ u8"\ue407" };
	constexpr auto NAVIGATE_BEFORE{ u8"\ue408" };
	constexpr auto NAVIGATE_NEXT{ u8"\ue409" };
	constexpr auto NAVIGATION{ u8"\ue55d" };
	constexpr auto NEAR_ME{ u8"\ue569" };
	constexpr auto NETWORK_CELL{ u8"\ue1b9" };
	constexpr auto NETWORK_CHECK{ u8"\ue640" };
	constexpr auto NETWORK_LOCKED{ u8"\ue61a" };
	constexpr auto NETWORK_WIFI{ u8"\ue1ba" };
	constexpr auto NEW_RELEASES{ u8"\ue031" };
	constexpr auto NEXT_WEEK{ u8"\ue16a" };
	constexpr auto NFC{ u8"\ue1bb" };
	constexpr auto NO_ENCRYPTION{ u8"\ue641" };
	constexpr auto NO_SIM{ u8"\ue0cc" };
	constexpr auto NOT_INTERESTED{ u8"\ue033" };
	constexpr auto NOTE{ u8"\ue06f" };
	constexpr auto NOTE_ADD{ u8"\ue89c" };
	constexpr auto NOTIFICATIONS{ u8"\ue7f4" };
	constexpr auto NOTIFICATIONS_ACTIVE{ u8"\ue7f7" };
	constexpr auto NOTIFICATIONS_NONE{ u8"\ue7f5" };
	constexpr auto NOTIFICATIONS_OFF{ u8"\ue7f6" };
	constexpr auto NOTIFICATIONS_PAUSED{ u8"\ue7f8" };
	constexpr auto OFFLINE_PIN{ u8"\ue90a" };
	constexpr auto ONDEMAND_VIDEO{ u8"\ue63a" };
	constexpr auto OPACITY{ u8"\ue91c" };
	constexpr auto OPEN_IN_BROWSER{ u8"\ue89d" };
	constexpr auto OPEN_IN_NEW{ u8"\ue89e" };
	constexpr auto OPEN_WITH{ u8"\ue89f" };
	constexpr auto PAGES{ u8"\ue7f9" };
	constexpr auto PAGEVIEW{ u8"\ue8a0" };
	constexpr auto PALETTE{ u8"\ue40a" };
	constexpr auto PAN_TOOL{ u8"\ue925" };
	constexpr auto PANORAMA{ u8"\ue40b" };
	constexpr auto PANORAMA_FISH_EYE{ u8"\ue40c" };
	constexpr auto PANORAMA_HORIZONTAL{ u8"\ue40d" };
	constexpr auto PANORAMA_VERTICAL{ u8"\ue40e" };
	constexpr auto PANORAMA_WIDE_ANGLE{ u8"\ue40f" };
	constexpr auto PARTY_MODE{ u8"\ue7fa" };
	constexpr auto PAUSE{ u8"\ue034" };
	constexpr auto PAUSE_CIRCLE_FILLED{ u8"\ue035" };
	constexpr auto PAUSE_CIRCLE_OUTLINE{ u8"\ue036" };
	constexpr auto PAYMENT{ u8"\ue8a1" };
	constexpr auto PEOPLE{ u8"\ue7fb" };
	constexpr auto PEOPLE_OUTLINE{ u8"\ue7fc" };
	constexpr auto PERM_CAMERA_MIC{ u8"\ue8a2" };
	constexpr auto PERM_CONTACT_CALENDAR{ u8"\ue8a3" };
	constexpr auto PERM_DATA_SETTING{ u8"\ue8a4" };
	constexpr auto PERM_DEVICE_INFORMATION{ u8"\ue8a5" };
	constexpr auto PERM_IDENTITY{ u8"\ue8a6" };
	constexpr auto PERM_MEDIA{ u8"\ue8a7" };
	constexpr auto PERM_PHONE_MSG{ u8"\ue8a8" };
	constexpr auto PERM_SCAN_WIFI{ u8"\ue8a9" };
	constexpr auto PERSON{ u8"\ue7fd" };
	constexpr auto PERSON_ADD{ u8"\ue7fe" };
	constexpr auto PERSON_OUTLINE{ u8"\ue7ff" };
	constexpr auto PERSON_PIN{ u8"\ue55a" };
	constexpr auto PERSON_PIN_CIRCLE{ u8"\ue56a" };
	constexpr auto PERSONAL_VIDEO{ u8"\ue63b" };
	constexpr auto PETS{ u8"\ue91d" };
	constexpr auto PHONE{ u8"\ue0cd" };
	constexpr auto PHONE_ANDROID{ u8"\ue324" };
	constexpr auto PHONE_BLUETOOTH_SPEAKER{ u8"\ue61b" };
	constexpr auto PHONE_FORWARDED{ u8"\ue61c" };
	constexpr auto PHONE_IN_TALK{ u8"\ue61d" };
	constexpr auto PHONE_IPHONE{ u8"\ue325" };
	constexpr auto PHONE_LOCKED{ u8"\ue61e" };
	constexpr auto PHONE_MISSED{ u8"\ue61f" };
	constexpr auto PHONE_PAUSED{ u8"\ue620" };
	constexpr auto PHONELINK{ u8"\ue326" };
	constexpr auto PHONELINK_ERASE{ u8"\ue0db" };
	constexpr auto PHONELINK_LOCK{ u8"\ue0dc" };
	constexpr auto PHONELINK_OFF{ u8"\ue327" };
	constexpr auto PHONELINK_RING{ u8"\ue0dd" };
	constexpr auto PHONELINK_SETUP{ u8"\ue0de" };
	constexpr auto PHOTO{ u8"\ue410" };
	constexpr auto PHOTO_ALBUM{ u8"\ue411" };
	constexpr auto PHOTO_CAMERA{ u8"\ue412" };
	constexpr auto PHOTO_FILTER{ u8"\ue43b" };
	constexpr auto PHOTO_LIBRARY{ u8"\ue413" };
	constexpr auto PHOTO_SIZE_SELECT_ACTUAL{ u8"\ue432" };
	constexpr auto PHOTO_SIZE_SELECT_LARGE{ u8"\ue433" };
	constexpr auto PHOTO_SIZE_SELECT_SMALL{ u8"\ue434" };
	constexpr auto PICTURE_AS_PDF{ u8"\ue415" };
	constexpr auto PICTURE_IN_PICTURE{ u8"\ue8aa" };
	constexpr auto PICTURE_IN_PICTURE_ALT{ u8"\ue911" };
	constexpr auto PIE_CHART{ u8"\ue6c4" };
	constexpr auto PIE_CHART_OUTLINED{ u8"\ue6c5" };
	constexpr auto PIN_DROP{ u8"\ue55e" };
	constexpr auto PLACE{ u8"\ue55f" };
	constexpr auto PLAY_ARROW{ u8"\ue037" };
	constexpr auto PLAY_CIRCLE_FILLED{ u8"\ue038" };
	constexpr auto PLAY_CIRCLE_OUTLINE{ u8"\ue039" };
	constexpr auto PLAY_FOR_WORK{ u8"\ue906" };
	constexpr auto PLAYLIST_ADD{ u8"\ue03b" };
	constexpr auto PLAYLIST_ADD_CHECK{ u8"\ue065" };
	constexpr auto PLAYLIST_PLAY{ u8"\ue05f" };
	constexpr auto PLUS_ONE{ u8"\ue800" };
	constexpr auto POLL{ u8"\ue801" };
	constexpr auto POLYMER{ u8"\ue8ab" };
	constexpr auto POOL{ u8"\ueb48" };
	constexpr auto PORTABLE_WIFI_OFF{ u8"\ue0ce" };
	constexpr auto PORTRAIT{ u8"\ue416" };
	constexpr auto POWER{ u8"\ue63c" };
	constexpr auto POWER_INPUT{ u8"\ue336" };
	constexpr auto POWER_SETTINGS_NEW{ u8"\ue8ac" };
	constexpr auto PREGNANT_WOMAN{ u8"\ue91e" };
	constexpr auto PRESENT_TO_ALL{ u8"\ue0df" };
	constexpr auto PRINT{ u8"\ue8ad" };
	constexpr auto PRIORITY_HIGH{ u8"\ue645" };
	constexpr auto PUBLIC{ u8"\ue80b" };
	constexpr auto PUBLISH{ u8"\ue255" };
	constexpr auto QUERY_BUILDER{ u8"\ue8ae" };
	constexpr auto QUESTION_ANSWER{ u8"\ue8af" };
	constexpr auto QUEUE{ u8"\ue03c" };
	constexpr auto QUEUE_MUSIC{ u8"\ue03d" };
	constexpr auto QUEUE_PLAY_NEXT{ u8"\ue066" };
	constexpr auto RADIO{ u8"\ue03e" };
	constexpr auto RADIO_BUTTON_CHECKED{ u8"\ue837" };
	constexpr auto RADIO_BUTTON_UNCHECKED{ u8"\ue836" };
	constexpr auto RATE_REVIEW{ u8"\ue560" };
	constexpr auto RECEIPT{ u8"\ue8b0" };
	constexpr auto RECENT_ACTORS{ u8"\ue03f" };
	constexpr auto RECORD_VOICE_OVER{ u8"\ue91f" };
	constexpr auto REDEEM{ u8"\ue8b1" };
	constexpr auto REDO{ u8"\ue15a" };
	constexpr auto REFRESH{ u8"\ue5d5" };
	constexpr auto REMOVE{ u8"\ue15b" };
	constexpr auto REMOVE_CIRCLE{ u8"\ue15c" };
	constexpr auto REMOVE_CIRCLE_OUTLINE{ u8"\ue15d" };
	constexpr auto REMOVE_FROM_QUEUE{ u8"\ue067" };
	constexpr auto REMOVE_RED_EYE{ u8"\ue417" };
	constexpr auto REMOVE_SHOPPING_CART{ u8"\ue928" };
	constexpr auto REORDER{ u8"\ue8fe" };
	constexpr auto REPEAT{ u8"\ue040" };
	constexpr auto REPEAT_ONE{ u8"\ue041" };
	constexpr auto REPLAY{ u8"\ue042" };
	constexpr auto REPLAY_10{ u8"\ue059" };
	constexpr auto REPLAY_30{ u8"\ue05a" };
	constexpr auto REPLAY_5{ u8"\ue05b" };
	constexpr auto REPLY{ u8"\ue15e" };
	constexpr auto REPLY_ALL{ u8"\ue15f" };
	constexpr auto REPORT{ u8"\ue160" };
	constexpr auto REPORT_PROBLEM{ u8"\ue8b2" };
	constexpr auto RESTAURANT{ u8"\ue56c" };
	constexpr auto RESTAURANT_MENU{ u8"\ue561" };
	constexpr auto RESTORE{ u8"\ue8b3" };
	constexpr auto RESTORE_PAGE{ u8"\ue929" };
	constexpr auto RING_VOLUME{ u8"\ue0d1" };
	constexpr auto ROOM{ u8"\ue8b4" };
	constexpr auto ROOM_SERVICE{ u8"\ueb49" };
	constexpr auto ROTATE_90_DEGREES_CCW{ u8"\ue418" };
	constexpr auto ROTATE_LEFT{ u8"\ue419" };
	constexpr auto ROTATE_RIGHT{ u8"\ue41a" };
	constexpr auto ROUNDED_CORNER{ u8"\ue920" };
	constexpr auto ROUTER{ u8"\ue328" };
	constexpr auto ROWING{ u8"\ue921" };
	constexpr auto RSS_FEED{ u8"\ue0e5" };
	constexpr auto RV_HOOKUP{ u8"\ue642" };
	constexpr auto SATELLITE{ u8"\ue562" };
	constexpr auto SAVE{ u8"\ue161" };
	constexpr auto SCANNER{ u8"\ue329" };
	constexpr auto SCHEDULE{ u8"\ue8b5" };
	constexpr auto SCHOOL{ u8"\ue80c" };
	constexpr auto SCREEN_LOCK_LANDSCAPE{ u8"\ue1be" };
	constexpr auto SCREEN_LOCK_PORTRAIT{ u8"\ue1bf" };
	constexpr auto SCREEN_LOCK_ROTATION{ u8"\ue1c0" };
	constexpr auto SCREEN_ROTATION{ u8"\ue1c1" };
	constexpr auto SCREEN_SHARE{ u8"\ue0e2" };
	constexpr auto SD_CARD{ u8"\ue623" };
	constexpr auto SD_STORAGE{ u8"\ue1c2" };
	constexpr auto SEARCH{ u8"\ue8b6" };
	constexpr auto SECURITY{ u8"\ue32a" };
	constexpr auto SELECT_ALL{ u8"\ue162" };
	constexpr auto SEND{ u8"\ue163" };
	constexpr auto SENTIMENT_DISSATISFIED{ u8"\ue811" };
	constexpr auto SENTIMENT_NEUTRAL{ u8"\ue812" };
	constexpr auto SENTIMENT_SATISFIED{ u8"\ue813" };
	constexpr auto SENTIMENT_VERY_DISSATISFIED{ u8"\ue814" };
	constexpr auto SENTIMENT_VERY_SATISFIED{ u8"\ue815" };
	constexpr auto SETTINGS{ u8"\ue8b8" };
	constexpr auto SETTINGS_APPLICATIONS{ u8"\ue8b9" };
	constexpr auto SETTINGS_BACKUP_RESTORE{ u8"\ue8ba" };
	constexpr auto SETTINGS_BLUETOOTH{ u8"\ue8bb" };
	constexpr auto SETTINGS_BRIGHTNESS{ u8"\ue8bd" };
	constexpr auto SETTINGS_CELL{ u8"\ue8bc" };
	constexpr auto SETTINGS_ETHERNET{ u8"\ue8be" };
	constexpr auto SETTINGS_INPUT_ANTENNA{ u8"\ue8bf" };
	constexpr auto SETTINGS_INPUT_COMPONENT{ u8"\ue8c0" };
	constexpr auto SETTINGS_INPUT_COMPOSITE{ u8"\ue8c1" };
	constexpr auto SETTINGS_INPUT_HDMI{ u8"\ue8c2" };
	constexpr auto SETTINGS_INPUT_SVIDEO{ u8"\ue8c3" };
	constexpr auto SETTINGS_OVERSCAN{ u8"\ue8c4" };
	constexpr auto SETTINGS_PHONE{ u8"\ue8c5" };
	constexpr auto SETTINGS_POWER{ u8"\ue8c6" };
	constexpr auto SETTINGS_REMOTE{ u8"\ue8c7" };
	constexpr auto SETTINGS_SYSTEM_DAYDREAM{ u8"\ue1c3" };
	constexpr auto SETTINGS_VOICE{ u8"\ue8c8" };
	constexpr auto SHARE{ u8"\ue80d" };
	constexpr auto SHOP{ u8"\ue8c9" };
	constexpr auto SHOP_TWO{ u8"\ue8ca" };
	constexpr auto SHOPPING_BASKET{ u8"\ue8cb" };
	constexpr auto SHOPPING_CART{ u8"\ue8cc" };
	constexpr auto SHORT_TEXT{ u8"\ue261" };
	constexpr auto SHOW_CHART{ u8"\ue6e1" };
	constexpr auto SHUFFLE{ u8"\ue043" };
	constexpr auto SIGNAL_CELLULAR_4_BAR{ u8"\ue1c8" };
	constexpr auto SIGNAL_CELLULAR_CONNECTED_NO_INTERNET_4_BAR{ u8"\ue1cd" };
	constexpr auto SIGNAL_CELLULAR_NO_SIM{ u8"\ue1ce" };
	constexpr auto SIGNAL_CELLULAR_NULL{ u8"\ue1cf" };
	constexpr auto SIGNAL_CELLULAR_OFF{ u8"\ue1d0" };
	constexpr auto SIGNAL_WIFI_4_BAR{ u8"\ue1d8" };
	constexpr auto SIGNAL_WIFI_4_BAR_LOCK{ u8"\ue1d9" };
	constexpr auto SIGNAL_WIFI_OFF{ u8"\ue1da" };
	constexpr auto SIM_CARD{ u8"\ue32b" };
	constexpr auto SIM_CARD_ALERT{ u8"\ue624" };
	constexpr auto SKIP_NEXT{ u8"\ue044" };
	constexpr auto SKIP_PREVIOUS{ u8"\ue045" };
	constexpr auto SLIDESHOW{ u8"\ue41b" };
	constexpr auto SLOW_MOTION_VIDEO{ u8"\ue068" };
	constexpr auto SMARTPHONE{ u8"\ue32c" };
	constexpr auto SMOKE_FREE{ u8"\ueb4a" };
	constexpr auto SMOKING_ROOMS{ u8"\ueb4b" };
	constexpr auto SMS{ u8"\ue625" };
	constexpr auto SMS_FAILED{ u8"\ue626" };
	constexpr auto SNOOZE{ u8"\ue046" };
	constexpr auto SORT{ u8"\ue164" };
	constexpr auto SORT_BY_ALPHA{ u8"\ue053" };
	constexpr auto SPA{ u8"\ueb4c" };
	constexpr auto SPACE_BAR{ u8"\ue256" };
	constexpr auto SPEAKER{ u8"\ue32d" };
	constexpr auto SPEAKER_GROUP{ u8"\ue32e" };
	constexpr auto SPEAKER_NOTES{ u8"\ue8cd" };
	constexpr auto SPEAKER_NOTES_OFF{ u8"\ue92a" };
	constexpr auto SPEAKER_PHONE{ u8"\ue0d2" };
	constexpr auto SPELLCHECK{ u8"\ue8ce" };
	constexpr auto STAR{ u8"\ue838" };
	constexpr auto STAR_BORDER{ u8"\ue83a" };
	constexpr auto STAR_HALF{ u8"\ue839" };
	constexpr auto STARS{ u8"\ue8d0" };
	constexpr auto STAY_CURRENT_LANDSCAPE{ u8"\ue0d3" };
	constexpr auto STAY_CURRENT_PORTRAIT{ u8"\ue0d4" };
	constexpr auto STAY_PRIMARY_LANDSCAPE{ u8"\ue0d5" };
	constexpr auto STAY_PRIMARY_PORTRAIT{ u8"\ue0d6" };
	constexpr auto STOP{ u8"\ue047" };
	constexpr auto STOP_SCREEN_SHARE{ u8"\ue0e3" };
	constexpr auto STORAGE{ u8"\ue1db" };
	constexpr auto STORE{ u8"\ue8d1" };
	constexpr auto STORE_MALL_DIRECTORY{ u8"\ue563" };
	constexpr auto STRAIGHTEN{ u8"\ue41c" };
	constexpr auto STREETVIEW{ u8"\ue56e" };
	constexpr auto STRIKETHROUGH_S{ u8"\ue257" };
	constexpr auto STYLE{ u8"\ue41d" };
	constexpr auto SUBDIRECTORY_ARROW_LEFT{ u8"\ue5d9" };
	constexpr auto SUBDIRECTORY_ARROW_RIGHT{ u8"\ue5da" };
	constexpr auto SUBJECT{ u8"\ue8d2" };
	constexpr auto SUBSCRIPTIONS{ u8"\ue064" };
	constexpr auto SUBTITLES{ u8"\ue048" };
	constexpr auto SUBWAY{ u8"\ue56f" };
	constexpr auto SUPERVISOR_ACCOUNT{ u8"\ue8d3" };
	constexpr auto SURROUND_SOUND{ u8"\ue049" };
	constexpr auto SWAP_CALLS{ u8"\ue0d7" };
	constexpr auto SWAP_HORIZ{ u8"\ue8d4" };
	constexpr auto SWAP_VERT{ u8"\ue8d5" };
	constexpr auto SWAP_VERTICAL_CIRCLE{ u8"\ue8d6" };
	constexpr auto SWITCH_CAMERA{ u8"\ue41e" };
	constexpr auto SWITCH_VIDEO{ u8"\ue41f" };
	constexpr auto SYNC{ u8"\ue627" };
	constexpr auto SYNC_DISABLED{ u8"\ue628" };
	constexpr auto SYNC_PROBLEM{ u8"\ue629" };
	constexpr auto SYSTEM_UPDATE{ u8"\ue62a" };
	constexpr auto SYSTEM_UPDATE_ALT{ u8"\ue8d7" };
	constexpr auto TAB{ u8"\ue8d8" };
	constexpr auto TAB_UNSELECTED{ u8"\ue8d9" };
	constexpr auto TABLET{ u8"\ue32f" };
	constexpr auto TABLET_ANDROID{ u8"\ue330" };
	constexpr auto TABLET_MAC{ u8"\ue331" };
	constexpr auto TAG_FACES{ u8"\ue420" };
	constexpr auto TAP_AND_PLAY{ u8"\ue62b" };
	constexpr auto TERRAIN{ u8"\ue564" };
	constexpr auto TEXT_FIELDS{ u8"\ue262" };
	constexpr auto TEXT_FORMAT{ u8"\ue165" };
	constexpr auto TEXTSMS{ u8"\ue0d8" };
	constexpr auto TEXTURE{ u8"\ue421" };
	constexpr auto THEATERS{ u8"\ue8da" };
	constexpr auto THUMB_DOWN{ u8"\ue8db" };
	constexpr auto THUMB_UP{ u8"\ue8dc" };
	constexpr auto THUMBS_UP_DOWN{ u8"\ue8dd" };
	constexpr auto TIME_TO_LEAVE{ u8"\ue62c" };
	constexpr auto TIMELAPSE{ u8"\ue422" };
	constexpr auto TIMELINE{ u8"\ue922" };
	constexpr auto TIMER{ u8"\ue425" };
	constexpr auto TIMER_10{ u8"\ue423" };
	constexpr auto TIMER_3{ u8"\ue424" };
	constexpr auto TIMER_OFF{ u8"\ue426" };
	constexpr auto TITLE{ u8"\ue264" };
	constexpr auto TOC{ u8"\ue8de" };
	constexpr auto TODAY{ u8"\ue8df" };
	constexpr auto TOLL{ u8"\ue8e0" };
	constexpr auto TONALITY{ u8"\ue427" };
	constexpr auto TOUCH_APP{ u8"\ue913" };
	constexpr auto TOYS{ u8"\ue332" };
	constexpr auto TRACK_CHANGES{ u8"\ue8e1" };
	constexpr auto TRAFFIC{ u8"\ue565" };
	constexpr auto TRAIN{ u8"\ue570" };
	constexpr auto TRAM{ u8"\ue571" };
	constexpr auto TRANSFER_WITHIN_A_STATION{ u8"\ue572" };
	constexpr auto TRANSFORM{ u8"\ue428" };
	constexpr auto TRANSLATE{ u8"\ue8e2" };
	constexpr auto TRENDING_DOWN{ u8"\ue8e3" };
	constexpr auto TRENDING_FLAT{ u8"\ue8e4" };
	constexpr auto TRENDING_UP{ u8"\ue8e5" };
	constexpr auto TUNE{ u8"\ue429" };
	constexpr auto TURNED_IN{ u8"\ue8e6" };
	constexpr auto TURNED_IN_NOT{ u8"\ue8e7" };
	constexpr auto TV{ u8"\ue333" };
	constexpr auto UNARCHIVE{ u8"\ue169" };
	constexpr auto UNDO{ u8"\ue166" };
	constexpr auto UNFOLD_LESS{ u8"\ue5d6" };
	constexpr auto UNFOLD_MORE{ u8"\ue5d7" };
	constexpr auto UPDATE{ u8"\ue923" };
	constexpr auto USB{ u8"\ue1e0" };
	constexpr auto VERIFIED_USER{ u8"\ue8e8" };
	constexpr auto VERTICAL_ALIGN_BOTTOM{ u8"\ue258" };
	constexpr auto VERTICAL_ALIGN_CENTER{ u8"\ue259" };
	constexpr auto VERTICAL_ALIGN_TOP{ u8"\ue25a" };
	constexpr auto VIBRATION{ u8"\ue62d" };
	constexpr auto VIDEO_CALL{ u8"\ue070" };
	constexpr auto VIDEO_LABEL{ u8"\ue071" };
	constexpr auto VIDEO_LIBRARY{ u8"\ue04a" };
	constexpr auto VIDEOCAM{ u8"\ue04b" };
	constexpr auto VIDEOCAM_OFF{ u8"\ue04c" };
	constexpr auto VIDEOGAME_ASSET{ u8"\ue338" };
	constexpr auto VIEW_AGENDA{ u8"\ue8e9" };
	constexpr auto VIEW_ARRAY{ u8"\ue8ea" };
	constexpr auto VIEW_CAROUSEL{ u8"\ue8eb" };
	constexpr auto VIEW_COLUMN{ u8"\ue8ec" };
	constexpr auto VIEW_COMFY{ u8"\ue42a" };
	constexpr auto VIEW_COMPACT{ u8"\ue42b" };
	constexpr auto VIEW_DAY{ u8"\ue8ed" };
	constexpr auto VIEW_HEADLINE{ u8"\ue8ee" };
	constexpr auto VIEW_LIST{ u8"\ue8ef" };
	constexpr auto VIEW_MODULE{ u8"\ue8f0" };
	constexpr auto VIEW_QUILT{ u8"\ue8f1" };
	constexpr auto VIEW_STREAM{ u8"\ue8f2" };
	constexpr auto VIEW_WEEK{ u8"\ue8f3" };
	constexpr auto VIGNETTE{ u8"\ue435" };
	constexpr auto VISIBILITY{ u8"\ue8f4" };
	constexpr auto VISIBILITY_OFF{ u8"\ue8f5" };
	constexpr auto VOICE_CHAT{ u8"\ue62e" };
	constexpr auto VOICEMAIL{ u8"\ue0d9" };
	constexpr auto VOLUME_DOWN{ u8"\ue04d" };
	constexpr auto VOLUME_MUTE{ u8"\ue04e" };
	constexpr auto VOLUME_OFF{ u8"\ue04f" };
	constexpr auto VOLUME_UP{ u8"\ue050" };
	constexpr auto VPN_KEY{ u8"\ue0da" };
	constexpr auto VPN_LOCK{ u8"\ue62f" };
	constexpr auto WALLPAPER{ u8"\ue1bc" };
	constexpr auto WARNING{ u8"\ue002" };
	constexpr auto WATCH{ u8"\ue334" };
	constexpr auto WATCH_LATER{ u8"\ue924" };
	constexpr auto WB_AUTO{ u8"\ue42c" };
	constexpr auto WB_CLOUDY{ u8"\ue42d" };
	constexpr auto WB_INCANDESCENT{ u8"\ue42e" };
	constexpr auto WB_IRIDESCENT{ u8"\ue436" };
	constexpr auto WB_SUNNY{ u8"\ue430" };
	constexpr auto WC{ u8"\ue63d" };
	constexpr auto WEB{ u8"\ue051" };
	constexpr auto WEB_ASSET{ u8"\ue069" };
	constexpr auto WEEKEND{ u8"\ue16b" };
	constexpr auto WHATSHOT{ u8"\ue80e" };
	constexpr auto WIDGETS{ u8"\ue1bd" };
	constexpr auto WIFI{ u8"\ue63e" };
	constexpr auto WIFI_LOCK{ u8"\ue1e1" };
	constexpr auto WIFI_TETHERING{ u8"\ue1e2" };
	constexpr auto WORK{ u8"\ue8f9" };
	constexpr auto WRAP_TEXT{ u8"\ue25b" };
	constexpr auto YOUTUBE_SEARCHED_FOR{ u8"\ue8fa" };
	constexpr auto ZOOM_IN{ u8"\ue8ff" };
	constexpr auto ZOOM_OUT{ u8"\ue900" };
	constexpr auto ZOOM_OUT_MAP{ u8"\ue56b" };
}
