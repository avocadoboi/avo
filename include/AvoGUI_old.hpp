/*
MIT License

Copyright (c) 2020 Björn Sundin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

//------------------------------

#include <cmath>
#include <cstdint> // Fixed-size integer typedefs
#include <cstring>

// I/O
#include <fstream>
#include <iostream>

// Data structures
#include <array>
#include <deque>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Utilities
#include <functional>
#include <optional>

// Algorithms
#include <algorithm>
#include <charconv>
#include <numeric>
#include <ranges>

// Threading
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

// Debugging
#include <cassert>

//------------------------------

namespace Avo {

template<typename T>
concept IsRatio = requires (T ratio) { std::ratio{ratio}; };

/*
	Evaluates to true if T meets the "Mutex" named requirements.
	https://en.cppreference.com/w/cpp/named_req/Mutex
*/
template<typename T>
concept IsMutex = requires (T mutex) {
	requires !std::copyable<T> && !std::movable<T>;
    requires std::default_initializable<T>;
    requires std::destructible<T>;
    { mutex.lock() } -> std::same_as<void>;
    { mutex.unlock() } -> std::same_as<void>;
    { mutex.try_lock() } -> std::same_as<bool>;
};

template<typename _Invocable, typename _Return, typename ... _Args>
concept IsInvocableWithReturn = std::is_invocable_r_v<_Return, _Invocable, _Args...>;

//------------------------------

// TODO: add String class with built in UTF-8 support and more operations, ICU library used if available for toLowercase/toUppercase support.
// TODO: add Character class for UTF-8 characters with operators and stuff

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
[[nodiscard]] auto stringToNumber(std::string_view const string) -> std::optional<T>
{
	auto value = T{};
	if (auto error = std::from_chars(string.data(), string.data() + string.size(), value).ec;
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
template<int precision = 6, IsNumber T>
[[nodiscard]] auto number_to_string(std::span<char8> buffer, T number) -> std::string_view
{
	if (!number) {
		return "0";
	}

	constexpr auto max_number_of_integer_digits = 39;
	if constexpr (std::integral<T>) {
		auto position = buffer.end();
		
		auto const write_the_digits = [&]{
			while (number) {
				*--position = '0' + number % 10;
				number /= 10;
			}
		};

		if constexpr (std::is_signed_v<T>) {
			auto const is_negative = number < 0;
			number = std::abs(number);

			write_the_digits();

			if (is_negative) {
				*--position = '-';
			}
		}
		else {
			write_the_digits();
		}

		return {position, static_cast<std::string_view::size_type>(buffer.end() - position)};
	}
	else {
		auto position = buffer.begin() + max_number_of_integer_digits;

		auto const is_negative = number < 0;
		auto decimal_part = static_cast<long double>(std::abs(number));

		long double integer_part_double;
		decimal_part = std::modf(decimal_part, &integer_part_double);

		auto integer_part_int = static_cast<int64>(integer_part_double);
		do {
			*--position = '0' + integer_part_int % 10;
			integer_part_int /= 10;
		} while (integer_part_int);

		auto start_position = position;
		if (is_negative) {
			*--start_position = '-';
		}

		*(position = buffer.begin() + max_number_of_integer_digits) = '.';

		auto const last = buffer.end() - 1;
		while (position != last) {
			decimal_part = std::modf(decimal_part*10.l, &integer_part_double);
			*++position = '0' + static_cast<int64>(integer_part_double) % 10;
		}

		if (*position == '9') {
			while (*--position == '9') {}
			if (*position == '.') {
				--position;
			}
			++*position;
		}
		else {
			while (*--position == '0') {}
			if (*position == '.') {
				--position;
			}
			else {
				*position += *position != '9' && *(position + 1) >= '5';
			}
		}
		return std::string_view{start_position, static_cast<std::string_view::size_type>(position + 1 - start_position)};
	}
}
/*
	Converts a number to a string, using . (dot) for the decimal point.
	It's about 8x faster than using std::ostringstream for floating point numbers
	and 10x faster for integers.
*/
template<int precision = 5, IsNumber T>
[[nodiscard]] auto number_to_string(T const number) -> std::string
{
	constexpr auto max_number_of_integer_digits = 39;
	auto buffer = std::array<char8, max_number_of_integer_digits + 1 + (std::floating_point<T> ? precision + 1 : 0)>();
	return std::string{number_to_string<precision>(buffer, number)};
}

/*
	Converts a number rounded at a certain digit to a string, using . (dot) for the decimal point.
	If p_roundingIndex is 0, only all decimals are rounded off and it becomes an integer.
	Positive goes to the right and negative goes to the left.
*/
template<typename T>
[[nodiscard]] auto number_to_string(
	T const value, 
	Index const p_roundingIndex, 
	RoundingType const p_roundingType = RoundingType::Nearest
) -> std::string
{
	auto const roundingFactor = std::pow(10., p_roundingIndex);
	if (p_roundingType == RoundingType::Nearest)
	{
		return number_to_string(std::round(value*roundingFactor)/roundingFactor);
	}
	else if (p_roundingType == RoundingType::Down)
	{
		return number_to_string(std::floor(value*roundingFactor)/roundingFactor);
	}
	else
	{
		return number_to_string(std::ceil(value*roundingFactor)/roundingFactor);
	}
}

[[nodiscard]] 
inline auto to_string(IsNumber auto const number) -> std::string {
	return number_to_string(number);
}

[[nodiscard]] 
inline auto to_string(char8 const p_character) -> std::string {
	return std::string{1, p_character};
}
[[nodiscard]] 
inline auto to_string(std::string const& string) -> std::string const& {
	return string;
}
[[nodiscard]] 
inline auto to_string(std::string&& string) -> std::string&& {
	return std::move(string);
}
[[nodiscard]] 
inline auto to_string(std::string_view const string) -> std::string {
	return std::string{string};
}
template<std::size_t size>
[[nodiscard]] 
auto to_string(char8 const (&p_array)[size]) -> std::string {
	return p_array;
}

[[nodiscard]] 
inline auto to_string(char16 const p_character) -> std::string {
	return convert_utf16_to_utf8({&p_character, 1});
}
[[nodiscard]] 
inline auto to_string(std::u16string const& string) -> std::string {
	return convert_utf16_to_utf8(string);
}
[[nodiscard]] 
inline auto to_string(std::u16string_view string) -> std::string {
	return convert_utf16_to_utf8(string);
}
template<std::size_t size>
[[nodiscard]]
auto to_string(char16 const (&p_array)[size]) -> std::string {
	return convert_utf16_to_utf8(p_array);
}

[[nodiscard]] 
inline auto to_string(wchar_t const p_character) -> std::string {
	static_assert(
		sizeof(wchar_t) == sizeof(char16), 
		"A wchar_t character can only be converted to a std::string if the size of wchar_t" 
		"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
		"AvoGUI does not support the UTF-32 encoding."
	);

	return convert_utf16_to_utf8({reinterpret_cast<char16 const*>(&p_character), 1});
}
[[nodiscard]] 
inline auto to_string(std::wstring const& string) -> std::string {
	static_assert(
		sizeof(wchar_t) == sizeof(char16), 
		"A std::wstring can only be converted to a std::string if the size of wchar_t" 
		"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
		"AvoGUI does not support the UTF-32 encoding."
	);
	return convert_utf16_to_utf8({reinterpret_cast<char16 const*>(string.data()), string.size()});
}
[[nodiscard]] 
inline auto to_string(std::wstring_view const& string) -> std::string {
	static_assert(
		sizeof(wchar_t) == sizeof(char16), 
		"A std::wstring_view can only be converted to a std::string if the size of wchar_t" 
		"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
		"AvoGUI does not support the UTF-32 encoding."
	);
	return convert_utf16_to_utf8({reinterpret_cast<char16 const*>(string.data()), string.size()});
}
template<std::size_t size>
[[nodiscard]] auto to_string(wchar_t const (&p_array)[size]) -> std::string {
	static_assert(
		sizeof(wchar_t) == sizeof(char16), 
		"A wchar_t string literal can only be converted to a std::string if the size of wchar_t" 
		"is the same as char16. It is then assumed to be encoded in UTF-16, as on Windows."
		"AvoGUI does not support the UTF-32 encoding."
	);
	return convert_utf16_to_utf8({reinterpret_cast<char16 const (&)[size]>(p_array)});
}

/*
	Function to format a string by replacing placeholders in format_string with objects.
	objects can be any objects which Avo::to_string can be called on.

	The placeholders are in the form of {index} where index is the index of the argument to be inserted.
	Writing {0} will insert the first item, {1} will insert the second item.
	The index can also be omitted, writing only {}. In that case the next object will be inserted.
*/
template<typename ... FormattableType>
[[nodiscard]] 
auto format_string(std::string_view const format_string, FormattableType&& ... objects)
	-> std::string
{
	auto const object_strings = std::array{to_string(std::forward<FormattableType>(objects))...};
	auto result = std::string();
	
	auto position = Index{};
	auto objectIndex = Index{-1};
	while (true) {
		/*
			For every iteration we will first append a slice of 
			text from format_string onto the result, then an object string.
		*/
		
		// This is the position of the first text character in format_string to be appended.
		auto const format_slice_start = position;

		// Find where to insert the next object

		auto const open_brace_position = format_string.find('{', format_slice_start);
		if (open_brace_position == std::string_view::npos) {
			// We're done with formatting.
			break;
		}
		
		auto const close_brace_position = format_string.find('}', open_brace_position + 1);
		if (close_brace_position == std::string_view::npos) {
			// We're done with formatting.
			break;
		}
		
		// Choose the object string to append from what is between the '{' and the '}'.
		// There are some different possible cases.

		auto objectString = static_cast<std::string const*>(nullptr);
		if (close_brace_position == open_brace_position + 1) {
			// There was a {} sequence. Just insert the next object.
			if (++objectIndex == object_strings.size()) {
				// We're done with formatting.
				break;
			}
			else {
				objectString = &object_strings[objectIndex];
			}
		}
		else if (format_string[open_brace_position + 1] == '{') {
			// Two { were after each other. 
			// Insert single { and start the next iteration after the {{.
			result += format_string.substr(format_slice_start, open_brace_position + 1 - format_slice_start);
			position = open_brace_position + 2; // Here we put the next position after the {{
			continue;
		}
		else {
			if (std::optional<Index> const index = stringToNumber<Index>(
					format_string.substr(
						open_brace_position + 1, 
						close_brace_position - open_brace_position - 1
					)
				))
			{
				objectString = &object_strings[*index];
				objectIndex = *index;
			}
			else {
				// There was no valid index between '{' and '}'.
				// Just write the slice including the { and continue after the { in the next iteration.
				position = open_brace_position + 1;
				result += format_string.substr(format_slice_start, position - format_slice_start);
				continue;
			}
		}
		
		// Append both the text slice and the object string.
		(result += format_string.substr(format_slice_start, open_brace_position - format_slice_start)) += *objectString;

		// Will become the start of the next text slice.
		position = close_brace_position + 1;
	}
	if (position < format_string.size() - 1) {
		result += format_string.substr(position);
	}
	return result;
}

//------------------------------

template<typename T>
concept IsTimerThreadCallback = IsInvocableWithReturn<T, void>;

/*
	A TimerThread is used for timer callbacks. 
	The first time a callback is added, it spawns a thread that sleeps until the next callback should be called.
*/
template<IsMutex _Mutex = std::mutex>
class TimerThread {
	using _Clock = chrono::steady_clock;

	class Timeout {
	private:
		std::function<void()> _callback;
	public:
		auto operator()() const -> void {
			_callback();
		}
		
		_Clock::time_point end_time;
		Id id = 0;

		template<IsTimerThreadCallback _Callback, IsNumber _DurationType, IsRatio _DurationPeriod>
		Timeout(_Callback const& callback, chrono::duration<_DurationType, _DurationPeriod> const duration, Id const p_id) :
			_callback{callback},
			end_time{_Clock::now() + chrono::duration_cast<_Clock::duration>(duration)},
			id{p_id}
		{}
	};

	std::vector<Timeout> _timeouts;

	// Protects the _timeouts vector because it is modified in different threads
	std::mutex _timeouts_mutex;

	// This is a pointer to a user-provided mutex that should be locked during all timeout callbacks.
	_Mutex* _callback_mutex = nullptr;

	// The timeouts have their own IDs and this is used to generate new ones
	std::atomic<Count> _id_counter = 1;

	//------------------------------

	auto wait_for_new_timeout() -> void {
		_id_counter = 0;
		if (!_needs_to_wake) {
			auto const lock = std::unique_lock{_wakeMutex};
			_wake_condition_variable.wait(lock, [&] { return static_cast<bool>(_needs_to_wake); });
		}
		_needs_to_wake = false;
	}
	auto wait_for_timeout_to_end() -> void {
		if (!_needs_to_wake) {
			auto const lock = std::unique_lock{_wakeMutex};
			_wake_condition_variable.wait_until(lock, _timeouts.begin()->end_time, [&] { return static_cast<bool>(_needs_to_wake); });
		}
		_needs_to_wake = false;
	}
	auto notify_ended_timeouts() -> void {
		auto const nextTimeout = std::find_if(
			_timeouts.begin(), _timeouts.end(), 
			[&](Timeout& timeout) {
				if (timeout.end_time >= _Clock::now()) {
					return true;
				}
				if (_callback_mutex) {
					auto const lock = std::scoped_lock{*_callback_mutex};
					timeout();
				}
				else {
					timeout();
				}
				return false;
			}
		);
		auto const lock = std::scoped_lock{_timeouts_mutex};
		_timeouts.erase(_timeouts.begin(), nextTimeout);
	}

	std::atomic<bool> _is_running = false;
	std::thread _thread;

	auto thread_run() -> void {
		_is_running = true;
		while (_is_running)  {
			if (_timeouts.empty()) {
				wait_for_new_timeout();
			}
			else {
				wait_for_timeout_to_end();
				
				if (_timeouts.empty()) {
					continue;
				}

				notify_ended_timeouts();
			}
		}
	}

	//------------------------------

	std::atomic<bool> _needs_to_wake = false;
	std::condition_variable _wake_condition_variable;
	std::mutex _wakeMutex;

	auto wake() -> void {
		if (!_needs_to_wake) {
			_wakeMutex.lock();
			_needs_to_wake = true;
			_wakeMutex.unlock();
			_wake_condition_variable.notify_one();
		}
	}

	//------------------------------

	auto run() -> void {
		_thread = std::thread{&TimerThread::thread_run, this};
		_is_running = true;
	}

public:
	/*
		Adds a function that will be called in duration from now.
	*/
	template<IsTimerThreadCallback _Callback, IsNumber _DurationType, IsRatio _DurationPeriod>
	auto add_callback(_Callback const& callback, chrono::duration<_DurationType, _DurationPeriod> const duration) 
		-> Id
	{
		if (!_is_running) {
			run();
		}

		auto timeout = Timeout{callback, duration, _id_counter++};
		auto const id = timeout.id;
		{
			auto const lock = std::scoped_lock{_timeouts_mutex};
			// Find the right position to keep the vector sorted.
			auto const position = std::ranges::lower_bound(
				_timeouts, timeout, 
				[](Timeout const& p_a, Timeout const& p_b) {
					return p_a.end_time < p_b.end_time; 
				}
			);
			_timeouts.insert(position, std::move(timeout));
		}
		wake();
		return id;
	}
	template<typename _Callback>
	auto add_callback(_Callback const& callback, float const milliseconds)
		-> Id
	{
		return add_callback(callback, chrono::duration<float, std::milli>{milliseconds});
	}

	auto cancel_callback(Id id) -> void {
		auto const lock = std::scoped_lock{_timeouts_mutex};

		auto const position = std::ranges::find_if(
			_timeouts, 
			[=](Timeout const& p_timeout) {
				return p_timeout.id == id; 
			}
		);
		if (position != _timeouts.end()) {
			_timeouts.erase(position);
		}
	}

	TimerThread() = default;
	/*
		p_callbackMutex is a mutex that is locked every time a timer callback is called.
	*/
	TimerThread(_Mutex& p_callbackMutex) :
		_callback_mutex{&p_callbackMutex}
	{}
	~TimerThread() {
		if (_is_running) {
			_is_running = false;
			wake();
			_thread.join();
		}
	}
};

//------------------------------

enum class RectangleCornerType {
	Round,
	Cut
};
struct RectangleCorners {
	RectangleCornerType top_left_type;
	RectangleCornerType top_right_type;
	RectangleCornerType bottom_left_type;
	RectangleCornerType bottom_right_type;

	Dip top_left_size_x;
	Dip top_left_size_y;

	Dip top_right_size_x;
	Dip top_right_size_y;

	Dip bottom_left_size_x;
	Dip bottom_left_size_y;

	Dip bottom_right_size_x;
	Dip bottom_right_size_y;

	RectangleCorners() :
		top_left_size_x{0.f}, top_left_size_y{0.f}, top_right_size_x{0.f}, top_right_size_y{0.f},
		bottom_left_size_x{0.f}, bottom_left_size_y{0.f}, bottom_right_size_x{0.f}, bottom_right_size_y{0.f},
		top_left_type{RectangleCornerType::Round}, top_right_type{RectangleCornerType::Round},
		bottom_left_type{RectangleCornerType::Round}, bottom_right_type{RectangleCornerType::Round}
	{}
	explicit RectangleCorners(Dip const corner_size, RectangleCornerType const corner_type = RectangleCornerType::Round) :
		top_left_size_x{corner_size}, top_left_size_y{corner_size},
		top_right_size_x{corner_size}, top_right_size_y{corner_size},
		bottom_left_size_x{corner_size}, bottom_left_size_y{corner_size},
		bottom_right_size_x{corner_size}, bottom_right_size_y{corner_size},
		top_left_type{corner_type}, top_right_type{corner_type},
		bottom_left_type{corner_type}, bottom_right_type{corner_type}
	{
	}
	RectangleCorners(Dip const corner_size_x, Dip const corner_size_y, RectangleCornerType const corner_type = RectangleCornerType::Cut) :
		top_left_size_x{corner_size_x}, top_left_size_y{corner_size_y},
		top_right_size_x{corner_size_x}, top_right_size_y{corner_size_y},
		bottom_left_size_x{corner_size_x}, bottom_left_size_y{corner_size_y},
		bottom_right_size_x{corner_size_x}, bottom_right_size_y{corner_size_y},
		top_left_type{corner_type}, top_right_type{corner_type},
		bottom_left_type{corner_type}, bottom_right_type{corner_type}
	{
	}

	RectangleCorners(Dip const top_left_size, Dip const top_right_size, 
			Dip const bottom_left_size, Dip const bottom_right_size, 
			RectangleCornerType const corner_type = RectangleCornerType::Round) :
		top_left_size_x{top_left_size}, top_left_size_y{top_left_size},
		top_right_size_x{top_right_size}, top_right_size_y{top_right_size},
		bottom_left_size_x{bottom_left_size}, bottom_left_size_y{bottom_left_size},
		bottom_right_size_x{bottom_right_size}, bottom_right_size_y{bottom_right_size},
		top_left_type{corner_type}, top_right_type{corner_type},
		bottom_left_type{corner_type}, bottom_right_type{corner_type}
	{
	}
};

//------------------------------

class View;

template<typename T>
concept IsAnimationCallback = IsInvocableWithReturn<T, void, float>;

/*
	Class used for making animations.
	Preferrablý use the constructor directly, but there is also View::add_animation 
	methods to dynamically create animations that have the same lifetime as a view.
*/
class Animation {
	friend class Gui;

private:
	using _Clock = chrono::steady_clock;
	using _Duration = chrono::duration<float, std::milli>;

	_Duration _duration;
public:
	auto set_duration(float const milliseconds) -> void {
		_duration = _Duration{milliseconds};
	}
	/*
		Returns the duration of the animation in milliseconds.
	*/
	auto get_duration() noexcept -> float {
		return _duration.count();
	}
	/*
		Sets the duration of the animation in any type from the standard chrono library.
		Examples:
			animation.set_duration(1min/5); // Minutes
			animation.set_duration(2s); // Seconds
			animation.set_duration(400ms); // Milliseconds
	*/
	template<IsNumber _DurationType, IsRatio _DurationPeriod>
	auto set_duration(chrono::duration<_DurationType, _DurationPeriod> const duration) -> void {
		_duration = chrono::duration_cast<_Duration>(duration);
	}
	/*
		Returns the duration of the animation in any duration type from the standard chrono library.
		Example:
			auto seconds = animation.get_duration<std::chrono::seconds>();
	*/
	template<typename _DurationType>
	auto get_duration() -> _DurationType {
		return chrono::duration_cast<_DurationType>(_duration);
	}

private:
	bool _is_reversed = false;
public:
	auto set_is_reversed(bool const is_reversed) -> void {
		if (is_reversed != _is_reversed) {
			auto const value = _easing.ease_value(chrono::duration_cast<_Duration>(_Clock::now() - _start_time)/_duration, _easing_precision);
			_start_time = _Clock::now() - chrono::duration_cast<_Clock::duration>(_easing.easeValueInverse(1.f - value)*_duration);
			_is_reversed = is_reversed;
		}
	}
	auto get_is_reversed() noexcept -> bool {
		return _is_reversed;
	}

private:
	bool _is_done = true;
public:
	auto get_is_done() noexcept -> bool {
		return _is_done;
	}

private:
	Easing _easing;
public:
	auto set_easing(Easing const easing) noexcept -> void {
		_easing = easing;
	}
	auto get_easing() noexcept -> Easing {
		return _easing;
	}

private:
	float _easing_precision = 0.005f;
public:
	auto set_easing_precision(float const easing_precision) -> void {
		_easing_precision = easing_precision;
	}
	auto get_easing_precision() -> float {
		return _easing_precision;
	}

private:
	Gui* _gui = nullptr;
	bool _is_in_update_queue = false;
	auto queue_update() -> void;

	bool _are_updates_cancelled = false;
	auto update() -> void {
		if (_are_updates_cancelled) {
			_is_in_update_queue = false;
			return;
		}
		auto value = _easing.ease_value(chrono::duration_cast<_Duration>(_Clock::now() - _start_time)/_duration, _easing_precision);
		if (value >= 1.f) {
			_is_done = true;
			value = 1.f;
		}
		if (_is_reversed) {
			value = 1.f - value;
		}

		update_listeners(value);

		_is_in_update_queue = false;
		if (!_is_done) {
			queue_update();
		}
	}
	auto cancel_all_updates() -> void;

public:
	/*
		Listener signature:
			void(float value)
		value is between 0 and 1.
		At first the animation goes forward, but if you call reverse() the direction is switched.
	*/
	EventListeners<void(float)> update_listeners;

private:
	bool _is_paused = false;
	_Clock::time_point _start_time;
	_Clock::time_point _pauseTime;
public:
	auto play(bool const is_reversed) -> void {
		set_is_reversed(is_reversed);
		if (_is_paused) {
			_start_time += _Clock::now() - _pauseTime;
		}
		else if (_is_done) {
			_start_time = _Clock::now();
		}
		else return;
		_is_done = false;
		queue_update();
	}
	auto play() -> void {
		play(_is_reversed);
	}
	/*
		If the animation is reversed then the animation value will start at 1 if start_progress is 0.
	*/
	auto play(float const start_progress) -> void {
		_is_done = false;
		if (_is_reversed) {
			_start_time = _Clock::now() - chrono::duration_cast<_Clock::duration>((1.f - start_progress)*_duration);
		}
		else {
			_start_time = _Clock::now() - chrono::duration_cast<_Clock::duration>(start_progress*_duration);
		}
	}
	auto pause() -> void {
		_is_paused = true;
		_is_done = true;
	}
	auto stop() -> void {
		_is_paused = false;
		_is_done = true;
	}
	auto replay() -> void {
		stop();
		play();
	}

	//------------------------------

	Animation(Gui* const gui, Easing const easing, float const milliseconds) :
		_gui{gui},
		_easing{easing},
		_duration{milliseconds}
	{}
	Animation(Gui* const gui, Easing const easing, float const milliseconds, IsAnimationCallback auto const& callback) :
		_gui{gui},
		_easing{easing},
		_duration{milliseconds}
	{
		update_listeners += callback;
	}
	template<IsNumber _DurationType, IsRatio _DurationPeriod>
	Animation(Gui* gui, Easing easing, chrono::duration<_DurationType, _DurationPeriod> const& duration) :
		Animation{gui, easing, chrono::duration_cast<_Duration>(duration).count()}
	{}
	template<IsNumber _DurationType, IsRatio _DurationPeriod>
	Animation(Gui* gui, Easing easing, chrono::duration<_DurationType, _DurationPeriod> const& duration, IsAnimationCallback auto const& callback) :
		Animation{gui, easing, chrono::duration_cast<_Duration>(duration).count(), callback}
	{}

	auto operator=(Animation const&) -> Animation& = default;
	auto operator=(Animation&& other) noexcept -> Animation& = default;

	Animation() = default;
	Animation(Animation const&) = default;
	Animation(Animation&&) noexcept = default;
	~Animation() {
		cancel_all_updates();
	}
};

//------------------------------

/*
	Runs a callable on construction.
*/
struct Initializer {
	template<typename Lambda>
	Initializer(Lambda&& initializer) {
		std::forward<Lambda>(initializer)();
	}
};

//------------------------------

template<typename T>
class DrawingContextObject {
public:
	auto operator==(DrawingContextObject<T> const& other) const -> bool = default;

	auto get_is_valid() const -> bool {
		return _implementation;
	}
	operator bool() const {
		return _implementation.operator bool();
	}
	auto destroy() -> void {
		_implementation = nullptr;
	}

protected:
	std::shared_ptr<T> _implementation;
	DrawingContextObject(std::shared_ptr<T>&& p_implementation) :
		_implementation{std::move(p_implementation)}
	{}
	DrawingContextObject(std::shared_ptr<T> const& p_implementation) :
		_implementation{p_implementation}
	{}

public:
	auto get_implementation() const -> T* {
		return _implementation.get();
	}

	DrawingContextObject() = default;
};

//------------------------------

/*
	This specifies what is done to fit the image within its bounds.
*/
enum class ImageBoundsSizing {
	Stretch, // This stretches the image so that it fills its bounds.
	Contain, // This makes sure the image is as big as possible while still keeping the image within its bounds. Aspect ratio is kept.
	Fill, // This makes sure the image is so big that it fills its bounds while keeping aspect ratio. Edges may be clipped.
	Unknown = -1
};

/*
	This specifies how the pixels of an image are interpolated when it is scaled.
*/
enum class ImageScalingMethod {
	Pixelated, // Uses nearest neighbor interpolation
	Smooth, // Uses linear interpolation
	Unknown = -1
};

/*
	The formats that an image can be encoded to and decoded from.
*/
enum class ImageFormat {
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
		Image image_0 = get_drawing_context()->create_image("image.png");
		Image image_1 = image_0;
	image_1 and image_0 are referring to the same image, and the internal image object
	is released once all references have been destroyed.
*/
class Image : public ProtectedRectangle, public DrawingContextObject<Image> {
public:
	/*
		Sets a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
		This is in original image DIP coordinates, meaning sizing is not taken into account.
	*/
	virtual auto set_crop_rectangle(Rectangle<> rectangle) -> void {
		if (_implementation) {
			_implementation->set_crop_rectangle(rectangle);
		}
	}
	/*
		Returns a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
		This is in original image DIP coordinates, meaning sizing is not taken into account.
	*/
	virtual auto get_crop_rectangle() const -> Rectangle<> {
		if (_implementation) {
			return _implementation->get_crop_rectangle();
		}
		return {};
	}

	/*
		Returns the DIP size of the actual image.
	*/
	virtual auto get_original_size() const -> Size<> {
		if (_implementation) {
			return _implementation->get_original_size();
		}
		return {};
	}
	/*
		Returns the DIP width of the actual image.
	*/
	virtual auto get_original_width() const -> Dip {
		if (_implementation) {
			return _implementation->get_original_width();
		}
		return 0;
	}
	/*
		Returns the DIP height of the actual image.
	*/
	virtual auto get_original_height() const -> Dip {
		if (_implementation) {
			return _implementation->get_original_height();
		}
		return 0;
	}

	virtual auto get_original_pixel_size() const -> Size<Pixels> {
		if (_implementation) {
			return _implementation->get_original_pixel_size();
		}
		return {};
	}
	virtual auto get_original_pixel_width() const -> Pixels {
		if (_implementation) {
			return _implementation->get_original_pixel_width();
		}
		return {};
	}
	virtual auto get_original_pixel_height() const -> Pixels {
		if (_implementation) {
			return _implementation->get_original_pixel_height();
		}
		return {};
	}

	//------------------------------

	/*
		Sets the way the image is fit within its bounds.
	*/
	virtual auto set_bounds_sizing(ImageBoundsSizing const size_mode) -> void {
		if (_implementation) {
			_implementation->set_bounds_sizing(size_mode);
		}
	}
	/*
		Returns the way the image is fit within its bounds.
	*/
	virtual auto get_bounds_sizing() const -> ImageBoundsSizing {
		if (_implementation) {
			return _implementation->get_bounds_sizing();
		}
		return ImageBoundsSizing::Unknown;
	}

	/*
		Sets the way the image is positioned within its bounds.

		factor represents the coordinates of the point on the image that aligns with the same point but relative to the bounds.
		It is expressed as a factor of the size of the image. For example, if factor is (1, 1), the bottom right corner of the image will be
		aligned with the bottom right corner of the bounds. 0.5 means the centers will be aligned.
	*/
	virtual auto set_bounds_positioning(Point<Factor> factor) -> void {
		if (_implementation) {
			_implementation->set_bounds_positioning(factor);
		}
	}
	/*
		Sets the way the image is positioned within its bounds on the x-axis.

		x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds.
		x is expressed as a factor of the width of the image. For example, if x is 1, the right edge of the image will be
		aligned with the right edge of the bounds. 0.5 means the centers will be aligned.
	*/
	virtual auto set_bounds_positioning_x(Factor x) -> void {
		if (_implementation) {
			_implementation->set_bounds_positioning_x(x);
		}
	}
	/*
		Sets the way the image is positioned within its bounds on the y-axis.

		y represents the y coordinate of the point on the image that aligns with the same point but relative to the bounds.
		y is expressed as a factor of the height of the image. For example, if y is 1, the bottom edge of the image will be
		aligned with the bottom edge of the bounds. 0.5 means the centers will be aligned.
	*/
	virtual auto setBoundsPositioningY(Factor y) -> void {
		if (_implementation) {
			_implementation->setBoundsPositioningY(y);
		}
	}
	/*
		Returns the way the image is positioned within its bounds. See set_bounds_positioning for more info.
	*/
	virtual auto getBoundsPositioning() const -> Point<Factor> {
		if (_implementation) {
			return _implementation->getBoundsPositioning();
		}
		return {};
	}
	/*
		Returns the way the image is positioned within its bounds on the x-axis. See set_bounds_positioning_x for more info.
	*/
	virtual auto getBoundsPositioningX() const -> Factor {
		if (_implementation) {
			return _implementation->getBoundsPositioningX();
		}
		return 0.f;
	}
	/*
		Returns the way the image is positioned within its bounds on the y-axis. See setBoundsPositioningY for more info.
	*/
	virtual auto getBoundsPositioningY() const -> Factor {
		if (_implementation) {
			return _implementation->getBoundsPositioningY();
		}
		return 0.f;
	}

	//------------------------------

	/*
		Sets how the pixels of the image are interpolated when the image is scaled.
	*/
	virtual auto setScalingMethod(ImageScalingMethod p_scalingMethod) -> void {
		if (_implementation) {
			_implementation->setScalingMethod(p_scalingMethod);
		}
	}
	/*
		Returns how the pixels of the image are interpolated when the image is scaled.
	*/
	virtual auto getScalingMethod() const -> ImageScalingMethod {
		if (_implementation) {
			return _implementation->getScalingMethod();
		}
		return ImageScalingMethod::Unknown;
	}

	//------------------------------

	/*
		Sets how opaque the image is being drawn.
	*/
	virtual auto set_opacity(float opacity) -> void {
		if (_implementation) {
			_implementation->set_opacity(opacity);
		}
	}
	/*
		Returns how opaque the image is being drawn.
	*/
	virtual auto get_opacity() const -> float {
		if (_implementation) {
			return _implementation->get_opacity();
		}
		return 0;
	}

	//------------------------------

	/*
		Returns the drawn width of the image within the bounds, calculated using the sizing options and the crop rectangle.
	*/
	virtual auto get_inner_width() const -> Dip {
		if (_implementation) {
			return _implementation->get_inner_width();
		}
		return 0;
	}
	/*
		Returns the drawn height of the image within the bounds, calculated using the sizing options and the crop rectangle.
	*/
	virtual auto get_inner_height() const -> Dip {
		if (_implementation) {
			return _implementation->get_inner_height();
		}
		return 0;
	}
	/*
		Returns the drawn size of the image within the bounds, calculated using the sizing options and the crop rectangle.
	*/
	virtual auto get_inner_size() const -> Point<> {
		if (_implementation) {
			return _implementation->get_inner_size();
		}
		return {};
	}
	/*
		Returns the drawn inner bounds of the image within the outer bounds, calculated using the positioning options, sizing options and the crop rectangle.
	*/
	virtual auto get_inner_bounds() const -> Rectangle<> {
		if (_implementation) {
			return _implementation->get_inner_bounds();
		}
		return {};
	}

	//------------------------------

protected:
	auto handle_protected_rectangle_change(Rectangle<> p_old) -> void override {
		if (_implementation) {
			_implementation->set_bounds(_bounds);
		}
	}

private:
	void initialize_bounds_from_implementation() {
		if (_implementation) {
			_bounds = _implementation->_bounds;
		}
	}
public:
	auto operator=(Image&& image) -> Image& {
		_implementation = std::move(image._implementation);
		initialize_bounds_from_implementation();
		return *this;
	}
	auto operator=(Image const& image) -> Image& {
		_implementation = image._implementation;
		initialize_bounds_from_implementation();
	}

protected:
	friend class DrawingContext;
	Image(std::shared_ptr<Image>&& p_implementation) :
		DrawingContextObject{std::move(p_implementation)}
	{
		initialize_bounds_from_implementation();
	}
	Image(std::shared_ptr<Image> const& p_implementation) :
		DrawingContextObject{p_implementation}
	{
		initialize_bounds_from_implementation();
	}
public:
	Image() = default;
	~Image() = default;
	Image(Image&& image) :
		DrawingContextObject{std::move(image._implementation)}
	{
		initialize_bounds_from_implementation();
	}
	Image(Image const& image) :
		DrawingContextObject{image._implementation}
	{
		initialize_bounds_from_implementation();
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
		Text text_0 = get_drawing_context()->create_text("some text", 12.f);
		Text text_1 = text_0;
	text_1 and text_0 are referring to the same text, and the internal text object
	is released once all references have been destroyed.
*/
class Text : public ProtectedRectangle, public DrawingContextObject<Text> {
public:
	/*
		Sets the rules for inserting line breaks in the text to avoid overflow.
	*/
	virtual auto set_word_wrapping(WordWrapping p_wordWrapping) -> void {
		if (_implementation) {
			_implementation->set_word_wrapping(p_wordWrapping);
		}
	}
	/*
		Returns the type of rules used for inserting line breaks in the text to avoid overflow.
	*/
	virtual auto get_word_wrapping() const -> WordWrapping {
		if (_implementation) {
			return _implementation->get_word_wrapping();
		}
		return WordWrapping::Unknown;
	}

	/*
		Sets the size of the bounding box to fit the text.
		There may still be space between the tallest character in the text and the top edge of the bounds.
		If you want the text to be positioned without any space on the top, call set_is_top_trimmed(true) before this.
	*/
	virtual auto fit_size_to_text() -> void {
		if (_implementation) {
			_implementation->fit_size_to_text();
		}
	}
	/*
		Sets the width of the bounding box to fit the text.
	*/
	virtual auto fit_width_to_text() -> void {
		if (_implementation) {
			_implementation->fit_width_to_text();
		}
	}
	/*
		Sets the height of the bounding box to fit the text.
		There may still be space between the tallest character in the text and the top edge of the bounds.
		If you want the text to be positioned without any space on the top, call set_is_top_trimmed(true) before this.
	*/
	virtual auto fit_height_to_text() -> void {
		if (_implementation) {
			_implementation->fit_height_to_text();
		}
	}
	/*
		Returns the smallest size to contain the actual text.
		If get_is_top_trimmed() == false, the height includes the space between the top of the tallest character
		and the top edge of the bounds.
	*/
	virtual auto get_minimum_size() const -> Point<> {
		if (_implementation) {
			return _implementation->get_minimum_size();
		}
		return {};
	}
	/*
		Returns the smallest width to contain the actual text.
	*/
	virtual auto get_minimum_width() const -> Dip {
		if (_implementation) {
			return _implementation->get_minimum_width();
		}
		return {};
	}
	/*
		Returns the smallest height to contain the actual text.
		If get_is_top_trimmed() == false, this includes the space between the top of the tallest character
		and the top edge of the bounds.
	*/
	virtual auto get_minimum_height() const -> Dip {
		if (_implementation) {
			return _implementation->get_minimum_height();
		}
		return {};
	}

	//------------------------------

	/*
		Sets whether the top of the text is trimmed so that there is no space between the top of the tallest
		character of the text and the top edge of the bounds.

		If this is false, the text is positioned within the bounds so that the baseline is at a fixed position,
		and there may be space above the characters in the text to allow this. This is the default.

		Setting this to true can be useful when you want to perfectly center text vertically.
	*/
	virtual auto set_is_top_trimmed(bool const is_top_trimmed) -> void {
		if (_implementation) {
			_implementation->set_is_top_trimmed(is_top_trimmed);
		}
	}
	/*
		Returns whether the top of the text is trimmed so that there is no space between the top of the tallest
		character of the text and the top edge of the bounds. This is false by default.
	*/
	virtual auto get_is_top_trimmed() const -> bool {
		if (_implementation) {
			return _implementation->get_is_top_trimmed();
		}
		return false;
	}

	//------------------------------

	/*
		Returns the 2d position of a character in the text, specified by its index in the string.
		is_relative_to_origin is whether the position returned is relative to the origin of the drawing context.
		If not, it is relative to the bounds of the text.
	*/
	virtual auto get_character_position(Index const character_index, bool const is_relative_to_origin = false) const -> Point<> {
		if (_implementation) {
			return _implementation->get_character_position(character_index, is_relative_to_origin);
		}
		return {};
	}
	/*
		Returns the width and height of a character in the text, specified by its index in the string.
	*/
	virtual auto get_character_size(Index const character_index) const -> Point<> {
		if (_implementation) {
			return _implementation->get_character_size(character_index);
		}
		return {};
	}
	/*
		Returns a rectangle enclosing a character in the text, specified by its index in the string.
		is_relative_to_origin is whether the position of the bounds returned is relative to the origin of the drawing context.
		If not, it is relative to the bounds of the text.
	*/
	virtual auto get_character_bounds(Index const character_index, bool const is_relative_to_origin = false) const -> Rectangle<> {
		if (_implementation) {
			return _implementation->get_character_bounds(character_index, is_relative_to_origin);
		}
		return {};
	}

	/*
		Returns the index of the character which is nearest to a point.
		is_relative_to_origin is whether the position given is relative to the origin of the drawing context.
		If not, it is relative to the bounds of the text.
	*/
	virtual auto get_nearest_character_index(Point<> p_point, bool is_relative_to_origin = false) const -> Index {
		if (_implementation) {
			return _implementation->get_nearest_character_index(p_point, is_relative_to_origin);
		}
		return {};
	}
	/*
		Returns the index and position of the character which is nearest to a point.

		is_relative_to_origin is whether the input and output points are relative to the origin of the drawing context.
		If not, they are relative to the bounds of the text.
	*/
	virtual auto get_nearest_character_index_and_position(Point<> p_point, bool is_relative_to_origin = false) const 
		-> std::pair<Index, Point<>>
	{
		if (_implementation) {
			return _implementation->get_nearest_character_index_and_position(p_point, is_relative_to_origin);
		}
		return {};
	}
	/*
		Returns the index and bounds of the character which is nearest to a point.

		is_relative_to_origin is whether the input and output points are relative to the origin of the drawing context. 
		If not, they are relative to the bounds of the text.
	*/
	virtual auto get_nearest_character_index_and_bounds(Point<> point, bool is_relative_to_origin = false) const 
		-> std::pair<Index, Rectangle<>>
	{
		if (_implementation) {
			return _implementation->get_nearest_character_index_and_bounds(point, is_relative_to_origin);
		}
		return {};
	}

	//------------------------------

	/*
		Sets how the text is placed within the bounds.
	*/
	virtual auto set_text_align(TextAlign text_align) -> void {
		if (_implementation) {
			_implementation->set_text_align(text_align);
		}
	}
	/*
		Returns how the text is placed within the bounds.
	*/
	virtual auto get_text_align() const -> TextAlign {
		if (_implementation) {
			return _implementation->get_text_align();
		}
		return TextAlign::Unknown;
	}

	//------------------------------

	/*
		Sets the layout direction of the text.
	*/
	virtual auto setReadingDirection(ReadingDirection p_readingDirection) -> void
	{
		if (_implementation)
		{
			_implementation->setReadingDirection(p_readingDirection);
		}
	}
	/*
		Returns the layout direction of the text.
	*/
	virtual auto getReadingDirection() const -> ReadingDirection
	{
		if (_implementation)
		{
			return _implementation->getReadingDirection();
		}
		return ReadingDirection::Unknown;
	}

	//------------------------------

	/*
		Represents a part of the text that a property-changing method will affect.
	*/
	struct TextRange {
		/*
			The position of the first character that a property is set on.
			If this is negative, it is relative to the end of the text.
		*/
		Index start_position{};
		/*
			The number of characters that a property is set on.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the start position is affected.
		*/
		Count length{};
	};

	//------------------------------

	/*
		Sets the font family to be used in a section of the text.

		name is the name of the font family.
	*/
	virtual auto set_font_family(std::string_view name, TextRange range = {}) -> void {
		if (_implementation) {
			_implementation->set_font_family(name, range);
		}
	}

	//------------------------------

	/*
		Sets the spacing between characters in a section of the text.
	*/
	auto set_character_spacing(float character_spacing, TextRange range = {}) -> void {
		set_character_spacing(character_spacing*0.5f, character_spacing*0.5f, range);
	}
	/*
		Sets the leading and trailing spacing of the characters in a section of the text.

		leading is the spacing before the characters of the text.
		trailing is the spacing after the characters of the text.
	*/
	virtual auto set_character_spacing(float leading, float trailing, TextRange range = {}) -> void {
		if (_implementation) {
			_implementation->set_character_spacing(leading, trailing, range);
		}
	}
	/*
		Returns the spacing before one of the characters.
	*/
	virtual auto get_leading_character_spacing(Index character_index = 0) const -> float
	{
		if (_implementation)
		{
			return _implementation->get_leading_character_spacing(character_index);
		}
		return 0.f;
	}
	/*
		Returns the spacing after one of the characters.
	*/
	virtual auto getTrailingCharacterSpacing(Index character_index = 0) const -> float
	{
		if (_implementation)
		{
			return _implementation->getTrailingCharacterSpacing(character_index);
		}
		return 0.f;
	}

	//------------------------------

	/*
		Sets the distance between the baseline of lines in the text, as a factor of the default.
	*/
	virtual auto setLineHeight(Factor p_lineHeight) -> void
	{
		if (_implementation)
		{
			_implementation->setLineHeight(p_lineHeight);
		}
	}
	/*
		Returns the distance between the baseline of lines in the text, as a factor of the default.
	*/
	virtual auto getLineHeight() const -> Factor
	{
		if (_implementation)
		{
			return _implementation->getLineHeight();
		}
		return 0.f;
	}

	//------------------------------

	/*
		Sets the thickness of characters in a section of the text.
	*/
	virtual auto set_font_weight(FontWeight p_fontWeight, TextRange range = {}) -> void
	{
		if (_implementation)
		{
			_implementation->set_font_weight(p_fontWeight, range);
		}
	}
	/*
		Returns the weight/thickness of a character in the text.
	*/
	virtual auto getFontWeight(Index p_characterPosition = 0) const -> FontWeight
	{
		if (_implementation)
		{
			return _implementation->getFontWeight(p_characterPosition);
		}
		return FontWeight::Unknown;
	}

	//------------------------------

	/*
		Sets the font style in a section of the text.
	*/
	virtual auto setFontStyle(FontStyle p_fontStyle, TextRange range = {}) -> void
	{
		if (_implementation)
		{
			_implementation->setFontStyle(p_fontStyle, range);
		}
	}
	/*
		Returns the style of a character in the text.
	*/
	virtual auto getFontStyle(Index p_characterPosition = 0) const -> FontStyle
	{
		if (_implementation)
		{
			return _implementation->getFontStyle(p_characterPosition);
		}
		return FontStyle::Unknown;
	}

	//------------------------------

	/*
		Sets the font stretch in a section of the text. Not all fonts support this.
	*/
	virtual auto setFontStretch(FontStretch p_fontStretch, TextRange range = {}) -> void
	{
		if (_implementation)
		{
			_implementation->setFontStretch(p_fontStretch, range);
		}
	}
	/*
		Returns the font stretch of a character in the text.
	*/
	virtual auto getFontStretch(Index p_characterPosition = 0) const -> FontStretch
	{
		if (_implementation)
		{
			return _implementation->getFontStretch(p_characterPosition);
		}
		return FontStretch::Unknown;
	}

	//------------------------------

	/*
		Sets the font size in a section of the text.
	*/
	virtual auto set_font_size(float font_size, TextRange range = {}) -> void {
		if (_implementation) {
			_implementation->set_font_size(font_size, range);
		}
	}
	/*
		Returns the size (height) of a character in the text.
	*/
	virtual auto get_font_size(Index p_characterPosition = 0) const -> float {
		if (_implementation) {
			return _implementation->get_font_size(p_characterPosition);
		}
		return {};
	}

	//------------------------------

	virtual auto get_string() const -> std::string_view {
		if (_implementation) {
			return _implementation->get_string();
		}
		return {};
	}

	//------------------------------

	using ProtectedRectangle::set_bounds;
	auto set_bounds(Rectangle<> rectangle) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_bounds(rectangle);
		}
	}
	auto get_bounds() const noexcept -> Rectangle<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_bounds();
		}
		return {};
	}

	using ProtectedRectangle::move;
	auto move(Vector2d<> offset) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::move(offset);
		}
	}
	auto move_x(Dip offset_x) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::move_x(offset_x);
		}
	}
	auto move_y(Dip p_offsetY) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::move_y(p_offsetY);
		}
	}

	using ProtectedRectangle::set_top_left;
	auto set_top_left(Point<> top_left, bool will_keep_size = true) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_top_left(top_left, will_keep_size);
		}
	}
	auto get_top_left() const noexcept -> Point<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_top_left();
		}
		return {};
	}

	using ProtectedRectangle::set_top_right;
	auto set_top_right(Point<> top_right, bool will_keep_size = true) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_top_right(top_right, will_keep_size);
		}
	}
	auto get_top_right() const noexcept -> Point<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_top_right();
		}
		return {};
	}

	using ProtectedRectangle::set_botto_left;
	auto set_bottom_left(Point<> p_bottomLeft, bool will_keep_size = true) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_bottom_left(p_bottomLeft, will_keep_size);
		}
	}
	auto get_bottom_left() const noexcept -> Point<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_bottom_left();
		}
		return {};
	}

	using ProtectedRectangle::set_bottom_right;
	auto set_bottom_right(Point<> p_bottomRight, bool will_keep_size = true) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_bottom_right(p_bottomRight, will_keep_size);
		}
	}
	auto get_bottom_right() const noexcept -> Point<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_bottom_right();
		}
		return {};
	}

	using ProtectedRectangle::set_center;
	auto set_center(Point<> circle) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_center(circle);
		}
	}
	auto get_center() const noexcept -> Point<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_center();
		}
		return {};
	}
	auto set_center_x(Dip x) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_center_x(x);
		}
	}
	auto get_center_x() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::get_center_x();
		}
		return 0.f;
	}
	auto set_center_y(Dip y) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_center_y(y);
		}
	}
	auto get_center_y() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::get_center_y();
		}
		return 0.f;
	}
	auto set_left(Dip left, bool will_keep_width = true) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_left(left, will_keep_width);
		}
	}
	auto getLeft() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::getLeft();
		}
		return 0.f;
	}
	auto set_top(Dip p_top, bool will_keep_height = true) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_top(p_top, will_keep_height);
		}
	}
	auto getTop() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::getTop();
		}
		return 0.f;
	}
	auto set_right(Dip right, bool will_keep_width = true) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_right(right, will_keep_width);
		}
	}
	auto getRight() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::getRight();
		}
		return 0.f;
	}
	auto set_bottom(Dip p_bottom, bool will_keep_height = true) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_bottom(p_bottom, will_keep_height);
		}
	}
	auto getBottom() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::getBottom();
		}
		return 0.f;
	}
	auto set_width(Dip width) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_width(width);
		}
	}
	auto get_width() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::get_width();
		}
		return 0.f;
	}
	auto set_height(Dip height) -> void override 
	{
		if (_implementation)
		{
			_implementation->ProtectedRectangle::set_height(height);
		}
	}
	auto get_height() const noexcept -> Dip override 
	{
		if (_implementation)
		{
			return _implementation->ProtectedRectangle::get_height();
		}
		return 0.f;
	}

	using ProtectedRectangle::set_size;
	auto set_size(Size<> size) -> void override {
		if (_implementation) {
			_implementation->ProtectedRectangle::set_size(size);
		}
	}
	auto get_size() const noexcept -> Size<> override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_size();
		}
		return {};
	}

	using ProtectedRectangle::get_is_intersecting;
	auto get_is_intersecting(Rectangle<> rectangle) const noexcept -> bool override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_is_intersecting(rectangle);
		}
		return false;
	}
	auto get_is_containing(Rectangle<> rectangle) const noexcept -> bool override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_is_containing(rectangle);
		}
		return false;
	}

	using ProtectedRectangle::get_is_containing;
	auto get_is_containing(Point<> point) const noexcept -> bool override {
		if (_implementation) {
			return _implementation->ProtectedRectangle::get_is_containing(p_point);
		}
		return false;
	}

	friend class DrawingContext;
	using DrawingContextObject<Text>::DrawingContextObject;
};

