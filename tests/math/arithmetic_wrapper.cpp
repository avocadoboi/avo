#include <avo/math/angle.hpp>

#include <catch.hpp>

TEST_CASE("avo::math::ArithmeticBase formatting") {
	using namespace avo::math::literals;
	REQUIRE(fmt::format("{} degrees", 283_deg) == "283 degrees");
	REQUIRE(fmt::format("{} degrees", 283.85_deg) == "283.85 degrees");
	REQUIRE(fmt::format("{:.3f} radians", avo::math::Radians{std::numbers::pi}) == "3.142 radians");
}
