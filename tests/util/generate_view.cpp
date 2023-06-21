#include <avo/util/generate_view.hpp>
#include <avo/util/int_range.hpp>

#include <catch.hpp>

TEST_CASE("avo::util::generate") {
	auto generator = avo::util::generate([i = 0]() mutable {
		return i < 10 ? std::optional{i++} : std::nullopt;
	}) | std::views::drop(1);

	for ([[maybe_unused]] auto a : avo::util::Range{2})
	{
		for (auto i = 1; auto const value : generator) 
		{
			REQUIRE(value == i);
			REQUIRE(value < 10);
			++i;
		}
	}
}
TEST_CASE("avo::util::generate with views") {
	constexpr auto generator = avo::util::generate([i = 0]() mutable {
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