class LinearGradient : public DrawingContextObject<LinearGradient> {
public:
	/*
		Sets an offset in the start and end positions.
	*/
	virtual auto set_offset(Point<> offset) -> void {
		if (_implementation) {
			_implementation->set_offset(offset);
		}
	}
	/*
		Sets the horizontal offset in the start position.
	*/
	virtual auto setOffsetX(Dip x) -> void {
		if (_implementation) {
			_implementation->setOffsetX(x);
		}
	}
	/*
		Sets the vertical offset in the start position.
	*/
	virtual auto setOffsetY(Dip y) -> void
	{
		if (_implementation)
		{
			_implementation->setOffsetY(y);
		}
	}

	/*
		Returns the offset in the start and end positions.
	*/
	virtual auto getOffset() const -> Point<>
	{
		if (_implementation)
		{
			return _implementation->getOffset();
		}
		return {};
	}
	/*
		Returns the horizontal offset in the start and end positions.
	*/
	virtual auto getOffsetX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getOffsetX();
		}
		return 0.f;
	}
	/*
		Returns the vertical offset in the start and end positions.
	*/
	virtual auto getOffsetY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getOffsetY();
		}
		return 0.f;
	}

	/*
		Sets the coordinates where the gradient will start, relative to the origin.
	*/
	virtual auto setStartPosition(Point<> start_position) -> void
	{
		if (_implementation)
		{
			_implementation->setStartPosition(start_position);
		}
	}
	/*
		Returns the coordinates relative to the origin where the gradient will start.
	*/
	virtual auto getStartPosition() const -> Point<>
	{
		if (_implementation)
		{
			return _implementation->getStartPosition();
		}
		return {};
	}
	/*
		Returns the X coordinate relative to the origin where the gradient will start.
	*/
	virtual auto getStartPositionX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getStartPositionX();
		}
		return 0.f;
	}
	/*
		Returns the Y coordinate relative to the origin where the gradient will start.
	*/
	virtual auto getStartPositionY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getStartPositionY();
		}
		return 0.f;
	}

	/*
		Sets the coordinates relative to the origin where the gradient will end.
	*/
	virtual auto setEndPosition(Point<> endPosition) -> void
	{
		if (_implementation)
		{
			_implementation->setEndPosition(endPosition);
		}
	}
	/*
		Returns the coordinates relative to the origin where the gradient will end.
	*/
	virtual auto getEndPosition() const -> Point<>
	{
		if (_implementation)
		{
			return _implementation->getEndPosition();
		}
		return {};
	}
	/*
		Returns the X coordinate relative to the origin where the gradient will end.
	*/
	virtual auto getEndPositionX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getEndPositionX();
		}
		return 0.f;
	}
	/*
		Returns the Y coordinate relative to the origin where the gradient will end.
	*/
	virtual auto getEndPositionY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getEndPositionY();
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
	virtual auto set_offset(Point<> offset) -> void
	{
		if (_implementation)
		{
			_implementation->set_offset(offset);
		}
	}
	/*
		Sets the horizontal offset in the start position.
	*/
	virtual auto setOffsetX(Dip x) -> void
	{
		if (_implementation)
		{
			_implementation->setOffsetX(x);
		}
	}
	/*
		Sets the vertical offset in the start position.
	*/
	virtual auto setOffsetY(Dip y) -> void
	{
		if (_implementation)
		{
			_implementation->setOffsetY(y);
		}
	}
	/*
		Returns the offset in the start position.
	*/
	virtual auto getOffset() const -> Point<>
	{
		if (_implementation)
		{
			return _implementation->getOffset();
		}
		return {};
	}
	/*
		Returns the horizontal offset in the start position.
	*/
	virtual auto getOffsetX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getOffsetX();
		}
		return 0.f;
	}
	/*
		Returns the vertical offset in the start position.
	*/
	virtual auto getOffsetY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getOffsetY();
		}
		return 0.f;
	}

	/*
		Sets the coordinates where the gradient will start, relative to the origin.
	*/
	virtual auto setStartPosition(Point<> start_position) -> void
	{
		if (_implementation)
		{
			_implementation->setStartPosition(start_position);
		}
	}
	/*
		Returns the coordinates relative to the origin where the gradient will start.
	*/
	virtual auto getStartPosition() const -> Point<>
	{
		if (_implementation)
		{
			return _implementation->getStartPosition();
		}
		return Point<>{};
	}
	/*
		Returns the X coordinate relative to the origin where the gradient will start.
	*/
	virtual auto getStartPositionX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getStartPositionX();
		}
		return 0.f;
	}
	/*
		Returns the Y coordinate relative to the origin where the gradient will start.
	*/
	virtual auto getStartPositionY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getStartPositionY();
		}
		return 0.f;
	}

	/*
		Sets the horizontal and vertical size of the gradient.
	*/
	virtual auto setRadius(Size<> radius) -> void
	{
		if (_implementation)
		{
			_implementation->setRadius(radius);
		}
	}
	/*
		Returns the horizontal and vertical size of the gradient.
	*/
	virtual auto getRadius() const -> Size<>
	{
		if (_implementation)
		{
			return _implementation->getRadius();
		}
		return {};
	}
	/*
		Returns the horizontal size of the gradient.
	*/
	virtual auto getRadiusX() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getRadiusX();
		}
		return 0.f;
	}
	/*
		Returns the vertical size of the gradient.
	*/
	virtual auto getRadiusY() const -> Dip
	{
		if (_implementation)
		{
			return _implementation->getRadiusY();
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
	std::string fontFamilyName = std::string{fontFamily_roboto};

	FontWeight fontWeight = FontWeight::Medium;
	FontStyle fontStyle = FontStyle::Normal;
	FontStretch fontStretch = FontStretch::Medium;
	TextAlign textAlign = TextAlign::Left;
	ReadingDirection readingDirection = ReadingDirection::LeftToRight;

	float characterSpacing = 0.f; // Only supported for text objects.
	float lineHeight = 1.f;
	float font_size = 22.f;
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

/*
	A drawing context interface, created by a GUI to be used to create objects
	like text and images (and more) as well as to draw graphics in views.
*/
class DrawingContext
{
protected:
	TextProperties _textProperties;

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
	static auto createGeometryFromImplementation(std::shared_ptr<Geometry>&& geometry) -> Geometry
	{
		return {std::move(geometry)};
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
	virtual auto finishDrawing() -> void = 0;

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
	virtual auto set_is_fullscreen(bool p_isFullscreen) -> void = 0;
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
	virtual auto setBackgroundColor(Color color) -> void = 0;
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
	virtual auto move_origin(Vector2d<> offset) -> void = 0;
	/*
		Sets the screen position of the coordinate (0, 0).
	*/
	virtual auto set_origin(Point<> origin) -> void = 0;
	/*
		Returns the screen position of the coordinate (0, 0).
	*/
	virtual auto get_origin() -> Point<> = 0;

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
	virtual auto scale(Vector2d<Factor> p_scale, Point<> origin) -> void = 0;

	/*
		Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
		drawing so that it is bigger or smaller than normal. Everything will be scaled towards the origin.
	*/
	virtual auto set_scale(Vector2d<Factor> p_scale) -> void = 0;
	/*
		Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics drawing so that
		it is bigger or smaller than normal. Everything will be scaled towards the origin parameter, which is relative
		to the top-left corner of the window.
	*/
	virtual auto set_scale(Vector2d<Factor> p_scale, Point<> origin) -> void = 0;
	/*
		Returns the sizing factor which is transforming graphics drawing so that it is bigger or smaller.
		If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
	*/
	virtual auto get_scale() -> Vector2d<Factor> = 0;
	/*
		Returns the sizing factor for the x-axis which is transforming graphics drawing so that it is bigger or smaller.
		If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
	*/
	virtual auto get_scale_x() -> Factor = 0;
	/*
		Returns the sizing factor for the y-axis which is transforming graphics drawing so that it is bigger or smaller.
		If it is 2, graphics is drawn twice as big as normal. 0.5 is half as big as normal.
	*/
	virtual auto get_scale_y() -> Factor = 0;

	//------------------------------

	/*
		Rotates all future graphics drawing with an angle. Graphics will be rotated relative to the origin.
		Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
	*/
	virtual auto rotate(Arithmetic<float, Radians> angle) -> void = 0;
	/*
		Rotates all future graphics drawing with an angle. Graphics will be rotated relative to the origin.
		Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
	*/
	virtual auto rotate(Arithmetic<float, Degrees> angle) -> void = 0;
	/*
		Rotates all future graphics drawing with an angle.
		Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
		Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
	*/
	virtual auto rotate(Arithmetic<float, Radians> angle, Point<> origin) -> void = 0;
	/*
		Rotates all future graphics drawing with an angle.
		Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
		Positive angle is clockwise and negative is anticlockwise (in our coordinate system).
	*/
	virtual auto rotate(Arithmetic<float, Degrees> angle, Point<> origin) -> void = 0;

	//------------------------------

	/*
		Resets all graphics drawing transformations, so that every coordinate used in any drawing 
		operation is unaltered, and relative to the top-left corner of the target.
	*/
	virtual auto reset_transformations() -> void = 0;

	//------------------------------

	/*
		Resizes the drawing buffers for the window. The GUI calls this for you when it is being resized.
		The size is expressed in dips.
	*/
	virtual auto set_size(Size<> size) -> void = 0;
	/*
		Returns the size of the drawing buffers for the window, in dips.
	*/
	virtual auto get_size() -> Size<> = 0;

	//------------------------------

	/*
		Clears the whole buffer with the specified color.
	*/
	virtual auto clear(Color color) -> void = 0;
	/*
		Clears the whole buffer with a transparent background.
	*/
	virtual auto clear() -> void = 0;

	//------------------------------

	/*
		Draws a filled rectangle using the current color or gradient.
		Change color being used with method set_color or gradient with setGradientBrush.
	*/
	virtual auto fill_rectangle(Rectangle<> rectangle) -> void = 0;

	/*
		Draws a filled rectangle with custom corners using the current color or gradient.
		Change color being used with method set_color or gradient with setGradientBrush.
	*/
	virtual auto fill_rectangle(Rectangle<> rectangle, RectangleCorners const& rectangle_corners) -> void = 0;

	/*
		Draws a filled rounded rectangle using the current color or gradient.
		Change color being used with method set_color or gradient with setGradientBrush.
	*/
	virtual auto fill_rounded_rectangle(Rectangle<> rectangle, Size<> radius) -> void = 0;

	//------------------------------

	/*
		Draws a rectangle outline using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.
	*/
	virtual auto stroke_rectangle(Rectangle<> rectangle, Dip stroke_width) -> void = 0;

	/*
		Draws a rectangle outline with custom corners using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.
	*/
	virtual auto stroke_rectangle(
		Rectangle<> rectangle, RectangleCorners const& rectangle_corners, Dip stroke_width
	) -> void = 0;

	/*
		Draws a rounded rectangle outline using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.
	*/
	virtual auto stroke_rounded_rectangle(Rectangle<> rectangle, Size<> radius, Dip stroke_width) -> void = 0;

	//------------------------------

	/*
		Draws a filled circle using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.

		circle is the center position of the circle.
	*/
	virtual auto fill_circle(Point<> circle, Dip radius) -> void = 0;

	/*
		Draws a circle outline using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.

		circle is the center position of the circle.
	*/
	virtual auto stroke_circle(Point<> circle, Dip radius, Dip stroke_width) -> void = 0;

	virtual auto fill_ellipse(Point<> circle, Size<> radius) -> void = 0;

	virtual auto stroke_ellipse(Point<> circle, Size<> radius, Dip stroke_width) -> void = 0;

	//------------------------------

	/*
		Draws a straight line between two points using the current color or gradient.
		Change the color being used with the method set_color or the gradient with setGradientBrush.
	*/
	virtual auto draw_line(Point<> p_point_0, Point<> p_point_1, float p_thickness = 1.f) -> void = 0;

	//------------------------------

	/*
		Draws the edge of a custom shape.

		vertices is a vector of the points that make up the shape.
		line_thickness is how thicc the edges of the shape are.
		is_closed is whether the last vertex will be connected to the first one to close the shape.
	*/
	virtual auto stroke_shape(
		std::span<Point<> const> vertices, float line_thickness, bool is_closed = false
	) -> void = 0;
	/*
		Fills a custom shape with the current color or gradient.

		p_shape is a vector of points that make up the shape.
	*/
	virtual auto fill_shape(std::span<Point<> const> vertices) -> void = 0;

	//------------------------------

	/*
		Draws a stroked cached geometry with its coordinates relative to the origin.
		If you want to move the geometry, use move_origin().
		If you want to scale the geometry, use scale().
		You can also change the stroke color with set_color().
	*/
	virtual auto stroke_geometry(Geometry const& geometry, float stroke_width = 1.f) -> void = 0;
	/*
		Draws a filled cached geometry with its coordinates relative to the origin.
		If you want to move the geometry, use move_origin().
		If you want to scale the geometry, use scale().
		You can also change the fill color with set_color().
	*/
	virtual auto fill_geometry(Geometry const& geometry) -> void = 0;

	//------------------------------

	/*
		Creates a Geometry object which represents a rounded rectangle.
		The Geometry object can be cached and allows for faster drawing.
	*/
	virtual auto create_rounded_rectangle_geometry(
		Rectangle<> rectangle, float radius, bool p_isStroked = false
	) -> Geometry = 0;
	/*
		Creates a Geometry object which represents a rectangle with custom corners.
		The Geometry object can be cached and allows for faster drawing.
	*/
	virtual auto create_corner_rectangle_geometry(
		Rectangle<> rectangle, RectangleCorners const& p_corners, bool p_isStroked = false
	) -> Geometry = 0;

	//------------------------------

	/*
		Creates a geometry object that represents a polygon.
		The Geometry object can be cached and allows for faster drawing.
	*/
	virtual auto create_polygon_geometry(
		Range<Point<> const*> vertices, bool p_isStroked = false, bool is_closed = true
	) -> Geometry = 0;

	//------------------------------

	/*
		Changes the way both start- and endpoints of lines are drawn.
	*/
	virtual auto set_line_cap(LineCap line_cap) -> void = 0;
	/*
		Changes the way startpoints of lines are drawn.
	*/
	virtual auto set_start_line_cap(LineCap line_cap) -> void = 0;
	/*
		Changes the way endpoints of lines are drawn.
	*/
	virtual auto set_end_line_cap(LineCap line_cap) -> void = 0;
	/*
		Returns the way startpoints of lines are drawn.
	*/
	virtual auto get_start_line_cap() -> LineCap = 0;
	/*
		Returns the way endpoints of lines are drawn.
	*/
	virtual auto get_end_line_cap() -> LineCap = 0;

	//------------------------------

	/*
		Sets how and if lines are dashed/dotted.
	*/
	virtual auto set_line_dash_style(LineDashStyle dash_style) -> void = 0;
	/*
		Returns how and if lines are dashed/dotted.
	*/
	virtual auto get_line_dash_style() -> LineDashStyle = 0;

	/*
		Sets the offset of line dashing/dotting.
	*/
	virtual auto set_line_dash_offset(float dash_offset) -> void = 0;
	/*
		Returns the offset of line dashing/dotting.
	*/
	virtual auto get_line_dash_offset() -> float = 0;

	/*
		This changes the way the endpoints of dots and dashes on lines are drawn.
	*/
	virtual auto set_line_dash_cap(LineCap dash_cap) -> void = 0;
	/*
		Returns the way the endpoints of dots and dashes on lines are drawn.
	*/
	virtual auto get_line_dash_cap() -> LineCap = 0;

	//------------------------------

	/*
		Sets the way line joints are drawn.
	*/
	virtual auto set_line_join(LineJoin line_join) -> void = 0;
	/*
		Returns the way line joints are drawn.
	*/
	virtual auto get_line_join() -> LineJoin = 0;

	/*
		Sets the lower limit of the thickness of pointy "mitered" joints.
	*/
	virtual auto set_line_join_miter_limit(float miter_limit) -> void = 0;
	/*
		Returns the lower limit of the thickness of pointy "mitered" joints.
	*/
	virtual auto get_line_join_miter_limit() -> float = 0;

	//------------------------------

	/*
		After calling this, all graphics drawn outside the cached geometry will be invisible, on pixel level.
		Call pop_clip_shape to remove the last pushed clip geometry.
		The alpha of the clipped content will be multiplied by opacity.
	*/
	virtual auto push_clip_geometry(Geometry const& geometry, float opacity = 1.f) -> void = 0;

	//------------------------------

	/*
		After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
		Call pop_clip_shape to remove the last pushed clip shape.
		The alpha of the clipped content will be multiplied by opacity.
	*/
	virtual auto push_clip_shape(std::span<Point<> const> points, float opacity = 1.f) -> void = 0;

	/*
		This removes the last added clipping shape.
	*/
	virtual auto pop_clip_shape() -> void = 0;

	//------------------------------

	/*
		After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
		Call pop_clip_shape to remove the last pushed clip rectangle.
	*/
	virtual auto pushClipRectangle(Rectangle<> rectangle, float opacity = 1.f) -> void = 0;

	/*
		After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
		Call pop_clip_shape to remove the last pushed clip corner rectangle.
		The alpha of the clipped content will be multiplied by opacity.
	*/
	virtual auto pushClipRectangle(
		Rectangle<> rectangle, RectangleCorners const& p_corners, float opacity = 1.f
	) -> void = 0;

	//------------------------------

	/*
		After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
		Call pop_clip_shape to remove the last pushed rounded clip rectangle.
		The alpha of the clipped content will be multiplied by opacity.
	*/
	virtual auto pushRoundedClipRectangle(
		Rectangle<> rectangle, float radius, float opacity = 1.f
	) -> void = 0;

	//------------------------------

	/*
		Generates an image of a shadow that is cast by a rectangle.

		size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
		p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
		color is the color of the resulting shadow.
	*/
	virtual auto createRectangleShadowImage(Size<> size, float p_blur, Color color) -> Image = 0;

	/*
		Generates an image of a shadow that is cast by a rectangle with custom corners.

		size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
		p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
		color is the color of the resulting shadow.
	*/
	virtual auto createRectangleShadowImage(
		Size<> size, RectangleCorners const& p_corners, float p_blur, Color color
	) -> Image = 0;

	//------------------------------

	/*
		Generates an image of a shadow that is cast by a rounded rectangle.

		size is the size of the rounded rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
		radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
		p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
		color is the color of the resulting shadow.
	*/
	virtual auto createRoundedRectangleShadowImage(
		Size<> size, float radius, float p_blur, Color color
	) -> Image = 0;

	//------------------------------

	/*
		Loads an image from pixel data in BGRA format.

		p_pixelData is an array which is 4*width*height bytes in size.
		It contains the color values for every pixel in the image, row-by-row. One byte for every color channel.
	*/
	virtual auto create_image(std::byte const* p_pixelData, Size<Pixels> size) -> Image = 0;
	/*
		Loads an image from the data of an image file.
	*/
	virtual auto create_image(DataView p_imageData) -> Image = 0;
	/*
		Loads an image from a file. Most standard image formats/codecs are supported.
		p_filePath is the path, relative or absolute, to the image file to be loaded.
		If this returns an invalid image, then the file path is probably incorrect.
	*/
	virtual auto create_image(std::string_view p_filePath) -> Image = 0;
	/*
		Creates an image from an OS-specific handle.

		On Windows, it is an HICON or HBITMAP.
	*/
	virtual auto create_image_from_handle(void* p_handle) -> Image = 0;
	/*
		Draws an image, placed according to the image's bounds and positioning/scaling options.
	*/
	virtual auto drawImage(Image const& image, float p_multiplicativeOpacity = 1.f) -> void = 0;

	//------------------------------

	/*
		Creates a buffer that contains the file data of an image, encoded in the format.
	*/
	virtual auto createImageFileData(
		Image const& image, ImageFormat format = ImageFormat::Png
	) -> DataVector = 0;
	/*
		Creates a stream that contains the file data of an image, encoded in the format.
		On Windows, the return pointer type is IStream.
	*/
	virtual auto createImageFileDataNativeStream(
		Image const& image, ImageFormat format = ImageFormat::Png
	) -> void* = 0;
	/*
		Saves an image to a file, encoded in the format format.
		Returns true if it succeeded and false if it failed.
	*/
	virtual auto saveImageToFile(
		Image const& image, std::string_view p_filePath, ImageFormat format = ImageFormat::Png
	) -> bool = 0;

	//------------------------------

	/*
		Creates an OS API native image from an AvoGUI image.
		On Windows, it returns an HBITMAP.
	*/
	virtual auto createNativeImageFromImage(Image const& image) -> void* = 0;

	//------------------------------

	/*
		Creates a linear gradient that can be used as a brush when drawing things.
	*/
	virtual auto createLinearGradient(
		Range<GradientStop*> p_gradientStops, Point<> start_position = {}, Point<> endPosition = {}
	) -> LinearGradient = 0;

	/*
		Creates a radial gradient that can be used as a brush when drawing things.
	*/
	virtual auto createRadialGradient(
		Range<GradientStop*> p_gradientStops, Point<> start_position = {}, Point<> radius = {}
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
	virtual auto set_color(Color color) -> void = 0;

	/*
		Sets the transparency of all graphics that will be drawn.
	*/
	virtual auto set_opacity(float opacity) -> void = 0;

	//------------------------------

	/*
		Adds a new font family that can be used by text.
		p_filePath is a path to a font file with a common format.
	*/
	virtual auto addFont(std::string_view p_filePath) -> void = 0;

	/*
		Adds a new font to a font family that can be used by text.
		data is the data that would be in a font file with a common format.
		The data is moved from data.
	*/
	virtual auto addFont(DataVector&& data) -> void = 0;
	/*
		Adds a new font to a font family that can be used by text.
		data is the data that would be in a font file with a common format.
	*/
	virtual auto addFont(DataView data) -> void = 0;

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
	virtual auto create_text(std::string_view string, float font_size, Rectangle<> p_bounds = {}) -> Text = 0;
	/*
		Draws pre-calculated text created with the create_text method.
	*/
	virtual auto draw_text(Text const& p_text) -> void = 0;

	/*
		Lays out and draws a string in a rectangle.
		If you're drawing the same text repeatedly, use a Text object (created with method create_text).
	*/
	virtual auto draw_text(std::string_view string, Rectangle<> rectangle) -> void = 0;

	/*
		Lays out and draws a string at a position usinng the current text properties.
		If you're drawing the same text repeatedly, use a Text object (created with create_text()).
	*/
	virtual auto draw_text(std::string_view string, Point<> position) -> void = 0;

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
constexpr auto operator&(ModifierKeyFlags left, ModifierKeyFlags right) noexcept -> bool
{
	return static_cast<uint32>(left) & static_cast<uint32>(right);
}
constexpr auto operator|(ModifierKeyFlags left, ModifierKeyFlags right) noexcept -> ModifierKeyFlags
{
	return static_cast<ModifierKeyFlags>(static_cast<uint32>(left) | static_cast<uint32>(right));
}
constexpr auto operator|=(ModifierKeyFlags& left, ModifierKeyFlags right) noexcept -> ModifierKeyFlags&
{
	return left = left | right;
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
	MouseButton mouse_button = MouseButton::None;
	/*
		The modifier keys and mouse buttons that were down when the event occurred.
	*/
	ModifierKeyFlags modifier_keys = ModifierKeyFlags::None;
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
	constexpr MouseEvent(MouseEvent const& other) noexcept :
		target{other.target},
		xy{other.xy},
		movement{other.movement},
		scrollDelta{other.scrollDelta},
		mouse_button{other.mouse_button},
		modifier_keys{other.modifier_keys},
		isDoubleClick{other.isDoubleClick}
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
	virtual auto get_data_for_format(Index p_formatIndex) const -> DataView = 0;
	/*
		format is one of the values in the "formats" vector.
	*/
	virtual auto get_format_name(uint32 format) const -> std::string = 0;

	/*
		Returns the text of what is to be dropped, in UTF-8 encoding.
	*/
	virtual auto get_string() const -> std::string = 0;
	/*
		Returns the text of what is to be dropped, in UTF-16 encoding.
	*/
	virtual auto get_utf16_string() const -> std::u16string = 0;
	/*
		Returns whether the item to be dropped has any text.
	*/
	virtual auto get_has_string() const -> bool = 0;

	/*
		Return the names of what is to be dropped, in UTF-8 encoding.
		Keep in mind that this includes both dragged files and directories.
	*/
	virtual auto get_item_names() const -> std::vector<std::string> = 0;
	/*
		Returns the names of what is to be dropped, in UTF-16 encoding.
		Keep in mind that this includes both dragged files and directories.
	*/
	virtual auto get_utf16_item_names() const -> std::vector<std::u16string> = 0;
	/*
		Returns the number of items that have a name.
		Keep in mind that this includes both dragged files and directories.
	*/
	virtual auto get_number_of_item_names() const -> Count = 0;

	/*
		Returns the file names of the respective file contents, in UTF-8 encoding.
	*/
	virtual auto get_file_names() const -> std::vector<std::string> = 0;
	/*
		Returns the file names of the respective file contents, in UTF-16 encoding.
	*/
	virtual auto get_utf16_file_names() const -> std::vector<std::u16string> = 0;
	/*
		Returns the file contents of every file that is being dragged.
	*/
	virtual auto get_file_contents() const -> std::vector<DataVector> = 0;
	/*
		Returns the file contents of an item that is being dragged, by its index.
	*/
	virtual auto get_file_contents(Index index) const -> DataVector = 0;
	/*
		Returns the number of dragged items that have file contents.
	*/
	virtual auto get_number_of_files() const -> Count = 0;

	/*
		Returns the additional data that has been assigned by an AvoGUI application.
	*/
	virtual auto get_additional_data() const -> uint64 = 0;

	/*
		If an image is being dragged, this creates and returns an Image object representing the image that was dragged.
		If no image is being dragged, it returns 0.
	*/
	virtual auto get_image() const -> Image = 0;
};

struct DragDropEvent {
	/*
		The view that the event is directed towards.
	*/
	View* target{};
	/*
		The modifier keys that were pressed when the event fired.
	*/
	ModifierKeyFlags modifier_keys = ModifierKeyFlags::None;

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
	DragDropEvent(DragDropEvent const& other) :
		target{other.target},
		modifier_keys{other.modifier_keys},
		xy{other.xy},
		movement{other.movement},
		data{other.data}
	{}
};

//------------------------------

class Window;
struct WindowEvent {
	/*
		The window that has received the event from the OS.
	*/
	Window* window{};
	/*
		The new size of the window if it has changed size (includes sizeChange/maximize/restore events).
	*/
	Size<> size;
};

//------------------------------

enum class WindowStyleFlags {
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

constexpr auto operator&(WindowStyleFlags left, WindowStyleFlags right) noexcept -> WindowStyleFlags
{
	return static_cast<WindowStyleFlags>(static_cast<uint32>(left) & static_cast<uint32>(right));
}
constexpr auto operator|(WindowStyleFlags left, WindowStyleFlags right) noexcept -> WindowStyleFlags
{
	return static_cast<WindowStyleFlags>(static_cast<uint32>(left) | static_cast<uint32>(right));
}
constexpr auto operator|=(WindowStyleFlags& left, WindowStyleFlags right) noexcept -> WindowStyleFlags&
{
	left = left | right;
	return left;
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
class Window {
public:
	/*
		Creates the window. To close it, use close().

		title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.

		position_factor is the position of the window, expressed as a factor between 0 and 1, where 0 means the left/top edge
		of the primary monitor and the left/top edge of the window are aligned, and 1 means the right/bottom edges are aligned.

		size is the size of the client area in DIPs (device independent pixels).
		p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
		parent is an optional parent window, which this window would appear above.
	*/
	virtual auto create(
		std::string_view title, 
		Point<Factor> position_factor, 
		Size<> size, 
		WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, 
		Window* parent = nullptr
	) -> void = 0;
	/*
		Creates the window in the center of the screen. To close it, use close().

		title is the text that appears in the title bar of the window (if it has a border), in UTF-8 encoding.
		size is the size of the client area in DIPs (device independent pixels).
		p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
		parent is an optional parent window, which this window would appear above.
	*/
	virtual auto create(
		std::string_view title, 
		Size<> size, 
		WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, 
		Window* parent = nullptr
	) -> void
	{
		create(title, {0.5f}, size, p_styleFlags, parent);
	}

protected:
	bool _is_running = false;
	std::mutex _isRunningMutex;
	std::condition_variable _isRunningConditionVariable;

	friend class Gui;

	/*
		Makes the window start receiving events.
	*/
	auto run() -> void
	{
		_isRunningMutex.lock();
		_is_running = true;
		_isRunningMutex.unlock();
		_isRunningConditionVariable.notify_one();
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
	bool _will_close = false;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns whether the GUI and its window is awaiting being closed by the animation/drawing thread.
	*/
	auto get_will_close() const -> bool {
		return _will_close;
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
	virtual auto set_title(std::string_view title) -> void = 0;
	/*
		Returns the text shown in the titlebar.
	*/
	virtual auto get_title() const -> std::string = 0;

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
	virtual auto get_native_handle() const -> void* = 0;
	/*
		Returns the OS-specific window object associated with this window.
	*/
	template<typename T>
	auto get_native_handle() const -> T
	{
		return static_cast<T>(get_native_handle());
	}

	//------------------------------

	/*
		Changes whether the client area of the window fills the whole screen.
	*/
	virtual auto set_is_fullscreen(bool p_isFullscreen) -> void = 0;
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
	virtual auto setPosition(Point<Pixels> position) -> void = 0;
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
	auto set_size(Size<> size) -> void 
	{
		set_pixel_size(dips_to_pixels(size));
	}
	/*
		Returns the size of the client area of the window, in dip units.
	*/
	auto get_size() const -> Size<> 
	{
		return pixels_to_dips(get_pixel_size());
	}
	/*
		Returns the width of the client area of the window, in dip units.
	*/
	auto get_width() const -> Dip 
	{
		return pixels_to_dips(get_pixel_width());
	}
	/*
		Returns the height of the client area of the window, in dip units.
	*/
	auto get_height() const -> Dip 
	{
		return pixels_to_dips(get_pixel_height());
	}

	/*
		Sets the size of the client area of the window, in pixel units.
	*/
	virtual auto set_pixel_size(Size<Pixels> size) -> void = 0;
	/*
		Returns the size of the client area of the window, in pixel units.
	*/
	virtual auto get_pixel_size() const -> Size<Pixels> = 0;
	/*
		Returns the width of the client area of the window, in pixel units.
	*/
	virtual auto get_pixel_width() const -> Pixels = 0;
	/*
		Returns the height of the client area of the window, in pixel units.
	*/
	virtual auto get_pixel_height() const -> Pixels = 0;
	
	/*
		Sets the smallest allowed size for the window when the user is resizing it, in dip units.
	*/
	auto setMinSize(Size<> p_minSize) -> void
	{
		set_min_pixel_size(dips_to_pixels(p_minSize));
	}
	/*
		Returns the smallest allowed size for the window when the user is resizing it, in dip units.
	*/
	auto getMinSize() const -> Size<>
	{
		return pixels_to_dips(get_min_pixel_size());
	}
	/*
		Returns the smallest allowed width for the window when the user is resizing it, in dip units.
	*/
	auto getMinWidth() const -> Dip
	{
		return pixels_to_dips(get_min_pixel_width());
	}
	/*
		Returns the smallest allowed height for the window when the user is resizing it, in dip units.
	*/
	auto getMinHeight() const -> Dip
	{
		return pixels_to_dips(get_min_pixel_height());
	}

	/*
		Sets the smallest allowed size for the window when the user is resizing it, in pixel units.
	*/
	virtual auto set_min_pixel_size(Size<Pixels> p_minSize) -> void = 0;
	/*
		Returns the smallest allowed size for the window when the user is resizing it, in pixel units.
	*/
	virtual auto get_min_pixel_size() const -> Size<Pixels> = 0;
	/*
		Returns the smallest allowed width for the window when the user is resizing it, in pixel units.
	*/
	virtual auto get_min_pixel_width() const -> Pixels = 0;
	/*
		Returns the smallest allowed height for the window when the user is resizing it, in pixel units.
	*/
	virtual auto get_min_pixel_height() const -> Pixels = 0;

	/*
		Sets the biggest allowed size for the window when the user is resizing it, in dip units.
	*/
	auto setMaxSize(Size<> p_maxSize) -> void
	{
		set_max_pixel_size(dips_to_pixels(p_maxSize));
	}
	/*
		Returns the biggest allowed size for the window when the user is resizing it, in dip units.
	*/
	auto getMaxSize() const -> Size<>
	{
		return pixels_to_dips(get_max_pixel_size());
	}
	/*
		Returns the biggest allowed width for the window when the user is resizing it, in dip units.
	*/
	auto getMaxWidth() const -> Dip
	{
		return pixels_to_dips(getMaxPixelWidth());
	}
	/*
		Returns the biggest allowed height for the window when the user is resizing it, in dip units.
	*/
	auto getMaxHeight() const -> Dip
	{
		return pixels_to_dips(getMaxPixelHeight());
	}

	/*
		Sets the biggest allowed size for the window when the user is resizing it, in pixel units.
	*/
	virtual auto set_max_pixel_size(Size<Pixels> p_maxSize) -> void = 0;
	/*
		Returns the biggest allowed size for the window when the user is resizing it, in pixel units.
	*/
	virtual auto get_max_pixel_size() const -> Size<Pixels> = 0;
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
	virtual auto get_monitor_bounds() const -> Rectangle<Pixels> = 0;
	/*
		Returns the virtual position of the current monitor used by the window relative to other monitors, in pixel units.
	*/
	virtual auto get_monitor_position() const -> Point<Pixels> = 0;
	/*
		Returns the size of the current monitor used by the window, in pixel units.
	*/
	virtual auto get_monitor_size() const -> Size<Pixels> = 0;
	/*
		Returns the width of the current monitor used by the window, in pixel units.
	*/
	virtual auto get_monitor_width() const -> Pixels = 0;
	/*
		Returns the height of the current monitor used by the window, in pixel units.
	*/
	virtual auto get_monitor_height() const -> Pixels = 0;

	//------------------------------

	/*
		Returns the bounds of the work area of the monitor currently used by the window, in pixel units.
		This excludes the taskbar on Windows.
	*/
	virtual auto get_work_area_bounds() const -> Rectangle<Pixels> = 0;
	/*
		Returns the virtual position of the work area of the monitor currently used by the window, in pixel units.
		This excludes the taskbar on Windows.
	*/
	virtual auto get_work_area_position() const -> Point<Pixels> = 0;
	/*
		Returns the size of the work area of the monitor currently used by the window, in pixel units.
		This excludes the taskbar on Windows.
	*/
	virtual auto get_work_area_size() const -> Size<Pixels> = 0;
	/*
		Returns the width of the work area of the monitor currently used by the window, in pixel units.
		This excludes the taskbar on Windows.
	*/
	virtual auto get_work_area_width() const -> Pixels = 0;
	/*
		Returns the height of the work area of the monitor currently used by the window, in pixel units.
		This excludes the taskbar on Windows.
	*/
	virtual auto get_work_area_height() const -> Pixels = 0;

	//------------------------------

	/*
		Returns whether a key is currently pressed down.
	*/
	virtual auto get_is_key_down(KeyboardKey p_key) const -> bool = 0;
	/*
		Returns whether a mouse button is currently pressed down.
	*/
	virtual auto get_is_mouse_button_down(MouseButton p_button) const -> bool = 0;
	/*
		Returns the position of the mouse cursor, relative to the top-left corner of the window.
	*/
	virtual auto get_mouse_position() const -> Point<> = 0;

	//------------------------------

	/*
		Changes what the mouse cursor looks like.
	*/
	virtual auto set_cursor(Cursor p_cursor) -> void = 0;
	/*
		Returns the current mouse cursor.
	*/
	virtual auto get_cursor() const -> Cursor = 0;

	//------------------------------

protected:
	Factor _dip_to_pixel_factor = 1.f;
public:
	/*
		Returns the factor that is used to convert DIP units to pixel units in the window.
		This can change during the lifetime of the window, if the user drags it to a monitor with a different DPI for example.
	*/
	auto get_dip_to_pixel_factor() const -> Factor {
		return _dip_to_pixel_factor;
	}
	/*
		Converts device independent units to pixel units.
		This conversion depends on the DPI of the monitor.
		1 dip will convert to more pixels on a high DPI display.
	*/
	auto dips_to_pixels(Dip dip) const -> Pixels {
		return dip*_dip_to_pixel_factor;
	}
	/*
		Converts device independent units to pixel units.
		This conversion depends on the DPI of the monitor.
		1 dip will convert to more pixels on a high DPI display.
	*/
	template<template<typename>typename C>
	auto dips_to_pixels(Vector2dBase<Dip, C> container) const -> Vector2dBase<Pixels, C>
	{
		return Vector2dBase<Pixels, C>{container*_dip_to_pixel_factor};
	}
	/*
		Converts pixel units to device independent units.
		This conversion depends on the DPI of the monitor.
		1 pixel will convert to fewer dips on a high DPI display.
	*/
	auto pixels_to_dips(Pixels p_pixels) const -> Dip
	{
		return p_pixels/_dip_to_pixel_factor;
	}
	/*
		Converts pixel units to device independent units.
		This conversion depends on the DPI of the monitor.
		1 pixel will convert to fewer dips on a high DPI display.
	*/
	template<template<typename>typename C>
	auto pixels_to_dips(Vector2dBase<Pixels, C> container) const -> Vector2dBase<Dip, C>
	{
		return Vector2dBase<Dip, C>{container/_dip_to_pixel_factor};
	}

	//------------------------------

	/*
		Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_string(
		std::string_view string, 
		Image const& drag_image = {},
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;
	/*
		Runs a blocking loop that allows the user to drag string data from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_string(
		std::u16string_view string, 
		Image const& drag_image = {},
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;

	/*
		Runs a blocking loop that allows the user to drag image data from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_image(
		Image const& image, 
		Image const& drag_image = {},
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;

	/*
		Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_file(
		DataView data, 
		std::string_view name, 
		Image const& drag_image = {}, 
		Point<> drag_image_cursor_position = {},
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;
	/*
		Runs a blocking loop that allows the user to drag file data from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_file(
		DataView data, 
		std::u16string_view name, 
		Image const& drag_image = {}, 
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;

	/*
		Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_file(
		std::string_view path, 
		Image const& drag_image = {}, 
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;
	/*
		Runs a blocking loop that allows the user to drag file data or a directory from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_file(
		std::u16string_view path, 
		Image const& drag_image = {}, 
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;

	/*
		Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_files(
		Range<std::string*> paths, 
		Image const& drag_image = {},
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;
	/*
		Runs a blocking loop that allows the user to drag regular files and/or directories from this application to another one, or to itself.
		This method sends events to the drop target(s).
		The return value indicates what operation was made after the drop.
	*/
	virtual auto drag_and_drop_files(
		Range<std::u16string*> paths, 
		Image const& drag_image = {},
		Point<> drag_image_cursor_position = {}, 
		uint64 additional_data = 0u
	) -> DragDropOperation = 0;

	//------------------------------

	/*
		Gives a UTF-8 encoded string for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this string.
	*/
	virtual auto set_clipboard_string(std::string_view string, uint64 additional_data = 0u) const -> void = 0;
	/*
		Gives a UTF-16 encoded string for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this string.
	*/
	virtual auto set_clipboard_string(std::u16string_view string, uint64 additional_data = 0u) const -> void = 0;

	/*
		Gives an image for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_image(Image const& image, uint64 additional_data = 0u) const -> void = 0;

	/*
		Gives file data for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_file(DataView data, std::string_view name, uint64 additional_data = 0u) const -> void = 0;
	/*
		Gives file data for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_file(DataView data, std::u16string_view name, uint64 additional_data = 0u) const -> void = 0;
	/*
		Gives a UTF-8 file path for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_file(std::string_view path, uint64 additional_data = 0u) const -> void = 0;
	/*
		Gives a UTF-16 file path for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_file(std::u16string_view path, uint64 additional_data = 0u) const -> void = 0;

	/*
		Gives UTF-8 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_files(Range<std::string*> paths, uint64 additional_data = 0u) const -> void = 0;
	/*
		Gives UTF-16 file/directory paths for the OS to store globally. Other programs, or this one, can then access it.
		The data currently stored on the clipboard is freed and replaced by this data.
	*/
	virtual auto set_clipboard_files(Range<std::u16string*> paths, uint64 additional_data = 0u) const -> void = 0;

	/*
		Returns the data that is currently stored on the clipboard.
	*/
	virtual auto get_clipboard_data() const -> std::unique_ptr<ClipboardData> = 0;

	//------------------------------
	// Window events

protected:
	auto set_window_close_events(WindowEvent const& event) -> bool {
		return std::ranges::all_of(
			window_close_listeners, [&](auto const& listener){ return listener(event); }
		);
	}

public:
	using WindowListeners = EventListeners<void(WindowEvent const&)>;
	EventListeners<bool(WindowEvent const&)> window_close_listeners;
	WindowListeners window_create_listeners;
	WindowListeners window_destroy_listeners;
	WindowListeners window_minimize_listeners;
	WindowListeners window_maximize_listeners;
	WindowListeners window_restore_listeners;
	WindowListeners window_size_change_listeners;
	WindowListeners window_focus_gain_listeners;
	WindowListeners window_focus_lose_listeners;
};

class Gui;

/*
	A rectangle that can draw itself and receive events. Used for GUI components and stuff.
*/
class View : public Component, public ProtectedRectangle {
private:
	friend class Gui;

public:
	View(View* parent, Rectangle<> p_bounds = {});
	View(View* parent, Id id, Rectangle<> p_bounds = {});

protected:
	Geometry _clip_geometry;
	/*
		LIBRARY IMPLEMENTED
		This is called whenever the clipping geometry of the view needs to be updated.
		You can override this if you want a custom clipping geometry, just replace _clip_geometry.
	*/
	virtual auto update_clip_geometry() -> void;

public:
	/*
		Sets the geometry being used to clip the view's contents.
		The clip geometry of the view is by default updated automatically in the updateGeometry method when the size has changed.
		Note that hit testing is not by default affected by this, override get_is_containing(Point<>) if you want custom hit testing.
	*/
	auto set_clip_geometry(Geometry const& geometry) -> void {
		_clip_geometry = geometry;
	}
	/*
		Returns the geometry being used to clip the view's contents.
	*/
	[[nodiscard]]
	auto getClipGeometry() const -> Geometry const& {
		return _clip_geometry;
	}

	//------------------------------

private:
	bool _is_overlay = false;
public:
	/*
		LIBRARY IMPLEMENTED
		If you set this to true, this view will not block any mouse events from reaching views below this one.
		Whether this view is a mouse listener has no effect on this.
	*/
	auto set_is_overlay(bool const is_overlay) -> void {
		_is_overlay = is_overlay;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns whether this view blocks mouse events from reaching views below this one.
		False means it blocks, true means it does not.
	*/
	[[nodiscard]]
	auto get_is_overlay() const -> bool {
		return _is_overlay;
	}

	//------------------------------

private:
	bool _is_visible = true;
public:
	/*
		LIBRARY IMPLEMENTED
		Sets whether the view is visible and can receive events.
	*/
	auto set_is_visible(bool const p_isVisible) -> void {
		if (p_isVisible != _is_visible) {
			_is_visible = p_isVisible;
			invalidate();
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns whether the view is visible and can receive events.
	*/
	[[nodiscard]]
	constexpr auto get_is_visible() noexcept const -> bool {
		return _is_visible;
	}

	//------------------------------

private:
	Factor _opacity = 1.f;
public:
	/*
		Sets how opaque the view and its children are (multiplied with parent opacity).
	*/
	constexpr auto set_opacity(Factor const opacity) noexcept -> void {
		_opacity = opacity;
	}
	/*
		Returns how opaque the view and its children are (multiplied with parent opacity).
	*/
	[[nodiscard]]
	constexpr auto get_opacity() noexcept const -> Factor {
		return _opacity;
	}

	//------------------------------

	// TODO: Add view transforms

	//------------------------------

private:
	Cursor _cursor = Cursor::Arrow;
public:
	/*
		LIBRARY IMPLEMENTED
		Sets the cursor that will by default be shown when the mouse enters the view.
		The default implementation of handle_mouse_background_enter sets the cursor to this one, and you can override this behaviour.
		This method also calls enable_mouse_events().
	*/
	auto set_cursor(Cursor const p_cursor) -> void {
		_cursor = p_cursor;
		enable_mouse_events();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the cursor that will by default be shown when the mouse enters the view.
	*/
	[[nodiscard]]
	auto get_cursor() const -> Cursor {
		return _cursor;
	}

	//------------------------------

private:
	Gui* _gui = nullptr;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns a pointer to the highest view in the hierarchy, the GUI.
	*/
	[[nodiscard]]
	auto get_gui() const -> Gui* {
		return _gui;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns a pointer to the highest view in the hierarchy, the GUI.
		It is cast to another pointer type.
	*/
	template<typename T>
	[[nodiscard]]
	auto get_gui() const -> T* {
		return dynamic_cast<T*>(_gui);
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the object used for drawing.
		The same as calling get_gui()->get_drawing_context(), but more convenient.
	*/
	[[nodiscard]]
	auto get_drawing_context() const -> DrawingContext*;
	/*
		LIBRARY IMPLEMENTED
		Returns the window that is attached to the GUI.
	*/
	[[nodiscard]]
	auto get_window() const -> Window*;

	//------------------------------

private:
	std::vector<std::unique_ptr<Animation>> _animations;

public:

	/*
		Creates an animation that is released by this view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		milliseconds is the duration of the animation, can be changed later on the returned object.
	*/
	auto add_animation(Easing const easing, float const milliseconds) -> Animation* {
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), easing, milliseconds)
		).get();
	}
	/*
		Creates an animation.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		easing_id is the theme easing ID of the animation easing to be used.
		milliseconds is the duration of the animation, can be changed later on the returned object.
	*/
	auto add_animation(Id const easing_id, float const milliseconds) -> Animation* {
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), get_theme_easing(easing_id), milliseconds)
		).get();
	}
	/*
		Creates an animation that is released by this view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		milliseconds is the duration of the animation, can be changed later on the returned object.
		callback is a function that will be called every time the animation has been updated, it takes 
		the current animation value as a parameter.
	*/
	auto add_animation(
		Easing const easing, 
		float const milliseconds, 
		IsAnimationCallback auto const& callback
	) -> Animation* {
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), easing, milliseconds, callback)
		).get();
	}
	/*
		Creates an animation that is released by the view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		easing_id is the theme easing ID of the animation easing to be used.
		milliseconds is the duration of the animation, can be changed later on the returned object.
		callback is a function that will be called every time the animation has been updated, it takes 
		the current animation value as a parameter.
	*/
	auto add_animation(
		Id const easing_id, 
		float const milliseconds,
		IsAnimationCallback auto const& callback
	) -> Animation*
	{
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), get_theme_easing(easing_id), milliseconds, callback)
		).get();
	}

	/*
		Creates an animation that is released by this view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		duration is the duration of the animation, can be changed later on the returned object.
	*/
	template<typename _DurationType, typename _DurationPeriod>
	auto add_animation(
		Easing const easing, 
		chrono::duration<_DurationType, _DurationPeriod> const duration
	) -> Animation*
	{
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), easing, milliseconds)
		).get();
	}
	/*
		Creates an animation that is released by the view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		easing_id is the theme easing ID of the animation easing to be used.
		duration is the duration of the animation, can be changed later on the returned object.
	*/
	template<typename _DurationType, typename _DurationPeriod>
	auto add_animation(
		Id const easing_id, 
		chrono::duration<_DurationType, _DurationPeriod> const duration
	) -> Animation*
	{
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), get_theme_easing(easing_id), milliseconds)
		).get();
	}
	/*
		Creates an animation that is released by this view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		duration is the duration of the animation, can be changed later on the returned object.
		callback is a function that will be called every time the animation has been updated, it takes 
		the current animation value as a parameter.
	*/
	template<IsNumber _DurationType, IsRatio _DurationPeriod>
	auto add_animation(
		Easing const easing, 
		chrono::duration<_DurationType, _DurationPeriod> const duration, 
		IsAnimationCallback auto const& callback
	) -> Animation*
	{
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), easing, duration, callback)
		).get();
	}
	/*
		Creates an animation that is released by the view when it is destroyed.
		It is recommended to create Animation objects as stack-allocated member variables of the view 
		instead of using these methods if you can.
		easing_id is the theme easing ID of the animation easing to be used.
		duration is the duration of the animation, can be changed later on the returned object.
		callback is a function that will be called every time the animation has been updated, it takes 
		the current animation value as a parameter.
	*/
	template<IsNumber _DurationType, IsRatio _DurationPeriod>
	auto add_animation(
		Id const easing_id, 
		chrono::duration<_DurationType, _DurationPeriod> const duration, 
		IsAnimationCallback auto const& callback
	) -> Animation*
	{
		return _animations.emplace_back(
			std::make_unique<Animation>(get_gui(), get_theme_easing(easing_id), duration, callback)
		).get();
	}

