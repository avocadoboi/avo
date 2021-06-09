#ifndef AVO_UTILS_MISCELLANEOUS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_MISCELLANEOUS_HPP_BJORN_SUNDIN_JUNE_2021

//------------------------------

#include "concepts.hpp"

#include <fstream>
#include <source_location>
#include <span>
#include <string>
#include <vector>

#include <fmt/format.h>

//------------------------------

namespace avo::utils {

template<class T, bool is_const>
using MaybeConst = std::conditional_t<is_const, 
	std::conditional_t<std::is_lvalue_reference_v<T>, std::remove_reference_t<T> const&,
		std::conditional_t<std::is_rvalue_reference_v<T>, std::remove_reference_t<T> const&&, T const>
	>, T>;

//------------------------------

/*
	This can be called when the program reaches a path that should never be reachable.
	It prints error output and exits the program.
*/
#ifdef __cpp_lib_source_location
[[noreturn]]
inline void unreachable(std::source_location const& source_location = std::source_location::current()) {
	fmt::print(stderr, "Reached an unreachable code path in file {}, in function {}, on line {}.\n", 
		source_location.file_name(), source_location.function_name(), source_location.line());
	std::exit(1);
}
#else
[[noreturn]]
inline void unreachable() {
	fmt::print(stderr, "Reached an unreachable code path, exiting.\n");
	std::exit(1);
}
#endif

//------------------------------

template<std::three_way_comparable T>
struct MinMax { 
	T min, max;

	[[nodiscard]]
	constexpr bool valid() const noexcept {
		return min < max;
	}

	[[nodiscard]]
	constexpr operator bool() const noexcept 
		requires std::convertible_to<T, bool>
	{
		return static_cast<bool>(min) || static_cast<bool>(max);
	}

	[[nodiscard]]
	constexpr bool operator==(MinMax const&) const = default;
};

template<class T>
concept IsMinMax = IsInstantiationOf<T, MinMax>;

//------------------------------

template<class T>
concept IsBitFlag = requires(T a, T const b) {
	requires IsEnum<T>;
	{ b | b } -> std::same_as<T>;
	{ a |= b } -> std::same_as<T&>;
	{ b & b } -> std::same_as<T>;
};

/*
	Returns true if "flag" is a bitwise subset of "flags".
*/
template<IsBitFlag T>
[[nodiscard]]
constexpr bool has_flag(T const flags, T const flag) noexcept {
	return (flags & flag) != T{};
}

//------------------------------

/*
	Removes all elements matching the value argument from a vector, without keeping the order of the elements.
*/
template<std::equality_comparable T>
std::vector<T>& unordered_erase(std::vector<T>& vector, T const& value) {
	for (auto pos = vector.begin(); 
		(pos = std::ranges::find(pos, vector.end(), value)) != vector.end();)
	{
		*pos = std::move(vector.back());
		vector.pop_back();
	}
	return vector;
}

//------------------------------

/*
	Used to invoke a lambda at the end of a scope.
*/
template<std::invocable T>
class [[nodiscard]] Cleanup final {
public:
	Cleanup() = delete;

	[[nodiscard]] 
	Cleanup(T&& callable) :
		callable_{std::forward<T>(callable)}
	{}

	~Cleanup() {
		callable_();
	}

	Cleanup(Cleanup&&) = delete;
	Cleanup& operator=(Cleanup&&) = delete;

	Cleanup(Cleanup const&) = delete;
	Cleanup& operator=(Cleanup const&) = delete;

private:
	T callable_;
};

//------------------------------

using DataVector = std::vector<std::byte>;
using DataView = std::span<std::byte const>;
using DataRange = std::span<std::byte>;

[[nodiscard]]
inline DataVector read_file(std::string const path) {
	auto file = std::ifstream(path.data(), std::ios::ate | std::ios::binary);

	if (!file) {
		return {};
	}

	// tellg() only returns a negative value if fail() is true (which it isn't because we just checked).
	auto result = DataVector(static_cast<std::size_t>(file.tellg()));
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(result.data()), static_cast<std::streamsize>(result.size()));

	return result;
}

template<std::ranges::contiguous_range DataRange_> requires IsByte<std::ranges::range_value_t<DataRange_>>
void write_to_file(DataRange_ const& data, std::string const& file_name) {
	// std::string because std::ofstream does not take std::string_view.
	auto file_stream = std::ofstream{file_name, std::ios::binary};
	file_stream.write(reinterpret_cast<char const*>(std::ranges::data(data)), std::ranges::size(data));
}

} // namespace avo::utils

#endif
