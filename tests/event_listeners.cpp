#include <avo/event_listeners.hpp>

#include <catch.hpp>

TEST_CASE("avo::EventListeners test") {
	auto result = 0.f;
	
	auto listeners = avo::EventListeners<void(float)>{};

	auto const first = [&](float const value) {
		result += value;
	};
	listeners += first;
	
	auto const second = [&](float const value) {
		result += value*0.5f;
	};
	listeners += second;

	REQUIRE(result == 0.f);
	
	listeners(5.f);
	REQUIRE(result == 7.5f);
	
	listeners -= first;
	listeners(5.f);
	REQUIRE(result == 10.f);
	
	listeners -= second;
	listeners(5.f);
	REQUIRE(result == 10.f);

	listeners += first;
	listeners(5.f);
	REQUIRE(result == 15.f);
}
