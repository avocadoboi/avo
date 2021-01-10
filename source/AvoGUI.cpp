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

#include "AvoGUI.hpp"

#ifdef _WIN32
#	include <windows.h>
#else
#	include <iconv.h>
#endif

namespace avo {

namespace unicode {
auto enable_utf8_console() -> void {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	// Pretty much everyone else uses UTF-8 by default.
}

#ifndef _WIN32
using IconvHandle = utils::UniqueHandle<iconv_t, decltype([](iconv_t const handle){iconv_close(handle);})>;
#endif

auto utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) -> std::optional<std::size_t> {
#ifdef _WIN32
	auto const length = MultiByteToWideChar(
		CP_UTF8, 0,
		input.data(), static_cast<int>(input.size()),
		reinterpret_cast<wchar_t*>(output.data()), static_cast<int>(output.size())
	);

	if (length == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return {};
	}
	return length;
#else
	// I have no idea why the input string data parameter isn't char const**.
	// It shouldn't be modified, so a const_cast is made here.

	auto in_pointer = const_cast<char*>(input.data());
	auto in_bytes_left = input.size();
	auto out_pointer = reinterpret_cast<char*>(output.data());
	auto out_bytes_left = output.size()*sizeof(char16_t);

	if (iconv(
			IconvHandle{iconv_open("UTF-16LE", "UTF-8")}.get(), 
			&in_pointer, 
			&in_bytes_left, 
			&out_pointer, 
			&out_bytes_left
		) == static_cast<std::size_t>(-1)) 
	{
		return {};
	}
	
	return output.size() - out_bytes_left/sizeof(char16_t);
#endif
}
auto utf8_to_utf16(std::string_view const input) -> std::u16string {
#ifdef _WIN32
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
#else
	auto output = std::u16string(input.size(), u'\0');
	if (auto const length = utf8_to_utf16(input, std::span{output})) {
		// length includes null terminator because of std::span constructor.
		output.resize(*length);
		return output;
	}
	return {};
#endif
}

auto utf16_to_utf8(std::u16string_view const input, std::span<char> const output) -> std::optional<std::size_t> {
#ifdef _WIN32
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
#else
	auto in_pointer = const_cast<char*>(reinterpret_cast<char const*>(input.data()));
	auto in_bytes_left = input.size()*sizeof(char16_t);
	auto out_pointer = output.data();
	auto out_bytes_left = output.size();

	if (iconv(
			IconvHandle{iconv_open("UTF-8", "UTF-16")}.get(), 
			&in_pointer, 
			&in_bytes_left, 
			&out_pointer, 
			&out_bytes_left
		) == static_cast<std::size_t>(-1)) 
	{
		return {};
	}
	return output.size() - out_bytes_left;
#endif
}
auto utf16_to_utf8(std::u16string_view const input) -> std::string {
#ifdef _WIN32
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
#else
	auto output = std::string(input.size()*3, '\0');
	if (auto const length = utf16_to_utf8(input, std::span{output})) {
		output.resize(*length);
		return output;
	}
	return {};
#endif
}

} // namespace unicode


} // namespace avo
