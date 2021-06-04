/*
MIT License

Copyright (c) 2021 Björn Sundin

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

#ifndef AVO_UNICODE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UNICODE_HPP_BJORN_SUNDIN_JUNE_2021

#include "../utils/concepts.hpp"

/*
	Unicode support library.
*/
namespace avo::unicode {

/*
	Enables UTF-8 encoded console output on Windows.
	Pretty much all other platforms use UTF-8 by default.
*/
void enable_utf8_console();

//------------------------------

/*
	Converts a UTF-8 encoded char string to a UTF-16 encoded char16 string.
	Returns the length of the converted string, in code point units (char16_t).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
std::optional<std::size_t> utf8_to_utf16(std::string_view input, std::span<char16_t> output);
/*
	Converts a UTF-8 encoded string to a UTF-16 encoded std::u16string.
*/
[[nodiscard]]
std::u16string utf8_to_utf16(std::string_view input);

/*
	Converts a UTF-16 encoded char16 string to a UTF-8 encoded char string.
	Returns the length of the converted string, in code point units (char).
	If no value is returned then the output span is too small to fit the whole converted string.
*/
std::optional<std::size_t> utf16_to_utf8(std::u16string_view input, std::span<char> output);
/*
	Converts a UTF-16 char16 string to a UTF-8 encoded std::string.
*/
[[nodiscard]]
std::string utf16_to_utf8(std::u16string_view input);

//------------------------------

/*
	Returns the number of UTF-8 code points a character starting with 
	first_code_point_in_character consists of in total.
	Returns 0 if the code point is not the first one in a character.
	Returns -1 if the code point is an invalid UTF-8 code point.
*/
[[nodiscard]]
constexpr int code_point_count(char const first_code_point_in_character) noexcept {
	// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 126
	if (!(first_code_point_in_character & 0x80)) // 0xxxxxxx
		return 1;
	if ((first_code_point_in_character & 0xc0) == 0x80) // 10??????
		return 0;
	if ((first_code_point_in_character & 0xe0) == 0xc0) // 110yyyyy
		return 2;
	if ((first_code_point_in_character & 0xf0) == 0xe0) // 1110zzzz
		return 3;
	if ((first_code_point_in_character & 0xf8) == 0xf0) // 11110uuu
		return 4;
	return -1;
}

/*
	Returns the number of UTF-16 code points a character starting with 
	first_code_point_in_character consists of in total.
	Returns 0 if the code point is not the first one in a character.
	Returns -1 if the code point is an invalid UTF-16 code point.
*/
[[nodiscard]]
constexpr int code_point_count(char16_t const first_code_point_in_character) noexcept {
	// http://www.unicode.org/versions/Unicode12.1.0/ch03.pdf , page 125
	if ((first_code_point_in_character & 0xfc00) == 0xd800) // 110110wwwwxxxxxx
		return 2;
	if ((first_code_point_in_character & 0xfc00) == 0xdc00) // 110111xxxxxxxxxx
		return 0;
	return 1; // xxxxxxxxxxxxxxxx
}

/*
	Returns whether the passed code point is the start of a UTF-8 encoded character.
*/
[[nodiscard]]
constexpr bool is_first_code_point(char const code_point) noexcept {
	return (code_point & 0xc0) != 0x80;
}

/*
	Returns whether p_unit is the start of a UTF-16 encoded character
*/
[[nodiscard]]
constexpr bool is_first_code_point(char16_t const code_point) noexcept {
	return (code_point & 0xfc00) != 0xdc00;
}

template<class T>
concept IsCodePoint = utils::IsAnyOf<T, char, char16_t>;

/*
	Returns the index of the code point at a certain character index in a UTF-8 or UTF-16 encoded string.
	If character_index is outside of the string, the size of the string is returned.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr std::size_t code_point_index(std::basic_string_view<T> const string, std::size_t const character_index) 
{
	if (!character_index) {
		return {};
	}
	if (character_index >= string.size()) {
		return string.size();
	}

	auto const position = std::ranges::find_if(
		string, [character_index, char_count = std::size_t{}](T const code_point) mutable {
			return is_first_code_point(code_point) && char_count++ == character_index;
		}
	);
	return position - string.begin();
}

/*
	Returns the index of the character that the code point at code_point_index in the UTF-8 or UTF-16 encoded string belongs to.
	If code_point_index is outside of the string, the last character index is returned.
	If code_point_index is past the start of a character but before the next one, it returns the index of the character it is part of.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr std::size_t character_index(std::basic_string_view<T> const string, std::size_t const code_point_index) 
{
	if (!code_point_index) {
		return {};
	}
	if (code_point_index >= string.size()) {
		return string.size();
	}

	return std::ranges::count_if(
		string.begin() + 1, string.begin() + code_point_index + 1,
		[](T const code_point) { return is_first_code_point(code_point); }
	);
}

/*
	Returns the number of unicode characters that a UTF-8 or UTF-16 string consists of.
*/
template<IsCodePoint T>
[[nodiscard]]
constexpr std::size_t character_count(std::basic_string_view<T> const string) {
	return character_index(string, string.size()) + 1;
}

#ifdef BUILD_TESTING
static_assert(
	code_point_count('a') == 1 &&
	code_point_count("å"[0]) == 2 &&
	code_point_count("√"[0]) == 3 &&
	code_point_count("🪢"[0]) == 4 &&
	code_point_count(static_cast<char>(0b10101010)) == 0 &&
	code_point_count(static_cast<char>(0b11111111)) == -1,
	"avo::unicode::code_point_count does not work correctly with UTF-8."
);
static_assert(
	code_point_count(u'a') == 1 &&
	code_point_count(u"å"[0]) == 1 &&
	code_point_count(u"√"[0]) == 1 &&
	code_point_count(u"🪢"[0]) == 2 &&
	code_point_count(static_cast<char16_t>(0b1101111010000011)) == 0,
	"avo::unicode::code_point_count does not work correctly with UTF-16."
);
static_assert(
	code_point_index(std::string_view{"🪢 här √ är knut"}, 10) == 17 &&
	code_point_index(std::string_view{"🪢 här 🪢 är knut"}, 10) == 18, 
	"code_point_index does not work correctly with UTF-8."
);
static_assert(
	code_point_index(std::u16string_view{u"🪢 här √ är knut"}, 10) == 11 &&
	code_point_index(std::u16string_view{u"🪢 här 🪢 är knut"}, 10) == 12, 
	"code_point_index does not work correctly with UTF-16."
);
static_assert(
	character_index(std::string_view{"🪢 här √ är knut"}, 17) == 10 &&
	character_index(std::string_view{"🪢 här 🪢 är knut"}, 18) == 10, 
	"character_index does not work correctly with UTF-8."
);
static_assert(
	character_index(std::u16string_view{u"🪢 här √ är knut"}, 11) == 10 &&
	character_index(std::u16string_view{u"🪢 här 🪢 är knut"}, 12) == 10, 
	"character_index does not work correctly with UTF-16."
);
#endif // BUILD_TESTING

} // namespace avo::unicode

#endif
