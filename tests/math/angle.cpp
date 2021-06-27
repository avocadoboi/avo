#include <avo/math/angle.hpp>

using avo::math::Degrees, avo::math::Radians;

using namespace avo::math::angle_literals;

static_assert(avo::util::IsTrivial<Degrees<int>>);
static_assert(std::same_as<decltype(Degrees{5} + Degrees{3.1}), Degrees<double>>);
static_assert(std::same_as<decltype(Degrees{5}*3.1), Degrees<double>>);
static_assert(3.f*(Radians{5.f} + Radians{3.f}*3.f)/2.f - Radians{3.f} == Radians{18.f}, "Radian arithmetic does not work");
static_assert([]{
	auto angle = Radians{5.f};
	angle += Radians{2.f};
	if (angle != Radians{7.f}) return false;
	angle -= Radians{2.f};
	if (angle != Radians{5.f}) return false;
	angle /= 2.f;
	if (angle != Radians{2.5f}) return false;
	angle *= 4.f;
	if (angle != Radians{10.f}) return false;
	return true;
}(), "Radian arithmetic does not work.");
static_assert(80_deg == Degrees{80} && -80_deg == Degrees{-80});
static_assert(80._deg == Degrees{80.} && 80._degf == Degrees{80.f});
static_assert(3.14_rad == Radians{3.14} && 3.14_radf == Radians{3.14f});
static_assert(Degrees{80} > Degrees{-30} && Degrees{-30} < Degrees{80});
static_assert(Degrees{50} == Degrees{50} && Degrees{50} != Degrees{51});
static_assert(avo::math::to_radians<float>(Degrees{180.f}) == Radians{std::numbers::pi_v<float>});
static_assert(avo::math::to_degrees<int>(Radians{std::numbers::pi_v<float>}) == Degrees{180});
static_assert(avo::math::to_degrees<float>(Degrees{50}) == Degrees{50.f});
static_assert(avo::math::normalized<float>(Degrees{90}) == 0.25f);
static_assert(avo::math::normalized(Degrees{90.f}) == 0.25f);
static_assert(avo::math::normalized(Radians{std::numbers::pi_v<float>/2}) == 0.25f);
