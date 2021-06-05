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

#ifndef AVO_MATH_RANDOM_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_RANDOM_HPP_BJORN_SUNDIN_JUNE_2021

#include <random>
#include <ranges>

namespace avo::math {

/*
	A random number generator, a small abstraction on top of a subset 
	of the standard library random utilities.
*/
class Random final {
public:
	/*
		Generates a new uniformly distributed random floating point number in the range [min, max).
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next(T const min, T const max) {
		return std::uniform_real_distribution<T>{min, max}(_engine);
	}
	/*
		Generates a new uniformly distributed random floating point number in the range [0, max).
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next(T const max) {
		return next(T{}, max);
	}
	/*
		Generates a new uniformly distributed random integer in the range [min, max].
	*/
	template<std::integral T> requires (!std::same_as<T, bool>)
	[[nodiscard]]
	T next(T const min, T const max) {
		return std::uniform_int_distribution<T>{min, max}(_engine);
	}
	/*
		Generates a new uniformly distributed random integer in the range [0, max].
	*/
	template<std::integral T> requires (!std::same_as<T, bool>)
	[[nodiscard]]
	T next(T const max) {
		return next(T{}, max);
	}

	/*
		Returns a random element in a random access range.
	*/
	template<std::ranges::random_access_range Range_>
	[[nodiscard]]
	std::ranges::range_reference_t<Range_> select(Range_& range) {
		return range[next(std::ranges::distance(range) - 1)];
	}
	
	/*
		Returns a random coin flip, true or false.
	*/
	template<std::same_as<bool> T>
	[[nodiscard]]
	T next() {
		return static_cast<T>(next(0, 1));
	}
	
	/*
		Generates a new random floating point number distributed according to a gaussian distribution
		with a mean and a standard deviation.
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next_normal(T const mean, T const standard_deviation) {
		return std::normal_distribution<T>{mean, standard_deviation}(_engine);
	}

	Random() = default;
	Random(std::uint_fast32_t const seed) :
		_engine{seed}
	{}

private:
	std::mt19937 _engine{std::random_device{}()};
};
	
} // namespace avo::math

#endif
