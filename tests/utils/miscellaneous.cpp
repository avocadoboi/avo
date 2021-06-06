#include <avo/utils/miscellaneous.hpp>

#include <catch.hpp>

TEST_CASE("avo::utils::unordered_erase single element in middle") {
	auto list = std::vector{1, 2, 3, 4, 5, 6, 7};
	avo::utils::unordered_erase(list, 4);
	REQUIRE(list == std::vector{1, 2, 3, 7, 5, 6});
}
TEST_CASE("avo::utils::unordered_erase single element at end") {
	auto list = std::vector{1, 2, 3, 4, 5, 6, 7};
	avo::utils::unordered_erase(list, 7);
	REQUIRE(list == std::vector{1, 2, 3, 4, 5, 6});
}
TEST_CASE("avo::utils::unordered_erase two elements in middle") {
	auto list = std::vector{1, 2, 3, 4, 4, 5, 6, 7};
	avo::utils::unordered_erase(list, 4);
	REQUIRE(list == std::vector{1, 2, 3, 7, 6, 5});
}
TEST_CASE("avo::utils::unordered_erase two elements at end") {
	auto list = std::vector{1, 2, 3, 4, 4, 5, 6, 7, 7};
	avo::utils::unordered_erase(list, 7);
	REQUIRE(list == std::vector{1, 2, 3, 4, 4, 5, 6});
}