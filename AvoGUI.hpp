/*
	GUI library by Bj�rn Sundin aka Avocado Boy. It is designed to be logical,
	structured and beautiful from the perspective of the user of the library -
	while still being efficient. The library comes with some Material Design
	components ready to use as well as the Roboto font, making it easy to
	create good looking and feeling programs quickly.

	    (me)
         v
         --
   \   --  --   /
    \ /      \ /
     |   /\   |
     \   \/   /
      \      /
       ------
        | |
	
	Info about documentation: a class that has virtual methods starts the documentation of methods with either 
	"LIBRARY IMPLEMENTED" or "USER IMPLEMENTED". If a method is "USER IMPLEMENTED", it means that the method 
	has no default library implementation and is meant to be implemented by you, the user of the library. A 
	method that is "LIBRARY IMPLEMENTED" can sometimes be overridden (if it is virtual) and in that case you 
	can choose to keep the library implementation and only add to it (by calling Parent::method(...) from
	inside your overridden method), or you can completely override it - although that can sometimes remove
	important functionality.

	Submit issues on GitHub!
	https://github.com/avocadoboi/AvoGUI

	Started development in summer 2018.
*/

#pragma once

//------------------------------

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

// For debugging.
#include <iostream>

// Memory leak detection
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

//------------------------------

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

//------------------------------

namespace AvoGUI
{
	double const E =		2.71828182845904523;
	double const HALF_PI =	1.57079632679489661;
	double const PI =		3.14159265358979323;
	double const TAU =		6.28318530717958647;

	/*
		Returns a number multiplied by itself (x to the 2nd power, meaning x^2, meaning x*x). 
		Can be useful if you want to quickly square a longer expression.
	*/
	template<typename T>
	T square(T p_x)
	{
		return p_x * p_x;
	}

	/*
		Returns the square root of a float using a fast but less accurate algorithm.
	*/
	inline float fastSqrt(float p_x)
	{
		int32 bits = (*(int32*)&p_x - (1 << 23) >> 1) + (1 << 29);
		float approximation = *(float*)&bits;
		return (p_x / approximation + approximation) * 0.5f;
	}
	/*
		Returns the inverse square root of a float using a fast but less accurate algorithm.
	*/
	inline float fastInverseSqrt(float p_x)
	{
		int32 bits = 0x5f375a86 - (*(int32*)&p_x >> 1);
		float approximation = *(float*)&bits;
		return approximation * (1.5f - 0.5f*p_x*approximation*approximation);
	}

	/*
		Returns a random double between 0 and 1 from a uniform distribution. 
		It just uses the standard library random header. Convenient function.
	*/
	double random();
	/*
		Returns a random double between 0 and 1 from a normal distribution with standard deviation 1 and mean 0.
		It just uses the standard library random header. Convenient function.
	*/
	double randomNormal();

	/*
		Returns the biggest of two numbers.
	*/
	template<typename Type>
	Type max(Type p_a, Type p_b)
	{
		return p_a > p_b ? p_a : p_b;
	}
	/*
		Returns the biggest of three numbers.
	*/
	template<typename Type>
	Type max(Type p_a, Type p_b, Type p_c)
	{
		return p_a > p_b ? (p_a > p_c ? p_a : p_c) : (p_b > p_c ? p_b : p_c);
	}

	/*
		Returns the smallest of two numbers.
	*/
	template<typename Type>
	Type min(Type p_a, Type p_b)
	{
		return p_a < p_b ? p_a : p_b;
	}
	/*
		Returns the smallest of three numbers.
	*/
	template<typename Type>
	Type min(Type p_a, Type p_b, Type p_c)
	{
		return p_a < p_b ? (p_a < p_c ? p_a : p_c) : (p_b < p_c ? p_b : p_c);
	}

	//------------------------------

	/*
		Returns a value between p_start and p_end depending on p_progress. This is linear interpolation.
		If p_progress is below 0 or above 1, the returned value will not be within the start and end position.
	*/
	inline float interpolate(float p_start, float p_end, float p_progress)
	{
		return p_start * (1.f - p_progress) + p_end * p_progress;
	}
	/*
		Returns a value between p_start and p_end depending on p_progress. This is linear interpolation.
		If p_progress is below 0 or above 1, the returned value will not be within the start and end position.
	*/
	inline double interpolate(double p_start, double p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * p_progress;
	}
	/*
		Clips p_value so that the returned value is never below p_min or above p_max. 
		If p_min <= p_value <= p_max, then the returned value is equal to p_value.
	*/
	template<typename Type>
	Type constrain(Type p_value, Type p_min = 0, Type p_max = 1)
	{
		return p_value < p_min ? p_min : (p_value > p_max ? p_max : p_value);
	}

	//------------------------------

	/*
		Returns -1 if the number is negative, 0 if it's 0 and 1 if it's positive.
	*/
	template<typename Type>
	inline Type sign(Type p_number)
	{
		return (p_number > (Type)0) - (p_number < (Type)0);
	}

	//------------------------------

	/*
		Removes an element from a vector. The function returns true if the element existed in the vector and was removed.
	*/
	template<typename Type>
	bool removeVectorElementWhileKeepingOrder(std::vector<Type>& p_vector, Type p_value)
	{
		for (auto element = p_vector.begin(); element != p_vector.end(); element++)
		{
			if (*element == p_value)
			{
				p_vector.erase(element);
				return true;
			}
		}
		return false;
	}

	/*
		Removes an element from a vector without keeping the order of the elements in the vector, making it more efficient. 
		The function returns true if the element existed in the vector and was removed (replaced by the last element).
	*/
	template<typename Type>
	bool removeVectorElementWithoutKeepingOrder(std::vector<Type>& p_vector, Type p_element)
	{
		for (auto element = p_vector.begin(); element != p_vector.end(); element++)
		{
			if (*element == p_element)
			{
				*element = p_vector.back();
				p_vector.pop_back();
				return true;
			}
		}
		return false;
	}

	//------------------------------

	/*
		Converts a UTF-8 encoded char* string to a UTF-16 encoded wchar_t* string.
		p_input should be null-terminated.
		p_output should be allocated with p_numberOfCharactersInOutput number of wchar_t characters.
		The output includes the null terminator.
	*/
	void widenString(char const* p_input, wchar_t* p_output, uint32 p_numberOfCharactersInOutput);
	/*
		Returns the number of UTF-16 encoded wchar_t* characters that would be used to represent the same characters in a UTF-8 encoded char* string.
		p_input should be null-terminated. The output includes the null terminator.
	*/
	uint32 getNumberOfCharactersInWidenedString(char const* p_input);
	/*
		Converts a UTF-16 encoded wchar_t const* string to a UTF-8 encoded char* string.
		p_string should be null-terminated.
		p_result should be allocated with p_numberOfCharactersInResult number of char characters.
		The output includes the null terminator.
	*/
	void narrowString(wchar_t const* p_input, char* p_output, uint32 p_numberOfCharactersInResult);
	/*
		Returns the number of UTF-8 encoded char* characters that would be used to represent the same characters in a UTF-16 encoded wchar_t* string.
		p_input should be null-terminated. The output includes the null-terminator.
	*/
	uint32 getNumberOfCharactersInNarrowedString(wchar_t const* p_input);

	//------------------------------

	enum class RoundingType
	{
		Down, 
		Up, 
		Nearest
	};

	/*
		Converts a number to a string, using . (dot) for the decimal point.
	*/
	template<typename T>
	std::string convertNumberToString(T p_value)
	{
		std::ostringstream stream;
		stream.precision(10);
		stream << p_value;
		return stream.str();
	}

	/*
		Converts a number rounded at a certain digit to a string, using . (dot) for the decimal point.
		If p_numberOfDigitsToRound is 0, only all decimals are rounded off and it becomes an integer.
		Positive goes to the right and negative goes to the left.
	*/
	template<typename T>
	std::string convertNumberToString(T p_value, int32 p_roundingIndex, RoundingType p_roundingType = RoundingType::Nearest)
	{
		double roundingFactor = std::pow(10., p_roundingIndex);
		std::ostringstream stream;
		stream.precision(10);
		if (p_roundingType == RoundingType::Nearest)
		{
			stream << std::round(p_value*roundingFactor)/roundingFactor;
		}
		else if (p_roundingType == RoundingType::Down)
		{
			stream << std::floor(p_value * roundingFactor) / roundingFactor;
		}
		else
		{
			stream << std::ceil(p_value * roundingFactor) / roundingFactor;
		}
		return stream.str();
	}

	//------------------------------

	/*
		A 2D point/vector where x is the horizontal component and y is the vertical component if you were to think of it graphically.
	*/
	template<typename PointType = float>
	struct Point
	{
		PointType x, y;

		Point()
		{
			x = y = (PointType)0;
		}
		Point(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
		}
		/*
			Initializes the point with the same x and y coordinates.
		*/
		Point(PointType p_coordinate)
		{
			x = p_coordinate;
			y = p_coordinate;
		}
		template<typename T>
		Point(Point<T> const& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
		}
		template<typename T>
		Point(Point<T>&& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
		}

		//------------------------------

		Point<PointType> operator-() const
		{
			return Point<PointType>(-x, -y);
		}

		//------------------------------

		template<typename T>
		Point<PointType>& operator=(Point<T> const& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}
		template<typename T>
		Point<PointType>& operator=(Point<T>&& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}

		/*
			Sets the same value for the x and the y coordinates.
		*/
		Point<PointType>& operator=(PointType p_coordinate)
		{
			x = y = p_coordinate;
			return *this;
		}

		//------------------------------

		Point<PointType>& set(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
			return *this;
		}
		/*
			Sets the polar coordinates of the point.
			p_angle is the angle between the ray to the point and the x-axis, counter-clockwise.
			p_length is the distance from the origin of the coordinates.
		*/
		Point<PointType>& setPolar(double p_angle, double p_length = 1.f)
		{
			x = std::cos(p_angle) * p_length;
			y = std::sin(p_angle) * p_length;
			return *this;
		}

		//------------------------------

		bool operator==(Point<PointType> const& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		bool operator==(Point<PointType>&& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		bool operator==(PointType p_coordinate) const
		{
			return x == p_coordinate && y == p_coordinate;
		}

		//------------------------------

		bool operator!=(Point<PointType> const& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		bool operator!=(Point<PointType>&& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		bool operator!=(PointType p_coordinate) const
		{
			return x != p_coordinate || y != p_coordinate;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator+(Point<T> const& p_point) const
		{
			return Point<PointType>(x + (PointType)p_point.x, y + (PointType)p_point.y);
		}
		template<typename T>
		Point<PointType> operator+(Point<T>&& p_point) const
		{
			return Point<PointType>(x + (PointType)p_point.x, y + (PointType)p_point.y);
		}
		/*
			Returns a version of this point that is offset by an equal amount on the x- and y-axis.
		*/
		template<typename OffsetType>
		Point<PointType> operator+(OffsetType p_offset) const
		{
			return Point<PointType>(x + (PointType)p_offset, y + (PointType)p_offset);
		}
		Point<PointType> createAddedCopy(PointType p_x, PointType p_y) const
		{
			return Point<PointType>(x + p_x, y + p_y);
		}

		template<typename OffsetType>
		Point<PointType>& operator+=(Point<OffsetType> const& p_offset)
		{
			x += (PointType)p_offset.x;
			y += (PointType)p_offset.y;
			return *this;
		}
		Point<PointType>& operator+=(PointType p_offset)
		{
			x += p_offset;
			y += p_offset;
			return *this;
		}
		Point<PointType>& move(PointType p_x, PointType p_y)
		{
			x += p_x;
			y += p_y;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator-(Point<T> const& p_point) const
		{
			return Point<PointType>(x - (PointType)p_point.x, y - (PointType)p_point.y);
		}
		template<typename T>
		Point<PointType> operator-(Point<T>&& p_point) const
		{
			return Point<PointType>(x - (PointType)p_point.x, y - (PointType)p_point.y);
		}
		/*
			Returns a version of this point that is offset negatively by the same amount on the x- and y-axis.
		*/
		Point<PointType> operator-(PointType p_offset) const
		{
			return Point<PointType>(x - p_offset, y - p_offset);
		}
		Point<PointType> createSubtractedCopy(PointType p_x, PointType p_y) const
		{
			return Point<PointType>(x - p_x, y - p_y);
		}

		template<typename OffsetType>
		Point<PointType>& operator-=(Point<OffsetType> const& p_offset)
		{
			x -= (PointType)p_offset.x;
			y -= (PointType)p_offset.y;
			return *this;
		}
		Point<PointType>& operator-=(PointType p_offset)
		{
			x -= p_offset;
			y -= p_offset;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator*(Point<T> const& p_point) const
		{
			return Point<PointType>(x*(PointType)p_point.x, y*(PointType)p_point.y);
		}
		template<typename T>
		Point<PointType> operator*(Point<T>&& p_point) const
		{
			return Point<PointType>(x*(PointType)p_point.x, y*(PointType)p_point.y);
		}
		template<typename T>
		Point<PointType> operator*(T p_factor) const
		{
			return Point<PointType>(x*p_factor, y*p_factor);
		}
		Point<PointType> createMultipliedCopy(PointType p_x, PointType p_y) const
		{
			return Point<PointType>(x * p_x, y * p_y);
		}

		template<typename T>
		Point<PointType>& operator*=(Point<T> const& p_point)
		{
			x *= (PointType)p_point.x;
			y *= (PointType)p_point.y;
			return *this;
		}
		Point<PointType>& operator*=(double p_factor)
		{
			x *= p_factor;
			y *= p_factor;
			return *this;
		}

		//------------------------------

		template<typename T>
		Point<PointType> operator/(Point<T> const& p_point) const
		{
			return Point<PointType>(x / (PointType)p_point.x, y / (PointType)p_point.y);
		}
		template<typename T>
		Point<PointType> operator/(Point<T>&& p_point) const
		{
			return Point<PointType>(x / (PointType)p_point.x, y / (PointType)p_point.y);
		}
		Point<PointType> operator/(double p_divisor) const
		{
			return Point<PointType>(x / p_divisor, y / p_divisor);
		}
		Point<PointType> createDividedCopy(PointType p_x, PointType p_y) const
		{
			return Point<PointType>(x / p_x, y / p_y);
		}

		template<typename T>
		Point<PointType>& operator/=(Point<T> const& p_point)
		{
			x /= (PointType)p_point.x;
			y /= (PointType)p_point.y;
			return *this;
		}
		Point<PointType>& operator/=(double p_divisor)
		{
			x /= p_divisor;
			y /= p_divisor;
			return *this;
		}

		//------------------------------

		template<typename T>
		double getDotProduct(Point<T> const& p_point) const
		{
			return x * p_point.x + y * p_point.y;
		}
		double getDotProduct(double p_x, double p_y) const
		{
			return x * p_x + y * p_y;
		}

		template<typename T>
		double getCrossProduct(Point<T> const& p_point) const
		{
			return x * p_point.x - y * p_point.x;
		}
		double getCrossProduct(double p_x, double p_y) const
		{
			return x * p_y - y * p_x;
		}

		//------------------------------

		/*
			Calculates the length of the 2d vector with pythagorean theorem. 
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		PointType getLengthSquared() const
		{
			return x * x + y * y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of the 2d vector with pythagorean teorem.
		*/
		double getLength() const
		{
			return sqrt(x*x + y * y);
		}
		/*
			Uses a fast but less accurate algorithm to calculate the length of the 2d vector with pythagorean teorem.
		*/
		double getLengthFast() const
		{
			return fastSqrt(x*x + y * y);
		}

		/*
			Calculates the distance between this point and another point with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T>
		double getDistanceSquared(Point<T> const& p_point)
		{
			return (x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y);
		}
		/*
			Calculates the distance between this point and another point with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		double getDistanceSquared(PointType p_x, PointType p_y)
		{
			return (x - p_x)*(x - p_x) + (y - p_y)*(y - p_y);
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T>
		double getDistance(Point<T> const& p_point)
		{
			return sqrt((x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		double getDistance(PointType p_x, PointType p_y)
		{
			return sqrt((x - p_x)*(x - p_x) + (y - p_y)*(y - p_y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		template<typename T>
		double getDistanceFast(Point<T> const& p_point)
		{
			return sqrtFast((x - p_point.x)*(x - p_point.x) + (y - p_point.y)*(y - p_point.y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between this point and another point with pythagorean theorem.
		*/
		double getDistanceFast(PointType p_x, PointType p_y)
		{
			return sqrtFast((x - p_x)*(x - p_x) + (y - p_y)*(y - p_y));
		}

		//------------------------------

		/*
			Calculates the length of a 2d vector with pythagorean theorem.
			This is faster than getLength() and getLengthFast() since no square root is needed, so use this one when you can!
		*/
		static double getLengthSquared(float p_x, float p_y)
		{
			return p_x * p_x + p_y * p_y;
		}
		/*
			Uses an accurate but slower algorithm to calculate the length of a 2d vector with pythagorean teorem.
		*/
		static double getLength(float p_x, float p_y)
		{
			return sqrt(p_x * p_x + p_y * p_y);
		}
		/*
			Uses a fast but less accurate algorithm to calculate the length of a 2d vector with pythagorean teorem.
		*/
		static double getLengthFast(float p_x, float p_y)
		{
			return fastSqrt(p_x * p_x + p_y * p_y);
		}

		/*
			Calculates the distance between two points with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T>
		static T getDistanceSquared(Point<T> const& p_point_0, Point<T> const& p_point_1)
		{
			return (p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y);
		}
		/*
			Calculates the distance between two points with pythagorean theorem.
			This is faster than getDistance() and getDistanceFast() since no square root is needed, so use this one when you can!
		*/
		template<typename T>
		static T getDistanceSquared(T p_x0, T p_y0, T p_x1, T p_y1)
		{
			return double((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1>
		static double getDistance(Point<T0> const& p_point_0, Point<T1> const& p_point_1)
		{
			return sqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		/*
			Uses an accurate but slower algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T>
		static double getDistance(T p_x0, T p_y0, T p_x1, T p_y1)
		{
			return sqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T0, typename T1>
		static float getDistanceFast(Point<T0> const& p_point_0, Point<T1> const& p_point_1)
		{
			return fastSqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		/*
			Uses a fast but less accurate algorithm to calculate the distance between two points with pytagorean theorem.
		*/
		template<typename T>
		static float getDistanceFast(T p_x0, T p_y0, T p_x1, T p_y1)
		{
			return fastSqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}

		//------------------------------

		/*
			Rotates the vector anticlockwise by p_angle radians so that it keeps its length.
		*/
		void rotate(double p_angle)
		{
			PointType xBefore = x;
			double cos = std::cos(p_angle);
			double sin = std::sin(p_angle);
			x = x * cos - y * sin;
			y = y * cos + xBefore * sin;
		}
		/*
			Rotates the point anticlockwise relative to (p_originX, p_originY) by p_angle radians so that it keeps its distance from that origin point.
		*/
		void rotate(double p_angle, PointType p_originX, PointType p_originY)
		{
			PointType xBefore = x;
			double cos = std::cos(p_angle);
			double sin = std::sin(p_angle);
			x = (x - p_originX) * cos - (y - p_originY) * sin + p_originX;
			y = (y - p_originY) * cos + (xBefore - p_originX) * sin + p_originY;

		}
		/*
			Rotates the point anticlockwise relative to p_origin by p_angle radians so that it keeps its distance from p_origin.
		*/
		void rotate(double p_angle, Point<PointType> const& p_origin)
		{
			rotate(p_angle, p_origin.x, p_origin.y);
		}

		/*
			Rotates the vector so that its angle is equal to p_angle radians.
		*/
		void setAngle(double p_angle)
		{
			double length = getLength();
			x = std::cos(p_angle) * length;
			y = std::sin(p_angle) * length;
		}
		/*
			Rotates the vector so that its angle relative to (p_originX, p_originY) is p_angle radians.
		*/
		void setAngle(double p_angle, PointType p_originX, PointType p_originY)
		{
			double length = getDistance(p_originX, p_originY);
			x = std::cos(p_angle) * length + p_originX;
			y = std::sin(p_angle) * length + p_originY;
		}
		/*
			Rotates the vector so that its angle relative to p_origin is p_angle radians.
		*/
		void setAngle(double p_angle, Point<PointType> const& p_origin)
		{
			setAngle(p_angle, p_origin.x, p_origin.y);
		}

		/*
			Returns the angle between the ray to the point and the x-axis, in radians and in the range [0, 2pi].
		*/
		double getAngle() const
		{
			if (!x && !y)
			{
				return 0.;
			}
			double atan2 = std::atan2(y, x);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}
		/*
			Returns the angle between the ray to the point and the x-axis, relative to (p_originX, p_originY).
			Angle is in radians and in the range [0, 2pi].
		*/
		double getAngle(PointType p_originX, PointType p_originY) const
		{
			if (x == p_originX && y == p_originY)
			{
				return 0.;
			}
			double atan2 = std::atan2(y - p_originY, x - p_originX);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}
		/*
			Returns the angle between the ray to the point and the x-axis, relative to p_origin.
			Angle is in radians and in the range [0, 2pi].
		*/
		double getAngle(Point<PointType> const& p_origin) const
		{
			if (x == p_origin.x && y == p_origin.y)
			{
				return 0.;
			}
			double atan2 = std::atan2(y - p_origin.y, x - p_origin.x);
			if (atan2 < 0.)
			{
				return atan2 + TAU;
			}
			return atan2;
		}

		//------------------------------

		/*
			Uses an accurate but slower algorithm to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		Point<PointType>& normalize()
		{
			float length = sqrt(x*x + y * y);
			x /= length;
			y /= length;
			return *this;
		}
		/*
			Uses a fast but less accurate algorithm to set the length of the 2d vector to 1.
			The angle remains the same.
		*/
		Point<PointType>& normalizeFast()
		{
			float inverseLength = fastInverseSqrt(x*x + y * y);
			x *= inverseLength;
			y *= inverseLength;
			return *this;
		}
	};

	template<typename T1, typename T2>
	Point<T2> operator*(T1 p_factor, Point<T2> const& p_point)
	{
		return Point<T2>(p_point.x * p_factor, p_point.y * p_factor);
	}
	template<typename T1, typename T2>
	Point<T2> operator/(T1 p_dividend, Point<T2> const& p_point)
	{
		return Point<T2>(p_dividend / p_point.x, p_dividend/p_point.y);
	}

	/*
		Linearly interpolates between p_start and p_end. This means we are calculating a point on the line segment between the two points.
	*/
	template<typename T>
	Point<T> interpolate(Point<T> const& p_start, Point<T> const& p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * (p_progress);
	}

	//------------------------------

#pragma region Rectangles

	class ProtectedRectangle;

	/*
		A 2D axis-aligned rectangle. right > left and bottom > top. 
	*/
	template<typename RectangleType = float>
	class Rectangle
	{
	public:
		RectangleType left, top, right, bottom;

		Rectangle()
		{
			left = top = right = bottom = (RectangleType)0;
		}
		Rectangle(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			set(p_left, p_top, p_right, p_bottom);
		}
		template<typename PositionType, typename SizeType>
		Rectangle(Point<PositionType> const& p_position, Point<SizeType> const& p_size)
		{
			set(p_position, p_size);
		}
		template<typename ParameterRectangleType>
		Rectangle(Rectangle<ParameterRectangleType> const& p_rectangle)
		{
			*this = p_rectangle;
		}
		template<typename ParameterRectangleType>
		Rectangle(Rectangle<ParameterRectangleType>&& p_rectangle)
		{
			*this = p_rectangle;
		}

		//------------------------------

		void set(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			left = p_left;
			top = p_top;
			right = p_right;
			bottom = p_bottom;
		}
		template<typename PositionType, typename SizeType>
		void set(Point<PositionType> const& p_position, Point<SizeType> const& p_size)
		{
			left = (RectangleType)p_position.x;
			top = (RectangleType)p_position.y;
			right = left + (RectangleType)p_size.x;
			bottom = top + (RectangleType)p_size.y;
		}

		//------------------------------

		template<typename ParameterRectangleType>
		Rectangle<RectangleType>& operator=(Rectangle<ParameterRectangleType> const& p_rectangle)
		{
			left = (RectangleType)p_rectangle.left;
			top = (RectangleType)p_rectangle.top;
			right = (RectangleType)p_rectangle.right;
			bottom = (RectangleType)p_rectangle.bottom;
			return *this;
		}
		template<typename ParameterRectangleType>
		Rectangle<RectangleType>& operator=(Rectangle<ParameterRectangleType>&& p_rectangle)
		{
			left = (RectangleType)p_rectangle.left;
			top = (RectangleType)p_rectangle.top;
			right = (RectangleType)p_rectangle.right;
			bottom = (RectangleType)p_rectangle.bottom;
			return *this;
		}

		//------------------------------

		/*
			Offsets the position of the rectangle.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& operator+=(Point<OffsetType> const& p_offset)
		{
			left += (RectangleType)p_offset.x;
			top += (RectangleType)p_offset.y;
			right += (RectangleType)p_offset.x;
			bottom += (RectangleType)p_offset.y;
			return *this;
		}
		/*
			Offsets the position of the rectangle negatively.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& operator-=(Point<OffsetType> const& p_offset)
		{
			left -= (RectangleType)p_offset.x;
			top -= (RectangleType)p_offset.y;
			right -= (RectangleType)p_offset.x;
			bottom -= (RectangleType)p_offset.y;
			return *this;
		}

		//------------------------------

		template<typename ParameterRectangleType>
		bool operator==(Rectangle<ParameterRectangleType> const& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}
		template<typename ParameterRectangleType>
		bool operator==(Rectangle<ParameterRectangleType>&& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}

		template<typename ParameterRectangleType>
		bool operator!=(Rectangle<ParameterRectangleType> const& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}
		template<typename ParameterRectangleType>
		bool operator!=(Rectangle<ParameterRectangleType>&& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}

		//------------------------------

		Rectangle<RectangleType> createCopyAtOrigin() const
		{
			return Rectangle<RectangleType>(0, 0, right - left, bottom - top);
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new top and left edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopLeft(RectangleType p_topAndLeft, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_topAndLeft, p_topAndLeft, p_willKeepSize*(p_topAndLeft - left) + right, p_willKeepSize*(p_topAndLeft - top) + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new top-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType> createCopyWithTopLeft(Point<PositionType> const& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_position.x, p_position.y, p_willKeepSize*(p_position.x - left) + right, p_willKeepSize*(p_position.y - top) + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new top-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_left, p_top, p_willKeepSize*(p_left - left) + right, p_willKeepSize*(p_top - top) + bottom);
		}

		/*
			Sets the same position for the top and left edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopLeft(RectangleType p_topAndLeft, bool p_willKeepSize = true)
		{
			return setTopLeft(p_topAndLeft, p_topAndLeft, p_willKeepSize);
		}
		/*
			Sets the top left coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType>& setTopLeft(Point<PositionType> const& p_position, bool p_willKeepSize = true)
		{
			return setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the top left coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				right += p_left - left;
				bottom += p_top - top;
			}
			left = p_left;
			top = p_top;
			return *this;
		}
		/*
			Returns the top left coordinates of the rectangle.
		*/
		Point<RectangleType> getTopLeft() const
		{
			return Point<RectangleType>(left, top);
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new top and right edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopRight(RectangleType p_topAndRight, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_topAndRight - right) + left, p_topAndRight, p_topAndRight, p_willKeepSize*(p_topAndRight - top) + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new top-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType> createCopyWithTopRight(Point<PositionType> const& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_position.x - right) + left, p_position.y, p_position.x, p_willKeepSize*(p_position.y - top) + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new top-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_right - right) + left, p_top, p_right, p_willKeepSize*(p_top - top) + bottom);
		}
		/*
			Sets the same position for the top and right edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopRight(RectangleType p_topAndRight, bool p_willKeepSize = true)
		{
			return setTopRight(p_topAndRight, p_topAndRight, p_willKeepSize);
		}
		/*
			Sets the top right coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType>& setTopRight(Point<PositionType> const& p_position, bool p_willKeepSize = true)
		{
			return setTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the top right coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				left += p_right - right;
				bottom += p_top - top;
			}
			right = p_right;
			top = p_top;
			return *this;
		}
		/*
			Returns the top right coordinates of the rectangle.
		*/
		Point<RectangleType> getTopRight() const
		{
			return Point<RectangleType>(right, top);
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new bottom and left edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomLeft(RectangleType p_bottomAndLeft, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_bottomAndLeft, p_willKeepSize*(p_bottomAndLeft - bottom) + top, (p_bottomAndLeft - left) + right, p_bottomAndLeft);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType> createCopyWithBottomLeft(Point<PositionType> const& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_position.x, p_willKeepSize*(p_position.y - bottom) + top, (p_position.x - left) + right, p_position.y);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-left corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_left, p_willKeepSize*(p_bottom - bottom) + top, (p_left - left) + right, p_bottom);
		}
		/*
			Sets the same position for the bottom and left edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomLeft(RectangleType p_bottomAndLeft, bool p_willKeepSize = true)
		{
			return setBottomLeft(p_bottomAndLeft, p_bottomAndLeft, p_willKeepSize);
		}
		/*
			Sets the bottom left coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType>& setBottomLeft(Point<PositionType> const& p_position, bool p_willKeepSize = true)
		{
			return setBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the bottom left coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				right += p_left - left;
				top += p_bottom - bottom;
			}
			left = p_left;
			bottom = p_bottom;
			return *this;
		}
		/*
			Returns the bottom left coordinates of the rectangle.
		*/
		Point<RectangleType> getBottomLeft() const
		{
			return Point<RectangleType>(left, bottom);
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new bottom and right edge coordinate.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_bottomAndRight, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_bottomAndRight - right) + left, p_willKeepSize*(p_bottomAndRight - bottom) + top, p_bottomAndRight, p_bottomAndRight);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType> createCopyWithBottomRight(Point<PositionType> const& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_position.x - right) + left, p_willKeepSize*(p_position.y - bottom) + top, p_position.x, p_position.y);
		}
		/*
			Creates a copy of this rectangle, with a new bottom-right corner.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_right - right) + left, p_willKeepSize*(p_bottom - bottom) + top, p_right, p_bottom);
		}
		/*
			Sets the same position for the bottom and right edge of the rectangle.
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomRight(RectangleType p_bottomAndRight, bool p_willKeepSize = true)
		{
			return setBottomRight(p_bottomAndRight, p_bottomAndRight, p_willKeepSize);
		}
		/*
			Sets the bottom right coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		template<typename PositionType>
		Rectangle<RectangleType>& setBottomRight(Point<PositionType> const& p_position, bool p_willKeepSize = true)
		{
			return setBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			Sets the bottom right coordinates of the rectangle. 
			If p_willKeepSize is true, the rectangle will only get moved, keeping its size.
		*/
		Rectangle<RectangleType>& setBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true)
		{
			if (p_willKeepSize)
			{
				left += p_right - right;
				top += p_bottom - bottom;
			}
			right = p_right;
			bottom = p_bottom;
			return *this;
		}
		/*
			Returns the bottom right coordinates of the rectangle.
		*/
		Point<RectangleType> getBottomRight() const
		{
			return Point<RectangleType>(right, bottom);
		}

		//------------------------------

		/*
			Sets the left coordinate of the rectangle.
			If p_willKeepWidth is true, the right coordinate will be changed so that the width stays the same.
		*/
		Rectangle<RectangleType>& setLeft(RectangleType p_left, bool p_willKeepWidth = true)
		{
			if (p_willKeepWidth)
			{
				right += p_left - left;
			}
			left = p_left;
			return *this;
		}
		/*
			Sets the top coordinate of the rectangle.
			If p_willKeepHeight is true, the bottom coordinate will be changed so that the height stays the same.
		*/
		Rectangle<RectangleType>& setTop(RectangleType p_top, bool p_willKeepHeight = true)
		{
			if (p_willKeepHeight)
			{
				bottom += p_top - top;
			}
			top = p_top;
			return *this;
		}
		/*
			Sets the right coordinate of the rectangle.
			If p_willKeepWidth is true, the left coordinate will be changed so that the width stays the same.
		*/
		Rectangle<RectangleType>& setRight(RectangleType p_right, bool p_willKeepWidth = true)
		{
			if (p_willKeepWidth)
			{
				left += p_right - right;
			}
			right = p_right;
			return *this;
		}
		/*
			Sets the bottom coordinate of the rectangle.
			If p_willKeepWidth is true, the top coordinate will be changed so that the height stays the same.
		*/
		Rectangle<RectangleType>& setBottom(RectangleType p_bottom, bool p_willKeepHeight = true)
		{
			if (p_willKeepHeight)
			{
				top += p_bottom - bottom;
			}
			bottom = p_bottom;
			return *this;
		}

		//------------------------------

		/*
			Returns a copy of the rectangle where the coordinates are rounded in the directions that expand the rectangle.
		*/
		Rectangle<RectangleType> createCopyWithOutwardsRoundedCoordinates() const
		{
			Rectangle<RectangleType> rounded;
			rounded.left = floor(left);
			rounded.top = floor(top);
			rounded.right = ceil(right);
			rounded.bottom = ceil(bottom);
			return rounded;
		}
		/*
			Rounds the coordinates of the rectangle in the directions that expand the rectangle.
		*/
		Rectangle<RectangleType>& roundCoordinatesOutwards()
		{
			left = floor(left);
			top = floor(top);
			right = ceil(right);
			bottom = ceil(bottom);
			return *this;
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, with a new center position for both the x-axis and the y-axis.
		*/
		Rectangle<RectangleType> createCopyWithCenter(RectangleType p_centerXY) const
		{
			RectangleType offsetX = (RectangleType)p_centerXY - (left + right) / 2;
			RectangleType offsetY = (RectangleType)p_centerXY - (top + bottom) / 2;
			return Rectangle<RectangleType>(offsetX + left, offsetY + top, offsetX + right, offsetY + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new center position.
		*/
		template<typename PositionType>
		Rectangle<RectangleType> createCopyWithCenter(Point<PositionType> const& p_position) const
		{
			RectangleType offsetX = (RectangleType)p_position.x - (left + right)/2;
			RectangleType offsetY = (RectangleType)p_position.y - (top + bottom)/2;
			return Rectangle<RectangleType>(offsetX + left, offsetY + top, offsetX + right, offsetY + bottom);
		}
		/*
			Creates a copy of this rectangle, with a new center position.
		*/
		Rectangle<RectangleType> createCopyWithCenter(RectangleType p_centerX, RectangleType p_centerY) const
		{
			RectangleType offsetX = p_centerX - (left + right)/2;
			RectangleType offsetY = p_centerY - (top + bottom)/2;
			return Rectangle<RectangleType>(offsetX + left, offsetY + top, offsetX + right, offsetY + bottom);
		}
		/*
			Sets the same center coordinates of the rectangle for the x-axis and the y-axis. 
		*/
		Rectangle<RectangleType>& setCenter(RectangleType p_centerXY)
		{
			return setCenter(p_centerXY, p_centerXY);
		}
		/*
			Sets the center coordinates of the rectangle by moving it. 
		*/
		template<typename PositionType>
		Rectangle<RectangleType>& setCenter(Point<PositionType> const& p_position)
		{
			return setCenter(p_position.x, p_position.y);
		}
		/*
			Sets the center coordinates of the rectangle by moving it. 
		*/
		Rectangle<RectangleType>& setCenter(RectangleType p_x, RectangleType p_y)
		{
			RectangleType halfWidth = (right - left) / 2;
			RectangleType halfHeight = (bottom - top) / 2;
			left = p_x - halfWidth;
			top = p_y - halfHeight;
			right = p_x + halfWidth;
			bottom = p_y + halfHeight;
			return *this;
		}
		/*
			Sets the horizontal center coordinate of the rectangle by moving it.
		*/
		Rectangle<RectangleType>& setCenterX(RectangleType p_x)
		{
			RectangleType halfWidth = (right - left) / 2;
			left = p_x - halfWidth;
			right = p_x + halfWidth;
			return *this;
		}
		/*
			Sets the vertical center coordinate of the rectangle by moving it.
		*/
		Rectangle<RectangleType>& setCenterY(RectangleType p_y)
		{
			RectangleType halfHeight = (bottom - top) / 2;
			top = p_y - halfHeight;
			bottom = p_y + halfHeight;
			return *this;
		}
		/*
			Returns the center coordinates of the rectangle.
		*/
		Point<RectangleType> getCenter() const
		{
			return Point<RectangleType>((left + right) / 2, (top + bottom) / 2);
		}
		/*
			Returns the x-axis center coordinate of the rectangle.
		*/
		RectangleType getCenterX() const
		{
			return (left + right) / 2;
		}
		/*
			Returns the y-axis center coordinate of the rectangle.
		*/
		RectangleType getCenterY() const
		{
			return (top + bottom) / 2;
		}

		//------------------------------

		/*
			Moves the left and top coordinates of the rectangle without affecting the other two.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& moveTopLeft(Point<OffsetType> const& p_offset)
		{
			left += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		/*
			Moves the left and top coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveTopLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		/*
			Moves the right and top coordinates of the rectangle without affecting the other two.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& moveTopRight(Point<OffsetType> const& p_offset)
		{
			right += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		/*
			Moves the right and top coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveTopRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		/*
			Moves the left and bottom coordinates of the rectangle without affecting the other two.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& moveBottomLeft(Point<OffsetType> const& p_offset)
		{
			left += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		/*
			Moves the left and bottom coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveBottomLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		/*
			Moves the right and bottom coordinates of the rectangle without affecting the other two.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& moveBottomRight(Point<OffsetType> const& p_offset)
		{
			right += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		/*
			Moves the right and bottom coordinates of the rectangle without affecting the other two.
		*/
		Rectangle<RectangleType>& moveBottomRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/*
			Creates a copy of this rectangle, offseted by an amount.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType> createMovedCopy(Point<OffsetType> const& p_offset) const
		{
			return Rectangle<RectangleType>(left + p_offset.x, top + p_offset.y, right + p_offset.x, bottom + p_offset.y);
		}
		/*
			Creates a copy of this rectangle, offseted by an amount.
		*/
		Rectangle<RectangleType> createMovedCopy(RectangleType p_offsetX, RectangleType p_offsetY) const
		{
			return Rectangle<RectangleType>(left + p_offsetX, top + p_offsetY, right + p_offsetX, bottom + p_offsetY);
		}
		/*
			Does the same as the += operator, offsets the whole rectangle.
		*/
		template<typename OffsetType>
		Rectangle<RectangleType>& move(Point<OffsetType> const& p_offset)
		{
			left += p_offset.x;
			right += p_offset.x;
			top += p_offset.y;
			bottom += p_offset.y;
			return *this;
		}
		/*
			Does the same as the += operator, offsets the whole rectangle.
		*/
		Rectangle<RectangleType>& move(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			right += p_offsetX;
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}
		/*
			Offsets the left and right coordinates by an amount.
		*/
		Rectangle<RectangleType>& moveX(RectangleType p_offsetX)
		{
			left += p_offsetX;
			right += p_offsetX;
			return *this;
		}
		/*
			Offsets the top and bottom coordinates by an amount.
		*/
		Rectangle<RectangleType>& moveY(RectangleType p_offsetY)
		{
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/*
			Sets the width and height of the rectangle, changing only the right and bottom coordinates.
		*/
		template<typename SizeType>
		Rectangle<RectangleType>& setSize(Point<SizeType> const& p_size)
		{
			return setSize(p_size.x, p_size.y);
		}
		/*
			Sets the width and height of the rectangle, changing only the right and bottom coordinates.
		*/
		Rectangle<RectangleType>& setSize(RectangleType p_width, RectangleType p_height)
		{
			right = left + p_width;
			bottom = top + p_height;
			return *this;
		}
		/*
			Returns a point representing the size of the rectangle, where the x component is width and y is height.
		*/
		Point<RectangleType> getSize() const
		{
			return Point<RectangleType>(right - left, bottom - top);
		}

		/*
			Sets the width of the rectangle, changing only the right coordinate.
		*/
		Rectangle<RectangleType>& setWidth(RectangleType p_width)
		{
			right = left + p_width;
			return *this;
		}
		/*
			Returns the distance between right and left coordinates of the rectangle.
		*/
		RectangleType getWidth() const
		{
			return right - left;
		}

		/*
			Sets the height of the rectangle, changing only the bottom coordinate.
		*/
		Rectangle<RectangleType>& setHeight(RectangleType p_height)
		{
			bottom = top + p_height;
			return *this;
		}
		/*
			Returns the distance between bottom and top coordinates of the rectangle.
		*/
		RectangleType getHeight() const
		{
			return bottom - top;
		}

		//------------------------------

		/*
			Returns a new copy of this rectangle, that is clipped to fit into the parameter rectangle.
		*/
		template<typename ParameterRectangleType>
		Rectangle<RectangleType> createBoundedCopy(Rectangle<ParameterRectangleType> const& p_bounds) const
		{
			Rectangle<RectangleType> bounded;
			bounded.left = constrain(left, p_bounds.left, p_bounds.right);
			bounded.top = constrain(top, p_bounds.top, p_bounds.bottom);
			bounded.right = constrain(right, p_bounds.left, p_bounds.right);
			bounded.bottom = constrain(bottom, p_bounds.top, p_bounds.bottom);

			return bounded;
		}
		/*
			Returns a new copy of this rectangle, that is clipped to fit into the parameter rectangle.
		*/
		Rectangle<RectangleType> createBoundedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			Rectangle<RectangleType> bounded;
			bounded.left = constrain(left, p_left, p_right);
			bounded.top = constrain(top, p_top, p_bottom);
			bounded.right = constrain(right, p_left, p_right);
			bounded.bottom = constrain(bottom, p_top, p_bottom);

			return bounded;
		}

		/*
			Clips this rectangle to fit into the parameter rectangle.
		*/
		template<typename ParameterRectangleType>
		Rectangle<RectangleType>& bound(Rectangle<ParameterRectangleType> const& p_bounds)
		{
			left = constrain(left, p_bounds.left, p_bounds.right);
			top = constrain(top, p_bounds.top, p_bounds.bottom);
			right = constrain(right, p_bounds.left, p_bounds.right);
			bottom = constrain(bottom, p_bounds.top, p_bounds.bottom);

			return *this;
		}
		/*
			Clips this rectangle to fit into the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType>& bound(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			left = constrain(left, p_left, p_right);
			top = constrain(top, p_top, p_bottom);
			right = constrain(right, p_left, p_right);
			bottom = constrain(bottom, p_top, p_bottom);

			return *this;
		}

		//------------------------------

		/*
			Returns a copy of this rectangle that is extended so that it contains the parameter rectangle.
		*/
		template<typename ParameterRectangleType>
		Rectangle<RectangleType> createContainedCopy(Rectangle<ParameterRectangleType> const& p_rectangle) const
		{
			Rectangle<RectangleType> contained;
			contained.left = min(left, p_rectangle.left);
			contained.top = min(top, p_rectangle.top);
			contained.right = max(right, p_rectangle.right);
			contained.bottom = max(bottom, p_rectangle.bottom);

			return contained;
		}
		/*
			Returns a copy of this rectangle that is extended so that it contains the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType> createContainedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			Rectangle<RectangleType> contained;
			contained.left = min(left, p_left);
			contained.top = min(top, p_top);
			contained.right = max(right, p_right);
			contained.bottom = max(bottom, p_bottom);

			return contained;
		}

		/*
			Extends the rectangle so that it contains the parameter rectangle.
		*/
		template<typename ParameterRectangleType>
		Rectangle<RectangleType>& contain(Rectangle<ParameterRectangleType> const& p_rectangle)
		{
			if (p_rectangle.left < left) 
				left = p_rectangle.left;
			if (p_rectangle.top < top) 
				top = p_rectangle.top;
			if (p_rectangle.right > right) 
				right = p_rectangle.right;
			if (p_rectangle.bottom > bottom) 
				bottom = p_rectangle.bottom;

			return *this;
		}

		/*
			Extends the rectangle so that it contains the parameter rectangle edge coordinates.
		*/
		Rectangle<RectangleType>& contain(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			if (p_left < left) 
				left = p_left;
			if (p_top < top) 
				top = p_top;
			if (p_right > right) 
				right = p_right;
			if (p_bottom > bottom) 
				bottom = p_bottom;

			return *this;
		}

		//------------------------------

		/*
			Returns whether a point lies within this rectangle.
		*/
		template<typename PointType>
		bool getIsContaining(Point<PointType> const& p_point) const
		{
			return p_point.x >= left && p_point.x < right
				&& p_point.y >= top && p_point.y < bottom;
		}
		/*
			Returns whether a point lies within this rectangle.
		*/
		bool getIsContaining(RectangleType p_x, RectangleType p_y) const
		{
			return p_x >= left && p_x < right
				&& p_y >= top && p_y < bottom;
		}

		/*
			Returns whether another rectangle is fully inside this rectangle.
		*/
		template<typename ParameterRectangleType>
		bool getIsContaining(ParameterRectangleType p_left, ParameterRectangleType p_top, ParameterRectangleType p_right, ParameterRectangleType p_bottom) const
		{
			return p_left >= left && p_right <= right
				&& p_top >= top && p_bottom <= bottom;
		}
		/*
			Returns whether another rectangle is fully inside this rectangle.
		*/
		template<typename ParameterRectangleType>
		bool getIsContaining(Rectangle<ParameterRectangleType> const& p_rectangle) const
		{
			return p_rectangle.left >= left && p_rectangle.right <= right
				&& p_rectangle.top >= top && p_rectangle.bottom <= bottom;
		}
		/*
			Returns whether a protected rectangle is fully inside this rectangle.
		*/
		bool getIsContaining(ProtectedRectangle* p_protectedRectangle) const;

		/*
			Returns whether this rectangle intersects/overlaps/touches another rectangle.
		*/
		template<typename ParameterRectangleType>
		bool getIsIntersecting(ParameterRectangleType p_left, ParameterRectangleType p_top, ParameterRectangleType p_right, ParameterRectangleType p_bottom) const
		{
			return p_right >= left && p_bottom >= top
				&& p_left <= right && p_top <= bottom;
		}
		/*
			Returns whether this rectangle intersects/overlaps/touches another rectangle.
		*/
		template<typename ParameterRectangleType>
		bool getIsIntersecting(Rectangle<ParameterRectangleType> const& p_rectangle) const
		{
			return p_rectangle.right >= left && p_rectangle.bottom >= top
				&& p_rectangle.left <= right && p_rectangle.top <= bottom;
		}
		/*
			Returns whether this rectangle intersects/overlaps/touches a protected rectangle.
		*/
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const;
	};

	// It would be useless and too much job to make ProtectedRectangle templated... floats are pretty nice anyways

	/*
		An abstract class for objects that have 2d bounds for which changes by the user of the object can be responded to.
	*/
	class ProtectedRectangle
	{
	protected:
		Rectangle<float> m_bounds;

	public:
		ProtectedRectangle() { }
		ProtectedRectangle(Rectangle<float> const& p_bounds) : m_bounds(p_bounds) { }
		ProtectedRectangle(Rectangle<float>&& p_bounds) : m_bounds(p_bounds) { }

		virtual void setBounds(Rectangle<float> const& p_rectangle)
		{
			m_bounds = p_rectangle;
		}
		virtual void setBounds(float p_left, float p_top, float p_right, float p_bottom)
		{
			m_bounds.set(p_left, p_top, p_right, p_bottom);
		}
		virtual void setBounds(Point<float> const& p_position, Point<float> const& p_size)
		{
			m_bounds.set(p_position, p_size);
		}
		virtual Rectangle<float> const& getBounds() const
		{
			return m_bounds;
		}

		//------------------------------

		virtual void move(Point<float> const& p_offset)
		{
			m_bounds.move(p_offset);
		}
		virtual void move(float p_offsetX, float p_offsetY)
		{
			m_bounds.move(p_offsetX, p_offsetY);
		}
		virtual void moveX(float p_offsetX)
		{
			m_bounds.moveX(p_offsetX);
		}
		virtual void moveY(float p_offsetY)
		{
			m_bounds.moveY(p_offsetY);
		}

		//------------------------------

		virtual void setTopLeft(float p_topAndLeft, bool p_willKeepSize = true)
		{
			m_bounds.setTopLeft(p_topAndLeft, p_willKeepSize);
		}
		virtual void setTopLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			m_bounds.setTopLeft(p_position, p_willKeepSize);
		}
		virtual void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true)
		{
			m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
		}
		virtual Point<float> getTopLeft() const
		{
			return m_bounds.getTopLeft();
		}

		virtual void setTopRight(float p_topAndRight, bool p_willKeepSize = true)
		{
			m_bounds.setTopRight(p_topAndRight, p_willKeepSize);
		}
		virtual void setTopRight(Point<float> const& p_topRight, bool p_willKeepSize = true)
		{
			m_bounds.setTopRight(p_topRight, p_willKeepSize);
		}
		virtual void setTopRight(float p_right, float p_top, bool p_willKeepSize = true)
		{
			m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
		}
		virtual Point<float> getTopRight() const
		{
			return m_bounds.getTopRight();
		}

		virtual void setBottomLeft(float p_bottomAndLeft, bool p_willKeepSize = true)
		{
			m_bounds.setBottomLeft(p_bottomAndLeft, p_willKeepSize);
		}
		virtual void setBottomLeft(Point<float> const& p_bottomLeft, bool p_willKeepSize = true)
		{
			m_bounds.setBottomLeft(p_bottomLeft, p_willKeepSize);
		}
		virtual void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true)
		{
			m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
		}
		virtual Point<float> getBottomLeft() const
		{
			return m_bounds.getBottomLeft();
		}

		virtual void setBottomRight(float p_bottomAndRight, bool p_willKeepSize = true)
		{
			m_bounds.setBottomRight(p_bottomAndRight, p_willKeepSize);
		}
		virtual void setBottomRight(Point<float> const& p_bottomRight, bool p_willKeepSize = true)
		{
			m_bounds.setBottomRight(p_bottomRight, p_willKeepSize);
		}
		virtual void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true)
		{
			m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
		}
		virtual Point<float> getBottomRight() const
		{
			return m_bounds.getBottomRight();
		}

		//------------------------------

		virtual void setCenter(float p_centerXY)
		{
			m_bounds.setCenter(p_centerXY);
		}
		virtual void setCenter(Point<float> const& p_position)
		{
			m_bounds.setCenter(p_position);
		}
		virtual void setCenter(float p_x, float p_y)
		{
			m_bounds.setCenter(p_x, p_y);
		}
		virtual void setCenterX(float p_x)
		{
			m_bounds.setCenterX(p_x);
		}
		virtual void setCenterY(float p_y)
		{
			m_bounds.setCenterY(p_y);
		}

		virtual Point<float> getCenter() const
		{
			return m_bounds.getCenter();
		}
		virtual float getCenterX() const
		{
			return m_bounds.getCenterX();
		}
		virtual float getCenterY() const
		{
			return m_bounds.getCenterY();
		}

		//------------------------------

		virtual void setLeft(float p_left, bool p_willKeepWidth = true)
		{
			m_bounds.setLeft(p_left, p_willKeepWidth);
		}
		virtual float getLeft() const
		{
			return m_bounds.left;
		}

		virtual void setTop(float p_top, bool p_willKeepHeight = true)
		{
			m_bounds.setTop(p_top, p_willKeepHeight);
		}
		virtual float getTop() const
		{
			return m_bounds.top;
		}

		virtual void setRight(float p_right, bool p_willKeepWidth = true)
		{
			m_bounds.setRight(p_right, p_willKeepWidth);
		}
		virtual float getRight() const
		{
			return m_bounds.right;
		}

		virtual void setBottom(float p_bottom, bool p_willKeepHeight = true)
		{
			m_bounds.setBottom(p_bottom, p_willKeepHeight);
		}
		virtual float getBottom() const
		{
			return m_bounds.bottom;
		}

		//------------------------------

		virtual void setWidth(float p_width)
		{
			m_bounds.setWidth(p_width);
		}
		virtual float getWidth() const
		{
			return m_bounds.getWidth();
		}

		virtual void setHeight(float p_height)
		{
			m_bounds.setHeight(p_height);
		}
		virtual float getHeight() const
		{
			return m_bounds.getHeight();
		}

		virtual void setSize(Point<float> const& p_size)
		{
			m_bounds.setSize(p_size);
		}
		virtual void setSize(float p_width, float p_height)
		{
			m_bounds.setSize(p_width, p_height);
		}
		virtual Point<float> getSize() const
		{
			return m_bounds.getSize();
		}

		//------------------------------

		virtual bool getIsIntersecting(float p_left, float p_top, float p_right, float p_bottom) const
		{
			return m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom);
		}
		virtual bool getIsIntersecting(Rectangle<float> const& p_rectangle) const
		{
			return m_bounds.getIsIntersecting(p_rectangle);
		}
		virtual bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const
		{
			return m_bounds.getIsIntersecting(p_protectedRectangle->getBounds());
		}

		virtual bool getIsContaining(float p_left, float p_top, float p_right, float p_bottom) const
		{
			return m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom);
		}
		virtual bool getIsContaining(Rectangle<float> const& p_rectangle) const
		{
			return m_bounds.getIsContaining(p_rectangle);
		}
		virtual bool getIsContaining(ProtectedRectangle* p_rectangle) const
		{
			return m_bounds.getIsContaining(p_rectangle);
		}

		virtual bool getIsContaining(float p_x, float p_y) const
		{
			return m_bounds.getIsContaining(p_x, p_y);
		}
		virtual bool getIsContaining(Point<float> const& p_point) const
		{
			return m_bounds.getIsContaining(p_point);
		}
	};

