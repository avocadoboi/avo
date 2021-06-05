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

#ifndef AVO_UTILS_UNIQUE_HANDLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_UNIQUE_HANDLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"

namespace avo::utils {

/*
	Similar to std::unique_ptr except that non-pointer types can be held
	and that a custom deleter must be specified. 

	This is useful for OS handles that are integer types, for example a native socket handle.
	Use C++20 lambdas in unevaluated contexts to specify a deleter, or use an already defined 
	functor type. 
	
	Example:
	using DllHandle = avo::utils::UniqueHandle<HMODULE, decltype([](auto& h){ FreeLibrary(h); })>;
*/
template<IsTrivial T, std::invocable<T> Deleter_, T invalid_handle = T{}>
class UniqueHandle final {
	T _handle{invalid_handle};

	void close() {
		if (_handle != invalid_handle) {
			Deleter_{}(_handle);
			_handle = invalid_handle;
		}
	}
public:
	[[nodiscard]]
	explicit operator T() const {
		return _handle;
	}
	[[nodiscard]]
	T get() const {
		return _handle;
	}
	[[nodiscard]]
	T& get() {
		return _handle;
	}

	[[nodiscard]]
	T const* operator->() const {
		return &_handle;
	}
	[[nodiscard]]
	T* operator->() {
		return &_handle;
	}

	[[nodiscard]]
	T const* operator&() const {
		return &_handle;
	}
	[[nodiscard]]
	T* operator&() {
		return &_handle;
	}

	[[nodiscard]]
	explicit operator bool() const {
		return _handle != invalid_handle;
	}
	[[nodiscard]]
	bool operator!() const {
		return _handle == invalid_handle;
	}

	UniqueHandle& operator=(T handle) {
		close();
		_handle = handle;
		return *this;
	}

	UniqueHandle() = default;
	explicit UniqueHandle(T const handle) :
		_handle{handle}
	{}

	~UniqueHandle() {
		close();
	}

	UniqueHandle(UniqueHandle&& handle) noexcept :
		_handle{handle._handle}
	{
		handle._handle = invalid_handle;
	}
	UniqueHandle& operator=(UniqueHandle&& handle) noexcept {
		_handle = handle._handle;
		handle._handle = invalid_handle;
		return *this;
	}

	UniqueHandle(UniqueHandle const&) = delete;
	UniqueHandle& operator=(UniqueHandle const&) = delete;
};

} // namespace avo::utils

#endif
