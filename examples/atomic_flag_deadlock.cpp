#include <atomic>
#include <thread>

constexpr auto count_limit = 1'000'000;

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
}
