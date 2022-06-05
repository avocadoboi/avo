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
