#include <avo/util/enumerate_view.hpp>
#include <avo/util/int_range.hpp>

#include <array>

#include <catch.hpp>

TEST_CASE("avo::util::enumerate") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};

	auto correct_index = std::size_t{};

	for (auto const [index, element] : avo::util::enumerate(container)) 
	{
		CHECK(index == correct_index);
		CHECK(element == container[correct_index]);
		++correct_index;
	}

	CHECK(correct_index == container.size());
}

TEST_CASE("Random access avo::util::enumerate") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
	
	auto const range = avo::util::enumerate(container) | std::views::reverse;
	
	for (auto const correct_index : avo::util::indices(container)) 
	{
		auto const [index, element] = range[static_cast<std::ptrdiff_t>(container.size() - 1 - correct_index)];
		CHECK(index == correct_index);
		CHECK(element == container[correct_index]);
	}
}

TEST_CASE("Random access avo::util::enumerate pipe") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
	
	auto const range = container | std::views::reverse | avo::util::enumerate;
	
	for (auto const correct_index : avo::util::indices(container)) 
	{
		auto const [index, element] = range[static_cast<std::ptrdiff_t>(correct_index)];
		CHECK(index == correct_index);
		CHECK(element == container[container.size() - 1 - correct_index]);
	}
}
