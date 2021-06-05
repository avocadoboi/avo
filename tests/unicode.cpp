#include <avo/unicode.hpp>

#include <catch.hpp>

//------------------------------

using namespace avo::unicode;
using namespace std::string_view_literals;

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

//------------------------------

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
