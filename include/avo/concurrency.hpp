#ifndef AVO_CONCURRENCY_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_CONCURRENCY_HPP_BJORN_SUNDIN_JUNE_2021

#include <concepts>
#include <mutex>
#include <optional>
#include <queue>

namespace avo::concurrency {

/*
	Holds a queue of messages 
*/
template<std::move_constructible T>
class MessageQueue final {
public:
	void add(T&& message) {
		auto const lock = std::lock_guard{mutex_};
		queue_.push(std::move(message));
	}

	[[nodiscard]]
	std::optional<T> get_next() 
	{
		auto const lock = std::lock_guard{mutex_};

		if (queue_.empty()) {
			return std::nullopt;
		}

		auto message = std::move(queue_.front());
		queue_.pop();

		return std::optional{std::move(message)};
	}
		
private:
	std::queue<T> queue_;
	std::mutex mutex_;
};

} // namespace avo::concurrency

#endif
