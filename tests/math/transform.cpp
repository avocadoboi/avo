#include <avo/math/transform.hpp>

#include <catch.hpp>

using namespace avo::math;

template<std::floating_point T>
constexpr bool get_is_approximately_identity(Transform<T> const t) {
	return approximately_equal(t.x_to_x, T{1}) && approximately_equal(t.y_to_x, T{}) && approximately_equal(t.offset_x, T{}) &&
		approximately_equal(t.x_to_y, T{}) && approximately_equal(t.y_to_y, T{1}) && approximately_equal(t.offset_y, T{});
}

static_assert(
	[]{
		constexpr auto a = Transform{
			11., 2.9, 3.5, 
			4.3, 5.7, 6.2
		};
		// return get_is_approximately_identity(a*inverse(a));// && get_is_approximately_identity(inverse(a)*a);
		return get_is_approximately_identity(a*inverse(a)) && get_is_approximately_identity(inverse(a)*a);
	}()
);
static_assert(
	[]{
		constexpr auto a = Transform{
			11.f, 2.9f, 3.5f, 
			4.3f, 5.7f, 6.2f
		};
		constexpr auto b = Transform{
			-4.8f, -3.6f, -2.6f, 
			-1.2f, 1.5f, -3.31f
		};

		constexpr auto c = Vector2d{3.14f, 5.158f};

		return a*(b*c) == (a*b)*c && a*b != b*a;
	}(),
	"avo::math::Transform does not hold the composition and non-commutative properties."
);
static_assert(
	[]{
		constexpr auto a = Transform{
			11.f, 2.9f, 3.5f, 
			4.3f, 5.7f, 6.2f
		};
		return scaled(a, square<Vector2d>(0.6f)) == scaled_x(a, 0.6f).scale_y(0.6f);
	}(),
	"The scaling functions for avo::math::Transform do not work properly."
);

TEST_CASE("Runtime operations with transforms and 2D vectors") {
	auto const to_transform = Vector2d{3.14f, -8.24f};
	auto const result_0 = Transform<float>{}.rotate(Degrees{40})
		.translate(Vector2d{-1.5f, -4.5f})
		.scale(square<Vector2d>(0.6f)) * to_transform;
	auto const result_1 = (rotated(to_transform, Degrees{40}) + Vector2d{-1.5f, -4.5f}) * 0.6f;
	CHECK(result_0.x == Catch::Approx{result_1.x});
	CHECK(result_0.y == Catch::Approx{result_1.y});
}

TEST_CASE("avo::math::Transform formatting") {
	auto const transform = Transform{
		0.123, 95487.147823, 0.9417, 
		0.698574, 4938.71, 19.8704752
	};
	REQUIRE(fmt::format("{:^10.3f}", transform) == "[  0.123    95487.148    0.942   ]\n[  0.699     4938.710    19.870  ]");
}
