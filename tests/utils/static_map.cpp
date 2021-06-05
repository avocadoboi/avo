#include <avo/utils/static_map.hpp>

#include <catch.hpp>

constexpr void test_static_map(auto& map)
{
	REQUIRE(std::ranges::size(map) == 6);
	REQUIRE(*map.find(-9) == '+');
	REQUIRE(map.find_or(-9, 'u') == '+');
	REQUIRE(*map.find('e') == 5);
	REQUIRE(map.find_or('e', 22) == 5);
	REQUIRE(std::ranges::equal(map.find_all('e'), std::array{5, -9}));
	REQUIRE(std::ranges::equal(map.find_all(-9), std::array{'+', 'e'}));
	REQUIRE(map.find(100) == nullptr);
	REQUIRE(map.find('-') == nullptr);
	REQUIRE(map.find_or(100, 'u') == 'u');
	REQUIRE(map.find_or('-', 22) == 22);
}

TEST_CASE("StaticMap with initializer_list") {
	constexpr auto map = avo::utils::StaticMap<int, char, 6>{
		{-2, 'a'},
		{5, 'e'},
		{-9, '+'},
		{8, 'o'},
		{4, 'z'},
		{-9, 'e'},
	};
	test_static_map(map);
}
TEST_CASE("StaticMap with pairs") {
	constexpr auto map = avo::utils::StaticMap{
		std::pair{-2, 'a'},
		std::pair{5, 'e'},
		std::pair{-9, '+'},
		std::pair{8, 'o'},
		std::pair{4, 'z'},
		std::pair{-9, 'e'},
	};
	test_static_map(map);
}

TEST_CASE("Mutable StaticMap") {
	auto map = avo::utils::StaticMap<int, char, 6>{
		{-2, 'a'},
		{5, 'e'},
		{-9, '+'},
		{8, 'o'},
		{4, 'z'},
		{-9, 'e'},
	};
	test_static_map(map);

	*map.find(8) = '3';
	REQUIRE(map.find_or(8, char{}) == '3');
	*map.find('z') = 1;
	REQUIRE(map.find_or('z', 0) == 1);
}
