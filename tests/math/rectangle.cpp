#include <avo/math/rectangle.hpp>

#include <catch.hpp>

using namespace avo::math;

static_assert(Rectangle{Size{5, 8}}.to<float>() == Rectangle{0.f, 0.f, 5.f, 8.f});
static_assert(Rectangle{Point{9, 1}, Point{11, 6}} == Rectangle{9, 1, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left() == Point{9, 1});
static_assert(Rectangle{9, 1, 11, 6}.top_right() == Point{11, 1});
static_assert(Rectangle{9, 1, 11, 6}.bottom_right() == Point{11, 6});
static_assert(Rectangle{9, 1, 11, 6}.bottom_left() == Point{9, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left<false>(Point{-2, -2}) == Rectangle{-2, -2, 11, 6});
static_assert(Rectangle{9, 1, 11, 6}.top_left(Point{-2, -2}) == Rectangle{-2, -2, 0, 3});
static_assert(Rectangle{9, 1, 11, 6}.move_top_left(Vector2d{-2, -3}) == Rectangle{7, -2, 11, 6});
static_assert(not Rectangle{3, 4, 18, 9}.contains(Rectangle{3, 4, 18, 9}));
static_assert(not Rectangle{3.f, 4.f, 18.f, 9.f}.contains(Rectangle{3.f, 4.f, 18.f, 9.f}));
static_assert(not Rectangle{3, 4, 18, 9}.contains(Rectangle{3.1f, 4.f, 18.f, 9.f}));
static_assert(Rectangle{3, 4, 18, 9}.contains(Rectangle{3.1f, 4.1f, 17.9f, 8.9f}));
static_assert(Rectangle{-10, -4, 1, -1}.intersects(Rectangle{0, -2, 100, 128}));
static_assert(not Rectangle{-10, -4, 1, -1}.intersects(Rectangle{1, -1, 100, 128}));
static_assert(square<Rectangle>(5.f) == Rectangle{0.f, 0.f, 5.f, 5.f});
static_assert(with_negative_space_clipped(Rectangle{4.f, 4.5f, 3.8f, 4.7f}) == Rectangle{4.f, 4.5f, 4.f, 4.7f});
static_assert(with_negative_space_clipped(Rectangle{4.f, 4.5f, 3.8f, 4.f}) == Rectangle{Point{4.f, 4.5f}});
static_assert(Rectangle{2, 3, 4, 5} + Size{3, 1} == Rectangle{2, 3, 7, 6});

TEST_CASE("avo::math::Rectangle formatting") {
	REQUIRE(fmt::format("{:.1f}", Rectangle{0.18f, 0.83f, 0.46f, 0.77f}) == "(0.2, 0.8, 0.5, 0.8)");
}
