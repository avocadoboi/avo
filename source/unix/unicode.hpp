// Included exactly once.

#include <avo/unicode.hpp>
#include <avo/utils/unique_handle.hpp>

#include <iconv.h>

namespace avo::unicode {

void enable_utf8_console() {
	// Pretty much everyone but Windows uses UTF-8 by default.
}

using IconvHandle = utils::UniqueHandle<iconv_t, decltype([](iconv_t const handle){ iconv_close(handle); })>;

std::optional<std::size_t> utf8_to_utf16(std::string_view const input, std::span<char16_t> const output) 
{
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
}

std::u16string utf8_to_utf16(std::string_view const input) 
{
	auto output = std::u16string(input.size(), u'\0');
	if (auto const length = utf8_to_utf16(input, std::span{output})) 
	{
		// length includes null terminator because of std::span constructor.
		output.resize(*length);
		return output;
	}
	return {};
}

std::optional<std::size_t> utf16_to_utf8(std::u16string_view const input, std::span<char> const output) 
{
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
}

std::string utf16_to_utf8(std::u16string_view const input) 
{
	auto output = std::string(input.size()*3, '\0');
	if (auto const length = utf16_to_utf8(input, std::span{output})) 
	{
		output.resize(*length);
		return output;
	}
	return {};
}

} // namespace avo::unicode
