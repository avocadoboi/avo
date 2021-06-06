// Included exactly once.

#include "common.hpp"

#include <avo/unicode.hpp>

namespace avo::unicode {

void enable_utf8_console() {
	SetConsoleOutputCP(CP_UTF8);
}

std::optional<std::size_t> utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) 
{
	auto const length = MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(output.data()), static_cast<int>(output.size())
	);

	if (length == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return {};
	}
	return length;
}

std::u16string utf8_to_utf16(std::string_view const input) 
{
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

std::optional<std::size_t> utf16_to_utf8(std::u16string_view const input, std::span<char> const output) 
{
	auto const length = WideCharToMultiByte(
		CP_UTF8, 0,
		reinterpret_cast<wchar_t const*>(input.data()), static_cast<int>(input.size()),
		output.data(), static_cast<int>(output.size()),
		nullptr, nullptr
	);
	if (length == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return {};
	}
	return length;
}

std::string utf16_to_utf8(std::u16string_view const input) 
{
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

} // namespace avo::unicode
