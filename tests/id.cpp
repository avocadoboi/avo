#include <avo/id.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("avo::Id formatting") {
	REQUIRE(std::format("{}", avo::Id{91}) == "91");
	REQUIRE(std::format("{:>10}", avo::Id{91}) == "        91");
}
