#include <avo/graphics/color.hpp>

#include <catch.hpp>

using avo::graphics::Color,
	avo::math::Degrees;

static_assert(Color::hsb(Degrees{30}, 1.f, 1.f).hue<Degrees<int>>() == Degrees{30});
static_assert(Color::hsb(Degrees{180}, 1.f, 1.f).hue() == 0.5f);
static_assert(Color::hsb(Degrees{30}, 0.77f, 1.f).hsb_saturation() == 0.77f);

static_assert(Color::hsl(Degrees{30}, 1.f, 0.8f).hue<Degrees<int>>() == Degrees{30});
static_assert(Color::hsl(Degrees{180}, 1.f, 0.8f).hue() == 0.5f);
static_assert(Color::hsl(Degrees{30}, 0.77f, 0.8f).hsl_saturation() == 0.77f);

static_assert(Color{0.1f} == Color{0.1f, 0.1f, 0.1f, 1.f});
static_assert(Color{0.1f} == Color::rgb(0.1f, 0.1f, 0.1f));
static_assert(Color{0.1f, 0.2f, 0.3f} + Color{1.f, 0.7f, 0.5f} == Color{1.f, 0.9f, 0.8f});
static_assert(Color{1.f, 0.9f, 0.8f} - Color{1.f, 0.7f, 0.5f, 0.5f} == Color{0.f, 0.9f - 0.7f, 0.8f - 0.5f, 0.5f});
static_assert(Color{0.1f, 0.2f, 0.3f} + 0.2f == Color{0.3f, 0.4f, 0.5f});
static_assert(Color{0.2f, 0.3f, 0.4f} - 0.2f == Color{0.f, 0.3f - 0.2f, 0.2f});
static_assert(1.f - Color{0.2f, 0.3f, 0.4f} == Color{0.8f, 0.7f, 0.6f});
static_assert(Color{0.2f, 0.3f, 0.4f} * 2.f == Color{0.4f, 0.6f, 0.8f});

static_assert(Color{0.1f, 0.2f, 0.9f}.hue(Degrees{71}).hue<Degrees<int>>() == Degrees{71});
static_assert(Color{0.1f, 0.2f, 0.9f}.hue(0.3f).hue() == 0.3f);
static_assert(Color{0.1f, 0.2f, 0.9f}.brightness(0.3f).brightness() == 0.3f);
static_assert(Color{0.1f, 0.2f, 0.9f}.lightness(0.3f).lightness() == 0.3f);
static_assert(avo::math::approximately_equal(Color{0.1f, 0.2f, 0.9f}.hsl_saturation(0.3f).hsl_saturation(), 0.3f));
static_assert(Color{0.1f, 0.2f, 0.9f}.hsb_saturation(0.3f).hsb_saturation() == 0.3f);

static_assert(avo::math::interpolate(Color{0.2f, 0.3f, 0.4f}, Color{0.8f, 0.7f, 0.6f}, 0.5f) == Color{0.5f});

static_assert(Color::alpha_channel(0xabcdef12) == 0xab);
static_assert(Color::red_channel(0xabcdef12) == 0xcd);
static_assert(Color::green_channel(0xabcdef12) == 0xef);
static_assert(Color::blue_channel(0xabcdef12) == 0x12);

TEST_CASE("avo::Color formatting") {
    REQUIRE(fmt::format("{:.1f}", Color{0.18f, 0.83f, 0.46f}) == "rgba(0.2, 0.8, 0.5, 1.0)");
}
