#include <avo/util/miscellaneous.hpp>

#include <catch.hpp>

static_assert(avo::util::IsTrivial<avo::util::MinMax<int>>);

TEST_CASE("avo::util::unordered_erase single element in middle") {
	auto list = std::vector{1, 2, 3, 4, 5, 6, 7};
	avo::util::unordered_erase(list, 4);
	REQUIRE(list == std::vector{1, 2, 3, 7, 5, 6});
}
TEST_CASE("avo::util::unordered_erase single element at end") {
	auto list = std::vector{1, 2, 3, 4, 5, 6, 7};
	avo::util::unordered_erase(list, 7);
	REQUIRE(list == std::vector{1, 2, 3, 4, 5, 6});
}
TEST_CASE("avo::util::unordered_erase two elements in middle") {
	auto list = std::vector{1, 2, 3, 4, 4, 5, 6, 7};
	avo::util::unordered_erase(list, 4);
	REQUIRE(list == std::vector{1, 2, 3, 7, 6, 5});
}
TEST_CASE("avo::util::unordered_erase two elements at end") {
	auto list = std::vector{1, 2, 3, 4, 4, 5, 6, 7, 7};
	avo::util::unordered_erase(list, 7);
	REQUIRE(list == std::vector{1, 2, 3, 4, 4, 5, 6});
}