#include <avo/concurrency.hpp>
#include <avo/utils/int_range.hpp>

#include <catch.hpp>

#include <atomic>
#include <thread>

constexpr auto messages = std::array{5, 184, 9, -4, 77, 1};

TEST_CASE("Message channel, send all at once and receive one at a time") {
	auto [sender, receiver] = avo::concurrency::create_channel<int>();

	REQUIRE(receiver.queue_size() == 0);
	REQUIRE(receiver.is_queue_empty());

	auto const thread = std::jthread{[sender = std::move(sender)]() mutable {
		for (auto const message : messages) {
			sender.send(message);
		}
	}};

	for (auto const expected_message : messages) {
		REQUIRE(receiver.receive() == expected_message);
	}
}

TEST_CASE("Message channel, send all at once and receive all at once") {
	auto [sender, receiver] = avo::concurrency::create_channel<int>();

	auto sent_all = std::atomic_flag{};
	auto const thread = std::jthread{[&sent_all, sender = std::move(sender)]() mutable {
		for (int const message : messages) {
			sender.send(message);
		}
		sent_all.test_and_set();
		sent_all.notify_one();
	}};

	sent_all.wait(false);

	for (auto i = std::size_t{}; !receiver.is_queue_empty(); ++i) 
	{
		REQUIRE(receiver.queue_size() == messages.size() - i);
		REQUIRE(receiver.receive() == messages.at(i));
	}
}

TEST_CASE("Message channel, send waiting and receive waiting") {
	auto [sender, receiver] = avo::concurrency::create_channel<int>();

	auto counter = std::atomic<std::size_t>{};

	auto const thread = std::jthread{[&counter, sender = std::move(sender)]() mutable {
		for (auto const message : messages) {
			sender.send_wait(message);
			++counter;
		}
	}};

	for (auto const i : avo::utils::indices(messages))
	{
		REQUIRE(receiver.receive() == messages[i]);
		REQUIRE((counter == i || counter == i + 1));
	}
}

