#ifndef AVO_UTILS_UNIQUE_HANDLE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_UNIQUE_HANDLE_HPP_BJORN_SUNDIN_JUNE_2021

#include "concepts.hpp"

namespace avo::util {

/*
	Similar to std::unique_ptr except that non-pointer types can be held
	and that a custom deleter must be specified. 

	This is useful for OS handles that are integer types, for example a native socket handle.
	Use C++20 lambdas in unevaluated contexts to specify a deleter, or use an already defined 
	functor type. 
	
	Example:
	using DllHandle = avo::util::UniqueHandle<HMODULE, decltype([](auto& h){ FreeLibrary(h); })>;
*/
template<IsTrivial T, std::invocable<T> Deleter_, T invalid_handle = T{}>
class UniqueHandle final {
	T handle_{invalid_handle};

	void close() {
		if (handle_ != invalid_handle) {
			Deleter_{}(handle_);
			handle_ = invalid_handle;
		}
	}
public:
	[[nodiscard]]
	explicit operator T() const {
		return handle_;
	}
	[[nodiscard]]
	T get() const {
		return handle_;
	}
	[[nodiscard]]
	T& get() {
		return handle_;
	}

	[[nodiscard]]
	T const* operator->() const {
		return &handle_;
	}
	[[nodiscard]]
	T* operator->() {
		return &handle_;
	}

	[[nodiscard]]
	T const* operator&() const {
		return &handle_;
	}
	[[nodiscard]]
	T* operator&() {
		return &handle_;
	}

	[[nodiscard]]
	explicit operator bool() const {
		return handle_ != invalid_handle;
	}
	[[nodiscard]]
	bool operator!() const {
		return handle_ == invalid_handle;
	}

	UniqueHandle& operator=(T handle) {
		close();
		handle_ = handle;
		return *this;
	}

	UniqueHandle() = default;
	explicit UniqueHandle(T const handle) :
		handle_{handle}
	{}

	~UniqueHandle() {
		close();
	}

	UniqueHandle(UniqueHandle&& handle) noexcept :
		handle_{handle.handle_}
	{
		handle.handle_ = invalid_handle;
	}
	UniqueHandle& operator=(UniqueHandle&& handle) noexcept {
		handle_ = handle.handle_;
		handle.handle_ = invalid_handle;
		return *this;
	}

	UniqueHandle(UniqueHandle const&) = delete;
	UniqueHandle& operator=(UniqueHandle const&) = delete;
};

} // namespace avo::util

#endif
