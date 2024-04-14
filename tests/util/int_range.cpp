#include <avo/util/int_range.hpp>

#include <algorithm>
#include <array>
#include <ranges>

using avo::util::Range;

static_assert(
	[] {
		constexpr auto check_with = []<avo::util::IsIntRange<int> T>(T const range) {
			return range.reverse().reverse() == range;
		};
		return check_with(Range{31415}) && check_with(Range{-6283, 31415});
	}(), 
	"Inverse property of avo::util::Range::reverse() is not held"
);
static_assert(
	[] {
		constexpr auto number_of_iterations = std::size_t{10};

		auto a = std::size_t{};
		for (auto const b : Range{number_of_iterations}) {
			if (a != b) {
				return false;
			}
			++a;
		}
		return a == number_of_iterations;
	}(),
	"avo::util::Range with single constructor argument works incorrectly."
);
static_assert(
	[] {
		constexpr auto number_of_iterations = std::size_t{10};

		auto a = number_of_iterations;
		for (auto const b : Range{number_of_iterations}.reverse()) {
			--a;
			if (a != b) {
				return false;
			}
		}
		return a == 0;
	}(),
	"Reversed avo::util::Range with single constructor argument works incorrectly."
);
static_assert(
	[] {
		constexpr auto min = -5;
		constexpr auto max = 8;

		auto a = min;
		for (auto const b : Range{min, max}) {
			if (a != b) {
				return false;
			}
			++a;
		}
		return a == max + 1;
	}(),
	"avo::util::Range with two constructor arguments works incorrectly."
);
static_assert(
	[] {
		constexpr auto min = -5;
		constexpr auto max = 8;

		auto a = max;
		for (auto const b : Range{min, max}.reverse()) {
			if (a != b) {
				return false;
			}
			--a;
		}
		return a == min - 1;
	}(),
	"Reversed avo::util::Range with two constructor arguments works incorrectly."
);
static_assert(
	std::ranges::equal(
		Range{-5, 3} | std::views::transform([](int i){ return i*2; }) | std::views::reverse,
		std::array{6, 4, 2, 0, -2, -4, -6, -8, -10}
	),
	"avo::util::Range with standard ranges library does not work."
);
static_assert(
	std::ranges::equal(
		Range{-5, 3} | std::views::reverse,
		std::array{3, 2, 1, 0, -1, -2, -3, -4, -5}
	),
	"avo::util::Range with std::views::reverse does not work."
);

static_assert(
	[] {
		constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
		return avo::util::indices(container) == Range{container.size()} && (container | avo::util::indices) == Range{container.size()};
	}(),
	"avo::util::indices with lvalue reference failed."
);
static_assert(
	avo::util::indices(std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6}) == Range{std::size_t{11}},
	"avo::util::indices with rvalue reference failed."
);

static_assert(
	Range{5}.size() == 5 && Range{5}.reverse().size() == 5 && Range{-8, 3}.size() == 12 && Range{-8, 3}.reverse().size() == 12,
	"avo::util::Range::size() does not work."
);

static_assert(std::ranges::sized_range<Range<int>>);
static_assert(std::ranges::random_access_range<Range<int>>);
static_assert(std::ranges::range<Range<int>>);

static_assert(std::ranges::sized_range<Range<std::size_t>>);
static_assert(std::ranges::random_access_range<Range<std::size_t>>);
static_assert(std::ranges::range<Range<std::size_t>>);

// static_assert(std::ranges::equal(product, avo::util::indices(std::mdspan{std::array{}})))
static_assert(
	[] {
		return std::ranges::size(avo::util::indices(std::extents<std::size_t, 2, 3>{})) == 6;
	}(),
	"avo::util::indices with static std::extents failed"
);
static_assert(
	[] {
		constexpr auto container = std::array{3, 1, 4, 1, 5, 9};
		return std::ranges::size(avo::util::indices(std::mdspan{container.data(), std::extents<std::size_t, 2, 3>{}})) == 6;
	}(),
	"avo::util::indices with static std::mdspan failed"
);

static_assert(
	[] {
		return std::ranges::size(avo::util::indices(std::dextents<std::size_t, 2>{2, 3})) == 6;
	}(),
	"avo::util::indices with dynamic std::extents failed"
);
static_assert(
	[] {
		constexpr auto container = std::array{3, 1, 4, 1, 5, 9};
		return std::ranges::size(avo::util::indices(std::mdspan{container.data(), 2, 3})) == 6;
	}(),
	"avo::util::indices with dynamic std::mdspan failed"
);
