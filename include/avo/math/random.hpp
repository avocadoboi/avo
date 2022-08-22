#ifndef AVO_MATH_RANDOM_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_MATH_RANDOM_HPP_BJORN_SUNDIN_JUNE_2021

#include <random>
#include <ranges>

namespace avo::math {

/*
	A random number generator, a small abstraction on top of a subset 
	of the standard library random utilities.
*/
template<std::uniform_random_bit_generator Engine_ = std::mt19937>
class Random final {
public:
	Engine_& engine() {
		return engine_;
	}
	/*
		Generates a new random integer.
	*/
	Engine_::result_type next() {
		return engine_();
	}
	/*
		Generates a new uniformly distributed random floating point number in the range [min, max).
	*/
	template<std::floating_point T>
	[[nodiscard]]
	T next(T const min, T const max) {
		return std::uniform_real_distribution<T>{min, max}(engine_);
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
	template<std::integral T> requires (not std::same_as<T, bool>)
	[[nodiscard]]
	T next(T const min, T const max) {
		return std::uniform_int_distribution<T>{min, max}(engine_);
	}
	/*
		Generates a new uniformly distributed random integer in the range [0, max].
	*/
	template<std::integral T> requires (not std::same_as<T, bool>)
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
		return std::normal_distribution<T>{mean, standard_deviation}(engine_);
	}

	Random() = default;
	explicit Random(Engine_::result_type const seed) :
		engine_{seed}
	{}

private:
	Engine_ engine_{std::random_device{}()};
};
	
} // namespace avo::math

#endif
