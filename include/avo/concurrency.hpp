#ifndef AVO_CONCURRENCY_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_CONCURRENCY_HPP_BJORN_SUNDIN_JUNE_2021

#include <atomic>
#include <concepts>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

// #include <fmt/format.h>

namespace avo::concurrency {

/*
	A thread-safe queue.

	It does not automatically enforce these rules:
		1. The queue is used by exactly two threads.
		2. Only one thread pushes messages and only the other thread takes them out.
		3. The MessageQueue is owned by the thread that lives the longest.

	It is not logical to provide the same interface for both threads.
	Therefore a channel interface is provided that abstracts a message queue and consists of a sender and a receiver.
	The message queue is held in a std::shared_ptr to guarantee that no references or pointers are ever left dangling.

	See create_channel, Channel, Sender, Receiver.
*/
template<std::move_constructible T>
class MessageQueue final {
public:
	/*
		Adds a message onto the queue.
	*/
	template<class ... Argument_>
		requires std::constructible_from<T, Argument_&&...>
	void push(Argument_&& ... argument) {
		{
			auto const lock = std::lock_guard{mutex_};
			queue_.emplace(std::forward<Argument_>(argument)...);
		}
		notify_next_message_();
	}

	/*
		Adds a message onto the queue and waits until it has been removed from the queue by another thread.
	*/
	template<class ... Argument_>
		requires std::constructible_from<T, Argument_&&...>
	void push_wait(Argument_&& ... argument) {
		{
			auto const lock = std::lock_guard{mutex_};
			queue_.emplace(std::forward<Argument_>(argument)...);
		}

		notify_next_message_();

		// Wait until the queue is empty (this message has been removed by the other thread).
		// fmt::print("Waiting to be received\n");
		has_messages_flag_.wait(true);
		// fmt::print("Finished waiting to be received\n");
	}

	/*
		Moves the next message off the queue, returning it.
		If the queue is empty, it waits until a new message has been pushed.
	*/
	[[nodiscard]]
	T take_next() {
		wait_for_next_();

		auto const lock = std::lock_guard{mutex_};

		auto message = std::move(queue_.front());
		pop_message_(lock);

		return message;
	}
	/*
		Returns a copy of the next message in the queue.
		If the queue is empty, it waits until a new message has been pushed.
		Returning a reference would not be thread-safe.
	*/
	[[nodiscard]]
	T peek_next() const 
		requires std::copy_constructible<T>
	{
		wait_for_next_();
		
		auto const lock = std::lock_guard{mutex_};
		return queue_.front();
	}

	/*
		Removes the next message from the queue.
		Does nothing if the queue is empty.
	*/
	void remove_next() {
		auto const lock = std::lock_guard{mutex_};
		pop_message_(lock);
	}
	
	/*
		Returns the number of messages currently in the queue.
	*/
	[[nodiscard]]
	std::size_t recent_size() const {
		auto const lock = std::lock_guard{mutex_};
		return queue_.size();
	}
	/*
		Returns whether the message queue is currently empty.
	*/
	[[nodiscard]]
	bool was_recently_empty() const {
		auto const lock = std::lock_guard{mutex_};
		return queue_.empty();
	}

	MessageQueue() = default;
	~MessageQueue() = default;

	MessageQueue(MessageQueue&&) = default;
	MessageQueue& operator=(MessageQueue&&) = default;

	MessageQueue(MessageQueue const&) = delete;
	MessageQueue& operator=(MessageQueue const&) = delete;
		
private:
	void pop_message_(std::lock_guard<std::mutex> const&) {
		queue_.pop();

		// If the queue has been emptied then update the flag.
		if (queue_.empty()) {
			has_messages_flag_.clear();
			has_messages_flag_.notify_one();
		}
	}

	/*
		Assuming a message has been added to the queue, notify any thread that is waiting for new messages using wait_for_next,
		only if the queue was previously empty.
	*/
	void notify_next_message_() {
		if (!has_messages_flag_.test_and_set()) {
			has_messages_flag_.notify_one();
		}
	}

	/*
		Waits until a message has beed pushed onto the queue.
		Returns immediately if the queue already contains message(s).
	*/
	void wait_for_next_() const {
		// fmt::print("Waiting to receive\n");
		has_messages_flag_.wait(false);
		// fmt::print("Finished waiting to receive\n");
	}

	std::queue<T> queue_;
	mutable std::mutex mutex_;

	// True when the queue is not empty.
	std::atomic_flag has_messages_flag_{};
};

template<std::move_constructible T>
class Sender final {
public:
	/*
		Sends a message through the channel without waiting.
	*/
	template<class ... Argument_> 
		requires std::constructible_from<T, Argument_&&...>
	void send(Argument_&& ... argument) {
		queue_->push(std::forward<Argument_>(argument)...);
	}
	/*
		Sends a message through the channel and waits until it has been received and taken off the queue.
	*/
	template<class ... Argument_> 
		requires std::constructible_from<T, Argument_&&...>
	void send_wait(Argument_&& ... argument) {
		queue_->push_wait(std::forward<Argument_>(argument)...);
	}

	/*
		Returns the number of messages that have been sent but not yet taken off the queue by the receiver.
	*/
	std::size_t recent_queue_size() const {
		return queue_->recent_size();
	}
	/*
		Returns whether any messages have been sent but not yet taken off the queue by the receiver.
	*/
	[[nodiscard]]
	bool was_queue_recently_empty() const {
		return queue_->was_recently_empty();
	}

	Sender(std::shared_ptr<MessageQueue<T>> queue) :
		queue_{std::move(queue)}
	{}
	
private:
	std::shared_ptr<MessageQueue<T>> queue_;
};

template<std::move_constructible T>
class Receiver final {
public:
	/*
		Waits for the next message and moves it from the queue.
	*/
	[[nodiscard]]
	T receive() {
		return queue_->take_next();
	}
	/*
		Waits for the next message and returns a copy of it.
		It is still left in the queue.
	*/
	[[nodiscard]]
	T receive_peek() const 
		requires std::copy_constructible<T>
	{
		return queue_->peek_next();
	}
	/*
		Removes the next message from the queue.
		Does nothing if there are currently no messages in the queue.
	*/
	void remove_next() {
		queue_->remove_next();
	}

	/*
		Returns the number of messages currently waiting in the queue to be received immediately.
	*/
	[[nodiscard]]
	std::size_t recent_queue_size() const {
		return queue_->recent_size();
	}
	/*
		Returns whether there are any messages waiting in the queue to be received immediately.
	*/
	[[nodiscard]]
	bool was_queue_recently_empty() const {
		return queue_->was_recently_empty();
	}

	Receiver(std::shared_ptr<MessageQueue<T>> queue) :
		queue_{std::move(queue)}
	{}

private:
	std::shared_ptr<MessageQueue<T>> queue_;
};

template<std::move_constructible T>
struct Channel final {
	Sender<T> sender;
	Receiver<T> receiver;
};

/*
	Creates a message channel.
	A message channel consists of a sender and a receiver, and is meant to be used to synchronize/communicate between threads.
	The sender and receiver privately share a thread-safe message queue (avo::concurrency::MessageQueue<T>) which they push and pop messages on. 
	The sender can wait for its message to be received and the receiver can wait for new messages to be sent.
*/
template<std::move_constructible T>
[[nodiscard]]
Channel<T> create_channel() {
	auto message_queue = std::make_shared<MessageQueue<T>>();
	return Channel{
		.sender = Sender{message_queue},
		.receiver = Receiver{std::move(message_queue)}
	};
}

} // namespace avo::concurrency

#endif
