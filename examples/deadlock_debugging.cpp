// #include <avo/concurrency.hpp>
// #include <avo/utils/int_range.hpp>

// #include <fmt/format.h>

// #include <thread>

// constexpr auto messages = std::array{5, 184, 9, -4, 77, 1};

// int main () {
// 	auto [sender, receiver] = avo::concurrency::create_channel<int>();

// 	auto const thread = std::jthread{[sender = std::move(sender)]() mutable {
// 		for (int const message : messages) {
// 			fmt::print("Sending {}\n", message);
// 			sender.send_wait(message);
// 		}
// 	}};

// 	for ([[maybe_unused]] auto const i : avo::utils::indices(messages))
// 	{
// 		fmt::print("Received {}\n", receiver.receive());
// 	}
// }
#include <atomic>
#include <iostream>
#include <thread>

constexpr auto count_limit = 10'000;

auto atomic_flag = std::atomic_flag{};
auto counter = std::atomic<int>{};

void ping() {
	while (counter < count_limit) {
		atomic_flag.wait(true);
		
		++counter;
		
		atomic_flag.test_and_set();
		atomic_flag.notify_one();
	}
}

void pong() {
	while (counter < count_limit) {
		atomic_flag.wait(false);
		atomic_flag.clear();
		atomic_flag.notify_one();
	}
}

int main() {
	atomic_flag.test_and_set();

	auto t1 = std::thread{ping};
	auto t2 = std::thread{pong};
	t1.join();
	t2.join();

	std::cout << "Finished\n";
}

/*

counter = 0
test_and_set();

pong: while (counter < 1) // 0 < 1: true
pong: wait(false); // already true, continue
pong: clear();
pong: notify_one(); // no-op

ping: while (counter <= 1) // 0 <= 1: true
ping: wait(true); // already false, continue
ping: test_and_set();
ping: ++counter; // = 1
ping: notify_one(); // no-op

pong: while (counter < 1) // 1 < 1: false, return

ping: while (counter <= 1) // 1 <= 1: true
ping: wait(true); // remains true, stuck.

*/

