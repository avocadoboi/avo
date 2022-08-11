#include <avo/util/circular_stack_buffer.hpp>

#include <avo/util/int_range.hpp>

#include <catch.hpp>

TEST_CASE("CircularStackBuffer push_back, push_front, size, front, back, indexing") {
	auto stack_buffer = avo::util::CircularStackBuffer<int, 3>{};

	REQUIRE(stack_buffer.is_empty());
	REQUIRE(stack_buffer.size() == 0);

	stack_buffer.push_back(1);
	
	REQUIRE(not stack_buffer.is_empty());
	REQUIRE(stack_buffer.size() == 1);
	REQUIRE(stack_buffer.front() == 1);
	REQUIRE(stack_buffer.back() == 1);
	REQUIRE(stack_buffer[0] == 1);
	
	stack_buffer.push_back(2);

	REQUIRE(not stack_buffer.is_empty());
	REQUIRE(stack_buffer.size() == 2);
	REQUIRE(stack_buffer.front() == 1);
	REQUIRE(stack_buffer.back() == 2);
	REQUIRE(stack_buffer[0] == 1);
	REQUIRE(stack_buffer[1] == 2);

	stack_buffer.push_back(3);

	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 1);
	REQUIRE(stack_buffer.back() == 3);
	REQUIRE(stack_buffer[0] == 1);
	REQUIRE(stack_buffer[1] == 2);
	REQUIRE(stack_buffer[2] == 3);
	
	stack_buffer.push_back(4);

	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 2);
	REQUIRE(stack_buffer.back() == 4);
	REQUIRE(stack_buffer[0] == 2);
	REQUIRE(stack_buffer[1] == 3);
	REQUIRE(stack_buffer[2] == 4);

	stack_buffer.push_back(5);
	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 3);
	REQUIRE(stack_buffer.back() == 5);
	REQUIRE(stack_buffer[0] == 3);
	REQUIRE(stack_buffer[1] == 4);
	REQUIRE(stack_buffer[2] == 5);

	stack_buffer.push_back(6);
	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 4);
	REQUIRE(stack_buffer.back() == 6);
	REQUIRE(stack_buffer[0] == 4);
	REQUIRE(stack_buffer[1] == 5);
	REQUIRE(stack_buffer[2] == 6);

	stack_buffer.push_front(3);
	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 3);
	REQUIRE(stack_buffer.back() == 5);
	REQUIRE(stack_buffer[0] == 3);
	REQUIRE(stack_buffer[1] == 4);
	REQUIRE(stack_buffer[2] == 5);

	stack_buffer.push_front(2);
	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 2);
	REQUIRE(stack_buffer.back() == 4);
	REQUIRE(stack_buffer[0] == 2);
	REQUIRE(stack_buffer[1] == 3);
	REQUIRE(stack_buffer[2] == 4);

	stack_buffer.push_front(1);
	REQUIRE(stack_buffer.size() == 3);
	REQUIRE(stack_buffer.front() == 1);
	REQUIRE(stack_buffer.back() == 3);
	REQUIRE(stack_buffer[0] == 1);
	REQUIRE(stack_buffer[1] == 2);
	REQUIRE(stack_buffer[2] == 3);
}

TEST_CASE("CircularStackBuffer pop_front and pop_back") {
	auto stack_buffer = avo::util::CircularStackBuffer<int, 3>{};
	stack_buffer.push_back(1);
	stack_buffer.pop_back();
	REQUIRE(stack_buffer.is_empty());

	stack_buffer.pop_back();
	REQUIRE(stack_buffer.is_empty());

	stack_buffer.pop_front();
	REQUIRE(stack_buffer.is_empty());

	stack_buffer.push_back(1);
	stack_buffer.push_back(2);
	stack_buffer.push_back(3);
	REQUIRE(stack_buffer.size() == 3);

	stack_buffer.pop_front();
	REQUIRE(stack_buffer.size() == 2);
	REQUIRE(stack_buffer.front() == 2);
	REQUIRE(stack_buffer[0] == 2);
	REQUIRE(stack_buffer.back() == 3);
	REQUIRE(stack_buffer[1] == 3);
	
	stack_buffer.pop_back();
	REQUIRE(stack_buffer.size() == 1);
	REQUIRE(stack_buffer.front() == 2);
	REQUIRE(stack_buffer.back() == 2);
	REQUIRE(stack_buffer[0] == 2);
}

TEST_CASE("CircularStackBuffer std::ranges::copy and std::ranges::equal") {
	constexpr auto source_buffer = std::array{-81, 74, -44, -82, -37, 87, -52, -52, 8, -68};
	
	using Buffer = avo::util::CircularStackBuffer<int, source_buffer.size()>;
	
	auto buffer_a = Buffer{};
	for (auto const n : source_buffer) {
		buffer_a.push_back(n);
	}

	auto buffer_b = Buffer{};
	buffer_b.resize(buffer_a.size());
	std::ranges::copy(source_buffer, buffer_b.begin());

	REQUIRE(std::ranges::equal(buffer_a, buffer_b));
	REQUIRE(buffer_a.size() == source_buffer.size());
	REQUIRE(buffer_a.size() == buffer_b.size());

	for (auto const i : avo::util::indices(source_buffer)) {
		REQUIRE(buffer_a[i] == source_buffer[i]);
		REQUIRE(buffer_b[i] == source_buffer[i]);
	}
}
