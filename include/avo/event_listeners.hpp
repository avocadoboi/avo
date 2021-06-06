#ifndef AVO_EVENT_LISTENERS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_EVENT_LISTENERS_HPP_BJORN_SUNDIN_JUNE_2021

#include <algorithm>
#include <functional>
#include <ranges>

namespace avo {

template<class T>
class EventListeners;

/*
	This is a class used to easily manage event listeners. Any type of callable can be a listener.
	The return type and arguments have to be the same for all listeners added to one instance of EventListeners.
*/
template<class Return_, class ... Arguments_>
class EventListeners<Return_(Arguments_...)> final {
public:
	using FunctionType = Return_(Arguments_...);
	using ContainerType = std::vector<std::function<FunctionType>>;
	
	using Iterator = std::ranges::iterator_t<ContainerType>;
	using ConstIterator = std::ranges::iterator_t<ContainerType const>;
	
	[[nodiscard]]
	Iterator begin() {
		return listeners_.begin();
	}
	[[nodiscard]]
	ConstIterator begin() const {
		return listeners_.begin();
	}
	[[nodiscard]]
	Iterator end() {
		return listeners_.end();
	}
	[[nodiscard]]
	ConstIterator end() const {
		return listeners_.end();
	}

	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::operator+=.
	*/
	void add(std::function<FunctionType> listener) 
	{
		listeners_.emplace_back(std::move(listener));
	}
	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::add.
	*/
	EventListeners& operator+=(std::function<FunctionType> listener) 
	{
		add(std::move(listener));
		return *this;
	}

	/*
		Removes a listener from the EventListeners instance that matches the passed function.
		Equivalent to EventListeners::operator-=.
	*/
	void remove(std::function<FunctionType> const& listener) 
	{
		auto const& listener_type = listener.target_type();
		auto const found_position = std::ranges::find_if(listeners_, [&](auto const& listener_element) {
			// template keyword is used to expicitly tell the compiler that target is a template method for
			// std::function<FunctionType> and < shouldn't be parsed as the less-than operator
			return listener_type == listener_element.target_type() &&
				*(listener.template target<FunctionType>()) == *(listener_element.template target<FunctionType>());
		});

		if (found_position != listeners_.end()) 
		{
			*found_position = std::move(listeners_.back());
			listeners_.pop_back();
		}
	}
	/*
		Removes a listener from the EventListeners instance that matches the passed function.
		Equivalent to EventListeners::remove.
	*/
	EventListeners& operator-=(std::function<FunctionType> const& listener) 
	{
		remove(listener);
		return *this;
	}

	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::operator().
	*/
	void notify_all(Arguments_&& ... event_arguments) 
	{
		for (auto& listener : listeners_) {
			listener(std::forward<Arguments_>(event_arguments)...);
		}
	}
	/*
		Calls all of the listeners with event_arguments as arguments.
		Equivalent to EventListeners::notify_all.
	*/
	void operator()(Arguments_&& ... event_arguments) 
	{
		notify_all(std::forward<Arguments_>(event_arguments)...);
	}

private:
	ContainerType listeners_;
};

} // namespace avo

#endif
