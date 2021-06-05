#include <avo/math/operations.hpp>

using namespace avo::math;

static_assert(floor<double>(-4.5) == -5. && floor<int>(-4.4) == -5);
static_assert(floor<double>(4.5) == 4. && floor<int>(4.7) == 4);
static_assert(ceil<double>(-4.5) == -4. && ceil<int>(-4.4) == -4);
static_assert(ceil<double>(4.5) == 5. && ceil<int>(4.7) == 5);

static_assert(
	min(1, 9.89, 3, 6.1, -6, 0., 1845, 14) == -6 &&
	min(-1, 2) == -1 &&
	min(-1) == -1,
	"avo::math::min works incorrectly."
);
static_assert(
	max(1, 9.89, 3, 6.1, -6, 0., 1845, 14) == 1845 &&
	max(-1, 2) == 2 &&
	max(-1) == -1,
	"avo::math::max works incorrectly."
);
