#include "testing_header.hpp"

TEST_CASE("avo::Id formatting") {
    REQUIRE(fmt::format("{}", avo::Id{91}) == "91");
    REQUIRE(fmt::format("{:>10}", avo::Id{91}) == "        91");
}
TEST_CASE("avo::math::ArithmeticBase formatting") {
    using namespace avo::math::literals;
    REQUIRE(fmt::format("{} degrees", 283_deg) == "283 degrees");
    REQUIRE(fmt::format("{} degrees", 283.85_deg) == "283.85 degrees");
    REQUIRE(fmt::format("{:.3f} radians", avo::math::Radians{std::numbers::pi}) == "3.142 radians");
}

TEST_CASE("avo::math::Vector2dBase formatting") {
    REQUIRE(fmt::format("{}", avo::math::Vector2d{1.5f, -0.9f}) == "(1.5, -0.9)");
    REQUIRE(fmt::format("{:.2f}", avo::math::Vector2d{std::numbers::pi, std::numbers::e}) == "(3.14, 2.72)");
}

TEST_CASE("avo::math::Rectangle formatting") {
    REQUIRE(fmt::format("{:.1f}", avo::math::Rectangle{0.18f, 0.83f, 0.46f, 0.77f}) == "(0.2, 0.8, 0.5, 0.8)");
}

TEST_CASE("avo::Color formatting") {
    REQUIRE(fmt::format("{:.1f}", avo::Color{0.18f, 0.83f, 0.46f}) == "rgba(0.2, 0.8, 0.5, 1.0)");
}

TEST_CASE("avo::math::Transform formatting") {
    auto const transform = avo::math::Transform{
        0.123, 95487.147823, 0.9417, 
        0.698574, 4938.71, 19.8704752
    };
    REQUIRE(fmt::format("{:^10.3f}", transform) == "[  0.123    95487.148    0.942   ]\n[  0.699     4938.710    19.870  ]");
}
