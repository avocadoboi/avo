#include "testing_header.hpp"

TEST_CASE("avo::utils::generate") {
	auto generator = avo::utils::generate([i = 0]() mutable {
		return i < 10 ? std::optional{i++} : std::nullopt;
	}) | std::views::drop(1);

	for ([[maybe_unused]] auto a : avo::utils::Range{2})
	{
		for (auto i = 1; auto const value : generator) 
		{
			REQUIRE(value == i);
			REQUIRE(value < 10);
			++i;
		}
	}
}
TEST_CASE("avo::utils::generate with views") {
	constexpr auto generator = avo::utils::generate([i = 0]() mutable {
		return i < 10 ? std::optional{i++} : std::nullopt;
	});

	for (auto i = 0; auto const value : generator 
			| std::views::filter([](int const i) { return i % 2 == 0; })
			| std::views::transform([](int const i) { return 1./i; })) 
	{
		REQUIRE(value == 1./i);
		REQUIRE(i < 10);
		i += 2;
	}
}
