/*
MIT License

Copyright (c) 2021 Björn Sundin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef AVO_EVENT_LISTENERS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_EVENT_LISTENERS_HPP_BJORN_SUNDIN_JUNE_2021

#include <algorithm>
#include <functional>
#include <mutex>
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
	Iterator begin() noexcept {
		return _listeners.begin();
	}
	[[nodiscard]]
	ConstIterator begin() const noexcept {
		return _listeners.begin();
	}
	[[nodiscard]]
	Iterator end() noexcept {
		return _listeners.end();
	}
	[[nodiscard]]
	ConstIterator end() const noexcept {
		return _listeners.end();
	}

	/*
		Adds a listener to the EventListeners instance that will be called when nofity_all or operator() is called.
		Equivalent to EventListeners::operator+=.
	*/
	void add(std::function<FunctionType> listener) 
	{
		auto const lock = std::scoped_lock{_mutex};    
		_listeners.emplace_back(std::move(listener));
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
		auto const lock = std::scoped_lock{_mutex};

		auto const& listener_type = listener.target_type();
		auto const found_position = std::ranges::find_if(_listeners, [&](auto const& listener_element) {
			// template keyword is used to expicitly tell the compiler that target is a template method for
			// std::function<FunctionType> and < shouldn't be parsed as the less-than operator
			return listener_type == listener_element.target_type() &&
				*(listener.template target<FunctionType>()) == *(listener_element.template target<FunctionType>());
		});

		if (found_position != _listeners.end()) 
		{
			*found_position = std::move(_listeners.back());
			_listeners.pop_back();
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
		auto const lock = std::scoped_lock{_mutex};
		for (auto& listener : _listeners) {
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

	EventListeners() = default;

	EventListeners(EventListeners&& other) noexcept :
		_listeners{std::move(other._listeners)}
	{}
	EventListeners(EventListeners const&) = delete;

	EventListeners& operator=(EventListeners&& other) noexcept 
	{
		_listeners = std::move(other._listeners);
		return *this;
	}
	EventListeners& operator=(EventListeners const&) = delete;

private:
	std::recursive_mutex _mutex;
	ContainerType _listeners;
};

} // namespace avo

#endif