private:
	/*
		LIBRARY IMPLEMENTED
		Makes sure the view is drawn at the correct time, according to elevation.
		Re-sorts a child view according to elevation.
	*/
	auto update_view_drawing_index(View* const view) -> void {
		auto const number_of_views = get_number_of_views();
		if (number_of_views <= 1 || view->get_parent<View>() != this) {
			// Nothing we can do!
			return;
		}

		auto const elevation = view->get_elevation();
		auto const previous_index = view->get_index();

		auto const is_ordered_with_view_before = !previous_index || elevation > _child_views[previous_index - 1]->get_elevation();
		auto const is_ordered_with_view_after = previous_index >= number_of_views - 1 || elevation < _child_views[previous_index + 1]->get_elevation();

		if (is_ordered_with_view_before && is_ordered_with_view_after) {
			// Nothing we can do!
			return;
		}
		else if (!is_ordered_with_view_before) {
			for (auto const a : Range{0, previous_index}.reverse()) {
				if (!a || _child_views[a - 1]->get_elevation() <= elevation) {
					_child_views[a] = view;
					view->_index = a;
					return;
				}
				else {
					_child_views[a] = _child_views[a - 1];
					_child_views[a]->_index = a;
				}
			}
		}
		else //!is_ordered_with_view_after
		{
			for (auto const a : Range{previous_index, number_of_views}) {
				if (a == number_of_views - 1 || _child_views[a + 1]->get_elevation() >= elevation) {
					_child_views[a] = view;
					view->_index = a;
					return;
				}
				else {
					_child_views[a] = _child_views[a + 1];
					_child_views[a]->_index = a;
				}
			}
		}
	}

	Index _index{};
