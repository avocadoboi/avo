#include "testing_header.hpp"

constexpr auto utf8_string = "مجفف شعر أم مكنسة كهربائية؟"sv;
constexpr auto utf16_string = u"مجفف شعر أم مكنسة كهربائية؟"sv;

TEST_CASE("Unicode conversion simple") {	
	CHECK(avo::unicode::utf8_to_utf16(utf8_string) == utf16_string);
	CHECK(avo::unicode::utf16_to_utf8(utf16_string) == utf8_string);
}
TEST_CASE("UTF-16 to UTF-8 with out parameter") {
	auto output_string_utf8 = std::string(utf16_string.size()*3, '\0');
	auto const utf8_length = avo::unicode::utf16_to_utf8(utf16_string, output_string_utf8);
	REQUIRE(utf8_length);
	output_string_utf8.resize(*utf8_length);
	CHECK(output_string_utf8 == utf8_string);
	CHECK(*utf8_length == utf8_string.size());
}
TEST_CASE("UTF-8 to UTF-16 with out parameter") {
	auto output_string_utf16 = std::u16string(utf8_string.size(), '\0');
	auto const utf16_length = avo::unicode::utf8_to_utf16(utf8_string, output_string_utf16);
	REQUIRE(utf16_length);
	output_string_utf16.resize(*utf16_length);
	CHECK(output_string_utf16 == utf16_string);
	CHECK(*utf16_length == utf16_string.size());
}
