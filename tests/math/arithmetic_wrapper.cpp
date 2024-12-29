#include <avo/math/angle.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("avo::math::ArithmeticBase formatting") {
	using namespace avo::math::literals;
	REQUIRE(std::format("{} degrees", 283_deg) == "283 degrees");
	REQUIRE(std::format("{} degrees", 283.85_deg) == "283.85 degrees");
	REQUIRE(std::format("{:.3f} radians", avo::math::Radians{std::numbers::pi}) == "3.142 radians");
}