public:
	/*
		LIBRARY IMPLEMENTED
		Returns the index of this view relative to its siblings.
	*/
	auto get_index() const -> Index {
		return _index;
	}

private:
	Index _layer_index{};
public:
	/*
		LIBRARY IMPLEMENTED
		Returns the layer index of the view, how deep down the view hierarchy it is.
		The GUI view has a layer index of 0.
	*/
	auto get_layer_index() const -> Index {
		return _layer_index;
	}

private:
	View* _parent{};
public:
	/*
		LIBRARY IMPLEMENTED
		Attaches this view to a new parent, which will manage the lifetime of the view.
		If the parameter is 0, the view is only detached from its old parent, and is left alone with no parents :^(.
	*/
	auto set_parent(View* const container) -> void {
		if (container == _parent) {
			return;
		}

		Component::set_parent(container);

		if (_parent) {
			_parent->remove_view(this);
		}

		if (_parent = container) {
			_gui = _parent->_gui;

			if (dynamic_cast<View*>(_gui) == this) {
				_layer_index = 0;
			}
			else {
				_layer_index = _parent->_layer_index + 1U;
			}
			_absolute_position = _parent->get_absolute_top_left() + _bounds.get_top_left();

			_parent->insert_view(this);
			// _index = _parent->_child_views.size();
			// _parent->_child_views.push_back(this);

			_parent->child_view_attachment_listeners(this);
			// _parent->update_view_drawing_index(this);
		}
		else {
			_layer_index = 0;
			_index = 0;
		}
	}

private:
	std::vector<View*> _child_views;

	auto insert_view(View* const child_view) -> void {
		auto const position = std::ranges::lower_bound(
			_child_views, child_view, 
			[](View* const a, View* const b){
				return a->get_elevation() < b->get_elevation();
			}
		);
		_child_views.insert(position, child_view);
		for (auto const view : std::ranges::subrange{position + 1, end()}) {
			++view->_index;
		}
	}

	/*
		Applies an operation to all child views within this view, 
		and to the child views of those child views and so on.
		This is done in determined sequential order, the same
		order as the child views appear in their vector container.
	*/
	template<std::invocable<View*> _Callback>
	auto apply_to_all_child_views_recursively(_Callback&& p_apply) -> void {
		auto const currentContainer = this;
		auto startIndex = Index{};
		while (true) {
			auto const nextContainer = std::find_if(
				currentContainer->begin() + startIndex, currentContainer->end(),
				[&](View* const childView){
					std::forward<_Callback>(p_apply)(childView);
					return childView->get_has_views();
				}
			);
			if (nextContainer == currentContainer->end()) {
				if (currentContainer == this) {
					break;
				}
				else {
					startIndex = currentContainer->get_index() + 1;
					currentContainer = currentContainer->get_parent<View>();
				}
			}
			else {
				currentContainer = *nextContainer;
				startIndex = 0;
			}
		}
	}

