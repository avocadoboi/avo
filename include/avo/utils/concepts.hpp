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

#ifndef AVO_UTILS_CONCEPTS_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_UTILS_CONCEPTS_HPP_BJORN_SUNDIN_JUNE_2021

#include <concepts>
#include <ranges>

namespace avo::utils {

template<class T>
concept IsTrivial = std::is_trivial_v<T>;

template<class T>
concept IsEnum = std::is_enum_v<T>;

template<class T>
concept IsObject = std::is_object_v<T>;

template<class T>
concept IsConst = std::is_const_v<T>;

//------------------------------

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsRange = std::ranges::range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsBorrowedRange = std::ranges::borrowed_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsSizedRange = std::ranges::sized_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsView = std::ranges::view<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsInputRange = std::ranges::input_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsBidirectionalRange = std::ranges::bidirectional_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsRandomAccessRange = std::ranges::random_access_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsContiguousRange = std::ranges::contiguous_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsCommonRange = std::ranges::common_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

template<class Range_, class Element_ = std::ranges::range_value_t<Range_>>
concept IsViewableRange = std::ranges::common_range<Range_> 
	&& std::same_as<std::ranges::range_value_t<Range_>, Element_>;

//------------------------------

namespace detail {

template<class T, template<class...> class U>
struct IsInstantiationOf : std::false_type {};

template<class ... T, template<class...> class U>
struct IsInstantiationOf<U<T...>, U> : std::true_type {};

} // namespace detail

template<class T, template<class...> class U>
concept IsInstantiationOf = detail::IsInstantiationOf<T, U>::value;

//------------------------------

template<class T, template<class...> class ... U>
concept IsInstantiationOfAny = (IsInstantiationOf<T, U> || ...);

template<class T, class ... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template<class T>
concept IsNumber = std::integral<T> || std::floating_point<T>;

template<class T>
concept IsByte = sizeof(T) == 1 && IsTrivial<std::remove_reference_t<T>>;

} // namespace avo::utils

#endif
