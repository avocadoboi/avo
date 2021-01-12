#include "testing_header.hpp"

using namespace avo::math;

TEST_CASE("Runtime operations on vectors") {
	CHECK(Vector2d{3, 4}.get_length() == 5.);
	CHECK(distance(Vector2d{2, 1}, Vector2d{-1, -3}) == 5.);
	CHECK(normalized(Vector2d{9.f, 9.f}) == square<Vector2d>(1.f/std::numbers::sqrt2_v<float>));
	CHECK(rotated(Vector2d{5.f, 4.f}, Degrees{90}) == Vector2d{-4.f, 5.f});
	CHECK(with_angle(Vector2d{4.95f, -3.14f}, Degrees{90}).get_angle<Degrees<int>>() == Degrees{90});
}

TEST_CASE("Runtime operations with transforms and vectors") {
	auto const to_transform = Vector2d{3.14f, -8.24f};
	auto const result_0 = Transform<float>{}.rotate(Degrees{40})
		.translate(Vector2d{-1.5f, -4.5f})
		.scale(square<Vector2d>(0.6f)) * to_transform;
	auto const result_1 = (rotated(to_transform, Degrees{40}) + Vector2d{-1.5f, -4.5f}) * 0.6f;
	CHECK(result_0.x == Approx{result_1.x});
	CHECK(result_0.y == Approx{result_1.y});
}