public:
	template<std::derived_from<View> T, typename ... _Arguments>
	auto add_view(_Arguments&& ... p_arguments) -> T* {
		auto const newView = static_cast<View*>(add_component<T>(std::forward<_Arguments>(p_arguments)...));
		insert_view(newView);
		return newView;
	}
	/*
		LIBRARY IMPLEMENTED
		Removes a child view from this view. This deletes the view being removed.
		If you want to steal the child instead of just killing it, use stealView instead.
	*/
	auto remove_view(View const* const view) -> void {
		if (view && view->_parent == this) {
			remove_view(view->_index);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Removes a child view from this view. This forgets the view being removed.
		If you haven't remembered it yourself, it will get deleted.
	*/
	auto remove_view(Index const view_index) -> void {
		auto const view_iterator = begin() + view_index;
		
		auto const view_to_remove = *view_iterator;
		view_to_remove->_parent = nullptr;

		child_view_detachment_listeners(view_to_remove);
		
		_child_views.erase(view_iterator);
		for (auto const child : std::ranges::subrange{view_iterator, end()}) {
			--child->_index;
		}

		remove_component(view_to_remove);
	}
	/*
		LIBRARY IMPLEMENTED
		Forgets the children views and empties this view from children.
	*/
	auto remove_all_views() -> void {
		for (auto const child : _child_views) {
			child->_parent = nullptr;
			child_view_detachment_listeners(child);
		}
		_child_views.clear();

		remove_all_components();
	}

	/*
		LIBRARY IMPLEMENTED
		Returns the child view at an index.
	*/
	auto get_view(Index const view_index) const -> View* {
		return _child_views[view_index];
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the child view at an index, casted to a pointer of another type.
	*/
	template<typename T>
	auto get_view(Index const view_index) const -> T* {
		return dynamic_cast<T*>(_child_views[view_index]);
	}
	/*
		LIBRARY IMPLEMENTED
		Returns a vector containing the child views that are attached to this view.
	*/
	auto get_views() const -> std::vector<View*> const& {
		return _child_views;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the number of child views that are attached to this view.
	*/
	auto get_number_of_views() const -> Count {
		return static_cast<Count>(_child_views.size());
	}
	/*
		LIBRARY IMPLEMENTED
		Returns whether the view has any child views.
	*/
	auto get_has_views() const noexcept -> bool {
		return !_child_views.empty();
	}

	using iterator = std::vector<View*>::iterator;
	/*
		LIBRARY IMPLEMENTED
		Returns the begin iterator for the view's child views.
		This allows writing range-based for loops over a view's children with simpler syntax.
	*/
	[[nodiscard]] auto begin() noexcept -> iterator
	{
		return _child_views.begin();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the end iterator for the view's child views.
		This allows writing range-based for loops over a view's children with simpler syntax.
	*/
	[[nodiscard]] auto end() noexcept -> iterator
	{
		return _child_views.end();
	}
	
	using const_iterator = std::vector<View*>::const_iterator;
	/*
		LIBRARY IMPLEMENTED
		Returns the begin iterator for the view's child views.
		This allows writing range-based for loops over a view's children with simpler syntax.
	*/
	[[nodiscard]] auto begin() const noexcept -> const_iterator
	{
		return _child_views.begin();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the end iterator for the view's child views.
		This allows writing range-based for loops over a view's children with simpler syntax.
	*/
	[[nodiscard]] auto end() const noexcept -> const_iterator
	{
		return _child_views.end();
	}

	using reverse_iterator = std::vector<View*>::reverse_iterator;
	/*
		LIBRARY IMPLEMENTED
		Returns a reverse iterator pointing to the last child view.
		Incrementing this iterator moves it backwards towards the first child view.
	*/
	[[nodiscard]] auto rbegin() noexcept -> reverse_iterator
	{
		return _child_views.rbegin();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns a reverse iterator pointing to a non-existing child view preceding the first one.
		Incrementing a reverse_iterator moves it backwards towards the first element in the container.
	*/
	[[nodiscard]] auto rend() noexcept -> reverse_iterator
	{
		return _child_views.rend();
	}

	using const_reverse_iterator = std::vector<View*>::const_reverse_iterator;
	/*
		LIBRARY IMPLEMENTED
		Returns a reverse iterator pointing to the last child view.
		Incrementing this iterator moves it backwards towards the first child view.
	*/
	[[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator
	{
		return _child_views.rbegin();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns a reverse iterator pointing to a non-existing child view preceding the first one.
		Incrementing a const_reverse_iterator moves it backwards towards the first element in the container.
	*/
	[[nodiscard]] auto rend() const noexcept -> const_reverse_iterator
	{
		return _child_views.rend();
	}

	//------------------------------

private:
	Image _shadow_image;
	float _elevation = 0.f;

public:
	/*
		Updates the shadow bounds and the shadow image.
	*/
	auto update_shadow() -> void;

	/*
		LIBRARY IMPLEMENTED
		Sets the elevation of the view. This both changes its shadow (if the view has shadow) and drawing order.
		The higher the elevation is, the later it will get drawn.
		If elevation is negative, it is set from the top of the elevation space.
	*/
	auto set_elevation(float elevation) -> void {
		elevation = static_cast<float>(elevation < 0.f)*std::numeric_limits<float>::max() + elevation;

		if (_elevation != elevation) {
			_elevation = elevation;
			update_shadow();
			_parent->update_view_drawing_index(this);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the elevation of the view. See the set_elevation method.
	*/
	auto get_elevation() const noexcept -> float {
		return _elevation;
	}

private:
	bool _has_shadow = true;
public:
	/*
		LIBRARY IMPLEMENTED
		Sets whether the elevation is shown with a shadow.
	*/
	auto set_has_shadow(bool has_shadow) -> void;
	/*
		LIBRARY IMPLEMENTED
		Returns whether the elevation is shown with a shadow.
	*/
	auto getHasShadow() const -> bool {
		return _has_shadow;
	}

private:
	Rectangle<> _shadow_bounds;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns the rectangle that represents the area where the shadow is drawn, relative to the view position.
		The view is always contained within the shadow bounds.
	*/
	auto getShadowBounds() const -> Rectangle<>
	{
		return _shadow_bounds;
	}

	//------------------------------

private:
	bool _isInAnimationUpdateQueue = false;
public:
	/*
		LIBRARY IMPLEMENTED
		Queues an animation update for the next frame.
	*/
	auto queue_animation_update() -> void;
	/*
		USER IMPLEMENTED
		Updates things like animations and does anything that you never want to happen more than once every frame.
		Call queue_animation_update() when you want this method to be called in the next interval.
		This system allows for animations to only get updated when they have to.
	*/
	virtual auto update_animations() -> void {}

	//------------------------------

private:
	/*
		Draws the shadow of the view.
	*/
	auto draw_shadow(DrawingContext* drawing_context) -> void;

public:
	/*
		LIBRARY IMPLEMENTED
		Call this if you want the view to get redrawn.
	*/
	auto invalidate() -> void;

	/*
		USER IMPLEMENTED
		Draws the contents of the view.

		drawing_context is an object used to draw graphics to the window and create graphics objects like text and images.
	*/
	virtual auto draw(DrawingContext* drawing_context) -> void {}

	/*
		USER IMPLEMENTED
		Draws on top of child views.

		drawing_context is an object used to draw graphics to the window and create graphics objects like text and images.
	*/
	virtual auto drawOverlay(DrawingContext* drawing_context) -> void {}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Returns the smallest possible rectangle that contains all child views belonging to this View.
		The rectangle is relative to the position of this view.
	*/
	auto calculateContentBounds() const -> Rectangle<>
	{
		if (_child_views.empty())
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
		if (_child_views.empty())
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
		if (_child_views.empty())
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
		return calculateContentBounds().get_size();
	}

	/*
		LIBRARY IMPLEMENTED
		Returns the leftmost edge of all child views belonging to this View.
		The returned offset is relative to the left edge of this view.
	*/
	auto calculateContentLeft() const -> Dip
	{
		if (_child_views.empty()) {
			return 0.f;
		}

		auto const view = *std::min_element(_child_views.begin(), _child_views.end(), 
			[](View* const a, View* const b) { 
				return a->getLeft() < b->getLeft(); 
			}
		);
		return view->getLeft();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the rightmost edge of all child views belonging to this View.
		The returned offset is relative to the left edge of this view.
	*/
	auto calculateContentRight() const -> Dip
	{
		if (_child_views.empty()) {
			return 0.f;
		}

		auto const view = *std::max_element(_child_views.begin(), _child_views.end(), 
			[](View* const a, View* const b) { 
				return a->getRight() < b->getRight(); 
			}
		);
		return view->getRight();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the topmost edge of all child views belonging to this View.
		The returned offset is relative to the top edge of this view.
	*/
	auto calculateContentTop() const -> Dip
	{
		if (_child_views.empty()) {
			return 0.f;
		}

		auto const view = *std::min_element(_child_views.begin(), _child_views.end(), 
			[](View* const a, View* const b) { 
				return a->getTop() < b->getTop(); 
			}
		);
		return view->getTop();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the bottommost edge of all child views belonging to this View.
		The returned offset is relative to the top edge of this view.
	*/
	auto calculateContentBottom() const -> Dip
	{
		if (_child_views.empty()) {
			return 0.f;
		}

		auto const view = *std::max_element(_child_views.begin(), _child_views.end(), 
			[](View* const a, View* const b) { 
				return a->getBottom() < b->getBottom(); 
			}
		);
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
		auto const offset = Vector2d{
			p_leftPadding - contentBounds.left,
			p_topPadding - contentBounds.top
		};
		for (auto const child : _child_views) {
			child->move(offset);
		}
		set_size({
			contentBounds.get_width() + p_leftPadding + p_rightPadding, 
			contentBounds.get_height() + p_topPadding + p_bottomPadding
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
		for (auto const child : _child_views)
		{
			child->move_x(offset);
		}
		set_width(get_width() + offset);
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the spacing between the rightmost edge of the contents and the right edge of this View.
		This changes the width of this view.
	*/
	auto setRightPadding(Dip const p_rightPadding) -> void
	{
		set_width(calculateContentRight() + p_rightPadding);
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the spacing between the topmost edge of the contents and the top edge of this View.
		This may move the child views with a uniform offset and change the height of this view.
	*/
	auto setTopPadding(Dip const p_topPadding) -> void
	{
		auto const offset = p_topPadding - calculateContentTop();
		for (auto const child : _child_views)
		{
			child->move_y(offset);
		}
		set_height(get_height() + offset);
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the spacing between the bottommost edge of the contents and the bottom edge of this View.
		This changes the height of this view.
	*/
	auto setBottomPadding(Dip const p_bottomPadding) -> void
	{
		set_height(calculateContentBottom() + p_bottomPadding);
	}

	//------------------------------

	/*
		Listener signature:
			void (Id id, Color color)
		See View::handle_theme_color_change for more information.
	*/
	EventListeners<void(Id, Color)> themeColorChangeListeners;
	/*
		USER IMPLEMENTED
		This gets called whenever a theme color has changed, not including initialization.
	*/
	virtual auto handle_theme_color_change(Id id, Color p_newColor) -> void {}

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
	virtual auto handleThemeEasingChange(Id id, Easing p_newEasing) -> void {};

	/*
		Listener signature:
			void (Id id, float value)
		See View::handle_theme_value_change for more information.
	*/
	EventListeners<void(Id, float)> themeValueChangeListeners;
	/*
		USER IMPLEMENTED
		This gets called whenever a theme value has changed, not including initialization.
	*/
	virtual auto handle_theme_value_change(Id id, float p_newValue) -> void {};

	//------------------------------

private:
	std::shared_ptr<Theme> _theme;

private:
	template<typename T, typename U>
	auto propagateThemePropertyChange(
		auto (View::* const function)(Id, T, bool) -> void, 
		Id const id, 
		U&& p_property, 
		bool const will_affect_children
	) -> void
	{
		if (will_affect_children)
		{
			apply_to_all_child_views_recursively([&](View* const childView) {
				(childView->*function)(id, std::forward<U>(p_property), false);
			});
		}

		if (!_theme)
		{
			_theme = std::make_shared<Theme>();
		}
		else if (_theme.use_count() > 1)
		{
			_theme = std::make_shared<Theme>(*_theme);
		}
	}

public:
	/*
		LIBRARY IMPLEMENTED

		Some IDs have a default color that can be changed.
		These colors may be used by views that come with the library, but you can use them yourself too.
		The default color IDs are in the ThemeColors namespace.
		If id is anything else, the color is kept in the theme and you can use it yourself.

		If will_affect_children is true, all children and views below those too will change this color in their themes.
	*/
	auto setThemeColor(Id const id, Color const color, bool const will_affect_children = true) -> void
	{
		propagateThemePropertyChange(&View::setThemeColor, id, color, will_affect_children);

		if (auto& theme_color = _theme->colors[id];
			theme_color != color)
		{
			theme_color = color;
			themeColorChangeListeners(id, color);
		}
	}
	/*
		LIBRARY IMPLEMENTED

		Sets multiple theme colors.
		Example usage:
			using namespace ThemeColors;
			setThemeColors({
				{background, 0.f},
				{on_background, {1.f, 0.f, 0.2f}},
			});

		See setThemeColor for more details.
	*/
	template<typename Pairs = std::initializer_list<std::pair<Id, Color>>>
	auto setThemeColors(Pairs const& p_pairs, bool const will_affect_children = true) -> void
	{
		for (auto const& [id, color] : p_pairs)
		{
			setThemeColor(id, color, will_affect_children);
		}
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto get_theme_color(Id const id) const -> Color
	{
		if (auto const result = _theme->colors.find(id);
			result == _theme->colors.end())
		{
			return {};
		}
		else {
			return result->second;
		}
	}
	/*
		LIBRARY IMPLEMENTED
		color is inserted into the theme with the id id if it doesn't already have a value.
	*/
	auto initialize_theme_color(Id const id, Color const color) -> void {
		_theme->colors.insert({id, color});
	}

	/*
		LIBRARY IMPLEMENTED

		Some IDs have a default easing that can be changed.
		These easings may be used by views that come with the library, but you can use them yourself too.
		The default easing IDs are in the ThemeEasings namespace.
		If id is anything else, the easing is kept in the theme and you can use it yourself.

		if will_affect_children is true, all children and views below those too will change this easing in their themes.
	*/
	auto setThemeEasing(Id const id, Easing const easing, bool const will_affect_children = true) -> void
	{
		propagateThemePropertyChange(&View::setThemeEasing, id, easing, will_affect_children);

		if (auto& theme_easing = _theme->easings[id];
			theme_easing != easing)
		{
			theme_easing = easing;
			themeEasingChangeListeners(id, easing);
		}
	}
	/*
		LIBRARY IMPLEMENTED

		Sets multiple theme easings.
		Example usage:
			using namespace ThemeEasings;
			set_theme_easings({
				{in, {1.f, 0.f, 1.f, 1.f}},
				{in_out, {1.f, 0.f, 0.f, 1.f}},
			});

		See setThemeEasing for more details.
	*/
	template<IsRangeOf<std::pair<Id, Easing>> Pairs = std::initializer_list<std::pair<Id, Easing>>>
	auto set_theme_easings(Pairs const& p_pairs, bool const will_affect_children = true) -> void {
		for (auto const& [id, easing] pair : p_pairs) {
			setThemeEasing(id, easing, will_affect_children);
		}
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto get_theme_easing(Id const id) const -> Easing {
		if (auto const result = _theme->easings.find(id);
			result == _theme->easings.end())
		{
			return {};
		}
		else {
			return result->second;
		}
	}
	/*
		LIBRARY IMPLEMENTED
		easing is inserted into the theme with the ID id if it doesn't already have a value.
	*/
	auto initialize_theme_easing(Id const id, Easing const easing) -> void {
		_theme->easings.insert({id, easing});
	}

	/*
		LIBRARY IMPLEMENTED

		Some IDs have a default value that can be changed.
		These values may be used by views that come with the library, but you can use them yourself too.
		The default value IDs are in the ThemeValues namespace.
		If id is anything else, the value is kept in the theme and you can use it yourself.

		if will_affect_children is true, all children and views below those too will change this value in their themes.
	*/
	auto set_theme_value(Id const id, float const p_value, bool const will_affect_children = true) -> void
	{
		propagateThemePropertyChange(&View::set_theme_value, id, p_value, will_affect_children);

		if (auto& value = _theme->values[id];
			value != p_value)
		{
			value = p_value;
			themeValueChangeListeners(id, p_value);
		}
	}
	/*
		LIBRARY IMPLEMENTED

		Sets multiple theme values.
		Example usage:
			using namespace Avo::ThemeValues;
			set_theme_values({
				{hover_animation_duration, 100},
				{tooltip_font_size, 13.f},
			});

		See set_theme_value for more details.
	*/
	template<typename Pairs = std::initializer_list<std::pair<Id, float>>>
	auto set_theme_values(Pairs const& p_pairs, bool const will_affect_children = true) -> void {
		for (auto const& [id, value] : p_pairs) {
			set_theme_value(id, value, will_affect_children);
		}
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto get_theme_value(Id const id) const -> float {
		if (auto result = _theme->values.find(id);
			result == _theme->values.end())
		{
			return {};
		}
		else {
			return result->second;
		}
	}
	/*
		LIBRARY IMPLEMENTED
		value is inserted into the theme with the ID id if it doesn't already have a value.
	*/
	auto initialize_theme_value(Id const id, float const value) -> void {
		_theme->values.insert({id, value});
	}

	//------------------------------

private:
	Point<> _absolute_position;

	/*
		Moves the point(s) representing the absolute position(s) of this view and/or all children of this view (recursively).
		The absolute positions of views are used often for mouse event targeting, among other things.
		Because of this, it is pre-calculated in this way only when this view or a parent view has moved.
	*/
	auto move_absolute_positions(Vector2d<> const offset, bool const will_update_children = true) -> void {
		_absolute_position += offset;

		if (will_update_children && !_child_views.empty()) {
			apply_to_all_child_views_recursively([&](View* const view) {
				view->move_absolute_positions(offset, false);
			});
		}
	}
	auto calculate_absolute_position_relative_to(Point<> position) const -> Point<>;

	auto create_bounds_change() {
		struct _BoundsChange {
			View* const view;
			Rectangle<> const bounds_before;

			_BoundsChange(View* const p_view) :
				view{p_view},
				bounds_before{p_view->_bounds}
			{}
			~_BoundsChange() {
				view->send_bounds_change_events(bounds_before);
			}
		};
		return _BoundsChange{this};
	}
	
public:
	/*
		LIBRARY IMPLEMENTED
		Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
	*/
	auto set_absolute_bounds(Rectangle<> const rectangle) -> void {
		auto const change = create_bounds_change();

		if (auto const offset = Vector2d{rectangle.get_top_left() - _absolute_position}) {
			move_absolute_positions(offset);
			_bounds.move_top_left(offset);
		}

		_bounds.set_bottom_right(_bounds.get_top_left() + rectangle.get_size(), false);
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the bounds of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_bounds() const -> Rectangle<> {
		return {_absolute_position, _bounds.get_size()};
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the bounds of the view shadow relative to the top left corner of the GUI.
	*/
	auto get_absolute_shadow_bounds() const -> Rectangle<> {
		return {Point{_absolute_position + (_bounds.get_size() - _shadow_bounds.get_size())/2}, _shadow_bounds.get_size()};
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Sets the top left coordinates of the view relative to the top left corner of the GUI.
		If will_keep_size is true, the view will only get positioned, keeping its size.
	*/
	auto set_absolute_top_left(Point<> const position, bool const will_keep_size = true) -> void {
		if (auto const offset = Vector2d{position - _absolute_position}) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions(offset);
			_bounds.set_top_left(_bounds.get_top_left() + offset, will_keep_size);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinates of the top left corner of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_top_left() const -> Point<> {
		return _absolute_position;
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the top right coordinates of the view relative to the top left corner of the GUI.
		If will_keep_size is true, the view will only get positioned, keeping its size.
	*/
	auto set_absolute_top_right(Point<> const position, bool const will_keep_size = true) -> void {
		if (auto const offset = Vector2d{
				position.x - _absolute_position.x + _bounds.left - _bounds.right,
				position.y - _absolute_position.y
			})
		{
			auto const change = create_bounds_change();
			move_absolute_positions({will_keep_size ? offset.x : 0, offset.y});
			_bounds.set_top_right(_bounds.get_top_right() + offset, will_keep_size);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinates of the top right corner of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_top_right() const -> Point<> {
		return {_absolute_position.x + _bounds.get_width(), _absolute_position.y};
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the bottom left coordinates of the view relative to the top left corner of the GUI.
		If will_keep_size is true, the view will only get positioned, keeping its size.
	*/
	auto set_absolute_bottom_left(Point<> const position, bool const will_keep_size = true) -> void {
		if (auto const offset = Vector2d{
				position.x - _absolute_position.x,
				position.y - _absolute_position.y + _bounds.top - _bounds.bottom
			})
		{
			auto const change = create_bounds_change();
			move_absolute_positions({offset.x, will_keep_size ? offset.y : 0.f});
			_bounds.set_bottom_left(_bounds.get_bottom_left() + offset, will_keep_size);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinates of the bottom left corner of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_bottom_left() const -> Point<> {
		return {_absolute_position.x, _absolute_position.y + _bounds.bottom - _bounds.top};
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the bottom right coordinates of the view relative to the top left corner of the GUI.
		If will_keep_size is true, the view will only get positioned, keeping its size.
	*/
	auto set_absolute_bottom_right(Point<> const position, bool const will_keep_size = true) -> void {
		if (auto const offset = Vector2d{
				position.x - _absolute_position.x + _bounds.left - _bounds.right,
				position.y - _absolute_position.y + _bounds.top - _bounds.bottom
			})
		{
			auto const change = create_bounds_change();
			if (will_keep_size) {
				move_absolute_positions(offset);
				_bounds += offset;
			}
			else {
				_bounds.move_bottom_right(offset);
			}
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinates of the bottom right corner of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_bottom_right() const -> Point<> {
		return {_absolute_position.x + _bounds.right - _bounds.left, _absolute_position.y + _bounds.bottom - _bounds.top};
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Sets the center coordinates of the view relative to the top left corner of the GUI.
	*/
	auto set_absolute_center(Point<> const position) -> void {
		if (auto const offset = Vector2d{position - _absolute_position - get_size()/2}) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions(offset);
			_bounds += offset;
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the horizontal center coordinate of the view relative to the left edge of the GUI.
	*/
	auto set_absolute_center_x(float const x) -> void {
		if (auto const offset_x = x - _absolute_position.x - get_width()*0.5f) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions({offset_x, 0});
			_bounds.move_x(offset_x);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the vertical center coordinate of the view relative to the top edge of the GUI.
	*/
	auto set_absolute_center_y(float const y) -> void {
		if (auto const offset_y = y - _absolute_position.y - get_height()*0.5f) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions(0.f, offset_y);
			_bounds.move_y(offset_y);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the center coordinates of the view relative to the top left corner of the GUI.
	*/
	auto get_absolute_center() const noexcept -> Point<> {
		return _absolute_position + get_size()/2;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the x-axis center coordinate of the view relative to the left edge of the GUI.
	*/
	auto get_absolute_center_x() const noexcept -> float {
		return _absolute_position.x + get_width()*0.5f;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the y-axis center coordinate of the view relative to the top edge of the GUI.
	*/
	auto get_absolute_center_y() const noexcept -> float {
		return _absolute_position.y + get_height()*0.5f;
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Sets the left coordinate of this view and updates the layout relative to the left edge of the GUI.
		If will_keep_width is true, the right coordinate will also be changed so that the width of the view stays the same.
	*/
	auto set_absolute_left(float const left, bool const will_keep_width = true) -> void {
		if (auto const offset = left - _absolute_position.x) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions(offset, 0);
			_bounds.set_left(_bounds.left + offset, will_keep_width);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the left coordinate of this view relative to the left edge of the GUI.
	*/
	auto get_absolute_left() const noexcept -> float {
		return _absolute_position.x;
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the top coordinate of this view relative to the top edge of the GUI.
		If will_keep_height is true, the bottom coordinate will also be changed so that the height of the view stays the same.
	*/
	auto set_absolute_top(float const p_top, bool const will_keep_height = true) -> void {
		if (auto const offset = p_top - _absolute_position.y) 
		{
			auto const change = create_bounds_change();
			move_absolute_positions(0, offset);
			_bounds.set_top(_bounds.top + offset, will_keep_height);
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the top coordinate of this view relative to the top edge of the GUI.
	*/
	auto get_absolute_top() const noexcept -> float {
		return _absolute_position.y;
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the right coordinate of this view relative to the left edge of the GUI.
		If will_keep_width is true, the left coordinate will also be changed so that the width of the view stays the same.
	*/
	auto set_absolute_right(float const right, bool const will_keep_width = true) -> void {
		if (auto const offset = right - _absolute_position.x + _bounds.left - _bounds.right) 
		{
			auto const change = create_bounds_change();
			if (will_keep_width) {
				move_absolute_positions(offset, 0);
				_bounds.move_x(offset);
			}
			else {
				_bounds.right += offset;
			}
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinate of the right edge of this view relative to the left edge of the GUI.
	*/
	auto get_absolute_right() const noexcept -> float {
		return _absolute_position.x + _bounds.right - _bounds.left;
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the bottom coordinate of this view relative to the top edge of the GUI and updates the layout.
		If will_keep_height is true, the top coordinate will also be changed so that the height of the view stays the same.
	*/
	auto set_absolute_bottom(float const p_bottom, bool const will_keep_height = true) -> void {
		if (auto const offset = p_bottom - _absolute_position.y + _bounds.top - _bounds.bottom) {
			auto const change = create_bounds_change();
			if (will_keep_height) {
				_bounds.move_y(offset);
				move_absolute_positions(0, offset);
			}
			else {
				_bounds.bottom += offset;
			}
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the coordinate of the bottom edge of this view relative to the top edge of the GUI.
	*/
	auto get_absolute_bottom() const noexcept -> float {
		return _absolute_position.y + _bounds.bottom - _bounds.top;
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Returns whether this view intersects/overlaps a rectangle that is relative to the top left corner of the parent.
		If you have a custom clipping geometry, you could override this.
	*/
	auto get_is_intersecting(Rectangle<> const rectangle) const noexcept -> bool override
	{
		if (_corners.top_left_size_x && _corners.top_left_size_y || 
			_corners.top_right_size_x && _corners.top_right_size_y ||
			_corners.bottom_left_size_x && _corners.bottom_left_size_y || 
			_corners.bottom_right_size_x && _corners.bottom_right_size_y)
		{
			if (_bounds.get_is_intersecting(rectangle))
			{
				if (rectangle.right < _bounds.left + _corners.top_left_size_x && 
					rectangle.bottom < _bounds.top + _corners.top_left_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.left + _corners.top_left_size_x - rectangle.right, 
							(_bounds.top + _corners.top_left_size_y - rectangle.bottom)*
							_corners.top_left_size_x/_corners.top_left_size_y
						) < _corners.top_left_size_x*_corners.top_left_size_x;
					}
					return rectangle.bottom > _bounds.top + _corners.top_left_size_y - 
						(rectangle.right - _bounds.left)*_corners.top_left_size_y/_corners.top_left_size_x;
				}
				else if (rectangle.right < _bounds.left + _corners.bottom_left_size_x && 
							rectangle.top > _bounds.bottom - _corners.bottom_left_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.left + _corners.bottom_left_size_x - rectangle.right, 
							(_bounds.bottom - _corners.bottom_left_size_y - rectangle.top)*
							_corners.bottom_left_size_x/_corners.bottom_left_size_y
						) < _corners.bottom_left_size_x*_corners.bottom_left_size_x;
					}
					return rectangle.top < _bounds.bottom - _corners.bottom_left_size_y + 
						(rectangle.right - _bounds.left)*_corners.bottom_left_size_y/_corners.bottom_left_size_x;
				}
				else if (rectangle.left > _bounds.right - _corners.top_right_size_x && 
					rectangle.bottom < _bounds.top + _corners.top_right_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.right - _corners.top_right_size_x - rectangle.left, 
							(_bounds.top + _corners.top_right_size_y - rectangle.bottom)*
							_corners.top_right_size_x/_corners.top_right_size_y
						) < _corners.top_right_size_x*_corners.top_right_size_x;
					}
					return rectangle.bottom > _bounds.top + (_bounds.right - rectangle.left)*
						_corners.top_right_size_y/_corners.top_right_size_x;
				}
				else if (rectangle.left > _bounds.right - _corners.bottom_right_size_x && 
					rectangle.top > _bounds.bottom - _corners.bottom_right_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.right - _corners.bottom_right_size_x - rectangle.left, 
							(_bounds.bottom - _corners.bottom_right_size_y - rectangle.top)*
							_corners.bottom_right_size_x/_corners.bottom_right_size_y
						) < _corners.bottom_right_size_x*_corners.bottom_right_size_x;
					}
					return rectangle.top < _bounds.bottom - (_bounds.right - rectangle.left)*
						_corners.bottom_right_size_y/_corners.bottom_right_size_x;
				}
				return true;
			}
			return false;
		}
		return _bounds.get_is_intersecting(rectangle);
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Returns whether a rectangle can be contained within this view. The rectangle is relative to the parent of this view.
		If you have a custom clipping geometry, you could override this.
	*/
	auto get_is_containing(Rectangle<> const rectangle) const noexcept -> bool override
	{
		if (_corners.top_left_size_x && _corners.top_left_size_y || _corners.top_right_size_x && _corners.top_right_size_y ||
			_corners.bottom_left_size_x && _corners.bottom_left_size_y || _corners.bottom_right_size_x && _corners.bottom_right_size_y)
		{
			if (_bounds.get_is_containing(rectangle))
			{
				if (rectangle.left < _bounds.left + _corners.top_left_size_x && 
					rectangle.top < _bounds.top + _corners.top_left_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						if (Vector2d<>::get_length_squared(
								_bounds.left + _corners.top_left_size_x - rectangle.left, 
								(_bounds.top + _corners.top_left_size_y - rectangle.top)*_corners.top_left_size_x/_corners.top_left_size_y
							) > _corners.top_left_size_x*_corners.top_left_size_x)
						{
							return false;
						}
					}
					else if (rectangle.top > _bounds.top + _corners.top_left_size_y - (rectangle.left - _bounds.left)*
								_corners.top_left_size_y/_corners.top_left_size_x)
					{
						return false;
					}
				}
				else if (rectangle.left < _bounds.left + _corners.bottom_left_size_x && 
							rectangle.bottom > _bounds.bottom - _corners.bottom_left_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						if (Vector2d<>::get_length_squared(
								_bounds.left + _corners.bottom_left_size_x - rectangle.left, 
								(_bounds.bottom - _corners.bottom_left_size_y - rectangle.bottom)*
								_corners.bottom_left_size_x/_corners.bottom_left_size_y
							) > _corners.bottom_left_size_x*_corners.bottom_left_size_x)
						{
							return false;
						}
					}
					else if (rectangle.bottom < _bounds.bottom - _corners.bottom_left_size_y + 
						(_bounds.right - rectangle.left)*_corners.top_right_size_y/_corners.top_right_size_x)
					{
						return false;
					}
				}
				else if (rectangle.right > _bounds.right - _corners.top_right_size_x && 
							rectangle.top < _bounds.top + _corners.top_right_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						if (Vector2d<>::get_length_squared(
								_bounds.right - _corners.top_right_size_x - rectangle.right, 
								(_bounds.top + _corners.top_right_size_y - rectangle.top)*
								_corners.top_right_size_x/_corners.top_right_size_y
							) > _corners.top_right_size_x*_corners.top_right_size_x)
						{
							return false;
						}
					}
					else if (rectangle.top > _bounds.top + (_bounds.right - rectangle.right)*
						_corners.top_right_size_y/_corners.top_right_size_y)
					{
						return false;
					}
				}
				else if (rectangle.right > _bounds.right - _corners.bottom_right_size_x && 
							rectangle.bottom > _bounds.bottom - _corners.bottom_right_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						if (Vector2d<>::get_length_squared(
								_bounds.right - _corners.bottom_right_size_x - rectangle.right, 
								(_bounds.bottom - _corners.bottom_right_size_y - rectangle.bottom)*
								_corners.bottom_right_size_x/_corners.bottom_right_size_y
							) > _corners.bottom_right_size_x*_corners.bottom_right_size_x)
						{
							return false;
						}
					}
					else if (rectangle.bottom < _bounds.bottom - (_bounds.right - rectangle.right)*
						_corners.bottom_right_size_y/_corners.bottom_right_size_x)
					{
						return false;
					}
				}
				return true;
			}
			return false;
		}
		return _bounds.get_is_containing(rectangle);
	}

	/*
		LIBRARY IMPLEMENTED
		Returns whether a point is within the bounds of this view. The point is relative to the parent of this view.
		If you have a custom clipping geometry, you could override this.
	*/
	auto get_is_containing(Point<> const p_point) const noexcept -> bool override
	{
		auto const [x, y] = p_point;
		if (_corners.top_left_size_x && _corners.top_left_size_y || _corners.top_right_size_x && _corners.top_right_size_y ||
			_corners.bottom_left_size_x && _corners.bottom_left_size_y || _corners.bottom_right_size_x && _corners.bottom_right_size_y)
		{
			if (_bounds.get_is_containing(p_point))
			{
				if (x < _bounds.left + _corners.top_left_size_x && y < _bounds.top + _corners.top_left_size_y)
				{
					if (_corners.top_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.left + _corners.top_left_size_x - x, 
							(_bounds.top + _corners.top_left_size_y - y)*_corners.top_left_size_x/_corners.top_left_size_y
						) < _corners.top_left_size_x*_corners.top_left_size_x;
					}
					return y > _bounds.top + _corners.top_left_size_y - (x - _bounds.left)*_corners.top_left_size_y/_corners.top_left_size_x;
				}
				else if (x > _bounds.right - _corners.top_right_size_x && y < _bounds.top + _corners.top_right_size_y)
				{
					if (_corners.top_right_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.right - _corners.top_right_size_x - x, 
							(_bounds.top + _corners.top_right_size_y - y)*_corners.top_right_size_x/_corners.top_right_size_y
						) < _corners.top_right_size_x*_corners.top_right_size_x;
					}
					return y > _bounds.top + (_bounds.right - x)*_corners.top_right_size_y/_corners.top_right_size_x;
				}
				else if (x < _bounds.left + _corners.bottom_left_size_x && y > _bounds.bottom - _corners.bottom_left_size_y)
				{
					if (_corners.bottom_left_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.left + _corners.bottom_left_size_x - x, 
							(_bounds.bottom - _corners.bottom_left_size_y - y)*_corners.bottom_left_size_x/_corners.bottom_left_size_y
						) < _corners.bottom_left_size_x*_corners.bottom_left_size_x;
					}
					return y < _bounds.bottom - _corners.bottom_left_size_y + 
						(x - _bounds.left)*_corners.bottom_left_size_y/_corners.bottom_left_size_x;
				}
				else if (x > _bounds.right - _corners.bottom_right_size_x && y > _bounds.bottom - _corners.bottom_right_size_y)
				{
					if (_corners.bottom_right_type == RectangleCornerType::Round)
					{
						return Vector2d<>::get_length_squared(
							_bounds.right - _corners.bottom_right_size_x - x, 
							(_bounds.bottom - _corners.bottom_right_size_y - y)*_corners.bottom_right_size_x/_corners.bottom_right_size_y
						) < _corners.bottom_right_size_x*_corners.bottom_right_size_x;
					}
					return y < _bounds.bottom - (_bounds.right - x)*_corners.bottom_right_size_y/_corners.bottom_right_size_x;
				}
				return true;
			}
			return false;
		}
		return _bounds.get_is_containing(p_point);
	}
	/*
		LIBRARY IMPLEMENTED
		Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
	*/
	auto get_is_containing_absolute(Point<> const p_point) const noexcept -> bool
	{
		if (_parent)
		{
			return get_is_containing(p_point - _parent->get_absolute_top_left());
		}
		return get_is_containing(p_point);
	}

	//------------------------------
	// Size change events

private:
	auto handle_protected_rectangle_change(Rectangle<> const old_rectangle) -> void override 
	{
		if (old_rectangle != _bounds) {
			if (auto const new_top_left = _bounds.get_top_left(), oldTopLeft = old_rectangle.get_top_left(); 
				oldTopLeft != new_top_left)
			{
				move_absolute_positions(Vector2d{new_top_left - oldTopLeft});
			}
			send_bounds_change_events(old_rectangle);
		}
	}

	virtual auto send_bounds_change_events(Rectangle<> previous_bounds) -> void;
public:
	EventListeners<void(Rectangle<>)> boundsChangeListeners;
	/*
		USER IMPLEMENTED
		Implement this method in your view if you want to update things when the bounds of the view have been changed.
	*/
	virtual auto handle_bounds_change(Rectangle<> previous_bounds) -> void {}

public:
	/*
		Listener signature:
			void (View* target, float previousWidth, float previousHeight)
		target is a pointer to the view that changed size.
		See View::handle_size_change for more info.
	*/
	EventListeners<void(Size<>)> size_change_listeners;
	/*
		LIBRARY IMPLEMENTED
		This calls handle_size_change() by default. Override this method if you need to know the previous size of the view.
	*/
	virtual auto handle_size_change(Size<>) -> void
	{
		handle_size_change();
	}
	/*
		USER IMPLEMENTED
		Implement this method in your view if you want to update things when the size of the view has been changed.
	*/
	virtual auto handle_size_change() -> void {}

	//------------------------------

private:
	RectangleCorners _corners;

public:
	/*
		LIBRARY IMPLEMENTED
		Sets the roundness of the corners of the view. radius is the radius of the corner circles.
	*/
	auto set_corner_radius(float const radius) -> void
	{
		_corners.top_left_size_x = _corners.top_left_size_y = radius;
		_corners.top_right_size_x = _corners.top_right_size_y = radius;
		_corners.bottom_left_size_x = _corners.bottom_left_size_y = radius;
		_corners.bottom_right_size_x = _corners.bottom_right_size_y = radius;
		_corners.top_left_type = _corners.top_right_type = 
			_corners.bottom_left_type = _corners.bottom_right_type = RectangleCornerType::Round;
		update_clip_geometry();
		update_shadow();
	}
	/*
		LIBRARY IMPLEMENTED
		Sets the roundness of the different corners of the view, as the radius of a quarter circle.
	*/
	auto set_corner_radius(
		float const p_topLeftRadius, float const p_topRightRadius, 
		float const p_bottomLeftRadius, float const p_bottomRightRadius
	) -> void
	{
		_corners.top_left_size_x = _corners.top_left_size_y = p_topLeftRadius;
		_corners.top_right_size_x = _corners.top_right_size_y = p_topRightRadius;
		_corners.bottom_left_size_x = _corners.bottom_left_size_y = p_bottomLeftRadius;
		_corners.bottom_right_size_x = _corners.bottom_right_size_y = p_bottomRightRadius;
		_corners.top_left_type = _corners.top_right_type = 
			_corners.bottom_left_type = _corners.bottom_right_type = RectangleCornerType::Round;
		update_clip_geometry();
		update_shadow();
	}

	/*
		LIBRARY IMPLEMENTED
		Sets how much the corners of the view are cut.
	*/
	auto setCornerCutSize(float const p_cutSize) -> void
	{
		_corners.top_left_size_x = _corners.top_left_size_y = p_cutSize;
		_corners.top_right_size_x = _corners.top_right_size_y = p_cutSize;
		_corners.bottom_left_size_x = _corners.bottom_left_size_y = p_cutSize;
		_corners.bottom_right_size_x = _corners.bottom_right_size_y = p_cutSize;
		_corners.top_left_type = _corners.top_right_type = 
			_corners.bottom_left_type = _corners.bottom_right_type = RectangleCornerType::Cut;
		update_clip_geometry();
		update_shadow();
	}
	/*
		LIBRARY IMPLEMENTED
		Sets how much the corners of the view are cut.
	*/
	auto setCornerCutSize(
		float const top_left_size, float const top_right_size, 
		float const bottom_left_size, float const bottom_right_size
	) -> void
	{
		_corners.top_left_size_x = _corners.top_left_size_y = top_left_size;
		_corners.top_right_size_x = _corners.top_right_size_y = top_right_size;
		_corners.bottom_left_size_x = _corners.bottom_left_size_y = bottom_left_size;
		_corners.bottom_right_size_x = _corners.bottom_right_size_y = bottom_right_size;
		_corners.top_left_type = _corners.top_right_type = _corners.bottom_left_type = _corners.bottom_right_type = RectangleCornerType::Cut;
		update_clip_geometry();
		update_shadow();
	}

	/*
		LIBRARY IMPLEMENTED
		Sets the shapes of the corners of the view.
	*/
	auto set_corners(RectangleCorners const& p_corners) -> void
	{
		_corners = p_corners;
		update_clip_geometry();
		update_shadow();
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the shapes of the corners of the view.
	*/
	auto get_corners() -> RectangleCorners&
	{
		return _corners;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns whether any of the corners of the view are non-rectangular.
	*/
	auto getHasCornerStyles() const -> bool
	{
		return _corners.top_left_size_x && _corners.top_left_size_y || _corners.top_right_size_x && _corners.top_right_size_y ||
			_corners.bottom_left_size_x && _corners.bottom_left_size_y || _corners.bottom_right_size_x && _corners.bottom_right_size_y;
	}

	//------------------------------

	EventListeners<void(KeyboardEvent const&)> characterInputListeners;
	/*
		USER IMPLEMENTED
		This method is called when a character key has been pressed.
		Only event.character and event.isRepeated are valid for this event type.
	*/
	virtual auto handle_character_input(KeyboardEvent const& event) -> void {}

	EventListeners<void(KeyboardEvent const&)> keyboard_key_down_listeners;
	/*
		USER IMPLEMENTED
		This method is called when a keyboard key has been pressed.
		Only event.key and event.isRepeated are valid for this event type.
	*/
	virtual auto handle_keyboard_key_down(KeyboardEvent const& event) -> void {}

	EventListeners<void(KeyboardEvent const&)> keyboard_key_up_listeners;
	/*
		USER IMPLEMENTED
		This method is called when a keyboard key has been released.
		Only event.key is valid for this event type.
	*/
	virtual auto handleKeyboardKeyUp(KeyboardEvent const& event) -> void {}

	EventListeners<void()> keyboard_focus_lose_listeners;
	/*
		USER IMPLEMENTED
		Gets called when another keyboard event listener becomes the target of keyboard events.
	*/
	virtual auto handleKeyboardFocusLose() -> void {}

	EventListeners<void()> keyboard_focus_gain_listeners;
	/*
		USER IMPLEMENTED
		Gets called when this keyboard event listener becomes the target of keyboard events.
	*/
	virtual auto handle_keyboard_focus_gain() -> void {}

	//------------------------------

private:
	bool _are_drag_drop_events_enabled = false;
	bool _is_dragging_over = false;
public:
	/*
		LIBRARY IMPLEMENTED
		Drag drop events are disabled by default.
	*/
	auto enableDragDropEvents() -> void
	{
		_are_drag_drop_events_enabled = true;
	}
	/*
		LIBRARY IMPLEMENTED
		Drag drop events are disabled by default.
	*/
	auto disableDragDropEvents() -> void
	{
		_are_drag_drop_events_enabled = false;
	}
	/*
		LIBRARY IMPLEMENTED
		Drag drop events are disabled by default.
	*/
	auto get_are_drag_drop_events_enabled() const -> bool
	{
		return _are_drag_drop_events_enabled;
	}

	/*
		LIBRARY IMPLEMENTED (only default behavior)
		This is used to tell the OS what type of operation the application supports for the dragged data when dropped at the mouse location specified by event.x and event.y.
		The coordinates are relative to the top-left corner of this view.
	*/
	virtual auto get_drag_drop_operation(DragDropEvent const& event) const -> DragDropOperation
	{
		return DragDropOperation::None;
	}

	EventListeners<void(DragDropEvent const&)> drag_drop_enter_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the cursor enters the bounds of the view during a drag and drop operation.
		event contains information about the event, including the data and data type of what is to be dropped.
	*/
	virtual auto handle_drag_drop_enter(DragDropEvent const& event) -> void {}

	EventListeners<void(DragDropEvent const&)> dragDropBackgroundEnterListeners;
	/*
		USER IMPLEMENTED
		Gets called when the cursor enters the parts of the view that are not occupied by children, during a drag and drop operation.
		event contains information about the event, including the data and data type of what is to be dropped.
	*/
	virtual auto handle_drag_drop_background_enter(DragDropEvent const& event) -> void {}

	EventListeners<void(DragDropEvent const&)> dragDropMoveListeners;
	/*
		USER IMPLEMENTED
		Gets called when the cursor moves over the view during a drag and drop operation.
	*/
	virtual auto handle_drag_drop_move(DragDropEvent const& event) -> void {}

	EventListeners<void(DragDropEvent const&)> drag_drop_leave_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the cursor leaves the bounds of the view during a drag and drop operation.
		event contains information about the event, including the data and data type of what is to be dropped.
	*/
	virtual auto handle_drag_drop_leave(DragDropEvent const& event) -> void {}

	EventListeners<void(DragDropEvent const&)> dragDropBackgroundLeaveListeners;
	/*
		USER IMPLEMENTED
		Gets called when the cursor leaves the parts of the view that are not occupied by children, during a drag and drop operation.
		event contains information about the event, including the data and data type of what is to be dropped.
	*/
	virtual auto handle_drag_drop_background_leave(DragDropEvent const& event) -> void {}

	EventListeners<void(DragDropEvent const&)> drag_drop_finish_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the user drops data above the view, finishing a drag and drop operation.
		event contains information about the event, including the data and data type of what was dropped.
	*/
	virtual auto handle_drag_drop_finish(DragDropEvent const& event) -> void {}

	//------------------------------

private:
	bool _are_mouse_events_enabled = false;
	bool _is_mouse_hovering = false;
public:
	/*
		LIBRARY IMPLEMENTED
		Mouse events are disabled by default.
	*/
	auto enable_mouse_events() -> void {
		_are_mouse_events_enabled = true;
	}
	/*
		LIBRARY IMPLEMENTED
		Mouse events are disabled by default.
	*/
	auto disable_mouse_events() -> void {
		_are_mouse_events_enabled = false;
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto get_are_mouse_events_enabled() const -> bool {
		return _are_mouse_events_enabled;
	}

	using MouseListener = void(MouseEvent const&);

	EventListeners<MouseListener> mouse_down_listeners;
	/*
		USER IMPLEMENTED
		Gets called when a mouse button has been pressed down while the pointer is above the view.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_down(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_up_listeners;
	/*
		USER IMPLEMENTED
		Gets called when a mouse button has been released after having been pressed down when the mouse pointer was above the view.
		The mouse cursor may have left the view during the time the button is pressed, but it will still receive the event.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_up(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_move_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the mouse pointer has been moved within the bounds of the view.
		This can be blocked by non-overlay views which have the same parent and are overlapping this one.
		The event is never blocked by children of this view.
		If it has entered the view, a mouse enter event is sent, and if it has left the view, a mouse leave event is sent.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_move(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_enter_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the mouse pointer has entered the bounds of the view.
		This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
		that the mouse has entered gets the event (except for overlay views, they always get the event as long as they are targeted).
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_enter(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_leave_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the mouse cursor has left the bounds of the view.
		This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
		that the mouse has left gets the event (except for overlay views, they always get the event as long as they are targeted).
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_leave(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_background_enter_listeners;
	/*
		LIBRARY IMPLEMENTED (only default behavior)
		Gets called when the mouse pointer has entered any part of the view that is not occupied by children of this view.
		By default, this changes the mouse cursor to the cursor that is set with set_cursor on the view.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_background_enter(MouseEvent const& event) -> void;

	EventListeners<MouseListener> mouse_background_leave_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the mouse pointer has left any part of the view that is not occupied by children of this view.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_background_leave(MouseEvent const& event) -> void {}

	EventListeners<MouseListener> mouse_scroll_listeners;
	/*
		USER IMPLEMENTED
		Gets called when the mouse wheel has been moved/scrolled while the mouse pointer is above the view.
		event is an object that contains information about the mouse event.
	*/
	virtual auto handle_mouse_scroll(MouseEvent const& event) -> void {}

	//------------------------------

	/*
		Listener signature:
			void (View* attachedChild)
		See View::handle_child_view_attachment for more info.
	*/
	EventListeners<void(View*)> child_view_attachment_listeners;
	/*
		USER IMPLEMENTED
		Gets called when a child view has been added to this view.
	*/
	virtual auto handle_child_view_attachment(View* attached_child) -> void {}

	/*
		Listener signature:
			void (View* attachedChild)
		See View::handle_child_view_detachment for more info.
	*/
	EventListeners<void(View*)> child_view_detachment_listeners;
	/*
		USER IMPLEMENTED
		Gets called when a child view has been removed from this view.
	*/
	virtual auto handle_child_view_detachment(View* detached_child) -> void {}
};

//------------------------------

// TODO: continue switch to trailing return types

/*
	The highest, "root" view in the view hierarchy.
	It is connected to a window which it holds and receives events from.
*/
class Gui : public View {
private:
	friend View;
	friend Animation;

	static Count s_number_of_instances;

	//------------------------------

	auto handle_theme_color_change(Id const id, Color const p_newColor) -> void override
	{
		if (id == ThemeColors::background) {
			get_drawing_context()->setBackgroundColor(p_newColor);
		}
	}

public:
	/*
		LIBRARY IMPLEMENTED
		This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
		A call to GUI::createContent will be made when these objects have been created and can be used.
		After that, an initial call to GUI::handle_size_change will also be made.

		run must be called after creation and before the main thread returns.

		title is the text that appears in the title bar of the window (if it has an OS border).
		size is the size of the client area in DIPs (device independent pixels).
		p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
		parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
	*/
	auto create(
		std::string_view const title, Size<Dip> const size, 
		WindowStyleFlags const p_windowFlags = WindowStyleFlags::Default, 
		Gui* const parent = nullptr
	) -> void
	{
		create(title, Point{0.5f}, size, p_windowFlags, parent);
	}
	/*
		LIBRARY IMPLEMENTED
		This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
		A call to GUI::createContent will be made when these objects have been created and can be used.
		After that, an initial call to GUI::handle_size_change will also be made.

		run must be called after creation and before the main thread returns.

		title is the text that appears in the title bar of the window (if it has an OS border).
		position_factor is the position of the window, expressed as horizontal and vertical factors between 0 and 1, 
		where (0, 0) means the top left corner of the primary monitor and the top left edge of the window are aligned, 
		and (1, 1) means the bottom right corners are aligned.
		size is the size of the client area in DIPs (device independent pixels).
		p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
		parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
	*/
	auto create(
		std::string_view title, 
		Point<Factor> position_factor, Size<Dip> size, 
		WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, 
		Gui* parent = nullptr
	) -> void;

private:
	Gui* _parent = nullptr;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns the GUI that owns the parent window of the window of this GUI.
		If the window does not have a parent, it returns 0.
	*/
	auto get_parent() const noexcept -> Gui* {
		return _parent;
	}

	//------------------------------

	/*
		LIBRARY IMPLEMENTED
		Returns the topmost non-overlay view which contains the coordinates given.
	*/
	auto get_view_at(Point<> coordinates) -> View*;

	//------------------------------

private:
	TimerThread<std::recursive_mutex> _timer_thread{_sharedStateMutex};
public:
	/*
		Adds a function that will be called in duration from now
		and returns an ID that identifies the timer callback.
	*/
	template<typename _Callback, typename DurationTime, typename _DurationPeriod>
	auto add_timer_callback(
		_Callback const& callback, 
		chrono::duration<DurationTime, _DurationPeriod> const duration
	) -> Id {
		return _timer_thread.add_callback(callback, duration);
	}
	/*
		Adds a function that will be called in milliseconds milliseconds from now
		and returns an ID that identifies the timer callback.
	*/
	template<typename _Callback>
	auto add_timer_callback(_Callback const& callback, float const milliseconds) -> Id {
		return _timer_thread.add_callback(callback, milliseconds);
	}
	auto cancelTimerCallback(Id const id) -> void {
		_timer_thread.cancel_callback(id);
	}

	//------------------------------

private:
	std::deque<View*> _viewAnimationUpdateQueue;
	std::deque<Animation*> _animationUpdateQueue;

	bool _hasAnimationLoopStarted = false;
	std::thread _animation_thread;

	auto thread_run_animation_loop() -> void;
	
public:
	/*
		LIBRARY IMPLEMENTED
		
		This method starts allowing events to be received, and starts drawing. 
		The content of the GUI should be created between a create and a run call.
		If this is the last GUI instance among the currently existing ones to run, the method waits for all GUIs to finish before returning.
		The GUI is forgotten when the window closes.
	*/
	auto run() -> void {
		/*
			The size of the GUI might have changed.
			In that case, corresponding size event(s) will be caused later by a window size change event.
		*/
		if (get_size() == _window->get_size())
		{
			View::send_bounds_change_events({});
		}
		invalidate();

		_window->run();
		_animation_thread = std::thread{&Gui::thread_run_animation_loop, this};

		static std::vector<Gui*> s_instances_to_join;
		static std::mutex s_instances_to_join_mutex;
		static bool s_is_waiting_for_instances_to_finish = false;

		if (s_is_waiting_for_instances_to_finish)
		{
			_animation_thread.detach();
		}
		else
		{
			auto const lock = std::scoped_lock{s_instances_to_join_mutex};
			s_instances_to_join.push_back(this);
		}
		if (s_number_of_instances == s_instances_to_join.size() && !s_is_waiting_for_instances_to_finish) {
			s_is_waiting_for_instances_to_finish = true;

			s_instances_to_join_mutex.lock();
			auto instancesToJoin = std::move(s_instances_to_join);
			s_instances_to_join = {};
			s_instances_to_join_mutex.unlock();

			for (auto& instance : instancesToJoin) {
				instance->_animation_thread.join();
			}
			s_is_waiting_for_instances_to_finish = false;
		}
	}

	//------------------------------

private:
	std::recursive_mutex _sharedStateMutex;
public:
	/*
		LIBRARY IMPLEMENTED
		Gives the running thread exclusive access to modify any state that is shared by the event thread and animation thread.
	*/
	auto lock_threads() -> void {
		_sharedStateMutex.lock();
	}
	/*
		LIBRARY IMPLEMENTED
		Gives back the other threads access to modify any state that is shared by the event thread and animation thread.
	*/
	auto unlock_threads() -> void {
		_sharedStateMutex.unlock();
	}
	/*
		LIBRARY IMPLEMENTED

	*/
	auto create_thread_lock() {
		return std::scoped_lock{_sharedStateMutex};
	}

	//------------------------------

private:
	auto handle_window_create(WindowEvent const& event) -> void;

	Size<> _last_updated_window_size;
	auto handle_window_size_change(WindowEvent const& event) -> void;

	auto send_bounds_change_events(Rectangle<> const previous_bounds) -> void override {
		if (auto const size = get_size();
			_window->dips_to_pixels(size) != _window->dips_to_pixels(_window->get_size()))
		{
			_window->set_size(size);
		}
		else {
			View::send_bounds_change_events(previous_bounds);
		}
	}

	//------------------------------

	/*
		This exists because we have both "mouse-" and "drag_drop-" events which behave in exactly the same way,
		but are separate. Therefore we define some generic cursor event dispatch algorithms which take
		a struct of view members which are needed for the algorithms. I know it feels backwards
		to make a struct of view member pointers here instead of just actually putting the members in a struct
		in the view class instead, but that's not really possible because half of the members are
		public EventListeners.
	*/
	template<typename _EventType>
	struct CursorEventInfo {
		_EventType& event;
		bool View::* areEventsEnabledMember;
		bool View::* is_cursor_over_member;
	};

	template<typename _EventType>
	struct CursorMoveEnterLeaveEventInfo : CursorEventInfo<_EventType>
	{
		bool wasActuallyMoved; // Only enter/leave events are sent if this is true.
		EventListeners<void(_EventType const&)> View::* cursorMoveListenersMember;
		EventListeners<void(_EventType const&)> View::* cursorEnterListenersMember;
		EventListeners<void(_EventType const&)> View::* cursorBackgroundEnterListenersMember;
		EventListeners<void(_EventType const&)> View::* cursorLeaveListenersMember;
		EventListeners<void(_EventType const&)> View::* cursorBackgroundLeaveListenersMember;
	};
	template<typename _EventType>
	struct CursorDownEventInfo : CursorDownEventInfo<_EventType>
	{
		EventListeners<void(_EventType const&)> View::* cursorDownListenersMember;
	};
	template<typename _EventType>
	struct CursorUpEventInfo : CursorDownEventInfo<_EventType>
	{
		EventListeners<void(_EventType const&)> View::* cursorDownListenersMember;
	};

	template<typename _EventType>
	auto send_cursor_move_enter_leave_events_on_gui(CursorMoveEnterLeaveEventInfo<_EventType> const& p) -> void
	{
		if (get_is_containing(event.xy)) {
			if (get_member(p.areEventsEnabledMember)) {
				if (get_member(p.is_cursor_over_member)) {
					get_member(p.cursorMoveListenersMember)(p.event);
				}
				else {
					get_member(p.cursorEnterListenersMember)(p.event);
					if (!get_has_views()) {
						get_member(p.cursorBackgroundEnterListenersMember)(p.event);
					}
				}
			}
			get_member(p.isCursorOver) = true;
		}
		else if (get_member(p.is_cursor_over_member)) {
			if (get_member(p.areEventsEnabledMember)) {
				get_member(p.cursorLeaveListenersMember)(p.event);
				if (!get_has_views()) {
					get_member(p.cursorBackgroundLeaveListenersMember)(p.event);
				}
			}
			get_member(p.isCursorOver) = false;
		}
	}
	
	template<typename _EventType>
	auto send_cursor_move_enter_leave_events(CursorEventInfo<_EventType> const& p) -> void
	{
		send_cursor_move_enter_leave_events_on_gui(p);
		if (!get_has_views()) {
			return;
		}
		
		auto const this_view = static_cast<Avo::View*>(this);

		auto was_hovering_container_stack = std::stack<bool>{};
		was_hovering_container_stack.push(this_view->get_member(p.is_cursor_over_member));

		auto container = this_view;
		auto start_iterator = rbegin();

		auto const absolute = event.xy;

		while (true)
		{

		}
	}

public:
	/*
		This is used to tell the OS what type of operation is supported for the dragged data.
		Queries the targeted views.
	*/
	auto get_global_drag_drop_operation(DragDropEvent& event) -> DragDropOperation {
		auto const targets = get_top_mouse_listeners_at(event.xy);

		auto const absolute = event.xy;

		auto result = DragDropOperation::None;
		for (View* const target : targets) {
			event.xy = absolute - target->get_absolute_top_left();
			if (auto const operation = target->get_drag_drop_operation(event);
				operation != DragDropOperation::None)
			{
				result = operation;
				break;
			}
		}

		event.xy = absolute;

		return result;
	}

	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_drag_drop_enter(DragDropEvent& event) -> void {
		handle_global_drag_drop_move(event);
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_drag_drop_move(DragDropEvent& event) -> void;
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_drag_drop_leave(DragDropEvent& event) -> void;
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handleGlobalDragDropFinish(DragDropEvent& event) -> void {
		if (_are_drag_drop_events_enabled) {
			handle_drag_drop_finish(event);
		}

		auto const absolute = event.xy;

		auto container = static_cast<View*>(this);
		auto start_position = container->rbegin();

		while (true) {
			auto const hit_view_iterator = std::find_if(
				start_position, container->rend(),
				[&](View* child) {
					if (child->get_is_visible() && child->get_is_containing_absolute(absolute)) {
						if (child->get_are_drag_drop_events_enabled()) {
							event.xy = absolute - child->get_absolute_top_left();
							child->drag_drop_finish_listeners(event);
						}
						return !child->get_is_overlay();
					}
					else {
						return false;
					}
				}
			);
			if (hit_view_iterator == container->rend()) {
				if (!container->get_is_overlay() || container == this) {
					break;
				}
				else {
					auto const parent = container->get_parent<View>();
					start_position = parent->rend() - 1 - container->get_index();
					container = parent;
				}
			}
			else {
				if (auto const hitView = *hit_view_iterator;
					hitView->get_has_views())
				{
					container = hitView;
					start_position = container->rbegin();
				}
				else break;
			}
			
		// loopStart:
		// 	for (Index a = startIndex; a >= 0; a--)
		// 	{
		// 		if (auto const child = container->get_view(a);
		// 		    child->get_is_visible() && child->get_is_containing_absolute(absolute))
		// 		{
		// 			if (child->get_are_drag_drop_events_enabled())
		// 			{
		// 				event.xy = absolute - child->get_absolute_top_left();
		// 				child->drag_drop_finish_listeners(event);
		// 			}
		// 			if (auto const childCount = child->get_number_of_views())
		// 			{
		// 				container = child;
		// 				startIndex = childCount - 1;
		// 				goto loopStart;
		// 			}
		// 			else if (!child->get_is_overlay())
		// 				// This is only used to determine if the outer loop should be exited afterwards.
		// 				hasFoundTopView = true;
		// 				break;
		// 			}
		// 		}
		// 	}

		// 	if (!container->get_is_overlay() || hasFoundTopView || container == this)
		// 	{
		// 		break;
		// 	}

		// 	startIndex = container->get_index() - 1;
		// 	container = container->get_parent<View>();
		}
	}

	EventListeners<void(DragDropOperation)> drag_drop_operation_change_listeners;

	//------------------------------

private:
	/*
		Returns the topmost non-overlay view which contains the coordinates given, as well as any overlay views which are above the non-overlay view.
	*/
	std::vector<View*> get_top_mouse_listeners_at(Point<> coordinates);

	std::vector<View*> _pressed_mouse_event_listeners;
	Point<> _mouse_down_position;
public:
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handleGlobalMouseDown(MouseEvent& event) -> void {
		if (auto const targets = get_top_mouse_listeners_at(event.xy);
			!targets.empty())
		{
			auto const absolute = event.xy;
			for (auto const view : targets) {
				event.xy = absolute - view->get_absolute_top_left();

				view->mouse_down_listeners(event);
				_pressed_mouse_event_listeners.push_back(view);
			}
			_mouse_down_position = absolute;
		}
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handleGlobalMouseUp(MouseEvent& event) -> void {
		if (!_pressed_mouse_event_listeners.empty()) {
			auto const absolute = event.xy;
			for (auto const view : _pressed_mouse_event_listeners) {
				event.xy = absolute - view->get_absolute_top_left();
				view->mouse_up_listeners(event);
			}
			_pressed_mouse_event_listeners.clear();

			if (absolute != _mouse_down_position) {
				event.xy = absolute;
				event.movement = Vector2d{absolute - _mouse_down_position};
				handle_global_mouse_move(event); // This is so that any views that the mouse has entered while pressed get their events.
			}
		}
	}
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_mouse_move(MouseEvent& event) -> void;
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_mouse_leave(MouseEvent& event) -> void;
	/*
		LIBRARY IMPLEMENTED
	*/
	auto handle_global_mouse_scroll(MouseEvent& event) -> void {
		auto const targets = get_top_mouse_listeners_at(event.xy);

		auto const absolute = event.xy;

		if (!targets.empty()) {
			for (auto const view : targets) {
				event.xy = absolute - view->get_absolute_top_left();

				view->mouse_scroll_listeners(event);
			}
			for (auto const view : targets) {
				view->forget();
			}
		}

		event.xy = absolute;
		handle_global_mouse_move(event);
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
	virtual auto get_window_border_area_at_position(Point<> const p) const -> WindowBorderArea
	{
		constexpr float border_width = 5.f;
		constexpr float diagonal_border_width = 7.f;

		auto const size = get_size();

		if (p.y < diagonal_border_width) {
			if (p.x < diagonal_border_width) {
				return WindowBorderArea::TopLeftResize;
			}
			if (p.x >= size.width - diagonal_border_width) {
				return WindowBorderArea::TopRightResize;
			}
			if (p.y < border_width) {
				return WindowBorderArea::TopResize;
			}
		}
		if (p.y >= size.height - diagonal_border_width) {
			if (p.x < diagonal_border_width) {
				return WindowBorderArea::BottomLeftResize;
			}
			if (p.x >= size.width - diagonal_border_width) {
				return WindowBorderArea::BottomRightResize;
			}
			if (p.y >= size.height - border_width) {
				return WindowBorderArea::BottomResize;
			}
		}
		if (p.x < border_width) {
			return WindowBorderArea::LeftResize;
		}
		if (p.x >= size.width - border_width) {
			return WindowBorderArea::RightResize;
		}
		return WindowBorderArea::None;
	}

	//------------------------------

private:
	View* _keyboard_focus = nullptr;
public:
	/*
		LIBRARY IMPLEMENTED
		Sets the keyboard event listener that keyboard events are sent to.
	*/
	auto set_keyboard_focus(View* const p_view) -> void
	{
		if (_keyboard_focus == p_view) {
			return;
		}

		auto const focusBefore = _keyboard_focus;

		_keyboard_focus = p_view;

		if (focusBefore) {
			focusBefore->keyboard_focus_lose_listeners();
		}

		if (p_view) {
			p_view->keyboard_focus_gain_listeners();
		}
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the keyboard event listener that keyboard events are sent to.
	*/
	auto get_keyboard_focus() const noexcept -> View* {
		return _keyboard_focus;
	}
	/*
		LIBRARY IMPLEMENTED
		Returns the keyboard event listener that keyboard events are sent to.
	*/
	template<typename T>
	auto get_keyboard_focus() const noexcept -> T* {
		return dynamic_cast<T*>(_keyboard_focus);
	}

	auto send_global_character_input_events(KeyboardEvent const& event) -> void
	{
		if (_keyboard_focus) {
			_keyboard_focus->characterInputListeners(event);
		}
		globalCharacterInputListeners(event);
	}
	EventListeners<void(KeyboardEvent const&)> globalCharacterInputListeners;

	auto send_global_keyboard_key_down_events(KeyboardEvent const& event) -> void
	{
		if (_keyboard_focus) {
			_keyboard_focus->keyboard_key_down_listeners(event);
		}
		global_keyboard_key_down_listeners(event);
	}
	EventListeners<void(KeyboardEvent const&)> global_keyboard_key_down_listeners;

	auto sendGlobalKeyboardKeyUpEvents(KeyboardEvent const& event) -> void
	{
		if (_keyboard_focus) {
			_keyboard_focus->keyboard_key_up_listeners(event);
		}
		global_keyboard_key_up_listeners(event);
	}
	EventListeners<void(KeyboardEvent const&)> global_keyboard_key_up_listeners;

	//------------------------------

private:
	std::unique_ptr<Window> _window;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns a pointer to the window used by this GUI.
	*/
	auto get_window() const noexcept -> Window* {
		return _window.get();
	}

private:
	std::unique_ptr<DrawingContext> _drawing_context;
	DrawingState _drawing_context_state;
public:
	/*
		LIBRARY IMPLEMENTED
		Returns a pointer to the drawing context used by this GUI.
	*/
	auto get_drawing_context() const noexcept -> DrawingContext* {
		return _drawing_context.get();
	}

	//------------------------------

private:
	std::atomic<bool> _isInvalid = true;
	/*
		LIBRARY IMPLEMENTED
		Redraws the whole GUI.
	*/
	auto draw_views() -> void;
public:
	auto invalidate() -> void {
		_isInvalid = true;
	}
	
	//------------------------------

	Gui();
	Gui(Component* parent);
	~Gui() override;
};

//------------------------------

namespace ThemeColors {

inline Id const tooltip_background;
inline Id const tooltip_on_background;

}
namespace ThemeValues {

inline Id const tooltip_font_size;
/*
	In milliseconds.
*/
inline Id const tooltip_delay;

}

/*
	Shows a short info message about a view.
	The parent of a tooltip is the GUI.
*/
class Tooltip : public View {
private:
	Animation _show_animation{
		get_gui(), get_theme_easing(ThemeEasings::out), 100ms, 
		[this](float const value) {
			_opacity = value;
			invalidate();
		} 
	};
	bool _is_showing = false;

	Id _timerId = 0;

public:
	/*
		Makes the tooltip appear
		string is the string to be displayed on the tooltip.
		p_targetBounds is the area that the tooltip points to and is relative to the parent of this tooltip. 
		The tooltip decides the exact positioning.
	*/
	virtual auto show(std::string_view const string, Rectangle<> const target_rectangle) -> void
	{
		if (!_is_showing) {
			if (!_text || string != _text.get_string()) {
				auto const font_size = get_theme_value(ThemeValues::tooltip_font_size);
				_text = get_gui()->get_drawing_context()->create_text(string, font_size);
				_text.fit_size_to_text();
				set_size(_text.get_size() + Size{2.2f*font_size, 1.8f*font_size});
				_text.set_center(Point{get_size()/2});
			}

			if (target_rectangle.bottom + 7.f + get_height() >= get_gui()->get_height()) {
				set_bottom(max(1.f, target_rectangle.top - 7.f), true);
			}
			else {
				set_top(target_rectangle.bottom + 7.f, true);
			}
			set_center_x(max(1.f + get_width()/2, min(get_gui()->get_width() - get_width()/2 - 1.f, target_rectangle.get_center_x())));

			_is_showing = true;
			_timerId = get_gui()->add_timer_callback([this] {
				_opacity = 0.f;
				_show_animation.play(false);
			}, get_theme_value(ThemeValues::tooltip_delay));
		}
	}

	/*
		Makes the tooltip disappear.
	*/
	virtual auto hide() -> void {
		if (_is_showing) {
			get_gui()->cancelTimerCallback(_timerId);
			_is_showing = false;
			if (_opacity) {
				_show_animation.play(true);
			}
		}
	}

	//------------------------------

private:
	Text _text;
	float _opacity = 0.f;
public:
	auto draw(DrawingContext* const drawing_context) -> void override {
		if (_text) {
			drawing_context->scale(_opacity*0.3f + 0.7f, get_absolute_center());
			drawing_context->set_color({get_theme_color(ThemeColors::tooltip_background), _opacity});
			drawing_context->fill_rectangle(get_size());
			drawing_context->set_color(Color(get_theme_color(ThemeColors::tooltip_on_background), _opacity));
			drawing_context->draw_text(_text);
			drawing_context->scale(1.f/(_opacity*0.3f + 0.7f), get_absolute_center());
		}
	}

	//------------------------------

	Tooltip(View* parent) :
		View{parent}
	{
		initialize_theme_color(ThemeColors::tooltip_background, {0.2f, 0.8f});
		initialize_theme_color(ThemeColors::tooltip_on_background, {1.f, 0.95f});
		initialize_theme_value(ThemeValues::tooltip_font_size, 12.f);
		initialize_theme_value(ThemeValues::tooltip_delay, 400.f);

		set_has_shadow(false);
		set_elevation(-1.f);
		set_corner_radius(2.f);
		set_is_overlay(true); // Don't want to block any events from reaching views below the tooltip, especially not when it has faded away.
	}
};

//------------------------------

class OpenFileDialog {
public:
	struct FileExtensionFilter {
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
	bool _can_select_multiple_files = false;
public:
	/*
		If this is true, the user can select more than 1 file to open.
	*/
	constexpr auto set_can_select_multiple_files(bool const can_select_multiple_files) noexcept -> void {
		_can_select_multiple_files = can_select_multiple_files;
	}
	[[nodiscard]]
	constexpr auto get_can_select_multiple_files() const noexcept -> bool {
		return _can_select_multiple_files;
	}

private:
	std::string _title = "Open file...";
public:
	/*
		Sets the title shown in the top border of the open file dialog.
	*/
	auto set_title(std::string_view title) noexcept -> void {
		_title = title;
	}
	/*
		Returns the title shown in the thop border of the open file dialog.
	*/
	[[nodiscard]]
	auto get_title() const noexcept -> std::string_view {
		return _title;
	}

private:
	std::vector<FileExtensionFilter> _file_extensions;
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
	auto set_file_extensions(std::vector<FileExtensionFilter>&& file_extensions) noexcept -> void {
		_file_extensions = std::move(file_extensions);
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
	auto set_file_extensions(std::span<FileExtensionFilter const> const file_extensions) -> void {
		_file_extensions.resize(file_extensions.size());
		std::ranges::copy(file_extensions, _file_extensions.begin());
	}
	/*
		Returns the file extension filters that can be selected in the dialog.
	*/
	[[nodiscard]]
	auto get_file_extensions() const -> std::span<FileExtensionFilter const> {
		return _file_extensions;
	}

	/*
		Opens the dialog and returns when the user has selected the files or closed the window.
		Returns a vector which contains the file paths in UTF-8 format of the files the user has selected to open.
		It can be empty if the user closed the window without selecting any files.
	*/
	auto open() const -> std::vector<std::string>;

private:
	Gui* _gui;
public:
	OpenFileDialog() :
		_gui{nullptr}
	{}
	OpenFileDialog(Gui* const gui) :
		_gui{gui}
	{}
};

//------------------------------

/*
	A view that displays text.
*/
class TextView : public View {
private:
	Color _color = get_theme_color(ThemeColors::on_background);
public:
	auto set_color(Color color) noexcept -> void {
		_color = color;
	}
	auto get_color() const noexcept -> Color {
		return _color;
	}

private:
	float _font_size;
public:
	auto set_font_size(float font_size) -> void {
		_font_size = font_size;
		if (_text) {
			_text.set_font_size(font_size);
		}
	}
	auto get_font_size() const noexcept -> float {
		return _font_size;
	}

private:
	Text _text;
public:
	auto set_string(std::string_view string) -> void {
		if (string.empty()) {
			return;
		}
		_text = get_drawing_context()->create_text(string, _font_size);
		_text.set_is_top_trimmed(true);
		//_text.set_size(get_size());
		set_size(_text.get_size() + 1.f);
	}
	auto set_text(Text p_text) -> void {
		_text = p_text;
	}
	auto get_text() -> Text& {
		return _text;
	}

	auto fit_size_to_text() -> void {
		if (_text) {
			_text.fit_size_to_text();
			set_size(_text.get_size());
		}
	}
	auto fit_width_to_text() -> void {
		if (_text) {
			_text.fit_width_to_text();
			set_width(_text.get_width());
		}
	}
	auto fit_height_to_text() -> void {
		if (_text) {
			_text.fit_height_to_text();
			set_height(_text.get_height());
		}
	}

	auto handle_size_change() -> void override {
		if (_text) {
			_text.set_size(get_size());
		}
	}

	auto draw(DrawingContext* context) -> void override {
		if (_text) {
			context->set_color(_color);
			context->draw_text(_text);
		}
	}

	//------------------------------

	TextView(View* parent, float font_size, std::string_view string = "") :
		View{parent},
		_font_size{font_size}
	{
		set_string(string);
	}
};

//------------------------------

namespace ThemeEasings {
	
inline Id const ripple;

}

namespace ThemeValues {

/*
	In milliseconds
*/
inline Id const rippleDuration;

}

/*
	A view that shows a ripple effect when you click it and optionally shows a hover effect when the mouse hovers over it.
	It is a mouse event overlay which means views behind this view are targeted as if this view didn't exist.
*/
class Ripple : public View {
private:
	bool _is_enabled = true;
public:
	/*
		Disables the ripple and hover effects.
	*/
	constexpr auto disable() noexcept -> void {
		_is_enabled = false;
	}
	/*
		Enables the ripple and hover effects.
	*/
	constexpr auto enable() noexcept -> void {
		_is_enabled = true;
	}
	/*
		Returns whether the ripple and hover effects are enabled.
	*/
	[[nodiscard]]
	constexpr auto get_is_enabled() const noexcept -> bool {
		return _is_enabled;
	}

	//------------------------------

private:
	Color _color;
public:
	/*
		Sets the color that is used by the ripple and hover effects.
	*/
	constexpr auto set_color(Color const color) noexcept -> void {
		_color = color;
	}
	/*
		Returns the color that is used by the ripple and hover effects.
	*/
	[[nodiscard]]
	constexpr auto get_color() const noexcept -> Color {
		return _color;
	}

	//------------------------------

private:
	bool _has_hover_effect = true;
public:
	/*
		Sets whether the view will be lightly highlighted when the mouse hovers over it.
		This is true by default and is recommended since it indicates that the view can be pressed.
	*/
	constexpr auto set_has_hover_effect(bool has_hover_effect) noexcept -> void {
		_has_hover_effect = has_hover_effect;
	}
	/*
		Returns whether the view will be lightly highlighted when the mouse hovers over it.
	*/
	[[nodiscard]]
	constexpr auto get_has_hover_effect() const noexcept -> bool {
		return _has_hover_effect;
	}

private:
	Point<> _position;
	float _max_size{};
public:
	auto update_max_size() noexcept -> void {
		_max_size = 2.f*Point<>::get_distance(_position, Point{
			_position.x < get_width()*0.5f ? get_width() : 0.f,
			_position.y < get_height()*0.5f ? get_height() : 0.f
		});
	}

private:
	Initializer init_theme = [=] {
		initialize_theme_easing(ThemeEasings::ripple, {0.1, 0.8, 0.2, 0.95});
		initialize_theme_value(ThemeValues::rippleDuration, 300);
	};

	float _size = 0.f;
	Animation _ripple_animation {
		get_gui(), get_theme_easing(ThemeEasings::ripple), get_theme_value(ThemeValues::rippleDuration),
		[=](float value) {
			_size = interpolate(Range{_max_size*0.4f, _max_size}, value);
			_alpha_factor = 1.f;
			if (!_is_mouse_down && value == 1.f) {
				_ripple_fade_animation.replay();
			}
			invalidate();
		}
	};

	bool _is_mouse_down = false;
	Initializer init_press = [=] {
		auto mouse_down_listener = [=](MouseEvent const& event) {
			if (_is_enabled && event.mouse_button == MouseButton::Left) {
				_ripple_fade_animation.stop();

				_is_mouse_down = true;
				_position = event.xy;
				_alpha_factor = 1.f;
				update_max_size();

				_ripple_animation.replay();
			}
		};
		mouse_down_listeners += mouse_down_listener;
	};

	float _alpha_factor = 0.f;
	Animation _ripple_fade_animation {
		get_gui(), get_theme_easing(ThemeEasings::in_out), 400ms,
		[=](float value) {
			_alpha_factor = 1.f - value;
			invalidate();
		}
	};

	Initializer init_release = [=] {
		mouse_up_listeners += [=](MouseEvent const& event) {
			if (_is_mouse_down && event.mouse_button == MouseButton::Left) {
				_is_mouse_down = false;
				if (_size == _max_size && _alpha_factor == 1.f) {
					_ripple_animation.stop();
					_ripple_fade_animation.replay();
				}
			}
		};
	};

	float _overlay_alpha_factor = 0.f;
	Animation _hover_animation {
		get_gui(), get_theme_easing(ThemeEasings::in_out), get_theme_value(ThemeValues::hover_animation_duration),
		[=](float value) {
			_overlay_alpha_factor = value;
			invalidate();
		}
	};

	Initializer init_hover = [=] {
		mouse_background_enter_listeners += [=](auto const&) {
			_hover_animation.play(false);
		};
		mouse_background_leave_listeners += [=](auto const&) {
			_hover_animation.play(true);
		};
	};

	//------------------------------

public:
	void draw(DrawingContext* drawing_context) override {
		if (_is_enabled) {
			drawing_context->set_color({_color, _color.alpha*_overlay_alpha_factor*0.3f});
			drawing_context->fill_rectangle(get_size());

			if (_color.alpha*_alpha_factor >= 0.f) {
				drawing_context->set_color({_color, _color.alpha*_alpha_factor*0.8f});
				drawing_context->fill_circle(_position, _size*0.5f);
			}
		}
	}

	Ripple(View* parent, Color color = {1.f, 0.45f}) :
		View{parent, parent->get_size()},
		_color{color}
	{
		set_is_overlay(true); // Mouse events should be sent through
		set_has_shadow(false);
		set_elevation(std::numeric_limits<float>::max());
		enable_mouse_events();

		parent->size_change_listeners += [=](auto) {
			set_size(get_parent<View>()->get_size());
			update_max_size();
		};
	}
};

//------------------------------

namespace ThemeValues {

inline Id const button_font_size;
inline Id const button_character_spacing;

}

// TOOD: Update/remake Button class

class Button : public View {
public:
	enum class Emphasis {
		Low, // Text
		Medium, // Outlined
		High // Raised
	};

private:
	Text _text;

	Tooltip* _tooltip_view = nullptr;
	std::string _tooltip_string;

	Image _icon;

	float _press_animation_time = 1.f;
	bool _is_pressed = false;
	bool _is_raising = false;
	Emphasis _emphasis;

	bool _is_enabled = true;
	Color _current_color;
	float _color_animation_time = 1.f;
	bool _is_accent = false;

	bool _is_mouse_hovering = false;

	Ripple* _ripple = nullptr;

protected:
	void handle_theme_value_change(Id id, float p_newValue) override {
		if (id == ThemeValues::button_font_size) {
			_text.set_font_size(p_newValue);
			if (id == ThemeValues::button_character_spacing) {
				_text.set_character_spacing(p_newValue);
			}
			update_size();
		}
		else if (id == ThemeValues::button_character_spacing) {
			_text.set_character_spacing(p_newValue);
			update_size();
		}
	}
	void handle_theme_color_change(Id id, Color p_newColor) override {
		if (_emphasis == Emphasis::High) {
			if (id == (_is_accent ? ThemeColors::secondary : ThemeColors::primary) ||
				id == (_is_accent ? ThemeColors::on_secondary : ThemeColors::on_primary))
			{
				_current_color = p_newColor;
			}
		}
		else if (id == (_is_accent ? ThemeColors::secondary_on_background : ThemeColors::primary_on_background))
		{
			_current_color = p_newColor;
			_ripple->set_color(Color(p_newColor, 0.3f));
		}
	}

public:
	explicit Button(View* parent, std::string_view p_text = "", Emphasis emphasis = Emphasis::High, bool is_accent = false) :
		View{parent},
		_emphasis{emphasis}
	{
		initialize_theme_value(ThemeValues::button_font_size, 14.f);
		initialize_theme_value(ThemeValues::button_character_spacing, 1.f);

		set_string(p_text);

		set_corner_radius(4.f);

		_ripple = add_view<Ripple>();
		_ripple->set_cursor(Cursor::Hand);

		set_is_accent(is_accent);
		if (emphasis == Emphasis::High) {
			set_elevation(2.f);
		}

		enable_mouse_events();
	}

	//------------------------------

	void update_size() {
		if (_text) {
			auto const size_factor = get_theme_value(ThemeValues::button_font_size)/14.f;
			if (_icon) {
				_icon.set_size(16.f*size_factor);
				_icon.set_center({size_factor*38.f*0.5f, get_height()*0.5f});

				_text.set_left(38.f*size_factor);
				set_size({std::round(_text.get_width()) + size_factor*(16.f + 38.f), 36.f*size_factor});
			}
			else {
				if (_text.get_width() >= 32.f*size_factor) {
					set_size(Size{std::round(_text.get_width()) + 32.f, 36.f}*size_factor);
				}
				else {
					set_size(Size{64.f, 36.f}*size_factor);
				}
				_text.set_center(get_center() - get_top_left());
			}
		}
		else if (_icon) {
			_icon.set_center(get_center() - get_top_left());
		}
	}

	//------------------------------

	/*
		Makes the user unable to use the button and makes it gray.
	*/
	void disable() {
		if (_is_enabled) {
			_is_enabled = false;
			_color_animation_time = 1.f;
			queue_animation_update();

			_ripple->disable();

			//if (_is_mouse_hovering)
			//{
			//	get_gui()->get_window()->set_cursor(Cursor::Arrow);
			//}
		}
	}

	/*
		Makes the user able to use the button.
	*/
	void enable() {
		if (!_is_enabled) {
			_is_enabled = true;
			_color_animation_time = 0.f;
			queue_animation_update();

			_ripple->enable();

			//if (_is_mouse_hovering)
			//{
			//	get_gui()->get_window()->set_cursor(get_cursor());
			//}
		}
	}

	/*
		Returns whether the user can use the button.
	*/
	bool get_is_enabled() {
		return _is_enabled;
	}

	//------------------------------

	/*
		Sets whether the button uses the secondary/accent color. If not, it uses the primary color. The button uses primary color by default.
	*/
	auto set_is_accent(bool const is_accent) -> void {
		_is_accent = is_accent;
		if (_emphasis == Emphasis::High) {
			_current_color = _is_accent ? get_theme_color(ThemeColors::secondary) : get_theme_color(ThemeColors::primary);
			_ripple->set_color(Color(_is_accent ? get_theme_color(ThemeColors::on_secondary) : get_theme_color(ThemeColors::on_primary), 0.3f));
		}
		else {
			_current_color = _is_accent ? get_theme_color(ThemeColors::secondary_on_background) : get_theme_color(ThemeColors::primary_on_background);
			_ripple->set_color(Color(_is_accent ? get_theme_color(ThemeColors::secondary_on_background) : get_theme_color(ThemeColors::primary_on_background), 0.3f));
		}
	}
	/*
		Returns whether the button uses the secondary/accent color. If not, it uses the primary color. The button uses primary color by default.
	*/
	auto get_is_accent() const noexcept -> bool {
		return _is_accent;
	}

	//------------------------------

	/*
		Sets the string that the button displays.
	*/
	auto set_string(std::string_view string) -> void {
		if (string[0]) {
			_text = get_gui()->get_drawing_context()->create_text(string, get_theme_value(ThemeValues::button_font_size));
			_text.set_word_wrapping(WordWrapping::Never);
			_text.set_character_spacing(get_theme_value(ThemeValues::button_character_spacing));
			_text.set_font_weight(FontWeight::Medium);
			//_text.set_is_top_trimmed(true);
			_text.fit_size_to_text();
		}
		else {
			_text.destroy();
		}
		update_size();
	}

	/*
		Returns the string that the button displays.
	*/
	auto get_string() const -> std::string_view {
		if (_text) {
			return _text.get_string();
		}
		return "";
	}

	/*
		Returns the text object that is used to display the button label.
	*/
	auto get_text() const -> Text {
		return _text;
	}

	//------------------------------

	/*
		Sets an image to be shown together with the text. Unless you have remembered the image yourself, your are transferring ownership of the image to the button.
		It is best to keep a text label with the icon, unless it is very clear to all users what the button does with the icon alone, or if you have set a tooltip.
		If icon is 0, the icon is removed.
	*/
	auto set_icon(Image const& icon) -> void {
		if (icon != _icon) {
			if (icon) {
				_icon = icon;
				_icon.set_bounds_sizing(ImageBoundsSizing::Contain);
			}
			else {
				_icon.destroy();
			}
			update_size();
			invalidate();
		}
	}

	/*
		Returns the image that is shown together with the button text.
	*/
	auto get_icon() const -> Image {
		return _icon;
	}

	//------------------------------

	/*
		Sets a string to be shown as a tooltip when the mouse hovers over the button.
		Should give the user additional information about the button's purpose.
		An empty string disables the tooltip.
	*/
	auto set_tooltip(Tooltip* const tooltip_view, std::string_view const p_info) -> void {
		_tooltip_view = tooltip_view;
		_tooltip_string = p_info;
	}

	//------------------------------

	EventListeners<void(Button*)> button_click_listeners;

	//------------------------------

	auto handle_mouse_background_enter(MouseEvent const& event) override -> void  {
		if (_tooltip_view && !_tooltip_string.empty()) {
			_tooltip_view->show(_tooltip_string, get_absolute_bounds());
		}
	}
	auto handle_mouse_move(MouseEvent const& event) override -> void {
		_is_mouse_hovering = true;
	}
	auto handle_mouse_background_leave(MouseEvent const& event) override -> void {
		if (_tooltip_view && !_tooltip_string.empty()) {
			_tooltip_view->hide();
		}
		_is_mouse_hovering = false;
	}
	auto handle_mouse_down(MouseEvent const& event) override -> void  {
		if (event.mouse_button == MouseButton::Left && _is_enabled && _emphasis == Emphasis::High) {
			_is_pressed = true;
			_is_raising = true;
			_press_animation_time = 0.f;
			queue_animation_update();
		}
	}
	void handle_mouse_up(MouseEvent const& event) override {
		if (event.mouse_button == MouseButton::Left) {
			if (_emphasis == Emphasis::High) {
				_is_pressed = false;
				queue_animation_update();
			}
			if (_is_enabled && get_is_containing(Point{event.x + getLeft(), event.y + getTop()})) {
				button_click_listeners(this);
			}
		}
	}

	//------------------------------

	auto update_animations() -> void override {
		if ((_color_animation_time != 1.f && _is_enabled) || (_color_animation_time != 0.f && !_is_enabled)) {
			auto const color_animation_value = get_theme_easing(ThemeEasings::symmetrical_in_out).ease_value(_color_animation_time);
			if (_emphasis == Emphasis::High) {
				_current_color = _is_accent ? get_theme_color(ThemeColors::secondary) : get_theme_color(ThemeColors::primary);
			}
			else {
				_current_color = _is_accent ? 
					get_theme_color(ThemeColors::secondary_on_background) : 
					get_theme_color(ThemeColors::primary_on_background);
			}
			_current_color.setSaturationHSL(color_animation_value);

			if (_is_enabled) {
				if (_color_animation_time < 1.f) {
					_color_animation_time = min(1.f, _color_animation_time + 0.1f);
					queue_animation_update();
				}
			}
			else {
				if (_color_animation_time > 0.f) {
					_color_animation_time = max(0.f, _color_animation_time - 0.1f);
					queue_animation_update();
				}
			}
		}

		if (_emphasis == Emphasis::High) {
			auto const press_animation_value = get_theme_easing(ThemeEasings::in_out).ease_value(_press_animation_time);
			_press_animation_time += 0.06f;

			if (_is_raising || _is_pressed) {
				set_elevation(2.f + press_animation_value*4.f);
				if (!_is_pressed && press_animation_value == 1.f) {
					_press_animation_time = 0.f;
					_is_raising = false;
					queue_animation_update();
				}
			}
			else {
				set_elevation(2.f + (1.f - press_animation_value)*4.f);
			}

			if (press_animation_value < 1.f) {
				queue_animation_update();
			}
		}

		invalidate();
	}


	//------------------------------

	void drawOverlay(DrawingContext* drawing_context) override {
		if (_emphasis == Emphasis::Medium) {
			drawing_context->set_color(Color(get_theme_color(ThemeColors::on_background), 0.25f));
			drawing_context->stroke_rounded_rectangle(Rectangle<>(0.5f, 0.5f, get_width() - 0.5f, get_height() - 0.5f), get_corners().top_left_size_x, 1.f);
		}
	}

	void draw(DrawingContext* drawing_context) override {
		if (_emphasis == Emphasis::High) {
			drawing_context->clear(_current_color);
			drawing_context->set_color(_is_accent ? get_theme_color(ThemeColors::on_secondary) : get_theme_color(ThemeColors::on_primary));
		}
		else {
			drawing_context->set_color(_current_color);
		}

		if (_icon) {
			drawing_context->drawImage(_icon);
		}

		if (_text) {
			drawing_context->draw_text(_text);
		}
	}
};

//------------------------------

namespace ThemeValues {

inline Id const editable_text_caret_blink_rate;

}

// TODO: Update/remake EditableText

/*
	A view that only consists of text that can be edited by the user.
*/
class EditableText : public View {
private:
	Text _text;
	float _text_drawing_offset_x = 0.f;
	float _font_size;
	TextAlign _text_align = TextAlign::Left;

	Index _caret_character_index = 0;
	Index _caret_byte_index = 0;
	Point<> _caret_position;
	bool _isCaretVisible = false;
	Count _caretFrameCount = 0;

	Index _selectionEndCharacterIndex = 0;
	Index _selectionEndByteIndex = 0;
	Point<> _selection_end_position;
	bool _isSelectingWithMouse = false;
	bool _isSelectionVisible = false;

	//------------------------------

	auto update_caret_tracking() -> void {
		if (!_text) {
			return;
		}

		if (_caret_position.x + _text_drawing_offset_x > get_width())
		{
			_text_drawing_offset_x = get_width() - _caret_position.x;
		}
		else if (_caret_position.x + _text_drawing_offset_x < 0.f)
		{
			_text_drawing_offset_x = -_caret_position.x;
		}

		if (_text_align == TextAlign::Left)
		{
			if (_text.get_minimum_width() > get_width())
			{
				if (_text.get_minimum_width() + _text_drawing_offset_x < get_width())
				{
					_text_drawing_offset_x = get_width() - _text.get_minimum_width();
				}
			}
			else
			{
				_text_drawing_offset_x = 0.f;
			}
		}
		else if (_text_align == TextAlign::Right)
		{
			if (_text.get_minimum_width() > get_width())
			{
				if (get_width() - _text.get_minimum_width() + _text_drawing_offset_x > 0.f)
				{
					_text_drawing_offset_x = _text.get_minimum_width() - get_width();
				}
			}
			else
			{
				_text_drawing_offset_x = 0;
			}
		}
	}
	void updateSelectionEndTracking()
	{
		if (_selection_end_position.x + _text_drawing_offset_x > get_width())
		{
			_text_drawing_offset_x = get_width() - _selection_end_position.x;
		}
		else if (_selection_end_position.x + _text_drawing_offset_x < 0.f)
		{
			_text_drawing_offset_x = -_selection_end_position.x;
		}
	}

public:
	explicit EditableText(View* parent, float width = 0.f, float font_size = 12.f) :
		View(parent, Rectangle<>(0.f, 0.f, width, font_size*1.2f)),
		_font_size(font_size)
	{
		initialize_theme_value(ThemeValues::editable_text_caret_blink_rate, 20);

		set_cursor(Cursor::Ibeam);
		enable_mouse_events();
	}

	//------------------------------

	/*
		Listeners that get called when the text is about to be changed, either by the user or programmatically.
		
		Listener signature:
			bool (EditableText* target, std::string& new_string, newCaretCharacterIndex)
		new_string is the string that will be set if all listeners return true from this handler. Otherwise, the string is left unchanged.
		new_string can be modified, and the contents of the string after all listeners have handled the event is what will be set as the new text.
		newCaretCharacterIndex works in a similar way, and it is the index of the cursor showing where new user input is inserted.
		This index can be equal to the size of the new string, and in that case the cursor ends up at the end of the text.
	*/
	EventListeners<bool(EditableText*, std::string&, Index&)> editableTextChangeListeners;

	/*
		Listeners that get called when the user has pressed the enter/return key while p_editableText has keyboard focus.
	*/
	EventListeners<void(EditableText*)> editable_text_enter_listeners;

	//------------------------------

private:
	auto handleDoubleClick(MouseEvent const& event) -> void
	{
		if (_text)
		{	
			auto const string = _text.get_string();

			auto const clickCharacterIndex = _text.get_nearest_character_index({event.x - _text_drawing_offset_x, event.y}, true);
			auto const clickUnitIndex = getUnitIndexFromCharacterIndex(string, clickCharacterIndex);
			
			auto leftBound = string.rfind(' ', clickUnitIndex);
			if (leftBound == std::string_view::npos)
			{
				leftBound = 0;
			}
			_caret_character_index = getCharacterIndexFromUnitIndex(string, leftBound);
			_caret_byte_index = leftBound;
			_caret_position = _text.get_character_position(_caret_character_index, true);
			
			if (auto const rightBound = string.find(' ', clickUnitIndex);
				rightBound == std::string_view::npos)
			{
				_selectionEndCharacterIndex = get_number_of_characters_in_string(string);
				_selectionEndByteIndex = string.size();
			}
			else
			{
				_selectionEndCharacterIndex = getCharacterIndexFromUnitIndex(string, rightBound);
				_selectionEndByteIndex = rightBound;
			}
			_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex);

			// _caret_character_index = 0;
			// _caret_byte_index = 0;

			// auto characterIndex = Index{};
			// for (auto byteIndex : Indices{string, 1})
			// {
			// 	if (byteIndex == string.size() || get_is_unit_start_of_character(string[byteIndex]))
			// 	{
			// 		if (byteIndex == string.size() || string[byteIndex] == ' ')
			// 		{
			// 			if (characterIndex >= clickCharacterIndex)
			// 			{
			// 				_selectionEndCharacterIndex = characterIndex;
			// 				_selectionEndByteIndex = byteIndex;
			// 				_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex);
			// 				_caret_position = _text.get_character_position(_caret_character_index, true);
			// 				update_caret_tracking();
			// 				break;
			// 			}
			// 			else
			// 			{
			// 				_caret_character_index = characterIndex + 1;
			// 				_caret_byte_index = byteIndex + 1;
			// 			}
			// 		}
			// 		++characterIndex;
			// 	}
			// }
			if (_caret_character_index != _selectionEndCharacterIndex)
			{
				_isSelectionVisible = true;
				invalidate();
			}
		}
	}
public:
	void handle_mouse_down(MouseEvent const& event) override
	{
		if (event.isDoubleClick)
		{
			handleDoubleClick(event);
		}
		else
		{
			if (_text)
			{
				if (event.modifier_keys & ModifierKeyFlags::Shift)
				{
					std::tie(_selectionEndCharacterIndex, _selection_end_position) = 
						_text.get_nearest_character_index_and_position({event.x - _text_drawing_offset_x, event.y}, true);
					
					_selectionEndByteIndex = getUnitIndexFromCharacterIndex(_text.get_string(), _selectionEndCharacterIndex);

					if (_selectionEndCharacterIndex == _caret_character_index)
					{
						_caretFrameCount = 1;
						_isCaretVisible = true;
						_isSelectionVisible = false;
					}
					else
					{
						updateSelectionEndTracking();
						_isSelectionVisible = true;
					}
					_isSelectingWithMouse = true;
				}
				else
				{
					std::tie(_caret_character_index, _caret_position) = 
						_text.get_nearest_character_index_and_position({event.x - _text_drawing_offset_x, event.y}, true);

					_caret_byte_index = getUnitIndexFromCharacterIndex(_text.get_string(), _caret_character_index);
					update_caret_tracking();

					_isCaretVisible = true;
					_caretFrameCount = 1;
					_isSelectingWithMouse = true;
					_isSelectionVisible = false;
				}
			}
			else
			{
				//set_string("");
			}

			get_gui()->set_keyboard_focus(this);

			invalidate();
			queue_animation_update();
		}
	}
	void handle_mouse_move(MouseEvent const& event) override
	{
		if (_isSelectingWithMouse)
		{
			std::tie(_selectionEndCharacterIndex, _selection_end_position) = 
				_text.get_nearest_character_index_and_position({event.x - _text_drawing_offset_x, 0}, true);

			_selectionEndByteIndex = getUnitIndexFromCharacterIndex(_text.get_string(), _selectionEndCharacterIndex);
			updateSelectionEndTracking();
			_isSelectionVisible = _selectionEndCharacterIndex != _caret_character_index;
			_isCaretVisible = true;
			_caretFrameCount = 1;
			invalidate();
		}
	}
	void handle_mouse_up(MouseEvent const& event) override
	{
		_isSelectingWithMouse = false;
	}

	void handle_keyboard_focus_gain() override
	{
		_caretFrameCount = 1;
		_isCaretVisible = true;

		queue_animation_update();
		invalidate();
	}
	void handleKeyboardFocusLose() override
	{
		_caretFrameCount = 1;
		_isCaretVisible = false;
		_isSelectionVisible = false;

		invalidate();
	}
	void handle_character_input(KeyboardEvent const& event) override
	{
		if (event.character > u8"\u001f" && (event.character < u8"\u007f" || event.character > u8"\u009f"))
		{
			auto string = std::string{_text ? _text.get_string() : ""};
			if (_isSelectionVisible)
			{
				if (_caret_character_index <= _selectionEndCharacterIndex)
				{
					string.erase(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index);
				}
				else
				{
					string.erase(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex);
					_caret_character_index = _selectionEndCharacterIndex;
					_caret_byte_index = _selectionEndByteIndex;
				}
				_isSelectionVisible = false;
			}

			string.insert(_caret_byte_index, event.character);

			set_string(string, _caret_character_index + 1);

			update_caret_tracking();

			_caretFrameCount = 1;
			_isCaretVisible = true;

			invalidate();
		}
	}
	void handle_keyboard_key_down(KeyboardEvent const& event) override
	{
		Window* window = get_window();

		auto string = _text ? std::string{_text.get_string()} : "";

		if (_isSelectionVisible && 
			(event.key == KeyboardKey::Backspace || event.key == KeyboardKey::Delete) && 
			_caret_character_index != _selectionEndCharacterIndex)
		{
			if (_caret_character_index <= _selectionEndCharacterIndex)
			{
				string.erase(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index);
				_isSelectionVisible = false;
				set_string(string);
				update_caret_tracking();
			}
			else
			{
				string.erase(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex);
				_isSelectionVisible = false;
				set_string(string, _selectionEndCharacterIndex);
				update_caret_tracking();
			}
			if (_text_align == TextAlign::Center && _text)
			{
				_caret_position = _text.get_character_position(_caret_character_index);
				update_caret_tracking();
			}

			_caretFrameCount = 1;
			_isCaretVisible = true;
			invalidate();
			return;
		}
		switch (event.key)
		{
			case KeyboardKey::Backspace:
			{
				if (!_text)
				{
					return;
				}
				if (!_isSelectionVisible && _caret_character_index > 0)
				{
					if (window->get_is_key_down(KeyboardKey::Control))
					{
						Index characterIndex = _caret_character_index - 1;
						for (Index byteIndex = _caret_byte_index - 1; byteIndex >= 0; byteIndex--)
						{
							if (get_is_unit_start_of_character(string[byteIndex]))
							{
								if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
								{
									string.erase(byteIndex, _caret_byte_index - byteIndex);
									set_string(string, characterIndex);
									break;
								}
								characterIndex--;
							}
						}
					}
					else {
						for (Index byteIndex = _caret_byte_index - 1; byteIndex >= 0; byteIndex--) {
							if (auto const numberOfBytesInCharacter = get_number_of_units_in_character(string[byteIndex])) {
								set_string(string.erase(byteIndex, numberOfBytesInCharacter), _caret_character_index - 1);
								break;
							}
						}
					}
				}
				_caretFrameCount = 1;
				_isCaretVisible = true;
				_isSelectionVisible = false;
				break;
			}
			case KeyboardKey::Delete: {
				if (!_text) {
					return;
				}
				if (!_isSelectionVisible && _caret_byte_index < string.size()) {
					if (window->get_is_key_down(KeyboardKey::Control)) {
						for (auto const byteIndex : Range{_caret_byte_index, string.size() - 1})
						{
							if (byteIndex == string.size() - 1 || (string[byteIndex + 1] == ' ' && string[byteIndex] != ' '))
							{
								string.erase(_caret_byte_index, byteIndex - _caret_byte_index + 1);
								set_string(string);
								break;
							}
						}
					}
					else {
						set_string(string.erase(_caret_byte_index, get_number_of_units_in_character(string[_caret_byte_index])));
					}
				}
				_caretFrameCount = 1;
				_isCaretVisible = true;
				_isSelectionVisible = false;
				break;
			}
			case KeyboardKey::Left: {
				if (!_text) {
					return;
				}
				if (window->get_is_key_down(KeyboardKey::Control))
				{
					if (window->get_is_key_down(KeyboardKey::Shift))
					{
						if (!_isSelectionVisible)
						{
							_selectionEndCharacterIndex = _caret_character_index;
							_selectionEndByteIndex = _caret_byte_index;
						}
						Index characterIndex = _selectionEndCharacterIndex - 1;
						for (Index byteIndex = _selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
						{
							if (get_is_unit_start_of_character(string[byteIndex]))
							{
								if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
								{
									_selectionEndByteIndex = byteIndex;
									_selectionEndCharacterIndex = characterIndex;
									if (_selectionEndCharacterIndex == _caret_character_index)
									{
										_isSelectionVisible = false;
									}
									else {
										_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
										updateSelectionEndTracking();
										_isSelectionVisible = true;
									}
									break;
								}
								characterIndex--;
							}
						}
					}
					else
					{
						Index characterIndex = _caret_character_index - 1;
						for (Index byteIndex = _caret_byte_index - 1; byteIndex >= 0; byteIndex--)
						{
							if (get_is_unit_start_of_character(string[byteIndex]))
							{
								if (!byteIndex || (string[byteIndex - 1U] == ' ' && string[byteIndex] != ' '))
								{
									_caret_byte_index = byteIndex;
									_caret_character_index = characterIndex;
									_caret_position = _text.get_character_position(_caret_character_index, true);
									update_caret_tracking();
									_isSelectionVisible = false;
									break;
								}
								characterIndex--;
							}
						}
					}
				}
				else if (window->get_is_key_down(KeyboardKey::Shift)) {
					if (!_isSelectionVisible) {
						_selectionEndCharacterIndex = _caret_character_index;
						_selectionEndByteIndex = _caret_byte_index;
					}
					if (_selectionEndCharacterIndex > 0)
					{
						for (Index byteIndex = _selectionEndByteIndex - 1; byteIndex >= 0; byteIndex--)
						{
							if (get_is_unit_start_of_character(string[byteIndex]))
							{
								_selectionEndCharacterIndex--;
								_selectionEndByteIndex = byteIndex;
								if (_selectionEndCharacterIndex == _caret_character_index) {
									_isSelectionVisible = false;
								}
								else {
									_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
									updateSelectionEndTracking();
									_isSelectionVisible = true;
								}
								break;
							}
						}
					}
				}
				else {
					if (_isSelectionVisible) {
						if (_caret_character_index > _selectionEndCharacterIndex) 
						{
							_caret_character_index = _selectionEndCharacterIndex;
							_caret_byte_index = _selectionEndByteIndex;
							_caret_position = _selection_end_position;
						}
						update_caret_tracking();
						_isSelectionVisible = false;
					}
					else {
						if (_caret_character_index > 0)
						{
							for (Index byteIndex = _caret_byte_index - 1; byteIndex >= 0; byteIndex--)
							{
								if (get_is_unit_start_of_character(string[byteIndex]))
								{
									_caret_character_index--;
									_caret_byte_index = byteIndex;
									_caret_position = _text.get_character_position(_caret_character_index, true);
									update_caret_tracking();
									break;
								}
							}
						}
					}
				}
				_caretFrameCount = 1;
				_isCaretVisible = true;
				invalidate();
				break;
			}
			case KeyboardKey::Right: {
				if (!_text) {
					return;
				}
				if (window->get_is_key_down(KeyboardKey::Control)) {
					if (window->get_is_key_down(KeyboardKey::Shift)) {
						if (!_isSelectionVisible) {
							_selectionEndCharacterIndex = _caret_character_index;
							_selectionEndByteIndex = _caret_byte_index;
						}
						Index characterIndex = _selectionEndCharacterIndex;
						for (Index byteIndex = _selectionEndByteIndex + 1; byteIndex <= string.size(); byteIndex++) 
						{
							if (byteIndex == string.size() || get_is_unit_start_of_character(string[byteIndex]))
							{
								characterIndex++;
								if (byteIndex == string.size() || string[byteIndex] == ' ' && string[byteIndex - 1] != ' ') 
								{
									_selectionEndByteIndex = byteIndex;
									_selectionEndCharacterIndex = characterIndex;
									if (_selectionEndCharacterIndex == _caret_character_index) {
										_isSelectionVisible = false;
									}
									else {
										_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
										updateSelectionEndTracking();
										_isSelectionVisible = true;
									}
									break;
								}
							}
						}
					}
					else {
						Index characterIndex = _caret_character_index;
						for (Index byteIndex = _caret_byte_index + 1; byteIndex <= string.size(); byteIndex++) {
							if (byteIndex == string.size() || get_is_unit_start_of_character(string[byteIndex])) 
							{
								characterIndex++;
								if (byteIndex == string.size() || string[byteIndex] == ' ' && string[byteIndex - 1] != ' ') 
								{
									_caret_byte_index = byteIndex;
									_caret_character_index = characterIndex;
									_caret_position = _text.get_character_position(_caret_character_index, true);
									update_caret_tracking();
									_isSelectionVisible = false;
									break;
								}
							}
						}
					}
				}
				else if (window->get_is_key_down(KeyboardKey::Shift)) {
					if (!_isSelectionVisible) {
						_selectionEndCharacterIndex = _caret_character_index;
					}
					if (_selectionEndByteIndex < string.size()) {
						_selectionEndByteIndex += get_number_of_units_in_character(string[_selectionEndByteIndex]);
						_selectionEndCharacterIndex++;
						if (_selectionEndCharacterIndex == _caret_character_index) {
							_isSelectionVisible = false;
						}
						else {
							_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
							updateSelectionEndTracking();
							_isSelectionVisible = true;
						}
					}
				}
				else {
					if (_isSelectionVisible) {
						if (_caret_character_index < _selectionEndCharacterIndex) {
							_caret_character_index = _selectionEndCharacterIndex;
							_caret_byte_index = _selectionEndByteIndex;
							_caret_position = _selection_end_position;
							update_caret_tracking();
						}
						_isSelectionVisible = false;
					}
					else {
						if (_caret_byte_index < string.size()) {
							_caret_byte_index += get_number_of_units_in_character(string[_caret_byte_index]);
							_caret_character_index++;
							_caret_position = _text.get_character_position(_caret_character_index, true);
							update_caret_tracking();
						}
					}
				}
				_caretFrameCount = 1;
				_isCaretVisible = true;
				invalidate();
				break;
			}
			case KeyboardKey::C: {
				if (!_text) {
					return;
				}
				if (window->get_is_key_down(KeyboardKey::Control) && _isSelectionVisible) {
					if (_caret_character_index < _selectionEndCharacterIndex) {
						window->set_clipboard_string(string.substr(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index));
					}
					else {
						window->set_clipboard_string(string.substr(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex));
					}
				}
				break;
			}
			case KeyboardKey::X: {
				if (!_text) {
					return;
				}
				if (window->get_is_key_down(KeyboardKey::Control) && _isSelectionVisible) {
					if (_caret_character_index < _selectionEndCharacterIndex) {
						window->set_clipboard_string(string.substr(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index));
						string.erase(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index);
						set_string(string);
					}
					else {
						window->set_clipboard_string(string.substr(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex));
						string.erase(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex);
						set_string(string, _selectionEndCharacterIndex);
					}

					_isSelectionVisible = false;

					_caretFrameCount = 1;
					_isCaretVisible = true;
				}
				break;
			}
			case KeyboardKey::V: {
				if (window->get_is_key_down(KeyboardKey::Control)) {
					Index caret_character_index = _caret_character_index;
					Index caret_byte_index = _caret_byte_index;
					if (_isSelectionVisible) {
						if (caret_character_index < _selectionEndCharacterIndex) {
							string.erase(_caret_byte_index, _selectionEndByteIndex - _caret_byte_index);
							_selectionEndCharacterIndex = _caret_character_index;
							_selectionEndByteIndex = _caret_byte_index;
						}
						else {
							string.erase(_selectionEndByteIndex, _caret_byte_index - _selectionEndByteIndex);
							caret_character_index = _selectionEndCharacterIndex;
							caret_byte_index = _selectionEndByteIndex;
						}
						_isSelectionVisible = false;
					}
					auto clipboard_data = window->get_clipboard_data();
					auto clipboard_string = clipboard_data->get_string();
					string.insert(caret_byte_index, clipboard_string);
					set_string(string, caret_character_index + get_number_of_characters_in_string<char>(clipboard_string));

					_caretFrameCount = 1;
					_isCaretVisible = true;
				}
				break;
			}
			case KeyboardKey::A: {
				if (!_text) {
					return;
				}
				if (window->get_is_key_down(KeyboardKey::Control)) {
					select_all();
					return;
				}
				break;
			}
			case KeyboardKey::Enter: {
				editable_text_enter_listeners(this);
				break;
			}
		}
	}

	//------------------------------

	/*
		startIndex is the index of the first character to be selected and endIndex is the index of the character after the last selected character.
	*/
	auto setSelection(Index startIndex, Index endIndex) -> void
	{
		if (_text)
		{
			auto const number_of_characters_in_string = get_number_of_characters_in_string(_text.get_string());
			startIndex = min(number_of_characters_in_string, startIndex);
			endIndex = min(number_of_characters_in_string, max(startIndex, endIndex));
			if (startIndex != endIndex)
			{
				if (startIndex != _caret_character_index)
				{
					_caret_character_index = startIndex;
					_caret_byte_index = getUnitIndexFromCharacterIndex(_text.get_string(), _caret_character_index);
					_caret_position = _text.get_character_position(_caret_character_index, true);
				}

				if (endIndex != _selectionEndCharacterIndex)
				{
					_selectionEndCharacterIndex = endIndex;
					_selectionEndByteIndex = getUnitIndexFromCharacterIndex(_text.get_string(), _selectionEndCharacterIndex);
					_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
				}
				_isSelectionVisible = true;
				invalidate();
			}
		}
	}
	/*
		Selects all of the text.
	*/
	auto select_all() -> void {
		if (_text) {
			if (auto const stringLength = _text.get_string().size()) {
				if (_caret_character_index != 0) {
					_caret_character_index = 0;
					_caret_byte_index = 0;
					_caret_position = _text.get_character_position(_caret_character_index, true);
				}

				if (_selectionEndCharacterIndex != stringLength) {
					_selectionEndCharacterIndex = get_number_of_characters_in_string(_text.get_string());
					_selectionEndByteIndex = stringLength;
					_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
				}
				_isSelectionVisible = true;
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
	auto set_string(std::string_view string, Index p_newCaretCharacterIndex = -1) -> void
	{
		if (_text && _text.get_string() == string) {
			return;
		}
		if (p_newCaretCharacterIndex == -1) {
			p_newCaretCharacterIndex = _caret_character_index;
		}

		auto new_string = std::string{string};

		for (auto& listener : editableTextChangeListeners) {
			if (!listener(this, new_string, p_newCaretCharacterIndex)) {
				return;
			}
		}

		if (!new_string.size()) {
			_text.destroy();
			_caret_character_index = 0;
			_caret_byte_index = 0;
			_caret_position.y = 0;
			if (_text_align == TextAlign::Left) {
				_caret_position.x = 0;
			}
			else if (_text_align == TextAlign::Right) {
				_caret_position.x = get_width();
			}
			else if (_text_align == TextAlign::Center) {
				_caret_position.x = get_width()*0.5f;
			}
			_text_drawing_offset_x = 0.f;
			_isSelectionVisible = false;
			invalidate();
			return;
		}

		_text = get_drawing_context()->create_text(new_string, _font_size);
		_text.set_font_weight(FontWeight::Regular);
		_text.set_text_align(_text_align);
		_text.set_width(get_width());
		_text.set_top(2.f);
		_text.set_bottom(get_height(), false);

		auto const characterCount = get_number_of_characters_in_string<char>(new_string);
		if (p_newCaretCharacterIndex > characterCount)
		{
			_caret_byte_index = new_string.size();
			_caret_character_index = characterCount;
		}
		else if (p_newCaretCharacterIndex != _caret_character_index)
		{
			if (p_newCaretCharacterIndex < 0)
			{
				_caret_character_index = 0;
				_caret_byte_index = 0;
			}
			else
			{
				_caret_character_index = p_newCaretCharacterIndex;
				_caret_byte_index = getUnitIndexFromCharacterIndex<char>(new_string, p_newCaretCharacterIndex);
			}
		}
		_caret_position = _text.get_character_position(_caret_character_index, true);
		update_caret_tracking();

		if (_isSelectionVisible) {
			if (_selectionEndByteIndex > new_string.size()) {
				_selectionEndByteIndex = new_string.size();
				_selectionEndCharacterIndex = characterCount;
				if (_selectionEndCharacterIndex == _caret_character_index) {
					_isSelectionVisible = false;
				}
				else {
					_selection_end_position = _text.get_character_position(_selectionEndCharacterIndex, true);
				}
			}
		}
		invalidate();
	}
	/*
		Returns the content of the editable text.
	*/
	auto get_string() const -> std::string_view {
		if (_text) {
			return _text.get_string();
		}
		return "";
	}

	/*
		Sets the content of the editable text as a value.
	*/
	template<typename T>
	auto setValue(T value) -> void {
		set_string(number_to_string(value));
	}
	/*
		Sets the content of the editable text as a value, rounded at a certain digit.
	*/
	template<IsNumber T>
	auto setValue(T const value, Index const p_roundingDigit, RoundingType const p_roundingType = RoundingType::Nearest) -> void
	{
		set_string(number_to_string(value, p_roundingDigit, p_roundingType));
	}
	/*
		Returns the content of the editable text as an arithmetic value.
	*/
	template<typename T>
	auto getValue() -> std::optional<T> {
		return stringToNumber<T>(get_string());
	}

	/*
		Returns the internal text graphics object.
	*/
	auto get_text() const -> Text {
		return _text;
	}

	//------------------------------

	/*
		Sets the way text is horizontally aligned within the bounds of the editable text.
	*/
	auto set_text_align(TextAlign text_align) -> void
	{
		_text_align = text_align;
		if (_text)
		{
			_text.set_text_align(_text_align);
			invalidate();
		}
	}
	/*
		Returns the way text is horizontally aligned within the bounds of the editable text.
	*/
	auto get_text_align() const -> TextAlign
	{
		return _text_align;
	}

	//------------------------------

	auto set_font_size(float font_size) -> void
	{
		_font_size = font_size;
		if (_text)
		{
			_text.set_font_size(font_size);
		}
		set_height(font_size*1.2f);
		invalidate();
	}
	auto get_font_size() const -> float
	{
		return _font_size;
	}

	//------------------------------

	auto handle_size_change() -> void override
	{
		update_caret_tracking();
		if (_text)
		{
			_text.set_width(get_width());
		}
	}

	//------------------------------

	auto update_animations() -> void override
	{
		if (get_gui()->get_keyboard_focus() == this)
		{
			if (_caretFrameCount % (uint32)get_theme_value(ThemeValues::editable_text_caret_blink_rate) == 0 && !_isSelectionVisible)
			{
				_isCaretVisible = !_isCaretVisible;
				invalidate();
			}
			_caretFrameCount++;
			queue_animation_update();
		}
	}

	auto draw(DrawingContext* context) -> void override
	{
		//context->set_color(Color(0.f));
		//context->stroke_rectangle(get_size(), 1.f);
		context->move_origin({_text_drawing_offset_x, 0.f});
		context->set_color(get_theme_color(ThemeColors::on_background));
		if (_text)
		{
			context->draw_text(_text);
			if (_isSelectionVisible)
			{
				context->set_color(get_theme_color(ThemeColors::selection));
				context->fill_rectangle({_caret_position.x, 0.f, _selection_end_position.x, get_height()});
			}
		}
		if (_isCaretVisible && !_isSelectionVisible)
		{
			context->draw_line({_caret_position.x, 0.f}, {_caret_position.x, get_height()}, 1.f);
		}
		context->move_origin({-_text_drawing_offset_x, 0.f});
	}
};

//------------------------------

namespace ThemeValues
{
	inline Id const text_field_font_size;
	inline Id const text_field_height;
	inline Id const text_field_padding_left;
	inline Id const text_field_padding_right;
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

	static constexpr float outlined_padding_label = 5.f;
private:
	Type _type;

	EditableText* _editable_text = add_view<EditableText>();
public:
	auto getEditableText() const noexcept -> EditableText* {
		return _editable_text;
	}

protected:
	auto handle_theme_value_change(Id const id, float const p_newValue) -> void override
	{
		if (id == ThemeValues::text_field_font_size)
		{
			if (_label_text)
			{
				_label_text.set_font_size(p_newValue);
				_label_text.fit_size_to_text();
			}
			if (_prefixText)
			{
				_prefixText.set_font_size(p_newValue);
				_prefixText.fit_size_to_text();
			}
			if (_suffixText)
			{
				_suffixText.set_font_size(p_newValue);
				_suffixText.fit_size_to_text();
			}
			_editable_text->set_font_size(p_newValue);
		}
		if (id == ThemeValues::text_field_font_size || id == ThemeValues::text_field_height)
		{
			// Text positions will be updated in handle_size_change()
			set_height(get_theme_value(ThemeValues::text_field_font_size)*1.2f*get_theme_value(ThemeValues::text_field_height) + outlined_padding_label*(_type == Type::Outlined));
		}
		if (id == ThemeValues::text_field_padding_left)
		{
			if (_label_text)
			{
				_label_text.set_left(p_newValue);
			}
			if (_prefixText)
			{
				_prefixText.set_left(p_newValue);
				_editable_text->set_left(_prefixText.getRight() + 1.f, false);
			}
			else
			{
				_editable_text->set_left(p_newValue, false);
			}
		}
		else if (id == ThemeValues::text_field_padding_right)
		{
			if (_suffixText)
			{
				_suffixText.set_right(get_width() - p_newValue);
				_editable_text->set_right(_suffixText.getLeft() - 1.f, false);
			}
			else
			{
				_editable_text->set_right(get_width() - p_newValue, false);
			}
		}
		else if (id == ThemeValues::textFieldFilledPaddingBottom)
		{
			if (_prefixText)
			{
				_prefixText.set_bottom(get_height() - p_newValue);
			}
			if (_suffixText)
			{
				_suffixText.set_bottom(get_height() - p_newValue);
			}
			_editable_text->set_bottom(get_height() - p_newValue);
		}
	}

public:
	auto handle_size_change() -> void override
	{
		if (_suffixText)
		{
			_suffixText.set_right(get_width() - get_theme_value(ThemeValues::text_field_padding_right));
			_editable_text->set_right(_suffixText.getLeft() - 1.f, false);
		}
		else
		{
			_editable_text->set_right(get_width() - get_theme_value(ThemeValues::text_field_padding_right), false);
		}

		if (_type == Type::Filled)
		{
			auto const bottom = get_height() - get_theme_value(ThemeValues::textFieldFilledPaddingBottom);
			if (_label_text)
			{
				_label_text.set_center_y(get_height()*0.5f);
			}
			if (_prefixText)
			{
				_prefixText.set_bottom(bottom);
			}
			if (_suffixText)
			{
				_suffixText.set_bottom(bottom);
			}
			_editable_text->set_bottom(bottom);
		}
		else
		{
			auto const centerY = outlined_padding_label + (get_height() - outlined_padding_label)*0.5f;
			if (_label_text)
			{
				_label_text.set_center_y(centerY);
			}
			if (_prefixText)
			{
				_prefixText.set_center_y(centerY);
			}
			if (_suffixText)
			{
				_suffixText.set_center_y(centerY);
			}
			_editable_text->set_center_y(centerY);
		}
	}

private:
	Text _label_text;
	Color _label_color;

public:
	auto set_label(std::string_view const p_label) -> void
	{
		if (_label_text)
		{
			if (p_label == _label_text.get_string())
			{
				return;
			}
		}
		if (p_label.empty())
		{
			_label_text.destroy();
		}
		else
		{
			_label_text = get_gui()->get_drawing_context()->create_text(p_label, get_theme_value(ThemeValues::text_field_font_size));
			_label_text.set_font_weight(FontWeight::Regular);
			_label_text.fit_size_to_text();
			if (_type == Type::Filled)
			{
				_label_text.set_center_y(get_height()*0.5f);
			}
			else if (_type == Type::Outlined)
			{
				_label_text.set_center_y(outlined_padding_label + (get_height() - outlined_padding_label)*0.5f);
			}
			queue_animation_update();
		}
	}
	auto getLabel() const -> std::string_view
	{
		if (_label_text)
		{
			return _label_text.get_string();
		}
		return "";
	}

	//------------------------------

private:
	Text _prefixText;
	Text _suffixText;

	auto setAffixString(std::string_view const string, Text& p_affixText) -> bool
	{
		if (p_affixText)
		{
			if (p_affixText.get_string() == string)
			{
				return false;
			}
		}
		if (string.empty())
		{
			p_affixText.destroy();
			return false;
		}
		p_affixText = get_drawing_context()->create_text(string, get_theme_value(ThemeValues::text_field_font_size));
		p_affixText.set_font_weight(FontWeight::Regular);
		p_affixText.set_height(p_affixText.get_font_size()*1.2f);
		if (_type == Type::Filled)
		{
			p_affixText.set_bottom(get_theme_value(ThemeValues::textFieldFilledPaddingBottom));
		}
		else
		{
			p_affixText.set_top(_editable_text->getTop() + 2.f);
		}
		return true;
	}
public:
	auto setPrefixString(std::string_view const string) -> void
	{
		if (setAffixString(string, _prefixText))
		{
			_prefixText.set_left(get_theme_value(ThemeValues::text_field_padding_left));
			_editable_text->set_left(_prefixText.getRight() + 1.f, false);
			if (_label_text)
			{
				_label_text.set_left(_prefixText.getRight() + 1.f);
			}
		}
	}
	auto getPrefixString() const -> std::string_view
	{
		if (_suffixText)
		{
			return _suffixText.get_string();
		}
		return "";
	}

	auto setSuffixString(std::string_view const string) -> void
	{
		if (setAffixString(string, _suffixText))
		{
			_suffixText.set_right(get_width() - get_theme_value(ThemeValues::text_field_padding_right));
			_editable_text->set_right(_suffixText.getLeft() - 1.f, false);
		}
	}
	auto getSuffixString() const -> std::string_view
	{
		if (_suffixText)
		{
			return _suffixText.get_string();
		}
		return "";
	}

	//------------------------------

	auto set_string(std::string_view const string) -> void
	{
		_editable_text->set_string(string);
		if (_type == Type::Filled)
		{
			_editable_text->set_bottom(get_height() - get_theme_value(ThemeValues::textFieldFilledPaddingBottom));
		}
		else if (_type == Type::Outlined)
		{
			_editable_text->set_center_y(outlined_padding_label + (get_height() - outlined_padding_label)*0.5f);
		}
	}
	auto get_string() const -> std::string_view
	{
		return _editable_text->get_string();
	}
	
	/*
		Sets the content of the text field as a value.
	*/
	template<typename T>
	auto setValue(T const value) -> void
	{
		set_string(number_to_string(value));
	}
	/*
		Sets the content of the text field as a value, rounded at a certain digit.
	*/
	template<typename T>
	auto setValue(
		T const value, Index const p_roundingDigit, 
		RoundingType const p_roundingType = RoundingType::Nearest
	) -> void
	{
		set_string(number_to_string(value, p_roundingDigit, p_roundingType));
	}
	template<typename T>
	auto getValue() -> std::optional<T>
	{
		return _editable_text->getValue<T>();
	}

	//------------------------------

	auto set_text_align(TextAlign text_align) -> void {
		_editable_text->set_text_align(text_align);
	}
	auto get_text_align() const -> TextAlign {
		return _editable_text->get_text_align();
	}

	//------------------------------

	void handle_mouse_down(MouseEvent const& event) override {
		MouseEvent event_copy = event;
		event_copy.y = 0;
		event_copy.x -= _editable_text->getLeft();
		_editable_text->handle_mouse_down(event_copy);
	}
	void handle_mouse_up(MouseEvent const& event) override
	{
		MouseEvent event_copy = event;
		event_copy.y = 0;
		event_copy.x -= _editable_text->getLeft();
		_editable_text->handle_mouse_up(event_copy);
	}
	void handle_mouse_move(MouseEvent const& event) override
	{
		MouseEvent event_copy = event;
		event_copy.y = 0;
		event_copy.x -= _editable_text->getLeft();
		_editable_text->handle_mouse_move(event_copy);
	}
	void handle_mouse_enter(MouseEvent const& event) override
	{
		View::handle_mouse_background_enter(event);
		_is_mouse_hovering = true;
		queue_animation_update();
	}
	void handle_mouse_leave(MouseEvent const& event) override
	{
		_is_mouse_hovering = false;
		queue_animation_update();
	}

	void handle_keyboard_focus_gain() override {
		get_gui()->set_keyboard_focus(_editable_text);
	}

	//------------------------------

	/*
		Returns whether the EditableText child of this text field has keyboard focus.
	*/
	bool get_has_keyboard_focus() const {
		return _editable_text == get_gui()->get_keyboard_focus();
	}

	//------------------------------

private:
	float _focus_animation_time = 0.f;
	Factor _focus_animation_value = 0.f;

	bool _is_mouse_hovering = false;
	float _hover_animation_time = 0.f;
	Factor _hover_animation_value = 0.f;

public:
	auto update_animations() -> void override {
		if (get_gui()->get_keyboard_focus() == _editable_text) {
			if (_focus_animation_value < 1.f) {
				_focus_animation_value = get_theme_easing(ThemeEasings::in_out).ease_value(_focus_animation_time);
				_focus_animation_time = min(1.f, _focus_animation_time + 0.09f);
				invalidate();
				queue_animation_update();
			}
		}
		else if (_focus_animation_value > 0.f) {
			_focus_animation_value = 1.f - get_theme_easing(ThemeEasings::in_out).ease_value(1.f - _focus_animation_time);
			_focus_animation_time = max(0.f, _focus_animation_time - 0.09f);
			invalidate();
			queue_animation_update();
		}
		if (_is_mouse_hovering) {
			if (_hover_animation_value < 1.f) {
				_hover_animation_value = get_theme_easing(ThemeEasings::symmetrical_in_out).ease_value(_hover_animation_time);
				_hover_animation_time = min(1.f, _hover_animation_time + get_theme_value(ThemeValues::hover_animation_speed));
				invalidate();
				queue_animation_update();
			}
		}
		else if (_hover_animation_value > 0.f) {
			_hover_animation_value = 1.f - get_theme_easing(ThemeEasings::symmetrical_in_out).ease_value(1.f - _hover_animation_time);
			_hover_animation_time = max(0.f, _hover_animation_time - get_theme_value(ThemeValues::hover_animation_speed));
			invalidate();
			queue_animation_update();
		}
		_label_color = interpolate(
			interpolate(
				get_theme_color(ThemeColors::background), 
				get_theme_color(ThemeColors::on_background), 
				(1.f - _focus_animation_value)*_hover_animation_value*0.3f + 0.4f
			), 
			get_theme_color(ThemeColors::primary_on_background), 
			_focus_animation_value
		);
	}

	void draw(DrawingContext* const context) override {
		if (_type == Type::Filled) {
			context->set_color({
				interpolate(
					get_theme_color(ThemeColors::background), get_theme_color(ThemeColors::on_background), 
					0.05f + 0.05f*min(_hover_animation_value*0.3f + _focus_animation_value, 1.f)
				), 1.f 
			});
			context->fill_rectangle(get_size());
			context->set_color(Color{get_theme_color(ThemeColors::on_background), 0.4f});
			context->draw_line({0.f, get_height() - 1.f}, {get_width(), get_height() - 0.5f}, 1.f);
			if (_focus_animation_value > 0.01f) {
				context->set_color(get_theme_color(ThemeColors::primary_on_background));
				context->draw_line(
					{(1.f - _focus_animation_value)*get_width()*0.5f, get_height() - 1.f}, 
					{(1.f + _focus_animation_value)*get_width()*0.5f, get_height() - 1.f}, 
					2.f
				);
			}
			if (_label_text) {
				Factor label_animation_value = _editable_text->get_string().empty() ? _focus_animation_value : 1.f;
				Dip leftPadding = get_theme_value(ThemeValues::text_field_padding_left);
				context->move_origin({
					leftPadding + 2.f*label_animation_value, 
					-0.17f*(get_height() - _label_text.get_height() - leftPadding)*label_animation_value
				});
				context->set_scale(1.f - label_animation_value*0.3f);
				context->set_color(_label_color);
				context->draw_text(_label_text);
				context->set_scale(1.f);
				context->set_origin(get_absolute_top_left());
			}
		}
		else if (_type == Type::Outlined) {
			context->set_color(_label_color);
			context->stroke_rectangle({1.f, 1.f + outlined_padding_label, get_width() - 1.f, get_height() - 1.f}, get_corners(), _focus_animation_value + 1.f);

			if (_label_text) {
				Factor label_animation_value = _editable_text->get_string().empty() ? _focus_animation_value : 1.f;
				context->move_origin({
					get_theme_value(ThemeValues::text_field_padding_left) + 2.f*label_animation_value, 
					-(get_height() - outlined_padding_label)*0.3f*label_animation_value
				});
				context->set_scale(1.f - label_animation_value*0.3f);

				context->set_color(get_theme_color(ThemeColors::background));
				context->fill_rounded_rectangle(
					{
						_label_text.getLeft() - 4.f, _label_text.getTop(), 
						_label_text.getRight() + 4.f, _label_text.getBottom() 
					}, 2.f
				);

				context->set_color(_label_color);
				context->draw_text(_label_text);

				context->set_scale(1.f);
				context->set_origin(get_absolute_top_left());
			}
		}

		if (_prefixText) {
			context->set_color({get_theme_color(ThemeColors::on_background), 0.5f});
			context->draw_text(_prefixText);
		}
		if (_suffixText) {
			context->set_color({get_theme_color(ThemeColors::on_background), 0.5f});
			context->draw_text(_suffixText);
		}
	}

	//------------------------------

	TextField(
		View* const parent, 
		Type const p_type = Type::Filled, 
		std::string_view const p_label = "", 
		Dip const width = 120.f
	) :
		View{parent},
		_type{p_type}
	{
		initialize_theme_value(ThemeValues::text_field_font_size, 15.f);
		initialize_theme_value(ThemeValues::text_field_height, 3.f);
		initialize_theme_value(ThemeValues::text_field_padding_left, 14.f);
		initialize_theme_value(ThemeValues::text_field_padding_right, 14.f);
		initialize_theme_value(ThemeValues::textFieldFilledPaddingBottom, 9.f);

		set_label(p_label);
		set_cursor(Cursor::Ibeam);
		enable_mouse_events();

		_editable_text->set_font_size(get_theme_value(ThemeValues::text_field_font_size));
		_editable_text->set_left(get_theme_value(ThemeValues::text_field_padding_left));
		_editable_text->set_right(width - get_theme_value(ThemeValues::text_field_padding_right), false);

		auto const handle_editable_text_focus_change = [this]() {
			queue_animation_update();
		};
		_editable_text->keyboard_focus_gain_listeners += handle_editable_text_focus_change;
		_editable_text->keyboard_focus_lose_listeners += handle_editable_text_focus_change;

		set_size({
			width, 
			get_theme_value(ThemeValues::text_field_font_size)*1.2f*get_theme_value(ThemeValues::text_field_height) + 
			outlined_padding_label*(_type == Type::Outlined)
		});

		if (p_type == Type::Filled) {
			set_corners({5.f, 5.f, 0.f, 0.f});
		}
		else {
			set_corner_radius(5.f);
		}

		set_string("");

		queue_animation_update();
	}
};

} // namespace Avo

/*
	Material design 2014 colors
*/
namespace MaterialColors {

inline Avo::ColorInt const
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

} // namespace MaterialColors

/*
	These are the codepoints for the material icon font!
*/
namespace MaterialIcons {

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

} // namespace MaterialIcons