	//------------------------------

	enum class RectangleCornerType
	{
		Round,
		Cut
	};
	class RectangleCorners
	{
	public:
		RectangleCornerType topLeftType;
		RectangleCornerType topRightType;
		RectangleCornerType bottomLeftType;
		RectangleCornerType bottomRightType;

		float topLeftSizeX;
		float topLeftSizeY;

		float topRightSizeX;
		float topRightSizeY;

		float bottomLeftSizeX;
		float bottomLeftSizeY;

		float bottomRightSizeX;
		float bottomRightSizeY;
		
		RectangleCorners() :
			topLeftSizeX(0.f), topLeftSizeY(0.f), topRightSizeX(0.f), topRightSizeY(0.f),
			bottomLeftSizeX(0.f), bottomLeftSizeY(0.f), bottomRightSizeX(0.f), bottomRightSizeY(0.f),
			topLeftType(RectangleCornerType::Round), topRightType(RectangleCornerType::Round),
			bottomLeftType(RectangleCornerType::Round), bottomRightType(RectangleCornerType::Round)
		{
		}
		RectangleCorners(float p_cornerSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX(p_cornerSize), topLeftSizeY(p_cornerSize),
			topRightSizeX(p_cornerSize), topRightSizeY(p_cornerSize),
			bottomLeftSizeX(p_cornerSize), bottomLeftSizeY(p_cornerSize),
			bottomRightSizeX(p_cornerSize), bottomRightSizeY(p_cornerSize),
			topLeftType(p_cornerType), topRightType(p_cornerType),
			bottomLeftType(p_cornerType), bottomRightType(p_cornerType)
		{
		}
		RectangleCorners(float p_cornerSizeX, float p_cornerSizeY, RectangleCornerType p_cornerType = RectangleCornerType::Cut) :
			topLeftSizeX(p_cornerSizeX), topLeftSizeY(p_cornerSizeY),
			topRightSizeX(p_cornerSizeX), topRightSizeY(p_cornerSizeY),
			bottomLeftSizeX(p_cornerSizeX), bottomLeftSizeY(p_cornerSizeY),
			bottomRightSizeX(p_cornerSizeX), bottomRightSizeY(p_cornerSizeY),
			topLeftType(p_cornerType), topRightType(p_cornerType),
			bottomLeftType(p_cornerType), bottomRightType(p_cornerType)
		{
		}

		RectangleCorners(float p_topLeftSize, float p_topRightSize, float p_bottomLeftSize, float p_bottomRightSize, RectangleCornerType p_cornerType = RectangleCornerType::Round) :
			topLeftSizeX(p_topLeftSize), topLeftSizeY(p_topLeftSize),
			topRightSizeX(p_topRightSize), topRightSizeY(p_topRightSize),
			bottomLeftSizeX(p_bottomLeftSize), bottomLeftSizeY(p_bottomLeftSize),
			bottomRightSizeX(p_bottomRightSize), bottomRightSizeY(p_bottomRightSize),
			topLeftType(p_cornerType), topRightType(p_cornerType),
			bottomLeftType(p_cornerType), bottomRightType(p_cornerType)
		{
		}
	};

#pragma endregion

	//------------------------------

	/*
		Cubic bezier animation easing.
		Try out this, can be useful for making your easing curves:
		http://bjornsundin.com/projects/cubic-bezier-easing

		Ease in example:
		Easing(0.7, 0, 1, 1).easeValue(x)
		Ease out example:
		Easing(0, 0, 0.3, 1).easeValue(x)
		See the constructors and easeValue() for more info.

		Storing Easing objects in a Theme can be a good idea because you can use the same easings within your whole application, or different parts of it.
	*/
	struct Easing
	{
		float x0, y0, x1, y1;

		//------------------------------

		Easing() : x0(0.f), y0(0.f), x1(1.f), y1(1.f) { }
		Easing(Easing const& p_easing)
		{
			*this = p_easing;
		}
		Easing(Easing&& p_easing) noexcept
		{
			*this = p_easing;
		}
		/*
			Initializes the control points of the bezier curve easing.
			The parameters are the coordinates of the first and second control points, respectively.
		*/
		Easing(float p_x0, float p_y0, float p_x1, float p_y1) :
			x0(p_x0), y0(p_y0), x1(p_x1), y1(p_y1)
		{ }
		/*
			Initializes the control points of the bezier curve easing.
			The parameters are the coordinates of the first and second control points, respectively.
		*/
		Easing(double p_x0, double p_y0, double p_x1, double p_y1) :
			x0((float)p_x0), y0((float)p_y0), x1((float)p_x1), y1((float)p_y1)
		{ }

		Easing& operator=(Easing const& p_easing)
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}
		Easing& operator=(Easing&& p_easing) noexcept
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}

		//------------------------------

		bool operator==(Easing const& p_easing) const
		{
			return x0 == p_easing.x0 && y0 == p_easing.y0 && x1 == p_easing.x1 && y1 == p_easing.y1;
		}
		bool operator!=(Easing const& p_easing) const
		{
			return x0 != p_easing.x0 || y0 != p_easing.y0 || x1 != p_easing.x1 || y1 != p_easing.y1;
		}

		//------------------------------

		/*
			Transforms a normalized value according to a cubic bezier curve.
			p_precision is the maximum amount of error in the output value.

			It calculates a quick newton's method estimation since the cubic bezier curve is defined as a calculation of points; 
			f(t) = (x, y) where 0 <= t <= 1, and we want to ease over x (p_value is x) and not t. This why we have a precision parameter.
		*/
		float easeValue(float p_value, float p_precision = 0.005f) const;
	};

	//------------------------------

	/*
		This is very useful when storing pointers to dynamically allocated objects in multiple places.
		The object doesn't get deleted until every remember() has a forget().
		The constructor is the first remember(), meaning m_referenceCount is initialized with 1. 
		Don't use the delete operator with objects that are ReferenceCounted, use forget() instead.

		I just like this a lot more than using std::shared_ptr etc. You have more control! And it may be more efficient.
	*/
	class ReferenceCounted
	{
	private:
		uint32 m_referenceCount;

	public:
		ReferenceCounted() : m_referenceCount(1U) { }
		virtual ~ReferenceCounted() { };

		/*
			Increments the reference count and returns the new reference count.
			Remembers a pointer!
		*/
		uint32 remember()
		{
			return ++m_referenceCount;
		}

		/*
			Decrements the reference count, returns the new reference count and 
			deletes the object if the reference count has reached 0.
		*/
		uint32 forget()
		{
			m_referenceCount--;
			if (!m_referenceCount)
			{
				delete this;
				return 0;
			}
			return m_referenceCount;
		}

		/*
			Returns the number of pointers to the dynamic object that have been remembered.
		*/
		uint32 getReferenceCount()
		{
			return m_referenceCount;
		}
	};

	//------------------------------
	// Color stuff

#pragma region Color
	/*
		ARGB formatted 32-bit packed color, where every channel has 8 bits.
	*/
	typedef uint32 colorInt;

	inline uint8 getRedChannel(colorInt p_color)
	{
		return (p_color >> 16) & 0xff;
	}
	inline uint8 getGreenChannel(colorInt p_color)
	{
		return (p_color >> 8) & 0xff;
	}
	inline uint8 getBlueChannel(colorInt p_color)
	{
		return p_color & 0xff;
	}
	inline uint8 getAlphaChannel(colorInt p_color)
	{
		return (p_color >> 24) & 0xff;
	}

	/*
		Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
		This means that a Color object is 4 times as big as a packed 32-bit color.
	*/
	class Color
	{
	public:
		float red;
		float green;
		float blue;
		float alpha;

		//------------------------------

