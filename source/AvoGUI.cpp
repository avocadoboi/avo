#include "AvoGUI.hpp"

#ifdef _WIN32
#	include <windows.h>
#endif

namespace avo {

namespace utils {

auto enable_utf8_console() -> void {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	// Pretty much everyone else uses utf-8 by default.
}

auto utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) -> void {
	auto const length = MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(output.data()), static_cast<int>(output.size())
	);

	if (length > 0) {
		output[length] = 0;
	}
}
auto utf8_to_utf16(std::string_view const input) -> std::u16string {
	auto result = std::u16string(MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		0, 0
	), '\0');

	MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(result.data()), static_cast<int>(result.size())
	);

	return result;
}
auto utf8_to_utf16_unit_count(std::string_view const input) -> std::size_t {
	return MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		0, 0
	);
}

auto utf16_to_utf8(std::u16string_view const input, std::span<char> const output) -> void {
	auto const length = WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		output.data(), static_cast<int>(output.size()),
		nullptr, nullptr
	);

	if (length > 0) {
		output[length] = 0;
	}
}
auto utf16_to_utf8(std::u16string_view const input) -> std::string {
	auto result = std::string(WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		0, 0, nullptr, nullptr
	), '\0');

	WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		result.data(), static_cast<int>(result.size()),
		nullptr, nullptr
	);

	return result;
}
auto utf16_to_utf8_unit_count(std::u16string_view const input) -> std::size_t {
	return WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		0, 0, nullptr, nullptr
	);
}

} // namespace utils

} // namespace avo
