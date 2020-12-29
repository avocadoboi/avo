#include "testing_header.hpp"

constexpr auto utf8_string = "مجفف شعر أم مكنسة كهربائية؟"sv;
constexpr auto utf16_string = u"مجفف شعر أم مكنسة كهربائية؟"sv;

TEST_CASE("Unicode conversion simple") {	
	CHECK(avo::utils::utf8_to_utf16(utf8_string) == utf16_string);
	CHECK(avo::utils::utf16_to_utf8(utf16_string) == utf8_string);
}
TEST_CASE("Unicode conversion with out parameter") {
	auto output_string_utf8 = std::string(avo::utils::utf16_to_utf8_unit_count(utf16_string), '\0');
	avo::utils::utf16_to_utf8(utf16_string, output_string_utf8);
	CHECK(output_string_utf8 == utf8_string);

	auto output_string_utf16 = std::u16string(avo::utils::utf8_to_utf16_unit_count(utf8_string), '\0');
	avo::utils::utf8_to_utf16(utf8_string, output_string_utf16);
	CHECK(output_string_utf16 == utf16_string);
}

TEST_CASE("Unicode conversion unit count") {
	// ∑ is 3 units in utf-8 and 1 unit in utf-16.
	// 🪕 is 4 units in utf-8 and 2 units in utf-16.
	CHECK(avo::utils::utf8_to_utf16_unit_count("åäöabc∑🪕") == 9);
	CHECK(avo::utils::utf16_to_utf8_unit_count(u"åäöabc∑🪕") == 16);
}