		Color() :
			red(0.f), green(0.f), blue(0.f), alpha(1.f)
		{ }
		/*
			The channels are floats in the range [0, 1].
		*/
		Color(float p_red, float p_green, float p_blue, float p_alpha = 1.f) :
			red(p_red), green(p_green), blue(p_blue), alpha(p_alpha)
		{ }
		/*
			The channels are doubles in the range [0, 1].
		*/
		Color(double p_red, double p_green, double p_blue, double p_alpha = 1.f) :
			red(p_red), green(p_green), blue(p_blue), alpha(p_alpha)
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(uint8 p_red, uint8 p_green, uint8 p_blue, uint8 p_alpha = (uint8)255) :
			red(float(p_red) / 255.f), green(float(p_green) / 255.f), blue(float(p_blue) / 255.f), alpha(float(p_alpha) / 255.f)
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(uint32 p_red, uint32 p_green, uint32 p_blue, uint32 p_alpha = (uint32)255) :
			red(float(p_red) / 255.f), green(float(p_green) / 255.f), blue(float(p_blue) / 255.f), alpha(float(p_alpha) / 255.f)
		{ }
		/*
			The channels are in the range [0, 255]
		*/
		Color(int32 p_red, int32 p_green, int32 p_blue, int32 p_alpha = (int32)255) :
			red(float(p_red) / 255.f), green(float(p_green) / 255.f), blue(float(p_blue) / 255.f), alpha(float(p_alpha) / 255.f)
		{ }
		/*
			Initializes the color with a grayscale value. The values are floats in the range [0, 1].
		*/
		Color(float p_lightness, float p_alpha = 1.f)
		{
			red = constrain(p_lightness);
			green = red;
			blue = red;
			alpha = constrain(p_alpha);
		}
		/*
			Initializes the color with a grayscale value. The values are doubles in the range [0, 1].
		*/
		Color(double p_lightness, double p_alpha = 1.)
		{
			red = constrain(p_lightness);
			green = red;
			blue = red;
			alpha = constrain(p_alpha);
		}
		/*
			Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
		*/
		Color(uint8 p_lightness, uint8 p_alpha = (uint8)255)
		{
			red = float(p_lightness) / 255.f;
			green = red;
			blue = red;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Creates a copy of another color but with a new alpha.
		*/
		Color(Color const& p_color, float p_alpha) :
			red(p_color.red), green(p_color.green), blue(p_color.blue), alpha(p_alpha)
		{ }
		/*
			Creates a copy of another color but with a new alpha.
		*/
		Color(Color const& p_color, double p_alpha) :
			red(p_color.red), green(p_color.green), blue(p_color.blue), alpha(p_alpha)
		{ }
		/*
			Initializes with a 4-byte packed RGBA color.
		*/
		Color(colorInt const& p_color)
		{
			operator=(p_color);
		}
		Color(Color const& p_color)
		{
			operator=(p_color);
		}
		Color(Color&& p_color) noexcept
		{
			operator=(p_color);
		}

		Color& operator=(colorInt const& p_color)
		{
			alpha = float(p_color >> 24) / 255.f;
			red = float(p_color >> 16 & 0xff) / 255.f;
			green = float((p_color >> 8 & 0xff)) / 255.f;
			blue = float(p_color & 0xff) / 255.f;
			return *this;
		}
		Color& operator=(Color const& p_color)
		{
			red = p_color.red;
			green = p_color.green;
			blue = p_color.blue;
			alpha = p_color.alpha;
			return *this;
		}
		Color& operator=(Color&& p_color) noexcept
		{
			red = p_color.red;
			green = p_color.green;
			blue = p_color.blue;
			alpha = p_color.alpha;
			return *this;
		}

		//------------------------------

		bool operator==(Color const& p_color) const
		{
			return red == p_color.red && green == p_color.green && blue == p_color.blue && alpha == p_color.alpha;
		}
		bool operator!=(Color const& p_color) const
		{
			return red != p_color.red || green != p_color.green || blue != p_color.blue || alpha != p_color.alpha;
		}

		//------------------------------

		/*
			Sets the values for the red, green, blue and alpha channels. They are all floats in the range of [0, 1].
		*/
		void setRGBA(float p_red, float p_green, float p_blue, float p_alpha = 1.f)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. They are floats in the range of [0, 1].
		*/
		void setRGBA(float p_grayscale, float p_alpha = 1.f)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the values for the red, green and blue channels. They are all floats in the range of [0, 1].
		*/
		void setRGB(float p_red, float p_green, float p_blue)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
		}
		/*
			Sets the same values for the red, green and blue channels. They are floats in the range of [0, 1].
		*/
		void setRGB(float p_grayscale)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
		}
		/*
			Sets the values for the red, green, blue and alpha channels. They are all doubles in the range of [0, 1].
		*/
		void setRGBA(double p_red, double p_green, double p_blue, double p_alpha = 1.f)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. They are doubles in the range of [0, 1].
		*/
		void setRGBA(double p_grayscale, double p_alpha = 1.f)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
			alpha = constrain(p_alpha);
		}
		/*
			Sets the values for the red, green and blue channels. They are all doubles in the range of [0, 1].
		*/
		void setRGB(double p_red, double p_green, double p_blue)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
		}
		/*
			Sets the same values for the red, green and blue channels. They are doubles in the range of [0, 1].
		*/
		void setRGB(double p_grayscale)
		{
			red = constrain(p_grayscale);
			green = constrain(p_grayscale);
			blue = constrain(p_grayscale);
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGBA(uint8 p_red, uint8 p_green, uint8 p_blue, uint8 p_alpha = (uint8)255)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGBA(uint8 p_grayscale, uint8 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGB(uint8 p_red, uint8 p_green, uint8 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are bytes, in the range of [0, 255].
		*/
		void setRGB(uint8 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(uint32 p_red, uint32 p_green, uint32 p_blue, uint32 p_alpha = 255U)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(uint32 p_grayscale, uint32 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(uint32 p_red, uint32 p_green, uint32 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(uint32 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}
		/*
			Sets the values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(int32 p_red, int32 p_green, int32 p_blue, int32 p_alpha = 255U)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the same values for the red, green, blue and alpha channels. The parameters are in the range [0, 255].
		*/
		void setRGBA(int32 p_grayscale, int32 p_alpha = 255)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}
		/*
			Sets the values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(int32 p_red, int32 p_green, int32 p_blue)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
		}
		/*
			Sets the same values for the red, green and blue channels. The parameters are in the range [0, 255].
		*/
		void setRGB(int32 p_grayscale)
		{
			red = float(p_grayscale) / 255.f;
			green = float(p_grayscale) / 255.f;
			blue = float(p_grayscale) / 255.f;
		}

		//------------------------------

		/*
			Sets the hue, saturation and brightness values of the color. They are all floats in the range [0, 1].
		*/
		Color& setHSB(float p_hue, float p_saturation, float p_brightness)
		{
			p_hue -= floor(p_hue);
			p_brightness = constrain(p_brightness);
			float factor = p_brightness * constrain(p_saturation);
			red = p_brightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 1.f);
			green = p_brightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 1.f);
			blue = p_brightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 1.f);
			return *this;
		}
		/*
			Sets the hue, saturation, brightness and alpha values of the color. They are all floats in the range [0, 1].
		*/
		Color& setHSBA(float p_hue, float p_saturation, float p_brightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSB(p_hue, p_saturation, p_brightness);
		}
		/*
			Sets the hue, saturation and lightness values of the color. They are all floats in the range [0, 1]
		*/
		Color& setHSL(float p_hue, float p_saturation, float p_lightness)
		{
			p_hue -= floor(p_hue);
			p_lightness = constrain(p_lightness);
			float factor = 2.f*constrain(p_saturation)*(p_lightness < 0.5f ? p_lightness : (1.f - p_lightness));
			red = p_lightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 0.5f);
			green = p_lightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 0.5f);
			blue = p_lightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 0.5f);
			return *this;
		}
		/*
			Sets the hue, saturation, lightness and alpha values of the color. They are all floats in the range [0, 1]
		*/
		Color& setHSLA(float p_hue, float p_saturation, float p_lightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSL(p_hue, p_saturation, p_lightness);
		}

		/*
			Changes the hue of the color. The hue is a float in the range [0, 1].
		*/
		Color& setHue(float p_hue)
		{
			p_hue -= floor(p_hue);
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			red = minColor + (maxColor - minColor)*(constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f));
			green = minColor + (maxColor - minColor)*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f));
			blue = minColor + (maxColor - minColor)*(constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f));
			return *this;
		}
		/*
			Returns the hue of the color. The hue is a float in the range [0, 1].
		*/
		float getHue() const
		{
			if (red + green + blue == 0.f)
			{
				return 0.f;
			}

			if (red > green)
			{
				if (red > blue)
				{
					if (green > blue)
					{
						// (1, 0, 0) -> (1, 1, 0) : 0/6 < hue < 1/6, max = red, min = blue
						return (green - blue) / (red - blue) / 6.f;
					}
					else
					{
						// (1, 0, 1) -> (1, 0, 0) : 5/6 < hue < 6/6, max = red, min = green
						return 1.f - (blue - green) / (red - green) / 6.f;
					}
				}
				else
				{
					// (0, 0, 1) -> (1, 0, 1) : 4/6 < hue < 5/6, max = blue, min = green
					return (4.f + (red - green) / (blue - green)) / 6.f;
				}
			}
			else
			{
				if (green > blue)
				{
					if (red > blue)
					{
						// (1, 1, 0) -> (0, 1, 0) : 1/6 < hue < 2/6, max = green, min = blue
						return (2.f - (red - blue) / (green - blue)) / 6.f;
					}
					else
					{
						// (0, 1, 0) -> (0, 1, 1) : 2/6 < hue < 3/6, max = green, min = red
						return (2.f + (blue - red) / (green - red)) / 6.f;
					}
				}
				else
				{
					// (0, 1, 1) -> (0, 0, 1) : 3/6 < hue < 4/6, max = blue, min = red
					return (4.f - (green - red) / (blue - red)) / 6.f;
				}
			}
		}

		/*
			Sets the HSB saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is grayscale, because only RGBA values are stored.
		*/
		Color& setSaturationHSB(float p_saturation)
		{
			if (red == green && red == blue)
			{
				return *this;
			}

			p_saturation = constrain(p_saturation);
			float factor = p_saturation / getSaturationHSB();

			float brightness = max(red, green, blue);
			red = brightness + factor * (red - brightness);
			green = brightness + factor * (green - brightness);
			blue = brightness + factor * (blue - brightness);

			return *this;
		}
		/*
			Returns the HSB saturation of the color. The saturation is a float in the range [0, 1].
		*/
		float getSaturationHSB() const
		{
			float brightness = getBrightness();
			if (brightness)
			{
				return 1.f - min(red, green, blue) / brightness;
			}
			return 0.f;
		}

		/*
			Sets the HSL saturation of the color. The saturation is a float in the range [0, 1].
			HSB saturation can change lightness, and HSL saturation can change brightness.
			Keep in mind that you can't change the saturation if the color is gray, since only RGBA values are stored.
		*/
		Color& setSaturationHSL(float p_saturation)
		{
			p_saturation = constrain(p_saturation);

			float lightness = getLightness();
			float factor = p_saturation / getSaturationHSL();
			if (factor == p_saturation / 0.f)
			{
				return *this;
			}
			red = lightness + factor * (red - lightness);
			green = lightness + factor * (green - lightness);
			blue = lightness + factor * (blue - lightness);

			return *this;
		}
		/*
			Returns the HSL saturation of the color. The saturation is a float in the range [0, 1].
		*/
		float getSaturationHSL() const
		{
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			if (minColor == maxColor)
			{
				return 0.f;
			}
			return max((maxColor - minColor) / (maxColor + minColor), (maxColor - minColor) / (2.f - maxColor - minColor));
		}

		/*
			Sets the brightness of the color. The brightness is a float in the range [0, 1]. A brightness of 0 makes the
			color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
		*/
		Color& setBrightness(float p_brightness)
		{
			p_brightness = constrain(p_brightness);

			if (red == green && red == blue)
			{
				red = p_brightness;
				green = p_brightness;
				blue = p_brightness;
				return *this;
			}

			float brightness = max(red, green, blue);
			red *= p_brightness / brightness;
			green *= p_brightness / brightness;
			blue *= p_brightness / brightness;

			return *this;
		}
		/*
			Returns the brightness of the color. The brightness is a float in the range [0, 1].
		*/
		float getBrightness() const
		{
			return max(red, green, blue);
		}

		/*
			Changes the lightness of the color. The lightness a float in the range [0, 1]. A lightness of 0 makes the
			color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
		*/
		Color& setLightness(float p_lightness)
		{
			p_lightness = constrain(p_lightness);

			if (red == green && red == blue)
			{
				red = p_lightness;
				green = p_lightness;
				blue = p_lightness;
				return *this;
			}

			float lightness = getLightness();
			if (lightness <= 0.5f)
			{
				if (p_lightness <= 0.5f)
				{
					red = red * p_lightness / lightness;
					green = green * p_lightness / lightness;
					blue = blue * p_lightness / lightness;
				}
				else
				{
					red = (red - lightness) * (1.f - p_lightness) / lightness + p_lightness;
					green = (green - lightness) * (1.f - p_lightness) / lightness + p_lightness;
					blue = (blue - lightness) * (1.f - p_lightness) / lightness + p_lightness;
				}
			}
			else
			{
				if (p_lightness <= 0.5)
				{
					red = (red - lightness)*p_lightness / (1.f - lightness) + p_lightness;
					green = (green - lightness)*p_lightness / (1.f - lightness) + p_lightness;
					blue = (blue - lightness)*p_lightness / (1.f - lightness) + p_lightness;
				}
				else
				{
					red = (red - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
					green = (green - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
					blue = (blue - lightness)*(1.f - p_lightness) / (1.f - lightness) + p_lightness;
				}
			}
			return *this;
		}
		/*
			Returns the lightness of the color. The lightness is a float in the range [0, 1].
		*/
		float getLightness() const
		{
			return 0.5f*(min(red, green, blue) + max(red, green, blue));
		}

		//------------------------------

		/*
			A contrast of 0 makes the color gray, 0.5 leaves it unchanged and 1 is maximum contrast.
		*/
		void setContrast(float p_contrast)
		{
			if (p_contrast == 0.5)
			{
				return;
			}
			if (p_contrast < 0.5f)
			{
				red = (red - 0.5f) * p_contrast*2.f + 0.5f;
				green = (green - 0.5f) * p_contrast*2.f + 0.5f;
				blue = (blue - 0.5f) * p_contrast*2.f + 0.5f;
			}
			else
			{
				red = (float(red >= 0.5f) - red)*(p_contrast * 2.f - 1.f) + red;
				green = (float(green >= 0.5f) - green) * (p_contrast * 2.f - 1.f) + green;
				blue = (float(blue >= 0.5f) - blue) * (p_contrast * 2.f - 1.f) + blue;
			}
		}

		//------------------------------

		/*
			Packs the color into a 32-bit integer in ARGB format.
		*/
		colorInt getPacked() const
		{
			return (colorInt(alpha * 0xff) << 24) | (colorInt(red * 0xff) << 16) | (colorInt(green * 0xff) << 8) | (colorInt(blue * 0xff));
		}
	};

	/*
		Linearly interpolates a color between p_start and p_end. Each channel is faded individually.
	*/
	inline Color interpolate(Color const& p_start, Color const& p_end, float p_progress)
	{
		return Color(
			p_start.red * (1.f - p_progress) + p_end.red*p_progress, 
			p_start.green * (1.f - p_progress) + p_end.green*p_progress, 
			p_start.blue * (1.f - p_progress) + p_end.blue*p_progress, 
			p_start.alpha * (1.f - p_progress) + p_end.alpha*p_progress
		);
	}

	//
	// Material design 2014 colors
	//

	colorInt const
		COLOR_RED_50 = 0xFFFFEBEE,
		COLOR_RED_100 = 0xFFFFCDD2,
		COLOR_RED_200 = 0xFFEF9A9A,
		COLOR_RED_300 = 0xFFE57373,
		COLOR_RED_400 = 0xFFEF5350,
		COLOR_RED_500 = 0xFFF44336,
		COLOR_RED_600 = 0xFFE53935,
		COLOR_RED_700 = 0xFFD32F2F,
		COLOR_RED_800 = 0xFFC62828,
		COLOR_RED_900 = 0xFFB71C1C,
		COLOR_RED_A100 = 0xFFFF8A80,
		COLOR_RED_A200 = 0xFFFF5252,
		COLOR_RED_A400 = 0xFFFF1744,
		COLOR_RED_A700 = 0xFFD50000;

	colorInt const
		COLOR_PINK_50 = 0xFFFCE4EC,
		COLOR_PINK_100 = 0xFFF8BBD0,
		COLOR_PINK_200 = 0xFFF48FB1,
		COLOR_PINK_300 = 0xFFF06292,
		COLOR_PINK_400 = 0xFFEC407A,
		COLOR_PINK_500 = 0xFFE91E63,
		COLOR_PINK_600 = 0xFFD81B60,
		COLOR_PINK_700 = 0xFFC2185B,
		COLOR_PINK_800 = 0xFFAD1457,
		COLOR_PINK_900 = 0xFF880E4F,
		COLOR_PINK_A100 = 0xFFFF80AB,
		COLOR_PINK_A200 = 0xFFFF4081,
		COLOR_PINK_A400 = 0xFFF50057,
		COLOR_PINK_A700 = 0xFFC51162;

	colorInt const
		COLOR_PURPLE_50 = 0xFFF3E5F5,
		COLOR_PURPLE_100 = 0xFFE1BEE7,
		COLOR_PURPLE_200 = 0xFFCE93D8,
		COLOR_PURPLE_300 = 0xFFBA68C8,
		COLOR_PURPLE_400 = 0xFFAB47BC,
		COLOR_PURPLE_500 = 0xFF9C27B0,
		COLOR_PURPLE_600 = 0xFF8E24AA,
		COLOR_PURPLE_700 = 0xFF7B1FA2,
		COLOR_PURPLE_800 = 0xFF6A1B9A,
		COLOR_PURPLE_900 = 0xFF4A148C,
		COLOR_PURPLE_A100 = 0xFFEA80FC,
		COLOR_PURPLE_A200 = 0xFFE040FB,
		COLOR_PURPLE_A400 = 0xFFD500F9,
		COLOR_PURPLE_A700 = 0xFFAA00FF;

	colorInt const
		COLOR_DEEP_PURPLE_50 = 0xFFEDE7F6,
		COLOR_DEEP_PURPLE_100 = 0xFFD1C4E9,
		COLOR_DEEP_PURPLE_200 = 0xFFB39DDB,
		COLOR_DEEP_PURPLE_300 = 0xFF9579CD,
		COLOR_DEEP_PURPLE_400 = 0xFF7E57C2,
		COLOR_DEEP_PURPLE_500 = 0xFF673AB7,
		COLOR_DEEP_PURPLE_600 = 0xFF5E35B1,
		COLOR_DEEP_PURPLE_700 = 0xFF512DA8,
		COLOR_DEEP_PURPLE_800 = 0xFF4527A0,
		COLOR_DEEP_PURPLE_900 = 0xFF311B92,
		COLOR_DEEP_PURPLE_A100 = 0xFFB388FF,
		COLOR_DEEP_PURPLE_A200 = 0xFF7C4DFF,
		COLOR_DEEP_PURPLE_A400 = 0xFF651FFF,
		COLOR_DEEP_PURPLE_A700 = 0xFF6200EA;

	colorInt const
		COLOR_INDIGO_50 = 0xFFE8EAF6,
		COLOR_INDIGO_100 = 0xFFC5CAE9,
		COLOR_INDIGO_200 = 0xFF9FA8DA,
		COLOR_INDIGO_300 = 0xFF7986CB,
		COLOR_INDIGO_400 = 0xFF5C6BC0,
		COLOR_INDIGO_500 = 0xFF3F51B5,
		COLOR_INDIGO_600 = 0xFF3949AB,
		COLOR_INDIGO_700 = 0xFF303F9F,
		COLOR_INDIGO_800 = 0xFF283593,
		COLOR_INDIGO_900 = 0xFF1A237E,
		COLOR_INDIGO_A100 = 0xFF8C9EFF,
		COLOR_INDIGO_A200 = 0xFF536DFE,
		COLOR_INDIGO_A400 = 0xFF3D5AFE,
		COLOR_INDIGO_A700 = 0xFF304FFE;

	colorInt const
		COLOR_BLUE_50 = 0xFFE3F2FD,
		COLOR_BLUE_100 = 0xFFBBDEFB,
		COLOR_BLUE_200 = 0xFF90CAF9,
		COLOR_BLUE_300 = 0xFF64B5F6,
		COLOR_BLUE_400 = 0xFF42A5F5,
		COLOR_BLUE_500 = 0xFF2196F3,
		COLOR_BLUE_600 = 0xFF1E88E5,
		COLOR_BLUE_700 = 0xFF1976D2,
		COLOR_BLUE_800 = 0xFF1565C0,
		COLOR_BLUE_900 = 0xFF0D47A1,
		COLOR_BLUE_A100 = 0xFF82B1FF,
		COLOR_BLUE_A200 = 0xFF448AFF,
		COLOR_BLUE_A400 = 0xFF2979FF,
		COLOR_BLUE_A700 = 0xFF2962FF;

	colorInt const
		COLOR_LIGHT_BLUE_50 = 0xFFE1F5FE,
		COLOR_LIGHT_BLUE_100 = 0xFFB3E5FC,
		COLOR_LIGHT_BLUE_200 = 0xFF81D4FA,
		COLOR_LIGHT_BLUE_300 = 0xFF4FC3F7,
		COLOR_LIGHT_BLUE_400 = 0xFF29B6F6,
		COLOR_LIGHT_BLUE_500 = 0xFF03A9F4,
		COLOR_LIGHT_BLUE_600 = 0xFF039BE5,
		COLOR_LIGHT_BLUE_700 = 0xFF0288D1,
		COLOR_LIGHT_BLUE_800 = 0xFF0277BD,
		COLOR_LIGHT_BLUE_900 = 0xFF01579B,
		COLOR_LIGHT_BLUE_A100 = 0xFF80D8FF,
		COLOR_LIGHT_BLUE_A200 = 0xFF40C4FF,
		COLOR_LIGHT_BLUE_A400 = 0xFF00B0FF,
		COLOR_LIGHT_BLUE_A700 = 0xFF0091EA;

	colorInt const
		COLOR_CYAN_50 = 0xFFE0F7FA,
		COLOR_CYAN_100 = 0xFFB2EBF2,
		COLOR_CYAN_200 = 0xFF80DEEA,
		COLOR_CYAN_300 = 0xFF4DD0E1,
		COLOR_CYAN_400 = 0xFF26C6DA,
		COLOR_CYAN_500 = 0xFF00BCD4,
		COLOR_CYAN_600 = 0xFF00ACC1,
		COLOR_CYAN_700 = 0xFF0097A7,
		COLOR_CYAN_800 = 0xFF00838F,
		COLOR_CYAN_900 = 0xFF006064,
		COLOR_CYAN_A100 = 0xFF84FFFF,
		COLOR_CYAN_A200 = 0xFF18FFFF,
		COLOR_CYAN_A400 = 0xFF00E5FF,
		COLOR_CYAN_A700 = 0xFF00B8D4;

	colorInt const
		COLOR_TEAL_50 = 0xFFE0F2F1,
		COLOR_TEAL_100 = 0xFFB2DFDB,
		COLOR_TEAL_200 = 0xFF80CBC4,
		COLOR_TEAL_300 = 0xFF4DB6AC,
		COLOR_TEAL_400 = 0xFF26A69A,
		COLOR_TEAL_500 = 0xFF009688,
		COLOR_TEAL_600 = 0xFF00897B,
		COLOR_TEAL_700 = 0xFF00796B,
		COLOR_TEAL_800 = 0xFF00695C,
		COLOR_TEAL_900 = 0xFF004D40,
		COLOR_TEAL_A100 = 0xFFA7FFEB,
		COLOR_TEAL_A200 = 0xFF64FFDA,
		COLOR_TEAL_A400 = 0xFF1DE9B6,
		COLOR_TEAL_A700 = 0xFF00BFA5;

	colorInt const
		COLOR_GREEN_50 = 0xFFE8F5E9,
		COLOR_GREEN_100 = 0xFFC8E6C9,
		COLOR_GREEN_200 = 0xFFA5D6A7,
		COLOR_GREEN_300 = 0xFF81C784,
		COLOR_GREEN_400 = 0xFF66BB6A,
		COLOR_GREEN_500 = 0xFF4CAF50,
		COLOR_GREEN_600 = 0xFF43A047,
		COLOR_GREEN_700 = 0xFFE88E3C,
		COLOR_GREEN_800 = 0xFF2E7D32,
		COLOR_GREEN_900 = 0xFF1B5E20,
		COLOR_GREEN_A100 = 0xFFB9F6CA,
		COLOR_GREEN_A200 = 0xFF69F0AE,
		COLOR_GREEN_A400 = 0xFF00E676,
		COLOR_GREEN_A700 = 0xFF00C853;

	colorInt const
		COLOR_LIGHT_GREEN_50 = 0xFFF1F8E9,
		COLOR_LIGHT_GREEN_100 = 0xFFDCEDC8,
		COLOR_LIGHT_GREEN_200 = 0xFFC5E1A5,
		COLOR_LIGHT_GREEN_300 = 0xFFAED581,
		COLOR_LIGHT_GREEN_400 = 0xFF9CCC65,
		COLOR_LIGHT_GREEN_500 = 0xFF8BC34A,
		COLOR_LIGHT_GREEN_600 = 0xFF7CB342,
		COLOR_LIGHT_GREEN_700 = 0xFF689F38,
		COLOR_LIGHT_GREEN_800 = 0xFF558B2F,
		COLOR_LIGHT_GREEN_900 = 0xFF33691E,
		COLOR_LIGHT_GREEN_A100 = 0xFFCCFF90,
		COLOR_LIGHT_GREEN_A200 = 0xFFB2FF59,
		COLOR_LIGHT_GREEN_A400 = 0xFF76FF03,
		COLOR_LIGHT_GREEN_A700 = 0xFF64DD17;

	colorInt const
		COLOR_LIME_50 = 0xFFF9FBE7,
		COLOR_LIME_100 = 0xFFF0F4C3,
		COLOR_LIME_200 = 0xFFE6EE9C,
		COLOR_LIME_300 = 0xFFDCE775,
		COLOR_LIME_400 = 0xFFD4E157,
		COLOR_LIME_500 = 0xFFCDDC39,
		COLOR_LIME_600 = 0xFFC0CA33,
		COLOR_LIME_700 = 0xFFAFB42B,
		COLOR_LIME_800 = 0xFF9E9D24,
		COLOR_LIME_900 = 0xFF827717,
		COLOR_LIME_A100 = 0xFFF4FF81,
		COLOR_LIME_A200 = 0xFFEEFF41,
		COLOR_LIME_A400 = 0xFFC6FF00,
		COLOR_LIME_A700 = 0xFFAEEA00;

	colorInt const
		COLOR_YELLOW_50 = 0xFFFFFDE7,
		COLOR_YELLOW_100 = 0xFFFFF9C4,
		COLOR_YELLOW_200 = 0xFFFFF59D,
		COLOR_YELLOW_300 = 0xFFFFF176,
		COLOR_YELLOW_400 = 0xFFFFEE58,
		COLOR_YELLOW_500 = 0xFFFFEB3B,
		COLOR_YELLOW_600 = 0xFFFDD835,
		COLOR_YELLOW_700 = 0xFFFBC02D,
		COLOR_YELLOW_800 = 0xFFF9A825,
		COLOR_YELLOW_900 = 0xFFF57F17,
		COLOR_YELLOW_A100 = 0xFFFFFF8D,
		COLOR_YELLOW_A200 = 0xFFFFFF00,
		COLOR_YELLOW_A400 = 0xFFFFEA00,
		COLOR_YELLOW_A700 = 0xFFFFD600;

	colorInt const
		COLOR_AMBER_50 = 0xFFFFF8E1,
		COLOR_AMBER_100 = 0xFFFFECB3,
		COLOR_AMBER_200 = 0xFFFFE082,
		COLOR_AMBER_300 = 0xFFFFD54F,
		COLOR_AMBER_400 = 0xFFFFCA28,
		COLOR_AMBER_500 = 0xFFFFC107,
		COLOR_AMBER_600 = 0xFFFFB300,
		COLOR_AMBER_700 = 0xFFFFA000,
		COLOR_AMBER_800 = 0xFFFF8F00,
		COLOR_AMBER_900 = 0xFFFF7F00,
		COLOR_AMBER_A100 = 0xFFFFE57F,
		COLOR_AMBER_A200 = 0xFFFFD740,
		COLOR_AMBER_A400 = 0xFFFFC400,
		COLOR_AMBER_A700 = 0xFFFFAB00;

	colorInt const
		COLOR_ORANGE_50 = 0xFFFFF3E0,
		COLOR_ORANGE_100 = 0xFFFFE0B2,
		COLOR_ORANGE_200 = 0xFFFFCC80,
		COLOR_ORANGE_300 = 0xFFFFB74D,
		COLOR_ORANGE_400 = 0xFFFFA726,
		COLOR_ORANGE_500 = 0xFFFF9800,
		COLOR_ORANGE_600 = 0xFFFB8C00,
		COLOR_ORANGE_700 = 0xFFF57C00,
		COLOR_ORANGE_800 = 0xFFEF6C00,
		COLOR_ORANGE_900 = 0xFFE65100,
		COLOR_ORANGE_A100 = 0xFFFFD180,
		COLOR_ORANGE_A200 = 0xFFFFAB40,
		COLOR_ORANGE_A400 = 0xFFFF9100,
		COLOR_ORANGE_A700 = 0xFFFF6D00;

	colorInt const
		COLOR_DEEP_ORANGE_50 = 0xFFFBE9E7,
		COLOR_DEEP_ORANGE_100 = 0xFFFFCCBC,
		COLOR_DEEP_ORANGE_200 = 0xFFFFAB91,
		COLOR_DEEP_ORANGE_300 = 0xFFFF8A65,
		COLOR_DEEP_ORANGE_400 = 0xFFFF7043,
		COLOR_DEEP_ORANGE_500 = 0xFFFF5722,
		COLOR_DEEP_ORANGE_600 = 0xFFF4511E,
		COLOR_DEEP_ORANGE_700 = 0xFFE64A19,
		COLOR_DEEP_ORANGE_800 = 0xFFD84315,
		COLOR_DEEP_ORANGE_900 = 0xFFBF360C,
		COLOR_DEEP_ORANGE_A100 = 0xFFFF9E80,
		COLOR_DEEP_ORANGE_A200 = 0xFFFF6E40,
		COLOR_DEEP_ORANGE_A400 = 0xFFFF3D00,
		COLOR_DEEP_ORANGE_A700 = 0xFFDD2C00;

	colorInt const
		COLOR_BROWN_50 = 0xFFEFEBE9,
		COLOR_BROWN_100 = 0xFFD7CCC8,
		COLOR_BROWN_200 = 0xFFBCAAA4,
		COLOR_BROWN_300 = 0xFFA1887F,
		COLOR_BROWN_400 = 0xFF8D6E63,
		COLOR_BROWN_500 = 0xFF795548,
		COLOR_BROWN_600 = 0xFF6D4C41,
		COLOR_BROWN_700 = 0xFF5D4037,
		COLOR_BROWN_800 = 0xFF4E342E,
		COLOR_BROWN_900 = 0xFF3E2723;

	colorInt const
		COLOR_GRAY_50 = 0xFFFAFAFA,
		COLOR_GRAY_100 = 0xFFF5F5F5,
		COLOR_GRAY_200 = 0xFFEEEEEE,
		COLOR_GRAY_300 = 0xFFE0E0E0,
		COLOR_GRAY_400 = 0xFFBDBDBD,
		COLOR_GRAY_500 = 0xFF9E9E9E,
		COLOR_GRAY_600 = 0xFF757575,
		COLOR_GRAY_700 = 0xFF616161,
		COLOR_GRAY_800 = 0xFF424242,
		COLOR_GRAY_900 = 0xFF212121;

	colorInt const
		COLOR_BLUE_GRAY_50 = 0xFFECEFF1,
		COLOR_BLUE_GRAY_100 = 0xFFCFD8DC,
		COLOR_BLUE_GRAY_200 = 0xFFB0BEC5,
		COLOR_BLUE_GRAY_300 = 0xFF90A4AE,
		COLOR_BLUE_GRAY_400 = 0xFF78909C,
		COLOR_BLUE_GRAY_500 = 0xFF607D8B,
		COLOR_BLUE_GRAY_600 = 0xFF546E7A,
		COLOR_BLUE_GRAY_700 = 0xFF455A64,
		COLOR_BLUE_GRAY_800 = 0xFF37474F,
		COLOR_BLUE_GRAY_900 = 0xFF263238;

	/*
		A theme consists of different variables that change the look and feel of the parts of the GUI that are using the theme.
		Can be used for changing and accessing any values, colors, easings and font families that you want child views to inherit.
	*/
	class Theme : public ReferenceCounted
	{
	public:
		std::unordered_map<char const*, Color> colors;
		std::unordered_map<char const*, Easing> easings;
		std::unordered_map<char const*, char const*> fontFamilies;
		std::unordered_map<char const*, float> values;

		/*
			This initializes the default theme. 
			If you want to know the default values you can look at the definition in AvoGUI.hpp.
			In Visual Studio, you can go to the definition of Theme (ctrl + T, "Theme") to find it quickly.
		*/
		Theme()
		{
			// Colors

			colors["background"] = 0xfffefefe;
			colors["on background"] = 0xff070707;

			colors["primary"] = COLOR_DEEP_PURPLE_A700;
			colors["primary on background"] = COLOR_DEEP_PURPLE_700;
			colors["on primary"] = ~0U;

			colors["secondary"] = COLOR_TEAL_A400;
			colors["secondary on background"] = COLOR_TEAL_A700;
			colors["on secondary"] = 0xff070707;

			colors["tooltip background"] = Color(0.2f, 0.8f);
			colors["tooltip on background"] = Color(1.f, 0.95f);

			colors["selection"] = 0x90488db5;

			colors["shadow"] = 0x68000000;

			//------------------------------
			// Easings

			easings["in"] = Easing(0.6, 0.0, 0.8, 0.2);
			easings["out"] = Easing(0.1, 0.9, 0.2, 1.0);
			easings["in out"] = Easing(0.4, 0.0, 0.0, 1.0);
			easings["symmetrical in out"] = Easing(0.6, 0.0, 0.4, 1.0);

			easings["ripple"] = Easing(0.1, 0.8, 0.2, 0.95);

			//------------------------------
			// Font families

			fontFamilies["main"] = "Roboto";

			//------------------------------
			// Values

			// Global values
			values["hover animation speed"] = 1.f/6.f; // 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.

			// Tooltip styles
			values["tooltip font size"] = 12.f;

			// Button styles
			values["button font size"] = 14.f;
			values["button character spacing"] = 1.f;

			// Editable text styles
			values["editable text caret blink rate"] = 20; // This is in frames

			// Text field styles
			values["text field font size"] = 15.f;
			values["text field height"] = 3.f; // This is a factor of the font size
			values["text field padding left"] = 14.f;
			values["text field padding right"] = 14.f;
			values["text field filled padding bottom"] = 9.f;
		}
		virtual ~Theme() { }
	};

#pragma endregion

	//------------------------------

	class View;

	class ViewListener
	{
	public:
		/*
			USER IMPLEMENTED
			This gets called when a view that has registered this listener has changed its size.
		*/
		virtual void handleViewSizeChange(View* p_view, float p_previousWidth, float p_previousHeight) { }
		/*
			USER IMPLEMENTED
			This gets called when a view that has registered this listener has changed any of its bounding rectangle coordinates.
			The size and/or the position of the view may have changed.
		*/
		virtual void handleViewBoundsChange(View* p_view, Rectangle<float> const& p_previousBounds) { }
		/*
			USER IMPLEMENTED
			This gets called when a view that has registered this listener has gotten a child attached to it.
		*/
		virtual void handleViewChildAttachment(View* p_parent, View* p_attachedChild) { }
		/*
			USER IMPLEMENTED
			This gets called when a view that has registered this listener has gotten a child detached from it.
		*/
		virtual void handleViewChildDetachment(View* p_parent, View* p_detachedChild) { }
	};

	// forward declaration <3

	class Gui;
	class DrawingContext;
	class Image;
	class Geometry;
	class MouseEvent;
	enum class Cursor;

	/*
		A rectangle that can draw itself and receive events. Used for GUI components and stuff.
	*/
	class View : public ReferenceCounted, public ProtectedRectangle
	{
	private:
		std::vector<ViewListener*> m_viewEventListeners;
		bool m_isInAnimationUpdateQueue;
		bool m_isVisible;
		bool m_isOverlay;
		bool m_areMouseEventsEnabled;
		Cursor m_cursor;

		float m_opacity;
		RectangleCorners m_corners;

		//------------------------------

		Point<float> m_absolutePosition;
		Rectangle<float> m_lastInvalidatedShadowBounds;
		Rectangle<float> m_shadowBounds;
		Image* m_shadowImage;
		bool m_hasShadow;

		float m_elevation;

		//------------------------------

		uint32 m_layerIndex;
		uint32 m_index;
		uint64 m_id;

		//------------------------------

		bool m_isMouseHovering;

		friend class Gui;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Moves the point(s) representing the absolute position(s) of this view and/or all children of this view (recursively).
			The absolute positions of views are used often for mouse event targeting, among other things.
			Because of this, it is pre-calculated in this way only when this view or a parent view has moved.
		*/
		void moveAbsolutePositions(float p_offsetX, float p_offsetY, bool p_willUpdateChildren = true)
		{
			m_absolutePosition.move(p_offsetX, p_offsetY);

			if (p_willUpdateChildren && m_children.size())
			{
				View* currentContainer = this;
				View* child = 0;
				uint32 startIndex = 0;
				while (true)
				{
				loopBody:
					for (uint32 a = startIndex; a < currentContainer->getNumberOfChildren(); a++)
					{
						child = currentContainer->getChild(a);
						child->moveAbsolutePositions(p_offsetX, p_offsetY, false);
						if (child->getNumberOfChildren())
						{
							currentContainer = child;
							startIndex = 0;
							goto loopBody; // THIS IS GOOD USE OF GOTO OK
						}
					}
					if (currentContainer == this)
					{
						return;
					}
					startIndex = currentContainer->getIndex() + 1;
					currentContainer = currentContainer->getParent();
				}
			}
		}
		Point<float> calculateAbsolutePositionRelativeTo(Point<float> p_position) const;

		/*
			LIBRARY IMPLEMENTED
			Only adds a child view to the child list of this view. 
		*/
		void addChild(View* p_view);
		/*
			LIBRARY IMPLEMENTED
			This doesn't change the actual index of this view, it only helps the view keep track of its current index.
		*/
		void setIndex(uint32 p_index)
		{
			m_index = p_index;
		}
		/*
			LIBRARY IMPLEMENTED
			Makes sure the view is drawn at the correct time, according to elevation.
		*/
		void updateViewDrawingIndex(View* p_view);
		/*
			LIBRARY IMPLEMENTED
			Updates the shadow bounds and the shadow image.
		*/
		void updateShadow();

	protected:
		Gui* m_gui;
		View* m_parent;
		Theme* m_theme;

		std::vector<View*> m_children;

		/*
			LIBRARY IMPLEMENTED
		*/
		virtual void sendBoundsChangeEvents(Rectangle<float> const& p_previousBounds)
		{
			if (p_previousBounds.left != m_bounds.left || p_previousBounds.right != m_bounds.right ||
				p_previousBounds.top != m_bounds.top || p_previousBounds.bottom != m_bounds.bottom)
			{
				if (abs(p_previousBounds.getWidth() - m_bounds.getWidth()) > 0.001 || abs(p_previousBounds.getHeight() - m_bounds.getHeight()) > 0.001)
				{
					updateClipGeometry();
					updateShadow(); // This is to update the shadow bounds and image.

					handleSizeChange(p_previousBounds.getWidth(), p_previousBounds.getHeight());
					for (auto viewListener : m_viewEventListeners)
					{
						viewListener->handleViewSizeChange(this, p_previousBounds.getWidth(), p_previousBounds.getHeight());
					}
				}

				handleBoundsChange(p_previousBounds);
				for (auto viewListener : m_viewEventListeners)
				{
					viewListener->handleViewBoundsChange(this, p_previousBounds);
				}
			}
		}

		/*
			USER IMPLEMENTED
			This gets called whenever a theme color has changed, not including initialization.
		*/
		virtual void handleThemeColorChange(std::string const& p_name, Color const& p_newColor) { };
		/*
			USER IMPLEMENTED
			This gets called whenever a theme easing has changed, not including initialization.
		*/
		virtual void handleThemeEasingChange(std::string const& p_name, Easing const& p_newEasing) { };
		/*
			USER IMPLEMENTED
			This gets called whenever a theme font family name has changed, not including initialization.
		*/
		virtual void handleThemeFontFamilyChange(std::string const& p_name, char const* p_newFontFamilyName) { };
		/*
			USER IMPLEMENTED
			This gets called whenever a theme value has changed, not including initialization.
		*/
		virtual void handleThemeValueChange(std::string const& p_name, float p_newValue) { };

		Geometry* m_clipGeometry;
		/*
			LIBRARY IMPLEMENTED
			This is called whenever the clipping geometry of the view needs to be updated.
			You can override this if you want a custom clipping geometry.
		*/
		virtual void updateClipGeometry();

	public:
		View(View* p_parent, Rectangle<float> const& p_bounds = Rectangle<float>(0.f, 0.f, 0.f, 0.f));
		template<typename T>
		View(View* p_parent, T p_id, Rectangle<float> const& p_bounds = Rectangle<float>(0.f, 0.f, 0.f, 0.f)) :
			View(p_parent, p_bounds)
		{
			setId(p_id);
		}
		virtual ~View();

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			If you set this to true, this view will not block any mouse events from reaching views below this one.
			Whether this view is a mouse listener has no effect on this.
		*/
		void setIsOverlay(bool p_isOverlay)
		{
			m_isOverlay = p_isOverlay;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view blocks mouse events from reaching views below this one. 
			False means it blocks, true means it does not.
		*/
		bool getIsOverlay()
		{
			return m_isOverlay;
		}

		//------------------------------

		/*
			Sets how opaque the view and its children are (multiplied with parent opacity).
		*/
		void setOpacity(float p_opacity)
		{
			m_opacity = p_opacity;
		}
		/*
			Returns how opaque the view and its children are (multiplied with parent opacity).
		*/
		float getOpacity()
		{
			return m_opacity;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the cursor that will by default be shown when the mouse enters the view.
			The default implementation of handleMouseBackgroundEnter sets the cursor to this one, and you can override this behaviour.
		*/
		void setCursor(Cursor p_cursor)
		{
			m_cursor = p_cursor;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the cursor that will by default be shown when the mouse enters the view.
		*/
		Cursor getCursor()
		{
			return m_cursor;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the highest view in the hierarchy, the GUI.
		*/
		Gui* getGui() const
		{
			return m_gui;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the highest view in the hierarchy, the GUI.
			It is cast to another pointer type.
		*/
		template<typename T>
		T* getGui() const
		{
			return (T*)m_gui;
		}

		/*
			LIBRARY IMPLEMENTED
			Attaches this view to a new parent, which will manage the lifetime of the view unless you've called remember() on it.
			If the parameter is 0, the view is only detached from its old parent, and is left alone with no parents :^(.
		*/
		void setParent(View* p_container);
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the parent of this view.
		*/
		View* getParent() const
		{
			return m_parent;
		}
		/*
			Returns a pointer to the parent of this view, casted to a pointer of another type.
		*/
		template<typename T>
		T* getParent() const
		{
			return (T*)m_parent;
		}

		/*
			LIBRARY IMPLEMENTED
			Removes a child view from this view. This forgets the view being removed. 
			If you haven't remembered it yourself, it will get deleted.
		*/
		void removeChild(View* p_view);
		/*
			LIBRARY IMPLEMENTED
			Removes a child view from this view. This forgets the view being removed. 
			If you haven't remembered it yourself, it will get deleted.
		*/
		void removeChild(uint32 p_viewIndex);
		/*
			LIBRARY IMPLEMENTED
			Forgets the children views and empties this view from children.
		*/
		void removeAllChildren();

		/*
			USER IMPLEMENTED
			Gets called when a child view has been added to this view.
		*/
		virtual void handleChildAttachment(View* p_attachedChild) { }
		/*
			USER IMPLEMENTED
			Gets called when a child view has been removed from this view.
		*/
		virtual void handleChildDetachment(View* p_detachedChild) { }

		/*
			LIBRARY IMPLEMENTED
			Returns the child view at an index.
		*/
		View* getChild(uint32 p_viewIndex) const
		{
			return m_children[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the child view at an index, casted to a pointer of another type.
		*/
		template<typename T>
		T* getChild(uint32 p_viewIndex) const
		{
			return (T*)m_children[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the number of child views that are attached to this view.
		*/
		uint32 getNumberOfChildren() const
		{
			return m_children.size();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a vector containing the child views that are attached to this view.
		*/
		std::vector<View*> const& getChildren() const
		{
			return m_children;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets an ID that can be used to retrieve the view from the view hierarchy.
			The type is cast to uint64 and could be for example a string literal or any pointer.
			p_id cannot be 0.
		*/
		template<typename T>
		void setId(T p_id)
		{
			setId((uint64)p_id);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets an ID that can be used to retrieve the view from the view hierarchy.
			p_id cannot be 0.
		*/
		void setId(uint64 p_id);
		/*
			LIBRARY IMPLEMENTED
			Returns the ID that can be used to retrieve the view from the view hierarchy.
			The ID is 0 by default.
			The type is cast from uint64 to T and could be for example a string literal or any pointer.
		*/
		template<typename T>
		T getId() const
		{
			return m_id;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the ID that can be used to retrieve the view from the view hierarchy.
			The ID is 0 by default.
		*/
		uint64 getId() const
		{
			return m_id;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the smallest possible rectangle that contains all child views belonging to this View.
			The rectangle is relative to the position of this view.
		*/
		Rectangle<float> calculateContentBounds() const
		{
			if (!m_children.size())
			{
				return Rectangle<float>();
			}

			float left = m_children[0]->getLeft();
			float right = m_children[0]->getRight();
			float top = m_children[0]->getTop();
			float bottom = m_children[0]->getBottom();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getLeft() < left)
				{
					left = m_children[a]->getLeft();
				}
				if (m_children[a]->getTop() < top)
				{
					top = m_children[a]->getTop();
				}
				if (m_children[a]->getRight() > right)
				{
					right = m_children[a]->getRight();
				}
				if (m_children[a]->getBottom() > bottom)
				{
					bottom = m_children[a]->getBottom();
				}
			}

			return Rectangle<float>(left, top, right, bottom);
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the width of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		float calculateContentWidth() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float left = m_children[0]->getLeft();
			float right = m_children[0]->getRight();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getLeft() < left)
				{
					left = m_children[a]->getLeft();
				}
				if (m_children[a]->getRight() > right)
				{
					right = m_children[a]->getRight();
				}
			}
			return right - left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the hight of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		float calculateContentHeight() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float top = m_children[0]->getTop();
			float bottom = m_children[0]->getBottom();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getTop() < top)
				{
					top = m_children[a]->getTop();
				}
				if (m_children[a]->getBottom() > bottom)
				{
					bottom = m_children[a]->getBottom();
				}
			}
			return bottom - top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the size of the smallest possible rectangle that contains all child views belonging to this View.
		*/
		Point<float> calculateContentSize() const
		{
			return calculateContentBounds().getSize();
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the leftmost edge of all child views belonging to this View. 
			The returned offset is relative to the left edge of this view.
		*/
		float calculateContentLeft() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float left = m_children[0]->getLeft();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getLeft() < left)
				{
					left = m_children[a]->getLeft();
				}
			}
			return left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the rightmost edge of all child views belonging to this View. 
			The returned offset is relative to the left edge of this view.
		*/
		float calculateContentRight() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float right = m_children[0]->getRight();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getRight() > right)
				{
					right = m_children[a]->getRight();
				}
			}
			return right;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost edge of all child views belonging to this View. 
			The returned offset is relative to the top edge of this view.
		*/
		float calculateContentTop() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float top = m_children[0]->getTop();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getTop() < top)
				{
					top = m_children[a]->getTop();
				}
			}
			return top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bottommost edge of all child views belonging to this View. 
			The returned offset is relative to the top edge of this view.
		*/
		float calculateContentBottom() const
		{
			if (!m_children.size())
			{
				return 0.f;
			}

			float bottom = m_children[0]->getBottom();
			for (uint32 a = 1; a < m_children.size(); a++)
			{
				if (m_children[a]->getBottom() > bottom)
				{
					bottom = m_children[a]->getBottom();
				}
			}
			return bottom;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and/or change the size of this view.
		*/
		void setPadding(float p_padding)
		{
			setPadding(p_padding, p_padding, p_padding, p_padding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and/or change the size of this view.
		
			p_horizontalPadding is the spacing at the left and right edges
			p_verticalPadding is the spacing at the top and bottom edges
		*/
		void setPadding(float p_horizontalPadding, float p_verticalPadding)
		{
			setPadding(p_horizontalPadding, p_horizontalPadding, p_verticalPadding, p_verticalPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets a certain spacing between the outer edges of the contents and the edges of this View.
			This may move the child views with a uniform offset and change the size of this view.
		*/
		void setPadding(float p_leftPadding, float p_rightPadding, float p_topPadding, float p_bottomPadding)
		{
			Rectangle<float> contentBounds(calculateContentBounds());
			float offsetX = p_leftPadding - contentBounds.left;
			float offsetY = p_topPadding - contentBounds.top;
			for (uint32 a = 0; a < m_children.size(); a++)
			{
				m_children[a]->move(offsetX, offsetY);
			}
			setSize(contentBounds.getWidth() + p_leftPadding + p_rightPadding, contentBounds.getHeight() + p_topPadding + p_bottomPadding);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the leftmost edge of the contents and the left edge of this View.
			This may move the child views with a uniform offset and change the width of this view.
		*/
		void setLeftPadding(float p_leftPadding)
		{
			float left = calculateContentLeft();
			float offset = p_leftPadding - left;
			for (uint32 a = 0; a < m_children.size(); a++)
			{
				m_children[a]->move(offset, 0.f);
			}
			setWidth(getWidth() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the rightmost edge of the contents and the right edge of this View. 
			This changes the width of this view.
		*/
		void setRightPadding(float p_rightPadding)
		{
			setWidth(calculateContentRight() + p_rightPadding);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the topmost edge of the contents and the top edge of this View.
			This may move the child views with a uniform offset and change the height of this view.
		*/
		void setTopPadding(float p_topPadding)
		{
			float top = calculateContentTop();
			float offset = p_topPadding - top;
			for (uint32 a = 0; a < m_children.size(); a++)
			{
				m_children[a]->move(offset, 0.f);
			}
			setHeight(getHeight() + offset);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the spacing between the bottommost edge of the contents and the bottom edge of this View. 
			This changes the height of this view.
		*/
		void setBottomPadding(float p_bottomPadding)
		{
			setHeight(calculateContentBottom() + p_bottomPadding);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED

			If p_name is any of the strings below, it has a default color that can be changed.
			These colors may be used by views that come with the library, but you could use them yourself too.

			"background"
			"on background"

			"primary"
			"primary on background"
			"on primary"

			"secondary"
			"secondary on background"
			"on secondary"

			"tooltip background"
			"tooltip on background"

			"selection"

			"shadow"

			If it is anything else, the color is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this color in their themes.

			Check out the constructor AvoGUI::Theme::Theme() in AvoGUI.hpp for the default colors and more details.
			In Visual Studio, you can go to the definition of Theme (ctrl + T, "Theme") to find it quickly.
		*/
		void setThemeColor(char const* p_name, Color const& p_color, bool p_willAffectChildren = true);
		/*
			LIBRARY IMPLEMENTED
			See setThemeColor for names that have colors by default.
		*/
		Color const& getThemeColor(char const* p_name) const
		{
			return m_theme->colors[p_name];
		}
		/*
			LIBRARY IMPLEMENTED

			If p_name is any of the strings below, it has a default easing that can be changed.
			These easings may be used by views that come with the library, but you could use them yourself too.

			"in"
			"out"
			"in out"
			"symmetrical in out"

			"ripple"

			If it is anything else, the easing is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this easing in their themes.

			Check out the constructor AvoGUI::Theme::Theme() in AvoGUI.hpp for the default easings and more details.
			In Visual Studio, you can go to the definition of Theme (ctrl + T, "Theme") to find it quickly.
		*/
		void setThemeEasing(char const* p_name, Easing const& p_easing, bool p_willAffectChildren = true)
		{
			if (p_willAffectChildren)
			{
				View* view = this;
				uint32 startIndex = 0;
				while (true)
				{
				loopStart:
					for (uint32 a = startIndex; a < view->getNumberOfChildren(); a++)
					{
						view->getChild(a)->setThemeEasing(p_name, p_easing, false);
						if (view->getChild(a)->getNumberOfChildren())
						{
							view = view->getChild(a);
							startIndex = 0;
							goto loopStart;
						}
					}
					if (view == this)
					{
						break;
					}
					startIndex = view->getIndex() + 1;
					view = view->getParent();
				}
			}

			// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
			// It's not the other way around because the parent lays out the children and the size of the children may changed in the handler.
			if (!m_theme)
			{
				m_theme = new Theme();
			}
			else if (m_theme->getReferenceCount() > 1)
			{
				m_theme->forget();
				m_theme = new Theme(*m_theme);
			}

			if (m_theme->easings[p_name] != p_easing)
			{
				m_theme->easings[p_name] = p_easing;
				handleThemeEasingChange(p_name, p_easing);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			See setThemeEasing for names that have easings by default.
		*/
		Easing const& getThemeEasing(char const* p_name) const
		{
			return m_theme->easings[p_name];
		}
		/*
			LIBRARY IMPLEMENTED

			p_name can be used to identify the usage or type of the font family, while p_fontFamilyName is the actual name of the font family.
			If p_name is "main" it has a default font family name that can be changed.
			That font family may be used by views that come with the library, but you can use it yourself too.
			If p_name is anything else, the font family is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this font family in their themes.

			Check out the constructor AvoGUI::Theme::Theme() in AvoGUI.hpp for the default font families and more details.
			In Visual Studio, you can go to the definition of Theme (ctrl + T, "Theme") to find it quickly.
		*/
		void setThemeFontFamily(char const* p_name, char const* p_fontFamilyName, bool p_willAffectChildren = true)
		{
			if (p_willAffectChildren)
			{
				View* view = this;
				uint32 startIndex = 0;
				while (true)
				{
				loopStart:
					for (uint32 a = startIndex; a < view->getNumberOfChildren(); a++)
					{
						view->getChild(a)->setThemeFontFamily(p_name, p_fontFamilyName, false);
						if (view->getChild(a)->getNumberOfChildren())
						{
							view = view->getChild(a);
							startIndex = 0;
							goto loopStart;
						}
					}
					if (view == this)
					{
						break;
					}
					startIndex = view->getIndex() + 1;
					view = view->getParent();
				}
			}

			// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
			// It's not the other way around because the parent lays out the children and the size of the children may changed in the handler.
			if (!m_theme)
			{
				m_theme = new Theme();
			}
			else if (m_theme->getReferenceCount() > 1)
			{
				m_theme->forget();
				m_theme = new Theme(*m_theme);
			}

			if (m_theme->fontFamilies[p_name] != p_fontFamilyName)
			{
				m_theme->fontFamilies[p_name] = p_fontFamilyName;
				handleThemeFontFamilyChange(p_name, p_fontFamilyName);
			}

		}
		/*
			LIBRARY IMPLEMENTED
			"main" has a font family by default, but you can also access your own font families here.
		*/
		char const* getThemeFontFamily(char const* p_name) const
		{
			return m_theme->fontFamilies[p_name];
		}
		
		/*
			LIBRARY IMPLEMENTED

			If p_name is any of the strings below, it has a default value that can be changed.
			These values may be used by views that come with the library, but you could use them yourself too.
			
			"hover animation speed"

			"tooltip font size"

			"button font size"
			"button character spacing"

			"editable text caret blink rate"

			"text field font size"
			"text field height"
			"text field padding left"
			"text field padding right"
			"text field filled padding bottom"

			If it is anything else, the value is kept in the theme and you can use it yourself.

			if p_willAffectChildren is true, all children and views below those too will change this value in their themes.

			Check out the constructor AvoGUI::Theme::Theme() in AvoGUI.hpp for the default values and more details.
			In Visual Studio, you can go to the definition of Theme (ctrl + T, "Theme") to find it quickly.
		*/
		void setThemeValue(char const* p_name, float p_value, bool p_willAffectChildren = true)
		{
			if (p_willAffectChildren)
			{
				View* view = this;
				uint32 startIndex = 0;
				while (true)
				{
					loopStart:
					for (uint32 a = startIndex; a < view->getNumberOfChildren(); a++)
					{
						view->getChild(a)->setThemeValue(p_name, p_value, false);
						if (view->getChild(a)->getNumberOfChildren())
						{
							view = view->getChild(a);
							startIndex = 0;
							goto loopStart;
						}
					}
					if (view == this)
					{
						break;
					}
					startIndex = view->getIndex() + 1;
					view = view->getParent();
				}
			}

			// This is done afterwards because the children should have updated themselves when it's time for the parent to update itself.
			// It's not the other way around because the parent lays out the children and the size of the children may changed in the handler.
			if (!m_theme)
			{
				m_theme = new Theme();
			}
			else if (m_theme->getReferenceCount() > 1)
			{
				m_theme->forget();
				m_theme = new Theme(*m_theme);
			}

			if (m_theme->values[p_name] != p_value)
			{
				m_theme->values[p_name] = p_value;
				handleThemeValueChange(p_name, p_value);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			See setThemeValue for names that have values by default.
		*/
		float getThemeValue(char const* p_name) const
		{
			return m_theme->values[p_name];
		}

		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the theme that is used by this view.
		*/
		Theme const* getTheme() const
		{
			return m_theme;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(float p_left, float p_top, float p_right, float p_bottom) override
		{
			Rectangle<float> boundsBefore = m_bounds;

			float offsetX = p_left - m_bounds.left;
			float offsetY = p_top - m_bounds.top;
			if (offsetX || offsetY)
			{
				moveAbsolutePositions(offsetX, offsetY);
			}

			m_bounds.left = p_left;
			m_bounds.top = p_top;
			m_bounds.right = p_right;
			m_bounds.bottom = p_bottom;

			sendBoundsChangeEvents(boundsBefore);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(float p_left, float p_top, float p_right, float p_bottom)
		{
			Rectangle<float> boundsBefore = m_bounds;

			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.left += offsetX;
				m_bounds.top += offsetY;
			}

			m_bounds.right = m_bounds.left + p_right - p_left;
			m_bounds.bottom = m_bounds.top + p_bottom - p_top;

			sendBoundsChangeEvents(boundsBefore);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(Point<float> const& p_position, Point<float> const& p_size) override
		{
			setBounds(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(Point<float> const& p_position, Point<float> const& p_size)
		{
			setAbsoluteBounds(p_position.x, p_position.y, p_position.x + p_size.x, p_position.y + p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		void setBounds(Rectangle<float> const& p_rectangle) override
		{
			setBounds(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the rectangle representing the bounds of this view relative to the top left corner of the GUI.
		*/
		void setAbsoluteBounds(Rectangle<float> const& p_rectangle)
		{
			setAbsoluteBounds(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a rectangle representing the bounds of this view relative to the top left corner of the parent.
		*/
		Rectangle<float> const& getBounds() const override
		{
			return m_bounds;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view relative to the top left corner of the GUI.
		*/
		Rectangle<float> getAbsoluteBounds() const
		{
			return Rectangle<float>(m_absolutePosition, m_bounds.getSize());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the bounds of the view shadow relative to the top left corner of the GUI.
		*/
		Rectangle<float> getAbsoluteShadowBounds() const
		{
			Rectangle<float> bounds;
			bounds.left = m_absolutePosition.x + (m_bounds.getWidth() - m_shadowBounds.getWidth())*0.5f;
			bounds.top = m_absolutePosition.y + (m_bounds.getHeight() - m_shadowBounds.getHeight())*0.5f;
			bounds.right = bounds.left + m_shadowBounds.getWidth();
			bounds.bottom = bounds.top + m_shadowBounds.getHeight();
			return bounds;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Moves the whoie view.
		*/
		void move(Point<float> const& p_offset) override
		{
			move(p_offset.x, p_offset.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Moves the whole view.
		*/
		void move(float p_offsetX, float p_offsetY) override
		{
			if (p_offsetX || p_offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_offsetX, p_offsetY);
				m_bounds.move(p_offsetX, p_offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_top != m_bounds.top)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, p_top - m_bounds.top);
				m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top left coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopLeft(float p_left, float p_top, bool p_willKeepSize = true)
		{
			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.setTopLeft(m_bounds.left + offsetX, m_bounds.top + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top left corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getTopLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.top);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top left corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> const& getAbsoluteTopLeft() const
		{
			return m_absolutePosition;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopRight(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopRight(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_top != m_bounds.top)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_willKeepSize ? p_right - m_bounds.right : 0, p_top - m_bounds.top);
				m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top right coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteTopRight(float p_right, float p_top, bool p_willKeepSize = true)
		{
			float offsetX = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			float offsetY = p_top - m_absolutePosition.y;
			if (offsetX || offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_willKeepSize ? offsetX : 0, offsetY);
				m_bounds.setTopRight(m_bounds.right + offsetX, m_bounds.top + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top right corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getTopRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.top);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the top right corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteTopRight() const
		{
			return Point<float>(m_absolutePosition.x + m_bounds.right - m_bounds.left, m_absolutePosition.y);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomLeft(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomLeft(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_bottom != m_bounds.bottom)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, p_willKeepSize ? p_bottom - m_bounds.bottom : 0);
				m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom left coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true)
		{
			float offsetX = p_left - m_absolutePosition.x;
			float offsetY = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offsetX || offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, p_willKeepSize ? offsetY : 0.f);
				m_bounds.setBottomLeft(m_bounds.left + offsetX, m_bounds.bottom + offsetY, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom left corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getBottomLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom left corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteBottomLeft() const
		{
			return Point<float>(m_absolutePosition.x, m_absolutePosition.y + m_bounds.bottom - m_bounds.top);
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomRight(Point<float> const& p_position, bool p_willKeepSize = true) override
		{
			setBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomRight(Point<float> const& p_position, bool p_willKeepSize = true)
		{
			setAbsoluteBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the parent. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_bottom != m_bounds.bottom)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepSize)
				{
					moveAbsolutePositions(p_right - m_bounds.right, p_bottom - m_bounds.bottom);
				}
				m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom right coordinates of the view relative to the top left corner of the GUI. 
			If p_willKeepSize is true, the view will only get positioned, keeping its size.
		*/
		void setAbsoluteBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true)
		{
			float offsetX = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			float offsetY = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offsetX || offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepSize)
				{
					moveAbsolutePositions(offsetX, offsetY);
					m_bounds.move(offsetX, offsetY);
				}
				else
				{
					m_bounds.moveBottomRight(offsetX, offsetY);
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom right corner of the view relative to the top left corner of the parent.
		*/
		Point<float> getBottomRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinates of the bottom right corner of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteBottomRight() const 
		{
			return Point<float>(m_absolutePosition.x + m_bounds.right - m_bounds.left, m_absolutePosition.y + m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the parent. 
		*/
		void setCenter(Point<float> const& p_position) override
		{
			setCenter(p_position.x, p_position.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the GUI. 
		*/
		void setAbsoluteCenter(Point<float> const& p_position)
		{
			setAbsoluteCenter(p_position.x, p_position.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the parent. 
		*/
		void setCenter(float p_x, float p_y) override
		{
			if (p_x != m_bounds.getCenterX() || p_y != m_bounds.getCenterY())
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_x - m_bounds.getCenterX(), p_y - m_bounds.getCenterY());
				m_bounds.setCenter(p_x, p_y);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the center coordinates of the view relative to the top left corner of the GUI. 
		*/
		void setAbsoluteCenter(float p_x, float p_y)
		{
			float offsetX = p_x - m_absolutePosition.x - getWidth()*0.5f;
			float offsetY = p_y - m_absolutePosition.y - getHeight()*0.5f;
			if (offsetX || offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, offsetY);
				m_bounds.move(offsetX, offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the horizontal center coordinate of the view relative to the left edge of the parent.
		*/
		void setCenterX(float p_x) override
		{
			if (p_x != m_bounds.getCenterX())
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_x - m_bounds.getCenterX(), 0);
				m_bounds.setCenterX(p_x);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the horizontal center coordinate of the view relative to the left edge of the GUI.
		*/
		void setAbsoluteCenterX(float p_x)
		{
			float offsetX = p_x - m_absolutePosition.x - getWidth() * 0.5f;
			if (offsetX)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(offsetX, 0);
				m_bounds.moveX(offsetX);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the vertical center coordinate of the view relative to the top edge of the parent.
		*/
		void setCenterY(float p_y) override
		{
			if (p_y != m_bounds.getCenterY())
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_y - m_bounds.getCenterY());
				m_bounds.setCenterY(p_y);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the vertical center coordinate of the view relative to the top edge of the GUI.
		*/
		void setAbsoluteCenterY(float p_y)
		{
			float offsetY = p_y - m_absolutePosition.y - getHeight() * 0.5f;
			if (offsetY)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0.f, offsetY);
				m_bounds.moveX(offsetY);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the center coordinates of the view relative to the top left corner of the parent.
		*/
		Point<float> getCenter() const override
		{
			return m_bounds.getCenter();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the center coordinates of the view relative to the top left corner of the GUI.
		*/
		Point<float> getAbsoluteCenter() const
		{
			return Point<float>(m_absolutePosition.x + getWidth()*0.5f, m_absolutePosition.y + getHeight()*0.5f);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the x-axis center coordinate of the view relative to the left edge of the parent.
		*/
		float getCenterX() const override
		{
			return m_bounds.getCenterX();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the x-axis center coordinate of the view relative to the left edge of the GUI.
		*/
		float getAbsoluteCenterX() const
		{
			return m_absolutePosition.x + getWidth()*0.5f;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the y-axis center coordinate of the view relative to the top edge of the parent.
		*/
		float getCenterY() const override
		{
			return m_bounds.getCenterY();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the y-axis center coordinate of the view relative to the top edge of the GUI.
		*/
		float getAbsoluteCenterY() const
		{
			return m_absolutePosition.y + getHeight()*0.5f;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the left coordinate of this view relative to the left edge of the parent. 
			If p_willKeepWidth is true, the right coordinate will also be changed so that the width of the view stays the same.
		*/
		void setLeft(float p_left, bool p_willKeepWidth = true) override
		{
			if (p_left != m_bounds.left)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_bounds.left, 0);
				m_bounds.setLeft(p_left, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the left coordinate of this view and updates the layout relative to the left edge of the GUI. 
			If p_willKeepWidth is true, the right coordinate will also be changed so that the width of the view stays the same.
		*/
		void setAbsoluteLeft(float p_left, bool p_willKeepWidth = true)
		{
			if (p_left != m_absolutePosition.x)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(p_left - m_absolutePosition.x, 0);
				m_bounds.setLeft(p_left - m_absolutePosition.x + m_bounds.left, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the left coordinate of this view relative to the left edge of the parent.
		*/
		float getLeft() const override
		{
			return m_bounds.left;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the left coordinate of this view relative to the left edge of the GUI.
		*/
		float getAbsoluteLeft() const 
		{
			return m_absolutePosition.x;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the top coordinate of this view relative to the top edge of the parent. 
			If p_willKeepHeight is true, the bottom coordinate will also be changed so that the height of the view stays the same.
		*/
		void setTop(float p_top, bool p_willKeepHeight = true) override
		{
			if (p_top != m_bounds.top)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_top - m_bounds.top);
				m_bounds.setTop(p_top, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the top coordinate of this view relative to the top edge of the GUI. 
			If p_willKeepHeight is true, the bottom coordinate will also be changed so that the height of the view stays the same.
		*/
		void setAbsoluteTop(float p_top, bool p_willKeepHeight = true)
		{
			if (p_top != m_absolutePosition.y)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				moveAbsolutePositions(0, p_top - m_absolutePosition.y);
				m_bounds.setTop(p_top - m_absolutePosition.y + m_bounds.top, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the top coordinate of this view relative to the top edge of the parent.
		*/
		float getTop() const override
		{
			return m_bounds.top;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the top coordinate of this view relative to the top edge of the GUI.
		*/
		float getAbsoluteTop() const
		{
			return m_absolutePosition.y;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the right coordinate of this view relative to the left edge of the parent. 
			If p_willKeepWidth is true, the left coordinate will also be changed so that the width of the view stays the same.
		*/
		void setRight(float p_right, bool p_willKeepWidth = true) override
		{
			if (p_right != m_bounds.right)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepWidth)
				{
					moveAbsolutePositions(p_right - m_bounds.right, 0);
				}
				m_bounds.setRight(p_right, p_willKeepWidth);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the right coordinate of this view relative to the left edge of the GUI. 
			If p_willKeepWidth is true, the left coordinate will also be changed so that the width of the view stays the same.
		*/
		void setAbsoluteRight(float p_right, bool p_willKeepWidth = true) 
		{
			float offset = p_right - m_absolutePosition.x + m_bounds.left - m_bounds.right;
			if (offset)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepWidth)
				{
					moveAbsolutePositions(offset, 0);
					m_bounds.moveX(offset);
				}
				else 
				{
					m_bounds.right += offset;
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the right edge of this view relative to the left edge of the parent.
		*/
		float getRight() const override
		{
			return m_bounds.right;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the right edge of this view relative to the left edge of the GUI.
		*/
		float getAbsoluteRight() const
		{
			return m_absolutePosition.x + m_bounds.right - m_bounds.left;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the bottom coordinate of this view relative to the top edge of the parent and updates the layout. 
			If p_willKeepHeight is true, the top coordinate will also be changed so that the height of the view stays the same.
		*/
		void setBottom(float p_bottom, bool p_willKeepHeight = true) override
		{
			if (p_bottom != m_bounds.bottom)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepHeight)
				{
					moveAbsolutePositions(0, p_bottom - m_bounds.bottom);
				}
				m_bounds.setBottom(p_bottom, p_willKeepHeight);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the bottom coordinate of this view relative to the top edge of the GUI and updates the layout. 
			If p_willKeepHeight is true, the top coordinate will also be changed so that the height of the view stays the same.
		*/
		void setAbsoluteBottom(float p_bottom, bool p_willKeepHeight = true)
		{
			float offset = p_bottom - m_absolutePosition.y + m_bounds.top - m_bounds.bottom;
			if (offset)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				if (p_willKeepHeight)
				{
					m_bounds.moveY(offset);
					moveAbsolutePositions(0, offset);
				}
				else
				{
					m_bounds.bottom += offset;
				}
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the bottom edge of this view relative to the top edge of the parent.
		*/
		float getBottom() const override
		{
			return m_bounds.bottom;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the coordinate of the bottom edge of this view relative to the top edge of the GUI.
		*/
		float getAbsoluteBottom() const 
		{
			return m_absolutePosition.y + m_bounds.bottom - m_bounds.top;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the width of this view by changing the right coordinate and updates the layout.
		*/
		void setWidth(float p_width) override
		{
			if (p_width != m_bounds.right - m_bounds.left)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setWidth(p_width);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the distance between the left and right edges of this view.
		*/
		float getWidth() const override
		{
			return m_bounds.right - m_bounds.left;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the height of this view by changing the bottom coordinate and updates the layout.
		*/
		void setHeight(float p_height) override
		{
			if (p_height != m_bounds.bottom - m_bounds.top)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setHeight(p_height);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the distance between the top and bottom edges of this view.
		*/
		float getHeight() const override
		{
			return m_bounds.bottom - m_bounds.top;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets the size of this view by changing the right and bottom coordinates and updates the layout.
		*/
		void setSize(Point<float> const& p_size) override
		{
			setSize(p_size.x, p_size.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the size of this view by changing the right and bottom coordinates and updates the layout.
		*/
		void setSize(float p_width, float p_height) override
		{
			if (p_width != m_bounds.right - m_bounds.left || p_height != m_bounds.bottom - m_bounds.top)
			{
				AvoGUI::Rectangle<float> boundsBefore = m_bounds;
				m_bounds.setSize(p_width, p_height);
				sendBoundsChangeEvents(boundsBefore);
			}
		}
		/*
			LIBRARY IMPLEMENTED
			x is width and y is height in the returned point.
		*/
		Point<float> getSize() const override
		{
			return Point<float>(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps a rectangle that is relative to the top left corner of the parent.
		*/
		bool getIsIntersecting(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom))
				{
					if (p_right < m_bounds.left + m_corners.topLeftSizeX && p_bottom < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_right, (m_bounds.top + m_corners.topLeftSizeY - p_bottom) * m_corners.topLeftSizeX / m_corners.topLeftSizeY) < m_corners.topLeftSizeX * m_corners.topLeftSizeX;
						}
						return p_bottom > m_bounds.top + m_corners.topLeftSizeY - (p_right - m_bounds.left) * m_corners.topLeftSizeY / m_corners.topLeftSizeX;
					}
					else if (p_right < m_bounds.left + m_corners.bottomLeftSizeX && p_top > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_right, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_top) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) < m_corners.bottomLeftSizeX * m_corners.bottomLeftSizeX;
						}
						return p_top < m_bounds.bottom - m_corners.bottomLeftSizeY + (p_right - m_bounds.left) * m_corners.bottomLeftSizeY / m_corners.bottomLeftSizeX;
					}
					else if (p_left > m_bounds.right - m_corners.topRightSizeX && p_bottom < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_left, (m_bounds.top + m_corners.topRightSizeY - p_bottom) * m_corners.topRightSizeX / m_corners.topRightSizeY) < m_corners.topRightSizeX * m_corners.topRightSizeX;
						}
						return p_bottom > m_bounds.top + (m_bounds.right - p_left) * m_corners.topRightSizeY / m_corners.topRightSizeX;
					}
					else if (p_left > m_bounds.right - m_corners.bottomRightSizeX && p_top > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_left, (m_bounds.bottom - m_corners.bottomRightSizeY - p_top) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) < m_corners.bottomRightSizeX * m_corners.bottomRightSizeX;
						}
						return p_top < m_bounds.bottom - (m_bounds.right - p_left) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsIntersecting(p_left, p_top, p_right, p_bottom);
		}
		bool getIsIntersecting(Rectangle<float> const& p_rectangle) const override
		{
			return getIsIntersecting(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps another protected rectangle that is relative to the top left corner of the parent.
		*/
		bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
		{
			return getIsIntersecting(p_protectedRectangle->getLeft(), p_protectedRectangle->getTop(), p_protectedRectangle->getRight(), p_protectedRectangle->getBottom());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view intersects/overlaps another view. Takes rounded corners of both views into account.
		*/
		//bool getIsIntersecting(View* p_view) const;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether a rectangle can be contained within this view. The rectangle is relative to the parent of this view.
		*/
		bool getIsContaining(float p_left, float p_top, float p_right, float p_bottom) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom))
				{
					if (p_left < m_bounds.left + m_corners.topLeftSizeX && p_top < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_left, (m_bounds.top + m_corners.topLeftSizeY - p_top)*m_corners.topLeftSizeX/m_corners.topLeftSizeY) > m_corners.topLeftSizeX*m_corners.topLeftSizeX)
							{
								return false;
							}
						}
						else if (p_top > m_bounds.top + m_corners.topLeftSizeY - (p_left - m_bounds.left) * m_corners.topLeftSizeY / m_corners.topLeftSizeX)
						{
							return false;
						}
					}
					else if (p_left < m_bounds.left + m_corners.bottomLeftSizeX && p_bottom > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_left, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_bottom) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) > m_corners.bottomLeftSizeX*m_corners.bottomLeftSizeX)
							{
								return false;
							}
						}
						else if (p_bottom < m_bounds.bottom - m_corners.bottomLeftSizeY + (m_bounds.right - p_left) * m_corners.topRightSizeY / m_corners.topRightSizeX)
						{
							return false;
						}
					}
					else if (p_right > m_bounds.right - m_corners.topRightSizeX && p_top < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_right, (m_bounds.top + m_corners.topRightSizeY - p_top) * m_corners.topRightSizeX / m_corners.topRightSizeY) > m_corners.topRightSizeX* m_corners.topRightSizeX)
							{
								return false;
							}
						}
						else if (p_top > m_bounds.top + (m_bounds.right - p_right) * m_corners.topRightSizeY / m_corners.topRightSizeY)
						{
							return false;
						}
					}
					else if (p_right > m_bounds.right - m_corners.bottomRightSizeX && p_bottom > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							if (Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_right, (m_bounds.bottom - m_corners.bottomRightSizeY - p_bottom) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) > m_corners.bottomRightSizeX* m_corners.bottomRightSizeX)
							{
								return false;
							}
						}
						else if (p_bottom < m_bounds.bottom - (m_bounds.right - p_right) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX)
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_left, p_top, p_right, p_bottom);
		}
		bool getIsContaining(Rectangle<float> const& p_rectangle)
		{
			return getIsContaining(p_rectangle.left, p_rectangle.top, p_rectangle.right, p_rectangle.bottom);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether another protected rectangle can be contained within this view. The rectangle is relative to the parent of this view.
		*/
		bool getIsContaining(ProtectedRectangle* p_rectangle) const override
		{
			return getIsContaining(p_rectangle->getLeft(), p_rectangle->getTop(), p_rectangle->getRight(), p_rectangle->getBottom());
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether this view contains another view. Takes rounded corners of both views into account.
		*/
		//bool getIsContaining(View* p_view) const;

		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the parent of this view.
		*/
		bool getIsContaining(float p_x, float p_y) const override
		{
			if (m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY ||
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY)
			{
				if (m_bounds.getIsContaining(p_x, p_y))
				{
					if (p_x < m_bounds.left + m_corners.topLeftSizeX && p_y < m_bounds.top + m_corners.topLeftSizeY)
					{
						if (m_corners.topLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.topLeftSizeX - p_x, (m_bounds.top + m_corners.topLeftSizeY - p_y) * m_corners.topLeftSizeX / m_corners.topLeftSizeY) < m_corners.topLeftSizeX * m_corners.topLeftSizeX;
						}
						return p_y > m_bounds.top + m_corners.topLeftSizeY - (p_x - m_bounds.left)* m_corners.topLeftSizeY / m_corners.topLeftSizeX;
					}
					else if (p_x > m_bounds.right - m_corners.topRightSizeX && p_y < m_bounds.top + m_corners.topRightSizeY)
					{
						if (m_corners.topRightType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.topRightSizeX - p_x, (m_bounds.top + m_corners.topRightSizeY - p_y) * m_corners.topRightSizeX / m_corners.topRightSizeY) < m_corners.topRightSizeX * m_corners.topRightSizeX;
						}
						return p_y > m_bounds.top + (m_bounds.right - p_x) * m_corners.topRightSizeY / m_corners.topRightSizeX;
					}
					else if (p_x < m_bounds.left + m_corners.bottomLeftSizeX && p_y > m_bounds.bottom - m_corners.bottomLeftSizeY)
					{
						if (m_corners.bottomLeftType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.left + m_corners.bottomLeftSizeX - p_x, (m_bounds.bottom - m_corners.bottomLeftSizeY - p_y) * m_corners.bottomLeftSizeX / m_corners.bottomLeftSizeY) < m_corners.bottomLeftSizeX * m_corners.bottomLeftSizeX;
						}
						return p_y < m_bounds.bottom - m_corners.bottomLeftSizeY + (p_x - m_bounds.left) * m_corners.bottomLeftSizeY / m_corners.bottomLeftSizeX;
					}
					else if (p_x > m_bounds.right - m_corners.bottomRightSizeX && p_y > m_bounds.bottom - m_corners.bottomRightSizeY)
					{
						if (m_corners.bottomRightType == RectangleCornerType::Round)
						{
							return Point<>::getLengthSquared(m_bounds.right - m_corners.bottomRightSizeX - p_x, (m_bounds.bottom - m_corners.bottomRightSizeY - p_y) * m_corners.bottomRightSizeX / m_corners.bottomRightSizeY) < m_corners.bottomRightSizeX * m_corners.bottomRightSizeX;
						}
						return p_y < m_bounds.bottom - (m_bounds.right - p_x) * m_corners.bottomRightSizeY / m_corners.bottomRightSizeX;
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_x, p_y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the parent of the view.
		*/
		bool getIsContaining(Point<float> const& p_point) const override
		{
			return getIsContaining(p_point.x, p_point.y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
		*/
		bool getIsContainingAbsolute(float p_x, float p_y) const
		{
			if (m_parent)
			{
				return getIsContaining(p_x - m_parent->getAbsoluteLeft(), p_y - m_parent->getAbsoluteTop());
			}
			return getIsContaining(p_x, p_y);
		}
		/*
			LIBRARY IMPLEMENTED
			Returns whether a point is within the bounds of this view. The point is relative to the top-left corner of the GUI.
		*/
		bool getIsContainingAbsolute(Point<float> const& p_point) const
		{
			if (m_parent)
			{
				return getIsContaining(p_point.x - m_parent->getAbsoluteLeft(), p_point.y - m_parent->getAbsoluteTop());
			}
			return getIsContaining(p_point.x, p_point.y);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets whether the view is visible and can receive events.
		*/
		void setIsVisible(bool p_isVisible);
		/*
			LIBRARY IMPLEMENTED
			Returns whether the view is visible and can receive events.
		*/
		bool getIsVisible() const
		{
			return m_isVisible;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the corners of the view. p_radius is the radius of the corner circles.
		*/
		void setCornerRadius(float p_radius)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_radius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_radius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_radius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_radius;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets the roundness of the different corners of the view, as the radius of a quarter circle.
		*/
		void setCornerRadius(float p_topLeftRadius, float p_topRightRadius, float p_bottomLeftRadius, float p_bottomRightRadius)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_topLeftRadius;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_topRightRadius;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_bottomLeftRadius;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_bottomRightRadius;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Round;
			updateClipGeometry();
		}

		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		void setCornerCutSize(float p_cutSize)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_cutSize;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_cutSize;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_cutSize;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_cutSize;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Cut;
			updateClipGeometry();
		}
		/*
			LIBRARY IMPLEMENTED
			Sets how much the corners of the view are cut.
		*/
		void setCornerCutSize(float p_topLeftSize, float p_topRightSize, float p_bottomLeftSize, float p_bottomRightSize)
		{
			m_corners.topLeftSizeX = m_corners.topLeftSizeY = p_topLeftSize;
			m_corners.topRightSizeX = m_corners.topRightSizeY = p_topRightSize;
			m_corners.bottomLeftSizeX = m_corners.bottomLeftSizeY = p_bottomLeftSize;
			m_corners.bottomRightSizeX = m_corners.bottomRightSizeY = p_bottomRightSize;
			m_corners.topLeftType = m_corners.topRightType = m_corners.bottomLeftType = m_corners.bottomRightType = RectangleCornerType::Cut;
			updateClipGeometry();
		}

		/*
			Sets the shapes of the corners of the view.
		*/
		void setCorners(RectangleCorners const& p_corners)
		{
			m_corners = p_corners;
			updateClipGeometry();
		}
		/*
			Returns the shapes of the corners of the view.
		*/
		RectangleCorners& getCorners()
		{
			return m_corners;
		}

		bool getHasCornerStyles()
		{
			return m_corners.topLeftSizeX && m_corners.topLeftSizeY || m_corners.topRightSizeX && m_corners.topRightSizeY || 
				m_corners.bottomLeftSizeX && m_corners.bottomLeftSizeY || m_corners.bottomRightSizeX && m_corners.bottomRightSizeY;
		}
		
		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the elevation of the view. This both changes its shadow (if the view has shadow) and drawing order.
			The higher the elevation is, the later it will get drawn.
			If p_elevation is negative, it is set from the top of the elevation space.
		*/
		void setElevation(float p_elevation);
		/*
			LIBRARY IMPLEMENTED
			Returns the elevation of the view. See the setElevation method.
		*/
		float getElevation() const
		{
			return m_elevation;
		}

		/*
			LIBRARY IMPLEMENTED
			Sets whether the elevation is shown with a shadow.
		*/
		void setHasShadow(bool p_hasShadow);
		/*
			LIBRARY IMPLEMENTED
			Returns whether the elevation is shown with a shadow.
		*/
		bool getHasShadow() const
		{
			return m_hasShadow;
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the rectangle that represents the area where the shadow is drawn, relative to the view position.
			The view is always contained within the shadow bounds.
		*/
		Rectangle<float> getShadowBounds() const
		{
			return m_shadowBounds;
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the index of this view relative to its siblings.
		*/
		uint32 getIndex() const
		{
			return m_index;
		}

		/*
			LIBRARY IMPLEMENTED
			Returns the layer index of the view, how deep down the view hierarchy it is.
			The GUI view has a layer index of 0.
		*/
		uint32 getLayerIndex() const
		{
			return m_layerIndex;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Queues an animation update for the next frame.
		*/
		void queueAnimationUpdate();
		/*
			LIBRARY IMPLEMENTED
			Don't do anything with this.
		*/
		void informAboutAnimationUpdateQueueRemoval()
		{
			m_isInAnimationUpdateQueue = false;
		}

		/*
			USER IMPLEMENTED
			Updates things like animations and does anything that you never want to happen more than once every frame.
			Call queueAnimationUpdate() when you want this method to be called in the next interval. 
			This system allows for animations to only get updated when they have to.
		*/
		virtual void updateAnimations() { }

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Adds an event listener to the view, which recieves events about when the view has changed size.
			The listener is not remembered, just put into a list.
		*/
		void addViewListener(ViewListener* p_eventListener)
		{
			m_viewEventListeners.push_back(p_eventListener);
		}
		/*
			LIBRARY IMPLEMENTED
			This only removes the pointer from a list, it doesn't forget it or anything.
		*/
		void removeViewListener(ViewListener* p_eventListener)
		{
			removeVectorElementWhileKeepingOrder(m_viewEventListeners, p_eventListener);
		}
		/*
			LIBRARY IMPLEMENTED
			This only removes a pointer from a list, it doesn't forget it or anything.
		*/
		void removeViewListener(uint32 p_index)
		{
			m_viewEventListeners.erase(m_viewEventListeners.begin() + p_index);
		}

		//------------------------------
		
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		virtual void enableMouseEvents();
		/*
			LIBRARY IMPLEMENTED
			Mouse events are disabled by default.
		*/
		virtual void disableMouseEvents();
		/*
			LIBRARY IMPLEMENTED
		*/
		virtual bool getAreMouseEventsEnabled()
		{
			return m_areMouseEventsEnabled;
		}

		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been pressed down while the pointer is above the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseDown(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been released after having been pressed down when the mouse pointer was above the view.
			The mouse cursor may have left the view during the time the button is pressed, but it will still recieve the event.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseUp(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been double clicked while the mouse pointer is above the view.
			The default implementation calls handleMouseUp.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseDoubleClick(MouseEvent const& p_event) 
		{
			handleMouseUp(p_event);
		}

		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has been moved within the bounds of the view. 
			This can be blocked by non-overlay views which have the same parent and are overlapping this one.
			The event is never blocked by children of this view.
			If it has entered the view, a mouse enter event is sent, and if it has left the view, a mouse leave event is sent.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseMove(MouseEvent const& p_event) { }
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when the mouse pointer has entered any part of the view that is not occupied by children of this view.
			By default, this changes the mouse cursor to the cursor that is set with setCursor on the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseBackgroundEnter(MouseEvent const& p_event);
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has left any part of the view that is not occupied by children of this view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseBackgroundLeave(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when the mouse pointer has entered the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view 
			that the mouse has entered gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseEnter(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when the mouse cursor has left the bounds of the view.
			This can be called on one or more views at the same time, but among views that have the same parent, only the topmost view
			that the mouse has left gets the event (except for overlay views, they always get the event as long as they are targeted).
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseLeave(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when the mouse wheel has been moved/scrolled while the mouse pointer is above the view.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseScroll(MouseEvent const& p_event) { }

		//------------------------------

		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the size of the view has been changed.
		*/
		virtual void handleSizeChange() { }
		/*
			LIBRARY IMPLEMENTED
			This calls handleSizeChange() by default. Override this method if you need to know the previous size of the view.
		*/
		virtual void handleSizeChange(float p_previousWidth, float p_previousHeight)
		{
			handleSizeChange();
		}
		/*
			USER IMPLEMENTED
			Implement this method in your view if you want to update things when the bounds of the view have been changed.
		*/
		virtual void handleBoundsChange(Rectangle<float> const& p_previousBounds) { }

		/*
			LIBRARY IMPLEMENTED
			Call this if you want the view to get redrawn. Adds an invalid rectangle to the window or two if the view has been moved.
		*/
		void invalidate();

		/*
			LIBRARY IMPLEMENTED
			Draws the shadow of the view. This gets called by the parent View before the
			content of the view is drawn.
		*/
		void drawShadow(DrawingContext* p_drawingContext);
		/*
			USER IMPLEMENTED
			Draws the contents of the view. 
			This method is called by default from the other draw method that also takes the target rectangle as input. 
			You often don't need to use that parameter.
			
			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
		*/
		virtual void draw(DrawingContext* p_drawingContext) { }
		/*
			USER IMPLEMENTED
			Draws the content of the view. Override this method if you want the target rectangle, override the overloaded
			method that only takes the drawing context otherwise.
		
			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI. 
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle)
		{
			draw(p_drawingContext);
		}

		/*
			USER IMPLEMENTED
			Draws on top of child views. 
			This method is called by default from the other drawOverlay method that also takes the target rectangle as input. 
			You do not often care about that parameter.
		
			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
		*/
		virtual void drawOverlay(DrawingContext* p_drawingContext) {}

		/*
			USER IMPLEMENTED
			Draws on top of child views. Override this method if you want the target rectangle, override the overloaded 
			method that only takes the drawing context otherwise.
		
			p_drawingContext is an object used to draw graphics to the window and create graphics objects like text and images.
			p_targetRectangle is the rectangle that needs to be drawn, relative to the top-left corner of the GUI.
			To optimize your application, you can make sure to only draw stuff in this region.
		*/
		virtual void drawOverlay(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle)
		{
			drawOverlay(p_drawingContext);
		}
	};

	//------------------------------

#pragma region Window
	class Window;

	class WindowEvent
	{
	public:
		/*
			The window that has recieved the event from the OS.
		*/
		Window* window = 0;
		/*
			The new width of the window if it has changed size (includes sizeChange/maximize/restore events).
		*/
		uint32 width = 0U;
		/*
			The new height of the window if it has changed size (includes sizeChange/maximize/restore events).
		*/
		uint32 height = 0U;
	};

	class WindowListener
	{
	public:
		/*
			USER IMPLEMENTED
			Gets called when a window has been created.
			p_event is an object that contains information about the event.
		*/
		virtual void handleWindowCreate(WindowEvent const& p_event) { }
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when a window has been requested to be closed. 
			If the handler returns true, the window will close and get destroyed. This is the default behavior.
			p_event is an object containing information about the event.
		*/
		virtual bool handleWindowClose(WindowEvent const& p_event) { return true; }

		/*
			USER IMPLEMENTED
			Gets called when a window has been minimized in the taskbar. 
			p_event is an object containing information about the event.
		*/
		virtual void handleWindowMinimize(WindowEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a window has been maximized so that it is as big as possible while still showing the border.
			The width and height properties of p_event tell you the new size of the window.
		*/
		virtual void handleWindowMaximize(WindowEvent const& p_event) { }

		/*
			USER IMPLEMENTED
			Gets called when a window has been restored after being in a minimized or maximized state.
			The width and height properties of p_event tell you the new size of the window.
		*/
		virtual void handleWindowRestore(WindowEvent const& p_event) { }

		/*
			USER IMPLEMENTED
			Gets called when the size of a window has changed. 
			This includes if it has been maximized, or if the border has been dragged to resize it. 
			The width and height properties of p_event tell you the new size of the window.
		*/
		virtual void handleWindowSizeChange(WindowEvent const& p_event) { }

		/*
			USER IMPLEMENTED
			Gets called when a window has been focused, meaning it has been interacted with so that another window loses focus.
			p_event is an object containing information about the event.
		*/
		virtual void handleWindowFocus(WindowEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a window has been unfocused, meaning another window is interacted with.
			p_event is an object containing information about the event.
		*/
		virtual void handleWindowUnfocus(WindowEvent const& p_event) { }
	};

	//------------------------------

	enum class Cursor
	{
		Arrow,
		Blocked,
		Hand,
		Ibeam,
		Wait,
		ResizeAll,
		ResizeWE,
		ResizeNS,
		ResizeNESW,
		ResizeNWSE
	};

	//------------------------------

	enum class ModifierKeyFlags
	{
		None = 0UL,
		Ctrl = 0x1UL,
		Alt = 0x2UL,
		Shift = 0x4UL,
		LeftMouse = 0x8UL,
		MiddleMouse = 0x10UL,
		RightMouse = 0x20UL,
		X0Mouse = 0x40UL,
		X1Mouse = 0x80UL
	};
	constexpr bool operator&(ModifierKeyFlags p_left, ModifierKeyFlags p_right)
	{
		return (uint32)p_left & (uint32)p_right;
	}
	constexpr ModifierKeyFlags operator|(ModifierKeyFlags p_left, ModifierKeyFlags p_right)
	{
		return ModifierKeyFlags((uint32)p_left | (uint32)p_right);
	}
	constexpr ModifierKeyFlags& operator|=(ModifierKeyFlags& p_left, ModifierKeyFlags p_right)
	{
		p_left = p_left | p_right;
		return p_left;
	}

	enum class MouseButton
	{
		None = 0,
		Left,
		Middle,
		Right,
		X0,
		X1
	};

	class MouseEvent
	{
	public:
		/*
			X coordinate of the mouse pointer.
		*/
		float x;
		/*
			Y coordinate of the mouse pointer.
		*/
		float y;
		/*
			The movement of the mouse pointer in the x-axis.
			If it is positive it has moved to the right and if it is negative it has moved to the left.
		*/
		float movementX;
		/*
			The movement of the mouse pointer in the y-axis.
			If it is positive it has moved down and if it is negative it has moved up.
		*/
		float movementY;
		/*
			How much the mouse wheel has been moved. 
			If it is positive, the wheel has been moved away from the user, if it negative it has moved towards the user. 
			It represents the number of ticks the wheel has been moved, but can be a fraction if the mouse has smooth scrolling.
		*/
		float scrollDelta;
		/*
			The mouse button that has been pressed, released or double clicked (depending on the mouse event).
		*/
		MouseButton mouseButton;
		/*
			The modifier keys and mouse buttons that were down when the event ocurred.
		*/
		ModifierKeyFlags modifierKeys;

		MouseEvent() :
			x(0.f), y(0.f), movementX(0.f), movementY(0.f), scrollDelta(0.f), 
			mouseButton(MouseButton::None), modifierKeys(ModifierKeyFlags::None)
		{ }
	};

	/*
		This can be inherited by any class. 
		Remember to register it to the GUI by calling the addGlobalMouseListener() method on it.
		A GlobalMouseListener will recieve mouse events as long as the window is focused.
	*/
	class GlobalMouseListener
	{
	public:
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been pressed down while the mouse cursor is inside the window.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleGlobalMouseDown(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been released after having been pressed down when the mouse pointer was inside the window.
			The mouse cursor may have left the window during the time the button is pressed, but it will still recieve the event.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleGlobalMouseUp(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when a mouse button has been double clicked while the mouse pointer is inside the window.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleGlobalMouseDoubleClick(MouseEvent const& p_event) { }

		/*
			USER IMPLEMENTED
			Gets called when the mouse has been moved while the mouse cursor is inside the window.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleGlobalMouseMove(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when the mouse wheel has been scrolled while the mouse pointer is inside the window.
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleGlobalMouseScroll(MouseEvent const& p_event) { }
		/*
			USER IMPLEMENTED
			Gets called when the cursor has been moved from the GUI.
		*/
		virtual void handleGlobalMouseLeave(MouseEvent const& p_event) { }
	};

	//------------------------------

	enum class KeyboardKey
	{
		None = 0,
		Backspace,
		Clear,
		Tab,
		Return,
		Enter = Return,
		Shift, ShiftLeft, ShiftRight,
		Control, ControlLeft, ControlRight,
		MenuLeft, MenuRight,
		Alt,
		CapsLock,
		Escape,
		Spacebar,
		PageUp, PageDown, Home, End,
		PrintScreen,
		Insert,
		Delete,
		Pause, Play,
		Help,
		Separator,
		Left, Right, Up, Down,
		NumLock,
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		Add, Subtract, Multiply, Divide, Decimal,
		Number0, Number1, Number2, Number3, Number4, Number5, Number6, Number7, Number8, Number9,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
		PreviousTrack, NextTrack, PlayPauseTrack, StopTrack,
		Comma, Period, Minus, Plus,

		// These keys vary by country/region.
		Regional1, Regional2, Regional3, Regional4, Regional5, Regional6, Regional7, Regional8
	};

	class KeyboardEvent
	{
	public:
		/*
			The character that was pressed. This is only valid for character press events.
		*/
		char character;
		/*
			The keyboard key that was pressed or released. This is not valid for character press events.
		*/
		KeyboardKey key;
		/*
			If this is true, this character/key press event is generated after the initial attack because the key is being held down.
		*/
		bool isRepeated;

		KeyboardEvent() :
			character(0), key(KeyboardKey::None), isRepeated(false)
		{ }
	};

	class KeyboardListener
	{
	public:
		/*
			This method is called when a character key has been pressed. 
			Only p_event.character and p_event.isRepeated are valid for this event type.
		*/
		virtual void handleCharacterInput(KeyboardEvent const& p_event) { }
		/*
			This method is called when a keyboard key has been pressed.
			Only p_event.key and p_event.isRepeated are valid for this event type.
		*/
		virtual void handleKeyboardKeyDown(KeyboardEvent const& p_event) { }
		/*
			This method is called when a keyboard key has been released.
			Only p_event.key is valid for this event type.
		*/
		virtual void handleKeyboardKeyUp(KeyboardEvent const& p_event) { }
		/*
			Gets called when another keyboard event listener becomes the target of keyboard events.
		*/
		virtual void handleKeyboardFocusLose() { }
		/*
			Gets called when this keyboard event listener becomes the target of keyboard events.
		*/
		virtual void handleKeyboardFocusGain() { }
	};

	//------------------------------

	enum class WindowStyleFlags
	{
		None = 0x0UL, // Borderless window.
		Border = 0x1UL,
		Invisible = 0x2UL, // Makes the window invisible at first. You can make it visible afterwards.
		Minimized = 0x8UL,
		Maximized = 0x10UL,
		MinimizeButton = 0x20UL,
		MaximizeButton = 0x40UL,
		ResizeBorder = 0x80UL,
		CustomBorder = 0x100UL, // This makes the client area take up the full window, and the GUI determines which areas are for resizing and moving the window.
		DefaultCustom = CustomBorder | MaximizeButton | MinimizeButton,
		Default = Border | MinimizeButton | MaximizeButton | ResizeBorder,
		DefaultNoResize = Border | MinimizeButton
	};

	constexpr WindowStyleFlags operator&(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32)p_left & (uint32)p_right);
	}
	constexpr WindowStyleFlags operator|(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32)p_left | (uint32)p_right);
	}
	constexpr WindowStyleFlags& operator|=(WindowStyleFlags& p_left, WindowStyleFlags p_right)
	{
		p_left = p_left | p_right;
		return p_left;
	}

	enum class WindowBorderArea
	{
		None = 0, // The area of the window is not part of the window border, meaning any mouse events are handled only by the GUI.
		TopLeftResize,
		TopResize,
		TopRightResize,
		LeftResize,
		RightResize,
		BottomLeftResize,
		BottomResize,
		BottomRightResize,
		Dragging // The area of the window is used for dragging the window, normally the title bar.
	};

	enum class WindowState
	{
		Minimized,
		Maximized,
		Restored
	};

	enum class ClipboardDataType
	{
		String, // Could be both 8-bit or 16-bit string.
		Unknown
	};

	class Gui;

	/*
		An abstract window, which has an OS-specific implementation. 
		The window is responsible for recieving events from the OS and sending them to the GUI. 
		It's like a portal between your application and the operating system. 
		It is only intended to be created by a GUI, and you can access and use it from there.
	*/
	class Window : public ReferenceCounted
	{
	public:
		/*
			Creates the window. To close it, use close().
		
			p_title is the text that appears in the title bar of the window (if it has a border).

			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.

			p_positionFactorY is the vertical equivalent to p_positionFactorX.

			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual void create(char const* p_title, float p_positionFactorX, float p_positionFactorY, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) = 0;
		/*
			Creates the window in the center of the screen. To close it, use close().
		
			p_title is the text that appears in the title bar of the window (if it has a border).
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_styleFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent window, which this window would appear above.
		*/
		virtual void create(char const* p_title, float p_width, float p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, Window* p_parent = 0) = 0;

		/*
			Closes the window, if the GUI associated with the window allows it.
			The animation thread will then safely destroy the window when it has terminated.
		*/
		virtual void close() = 0;
		/*
			Returns whether the OS window has been created and exists.
		*/
		virtual bool getIsOpen() const = 0;

		//------------------------------

		/*
			Usually used for the parent of a popup window when the popup is closed.
			Makes the window recieve mouse and keyboard events again.
		*/
		virtual void enableUserInteraction() = 0;
		/*
			Usually used for the parent of a popup window when the popup is opened.
			Makes the window not recieve any mouse and keyboard events, until enableUserInteraction is called.
		*/
		virtual void disableUserInteraction() = 0;

		//------------------------------

		/*
			Sets the text shown in the titlebar.
		*/
		virtual void setTitle(char const* p_title) = 0;
		/*
			Returns the text shown in the titlebar.
		*/
		virtual std::string getTitle() const = 0;

		//------------------------------

		/*
			Changes the styles that determine how the window is drawn by the OS. 
			These are set when the window is created, and you can change them afterwards here.
		*/
		virtual void setStyles(WindowStyleFlags p_styles) = 0;
		/*
			Returns the current styles that determine how the window is drawn by the OS.
		*/
		virtual WindowStyleFlags getStyles() const = 0;

		//------------------------------

		/*
			Returns the OS-specific window object associated with this window.
		*/
		virtual void* getNativeHandle() const = 0;

		//------------------------------

		/*
			Changes whether the client area of the window fills the whole screen.
		*/
		virtual void setIsFullscreen(bool p_isFullscreen) = 0;
		/*
			Switches between fullscreen and windowed mode. 
			If the window is currently windowed, it will become fullscreen, and the other way around.
		*/
		virtual void switchFullscreen() = 0;
		/*
			Returns whether the client area of the window fills the whole screen.
		*/
		virtual bool getIsFullscreen() const = 0;

		//------------------------------

		/*
			Makes the window invisible and disabled.
		*/
		virtual void hide() = 0;
		/*
			Shows the window and enables it.
		*/
		virtual void show() = 0;

		/*
			Makes the size of the window as big as possible so that the border still shows.
		*/
		virtual void maximize() = 0;
		/*
			Hides the window temporarily in a taskbar where it can be restored again.
		*/
		virtual void minimize() = 0;
		/*
			Shows/opens the window as it was before it was minimized.
		*/
		virtual void restore() = 0;

		/*
			Changes the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area 
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
			Methods maximize(), minimize() and restore() do the same thing.
		*/
		virtual void setState(WindowState p_state) = 0;
		/*
			Returns the window state, which determines how the window is viewed; hidden in the taskbar, maximized so it fills the client area 
			of the screen, or restored which is the default window state where the window can overlap other windows and be resized normally.
		*/
		virtual WindowState getState() const = 0;

		//------------------------------

		/*
			Sets the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual void setPosition(Point<int32> const& p_position) = 0;
		/*
			Sets the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual void setPosition(int32 p_x, int32 p_y) = 0;
		/*
			Returns the position of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual Point<int32> const& getPosition() const = 0;
		/*
			Returns the position of the left edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual int32 getPositionX() const = 0;
		/*
			Returns the position of the top edge of the window relative to the top-left corner of the screen, in pixel units.
		*/
		virtual int32 getPositionY() const = 0;

		/*
			Sets the size of the client area of the window, in dip units.
		*/
		virtual void setSize(Point<float> const& p_size) = 0;
		/*
			Sets the size of the client area of the window, in dip units.
		*/
		virtual void setSize(float p_width, float p_height) = 0;
		/*
			Returns the size of the client area of the window, in dip units.
		*/
		virtual Point<float> const& getSize() const = 0;
		/*
			Returns the width of the client area of the window, in dip units.
		*/
		virtual float getWidth() const = 0;
		/*
			Returns the height of the client area of the window, in dip units.
		*/
		virtual float getHeight() const = 0;

		/*
			Sets the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMinSize(Point<float> const& p_minSize) = 0;
		/*
			Sets the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMinSize(float p_minWidth, float p_minHeight) = 0;
		/*
			Returns the smallest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual Point<float> getMinSize() const = 0;
		/*
			Returns the smallest allowed width for the window when the user is resizing it, in dip units.
		*/
		virtual float getMinWidth() const = 0;
		/*
			Returns the smallest allowed height for the window when the user is resizing it, in dip units.
		*/
		virtual float getMinHeight() const = 0;

		/*
			Sets the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMaxSize(Point<float> const& p_maxSize) = 0;
		/*
			Sets the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual void setMaxSize(float p_maxWidth, float p_maxHeight) = 0;
		/*
			Returns the biggest allowed size for the window when the user is resizing it, in dip units.
		*/
		virtual Point<float> getMaxSize() const = 0;
		/*
			Returns the biggest allowed width for the window when the user is resizing it, in dip units.
		*/
		virtual float getMaxWidth() const = 0;
		/*
			Returns the biggest allowed height for the window when the user is resizing it, in dip units.
		*/
		virtual float getMaxHeight() const = 0;

		//------------------------------

		/*
			Returns the bounds of the current monitor used by the window, in pixel units.
		*/
		virtual Rectangle<uint32> getMonitorBounds() const = 0;
		/*
			Returns the virtual position of the current monitor used by the window relative to other monitors, in pixel units.
		*/
		virtual Point<uint32> getMonitorPosition() const = 0;
		/*
			Returns the size of the current monitor used by the window, in pixel units.
		*/
		virtual Point<uint32> getMonitorSize() const = 0;
		/*
			Returns the width of the current monitor used by the window, in pixel units.
		*/
		virtual uint32 getMonitorWidth() const = 0;
		/*
			Returns the height of the current monitor used by the window, in pixel units.
		*/
		virtual uint32 getMonitorHeight() const = 0;

		//------------------------------

		/*
			Returns the bounds of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Rectangle<uint32> getWorkAreaBounds() const = 0;
		/*
			Returns the virtual position of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Point<uint32> getWorkAreaPosition() const = 0;
		/*
			Returns the size of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual Point<uint32> getWorkAreaSize() const = 0;
		/*
			Returns the width of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual uint32 getWorkAreaWidth() const = 0;
		/*
			Returns the height of the work area of the monitor currently used by the window, in pixel units.
			This excludes the taskbar on Windows.
		*/
		virtual uint32 getWorkAreaHeight() const = 0;

		//------------------------------

		/*
			Returns whether a key is currently pressed down.
		*/
		virtual bool getIsKeyDown(KeyboardKey p_key) const = 0;
		/*
			Returns whether a mouse button is currently pressed down.
		*/
		virtual bool getIsMouseButtonDown(MouseButton p_button) const = 0;
		/*
			Returns the position of the mouse cursor, relative to the top-left corner of the window.
		*/
		virtual Point<float> getMousePosition() const = 0;

		//------------------------------

		/*
			Changes what the mouse cursor looks like.
		*/
		virtual void setCursor(Cursor p_cursor) = 0;
		/*
			Returns the current mouse cursor.
		*/
		virtual Cursor getCursor() const = 0;

		//------------------------------

		/*
			Gives a wide string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual void setClipboardWideString(std::wstring const& p_string) = 0;
		/*
			Gives a wide string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
			p_length is the number of characters in the string. If it is -1 then it assumes the string is null-terminated.
		*/
		virtual void setClipboardWideString(wchar_t const* p_string, int32 p_length = -1) = 0;

		/*
			Gives a string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
		*/
		virtual void setClipboardString(std::string const& p_string) = 0;
		/*
			Gives a string for the OS to store globally. Other programs, or this one, can then access it.
			The data currently stored on the clipboard is freed and replaced by this string.
			p_length is the number of characters in the string. If it is -1 then it assumes the string is null-terminated.
		*/
		virtual void setClipboardString(char const* p_string, int32 p_length = -1) = 0;

		/*
			Returns the 16-bit string which is currently stored on the OS clipboard, if there is any. 
			Otherwhise the returned string is empty.
		*/
		virtual std::wstring getClipboardWideString() const = 0;
		/*
			Returns the 8-bit string which is currently stored on the OS clipboard, if there is any. 
			Otherwhise the returned string is empty.
		*/
		virtual std::string getClipboardString() const = 0;
		
		/*
			Returns the main type of the current data that is on the OS clipboard.
		*/
		virtual ClipboardDataType getClipboardDataType() const = 0;
	};
#pragma endregion

	//------------------------------

#pragma region Image
	/*
		This specifies what is done to fit the image within its bounds.
	*/
	enum class ImageBoundsSizing
	{
		Stretch, // This stretches the image so that it fills its bounds.
		Contain, // This makes sure the image is as big as possible while still keeping the image within its bounds. Aspect ratio is kept.
		Fill // This makes sure the image is so big that it fills its bounds while keeping aspect ratio. Edges may be clipped.
	};

	/*
		This specifies how the pixels of an image are interpolated when it is scaled.
	*/
	enum class ImageScalingMethod
	{
		Pixelated, // Uses nearest neighbor interpolation
		Smooth // Uses linear interpolation
	};

	/*
		Represents an image on the GPU which can be created and drawn by a DrawingContext. 
		Notice that this is not a view but should be treated as a drawable object.
		It is your responsibility to manage its lifetime, using remember() and forget().
	*/
	class Image : public ReferenceCounted, public ProtectedRectangle
	{
	public:
		/*
			Sets a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual void setCropRectangle(Rectangle<float> const& p_rectangle) = 0;
		/*
			Returns a rectangle representing the portion of the image that will be drawn, relative to the top-left corner of the image.
			This is in original image DIP coordinates, meaning sizing is not taken into account.
		*/
		virtual Rectangle<float> const& getCropRectangle() const = 0;

		/*
			Returns the DIP size of the actual image.
		*/
		virtual Point<uint32> getOriginalSize() const = 0;
		/*
			Returns the DIP width of the actual image.
		*/
		virtual uint32 getOriginalWidth() const = 0;
		/*
			Returns the DIP height of the actual image.
		*/
		virtual uint32 getOriginalHeight() const = 0;

		//------------------------------

		/*
			Sets the way the image is fit within its bounds.
		*/
		virtual void setBoundsSizing(ImageBoundsSizing p_sizeMode) = 0;
		/*
			Returns the way the image is fit within its bounds.
		*/
		virtual ImageBoundsSizing getBoundsSizing() const = 0;

		/*
			Sets the way the image is positioned within its bounds.
		
			p_x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds. 
			p_x is expressed as a factor of the width of the image. For example, if p_x is 1, the right edge of the image will be 
			aligned with the right edge of the bounds. 0.5 means the centers will be aligned. 
			Same for p_y but vertical coordinates.
		*/
		virtual void setBoundsPositioning(float p_x, float p_y) = 0;
		/*
			Sets the way the image is positioned within its bounds on the x-axis.
		
			p_x represents the x coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_x is expressed as a factor of the width of the image. For example, if p_x is 1, the right edge of the image will be
			aligned with the right edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual void setBoundsPositioningX(float p_x) = 0;
		/*
			Sets the way the image is positioned within its bounds on the y-axis.
		
			p_y represents the y coordinate of the point on the image that aligns with the same point but relative to the bounds.
			p_y is expressed as a factor of the height of the image. For example, if p_y is 1, the bottom edge of the image will be
			aligned with the bottom edge of the bounds. 0.5 means the centers will be aligned.
		*/
		virtual void setBoundsPositioningY(float p_y) = 0;
		/*
			Returns the way the image is positioned within its bounds. See setBoundsPositioning for more info.
		*/
		virtual Point<float> const& getBoundsPositioning() const = 0;
		/*
			Returns the way the image is positioned within its bounds on the x-axis. See setBoundsPositioningX for more info.
		*/
		virtual float getBoundsPositioningX() const = 0;
		/*
			Returns the way the image is positioned within its bounds on the y-axis. See setBoundsPositioningY for more info.
		*/
		virtual float getBoundsPositioningY() const = 0;

		//------------------------------

		/*
			Sets how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual void setScalingMethod(ImageScalingMethod p_scalingMethod) = 0;
		/*
			Returns how the pixels of the image are interpolated when the image is scaled.
		*/
		virtual ImageScalingMethod getScalingMethod() const = 0;

		//------------------------------

		/*
			Sets how opaque the image is being drawn.
		*/
		virtual void setOpacity(float p_opacity) = 0;
		/*
			Returns how opaque the image is being drawn.
		*/
		virtual float getOpacity() const = 0;

		//------------------------------

		/*
			Returns the drawn width of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual float getInnerWidth() const = 0;
		/*
			Returns the drawn height of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual float getInnerHeight() const = 0;
		/*
			Returns the drawn size of the image within the bounds, calculated using the sizing options and the crop rectangle.
		*/
		virtual Point<float> getInnerSize() const = 0;
		/*
			Returns the drawn inner bounds of the image within the outer bounds, calculated using the positioning options, sizing options and the crop rectangle.
		*/
		virtual Rectangle<float> getInnerBounds() const = 0;

		//------------------------------

		/*
			Returns a pointer to the os-specific image object.
		*/
		virtual void* getHandle() const = 0;
	};
#pragma endregion

	//------------------------------

#pragma region Text
	enum class WordWrapping
	{
		Emergency, // Keeps words whole unless a word is wider than the maximum width.
		WholeWord, // Only wraps between words to prevent overflow.
		Always, // Always wraps to the next line to prevent overflow.
		Never // Allows overflow, never wraps.
	};

	enum class FontWeight
	{
		Thin = 100, // Thinnest option
		UltraLight = 200,
		Light = 300,
		SemiLight = 350,
		Regular = 400, // Normal thickness
		Medium = 500, // A bit thicker than regular
		SemiBold = 600,
		Bold = 700,
		UltraBold = 800,
		Black = 900, // Second most thick option
		UltraBlack = 950 // Most thick option
	};

	enum class FontStyle
	{
		Normal,
		Oblique,
		Italic
	};

	/*
		The horizontal stretch of the font.
		Only some fonts have different horizontal stretch options.
	*/
	enum class FontStretch
	{
		Undefined = 0,
		UltraCondensed = 1, // Most condensed
		ExtraCondensed = 2, // Second most condensed
		Condensed = 3,
		SemiCondensed = 4,
		Medium = 5,
		SemiStretched = 6,
		Stretched = 7,
		ExtraStretched = 8, // Second most stretched
		UltraStretched = 9 // Most stretched
	};

	enum class TextAlign
	{
		Left,
		Center,
		Right,
		Fill // Stretches the spaces of the text to make the left and right edges of the text line up with the bounds of the text.
	};
	enum class ReadingDirection
	{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop
	};

	/*
		Represents a text block which can be calculated once and drawn any number of times by a DrawingContext. 
		Notice that this is not a view, but should be treated as a drawable object created by a DrawingContext.
		It is your reponsibility to mange its lifetime, using remember() and forget().
	*/
	class Text : public ProtectedRectangle, public ReferenceCounted
	{
	public:
		/*
			Sets the rules for inserting line breaks in the text to avoid overflow.
		*/
		virtual void setWordWrapping(WordWrapping p_wordWrapping) = 0;
		/*
			Returns the type of rules used for inserting line breaks in the text to avoid overflow.
		*/
		virtual WordWrapping getWordWrapping() = 0;

		/*
			Sets the size of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual void fitSizeToText() = 0;
		/*
			Sets the width of the bounding box to fit the text.
		*/
		virtual void fitWidthToText() = 0;
		/*
			Sets the height of the bounding box to fit the text.
			There may still be space between the tallest character in the text and the top edge of the bounds.
			If you want the text to be positioned without any space on the top, call setIsTopTrimmed(true) before this.
		*/
		virtual void fitHeightToText() = 0;
		/*
			Returns the smallest size to contain the actual text.
			If getIsTopTrimmed() == false, the height includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual Point<float> getMinimumSize() = 0;
		/*
			Returns the smallest width to contain the actual text.
		*/
		virtual float getMinimumWidth() = 0;
		/*
			Returns the smallest height to contain the actual text.
			If getIsTopTrimmed() == false, this includes the space between the top of the tallest character
			and the top edge of the bounds.
		*/
		virtual float getMinimumHeight() = 0;

		//------------------------------

		/*
			Sets whether the top of the text is trimmed so that there is no space between the top of the tallest
			character of the text and the top edge of the bounds.

			If this is false, the text is positioned within the bounds so that the baseline is at a fixed position,
			and there may be space above the characters in the text to allow this. This is the default.

			Setting this to true can be useful when you want to perfectly center text vertically.
		*/
		virtual void setIsTopTrimmed(bool p_isTopTrimmed) = 0;
		/*
			Returns whether the top of the text is trimmed so that there is no space between the top of the tallest 
			character of the text and the top edge of the bounds. This is false by default.
		*/
		virtual bool getIsTopTrimmed() = 0;

		//------------------------------

		/*
			Returns the 2d position of a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position returned is relative to the origin of the drawing context. 
			If not, it is relative to the bounds of the text.
		*/
		virtual Point<float> getCharacterPosition(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the width and height of a character in the text, specified by its index in the string.
		*/
		virtual Point<float> getCharacterSize(uint32 p_characterIndex) = 0;
		/*
			Returns a rectangle enclosing a character in the text, specified by its index in the string.
			p_isRelativeToOrigin is whether the position of the bounds returned is relative to the origin of the drawing context. 
			If not, it is relative to the bounds of the text.
		*/
		virtual Rectangle<float> getCharacterBounds(uint32 p_characterIndex, bool p_isRelativeToOrigin = false) = 0;
		
		/*
			Returns the index of the character which is nearest to a point.
			p_isRelativeToOrigin is whether the position given is relative to the origin of the drawing context. 
			If not, it is relative to the bounds of the text.
		*/
		virtual uint32 getNearestCharacterIndex(Point<float> const& p_point, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the index of the character which is nearest to a point.
		
			p_isRelativeToOrigin is whether the position given is relative to the origin of the drawing context. 
			If not, it is relative to the bounds of the text.
		*/
		virtual uint32 getNearestCharacterIndex(float p_pointX, float p_pointY, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the index and position of the character which is nearest to a point.
		
			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterPosition is a pointer to the 2d position to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. 
			If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndPosition(Point<float> const& p_point, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the index and position of the character which is nearest to a point.
		
			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterPosition is a pointer to the 2d position to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. 
			If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndPosition(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Point<float>* p_outCharacterPosition, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the index and bounds of the character which is nearest to a point.
		
			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterBounds is a pointer to the bounding rectangle to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndBounds(Point<float> const& p_point, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) = 0;
		/*
			Returns the index and bounds of the character which is nearest to a point.
		
			p_outCharacterIndex is a pointer to the character index to be returned.
			p_outCharacterBounds is a pointer to the bounding rectangle to be returned.
			p_isRelativeToOrigin is whether the input and output points are relative to the origin of the drawing context. If not, they are relative to the bounds of the text.
		*/
		virtual void getNearestCharacterIndexAndBounds(float p_pointX, float p_pointY, uint32* p_outCharacterIndex, Rectangle<float>* p_outCharacterBounds, bool p_isRelativeToOrigin = false) = 0;

		//------------------------------

		/*
			Sets how the text is placed within the bounds.
		*/
		virtual void setTextAlign(TextAlign p_textAlign) = 0;
		/*
			Returns how the text is placed within the bounds.
		*/
		virtual TextAlign getTextAlign() = 0;

		//------------------------------

		/*
			Sets the layout direction of the text.
		*/
		virtual void setReadingDirection(ReadingDirection p_readingDirection) = 0;
		/*
			Returns the layout direction of the text.
		*/
		virtual ReadingDirection getReadingDirection() = 0;

		//------------------------------

		/*
			Sets the font family to be used in a section of the text.
	
			p_name is the name of the font family.

			p_startPosition is the position of the first character to use this font.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this font.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontFamily(char const* p_name, int32 p_startPosition = 0, int32 p_length = 0) = 0;

		//------------------------------

		/*
			Sets the spacing between characters in a section of the text.
		
			p_startPosition is the position of the first character to use this spacing.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this spacing.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/	
		virtual void setCharacterSpacing(float p_characterSpacing, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Sets the leading and trailing spacing of the characters in a section of the text.
		
			p_leading is the spacing before the characters of the text.
			p_trailing is the spacing after the characters of the text.
			p_startPosition is the position of the first character to use this spacing.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this spacing.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/	
		virtual void setCharacterSpacing(float p_leading, float p_trailing, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Returns the spacing before one of the characters.
		*/
		virtual float getLeadingCharacterSpacing(int32 p_characterIndex = 0) = 0;
		/*
			Returns the spacing after one of the characters.
		*/
		virtual float getTrailingCharacterSpacing(int32 p_characterIndex = 0) = 0;

		//------------------------------

		/*
			Sets the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual void setLineHeight(float p_lineHeight) = 0;
		/*
			Returns the distance between the baseline of lines in the text, as a factor of the default.
		*/
		virtual float getLineHeight() = 0;

		//------------------------------

		/*
			Sets the thickness of characters in a section of the text.
		
			p_startPosition is the position of the first character to use this font weight.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this font weight.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/	
		virtual void setFontWeight(FontWeight p_fontWeight, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Returns the weight/thickness of a character in the text.
		*/
		virtual FontWeight getFontWeight(uint32 p_characterPosition = 0) = 0;

		//------------------------------

		/*
			Sets the font style in a section of the text.
		
			p_startPosition is the position of the first character to use this font style.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this font style.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/	
		virtual void setFontStyle(FontStyle p_fontStyle, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Returns the style of a character in the text.
		*/
		virtual FontStyle getFontStyle(uint32 p_characterPosition = 0) = 0;

		//------------------------------

		/*
			Sets the font stretch in a section of the text. Not all fonts support this.
		
			p_startPosition is the position of the first character to use this font stretch.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this font stretch.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/	
		virtual void setFontStretch(FontStretch p_fontStretch, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Returns the font stretch of a character in the text.
		*/
		virtual FontStretch getFontStretch(uint32 p_characterPosition = 0) = 0;

		//------------------------------

		/*
			Sets the font size in a section of the text.
		
			p_startPosition is the position of the first character to use this font size.
			If this is negative, it is relative to the end of the text.
			
			p_length is the number of characters to use this font size.
			If this is negative, it goes to the left of the start position.
			If it is 0, everything after the starting position will be affected.
		*/
		virtual void setFontSize(float p_fontSize, int32 p_startPosition = 0, int32 p_length = 0) = 0;
		/*
			Returns the size (height) of a character in the text.
		*/
		virtual float getFontSize(uint32 p_characterPosition = 0) = 0;

		//------------------------------

		virtual std::string const& getString() = 0;

		//------------------------------

		/*
			Returns a pointer to an OS-specific text object.
		*/
		virtual void* getHandle() = 0;
	};
#pragma endregion

	//------------------------------

#pragma region Drawing context
	enum class LineCap
	{
		Flat,
		Round,
		Square,
		Triangle
	};

	enum class LineJoin
	{
		Bevel,
		Miter,
		Round
	};

	enum class LineDashStyle
	{
		Solid,
		Dash,
		Dot,
		DashDot,
		DashDotDot,
		Custom
	};

	class TextProperties
	{
	public:
		char const* fontFamilyName = "Roboto";

		FontWeight fontWeight = FontWeight::Medium;
		FontStyle fontStyle = FontStyle::Normal;
		FontStretch fontStretch = FontStretch::Medium;
		TextAlign textAlign = TextAlign::Left;
		ReadingDirection readingDirection = ReadingDirection::LeftToRight;
		
		float characterSpacing = 0.f; // Only supported for text objects.
		float lineHeight = 1.f;
		float fontSize = 22.f;
	};

	class LinearGradient : 
		public ReferenceCounted
	{
	public:
		/*
			Sets an offset in the start and end positions.
		*/
		virtual void setOffset(Point<float> const& p_offset) = 0;
		/*
			Sets an offset in the start and end positions.
		*/
		virtual void setOffset(float p_x, float p_y) = 0;
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual void setOffsetX(float p_x) = 0;
		/*
			Sets the vertical offset in the start position.
		*/
		virtual void setOffsetY(float p_y) = 0;

		/*
			Returns the offset in the start and end positions.
		*/
		virtual Point<float> const& getOffset() const = 0;
		/*
			Returns the horizontal offset in the start and end positions.
		*/
		virtual float getOffsetX() const = 0;
		/*
			Returns the vertical offset in the start and end positions.
		*/
		virtual float getOffsetY() const = 0;

		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual void setStartPosition(Point<float> const& p_startPosition) = 0;
		/*
			Sets the coordinates relative to the origin where the gradient will start.
		*/
		virtual void setStartPosition(float p_x, float p_y) = 0;
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual Point<float> const& getStartPosition() const = 0;
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionX() const = 0;
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionY() const = 0;

		/*
			Sets the coordinates relative to the origin where the gradient will end.
		*/
		virtual void setEndPosition(Point<float> const& p_endPosition) = 0;
		/*
			Sets the coordinates relative to the origin where the gradient will end.
		*/
		virtual void setEndPosition(float p_x, float p_y) = 0;
		/*
			Returns the coordinates relative to the origin where the gradient will end.
		*/
		virtual Point<float> const& getEndPosition() const = 0;
		/*
			Returns the X coordinate relative to the origin where the gradient will end.
		*/
		virtual float getEndPositionX() const = 0;
		/*
			Returns the Y coordinate relative to the origin where the gradient will end.
		*/
		virtual float getEndPositionY() const = 0;
	};
	class RadialGradient : 
		public ReferenceCounted
	{
		/*
			Sets an offset in the start position.
		*/
		virtual void setOffset(Point<float> const& p_offset) = 0;
		/*
			Sets an offset in the start position.
		*/
		virtual void setOffset(float p_x, float p_y) = 0;
		/*
			Sets the horizontal offset in the start position.
		*/
		virtual void setOffsetX(float p_x) = 0;
		/*
			Sets the vertical offset in the start position.
		*/
		virtual void setOffsetY(float p_y) = 0;
		/*
			Returns the offset in the start position.
		*/
		virtual Point<float> const& getOffset() const = 0;
		/*
			Returns the horizontal offset in the start position.
		*/
		virtual float getOffsetX() const = 0;
		/*
			Returns the vertical offset in the start position.
		*/
		virtual float getOffsetY() const = 0;

		/*
			Sets the coordinates where the gradient will start, relative to the origin.
		*/
		virtual void setStartPosition(Point<float> const& p_startPosition) = 0;
		/*
			Sets the coordinates relative to the origin where the gradient will start.
		*/
		virtual void setStartPosition(float p_x, float p_y) = 0;
		/*
			Returns the coordinates relative to the origin where the gradient will start.
		*/
		virtual Point<float> const& getStartPosition() const = 0;
		/*
			Returns the X coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionX() const = 0;
		/*
			Returns the Y coordinate relative to the origin where the gradient will start.
		*/
		virtual float getStartPositionY() const = 0;

		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(float p_radius) = 0;
		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(Point<float> const& p_radius) = 0;
		/*
			Sets the horizontal and vertical size of the gradient.
		*/
		virtual void setRadius(float p_radiusX, float p_radiusY) = 0;
		/*
			Returns the horizontal and vertical size of the gradient.
		*/
		virtual Point<float> const& getRadius() const = 0;
		/*
			Returns the horizontal size of the gradient.
		*/
		virtual float getRadiusX() const = 0;
		/*
			Returns the vertical size of the gradient.
		*/
		virtual float getRadiusY() const = 0;
	};

	/*
		A class representing a position and a color, used to make a gradient.
		The position between 0 and 1 and is relative to the start and end positions if it's linear,
		and relative to the start position and radius if it's radial.
	*/
	class GradientStop
	{
	public:
		AvoGUI::Color color;
		float position;

		GradientStop(AvoGUI::Color const& p_color, float p_position) :
			color(p_color), position(p_position)
		{
		}
	};

	/*
		Platform-specific interface for cached geometry that can be created and drawn by a DrawingContext.
		Used to accelerate drawing.
	*/
	class Geometry : public ReferenceCounted { };

	/*
		An abstract drawing context, created by a GUI to be used to create objects 
		like text and images as well as to draw graphics in views.
	*/
	class DrawingContext : public ReferenceCounted
	{
	protected:
		TextProperties m_textProperties;

	public:
		/*
			Initializes drawing. The GUI calls this for you.
		*/
		virtual void beginDrawing() = 0;
		/*
			Finishes the drawing and shows it. The GUI calls this for you.
		*/
		virtual void finishDrawing(std::vector<Rectangle<float>> const& p_updatedRectangles) = 0;

		//------------------------------

		/*
			Sets whether the target is fullscreen or windowed.
		*/
		virtual void setIsFullscreen(bool p_isFullscreen) = 0;
		/*
			Switches between windowed and fullscreen mode. 
			If it is currently windowed, it switches to fullscreen, and the other way around.
		*/
		virtual void switchFullscreen() = 0;
		/*
			Returns whether the target is fullscreen or windowed.
		*/
		virtual bool getIsFullscreen() = 0;

		//------------------------------

		/*
			Enables synchronization with the monitor.
		*/
		virtual void enableVsync() = 0;
		/*
			Disables synchronization with the monitor.
		*/
		virtual void disableVsync() = 0;
		/*
			Returns whether presentation is synchronized with the monitor.
		*/
		virtual bool getIsVsyncEnabled() = 0;

		//------------------------------

		/*
			Sets the color that the target is filled with before any drawing.
		*/
		virtual void setBackgroundColor(Color const& p_color) = 0;
		/*
			Returns the color that the target is filled with before any drawing.
		*/
		virtual Color getBackgroundColor() = 0;

		//------------------------------

		/*
			Converts a number of physical pixels to the corresponding number of device independent pixels (DIP). Everything you draw is by default in DIPs.
		*/
		virtual float convertPixelsToDeviceIndependentPixels(float p_pixels) = 0;
		/*
			Converts a number of device independent pixels to the corresponding number of physical pixels (DIP). Everything you draw is by default in DIPs.
		*/
		virtual float convertDeviceIndependentPixelsToPixels(float p_deviceIndependentPixels) = 0;

		//------------------------------

		/*
			Moves the screen position of the coordinate (0, 0).
		*/
		virtual void moveOrigin(Point<float> const& p_offset) = 0;
		/*
			Moves the screen position of the coordinate (0, 0).
		*/
		virtual void moveOrigin(float p_offsetX, float p_offsetY) = 0;
		/*
			Sets the screen position of the coordinate (0, 0).
		*/
		virtual void setOrigin(Point<float> const& p_origin) = 0;
		/*
			Sets the screen position of the coordinate (0, 0).
		*/
		virtual void setOrigin(float p_x, float p_y) = 0;
		/*
			Returns the screen position of the coordinate (0, 0).
		*/
		virtual Point<float> getOrigin() = 0;

		//------------------------------

		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin.
		*/
		virtual void scale(float p_scale) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin.
		*/
		virtual void scale(float p_scaleX, float p_scaleY) = 0;
		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scale, Point<float> const& p_origin) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual void scale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) = 0;
		/*
			Multiplies the size factor, which will be transforming future graphics drawing so that it is bigger or smaller.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scale, float p_originX, float p_originY) = 0;
		/*
			Multiplies the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. The origin is shifted so that everything is scaled towards the origin
			parameter, which is relative to the top-left corner of the window.
		*/
		virtual void scale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) = 0;
		/*
			sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin.
		*/
		virtual void setScale(float p_scale) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller than normal. Everything will be scaled towards the origin.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY) = 0;
		/*
			Sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scale, Point<float> const& p_origin) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics drawing so that
			it is bigger or smaller than normal. Everything will be scaled towards the origin parameter, which is relative
			to the top-left corner of the window.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY, Point<float> const& p_origin) = 0;
		/*
			Sets the size factor, which will be transforming future graphics drawing so that it is bigger or smaller than normal.
			Everything will be scaled towards the origin parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scale, float p_originX, float p_originY) = 0;
		/*
			Sets the size factor independently for the x-axis and y-axis, which will be transforming future graphics
			drawing so that it is bigger or smaller. The origin is shifted so that everything is scaled towards the origin
			parameter, which is relative to the top-left corner of the window.
		*/
		virtual void setScale(float p_scaleX, float p_scaleY, float p_originX, float p_originY) = 0;
		/*
			Returns the sizing factor which is transforming graphics drawing so that it is bigger or smaller. 
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual Point<float> const& getScale() = 0;
		/*
			Returns the sizing factor for the x-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual float getScaleX() = 0;
		/*
			Returns the sizing factor for the y-axis which is transforming graphics drawing so that it is bigger or smaller.
			If it is 2, graphics is drawn double as big as normal. 0.5 is half as big as normal.
		*/
		virtual float getScaleY() = 0;

		//------------------------------

		/*
			Rotates all future graphics drawing, with an angle in radians. Graphics will be rotated relative to the origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise.
		*/
		virtual void rotate(float p_radians) = 0;
		/*
			Rotates all future graphics drawing, with an angle in radians. 
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise.
		*/
		virtual void rotate(float p_radians, Point<float> const& p_origin) = 0;
		/*
			Rotates all future graphics drawing, with an angle in radians. 
			Graphics will be rotated relative to the origin parameter, which itself is relative to the current origin.
			p_radians is the angle to rotate, in radians.
			Positive angle is clockwise and negative is anticlockwise.
		*/
		virtual void rotate(float p_radians, float p_originX, float p_originY) = 0;

		//------------------------------

		/*
			Resets all graphics drawing transformations, so that every coordinate used in any drawing operation is unaltered, and relative to the top-left corner of the target.
		*/
		virtual void resetTransformations() = 0;

		//------------------------------

		/*
			Resizes the drawing buffers. The GUI calls this for you when it is being resized.
		*/
		virtual void setSize(Point<uint32> const& p_size) = 0;
		/*
			Resizes the drawing buffers. The GUI calls this for you when it is being resized.
		*/
		virtual void setSize(uint32 p_width, uint32 p_height) = 0;
		/*
			Returns the size of the drawing buffers.
		*/
		virtual Point<uint32> getSize() = 0;

		//------------------------------

		/*
			Clears the whole buffer with the specified color.
		*/
		virtual void clear(Color const& p_color) = 0;
		/*
			Clears the whole buffer with a transparent background.
		*/
		virtual void clear() = 0;

		//------------------------------

		/*
			Draws a filled rectangle using the current color or gradient. 
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle) = 0;
		/*
			Draws a filled rectangle using the current color or gradient. 
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle using the current color or gradient. 
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			Draws a filled rectangle at the origin using the current color or gradient. 
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle at the origin using the current color or gradient. 
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_width, float p_height) = 0;

		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rectangle with custom corners at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color or gradient.
			Change color being used with method setColor or gradient with setGradientBrush.
		*/
		virtual void fillRoundedRectangle(float p_width, float p_height, float p_radius) = 0;

		//------------------------------

		/*
			Draws a rectangle outline using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline at the origin using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a filled circle using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		
			p_position is the center position of the circle.
		*/
		virtual void fillCircle(Point<float> const& p_position, float p_radius) = 0;
		/*
			Draws a filled circle using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		
			p_x is the horizontal center position of the circle.
			p_y is the vertical center position of the circle.
		*/
		virtual void fillCircle(float p_x, float p_y, float p_radius) = 0;

		/*
			Draws a circle outline using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		
			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(Point<float> const& p_position, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a circle outline using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		
			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a straight line between two points using the current color or gradient. 
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void drawLine(Point<float> const& p_point_0, Point<float> const& p_point_1, float p_thickness = 1.f) = 0;
		/*
			Draws a straight line between two points using the current color or gradient.
			Change the color being used with the method setColor or the gradient with setGradientBrush.
		*/
		virtual void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) = 0;

		//------------------------------

		/*
			Draws the edge of a custom shape.
		
			p_vertices is a vector of the points that make up the shape.
			p_lineThickness is how thicc the edges of the shape are.
			p_isClosed is whether the last vertex will be connected to the first one to close the shape.
		*/
		virtual void strokeShape(std::vector<Point<float>> const& p_vertices, float p_lineThickness, bool p_isClosed = false) = 0;
		/*
			Draws the edge of a custom shape.
		
			p_vertices is an array of points that make up the shape.
			p_numberOfVertices is the number of points that make up the shape.
			p_lineThickness is how thicc the edges of the shape are.
			p_isClosed is whether the last vertex will be connected to the first one to close the shape.
		*/
		virtual void strokeShape(Point<float> const* p_vertices, uint32 p_numberOfVertices, float p_lineThickness, bool p_isClosed = false) = 0;
		/*
			Fills a custom shape with the current color or gradient.

			p_shape is a vector of points that make up the shape.
		*/
		virtual void fillShape(std::vector<Point<float>> const& p_vertices) = 0;
		/*
			Fills a custom shape with the current color or gradient.
		
			p_vertices is an array of points that make up the shape.
			p_numberOfVertices is he number of points that make up the shape.
		*/
		virtual void fillShape(Point<float> const* p_vertices, uint32 p_numberOfVertices) = 0;

		//------------------------------

		/*
			Draws a stroked cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the stroke color with setColor().
		*/
		virtual void strokeGeometry(Geometry* p_geometry, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a filled cached geometry with its coordinates relative to the origin.
			If you want to move the geometry, use moveOrigin().
			If you want to scale the geometry, use scale().
			You can also change the fill color with setColor().
		*/
		virtual void fillGeometry(Geometry* p_geometry) = 0;

		//------------------------------

		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createRoundedRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createRoundedRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, float p_radius) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createRoundedRectangleGeometry(Rectangle<float> const& p_rectangle, float p_radius) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createRoundedRectangleGeometry(float p_width, float p_height, float p_radius) = 0;
		/*
			Creates a Geometry object which represents a rounded rectangle at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createRoundedRectangleGeometry(Point<float> const& p_size, float p_radius) = 0;

		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createCornerRectangleGeometry(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createCornerRectangleGeometry(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_corners) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createCornerRectangleGeometry(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createCornerRectangleGeometry(float p_width, float p_height, RectangleCorners const& p_corners) = 0;
		/*
			Creates a Geometry object which represents a rectangle with custom corners at the origin.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createCornerRectangleGeometry(Point<float> const& p_size, RectangleCorners const& p_corners) = 0;

		//------------------------------

		/*
			Creates a geometry object which represents a polygon.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createPolygonGeometry(std::vector<Point<float>> const& p_vertices) = 0;
		/*
			Creates a geometry object which represents a polygon.
			The Geometry object can be cached and allows for faster drawing.
		*/
		virtual Geometry* createPolygonGeometry(Point<float> const* p_vertices, uint32 p_numberOfVertices) = 0;

		//------------------------------

		/*
			Changes the way both start- and endpoints of lines are drawn.
		*/
		virtual void setLineCap(LineCap p_lineCap) = 0;
		/*
			Changes the way startpoints of lines are drawn.
		*/
		virtual void setStartLineCap(LineCap p_lineCap) = 0;
		/*
			Changes the way endpoints of lines are drawn.
		*/
		virtual void setEndLineCap(LineCap p_lineCap) = 0;
		/*
			Returns the way startpoints of lines are drawn.
		*/
		virtual LineCap getStartLineCap() = 0;
		/*
			Returns the way endpoints of lines are drawn.
		*/
		virtual LineCap getEndLineCap() = 0;

		//------------------------------

		/*
			Sets how and if lines are dashed/dotted.
		*/
		virtual void setLineDashStyle(LineDashStyle p_dashStyle) = 0;
		/*
			Returns how and if lines are dashed/dotted.
		*/
		virtual LineDashStyle getLineDashStyle() = 0;

		/*
			Sets the offset of line dashing/dotting.
		*/
		virtual void setLineDashOffset(float p_dashOffset) = 0;
		/*
			Returns the offset of line dashing/dotting.
		*/
		virtual float getLineDashOffset() = 0;

		/*
			This changes the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual void setLineDashCap(LineCap p_dashCap) = 0;
		/*
			Returns the way the endpoints of dots and dashes on lines are drawn.
		*/
		virtual LineCap getLineDashCap() = 0;

		//------------------------------

		/*
			Sets the way line joints are drawn.
		*/
		virtual void setLineJoin(LineJoin p_lineJoin) = 0;
		/*
			Returns the way line joints are drawn.
		*/
		virtual LineJoin getLineJoin() = 0;

		/*
			Sets the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual void setLineJoinMiterLimit(float p_miterLimit) = 0;
		/*
			Returns the lower limit of the thickness of pointy "mitered" joints.
		*/
		virtual float getLineJoinMiterLimit() = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the cached geometry will be invisible, on pixel level.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipGeometry(Geometry* p_geometry, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipShape(std::vector<Point<float>> const& p_points, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipShape(Point<float> const* p_points, uint32 p_numberOfPoints, float p_opacity = 1.f) = 0;

		/*
			This removes the last added clipping shape.
			Use this when the last pushed clip shape was not a regular axis-aligned rectangle.
		*/
		virtual void popClipShape() = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipRectangle to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size, float p_opacity = 1.f) = 0;

		/*
			This removes the last added clipping rectangle.
			Only use this when the last pushed clip was a regular axis-aligned rectangle, without custom corners.
		*/
		virtual void popClipRectangle() = 0;

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed clip corner rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size, RectangleCorners const& p_corners, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_opacity = 1.f) = 0;
		/*
			After calling this, all graphics drawn outside a rounded rectangle at the origin with the given size and radius will be invisible, on pixel level.
			p_size is the size of the rounded clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed rounded clip rectangle.
			The alpha of the clipped content will be multiplied by p_opacity.
		*/
		virtual void pushRoundedClipRectangle(Point<float> const& p_size, float p_radius, float p_opacity = 1.f) = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rectangle.
		
			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRectangleShadowImage(Point<uint32> const& p_size, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rectangle.
		
			p_width is the width of the rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRectangleShadowImage(uint32 p_width, uint32 p_height, float p_blur, Color const& p_color) = 0;

		/*
			Generates an image of a shadow that is cast by a rectangle with custom corners.

			p_size is the size of the rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRectangleShadowImage(Point<uint32> const& p_size, RectangleCorners const& p_corners, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rectangle with custom corners.

			p_width is the width of the rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_blur is how far away from the surface the rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRectangleShadowImage(uint32 p_width, uint32 p_height, RectangleCorners const& p_corners, float p_blur, Color const& p_color) = 0;

		//------------------------------

		/*
			Generates an image of a shadow that is cast by a rounded rectangle.
		
			p_size is the size of the rounded rectangle which will cast the shadow. The shadow will have bigger dimensions than this if p_blur > 0.
			p_radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
			p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRoundedRectangleShadowImage(Point<uint32> const& p_size, float p_radius, float p_blur, Color const& p_color) = 0;
		/*
			Generates an image of a shadow that is cast by a rounded rectangle.
		
			p_width is the width of the rounded rectangle which will cast the shadow. The shadow will be wider than this if p_blur > 0.
			p_height is the height of the rounded rectangle which will cast the shadow. The shadow will be taller than this if p_blur > 0.
			p_radius is the corner radius ("roundness") of the rounded rectangle which will cast the shadow.
			p_blur is how far away from the surface the rounded rectangle is (how blurry the shadow is).
			p_color is the color of the resulting shadow.
		*/
		virtual Image* createRoundedRectangleShadowImage(uint32 p_width, uint32 p_height, float p_radius, float p_blur, Color const& p_color) = 0;

		//------------------------------

		/*
			Loads an image from pixel data in BGRA format.

			p_pixelData is an array which is 4*width*height bytes in size. 
			It contains the color values for every pixel in the image, row-by-row. One byte for every color channel.

			p_width and p_height are in pixels.
		*/
		virtual Image* createImage(void const* p_pixelData, uint32 p_width, uint32 p_height) = 0;
		/*
			Loads an image from the data of an image file.
			p_imageData is a memory block which is p_size bytes in size.
		*/
		virtual Image* createImage(void const* p_imageData, uint32 p_size) = 0;
		/*
			Loads an image from a file. Most standard image formats/codecs are supported.
			p_filePath is the path, relative or absolute, to the image file to be loaded.
			If this returns 0, then the file path is probably incorrect.
		*/
		virtual Image* createImage(char const* p_filePath) = 0;
		/*
			Creates an image from an OS-specific handle.

			On Windows, it is an HICON or HBITMAP.
		*/
		virtual Image* createImage(void* p_handle) = 0;
		/*
			Draws an image, placed according to the image's bounds and positioning/scaling options.
		*/
		virtual void drawImage(Image* p_image, float p_multiplicativeOpacity = 1.f) = 0;

		//------------------------------

		/*
			Creates a linear gradient that can be used as a brush when drawing things.
		*/
		virtual LinearGradient* createLinearGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX = 0.f, float p_startY = 0.f, float p_endX = 0.f, float p_endY = 0.f) = 0;
		/*
			Creates a linear gradient that can be used as a brush when drawing things.
		*/
		virtual LinearGradient* createLinearGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_endPosition) = 0;

		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX = 0.f, float p_startY = 0.f, float p_radiusX = 0.f, float p_radiusY = 0.f) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, float p_startX, float p_startY, float p_radius) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radiusX, float p_radiusY) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, float p_radius) = 0;
		/*
			Creates a radial gradient that can be used as a brush when drawing things.
		*/
		virtual RadialGradient* createRadialGradient(std::vector<GradientStop> const& p_gradientStops, Point<float> const& p_startPosition, Point<float> const& p_radius) = 0;

		/*
			Sets a linear gradient to be used as the brush when drawing things.
		*/
		virtual void setGradient(LinearGradient* p_gradient) = 0;
		/*
			Sets a radial gradient to be used as the brush when drawing things.
		*/
		virtual void setGradient(RadialGradient* p_gradient) = 0;
		/*
			Sets a color to be used when drawing things.
		*/
		virtual void setColor(Color const& p_color) = 0;

		/*
			Sets the transparency of all graphics that will be drawn.
		*/
		virtual void setOpacity(float p_opacity) = 0;

		//------------------------------

		/*
			Adds a new font family that can be used by text.
			p_data is the data that would be in a standard font file.
			p_dataSize is the length/size of the data in bytes.
		*/
		virtual void addFont(void const* p_data, uint32 p_dataSize) = 0;

		//------------------------------

		/*
			Sets the default properties of text created with this drawing context. 
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual void setDefaultTextProperties(TextProperties const& p_textProperties) = 0;
		/*
			Returns the default properties of text created with this drawing context. 
			These properties can be overridden by changing the properties of a text object.
		*/
		virtual TextProperties getDefaultTextProperties() = 0;

		//------------------------------

		/*
			Creates a new Text object which represents a pre-calculated text layout, using the current text properties.
			p_bounds is the maximum bounds of the text. If it's (0, 0, 0, 0) then the bounds will be calculated to fit the text.
		*/
		virtual Text* createText(char const* p_string, float p_fontSize, Rectangle<float> const& p_bounds = Rectangle<float>()) = 0;
		/*
			Draws pre-calculated text created with the createText method.
		*/
		virtual void drawText(Text* p_text) = 0;

		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText).
		*/
		virtual void drawText(char const* p_string, Rectangle<float> const& p_rectangle) = 0;
		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText()).
		*/
		virtual void drawText(char const* p_string, float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			Lays out and draws a string in a rectangle.
			If you're drawing the same text repeatedly, use a Text object (created with method createText()).
		*/
		virtual void drawText(char const* p_string, Point<float> const& p_position, Point<float> const& p_size) = 0;
		/*
			Lays out and draws a string at a position.
			If you're drawing the same text repeatedly, use a Text object (created with createText()).
		*/
		virtual void drawText(char const* p_string, float p_x, float p_y) = 0;
		/*
			Lays out and draws a string at a position.
			If you're drawing the same text repeatedly, use a Text object (created with createText()).
		*/
		virtual void drawText(char const* p_string, Point<float> const& p_position) = 0;
	};
#pragma endregion

	//------------------------------

	/*
		The highest, "root" view in the view hierarchy.
		It is connected to a window which it holds and recieves events from.
		When the window has been closed and destroyed, forget() is called on the GUI.
	*/
	class Gui : public View, public WindowListener, public GlobalMouseListener
	{
	private:
		Gui* m_parent;
		Window* m_window;
		DrawingContext* m_drawingContext;

		std::vector<WindowListener*> m_windowEventListeners;

		//------------------------------

		Point<uint32> m_lastWindowSize;
		Point<uint32> m_newWindowSize;
		bool m_hasNewWindowSize;
		std::deque<View*> m_animationUpdateQueue;

		std::mutex m_invalidRectanglesMutex;
		std::vector<Rectangle<float>> m_invalidRectangles;

		std::recursive_mutex m_animationThreadMutex;
		bool m_hasAnimationLoopStarted;
		bool m_willClose;

		//------------------------------

		std::vector<GlobalMouseListener*> m_globalMouseEventListeners;
		std::vector<View*> m_pressedMouseEventListeners;
		Point<float> m_mouseDownPosition;

		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given, as well as any overlay views which are above the non-overlay view.
		*/
		void getTopMouseListenersAt(Point<float> const& p_coordinates, std::vector<View*>& p_result);
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given, as well as any overlay views which are above the non-overlay view.
		*/
		void getTopMouseListenersAt(float p_x, float p_y, std::vector<View*>& p_result);

		//------------------------------

		std::vector<KeyboardListener*> m_globalKeyboardEventListeners;
		KeyboardListener* m_keyboardFocus;

		//------------------------------

		std::unordered_map<uint64, View*> m_viewsById;
		friend class View;

		//------------------------------

		void sendBoundsChangeEvents(AvoGUI::Rectangle<float> const& p_previousBounds) override
		{
			if ((uint32)getWidth() != m_lastWindowSize.x || (uint32)getHeight() != m_lastWindowSize.y)
			{
				m_window->setSize(getSize());
			}
			else
			{
				View::sendBoundsChangeEvents(p_previousBounds);
			}
		}

		std::thread m_animationThread;
		void thread_runAnimationLoop();

	public:
		Gui();
		~Gui();

		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to AvoGUI::GUI::createContent will be made when these objects have been created.
			After that, an initial call to AvoGUI::GUI::handleSizeChange will also be made.
		
			waitForFinish or detachFromParent must be called after creation and before the main thread returns.

			p_title is the text that appears in the title bar of the window (if it has an OS border).
			
			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge 
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.
			
			p_positionFactorY is the vertical equivalent to p_positionFactorX.
			
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(char const* p_title, float p_positionFactorX, float p_positionFactorY, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = 0);
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to AvoGUI::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to AvoGUI::GUI::handleSizeChange will also be made.

			waitForFinish or detachFromParent must be called after creation and before the main thread returns.
		
			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(char const* p_title, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, Gui* p_parent = 0);

		/*
			LIBRARY IMPLEMENTED
			This waits for all threads to finish, and returns when the window has been closed.
			It also forgets the GUI afterwards.
			This should be used on the main thread since all threads are often terminated by the OS when the main thread exits.
		*/
		void waitForFinish()
		{
			// The GUI is forgotten from the animation thread (to allow for cleanup when it's detached),
			// but the join call needs to return before the GUI is destroyed, hence the remember() and forget() here.
			remember();
			m_animationThread.join();
			forget();
		}
		/*
			LIBRARY IMPLEMENTED
			This detaches the GUI from the creator thread, so that it continues to run independently.
			The GUI is forgotten automatically when the window has closed.
			This is recommended to be used for child GUIs like popup windows and such, since the parent thread needs to continue running 
			and can't wait for the child GUI to finish.
		*/
		void detachFromParent()
		{
			m_animationThread.detach();
		}

		/*
			Returns the GUI that owns the parent window of the window of this GUI.
			If the window does not have a parent, it returns 0.
		*/
		Gui* getParent()
		{
			return m_parent;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether the GUI and its window is awaiting being closed by the animation/drawing thread.
		*/
		bool getWillClose()
		{
			return m_willClose;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given.
		*/
		View* getViewAt(Point<float> const& p_coordinates);
		/*
			LIBRARY IMPLEMENTED
			Returns the topmost non-overlay view which contains the coordinates given.
		*/
		View* getViewAt(float p_x, float p_y);
		/*
			LIBRARY IMPLEMENTED
			Finds the view that has a certain ID previously set by you.
		*/
		template<typename T>
		View* getViewById(T p_id)
		{
			return m_viewsById[(uint64)p_id];
		}
		/*
			LIBRARY IMPLEMENTED
			Finds the view that has a certain ID previously set by you.
		*/
		template<typename T, typename U>
		T* getViewById(U p_id)
		{
			return (T*)m_viewsById[(uint64)p_id];
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Creates a new drawing context and calls createContent() to initialize the layout, as well as sends the event down to all window listeners.
		*/
		void handleWindowCreate(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners and returns whether the window will close.
		*/
		bool handleWindowClose(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners.
		*/
		void handleWindowMinimize(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners.
		*/
		void handleWindowMaximize(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners.
		*/
		void handleWindowRestore(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Resizes the buffers held by the drawing context and updates the size of the GUI, as well as sends the event down to all window listeners.
		*/
		void handleWindowSizeChange(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners.
		*/
		void handleWindowFocus(WindowEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to all window listeners.
		*/
		void handleWindowUnfocus(WindowEvent const& p_event) override;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseDown(MouseEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseUp(MouseEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseDoubleClick(MouseEvent const& p_event) override;

		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseMove(MouseEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseLeave(MouseEvent const& p_event) override;
		/*
			LIBRARY IMPLEMENTED
			Sends the event down to global and targeted mouse event listeners.
		*/
		virtual void handleGlobalMouseScroll(MouseEvent const& p_event) override;

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			This is only called if the window of the GUI has the custom border style.
			It returns what part of the window border is under the given coordinates.

			The default implementation only handles the window resizing border areas, at the edges of the GUI.
			You can override this and either keep the default implementation and only add handling of window dragging areas
			(when the standard implementation returns WindowBorderArea::None), or detect all resize areas yourself too.
		*/
		virtual WindowBorderArea getWindowBorderAreaAtPosition(float p_x, float p_y)
		{
			float borderWidth = 5.f;
			float diagonalBorderWidth = 7.f;

			if (p_y < diagonalBorderWidth)
			{
				if (p_x < diagonalBorderWidth)
				{
					return WindowBorderArea::TopLeftResize;
				}
				if (p_x >= getWidth() - diagonalBorderWidth)
				{
					return WindowBorderArea::TopRightResize;
				}
				if (p_y < borderWidth)
				{
					return WindowBorderArea::TopResize;
				}
			}
			if (p_y >= getHeight() - diagonalBorderWidth)
			{
				if (p_x < diagonalBorderWidth)
				{
					return WindowBorderArea::BottomLeftResize;
				}
				if (p_x >= getWidth() - diagonalBorderWidth)
				{
					return WindowBorderArea::BottomRightResize;
				}
				if (p_y >= getHeight() - borderWidth)
				{
					return WindowBorderArea::BottomResize;
				}
			}
			if (p_x < borderWidth)
			{
				return WindowBorderArea::LeftResize;
			}
			if (p_x >= getWidth() - borderWidth)
			{
				return WindowBorderArea::RightResize;
			}
			return WindowBorderArea::None;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Sets the keyboard event listener that keyboard events are sent to.
		*/
		void setKeyboardFocus(KeyboardListener* p_keyboardFocus)
		{
			if (m_keyboardFocus == p_keyboardFocus)
			{
				return;
			}

			KeyboardListener* focusBefore = m_keyboardFocus;

			m_keyboardFocus = p_keyboardFocus;

			if (focusBefore)
			{
				focusBefore->handleKeyboardFocusLose();
			}

			if (p_keyboardFocus)
			{
				p_keyboardFocus->handleKeyboardFocusGain();
			}
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the keyboard event listener that keyboard events are sent to.
		*/
		KeyboardListener* getKeyboardFocus()
		{
			return m_keyboardFocus;
		}

		/*
			LIBRARY IMPLEMENTED
			Handles a character pressed event that has been sent directly from the window to the GUI. 
			If there are no global keyboard listeners, the event is only sent to the keyboard focus.
		*/
		virtual void handleCharacterInput(KeyboardEvent const& p_event);
		/*
			LIBRARY IMPLEMENTED
			Handles a key pressed event that has been sent directly from the window to the GUI. 
			If there are no global keyboard listeners, the event is only sent to the keyboard focus.
		*/
		virtual void handleKeyboardKeyDown(KeyboardEvent const& p_event);
		/*
			LIBRARY IMPLEMENTED
			Handles a key release event that has been sent directly from the window to the GUI. 
			If there are no global keyboard listeners, the event is only sent to the keyboard focus.
		*/
		virtual void handleKeyboardKeyUp(KeyboardEvent const& p_event);

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Enables a window event listener to recieve events.
		*/
		void addWindowListener(WindowListener* p_listener)
		{
			m_windowEventListeners.push_back(p_listener);
		}
		/*
			LIBRARY IMPLEMENTED
			Disables a window event listener to recieve events.
		*/
		void removeWindowListener(WindowListener* p_listener)
		{
			removeVectorElementWithoutKeepingOrder(m_windowEventListeners, p_listener);
		}
		/*
			LIBRARY IMPLEMENTED
			Enables a keyboard event listener to recieve events even if it is not the keyboard focus.
		*/
		void addGlobalKeyboardListener(KeyboardListener* p_listener)
		{
			m_globalKeyboardEventListeners.push_back(p_listener);
		}
		/*
			LIBRARY IMPLEMENTED
			Disables a keyboard event listener to recieve events when it is not the keyboard focus.
		*/
		void removeGlobalKeyboardListener(KeyboardListener* p_listener)
		{
			removeVectorElementWithoutKeepingOrder(m_globalKeyboardEventListeners, p_listener);
		}
		/*
			LIBRARY IMPLEMENTED
			Enables a global mouse event listener to recieve events.
		*/
		void addGlobalMouseListener(GlobalMouseListener* p_listener)
		{
			m_globalMouseEventListeners.push_back(p_listener);
		}
		/*
			LIBRARY IMPLEMENTED
			Disables a global mouse event listener to recieve events.
		*/
		void removeGlobalMouseListener(GlobalMouseListener* p_listener)
		{
			removeVectorElementWithoutKeepingOrder(m_globalMouseEventListeners, p_listener);
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the window used by this GUI.
		*/
		Window* getWindow()
		{
			return m_window;
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a pointer to the drawing context used by this GUI.
		*/
		DrawingContext* getDrawingContext()
		{
			return m_drawingContext;
		}

		//------------------------------

		/*
			USER IMPLEMENTED
			This is called after the window and drawing context have been created. 
			It is a good idea to initialize your GUI in this method, but do the layout in handleSizeChange() - it is called right after creation too.
		*/
		virtual void createContent() { };

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Returns whether the window has been resized since the last GUI size update. Used internally.
		*/
		bool getHasNewWindowSize()
		{
			return m_hasNewWindowSize;
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Adds a view to the animation update queue. Views that are in the animation update queue will be updated after a certain interval.
			Do not use this method, because it is possible to add a view twice to the queue. Instead use queueAnimationUpdate() on the view.
			Using that method esures that animations are only updated max once per interval for every view.
		*/
		void queueAnimationUpdateForView(View* p_view);
		/*
			LIBRARY IMPLEMENTED
			Should only need to be used internally. 
			Updates the animations of views which have been requested to get an animation update.
			Also resizes the GUI if the window has changed size.
		*/
		void updateQueuedAnimations();

		/*
			LIBRARY IMPLEMENTED
			This locks the animation thread mutex, so that the critical section in the animation thread does not run until the mutex is unlocked again (or the other way around).
		*/
		void excludeAnimationThread()
		{
			m_animationThreadMutex.lock();
		}
		/*
			LIBRARY IMPLEMENTED
			This unlocks the animation thread mutex, so that the critical section in the animation thread is allowed to run.
		*/
		void includeAnimationThread()
		{
			m_animationThreadMutex.unlock();
		}

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			Invalidates a part of the GUI that has been changed, and therefore needs to be redrawn. 
			Views that intersect with any invalid rectangles will be drawn in the next call to drawViews() (which is made internally) automatically.
		*/
		void invalidateRectangle(Rectangle<float> p_rectangle);
		/*
			LIBRARY IMPLEMENTED
			Invalidates a part of the GUI that has been changed, and therefore needs to be redrawn.
			Views that intersect with any invalid rectangles will be drawn in the next call to drawViews() (which is made internally) automatically.
		*/
		void invalidateRectangle(float p_left, float p_top, float p_right, float p_bottom)
		{
			invalidateRectangle(Rectangle<float>(p_left, p_top, p_right, p_bottom));
		}
		/*
			LIBRARY IMPLEMENTED
			Draws the invalid rectangles of the GUI. This method should only be called internally by the library. Use draw() to draw things directly on the GUI.
		*/
		void drawViews();
	};

	//------------------------------

	/*
		Shows a short info message about a view. 
		The parent of a tooltip is the GUI.
	*/
	class Tooltip : public View
	{
	private:
		Text* m_text;
		float m_opacityAnimationTime;
		float m_opacity;
		bool m_isShowing;
		uint32 m_timeSinceShow;

	public:
		Tooltip(View* p_parent) : View(p_parent), m_text(0), m_opacityAnimationTime(0.f), m_opacity(0.f), m_isShowing(false), m_timeSinceShow(0U)
		{
			setHasShadow(false);
			setElevation(-1.f);
			setCornerRadius(2.f);
			setIsOverlay(true); // Don't want to block any events from reaching views below the tooltip, especially not when it has faded away.
		}
		~Tooltip()
		{
			if (m_text)
			{
				m_text->forget();
			}
		}

		//------------------------------

		/*
			Makes the tooltip appear
			p_string is the string to be displayed on the tooltip.
			p_targetBounds is the area that the tooltip points to and is relative to the parent of this tooltip. The tooltip decides the exact positioning.
		*/
		virtual void show(char const* p_string, Rectangle<float> const& p_targetRectangle)
		{
			if (!m_isShowing)
			{
				if (!m_text || p_string != m_text->getString())
				{
					if (m_text)
					{
						m_text->forget();
					}
					m_text = getGui()->getDrawingContext()->createText(p_string, getThemeValue("tooltip font size"));
					m_text->fitSizeToText();
					setSize(m_text->getWidth() + 1.6 * getThemeValue("tooltip font size"), m_text->getHeight() + getThemeValue("tooltip font size") * 1.f);
					m_text->setCenter(getWidth() * 0.5f, getHeight() * 0.5f);
				}

				if (p_targetRectangle.bottom + 7.f + getHeight() >= getGui()->getHeight())
				{
					setBottom(max(1.f, p_targetRectangle.top - 7.f), true);
				}
				else
				{
					setTop(p_targetRectangle.bottom + 7.f, true);
				}
				setCenterX(max(1.f + getWidth() * 0.5f, min(getGui()->getWidth() - getWidth() * 0.5f - 1.f, p_targetRectangle.getCenterX())));

				m_opacityAnimationTime = 0.f;
				m_opacity = 0.f;
				m_isShowing = true;
				m_timeSinceShow = 0U;
				queueAnimationUpdate();
			}
		}

		/*
			Makes the tooltip disappear.
		*/
		virtual void hide()
		{
			if (m_isShowing)
			{
				m_isShowing = false;
				queueAnimationUpdate();
			}
		}

		//------------------------------

		virtual void updateAnimations() override
		{
			if (m_isShowing)
			{
				if (m_timeSinceShow > 6U)
				{
					m_opacity = getThemeEasing("out").easeValue(m_opacityAnimationTime);
					if (m_opacity < 1.f)
					{
						m_opacityAnimationTime = min(m_opacityAnimationTime + 0.08f, 1.f);
						queueAnimationUpdate();
					}
				}
				else
				{
					m_timeSinceShow++;
					queueAnimationUpdate();
				}
			}
			else
			{
				m_opacity = getThemeEasing("in out").easeValue(m_opacityAnimationTime);
				if (m_opacity > 0.f)
				{
					m_opacityAnimationTime = max(m_opacityAnimationTime - 0.2f, 0.f);
					queueAnimationUpdate();
				}
			}

			invalidate();
		}

		virtual void draw(DrawingContext* p_drawingContext) override
		{
			if (m_text)
			{
				p_drawingContext->scale(m_opacity * 0.3f + 0.7f, getAbsoluteCenter());
				p_drawingContext->setColor(Color(m_theme->colors["tooltip background"], m_opacity));
				p_drawingContext->fillRectangle(getSize());
				p_drawingContext->setColor(Color(m_theme->colors["tooltip on background"], m_opacity));
				p_drawingContext->drawText(m_text);
				p_drawingContext->scale(1.f / (m_opacity * 0.3f + 0.7f), getAbsoluteCenter());
			}
		}
	};

	//------------------------------

	class OpenFileDialog
	{
	public:
		struct FileExtensionFilter
		{
			/*
				This is the name that will be shown for the file extension filter.
			*/
			char const* name;
			/*
				This is the file extension(s) that the user can open when this filter is selected.
				If you want more than 1 file extension for this file extension name, you can seperate the extensions with ";".
				Wildcards are used to specify what part of the file name is filtered.
				For example: "*.png;*.jpg"
			*/
			char const* extensions;
		};

	private:
		Gui* m_gui;

		bool m_canSelectMultipleFiles;
		std::vector<FileExtensionFilter> m_fileExtensions;
		char const* m_title;

	public:
		OpenFileDialog() :
			m_gui(0),
			m_canSelectMultipleFiles(false), m_title("Open file...")
		{ }
		OpenFileDialog(Gui* p_gui) :
			m_gui(p_gui),
			m_canSelectMultipleFiles(false), m_title("Open file...")
		{ }

		/*
			If this is true, the user can select more than 1 file to open.
		*/
		void setCanSelectMultipleFiles(bool p_canSelectMultipleFiles)
		{
			m_canSelectMultipleFiles = p_canSelectMultipleFiles;
		}
		bool getCanSelectMultipleFiles()
		{
			return m_canSelectMultipleFiles;
		}

		/*
			Sets the title shown in the top border of the open file dialog.
		*/
		void setTitle(char const* p_title)
		{
			m_title = p_title;
		}
		/*
			Returns the title shown in the thop border of the open file dialog.
		*/
		char const* getTitle()
		{
			return m_title;
		}

		/*
			Sets the file extensions of the files that the user can open with the dialog.
			See the properties of FileExtensionFilter for details.

			You can initialize the vector like this:
			
			{
				{ "Images", "*.jpg;*.png" }
				{ "Sound files", "*.mp3;*.wav;*.ogg" }
			}
		*/
		void setFileExtensions(std::vector<FileExtensionFilter>& p_fileExtensions)
		{
			m_fileExtensions = std::move(p_fileExtensions);
		}
		/*
			These are the file extensions of the files that the user can open with the dialog.
			See the properties of FileExtensionFilter for details.

			You can initialize the vector like this:

			{
				{ "Images", "*.jpg;*.png" }
				{ "Sound files", "*.mp3;*.wav;*.ogg" }
			}
		*/
		void setFileExtensions(std::vector<FileExtensionFilter>&& p_fileExtensions)
		{
			m_fileExtensions = p_fileExtensions;
		}
		/*
			Returns the file extension filters that can be selected in the dialog.
		*/
		std::vector<FileExtensionFilter> const& getFileExtensions()
		{
			return m_fileExtensions;
		}

		/*
			Opens the dialog and returns when the user has selected the files or closed the window.
			p_openedFilePaths is a vector which will be filled with file paths in UTF-8 format of the files the user has selected to open.
			It can be empty if the user closed the window without selecting any files.
		*/
		void open(std::vector<std::string>& p_openedFilePaths);
		/*
			Opens the dialog and returns when the user has selected the files or closed the window.
			p_openedFilePaths is a vector which will be filled with file paths in UTF-16 format of the files the user has selected to open.
			It can be empty if the user closed the window without selecting any files.
		*/
		void open(std::vector<std::wstring>& p_openedFilePaths);
	};

	//------------------------------

	/*
		A view that shows a ripple effect when you click it and optionally shows a hover effect when the mouse hovers over it.
		It is a mouse event overlay which means views behind this view are targeted as if this view didn't exist.
	*/
	class Ripple : public View, public ViewListener
	{
	private:
		Color m_color;

		bool m_isEnabled;

		//------------------------------

		Point<float> m_position;
		float m_maxSize;
		float m_size;
		float m_circleAnimationTime;

		//------------------------------

		float m_alphaFactor;
		float m_alphaAnimationTime;
		bool m_isMouseDown;

		//------------------------------

		float m_overlayAlphaFactor;
		float m_overlayAnimationTime;
		bool m_isMouseHovering;
		bool m_hasHoverEffect;

	public:
		Ripple(View* p_parent, Color const& p_color = Color(1.f, 0.45f)) :
			View(p_parent, p_parent->getBounds().createCopyAtOrigin()), m_color(p_color),
			m_isEnabled(true), m_maxSize(0.f), m_size(0.f), m_circleAnimationTime(1.f), m_alphaFactor(0.f),
			m_alphaAnimationTime(0.f), m_isMouseDown(false), m_overlayAlphaFactor(0.f), m_overlayAnimationTime(0.f),
			m_isMouseHovering(false), m_hasHoverEffect(true)
		{
			setIsOverlay(true); // Mouse events should be sent through
			setHasShadow(false);
			setElevation(FLT_MAX); // Nothing can be above a ripple...
			enableMouseEvents();
			p_parent->addViewListener(this);
		}
		~Ripple()
		{
		}

		//------------------------------

		/*
			Disables the ripple and hover effects.
		*/
		void disable()
		{
			m_isEnabled = false;
		}
		/*
			Enables the ripple and hover effects.
		*/
		void enable()
		{
			m_isEnabled = true;
		}
		/*
			Returns whether the ripple and hover effects are enabled.
		*/
		bool getIsEnabled()
		{
			return m_isEnabled;
		}

		//------------------------------

		/*
			Sets the color that is used by the ripple and hover effects.
		*/
		void setColor(Color const& p_color)
		{
			m_color = p_color;
		}
		/*
			Returns the color that is used by the ripple and hover effects.
		*/
		Color const& getColor()
		{
			return m_color;
		}

		//------------------------------

		/*
			Sets whether the view will be lightly highlighted when the mouse hovers over it.
			This is true by default and is recommended since it indicates that the view can be pressed.
		*/
		void setHasHoverEffect(bool p_hasHoverEffect)
		{
			m_hasHoverEffect = p_hasHoverEffect;
		}
		/*
			Returns whether the view will be lightly highlighted when the mouse hovers over it.
		*/
		bool getHasHoverEffect()
		{
			return m_hasHoverEffect;
		}

		//------------------------------

		void handleViewSizeChange(View* p_view, float p_previousWidth, float p_previousHeight) override
		{
			setSize(p_view->getSize());
			m_maxSize = 2.f * Point<>::getDistanceFast(m_position, Point<float>(m_position.x < getWidth() * 0.5 ? getWidth() : 0, m_position.y < getHeight() * 0.5 ? getHeight() : 0));
		}

		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (m_isEnabled && p_event.mouseButton == MouseButton::Left)
			{
				m_position.set(p_event.x - getLeft(), p_event.y - getTop());
				m_circleAnimationTime = 0.f;
				m_alphaFactor = 1.f;
				m_isMouseDown = true;

				m_maxSize = 2.f * Point<>::getDistanceFast(m_position, Point<float>(m_position.x < getWidth() * 0.5 ? getWidth() : 0, m_position.y < getHeight() * 0.5 ? getHeight() : 0));

				queueAnimationUpdate();
			}
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			if (m_isMouseDown && p_event.mouseButton == MouseButton::Left)
			{
				m_isMouseDown = false;
				m_alphaAnimationTime = 0.f;
				queueAnimationUpdate();
			}
		}
		void handleMouseBackgroundEnter(MouseEvent const& p_event) override
		{
			if (m_isEnabled)
			{
				View::handleMouseBackgroundEnter(p_event);
				m_isMouseHovering = true;
				queueAnimationUpdate();
			}
		}
		void handleMouseBackgroundLeave(MouseEvent const& p_event) override
		{
			if (m_isMouseHovering)
			{
				m_isMouseHovering = false;
				queueAnimationUpdate();
			}
		}

		void updateAnimations() override
		{
			if (m_hasHoverEffect)
			{
				m_overlayAlphaFactor = getThemeEasing("in out").easeValue(m_overlayAnimationTime);

				if (m_isMouseHovering)
				{
					if (m_overlayAlphaFactor < 1.f)
					{
						m_overlayAnimationTime = min(m_overlayAnimationTime + m_theme->values["hover animation speed"], 1.f);
						queueAnimationUpdate();
					}
				}
				else if (m_overlayAlphaFactor > 0.f)
				{
					m_overlayAnimationTime = max(m_overlayAnimationTime - m_theme->values["hover animation speed"], 0.f);
					queueAnimationUpdate();
				}
			}

			float circleAnimationValue = 1.f;
			if (m_circleAnimationTime < 1.f)
			{
				circleAnimationValue = getThemeEasing("ripple").easeValue(m_circleAnimationTime);
				m_circleAnimationTime += 0.05f;
				m_size = interpolate(m_maxSize * 0.4f, m_maxSize, circleAnimationValue);
			}

			if (m_isMouseDown)
			{
				if (circleAnimationValue < 1.f)
				{
					queueAnimationUpdate();
				}
			}
			else if (circleAnimationValue >= 1.f)
			{
				if (m_alphaAnimationTime < 1.f)
				{
					m_alphaFactor = 1.f - getThemeEasing("in out").easeValue(m_alphaAnimationTime);
					m_alphaAnimationTime = min(1.f, m_alphaAnimationTime + 0.05f);

					queueAnimationUpdate();
				}
			}
			else
			{
				queueAnimationUpdate();
			}

			invalidate();
		}

		//------------------------------

		void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
		{
			if (m_isEnabled)
			{
				p_drawingContext->setColor(Color(m_color, m_color.alpha * m_overlayAlphaFactor * 0.3f));
				p_drawingContext->fillRectangle(getSize());

				if (m_color.alpha * m_alphaFactor >= 0.f)
				{
					p_drawingContext->setColor(Color(m_color, m_color.alpha * m_alphaFactor * 0.8f));
					p_drawingContext->fillCircle(m_position, m_size * 0.5f);
				}
			}
		}
	};

	//------------------------------

	class Button;

	class ButtonListener
	{
	public:
		virtual void handleButtonClick(Button* p_button) { };
	};

	class Button : public View
	{
	public:
		enum class Emphasis
		{
			Low, // Text
			Medium, // Outlined
			High // Raised
		};

	private:
		Text* m_text;

		Tooltip* m_tooltipView;
		char const* m_tooltipString;

		Image* m_icon;

		float m_pressAnimationTime;
		bool m_isPressed;
		bool m_isRaising;
		Emphasis m_emphasis;

		bool m_isEnabled;
		Color m_currentColor;
		float m_colorAnimationTime;
		bool m_isAccent;

		bool m_isMouseHovering;

		Ripple* m_ripple;

		std::vector<ButtonListener*> m_buttonListeners;

		void updateSize()
		{
			if (m_text)
			{
				float sizeFactor = getThemeValue("button font size") / 14.f;
				if (m_icon)
				{
					m_icon->setSize(16.f*sizeFactor, 16.f*sizeFactor);
					m_icon->setCenter(sizeFactor*38.f * 0.5f, getHeight() * 0.5f);

					m_text->setLeft(38.f*sizeFactor);
					setSize(round(m_text->getWidth()) + sizeFactor*(16.f + 38.f), round(m_text->getHeight()) + 17.f * sizeFactor);
				}
				else
				{
					if (m_text->getWidth() >= 32.f*sizeFactor)
					{
						setSize(round(m_text->getWidth()) + 32.f*sizeFactor, round(m_text->getHeight()) + 17.f*sizeFactor);
					}
					else
					{
						setSize(64.f*sizeFactor, round(m_text->getHeight()) + 17.f*sizeFactor);
					}
					m_text->setCenter(getCenter() - getTopLeft());
				}
			}
			else if (m_icon)
			{
				m_icon->setCenter(getCenter() - getTopLeft());
			}
		}

	protected:
		void handleThemeValueChange(std::string const& p_name, float p_newValue) override
		{
			if (p_name == "button font size")
			{
				m_text->setFontSize(p_newValue);
				if (p_name == "button character spacing")
				{
					m_text->setCharacterSpacing(p_newValue);
				}
				updateSize();
			}
			else if (p_name == "button character spacing")
			{
				m_text->setCharacterSpacing(p_newValue);
				updateSize();
			}
		}
		void handleThemeColorChange(std::string const& p_name, Color const& p_newColor)
		{
			if (m_emphasis == Emphasis::High)
			{
				if (p_name == (m_isAccent ? "secondary" : "primary"))
				{
					m_currentColor = p_newColor;
				}
				else if (p_name == (m_isAccent ? "on secondary" : "on primary"))
				{
					m_currentColor = p_newColor;
				}
			}
			else if (p_name == (m_isAccent ? "secondary on background" : "primary on background"))
			{
				m_currentColor = p_newColor;
				m_ripple->setColor(p_newColor);
			}
		}

	public:
		Button(View* p_parent, char const* p_text = "", Emphasis p_emphasis = Emphasis::High, bool p_isAccent = false) :
			View(p_parent), m_text(0), m_tooltipView(0), m_tooltipString(""),
			m_icon(0), m_pressAnimationTime(1.f), m_emphasis(p_emphasis), m_isEnabled(true),
			m_colorAnimationTime(1.f)
		{
			setString(p_text);

			setCornerRadius(4.f);

			m_ripple = new Ripple(this);
			m_ripple->setCursor(Cursor::Hand);

			setIsAccent(p_isAccent);
			if (p_emphasis == Emphasis::High)
			{
				setElevation(2.f);
			}

			enableMouseEvents();
		}
		~Button()
		{
			if (m_text)
			{
				m_text->forget();
			}
			if (m_icon)
			{
				m_icon->forget();
			}
		}

		//------------------------------

		/*
			Registers a button listener to this button. The button listener will get an event when the button has been pressed.
		*/
		void addButtonListener(ButtonListener* p_buttonListener)
		{
			m_buttonListeners.push_back(p_buttonListener);
		}

		//------------------------------

		/*
			Makes the user unable to use the button and makes it gray.
		*/
		void disable()
		{
			if (m_isEnabled)
			{
				m_isEnabled = false;
				m_colorAnimationTime = 1.f;
				queueAnimationUpdate();

				m_ripple->disable();

				//if (m_isMouseHovering)
				//{
				//	getGui()->getWindow()->setCursor(Cursor::Arrow);
				//}
			}
		}

		/*
			Makes the user able to use the button.
		*/
		void enable()
		{
			if (!m_isEnabled)
			{
				m_isEnabled = true;
				m_colorAnimationTime = 0.f;
				queueAnimationUpdate();

				m_ripple->enable();

				//if (m_isMouseHovering)
				//{
				//	getGui()->getWindow()->setCursor(getCursor());
				//}
			}
		}

		/*
			Returns whether the user can use the button.
		*/
		bool getIsEnabled()
		{
			return m_isEnabled;
		}

		//------------------------------

		/*
			Sets whether the button uses the secondary/accent color. If not, it uses the primary color. The button uses primary color by default.
		*/
		void setIsAccent(bool p_isAccent)
		{
			m_isAccent = p_isAccent;
			if (m_emphasis == Emphasis::High)
			{
				m_currentColor = m_isAccent ? getThemeColor("secondary") : getThemeColor("primary");
				m_ripple->setColor(Color(m_isAccent ? getThemeColor("on secondary") : getThemeColor("on primary"), 0.3f));
			}
			else
			{
				m_currentColor = m_isAccent ? getThemeColor("secondary on background") : getThemeColor("primary on background");
				m_ripple->setColor(Color(m_isAccent ? getThemeColor("secondary on background") : getThemeColor("primary on background"), 0.3f));
			}
		}
		/*
			Returns whether the button uses the secondary/accent color. If not, it uses the primary color. The buton uses primary color by default.
		*/
		bool getIsAccent()
		{
			return m_isAccent;
		}

		//------------------------------

		/*
			Sets the string that the button displays.
		*/
		void setString(char const* p_string)
		{
			if (m_text)
			{
				m_text->forget();
			}
			if (p_string[0])
			{
				m_text = getGui()->getDrawingContext()->createText(p_string, getThemeValue("button font size"));
				m_text->setFontFamily(m_theme->fontFamilies["main"]);
				m_text->setWordWrapping(WordWrapping::Never);
				m_text->setCharacterSpacing(getThemeValue("button character spacing"));
				m_text->setFontWeight(FontWeight::Medium);
				m_text->fitSizeToText();
			}
			else 
			{
				m_text = 0;
			}
			updateSize();
		}

		/*
			Returns the string that the button displays.
		*/
		char const* getString()
		{
			return m_text->getString().c_str();
		}

		/*
			Returns the text object that is used to display the button label.
		*/
		Text* getText()
		{
			return m_text;
		}

		//------------------------------

		/*
			Sets an image to be shown together with the text. Unless you have remembered the image yourself, your are transferring ownership of the image to the button.
			It is best to keep a text label with the icon, unless it is very clear to all users what the button does with the icon alone, or if you have set a tooltip.
			If p_icon is 0, the icon is removed.
		*/
		void setIcon(Image* p_icon)
		{
			if (p_icon != m_icon)
			{
				if (m_icon)
				{
					m_icon->forget();
				}
				if (p_icon)
				{
					m_icon = p_icon;
					m_icon->setBoundsSizing(ImageBoundsSizing::Contain);
				}
				else
				{
					m_icon = 0;
				}
				updateSize();
				invalidate();
			}
		}

		/*
			Returns the image that is shown together with the button text.
		*/
		Image* getIcon()
		{
			return m_icon;
		}

		//------------------------------

		/*
			Sets a string to be shown as a tooltip when the mouse hovers over the button. 
			Should give the user additional information about the button's purpose.
			An empty string disables the tooltip.
		*/
		void setTooltip(Tooltip* p_tooltipView, char const* p_info)
		{
			m_tooltipView = p_tooltipView;
			m_tooltipString = p_info;
		}

		//------------------------------

		void handleMouseBackgroundEnter(MouseEvent const& p_event) override 
		{ 
			if (m_tooltipView && m_tooltipString != "")
			{
				m_tooltipView->show(m_tooltipString, getAbsoluteBounds());
			}
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			m_isMouseHovering = true;
		}
		void handleMouseBackgroundLeave(MouseEvent const& p_event) override
		{
			if (m_tooltipView && m_tooltipString != "")
			{
				m_tooltipView->hide();
			}
			m_isMouseHovering = false;
		}
		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (m_isEnabled && m_emphasis == Emphasis::High)
			{
				m_isPressed = true;
				m_isRaising = true;
				m_pressAnimationTime = 0.f;
				queueAnimationUpdate();
			}
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			if (m_emphasis == Emphasis::High)
			{
				m_isPressed = false;
				queueAnimationUpdate();
			}
			if (m_isEnabled && getIsContaining(p_event.x + getLeft(), p_event.y + getTop()))
			{
				for (uint32 a = 0; a < m_buttonListeners.size(); a++)
				{
					m_buttonListeners[a]->handleButtonClick(this);
				}
			}
		}

		//------------------------------

		void updateAnimations() override
		{
			if ((m_colorAnimationTime != 1.f && m_isEnabled) || (m_colorAnimationTime != 0.f && !m_isEnabled))
			{
				float colorAnimationValue = getThemeEasing("symmetrical in out").easeValue(m_colorAnimationTime);
				if (m_emphasis == Emphasis::High)
				{
					m_currentColor = m_isAccent ? m_theme->colors["secondary"] : m_theme->colors["primary"];
				}
				else
				{
					m_currentColor = m_isAccent ? m_theme->colors["secondary on background"] : m_theme->colors["primary on background"];
				}
				m_currentColor.setSaturationHSL(colorAnimationValue);

				if (m_isEnabled)
				{
					if (m_colorAnimationTime < 1.f)
					{
						m_colorAnimationTime = min(1.f, m_colorAnimationTime + 0.1f);
						queueAnimationUpdate();
					}
				}
				else
				{
					if (m_colorAnimationTime > 0.f)
					{
						m_colorAnimationTime = max(0.f, m_colorAnimationTime - 0.1f);
						queueAnimationUpdate();
					}
				}
			}

			if (m_emphasis == Emphasis::High)
			{
				float pressAnimationValue = getThemeEasing("in out").easeValue(m_pressAnimationTime);
				m_pressAnimationTime += 0.06f;

				if (m_isRaising || m_isPressed)
				{
					setElevation(2.f + pressAnimationValue * 4.f);
					if (!m_isPressed && pressAnimationValue == 1.f)
					{
						m_pressAnimationTime = 0.f;
						m_isRaising = false;
						queueAnimationUpdate();
					}
				}
				else
				{
					setElevation(2.f + (1.f - pressAnimationValue) * 4.f);
				}

				if (pressAnimationValue < 1.f)
				{
					queueAnimationUpdate();
				}
			}

			invalidate();
		}


		//------------------------------

		void drawOverlay(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
		{
			if (m_emphasis == Emphasis::Medium)
			{
				p_drawingContext->setColor(Color(m_theme->colors["on background"], 0.25f));
				p_drawingContext->strokeRoundedRectangle(Rectangle<float>(0.5f, 0.5f, getWidth() - 0.5f, getHeight() - 0.5f), getCorners().topLeftSizeX, 1.f);
			}
		}

		void draw(DrawingContext* p_drawingContext, Rectangle<float> const& p_targetRectangle) override
		{
			if (m_emphasis == Emphasis::High)
			{
				p_drawingContext->clear(m_currentColor);
				p_drawingContext->setColor(m_isAccent ? m_theme->colors["on secondary"] : m_theme->colors["on primary"]);
			}
			else
			{
				p_drawingContext->setColor(m_currentColor);
			}

			if (m_icon)
			{
				p_drawingContext->drawImage(m_icon);
			}

			if (m_text)
			{
				p_drawingContext->drawText(m_text);
			}
		}
	};

	//------------------------------

	class EditableText;

	class EditableTextListener
	{
	public:
		/*
			USER IMPLEMENTED
			Gets called when an EditableText view has gained keyboard focus.
		*/
		virtual void handleEditableTextFocusGain(EditableText* p_editableText) { }
		/*
			USER IMPLEMENTED
			Gets called when an EditableText view has lost keyboard focus.
		*/
		virtual void handleEditableTextFocusLose(EditableText* p_editableText) { }
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when the text of an EditableText view is about to be changed, either by the user or programmatically.
			All listeners of p_editableText need to return true for the string to be changed.
			This is a simpler version of the handler, without p_newString and p_newCaretIndex and is called by the default implementation of the other overload.
			The default implementation of this method only returns true.
		*/
		virtual bool handleEditableTextChange(EditableText* p_editableText) { return true; }
		/*
			LIBRARY IMPLEMENTED (only default behavior)
			Gets called when the text of an EditableText view is about to be changed, either by the user or programmatically.
			p_newString is the string that will be set if all listeners return true from this handler. Otherwise, the string is left unchanged.
			p_newString can be modified, and the contents of the string after all listeners have handled the event is what will be set as the new text.
			p_newCaretIndex works in a similar way, and it is the index of the cursor showing where new user input is inserted. 
			This index can be equal to the size of the new string, and in that case the cursor ends up at the end of the text.
			The default implementation of this method calls the simpler version that only takes the p_editableText parameter.
		*/
		virtual bool handleEditableTextChange(EditableText* p_editableText, std::string& p_newString, int32& p_newCaretIndex) 
		{ 
			return handleEditableTextChange(p_editableText); 
		}
	};

	/*
		A view that only consists of text that can be edited by the user.
	*/
	class EditableText : public View, public KeyboardListener
	{
	private:
		Text* m_text;
		float m_textDrawingOffsetX;
		float m_fontSize;
		TextAlign m_textAlign;

		uint32 m_caretIndex;
		Point<float> m_caretPosition;
		bool m_isCaretVisible;
		uint32 m_caretFrameCount;

		uint32 m_selectionEndIndex;
		Point<float> m_selectionEndPosition;
		bool m_isSelectingWithMouse;
		bool m_isSelectionVisible;

		std::vector<EditableTextListener*> m_listeners;

		//------------------------------

		void updateCaretTracking()
		{
			if (!m_text)
			{
				return;
			}

			if (m_caretPosition.x + m_textDrawingOffsetX > getWidth())
			{
				m_textDrawingOffsetX = getWidth() - m_caretPosition.x;
			}
			else if (m_caretPosition.x + m_textDrawingOffsetX < 0.f)
			{
				m_textDrawingOffsetX = -m_caretPosition.x;
			}

			if (m_textAlign == TextAlign::Left)
			{
				if (m_text->getMinimumWidth() > getWidth())
				{
					if (m_text->getMinimumWidth() + m_textDrawingOffsetX < getWidth())
					{
						m_textDrawingOffsetX = getWidth() - m_text->getMinimumWidth();
					}
				}
				else
				{
					m_textDrawingOffsetX = 0.f;
				}
			}
			else if (m_textAlign == TextAlign::Right)
			{
				if (m_text->getMinimumWidth() > getWidth())
				{
					if (getWidth() - m_text->getMinimumWidth() + m_textDrawingOffsetX > 0.f)
					{
						m_textDrawingOffsetX = m_text->getMinimumWidth() - getWidth();
					}
				}
				else
				{
					m_textDrawingOffsetX = 0;
				}
			}
		}
		void updateSelectionEndTracking()
		{
			if (m_selectionEndPosition.x + m_textDrawingOffsetX > getWidth())
			{
				m_textDrawingOffsetX = getWidth() - m_selectionEndPosition.x;
			}
			else if (m_selectionEndPosition.x + m_textDrawingOffsetX < 0.f)
			{
				m_textDrawingOffsetX = -m_selectionEndPosition.x;
			}
		}

	protected:
		void handleThemeFontFamilyChange(std::string const& p_name, char const* p_newFontFamily) override
		{
			if (p_name == "main")
			{
				if (m_text)
				{
					m_text->setFontFamily(p_newFontFamily);
					m_text->fitSizeToText();
				}
			}
		}
		//void handleThemeValueChange(std::string const& p_name, float p_value) override
		//{

		//}

	public:
		EditableText(View* p_parent, float p_width = 0.f, float p_fontSize = 12.f) :
			View(p_parent, Rectangle<float>(0.f, 0.f, p_width, p_fontSize*1.2f)), 
			m_text(0), m_textDrawingOffsetX(0.f), m_fontSize(p_fontSize), m_textAlign(TextAlign::Left),
			m_caretIndex(0), m_isCaretVisible(false), m_caretFrameCount(0), 
			m_selectionEndIndex(0), m_isSelectingWithMouse(false), m_isSelectionVisible(false)
		{
			setCursor(Cursor::Ibeam);
			enableMouseEvents();
		}

		//------------------------------

		/*
			Enables an EditableTextListener to recieve events from this EditableText.
		*/
		void addEditableTextListener(EditableTextListener* p_listener)
		{
			m_listeners.push_back(p_listener);
		}
		/*
			Disables an EditableTextListener to recieve events from this EditableText.
		*/
		void removeEditableTextListener(EditableTextListener* p_listener)
		{
			removeVectorElementWithoutKeepingOrder(m_listeners, p_listener);
		}

		//------------------------------

		void handleMouseDoubleClick(MouseEvent const& p_event) override
		{
			if (m_text)
			{
				uint32 clickCharacterIndex = m_text->getNearestCharacterIndex(p_event.x - m_textDrawingOffsetX, p_event.y, true);
				std::string const& string = m_text->getString();
				for (int32 a = clickCharacterIndex; a >= 0; a--)
				{
					if (!a || string[a - 1] == ' ')
					{
						if (a != m_caretIndex)
						{
							m_caretPosition = m_text->getCharacterPosition(a, true);
							updateCaretTracking();
						}
						m_caretIndex = a;
						break;
					}
				}
				for (int32 a = clickCharacterIndex; a <= string.size(); a++)
				{
					if (a == string.size() || string[a] == ' ')
					{
						if (a != m_selectionEndIndex)
						{
							m_selectionEndPosition = m_text->getCharacterPosition(a, true);
							updateSelectionEndTracking();
						}
						m_selectionEndIndex = a;
						break;
					}
				}
				if (m_caretIndex != m_selectionEndIndex)
				{
					m_isSelectionVisible = true;
					invalidate();
				}
			}	
		}
		void handleMouseDown(MouseEvent const& p_event) override
		{
			if (m_text)
			{
				if (p_event.modifierKeys & ModifierKeyFlags::Shift)
				{
					m_text->getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, p_event.y, &m_selectionEndIndex, &m_selectionEndPosition, true);

					if (m_selectionEndIndex == m_caretIndex)
					{
						m_caretFrameCount = 1;
						m_isCaretVisible = true;
						m_isSelectionVisible = false;
					}
					else
					{
						updateSelectionEndTracking();
						m_isSelectionVisible = true;
					}
					m_isSelectingWithMouse = true;
				}
				else
				{
					m_text->getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, p_event.y, &m_caretIndex, &m_caretPosition, true);
					updateCaretTracking();

					m_isCaretVisible = true;
					m_caretFrameCount = 1;
					m_isSelectingWithMouse = true;
					m_isSelectionVisible = false;
				}
			}
			else
			{
				//setString("");
			}

			getGui()->setKeyboardFocus(this);

			invalidate();
			queueAnimationUpdate();
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			if (m_isSelectingWithMouse)
			{
				m_text->getNearestCharacterIndexAndPosition(p_event.x - m_textDrawingOffsetX, 0, &m_selectionEndIndex, &m_selectionEndPosition, true);
				updateSelectionEndTracking();
				m_isSelectionVisible = m_selectionEndIndex != m_caretIndex;
				m_isCaretVisible = true;
				m_caretFrameCount = 1;
				invalidate();
			}
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			m_isSelectingWithMouse = false;
		}

		void handleKeyboardFocusGain() override
		{
			m_caretFrameCount = 1;
			m_isCaretVisible = true;

			for (auto listener : m_listeners)
			{
				listener->handleEditableTextFocusGain(this);
			}

			queueAnimationUpdate();
			invalidate();
		}
		void handleKeyboardFocusLose() override
		{
			m_caretFrameCount = 1;
			m_isCaretVisible = false;
			m_isSelectionVisible = false;

			for (auto listener : m_listeners)
			{
				listener->handleEditableTextFocusLose(this);
			}

			invalidate();
		}
		void handleCharacterInput(KeyboardEvent const& p_event) override
		{
			if ((p_event.character >= 32 && p_event.character < 126 || p_event.character < 0))
			{
				std::string string(m_text ? m_text->getString() : "");
				if (m_isSelectionVisible)
				{
					if (m_caretIndex <= m_selectionEndIndex)
					{
						string.erase(m_caretIndex, m_selectionEndIndex - m_caretIndex);
					}
					else
					{
						string.erase(m_selectionEndIndex, m_caretIndex - m_selectionEndIndex);
						m_caretIndex = m_selectionEndIndex;
					}
					m_isSelectionVisible = false;
				}

				string.insert(m_caretIndex, 1U, p_event.character);

				setString(string.c_str(), m_caretIndex + 1);

				updateCaretTracking();

				m_caretFrameCount = 1;
				m_isCaretVisible = true;

				invalidate();
			}
		}
		void handleKeyboardKeyDown(KeyboardEvent const& p_event) override
		{
			Window* window = getGui()->getWindow();
			if (m_isSelectionVisible && (p_event.key == KeyboardKey::Backspace || p_event.key == KeyboardKey::Delete) && m_caretIndex != m_selectionEndIndex)
			{
				std::string string = m_text->getString();
				if (m_caretIndex <= m_selectionEndIndex)
				{
					string.erase(m_caretIndex, m_selectionEndIndex - m_caretIndex);
					m_isSelectionVisible = false;
					setString(string);
					updateCaretTracking();
				}
				else
				{
					string.erase(m_selectionEndIndex, m_caretIndex - m_selectionEndIndex);
					m_isSelectionVisible = false;
					setString(string, m_selectionEndIndex);
					updateCaretTracking();
				}
				if (m_textAlign == TextAlign::Center)
				{
					m_caretPosition = m_text->getCharacterPosition(m_caretIndex);
					updateCaretTracking();
				}

				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				invalidate();
				return;
			}
			switch (p_event.key)
			{
			case KeyboardKey::Backspace:
			{
				if (!m_text)
				{
					return;
				}
				if (!m_isSelectionVisible && m_caretIndex > 0)
				{
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						std::string string = m_text->getString();
						for (int32 a = m_caretIndex - 1; a >= 0; a--)
						{
							if (!a || (string[a - 1U] == ' ' && string[a] != ' '))
							{
								string.erase(a, (int32)m_caretIndex - a);
								setString(string, a);
								break;
							}
						}
					}
					else
					{
						setString(std::string(m_text->getString()).erase(m_caretIndex - 1, 1), m_caretIndex - 1);
					}
				}
				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				m_isSelectionVisible = false;
				break;
			}
			case KeyboardKey::Delete:
			{
				if (!m_text)
				{
					return;
				}
				if (!m_isSelectionVisible && m_caretIndex < m_text->getString().size())
				{
					if (window->getIsKeyDown(KeyboardKey::Control))
					{
						std::string string = m_text->getString();
						for (int32 a = m_caretIndex; a < string.size(); a++)
						{
							if (a == string.size() - 1 || (string[a + 1U] == ' ' && string[a] != ' '))
							{
								string.erase(m_caretIndex, a - (int32)m_caretIndex + 1);
								setString(string);
								break;
							}
						}
					}
					else
					{
						setString(std::string(m_text->getString()).erase(m_caretIndex, 1));
					}
				}
				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				m_isSelectionVisible = false;
				break;
			}
			case KeyboardKey::Left:
			{
				if (!m_text)
				{
					return;
				}
				if (window->getIsKeyDown(KeyboardKey::Control))
				{
					std::string string = m_text->getString();
					if (window->getIsKeyDown(KeyboardKey::Shift))
					{
						if (!m_isSelectionVisible)
						{
							m_selectionEndIndex = m_caretIndex;
						}
						for (int32 a = m_selectionEndIndex - 1; a >= 0; a--)
						{
							if (!a || (string[a - 1U] == ' ' && string[a] != ' '))
							{
								m_selectionEndIndex = a;
								if (m_selectionEndIndex == m_caretIndex)
								{
									m_isSelectionVisible = false;
								}
								else
								{
									m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
									updateSelectionEndTracking();
									m_isSelectionVisible = true;
								}
								break;
							}
						}
					}
					else
					{
						for (int32 a = m_caretIndex - 1; a >= 0; a--)
						{
							if (!a || (string[a - 1U] == ' ' && string[a] != ' '))
							{
								m_caretIndex = a;
								m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
								updateCaretTracking();
								m_isSelectionVisible = false;
								break;
							}
						}
					}
				}
				else if (window->getIsKeyDown(KeyboardKey::Shift))
				{
					if (!m_isSelectionVisible)
					{
						m_selectionEndIndex = m_caretIndex;
					}
					if (m_selectionEndIndex > 0)
					{
						m_selectionEndIndex--;
						if (m_selectionEndIndex == m_caretIndex)
						{
							m_isSelectionVisible = false;
						}
						else
						{
							m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
							updateSelectionEndTracking();
							m_isSelectionVisible = true;
						}
					}
				}
				else
				{
					if (m_isSelectionVisible)
					{
						if (m_caretIndex > m_selectionEndIndex)
						{
							m_caretIndex = m_selectionEndIndex;
							m_caretPosition = m_selectionEndPosition;
						}
						updateCaretTracking();
						m_isSelectionVisible = false;
					}
					else
					{
						if (m_caretIndex > 0)
						{
							m_caretIndex--;
							m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
							updateCaretTracking();
						}
					}
				}
				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				break;
			}
			case KeyboardKey::Right:
			{
				if (!m_text)
				{
					return;
				}
				if (window->getIsKeyDown(KeyboardKey::Control))
				{
					std::string string = m_text->getString();
					if (window->getIsKeyDown(KeyboardKey::Shift))
					{
						if (!m_isSelectionVisible)
						{
							m_selectionEndIndex = m_caretIndex;
						}
						for (uint32 a = m_selectionEndIndex; a < string.size(); a++)
						{
							if (a == string.size() - 1 || (string[a + 1U] == ' ' && string[a] != ' '))
							{
								m_selectionEndIndex = a + 1;
								if (m_selectionEndIndex == m_caretIndex)
								{
									m_isSelectionVisible = false;
								}
								else
								{
									m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
									updateSelectionEndTracking();
									m_isSelectionVisible = true;
								}
								break;
							}
						}
					}
					else
					{
						for (uint32 a = m_caretIndex; a < string.size(); a++)
						{
							if (a == string.size() - 1 || (string[a + 1U] == ' ' && string[a] != ' '))
							{
								m_caretIndex = a + 1;
								m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
								updateCaretTracking();
								m_isSelectionVisible = false;
								break;
							}
						}
					}
				}
				else if (window->getIsKeyDown(KeyboardKey::Shift))
				{
					if (!m_isSelectionVisible)
					{
						m_selectionEndIndex = m_caretIndex;
					}
					if (m_selectionEndIndex < m_text->getString().size())
					{
						m_selectionEndIndex++;
						if (m_selectionEndIndex == m_caretIndex)
						{
							m_isSelectionVisible = false;
						}
						else
						{
							m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
							updateSelectionEndTracking();
							m_isSelectionVisible = true;
						}
					}
				}
				else
				{
					if (m_isSelectionVisible)
					{
						if (m_caretIndex < m_selectionEndIndex)
						{
							m_caretIndex = m_selectionEndIndex;
							m_caretPosition = m_selectionEndPosition;
							updateCaretTracking();
						}
						m_isSelectionVisible = false;
					}
					else
					{
						if (m_caretIndex < m_text->getString().size())
						{
							m_caretIndex++;
							m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
							updateCaretTracking();
						}
					}
				}
				m_caretFrameCount = 1;
				m_isCaretVisible = true;
				break;
			}
			case KeyboardKey::C:
			{
				if (!m_text)
				{
					return;
				}
				if (window->getIsKeyDown(KeyboardKey::Control) && m_isSelectionVisible)
				{
					if (m_caretIndex < m_selectionEndIndex)
					{
						window->setClipboardString(m_text->getString().substr(m_caretIndex, m_selectionEndIndex - m_caretIndex));
					}
					else
					{
						window->setClipboardString(m_text->getString().substr(m_selectionEndIndex, m_caretIndex - m_selectionEndIndex));
					}
				}
				break;
			}
			case KeyboardKey::X:
			{
				if (!m_text)
				{
					return;
				}
				if (window->getIsKeyDown(KeyboardKey::Control) && m_isSelectionVisible)
				{
					std::string string(m_text->getString());
					if (m_caretIndex < m_selectionEndIndex)
					{
						window->setClipboardString(string.substr(m_caretIndex, m_selectionEndIndex - m_caretIndex));
						string.erase(m_caretIndex, m_selectionEndIndex - m_caretIndex);
						setString(string);
					}
					else {
						window->setClipboardString(string.substr(m_selectionEndIndex, m_caretIndex - m_selectionEndIndex));
						string.erase(m_selectionEndIndex, m_caretIndex - m_selectionEndIndex);
						setString(string, m_selectionEndIndex);
					}

					m_isSelectionVisible = false;

					m_caretFrameCount = 1;
					m_isCaretVisible = true;
				}
				break;
			}
			case KeyboardKey::V:
			{
				if (window->getIsKeyDown(KeyboardKey::Control))
				{
					std::string string(m_text ? m_text->getString() : "");
					uint32 caretIndex = m_caretIndex;
					if (m_isSelectionVisible)
					{
						if (caretIndex < m_selectionEndIndex)
						{
							string.erase(caretIndex, m_selectionEndIndex - caretIndex);
						}
						else
						{
							string.erase(m_selectionEndIndex, caretIndex - m_selectionEndIndex);
							caretIndex = m_selectionEndIndex;
						}
						m_isSelectionVisible = false;
					}
					std::string clipboardString = window->getClipboardString();
					string.insert(caretIndex, clipboardString);
					setString(string, caretIndex + clipboardString.size());
					
					m_caretFrameCount = 1;
					m_isCaretVisible = true;
				}
				break;
			}
			case KeyboardKey::A:
			{
				if (!m_text)
				{
					return;
				}
				if (window->getIsKeyDown(KeyboardKey::Control))
				{
					selectAll();
					return;
				}
				break;
			}
			}

			invalidate();
		}

		//------------------------------

		void setSelection(uint32 p_startIndex, uint32 p_endIndex)
		{
			if (m_text)
			{
				p_startIndex = min((uint32)m_text->getString().size(), p_startIndex);
				p_endIndex = min((uint32)m_text->getString().size(), max(p_startIndex, p_endIndex));
				if (p_startIndex != p_endIndex)
				{
					if (p_startIndex != m_caretIndex)
					{
						m_caretIndex = p_startIndex;
						m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
					}

					if (p_endIndex != m_selectionEndIndex)
					{
						m_selectionEndIndex = p_endIndex;
						m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
					}
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}
		void selectAll()
		{
			if (m_text)
			{
				uint32 stringLength = m_text->getString().size();
				if (stringLength)
				{
					if (m_caretIndex != 0)
					{
						m_caretIndex = 0;
						m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
					}

					if (m_selectionEndIndex != stringLength)
					{
						m_selectionEndIndex = stringLength;
						m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
					}
					m_isSelectionVisible = true;
					invalidate();
				}
			}
		}

		//------------------------------

		/*
			Changes the content of the editable text.
			p_newCaretIndex determines the caret index that will be set if no event listeners cancel the change.
			This is needed because the old caret index will be kept in case any event listener returns false.
		*/
		void setString(char const* p_string, int32 p_newCaretIndex = -1)
		{
			if (m_text && m_text->getString() == p_string)
			{
				return;
			}
			if (p_newCaretIndex == -1)
			{
				p_newCaretIndex = m_caretIndex;
			}

			std::string newString = p_string;
			for (auto listener : m_listeners)
			{
				if (!listener->handleEditableTextChange(this, newString, p_newCaretIndex))
				{
					return;
				}
			}

			if (m_text)
			{
				m_text->forget();
			}
			if (!newString.size())
			{
				m_text = 0;
				m_caretIndex = 0;
				m_caretPosition.y = 0;
				if (m_textAlign == TextAlign::Left)
				{
					m_caretPosition.x = 0;
				}
				else if (m_textAlign == TextAlign::Right)
				{
					m_caretPosition.x = getWidth();
				}
				else if (m_textAlign == TextAlign::Center)
				{
					m_caretPosition.x = getWidth() * 0.5f;
				}
				m_textDrawingOffsetX = 0.f;
				m_isSelectionVisible = false;
				return;
			}

			m_text = getGui()->getDrawingContext()->createText(newString.c_str(), m_fontSize);
			m_text->setFontFamily(getThemeFontFamily("main"));
			m_text->setFontWeight(FontWeight::Regular);
			m_text->setTextAlign(m_textAlign);
			m_text->setWidth(getWidth());
			m_text->setTop(2.f);
			m_text->setBottom(getHeight(), false);

			if (p_newCaretIndex > (int32)m_text->getString().size())
			{
				m_caretIndex = (uint32)m_text->getString().size();
			}
			else if (p_newCaretIndex != m_caretIndex)
			{
				if (p_newCaretIndex < 0)
				{
					m_caretIndex = 0;
				}
				else
				{
					m_caretIndex = p_newCaretIndex;
				}
			}
			m_caretPosition = m_text->getCharacterPosition(m_caretIndex, true);
			updateCaretTracking();

			if (m_isSelectionVisible)
			{
				if (m_selectionEndIndex > (uint32)m_text->getString().size())
				{
					m_selectionEndIndex = min(m_selectionEndIndex, (uint32)m_text->getString().size());
					if (m_selectionEndIndex == m_caretIndex)
					{
						m_isSelectionVisible = false;
					}
					else
					{
						m_selectionEndPosition = m_text->getCharacterPosition(m_selectionEndIndex, true);
					}
				}
			}
		}
		/*
			Sets the content of the editable text.
		*/
		void setString(std::string const& p_string, int32 p_caretIndex = -1)
		{
			setString(p_string.c_str(), p_caretIndex);
		}
		/*
			Sets the content of the editable text as a value.
		*/
		template<typename T>
		void setValue(T p_value)
		{
			setString(convertNumberToString(p_value));
		}
		/*
			Sets the content of the editable text as a value, rounded at a certain digit.
		*/
		template<typename T>
		void setValue(T p_value, int32 p_roundingDigit, RoundingType p_roundingType = RoundingType::Nearest)
		{
			setString(convertNumberToString(p_value, p_roundingDigit, p_roundingType));
		}
		/*
			Returns the content of the editable text.
		*/
		char const* getString()
		{
			if (m_text)
			{
				return m_text->getString().c_str();
			}
			return "";
		}

		/*
			Returns the internal text graphics object.
		*/
		Text* getText()
		{
			return m_text;
		}

		//------------------------------

		/*
			Sets the way text is horizontally aligned within the bounds of the editable text.
		*/
		void setTextAlign(TextAlign p_textAlign)
		{
			m_textAlign = p_textAlign;
			if (m_text)
			{
				m_text->setTextAlign(m_textAlign);
				invalidate();
			}
		}
		/*
			Returns the way text is horizontally aligned within the bounds of the editable text.
		*/
		TextAlign getTextAlign()
		{
			return m_textAlign;
		}

		//------------------------------

		void setFontSize(float p_fontSize)
		{
			m_fontSize = p_fontSize;
			if (m_text)
			{
				m_text->setFontSize(p_fontSize);
			}
			setHeight(p_fontSize * 1.2f);
			invalidate();
		}
		float getFontSize()
		{
			return m_fontSize;
		}

		//------------------------------

		void handleSizeChange() override
		{
			updateCaretTracking();
			if (m_text)
			{
				m_text->setWidth(getWidth());
			}
		}

		//------------------------------

		void updateAnimations() override
		{
			if (getGui()->getKeyboardFocus() == this)
			{
				if (m_caretFrameCount % (uint32)getThemeValue("editable text caret blink rate") == 0 && !m_isSelectionVisible)
				{
					m_isCaretVisible = !m_isCaretVisible;
					invalidate();
				}
				m_caretFrameCount++;
				queueAnimationUpdate();
			}
		}

		void draw(DrawingContext* p_context) override
		{
			//p_context->setColor(Color(0.f));
			//p_context->strokeRectangle(getSize(), 1.f);
			p_context->moveOrigin(m_textDrawingOffsetX, 0.f);
			p_context->setColor(getThemeColor("on background"));
			if (m_text)
			{
				p_context->drawText(m_text);
				if (m_isSelectionVisible)
				{
					p_context->setColor(getThemeColor("selection"));
					p_context->fillRectangle(m_caretPosition.x, 0.f, m_selectionEndPosition.x, getHeight());
				}
			}
			if (m_isCaretVisible && !m_isSelectionVisible)
			{
				p_context->drawLine(m_caretPosition.x, 0.f, m_caretPosition.x, getHeight(), 1.f);
			}
			p_context->moveOrigin(-m_textDrawingOffsetX, 0.f);
		}
	};

	//------------------------------

	constexpr float TEXT_FIELD_OUTLINED_PADDING_LABEL = 5.f;

	class TextField : public View, public KeyboardListener, public EditableTextListener
	{
	public:
		enum Type
		{
			Outlined,
			Filled
		};

	private:
		EditableText* m_editableText;

		Text* m_labelText;
		Color m_labelColor;
		float m_focusAnimationTime;
		float m_focusAnimationValue;

		Text* m_prefixText;
		Text* m_suffixText;

		bool m_isMouseHovering;
		float m_hoverAnimationTime;
		float m_hoverAnimationValue;

		Type m_type;

	protected:
		void handleThemeFontFamilyChange(std::string const& p_name, char const* p_newFontFamilyName) override
		{
			if (p_name == "main")
			{
				if (m_labelText)
				{
					m_labelText->setFontFamily(p_newFontFamilyName);
					m_labelText->fitSizeToText();
				}
				if (m_prefixText)
				{
					m_prefixText->setFontFamily(p_newFontFamilyName);
					m_prefixText->fitSizeToText();
					m_editableText->setLeft(m_prefixText->getRight() + 1.f, false);
					if (m_labelText)
					{
						m_labelText->setLeft(m_prefixText->getRight() + 1.f);
					}
				}
				if (m_suffixText)
				{
					m_suffixText->setFontFamily(p_newFontFamilyName);
					m_suffixText->fitSizeToText();
					m_editableText->setRight(m_suffixText->getRight() - 1.f, false);
				}
			}
		}
		void handleThemeValueChange(std::string const& p_name, float p_newValue) override
		{
			if (p_name == "text field font size")
			{
				if (m_labelText)
				{
					m_labelText->setFontSize(p_newValue);
					m_labelText->fitSizeToText();
				}
				if (m_prefixText)
				{
					m_prefixText->setFontSize(p_newValue);
					m_prefixText->fitSizeToText();
				}
				if (m_suffixText)
				{
					m_suffixText->setFontSize(p_newValue);
					m_suffixText->fitSizeToText();
				}
				m_editableText->setFontSize(p_newValue);
			}
			if (p_name == "text field font size" || p_name == "text field height")
			{
				// Text positions will be updated in handleSizeChange()
				setHeight(getThemeValue("text field font size") * 1.2f * getThemeValue("text field height") + TEXT_FIELD_OUTLINED_PADDING_LABEL * (m_type == Type::Outlined));
			}
			if (p_name == "text field padding left")
			{
				if (m_labelText)
				{
					m_labelText->setLeft(p_newValue);
				}
				if (m_prefixText)
				{
					m_prefixText->setLeft(p_newValue);
					m_editableText->setLeft(m_prefixText->getRight() + 1.f, false);
				}
				else
				{
					m_editableText->setLeft(p_newValue, false);
				}
			}
			else if (p_name == "text field padding right")
			{
				if (m_suffixText)
				{
					m_suffixText->setRight(getWidth() - p_newValue);
					m_editableText->setRight(m_suffixText->getLeft() - 1.f, false);
				}
				else
				{
					m_editableText->setRight(getWidth() - p_newValue, false);
				}
			}
			else if (p_name == "text field filled padding bottom")
			{
				if (m_prefixText)
				{
					m_prefixText->setBottom(getHeight() - p_newValue);
				}
				if (m_suffixText)
				{
					m_suffixText->setBottom(getHeight() - p_newValue);
				}
				m_editableText->setBottom(getHeight() - p_newValue);
			}
		}

	public:
		TextField(View* p_parent, Type p_type = Type::Filled, char const* p_label = "", float p_width = 120.f) :
			View(p_parent),
			m_labelText(0), m_focusAnimationTime(0.f), m_focusAnimationValue(0.f),
			m_isMouseHovering(false), m_hoverAnimationTime(0.f), m_hoverAnimationValue(0.f),
			m_type(p_type)
		{
			setLabel(p_label);
			setCursor(Cursor::Ibeam);
			enableMouseEvents();

			m_editableText = new EditableText(this);
			m_editableText->setFontSize(getThemeValue("text field font size"));
			m_editableText->setLeft(getThemeValue("text field padding left"));
			m_editableText->setRight(p_width - getThemeValue("text field padding right"), false);
			m_editableText->addEditableTextListener(this);

			setSize(p_width, getThemeValue("text field font size") * 1.2f * getThemeValue("text field height") + TEXT_FIELD_OUTLINED_PADDING_LABEL * (m_type == Type::Outlined));

			if (p_type == Type::Filled)
			{
				setCorners(AvoGUI::RectangleCorners(5.f, 5.f, 0.f, 0.f));
			}
			else
			{
				setCornerRadius(5.f);
			}

			setString("");

			queueAnimationUpdate();
		}
		~TextField()
		{
			if (m_labelText)
			{
				m_labelText->forget();
			}
			if (m_prefixText)
			{
				m_prefixText->forget();
			}
			if (m_suffixText)
			{
				m_suffixText->forget();
			}
		}

		//------------------------------

		void addEditableTextListener(EditableTextListener* p_listener)
		{
			m_editableText->addEditableTextListener(p_listener);
		}
		void removeEditableTextListener(EditableTextListener* p_listener)
		{
			m_editableText->removeEditableTextListener(p_listener);
		}

		//------------------------------

		EditableText* getEditableText()
		{
			return m_editableText;
		}

		//------------------------------

		void handleSizeChange() override
		{
			if (m_suffixText)
			{
				m_suffixText->setRight(getWidth() - getThemeValue("text field padding right"));
				m_editableText->setRight(m_suffixText->getLeft() - 1.f, false);
			}
			else
			{
				m_editableText->setRight(getWidth() - getThemeValue("text field padding right"), false);
			}

			if (m_type == Type::Filled)
			{
				float bottom = getHeight() - getThemeValue("text field filled padding bottom");
				if (m_labelText)
				{
					m_labelText->setCenterY(getHeight() * 0.5f);
				}
				if (m_prefixText)
				{
					m_prefixText->setBottom(bottom);
				}
				if (m_suffixText)
				{
					m_suffixText->setBottom(bottom);
				}
				m_editableText->setBottom(bottom);
			}
			else
			{
				float centerY = TEXT_FIELD_OUTLINED_PADDING_LABEL + (getHeight() - TEXT_FIELD_OUTLINED_PADDING_LABEL) * 0.5f;
				if (m_labelText)
				{
					m_labelText->setCenterY(centerY);
				}
				if (m_prefixText)
				{
					m_prefixText->setCenterY(centerY);
				}
				if (m_suffixText)
				{
					m_suffixText->setCenterY(centerY);
				}
				m_editableText->setCenterY(centerY);
			}
		}

		//------------------------------

		void setLabel(char const* p_label)
		{
			if (m_labelText)
			{
				if (p_label == m_labelText->getString())
				{
					return;
				}
				m_labelText->forget();
			}
			if (p_label[0] == 0)
			{
				m_labelText = 0;
			}
			else
			{
				m_labelText = getGui()->getDrawingContext()->createText(p_label, getThemeValue("text field font size"));
				m_labelText->setFontFamily(getThemeFontFamily("main"));
				m_labelText->setFontWeight(AvoGUI::FontWeight::Regular);
				m_labelText->fitSizeToText();
				if (m_type == Type::Filled)
				{
					m_labelText->setCenterY(getHeight() * 0.5f);
				}
				else if (m_type == Type::Outlined)
				{
					m_labelText->setCenterY(TEXT_FIELD_OUTLINED_PADDING_LABEL + (getHeight() - TEXT_FIELD_OUTLINED_PADDING_LABEL) * 0.5f);
				}
				queueAnimationUpdate();
			}
		}
		char const* getLabel()
		{
			if (m_labelText)
			{
				return m_labelText->getString().c_str();
			}
			return "";
		}

		//------------------------------

		void setPrefixString(char const* p_string)
		{
			if (m_prefixText)
			{
				if (m_prefixText->getString() == p_string)
				{
					return;
				}
				m_prefixText->forget();
			}
			if (p_string[0] == 0)
			{
				m_prefixText = 0;
				return;
			}
			m_prefixText = getGui()->getDrawingContext()->createText(p_string, getThemeValue("text field font size"));
			m_prefixText->setFontFamily(getThemeFontFamily("main"));
			m_prefixText->setFontWeight(AvoGUI::FontWeight::Regular);
			m_prefixText->setHeight(m_prefixText->getFontSize()*1.2f);
			if (m_type == Type::Filled)
			{
				m_prefixText->setBottom(getThemeValue("text field filled padding bottom"));
			}
			else
			{
				m_prefixText->setTop(m_editableText->getTop() + 2.f);
			}
			m_prefixText->setLeft(getThemeValue("text field padding left"));

			m_editableText->setLeft(m_prefixText->getRight() + 1.f, false);
			if (m_labelText)
			{
				m_labelText->setLeft(m_prefixText->getRight() + 1.f);
			}
		}
		char const* setPrefixString()
		{
			if (m_suffixText)
			{
				return m_suffixText->getString().c_str();
			}
			return "";
		}

		//------------------------------

		void setSuffixString(char const* p_string)
		{
			if (m_suffixText)
			{
				if (m_suffixText->getString() == p_string)
				{
					return;
				}
				m_suffixText->forget();
			}
			if (p_string[0] == 0)
			{
				m_suffixText = 0;
				return;
			}
			m_suffixText = getGui()->getDrawingContext()->createText(p_string, getThemeValue("text field font size"));
			m_suffixText->setFontFamily(getThemeFontFamily("main"));
			m_suffixText->setFontWeight(AvoGUI::FontWeight::Regular);
			m_suffixText->setHeight(m_suffixText->getFontSize()*1.2f);
			if (m_type == Type::Filled)
			{
				m_suffixText->setBottom(getThemeValue("text field filled padding bottom"));
			}
			else
			{
				m_suffixText->setTop(m_editableText->getTop() + 2.f);
			}
			m_suffixText->setRight(getWidth() - getThemeValue("text field padding right"));

			m_editableText->setRight(m_suffixText->getLeft() - 1.f, false);
		}
		char const* getSuffixString()
		{
			if (m_suffixText)
			{
				return m_suffixText->getString().c_str();
			}
			return "";
		}

		//------------------------------

		void setString(char const* p_string)
		{
			m_editableText->setString(p_string);
			if (m_type == Type::Filled)
			{
				m_editableText->setBottom(getHeight() - getThemeValue("text field filled padding bottom"));
			}
			else if (m_type == Type::Outlined)
			{
				m_editableText->setCenterY(TEXT_FIELD_OUTLINED_PADDING_LABEL + (getHeight() - TEXT_FIELD_OUTLINED_PADDING_LABEL) * 0.5f);
			}
		}
		void setString(std::string const& p_string)
		{
			setString(p_string.c_str());
		}
		/*
			Sets the content of the text field as a value.
		*/
		template<typename T>
		void setValue(T p_value)
		{
			setString(convertNumberToString(p_value));
		}
		/*
			Sets the content of the text field as a value, rounded at a certain digit.
		*/
		template<typename T>
		void setValue(T p_value, int32 p_roundingDigit, RoundingType p_roundingType = RoundingType::Nearest)
		{
			setString(convertNumberToString(p_value, p_roundingDigit, p_roundingType));
		}
		char const* getString()
		{
			return m_editableText->getString();
		}

		//------------------------------

		void setTextAlign(TextAlign p_textAlign)
		{
			m_editableText->setTextAlign(p_textAlign);
		}
		TextAlign getTextAlign()
		{
			return m_editableText->getTextAlign();
		}

		//------------------------------

		void handleMouseDown(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseDown(event);
		}
		void handleMouseUp(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseUp(event);
		}
		void handleMouseMove(MouseEvent const& p_event) override
		{
			MouseEvent event = p_event;
			event.y = 0;
			event.x -= m_editableText->getLeft();
			m_editableText->handleMouseMove(event);
		}
		void handleMouseEnter(MouseEvent const& p_event) override
		{
			View::handleMouseBackgroundEnter(p_event);
			m_isMouseHovering = true;
			queueAnimationUpdate();
		}
		void handleMouseLeave(MouseEvent const& p_event) override
		{
			m_isMouseHovering = false;
			queueAnimationUpdate();
		}

		void handleKeyboardFocusGain() override
		{
			getGui()->setKeyboardFocus(m_editableText);
		}
		void handleEditableTextFocusGain(EditableText* p_editableText) override
		{
			queueAnimationUpdate();
		}
		void handleEditableTextFocusLose(EditableText* p_editableText) override
		{
			queueAnimationUpdate();
		}

		//------------------------------

		/*
			Returns whether the EditableText child of this text field has keyboard focus.
		*/
		bool getHasKeyboardFocus()
		{
			return m_editableText == getGui()->getKeyboardFocus();
		}

		//------------------------------

		void updateAnimations() override
		{
			if (getGui()->getKeyboardFocus() == m_editableText)
			{
				if (m_focusAnimationValue < 1.f)
				{
					m_focusAnimationValue = getThemeEasing("in out").easeValue(m_focusAnimationTime);
					m_focusAnimationTime = min(1.f, m_focusAnimationTime + 0.09f);
					invalidate();
					queueAnimationUpdate();
				}
			}
			else if (m_focusAnimationValue > 0.f)
			{
				m_focusAnimationValue = 1.f - getThemeEasing("in out").easeValue(1.f - m_focusAnimationTime);
				m_focusAnimationTime = max(0.f, m_focusAnimationTime - 0.09f);
				invalidate();
				queueAnimationUpdate();
			}
			if (m_isMouseHovering)
			{
				if (m_hoverAnimationValue < 1.f)
				{
					m_hoverAnimationValue = getThemeEasing("symmetrical in out").easeValue(m_hoverAnimationTime);
					m_hoverAnimationTime = min(1.f, m_hoverAnimationTime + getThemeValue("hover animation speed"));
					invalidate();
					queueAnimationUpdate();
				}
			}
			else if (m_hoverAnimationValue > 0.f)
			{
				m_hoverAnimationValue = 1.f - getThemeEasing("symmetrical in out").easeValue(1.f - m_hoverAnimationTime);
				m_hoverAnimationTime = max(0.f, m_hoverAnimationTime - getThemeValue("hover animation speed"));
				invalidate();
				queueAnimationUpdate();
			}
			m_labelColor = interpolate(interpolate(getThemeColor("background"), getThemeColor("on background"), (1.f - m_focusAnimationValue) * m_hoverAnimationValue * 0.3f + 0.4f), getThemeColor("primary on background"), m_focusAnimationValue);
		}

		void draw(DrawingContext* p_context) override
		{
			if (m_type == Type::Filled)
			{
				p_context->setColor(Color(interpolate(getThemeColor("background"), getThemeColor("on background"), 0.05f + 0.05f * min(m_hoverAnimationValue*0.3f + m_focusAnimationValue, 1.f)), 1.f));
				p_context->fillRectangle(getSize());
				p_context->setColor(Color(getThemeColor("on background"), 0.4));
				p_context->drawLine(0.f, getHeight() - 1.f, getWidth(), getHeight() - 0.5f, 1.f);
				if (m_focusAnimationValue > 0.01f)
				{
					p_context->setColor(getThemeColor("primary on background"));
					p_context->drawLine((1.f - m_focusAnimationValue) * getWidth() * 0.5f, getHeight() - 1.f, (1.f + m_focusAnimationValue) * getWidth() * 0.5f, getHeight() - 1.f, 2.f);
				}
				if (m_labelText)
				{
					float labelAnimationValue = m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f;
					float leftPadding = getThemeValue("text field padding left");
					p_context->moveOrigin(leftPadding + 2.f*labelAnimationValue, -0.17f*(getHeight() - m_labelText->getHeight() - leftPadding) * labelAnimationValue);
					p_context->setScale(1.f - labelAnimationValue * 0.3f);
					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);
					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}
			else if (m_type == Type::Outlined)
			{
				p_context->setColor(m_labelColor);
				p_context->strokeRectangle(Rectangle<float>(1.f, 1.f + TEXT_FIELD_OUTLINED_PADDING_LABEL, getWidth() - 1.f, getHeight() - 1.f), getCorners(), m_focusAnimationValue + 1.f);

				if (m_labelText)
				{
					float labelAnimationValue = m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f;
					p_context->moveOrigin(getThemeValue("text field padding left") + 2.f * labelAnimationValue, -(getHeight() - TEXT_FIELD_OUTLINED_PADDING_LABEL) * 0.3f * labelAnimationValue);
					p_context->setScale(1.f - labelAnimationValue * 0.3f);

					p_context->setColor(getThemeColor("background"));
					p_context->fillRoundedRectangle(Rectangle<float>(m_labelText->getLeft() - 4.f, m_labelText->getTop(), m_labelText->getRight() + 4.f, m_labelText->getBottom()), 2.f);

					p_context->setColor(m_labelColor);
					p_context->drawText(m_labelText);

					p_context->setScale(1.f);
					p_context->setOrigin(getAbsoluteTopLeft());
				}
			}

			if (m_prefixText)
			{
				p_context->setColor(Color(getThemeColor("on background"), 0.5f/* * (m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f)*/));
				p_context->drawText(m_prefixText);
			}
			if (m_suffixText)
			{
				p_context->setColor(Color(getThemeColor("on background"), 0.5f/* * (m_editableText->getString()[0] == 0 ? m_focusAnimationValue : 1.f)*/));
				p_context->drawText(m_suffixText);
			}
		}
	};
}