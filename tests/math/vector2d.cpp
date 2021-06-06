#include <avo/math/vector2d.hpp>

#include <catch.hpp>

using namespace avo::math;

static_assert(avo::utils::IsTrivial<Vector2d<float>>);
static_assert(Vector2d{5.f, 3.f} != Vector2d{5.f, 3.1f});
static_assert(Vector2d{5.f, 3.f} == Vector2d{5.f, 3.f});
static_assert(Vector2d{-2.f, -3.f} < Vector2d{5.f, 3.f} && Vector2d{5.f, 3.f} > Vector2d{-2.f, -3.f});
static_assert(Vector2d{-2.f, 1.f}*2.f == Vector2d{16.f, -8.f}/-4.f);
static_assert(Vector2d{-2.f, 1.f}*2.f == 2.f*Vector2d{-2.f, 1.f});
static_assert(2.f/Vector2d{2.f, -4.f} == Vector2d{1.f, -0.5f});
static_assert(100/(3*Vector2d{4, 3} + Vector2d{2, 1}*2) == Vector2d{100/16, 100/11});

static_assert(interpolate(Vector2d{0.f, 0.f}, Vector2d{1.f, 1.f}, 0.5f) == Vector2d{0.5f, 0.5f});

static_assert(with_negative_space_clipped(Size{-4.f, 8.f}) == Size{0.f, 8.f});
static_assert(with_negative_space_clipped(Size{-4.f, -8.f}) == Size{});

static_assert(square<Vector2d>(5.f) == Vector2d{5.f, 5.f});
static_assert(square(5.f) == 25.f);

static_assert(dot(Vector2d{4, 2}, Vector2d{-2, -3}) == -14);
static_assert(cross(Vector2d{4, 2}, Vector2d{-2, -3}) == -8);

static_assert(Vector2d{2.f, 5.f}.to<Size<int>>() == Size{2, 5});

TEST_CASE("Runtime operations on 2D vectors") {
	CHECK(Vector2d{3, 4}.length() == 5.);
	CHECK(distance(Vector2d{2, 1}, Vector2d{-1, -3}) == 5.);
	CHECK(normalized(Vector2d{9.f, 9.f}) == square<Vector2d>(1.f/std::numbers::sqrt2_v<float>));
	CHECK(rotated(Vector2d{5.f, 4.f}, Degrees{90}) == Vector2d{-4.f, 5.f});
	CHECK(with_angle(Vector2d{4.95f, -3.14f}, Degrees{90}).angle<Degrees<int>>() == Degrees{90});
}

TEST_CASE("avo::math::Vector2dBase formatting") {
	REQUIRE(fmt::format("{}", Vector2d{1.5f, -0.9f}) == "(1.5, -0.9)");
	REQUIRE(fmt::format("{:.2f}", Vector2d{std::numbers::pi, std::numbers::e}) == "(3.14, 2.72)");
}
