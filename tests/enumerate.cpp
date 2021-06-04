#include "testing_header.hpp"

TEST_CASE("avo::utils::enumerate") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};

	auto correct_index = std::size_t{};

	for (auto const [index, element] : avo::utils::enumerate(container)) 
	{
		CHECK(index == correct_index);
		CHECK(element == container[correct_index]);
		++correct_index;
	}

	CHECK(correct_index == container.size());
}

TEST_CASE("Random access avo::utils::enumerate") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
	
	auto const range = avo::utils::enumerate(container) | std::views::reverse;
	
	for (auto const correct_index : avo::utils::indices(container)) 
	{
		auto const [index, element] = range[container.size() - 1 - correct_index];
		CHECK(index == correct_index);
		CHECK(element == container[correct_index]);
	}
}

TEST_CASE("Random access avo::utils::enumerate pipe") {
	constexpr auto container = std::array{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 6};
	
	auto const range = container | std::views::reverse | avo::utils::enumerate;
	
	for (auto const correct_index : avo::utils::indices(container)) 
	{
		auto const [index, element] = range[correct_index];
		CHECK(index == correct_index);
		CHECK(element == container[container.size() - 1 - correct_index]);
	}
}
