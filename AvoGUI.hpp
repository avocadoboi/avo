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
#include <map>
#include <thread>
#include <mutex>
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
		Returns a random double between 0 and 1. It just uses the standard library random header.
		Convenient function.
	*/
	double random();

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

		void set(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
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
		Point<PointType> operator*(double p_factor) const
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
			x /= (PointType)p_point.y;
			return *this;
		}
		Point<PointType>& operator/=(double p_divisor)
		{
			x /= p_divisor;
			x /= p_divisor;
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
			double atan2 = std::atan2(y, x);
			if (atan2 < 0.)
			{
				return atan2 + TWO_PI;
			}
			return atan2;
		}
		/*
			Returns the angle between the ray to the point and the x-axis, relative to (p_originX, p_originY).
			Angle is in radians and in the range [0, 2pi].
		*/
		double getAngle(PointType p_originX, PointType p_originY) const
		{
			double atan2 = std::atan2(y - p_originY, x - p_originX);
			if (atan2 < 0.)
			{
				return atan2 + TWO_PI;
			}
			return atan2;
		}
		/*
			Returns the angle between the ray to the point and the x-axis, relative to p_origin.
			Angle is in radians and in the range [0, 2pi].
		*/
		double getAngle(Point<PointType> const& p_origin) const
		{
			double atan2 = std::atan2(y - p_origin.y, x - p_origin.x);
			if (atan2 < 0.)
			{
				return atan2 + TWO_PI;
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

	/*
		Linearly interpolates between p_start and p_end. This means we are calculating a point on the line segment between the two points.
	*/
	template<typename Type>
	Point<Type> interpolate(Point<Type> const& p_start, Point<Type> const& p_end, double p_progress)
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
		std::map<char const*, Color> colors;
		std::map<char const*, Easing> easings;
		std::map<char const*, char const*> fontFamilies;
		std::map<char const*, float> values;

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

	class GUI;
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

		//------------------------------

		bool m_isMouseHovering;

		friend class GUI;

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
			Updates the shadow bounds and the shadow image.
		*/
		void updateShadow();

	protected:
		GUI* m_GUI;
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
				if (abs(p_previousBounds.getWidth() - m_bounds.getWidth()) > 0.0001 || abs(p_previousBounds.getHeight() - m_bounds.getHeight()) > 0.0001)
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
		GUI* getGUI() const
		{
			return m_GUI;
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
		View* getChild(uint32 p_viewIndex)
		{
			return m_children[p_viewIndex];
		}
		/*
			LIBRARY IMPLEMENTED
			Returns the number of child views that are attached to this view.
		*/
		uint32 getNumberOfChildren()
		{
			return m_children.size();
		}
		/*
			LIBRARY IMPLEMENTED
			Returns a vector containing the child views that are attached to this view.
		*/
		std::vector<View*> const& getChildren()
		{
			return m_children;
		}

		/*
			LIBRARY IMPLEMENTED
			Makes sure the view is drawn at the correct time, according to elevation. 
			You shouldn't need to use this method, it is used internally.
		*/
		void updateViewDrawingIndex(View* p_view);

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
			return Point<float>(m_absolutePosition.x + m_bounds.right - m_bounds.left, m_bounds.top);
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
			p_event is an object that contains information about the mouse event.
		*/
		virtual void handleMouseDoubleClick(MouseEvent const& p_event) { }

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
		Child = 0x4UL,
		Minimized = 0x8UL,
		Maximized = 0x10UL,
		MinimizeButton = 0x20UL,
		MaximizeButton = 0x40UL,
		ResizeBorder = 0x80UL,
		CustomBorder = 0x100UL, // This makes the client area take up the full window, and the GUI determines which areas are for resizing and moving the window.
		DefaultCustom = CustomBorder | ResizeBorder | Border | MaximizeButton,
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

	class GUI;

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
			Sets the size of the client area of the window, in pixel units.
		*/
		virtual void setSize(Point<uint32> const& p_size) = 0;
		/*
			Sets the size of the client area of the window, in pixel units.
		*/
		virtual void setSize(uint32 p_width, uint32 p_height) = 0;
		/*
			Returns the size of the client area of the window, in pixel units.
		*/
		virtual Point<uint32> const& getSize() const = 0;
		/*
			Returns the width of the client area of the window, in pixel units.
		*/
		virtual uint32 getWidth() const = 0;
		/*
			Returns the height of the client area of the window, in pixel units.
		*/
		virtual uint32 getHeight() const = 0;

		/*
			Sets the smallest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual void setMinSize(Point<uint32> const& p_minSize) = 0;
		/*
			Sets the smallest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual void setMinSize(uint32 p_minWidth, uint32 p_minHeight) = 0;
		/*
			Returns the smallest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual Point<uint32> getMinSize() const = 0;
		/*
			Returns the smallest allowed width for the window when the user is resizing it, in pixel units.
		*/
		virtual uint32 getMinWidth() const = 0;
		/*
			Returns the smallest allowed height for the window when the user is resizing it, in pixel units.
		*/
		virtual uint32 getMinHeight() const = 0;

		/*
			Sets the biggest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual void setMaxSize(Point<uint32> const& p_maxSize) = 0;
		/*
			Sets the biggest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual void setMaxSize(uint32 p_maxWidth, uint32 p_maxHeight) = 0;
		/*
			Returns the biggest allowed size for the window when the user is resizing it, in pixel units.
		*/
		virtual Point<uint32> getMaxSize() const = 0;
		/*
			Returns the biggest allowed width for the window when the user is resizing it, in pixel units.
		*/
		virtual uint32 getMaxWidth() const = 0;
		/*
			Returns the biggest allowed height for the window when the user is resizing it, in pixel units.
		*/
		virtual uint32 getMaxHeight() const = 0;

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
		virtual Point<int32> const& getMousePosition() const = 0;

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

	class Gradient
	{
	private:
		Point<float> m_startPosition;
		Point<float> m_endPosition;
		Point<float> m_radius;

		std::vector<Color> m_colors;
		std::vector<float> m_positions;
		bool m_isRadial;

	public:
		Gradient() :
			m_isRadial(false)
		{
			m_colors.reserve(5);
			m_positions.reserve(5);
		}

		/*
			Adds a color position to the gradient.
			p_position is expressed as a factor that is relative to the start and end positions if it's linear and to the radius if it's radial.
		*/
		void addStop(Color const& p_color, float p_position)
		{
			m_colors.push_back(p_color);
			m_positions.push_back(p_position);
		}
		uint32 getNumberOfStops() const
		{
			return m_colors.size();
		}

		Color const& getStopColor(uint32 p_index) const
		{
			return m_colors[p_index];
		}
		float getStopPosition(uint32 p_index) const
		{
			return m_positions[p_index];
		}

		/*
			Sets whether the gradient is radial or linear.
			A radial gradient goes from the start position and out in all directions.
			A linear gradient goes linearly from the start position to the end position.
		*/
		void setIsRadial(bool p_isRadial)
		{
			m_isRadial = p_isRadial;
		}
		/*
			Returns whether the gradient is radial or linear.
		*/
		bool getIsRadial() const
		{
			return m_isRadial;
		}

		void setStartPosition(Point<float> const& p_startPosition)
		{
			m_startPosition = p_startPosition;
		}
		void setStartPosition(float p_x, float p_y)
		{
			m_startPosition.set(p_x, p_y);
		}
		Point<float> const& getStartPosition() const
		{
			return m_startPosition;
		}

		void setEndPosition(Point<float> const& p_endPosition)
		{
			m_endPosition = p_endPosition;
		}
		void setEndPosition(float p_x, float p_y)
		{
			m_endPosition.set(p_x, p_y);
		}
		Point<float> const& getEndPosition() const
		{
			return m_endPosition;
		}

		void setRadius(float p_radius)
		{
			m_radius = p_radius;
		}
		void setRadius(Point<float> const& p_radius)
		{
			m_radius = p_radius;
		}
		void setRadius(float p_radiusX, float p_radiusY)
		{
			m_radius.set(p_radiusX, p_radiusY);
		}
		Point<float> const& getRadius() const
		{
			return m_radius;
		}
		float getRadiusX() const
		{
			return m_radius.x;
		}
		float getRadiusY() const
		{
			return m_radius.y;
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
			Draws a filled rectangle using the current color. 
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle) = 0;
		/*
			Draws a filled rectangle using the current color. 
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle using the current color. 
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			Draws a filled rectangle at the origin using the current color. 
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Point<float> const& p_size) = 0;
		/*
			Draws a filled rectangle at the origin using the current color. 
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(float p_width, float p_height) = 0;

		/*
			Draws a filled rectangle with custom corners using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rectangle with custom corners at the origin using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners) = 0;
		/*
			Draws a filled rectangle with custom corners at the origin using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners) = 0;

		/*
			Draws a filled rounded rectangle using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRoundedRectangle(Point<float> const& p_size, float p_radius) = 0;
		/*
			Draws a filled rounded rectangle at the origin using the current color.
			Change color being used with method setColor.
		*/
		virtual void fillRoundedRectangle(float p_width, float p_height, float p_radius) = 0;

		//------------------------------

		/*
			Draws a rectangle outline using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rectangle outline with custom corners using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Point<float> const& p_position, Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline with custom corners using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(Point<float> const& p_size, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rectangle outline at the origin with custom corners using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRectangle(float p_width, float p_height, RectangleCorners const& p_rectangleCorners, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRoundedRectangle(Rectangle<float> const& p_rectangle, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_position, Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) = 0;

		/*
			Draws a rounded rectangle outline at the origin using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRoundedRectangle(Point<float> const& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a rounded rectangle outline at the origin using the current color.
			Change the color being used with the method setColor.
		*/
		virtual void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a filled circle using the current color. 
			Change the color being used with the method setColor.
		
			p_position is the center position of the circle.
		*/
		virtual void fillCircle(Point<float> const& p_position, float p_radius) = 0;
		/*
			Draws a filled circle using the current color. 
			Change the color being used with the method setColor.
		
			p_x is the horizontal center position of the circle.
			p_y is the vertical center position of the circle.
		*/
		virtual void fillCircle(float p_x, float p_y, float p_radius) = 0;

		/*
			Draws a circle outline using the current color. 
			Change the color being used with the method setColor.
		
			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(Point<float> const& p_position, float p_radius, float p_strokeWidth = 1.f) = 0;
		/*
			Draws a circle outline using the current color. 
			Change the color being used with the method setColor.
		
			p_position is the center position of the circle.
		*/
		virtual void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		/*
			Draws a straight line between two points using the current color. 
			Change the color being used with the method setColor.
		*/
		virtual void drawLine(Point<float> const& p_point_0, Point<float> const& p_point_1, float p_thickness = 1.f) = 0;
		/*
			Draws a straight line between two points using the current color. 
			Change the color being used with the method setColor.
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
			Fills a custom shape with the current color.

			p_shape is a vector of points that make up the shape.
		*/
		virtual void fillShape(std::vector<Point<float>> const& p_vertices) = 0;
		/*
			Fills a custom shape with the current color.
		
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
		*/
		virtual void pushClipGeometry(Geometry* p_geometry) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
		*/
		virtual void pushClipShape(std::vector<Point<float>> const& p_points) = 0;
		/*
			After calling this, all graphics drawn outside the polygon will be invisible, on pixel level.
			Call popClipShape to remove the last pushed clip shape.
		*/
		virtual void pushClipShape(Point<float> const* p_points, uint32 p_numberOfPoints) = 0;

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
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipRectangle to remove the last pushed clip rectangle.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size) = 0;

		/*
			This removes the last added clipping rectangle.
			Only use this when the last pushed clip was a regular axis-aligned rectangle, without custom corners.
		*/
		virtual void popClipRectangle() = 0;

		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip corner rectangle.
		*/
		virtual void pushClipRectangle(float p_left, float p_top, float p_right, float p_bottom, RectangleCorners const& p_corners) = 0;
		/*
			After calling this, all graphics drawn outside the rectangle will be invisible, on pixel level.
			Call popClipRectangle to remove the last pushed clip corner rectangle.
		*/
		virtual void pushClipRectangle(Rectangle<float> const& p_rectangle, RectangleCorners const& p_corners) = 0;
		/*
			After calling this, all graphics drawn outside a rectangle at the origin with the given size will be invisible, on pixel level.
			p_size is the size of the clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed clip corner rectangle.
		*/
		virtual void pushClipRectangle(Point<float> const& p_size, RectangleCorners const& p_corners) = 0;

		//------------------------------

		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
		*/
		virtual void pushRoundedClipRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;
		/*
			After calling this, all graphics drawn outside the rounded rectangle will be invisible, on pixel-level.
			Call popClipShape to remove the last pushed rounded clip rectangle.
		*/
		virtual void pushRoundedClipRectangle(Rectangle<float> const& p_rectangle, float p_radius) = 0;
		/*
			After calling this, all graphics drawn outside a rounded rectangle at the origin with the given size and radius will be invisible, on pixel level.
			p_size is the size of the rounded clip rectangle positioned at the origin.
			Call popClipShape to remove the last pushed rounded clip rectangle.
		*/
		virtual void pushRoundedClipRectangle(Point<float> const& p_size, float p_radius) = 0;

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
		virtual void drawImage(Image* p_image) = 0;

		//------------------------------

		/*
			Sets the gradient being used as the brush when drawing shape.
		*/
		virtual void setGradientBrush(Gradient const& p_gradient) = 0;
		/*
			Sets the color being used when drawing shapes.
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
	class GUI : public View, public WindowListener, public GlobalMouseListener
	{
	private:
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

	public:
		GUI();
		~GUI();

		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to AvoGUI::GUI::createContent will be made when these objects have been created.
			After that, an initial call to AvoGUI::GUI::handleSizeChange will also be made.
		
			p_title is the text that appears in the title bar of the window (if it has an OS border).
			
			p_positionFactorX is the horizontal position of the window, expressed as a factor between 0 and 1, where 0 means the left edge 
			of the primary monitor and the left edge of the window are aligned, and 1 means the right edges are aligned.
			
			p_positionFactorY is the vertical equivalent to p_positionFactorX.
			
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(char const* p_title, float p_positionFactorX, float p_positionFactorY, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, GUI* p_parent = 0);
		/*
			LIBRARY IMPLEMENTED
			This method creates the window and drawing context as well as creates the content of the GUI and lays it out.
			A call to AvoGUI::GUI::createContent will be made when these objects have been created and can be used.
			After that, an initial call to AvoGUI::GUI::handleSizeChange will also be made.
		
			p_title is the text that appears in the title bar of the window (if it has an OS border).
			p_width is the width of the client area in DIPs (device independent pixels).
			p_height is the height of the client area in DIPs (device independent pixels).
			p_windowFlags are the styling options for the window which can be combined with the binary OR operator, "|".
			p_parent is an optional parent GUI, only used if the Child bit is turned on in p_windowFlags.
		*/
		void create(char const* p_title, float p_width, float p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, GUI* p_parent = 0);

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
			Should only need to be used internally. 
			This locks the animation thread mutex, so that the critical section in the animation thread does not run until the mutex is unlocked again (or the other way around).
		*/
		void excludeAnimationThread()
		{
			m_animationThreadMutex.lock();
		}
		/*
			LIBRARY IMPLEMENTED
			Should only need to be used internally. 
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
			Returns whether the GUi has invalid rectangles.
		*/
		bool getNeedsRedrawing()
		{
			return m_invalidRectangles.size();
		}
		/*
			LIBRARY IMPLEMENTED
			Draws the invalid rectangles of the GUI. This method should only be called internally by the library. Use draw() to draw things directly on the GUI.
		*/
		void drawViews();

		//------------------------------

		/*
			LIBRARY IMPLEMENTED
			This runs the global event loop and blocks until all windows have been destroyed.
		*/
		static void run();
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
					m_text = getGUI()->getDrawingContext()->createText(p_string, getThemeValue("tooltip font size"));
					m_text->fitSizeToText();
					setSize(m_text->getWidth() + 1.6 * getThemeValue("tooltip font size"), m_text->getHeight() + getThemeValue("tooltip font size") * 1.f);
					m_text->setCenter(getWidth() * 0.5f, getHeight() * 0.5f);
				}

				if (p_targetRectangle.bottom + 7.f + getHeight() >= getGUI()->getHeight())
				{
					setBottom(max(1.f, p_targetRectangle.top - 7.f), true);
				}
				else
				{
					setTop(p_targetRectangle.bottom + 7.f, true);
				}
				setCenterX(max(1.f + getWidth() * 0.5f, min(getGUI()->getWidth() - getWidth() * 0.5f - 1.f, p_targetRectangle.getCenterX())));

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
		GUI* m_gui;

		bool m_canSelectMultipleFiles;
		std::vector<FileExtensionFilter> m_fileExtensions;
		char const* m_title;

	public:
		OpenFileDialog() :
			m_gui(0),
			m_canSelectMultipleFiles(false), m_title("Open file...")
		{ }
		OpenFileDialog(GUI* p_gui) :
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

				if (m_isMouseHovering)
				{
					getGUI()->getWindow()->setCursor(Cursor::Arrow);
				}
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

				if (m_isMouseHovering)
				{
					getGUI()->getWindow()->setCursor(Cursor::Hand);
				}
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
				m_text = getGUI()->getDrawingContext()->createText(p_string, getThemeValue("button font size"));
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

			getGUI()->setKeyboardFocus(this);

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
			Window* window = getGUI()->getWindow();
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

			m_text = getGUI()->getDrawingContext()->createText(newString.c_str(), m_fontSize);
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
			if (getGUI()->getKeyboardFocus() == this)
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
				m_labelText = getGUI()->getDrawingContext()->createText(p_label, getThemeValue("text field font size"));
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
			m_prefixText = getGUI()->getDrawingContext()->createText(p_string, getThemeValue("text field font size"));
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
			m_suffixText = getGUI()->getDrawingContext()->createText(p_string, getThemeValue("text field font size"));
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
			getGUI()->setKeyboardFocus(m_editableText);
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
			return m_editableText == getGUI()->getKeyboardFocus();
		}

		//------------------------------

		void updateAnimations() override
		{
			if (getGUI()->getKeyboardFocus() == m_editableText)
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

	//------------------------------
	// Font data

	char const* const FONT_NAME_ROBOTO = "Roboto";

	char const FONT_DATA_ROBOTO_REGULAR[] =
	{
		0,1,0,0,0,18,1,0,0,4,0,32,71,68,69,70,-76,66,-80,-126,0,2,40,-36,0,0,2,98,71,80,79,83,-1,26,18,-41,0,2,43,64,0,0,93,-52,71,83,85,66,-21,-126,-28,89,0,2,-119,12,0,0,21,-112,79,83,47,50,-105,-126,-79,-88,0,0,1,-88,0,0,0,96,99,109,97,112,1,119,88,30,0,0,27,88,0,0,
		18,70,99,118,116,32,43,-88,7,-99,0,0,48,-88,0,0,0,84,102,112,103,109,119,-8,96,-85,0,0,45,-96,0,0,1,-68,103,97,115,112,0,8,0,19,0,2,40,-48,0,0,0,12,103,108,121,102,38,-70,11,-12,0,0,59,28,0,1,-23,108,104,100,109,120,85,122,96,122,0,0,22,64,0,0,5,24,104,101,97,100,-4,106,-46,122,
		0,0,1,44,0,0,0,54,104,104,101,97,10,-70,10,-82,0,0,1,100,0,0,0,36,104,109,116,120,-82,114,-113,-105,0,0,2,8,0,0,20,56,108,111,99,97,-128,119,-1,-69,0,0,48,-4,0,0,10,30,109,97,120,112,7,62,3,9,0,0,1,-120,0,0,0,32,110,97,109,101,-26,-92,21,-119,0,2,36,-120,0,0,4,38,112,111,
		115,116,-1,109,0,100,0,2,40,-80,0,0,0,32,112,114,101,112,-94,102,-6,-55,0,0,47,92,0,0,1,73,0,1,0,0,0,2,35,18,-118,127,112,72,95,15,60,-11,0,25,8,0,0,0,0,0,-60,-16,17,46,0,0,0,0,-43,1,82,-12,-6,27,-3,-43,9,48,8,115,0,0,0,9,0,2,0,0,0,0,0,0,0,1,0,0,
		7,108,-2,12,0,0,9,73,-6,27,-2,74,9,48,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,0,1,0,0,5,14,0,-113,0,22,0,84,0,5,0,1,0,0,0,0,0,14,0,0,2,0,2,36,0,6,0,1,0,3,4,-122,1,-112,0,5,0,0,5,-102,5,51,0,0,1,31,5,-102,5,51,0,0,3,-47,
		0,102,2,0,0,0,2,0,0,0,0,0,0,0,0,0,-32,0,2,-1,80,0,32,91,0,0,0,32,0,0,0,0,71,79,79,71,0,64,0,0,-1,-3,6,0,-2,0,0,102,7,-102,2,0,32,0,1,-97,0,0,0,0,4,58,5,-80,0,32,0,32,0,3,3,-116,0,100,0,0,0,0,0,0,0,0,1,-5,0,0,1,-5,0,0,
		2,15,0,-96,2,-113,0,-120,4,-19,0,119,4,126,0,110,5,-36,0,105,4,-7,0,101,1,101,0,103,2,-68,0,-123,2,-56,0,38,3,114,0,28,4,-119,0,78,1,-110,0,29,2,53,0,37,2,27,0,-112,3,76,0,18,4,126,0,115,4,126,0,-86,4,126,0,93,4,126,0,94,4,126,0,53,4,126,0,-102,4,126,0,-124,4,126,
		0,77,4,126,0,112,4,126,0,100,1,-16,0,-122,1,-79,0,41,4,17,0,72,4,100,0,-104,4,46,0,-122,3,-57,0,75,7,47,0,106,5,56,0,28,4,-5,0,-87,5,53,0,119,5,63,0,-87,4,-116,0,-87,4,108,0,-87,5,115,0,122,5,-76,0,-87,2,45,0,-73,4,106,0,53,5,4,0,-87,4,78,0,-87,6,-4,0,-87,
		5,-76,0,-87,5,-128,0,118,5,12,0,-87,5,-128,0,109,4,-19,0,-88,4,-65,0,80,4,-58,0,49,5,48,0,-116,5,23,0,28,7,25,0,61,5,4,0,57,4,-50,0,15,4,-54,0,86,2,31,0,-110,3,72,0,40,2,31,0,9,3,88,0,64,3,-100,0,4,2,121,0,57,4,90,0,109,4,125,0,-116,4,48,0,92,4,-125,
		0,95,4,61,0,93,2,-57,0,60,4,125,0,96,4,104,0,-116,1,-15,0,-115,1,-23,-1,-65,4,14,0,-115,1,-15,0,-100,7,3,0,-117,4,106,0,-116,4,-112,0,91,4,125,0,-116,4,-116,0,95,2,-75,0,-116,4,32,0,95,2,-99,0,9,4,105,0,-120,3,-32,0,33,6,3,0,43,3,-9,0,41,3,-55,0,22,3,-9,0,88,
		2,-75,0,64,1,-13,0,-81,2,-75,0,19,5,113,0,-125,1,-13,0,-117,4,96,0,105,4,-90,0,91,5,-76,0,105,4,51,0,15,1,-21,0,-109,4,-24,0,90,3,88,0,101,6,73,0,91,3,-109,0,-109,3,-63,0,102,4,110,0,127,6,74,0,90,3,-86,0,-114,2,-3,0,-126,4,70,0,97,2,-17,0,66,2,-17,0,62,2,-126,
		0,123,4,-120,0,-102,3,-23,0,67,2,22,0,-109,1,-5,0,116,2,-17,0,122,3,-93,0,122,3,-64,0,102,5,-36,0,85,6,53,0,80,6,57,0,111,3,-55,0,68,7,122,-1,-14,4,68,0,89,5,-128,0,118,4,-70,0,-90,4,-62,0,-117,6,-63,0,78,4,-80,0,126,4,-111,0,71,4,-120,0,91,4,-100,0,-107,4,-57,0,95,
		5,-102,0,29,1,-6,0,-101,4,115,0,-102,4,79,0,34,2,41,0,34,5,-117,0,-94,4,-120,0,-111,7,-95,0,104,7,68,0,97,1,-4,0,-96,5,-121,0,93,2,-71,-1,-28,5,126,0,101,4,-110,0,91,5,-112,0,-116,4,-13,0,-120,2,3,-1,-76,4,55,0,98,3,-60,0,-87,3,-115,0,-115,3,-85,0,-114,3,106,0,-127,1,-15,
		0,-115,2,-83,0,121,2,42,0,50,3,-58,0,123,2,-4,0,94,2,90,0,126,0,0,-4,-89,0,0,-3,111,0,0,-4,-117,0,0,-3,94,0,0,-4,39,0,0,-3,56,2,13,0,-73,4,11,0,113,2,23,0,-109,4,115,0,-79,5,-92,0,31,5,113,0,103,5,62,0,50,4,-111,0,120,5,-75,0,-78,4,-111,0,69,5,-69,0,77,
		5,-119,0,90,5,82,0,113,4,-123,0,100,4,-67,0,-96,4,2,0,46,4,-120,0,96,4,80,0,99,4,37,0,109,4,-120,0,-111,4,-114,0,122,2,-105,0,-61,4,110,0,37,3,-20,0,101,4,-60,0,41,4,-120,0,-111,4,77,0,101,4,-120,0,96,4,44,0,81,4,93,0,-113,5,-93,0,87,5,-102,0,95,6,-105,0,122,4,-95,
		0,121,4,66,-1,-38,6,72,0,74,5,-1,0,42,5,100,0,123,8,-111,0,49,8,-92,0,-79,6,-126,0,62,5,-76,0,-80,5,11,0,-94,6,4,0,50,7,67,0,27,4,-65,0,80,5,-76,0,-79,5,-87,0,47,5,7,0,77,6,44,0,83,5,-39,0,-81,5,122,0,-106,7,-121,0,-80,7,-64,0,-80,6,18,0,16,6,-21,0,-78,
		5,5,0,-93,5,100,0,-109,7,39,0,-73,5,24,0,89,4,108,0,97,4,-110,0,-99,3,91,0,-102,4,-44,0,46,6,32,0,21,4,16,0,88,4,-98,0,-100,4,82,0,-100,4,-96,0,44,5,-17,0,-99,4,-99,0,-100,4,-98,0,-100,3,-40,0,40,5,-51,0,100,4,-67,0,-100,4,89,0,103,6,120,0,-100,6,-98,0,-111,4,-9,
		0,30,6,54,0,-99,4,88,0,-99,4,77,0,100,6,-121,0,-99,4,100,0,47,4,104,-1,-24,4,77,0,103,6,-55,0,39,6,-28,0,-100,4,-119,-1,-3,4,-98,0,-100,7,8,0,-100,6,43,0,-127,4,86,-1,-36,7,43,0,-73,5,-8,0,-103,4,-46,0,40,4,70,0,15,7,11,0,-55,6,11,0,-68,6,-47,0,-109,5,-31,0,-106,
		9,4,0,-74,7,-47,0,-101,4,35,0,80,3,-37,0,76,5,113,0,103,4,-117,0,91,5,10,0,22,4,3,0,46,5,113,0,103,4,-120,0,91,7,1,0,-100,6,36,0,126,7,8,0,-100,6,43,0,-127,5,50,0,117,4,71,0,100,4,-3,0,116,0,0,-4,103,0,0,-4,113,0,0,-3,102,0,0,-3,-92,0,0,-6,27,0,0,
		-6,44,6,9,0,-79,4,-19,0,-100,4,86,-1,-36,5,27,0,-88,4,-119,0,-116,4,99,0,-94,3,-112,0,-111,4,-37,0,-79,4,5,0,-111,7,-94,0,27,6,97,0,21,5,-102,0,-78,4,-72,0,-100,5,9,0,-93,4,126,0,-102,6,-116,0,68,5,-125,0,62,5,-1,0,-87,4,-39,0,-100,7,-49,0,-88,5,-76,0,-111,8,49,0,-80,
		6,-12,0,-111,5,-18,0,113,4,-45,0,109,5,24,0,57,4,42,0,41,7,44,0,52,5,92,0,31,5,-68,0,-106,4,-106,0,103,5,111,0,-106,4,106,0,-125,5,111,0,-119,6,47,0,63,4,-67,-1,-34,5,9,0,-93,4,90,0,-102,5,-2,0,47,4,-17,0,44,5,-78,0,-79,4,-120,0,-111,6,18,0,-87,4,-20,0,-100,7,79,
		0,-87,6,62,0,-99,5,-121,0,93,4,-88,0,104,4,-88,0,105,4,-73,0,58,3,-85,0,59,5,46,0,57,4,64,0,41,4,-10,0,87,6,-108,0,89,6,-28,0,100,6,86,0,54,5,43,0,49,4,73,0,82,4,7,0,121,7,-63,0,68,6,117,0,63,7,-5,0,-87,6,-95,0,-112,4,-10,0,118,4,29,0,101,5,-83,0,35,
		5,32,0,70,5,100,0,-106,6,2,0,47,4,-14,0,44,3,32,0,111,4,20,0,0,8,41,0,0,4,20,0,0,8,41,0,0,2,-71,0,0,2,10,0,0,1,92,0,0,4,127,0,0,2,48,0,0,1,-94,0,0,1,0,0,0,0,-47,0,0,0,0,0,0,2,52,0,37,2,52,0,37,5,64,0,-94,6,63,0,-112,3,-91,
		0,13,1,-103,0,96,1,-103,0,48,1,-105,0,36,1,-103,0,79,2,-44,0,104,2,-37,0,60,2,-63,0,36,4,105,0,70,4,-113,0,87,2,-78,0,-118,3,-60,0,-108,5,90,0,-108,1,126,0,82,7,-86,0,68,2,102,0,108,2,102,0,89,3,-93,0,59,2,-17,0,54,3,96,0,122,4,-90,0,91,6,85,0,31,6,-112,0,-89,
		8,118,0,-88,5,-21,0,31,6,43,0,-116,4,126,0,95,5,-38,0,31,4,34,0,42,4,116,0,32,5,72,0,93,5,79,0,31,5,-25,0,122,3,-50,0,104,8,58,0,-94,5,1,0,103,5,23,0,-104,6,38,0,84,6,-41,0,100,6,-49,0,99,6,106,0,89,4,-113,0,106,5,-114,0,-87,4,-81,0,69,4,-110,0,-88,4,-59,
		0,63,8,58,0,98,2,12,-1,-80,4,-126,0,101,4,100,0,-104,4,17,0,62,4,47,0,-123,4,8,0,43,2,76,0,-75,2,-113,0,110,2,3,0,92,4,-13,0,60,4,110,0,31,4,-117,0,60,6,-44,0,60,6,-44,0,60,4,-18,0,60,6,-101,0,95,0,0,0,0,8,51,0,91,8,53,0,92,2,-17,0,66,2,-17,0,122,
		2,-17,0,80,4,15,0,85,4,15,0,96,4,15,0,66,4,14,0,114,4,15,0,-128,4,15,0,48,4,15,0,78,4,15,0,78,4,15,0,-104,4,15,0,99,4,35,0,71,4,43,0,13,4,84,0,38,6,21,0,49,4,103,0,20,4,124,0,116,4,38,0,40,4,32,0,67,4,74,0,-118,4,-69,0,89,4,92,0,-118,4,-69,
		0,96,4,-29,0,-118,6,2,0,-118,3,-76,0,-118,4,84,0,-118,3,-49,0,43,1,-24,0,-105,4,-29,0,-118,4,-84,0,99,3,-53,0,-118,4,32,0,67,4,51,0,48,3,-95,0,13,3,-81,0,-118,4,103,0,20,4,-69,0,96,4,103,0,20,3,-119,0,62,4,-50,0,-118,3,-17,0,63,5,103,0,96,5,23,0,96,4,-14,0,117,
		5,114,0,38,4,124,0,96,7,65,0,39,7,79,0,-118,5,116,0,40,4,-51,0,-118,4,89,0,-118,5,36,0,46,6,11,0,31,4,63,0,71,4,-20,0,-118,4,78,0,-117,4,-63,0,39,4,31,0,34,5,40,0,-118,4,106,0,61,6,81,0,-118,6,-84,0,-118,5,29,0,8,5,-15,0,-118,4,78,0,-118,4,123,0,75,6,118,
		0,-118,4,-121,0,80,4,17,0,11,6,71,0,31,4,121,0,-117,5,9,0,-117,5,55,0,35,5,-62,0,96,4,95,0,13,4,-88,0,38,6,97,0,38,4,106,0,61,4,106,0,-118,5,-61,0,2,4,-54,0,94,4,63,0,71,4,-69,0,96,4,51,0,48,3,-29,0,66,8,34,0,-118,4,-85,0,40,2,-17,0,62,2,-17,0,54,
		2,-17,0,91,2,-17,0,86,2,-17,0,58,2,-17,0,79,2,-17,0,73,3,-106,0,-113,2,-75,0,-98,3,-26,0,-118,4,58,0,30,4,-61,0,100,5,76,0,-79,5,36,0,-78,4,19,0,-110,5,61,0,-78,4,15,0,-110,4,-128,0,-118,4,124,0,96,4,80,0,-118,4,-123,0,19,1,-3,0,-97,3,-92,0,-127,0,0,-4,-92,3,-17,
		0,110,3,-13,-1,94,4,14,0,105,3,-12,0,105,3,-81,0,-118,3,-97,0,-127,3,-98,0,-127,2,-17,0,80,2,-17,0,54,2,-17,0,91,2,-17,0,86,2,-17,0,58,2,-17,0,79,2,-17,0,73,5,-127,0,126,5,-82,0,126,5,-109,0,-78,5,-32,0,126,5,-29,0,126,3,-43,0,-96,4,-126,0,-125,4,88,0,15,4,-49,0,62,
		4,107,0,101,4,46,0,74,3,-92,0,-125,1,-111,0,103,6,-92,0,96,4,-71,0,-126,1,-4,-1,-74,4,127,0,59,4,127,0,115,4,127,0,35,4,127,0,119,4,127,0,118,4,127,0,55,4,127,0,126,4,127,0,95,4,127,0,112,4,127,0,-12,2,6,-1,-76,2,4,-1,-76,1,-5,0,-101,1,-5,-1,-6,1,-5,0,-101,4,80,
		0,-118,5,0,0,120,4,32,0,59,4,125,0,-116,4,50,0,92,4,-109,0,91,4,-116,0,91,4,-98,0,90,4,-115,0,-116,4,-100,0,91,4,61,0,93,4,125,0,96,3,121,0,87,4,-42,0,103,3,-76,0,0,6,57,0,9,3,-8,0,-118,4,-69,0,96,4,-29,0,48,4,-29,0,-118,1,-5,0,0,2,53,0,37,5,93,0,7,
		5,93,0,7,4,-122,-1,-30,4,-58,0,49,2,-99,-1,-12,5,56,0,28,5,56,0,28,5,56,0,28,5,56,0,28,5,56,0,28,5,56,0,28,5,56,0,28,5,53,0,119,4,-116,0,-87,4,-116,0,-87,4,-116,0,-87,4,-116,0,-87,2,45,-1,-32,2,45,0,-80,2,45,-1,-23,2,45,-1,-43,5,-76,0,-87,5,-128,0,118,5,-128,
		0,118,5,-128,0,118,5,-128,0,118,5,-128,0,118,5,48,0,-116,5,48,0,-116,5,48,0,-116,5,48,0,-116,4,-50,0,15,4,90,0,109,4,90,0,109,4,90,0,109,4,90,0,109,4,90,0,109,4,90,0,109,4,90,0,109,4,48,0,92,4,61,0,93,4,61,0,93,4,61,0,93,4,61,0,93,1,-6,-1,-58,1,-6,0,-106,
		1,-6,-1,-49,1,-6,-1,-69,4,106,0,-116,4,-112,0,91,4,-112,0,91,4,-112,0,91,4,-112,0,91,4,-112,0,91,4,105,0,-120,4,105,0,-120,4,105,0,-120,4,105,0,-120,3,-55,0,22,3,-55,0,22,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,53,0,119,4,48,0,92,5,53,
		0,119,4,48,0,92,5,53,0,119,4,48,0,92,5,53,0,119,4,48,0,92,5,63,0,-87,5,25,0,95,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,5,115,0,122,4,125,0,96,5,115,0,122,4,125,0,96,5,115,0,122,
		4,125,0,96,5,115,0,122,4,125,0,96,5,-76,0,-87,4,104,0,-116,2,45,-1,-73,1,-6,-1,-99,2,45,-1,-52,1,-6,-1,-78,2,45,-1,-20,1,-6,-1,-46,2,45,0,24,1,-15,-1,-5,2,45,0,-87,6,-105,0,-73,3,-38,0,-115,4,106,0,53,2,3,-1,-76,5,4,0,-87,4,14,0,-115,4,78,0,-95,1,-15,0,-109,4,78,
		0,-87,1,-15,0,87,4,78,0,-87,2,-121,0,-100,4,78,0,-87,2,-51,0,-100,5,-76,0,-87,4,106,0,-116,5,-76,0,-87,4,106,0,-116,5,-76,0,-87,4,106,0,-116,4,106,-1,-68,5,-128,0,118,4,-112,0,91,5,-128,0,118,4,-112,0,91,5,-128,0,118,4,-112,0,91,4,-19,0,-88,2,-75,0,-116,4,-19,0,-88,2,-75,0,83,
		4,-19,0,-88,2,-75,0,99,4,-65,0,80,4,32,0,95,4,-65,0,80,4,32,0,95,4,-65,0,80,4,32,0,95,4,-65,0,80,4,32,0,95,4,-65,0,80,4,32,0,95,4,-58,0,49,2,-99,0,9,4,-58,0,49,2,-99,0,9,4,-58,0,49,2,-59,0,9,5,48,0,-116,4,105,0,-120,5,48,0,-116,4,105,0,-120,5,48,
		0,-116,4,105,0,-120,5,48,0,-116,4,105,0,-120,5,48,0,-116,4,105,0,-120,5,48,0,-116,4,105,0,-120,7,25,0,61,6,3,0,43,4,-50,0,15,3,-55,0,22,4,-50,0,15,4,-54,0,86,3,-9,0,88,4,-54,0,86,3,-9,0,88,4,-54,0,86,3,-9,0,88,7,122,-1,-14,6,-63,0,78,5,-128,0,118,4,-120,0,91,
		4,-128,-1,-66,4,-128,-1,-66,4,38,0,40,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,124,0,96,3,-26,0,-118,3,-26,0,-118,3,-26,0,-118,3,-26,0,-118,1,-24,-1,-66,1,-24,0,-114,1,-24,-1,-57,1,-24,-1,-77,4,-29,0,-118,4,-69,0,96,4,-69,0,96,4,-69,
		0,96,4,-69,0,96,4,-69,0,96,4,124,0,116,4,124,0,116,4,124,0,116,4,124,0,116,4,43,0,13,4,-123,0,19,4,-123,0,19,4,-123,0,19,4,124,0,96,4,124,0,96,4,124,0,96,4,124,0,96,4,-128,0,-118,3,-26,0,-118,3,-26,0,-118,3,-26,0,-118,3,-26,0,-118,3,-26,0,-118,4,-84,0,99,4,-84,0,99,
		4,-84,0,99,4,-84,0,99,4,-29,0,-118,1,-24,-1,-107,1,-24,-1,-86,1,-24,-1,-54,1,-24,0,6,1,-24,0,-120,3,-49,0,43,4,84,0,-118,3,-76,0,-126,3,-76,0,-118,3,-76,0,-118,3,-76,0,-118,4,-29,0,-118,4,-29,0,-118,4,-29,0,-118,4,-69,0,96,4,-69,0,96,4,-69,0,96,4,74,0,-118,4,74,0,-118,4,74,
		0,-118,4,32,0,67,4,32,0,67,4,32,0,67,4,32,0,67,4,38,0,40,4,38,0,40,4,38,0,40,4,124,0,116,4,124,0,116,4,124,0,116,4,124,0,116,4,124,0,116,4,124,0,116,6,21,0,49,4,43,0,13,4,43,0,13,4,35,0,71,4,35,0,71,4,35,0,71,5,56,0,28,4,-116,-1,41,5,-76,-1,55,
		2,45,-1,61,5,-108,-1,-26,5,50,-1,20,5,102,-1,-23,2,-105,-1,-101,5,56,0,28,4,-5,0,-87,4,-116,0,-87,4,-54,0,86,5,-76,0,-87,2,45,0,-73,5,4,0,-87,6,-4,0,-87,5,-76,0,-87,5,-128,0,118,5,12,0,-87,4,-58,0,49,4,-50,0,15,5,4,0,57,2,45,-1,-43,4,-50,0,15,4,-123,0,100,4,80,
		0,99,4,-120,0,-111,2,-105,0,-61,4,93,0,-113,4,115,0,-102,4,-112,0,91,4,-120,0,-102,3,-32,0,33,3,-9,0,41,2,-105,-1,-27,4,93,0,-113,4,-112,0,91,4,93,0,-113,6,-105,0,122,4,-116,0,-87,4,115,0,-79,4,-65,0,80,2,45,0,-73,2,45,-1,-43,4,106,0,53,5,36,0,-78,5,4,0,-87,5,7,0,77,
		5,56,0,28,4,-5,0,-87,4,115,0,-79,4,-116,0,-87,5,-76,0,-79,6,-4,0,-87,5,-76,0,-87,5,-128,0,118,5,-75,0,-78,5,12,0,-87,5,53,0,119,4,-58,0,49,5,4,0,57,4,90,0,109,4,61,0,93,4,-98,0,-100,4,-112,0,91,4,125,0,-116,4,48,0,92,3,-55,0,22,3,-9,0,41,4,61,0,93,3,91,
		0,-102,4,32,0,95,1,-15,0,-115,1,-6,-1,-69,1,-23,-1,-65,4,82,0,-100,3,-55,0,22,7,25,0,61,6,3,0,43,7,25,0,61,6,3,0,43,7,25,0,61,6,3,0,43,4,-50,0,15,3,-55,0,22,1,101,0,103,2,-113,0,-120,4,30,0,-96,2,3,-1,-76,1,-103,0,48,6,-4,0,-87,7,3,0,-117,5,56,0,28,
		4,90,0,109,4,-116,0,-87,5,-76,0,-79,4,61,0,93,4,-98,0,-100,5,-119,0,90,5,-102,0,95,5,10,0,22,4,3,-1,-5,8,89,0,91,9,73,0,118,4,-65,0,80,4,16,0,88,5,53,0,119,4,48,0,92,4,-50,0,15,4,2,0,46,2,45,0,-73,7,67,0,27,6,32,0,21,2,45,0,-73,5,56,0,28,4,90,
		0,109,5,56,0,28,4,90,0,109,7,122,-1,-14,6,-63,0,78,4,-116,0,-87,4,61,0,93,5,-121,0,93,4,55,0,98,4,55,0,98,7,67,0,27,6,32,0,21,4,-65,0,80,4,16,0,88,5,-76,0,-79,4,-98,0,-100,5,-76,0,-79,4,-98,0,-100,5,-128,0,118,4,-112,0,91,5,113,0,103,4,-117,0,91,5,113,0,103,
		4,-117,0,91,5,100,0,-109,4,77,0,100,5,7,0,77,3,-55,0,22,5,7,0,77,3,-55,0,22,5,7,0,77,3,-55,0,22,5,122,0,-106,4,89,0,103,6,-21,0,-78,6,54,0,-99,4,-125,0,95,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,-1,-54,5,56,
		0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,5,56,0,28,4,90,0,109,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,
		4,61,0,93,4,-116,-1,-16,4,61,-1,-70,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,4,-116,0,-87,4,61,0,93,2,45,0,-73,1,-6,0,-101,2,45,0,-93,1,-15,0,-123,5,-128,0,118,4,-112,0,91,5,-128,0,118,4,-112,0,91,5,-128,0,118,4,-112,0,91,5,-128,0,71,4,-112,-1,-60,5,-128,0,118,4,-112,
		0,91,5,-128,0,118,4,-112,0,91,5,-128,0,118,4,-112,0,91,5,126,0,101,4,-110,0,91,5,126,0,101,4,-110,0,91,5,126,0,101,4,-110,0,91,5,126,0,101,4,-110,0,91,5,126,0,101,4,-110,0,91,5,48,0,-116,4,105,0,-120,5,48,0,-116,4,105,0,-120,5,-112,0,-116,4,-13,0,-120,5,-112,0,-116,4,-13,0,-120,
		5,-112,0,-116,4,-13,0,-120,5,-112,0,-116,4,-13,0,-120,5,-112,0,-116,4,-13,0,-120,4,-50,0,15,3,-55,0,22,4,-50,0,15,3,-55,0,22,4,-50,0,15,3,-55,0,22,4,-95,0,95,4,-58,0,49,3,-40,0,40,5,122,0,-106,4,89,0,103,4,115,0,-79,3,91,0,-102,6,47,0,63,4,-67,-1,-34,4,104,0,-116,5,5,
		-1,-44,5,5,-1,-44,4,115,0,3,3,91,-1,-4,5,56,-1,-9,4,39,-1,-65,4,-50,0,15,4,2,0,46,5,4,0,57,3,-9,0,41,4,80,0,99,4,108,0,18,6,63,0,-112,4,126,0,93,4,126,0,94,4,126,0,53,4,126,0,-102,4,-110,0,-104,4,-90,0,-124,4,-110,0,100,4,-90,0,-121,5,115,0,122,4,125,0,96,
		5,-76,0,-87,4,106,0,-116,5,56,0,28,4,90,0,57,4,-116,0,95,4,61,0,41,2,45,-1,10,1,-6,-2,-16,5,-128,0,118,4,-112,0,51,4,-19,0,85,2,-75,-1,-117,5,48,0,-116,4,105,0,43,4,-90,-2,-42,4,-5,0,-87,4,125,0,-116,5,63,0,-87,4,-125,0,95,5,63,0,-87,4,-125,0,95,5,-76,0,-87,4,104,
		0,-116,5,4,0,-87,4,14,0,-115,5,4,0,-87,4,14,0,-115,4,78,0,-87,1,-15,0,-122,6,-4,0,-87,7,3,0,-117,5,-76,0,-87,4,106,0,-116,5,-128,0,118,5,12,0,-87,4,125,0,-116,4,-19,0,-88,2,-75,0,-126,4,-65,0,80,4,32,0,95,4,-58,0,49,2,-99,0,9,5,48,0,-116,5,23,0,28,3,-32,0,33,
		5,23,0,28,3,-32,0,33,7,25,0,61,6,3,0,43,4,-54,0,86,3,-9,0,88,5,-58,-2,50,4,-123,0,19,4,34,-1,99,5,31,-1,-128,2,36,-1,-124,4,-59,-1,-43,4,103,-1,27,4,-4,-1,-18,4,-123,0,19,4,80,0,-118,3,-26,0,-118,4,35,0,71,4,-29,0,-118,1,-24,0,-105,4,84,0,-118,6,2,0,-118,4,-29,
		0,-118,4,-69,0,96,4,92,0,-118,4,38,0,40,4,43,0,13,4,84,0,38,1,-24,-1,-77,4,43,0,13,3,-26,0,-118,3,-81,0,-118,4,32,0,67,1,-24,0,-105,1,-24,-1,-77,3,-49,0,43,4,84,0,-118,4,31,0,34,4,-123,0,19,4,80,0,-118,3,-81,0,-118,3,-26,0,-118,4,-20,0,-118,6,2,0,-118,4,-29,0,-118,
		4,-69,0,96,4,-50,0,-118,4,92,0,-118,4,124,0,96,4,38,0,40,4,84,0,38,4,63,0,71,4,-29,0,-118,4,124,0,96,4,43,0,13,5,-61,0,2,4,-20,0,-118,4,31,0,34,5,103,0,96,5,-73,0,-105,6,57,0,9,4,-69,0,96,4,32,0,67,6,21,0,49,6,21,0,49,6,21,0,49,4,43,0,13,5,56,
		0,28,4,90,0,109,4,-116,0,-87,4,61,0,93,4,-123,0,19,3,-26,0,-118,1,-6,0,-123,0,0,0,1,0,0,5,16,9,10,4,0,0,2,2,2,3,6,5,7,6,2,3,3,4,5,2,2,2,4,5,5,5,5,5,5,5,5,5,5,2,2,5,5,5,4,8,6,6,6,6,5,5,6,6,2,5,6,5,8,6,6,6,6,
		6,5,5,6,6,8,6,5,5,2,4,2,4,4,3,5,5,5,5,5,3,5,5,2,2,5,2,8,5,5,5,5,3,5,3,5,4,7,4,4,4,3,2,3,6,2,5,5,6,5,2,6,4,7,4,4,5,7,4,3,5,3,3,3,5,4,2,2,3,4,4,7,7,7,4,8,5,6,5,5,8,5,5,5,5,5,6,2,5,5,
		2,6,5,9,8,2,6,3,6,5,6,6,2,5,4,4,4,4,2,3,2,4,3,3,0,0,0,0,0,0,2,5,2,5,6,6,6,5,6,5,6,6,6,5,5,5,5,5,5,5,5,3,5,4,5,5,5,5,5,5,6,6,7,5,5,7,7,6,10,10,7,6,6,7,8,5,6,6,6,7,7,6,8,9,7,8,6,6,8,6,
		5,5,4,5,7,5,5,5,5,7,5,5,4,7,5,5,7,7,6,7,5,5,7,5,5,5,8,8,5,5,8,7,5,8,7,5,5,8,7,8,7,10,9,5,4,6,5,6,5,6,5,8,7,8,7,6,5,6,0,0,0,0,0,0,7,6,5,6,5,5,4,5,5,9,7,6,5,6,5,7,6,7,5,9,6,9,8,7,5,6,
		5,8,6,6,5,6,5,6,7,5,6,5,7,6,6,5,7,6,8,7,6,5,5,5,4,6,5,6,7,8,7,6,5,5,9,7,9,7,6,5,6,6,6,7,6,4,5,9,5,9,3,2,2,5,2,2,1,1,0,2,2,6,7,4,2,2,2,2,3,3,3,5,5,3,4,6,2,9,3,3,4,3,4,5,7,7,10,7,7,5,
		7,5,5,6,6,7,4,9,6,6,7,8,8,7,5,6,5,5,5,9,2,5,5,5,5,5,3,3,2,6,5,5,8,8,6,7,0,9,9,3,3,3,5,5,5,5,5,5,5,5,5,5,5,5,5,7,5,5,5,5,5,5,5,5,6,7,4,5,4,2,6,5,4,5,5,4,4,5,5,5,4,5,4,6,6,6,6,5,8,8,
		6,5,5,6,7,5,6,5,5,5,6,5,7,8,6,7,5,5,7,5,5,7,5,6,6,6,5,5,7,5,5,6,5,5,5,5,4,9,5,3,3,3,3,3,3,3,4,3,4,5,5,6,6,5,6,5,5,5,5,5,2,4,0,4,4,5,4,4,4,4,3,3,3,3,3,3,3,6,6,6,7,7,4,5,5,5,5,5,4,2,
		7,5,2,5,5,5,5,5,5,5,5,5,5,2,2,2,2,2,5,6,5,5,5,5,5,5,5,5,5,5,4,5,4,7,4,5,6,6,2,2,6,6,5,5,3,6,6,6,6,6,6,6,6,5,5,5,5,2,2,2,2,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,2,2,2,2,5,5,
		5,5,5,5,5,5,5,5,4,4,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,6,5,5,5,5,5,5,5,5,5,5,6,5,6,5,6,5,6,5,6,5,2,2,2,2,2,2,2,2,2,7,4,5,2,6,5,5,2,5,2,5,3,5,3,6,5,6,5,6,5,5,6,5,6,5,6,5,6,3,6,3,6,3,5,5,
		5,5,5,5,5,5,5,5,5,3,5,3,5,3,6,5,6,5,6,5,6,5,6,5,6,5,8,7,5,4,5,5,4,5,4,5,4,8,8,6,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,2,2,2,2,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,5,5,5,5,6,2,
		2,2,2,2,4,5,4,4,4,4,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,7,5,5,5,5,5,6,5,6,2,6,6,6,3,6,6,5,5,6,2,6,8,6,6,6,5,5,6,2,5,5,5,5,3,5,5,5,5,4,4,3,5,5,5,7,5,5,5,2,2,5,6,6,6,6,6,5,5,
		6,8,6,6,6,6,6,5,6,5,5,5,5,5,5,4,4,5,4,5,2,2,2,5,4,8,7,8,7,8,7,5,4,2,3,5,2,2,8,8,6,5,5,6,5,5,6,6,6,5,9,10,5,5,6,5,5,5,2,8,7,2,6,5,6,5,8,8,5,5,6,5,5,8,7,5,5,6,5,6,5,6,5,6,5,6,5,6,5,6,
		4,6,4,6,4,6,5,8,7,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,2,2,2,2,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,6,6,6,6,6,6,6,
		6,6,5,4,5,4,5,4,5,5,4,6,5,5,4,7,5,5,6,6,5,4,6,5,5,5,6,4,5,5,7,5,5,5,5,5,5,5,5,6,5,6,5,6,5,5,5,2,2,6,5,6,3,6,5,5,6,5,6,5,6,5,6,5,6,5,6,5,5,2,8,8,6,5,6,6,5,6,3,5,5,5,3,6,6,4,6,4,8,7,
		5,4,7,5,5,6,2,5,5,6,5,5,4,5,6,2,5,7,6,5,5,5,5,5,2,5,4,4,5,2,2,4,5,5,5,5,4,4,6,7,6,5,5,5,5,5,5,5,6,5,5,6,6,5,6,6,7,5,5,7,7,7,5,6,5,5,5,5,4,2,0,0,0,3,0,0,0,3,0,0,0,28,0,3,0,1,0,0,0,28,
		0,3,0,10,0,0,6,-118,0,4,6,110,0,0,0,-12,0,-128,0,6,0,116,0,0,0,2,0,13,0,126,0,-96,0,-84,0,-83,0,-65,0,-58,0,-49,0,-26,0,-17,0,-2,1,15,1,17,1,37,1,39,1,48,1,83,1,95,1,103,1,126,1,127,1,-113,1,-110,1,-95,1,-80,1,-16,1,-1,2,27,2,55,2,89,2,-68,2,-57,
		2,-55,2,-35,2,-13,3,1,3,3,3,9,3,15,3,35,3,-118,3,-116,3,-110,3,-95,3,-80,3,-71,3,-55,3,-50,3,-46,3,-42,4,37,4,47,4,69,4,79,4,98,4,111,4,121,4,-122,4,-97,4,-87,4,-79,4,-70,4,-50,4,-41,4,-31,4,-11,5,1,5,16,5,19,30,1,30,63,30,-123,30,-15,30,-13,30,-7,31,77,32,9,
		32,11,32,17,32,21,32,30,32,34,32,39,32,48,32,51,32,58,32,60,32,68,32,116,32,127,32,-92,32,-86,32,-84,32,-79,32,-70,32,-67,33,5,33,19,33,22,33,34,33,38,33,46,33,94,34,2,34,6,34,15,34,18,34,26,34,30,34,43,34,72,34,96,34,101,37,-54,-18,2,-10,-61,-5,4,-2,-1,-1,-3,-1,-1,0,0,0,0,
		0,2,0,13,0,32,0,-96,0,-95,0,-83,0,-82,0,-64,0,-57,0,-48,0,-25,0,-16,0,-1,1,16,1,18,1,38,1,40,1,49,1,84,1,96,1,104,1,127,1,-113,1,-110,1,-96,1,-81,1,-16,1,-6,2,24,2,55,2,89,2,-68,2,-58,2,-55,2,-40,2,-13,3,0,3,3,3,9,3,15,3,35,3,-124,3,-116,3,-114,3,-109,
		3,-93,3,-79,3,-70,3,-54,3,-47,3,-42,4,0,4,38,4,48,4,70,4,80,4,99,4,112,4,122,4,-120,4,-96,4,-86,4,-78,4,-69,4,-49,4,-40,4,-30,4,-10,5,2,5,17,30,0,30,62,30,-128,30,-96,30,-14,30,-12,31,77,32,0,32,10,32,16,32,19,32,23,32,32,32,37,32,48,32,50,32,57,32,60,32,68,32,116,
		32,127,32,-93,32,-90,32,-85,32,-79,32,-71,32,-68,33,5,33,19,33,22,33,34,33,38,33,46,33,91,34,2,34,6,34,15,34,17,34,26,34,30,34,43,34,72,34,96,34,100,37,-54,-18,1,-10,-61,-5,1,-2,-1,-1,-4,-1,-1,0,1,0,0,-1,-10,-1,-28,1,-40,-1,-62,1,-52,-1,-63,0,0,1,-65,0,0,1,-70,0,0,1,-74,
		0,0,1,-76,0,0,1,-78,0,0,1,-86,0,0,1,-84,-1,22,-1,7,-1,5,-2,-8,-2,-21,1,-18,0,0,0,0,-2,101,-2,68,1,35,-3,-40,-3,-41,-3,-55,-3,-76,-3,-88,-3,-89,-3,-94,-3,-99,-3,-118,0,0,-1,-2,-1,-3,0,0,0,0,-3,10,0,0,-1,-34,-4,-2,-4,-5,0,0,-4,-70,0,0,-4,-78,0,0,-4,-89,0,0,
		-4,-95,0,0,-4,-103,0,0,-4,-111,0,0,-1,40,0,0,-1,37,0,0,-4,94,0,0,-27,-30,-27,-94,-27,83,-27,126,-28,-25,-27,124,-27,125,-31,114,-31,115,-31,111,0,0,-31,108,-31,107,-31,105,-31,97,-29,-87,-31,89,-29,-95,-31,80,-31,33,-31,23,0,0,-32,-14,0,0,-32,-19,-32,-26,-32,-27,-32,-98,-32,-111,-32,-113,-32,-124,-33,-108,-32,121,
		-32,77,-33,-86,-34,-84,-33,-98,-33,-99,-33,-106,-33,-109,-33,-121,-33,107,-33,84,-33,81,-37,-19,19,-73,10,-9,6,-69,2,-61,1,-57,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-28,0,0,0,-18,0,0,1,24,0,0,1,50,0,0,1,50,0,0,1,50,0,0,1,116,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,116,1,126,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,108,0,0,0,0,1,116,1,-112,0,0,1,-88,0,0,0,0,0,0,1,-64,0,0,2,8,0,0,2,48,0,0,2,82,0,0,2,98,0,0,2,-114,0,0,2,-102,0,0,2,-66,0,0,2,-50,0,0,2,-30,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,-46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,-62,0,0,2,-62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,2,127,2,-128,2,-127,2,-126,2,-125,2,-124,0,-127,2,123,2,-113,2,-112,2,-111,2,-110,2,-109,2,-108,0,-126,0,-125,2,-107,2,-106,2,-105,2,-104,2,-103,0,-124,0,-123,2,-102,2,-101,2,-100,2,-99,2,-98,2,-97,0,-122,0,-121,2,-86,2,-85,2,-84,2,-83,2,-82,2,-81,0,-120,0,-119,2,-80,
		2,-79,2,-78,2,-77,2,-76,0,-118,2,122,0,-117,0,-116,2,124,0,-115,2,-29,2,-28,2,-27,2,-26,2,-25,2,-24,0,-114,2,-23,2,-22,2,-21,2,-20,2,-19,2,-18,2,-17,2,-16,0,-113,0,-112,2,-15,2,-14,2,-13,2,-12,2,-11,2,-10,2,-9,0,-111,0,-110,2,-8,2,-7,2,-6,2,-5,2,-4,2,-3,0,-109,0,-108,3,12,
		3,13,3,16,3,17,3,18,3,19,2,125,2,126,2,-123,2,-96,3,43,3,44,3,45,3,46,3,10,3,11,3,14,3,15,0,-82,0,-81,3,-122,0,-80,3,-121,3,-120,3,-119,0,-79,0,-78,3,-112,3,-111,3,-110,0,-77,3,-109,3,-108,0,-76,3,-107,3,-106,0,-75,3,-105,0,-74,3,-104,0,-73,3,-103,3,-102,0,-72,3,-101,0,-71,
		0,-70,3,-100,3,-99,3,-98,3,-97,3,-96,3,-95,3,-94,3,-93,0,-60,3,-91,3,-90,0,-59,3,-92,0,-58,0,-57,0,-56,0,-55,0,-54,0,-53,0,-52,3,-89,0,-51,0,-50,3,-28,3,-83,0,-46,3,-82,0,-45,3,-81,3,-80,3,-79,3,-78,0,-44,0,-43,0,-42,3,-76,3,-27,3,-75,0,-41,3,-74,0,-40,3,-73,3,-72,0,-39,
		3,-71,0,-38,0,-37,0,-36,3,-70,3,-77,0,-35,3,-69,3,-68,3,-67,3,-66,3,-65,3,-64,3,-63,0,-34,0,-33,3,-62,3,-61,0,-22,0,-21,0,-20,0,-19,3,-60,0,-18,0,-17,0,-16,3,-59,0,-15,0,-14,0,-13,0,-12,3,-58,0,-11,3,-57,3,-56,0,-10,3,-55,0,-9,3,-54,3,-26,3,-53,1,2,3,-52,1,3,3,-51,
		3,-50,3,-49,3,-48,1,4,1,5,1,6,3,-47,3,-25,3,-46,1,7,1,8,1,9,4,-127,3,-24,3,-23,1,23,1,24,1,25,1,26,3,-22,3,-21,3,-19,3,-20,1,40,1,41,1,42,1,43,4,-128,1,44,1,45,1,46,1,47,1,48,4,-126,4,-125,1,49,1,50,1,51,1,52,3,-18,3,-17,1,53,1,54,1,55,1,56,
		4,-124,4,-123,3,-16,3,-15,4,119,4,120,3,-14,3,-13,4,-122,4,-121,4,127,1,76,1,77,4,125,4,126,3,-12,3,-11,3,-10,1,78,1,79,1,80,1,81,1,82,1,83,1,84,1,85,4,121,4,122,1,86,1,87,1,88,4,1,4,0,4,2,4,3,4,4,4,5,4,6,1,89,1,90,4,123,4,124,4,27,4,28,1,91,
		1,92,1,93,1,94,4,-120,4,-119,1,95,4,29,4,-118,1,111,1,112,1,-127,1,-126,4,-116,4,-117,1,-105,4,118,1,-99,0,12,0,0,0,0,11,-68,0,0,0,0,0,0,0,-7,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,13,0,0,0,13,0,0,0,3,0,0,0,32,
		0,0,0,126,0,0,0,4,0,0,0,-96,0,0,0,-96,0,0,2,120,0,0,0,-95,0,0,0,-84,0,0,0,99,0,0,0,-83,0,0,0,-83,0,0,2,121,0,0,0,-82,0,0,0,-65,0,0,0,111,0,0,0,-64,0,0,0,-59,0,0,2,127,0,0,0,-58,0,0,0,-58,0,0,0,-127,0,0,0,-57,0,0,0,-49,0,0,
		2,-122,0,0,0,-48,0,0,0,-48,0,0,2,123,0,0,0,-47,0,0,0,-42,0,0,2,-113,0,0,0,-41,0,0,0,-40,0,0,0,-126,0,0,0,-39,0,0,0,-35,0,0,2,-107,0,0,0,-34,0,0,0,-33,0,0,0,-124,0,0,0,-32,0,0,0,-27,0,0,2,-102,0,0,0,-26,0,0,0,-26,0,0,0,-122,0,0,0,-25,
		0,0,0,-17,0,0,2,-95,0,0,0,-16,0,0,0,-16,0,0,0,-121,0,0,0,-15,0,0,0,-10,0,0,2,-86,0,0,0,-9,0,0,0,-8,0,0,0,-120,0,0,0,-7,0,0,0,-3,0,0,2,-80,0,0,0,-2,0,0,0,-2,0,0,0,-118,0,0,0,-1,0,0,1,15,0,0,2,-75,0,0,1,16,0,0,1,16,0,0,
		2,122,0,0,1,17,0,0,1,17,0,0,0,-117,0,0,1,18,0,0,1,37,0,0,2,-58,0,0,1,38,0,0,1,38,0,0,0,-116,0,0,1,39,0,0,1,39,0,0,2,124,0,0,1,40,0,0,1,48,0,0,2,-38,0,0,1,49,0,0,1,49,0,0,0,-115,0,0,1,50,0,0,1,55,0,0,2,-29,0,0,1,56,
		0,0,1,56,0,0,0,-114,0,0,1,57,0,0,1,64,0,0,2,-23,0,0,1,65,0,0,1,66,0,0,0,-113,0,0,1,67,0,0,1,73,0,0,2,-15,0,0,1,74,0,0,1,75,0,0,0,-111,0,0,1,76,0,0,1,81,0,0,2,-8,0,0,1,82,0,0,1,83,0,0,0,-109,0,0,1,84,0,0,1,95,0,0,
		2,-2,0,0,1,96,0,0,1,97,0,0,3,12,0,0,1,98,0,0,1,101,0,0,3,16,0,0,1,102,0,0,1,103,0,0,2,125,0,0,1,104,0,0,1,126,0,0,3,20,0,0,1,127,0,0,1,127,0,0,0,-107,0,0,1,-113,0,0,1,-113,0,0,0,-106,0,0,1,-110,0,0,1,-110,0,0,0,-105,0,0,1,-96,
		0,0,1,-95,0,0,0,-104,0,0,1,-81,0,0,1,-80,0,0,0,-102,0,0,1,-16,0,0,1,-16,0,0,3,-34,0,0,1,-6,0,0,1,-6,0,0,2,-123,0,0,1,-5,0,0,1,-5,0,0,2,-96,0,0,1,-4,0,0,1,-1,0,0,3,43,0,0,2,24,0,0,2,25,0,0,3,10,0,0,2,26,0,0,2,27,0,0,
		3,14,0,0,2,55,0,0,2,55,0,0,0,-100,0,0,2,89,0,0,2,89,0,0,0,-99,0,0,2,-68,0,0,2,-68,0,0,3,-33,0,0,2,-58,0,0,2,-57,0,0,0,-98,0,0,2,-55,0,0,2,-55,0,0,0,-96,0,0,2,-40,0,0,2,-35,0,0,0,-95,0,0,2,-13,0,0,2,-13,0,0,0,-89,0,0,3,0,
		0,0,3,1,0,0,0,-88,0,0,3,3,0,0,3,3,0,0,0,-86,0,0,3,9,0,0,3,9,0,0,0,-85,0,0,3,15,0,0,3,15,0,0,0,-84,0,0,3,35,0,0,3,35,0,0,0,-83,0,0,3,-124,0,0,3,-123,0,0,0,-82,0,0,3,-122,0,0,3,-122,0,0,3,-122,0,0,3,-121,0,0,3,-121,0,0,
		0,-80,0,0,3,-120,0,0,3,-118,0,0,3,-121,0,0,3,-116,0,0,3,-116,0,0,3,-118,0,0,3,-114,0,0,3,-110,0,0,3,-117,0,0,3,-109,0,0,3,-108,0,0,0,-79,0,0,3,-107,0,0,3,-105,0,0,3,-112,0,0,3,-104,0,0,3,-104,0,0,0,-77,0,0,3,-103,0,0,3,-102,0,0,3,-109,0,0,3,-101,
		0,0,3,-101,0,0,0,-76,0,0,3,-100,0,0,3,-99,0,0,3,-107,0,0,3,-98,0,0,3,-98,0,0,0,-75,0,0,3,-97,0,0,3,-97,0,0,3,-105,0,0,3,-96,0,0,3,-96,0,0,0,-74,0,0,3,-95,0,0,3,-95,0,0,3,-104,0,0,3,-93,0,0,3,-93,0,0,0,-73,0,0,3,-92,0,0,3,-91,0,0,
		3,-103,0,0,3,-90,0,0,3,-90,0,0,0,-72,0,0,3,-89,0,0,3,-89,0,0,3,-101,0,0,3,-88,0,0,3,-87,0,0,0,-71,0,0,3,-86,0,0,3,-80,0,0,3,-100,0,0,3,-79,0,0,3,-71,0,0,0,-69,0,0,3,-70,0,0,3,-70,0,0,3,-93,0,0,3,-69,0,0,3,-69,0,0,0,-60,0,0,3,-68,
		0,0,3,-67,0,0,3,-91,0,0,3,-66,0,0,3,-66,0,0,0,-59,0,0,3,-65,0,0,3,-65,0,0,3,-92,0,0,3,-64,0,0,3,-58,0,0,0,-58,0,0,3,-57,0,0,3,-57,0,0,3,-89,0,0,3,-56,0,0,3,-55,0,0,0,-51,0,0,3,-54,0,0,3,-50,0,0,3,-88,0,0,3,-47,0,0,3,-46,0,0,
		0,-49,0,0,3,-42,0,0,3,-42,0,0,0,-47,0,0,4,0,0,0,4,0,0,0,3,-28,0,0,4,1,0,0,4,1,0,0,3,-83,0,0,4,2,0,0,4,2,0,0,0,-46,0,0,4,3,0,0,4,3,0,0,3,-82,0,0,4,4,0,0,4,4,0,0,0,-45,0,0,4,5,0,0,4,8,0,0,3,-81,0,0,4,9,
		0,0,4,11,0,0,0,-44,0,0,4,12,0,0,4,12,0,0,3,-76,0,0,4,13,0,0,4,13,0,0,3,-27,0,0,4,14,0,0,4,14,0,0,3,-75,0,0,4,15,0,0,4,15,0,0,0,-41,0,0,4,16,0,0,4,16,0,0,3,-74,0,0,4,17,0,0,4,17,0,0,0,-40,0,0,4,18,0,0,4,19,0,0,
		3,-73,0,0,4,20,0,0,4,20,0,0,0,-39,0,0,4,21,0,0,4,21,0,0,3,-71,0,0,4,22,0,0,4,24,0,0,0,-38,0,0,4,25,0,0,4,25,0,0,3,-70,0,0,4,26,0,0,4,26,0,0,3,-77,0,0,4,27,0,0,4,27,0,0,0,-35,0,0,4,28,0,0,4,34,0,0,3,-69,0,0,4,35,
		0,0,4,36,0,0,0,-34,0,0,4,37,0,0,4,37,0,0,3,-62,0,0,4,38,0,0,4,47,0,0,0,-32,0,0,4,48,0,0,4,48,0,0,3,-61,0,0,4,49,0,0,4,52,0,0,0,-22,0,0,4,53,0,0,4,53,0,0,3,-60,0,0,4,54,0,0,4,56,0,0,0,-18,0,0,4,57,0,0,4,57,0,0,
		3,-59,0,0,4,58,0,0,4,61,0,0,0,-15,0,0,4,62,0,0,4,62,0,0,3,-58,0,0,4,63,0,0,4,63,0,0,0,-11,0,0,4,64,0,0,4,65,0,0,3,-57,0,0,4,66,0,0,4,66,0,0,0,-10,0,0,4,67,0,0,4,67,0,0,3,-55,0,0,4,68,0,0,4,68,0,0,0,-9,0,0,4,69,
		0,0,4,69,0,0,3,-54,0,0,4,70,0,0,4,79,0,0,0,-8,0,0,4,80,0,0,4,80,0,0,3,-26,0,0,4,81,0,0,4,81,0,0,3,-53,0,0,4,82,0,0,4,82,0,0,1,2,0,0,4,83,0,0,4,83,0,0,3,-52,0,0,4,84,0,0,4,84,0,0,1,3,0,0,4,85,0,0,4,88,0,0,
		3,-51,0,0,4,89,0,0,4,91,0,0,1,4,0,0,4,92,0,0,4,92,0,0,3,-47,0,0,4,93,0,0,4,93,0,0,3,-25,0,0,4,94,0,0,4,94,0,0,3,-46,0,0,4,95,0,0,4,97,0,0,1,7,0,0,4,98,0,0,4,98,0,0,4,-127,0,0,4,99,0,0,4,111,0,0,1,10,0,0,4,112,
		0,0,4,113,0,0,3,-24,0,0,4,114,0,0,4,117,0,0,1,23,0,0,4,118,0,0,4,119,0,0,3,-22,0,0,4,120,0,0,4,120,0,0,3,-19,0,0,4,121,0,0,4,121,0,0,3,-20,0,0,4,122,0,0,4,-122,0,0,1,27,0,0,4,-120,0,0,4,-117,0,0,1,40,0,0,4,-116,0,0,4,-116,0,0,
		4,-128,0,0,4,-115,0,0,4,-111,0,0,1,44,0,0,4,-110,0,0,4,-109,0,0,4,-126,0,0,4,-108,0,0,4,-105,0,0,1,49,0,0,4,-104,0,0,4,-103,0,0,3,-18,0,0,4,-102,0,0,4,-99,0,0,1,53,0,0,4,-98,0,0,4,-97,0,0,4,-124,0,0,4,-96,0,0,4,-87,0,0,1,57,0,0,4,-86,
		0,0,4,-85,0,0,3,-16,0,0,4,-84,0,0,4,-83,0,0,4,119,0,0,4,-82,0,0,4,-81,0,0,3,-14,0,0,4,-80,0,0,4,-79,0,0,4,-122,0,0,4,-78,0,0,4,-70,0,0,1,67,0,0,4,-69,0,0,4,-69,0,0,4,127,0,0,4,-68,0,0,4,-67,0,0,1,76,0,0,4,-66,0,0,4,-65,0,0,
		4,125,0,0,4,-64,0,0,4,-62,0,0,3,-12,0,0,4,-61,0,0,4,-54,0,0,1,78,0,0,4,-53,0,0,4,-52,0,0,4,121,0,0,4,-51,0,0,4,-50,0,0,1,86,0,0,4,-49,0,0,4,-41,0,0,3,-9,0,0,4,-40,0,0,4,-40,0,0,1,88,0,0,4,-39,0,0,4,-39,0,0,4,1,0,0,4,-38,
		0,0,4,-38,0,0,4,0,0,0,4,-37,0,0,4,-33,0,0,4,2,0,0,4,-32,0,0,4,-31,0,0,1,89,0,0,4,-30,0,0,4,-11,0,0,4,7,0,0,4,-10,0,0,4,-9,0,0,4,123,0,0,4,-8,0,0,4,-7,0,0,4,27,0,0,4,-6,0,0,4,-3,0,0,1,91,0,0,4,-2,0,0,4,-1,0,0,
		4,-120,0,0,5,0,0,0,5,0,0,0,1,95,0,0,5,1,0,0,5,1,0,0,4,29,0,0,5,2,0,0,5,16,0,0,1,96,0,0,5,17,0,0,5,17,0,0,4,-118,0,0,5,18,0,0,5,19,0,0,1,111,0,0,30,0,0,0,30,1,0,0,3,-30,0,0,30,62,0,0,30,63,0,0,3,-32,0,0,30,-128,
		0,0,30,-123,0,0,3,-45,0,0,30,-96,0,0,30,-15,0,0,4,30,0,0,30,-14,0,0,30,-13,0,0,3,-39,0,0,30,-12,0,0,30,-7,0,0,4,112,0,0,31,77,0,0,31,77,0,0,4,-54,0,0,32,0,0,0,32,9,0,0,1,114,0,0,32,10,0,0,32,11,0,0,1,125,0,0,32,16,0,0,32,17,0,0,
		1,127,0,0,32,19,0,0,32,20,0,0,1,-127,0,0,32,21,0,0,32,21,0,0,4,-116,0,0,32,23,0,0,32,30,0,0,1,-125,0,0,32,32,0,0,32,34,0,0,1,-117,0,0,32,37,0,0,32,39,0,0,1,-114,0,0,32,48,0,0,32,48,0,0,1,-111,0,0,32,50,0,0,32,51,0,0,3,-37,0,0,32,57,
		0,0,32,58,0,0,1,-110,0,0,32,60,0,0,32,60,0,0,3,-35,0,0,32,68,0,0,32,68,0,0,1,-108,0,0,32,116,0,0,32,116,0,0,1,-107,0,0,32,127,0,0,32,127,0,0,1,-106,0,0,32,-93,0,0,32,-93,0,0,4,-117,0,0,32,-92,0,0,32,-92,0,0,1,-105,0,0,32,-90,0,0,32,-86,0,0,
		1,-104,0,0,32,-85,0,0,32,-85,0,0,4,118,0,0,32,-84,0,0,32,-84,0,0,1,-99,0,0,32,-79,0,0,32,-79,0,0,1,-98,0,0,32,-71,0,0,32,-70,0,0,1,-97,0,0,32,-68,0,0,32,-67,0,0,1,-95,0,0,33,5,0,0,33,5,0,0,1,-93,0,0,33,19,0,0,33,19,0,0,1,-92,0,0,33,22,
		0,0,33,22,0,0,1,-91,0,0,33,34,0,0,33,34,0,0,1,-90,0,0,33,38,0,0,33,38,0,0,0,-70,0,0,33,46,0,0,33,46,0,0,1,-89,0,0,33,91,0,0,33,94,0,0,1,-88,0,0,34,2,0,0,34,2,0,0,1,-84,0,0,34,6,0,0,34,6,0,0,0,-78,0,0,34,15,0,0,34,15,0,0,
		1,-83,0,0,34,17,0,0,34,18,0,0,1,-82,0,0,34,26,0,0,34,26,0,0,1,-80,0,0,34,30,0,0,34,30,0,0,1,-79,0,0,34,43,0,0,34,43,0,0,1,-78,0,0,34,72,0,0,34,72,0,0,1,-77,0,0,34,96,0,0,34,96,0,0,1,-76,0,0,34,100,0,0,34,101,0,0,1,-75,0,0,37,-54,
		0,0,37,-54,0,0,1,-73,0,0,-18,1,0,0,-18,2,0,0,1,-72,0,0,-10,-61,0,0,-10,-61,0,0,1,-70,0,0,-5,1,0,0,-5,4,0,0,1,-68,0,0,-2,-1,0,0,-2,-1,0,0,1,-62,0,0,-1,-4,0,0,-1,-3,0,0,1,-61,0,0,-80,0,44,75,-80,9,80,88,-79,1,1,-114,89,-72,1,-1,-123,-80,-124,29,
		-79,9,3,95,94,45,-80,1,44,32,32,69,105,68,-80,1,96,45,-80,2,44,-80,1,42,33,45,-80,3,44,32,70,-80,3,37,70,82,88,35,89,32,-118,32,-118,73,100,-118,32,70,32,104,97,100,-80,4,37,70,32,104,97,100,82,88,35,101,-118,89,47,32,-80,0,83,88,105,32,-80,0,84,88,33,-80,64,89,27,105,32,-80,0,84,88,33,
		-80,64,101,89,89,58,45,-80,4,44,32,70,-80,4,37,70,82,88,35,-118,89,32,70,32,106,97,100,-80,4,37,70,32,106,97,100,82,88,35,-118,89,47,-3,45,-80,5,44,75,32,-80,3,38,80,88,81,88,-80,-128,68,27,-80,64,68,89,27,33,33,32,69,-80,-64,80,88,-80,-64,68,27,33,89,89,45,-80,6,44,32,32,69,105,68,-80,1,
		96,32,32,69,125,105,24,68,-80,1,96,45,-80,7,44,-80,6,42,45,-80,8,44,75,32,-80,3,38,83,88,-80,64,27,-80,0,89,-118,-118,32,-80,3,38,83,88,35,33,-80,-128,-118,-118,27,-118,35,89,32,-80,3,38,83,88,35,33,-80,-64,-118,-118,27,-118,35,89,32,-80,3,38,83,88,35,33,-72,1,0,-118,-118,27,-118,35,89,32,-80,3,38,
		83,88,35,33,-72,1,64,-118,-118,27,-118,35,89,32,-80,3,38,83,88,-80,3,37,69,-72,1,-128,80,88,35,33,-72,1,-128,35,33,27,-80,3,37,69,35,33,35,33,89,27,33,89,68,45,-80,9,44,75,83,88,69,68,27,33,33,89,45,-80,10,44,-80,40,69,45,-80,11,44,-80,41,69,45,-80,12,44,-79,39,1,-120,32,-118,83,88,-71,64,
		0,4,0,99,-72,8,0,-120,84,88,-71,0,40,3,-24,112,89,27,-80,35,83,88,-80,32,-120,-72,16,0,84,88,-71,0,40,3,-24,112,89,89,89,45,-80,13,44,-80,64,-120,-72,32,0,90,88,-79,41,0,68,27,-71,0,41,3,-24,68,89,45,-80,12,43,-80,0,43,0,-78,1,16,2,43,1,-78,17,1,2,43,1,-73,17,58,48,37,27,16,
		0,8,43,0,-73,1,72,59,46,33,20,0,8,43,-73,2,88,72,56,40,20,0,8,43,-73,3,82,67,52,37,22,0,8,43,-73,4,94,77,60,43,25,0,8,43,-73,5,54,44,34,25,15,0,8,43,-73,6,113,93,70,50,27,0,8,43,-73,7,-111,119,92,58,35,0,8,43,-73,8,126,103,80,57,26,0,8,43,-73,9,84,69,54,38,
		20,0,8,43,-73,10,118,96,75,54,29,0,8,43,-73,11,-125,100,78,58,35,0,8,43,-73,12,-39,-78,-118,99,60,0,8,43,-73,13,20,16,12,9,6,0,8,43,-73,14,60,50,39,28,17,0,8,43,-73,15,64,52,41,29,20,0,8,43,-73,16,80,65,46,33,20,0,8,43,0,-78,18,11,7,43,-80,0,32,69,125,105,24,68,-78,63,
		26,1,115,-78,95,26,1,115,-78,127,26,1,115,-78,47,26,1,116,-78,79,26,1,116,-78,111,26,1,116,-78,-113,26,1,116,-78,-81,26,1,116,-78,-1,26,1,116,-78,31,26,1,117,-78,63,26,1,117,-78,95,26,1,117,-78,127,26,1,117,-78,15,30,1,115,-78,127,30,1,115,-78,-17,30,1,115,-78,31,30,1,116,-78,95,30,1,116,-78,-113,
		30,1,116,-78,-49,30,1,116,-78,-1,30,1,116,-78,63,30,1,117,-78,111,30,1,117,-78,47,32,1,115,-78,111,32,1,115,0,0,0,0,42,0,-99,0,-128,0,-118,0,120,0,-44,0,100,0,78,0,90,0,-121,0,96,0,86,0,52,2,60,0,-68,0,-78,0,-114,0,-60,0,0,0,20,-2,96,0,20,2,-101,0,32,3,33,0,11,4,58,
		0,20,4,-115,0,16,5,-80,0,20,6,24,0,21,1,-90,0,17,6,-64,0,14,6,-39,0,6,0,0,0,0,0,0,0,97,0,97,0,97,0,97,0,97,0,-108,0,-71,1,58,1,-82,2,64,2,-44,2,-21,3,21,3,63,3,114,3,-104,3,-73,3,-50,3,-16,4,7,4,85,4,-125,4,-45,5,74,5,-114,5,-16,6,81,6,126,6,-13,
		7,91,7,112,7,-123,7,-92,7,-52,7,-21,8,74,8,-17,9,53,9,-107,9,-22,10,48,10,114,10,-87,11,22,11,97,11,124,11,-81,12,4,12,40,12,118,12,-78,13,8,13,84,13,-70,14,23,14,-125,14,-82,14,-16,15,32,15,117,15,-54,15,-6,16,51,16,88,16,111,16,-107,16,-68,16,-41,16,-9,17,113,17,-48,18,36,18,-125,18,-20,
		19,63,19,-70,20,0,20,57,20,-122,20,-35,20,-8,21,100,21,-81,21,-2,22,99,22,-59,23,3,23,111,23,-62,24,9,24,57,24,-121,24,-50,25,20,25,77,25,-114,25,-91,25,-27,26,45,26,97,26,-66,27,49,27,-107,27,-9,28,22,28,-67,28,-20,29,-108,30,4,30,16,30,46,30,-24,31,2,31,63,31,-125,31,-44,32,80,32,112,32,-70,
		32,-26,33,6,33,66,33,116,33,-65,33,-53,33,-27,33,-1,34,25,34,123,34,-32,35,30,35,-102,35,-17,36,96,37,32,37,-112,37,-29,38,85,38,-75,39,44,39,-117,39,-90,39,-10,40,65,40,127,40,-48,41,44,41,-79,42,76,42,125,42,-28,43,76,43,-73,44,24,44,108,44,-58,44,-11,45,90,45,-120,45,-84,45,-70,45,-26,46,6,46,63,
		46,117,46,-70,46,-19,47,43,47,72,47,101,47,110,47,-95,47,-46,47,-18,48,10,48,78,48,90,48,-127,48,-81,49,44,49,89,49,-99,49,-52,50,9,50,126,50,-40,51,65,51,-73,52,46,52,97,52,-44,53,66,53,-97,53,-22,54,107,54,-103,54,-13,55,99,55,-75,56,16,56,108,56,-60,57,8,57,74,57,-76,58,17,58,120,58,-16,59,68,
		59,-69,60,23,60,-110,61,10,61,126,61,-45,62,16,62,105,62,-62,63,49,63,-88,63,-19,64,56,64,-128,64,-14,65,40,65,109,65,-85,65,-12,66,77,66,-79,66,-2,67,125,68,15,68,107,68,-36,69,84,69,123,69,-46,70,70,70,-63,70,-6,71,82,71,-102,71,-30,72,63,72,110,72,-102,73,38,73,92,73,-99,73,-37,74,32,74,120,74,-37,
		75,38,75,-103,76,32,76,124,76,-11,77,119,77,-18,78,93,78,-59,79,1,79,100,79,-59,80,46,80,-78,81,78,81,-102,81,-23,82,84,82,-61,83,57,83,-87,84,53,84,-64,85,82,85,-19,86,112,86,-22,87,47,87,117,87,-30,88,74,89,5,89,-63,90,65,90,-63,91,19,91,97,91,-106,91,-78,91,-22,92,0,92,22,92,-22,93,93,93,120,
		93,-109,93,-3,94,89,94,-51,94,-3,95,40,95,126,95,-44,95,-32,95,-20,95,-8,96,4,96,91,96,-66,97,19,97,115,97,127,97,-117,97,-42,98,64,98,-97,98,-1,99,-96,100,57,100,69,100,81,100,-94,100,-26,100,-14,100,-2,101,78,101,-100,101,-34,102,80,102,-62,103,27,103,-128,103,-116,103,-104,104,18,104,-118,104,-106,104,-94,104,-82,104,-70,
		105,36,105,-123,105,-32,105,-17,106,3,106,15,106,27,106,105,106,-51,107,85,107,-57,108,54,108,-102,108,-4,109,107,109,-42,110,96,110,-29,111,64,111,-109,111,-26,112,56,112,-81,112,-69,112,-57,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-10,112,-2,113,6,113,16,113,26,113,50,113,86,
		113,122,113,-99,113,-72,113,-60,113,-48,114,8,114,71,114,-87,114,-51,114,-39,114,-23,115,12,115,-33,115,-5,116,24,116,43,116,63,116,-122,117,16,117,-82,118,63,118,75,119,43,119,-113,120,13,120,-84,121,16,121,-117,121,-27,122,81,123,3,123,106,124,0,124,94,124,-62,124,-36,124,-10,125,16,125,42,125,-100,125,-61,125,-4,126,24,126,77,126,-32,
		127,34,127,-81,127,-16,-128,14,-128,44,-128,101,-128,114,-128,-100,-128,-65,-128,-53,-127,52,-127,-121,-126,20,-126,-125,-126,-10,-125,-61,-125,-61,-123,118,-123,-30,-122,50,-122,94,-122,-88,-121,6,-121,125,-121,-82,-120,21,-120,121,-120,-64,-119,62,-119,-110,-119,-60,-118,18,-118,75,-118,123,-118,-60,-117,28,-117,76,-117,-118,-117,-75,-116,28,-116,117,-116,-44,-115,31,-115,115,-115,-84,
		-115,-3,-114,33,-114,100,-114,-102,-114,-75,-114,-10,-113,86,-113,-114,-112,2,-112,103,-112,-58,-112,-16,-111,38,-111,-114,-111,-64,-110,14,-110,64,-110,-128,-110,-25,-109,63,-109,-95,-108,0,-108,114,-108,-24,-107,94,-107,-79,-107,-15,-106,74,-106,-94,-105,22,-105,-111,-105,-51,-104,29,-104,102,-104,-84,-104,-25,-103,41,-103,105,-103,-77,-102,13,-102,25,-102,103,-102,-41,-101,85,-101,-83,
		-101,-16,-100,118,-100,-40,-99,57,-99,-105,-98,44,-98,61,-98,-104,-98,-27,-97,51,-97,117,-97,-26,-96,74,-96,-80,-95,33,-95,-75,-94,59,-94,-46,-93,69,-93,-75,-93,-8,-92,85,-92,-81,-92,-36,-91,89,-91,-72,-91,-49,-90,53,-90,122,-89,37,-89,-119,-89,-19,-88,61,-88,-125,-88,-60,-87,6,-87,78,-87,-93,-86,10,-86,74,-86,100,-86,-77,-85,40,-85,112,-85,-72,
		-84,24,-84,-122,-84,-77,-83,2,-83,98,-83,118,-83,-118,-83,-100,-83,-80,-83,-62,-83,-39,-83,-19,-82,73,-82,-69,-81,8,-81,104,-81,-47,-81,-4,-80,80,-80,-94,-80,-26,-79,61,-79,100,-79,-43,-79,-21,-78,111,-78,-46,-78,-2,-77,15,-77,32,-77,51,-77,68,-77,85,-77,104,-77,123,-77,-114,-77,-92,-77,-84,-77,-76,-77,-68,-77,-51,-77,-40,-77,-32,-76,72,-76,-105,
		-76,-60,-75,37,-75,120,-75,-39,-74,84,-74,-98,-73,4,-73,102,-73,-54,-72,67,-72,75,-72,-26,-71,51,-71,-97,-71,-17,-70,104,-70,-42,-69,39,-69,39,-69,47,-69,-107,-69,-5,-68,90,-68,-99,-67,3,-67,26,-67,49,-67,72,-67,95,-67,120,-67,-111,-67,-99,-67,-87,-67,-64,-67,-41,-67,-18,-66,7,-66,30,-66,53,-66,76,-66,101,-66,124,-66,-109,-66,-86,-66,-63,
		-66,-40,-66,-15,-65,8,-65,31,-65,54,-65,79,-65,102,-65,125,-65,-108,-65,-86,-65,-64,-65,-39,-65,-14,-65,-2,-64,10,-64,33,-64,56,-64,78,-64,103,-64,125,-64,-109,-64,-86,-64,-61,-64,-39,-64,-16,-63,7,-63,29,-63,51,-63,76,-63,99,-63,122,-63,-112,-63,-87,-63,-64,-63,-40,-63,-17,-62,5,-62,28,-62,51,-62,-105,-61,47,-61,70,-61,93,-61,116,-61,-118,
		-61,-95,-61,-72,-61,-49,-61,-27,-61,-4,-60,45,-60,68,-60,90,-60,113,-60,-120,-60,-97,-60,-74,-59,32,-59,-90,-59,-67,-59,-45,-59,-22,-58,0,-58,23,-58,46,-58,69,-58,92,-58,104,-58,127,-58,-106,-58,-88,-58,-65,-58,-42,-58,-19,-57,4,-57,27,-57,50,-57,61,-57,72,-57,95,-57,107,-57,119,-57,-114,-57,-91,-57,-79,-57,-67,-57,-44,-57,-21,-57,-9,-56,3,
		-56,24,-56,77,-56,89,-56,101,-56,124,-56,-109,-56,-97,-56,-85,-56,-62,-56,-40,-56,-19,-55,4,-55,26,-55,49,-55,72,-55,97,-55,122,-55,-111,-55,-88,-55,-76,-55,-64,-55,-41,-55,-19,-54,4,-54,27,-54,50,-54,72,-54,84,-54,96,-54,108,-54,120,-54,-113,-54,-91,-54,-79,-54,-67,-54,-55,-54,-43,-54,-20,-53,2,-53,25,-53,47,-53,70,-53,92,-53,115,-53,-118,
		-53,-93,-53,-68,-53,-43,-53,-18,-52,76,-52,-77,-52,-54,-52,-31,-52,-8,-51,14,-51,39,-51,62,-51,85,-51,108,-51,-125,-51,-102,-51,-80,-51,-57,-51,-34,-51,-11,-50,12,-50,47,-50,87,-50,106,-50,-127,-50,-104,-50,-82,-50,-60,-50,-35,-50,-10,-49,2,-49,14,-49,37,-49,60,-49,82,-49,106,-49,-128,-49,-106,-49,-83,-49,-58,-49,-35,-49,-12,-48,11,-48,34,-48,57,
		-48,82,-48,105,-48,-128,-48,-106,-48,-81,-48,-58,-48,-36,-48,-13,-47,87,-47,110,-47,-124,-47,-101,-47,-78,-47,-56,-47,-34,-47,-12,-46,11,-46,118,-46,-116,-46,-94,-46,-71,-46,-48,-46,-36,-46,-13,-45,10,-45,33,-45,56,-45,67,-45,89,-45,112,-45,124,-45,-110,-45,-98,-45,-77,-45,-65,-45,-42,-45,-30,-45,-7,-44,16,-44,39,-44,64,-44,87,-44,99,-44,121,-44,-112,
		-44,-90,-44,-78,-44,-56,-44,-44,-44,-22,-44,-10,-43,12,-43,34,-43,57,-43,82,-43,107,-43,-56,-43,-33,-43,-11,-42,13,-42,36,-42,59,-42,81,-42,92,-42,104,-42,116,-42,-128,-42,-116,-42,-104,-42,-92,-42,-64,-42,-56,-42,-48,-42,-40,-42,-32,-42,-24,-42,-16,-42,-8,-41,0,-41,8,-41,16,-41,24,-41,32,-41,40,-41,48,-41,73,-41,98,-41,121,-41,-112,-41,-89,
		-41,-67,-41,-40,-41,-32,-41,-24,-41,-16,-41,-8,-40,99,-40,123,-40,-109,-40,-86,-40,-63,-40,-40,-40,-15,-39,8,-39,116,-39,124,-39,-107,-39,-99,-39,-91,-39,-68,-39,-45,-39,-37,-39,-29,-39,-21,-39,-13,-38,10,-38,18,-38,26,-38,34,-38,42,-38,50,-38,58,-38,66,-38,74,-38,82,-38,90,-38,113,-38,121,-38,-127,-38,-43,-38,-35,-38,-27,-38,-2,-37,21,-37,29,
		-37,37,-37,62,-37,70,-37,93,-37,115,-37,-118,-37,-95,-37,-72,-37,-49,-37,-24,-36,1,-36,24,-36,47,-36,55,-36,63,-36,75,-36,98,-36,106,-36,-127,-36,-104,-36,-92,-36,-80,-36,-57,-36,-34,-36,-11,-35,12,-35,20,-35,28,-35,53,-35,78,-35,90,-35,102,-35,114,-35,126,-35,-118,-35,-106,-35,-98,-35,-90,-35,-82,-35,-59,-35,-36,-35,-28,-35,-5,-34,18,-34,43,
		-34,68,-34,76,-34,84,-34,107,-34,-126,-34,-101,-34,-93,-34,-68,-34,-43,-34,-18,-33,7,-33,31,-33,54,-33,76,-33,101,-33,126,-33,-105,-33,-80,-33,-72,-33,-64,-33,-39,-33,-14,-32,11,-32,35,-32,58,-32,80,-32,105,-32,-127,-32,-102,-32,-77,-32,-52,-32,-28,-31,1,-31,30,-31,38,-31,50,-31,62,-31,85,-31,108,-31,-123,-31,-99,-31,-74,-31,-50,-31,-25,-31,-1,
		-30,24,-30,48,-30,75,-30,101,-30,126,-30,-105,-30,-80,-30,-55,-30,-30,-30,-5,-29,20,-29,45,-29,72,-29,99,-29,111,-29,123,-29,-110,-29,-87,-29,-64,-29,-42,-29,-17,-28,7,-28,32,-28,56,-28,81,-28,105,-28,-126,-28,-102,-28,-75,-28,-49,-28,-26,-28,-3,-27,9,-27,21,-27,33,-27,45,-27,68,-27,91,-27,116,-27,-116,-27,-91,-27,-67,-27,-42,-27,-18,-26,7,
		-26,31,-26,58,-26,84,-26,107,-26,-126,-26,-103,-26,-80,-26,-57,-26,-34,-26,-11,-25,11,-25,23,-25,35,-25,47,-25,59,-25,82,-25,105,-25,-128,-25,-105,-25,-82,-25,-59,-25,-36,-25,-13,-24,10,-24,32,-24,44,-24,56,-24,68,-24,80,-24,103,-24,126,-24,-107,-24,-85,-24,-64,-24,-52,-24,-40,-24,-28,-24,-16,-24,-4,-23,8,-23,20,-23,32,-23,40,-23,-120,-23,-24,
		-22,43,-22,107,-22,-49,-21,46,-21,120,-21,-56,-20,33,-20,120,-20,-128,-20,-116,-20,-106,-20,-98,-20,-90,-20,-82,-20,-74,-20,-66,-20,-58,-20,-50,-20,-42,-20,-19,-19,4,-19,27,-19,50,-19,75,-19,100,-19,125,-19,-106,-19,-81,-19,-56,-19,-31,-19,-6,-18,19,-18,44,-18,69,-18,94,-18,106,-18,118,-18,-126,-18,-114,-18,-102,-18,-85,-18,-73,-18,-61,-18,-49,-18,-26,
		-18,-8,-17,4,-17,16,-17,28,-17,40,-17,52,-17,64,-17,76,-17,88,-17,122,-17,-111,-17,-88,-17,-76,-17,-64,-17,-52,-17,-40,-17,-28,-17,-16,-16,8,-16,31,-16,53,-16,65,-16,77,-16,89,-16,101,-16,113,-16,125,-16,-119,-16,-107,-16,-95,-16,-83,-16,-71,-16,-59,-16,-47,-16,-35,-16,-27,-16,-19,-16,-11,-16,-3,-15,5,-15,13,-15,21,-15,29,-15,37,-15,45,
		-15,53,-15,61,-15,69,-15,77,-15,102,-15,126,-15,-106,-15,-83,-15,-75,-15,-67,-15,-42,-15,-34,-15,-11,-14,11,-14,19,-14,27,-14,35,-14,43,-14,66,-14,74,-14,82,-14,90,-14,98,-14,106,-14,114,-14,122,-14,-126,-13,13,-13,90,-13,-71,-13,-63,-13,-51,-13,-28,-13,-6,-12,2,-12,14,-12,26,-12,38,-12,50,-12,62,-12,74,-12,86,-12,98,-12,110,-12,122,
		-12,-122,-12,-110,-12,-98,-12,-86,-12,-74,0,0,0,5,0,100,0,0,3,40,5,-80,0,3,0,6,0,9,0,12,0,15,0,113,-78,12,16,17,17,18,57,-80,12,16,-80,0,-48,-80,12,16,-80,6,-48,-80,12,16,-80,9,-48,-80,12,16,-80,13,-48,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,
		62,89,-78,4,2,0,17,18,57,-78,5,2,0,17,18,57,-78,7,2,0,17,18,57,-78,8,2,0,17,18,57,-79,10,12,-12,-78,12,2,0,17,18,57,-78,13,2,0,17,18,57,-80,2,16,-79,14,12,-12,48,49,33,33,17,33,3,17,1,1,17,1,3,33,1,53,1,33,3,40,-3,60,2,-60,54,-2,-18,-2,-70,1,12,-28,2,3,-2,
		-2,1,2,-3,-3,5,-80,-6,-92,5,7,-3,125,2,119,-5,17,2,120,-3,94,2,94,-120,2,94,0,2,0,-96,-1,-11,1,123,5,-80,0,3,0,12,0,48,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-79,6,5,-80,10,43,88,33,-40,27,-12,89,-78,1,6,2,17,18,57,48,49,
		1,35,3,51,3,52,54,50,22,20,6,34,38,1,91,-89,13,-62,-55,55,108,56,56,108,55,1,-101,4,21,-6,-83,45,61,61,90,59,59,0,0,2,0,-120,4,18,2,35,6,0,0,4,0,9,0,25,0,-80,3,47,-78,2,10,3,17,18,57,-80,2,47,-80,7,-48,-80,3,16,-80,8,-48,48,49,1,3,35,19,51,5,3,35,19,51,1,
		21,30,111,1,-116,1,14,30,111,1,-116,5,120,-2,-102,1,-18,-120,-2,-102,1,-18,0,2,0,119,0,0,4,-45,5,-80,0,27,0,31,0,-111,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,
		89,-78,29,12,2,17,18,57,124,-80,29,47,24,-79,0,3,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,29,16,-80,6,-48,-80,29,16,-80,11,-48,-80,11,47,-79,8,3,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-80,14,-48,-80,11,16,-80,18,-48,-80,8,16,-80,20,-48,-80,29,16,-80,22,-48,-80,0,16,-80,24,-48,-80,8,16,-80,30,
		-48,48,49,1,33,3,35,19,35,53,33,19,33,53,33,19,51,3,33,19,51,3,51,21,35,3,51,21,35,3,35,3,33,19,33,2,-3,-2,-8,80,-113,80,-17,1,9,69,-2,-2,1,29,82,-113,82,1,8,82,-112,82,-52,-25,69,-31,-5,80,-112,-98,1,8,69,-2,-8,1,-102,-2,102,1,-102,-119,1,98,-117,1,-96,-2,96,1,-96,-2,96,-117,
		-2,-98,-119,-2,102,2,35,1,98,0,0,1,0,110,-1,48,4,17,6,-100,0,43,0,105,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,34,47,27,-79,34,18,62,89,-78,2,34,9,17,18,57,-80,9,16,-80,12,-48,-80,9,16,-80,16,-48,-80,9,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,25,
		1,-80,10,43,88,33,-40,27,-12,89,-80,34,16,-80,31,-48,-80,34,16,-80,38,-48,-80,34,16,-79,41,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,39,38,38,53,52,54,55,53,51,21,22,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,6,7,21,35,53,38,38,53,51,20,22,51,50,54,3,88,-127,-103,-43,-61,-65,
		-89,-107,-88,-69,-72,-122,114,119,126,-123,1,49,-85,81,-53,-73,-108,-70,-45,-71,-110,-122,-125,-106,1,119,92,126,51,65,-47,-95,-92,-46,20,-37,-36,23,-20,-51,-115,-90,123,110,102,121,99,119,-98,106,-87,-50,19,-65,-65,17,-25,-58,-117,-106,126,0,0,5,0,105,-1,-21,5,-125,5,-59,0,13,0,26,0,38,0,52,0,56,0,124,0,-80,0,69,88,-80,
		3,47,27,-79,3,30,62,89,-80,0,69,88,-80,35,47,27,-79,35,18,62,89,-80,3,16,-80,10,-48,-80,10,47,-79,17,4,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,4,-80,10,43,88,33,-40,27,-12,89,-80,35,16,-80,29,-48,-80,29,47,-80,35,16,-79,42,4,-80,10,43,88,33,-40,27,-12,89,-80,29,16,-79,49,4,-80,10,43,
		88,33,-40,27,-12,89,-78,53,35,3,17,18,57,-80,53,47,-78,55,3,35,17,18,57,-80,55,47,48,49,19,52,54,51,50,22,21,21,20,6,35,34,38,53,23,20,22,51,50,54,53,53,52,38,34,6,21,1,52,54,32,22,21,21,20,6,32,38,53,23,20,22,51,50,54,53,53,52,38,35,34,6,21,5,39,1,23,105,-89,-125,-123,-91,
		-89,-127,-126,-86,-118,88,74,71,87,86,-108,86,2,59,-89,1,6,-88,-89,-2,-4,-86,-118,88,74,72,86,87,73,71,89,-2,7,105,2,-57,105,4,-104,-125,-86,-85,-120,71,-124,-89,-89,-117,7,78,101,98,85,73,78,102,102,82,-4,-47,-125,-87,-88,-117,71,-125,-87,-89,-117,6,79,101,99,85,74,79,100,99,84,-13,66,4,114,66,0,3,0,101,-1,-20,
		4,-13,5,-60,0,30,0,39,0,51,0,-121,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,62,89,-80,0,69,88,-80,24,47,27,-79,24,18,62,89,-78,34,28,9,17,18,57,-78,42,9,28,17,18,57,-78,3,34,42,17,18,57,-78,16,42,34,17,18,57,-78,17,9,28,17,18,57,-78,19,28,
		9,17,18,57,-78,25,28,9,17,18,57,-78,22,17,25,17,18,57,-80,28,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,-78,33,31,17,17,18,57,-80,9,16,-79,49,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,55,38,38,53,52,54,51,50,22,21,20,6,7,7,1,54,53,51,20,7,23,35,39,6,6,35,34,36,5,50,
		55,1,7,6,21,20,22,3,20,23,55,54,54,53,52,38,35,34,6,101,117,-91,97,66,-60,-88,-106,-60,89,111,107,1,68,68,-89,123,-48,-34,97,74,-57,103,-43,-2,-2,1,-41,-109,122,-2,-99,33,-89,-103,34,118,118,68,50,100,76,82,96,1,-121,105,-80,117,118,-112,71,-90,-68,-81,-123,88,-107,82,79,-2,125,-126,-97,-1,-88,-7,115,66,69,-30,
		75,112,1,-87,24,123,-126,118,-114,3,-27,96,-112,83,48,87,62,67,89,111,0,1,0,103,4,33,0,-3,6,0,0,4,0,16,0,-80,3,47,-78,2,5,3,17,18,57,-80,2,47,48,49,19,3,35,19,51,-3,21,-127,1,-107,5,-111,-2,-112,1,-33,0,1,0,-123,-2,42,2,-107,6,107,0,17,0,9,0,-80,14,47,-80,4,47,48,49,19,
		52,18,18,55,23,6,2,3,7,16,19,22,23,7,38,39,2,-123,121,-16,-127,38,-110,-69,9,1,-115,85,117,38,-123,121,-20,2,79,-30,1,-96,1,84,70,122,112,-2,52,-2,-29,85,-2,126,-2,-28,-86,96,113,74,-82,1,84,0,0,1,0,38,-2,42,2,55,6,107,0,17,0,9,0,-80,14,47,-80,4,47,48,49,1,20,2,2,7,39,54,
		18,19,53,52,2,2,39,55,22,18,18,2,55,117,-15,-124,39,-102,-69,2,88,-99,98,39,-124,-17,119,2,69,-33,-2,103,-2,-90,73,113,118,1,-15,1,47,32,-46,1,105,1,30,80,113,73,-2,-86,-2,100,0,1,0,28,2,97,3,85,5,-80,0,14,0,32,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,-48,25,-80,0,47,24,
		-80,9,-48,25,-80,9,47,24,48,49,1,37,55,5,3,51,3,37,23,5,19,7,3,3,39,1,74,-2,-46,46,1,46,9,-103,10,1,41,46,-2,-51,-58,124,-70,-76,125,3,-41,90,-105,112,1,88,-2,-93,110,-104,91,-2,-15,94,1,32,-2,-25,91,0,0,1,0,78,0,-110,4,52,4,-74,0,11,0,27,0,-80,9,47,-80,0,-48,-80,9,16,
		-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,48,49,1,33,21,33,17,35,17,33,53,33,17,51,2,-98,1,-106,-2,106,-70,-2,106,1,-106,-70,3,13,-81,-2,52,1,-52,-81,1,-87,0,0,1,0,29,-2,-34,1,52,0,-37,0,8,0,24,0,-80,9,47,-79,4,5,-80,10,43,88,33,-40,27,-12,89,-80,0,-48,-80,0,47,48,49,
		19,39,54,55,53,51,21,20,6,-122,105,94,4,-75,99,-2,-34,72,-125,-117,-89,-111,101,-54,0,1,0,37,2,31,2,13,2,-74,0,3,0,18,0,-80,2,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,2,13,-2,24,1,-24,2,31,-105,0,0,1,0,-112,-1,-11,1,118,0,-47,0,9,0,28,0,-80,0,69,88,-80,
		7,47,27,-79,7,18,62,89,-79,2,5,-80,10,43,88,33,-40,27,-12,89,48,49,55,52,54,50,22,21,20,6,34,38,-112,57,114,59,59,114,57,97,48,64,64,48,46,62,62,0,0,1,0,18,-1,-125,3,16,5,-80,0,3,0,19,0,-80,0,47,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,48,49,23,35,1,51,-79,-97,2,96,-98,
		125,6,45,0,0,2,0,115,-1,-20,4,10,5,-60,0,13,0,27,0,59,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,10,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,2,35,34,2,3,53,16,18,51,
		50,18,19,39,52,38,35,34,6,7,17,20,22,51,50,54,55,4,10,-34,-20,-23,-32,4,-34,-19,-21,-34,3,-71,-124,-113,-114,-126,2,-119,-117,-119,-123,3,2,109,-2,-69,-2,-60,1,53,1,51,-9,1,65,1,56,-2,-45,-2,-58,13,-21,-41,-42,-34,-2,-40,-20,-31,-44,-28,0,1,0,-86,0,0,2,-39,5,-73,0,6,0,58,0,-80,0,69,88,-80,
		5,47,27,-79,5,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,5,17,18,57,-80,4,47,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,2,3,5,17,18,57,48,49,33,35,17,5,53,37,51,2,-39,-70,-2,-117,2,18,29,4,-47,-119,-88,-57,0,1,0,93,0,0,4,51,5,-60,0,23,0,79,0,-80,0,69,
		88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,2,-48,-78,3,16,23,17,18,57,-80,16,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,12,-48,-78,21,23,16,17,18,57,48,49,33,33,53,1,54,54,53,52,38,35,34,6,21,35,52,
		36,51,50,22,21,20,1,1,33,4,51,-4,70,1,-8,112,85,-118,115,-118,-103,-71,1,3,-39,-53,-20,-2,-18,-2,122,2,-37,-123,2,48,127,-97,85,114,-110,-99,-116,-55,-8,-43,-79,-41,-2,-41,-2,89,0,1,0,94,-1,-20,3,-7,5,-60,0,38,0,123,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,25,47,27,-79,25,
		18,62,89,-78,0,13,25,17,18,57,-80,0,47,-78,-49,0,1,93,-78,-97,0,1,113,-78,47,0,1,93,-78,95,0,1,114,-80,13,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-80,9,-48,-80,0,16,-79,38,1,-80,10,43,88,33,-40,27,-12,89,-78,19,38,0,17,18,57,-80,25,16,-80,28,-48,-80,25,16,-79,31,1,-80,10,
		43,88,33,-40,27,-12,89,48,49,1,51,54,54,53,16,35,34,6,21,35,52,54,51,50,22,21,20,6,7,22,22,21,20,4,32,36,53,51,20,22,51,50,54,53,52,38,39,35,1,-122,-117,-125,-106,-1,120,-113,-71,-3,-61,-50,-22,123,106,120,-125,-1,0,-2,102,-2,-1,-70,-106,126,-122,-114,-100,-109,-117,3,50,2,-122,114,1,0,-119,113,-83,-27,
		-38,-62,95,-78,44,38,-80,127,-60,-26,-34,-74,115,-118,-116,-125,127,-120,2,0,0,2,0,53,0,0,4,80,5,-80,0,10,0,14,0,74,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,1,9,4,17,18,57,-80,1,47,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,1,
		16,-80,11,-48,-78,8,6,11,17,18,57,-78,13,9,4,17,18,57,48,49,1,51,21,35,17,35,17,33,53,1,51,1,33,17,7,3,-122,-54,-54,-70,-3,105,2,-116,-59,-3,-127,1,-59,22,1,-23,-105,-2,-82,1,82,109,3,-15,-4,57,2,-54,40,0,0,1,0,-102,-1,-20,4,45,5,-80,0,29,0,100,0,-80,0,69,88,-80,1,47,27,-79,
		1,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,1,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,7,13,1,17,18,57,-80,7,47,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,5,7,26,17,18,57,-80,13,16,-80,17,-48,-80,13,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,29,-48,48,49,
		19,19,33,21,33,3,54,51,50,18,21,20,2,35,34,38,39,51,22,22,51,50,54,53,52,38,35,34,7,7,-50,74,2,-22,-3,-77,44,107,-120,-57,-22,-13,-38,-63,-12,17,-81,17,-112,118,-127,-109,-97,-124,121,69,49,2,-38,2,-42,-85,-2,115,63,-2,-7,-32,-31,-2,-3,-42,-67,125,127,-80,-101,-110,-79,53,40,0,0,2,0,-124,-1,-20,4,28,
		5,-79,0,20,0,33,0,81,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,0,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,7,13,0,17,18,57,-80,7,47,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,
		21,35,6,4,7,54,51,50,18,21,20,2,35,34,0,53,53,16,0,37,3,34,6,7,21,20,22,51,50,54,53,52,38,3,79,34,-40,-1,0,20,115,-57,-66,-29,-11,-50,-47,-2,-4,1,87,1,83,-46,95,-96,31,-94,121,125,-113,-111,5,-79,-99,4,-8,-31,-124,-2,-12,-44,-31,-2,-14,1,65,-3,71,1,-110,1,-87,5,-3,112,114,86,68,-76,
		-36,-72,-107,-106,-71,0,0,1,0,77,0,0,4,37,5,-80,0,6,0,51,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,5,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,0,3,5,17,18,57,48,49,1,1,35,1,33,53,33,4,37,-3,-91,-62,2,89,-4,-20,3,-40,5,
		72,-6,-72,5,24,-104,0,3,0,112,-1,-20,4,14,5,-60,0,23,0,33,0,43,0,100,0,-80,0,69,88,-80,21,47,27,-79,21,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,39,9,21,17,18,57,-80,39,47,-78,-49,39,1,93,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,3,26,39,17,18,57,-78,15,39,26,17,
		18,57,-80,9,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,22,22,21,20,6,35,34,38,53,52,54,55,38,38,53,52,54,51,50,22,3,52,38,34,6,20,22,51,50,54,1,34,6,21,20,22,50,54,52,38,3,-20,115,98,114,-123,-1,-48,-46,-3,-127,114,
		97,112,-20,-63,-64,-19,-105,-101,-6,-105,-109,-125,-126,-108,-2,-22,109,-121,-123,-34,-123,-118,4,52,109,-86,48,49,-68,119,-67,-32,-31,-68,118,-66,49,48,-86,108,-72,-40,-40,-4,-95,122,-102,-104,-8,-114,-113,4,26,-121,116,111,-119,-119,-34,-116,0,2,0,100,-1,-1,3,-8,5,-60,0,23,0,36,0,91,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,
		-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-78,3,19,11,17,18,57,-80,3,47,-78,0,3,11,17,18,57,-80,19,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,6,6,35,34,38,38,53,52,54,54,51,50,
		18,17,21,16,0,5,35,53,51,54,54,37,50,54,55,53,52,38,35,34,6,21,20,22,3,62,58,-95,96,126,-69,102,111,-52,-120,-40,-7,-2,-80,-2,-83,36,39,-27,-10,-2,-18,93,-99,36,-98,121,122,-108,-113,2,-128,69,84,124,-31,-120,-110,-22,124,-2,-67,-2,-23,54,-2,87,-2,121,5,-100,4,-25,-6,114,84,74,-74,-28,-69,-103,-107,-63,-1,-1,
		0,-122,-1,-11,1,109,4,68,0,38,0,18,-10,0,1,7,0,18,-1,-9,3,115,0,16,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,48,49,-1,-1,0,41,-2,-34,1,85,4,68,0,39,0,18,-1,-33,3,115,1,6,0,16,12,0,0,16,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,48,49,0,1,0,72,0,-61,3,122,
		4,74,0,6,0,22,0,-80,0,69,88,-80,5,47,27,-79,5,26,62,89,-80,2,-48,-80,2,47,48,49,1,5,21,1,53,1,21,1,8,2,114,-4,-50,3,50,2,-124,-3,-60,1,123,-110,1,122,-60,0,0,2,0,-104,1,-113,3,-38,3,-49,0,3,0,7,0,39,0,-80,7,47,-80,3,-48,-80,3,47,-79,0,1,-80,10,43,88,33,-40,27,
		-12,89,-80,7,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,17,33,53,33,3,-38,-4,-66,3,66,-4,-66,3,66,3,46,-95,-3,-64,-96,0,0,1,0,-122,0,-60,3,-36,4,75,0,6,0,22,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,5,-48,-80,5,47,48,49,1,1,53,1,21,1,53,3,27,-3,
		107,3,86,-4,-86,2,-118,1,3,-66,-2,-122,-110,-2,-123,-64,0,2,0,75,-1,-11,3,118,5,-60,0,24,0,33,0,83,0,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,32,47,27,-79,32,18,62,89,-79,27,5,-80,10,43,88,33,-40,27,-12,89,-78,0,27,16,17,18,57,-78,4,16,0,17,18,57,-80,16,16,-79,9,
		1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,12,-48,-78,21,0,16,17,18,57,48,49,1,54,54,55,55,54,53,52,38,35,34,6,21,35,54,54,51,50,22,21,20,7,7,6,21,3,52,54,50,22,20,6,34,38,1,101,2,50,77,-125,84,110,105,102,124,-71,2,-29,-74,-67,-45,-94,109,73,-63,55,108,56,56,108,55,1,-102,119,-118,
		84,-121,95,109,105,119,108,91,-94,-57,-53,-79,-81,-86,108,81,-104,-2,-61,45,61,61,90,59,59,0,0,2,0,106,-2,59,6,-42,5,-105,0,53,0,66,0,108,0,-80,50,47,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,3,-48,-78,15,50,8,17,18,57,-80,15,47,-78,5,8,15,17,18,57,-80,8,16,-79,57,2,-80,10,43,88,33,
		-40,27,-12,89,-80,21,-48,-80,50,16,-79,27,2,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,42,-48,-80,42,47,-79,35,2,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,64,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,6,2,35,34,39,6,6,35,34,38,55,54,18,54,51,50,22,23,3,6,51,50,54,55,18,0,33,34,4,
		2,7,6,18,4,51,50,54,55,23,6,6,35,34,36,2,19,18,18,36,51,50,4,18,1,6,22,51,50,54,55,55,19,38,35,34,6,6,-54,12,-40,-75,-69,53,54,-117,74,-114,-110,19,15,121,-65,105,81,-128,80,52,19,-109,113,-116,6,19,-2,-71,-2,-78,-55,-2,-56,-76,11,12,-112,1,39,-47,90,-75,60,37,62,-51,105,-6,-2,-104,-77,12,
		12,-34,1,124,-17,-7,1,100,-82,-5,-14,14,81,88,60,111,36,1,46,56,64,117,-103,1,-10,-14,-2,-24,-88,85,83,-24,-51,-91,1,3,-108,43,63,-3,-42,-25,-32,-76,1,-123,1,-104,-57,-2,-120,-10,-8,-2,-109,-63,44,35,115,39,50,-31,1,-89,1,27,1,19,1,-73,-17,-32,-2,90,-2,-112,-114,-104,102,95,9,1,-9,29,-18,0,0,2,0,28,
		0,0,5,29,5,-80,0,7,0,10,0,84,-78,10,11,12,17,18,57,-80,10,16,-80,4,-48,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,8,4,2,17,18,57,-80,8,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,10,4,
		2,17,18,57,48,49,1,33,3,35,1,51,1,35,1,33,3,3,-51,-3,-98,-119,-58,2,44,-88,2,45,-59,-3,77,1,-17,-8,1,124,-2,-124,5,-80,-6,80,2,26,2,-87,0,3,0,-87,0,0,4,-120,5,-80,0,14,0,22,0,31,0,88,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,
		89,-78,23,0,1,17,18,57,-80,23,47,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,8,15,23,17,18,57,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,22,21,20,6,7,22,22,21,20,6,35,1,17,33,50,54,53,16,33,37,33,50,54,53,
		52,38,35,33,-87,1,-36,-19,-17,116,100,118,-119,-2,-24,-2,-57,1,61,-122,-101,-2,-30,-2,-64,1,34,126,-105,-116,-113,-2,-28,5,-80,-60,-64,102,-99,43,33,-71,-128,-60,-32,2,-87,-3,-12,-117,122,1,7,-102,126,108,120,109,0,1,0,119,-1,-20,4,-40,5,-60,0,28,0,71,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,
		-80,3,47,27,-79,3,18,62,89,-80,11,16,-80,15,-48,-80,11,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,28,-48,48,49,1,6,4,35,32,0,17,53,52,18,36,51,50,0,23,35,38,38,35,34,2,21,21,20,18,51,50,54,55,4,-40,27,-2,-31,-18,-2,-2,
		-2,-55,-111,1,10,-81,-24,1,24,23,-63,25,-89,-106,-72,-47,-58,-78,-96,-85,28,1,-50,-25,-5,1,114,1,54,-116,-53,1,52,-91,-2,-3,-27,-82,-100,-2,-16,-5,-115,-19,-2,-24,-111,-76,0,2,0,-87,0,0,4,-58,5,-80,0,11,0,21,0,59,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,
		89,-80,1,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,4,18,23,21,20,2,4,7,3,17,51,50,18,53,53,52,2,39,-87,1,-101,-66,1,36,-97,1,-97,-2,-39,-60,-45,-54,-34,-9,-23,-42,5,-80,-88,-2,-54,-55,93,-50,-2,-54,-90,2,5,18,-5,-117,1,
		20,-1,85,-8,1,19,2,0,0,1,0,-87,0,0,4,70,5,-80,0,11,0,81,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,11,4,6,17,18,57,-80,11,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,
		8,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,33,17,33,21,33,17,33,3,-32,-3,-119,2,-35,-4,99,3,-109,-3,45,2,119,2,-95,-3,-4,-99,5,-80,-98,-2,44,0,0,1,0,-87,0,0,4,47,5,-80,0,9,0,66,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,
		89,-78,9,2,4,17,18,57,-80,9,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,21,33,17,33,3,-52,-3,-99,-64,3,-122,-3,58,2,99,2,-125,-3,125,5,-80,-98,-2,14,0,1,0,122,-1,-20,4,-36,5,-60,0,31,0,108,0,-80,0,69,88,-80,
		11,47,27,-79,11,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,11,16,-80,15,-48,-80,11,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-78,30,3,11,17,18,57,-80,30,47,-76,-65,30,-49,30,2,93,-76,15,30,31,30,2,93,-76,63,30,79,30,2,93,-79,29,
		1,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,4,35,34,36,2,39,53,16,0,33,50,4,23,35,2,33,34,2,3,21,20,18,51,50,54,55,17,33,53,33,4,-36,74,-2,-9,-80,-78,-2,-20,-105,2,1,51,1,22,-28,1,22,31,-64,54,-2,-34,-63,-57,1,-32,-65,108,-94,53,-2,-81,2,16,-65,106,105,-89,1,52,-53,127,1,73,1,
		106,-23,-42,1,33,-2,-15,-2,-1,119,-11,-2,-33,48,57,1,71,-100,0,1,0,-87,0,0,5,8,5,-80,0,11,0,103,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,16,-80,9,
		-48,-80,9,47,-78,-17,9,1,93,-76,-49,9,-33,9,2,113,-78,-113,9,1,113,-78,47,9,1,93,-78,-97,9,1,114,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,33,17,35,17,51,17,33,17,51,5,8,-63,-3,34,-64,-64,2,-34,-63,2,-95,-3,95,5,-80,-3,-114,2,114,0,0,1,0,-73,0,0,1,119,5,-80,0,3,
		0,29,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,48,49,33,35,17,51,1,119,-64,-64,5,-80,0,0,1,0,53,-1,-20,3,-52,5,-80,0,15,0,47,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,9,-48,-80,5,16,-79,
		12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,17,20,6,35,34,38,53,51,20,22,51,50,54,55,3,11,-63,-5,-47,-39,-14,-64,-119,-126,119,-109,1,5,-80,-5,-7,-47,-20,-34,-56,125,-116,-106,-121,0,1,0,-87,0,0,5,5,5,-80,0,11,0,116,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,7,47,27,
		-79,7,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,2,5,17,18,57,64,17,74,0,90,0,106,0,122,0,-118,0,-102,0,-86,0,-70,0,8,93,-78,57,0,1,93,-78,6,5,2,17,18,57,64,19,54,6,70,6,86,6,102,6,118,6,-122,6,-106,6,-90,6,-74,6,
		9,93,48,49,1,7,17,35,17,51,17,1,51,1,1,35,2,27,-78,-64,-64,2,-121,-24,-3,-61,2,106,-26,2,-91,-71,-2,20,5,-80,-3,48,2,-48,-3,125,-4,-45,0,1,0,-87,0,0,4,28,5,-80,0,5,0,41,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,0,1,-80,10,
		43,88,33,-40,27,-12,89,48,49,37,33,21,33,17,51,1,106,2,-78,-4,-115,-63,-99,-99,5,-80,0,1,0,-87,0,0,6,82,5,-80,0,14,0,89,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,
		18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,1,0,4,17,18,57,-78,7,0,4,17,18,57,-78,10,0,4,17,18,57,48,49,9,2,51,17,35,17,19,1,35,1,19,17,35,17,1,-95,1,-36,1,-36,-7,-64,18,-2,34,-109,-2,35,19,-64,5,-80,-5,92,4,-92,-6,80,2,55,2,100,-5,101,4,-104,-3,-97,-3,-55,5,
		-80,0,0,1,0,-87,0,0,5,8,5,-80,0,9,0,76,-78,1,10,11,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,2,5,0,17,18,57,-78,7,5,0,17,18,57,
		48,49,33,35,1,17,35,17,51,1,17,51,5,8,-63,-3,35,-63,-63,2,-33,-65,4,98,-5,-98,5,-80,-5,-103,4,103,0,2,0,118,-1,-20,5,9,5,-60,0,17,0,31,0,59,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,13,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,
		-80,4,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,4,35,34,36,2,39,53,52,18,36,51,50,4,18,21,39,16,2,35,34,2,7,21,20,18,51,50,18,55,5,9,-112,-2,-8,-80,-84,-2,-10,-109,2,-110,1,11,-84,-81,1,11,-112,-65,-48,-69,-74,-47,3,-45,-71,-70,-52,3,2,-87,-42,-2,-63,-88,-87,1,57,-50,105,
		-46,1,66,-85,-87,-2,-65,-43,2,1,3,1,21,-2,-21,-10,107,-5,-2,-31,1,15,-3,0,0,2,0,-87,0,0,4,-64,5,-80,0,10,0,19,0,79,-78,10,20,21,17,18,57,-80,10,16,-80,12,-48,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-78,11,3,1,17,18,57,-80,11,47,
		-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,50,4,21,20,4,35,37,33,50,54,53,52,38,39,33,1,105,-64,2,25,-17,1,15,-2,-9,-9,-2,-87,1,89,-102,-92,-92,-113,-2,-100,2,58,-3,-58,5,-80,-12,-55,-44,-27,-99,-111,-119,-126,-100,3,0,2,0,109,
		-1,10,5,6,5,-60,0,21,0,34,0,79,-78,8,35,36,17,18,57,-80,8,16,-80,25,-48,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,3,8,17,17,18,57,-80,17,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,
		49,1,20,2,7,5,7,37,6,35,34,36,2,39,53,52,18,36,51,50,4,18,21,39,16,2,35,34,2,7,21,20,18,32,18,55,5,1,-122,121,1,4,-125,-2,-51,72,80,-84,-2,-10,-109,2,-110,1,11,-84,-80,1,11,-112,-64,-51,-66,-75,-47,3,-47,1,116,-52,3,2,-87,-45,-2,-49,86,-52,121,-12,18,-87,1,57,-50,105,-46,1,66,-85,
		-86,-2,-63,-43,1,1,1,1,23,-2,-21,-10,107,-6,-2,-32,1,15,-3,0,0,2,0,-88,0,0,4,-55,5,-80,0,14,0,23,0,99,-78,5,24,25,17,18,57,-80,5,16,-80,22,-48,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-78,
		16,4,2,17,18,57,-80,16,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,11,0,4,17,18,57,-80,4,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,50,4,21,20,6,7,1,21,35,1,33,50,54,53,52,38,39,33,2,-65,-2,-86,-63,1,-30,-10,1,9,-109,-125,1,86,-50,-3,110,1,39,-113,-87,
		-95,-104,-2,-38,2,77,-3,-77,5,-80,-32,-42,-120,-54,50,-3,-106,12,2,-22,-108,124,-121,-112,1,0,0,1,0,80,-1,-20,4,114,5,-60,0,38,0,100,-78,0,39,40,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,89,-80,6,16,-80,11,-48,-80,6,16,-79,14,1,-80,10,43,88,
		33,-40,27,-12,89,-78,38,26,6,17,18,57,-80,38,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,26,16,-80,31,-48,-80,26,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,38,38,53,52,36,51,50,22,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,4,35,34,36,38,53,51,20,22,51,50,54,52,38,2,
		86,-9,-31,1,19,-36,-106,-21,-127,-63,-88,-103,-114,-97,-105,1,107,-51,99,-2,-20,-25,-106,-2,-4,-115,-63,-61,-93,-104,-94,-106,2,-119,71,-49,-104,-84,-31,116,-52,121,-124,-105,125,111,89,123,102,123,-92,111,-79,-43,115,-56,127,-124,-103,124,-42,117,0,1,0,49,0,0,4,-105,5,-80,0,7,0,47,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,
		-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,6,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,48,49,1,33,17,35,17,33,53,33,4,-105,-2,44,-65,-2,45,4,102,5,18,-6,-18,5,18,-98,0,0,1,0,-116,-1,-20,4,-86,5,-80,0,18,0,61,-78,5,19,20,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,
		30,62,89,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-79,14,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,6,0,7,7,34,0,39,17,51,17,20,22,51,50,54,53,17,4,-86,1,-2,-1,-36,51,-17,-2,-28,2,-66,-82,-95,-93,-83,5,-80,-4,34,-50,-2,-6,16,2,1,2,-30,
		3,-32,-4,38,-98,-81,-82,-98,3,-37,0,1,0,28,0,0,4,-3,5,-80,0,6,0,56,-78,0,7,8,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,0,1,3,17,18,57,48,49,37,1,51,1,35,1,51,2,-117,1,
		-96,-46,-3,-28,-86,-3,-27,-47,-1,4,-79,-6,80,5,-80,0,0,1,0,61,0,0,6,-19,5,-80,0,18,0,89,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,15,47,
		27,-79,15,18,62,89,-78,1,3,10,17,18,57,-78,6,3,10,17,18,57,-78,13,3,10,17,18,57,48,49,1,23,55,1,51,1,23,55,19,51,1,35,1,39,7,1,35,1,51,1,-29,28,41,1,32,-94,1,25,40,31,-30,-63,-2,-97,-81,-2,-44,23,23,-2,-55,-81,-2,-96,-64,1,-53,-64,-83,3,-8,-4,8,-80,-60,3,-28,-6,80,4,37,
		111,111,-5,-37,5,-80,0,1,0,57,0,0,4,-50,5,-80,0,11,0,107,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,1,4,17,18,57,64,9,-122,0,-106,0,-90,0,-74,0,
		4,93,-78,6,1,4,17,18,57,64,9,-119,6,-103,6,-87,6,-71,6,4,93,-78,3,0,6,17,18,57,-78,9,6,0,17,18,57,48,49,1,1,51,1,1,35,1,1,35,1,1,51,2,-124,1,93,-30,-2,52,1,-41,-28,-2,-102,-2,-104,-29,1,-40,-2,51,-31,3,-126,2,46,-3,46,-3,34,2,56,-3,-56,2,-34,2,-46,0,0,1,0,15,
		0,0,4,-69,5,-80,0,8,0,49,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,0,1,4,17,18,57,48,49,1,1,51,1,17,35,17,1,51,2,101,1,124,-38,-2,10,-64,-2,10,-36,2,-43,2,-37,-4,111,-3,-31,2,31,3,
		-111,0,0,1,0,86,0,0,4,122,5,-80,0,9,0,70,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,0,2,17,18,57,-80,7,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-78,9,5,7,17,18,57,48,49,37,33,21,33,
		53,1,33,53,33,21,1,57,3,65,-5,-36,3,30,-4,-17,3,-9,-99,-99,-112,4,-126,-98,-115,0,0,1,0,-110,-2,-56,2,11,6,-128,0,7,0,36,0,-80,4,47,-80,7,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,51,21,33,17,33,2,11,-65,-65,-2,-121,
		1,121,5,-24,-7,120,-104,7,-72,0,0,1,0,40,-1,-125,3,56,5,-80,0,3,0,19,0,-80,2,47,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,48,49,19,51,1,35,40,-80,2,96,-80,5,-80,-7,-45,0,1,0,9,-2,-56,1,-125,6,-128,0,7,0,39,0,-80,2,47,-80,1,47,-80,2,16,-79,5,1,-80,10,43,88,33,-40,27,
		-12,89,-80,1,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,33,17,33,53,51,17,35,9,1,122,-2,-122,-63,-63,6,-128,-8,72,-104,6,-120,0,0,1,0,64,2,-39,3,20,5,-80,0,6,0,39,-78,0,7,8,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,-48,-78,1,7,3,17,18,57,-80,1,47,
		-80,5,-48,48,49,1,3,35,1,51,1,35,1,-86,-66,-84,1,43,127,1,42,-85,4,-69,-2,30,2,-41,-3,41,0,1,0,4,-1,105,3,-104,0,0,0,3,0,28,0,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,33,53,33,3,-104,-4,108,3,-108,-105,-105,0,1,0,57,4,-38,
		1,-38,6,0,0,3,0,35,0,-80,1,47,-78,15,1,1,93,-80,0,-48,25,-80,0,47,24,-80,1,16,-80,2,-48,-80,2,47,-76,15,2,31,2,2,93,48,49,1,35,1,51,1,-38,-97,-2,-2,-33,4,-38,1,38,0,0,2,0,109,-1,-20,3,-22,4,78,0,30,0,40,0,124,-78,23,41,42,17,18,57,-80,23,16,-80,32,-48,0,-80,0,
		69,88,-80,23,47,27,-79,23,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,2,23,4,17,18,57,-78,11,23,4,17,18,57,-80,11,47,-80,23,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,18,11,23,17,18,57,-80,4,16,-79,31,1,-80,10,43,88,33,-40,27,-12,
		89,-80,11,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,38,39,6,35,34,38,53,52,36,51,51,53,52,38,35,34,6,21,35,52,54,54,51,50,22,23,17,20,23,21,37,50,54,55,53,35,32,21,20,22,3,40,16,10,-127,-77,-96,-51,1,1,-23,-76,116,113,99,-122,-70,115,-59,118,-69,-44,4,38,-2,11,87,-100,35,-111,-2,
		-84,116,32,82,-122,-75,-117,-87,-69,85,97,115,100,71,81,-105,88,-69,-92,-2,14,-107,88,16,-115,90,72,-34,-57,87,98,0,0,2,0,-116,-1,-20,4,32,6,0,0,14,0,25,0,102,-78,18,26,27,17,18,57,-80,18,16,-80,3,-48,0,-80,8,47,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,
		89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,5,8,3,17,18,57,-78,10,12,3,17,18,57,-80,12,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,35,34,39,7,35,17,51,17,54,32,18,17,39,52,38,35,34,7,17,22,51,50,54,4,32,-28,-64,
		-51,112,9,-86,-71,112,1,-118,-31,-71,-110,-119,-73,80,85,-76,-123,-108,2,17,-8,-2,-45,-111,125,6,0,-3,-61,-117,-2,-42,-2,-3,5,-67,-50,-86,-2,44,-86,-50,0,1,0,92,-1,-20,3,-20,4,78,0,29,0,75,-78,16,30,31,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,
		-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,3,-48,-80,16,16,-80,20,-48,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,50,54,55,51,14,2,35,34,0,17,53,52,54,54,51,50,22,23,35,38,38,35,34,6,21,21,20,22,2,62,99,-108,8,-81,5,118,-59,110,-35,-2,-5,116,-39,-108,-74,-15,8,-81,
		8,-113,105,-115,-101,-102,-125,120,90,93,-88,100,1,39,1,0,31,-98,-10,-120,-38,-82,105,-121,-53,-64,35,-69,-54,0,0,2,0,95,-1,-20,3,-16,6,0,0,15,0,26,0,102,-78,24,27,28,17,18,57,-80,24,16,-80,3,-48,0,-80,6,47,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,
		0,69,88,-80,8,47,27,-79,8,18,62,89,-78,5,3,12,17,18,57,-78,10,3,12,17,18,57,-80,12,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,17,51,17,35,39,6,35,34,2,53,23,20,22,51,50,55,17,38,35,34,6,95,-20,-65,-66,111,
		-71,-86,9,111,-58,-68,-19,-71,-104,-122,-80,81,83,-84,-120,-104,2,38,-7,1,47,-126,2,52,-6,0,116,-120,1,52,-8,7,-72,-48,-98,1,-15,-103,-46,0,0,2,0,93,-1,-20,3,-13,4,78,0,21,0,29,0,108,-78,8,30,31,17,18,57,-80,8,16,-80,22,-48,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,0,47,
		27,-79,0,18,62,89,-78,26,8,0,17,18,57,-80,26,47,-76,-65,26,-49,26,2,93,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-78,19,8,0,17,18,57,-80,8,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,0,53,53,52,54,54,51,50,18,17,21,33,22,22,
		51,50,54,55,23,6,1,34,6,7,33,53,38,38,2,77,-36,-2,-20,123,-35,-127,-45,-22,-3,35,4,-77,-118,98,-120,51,113,-120,-2,-39,112,-104,18,2,30,8,-120,20,1,33,-14,34,-95,-3,-113,-2,-22,-2,-3,77,-96,-59,80,66,88,-47,3,-54,-93,-109,14,-115,-101,0,0,1,0,60,0,0,2,-54,6,21,0,21,0,101,-78,15,22,23,17,18,
		57,0,-80,0,69,88,-80,8,47,27,-79,8,32,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,3,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,19,-48,
		-80,20,-48,48,49,51,17,35,53,51,53,52,54,51,50,23,7,38,35,34,6,21,21,51,21,35,17,-25,-85,-85,-70,-86,64,63,10,47,53,90,98,-25,-25,3,-85,-113,111,-82,-66,17,-106,9,105,98,114,-113,-4,85,0,2,0,96,-2,86,3,-14,4,78,0,25,0,36,0,-122,-78,34,37,38,17,18,57,-80,34,16,-80,11,-48,0,-80,0,69,88,
		-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,20,62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-78,5,3,23,17,18,57,-78,15,23,11,17,18,57,-80,11,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-78,21,3,23,17,18,57,-80,23,16,-79,29,1,
		-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,55,51,17,20,6,35,34,38,39,55,22,51,50,54,53,53,6,35,34,2,55,20,22,51,50,55,17,38,35,34,6,96,-22,-63,-58,111,9,-87,-7,-46,117,-32,59,96,119,-84,-121,-105,111,-64,-66,-21,-70,-106,-121,-81,82,85,
		-86,-121,-104,2,38,-3,1,43,-116,120,-5,-32,-46,-14,100,87,111,-109,-104,-118,93,-128,1,50,-13,-73,-47,-97,1,-18,-101,-46,0,1,0,-116,0,0,3,-33,6,0,0,17,0,74,-78,10,18,19,17,18,57,0,-80,16,47,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,0,69,88,-80,14,47,
		27,-79,14,18,62,89,-78,0,2,5,17,18,57,-80,2,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,54,51,32,19,17,35,17,38,38,35,34,6,7,17,35,17,51,1,69,123,-59,1,87,3,-71,1,105,111,90,-120,38,-71,-71,3,-73,-105,-2,125,-3,53,2,-52,117,112,96,78,-4,-3,6,0,0,0,2,0,-115,0,0,1,104,
		5,-60,0,3,0,12,0,63,-78,6,13,14,17,18,57,-80,6,16,-80,1,-48,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,2,16,-80,10,-48,-80,10,47,-79,6,5,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,51,3,52,54,50,22,20,6,34,38,1,85,-71,-71,-56,55,
		108,56,56,108,55,4,58,1,31,45,62,62,90,60,60,0,0,2,-1,-65,-2,75,1,89,5,-60,0,12,0,22,0,75,-78,16,23,24,17,18,57,-80,16,16,-80,0,-48,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,20,62,89,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-80,21,-48,
		-80,21,47,-79,16,5,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,16,33,34,39,53,22,51,50,54,53,17,3,52,54,51,50,22,20,6,34,38,1,75,-2,-27,61,52,32,52,62,65,19,55,53,54,56,56,108,54,4,58,-5,73,-2,-56,18,-108,8,67,83,4,-69,1,31,44,63,62,90,60,60,0,0,1,0,-115,0,0,4,12,6,0,
		0,12,0,117,0,-80,0,69,88,-80,4,47,27,-79,4,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,8,2,17,18,57,64,21,58,0,74,0,90,0,106,0,122,0,-118,0,-102,0,-86,0,-70,0,-54,0,10,93,-78,6,
		8,2,17,18,57,64,21,54,6,70,6,86,6,102,6,118,6,-122,6,-106,6,-90,6,-74,6,-58,6,10,93,48,49,1,7,17,35,17,51,17,55,1,51,1,1,35,1,-70,116,-71,-71,99,1,81,-31,-2,91,1,-42,-39,1,-11,121,-2,-124,6,0,-4,95,119,1,100,-2,60,-3,-118,0,1,0,-100,0,0,1,85,6,0,0,3,0,29,0,-80,
		0,69,88,-80,2,47,27,-79,2,32,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,48,49,33,35,17,51,1,85,-71,-71,6,0,0,0,1,0,-117,0,0,6,120,4,78,0,29,0,120,-78,4,30,31,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,
		-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,27,47,27,-79,27,18,62,89,-78,1,8,11,17,18,57,-78,5,8,11,17,18,57,-80,8,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,24,-48,48,49,1,23,54,51,50,23,54,54,
		51,32,19,17,35,17,52,38,35,34,6,7,17,35,17,52,35,34,7,17,35,17,1,58,5,119,-54,-29,82,54,-83,118,1,100,6,-71,106,125,103,-120,11,-70,-25,-74,67,-71,4,58,120,-116,-82,78,96,-2,-121,-3,43,2,-54,116,115,123,104,-3,50,2,-59,-20,-101,-4,-22,4,58,0,0,1,0,-116,0,0,3,-33,4,78,0,17,0,84,-78,11,
		18,19,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,1,3,6,17,18,57,-80,3,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,23,54,51,32,19,17,35,
		17,38,38,35,34,6,7,17,35,17,1,59,6,124,-56,1,87,3,-71,1,105,111,90,-120,38,-71,4,58,-120,-100,-2,125,-3,53,2,-52,117,112,96,78,-4,-3,4,58,0,2,0,91,-1,-20,4,52,4,78,0,15,0,27,0,69,-78,12,28,29,17,18,57,-80,12,16,-80,19,-48,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,
		88,-80,12,47,27,-79,12,18,62,89,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,54,51,50,0,21,21,20,6,6,35,34,0,53,23,20,22,51,50,54,53,52,38,35,34,6,91,125,-33,-113,-35,1,17,121,-31,-110,-36,-2,-17,-70,-89,-116,-115,-90,-87,-116,-119,-88,2,
		39,-97,-2,-118,-2,-50,-2,13,-98,-5,-116,1,50,-4,9,-76,-38,-35,-57,-78,-35,-38,0,2,0,-116,-2,96,4,30,4,78,0,15,0,26,0,112,-78,19,27,28,17,18,57,-80,19,16,-80,12,-48,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,20,62,
		89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,5,12,3,17,18,57,-78,10,12,3,17,18,57,-80,12,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,35,34,39,17,35,17,51,23,54,51,50,18,17,39,52,38,35,34,7,17,22,51,50,54,4,30,-30,
		-63,-59,113,-71,-87,9,113,-55,-61,-29,-71,-100,-120,-88,84,83,-85,-123,-99,2,17,-9,-2,-46,125,-3,-9,5,-38,120,-116,-2,-38,-2,-6,4,-73,-44,-107,-3,-5,-108,-45,0,0,2,0,95,-2,96,3,-17,4,78,0,15,0,26,0,109,-78,24,27,28,17,18,57,-80,24,16,-80,3,-48,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,
		88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,20,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,5,3,12,17,18,57,-78,10,3,12,17,18,57,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,55,51,17,35,17,
		6,35,34,2,53,23,20,22,51,50,55,17,38,35,34,6,95,-22,-59,-64,111,8,-86,-71,112,-70,-60,-23,-71,-99,-123,-91,87,88,-94,-122,-98,2,38,-1,1,41,-127,109,-6,38,2,4,120,1,49,-4,8,-70,-44,-110,2,18,-113,-43,0,1,0,-116,0,0,2,-105,4,78,0,13,0,71,-78,4,14,15,17,18,57,0,-80,0,69,88,-80,11,47,27,
		-79,11,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,11,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,9,11,5,17,18,57,48,49,1,38,35,34,7,17,35,17,51,23,54,51,50,23,2,-105,42,49,-74,65,-71,-76,3,91,-89,54,28,3,-108,7,-101,-3,0,4,58,
		125,-111,14,0,0,1,0,95,-1,-20,3,-69,4,78,0,38,0,100,-78,9,39,40,17,18,57,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,62,89,-78,3,28,9,17,18,57,-80,9,16,-80,13,-48,-80,9,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,21,1,-80,10,43,88,
		33,-40,27,-12,89,-80,28,16,-80,33,-48,-80,28,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,36,38,38,53,52,54,51,50,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,6,35,34,38,38,53,51,22,22,51,50,54,3,2,113,-2,-25,-91,79,-31,-81,-72,-27,-70,-127,98,101,114,106,1,21,-84,83,-24,-71,
		-126,-56,113,-71,5,-117,114,105,127,1,31,75,83,60,84,116,80,-123,-72,-66,-108,76,110,88,71,67,68,62,86,121,87,-111,-81,92,-91,96,93,109,85,0,0,1,0,9,-1,-20,2,86,5,64,0,21,0,97,-78,14,22,23,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-80,0,
		69,88,-80,13,47,27,-79,13,18,62,89,-80,1,16,-80,0,-48,-80,0,47,-80,1,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,17,-48,-80,18,-48,48,49,1,17,51,21,35,17,20,22,51,50,55,21,6,35,34,38,53,17,35,53,51,17,1,-121,-54,-54,54,65,32,
		56,73,69,124,126,-59,-59,5,64,-2,-6,-113,-3,97,65,65,12,-106,20,-106,-118,2,-97,-113,1,6,0,1,0,-120,-1,-20,3,-36,4,58,0,16,0,84,-78,10,17,18,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,
		88,-80,16,47,27,-79,16,18,62,89,-78,0,13,2,17,18,57,-80,2,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,35,34,38,39,17,51,17,20,51,50,55,17,51,17,35,3,40,108,-47,-83,-75,1,-71,-56,-44,70,-71,-80,107,127,-55,-59,2,-64,-3,69,-10,-98,3,19,-5,-58,0,1,0,33,0,0,3,-70,4,58,0,6,
		0,56,-78,0,7,8,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,0,5,3,17,18,57,48,49,37,1,51,1,35,1,51,1,-15,1,12,-67,-2,124,-115,-2,120,-67,-5,3,63,-5,-58,4,58,0,0,1,0,43,0,0,
		5,-45,4,58,0,12,0,96,-78,5,13,14,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,0,11,3,17,18,57,-78,5,
		11,3,17,18,57,-78,10,11,3,17,18,57,48,49,37,19,51,1,35,1,1,35,1,51,19,19,51,4,74,-48,-71,-2,-59,-106,-2,-7,-1,0,-106,-2,-58,-72,-43,-4,-107,-1,3,59,-5,-58,3,52,-4,-52,4,58,-4,-42,3,42,0,1,0,41,0,0,3,-54,4,58,0,11,0,83,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,
		69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,10,4,17,18,57,-78,6,10,4,17,18,57,-78,3,0,6,17,18,57,-78,9,6,0,17,18,57,48,49,1,19,51,1,1,35,3,3,35,1,1,51,1,-9,-16,-40,-2,-98,1,109,-42,-6,-6,
		-41,1,109,-2,-98,-42,2,-81,1,-117,-3,-23,-3,-35,1,-107,-2,107,2,35,2,23,0,1,0,22,-2,75,3,-80,4,58,0,15,0,74,-78,0,16,17,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,20,62,89,-78,0,14,5,17,18,57,
		-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,13,-48,48,49,1,19,51,1,2,35,39,39,53,23,50,54,55,55,1,51,1,-18,-4,-58,-2,77,101,-36,35,69,50,94,105,34,41,-2,126,-54,1,15,3,43,-5,31,-2,-14,3,13,-106,4,76,101,110,4,46,0,0,1,0,88,0,0,3,-77,4,58,0,9,0,70,0,-80,0,69,
		88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,0,2,17,18,57,-80,7,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-78,9,5,7,17,18,57,48,49,37,33,21,33,53,1,33,53,33,21,1,58,2,121,-4,-91,2,85,-3,-76,3,52,-105,-105,
		-120,3,25,-103,-125,0,0,1,0,64,-2,-110,2,-98,6,61,0,24,0,50,-78,19,25,26,17,18,57,0,-80,13,47,-80,0,47,-78,7,13,0,17,18,57,-80,7,47,-78,31,7,1,93,-79,6,3,-80,10,43,88,33,-40,27,-12,89,-78,19,6,7,17,18,57,48,49,1,38,38,53,53,52,35,53,50,53,53,54,54,55,23,6,17,21,20,7,
		22,21,21,18,23,2,120,-79,-77,-44,-44,2,-81,-77,38,-47,-89,-89,3,-50,-2,-110,50,-27,-68,-57,-13,-111,-14,-48,-73,-31,51,115,67,-2,-26,-54,-29,89,90,-27,-50,-2,-19,66,0,1,0,-81,-2,-14,1,68,5,-80,0,3,0,19,0,-80,0,47,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,48,49,1,35,17,51,1,68,-107,-107,-2,-14,6,
		-66,0,0,1,0,19,-2,-110,2,114,6,61,0,24,0,50,-78,5,25,26,17,18,57,0,-80,11,47,-80,24,47,-78,17,11,24,17,18,57,-80,17,47,-78,31,17,1,93,-79,18,3,-80,10,43,88,33,-40,27,-12,89,-78,5,18,17,17,18,57,48,49,23,54,19,53,52,55,38,53,53,16,39,55,22,22,23,21,20,51,21,34,21,21,20,6,
		7,19,-53,7,-75,-75,-47,38,-79,-78,1,-44,-44,-75,-81,-5,65,1,10,-36,-25,84,82,-23,-53,1,26,67,115,50,-31,-71,-46,-17,-111,-13,-54,-68,-30,50,0,1,0,-125,1,-110,4,-17,3,34,0,23,0,68,-78,17,24,25,17,18,57,0,-80,0,69,88,-80,15,47,27,-79,15,24,62,89,-80,0,-48,-80,15,16,-80,20,-48,-80,20,47,-79,3,1,
		-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,11,-48,48,49,1,20,6,35,34,46,2,35,34,6,21,7,52,54,51,50,22,22,23,23,50,54,53,4,-17,-69,-119,72,-128,-87,74,42,78,84,-95,-72,-117,76,-116,-80,64,29,76,95,3,9,-98,-39,53,-108,36,107,94,2,-96,-50,64,-95,
		10,2,116,95,0,2,0,-117,-2,-104,1,102,4,77,0,3,0,12,0,51,-78,6,13,14,17,18,57,-80,6,16,-80,0,-48,0,-80,2,47,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,6,5,-80,10,43,88,33,-40,27,-12,89,-78,1,2,6,17,18,57,48,49,19,51,19,35,19,20,6,34,38,52,54,50,22,-86,-88,13,-62,-55,55,
		108,56,56,108,55,2,-84,-5,-20,5,76,45,62,62,90,60,60,0,0,1,0,105,-1,11,3,-7,5,38,0,33,0,84,-78,0,34,35,17,18,57,0,-80,0,69,88,-80,20,47,27,-79,20,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,7,-48,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-80,3,-48,-80,20,16,
		-80,17,-48,-80,20,16,-80,24,-48,-80,20,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,50,54,55,51,6,6,7,21,35,53,38,2,53,53,52,18,55,53,51,21,22,22,23,35,38,38,35,34,6,21,21,20,22,2,74,100,-108,8,-81,6,-58,-112,-71,-77,-56,-54,-79,-71,-106,-64,6,-81,8,-113,105,-115,-101,-101,-125,121,89,126,-55,
		26,-23,-22,34,1,28,-36,35,-44,1,29,33,-30,-33,23,-44,-106,105,-121,-53,-64,35,-69,-54,0,1,0,91,0,0,4,104,5,-60,0,33,0,127,-78,28,34,35,17,18,57,0,-80,0,69,88,-80,20,47,27,-79,20,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,31,20,5,17,18,57,-80,31,47,-78,95,31,1,114,-78,-113,31,
		1,113,-78,-65,31,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,7,-48,-80,8,-48,-80,0,16,-80,13,-48,-80,31,16,-80,15,-48,-80,20,16,-80,24,-48,-80,20,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,23,20,7,33,7,33,53,51,54,54,55,53,39,35,
		53,51,3,52,54,51,50,22,21,35,52,38,35,34,6,21,19,33,21,1,-63,8,62,2,-35,1,-5,-8,77,40,50,2,8,-91,-96,9,-11,-56,-66,-34,-65,127,111,105,-126,9,1,63,2,110,-36,-102,91,-99,-99,9,-125,96,8,-35,-99,1,4,-57,-18,-44,-79,107,124,-102,125,-2,-4,-99,0,2,0,105,-1,-27,5,91,4,-15,0,27,0,42,0,65,
		-78,2,43,44,17,18,57,-80,2,16,-80,39,-48,0,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,16,-48,-80,16,47,-80,2,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-79,39,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,35,34,39,7,39,55,38,53,52,55,39,55,23,54,51,50,23,55,23,7,22,21,20,
		7,23,7,1,20,22,22,50,54,54,53,52,38,38,35,34,6,6,4,79,-97,-47,-49,-97,-122,-126,-117,104,112,-109,-126,-109,-98,-61,-60,-97,-107,-124,-105,110,102,-113,-124,-4,96,115,-60,-30,-60,113,113,-59,112,113,-60,115,112,-124,-126,-120,-121,-115,-100,-54,-50,-93,-105,-120,-106,120,121,-104,-119,-102,-93,-53,-60,-97,-112,-120,2,123,123,-44,122,123,-45,123,122,-45,
		121,120,-44,0,0,1,0,15,0,0,4,36,5,-80,0,22,0,113,-78,0,23,24,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,11,1,17,18,57,-78,7,1,11,17,18,57,-80,7,47,-80,3,-48,-80,3,47,-79,5,2,-80,10,43,88,33,-40,27,-12,89,-80,7,16,
		-79,9,2,-80,10,43,88,33,-40,27,-12,89,-80,13,-48,-80,7,16,-80,15,-48,-80,5,16,-80,17,-48,-80,3,16,-80,19,-48,-80,1,16,-80,21,-48,48,49,1,1,51,1,33,21,33,21,33,21,33,17,35,17,33,53,33,53,33,53,33,1,51,2,27,1,52,-43,-2,-111,1,5,-2,-68,1,68,-2,-68,-63,-2,-62,1,62,-2,-62,1,7,-2,-111,
		-40,3,25,2,-105,-3,48,125,-91,124,-2,-66,1,66,124,-91,125,2,-48,0,0,2,0,-109,-2,-14,1,77,5,-80,0,3,0,7,0,24,0,-80,0,47,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-78,5,1,3,43,48,49,19,17,51,17,17,35,17,51,-109,-70,-70,-70,-2,-14,3,23,-4,-23,3,-56,2,-10,0,2,0,90,-2,17,4,121,
		5,-60,0,52,0,68,0,-124,-78,35,69,70,17,18,57,-80,35,16,-80,53,-48,0,-80,8,47,-80,0,69,88,-80,35,47,27,-79,35,30,62,89,-78,22,8,35,17,18,57,-80,22,16,-79,63,1,-80,10,43,88,33,-40,27,-12,89,-78,2,22,63,17,18,57,-80,8,16,-80,14,-48,-80,8,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-78,48,
		35,8,17,18,57,-80,48,16,-79,55,1,-80,10,43,88,33,-40,27,-12,89,-78,29,55,48,17,18,57,-80,35,16,-80,39,-48,-80,35,16,-79,42,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,7,22,22,21,20,4,35,34,38,39,38,53,55,20,22,51,50,54,53,52,38,39,46,2,53,52,55,38,38,53,52,36,51,50,4,21,35,52,
		38,35,34,6,21,20,22,22,4,30,2,37,38,39,6,6,21,20,22,22,4,23,54,54,53,52,38,4,121,-70,69,72,-2,-4,-28,112,-55,70,-117,-70,-76,-100,-120,-90,-114,-47,-74,-64,93,-74,66,71,1,11,-34,-24,1,4,-71,-88,-117,-114,-95,56,-121,1,31,-87,113,58,-3,-31,90,75,80,75,54,-123,1,28,44,78,84,-117,1,-81,-67,85,49,-120,
		100,-88,-57,56,57,113,-51,2,-126,-105,117,96,89,105,62,48,111,-101,111,-70,88,49,-120,100,-90,-56,-30,-51,125,-101,115,98,69,80,65,80,72,97,-127,-85,24,27,19,101,69,70,80,66,82,17,20,101,69,88,109,0,0,2,0,101,4,-16,2,-18,5,-59,0,8,0,17,0,30,0,-80,7,47,-79,2,5,-80,10,43,88,33,-40,27,-12,89,-80,11,
		-48,-80,7,16,-80,16,-48,-80,16,47,48,49,19,52,54,50,22,20,6,34,38,37,52,54,50,22,20,6,34,38,101,55,108,56,56,108,55,1,-82,55,108,56,56,108,55,5,91,45,61,61,90,60,60,43,45,62,62,90,60,60,0,3,0,91,-1,-21,5,-26,5,-60,0,27,0,42,0,57,0,-103,-78,39,58,59,17,18,57,-80,39,16,-80,3,
		-48,-80,39,16,-80,54,-48,0,-80,0,69,88,-80,46,47,27,-79,46,30,62,89,-80,0,69,88,-80,54,47,27,-79,54,18,62,89,-78,3,54,46,17,18,57,-80,3,47,-76,15,3,31,3,2,93,-78,10,46,54,17,18,57,-80,10,47,-76,0,10,16,10,2,93,-78,14,10,3,17,18,57,-79,17,2,-80,10,43,88,33,-40,27,-12,89,-80,3,16,
		-79,24,2,-80,10,43,88,33,-40,27,-12,89,-78,27,3,10,17,18,57,-80,54,16,-79,32,4,-80,10,43,88,33,-40,27,-12,89,-80,46,16,-79,39,4,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,35,34,38,53,53,52,54,51,50,22,21,35,52,38,35,34,6,21,21,20,22,51,50,54,53,37,20,18,4,32,36,18,53,52,2,36,
		35,34,4,2,7,52,18,36,32,4,18,21,20,2,4,35,34,36,2,4,95,-83,-98,-99,-67,-65,-101,-96,-84,-110,95,91,94,108,108,94,92,93,-3,1,-96,1,19,1,64,1,18,-96,-98,-2,-19,-95,-96,-2,-20,-97,115,-69,1,75,1,-128,1,74,-69,-76,-2,-75,-58,-59,-2,-75,-74,2,85,-103,-95,-45,-74,110,-80,-45,-92,-107,99,85,-118,123,113,120,
		-118,84,101,-124,-84,-2,-37,-90,-90,1,37,-84,-86,1,34,-89,-91,-2,-36,-86,-54,1,90,-57,-57,-2,-90,-54,-59,-2,-88,-47,-49,1,88,0,0,2,0,-109,2,-77,3,15,5,-60,0,27,0,37,0,111,-78,14,38,39,17,18,57,-80,14,16,-80,29,-48,0,-80,0,69,88,-80,21,47,27,-79,21,30,62,89,-78,4,38,21,17,18,57,-80,4,47,-80,
		0,-48,-78,2,4,21,17,18,57,-78,11,4,21,17,18,57,-80,11,47,-80,21,16,-79,14,3,-80,10,43,88,33,-40,27,-12,89,-78,17,11,21,17,18,57,-80,4,16,-79,28,3,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,32,4,-80,10,43,88,33,-40,27,-12,89,48,49,1,38,39,6,35,34,38,53,52,54,51,51,53,52,35,34,6,
		21,39,52,54,51,50,22,21,17,20,23,37,50,54,55,53,35,6,6,21,20,2,106,12,6,76,-128,119,-126,-89,-84,108,124,69,79,-95,-84,-119,-123,-102,26,-2,-92,43,88,28,112,83,89,2,-63,34,38,86,124,103,111,120,52,-121,54,51,12,103,-126,-113,-122,-2,-60,97,81,123,40,27,-114,1,63,51,94,0,-1,-1,0,102,0,-105,3,100,3,-77,
		0,38,1,-110,-6,-2,0,7,1,-110,1,68,-1,-2,0,1,0,127,1,119,3,-66,3,32,0,5,0,27,0,-80,4,47,-80,1,-48,-80,1,47,-80,4,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,33,53,33,3,-66,-70,-3,123,3,63,1,119,1,8,-95,0,0,4,0,90,-1,-21,5,-27,5,-60,0,14,0,30,0,52,
		0,61,0,-83,-78,54,62,63,17,18,57,-80,54,16,-80,11,-48,-80,54,16,-80,19,-48,-80,54,16,-80,35,-48,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-79,19,4,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,27,4,-80,10,43,88,33,-40,27,-12,89,-78,32,11,3,17,18,57,
		-80,32,47,-78,34,3,11,17,18,57,-80,34,47,-76,0,34,16,34,2,93,-78,53,32,34,17,18,57,-80,53,47,-78,-65,53,1,93,-76,0,53,16,53,2,93,-79,31,2,-80,10,43,88,33,-40,27,-12,89,-78,40,31,53,17,18,57,-80,32,16,-80,47,-48,-80,47,47,-80,34,16,-79,61,2,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,
		36,32,4,18,21,20,2,4,35,34,36,2,55,20,18,4,51,50,36,18,53,52,2,36,35,34,4,2,5,17,35,17,33,50,22,21,20,7,22,23,21,20,23,21,35,38,52,39,38,39,39,51,54,54,53,52,38,35,35,90,-69,1,75,1,-128,1,74,-69,-76,-2,-75,-58,-59,-2,-75,-74,115,-96,1,19,-96,-95,1,20,-99,-99,-2,-20,-95,-96,
		-2,-20,-97,1,-64,-115,1,20,-103,-87,-128,122,1,17,-111,14,3,16,115,-80,-100,72,88,78,100,-118,2,-39,-54,1,90,-57,-57,-2,-90,-54,-59,-2,-88,-47,-49,1,88,-57,-84,-2,-37,-90,-87,1,34,-84,-85,1,33,-89,-91,-2,-36,-11,-2,-82,3,81,-125,125,123,65,50,-102,61,86,38,16,36,-71,17,96,4,-128,2,66,54,73,61,0,0,1,0,-114,
		5,22,3,46,5,-91,0,3,0,25,-78,1,4,5,17,18,57,0,-80,2,47,-79,0,16,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,3,46,-3,96,2,-96,5,22,-113,0,2,0,-126,3,-64,2,124,5,-60,0,11,0,22,0,49,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,12,-48,-80,12,47,-79,9,2,-80,10,43,
		88,33,-40,27,-12,89,-80,3,16,-79,18,2,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,51,50,22,21,20,6,35,34,38,23,50,54,53,52,38,35,34,6,20,22,-126,-107,106,104,-109,-109,104,105,-106,-1,54,74,74,54,55,75,75,4,-64,104,-100,-101,105,106,-106,-106,22,71,57,58,75,79,108,74,0,2,0,97,0,0,3,-11,4,-13,
		0,11,0,15,0,72,0,-80,9,47,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,9,16,-80,0,-48,-80,9,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,13,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-78,5,14,6,17,18,57,-76,11,5,27,5,2,93,48,49,1,33,21,33,17,35,17,33,53,33,17,51,
		1,33,53,33,2,-119,1,108,-2,-108,-89,-2,127,1,-127,-89,1,65,-4,-67,3,67,3,86,-105,-2,98,1,-98,-105,1,-99,-5,13,-104,0,0,1,0,66,2,-101,2,-85,5,-69,0,22,0,86,-78,8,23,24,17,18,57,0,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,22,62,89,-79,22,2,-80,10,43,
		88,33,-40,27,-12,89,-80,2,-48,-78,3,14,22,17,18,57,-80,14,16,-79,8,2,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-80,11,-48,-78,20,22,14,17,18,57,48,49,1,33,53,1,54,53,52,38,35,34,6,21,35,52,54,32,22,21,20,15,2,33,2,-85,-3,-87,1,44,109,64,60,75,71,-99,-89,1,8,-102,107,84,-80,1,-113,2,
		-101,108,1,26,102,69,49,61,76,57,114,-108,127,110,104,107,79,-111,0,1,0,62,2,-112,2,-102,5,-69,0,38,0,-116,-78,32,39,40,17,18,57,0,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,-80,0,69,88,-80,25,47,27,-79,25,22,62,89,-78,0,25,14,17,18,57,-80,0,47,-74,111,0,127,0,-113,0,3,93,-78,63,0,1,113,
		-74,15,0,31,0,47,0,3,93,-78,95,0,1,114,-80,14,16,-79,7,2,-80,10,43,88,33,-40,27,-12,89,-78,10,14,25,17,18,57,-80,0,16,-79,38,4,-80,10,43,88,33,-40,27,-12,89,-78,20,38,0,17,18,57,-78,29,25,14,17,18,57,-80,25,16,-79,32,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,50,54,53,52,38,35,
		34,6,21,35,52,54,51,50,22,21,20,6,7,22,21,20,6,35,34,38,53,51,20,22,51,50,54,53,52,39,35,1,9,84,74,72,63,70,57,75,-99,-93,124,-119,-100,70,66,-107,-86,-120,-124,-90,-98,79,67,70,73,-100,88,4,102,61,48,45,58,51,41,98,123,121,104,55,91,25,41,-113,106,125,126,107,45,60,60,51,113,2,0,1,0,123,
		4,-38,2,28,6,0,0,3,0,35,0,-80,2,47,-78,15,2,1,93,-80,0,-48,-80,0,47,-76,15,0,31,0,2,93,-80,2,16,-80,3,-48,25,-80,3,47,24,48,49,1,51,1,35,1,60,-32,-2,-12,-107,6,0,-2,-38,0,0,1,0,-102,-2,96,3,-18,4,58,0,18,0,81,-78,13,19,20,17,18,57,0,-80,0,69,88,-80,0,47,27,
		-79,0,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,16,47,27,-79,16,20,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,11,7,13,17,18,57,48,49,1,17,22,22,51,50,55,17,51,17,35,39,6,35,34,39,17,35,17,1,83,1,103,116,-57,
		62,-70,-89,9,93,-86,-109,81,-71,4,58,-3,-121,-93,-100,-104,3,32,-5,-58,115,-121,73,-2,43,5,-38,0,0,1,0,67,0,0,3,64,5,-80,0,10,0,43,-78,2,11,12,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,1,0,8,17,18,57,48,49,33,17,35,34,36,
		53,52,36,51,33,17,2,-122,84,-26,-2,-9,1,10,-26,1,13,2,8,-2,-42,-43,-1,-6,80,0,0,1,0,-109,2,107,1,121,3,73,0,9,0,23,-78,3,10,11,17,18,57,0,-80,2,47,-80,8,-80,10,43,88,-40,27,-36,89,48,49,19,52,54,50,22,21,20,6,34,38,-109,57,114,59,59,114,57,2,-39,48,64,64,48,47,63,63,0,
		0,1,0,116,-2,77,1,-86,0,0,0,14,0,66,-78,5,15,16,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,20,62,89,-76,19,6,35,6,2,93,-78,1,6,0,17,18,57,-80,7,-80,10,43,88,-40,27,-36,89,-80,1,16,-80,13,-48,48,49,33,7,22,21,20,6,35,39,50,54,
		53,52,38,39,55,1,29,12,-103,-96,-113,7,79,87,64,98,32,52,27,-110,97,113,107,52,47,44,42,9,-122,0,0,1,0,122,2,-101,1,-17,5,-80,0,6,0,65,-78,1,7,8,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,22,62,89,-78,4,0,5,17,18,57,-80,4,47,-79,3,
		2,-80,10,43,88,33,-40,27,-12,89,-78,2,3,5,17,18,57,48,49,1,35,17,7,53,37,51,1,-17,-99,-40,1,99,18,2,-101,2,89,57,-128,117,0,2,0,122,2,-78,3,39,5,-60,0,12,0,26,0,66,-78,3,27,28,17,18,57,-80,3,16,-80,16,-48,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-78,10,27,3,17,18,57,
		-80,10,47,-79,16,3,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,23,3,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,51,50,22,21,21,20,6,32,38,53,23,20,22,51,50,54,53,53,52,38,35,34,6,7,122,-68,-102,-101,-68,-69,-2,-52,-66,-93,97,84,83,95,97,83,81,96,2,4,99,-98,-61,-63,-90,74,-97,-62,-62,-91,6,
		100,114,115,101,78,99,114,110,97,0,-1,-1,0,102,0,-104,3,120,3,-75,0,38,1,-109,13,0,0,7,1,-109,1,106,0,0,-1,-1,0,85,0,0,5,-111,5,-83,0,39,1,-58,-1,-37,2,-104,0,39,1,-108,1,24,0,8,1,7,2,32,2,-42,0,0,0,16,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,48,49,-1,-1,0,80,
		0,0,5,-55,5,-83,0,39,1,-108,0,-20,0,8,0,39,1,-58,-1,-42,2,-104,1,7,1,-59,3,30,0,0,0,16,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,48,49,-1,-1,0,111,0,0,5,-19,5,-69,0,39,1,-108,1,-105,0,8,0,39,2,32,3,50,0,0,1,7,2,31,0,49,2,-101,0,16,0,-80,0,69,88,-80,
		33,47,27,-79,33,30,62,89,48,49,0,2,0,68,-2,127,3,120,4,77,0,24,0,34,0,89,-78,9,35,36,17,18,57,-80,9,16,-80,28,-48,0,-80,16,47,-80,0,69,88,-80,33,47,27,-79,33,26,62,89,-78,0,16,33,17,18,57,-78,3,16,0,17,18,57,-80,16,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,12,
		-48,-78,21,0,16,17,18,57,-80,33,16,-79,27,5,-80,10,43,88,33,-40,27,-12,89,48,49,1,14,3,7,7,20,22,51,50,54,53,51,6,6,35,34,38,53,52,55,55,54,53,19,20,6,34,38,53,52,54,50,22,2,76,1,41,96,-72,11,2,116,109,100,125,-71,2,-31,-73,-60,-42,-96,109,66,-63,55,108,56,56,108,55,2,-88,106,127,
		118,-63,99,37,109,115,113,91,-95,-52,-55,-77,-83,-81,113,78,-110,1,61,45,62,62,45,44,60,60,0,2,-1,-14,0,0,7,87,5,-80,0,15,0,18,0,123,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,17,6,0,17,18,57,-80,
		17,47,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-78,11,0,6,17,18,57,-80,11,47,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-78,18,6,0,17,18,57,48,49,33,33,3,33,3,35,1,33,21,33,19,33,21,33,19,
		33,1,33,3,7,87,-4,-115,15,-3,-52,-51,-30,3,112,3,-73,-3,77,20,2,78,-3,-72,22,2,-63,-6,-81,1,-56,31,1,97,-2,-97,5,-80,-104,-2,41,-105,-3,-19,1,120,2,-35,0,1,0,89,0,-50,3,-35,4,99,0,11,0,56,0,-80,3,47,-78,9,12,3,17,18,57,-80,9,47,-78,10,9,3,17,18,57,-78,4,3,9,17,18,57,
		-78,1,10,4,17,18,57,-80,3,16,-80,5,-48,-78,7,4,10,17,18,57,-80,9,16,-80,11,-48,48,49,19,1,1,55,1,1,23,1,1,7,1,1,89,1,74,-2,-72,119,1,73,1,73,119,-2,-72,1,74,119,-2,-75,-2,-75,1,73,1,80,1,79,123,-2,-79,1,79,123,-2,-79,-2,-80,123,1,81,-2,-81,0,0,3,0,118,-1,-93,5,29,
		5,-20,0,23,0,32,0,41,0,104,-78,4,42,43,17,18,57,-80,4,16,-80,29,-48,-80,4,16,-80,38,-48,0,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,26,16,4,17,18,57,-78,35,16,4,17,18,57,-80,35,16,-80,27,-48,-80,16,16,-79,29,1,-80,10,43,88,33,-40,27,-12,
		89,-80,26,16,-80,36,-48,-80,4,16,-79,38,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,4,35,34,39,7,35,55,38,17,53,52,18,36,51,50,23,55,51,7,22,19,5,20,23,1,38,35,34,2,7,5,52,39,1,22,51,50,18,55,5,9,-112,-2,-8,-80,-85,-125,97,-114,-112,-66,-110,1,11,-84,-42,-108,103,-115,-97,-119,2,-4,
		44,98,2,52,102,-90,-74,-47,3,3,21,56,-3,-37,91,121,-70,-52,3,2,-87,-42,-2,-63,-88,82,-101,-25,-64,1,104,83,-46,1,66,-85,125,-91,-1,-69,-2,-38,99,-12,-115,3,-120,111,-2,-21,-10,13,-74,-125,-4,-113,64,1,15,-3,0,2,0,-90,0,0,4,93,5,-80,0,13,0,22,0,89,-78,9,23,24,17,18,57,-80,9,16,-80,16,-48,0,
		-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,1,0,11,17,18,57,-80,1,47,-78,16,0,11,17,18,57,-80,16,47,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,33,50,22,22,21,20,4,35,33,17,35,17,19,
		17,33,50,54,53,52,38,39,1,96,1,23,-109,-36,119,-2,-8,-29,-2,-18,-70,-70,1,21,-114,-96,-96,-120,5,-80,-2,-37,105,-62,126,-62,-25,-2,-57,5,-80,-2,67,-3,-34,-105,120,123,-105,1,0,1,0,-117,-1,-20,4,106,6,18,0,42,0,107,-78,33,43,44,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,32,62,89,-80,0,69,88,-80,
		19,47,27,-79,19,18,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,10,19,5,17,18,57,-78,14,5,19,17,18,57,-80,19,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,32,19,5,17,18,57,-78,35,5,19,17,18,57,-80,5,16,-79,40,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,52,54,51,50,22,21,
		20,6,21,20,30,2,21,20,6,35,34,38,39,55,22,22,51,50,54,53,52,46,2,53,52,54,53,52,38,35,34,17,1,68,-71,-49,-70,-76,-59,-128,75,-68,86,-53,-74,81,-75,38,43,49,-121,53,107,113,74,-67,87,-117,104,88,-38,4,87,-48,-21,-77,-97,125,-53,69,51,95,-112,-120,76,-97,-78,44,28,-101,32,44,94,82,52,96,-109,-118,81,89,
		-49,84,94,107,-2,-37,0,3,0,78,-1,-20,6,124,4,78,0,42,0,53,0,61,0,-54,-78,2,62,63,17,18,57,-80,2,16,-80,46,-48,-80,2,16,-80,57,-48,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,0,69,88,-80,29,47,27,-79,29,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,5,47,
		27,-79,5,18,62,89,-78,2,29,0,17,18,57,-78,12,5,23,17,18,57,-80,12,47,-76,-65,12,-49,12,2,93,-80,23,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-78,19,12,23,17,18,57,-78,26,29,0,17,18,57,-78,58,29,0,17,18,57,-80,58,47,-76,-65,58,-49,58,2,93,-79,33,1,-80,10,43,88,33,-40,27,-12,89,-80,0,
		16,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,40,29,0,17,18,57,-80,43,-48,-80,12,16,-79,47,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,54,-48,48,49,5,32,39,6,6,35,34,38,53,52,54,51,51,53,52,38,35,34,6,21,39,52,54,51,50,22,23,54,54,51,50,18,21,21,33,22,22,51,50,55,55,23,6,37,
		50,54,55,53,35,6,6,21,20,22,1,34,6,7,33,53,52,38,4,-18,-2,-5,-120,65,-30,-115,-89,-68,-29,-35,-33,110,104,105,-116,-72,-14,-69,115,-80,50,63,-82,105,-46,-24,-3,40,7,-82,-107,-108,121,47,64,-98,-4,9,72,-98,50,-28,117,-116,106,3,80,115,-107,17,2,26,-122,20,-76,86,94,-83,-105,-99,-82,85,107,123,110,81,19,-113,-75,83,
		83,79,87,-2,-1,-23,115,-80,-65,76,31,-120,121,-106,74,54,-19,2,110,83,77,93,3,52,-85,-117,31,-124,-109,0,0,2,0,126,-1,-20,4,45,6,44,0,29,0,43,0,86,-78,7,44,45,17,18,57,-80,7,16,-80,40,-48,0,-80,0,69,88,-80,25,47,27,-79,25,32,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,15,25,7,
		17,18,57,-80,15,47,-78,17,25,7,17,18,57,-79,34,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,40,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,18,17,21,20,6,6,35,34,38,38,53,52,54,54,51,50,23,38,39,7,39,55,38,39,55,22,23,55,23,3,39,38,38,35,34,6,21,20,22,51,50,54,53,3,52,-7,117,
		-40,-122,-121,-36,121,112,-49,-127,-93,121,48,-115,-38,73,-64,-124,-73,57,-17,-81,-67,73,104,2,33,-117,92,-111,-94,-89,-128,125,-103,5,21,-2,-8,-2,103,93,-98,-3,-112,-127,-32,-122,-109,-23,-126,114,-61,-115,-108,99,-125,91,49,-97,54,-117,-127,100,-4,-13,56,61,73,-65,-89,-116,-60,-30,-72,0,0,3,0,71,0,-84,4,45,4,-70,0,3,0,13,0,23,
		0,83,-78,7,24,25,17,18,57,-80,7,16,-80,0,-48,-80,7,16,-80,17,-48,0,-80,2,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-80,12,-80,10,43,88,-40,27,-36,89,-80,6,-80,10,43,88,-40,27,-36,89,-80,1,16,-80,16,-80,10,43,88,-40,27,-36,89,-80,22,-80,10,43,88,-40,27,-36,89,48,49,1,33,53,33,1,
		52,54,50,22,21,20,6,34,38,17,52,54,50,22,21,20,6,34,38,4,45,-4,26,3,-26,-3,-96,57,114,59,59,114,57,57,114,59,59,114,57,2,88,-72,1,58,48,64,64,48,47,62,62,-4,-2,48,64,64,48,46,63,63,0,3,0,91,-1,122,4,52,4,-72,0,21,0,29,0,38,0,101,-78,4,39,40,17,18,57,-80,4,16,-80,27,
		-48,-80,4,16,-80,35,-48,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-79,35,1,-80,10,43,88,33,-40,27,-12,89,-78,33,35,4,17,18,57,-80,33,16,-80,24,-48,-80,4,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,25,27,15,17,18,57,-80,25,16,-80,32,-48,48,49,19,
		52,54,54,51,50,23,55,51,7,22,17,20,6,6,35,34,39,7,35,55,38,19,20,23,1,38,35,34,6,5,52,39,1,22,51,50,54,53,91,123,-31,-113,110,94,73,124,102,-61,124,-32,-112,104,86,74,124,100,-51,-71,97,1,87,62,72,-118,-88,2,102,87,-2,-84,55,66,-117,-89,2,39,-97,-3,-117,42,-108,-51,-102,-2,-64,-98,-2,-119,35,-107,
		-53,-107,1,55,-62,111,2,-74,32,-38,-75,-74,111,-3,80,25,-37,-71,0,2,0,-107,-2,96,4,39,6,0,0,15,0,26,0,102,-78,24,27,28,17,18,57,-80,24,16,-80,12,-48,0,-80,8,47,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,20,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,
		-78,5,12,3,17,18,57,-78,10,12,3,17,18,57,-80,12,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,35,34,39,17,35,17,51,17,54,51,50,18,17,39,52,38,35,34,7,17,22,51,50,54,4,39,-30,-63,-59,113,-71,-71,113,-62,-61,-29,-71,-100,-120,-88,84,
		83,-85,-123,-99,2,17,-9,-2,-46,125,-3,-9,7,-96,-3,-54,-124,-2,-38,-2,-6,4,-73,-44,-107,-3,-5,-108,-45,0,0,2,0,95,-1,-20,4,-84,6,0,0,23,0,34,0,-126,0,-80,20,47,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,
		15,20,1,93,-78,47,20,1,93,-78,19,3,20,17,18,57,-80,19,47,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-78,4,6,13,17,18,57,-78,15,13,6,17,18,57,-80,19,16,-80,22,-48,-80,6,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,35,
		39,6,35,34,2,53,53,52,18,51,50,23,17,33,53,33,53,51,21,51,1,20,22,51,50,55,17,38,35,34,6,4,-84,-68,-86,9,111,-58,-68,-19,-20,-65,-66,111,-2,-8,1,8,-71,-68,-4,108,-104,-122,-80,81,83,-84,-120,-104,4,-47,-5,47,116,-120,1,52,-8,14,-7,1,47,-126,1,5,-105,-104,-104,-4,-87,-72,-48,-98,1,-15,-103,-46,0,2,
		0,29,0,0,5,-120,5,-80,0,19,0,23,0,109,0,-80,0,69,88,-80,15,47,27,-79,15,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,20,8,15,17,18,57,-80,20,47,-78,16,20,15,17,18,57,-80,16,47,-80,0,-48,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,8,16,-80,5,-48,-80,20,
		16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,23,16,-80,10,-48,-80,16,16,-80,13,-48,-80,15,16,-80,18,-48,48,49,1,51,21,35,17,35,17,33,17,35,17,35,53,51,17,51,17,33,17,51,1,33,53,33,5,2,-122,-122,-63,-3,35,-63,-122,-122,-63,2,-35,-63,-4,98,2,-35,-3,35,4,-114,-114,-4,0,2,-95,-3,95,4,0,-114,1,
		34,-2,-34,1,34,-3,-114,-62,0,1,0,-101,0,0,1,85,4,58,0,3,0,29,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,48,49,33,35,17,51,1,85,-70,-70,4,58,0,0,1,0,-102,0,0,4,63,4,58,0,12,0,105,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,
		-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,2,16,-80,6,-48,-80,6,47,-78,-97,6,1,93,-76,-65,6,-49,6,2,93,-78,47,6,1,93,-78,-1,6,1,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,10,1,6,17,18,57,48,
		49,1,35,17,35,17,51,17,51,1,51,1,1,35,1,-65,107,-70,-70,91,1,-115,-33,-2,60,1,-24,-23,1,-51,-2,51,4,58,-2,54,1,-54,-3,-13,-3,-45,0,1,0,34,0,0,4,27,5,-80,0,13,0,93,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,1,12,6,17,18,57,
		-80,1,47,-80,0,-48,-80,1,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,6,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,8,-48,-80,9,-48,-80,0,16,-80,11,-48,-80,10,-48,48,49,1,37,21,5,17,33,21,33,17,7,53,55,17,51,1,105,1,7,-2,-7,2,-78,-4,-115,-122,-122,-63,3,75,84,125,
		84,-3,-49,-99,2,-111,42,125,42,2,-94,0,0,1,0,34,0,0,2,10,6,0,0,11,0,75,0,-80,0,69,88,-80,10,47,27,-79,10,32,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,1,4,10,17,18,57,-80,1,47,-80,0,-48,-80,1,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,6,-48,-80,7,-48,
		-80,0,16,-80,9,-48,-80,8,-48,48,49,1,55,21,7,17,35,17,7,53,55,17,51,1,108,-98,-98,-70,-112,-112,-70,3,101,61,123,61,-3,22,2,-93,55,123,55,2,-30,0,0,1,0,-94,-2,75,4,-15,5,-80,0,19,0,91,-78,6,20,21,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,16,47,27,-79,
		16,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,20,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-80,4,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,13,14,16,17,18,57,-78,18,14,0,17,18,57,48,49,1,17,20,6,35,34,39,55,22,51,50,53,53,1,17,35,17,51,1,17,4,-15,-85,-100,61,54,14,37,61,
		-120,-3,51,-64,-64,2,-51,5,-80,-7,-3,-88,-70,18,-102,14,-48,71,4,106,-5,-106,5,-80,-5,-104,4,104,0,1,0,-111,-2,75,3,-16,4,78,0,26,0,99,-78,13,27,28,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-80,
		0,69,88,-80,24,47,27,-79,24,18,62,89,-78,1,24,3,17,18,57,-80,10,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,23,54,51,50,22,23,17,20,6,35,34,39,55,22,51,50,53,17,52,38,35,34,7,17,35,17,1,55,13,116,-53,-77,-72,2,-89,-101,61,54,
		14,35,66,-119,111,125,-81,81,-70,4,58,-102,-82,-48,-53,-4,-12,-92,-72,18,-99,13,-62,2,-9,-117,-128,-123,-4,-44,4,58,0,2,0,104,-1,-21,7,9,5,-60,0,23,0,35,0,-106,-78,1,36,37,17,18,57,-80,1,16,-80,26,-48,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,-80,0,
		69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,14,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-78,19,0,14,17,18,57,-80,19,47,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,
		89,-80,12,16,-79,29,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,33,6,35,34,38,2,39,17,52,18,54,51,50,23,33,21,33,17,33,21,33,17,33,5,50,55,17,38,35,34,6,7,17,20,22,7,9,-4,-80,-78,114,-94,-2,-116,1,-117,-2,-94,124,-86,3,70,-3,45,2,119,-3,-119,2,-35,-5,-116,113,102,109,108,-83,-62,2,-61,21,
		-106,1,15,-85,1,53,-84,1,17,-105,20,-98,-2,44,-99,-3,-4,27,14,4,-114,15,-27,-49,-2,-57,-45,-21,0,3,0,97,-1,-20,7,0,4,78,0,32,0,44,0,52,0,-103,-78,6,53,54,17,18,57,-80,6,16,-80,38,-48,-80,6,16,-80,48,-48,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,
		62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-80,0,69,88,-80,29,47,27,-79,29,18,62,89,-78,7,10,23,17,18,57,-78,49,10,23,17,18,57,-80,49,47,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,23,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-78,20,10,23,17,18,57,-78,26,10,23,17,18,57,-80,36,-48,-80,
		4,16,-79,42,1,-80,10,43,88,33,-40,27,-12,89,-80,45,-48,48,49,19,52,54,54,51,50,22,23,54,54,51,50,22,21,21,33,22,22,51,50,55,23,6,35,34,38,39,6,6,35,34,0,53,23,20,22,51,50,54,53,52,38,35,34,6,37,34,6,7,33,53,52,38,97,121,-37,-114,-119,-55,61,65,-60,112,-49,-22,-3,50,7,-92,-122,-68,
		120,74,-119,-11,-121,-51,63,62,-57,-122,-36,-2,-8,-71,-96,-117,-119,-96,-95,-118,-121,-94,4,45,99,-106,22,2,14,-119,2,39,-96,-2,-119,117,100,102,115,-2,-21,116,-86,-59,108,126,-124,112,100,99,113,1,48,-2,9,-73,-40,-41,-50,-74,-39,-42,-42,-93,-118,26,125,-106,0,1,0,-96,0,0,2,-126,6,21,0,12,0,51,-78,3,13,14,17,18,57,0,
		-80,0,69,88,-80,4,47,27,-79,4,32,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,4,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,54,54,51,50,23,7,38,35,34,21,17,-96,1,-80,-94,59,84,23,40,51,-73,4,-82,-87,-66,21,-114,11,-35,-5,96,0,2,0,93,-1,-20,5,18,5,-60,0,23,0,31,
		0,94,-78,0,32,33,17,18,57,-80,24,-48,0,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,5,16,0,17,18,57,-80,5,47,-80,16,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,27,1,-80,10,43,88,33,
		-40,27,-12,89,48,49,5,32,0,17,53,33,53,16,2,35,34,7,7,39,55,54,51,32,0,17,21,20,2,4,39,50,18,55,33,21,20,22,2,-71,-2,-29,-2,-63,3,-12,-12,-35,-91,-117,61,47,22,-98,-24,1,46,1,100,-100,-2,-22,-89,-87,-34,15,-4,-49,-45,20,1,89,1,69,117,7,1,2,1,28,58,26,-113,13,88,-2,-121,-2,-79,84,
		-59,-2,-65,-74,-98,1,5,-37,34,-38,-28,0,0,1,-1,-28,-2,75,2,-68,6,21,0,30,0,116,-78,20,31,32,17,18,57,0,-80,0,69,88,-80,21,47,27,-79,21,32,62,89,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,29,47,27,-79,29,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,20,62,89,-80,29,16,-79,
		0,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,14,-48,-80,15,-48,-80,21,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,20,6,35,34,39,55,22,51,50,54,53,17,35,53,51,53,54,54,51,50,23,7,38,35,34,7,21,51,2,96,-53,-88,-102,61,50,
		14,30,67,65,71,-85,-85,2,-81,-95,59,84,22,38,60,-85,4,-53,3,-85,-5,-2,-89,-73,18,-109,13,104,92,4,4,-113,120,-89,-68,21,-109,10,-61,122,0,2,0,101,-1,-20,5,-99,6,55,0,23,0,37,0,85,-78,4,38,39,17,18,57,-80,4,16,-80,34,-48,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,4,47,
		27,-79,4,18,62,89,-78,15,13,4,17,18,57,-80,15,16,-80,21,-48,-80,13,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,4,35,34,36,2,39,53,52,18,36,51,50,23,54,54,53,51,16,5,22,23,7,16,2,35,34,2,7,21,20,18,51,50,18,17,4,
		-8,-112,-2,-8,-80,-85,-2,-10,-107,1,-110,1,11,-84,-16,-101,96,93,-89,-2,-7,97,1,-66,-49,-67,-74,-47,3,-45,-71,-65,-53,2,-87,-42,-2,-63,-88,-88,1,62,-49,100,-46,1,65,-84,-101,7,-125,-124,-2,-77,61,-84,-10,4,1,2,1,22,-2,-21,-10,107,-5,-2,-31,1,26,1,1,0,0,2,0,91,-1,-20,4,-70,4,-80,0,22,0,35,0,85,
		-78,19,36,37,17,18,57,-80,19,16,-80,26,-48,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-78,6,4,19,17,18,57,-80,6,16,-80,12,-48,-80,19,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,33,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,54,51,
		50,23,54,54,53,51,16,7,22,21,21,20,6,6,35,34,0,53,23,20,22,51,50,54,53,53,52,38,35,34,6,91,123,-31,-113,-49,-120,71,64,-106,-49,73,124,-32,-112,-34,-2,-15,-71,-89,-115,-117,-89,-87,-117,-118,-88,2,39,-97,-3,-117,-118,8,100,-128,-2,-35,51,-118,-87,22,-98,-2,-119,1,51,-5,9,-76,-38,-37,-71,16,-75,-38,-38,0,0,1,
		0,-116,-1,-20,6,29,6,2,0,26,0,77,-78,12,27,28,17,18,57,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-78,1,13,26,17,18,57,-80,1,16,-80,8,-48,-80,13,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,
		21,54,54,53,51,20,6,7,17,6,2,7,7,34,0,39,17,51,17,20,22,51,50,54,53,17,4,-86,115,97,-97,-79,-62,1,-12,-45,73,-17,-2,-28,2,-66,-82,-95,-93,-83,5,-80,-43,11,-119,-109,-46,-47,12,-3,126,-57,-2,-4,22,4,1,2,-30,3,-32,-4,38,-98,-81,-82,-98,3,-37,0,0,1,0,-120,-1,-20,5,15,4,-112,0,25,0,97,
		-78,7,26,27,17,18,57,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,21,8,19,17,18,57,-80,21,16,-80,3,-48,-78,6,8,19,17,18,57,-80,8,16,-79,16,1,-80,10,43,88,
		33,-40,27,-12,89,48,49,1,20,6,7,17,35,39,6,35,34,38,39,17,51,17,20,51,50,55,17,51,21,62,2,53,5,15,-109,-96,-80,4,108,-47,-83,-75,1,-71,-56,-44,70,-71,68,68,29,4,-112,-76,-109,4,-4,-69,107,127,-55,-59,2,-64,-3,69,-10,-98,3,19,-125,2,35,72,108,0,0,1,-1,-76,-2,75,1,101,4,58,0,13,0,41,
		0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,20,62,89,-79,9,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,20,6,35,34,39,55,22,51,50,54,53,17,1,101,-86,-104,59,52,14,30,67,65,72,4,58,-5,109,-86,-78,18,-109,13,104,92,4,-109,0,0,2,0,98,-1,-20,3,-23,4,79,
		0,20,0,28,0,104,-78,8,29,30,17,18,57,-80,8,16,-80,21,-48,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,13,0,8,17,18,57,-80,13,47,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-78,18,0,8,17,18,57,-80,8,16,-79,21,1,-80,10,43,88,33,-40,
		27,-12,89,-80,13,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,0,21,21,20,6,6,39,34,38,53,53,33,38,38,35,34,7,39,54,1,50,54,55,33,21,20,22,1,-1,-36,1,14,124,-40,122,-48,-23,2,-51,7,-95,-120,-70,123,73,-116,1,14,98,-105,21,-3,-13,-119,4,79,-2,-44,-7,36,-107,-8,-115,1,-2,-23,116,-88,
		-56,108,125,-122,-4,53,-92,-119,26,125,-106,0,0,1,0,-87,4,-28,3,6,6,0,0,8,0,52,0,-80,4,47,-80,7,-48,-80,7,47,-76,15,7,31,7,2,93,-78,5,4,7,17,18,57,25,-80,5,47,24,-80,1,-48,25,-80,1,47,24,-80,4,16,-80,2,-48,-78,3,4,7,17,18,57,48,49,1,21,35,39,7,35,53,19,51,3,6,-103,
		-106,-107,-103,-10,112,4,-18,10,-86,-86,12,1,16,0,0,1,0,-115,4,-29,2,-9,5,-1,0,8,0,32,0,-80,4,47,-80,1,-48,-80,1,47,-76,15,1,31,1,2,93,-78,0,4,1,17,18,57,-80,8,-48,-80,8,47,48,49,1,55,51,21,3,35,3,53,51,1,-63,-106,-96,-2,113,-5,-99,5,85,-86,10,-2,-18,1,18,10,-1,-1,0,-114,
		5,22,3,46,5,-91,1,6,0,112,0,0,0,10,0,-80,1,47,-79,2,3,-12,48,49,0,1,0,-127,4,-53,2,-40,5,-41,0,12,0,39,-78,9,13,14,17,18,57,0,-80,3,47,-78,15,3,1,93,-79,9,4,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,6,47,-80,12,-48,48,49,1,20,6,32,38,53,51,20,22,51,50,54,53,
		2,-40,-91,-2,-12,-90,-105,76,73,70,79,5,-41,121,-109,-108,120,70,79,78,71,0,0,1,0,-115,4,-18,1,104,5,-62,0,8,0,25,-78,2,9,10,17,18,57,0,-80,7,47,-79,2,5,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,50,22,20,6,34,38,-115,55,108,56,56,108,55,5,87,45,62,62,90,60,60,0,0,2,0,121,
		4,-76,2,39,6,80,0,9,0,20,0,42,-78,3,21,22,17,18,57,-80,3,16,-80,13,-48,0,-80,3,47,-80,7,-48,-80,7,47,-78,63,7,1,93,-80,3,16,-80,13,-48,-80,7,16,-80,18,-48,48,49,1,20,6,35,34,38,52,54,50,22,5,20,22,51,50,54,52,38,35,34,6,2,39,124,91,92,123,123,-72,123,-2,-75,67,49,48,68,
		67,49,50,66,5,-128,87,117,118,-84,122,122,86,47,68,66,98,69,70,0,0,1,0,50,-2,79,1,-110,0,56,0,16,0,50,-78,5,17,18,17,18,57,0,-80,16,47,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-79,5,3,-80,10,43,88,33,-40,27,-12,89,64,9,15,16,31,16,47,16,63,16,4,93,48,49,33,7,6,21,20,51,
		50,55,23,6,35,34,38,53,52,54,55,1,126,58,113,78,48,52,13,70,90,89,103,-122,123,45,91,86,72,26,121,44,104,86,89,-102,56,0,0,1,0,123,4,-39,3,62,5,-24,0,23,0,64,0,-80,3,47,-80,8,-48,-80,8,47,-76,15,8,31,8,2,93,-80,3,16,-80,11,-48,-80,11,47,-80,8,16,-79,15,3,-80,10,43,88,33,-40,
		27,-12,89,-80,3,16,-79,20,3,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-80,23,-48,48,49,1,20,6,35,34,46,2,35,34,6,21,39,52,54,51,50,30,2,51,50,54,53,3,62,123,92,41,60,97,43,28,41,58,124,121,93,35,56,96,51,31,43,57,5,-36,108,-122,20,62,13,63,49,7,107,-116,20,58,18,68,45,0,2,0,94,
		4,-48,3,44,5,-1,0,3,0,7,0,59,0,-80,2,47,-80,0,-48,-80,0,47,-76,15,0,31,0,2,93,-80,2,16,-80,3,-48,25,-80,3,47,24,-80,0,16,-80,5,-48,-80,5,47,-80,2,16,-80,6,-48,-80,6,47,-80,3,16,-80,7,-48,25,-80,7,47,24,48,49,1,51,1,35,3,51,3,35,2,93,-49,-2,-13,-87,109,-59,-38,-106,5,
		-1,-2,-47,1,47,-2,-47,0,0,2,0,126,-2,107,1,-43,-1,-75,0,11,0,22,0,52,0,-80,3,47,64,11,0,3,16,3,32,3,48,3,64,3,5,93,-80,9,-48,-80,9,47,64,9,48,9,64,9,80,9,96,9,4,93,-78,0,9,1,93,-80,14,-48,-80,3,16,-80,20,-48,48,49,23,52,54,51,50,22,21,20,6,35,34,38,55,20,
		22,50,54,53,52,38,35,34,6,126,100,74,71,98,96,73,76,98,87,52,70,48,48,35,37,50,-14,70,97,96,71,70,93,94,69,35,48,48,35,36,50,52,0,1,-4,-89,4,-38,-2,72,6,0,0,3,0,30,0,-80,1,47,-80,0,-48,25,-80,0,47,24,-80,1,16,-80,2,-48,-80,2,47,-76,15,2,31,2,2,93,48,49,1,35,1,51,
		-2,72,-97,-2,-2,-32,4,-38,1,38,0,1,-3,111,4,-38,-1,16,6,0,0,3,0,30,0,-80,2,47,-80,1,-48,-80,1,47,-76,15,1,31,1,2,93,-80,2,16,-80,3,-48,25,-80,3,47,24,48,49,1,51,1,35,-2,48,-32,-2,-12,-107,6,0,-2,-38,-1,-1,-4,-117,4,-39,-1,78,5,-24,0,7,0,-91,-4,16,0,0,0,1,-3,94,
		4,-39,-2,-108,6,116,0,14,0,46,0,-80,0,47,-78,15,0,1,93,-80,7,-48,-80,7,47,64,9,15,7,31,7,47,7,63,7,4,93,-80,6,-48,-78,1,0,6,17,18,57,-78,13,0,7,17,18,57,48,49,1,39,54,54,52,38,35,55,50,22,21,20,6,7,7,-3,116,1,75,70,91,75,7,-107,-102,78,77,1,4,-39,-103,5,30,78,
		39,106,103,85,61,80,11,71,0,2,-4,39,4,-28,-1,7,5,-18,0,3,0,7,0,55,0,-80,1,47,-80,0,-48,25,-80,0,47,24,-80,1,16,-80,5,-48,-80,5,47,-80,6,-48,-80,6,47,-74,15,6,31,6,47,6,3,93,-80,3,-48,-80,3,47,-80,0,16,-80,4,-48,25,-80,4,47,24,48,49,1,35,1,51,1,35,3,51,-2,2,-87,
		-2,-50,-31,1,-1,-106,-10,-50,4,-28,1,10,-2,-10,1,10,0,1,-3,56,-2,-94,-2,19,-1,118,0,8,0,18,0,-80,2,47,-79,7,5,-80,10,43,88,33,-40,27,-12,89,48,49,5,52,54,50,22,20,6,34,38,-3,56,55,108,56,56,108,55,-11,45,62,62,90,60,60,0,1,0,-73,4,-18,1,-101,6,63,0,3,0,29,0,-80,2,47,
		-80,0,-48,-80,0,47,-78,15,0,1,93,-78,3,2,0,17,18,57,25,-80,3,47,24,48,49,19,51,3,35,-19,-82,116,112,6,63,-2,-81,0,0,3,0,113,4,-16,3,-125,6,-120,0,3,0,12,0,21,0,56,0,-80,11,47,-80,2,-48,-80,2,47,-80,1,-48,-80,1,47,-80,2,16,-80,3,-48,25,-80,3,47,24,-80,11,16,-79,6,5,-80,
		10,43,88,33,-40,27,-12,89,-80,15,-48,-80,11,16,-80,20,-48,-80,20,47,48,49,1,51,3,35,5,52,54,50,22,20,6,34,38,37,52,54,50,22,20,6,34,38,1,-31,-68,101,-121,-2,-64,55,108,56,56,108,55,2,55,55,108,56,56,108,55,6,-120,-2,-8,37,45,61,61,90,60,60,43,45,62,62,90,60,60,0,-1,-1,0,-109,2,107,
		1,121,3,73,1,6,0,120,0,0,0,6,0,-80,2,47,48,49,0,1,0,-79,0,0,4,48,5,-80,0,5,0,44,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,4,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,4,48,-3,66,-63,3,127,5,
		18,-6,-18,5,-80,0,0,2,0,31,0,0,5,115,5,-80,0,3,0,6,0,48,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,6,2,0,17,18,57,48,49,1,51,1,33,37,33,1,2,-122,-86,2,67,-6,-84,1,6,3,76,-2,103,
		5,-80,-6,80,-99,4,40,0,0,3,0,103,-1,-20,4,-6,5,-60,0,3,0,21,0,35,0,122,-78,8,36,37,17,18,57,-80,8,16,-80,1,-48,-80,8,16,-80,32,-48,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,2,8,17,17,18,57,-80,2,47,-78,-49,2,1,93,-78,-1,2,
		1,93,-78,47,2,1,93,-76,-65,2,-49,2,2,113,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,5,20,2,4,35,34,36,2,39,53,52,18,36,51,50,4,18,23,7,16,2,35,34,2,7,21,20,18,
		51,50,18,55,3,-64,-3,-5,2,5,1,58,-113,-2,-8,-79,-84,-2,-10,-109,2,-110,1,11,-84,-81,1,8,-111,2,-65,-48,-69,-74,-47,3,-47,-69,-70,-52,3,2,-109,-104,-126,-43,-2,-62,-86,-87,1,57,-50,105,-46,1,66,-85,-88,-2,-59,-49,11,1,3,1,21,-2,-21,-10,107,-6,-2,-32,1,15,-3,0,0,1,0,50,0,0,5,3,5,-80,0,6,
		0,49,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,0,3,1,17,18,57,48,49,1,1,35,1,51,1,35,2,-102,-2,102,-50,2,18,-84,2,19,-49,4,-119,-5,119,5,-80,-6,80,0,0,3,0,120,0,0,4,33,5,-80,0,3,
		0,7,0,11,0,82,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-80,5,-48,-80,5,47,-78,47,5,1,93,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,55,33,
		21,33,19,33,21,33,3,33,21,33,120,3,-87,-4,87,87,2,-14,-3,14,83,3,-108,-4,108,-99,-99,3,63,-99,3,14,-98,0,0,1,0,-78,0,0,5,1,5,-80,0,7,0,57,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,6,
		16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,33,17,35,17,33,5,1,-63,-3,50,-64,4,79,5,18,-6,-18,5,-80,0,0,1,0,69,0,0,4,68,5,-80,0,12,0,62,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,
		89,-80,5,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,7,-48,48,49,1,1,33,21,33,53,1,1,53,33,21,33,1,2,-14,-2,67,3,15,-4,1,1,-31,-2,31,3,-50,-3,36,1,-69,2,-50,-3,-49,-99,-113,2,74,2,71,-112,-98,-3,-44,0,0,3,0,77,0,0,5,116,5,-80,0,21,0,28,0,35,0,110,-78,10,
		36,37,17,18,57,-80,10,16,-80,25,-48,-80,10,16,-80,32,-48,0,-80,0,69,88,-80,20,47,27,-79,20,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,19,20,9,17,18,57,-80,19,47,-80,0,-48,-78,8,9,20,17,18,57,-80,8,47,-80,11,-48,-80,8,16,-79,33,1,-80,10,43,88,33,-40,27,-12,89,-80,25,-48,-80,19,
		16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-80,32,-48,48,49,1,22,4,22,21,20,6,6,7,21,35,53,38,0,53,52,54,55,54,55,53,51,1,20,22,23,17,6,6,5,52,38,39,17,54,54,3,66,-95,1,1,-112,-113,-1,-92,-62,-5,-2,-56,125,116,-117,-73,-62,-3,-54,-62,-78,-76,-64,3,-87,-63,-78,-76,-65,4,-9,3,-118,-6,-100,
		-98,-6,-119,4,-81,-81,4,1,47,-16,-108,-18,73,87,3,-71,-3,34,-72,-56,4,3,9,4,-54,-75,-75,-54,4,-4,-9,4,-53,0,0,1,0,90,0,0,5,33,5,-80,0,24,0,93,-78,0,25,26,17,18,57,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,23,47,27,
		-79,23,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,22,4,11,17,18,57,-80,22,47,-80,0,-48,-80,22,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,10,-48,48,49,1,54,54,53,17,51,17,20,6,6,7,17,35,17,38,0,39,17,51,17,22,22,23,17,51,3,22,-100,-82,-63,127,-19,-97,-63,-25,-2,-17,3,-64,
		1,-91,-107,-63,2,11,23,-41,-86,2,13,-3,-16,-97,-11,-109,15,-2,-106,1,106,23,1,42,-19,2,24,-3,-17,-93,-41,25,3,-92,0,1,0,113,0,0,4,-53,5,-60,0,36,0,94,-78,25,37,38,17,18,57,0,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-80,0,69,88,-80,35,47,27,
		-79,35,18,62,89,-80,14,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,13,-48,-80,0,-48,-80,25,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,33,-48,-80,34,-48,48,49,37,54,18,55,53,52,38,32,6,21,21,20,18,23,21,33,53,51,38,2,53,53,52,18,54,51,50,22,18,23,21,20,2,7,51,21,33,2,
		-31,-118,-102,3,-62,-2,-82,-64,-99,-111,-2,20,-35,106,120,-115,-2,-95,-96,-3,-114,3,120,106,-36,-2,28,-94,27,1,28,-22,-122,-25,-10,-6,-27,113,-16,-2,-40,28,-94,-99,102,1,51,-94,111,-70,1,36,-97,-100,-2,-28,-76,-126,-96,-2,-51,102,-99,0,0,2,0,100,-1,-21,4,119,4,78,0,22,0,33,0,127,-78,31,34,35,17,18,57,-80,31,16,
		-80,19,-48,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,22,47,27,-79,22,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,8,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,10,19,8,17,18,57,-78,21,19,8,17,18,57,-80,12,16,-79,26,
		1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,22,51,50,55,23,6,35,34,39,6,35,34,2,53,53,16,18,51,50,23,55,1,20,22,51,50,55,17,38,35,34,6,3,-18,2,78,19,15,23,48,74,-109,38,107,-47,-64,-28,-30,-60,-53,107,17,-3,-52,-110,-121,-83,82,85,-88,-122,
		-107,4,58,-4,-29,-116,5,-119,34,-91,-91,1,27,-12,15,1,8,1,61,-95,-115,-3,-70,-81,-61,-70,1,-66,-68,-29,0,2,0,-96,-2,-128,4,77,5,-60,0,20,0,42,0,108,-78,0,43,44,17,18,57,-80,24,-48,0,-80,15,47,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,40,0,12,
		17,18,57,-80,40,47,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,6,37,40,17,18,57,-78,14,12,0,17,18,57,-80,0,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,22,21,20,6,7,22,22,21,20,6,35,34,39,17,35,17,52,54,54,1,52,38,35,34,
		6,7,17,22,22,51,50,54,53,52,38,39,35,53,51,50,54,2,93,-63,-21,98,88,123,-125,-7,-51,-75,120,-70,122,-49,1,103,-120,107,108,-106,1,44,-112,94,-122,-102,-116,109,-106,85,120,126,5,-60,-37,-82,91,-104,46,45,-61,-126,-51,-17,95,-2,53,5,-79,108,-68,107,-2,123,102,-121,-114,107,-4,-61,52,63,-96,-127,118,-91,3,-104,119,0,0,1,
		0,46,-2,96,3,-33,4,58,0,8,0,56,-78,0,9,10,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,20,62,89,-78,0,7,4,17,18,57,48,49,1,1,51,1,17,35,17,1,51,2,10,1,24,-67,-2,-123,-70,-2,-124,-67,1,20,
		3,38,-5,-1,-2,39,1,-32,3,-6,0,2,0,96,-1,-20,4,39,6,28,0,30,0,42,0,97,-78,20,43,44,17,18,57,-80,20,16,-80,34,-48,0,-80,0,69,88,-80,3,47,27,-79,3,32,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,3,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-78,27,20,3,17,18,57,-80,27,
		47,-79,40,11,-80,10,43,88,33,-40,27,-12,89,-80,12,-48,-80,20,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,51,50,23,7,38,35,34,6,21,20,4,18,23,21,20,6,6,35,34,0,53,53,52,18,55,39,38,38,19,20,22,51,50,54,53,52,38,39,34,6,-35,-53,-81,-117,-122,2,-105,124,86,101,1,-69,-49,5,
		118,-37,-111,-34,-2,-7,-68,-112,1,99,107,62,-95,-119,-120,-96,-87,125,-120,-92,4,-11,-120,-97,55,-96,59,72,62,108,-103,-2,-13,-60,39,-103,-13,-123,1,39,-14,13,-91,1,8,35,5,39,-116,-3,99,-80,-53,-54,-58,-120,-37,25,-51,0,0,1,0,99,-1,-20,3,-20,4,77,0,37,0,114,-78,3,38,39,17,18,57,0,-80,0,69,88,-80,21,47,27,
		-79,21,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-80,6,-48,-80,10,16,-80,34,-48,-80,34,47,-78,47,34,1,93,-78,-65,34,1,93,-79,35,1,-80,10,43,88,33,-40,27,-12,89,-78,15,35,34,17,18,57,-78,25,21,34,17,18,57,-80,21,16,-79,28,1,-80,10,43,
		88,33,-40,27,-12,89,48,49,1,20,22,51,50,54,53,51,20,6,35,34,38,53,52,55,38,38,53,52,54,51,50,22,21,35,52,38,35,34,6,21,20,51,51,21,35,6,1,30,-109,118,113,-101,-71,-1,-58,-52,-8,-51,88,98,-25,-54,-70,-7,-71,-113,107,112,-121,-12,-60,-32,-22,1,48,77,98,110,81,-101,-71,-79,-109,-70,66,36,122,73,-108,-90,
		-77,-114,70,101,91,74,-96,-108,6,0,0,1,0,109,-2,-127,3,-61,5,-80,0,31,0,77,-78,8,32,33,17,18,57,0,-80,15,47,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-79,29,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-78,21,32,0,17,18,57,-78,2,21,0,17,18,57,-80,21,16,-79,7,1,-80,10,43,88,33,-40,27,
		-12,89,-78,28,0,21,17,18,57,48,49,1,21,1,6,6,21,20,22,23,23,22,22,21,6,6,7,39,54,54,53,52,36,39,38,38,53,52,18,55,1,33,53,3,-61,-2,-94,-118,102,67,82,-9,81,71,2,108,67,98,47,51,-2,-52,54,103,91,-110,127,1,29,-3,-125,5,-80,120,-2,85,-95,-27,-123,90,97,25,72,24,88,78,69,-84,54,84,
		53,85,45,68,78,24,45,-103,-127,-126,1,64,-106,1,67,-104,0,1,0,-111,-2,97,3,-16,4,78,0,18,0,84,-78,12,19,20,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,20,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,
		-78,1,16,3,17,18,57,-80,3,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,23,54,51,50,22,23,17,35,17,52,38,35,34,6,7,17,35,17,1,56,11,120,-56,-66,-82,1,-71,108,-128,92,-126,34,-70,4,58,-120,-100,-59,-52,-5,-92,4,81,-120,124,87,78,-4,-17,4,58,0,3,0,122,-1,-20,4,18,5,-60,0,13,0,22,
		0,30,0,-107,-78,3,31,32,17,18,57,-80,3,16,-80,19,-48,-80,3,16,-80,27,-48,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,14,3,10,17,18,57,-80,14,47,-78,95,14,1,93,-78,-1,14,1,93,-76,-113,14,-97,14,2,113,-76,-65,14,-49,14,2,113,-78,47,14,1,113,-78,
		-49,14,1,93,-78,47,14,1,93,-76,-17,14,-1,14,2,113,-80,10,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,2,35,34,2,3,53,16,18,51,50,18,19,5,33,53,52,38,35,34,6,21,5,33,21,20,
		22,32,54,55,4,18,-20,-33,-37,-18,4,-20,-33,-34,-21,4,-3,33,2,37,-117,-120,-122,-116,2,37,-3,-37,-110,1,4,-115,2,2,-128,-2,-65,-2,-83,1,76,1,52,-51,1,61,1,78,-2,-68,-2,-51,44,55,-29,-15,-15,-29,-49,39,-27,-6,-16,-29,0,1,0,-61,-1,-12,2,75,4,58,0,12,0,41,0,-80,0,69,88,-80,0,47,27,-79,0,26,
		62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-79,4,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,20,22,51,50,55,23,6,35,34,17,17,1,124,55,64,48,39,1,70,73,-7,4,58,-4,-41,63,64,12,-105,19,1,38,3,32,0,0,1,0,37,-1,-17,4,59,5,-18,0,26,0,82,-78,16,27,28,17,18,57,0,-80,0,
		47,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,11,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-78,16,0,11,17,18,57,-80,16,16,-80,19,-48,-80,0,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,22,23,1,22,22,51,55,23,6,35,34,38,38,39,3,1,
		35,1,39,38,38,35,7,39,54,1,5,98,120,33,1,-85,20,45,35,38,6,36,42,77,78,62,29,-26,-2,-30,-50,1,-118,96,23,53,45,47,1,42,5,-18,80,95,-5,-85,51,39,3,-104,12,37,86,80,2,81,-4,-11,4,5,-21,56,46,2,-114,12,0,1,0,101,-2,119,3,-87,5,-60,0,45,0,89,-78,3,46,47,17,18,57,0,-80,23,
		47,-80,0,69,88,-80,43,47,27,-79,43,30,62,89,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,8,46,43,17,18,57,-80,8,47,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,30,46,43,17,18,57,-80,30,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,37,9,8,17,18,57,48,49,1,38,35,34,6,21,20,33,51,21,35,
		6,6,21,20,22,4,22,23,22,21,20,6,7,39,55,54,53,52,46,4,53,52,54,55,38,38,53,52,36,51,50,23,3,114,-124,97,-115,-96,1,77,-123,-106,-74,-57,-112,1,15,124,32,79,104,72,107,57,49,76,-26,-87,119,65,-92,-106,118,-125,1,2,-28,-111,112,5,8,36,103,85,-37,-104,2,-100,-93,112,-99,65,37,20,49,105,64,-89,61,84,
		64,60,62,39,46,51,66,105,-103,111,-111,-53,46,42,-104,96,-97,-71,39,0,0,1,0,41,-1,-12,4,-92,4,58,0,20,0,94,-78,11,21,22,17,18,57,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-80,19,16,-79,0,1,-80,10,43,
		88,33,-40,27,-12,89,-80,10,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,13,-48,-80,14,-48,-80,17,-48,-80,18,-48,48,49,1,35,17,20,22,51,50,55,23,6,35,34,17,17,33,17,35,17,35,53,33,4,113,-100,54,65,48,39,1,70,73,-7,-2,111,-71,-87,4,72,3,-95,-3,114,64,65,12,-105,19,1,38,2,-121,-4,
		95,3,-95,-103,0,2,0,-111,-2,96,4,31,4,78,0,15,0,27,0,89,-78,18,28,29,17,18,57,-80,18,16,-80,0,-48,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,9,0,7,17,18,57,-79,18,1,-80,10,43,88,33,-40,27,
		-12,89,-80,0,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,18,23,23,20,2,35,34,39,17,35,17,52,54,54,3,22,51,50,54,53,52,38,35,34,6,21,2,80,-49,-12,11,1,-32,-65,-61,114,-70,113,-51,-124,83,-85,-121,-106,-111,-123,117,-112,4,78,-2,-26,-2,66,-16,-2,-24,124,-3,-8,3,-28,-98,-20,-128,-4,-56,-109,-61,
		-61,-51,-32,-40,-87,0,0,1,0,101,-2,-118,3,-31,4,78,0,34,0,75,-78,0,35,36,17,18,57,0,-80,20,47,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,27,47,27,-79,27,18,62,89,-80,0,16,-80,4,-48,-80,0,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,27,16,-79,13,1,-80,10,43,88,33,-40,
		27,-12,89,48,49,1,50,22,21,35,52,38,35,34,6,21,21,16,5,23,22,22,21,6,6,7,39,55,54,53,52,38,39,38,2,53,53,52,54,54,2,61,-67,-25,-81,-122,111,-124,-101,1,64,-122,98,80,2,99,74,98,47,49,70,86,-20,-8,119,-41,4,78,-43,-76,110,-125,-37,-77,32,-2,-4,99,38,29,96,80,63,-89,62,85,54,60,70,43,
		43,19,52,1,1,-45,42,-104,-5,-119,0,2,0,96,-1,-20,4,123,4,58,0,17,0,29,0,78,-78,8,30,31,17,18,57,-80,8,16,-80,21,-48,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,16,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,21,1,-80,10,43,
		88,33,-40,27,-12,89,-80,0,16,-80,27,-48,48,49,1,33,22,17,21,20,6,6,35,34,0,53,53,52,54,54,55,33,1,20,22,51,50,54,53,52,38,35,34,6,4,123,-2,-28,-56,122,-35,-116,-38,-2,-10,118,-39,-116,2,64,-4,-97,-96,-118,-117,-97,-95,-117,-119,-97,3,-95,-108,-2,-17,17,-116,-21,-120,1,47,-1,13,-104,-14,-120,1,-3,-41,-73,
		-41,-39,-53,-84,-50,-52,0,1,0,81,-1,-20,3,-39,4,58,0,16,0,75,-78,10,17,18,17,18,57,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,15,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,13,-48,
		-80,14,-48,48,49,1,33,17,20,51,50,55,23,6,35,34,38,39,17,33,53,33,3,-39,-2,-115,105,43,49,42,76,106,125,117,1,-2,-91,3,-120,3,-92,-3,105,-123,26,-126,52,-109,-110,2,-109,-106,0,1,0,-113,-1,-20,3,-10,4,58,0,18,0,61,-78,14,19,20,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,
		88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-79,3,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,16,51,50,54,53,38,3,51,22,17,16,0,35,34,38,39,17,1,73,-55,-127,-86,5,118,-61,113,-2,-1,-38,-62,-56,2,4,58,-3,121,-2,-49,-6,-74,-25,1,33,-15,-2,-23,-2,-7,-2,-63,-32,
		-41,2,-105,0,0,2,0,87,-2,34,5,76,4,58,0,25,0,34,0,94,-78,15,35,36,17,18,57,-80,15,16,-80,26,-48,0,-80,24,47,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-80,0,-48,-80,23,16,-79,26,1,-80,10,43,88,33,
		-40,27,-12,89,-80,12,-48,-80,16,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,36,0,53,52,18,55,23,6,7,20,22,23,17,52,54,51,50,22,22,21,20,0,5,17,35,19,54,54,53,38,38,35,34,21,2,108,-1,0,-2,-21,-127,127,101,-95,10,-75,-90,-118,113,-126,-31,-126,-2,-34,-2,-5,-71,-71,-86,-60,5,-91,-126,66,17,
		23,1,51,-5,-88,1,7,87,-123,-116,-11,-83,-27,26,2,-52,105,125,-115,-8,-107,-13,-2,-41,21,-2,51,2,102,22,-34,-92,-87,-40,82,0,0,1,0,95,-2,40,5,67,4,58,0,25,0,89,-78,0,26,27,17,18,57,0,-80,13,47,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,
		88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-80,15,-48,-80,1,16,-80,24,-48,48,49,1,17,54,54,53,38,3,51,22,17,16,0,5,17,35,17,38,0,17,17,51,17,22,22,23,17,3,28,-85,-61,5,122,-62,118,-2,-29,-2,-10,-71,-1,-2,
		-5,-70,2,-90,-94,4,58,-4,78,24,-27,-78,-24,1,27,-20,-2,-23,-2,-3,-2,-48,21,-2,57,1,-55,26,1,54,1,19,1,-26,-2,14,-62,-28,25,3,-79,0,0,1,0,122,-1,-20,6,25,4,58,0,35,0,91,-78,27,36,37,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,
		-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-80,0,69,88,-80,30,47,27,-79,30,18,62,89,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-78,9,0,30,17,18,57,-80,14,-48,-78,27,19,25,17,18,57,48,49,1,2,7,20,22,51,50,54,53,17,51,17,22,22,51,50,54,53,38,3,51,22,17,16,2,35,34,39,6,6,35,34,2,
		17,16,55,1,-60,-118,7,114,106,108,113,-69,1,113,107,106,114,7,-118,-61,-121,-49,-68,-16,85,41,-92,119,-68,-49,-121,4,58,-2,-27,-17,-53,-29,-83,-90,1,45,-2,-50,-92,-86,-30,-52,-17,1,27,-12,-2,-22,-2,-19,-2,-49,-18,117,121,1,49,1,19,1,31,-21,0,2,0,121,-1,-20,4,121,5,-58,0,31,0,40,0,113,-78,20,41,42,17,18,
		57,-80,20,16,-80,38,-48,0,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,29,25,6,17,18,57,-80,29,47,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,11,25,6,17,18,57,-80,6,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-80,19,-48,-80,29,16,-80,35,-48,
		-80,25,16,-79,38,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,6,7,21,6,6,35,34,38,53,17,55,17,20,22,51,50,54,53,53,38,0,53,52,54,51,50,22,21,17,54,55,1,20,22,23,17,38,35,34,21,4,121,60,83,2,-27,-56,-53,-9,-70,-116,124,116,-126,-39,-2,-13,-72,-106,-97,-78,63,72,-3,-108,-94,-118,5,-109,-108,2,115,
		23,9,-90,-45,-18,-9,-41,1,71,2,-2,-80,-113,-101,-110,-104,-90,31,1,26,-39,-96,-69,-59,-78,-2,-95,5,19,1,82,-123,-67,30,1,104,-58,-60,0,1,-1,-38,0,0,4,110,5,-68,0,26,0,74,-78,0,27,28,17,18,57,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,23,47,27,-79,23,30,62,89,-80,0,69,88,
		-80,13,47,27,-79,13,18,62,89,-78,0,4,13,17,18,57,-80,4,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,18,-48,48,49,1,19,54,54,51,50,23,7,38,35,34,7,1,17,35,17,1,38,35,34,7,39,54,51,50,22,23,2,36,-31,43,107,87,72,52,36,13,39,70,36,-2,-41,-65,-2,-40,39,67,39,13,36,52,71,88,107,
		42,3,6,1,-5,99,88,27,-105,8,79,-3,119,-3,-58,2,60,2,-121,79,8,-106,28,84,93,0,0,2,0,74,-1,-20,6,27,4,58,0,18,0,38,0,114,-78,8,39,40,17,18,57,-80,8,16,-80,30,-48,0,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,10,47,27,
		-79,10,18,62,89,-80,17,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,8,17,6,17,18,57,-80,15,-48,-80,16,-48,-80,21,-48,-80,22,-48,-80,10,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,31,10,17,17,18,57,-80,36,-48,48,49,1,35,22,21,16,2,35,34,39,6,35,34,2,17,52,55,35,53,33,1,38,39,33,6,
		7,20,22,51,50,54,55,17,51,17,22,22,51,50,54,6,27,-120,64,-68,-85,-15,83,83,-16,-86,-67,64,116,5,-47,-2,-2,4,74,-4,-69,75,4,96,88,105,113,2,-69,2,113,106,86,96,3,-95,-84,-59,-2,-17,-2,-51,-17,-17,1,48,1,20,-65,-78,-103,-3,-10,-86,-57,-56,-87,-53,-29,-89,-94,1,7,-2,-7,-94,-89,-30,0,1,0,42,-1,-11,
		5,-79,5,-80,0,24,0,100,-78,17,25,26,17,18,57,0,-80,0,69,88,-80,23,47,27,-79,23,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,23,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,23,9,17,18,57,-80,4,47,-80,9,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,16,1,-80,10,
		43,88,33,-40,27,-12,89,-80,0,16,-80,21,-48,-80,22,-48,48,49,1,33,17,54,51,50,4,16,4,35,39,50,54,53,38,38,35,34,7,17,35,17,33,53,33,4,-108,-3,-10,-99,-124,-12,1,18,-2,-4,-19,2,-101,-104,2,-93,-94,-106,-118,-63,-2,97,4,106,5,18,-2,57,48,-15,-2,78,-29,-106,-111,-108,-114,-106,46,-3,90,5,18,-98,0,1,
		0,123,-1,-20,4,-36,5,-60,0,31,0,-119,-78,3,32,33,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,11,16,-80,15,-48,-80,11,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-78,22,3,11,17,18,57,-80,22,47,-76,-65,22,-49,22,2,113,-78,-49,22,1,93,-78,
		-97,22,1,113,-78,-1,22,1,93,-78,47,22,1,93,-78,95,22,1,114,-78,-113,22,1,114,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,31,-48,48,49,1,6,4,35,32,0,17,53,52,18,36,51,50,0,23,35,38,38,35,34,2,7,33,21,33,21,20,18,51,50,54,
		55,4,-36,27,-2,-31,-18,-2,-2,-2,-55,-113,1,11,-80,-24,1,24,23,-64,25,-89,-105,-71,-50,2,2,58,-3,-58,-58,-78,-96,-85,28,1,-50,-25,-5,1,114,1,54,-117,-55,1,53,-89,-2,-3,-27,-84,-98,-2,-15,-22,-99,2,-19,-2,-24,-111,-76,0,0,2,0,49,0,0,8,59,5,-80,0,24,0,33,0,119,-78,9,34,35,17,18,57,-80,9,16,
		-80,25,-48,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,1,0,8,17,18,57,-80,1,47,-80,0,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,25,1,-80,
		10,43,88,33,-40,27,-12,89,-80,18,16,-80,26,-48,-80,27,-48,48,49,1,17,33,22,4,21,20,4,7,33,17,33,3,2,2,6,7,35,53,55,62,2,55,19,1,17,33,50,54,53,52,38,39,4,-18,1,105,-34,1,6,-2,-2,-34,-3,-45,-2,0,26,15,89,-84,-112,63,40,93,100,52,11,30,3,119,1,95,-116,-94,-99,-118,5,-80,-3,-53,
		3,-16,-53,-58,-13,4,5,18,-3,-65,-2,-34,-2,-36,-119,2,-99,2,7,107,-22,-13,2,-62,-3,45,-3,-64,-98,-124,-128,-100,2,0,0,2,0,-79,0,0,8,77,5,-80,0,18,0,27,0,-123,-78,1,28,29,17,18,57,-80,1,16,-80,19,-48,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,
		-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,0,2,15,17,18,57,-80,0,47,-78,4,12,2,17,18,57,-80,4,47,-80,0,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,20,1,-80,10,43,88,33,-40,27,-12,
		89,48,49,1,33,17,51,17,33,22,4,21,20,4,7,33,17,33,17,35,17,51,1,17,33,50,54,53,52,38,39,1,114,2,-50,-64,1,106,-30,1,1,-2,-1,-33,-3,-45,-3,50,-63,-63,3,-114,1,95,-114,-96,-104,-118,3,57,2,119,-3,-98,3,-30,-67,-65,-23,4,2,-100,-3,100,5,-80,-3,1,-3,-11,-114,122,116,-116,3,0,0,1,0,62,
		0,0,5,-44,5,-80,0,21,0,95,-78,14,22,23,17,18,57,0,-80,0,69,88,-80,20,47,27,-79,20,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-80,20,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,20,8,17,18,57,-80,4,47,-79,13,1,-80,10,43,88,33,
		-40,27,-12,89,-80,0,16,-80,18,-48,-80,19,-48,48,49,1,33,17,54,51,50,22,23,17,35,17,38,38,35,34,7,17,35,17,33,53,33,4,-90,-3,-16,-96,-81,-6,-14,3,-63,1,-119,-92,-87,-90,-64,-2,104,4,104,5,18,-2,80,40,-38,-35,-2,45,1,-50,-104,-122,42,-3,62,5,18,-98,0,1,0,-80,-2,-103,4,-1,5,-80,0,11,0,73,
		0,-80,9,47,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,48,49,19,51,17,33,17,51,17,33,17,35,17,33,-80,-63,2,-50,-64,
		-2,64,-63,-2,50,5,-80,-6,-19,5,19,-6,80,-2,-103,1,103,0,0,2,0,-94,0,0,4,-79,5,-80,0,12,0,21,0,94,-78,15,22,23,17,18,57,-80,15,16,-80,3,-48,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,11,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,
		2,11,9,17,18,57,-80,2,47,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,22,4,21,20,4,7,33,17,33,1,17,33,50,54,53,52,38,39,4,33,-3,66,1,106,-28,1,0,-2,-2,-33,-3,-46,3,127,-3,66,1,95,-113,-97,-103,-115,5,18,-2,76,3,-28,
		-60,-59,-22,4,5,-80,-3,16,-3,-35,-104,-128,123,-114,2,0,0,2,0,50,-2,-102,5,-55,5,-80,0,14,0,21,0,93,-78,18,22,23,17,18,57,-80,18,16,-80,11,-48,0,-80,4,47,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,4,16,-80,1,-48,-80,2,16,-79,6,1,-80,10,43,
		88,33,-40,27,-12,89,-80,13,-48,-80,14,-48,-80,15,-48,-80,16,-48,-80,11,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,33,17,35,3,51,54,18,55,19,33,17,51,33,33,17,33,3,6,2,5,-57,-65,-5,-21,-64,1,119,94,111,14,32,3,103,-66,-5,-69,2,-58,-2,19,21,13,107,-2,-101,1,101,-2,-102,2,3,106,
		1,101,-43,2,111,-6,-19,4,117,-2,84,-5,-2,-98,0,1,0,27,0,0,7,53,5,-80,0,21,0,-121,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,
		6,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,2,16,-80,16,-48,-80,16,47,-78,47,16,1,93,-78,-49,16,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,8,16,0,17,18,57,-80,16,16,-80,11,-48,-78,19,0,16,17,18,57,48,49,1,35,17,35,17,35,1,35,1,1,51,1,51,17,51,17,51,
		1,51,1,1,35,4,-88,-100,-64,-91,-2,100,-16,1,-22,-2,60,-29,1,-125,-91,-64,-98,1,-125,-30,-2,60,1,-22,-17,2,-104,-3,104,2,-104,-3,104,3,0,2,-80,-3,-120,2,120,-3,-120,2,120,-3,81,-4,-1,0,0,1,0,80,-1,-20,4,106,5,-60,0,40,0,117,-78,3,41,42,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,
		89,-80,0,69,88,-80,22,47,27,-79,22,18,62,89,-80,11,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-80,6,-48,-78,37,22,11,17,18,57,-80,37,47,-78,-49,37,1,93,-78,-97,37,1,113,-79,36,1,-80,10,43,88,33,-40,27,-12,89,-78,17,36,37,17,18,57,-80,22,16,-80,27,-48,-80,22,16,-79,30,1,-80,10,43,88,
		33,-40,27,-12,89,48,49,1,52,38,35,34,6,21,35,52,54,54,51,50,4,21,20,6,7,4,21,20,4,35,34,38,38,53,51,20,22,51,50,54,53,16,37,35,53,51,54,54,3,-108,-87,-103,-128,-83,-64,127,-28,-118,-12,1,14,124,111,1,1,-2,-36,-12,-111,-19,-124,-64,-74,-116,-99,-69,-2,-61,-76,-77,-110,-106,4,41,116,-119,-115,104,116,-72,
		103,-37,-61,101,-90,48,86,-1,-60,-26,103,-66,-125,115,-103,-110,120,1,0,5,-98,3,126,0,0,1,0,-79,0,0,4,-1,5,-80,0,9,0,93,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,
		89,-78,4,0,2,17,18,57,64,9,-118,4,-102,4,-86,4,-70,4,4,93,-78,9,0,2,17,18,57,64,9,-123,9,-107,9,-91,9,-75,9,4,93,48,49,1,51,17,35,17,1,35,17,51,17,4,63,-64,-64,-3,51,-63,-63,5,-80,-6,80,4,98,-5,-98,5,-80,-5,-98,0,0,1,0,47,0,0,4,-10,5,-80,0,17,0,79,-78,4,18,19,
		17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,0,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,3,2,2,6,7,35,53,55,62,2,
		55,19,4,-10,-64,-3,-10,26,15,89,-84,-112,63,40,93,100,52,11,30,5,-80,-6,80,5,18,-3,-65,-2,-34,-2,-36,-119,2,-99,2,7,107,-22,-13,2,-62,0,0,1,0,77,-1,-21,4,-53,5,-80,0,17,0,75,-78,4,18,19,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,
		-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,1,7,17,18,57,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-78,15,7,16,17,18,57,48,49,1,1,51,1,14,2,35,34,39,55,23,50,63,2,1,51,2,-99,1,79,-33,-3,-3,52,90,121,91,79,22,6,91,105,51,25,38,-2,16,-41,2,99,3,77,-5,67,116,97,51,9,-104,
		4,101,52,89,4,54,0,3,0,83,-1,-60,5,-29,5,-20,0,24,0,33,0,42,0,93,-78,12,43,44,17,18,57,-80,12,16,-80,32,-48,-80,12,16,-80,34,-48,0,-80,11,47,-80,23,47,-78,21,23,11,17,18,57,-80,21,47,-80,0,-48,-78,9,11,23,17,18,57,-80,9,47,-80,13,-48,-80,21,16,-79,25,1,-80,10,43,88,33,-40,27,-12,
		89,-80,9,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,-80,31,-48,-80,25,16,-80,34,-48,48,49,1,51,22,4,18,21,20,2,4,7,35,21,35,53,35,34,36,2,16,18,36,51,51,53,51,3,34,6,21,20,22,51,51,17,51,17,51,50,54,53,52,38,35,3,120,31,-91,1,16,-105,-104,-2,-12,-92,35,-70,28,-89,-2,-17,-105,-105,1,
		17,-89,28,-70,-42,-68,-37,-38,-65,26,-70,28,-65,-41,-41,-61,5,30,1,-104,-2,-11,-91,-90,-2,-14,-105,2,-60,-60,-104,1,12,1,78,1,12,-104,-50,-2,-101,-25,-51,-50,-27,3,103,-4,-103,-21,-54,-56,-22,0,0,1,0,-81,-2,-95,5,-105,5,-80,0,11,0,60,0,-80,9,47,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,
		4,47,27,-79,4,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,48,49,19,51,17,33,17,51,17,51,3,35,17,33,-81,-63,2,-50,-64,-103,18,-83,-5,-41,5,-80,-6,-19,5,19,-6,-15,-2,0,1,95,0,1,0,-106,0,0,4,-56,5,-80,0,18,0,71,-78,5,19,20,
		17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-78,15,0,1,17,18,57,-80,15,47,-79,6,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,6,6,35,34,38,39,17,51,17,22,22,51,50,55,17,4,-56,-63,105,
		-84,110,-7,-14,3,-63,1,-119,-93,-66,-59,5,-80,-6,80,2,91,30,23,-40,-33,1,-45,-2,50,-104,-122,54,2,-74,0,1,0,-80,0,0,6,-41,5,-80,0,11,0,73,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,9,
		47,27,-79,9,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,5,-48,-80,6,-48,48,49,1,17,33,17,51,17,33,17,51,17,33,17,1,113,1,-11,-65,1,-14,-64,-7,-39,5,-80,-6,-19,5,19,-6,-19,5,19,-6,80,5,-80,0,0,1,0,-80,-2,-95,7,106,5,-80,0,15,0,85,0,-80,11,47,-80,0,69,88,-80,0,47,27,
		-79,0,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,5,-48,-80,6,-48,-80,9,-48,-80,10,-48,-80,2,-48,48,49,1,17,33,17,51,17,33,17,51,17,51,3,35,17,33,17,1,
		113,1,-11,-65,1,-14,-64,-109,18,-91,-7,-3,5,-80,-6,-19,5,19,-6,-19,5,19,-6,-25,-2,10,1,95,5,-80,0,2,0,16,0,0,5,-72,5,-80,0,12,0,21,0,97,-78,1,22,23,17,18,57,-80,1,16,-80,13,-48,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,2,0,9,
		17,18,57,-80,2,47,-80,0,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,33,17,33,50,4,21,20,4,7,33,17,33,1,17,33,50,54,53,52,38,39,16,2,91,1,90,-17,1,4,-2,-2,-30,-3,-42,-2,102,
		2,91,1,95,-114,-97,-103,-116,5,-80,-3,-82,-27,-58,-59,-21,3,5,24,-3,-88,-3,-35,-104,-128,123,-114,2,0,3,0,-78,0,0,6,48,5,-80,0,10,0,19,0,23,0,111,-78,18,24,25,17,18,57,-80,18,16,-80,6,-48,-80,18,16,-80,21,-48,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,22,47,27,-79,22,30,
		62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-78,0,9,7,17,18,57,-80,0,47,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,22,4,21,20,4,7,33,17,51,17,17,33,50,54,53,52,38,39,1,35,17,
		51,1,114,1,106,-28,1,0,-2,-2,-33,-3,-45,-64,1,95,-113,-97,-103,-115,3,87,-64,-64,3,94,3,-28,-60,-59,-22,4,5,-80,-3,16,-3,-35,-104,-128,123,-114,2,-3,64,5,-80,0,0,2,0,-93,0,0,4,-79,5,-80,0,10,0,19,0,79,-78,13,20,21,17,18,57,-80,13,16,-80,1,-48,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,
		89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,9,7,17,18,57,-80,0,47,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,22,4,21,20,4,7,33,17,51,17,17,33,50,54,53,52,38,39,1,99,1,106,-28,1,0,-2,-2,-33,-3,-45,-64,1,95,-113,-97,
		-103,-115,3,94,3,-28,-60,-59,-22,4,5,-80,-3,16,-3,-35,-104,-128,123,-114,2,0,0,1,0,-109,-1,-20,4,-12,5,-60,0,31,0,-110,-78,12,32,33,17,18,57,0,-80,0,69,88,-80,19,47,27,-79,19,30,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,62,89,-80,0,-48,-80,28,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,8,
		28,19,17,18,57,-80,8,47,-76,-17,8,-1,8,2,113,-78,-49,8,1,93,-78,47,8,1,113,-76,-65,8,-49,8,2,113,-78,-97,8,1,113,-78,-1,8,1,93,-78,47,8,1,93,-78,95,8,1,114,-78,-113,8,1,114,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-80,15,-48,
		48,49,1,22,22,51,50,18,55,33,53,33,52,2,35,34,6,7,35,54,0,51,50,4,18,21,21,20,2,4,35,34,36,39,1,84,28,-85,-96,-83,-55,2,-3,-61,2,61,-49,-70,-106,-89,25,-63,23,1,24,-24,-80,1,11,-113,-114,-2,-3,-88,-18,-2,-31,27,1,-50,-76,-111,1,14,-16,-98,-19,1,20,-100,-82,-27,1,3,-89,-2,-53,-55,-111,-55,
		-2,-52,-91,-5,-25,0,0,2,0,-73,-1,-20,6,-38,5,-60,0,23,0,37,0,-92,-78,33,38,39,17,18,57,-80,33,16,-80,18,-48,0,-80,0,69,88,-80,19,47,27,-79,19,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-78,15,
		10,13,17,18,57,-80,15,47,-78,95,15,1,93,-78,-1,15,1,93,-76,79,15,95,15,2,113,-76,-113,15,-97,15,2,113,-78,47,15,1,113,-78,-49,15,1,93,-78,47,15,1,93,-78,-49,15,1,113,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,34,1,-80,10,43,88,33,
		-40,27,-12,89,48,49,1,20,2,4,35,34,36,2,39,35,17,35,17,51,17,51,54,18,36,51,50,4,18,21,39,16,2,35,34,2,7,21,20,18,51,50,18,55,6,-38,-112,-2,-8,-80,-90,-2,-7,-107,8,-47,-64,-64,-48,3,-112,1,10,-84,-81,1,11,-112,-65,-48,-69,-74,-47,3,-45,-71,-70,-52,3,2,-87,-42,-2,-63,-88,-96,1,42,-57,-3,
		-125,5,-80,-3,100,-50,1,55,-85,-87,-2,-65,-43,2,1,3,1,21,-2,-21,-10,107,-5,-2,-31,1,15,-3,0,2,0,89,0,0,4,100,5,-80,0,12,0,21,0,99,-78,16,22,23,17,18,57,-80,16,16,-80,10,-48,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,3,
		47,27,-79,3,18,62,89,-78,17,10,0,17,18,57,-80,17,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,5,1,10,17,18,57,-80,10,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,17,33,1,35,1,36,17,52,36,51,33,17,1,20,22,23,33,17,33,34,6,3,-93,-2,-80,-2,-45,-51,1,82,-2,-26,1,17,-13,1,
		-49,-4,-19,-91,-109,1,26,-2,-17,-100,-91,2,55,-3,-55,2,108,111,1,30,-48,-25,-6,80,3,-7,-124,-96,1,2,62,-108,0,2,0,97,-1,-20,4,40,6,17,0,27,0,40,0,100,-78,28,41,42,17,18,57,-80,28,16,-80,8,-48,0,-80,0,69,88,-80,18,47,27,-79,18,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,0,
		18,8,17,18,57,-80,0,47,-78,23,0,18,17,18,57,-78,15,18,23,17,18,57,-78,26,0,8,17,18,57,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,18,21,21,20,6,6,35,34,0,53,53,16,18,55,54,54,53,51,20,6,7,7,6,6,7,54,23,34,6,21,
		21,20,22,51,50,54,53,52,38,2,103,-52,-11,118,-35,-112,-38,-2,-10,-3,-9,-116,98,-104,113,124,-118,-91,-91,25,-109,-81,-120,-96,-95,-119,-118,-96,-95,3,-4,-2,-17,-33,17,-103,-15,-123,1,35,-11,90,1,85,1,-110,44,25,72,63,125,-116,29,31,39,-71,-102,-86,-104,-73,-94,16,-82,-53,-52,-60,-103,-71,0,3,0,-99,0,0,4,41,4,58,0,14,
		0,22,0,28,0,-111,-78,24,29,30,17,18,57,-80,24,16,-80,2,-48,-80,24,16,-80,22,-48,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,23,1,0,17,18,57,-80,23,47,-76,-65,23,-49,23,2,93,-76,-97,23,-81,23,2,113,-78,-1,23,1,93,-78,15,23,1,113,-76,47,23,63,
		23,2,93,-76,111,23,127,23,2,114,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,8,15,23,17,18,57,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,22,21,20,6,7,22,22,21,20,6,35,1,17,33,50,54,53,52,35,37,51,32,16,39,35,
		-99,1,-90,-40,-25,90,88,98,119,-37,-56,-2,-48,1,50,116,115,-18,-2,-43,-17,1,4,-10,-3,4,58,-105,-110,75,121,32,23,-122,93,-107,-98,1,-37,-2,-70,86,78,-94,-108,1,48,5,0,1,0,-102,0,0,3,71,4,58,0,5,0,44,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,
		4,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,3,71,-2,13,-70,2,-83,3,-95,-4,95,4,58,0,0,2,0,46,-2,-62,4,-109,4,58,0,14,0,20,0,93,-78,18,21,22,17,18,57,-80,18,16,-80,4,-48,0,-80,12,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,10,47,27,
		-79,10,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,7,-48,-80,12,16,-80,9,-48,-80,7,16,-80,15,-48,-80,16,-48,-80,4,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,48,49,55,55,54,19,19,33,17,51,17,35,17,33,17,35,19,33,33,17,33,3,2,-125,64,108,15,17,2,-71,-117,-71,-3,13,-71,1,1,
		47,1,-15,-2,-77,11,17,-105,79,-116,1,24,1,-80,-4,93,-2,43,1,62,-2,-62,1,-43,2,-8,-2,-2,-2,-67,0,1,0,21,0,0,6,4,4,58,0,21,0,-111,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,2,
		47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,2,16,-80,16,-48,-80,16,47,-78,-65,16,1,93,-78,-1,16,1,93,-78,47,16,1,93,-78,-49,16,1,113,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,8,16,0,17,18,57,-80,16,16,-80,11,-48,
		-78,19,0,16,17,18,57,48,49,1,35,17,35,17,35,1,35,1,1,51,1,51,17,51,17,51,1,51,1,1,35,3,-21,-126,-71,-126,-2,-47,-22,1,-125,-2,-94,-32,1,23,127,-71,126,1,25,-32,-2,-95,1,-125,-22,1,-42,-2,42,1,-42,-2,42,2,48,2,10,-2,64,1,-64,-2,64,1,-64,-3,-11,-3,-47,0,0,1,0,88,-1,-19,3,-84,
		4,77,0,38,0,-119,-78,3,39,40,17,18,57,0,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-80,10,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,37,10,21,17,18,57,-80,37,47,-76,47,37,63,37,2,93,-76,-65,37,-49,37,2,93,-76,-97,37,-81,37,2,113,-76,111,37,127,
		37,2,114,-78,6,37,10,17,18,57,-79,34,1,-80,10,43,88,33,-40,27,-12,89,-78,16,34,37,17,18,57,-78,25,21,10,17,18,57,-80,21,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,35,34,6,21,35,52,54,51,50,22,21,20,6,7,22,21,20,6,35,34,38,53,51,20,22,51,50,54,53,52,38,35,35,53,
		51,54,2,-33,116,101,98,-125,-72,-20,-79,-66,-44,88,81,-67,-26,-64,-69,-13,-72,-115,105,106,-126,109,115,-71,-55,-67,3,18,76,89,102,69,-115,-76,-93,-105,73,122,36,64,-68,-107,-82,-73,-100,79,113,98,78,91,79,-100,5,0,0,1,0,-100,0,0,4,1,4,58,0,9,0,69,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,
		-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,4,7,2,17,18,57,-78,9,7,2,17,18,57,48,49,1,51,17,35,17,1,35,17,51,17,3,72,-71,-71,-2,13,-71,-71,4,58,-5,-58,3,21,-4,-21,4,58,-4,-22,0,0,1,0,-100,0,0,4,63,
		4,58,0,12,0,120,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,2,16,-80,6,-48,-80,6,47,-78,-97,6,1,93,-78,-1,6,1,93,-78,-49,6,1,113,-78,-97,6,1,113,-76,-65,
		6,-49,6,2,93,-78,47,6,1,93,-78,111,6,1,114,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,10,1,6,17,18,57,48,49,1,35,17,35,17,51,17,51,1,51,1,1,35,1,-35,-121,-70,-70,121,1,108,-32,-2,84,1,-48,-21,1,-51,-2,51,4,58,-2,54,1,-54,-3,-8,-3,-50,0,0,1,0,44,0,0,4,3,4,58,0,15,
		0,79,-78,4,16,17,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,3,2,6,7,
		35,53,55,54,54,55,19,4,3,-70,-2,-112,22,18,-105,-92,74,53,90,78,11,20,4,58,-5,-58,3,-95,-2,107,-2,-23,-16,5,-93,4,10,-68,-2,1,-49,0,0,1,0,-99,0,0,5,82,4,58,0,12,0,89,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,3,47,
		27,-79,3,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,0,11,3,17,18,57,-78,5,11,3,17,18,57,-78,8,11,3,17,18,57,48,49,37,1,51,17,35,17,1,35,1,17,35,17,51,2,-5,1,112,-25,-71,-2,-94,-128,-2,-101,-71,-16,-11,3,69,-5,-58,3,19,-4,-19,
		3,36,-4,-36,4,58,0,1,0,-100,0,0,4,0,4,58,0,11,0,-117,0,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,16,-80,9,-48,-80,9,47,-78,111,9,1,93,-76,-65,9,
		-49,9,2,93,-78,63,9,1,113,-76,-49,9,-33,9,2,113,-78,15,9,1,114,-76,-97,9,-81,9,2,113,-78,-1,9,1,93,-78,15,9,1,113,-78,-97,9,1,93,-78,47,9,1,93,-76,111,9,127,9,2,114,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,33,17,35,17,51,17,33,17,51,4,0,-71,-2,15,-70,-70,1,-15,
		-71,1,-50,-2,50,4,58,-2,43,1,-43,0,0,1,0,-100,0,0,4,1,4,58,0,7,0,57,0,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,6,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,33,17,35,17,
		33,4,1,-71,-2,14,-70,3,101,3,-95,-4,95,4,58,0,0,1,0,40,0,0,3,-80,4,58,0,7,0,50,0,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,6,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,48,49,1,33,17,35,17,33,53,33,3,-80,
		-2,-107,-71,-2,-100,3,-120,3,-92,-4,92,3,-92,-106,0,3,0,100,-2,96,5,105,6,0,0,26,0,37,0,48,0,-127,-78,7,49,50,17,18,57,-80,7,16,-80,32,-48,-80,7,16,-80,43,-48,0,-80,6,47,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,19,47,27,-79,
		19,20,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-80,10,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,-80,41,-48,-80,30,16,-80,46,-48,48,49,19,16,18,51,50,23,17,51,17,54,51,50,18,17,20,2,35,34,39,
		17,35,17,6,35,34,2,53,37,52,38,35,34,7,17,22,51,50,54,37,20,22,51,50,55,17,38,35,34,6,100,-46,-73,85,64,-71,70,94,-72,-46,-47,-73,97,69,-71,66,85,-74,-47,4,76,-116,123,63,47,45,67,124,-119,-4,109,-126,122,58,47,42,61,122,-124,2,9,1,15,1,54,29,1,-49,-2,43,35,-2,-54,-2,-36,-17,-2,-26,32,-2,
		85,1,-88,29,1,26,-11,15,-52,-31,20,-4,-15,17,-64,-78,-74,-68,18,3,17,17,-38,0,0,1,0,-100,-2,-65,4,-126,4,58,0,11,0,60,0,-80,8,47,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-79,2,1,-80,10,43,88,33,-40,
		27,-12,89,-80,6,-48,48,49,19,51,17,33,17,51,17,51,3,35,17,33,-100,-70,1,-14,-71,-127,18,-90,-4,-46,4,58,-4,93,3,-93,-4,93,-2,40,1,65,0,1,0,103,0,0,3,-67,4,59,0,16,0,71,-78,4,17,18,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,
		-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,12,15,0,17,18,57,-80,12,47,-79,4,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,6,35,34,38,39,17,51,17,22,51,50,55,17,51,3,-67,-70,122,-128,-53,-43,2,-71,5,-28,-128,122,-70,1,-120,32,-48,-64,1,67,-2,-73,-14,32,2,26,0,0,1,0,-100,0,0,5,-32,
		4,58,0,11,0,73,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,5,-48,-80,6,-48,48,49,1,17,33,17,51,17,33,17,51,17,33,
		17,1,86,1,-116,-71,1,-117,-70,-6,-68,4,58,-4,93,3,-93,-4,93,3,-93,-5,-58,4,58,0,0,1,0,-111,-2,-65,6,109,4,58,0,15,0,76,0,-80,12,47,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,13,47,
		27,-79,13,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,5,-48,-80,9,-48,48,49,1,17,33,17,51,17,33,17,51,17,51,3,35,17,33,17,1,75,1,-116,-71,1,-117,-70,-104,18,-90,-6,-36,4,58,-4,93,3,-93,-4,93,3,-93,-4,93,-2,40,1,65,4,58,0,0,2,0,30,0,0,4,-65,4,58,0,12,0,21,0,97,
		-78,1,22,23,17,18,57,-80,1,16,-80,13,-48,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,2,0,9,17,18,57,-80,2,47,-80,0,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,14,1,-80,10,43,88,
		33,-40,27,-12,89,48,49,19,33,17,33,22,22,21,20,6,35,33,17,33,1,17,33,50,54,53,52,38,39,30,1,-6,1,25,-72,-42,-36,-70,-2,54,-2,-65,1,-6,1,19,104,114,111,100,4,58,-2,-117,2,-68,-95,-94,-60,3,-94,-2,-116,-2,105,107,93,90,115,2,0,3,0,-99,0,0,5,127,4,58,0,10,0,14,0,23,0,111,-78,6,
		24,25,17,18,57,-80,6,16,-80,12,-48,-80,6,16,-80,19,-48,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,13,7,17,18,57,-80,0,47,-79,15,1,-80,10,43,88,33,-40,27,
		-12,89,-80,7,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,22,22,21,20,6,35,33,17,51,1,35,17,51,1,17,33,50,54,53,52,38,39,1,86,1,25,-72,-42,-36,-70,-2,54,-71,4,41,-70,-70,-5,-41,1,19,104,114,111,100,2,-59,2,-68,-95,-94,-60,4,58,-5,-58,4,58,-3,-12,-2,105,107,93,90,115,2,0,2,
		0,-99,0,0,3,-3,4,58,0,10,0,19,0,79,-78,7,20,21,17,18,57,-80,7,16,-80,13,-48,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,9,7,17,18,57,-80,0,47,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,12,1,-80,10,43,88,33,-40,27,-12,
		89,48,49,1,33,22,22,21,20,6,35,33,17,51,17,17,33,50,54,53,52,38,39,1,86,1,25,-72,-42,-36,-70,-2,54,-71,1,19,104,114,111,100,2,-59,2,-68,-95,-94,-60,4,58,-3,-12,-2,105,107,93,90,115,2,0,1,0,100,-1,-20,3,-32,4,78,0,31,0,-123,-78,0,32,33,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,
		26,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-80,8,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,29,8,16,17,18,57,-80,29,47,-76,47,29,63,29,2,93,-76,-65,29,-49,29,2,93,-76,-97,29,-81,29,2,113,-76,111,29,127,29,2,114,-78,3,8,29,17,18,57,-78,20,16,8,17,18,57,-80,16,16,-79,23,1,-80,
		10,43,88,33,-40,27,-12,89,-80,29,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,34,6,21,35,52,54,54,51,50,0,21,21,20,6,6,35,34,38,53,51,20,22,51,50,54,55,33,53,33,38,38,2,8,99,-111,-80,118,-60,106,-45,1,5,119,-41,-118,-76,-16,-80,-114,102,119,-102,12,-2,106,1,-108,14,-106,3,-74,126,86,93,
		-86,101,-2,-49,-10,31,-104,-5,-119,-32,-89,102,-117,-72,-95,-104,-110,-79,0,2,0,-99,-1,-20,6,48,4,78,0,20,0,31,0,-96,-78,13,32,33,17,18,57,-80,13,16,-80,21,-48,0,-80,0,69,88,-80,20,47,27,-79,20,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,0,69,
		88,-80,12,47,27,-79,12,18,62,89,-78,0,17,20,17,18,57,-80,0,47,-76,-65,0,-49,0,2,93,-76,-97,0,-81,0,2,113,-78,-1,0,1,93,-78,15,0,1,113,-76,47,0,63,0,2,93,-74,95,0,111,0,127,0,3,114,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,
		-79,29,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,54,0,51,50,0,23,23,20,6,6,35,34,0,39,33,17,35,17,51,1,20,22,32,54,53,52,38,35,34,6,1,86,1,4,21,1,9,-54,-44,1,14,11,1,124,-32,-112,-47,-2,-10,16,-2,-3,-71,-71,1,-70,-89,1,26,-91,-88,-116,-118,-88,2,111,-40,1,7,-2,-30,-27,58,-98,
		-2,-119,1,17,-38,-2,41,4,58,-3,-41,-76,-38,-34,-58,-79,-34,-38,0,2,0,47,0,0,3,-57,4,58,0,13,0,22,0,99,-78,20,23,24,17,18,57,-80,20,16,-80,13,-48,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,18,0,
		1,17,18,57,-80,18,47,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,7,3,0,17,18,57,-80,0,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,3,35,1,38,38,53,52,54,55,3,20,22,23,33,17,33,34,6,3,-57,-70,-2,-23,-1,-56,1,16,104,111,-34,-70,-34,108,89,1,38,-2,-10,103,122,4,58,
		-5,-58,1,-91,-2,91,1,-63,38,-97,106,-108,-75,1,-2,-76,79,97,1,1,103,101,0,1,-1,-24,-2,75,3,-33,6,0,0,34,0,-121,-78,13,35,36,17,18,57,0,-80,31,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-78,-65,31,1,
		93,-78,47,31,1,93,-78,15,31,1,93,-78,30,25,31,17,18,57,-80,30,47,-80,33,-48,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,2,25,4,17,18,57,-80,10,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,27,-48,48,49,1,33,17,54,51,32,19,17,20,
		6,35,34,39,55,22,50,54,53,17,52,38,35,34,6,7,17,35,17,35,53,51,53,51,21,33,2,99,-2,-30,123,-59,1,87,3,-86,-104,61,54,15,35,-126,72,105,112,90,-120,38,-71,-92,-92,-71,1,30,4,-71,-2,-2,-105,-2,125,-4,-36,-86,-78,18,-109,13,104,92,3,32,120,114,96,78,-4,-3,4,-71,-104,-81,-81,0,0,1,0,103,-1,-20,
		3,-9,4,78,0,31,0,-97,-78,0,32,33,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,3,8,16,17,18,57,-78,27,16,8,17,18,57,-80,27,47,-76,15,27,31,27,2,114,-76,-65,27,-49,27,2,93,-76,-97,27,-81,27,
		2,113,-76,-49,27,-33,27,2,113,-78,-1,27,1,93,-78,15,27,1,113,-76,47,27,63,27,2,93,-76,111,27,127,27,2,114,-78,-65,27,1,114,-78,20,16,27,17,18,57,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,27,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,50,54,55,51,14,2,35,34,0,17,53,52,
		54,54,51,50,22,23,35,38,38,35,34,6,7,33,21,33,22,22,2,72,99,-108,8,-80,5,120,-60,110,-34,-2,-3,117,-40,-108,-74,-15,8,-80,8,-113,104,-126,-102,10,1,-108,-2,108,10,-103,-125,120,90,94,-88,99,1,40,1,0,30,-97,-9,-122,-38,-82,105,-121,-79,-99,-104,-96,-83,0,0,2,0,39,0,0,6,-122,4,58,0,22,0,31,0,125,
		-78,9,32,33,17,18,57,-80,9,16,-80,23,-48,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,1,0,8,17,18,57,-80,1,47,-80,0,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,17,1,-80,10,43,88,33,-40,
		27,-12,89,-80,1,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,33,22,22,21,20,6,7,33,17,33,3,2,6,7,35,53,55,54,54,55,19,1,17,33,50,54,53,52,38,39,3,-33,1,30,-74,-45,-45,-73,-2,41,-2,-81,23,20,-100,-91,65,54,85,77,13,23,2,
		-68,1,19,101,117,114,99,4,58,-2,100,3,-75,-108,-109,-68,3,3,-95,-2,90,-2,-21,-28,2,-93,4,10,-89,-45,2,15,-3,-52,-2,-113,105,86,81,96,1,0,0,2,0,-100,0,0,6,-89,4,58,0,18,0,27,0,126,-78,1,28,29,17,18,57,-80,1,16,-80,19,-48,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,
		17,47,27,-79,17,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,1,17,11,17,18,57,-80,1,47,-80,4,-48,-80,1,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,20,1,-80,10,43,88,33,-40,27,
		-12,89,48,49,1,33,17,51,17,33,22,22,21,20,6,35,33,17,33,17,35,17,51,1,17,33,50,54,53,52,38,39,1,86,1,-15,-71,1,34,-76,-47,-39,-67,-2,54,-2,15,-70,-70,2,-86,1,19,101,117,114,99,2,-95,1,-103,-2,99,4,-79,-106,-105,-69,2,10,-3,-10,4,58,-3,-52,-2,-113,105,86,81,96,1,0,0,1,-1,-3,0,0,
		3,-33,6,0,0,25,0,123,-78,12,26,27,17,18,57,0,-80,22,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,-65,22,1,93,-78,47,22,1,93,-78,15,22,1,93,-78,25,16,22,17,18,57,-80,25,47,-79,0,1,-80,10,43,88,
		33,-40,27,-12,89,-78,2,4,7,17,18,57,-80,4,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,18,-48,-80,25,16,-80,20,-48,48,49,1,33,17,54,51,32,19,17,35,17,38,38,35,34,6,7,17,35,17,35,53,51,53,51,21,33,2,121,-2,-52,123,-59,1,87,3,-71,1,105,111,90,-120,38,-71,-113,-113,-71,1,52,4,
		-66,-2,-7,-105,-2,125,-3,53,2,-52,117,112,96,78,-4,-3,4,-66,-105,-85,-85,0,0,1,0,-100,-2,-100,4,1,4,58,0,11,0,70,0,-80,8,47,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,
		62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,33,17,51,17,33,17,35,17,33,17,1,86,1,-14,-71,-2,-83,-71,-2,-89,4,58,-4,93,3,-93,-5,-58,-2,-100,1,100,4,58,0,1,0,-100,-1,-20,6,117,5,-80,0,32,0,97,-78,7,33,34,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,
		88,-80,14,47,27,-79,14,30,62,89,-80,0,69,88,-80,23,47,27,-79,23,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-78,7,0,4,17,18,57,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,28,-48,48,49,1,17,20,6,35,34,38,39,6,6,35,34,38,39,17,51,17,
		20,22,51,50,54,53,17,51,17,20,22,51,50,54,53,17,6,117,-31,-61,109,-85,49,52,-78,113,-67,-41,1,-63,114,98,114,-126,-57,124,105,106,122,5,-80,-5,-34,-58,-36,87,89,89,87,-37,-61,4,38,-5,-35,123,-118,-119,124,4,35,-5,-35,125,-120,-119,125,4,34,0,0,1,0,-127,-1,-21,5,-83,4,58,0,30,0,97,-78,6,31,32,17,18,
		57,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,6,21,4,17,18,57,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,26,-48,48,
		49,1,17,20,6,35,34,39,6,35,34,38,39,17,51,17,22,22,51,50,54,53,17,51,17,20,22,51,50,54,55,17,5,-83,-54,-82,-58,89,95,-50,-89,-64,1,-71,1,91,83,98,111,-70,101,92,89,101,1,4,58,-3,39,-80,-58,-108,-108,-61,-80,2,-36,-3,35,102,117,120,103,2,-39,-3,39,103,120,117,102,2,-35,0,0,2,-1,-36,0,0,
		3,-4,6,22,0,17,0,26,0,116,-78,20,27,28,17,18,57,-80,20,16,-80,3,-48,0,-80,0,69,88,-80,14,47,27,-79,14,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,17,14,8,17,18,57,-80,17,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,2,14,8,17,18,57,-80,2,47,-80,0,16,-80,10,-48,-80,17,
		16,-80,12,-48,-80,2,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,22,22,16,6,7,33,17,35,53,51,17,51,17,33,1,17,33,50,54,53,52,38,39,2,-106,-2,-65,1,24,-69,-44,-44,-73,-2,42,-65,-65,-70,1,65,-2,-65,1,18,105,113,111,100,4,58,
		-2,-80,2,-54,-2,-74,-47,3,4,58,-105,1,69,-2,-69,-3,-127,-2,69,119,100,97,125,2,0,1,0,-73,-1,-19,6,-96,5,-59,0,38,0,-118,-78,30,39,40,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,38,47,27,-79,38,30,62,89,-80,0,69,88,-80,29,47,27,-79,29,18,62,89,-80,0,69,88,-80,
		35,47,27,-79,35,18,62,89,-78,16,5,29,17,18,57,-80,16,47,-80,0,-48,-80,5,16,-80,9,-48,-80,5,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,29,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-80,29,16,-80,25,-48,-80,17,16,-80,33,-48,48,49,1,51,54,18,
		36,51,50,0,23,35,38,38,35,34,2,7,33,21,33,21,20,18,51,50,54,55,51,6,4,35,32,0,17,53,35,17,35,17,51,1,120,-57,5,-109,1,6,-84,-26,1,25,24,-64,25,-89,-105,-76,-49,6,2,30,-3,-30,-58,-78,-93,-87,28,-64,27,-2,-31,-18,-2,-2,-2,-55,-57,-63,-63,3,64,-63,1,38,-98,-1,0,-24,-84,-98,-2,-5,-30,-105,
		26,-19,-2,-24,-109,-78,-25,-5,1,114,1,54,20,-3,87,5,-80,0,0,1,0,-103,-1,-20,5,-95,4,78,0,36,0,-57,-78,3,37,38,17,18,57,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,36,47,27,-79,36,26,62,89,-80,0,69,88,-80,33,47,27,-79,33,18,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,
		62,89,-78,15,28,4,17,18,57,-80,15,47,-76,-65,15,-49,15,2,93,-76,63,15,79,15,2,113,-76,-49,15,-33,15,2,113,-76,15,15,31,15,2,114,-76,-97,15,-81,15,2,113,-78,-1,15,1,93,-78,15,15,1,113,-76,47,15,63,15,2,93,-76,111,15,127,15,2,114,-80,0,-48,-78,8,15,4,17,18,57,-80,4,16,-79,11,1,-80,10,43,
		88,33,-40,27,-12,89,-80,15,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,28,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-78,23,28,4,17,18,57,-80,16,16,-80,31,-48,48,49,1,51,54,18,51,50,22,23,35,38,38,35,34,6,7,33,21,33,22,22,51,50,54,55,51,14,2,35,34,2,39,35,17,35,17,51,1,83,-65,
		16,-1,-47,-74,-15,8,-80,8,-113,104,-124,-104,10,1,-75,-2,75,10,-103,-125,99,-108,8,-80,5,120,-60,110,-47,-2,16,-64,-70,-70,2,103,-33,1,8,-38,-82,105,-121,-79,-98,-105,-96,-83,120,90,94,-88,99,1,6,-34,-2,48,4,58,0,2,0,40,0,0,4,-28,5,-80,0,11,0,14,0,87,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,
		-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-78,13,8,2,17,18,57,-80,13,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,14,8,2,17,18,57,48,49,1,35,17,35,17,35,3,35,1,51,1,35,1,33,3,3,-119,
		-86,-68,-98,-104,-59,2,13,-85,2,4,-59,-3,-97,1,-109,-57,1,-74,-2,74,1,-74,-2,74,5,-80,-6,80,2,90,2,73,0,2,0,15,0,0,4,37,4,58,0,11,0,16,0,87,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,
		69,88,-80,10,47,27,-79,10,18,62,89,-78,13,2,8,17,18,57,-80,13,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,15,8,2,17,18,57,48,49,1,35,17,35,17,35,3,35,1,51,1,35,1,33,3,39,7,2,-19,117,-71,124,119,-67,1,-70,-97,1,-67,-66,-2,25,1,47,-128,24,24,1,41,-2,-41,1,41,-2,-41,
		4,58,-5,-58,1,-63,1,59,89,89,0,2,0,-55,0,0,6,-11,5,-80,0,19,0,22,0,125,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,
		62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,21,2,4,17,18,57,-80,21,47,-80,0,-48,-80,21,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,10,-48,-80,6,16,-80,14,-48,-78,22,2,4,17,18,57,48,49,1,33,1,51,1,35,3,35,17,35,17,35,3,35,19,33,17,35,17,51,1,33,3,1,-118,1,-121,1,53,
		-85,2,4,-59,-106,-86,-68,-98,-104,-59,-98,-2,-77,-63,-63,2,69,1,-109,-57,2,89,3,87,-6,80,1,-74,-2,74,1,-74,-2,74,1,-72,-2,72,5,-80,-4,-86,2,73,0,2,0,-68,0,0,5,-28,4,58,0,19,0,24,0,-128,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,18,47,27,-79,18,26,62,89,-80,0,69,
		88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,0,16,18,17,18,57,-80,0,47,-80,1,-48,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,11,-48,-80,7,-48,-80,1,16,-80,20,-48,-80,21,-48,-78,
		23,18,4,17,18,57,48,49,1,33,1,51,1,35,3,35,17,35,17,35,3,35,19,35,17,35,17,51,1,33,3,39,7,1,118,1,15,1,3,-97,1,-67,-66,122,117,-71,124,119,-67,121,-47,-70,-70,1,-55,1,47,-128,24,24,1,-63,2,121,-5,-58,1,41,-2,-41,1,41,-2,-41,1,40,-2,-40,4,58,-3,-121,1,59,89,89,0,2,0,-109,
		0,0,6,63,5,-80,0,29,0,33,0,120,-78,30,34,35,17,18,57,-80,30,16,-80,14,-48,0,-80,0,69,88,-80,28,47,27,-79,28,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-78,1,13,28,17,18,57,-80,1,47,-79,10,
		1,-80,10,43,88,33,-40,27,-12,89,-80,16,-48,-80,1,16,-80,26,-48,-80,1,16,-80,30,-48,-80,28,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,50,22,23,17,35,17,38,38,39,35,7,17,35,17,39,35,34,6,7,17,35,17,54,54,51,51,1,33,1,51,1,33,4,65,27,-12,-20,3,-63,1,124,-102,-123,21,-63,13,
		-120,-98,-126,4,-64,3,-20,-13,42,-2,120,4,-78,-3,-97,16,1,26,-3,-69,3,42,-44,-40,-2,-126,1,120,-112,-126,2,35,-3,-105,2,118,22,123,-115,-2,124,1,126,-40,-44,2,-122,-3,122,1,-24,0,0,2,0,-106,0,0,5,75,4,58,0,27,0,31,0,117,-78,28,32,33,17,18,57,-80,28,16,-80,20,-48,0,-80,0,69,88,-80,6,47,27,
		-79,6,26,62,89,-80,0,69,88,-80,27,47,27,-79,27,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,28,20,6,17,18,57,-80,28,47,-80,4,-48,-80,28,16,-80,7,-48,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,23,-48,-80,6,16,-79,30,1,-80,10,43,88,33,-40,
		27,-12,89,48,49,51,53,54,54,55,1,33,1,22,22,23,21,35,53,38,38,35,35,7,17,35,17,39,35,34,6,7,21,1,51,19,33,-106,4,-54,-46,-2,-31,3,-65,-2,-32,-50,-59,2,-70,2,115,-116,53,11,-71,6,62,-116,117,2,1,-94,8,-73,-2,-117,-74,-51,-46,6,1,-33,-2,33,11,-45,-48,-83,-79,-110,-127,19,-2,79,1,-69,9,126,
		-107,-79,2,92,1,70,0,2,0,-74,0,0,8,114,5,-80,0,34,0,38,0,-107,-78,38,39,40,17,18,57,-80,38,16,-80,30,-48,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,0,69,88,-80,34,47,27,-79,34,18,62,89,-80,0,
		69,88,-80,27,47,27,-79,27,18,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-78,9,5,8,17,18,57,-80,9,47,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-80,35,-48,-80,13,-48,-80,4,16,-80,30,-48,-80,24,-48,-80,11,16,-79,38,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,17,54,55,33,17,35,17,51,
		17,33,1,33,1,51,50,22,23,17,35,17,38,38,39,35,7,17,35,17,39,35,34,6,7,17,1,51,1,33,2,-59,1,79,-2,98,-63,-63,3,89,-2,121,4,-77,-2,120,27,-12,-20,3,-63,1,124,-102,-123,22,-64,14,-121,-98,-126,4,2,21,16,1,26,-3,-69,1,120,-77,105,-3,108,5,-80,-3,124,2,-124,-3,122,-44,-40,-2,-126,1,120,-112,
		-126,2,37,-3,-103,2,117,23,123,-115,-2,124,3,42,1,-24,0,2,0,-101,0,0,7,59,4,58,0,33,0,37,0,-104,-78,30,38,39,17,18,57,-80,30,16,-80,37,-48,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,
		5,47,27,-79,5,18,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-78,10,11,0,17,18,57,-80,10,47,-79,29,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,10,16,-80,13,-48,-80,29,16,-80,22,-48,-80,10,16,-80,34,-48,-80,11,16,-79,36,1,-80,10,43,88,33,-40,27,
		-12,89,48,49,33,53,54,55,33,17,35,17,51,17,33,1,33,1,22,22,23,21,35,53,38,38,35,35,7,17,35,17,39,35,6,6,7,21,1,51,19,33,2,-122,2,70,-2,-121,-70,-70,2,-47,-2,-31,3,-65,-2,-32,-50,-59,2,-70,2,115,-116,53,11,-71,6,75,-123,111,2,1,-94,8,-73,-2,-117,-81,-83,104,-2,60,4,58,-2,34,1,-34,
		-2,33,11,-45,-48,-83,-79,-110,-127,19,-2,79,1,-69,9,2,-128,-109,-81,2,92,1,70,0,0,2,0,80,-2,70,3,-86,7,-122,0,41,0,50,0,-118,-78,42,51,52,17,18,57,-80,42,16,-80,2,-48,0,-80,25,47,-80,46,47,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,18,47,27,-79,18,18,62,89,-80,5,16,-79,
		3,1,-80,10,43,88,33,-40,27,-12,89,-78,40,5,18,17,18,57,-80,40,47,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,12,37,40,17,18,57,-80,18,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,-78,15,46,1,93,-80,46,16,-80,43,-48,-80,43,47,-76,15,43,31,43,2,93,-78,42,46,43,17,18,57,-80,50,-48,48,49,1,52,
		38,35,33,53,33,50,4,21,20,6,7,22,22,21,20,4,35,35,6,21,20,23,23,7,38,38,53,52,54,55,51,54,54,53,16,37,35,53,51,32,3,55,51,21,3,35,3,53,51,2,-38,-99,-121,-2,-50,1,43,-34,1,6,-127,115,-126,-119,-2,-9,-32,52,-115,-126,31,74,122,-115,-91,-94,52,-122,-97,-2,-66,-103,-122,1,63,-69,-105,-96,-2,114,
		-6,-99,4,42,110,-128,-104,-40,-78,103,-92,45,41,-83,-126,-60,-27,3,109,105,66,15,125,53,-88,99,122,-125,1,1,-108,121,1,8,5,-104,3,-91,-86,10,-2,-18,1,18,10,0,0,2,0,76,-2,70,3,118,6,48,0,41,0,50,0,-97,-78,46,51,52,17,18,57,-80,46,16,-80,31,-48,0,-80,24,47,-80,46,47,-80,0,69,88,-80,5,47,27,
		-79,5,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,5,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,40,5,17,17,18,57,-80,40,47,-78,47,40,1,93,-76,-65,40,-49,40,2,93,-76,-97,40,-81,40,2,113,-76,111,40,127,40,2,114,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,12,37,40,17,18,57,-80,17,
		16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,-80,46,16,-80,43,-48,-80,43,47,-76,15,43,31,43,2,93,-78,42,46,43,17,18,57,-80,50,-48,48,49,1,52,38,39,33,53,33,50,22,21,20,6,7,22,21,20,6,35,35,6,21,20,23,23,7,38,38,53,52,54,55,51,54,55,54,53,52,37,35,53,51,32,3,55,51,21,3,35,3,
		53,51,2,-89,127,112,-2,-55,1,39,-54,-18,102,91,-41,-13,-56,50,-115,-126,31,75,124,-118,-91,-94,54,114,67,63,-2,-24,-103,-120,1,19,-39,-105,-96,-2,114,-6,-99,3,9,67,83,2,-103,-86,-117,73,119,36,66,-81,-108,-81,3,109,105,66,15,125,55,-88,97,122,-125,1,2,48,46,72,-94,3,-104,3,29,-86,10,-2,-18,1,18,10,0,3,0,103,
		-1,-20,4,-6,5,-60,0,17,0,24,0,31,0,-116,-78,4,32,33,17,18,57,-80,4,16,-80,18,-48,-80,4,16,-80,25,-48,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,13,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-78,22,13,4,17,18,57,-80,22,47,-78,47,22,1,93,
		-78,-49,22,1,93,-78,47,22,1,113,-78,-1,22,1,93,-78,95,22,1,93,-76,79,22,95,22,2,113,-78,-97,22,1,113,-80,4,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,22,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,4,35,34,36,2,39,53,52,18,36,51,50,4,18,23,1,34,2,7,33,38,2,3,
		50,18,55,33,22,18,4,-6,-113,-2,-8,-79,-84,-2,-10,-109,2,-110,1,11,-84,-81,1,8,-111,2,-3,-74,-74,-48,4,3,20,4,-50,-74,-74,-54,8,-4,-20,8,-45,2,-87,-43,-2,-62,-86,-87,1,57,-50,105,-46,1,66,-85,-88,-2,-59,-49,2,13,-2,-19,-14,-8,1,13,-5,112,1,0,-12,-20,-2,-8,0,3,0,91,-1,-20,4,52,4,78,0,15,
		0,21,0,28,0,-118,-78,4,29,30,17,18,57,-80,4,16,-80,19,-48,-80,4,16,-80,22,-48,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,26,12,4,17,18,57,-80,26,47,-76,-65,26,-49,26,2,93,-76,-97,26,-81,26,2,113,-78,-1,26,1,93,-78,15,26,1,113,-76,47,26,63,
		26,2,93,-76,-49,26,-33,26,2,113,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,54,51,50,0,23,23,20,6,6,35,34,0,53,5,33,22,22,32,54,1,34,6,7,33,38,38,91,123,-31,-113,-44,1,14,
		11,1,124,-32,-112,-34,-2,-15,3,28,-3,-97,13,-92,1,2,-95,-2,-36,125,-94,15,2,94,18,-93,2,39,-97,-3,-117,-2,-30,-27,58,-98,-2,-119,1,51,-5,68,-101,-72,-70,2,121,-75,-109,-105,-79,0,0,1,0,22,0,0,4,-35,5,-61,0,15,0,71,-78,2,16,17,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,
		88,-80,15,47,27,-79,15,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,1,6,12,17,18,57,-80,6,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,23,55,1,54,54,51,23,7,34,6,7,1,35,1,51,2,67,33,35,1,8,51,-122,103,46,1,64,64,31,-2,124,-86,-2,7,-48,1,118,-126,-127,3,63,-105,
		120,1,-85,60,84,-5,121,5,-80,0,0,1,0,46,0,0,4,11,4,77,0,17,0,71,-78,2,18,19,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-78,1,5,14,17,18,57,-80,5,16,-79,10,1,-80,10,43,88,33,-40,
		27,-12,89,48,49,1,23,55,19,54,51,50,23,7,38,35,34,6,7,1,35,1,51,1,-37,23,25,-99,77,-84,71,35,21,13,29,31,60,16,-2,-41,-115,-2,-125,-67,1,60,100,100,2,31,-14,24,-108,8,48,45,-4,-76,4,58,0,2,0,103,-1,115,4,-6,6,52,0,19,0,39,0,84,-78,5,40,41,17,18,57,-80,5,16,-80,25,-48,0,
		-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,6,-48,-80,13,16,-80,16,-48,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,26,-48,-80,3,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,-80,33,-48,48,49,1,16,0,7,21,35,53,38,0,3,53,16,0,55,53,51,21,22,0,17,
		39,52,2,39,21,35,53,6,2,21,21,20,18,23,53,51,21,54,18,53,4,-6,-2,-2,-29,-71,-27,-2,-15,1,1,14,-25,-71,-30,1,3,-65,-103,-115,-71,-109,-93,-92,-110,-71,-113,-105,2,-87,-2,-35,-2,-111,35,-127,127,31,1,113,1,35,96,1,36,1,118,31,118,120,37,-2,-112,-2,-39,7,-32,1,9,35,97,100,31,-2,-18,-33,93,-34,-2,-20,
		31,102,100,34,1,11,-30,0,0,2,0,91,-1,-119,4,52,4,-75,0,19,0,37,0,90,-78,3,38,39,17,18,57,-80,3,16,-80,28,-48,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-80,3,16,-80,6,-48,-80,16,16,-80,13,-48,-80,16,16,-79,35,1,-80,10,43,88,33,-40,27,-12,
		89,-80,20,-48,-80,3,16,-79,29,1,-80,10,43,88,33,-40,27,-12,89,-80,26,-48,48,49,19,52,18,55,53,51,21,22,18,21,21,20,2,7,21,35,53,38,2,53,1,54,54,53,52,38,39,21,35,53,6,6,21,20,22,23,53,51,91,-44,-71,-71,-70,-39,-35,-74,-71,-76,-39,2,70,99,118,116,101,-71,98,114,113,99,-71,2,39,-46,1,42,
		34,112,111,32,-2,-40,-35,16,-40,-2,-40,29,107,108,31,1,39,-36,-2,121,31,-51,-85,-111,-48,32,98,97,33,-48,-91,-110,-53,34,102,0,0,3,0,-100,-1,-21,6,111,7,81,0,44,0,64,0,73,0,-86,-78,10,74,75,17,18,57,-80,10,16,-80,50,-48,-80,10,16,-80,73,-48,0,-80,0,69,88,-80,20,47,27,-79,20,30,62,89,-80,0,69,
		88,-80,13,47,27,-79,13,18,62,89,-80,20,16,-80,0,-48,-80,13,16,-80,7,-48,-78,10,13,20,17,18,57,-80,20,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-78,32,20,13,17,18,57,-80,37,-48,-80,21,16,-80,44,-48,-80,20,16,-80,56,-48,-80,56,47,-80,47,-48,-79,45,2,
		-80,10,43,88,33,-40,27,-12,89,-80,47,16,-80,52,-48,-80,52,47,-79,60,2,-80,10,43,88,33,-40,27,-12,89,-80,56,16,-80,68,-48,-80,73,-48,-80,73,47,48,49,1,50,22,21,17,20,6,35,34,38,39,6,6,35,34,38,39,17,52,54,51,21,34,6,21,17,20,22,51,50,54,53,17,51,17,20,22,51,50,54,53,17,52,38,35,19,
		21,35,34,46,2,35,34,21,21,35,53,52,54,51,50,30,2,1,54,55,53,51,21,20,6,7,4,-37,-69,-39,-39,-69,112,-78,52,52,-80,112,-71,-40,4,-40,-67,99,113,114,98,114,-126,-63,-126,115,99,112,111,100,104,43,80,-126,-72,52,24,113,-128,127,110,40,72,-65,106,-2,64,66,3,-99,91,59,5,-81,-16,-42,-3,-58,-44,-16,85,88,88,85,
		-24,-51,2,74,-44,-15,-98,-99,-119,-3,-60,-116,-101,-119,124,1,-84,-2,84,122,-117,-100,-116,2,58,-120,-97,1,-62,127,34,80,12,112,15,36,110,108,17,82,27,-2,-112,80,60,105,102,50,117,32,0,3,0,126,-1,-21,5,-86,5,-15,0,43,0,63,0,72,0,-80,-78,9,73,74,17,18,57,-80,9,16,-80,60,-48,-80,9,16,-80,72,-48,0,-80,0,
		69,88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,19,16,-80,0,-48,-80,12,16,-80,7,-48,-78,9,12,19,17,18,57,-80,19,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,31,19,12,17,18,57,-80,36,-48,-80,20,16,-80,43,-48,-80,
		19,16,-80,55,-48,-80,55,47,-80,45,-48,-80,45,47,-79,44,2,-80,10,43,88,33,-40,27,-12,89,-80,45,16,-80,51,-48,-80,51,47,-79,59,2,-80,10,43,88,33,-40,27,-12,89,-80,55,16,-80,67,-48,-80,67,47,-80,72,-48,-80,72,47,48,49,1,50,22,21,17,20,6,35,34,39,6,6,35,34,38,39,17,52,54,51,21,34,6,21,17,20,
		22,51,50,54,53,53,51,21,22,22,51,50,54,53,17,52,38,35,19,21,35,34,46,2,35,34,21,21,35,53,52,54,51,50,30,2,1,54,55,53,51,21,20,6,7,4,66,-88,-64,-64,-88,-48,95,47,-100,98,-93,-63,4,-64,-88,82,93,92,83,98,111,-71,1,112,97,81,93,93,81,-86,44,79,126,-64,48,24,114,-128,127,111,41,74,-73,109,
		-2,65,65,3,-98,91,59,4,68,-37,-62,-2,-33,-63,-38,-107,75,74,-48,-69,1,50,-63,-37,-104,-120,124,-2,-34,123,-119,120,103,-21,-18,103,117,-120,125,1,33,124,-120,1,-57,127,32,82,11,111,15,36,110,108,18,80,28,-2,-122,78,63,104,102,50,117,32,0,2,0,-100,-1,-20,6,117,7,3,0,32,0,40,0,-124,-78,7,41,42,17,18,57,-80,
		7,16,-80,39,-48,0,-80,0,69,88,-80,15,47,27,-79,15,30,62,89,-80,0,69,88,-80,23,47,27,-79,23,30,62,89,-80,0,69,88,-80,32,47,27,-79,32,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,4,-48,-78,7,10,15,17,18,57,-80,10,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,28,-48,-80,15,16,-80,
		39,-48,-80,39,47,-80,40,-48,-80,40,47,-79,34,6,-80,10,43,88,33,-40,27,-12,89,-80,40,16,-80,37,-48,-80,37,47,48,49,1,17,20,6,35,34,38,39,6,6,35,34,38,39,17,51,17,20,22,51,50,54,53,17,51,17,20,22,51,50,54,53,17,37,53,33,23,33,21,35,53,6,117,-31,-61,109,-85,49,52,-78,113,-67,-41,1,-63,114,
		98,114,-126,-57,124,105,106,122,-4,66,3,44,1,-2,-75,-88,5,-80,-5,-34,-58,-36,87,89,89,87,-37,-61,4,38,-5,-35,123,-118,-119,124,4,35,-5,-35,125,-120,-119,125,4,34,-24,107,107,125,125,0,0,2,0,-127,-1,-21,5,-83,5,-80,0,30,0,38,0,-121,-78,6,39,40,17,18,57,-80,6,16,-80,35,-48,0,-80,0,69,88,-80,13,47,27,
		-79,13,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-80,0,69,88,-80,30,47,27,-79,30,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,4,-48,-80,4,47,-78,6,8,13,17,18,57,-80,8,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,26,-48,-80,13,16,-80,37,-48,-80,37,47,-80,38,-48,-80,38,47,
		-79,32,6,-80,10,43,88,33,-40,27,-12,89,-80,38,16,-80,35,-48,-80,35,47,48,49,1,17,20,6,35,34,39,6,35,34,38,39,17,51,17,22,22,51,50,54,53,17,51,17,20,22,51,50,54,55,17,1,53,33,23,33,21,35,53,5,-83,-54,-82,-58,89,95,-50,-89,-64,1,-71,1,91,83,98,111,-70,101,92,89,101,1,-4,-109,3,44,3,
		-2,-77,-87,4,58,-3,39,-80,-58,-108,-108,-61,-80,2,-36,-3,35,102,117,120,103,2,-39,-3,39,103,120,117,102,2,-35,1,11,107,107,-128,-128,0,0,1,0,117,-2,-124,4,-68,5,-59,0,25,0,75,-78,24,26,27,17,18,57,0,-80,0,47,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,
		10,16,-80,14,-48,-80,10,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,38,0,53,53,52,18,36,51,50,0,23,35,38,38,35,34,2,21,21,20,18,23,51,3,20,-65,-40,-2,-8,-114,1,0,-96,-9,1,32,2,-63,2,-75,-95,-96,-51,-59,-99,124,-2,-124,1,108,
		28,1,86,-1,-12,-79,1,32,-97,-2,-8,-32,-98,-84,-2,-4,-44,-12,-54,-2,-5,4,0,1,0,100,-2,-126,3,-32,4,78,0,25,0,75,-78,24,26,27,17,18,57,0,-80,0,47,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,10,16,-80,14,-48,-80,10,16,-79,17,1,-80,10,43,88,33,
		-40,27,-12,89,-80,2,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,38,2,53,53,52,54,54,51,50,22,21,35,52,38,35,34,6,21,21,20,22,23,51,2,-94,-71,-79,-44,119,-41,-117,-77,-16,-81,-113,101,-124,-100,-106,-126,109,-2,-126,1,112,30,1,38,-39,35,-103,-7,-118,-31,-88,101,-116,-38,-75,31,-88,-37,3,0,0,1,
		0,116,0,0,4,-112,5,62,0,19,0,19,0,-80,14,47,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,48,49,1,5,7,37,3,35,19,37,55,5,19,37,55,5,19,51,3,5,7,37,2,88,1,33,68,-2,-35,-74,-88,-31,-2,-33,68,1,37,-51,-2,-34,70,1,35,-68,-91,-25,1,37,72,-2,-32,1,-66,-84,123,-86,-2,-65,1,-114,-85,
		123,-85,1,109,-85,125,-85,1,75,-2,104,-85,122,-86,0,1,-4,103,4,-90,-1,39,5,-4,0,7,0,18,0,-80,0,47,-79,3,6,-80,10,43,88,33,-40,27,-12,89,48,49,1,21,39,55,33,39,23,21,-3,13,-90,1,2,27,1,-91,5,35,125,1,-23,108,1,-40,0,1,-4,113,5,23,-1,100,6,21,0,19,0,48,0,-80,14,47,-80,8,
		-48,-80,8,47,-79,0,2,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-80,5,-48,-80,5,47,-80,14,16,-79,15,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,22,21,21,35,53,52,35,34,7,7,6,7,35,53,50,62,2,-2,118,111,127,-128,114,42,45,111,-119,118,60,108,106,-63,71,6,21,108,110,36,14,112,18,47,58,2,126,27,
		83,17,0,1,-3,102,5,22,-2,84,6,87,0,5,0,12,0,-80,1,47,-80,5,-48,-80,5,47,48,49,1,53,51,21,23,7,-3,102,-77,59,77,5,-36,123,-116,116,65,0,0,1,-3,-92,5,22,-2,-109,6,87,0,5,0,12,0,-80,3,47,-80,0,-48,-80,0,47,48,49,1,39,55,39,51,21,-3,-15,77,59,1,-75,5,22,65,116,-116,123,
		0,8,-6,27,-2,-60,1,-74,5,-81,0,12,0,26,0,39,0,53,0,66,0,79,0,92,0,106,0,127,0,-80,69,47,-80,83,47,-80,96,47,-80,56,47,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,9,11,-80,10,43,88,33,-40,27,-12,89,-80,69,16,-80,16,-48,-80,69,16,-79,76,11,-80,10,43,88,33,-40,27,-12,89,-80,23,-48,
		-80,83,16,-80,30,-48,-80,83,16,-79,90,11,-80,10,43,88,33,-40,27,-12,89,-80,37,-48,-80,96,16,-80,43,-48,-80,96,16,-79,103,11,-80,10,43,88,33,-40,27,-12,89,-80,50,-48,-80,56,16,-79,63,11,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,54,50,22,21,35,52,38,35,34,6,21,1,52,54,51,50,22,21,35,52,38,35,34,
		6,21,19,52,54,51,50,22,21,35,52,38,34,6,21,1,52,54,51,50,22,21,35,52,38,35,34,6,21,1,52,54,50,22,21,35,52,38,35,34,6,21,1,52,54,50,22,21,35,52,38,35,34,6,21,1,52,54,51,50,22,21,35,52,38,34,6,21,19,52,54,51,50,22,21,35,52,38,35,34,6,21,-3,8,115,-66,116,112,51,48,
		46,51,1,-34,116,93,95,117,113,53,46,44,51,72,117,93,95,116,112,53,92,51,-2,-53,116,93,95,116,112,53,46,45,51,-3,79,115,-66,116,112,51,48,46,51,-3,77,116,-66,116,112,51,48,46,51,-2,-34,117,93,95,116,112,53,92,51,53,117,93,95,117,113,53,46,45,51,4,-13,84,104,104,84,46,55,53,48,-2,-21,84,104,103,85,49,
		52,53,48,-2,9,85,103,104,84,49,52,55,46,-3,-7,84,104,104,84,49,52,55,46,-2,-28,84,104,104,84,46,55,55,46,5,26,84,104,104,84,46,55,53,48,-2,9,85,103,104,84,49,52,55,46,-3,-7,85,103,103,85,49,52,53,48,0,0,8,-6,44,-2,99,1,107,5,-58,0,4,0,9,0,14,0,19,0,24,0,29,0,34,0,39,
		0,57,0,-80,33,47,-80,18,47,-80,11,47,-80,27,47,-80,38,47,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,22,47,27,-79,22,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,20,62,89,48,49,5,23,3,35,19,3,39,19,51,3,1,55,5,21,37,5,7,37,53,5,1,55,37,23,5,1,7,5,39,37,3,
		39,3,55,19,1,23,19,7,3,-2,47,11,122,96,70,58,12,122,96,70,2,29,13,1,77,-2,-90,-5,117,13,-2,-77,1,90,3,-100,2,1,64,68,-2,-37,-4,-13,2,-2,-64,69,1,38,43,17,-108,65,-58,3,96,17,-108,66,-60,60,14,-2,-83,1,97,4,-94,14,1,82,-2,-96,-2,17,12,124,98,71,59,12,124,98,71,1,-82,16,-103,68,
		-56,-4,-114,17,-103,69,-56,2,-28,2,1,70,69,-2,-43,-4,-29,2,-2,-69,71,1,43,0,-1,-1,0,-79,-2,-101,5,-77,7,25,0,38,0,-36,0,0,0,39,0,-95,1,49,1,66,1,7,0,16,4,127,-1,-67,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,13,-36,48,49,0,-1,-1,0,-100,-2,-101,4,-75,5,-61,0,38,
		0,-16,0,0,0,39,0,-95,0,-95,-1,-20,1,7,0,16,3,-127,-1,-67,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,13,-36,48,49,0,0,2,-1,-36,0,0,3,-4,6,113,0,17,0,26,0,119,-78,20,27,28,17,18,57,-80,20,16,-80,3,-48,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,
		16,47,27,-79,16,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,16,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,2,12,8,17,18,57,-80,2,47,-80,0,16,-80,10,-48,-80,11,-48,-80,2,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,
		33,22,22,16,6,7,33,17,35,53,51,53,51,21,33,1,17,33,50,54,53,52,38,39,2,-106,-2,-65,1,24,-69,-44,-44,-73,-2,42,-65,-65,-70,1,65,-2,-65,1,18,105,113,111,100,5,24,-3,-46,2,-54,-2,-74,-47,3,5,24,-104,-63,-63,-4,-94,-2,69,119,100,97,125,2,0,0,2,0,-88,0,0,4,-41,5,-80,0,14,0,27,0,86,
		-78,4,28,29,17,18,57,-80,4,16,-80,23,-48,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-78,22,3,1,17,18,57,-80,22,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,9,0,3,17,18,57,-80,3,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,
		33,50,4,21,20,7,23,7,39,6,35,1,54,53,52,38,39,33,17,33,50,55,39,55,1,105,-63,2,25,-20,1,19,103,126,109,-117,118,-88,1,25,37,-91,-111,-2,-96,1,88,98,69,110,110,2,58,-3,-58,5,-80,-14,-53,-70,112,-118,103,-103,55,1,27,65,91,-126,-99,2,-3,-59,29,121,102,0,0,2,0,-116,-2,96,4,35,4,78,0,19,
		0,34,0,119,-78,28,35,36,17,18,57,-80,28,16,-80,16,-48,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,2,7,16,17,18,57,-78,9,16,7,17,18,57,-78,14,16,7,17,18,
		57,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,7,23,7,39,6,35,34,39,17,35,17,51,23,54,51,50,18,17,39,52,38,35,34,7,17,22,51,50,55,39,55,23,54,4,30,106,111,110,110,89,115,-59,113,-71,-87,9,113,-55,-61,-29,-71,-100,-120,-88,84,
		83,-85,82,60,102,110,90,50,2,17,-18,-105,125,102,123,56,125,-3,-9,5,-38,120,-116,-2,-38,-2,-6,4,-73,-44,-107,-3,-5,-108,39,115,103,103,98,0,0,1,0,-94,0,0,4,35,7,0,0,9,0,54,-78,3,10,11,17,18,57,0,-80,8,47,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,
		89,-80,6,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,21,33,17,35,17,33,17,51,4,35,3,-3,66,-64,2,-56,-71,5,24,6,-6,-18,5,-80,1,80,0,1,0,-111,0,0,3,66,5,118,0,7,0,47,0,-80,6,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,
		-80,4,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,17,51,3,66,-2,9,-70,1,-8,-71,3,-95,-4,95,4,58,1,60,0,0,1,0,-79,-2,-33,4,124,5,-80,0,21,0,94,-78,10,22,23,17,18,57,0,-80,9,47,-80,0,69,88,-80,20,47,27,-79,20,30,62,89,-80,0,69,88,-80,18,47,27,-79,18,
		18,62,89,-80,20,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,3,20,9,17,18,57,-80,3,47,-80,9,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,51,32,0,17,16,2,35,39,50,54,53,38,38,35,35,17,35,17,33,4,48,-3,66,-78,1,28,1,
		60,-11,-28,2,-111,-112,1,-52,-50,-75,-63,3,127,5,18,-2,47,-2,-49,-2,-16,-2,-8,-2,-25,-109,-61,-53,-53,-44,-3,97,5,-80,0,1,0,-111,-2,-27,3,-66,4,58,0,22,0,94,-78,11,23,24,17,18,57,0,-80,10,47,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-80,21,16,-79,0,
		1,-80,10,43,88,33,-40,27,-12,89,-78,3,21,10,17,18,57,-80,3,47,-80,10,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,51,50,0,21,20,6,6,7,39,54,54,53,52,38,35,35,17,35,17,33,3,62,-2,13,108,-17,1,24,98,-86,117,48,-128,120,-78,
		-104,112,-70,2,-83,3,-95,-2,-28,-2,-4,-41,98,-56,-122,21,-110,33,-103,121,-111,-88,-2,29,4,58,-1,-1,0,27,-2,-103,7,-126,5,-80,0,38,0,-38,0,0,0,7,2,81,6,97,0,0,-1,-1,0,21,-2,-103,6,61,4,58,0,38,0,-18,0,0,0,7,2,81,5,28,0,0,-1,-1,0,-78,-2,-105,5,68,5,-80,0,38,2,44,0,0,
		0,7,2,81,4,35,-1,-2,-1,-1,0,-100,-2,-103,4,-127,4,58,0,38,0,-15,0,0,0,7,2,81,3,96,0,0,0,1,0,-93,0,0,4,-1,5,-80,0,20,0,99,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,
		-80,10,47,27,-79,10,18,62,89,-80,15,-48,-80,15,47,-78,47,15,1,93,-78,-49,15,1,93,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-78,1,8,15,17,18,57,-80,5,-48,-80,15,16,-80,18,-48,48,49,9,2,35,1,35,21,35,53,35,17,35,17,51,17,51,17,51,17,51,1,4,-46,-2,112,1,-67,-15,-2,-94,80,-108,104,-63,-63,104,
		-108,77,1,67,5,-80,-3,78,-3,2,2,-114,-12,-12,-3,114,5,-80,-3,127,1,0,-1,0,2,-127,0,1,0,-102,0,0,4,127,4,58,0,20,0,124,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,20,47,27,-79,20,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,3,47,27,-79,3,
		18,62,89,-80,10,16,-80,14,-48,-80,14,47,-78,-97,14,1,93,-78,-1,14,1,93,-78,-97,14,1,113,-76,-65,14,-49,14,2,93,-78,47,14,1,93,-78,111,14,1,114,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,1,9,14,17,18,57,-80,5,-48,-80,14,16,-80,18,-48,48,49,9,2,35,1,35,21,35,53,35,17,35,17,51,17,51,53,
		51,21,51,1,4,90,-2,-82,1,119,-21,-2,-21,50,-108,101,-70,-70,101,-108,42,1,3,4,58,-3,-2,-3,-56,1,-51,-62,-62,-2,51,4,58,-2,54,-43,-43,1,-54,0,0,1,0,68,0,0,6,-117,5,-80,0,14,0,109,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,
		2,47,27,-79,2,18,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-78,8,6,2,17,18,57,-80,8,47,-78,47,8,1,93,-78,-49,8,1,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,12,1,8,17,18,57,48,49,1,35,17,35,17,33,53,33,17,51,1,51,1,
		1,35,3,-112,-80,-63,-2,37,2,-100,-106,1,-4,-17,-3,-44,2,86,-20,2,-114,-3,114,5,24,-104,-3,126,2,-126,-3,63,-3,17,0,1,0,62,0,0,5,125,4,58,0,14,0,-126,0,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,
		69,88,-80,13,47,27,-79,13,18,62,89,-80,2,16,-80,9,-48,-80,9,47,-78,-97,9,1,93,-78,-1,9,1,93,-78,-97,9,1,113,-76,-65,9,-49,9,2,93,-78,47,9,1,93,-78,111,9,1,114,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,12,0,9,17,18,57,48,49,1,35,
		17,35,17,33,53,33,17,51,1,51,1,1,35,3,27,-120,-70,-2,101,2,85,122,1,107,-31,-2,83,1,-47,-21,1,-51,-2,51,3,-95,-103,-2,54,1,-54,-3,-8,-3,-50,0,-1,-1,0,-87,-2,-103,5,-87,5,-80,0,38,0,44,0,0,0,7,2,81,4,-120,0,0,-1,-1,0,-100,-2,-103,4,-94,4,58,0,38,0,-12,0,0,0,7,2,81,
		3,-127,0,0,0,1,0,-88,0,0,7,-124,5,-80,0,13,0,96,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,1,-48,-80,1,47,-78,47,1,1,93,-80,2,16,-79,4,1,-80,10,
		43,88,33,-40,27,-12,89,-80,1,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,33,17,35,17,33,17,35,17,51,1,105,2,-34,3,61,-3,-125,-64,-3,34,-63,-63,3,62,2,114,-104,-6,-24,2,-95,-3,95,5,-80,0,1,0,-111,0,0,5,105,4,58,0,13,0,-99,0,-80,0,69,88,-80,2,47,27,-79,2,26,
		62,89,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,6,16,-80,1,-48,-80,1,47,-78,111,1,1,93,-76,-65,1,-49,1,2,93,-78,63,1,1,113,-76,-49,1,-33,1,2,113,-78,15,1,1,114,-76,-97,1,-81,1,2,113,-78,-1,1,1,
		93,-78,15,1,1,113,-78,-97,1,1,93,-78,47,1,1,93,-76,111,1,127,1,2,114,-80,2,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,33,17,35,17,33,17,35,17,51,1,75,1,-15,2,45,-2,-116,-71,-2,15,-70,-70,2,101,1,-43,-103,-4,95,1,
		-50,-2,50,4,58,0,0,1,0,-80,-2,-33,7,-51,5,-80,0,23,0,107,-78,17,24,25,17,18,57,0,-80,7,47,-80,0,69,88,-80,22,47,27,-79,22,30,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-78,1,22,7,17,18,57,-80,1,47,-80,7,16,-79,8,1,-80,10,43,88,
		33,-40,27,-12,89,-80,1,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,22,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,32,0,17,16,2,35,39,50,54,53,38,38,35,35,17,35,17,33,17,35,17,33,4,-1,118,1,28,1,60,-11,-28,2,-111,-112,1,-52,-50,121,-63,-3,50,-64,4,79,3,65,-2,-49,-2,-16,-2,
		-8,-2,-25,-109,-61,-53,-53,-44,-3,97,5,18,-6,-18,5,-80,0,1,0,-111,-2,-27,6,-80,4,58,0,24,0,107,-78,18,25,26,17,18,57,0,-80,8,47,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-80,0,69,88,-80,18,47,27,-79,18,18,62,89,-78,1,23,8,17,18,57,-80,1,47,
		-80,8,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,23,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,50,0,21,7,6,6,7,39,54,54,53,52,38,35,35,17,35,17,33,17,35,17,33,3,-10,-96,-8,1,34,3,20,-47,-103,48,124,123,-68,-96,-92,-71,-2,
		14,-70,3,101,2,-123,-2,-4,-41,38,-93,-31,27,-110,32,-106,125,-110,-89,-2,29,3,-95,-4,95,4,58,0,0,2,0,113,-1,-28,5,-94,5,-59,0,40,0,54,0,-96,-78,24,55,56,17,18,57,-80,24,16,-80,41,-48,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,31,47,27,-79,31,30,62,89,-80,0,69,88,-80,4,
		47,27,-79,4,18,62,89,-80,0,-48,-80,0,47,-78,2,4,31,17,18,57,-80,2,47,-80,13,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,44,1,-80,10,43,88,33,-40,27,-12,89,-78,23,2,44,17,18,57,-78,38,44,2,17,18,57,-80,0,16,-79,40,1,-80,10,
		43,88,33,-40,27,-12,89,-80,31,16,-79,51,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,39,6,35,34,36,2,53,53,52,18,54,51,23,34,6,21,21,20,18,51,50,55,38,2,53,53,52,54,54,51,50,18,21,21,20,2,7,22,51,1,20,22,23,54,54,53,53,52,38,35,34,6,21,5,-94,-41,-77,-114,-84,-78,-2,-28,-97,117,
		-46,-124,1,118,-108,-20,-65,70,56,121,-124,104,-67,118,-74,-26,111,102,104,121,-3,125,120,117,98,104,121,99,97,122,28,73,66,-78,1,66,-60,-84,-79,1,34,-93,-91,-2,-39,-90,-20,-2,-41,13,97,1,21,-86,-29,-102,-3,-115,-2,-52,-3,-21,-98,-2,-10,95,26,2,52,-104,-19,74,72,-25,-115,-7,-79,-50,-46,-78,0,2,0,109,-1,-21,4,-100,4,79,
		0,36,0,47,0,-89,-78,4,48,49,17,18,57,-80,4,16,-80,37,-48,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,28,47,27,-79,28,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,2,4,28,17,18,57,-80,2,47,-80,12,16,-79,13,1,-80,10,
		43,88,33,-40,27,-12,89,-80,4,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,39,1,-80,10,43,88,33,-40,27,-12,89,-78,22,20,39,17,18,57,-80,0,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,-78,34,39,36,17,18,57,-80,28,16,-79,44,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,39,6,35,34,38,
		2,53,53,52,18,51,21,34,6,21,21,20,22,51,50,55,38,17,53,52,54,51,50,22,21,21,20,7,22,51,1,20,23,54,55,53,52,38,34,6,7,4,-100,-78,-116,118,-113,-116,-31,127,-59,-101,73,93,-87,-119,46,44,-63,-83,-113,-116,-78,-128,79,97,-2,15,-97,102,3,73,120,70,1,12,57,66,-107,1,18,-89,58,-51,1,14,-98,-83,-110,56,
		-63,-16,11,-94,1,17,94,-64,-21,-7,-50,98,-29,-99,21,1,-87,-42,116,115,-70,117,-126,-98,-115,122,-1,-1,0,57,-2,-103,4,-8,5,-80,0,38,0,60,0,0,0,7,2,81,3,-41,0,0,-1,-1,0,41,-2,-103,4,6,4,58,0,38,0,92,0,0,0,7,2,81,2,-27,0,0,0,1,0,52,-2,-95,6,-109,5,-80,0,19,0,93,0,-80,
		17,47,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-80,7,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,0,-48,-80,7,16,-80,5,-48,-80,3,-48,-80,2,-48,-80,19,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,14,-48,48,
		49,1,33,53,33,53,51,21,33,21,33,17,33,17,51,17,51,3,35,17,33,1,-85,-2,-119,1,119,-63,1,-127,-2,127,2,-50,-63,-104,18,-84,-5,-42,5,24,-105,1,1,-105,-5,-123,5,19,-6,-15,-2,0,1,95,0,1,0,31,-2,-65,5,22,4,58,0,15,0,77,0,-80,13,47,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,
		88,-80,15,47,27,-79,15,18,62,89,-80,3,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,0,-48,-80,15,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,8,-48,-80,6,16,-80,10,-48,48,49,1,33,53,33,21,35,17,33,17,51,17,51,3,35,17,33,1,49,-2,-18,2,-60,-7,1,-14,-70,-128,18,-91,-4,-46,3,-93,
		-105,-105,-4,-12,3,-93,-4,93,-2,40,1,65,-1,-1,0,-106,-2,-103,5,103,5,-80,0,38,0,-31,0,0,0,7,2,81,4,70,0,0,-1,-1,0,103,-2,-103,4,95,4,59,0,38,0,-7,0,0,0,7,2,81,3,62,0,0,0,1,0,-106,0,0,4,-56,5,-80,0,23,0,80,-78,4,24,25,17,18,57,0,-80,0,69,88,-80,0,47,27,
		-79,0,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,7,0,12,17,18,57,-80,7,47,-80,4,-48,-80,7,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,19,-48,48,49,1,17,22,22,51,17,51,17,54,55,17,51,17,35,17,6,7,21,35,53,34,38,39,17,1,87,
		1,-119,-96,-107,121,120,-63,-63,114,127,-107,-8,-17,4,5,-80,-2,50,-102,-124,1,54,-2,-46,13,33,2,-74,-6,80,2,91,34,13,-18,-24,-39,-38,1,-41,0,1,0,-125,0,0,3,-39,4,59,0,22,0,80,-78,6,23,24,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-80,0,
		69,88,-80,0,47,27,-79,0,18,62,89,-78,15,21,0,17,18,57,-80,15,47,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,15,16,-80,18,-48,48,49,33,35,17,6,7,21,35,53,38,38,39,17,51,17,22,23,17,51,17,54,55,17,51,3,-39,-70,70,83,-106,-80,-69,2,-71,5,-81,-106,84,69,-70,1,-120,19,9,-121,-123,13,
		-52,-75,1,67,-2,-75,-45,26,1,24,-2,-22,10,17,2,26,0,1,0,-119,0,0,4,-70,5,-80,0,17,0,71,-78,5,18,19,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,5,1,0,17,18,57,-80,5,47,-79,14,1,
		-80,10,43,88,33,-40,27,-12,89,48,49,51,17,51,17,54,51,50,22,23,17,35,17,38,38,35,34,7,17,-119,-64,-71,-53,-8,-14,3,-64,1,-119,-93,-68,-56,5,-80,-3,-92,53,-40,-33,-2,46,1,-51,-104,-122,55,-3,76,0,2,0,63,-1,-22,5,-67,5,-61,0,29,0,37,0,103,-78,23,38,39,17,18,57,-80,23,16,-80,36,-48,0,-80,0,
		69,88,-80,15,47,27,-79,15,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,31,15,0,17,18,57,-80,31,47,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,31,16,-80,11,-48,-80,0,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,32,0,
		17,53,38,38,53,51,20,22,23,52,18,54,51,32,0,17,21,33,21,20,22,51,50,55,23,6,6,1,33,53,52,38,35,34,2,3,-23,-2,-30,-2,-77,-103,-90,-104,80,87,-114,-3,-106,1,2,1,28,-4,-126,-34,-52,-77,-90,47,64,-46,-3,-32,2,-66,-77,-85,-98,-62,22,1,81,1,41,91,19,-59,-94,90,125,20,-76,1,31,-94,-2,-93,-2,-66,
		108,93,-36,-9,83,-113,45,53,3,90,33,-39,-27,-2,-3,0,0,2,-1,-34,-1,-20,4,99,4,78,0,25,0,33,0,117,-78,20,34,35,17,18,57,-80,20,16,-80,27,-48,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,30,13,0,17,18,57,-80,30,47,-76,-65,30,-49,30,2,93,-79,
		17,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,30,16,-80,9,-48,-80,0,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-78,23,13,0,17,18,57,-80,13,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,0,53,38,38,53,51,20,23,62,2,51,50,18,17,21,33,22,22,51,50,55,23,6,1,34,6,7,33,53,
		38,38,2,-67,-36,-2,-20,120,119,-109,101,20,-124,-56,112,-45,-22,-3,35,4,-77,-118,-82,111,113,-120,-2,-39,112,-104,18,2,30,8,-120,20,1,33,-6,29,-82,-122,-109,48,-126,-55,110,-2,-22,-2,-3,77,-96,-59,-110,88,-47,3,-54,-93,-109,14,-115,-101,0,1,0,-93,-2,-42,4,-52,5,-80,0,22,0,95,-78,21,23,24,17,18,57,0,-80,14,47,-80,
		0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,2,17,18,57,-80,4,47,-80,8,-48,-80,14,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,51,17,51,1,
		51,1,22,0,21,16,2,35,39,50,54,53,38,38,39,33,1,100,-63,-63,-123,2,1,-30,-3,-8,-8,1,13,-7,-26,2,-112,-112,2,-57,-57,-2,-20,5,-80,-3,-113,2,113,-3,-120,22,-2,-46,-6,-2,-8,-2,-28,-104,-63,-55,-54,-46,1,0,0,1,0,-102,-2,-2,4,25,4,58,0,22,0,123,-78,13,23,24,17,18,57,0,-80,7,47,-80,0,69,
		88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-80,19,-48,-80,19,47,-78,-97,19,1,93,-78,-1,19,1,93,-78,-97,19,1,113,-76,-65,19,-49,19,2,93,-78,47,19,1,93,-78,-49,19,1,113,-80,0,-48,-80,7,16,-79,8,1,-80,10,43,88,33,-40,27,
		-12,89,-80,19,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,22,22,21,20,6,6,7,39,54,53,52,38,39,35,17,35,17,51,17,51,1,51,2,127,-61,-50,100,-84,112,48,-8,-83,-91,-78,-70,-70,91,1,-118,-32,2,100,31,-30,-76,93,-59,124,19,-110,57,-26,-118,-110,2,-2,51,4,58,-2,54,1,-54,0,-1,-1,0,47,-2,-101,
		5,-88,5,-80,0,38,0,-35,0,0,0,7,0,16,4,116,-1,-67,-1,-1,0,44,-2,-101,4,-73,4,58,0,38,0,-14,0,0,0,7,0,16,3,-125,-1,-67,0,1,0,-79,-2,75,4,-2,5,-80,0,21,0,-87,-78,10,22,23,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,
		-80,0,69,88,-80,8,47,27,-79,8,20,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-80,2,-48,-80,2,47,-78,95,2,1,93,-78,-49,2,1,93,-78,31,2,1,113,-76,111,2,127,2,2,113,-76,-65,2,-49,2,2,113,-76,15,2,31,2,2,114,-78,-17,2,1,113,-78,-97,2,1,113,-78,79,2,1,113,-78,-1,2,1,93,-78,-81,
		2,1,93,-78,47,2,1,93,-78,63,2,1,114,-80,8,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,33,17,51,17,20,6,35,34,39,55,22,51,50,54,53,17,33,17,35,17,1,114,2,-52,-64,-85,-100,60,54,14,37,61,65,72,-3,52,-63,5,-80,-3,110,2,-110,
		-7,-3,-88,-70,18,-102,14,103,92,2,-43,-3,127,5,-80,0,0,1,0,-111,-2,75,3,-11,4,58,0,22,0,-95,-78,10,23,24,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,20,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,
		-80,2,-48,-80,2,47,-78,111,2,1,93,-76,-65,2,-49,2,2,93,-78,63,2,1,113,-76,-49,2,-33,2,2,113,-78,15,2,1,114,-76,-97,2,-81,2,2,113,-78,-1,2,1,93,-78,15,2,1,113,-78,-97,2,1,93,-78,47,2,1,93,-76,111,2,127,2,2,114,-80,8,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,18,1,
		-80,10,43,88,33,-40,27,-12,89,48,49,1,17,33,17,51,17,20,6,35,34,39,55,22,23,23,50,54,53,17,33,17,35,17,1,75,1,-15,-71,-85,-104,60,52,15,17,60,20,66,72,-2,15,-70,4,58,-2,43,1,-43,-5,109,-86,-78,18,-109,7,5,1,104,92,2,39,-2,50,4,58,0,-1,-1,0,-87,-2,-101,5,-69,5,-80,0,38,0,44,
		0,0,0,7,0,16,4,-121,-1,-67,-1,-1,0,-100,-2,-101,4,-76,4,58,0,38,0,-12,0,0,0,7,0,16,3,-128,-1,-67,-1,-1,0,-87,-2,-101,6,-7,5,-80,0,38,0,49,0,0,0,7,0,16,5,-59,-1,-67,-1,-1,0,-99,-2,-101,6,7,4,58,0,38,0,-13,0,0,0,7,0,16,4,-45,-1,-67,0,2,0,93,-1,-20,5,18,
		5,-60,0,23,0,31,0,97,-78,8,32,33,17,18,57,-80,8,16,-80,24,-48,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,13,0,8,17,18,57,-80,13,47,-80,0,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,13,
		16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,32,0,17,21,20,2,4,35,32,0,17,53,33,53,16,2,35,34,7,7,39,55,54,1,50,18,55,33,21,20,22,2,-128,1,46,1,100,-100,-2,-22,-89,-2,-29,-2,-63,3,-12,-12,-35,-91,-117,61,47,22,-98,1,33,-87,-34,15,-4,-49,-45,5,-60,-2,-121,-2,-79,84,-59,-2,-65,-74,
		1,89,1,69,117,7,1,2,1,28,58,26,-113,13,88,-6,-58,1,5,-37,34,-38,-28,0,0,1,0,104,-1,-21,4,44,5,-80,0,27,0,106,-78,11,28,29,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,
		-78,5,2,11,17,18,57,-80,5,47,-80,11,16,-80,16,-48,-80,11,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-80,27,-48,48,49,1,33,53,33,23,1,22,22,21,20,4,35,34,38,38,53,51,20,22,51,50,54,53,52,38,35,35,53,3,29,-3,118,3,107,1,-2,
		107,-39,-23,-2,-13,-32,-122,-37,118,-64,-100,123,-119,-93,-90,-98,-115,5,18,-98,125,-2,30,14,-25,-58,-61,-24,105,-66,-126,114,-102,-110,120,-99,-114,-105,0,1,0,105,-2,117,4,40,4,58,0,26,0,93,-78,11,27,28,17,18,57,0,-80,11,47,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,
		-48,-78,5,2,11,17,18,57,-80,5,47,-80,11,16,-80,16,-48,-80,11,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,24,3,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-80,26,-48,48,49,1,33,53,33,23,1,22,22,21,20,4,35,34,38,38,53,51,20,22,51,50,54,53,16,37,35,53,3,12,-3,-120,3,101,1,-2,
		114,-44,-24,-2,-12,-34,-124,-41,122,-70,-98,125,-115,-92,-2,-55,-96,3,-95,-103,118,-2,17,16,-31,-59,-61,-25,102,-65,-125,113,-97,-107,121,1,34,8,-105,0,-1,-1,0,58,-2,75,4,116,5,-80,0,38,0,-79,68,0,0,38,2,38,-85,64,0,7,2,84,0,-16,0,0,-1,-1,0,59,-2,75,3,-106,4,58,0,38,0,-20,79,0,0,38,2,38,
		-84,-114,1,7,2,84,0,-31,0,0,0,8,0,-78,0,6,1,93,48,49,-1,-1,0,57,-2,75,5,14,5,-80,0,38,0,60,0,0,0,7,2,84,3,-89,0,0,-1,-1,0,41,-2,75,4,28,4,58,0,38,0,92,0,0,0,7,2,84,2,-75,0,0,0,2,0,87,0,0,4,101,5,-80,0,10,0,19,0,82,-78,4,20,21,17,18,
		57,-80,4,16,-80,13,-48,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,0,1,3,17,18,57,-80,0,47,-80,3,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,51,17,33,34,36,53,52,54,55,1,17,33,
		34,6,21,20,22,23,3,-93,-62,-3,-33,-28,-2,-9,-1,-32,1,109,-2,-95,-116,-95,-97,-118,3,115,2,61,-6,80,-14,-53,-57,-21,4,-3,42,2,56,-106,-128,-126,-97,1,0,2,0,89,0,0,6,103,5,-80,0,23,0,31,0,92,-78,7,32,33,17,18,57,-80,7,16,-80,24,-48,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,
		88,-80,0,47,27,-79,0,18,62,89,-78,7,8,0,17,18,57,-80,7,47,-80,0,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,10,-48,-78,16,0,8,17,18,57,-80,7,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,34,36,53,52,36,55,33,17,51,17,55,54,54,55,54,39,51,23,22,7,6,6,35,37,17,33,34,
		6,20,22,23,2,71,-27,-2,-9,1,1,-29,1,106,-63,88,111,114,3,4,64,-70,22,47,3,4,-27,-61,-2,-17,-2,-96,-114,-98,-104,-123,-12,-55,-58,-19,3,2,61,-6,-21,1,2,-110,123,-94,-89,68,-105,110,-61,-24,-99,2,56,-105,-2,-97,4,0,0,2,0,100,-1,-25,6,110,6,24,0,31,0,43,0,-122,-78,26,44,45,17,18,57,-80,26,16,
		-80,42,-48,0,-80,0,69,88,-80,6,47,27,-79,6,32,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,24,47,27,-79,24,18,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,62,89,-78,5,3,24,17,18,57,-80,24,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-78,17,3,24,17,18,57,-78,26,3,24,17,
		18,57,-80,3,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,-80,28,16,-79,40,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,16,18,51,50,23,17,51,17,6,22,51,54,54,55,54,39,55,22,22,7,14,2,35,6,39,6,35,34,2,53,1,38,35,34,6,21,20,22,51,50,55,39,100,-30,-60,-73,106,-71,2,95,78,-119,-105,4,4,
		65,-77,28,41,2,2,121,-39,-119,-14,78,108,-37,-64,-28,2,-57,82,-95,-121,-108,-111,-120,-89,83,5,2,9,1,8,1,61,-125,2,77,-5,65,95,120,2,-48,-67,-70,-40,1,102,-57,102,-87,-7,-124,4,-70,-74,1,27,-12,1,49,-122,-33,-34,-83,-65,-109,62,0,1,0,54,-1,-29,5,-43,5,-80,0,39,0,102,-78,16,40,41,17,18,57,0,-80,0,
		69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,33,47,27,-79,33,18,62,89,-78,1,40,9,17,18,57,-80,1,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-78,15,0,1,17,18,57,-80,33,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-78,26,33,9,17,18,57,
		48,49,19,53,51,54,54,53,52,33,33,53,33,22,22,21,20,7,22,19,21,20,22,51,54,54,55,54,39,51,23,22,7,6,2,35,4,3,53,52,38,39,-2,-101,-97,-109,-2,-53,-2,-96,1,107,-17,-4,-19,-37,5,83,65,116,-122,4,4,65,-70,23,48,3,4,-10,-57,-2,-67,15,-121,117,2,121,-98,2,123,-125,-5,-98,1,-47,-55,-24,98,69,
		-2,-4,80,79,91,2,-50,-71,-69,-40,88,-69,-128,-3,-2,-41,8,1,77,64,120,-112,1,0,0,1,0,49,-1,-29,4,-24,4,58,0,39,0,99,-78,15,40,41,17,18,57,0,-80,0,69,88,-80,31,47,27,-79,31,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,7,14,31,17,18,
		57,-78,23,40,31,17,18,57,-80,23,47,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,31,16,-79,29,1,-80,10,43,88,33,-40,27,-12,89,-78,37,20,23,17,18,57,48,49,37,6,51,54,54,55,54,39,51,22,22,7,6,6,35,6,38,39,53,52,35,35,39,51,54,54,53,52,38,35,33,39,33,22,22,21,20,7,22,23,2,-25,2,
		95,112,118,3,4,66,-76,45,24,1,4,-25,-72,-121,-119,7,-40,-51,2,-64,122,110,125,117,-2,-5,6,1,24,-60,-36,-68,-74,4,-43,88,2,-101,-119,-103,-90,-122,-128,57,-51,-16,3,112,-125,71,-99,-106,1,87,74,85,93,-106,3,-89,-104,-99,74,52,-78,0,0,1,0,82,-2,-41,3,-11,5,-81,0,33,0,96,-78,32,34,35,17,18,57,0,-80,23,
		47,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,89,-78,1,34,9,17,18,57,-80,1,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-78,15,0,1,17,18,57,-80,26,16,-80,18,-80,10,43,88,-40,27,-36,89,48,49,19,53,51,54,
		54,53,16,33,33,53,33,22,22,21,20,7,22,19,21,51,21,20,6,7,39,54,55,35,38,39,53,52,38,35,-81,-87,-92,-101,-2,-54,-2,-15,1,33,-24,-12,-27,-34,4,-87,97,77,106,81,14,107,60,3,-110,119,2,121,-105,1,125,-123,1,5,-105,3,-46,-55,-30,100,70,-2,-8,-87,-108,97,-56,64,72,115,110,52,-85,-113,126,-115,0,1,0,121,
		-2,-57,3,-39,4,58,0,32,0,96,-78,32,33,34,17,18,57,0,-80,23,47,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,89,-78,1,33,8,17,18,57,-80,1,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-78,15,0,1,17,18,
		57,-80,26,16,-80,18,-80,10,43,88,-40,27,-36,89,48,49,19,39,51,54,53,52,35,33,53,33,22,23,22,21,20,7,22,23,21,51,21,20,6,7,39,54,55,35,38,39,53,52,35,-62,1,-37,-23,-11,-2,-23,1,39,-35,108,86,-66,-67,1,-102,98,77,105,84,13,103,51,2,-38,1,-72,-105,2,-95,-78,-106,3,103,83,-124,-95,73,53,-54,76,
		-108,97,-54,62,72,116,125,33,-123,94,-76,0,0,1,0,68,-1,-21,7,112,5,-80,0,35,0,101,-78,0,36,37,17,18,57,0,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,-80,0,69,88,-80,32,47,27,-79,32,18,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-80,14,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,7,
		16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,32,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-78,25,14,32,17,18,57,48,49,1,33,3,2,2,6,7,35,53,55,62,2,55,19,33,17,20,22,51,50,54,55,54,39,55,22,22,7,6,2,7,7,34,38,53,4,39,-2,26,26,15,89,-84,-112,63,40,93,100,52,11,30,3,95,
		89,79,-126,-105,4,2,63,-70,28,41,2,3,-23,-61,46,-77,-73,5,18,-3,-65,-2,-34,-2,-36,-119,2,-99,2,7,107,-22,-13,2,-62,-5,-84,96,116,-51,-68,-64,-46,1,102,-57,102,-20,-2,-38,18,2,-70,-76,0,1,0,63,-1,-21,6,58,4,58,0,33,0,101,-78,32,34,35,17,18,57,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,
		0,69,88,-80,30,47,27,-79,30,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,12,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,30,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-78,22,30,12,17,18,57,48,49,1,33,3,2,6,7,35,53,55,54,54,
		55,19,33,17,20,22,51,50,54,55,54,39,51,23,22,7,14,2,35,34,38,39,3,49,-2,-69,23,20,-100,-91,65,54,85,77,13,23,2,-81,90,79,108,123,4,4,65,-77,22,48,3,2,108,-66,120,-82,-77,1,3,-95,-2,90,-2,-21,-28,2,-93,4,10,-89,-45,2,15,-3,33,96,121,-73,-85,-78,-53,80,-79,124,-102,-26,121,-72,-79,0,0,1,
		0,-87,-1,-25,7,113,5,-80,0,29,0,-80,-78,20,30,31,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-80,17,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,10,0,
		17,17,18,57,-80,23,16,-80,28,-48,-80,28,47,-78,-17,28,1,113,-78,95,28,1,93,-78,-49,28,1,93,-78,31,28,1,113,-76,111,28,127,28,2,113,-76,-65,28,-49,28,2,113,-78,-97,28,1,113,-78,79,28,1,113,-78,-1,28,1,93,-78,-81,28,1,93,-78,47,28,1,93,-76,15,28,31,28,2,114,-78,63,28,1,114,-79,21,1,-80,10,43,
		88,33,-40,27,-12,89,48,49,1,17,20,22,51,54,54,55,54,39,55,22,22,7,14,2,35,6,38,39,17,33,17,35,17,51,17,33,17,4,-23,93,74,-122,-108,4,4,66,-69,27,43,2,2,123,-40,-118,-85,-75,8,-3,66,-63,-63,2,-66,5,-80,-5,-84,101,111,2,-51,-70,-73,-37,1,98,-54,103,-88,-5,-125,4,-72,-69,1,39,-3,127,5,-80,
		-3,110,2,-110,0,1,0,-112,-1,-25,6,77,4,58,0,28,0,-91,-78,27,29,30,17,18,57,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,7,-48,-80,7,47,-78,111,7,1,93,-76,
		-65,7,-49,7,2,93,-78,63,7,1,113,-76,-49,7,-33,7,2,113,-78,15,7,1,114,-76,-97,7,-81,7,2,113,-78,-1,7,1,93,-78,15,7,1,113,-78,-97,7,1,93,-78,47,7,1,93,-76,111,7,127,7,2,114,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,25,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-78,18,25,8,17,18,
		57,48,49,1,33,17,35,17,51,17,33,17,51,17,20,22,51,54,54,55,54,39,51,23,22,7,6,2,35,6,38,39,3,67,-2,6,-71,-71,1,-6,-71,92,77,108,124,4,4,65,-78,23,48,3,4,-26,-69,-89,-77,8,1,-51,-2,51,4,58,-2,42,1,-42,-3,33,100,117,2,-75,-85,-84,-47,83,-79,121,-22,-2,-15,4,-73,-69,0,1,0,118,
		-1,-21,4,-96,5,-59,0,34,0,73,-78,21,35,36,17,18,57,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,9,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-78,27,0,9,17,18,57,48,49,5,34,36,2,39,17,52,
		18,36,51,50,23,7,38,35,34,2,21,21,20,22,22,51,54,54,55,54,39,51,23,22,7,14,2,2,-71,-92,-2,-8,-107,2,-108,1,10,-91,-36,-121,59,-122,-94,-84,-41,98,-80,113,-115,-106,3,3,53,-70,38,19,1,2,123,-34,21,-101,1,24,-83,1,16,-81,1,30,-99,88,-118,68,-2,-2,-46,-2,-125,-43,117,2,-103,-122,-102,-49,-77,91,91,-120,
		-55,109,0,1,0,101,-1,-21,3,-57,4,78,0,30,0,70,-78,19,31,32,17,18,57,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,5,11,19,17,18,57,-80,19,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,54,54,55,
		52,39,51,22,7,6,6,35,34,0,53,53,52,54,54,51,50,23,7,38,35,34,6,21,21,20,22,2,81,96,90,2,20,-78,28,1,4,-60,-83,-36,-2,-16,118,-42,-117,-71,96,44,99,-118,-125,-101,-90,-126,2,80,89,122,114,-106,86,-103,-87,1,50,-9,30,-105,-7,-116,66,-112,58,-36,-77,31,-85,-37,0,1,0,35,-1,-25,5,71,5,-80,0,24,
		0,79,-78,5,25,26,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,-80,21,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,14,2,21,17,18,57,48,49,1,33,53,33,21,33,17,20,22,
		51,54,54,18,39,55,22,22,7,14,2,35,6,38,39,1,-2,-2,37,4,-128,-2,28,92,76,-122,-108,8,66,-70,27,43,3,2,121,-39,-119,-86,-73,8,5,18,-98,-98,-4,72,96,114,2,-48,1,110,-37,1,98,-54,103,-87,-7,-124,4,-73,-68,0,0,1,0,70,-1,-25,4,-73,4,58,0,24,0,79,-78,22,25,26,17,18,57,0,-80,0,69,88,
		-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,-80,21,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,14,21,2,17,18,57,48,49,1,33,53,33,21,33,17,20,22,51,54,54,55,54,39,51,22,22,7,6,6,35,6,
		38,39,1,-84,-2,-102,3,-117,-2,-107,94,77,113,119,3,4,64,-78,42,27,1,4,-24,-71,-86,-77,8,3,-92,-106,-106,-3,-75,99,116,2,-99,-119,-105,-82,125,-116,60,-48,-17,4,-71,-71,0,1,0,-106,-1,-20,4,-1,5,-59,0,41,0,114,-78,36,42,43,17,18,57,0,-80,0,69,88,-80,22,47,27,-79,22,30,62,89,-80,0,69,88,-80,11,47,
		27,-79,11,18,62,89,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-80,6,-48,-78,37,11,22,17,18,57,-80,37,47,-78,-49,37,1,93,-78,-97,37,1,113,-79,38,1,-80,10,43,88,33,-40,27,-12,89,-78,16,38,37,17,18,57,-80,22,16,-80,27,-48,-80,22,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,22,51,
		50,54,53,51,20,6,6,35,32,36,53,52,37,38,38,53,52,36,33,50,22,22,21,35,52,38,35,34,6,21,20,22,23,51,21,35,6,6,1,88,-49,-80,-101,-52,-63,-115,-2,-99,-2,-5,-2,-60,1,20,120,-122,1,37,1,6,-109,-11,-116,-63,-63,-110,-89,-62,-83,-93,-60,-60,-79,-75,1,-110,120,-110,-104,116,-125,-66,103,-27,-59,-1,86,48,-90,101,
		-60,-37,101,-70,117,103,-113,-120,118,117,125,2,-98,2,126,0,-1,-1,0,47,-2,75,5,-84,5,-80,0,38,0,-35,0,0,0,7,2,84,4,69,0,0,-1,-1,0,44,-2,75,4,-69,4,58,0,38,0,-14,0,0,0,7,2,84,3,84,0,0,0,2,0,111,4,112,2,-55,5,-42,0,5,0,13,0,35,0,-80,11,47,-80,7,-48,-80,7,47,
		-80,1,-48,-80,1,47,-80,11,16,-80,4,-48,-80,4,47,-80,5,-48,25,-80,5,47,24,48,49,1,19,51,21,3,35,1,51,21,22,23,7,38,53,1,-111,116,-60,-33,89,-2,-34,-88,3,80,73,-78,4,-108,1,66,21,-2,-61,1,82,91,123,85,59,95,-69,0,-1,-1,0,37,2,31,2,13,2,-74,0,6,0,17,0,0,-1,-1,0,37,2,31,
		2,13,2,-74,0,6,0,17,0,0,-1,-1,0,-93,2,-117,4,-115,3,34,0,70,1,-81,-39,0,76,-51,64,0,-1,-1,0,-111,2,-117,5,-55,3,34,0,70,1,-81,-124,0,102,102,64,0,0,2,0,13,-2,107,3,-95,0,0,0,3,0,7,0,8,0,-78,5,2,3,43,48,49,1,33,53,33,53,33,53,33,3,-95,-4,108,3,-108,-4,108,
		3,-108,-2,107,-105,103,-105,0,0,1,0,96,4,49,1,120,6,19,0,8,0,33,-78,8,9,10,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,32,62,89,-78,5,9,0,17,18,57,-80,5,47,48,49,1,23,6,7,21,35,53,52,54,1,14,106,93,3,-72,97,6,19,72,127,-109,-120,116,102,-56,0,1,0,48,4,22,1,71,6,0,
		0,8,0,33,-78,8,9,10,17,18,57,0,-80,0,69,88,-80,4,47,27,-79,4,32,62,89,-78,0,9,4,17,18,57,-80,0,47,48,49,19,39,54,55,53,51,21,6,6,-103,105,93,3,-73,1,97,4,22,72,-126,-112,-112,-126,100,-57,0,1,0,36,-2,-27,1,59,0,-75,0,8,0,31,-78,8,9,10,17,18,57,0,-80,9,47,-79,4,5,
		-80,10,43,88,33,-40,27,-12,89,-80,0,-48,-80,0,47,48,49,19,39,54,55,53,51,21,20,6,-115,105,91,3,-71,99,-2,-27,73,127,-110,118,100,101,-54,0,0,1,0,79,4,22,1,103,6,0,0,8,0,12,0,-80,8,47,-80,4,-48,-80,4,47,48,49,1,21,22,23,7,38,38,39,53,1,6,4,93,106,77,95,2,6,0,-109,-112,127,
		72,64,-62,97,-121,0,-1,-1,0,104,4,49,2,-69,6,19,0,38,1,-124,8,0,0,7,1,-124,1,67,0,0,-1,-1,0,60,4,22,2,-122,6,0,0,38,1,-123,12,0,0,7,1,-123,1,63,0,0,0,2,0,36,-2,-45,2,100,0,-10,0,8,0,17,0,49,-78,10,18,19,17,18,57,-80,10,16,-80,5,-48,0,-80,18,47,-79,4,5,
		-80,10,43,88,33,-40,27,-12,89,-80,0,-48,-80,0,47,-80,9,-48,-80,9,47,-80,4,16,-80,13,-48,48,49,19,39,54,55,53,51,21,20,6,23,39,54,55,53,51,21,20,6,-115,105,91,3,-71,99,-35,105,91,3,-70,97,-2,-45,72,-119,-103,-71,-92,108,-45,64,72,-119,-103,-71,-92,107,-47,0,1,0,70,0,0,4,36,5,-80,0,11,0,76,
		0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,10,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,48,49,1,33,17,35,17,33,53,33,17,51,17,33,4,36,
		-2,108,-70,-2,112,1,-112,-70,1,-108,3,-95,-4,95,3,-95,-103,1,118,-2,-118,0,0,1,0,87,-2,96,4,52,5,-80,0,19,0,126,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,20,62,89,-80,
		0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,9,-48,-80,16,-48,-80,17,-48,-80,6,16,-80,18,-48,-80,19,-48,48,49,33,33,17,35,17,33,53,33,17,33,53,33,17,51,17,33,21,33,
		17,33,4,52,-2,106,-70,-2,115,1,-115,-2,115,1,-115,-70,1,-106,-2,106,1,-106,-2,96,1,-96,-105,3,10,-103,1,118,-2,-118,-103,-4,-10,0,0,1,0,-118,2,23,2,34,3,-53,0,13,0,23,-78,10,14,15,17,18,57,0,-80,3,47,-80,10,-80,10,43,88,-40,27,-36,89,48,49,19,52,54,51,50,22,21,21,20,6,35,34,38,53,-118,
		111,92,91,114,110,94,93,111,3,4,87,112,109,93,37,87,110,111,88,0,-1,-1,0,-108,-1,-11,3,47,0,-47,0,38,0,18,4,0,0,7,0,18,1,-71,0,0,-1,-1,0,-108,-1,-11,4,-50,0,-47,0,38,0,18,4,0,0,39,0,18,1,-71,0,0,0,7,0,18,3,88,0,0,0,1,0,82,2,2,1,44,2,-43,0,11,0,25,
		-78,3,12,13,17,18,57,0,-80,3,47,-79,9,5,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,51,50,22,21,20,6,35,34,38,82,54,54,54,56,56,54,54,54,2,107,45,61,61,45,45,60,60,0,0,6,0,68,-1,-21,7,87,5,-59,0,21,0,35,0,39,0,53,0,67,0,81,0,-68,-78,2,82,83,17,18,57,-80,2,16,
		-80,27,-48,-80,2,16,-80,38,-48,-80,2,16,-80,40,-48,-80,2,16,-80,54,-48,-80,2,16,-80,73,-48,0,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,18,47,27,-79,18,18,62,89,-80,3,-48,-80,3,47,-80,7,-48,-80,7,47,-80,18,16,-80,14,-48,-80,14,47,-80,25,16,-80,32,-48,-80,32,47,-78,36,18,25,17,18,
		57,-80,36,47,-78,38,25,18,17,18,57,-80,38,47,-80,18,16,-79,43,4,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,50,4,-80,10,43,88,33,-40,27,-12,89,-80,43,16,-80,57,-48,-80,50,16,-80,64,-48,-80,32,16,-79,71,4,-80,10,43,88,33,-40,27,-12,89,-80,25,16,-79,78,4,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,
		54,51,50,23,54,51,50,22,21,21,20,6,35,34,39,6,35,34,38,53,1,52,54,51,50,22,21,21,20,6,35,34,38,53,1,39,1,23,3,20,22,51,50,54,53,53,52,38,35,34,6,21,5,20,22,51,50,54,53,53,52,38,35,34,6,21,1,20,22,51,50,54,53,53,52,38,35,34,6,21,3,55,-89,-125,-104,77,79,-105,-125,-88,
		-89,-126,-103,79,76,-105,-126,-86,-3,13,-89,-125,-124,-89,-91,-124,-126,-86,1,105,104,2,-57,104,-77,88,74,72,86,87,73,71,89,1,-53,88,73,72,86,87,73,72,87,-5,66,88,74,71,87,86,74,72,88,1,101,-125,-87,121,121,-88,-117,71,-125,-87,120,120,-89,-117,3,123,-125,-86,-86,-120,72,-127,-86,-89,-117,-4,28,66,4,114,66,-4,55,79,101,99,
		85,74,79,100,99,84,74,79,101,102,82,74,79,100,100,83,2,-22,78,101,98,85,73,78,102,101,83,0,0,1,0,108,0,-103,2,32,3,-75,0,6,0,16,0,-80,5,47,-78,2,7,5,17,18,57,-80,2,47,48,49,1,1,35,1,53,1,51,1,30,1,2,-115,-2,-39,1,39,-115,2,38,-2,115,1,-124,19,1,-123,0,1,0,89,0,-104,
		2,14,3,-75,0,6,0,16,0,-80,0,47,-78,3,7,0,17,18,57,-80,3,47,48,49,19,1,21,1,35,1,1,-25,1,39,-2,-39,-114,1,2,-2,-2,3,-75,-2,123,19,-2,123,1,-114,1,-113,0,1,0,59,0,110,3,106,5,34,0,3,0,9,0,-80,0,47,-80,2,47,48,49,55,39,1,23,-93,104,2,-57,104,110,66,4,114,66,0,
		-1,-1,0,54,2,-101,2,-69,5,-80,3,7,2,32,0,0,2,-101,0,19,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,13,-48,48,49,0,0,1,0,122,2,-117,2,-8,5,-70,0,15,0,84,-78,10,16,17,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,
		69,88,-80,13,47,27,-79,13,22,62,89,-80,0,69,88,-80,6,47,27,-79,6,22,62,89,-78,1,13,3,17,18,57,-80,3,16,-79,10,3,-80,10,43,88,33,-40,27,-12,89,48,49,19,23,54,51,32,17,17,35,17,38,35,34,7,17,35,17,-6,30,74,-110,1,4,-86,3,-115,110,44,-86,5,-85,123,-118,-2,-58,-2,11,1,-26,-71,109,-3,-50,
		3,32,0,1,0,91,0,0,4,104,5,-60,0,41,0,-102,-78,33,42,43,17,18,57,0,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,41,25,6,17,18,57,-80,41,47,-79,0,2,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,
		-80,9,-48,-80,0,16,-80,14,-48,-80,41,16,-80,16,-48,-80,41,16,-80,21,-48,-80,21,47,-74,15,21,31,21,47,21,3,93,-79,18,2,-80,10,43,88,33,-40,27,-12,89,-80,25,16,-80,29,-48,-80,25,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-80,36,-48,-80,18,16,-80,38,-48,48,49,1,33,23,20,7,33,7,33,53,51,
		54,54,55,53,39,35,53,51,39,35,53,51,39,52,54,51,50,22,21,35,52,38,35,34,6,21,23,33,21,33,23,33,3,21,-2,-79,3,62,2,-35,1,-5,-8,77,40,50,2,3,-86,-90,4,-94,-99,6,-11,-56,-66,-34,-65,127,111,105,-126,6,1,92,-2,-87,4,1,83,1,-42,68,-102,91,-99,-99,9,-125,96,8,69,125,-120,125,-73,-57,-18,-44,
		-79,107,124,-102,125,-73,125,-120,0,5,0,31,0,0,6,54,5,-80,0,27,0,31,0,35,0,38,0,41,0,-77,0,-80,0,69,88,-80,23,47,27,-79,23,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,30,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,16,12,23,17,18,57,
		-80,16,47,-80,20,-48,-80,20,47,-76,15,20,31,20,2,93,-80,36,-48,-80,36,47,-80,24,-48,-80,24,47,-80,0,-48,-80,0,47,-80,20,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,31,-48,-80,35,-48,-80,3,-48,-80,16,16,-80,28,-48,-80,28,47,-80,32,-48,-80,32,47,-80,4,-48,-80,4,47,-80,16,16,-79,15,1,-80,10,43,88,33,
		-40,27,-12,89,-80,11,-48,-80,41,-48,-80,7,-48,-78,38,23,12,17,18,57,-78,39,9,26,17,18,57,48,49,1,51,21,35,21,51,21,35,17,35,1,33,17,35,17,35,53,51,53,35,53,51,17,51,1,33,17,51,1,33,39,35,5,51,53,33,37,51,39,1,53,35,5,87,-33,-33,-33,-33,-62,-2,-63,-2,98,-64,-39,-39,-39,-39,-64,1,81,
		1,-113,-65,-4,97,1,59,97,-38,2,20,-52,-2,-44,-2,76,119,119,2,-32,104,3,-84,-104,-108,-104,-2,24,1,-24,-2,24,1,-24,-104,-108,-104,2,4,-3,-4,2,4,-4,-48,-108,-108,-108,-104,-74,-4,-25,-97,0,0,2,0,-89,-1,-20,6,3,5,-80,0,31,0,40,0,-90,-78,35,41,42,17,18,57,-80,35,16,-80,17,-48,0,-80,0,69,88,-80,22,
		47,27,-79,22,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,26,62,89,-80,0,69,88,-80,30,47,27,-79,30,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,30,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,
		0,16,-80,14,-48,-80,15,-48,-78,33,20,22,17,18,57,-80,33,47,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,30,16,-80,29,-48,-80,29,47,-80,22,16,-79,39,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,20,22,51,50,55,23,6,35,34,38,53,17,35,6,6,7,35,17,35,17,33,50,22,23,51,17,51,17,51,1,51,
		50,54,53,52,38,39,35,5,-2,-54,54,65,35,52,1,73,70,124,126,-113,20,-25,-57,-55,-71,1,121,-54,-19,20,-113,-70,-54,-5,98,-64,-117,-117,-121,-124,-53,3,-85,-3,97,65,65,12,-106,20,-106,-118,2,-97,-73,-67,2,-3,-53,5,-80,-64,-74,1,6,-2,-6,-2,-110,-115,-105,-104,-114,2,-1,-1,0,-88,-1,-20,8,16,5,-80,0,38,0,54,0,0,
		0,7,0,87,4,85,0,0,0,7,0,31,0,0,5,-52,5,-80,0,31,0,35,0,39,0,43,0,48,0,53,0,58,0,-2,-78,57,59,60,17,18,57,-80,57,16,-80,30,-48,-80,57,16,-80,34,-48,-80,57,16,-80,39,-48,-80,57,16,-80,43,-48,-80,57,16,-80,45,-48,-80,57,16,-80,51,-48,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,
		89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,8,2,12,17,18,57,-80,8,47,-80,4,-48,-80,4,47,-80,0,-48,-80,4,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,14,-48,-80,10,16,-80,18,-48,-80,8,16,-80,20,
		-48,-80,6,16,-80,22,-48,-80,4,16,-80,24,-48,-80,2,16,-80,26,-48,-80,4,16,-80,28,-48,-80,2,16,-80,30,-48,-80,8,16,-80,32,-48,-80,6,16,-80,34,-48,-80,8,16,-80,36,-48,-80,6,16,-80,38,-48,-80,8,16,-80,40,-48,-80,6,16,-80,42,-48,-80,10,16,-80,45,-48,-78,48,2,12,17,18,57,-80,10,16,-80,50,-48,-78,53,2,12,
		17,18,57,-80,4,16,-80,54,-48,-78,57,2,12,17,18,57,48,49,1,51,19,51,3,51,21,35,7,51,21,35,3,35,3,35,3,35,3,35,53,51,39,35,53,51,3,51,19,51,19,51,1,51,55,35,5,51,55,35,5,51,39,35,3,55,35,23,23,37,55,35,23,23,1,51,39,39,7,3,-89,-22,88,-63,101,-121,-88,41,-47,-15,102,-72,
		86,-27,88,-72,103,-20,-52,41,-93,-126,101,-64,91,-15,86,-77,-2,72,112,35,-72,2,113,108,36,-77,-2,-36,-82,34,104,-42,2,55,1,23,2,101,1,53,2,27,-2,-64,50,1,24,24,3,-44,1,-36,-2,36,-104,-62,-104,-2,30,1,-30,-2,30,1,-30,-104,-62,-104,1,-36,-2,36,1,-36,-4,-54,-62,-62,-62,-62,-62,-2,-100,10,6,-46,-46,6,7,-53,
		2,-60,7,-83,-79,0,0,2,0,-116,0,0,5,-98,4,58,0,13,0,27,0,102,0,-80,0,69,88,-80,22,47,27,-79,22,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-78,5,17,
		0,17,18,57,-80,5,47,-80,0,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-78,15,10,11,17,18,57,-80,15,47,48,49,1,50,22,23,17,35,17,52,38,39,33,17,35,17,1,17,51,17,33,50,54,55,17,51,17,6,6,7,2,-70,-81,-88,4,-71,101,111,-2,-67,-71,1,-119,-71,1,62,113,103,1,-71,2,-91,-83,4,58,-63,-65,-2,
		-93,1,76,127,120,1,-4,95,4,58,-5,-58,2,-35,-3,-69,117,126,2,-81,-3,78,-62,-60,2,0,0,1,0,95,-1,-20,4,28,5,-60,0,35,0,-117,-78,21,36,37,17,18,57,0,-80,0,69,88,-80,22,47,27,-79,22,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,35,9,22,17,18,57,-80,35,47,-79,0,2,-80,10,43,
		88,33,-40,27,-12,89,-80,9,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,12,-48,-80,35,16,-80,15,-48,-80,35,16,-80,31,-48,-80,31,47,-74,15,31,31,31,47,31,3,93,-79,32,2,-80,10,43,88,33,-40,27,-12,89,-80,16,-48,-80,31,16,-80,19,-48,-80,22,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,
		33,22,22,51,50,55,23,6,35,34,0,3,35,53,51,53,35,53,51,18,0,51,50,23,7,38,35,34,6,7,33,21,33,21,33,3,81,-2,-128,4,-76,-91,116,102,20,120,120,-8,-2,-29,6,-78,-78,-78,-78,10,1,29,-13,106,-121,20,109,110,-92,-79,6,1,127,-2,-128,1,-128,2,29,-61,-46,34,-96,30,1,37,1,12,124,-119,125,1,6,1,
		31,31,-94,35,-53,-68,125,-119,0,4,0,31,0,0,5,-68,5,-80,0,25,0,30,0,35,0,40,0,-68,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,11,16,-79,40,1,-80,10,43,88,33,-40,27,-12,89,-78,36,40,1,17,18,57,-80,36,47,-78,112,36,1,113,-74,0,36,16,36,
		32,36,3,93,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-80,29,-48,-80,29,47,-78,112,29,1,113,-74,0,29,16,29,32,29,3,93,-79,32,1,-80,10,43,88,33,-40,27,-12,89,-80,33,-48,-80,33,47,-78,112,33,1,113,-78,32,33,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,32,16,-80,3,-48,-80,29,16,-80,6,-48,-80,6,
		47,-80,28,16,-80,7,-48,-80,36,16,-80,10,-48,-80,36,16,-80,15,-48,-80,28,16,-80,18,-48,-80,29,16,-80,20,-48,-80,20,47,48,49,1,17,35,17,35,53,51,53,35,53,51,53,33,50,22,23,51,21,35,23,7,51,21,35,6,33,1,39,33,21,33,7,33,21,33,50,1,33,38,35,33,1,-91,-64,-58,-58,-58,-58,2,25,-79,-21,54,-20,
		-61,3,2,-62,-27,107,-2,-116,1,68,4,-3,109,2,-107,63,-3,-86,1,89,-84,-3,-5,2,74,84,-98,-2,-88,2,58,-3,-58,3,48,-105,94,-105,-12,-124,112,-105,50,44,-105,-10,1,-73,52,94,-105,89,1,-27,86,0,0,1,0,42,0,0,3,-8,5,-80,0,26,0,105,0,-80,0,69,88,-80,25,47,27,-79,25,30,62,89,-80,0,69,88,-80,12,
		47,27,-79,12,18,62,89,-80,25,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-80,24,16,-80,20,-48,-80,20,47,-80,3,-48,-80,20,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,19,16,-80,14,-48,-80,14,47,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,13,9,14,17,18,57,48,49,1,35,22,23,51,
		7,35,6,6,35,1,21,35,1,39,51,54,54,55,33,55,33,38,39,33,55,33,3,-54,-20,64,17,-55,46,-104,18,-10,-37,1,-19,-29,-3,-18,1,-7,125,-100,21,-3,-67,46,2,19,48,-10,-2,-25,47,3,-99,5,18,81,117,-98,-78,-76,-3,-60,12,2,105,125,1,107,92,-98,-66,8,-98,0,0,1,0,32,-1,-18,4,26,5,-80,0,30,0,-112,
		0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,19,17,5,17,18,57,-80,19,47,-80,23,-48,-80,23,47,-78,0,23,1,93,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,25,-48,-80,8,-48,-80,9,-48,-80,23,16,-80,22,-48,-80,11,-48,-80,10,-48,-80,19,16,-79,20,1,-80,10,43,
		88,33,-40,27,-12,89,-80,21,-48,-80,12,-48,-80,13,-48,-80,19,16,-80,18,-48,-80,15,-48,-80,14,-48,-80,5,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,30,5,17,17,18,57,-80,30,47,48,49,1,21,6,2,4,35,34,39,17,7,53,55,53,7,53,55,17,51,17,55,21,7,21,55,21,7,17,54,18,17,53,4,26,2,-112,-2,
		-9,-81,80,108,-12,-12,-12,-12,-64,-5,-5,-5,-5,-66,-55,3,3,100,-46,-2,-57,-90,18,2,90,111,-78,111,-103,111,-78,111,1,89,-2,-1,115,-78,115,-103,115,-78,115,-3,-34,2,1,16,1,9,88,0,0,1,0,93,0,0,4,-21,4,58,0,23,0,93,-78,0,24,25,17,18,57,0,-80,0,69,88,-80,22,47,27,-79,22,26,62,89,-80,0,69,
		88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,0,10,22,17,18,57,-80,0,47,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,12,-48,-80,0,16,-80,21,-48,48,49,1,22,0,17,21,35,53,38,2,39,17,35,17,6,2,7,21,35,53,18,0,
		55,53,51,2,-1,-25,1,5,-71,2,-98,-109,-71,-113,-97,2,-71,3,1,7,-33,-71,3,113,33,-2,-115,-2,-38,-73,-56,-33,1,5,32,-3,52,2,-54,33,-2,-11,-40,-58,-59,1,29,1,109,34,-55,0,0,2,0,31,0,0,5,3,5,-80,0,22,0,31,0,112,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,88,-80,3,47,27,
		-79,3,18,62,89,-78,6,3,12,17,18,57,-80,6,47,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-80,6,16,-80,10,-48,-80,10,47,-76,15,10,31,10,2,93,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,20,-48,-80,6,16,-80,21,-48,-80,10,16,-80,23,-48,-80,12,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,
		1,33,17,35,17,35,53,51,53,35,53,51,17,33,50,4,21,20,4,7,33,21,33,1,33,50,54,53,52,38,39,33,2,-4,-2,-79,-65,-49,-49,-49,-49,2,25,-22,1,18,-2,-7,-14,-2,-93,1,79,-2,-79,1,90,-101,-94,-88,-113,-2,-96,1,19,-2,-19,1,19,-98,-119,-99,2,-39,-18,-53,-43,-25,1,-119,1,38,-110,-116,127,-99,1,0,0,4,
		0,122,-1,-21,5,-125,5,-59,0,27,0,39,0,53,0,57,0,-69,-78,28,58,59,17,18,57,-80,28,16,-80,0,-48,-80,28,16,-80,40,-48,-80,28,16,-80,56,-48,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,-80,37,47,27,-79,37,18,62,89,-80,10,16,-80,3,-48,-80,3,47,-78,14,10,3,17,18,57,-74,42,14,58,
		14,74,14,3,93,-80,10,16,-79,17,4,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,4,-80,10,43,88,33,-40,27,-12,89,-78,27,3,10,17,18,57,-76,54,27,70,27,2,93,-78,37,27,1,93,-80,37,16,-80,31,-48,-80,31,47,-80,37,16,-79,43,4,-80,10,43,88,33,-40,27,-12,89,-80,31,16,-79,50,4,-80,10,43,88,33,-40,
		27,-12,89,-78,54,37,10,17,18,57,-80,54,47,-78,56,10,37,17,18,57,-80,56,47,48,49,1,20,6,35,34,38,53,53,52,54,51,50,22,21,35,52,38,35,34,6,21,21,20,22,51,50,54,53,1,52,54,32,22,21,21,20,6,32,38,53,23,20,22,51,50,54,53,53,52,38,35,34,6,21,5,39,1,23,2,-88,-104,123,122,-95,-98,
		123,121,-100,-118,73,66,65,77,79,65,61,76,1,16,-89,1,6,-88,-89,-2,-4,-86,-118,88,74,72,86,87,73,71,89,-2,6,105,2,-57,105,4,30,110,-112,-88,-119,71,-126,-85,-111,111,58,77,102,82,73,78,101,76,58,-3,71,-125,-87,-88,-117,71,-125,-87,-89,-117,6,79,101,99,85,74,79,100,99,84,-13,66,4,114,66,0,0,2,0,104,-1,-21,
		3,106,6,19,0,23,0,33,0,103,-78,19,34,35,17,18,57,-80,19,16,-80,24,-48,0,-80,0,69,88,-80,12,47,27,-79,12,32,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,6,12,0,17,18,57,-80,6,47,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,19,-48,-80,0,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,
		-80,6,16,-80,24,-48,-80,12,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,38,53,6,35,53,50,55,17,54,54,51,50,22,21,21,20,2,7,21,20,22,51,3,54,54,53,53,52,38,35,34,7,2,-52,-62,-46,98,110,113,95,1,-99,-123,120,-105,-50,-85,107,112,-37,89,103,48,38,103,3,21,-22,-21,28,-80,35,2,36,-78,
		-58,-83,-109,37,-63,-2,-113,107,98,-102,-115,2,99,85,-11,123,39,82,76,-47,0,4,0,-94,0,0,7,-58,5,-64,0,3,0,16,0,30,0,40,0,-90,-78,31,41,42,17,18,57,-80,31,16,-80,1,-48,-80,31,16,-80,4,-48,-80,31,16,-80,17,-48,0,-80,0,69,88,-80,39,47,27,-79,39,30,62,89,-80,0,69,88,-80,37,47,27,-79,37,30,
		62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,34,47,27,-79,34,18,62,89,-80,0,69,88,-80,32,47,27,-79,32,18,62,89,-80,7,16,-80,13,-48,-80,2,-48,-80,2,47,-78,16,2,1,93,-79,1,3,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,20,3,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,27,
		3,-80,10,43,88,33,-40,27,-12,89,-78,33,37,32,17,18,57,-78,38,32,37,17,18,57,48,49,1,33,53,33,1,52,54,32,22,21,21,20,6,35,34,38,53,23,20,22,51,50,54,55,53,52,38,35,34,6,21,1,35,1,17,35,17,51,1,17,51,7,-92,-3,-103,2,103,-3,117,-70,1,56,-69,-71,-100,-98,-70,-93,95,86,84,93,1,95,
		85,84,95,-2,-68,-52,-3,-81,-71,-53,2,84,-73,1,-100,-114,2,61,-101,-66,-69,-93,93,-99,-70,-69,-95,5,98,107,106,96,101,97,107,107,99,-5,-101,4,110,-5,-110,5,-80,-5,-113,4,113,0,0,2,0,103,3,-105,4,56,5,-80,0,12,0,20,0,110,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,9,47,27,-79,9,
		30,62,89,-80,0,69,88,-80,19,47,27,-79,19,30,62,89,-78,1,21,6,17,18,57,-80,1,47,-78,0,9,1,17,18,57,-78,3,1,6,17,18,57,-80,4,-48,-78,8,1,9,17,18,57,-80,1,16,-80,11,-48,-80,6,16,-80,13,-80,10,43,88,-40,27,-36,89,-80,1,16,-80,15,-48,-80,13,16,-80,17,-48,-80,18,-48,48,49,1,3,35,3,
		17,35,17,51,19,19,51,17,35,1,35,17,35,17,35,53,33,3,-34,-116,52,-116,90,112,-112,-112,112,90,-2,11,-109,91,-108,1,-126,5,33,-2,118,1,-119,-2,119,2,25,-2,113,1,-113,-3,-25,1,-56,-2,56,1,-56,81,0,2,0,-104,-1,-20,4,-109,4,78,0,21,0,28,0,101,-78,2,29,30,17,18,57,-80,2,16,-80,22,-48,0,-80,0,
		69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,26,10,2,17,18,57,-80,26,47,-79,15,10,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,19,10,-80,10,43,88,33,-40,27,-12,89,-78,21,10,2,17,18,57,-80,10,16,-79,22,10,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,35,34,38,
		2,53,52,18,54,51,50,22,22,23,21,33,17,22,51,50,55,1,34,7,17,33,17,38,4,22,-73,-69,-111,-12,-121,-112,-8,-124,-123,-29,-124,3,-3,0,119,-102,-60,-84,-2,-112,-105,122,2,28,115,94,114,-99,1,1,-109,-113,1,3,-97,-117,-13,-112,62,-2,-72,110,122,3,42,122,-2,-21,1,30,113,0,-1,-1,0,84,-1,-11,5,-77,5,-101,0,39,
		1,-58,-1,-38,2,-122,0,39,1,-108,0,-26,0,0,1,7,2,36,3,20,0,0,0,16,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,48,49,-1,-1,0,100,-1,-11,6,83,5,-76,0,39,2,31,0,38,2,-108,0,39,1,-108,1,-91,0,0,1,7,2,36,3,-76,0,0,0,16,0,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,
		48,49,-1,-1,0,99,-1,-11,6,73,5,-92,0,39,2,33,0,8,2,-113,0,39,1,-108,1,-125,0,0,1,7,2,36,3,-86,0,0,0,16,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,48,49,-1,-1,0,89,-1,-11,5,-3,5,-92,0,39,2,35,0,31,2,-113,0,39,1,-108,1,32,0,0,1,7,2,36,3,94,0,0,0,16,
		0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,48,49,0,2,0,106,-1,-21,4,50,5,-20,0,27,0,42,0,94,-78,21,43,44,17,18,57,-80,21,16,-80,35,-48,0,-80,13,47,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-78,0,13,21,17,18,57,-80,0,47,-78,3,0,21,17,18,57,-80,13,16,-79,7,1,-80,10,43,88,33,
		-40,27,-12,89,-80,0,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,50,22,23,46,2,35,34,7,39,55,54,51,32,0,17,21,20,2,6,35,34,0,53,53,52,0,23,34,6,21,21,20,22,51,50,54,53,53,39,38,38,2,60,93,-90,58,14,105,-90,96,-127,-101,16,
		49,116,-105,1,7,1,31,120,-34,-112,-38,-2,-8,1,0,-28,-116,-97,-97,-118,-114,-97,4,28,-96,3,-2,77,68,-116,-39,121,59,-105,21,48,-2,78,-2,110,50,-68,-2,-42,-91,1,35,-10,14,-36,1,16,-104,-69,-96,16,-86,-49,-7,-37,61,15,90,106,0,1,0,-87,-1,43,4,-27,5,-80,0,7,0,40,0,-80,4,47,-80,0,69,88,-80,6,47,27,
		-79,6,30,62,89,-80,4,16,-80,1,-48,-80,6,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,35,17,33,17,35,17,33,4,-27,-71,-3,54,-71,4,60,-43,5,-19,-6,19,6,-123,0,0,1,0,69,-2,-13,4,-85,5,-80,0,12,0,55,0,-80,3,47,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,3,16,-79,2,1,-80,
		10,43,88,33,-40,27,-12,89,-80,5,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,7,-48,48,49,1,1,33,21,33,53,1,1,53,33,21,33,1,3,107,-3,-69,3,-123,-5,-102,2,97,-3,-97,4,25,-4,-57,2,70,2,65,-3,74,-104,-113,2,-52,2,-46,-112,-104,-3,66,0,1,0,-88,2,-117,3,-21,3,34,0,3,0,28,
		0,-80,0,69,88,-80,2,47,27,-79,2,24,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,3,-21,-4,-67,3,67,2,-117,-105,0,0,1,0,63,0,0,4,-104,5,-80,0,8,0,61,-78,3,9,10,17,18,57,0,-80,7,47,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,
		18,62,89,-78,0,1,3,17,18,57,-80,7,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,1,51,1,35,3,35,53,33,2,48,1,-85,-67,-3,-30,-115,-11,-71,1,59,1,28,4,-108,-6,80,2,116,-102,0,0,3,0,98,-1,-21,7,-53,4,78,0,28,0,44,0,60,0,113,-78,7,61,62,17,18,57,-80,7,16,-80,36,-48,-80,
		7,16,-80,52,-48,0,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,19,-48,-80,19,47,-80,25,-48,-80,25,47,-78,7,25,4,17,18,57,-78,22,25,4,17,18,57,-80,10,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,41,1,-80,10,43,88,33,-40,27,-12,89,-80,48,
		-48,-80,32,16,-80,57,-48,48,49,1,20,2,6,35,34,38,39,6,6,35,34,38,2,53,53,52,18,54,51,50,22,23,54,54,51,50,0,21,5,20,22,51,50,54,55,55,53,46,2,35,34,6,21,37,52,38,35,34,6,7,7,21,30,2,51,50,54,53,7,-53,126,-33,-119,-111,-18,80,81,-20,-112,-119,-34,-128,126,-33,-120,-111,-19,81,80,-17,
		-110,-50,1,22,-7,80,-90,-120,114,-71,52,11,24,114,-110,80,-122,-90,5,-9,-90,-123,115,-68,53,9,22,117,-112,80,-120,-91,2,15,-109,-1,0,-111,-72,-79,-77,-74,-113,1,0,-105,24,-109,1,0,-110,-73,-77,-79,-71,-2,-63,-13,13,-79,-36,-68,-93,39,42,99,-64,97,-36,-71,8,-82,-33,-67,-88,31,42,97,-59,96,-34,-72,0,1,-1,-80,-2,75,2,-114,
		6,21,0,21,0,63,-78,2,22,23,17,18,57,0,-80,0,69,88,-80,14,47,27,-79,14,32,62,89,-80,0,69,88,-80,3,47,27,-79,3,20,62,89,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,20,6,35,34,39,55,22,51,50,53,17,52,54,51,50,23,7,38,35,34,
		21,1,101,-92,-98,57,58,18,46,33,-101,-79,-95,60,84,24,37,54,-74,107,-94,-88,20,-111,13,-79,5,25,-86,-66,21,-114,11,-37,0,2,0,101,1,24,4,11,3,-12,0,21,0,43,0,-111,-78,28,44,45,17,18,57,-80,28,16,-80,5,-48,0,-80,3,47,-78,15,3,1,93,-80,13,-48,-80,13,47,-78,0,13,1,93,-79,8,1,-80,10,43,88,
		33,-40,27,-12,89,-80,3,16,-80,10,-48,-80,10,47,-80,3,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-80,21,-48,-80,21,47,-80,13,16,-80,25,-48,-80,25,47,-80,35,-48,-80,35,47,-78,0,35,1,93,-79,30,1,-80,10,43,88,33,-40,27,-12,89,-80,25,16,-80,32,-48,-80,32,47,-80,25,16,-79,40,1,-80,10,43,88,33,
		-40,27,-12,89,-80,35,16,-80,43,-48,-80,43,47,48,49,19,54,54,51,54,23,23,22,51,50,55,21,6,35,34,39,39,38,7,34,6,7,7,54,54,51,54,23,23,22,51,50,55,23,6,35,34,39,39,38,7,34,6,7,102,48,-125,66,82,74,-104,66,78,-122,102,103,-123,78,66,-95,68,79,66,-125,48,1,48,-126,66,82,74,-107,68,80,-123,
		102,1,103,-123,78,66,-104,74,82,66,-125,48,3,-123,51,58,2,35,78,31,-128,-66,109,31,83,31,2,68,60,-27,51,59,2,35,77,33,-128,-67,109,31,78,35,2,68,60,0,0,1,0,-104,0,-101,3,-38,4,-43,0,19,0,57,0,-80,19,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,19,16,-80,7,-48,-80,19,16,-80,15,
		-48,-80,15,47,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,-80,15,16,-80,11,-48,48,49,1,33,7,39,55,35,53,33,55,33,53,33,19,23,7,51,21,33,7,33,3,-38,-3,-19,-114,95,108,-82,1,11,-107,-2,96,1,-2,-103,95,119,-61,-2,-33,-108,1,-75,1,-113,-12,59,-71,-96,-1,-95,1,6,59,-53,-95,-1,0,-1,-1,0,62,
		0,2,3,-127,4,61,0,102,0,32,0,97,64,0,57,-102,1,7,1,-81,-1,-106,-3,119,0,29,0,-80,0,69,88,-80,5,47,27,-79,5,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,48,49,0,-1,-1,0,-123,0,1,3,-36,4,80,0,102,0,34,0,115,64,0,57,-102,1,7,1,-81,-1,-35,-3,118,0,29,0,-80,0,69,
		88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,48,49,0,0,2,0,43,0,0,3,-36,5,-80,0,5,0,9,0,56,-78,8,10,11,17,18,57,-80,8,16,-80,1,-48,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,6,0,3,17,18,57,-78,
		8,0,3,17,18,57,48,49,1,51,1,1,35,9,4,1,-68,-116,1,-108,-2,112,-115,-2,108,1,-42,-2,-23,1,28,1,24,5,-80,-3,39,-3,41,2,-41,2,15,-3,-15,-3,-14,2,14,0,-1,-1,0,-75,0,-89,1,-101,4,-11,0,39,0,18,0,37,0,-78,0,7,0,18,0,37,4,36,0,2,0,110,2,121,2,51,4,58,0,3,0,7,
		0,44,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,2,16,-80,0,-48,-80,0,47,-80,4,-48,-80,5,-48,48,49,19,35,17,51,1,35,17,51,-5,-115,-115,1,56,-115,-115,2,121,1,-63,-2,63,1,-63,0,0,1,0,92,-1,95,1,87,0,-17,0,8,0,32,-78,8,9,10,17,18,
		57,0,-80,9,47,-80,4,-48,-80,4,47,-76,64,4,80,4,2,93,-80,0,-48,-80,0,47,48,49,23,39,54,55,53,51,21,20,6,-59,105,72,2,-79,79,-95,72,109,127,92,76,91,-77,0,-1,-1,0,60,0,0,4,-10,6,21,0,38,0,74,0,0,0,7,0,74,2,44,0,0,0,2,0,31,0,0,3,-51,6,21,0,21,0,25,0,-123,
		-78,8,26,27,17,18,57,-80,8,16,-80,23,-48,0,-80,0,69,88,-80,8,47,27,-79,8,32,62,89,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,-80,0,69,88,-80,24,47,27,-79,24,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,22,47,27,-79,22,18,
		62,89,-80,3,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,19,-48,-80,20,-48,48,49,51,17,35,53,51,53,52,54,51,50,23,7,38,35,34,6,21,21,51,21,35,17,33,35,17,51,-54,-85,-85,-49,-67,112,-85,31,125,113,119,105,-35,-35,2,73,-70,-70,3,-85,-113,
		92,-75,-54,61,-100,50,107,107,94,-113,-4,85,4,58,0,1,0,60,0,0,3,-23,6,21,0,22,0,94,0,-80,0,69,88,-80,18,47,27,-79,18,32,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,0,69,88,-80,22,47,27,-79,22,18,62,89,-80,18,16,-79,2,1,-80,10,43,
		88,33,-40,27,-12,89,-80,6,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,11,-48,-80,6,16,-80,14,-48,48,49,1,38,35,34,21,21,51,21,35,17,35,17,35,53,51,53,54,54,51,50,5,17,35,3,48,124,76,-56,-25,-25,-71,-85,-85,1,-64,-79,101,1,43,-71,5,99,20,-46,107,-113,-4,85,3,-85,-113,118,-83,-72,61,-6,40,0,
		0,2,0,60,0,0,6,50,6,21,0,39,0,43,0,-97,0,-80,0,69,88,-80,22,47,27,-79,22,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,32,62,89,-80,0,69,88,-80,32,47,27,-79,32,26,62,89,-80,0,69,88,-80,18,47,27,-79,18,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,42,47,27,
		-79,42,26,62,89,-80,0,69,88,-80,41,47,27,-79,41,18,62,89,-80,0,69,88,-80,35,47,27,-79,35,18,62,89,-80,0,69,88,-80,39,47,27,-79,39,18,62,89,-80,32,16,-79,33,1,-80,10,43,88,33,-40,27,-12,89,-80,37,-48,-80,1,-48,-80,8,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,27,-48,48,49,51,17,35,53,51,
		53,52,54,51,50,23,7,38,35,34,6,21,21,33,53,52,54,51,50,23,7,38,35,34,6,21,21,51,21,35,17,35,17,33,17,33,35,17,51,-25,-85,-85,-70,-86,64,63,10,47,53,90,98,1,-112,-49,-67,112,-85,31,125,114,119,105,-34,-34,-71,-2,112,4,-110,-71,-71,3,-85,-113,111,-82,-66,17,-106,9,105,98,114,92,-75,-54,61,-100,50,106,
		108,94,-113,-4,85,3,-85,-4,85,4,58,0,0,1,0,60,0,0,6,50,6,21,0,40,0,108,0,-80,0,69,88,-80,8,47,27,-79,8,32,62,89,-80,0,69,88,-80,33,47,27,-79,33,26,62,89,-80,0,69,88,-80,40,47,27,-79,40,18,62,89,-80,33,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,-80,38,-48,-80,1,-48,-80,33,16,
		-80,18,-48,-80,4,-48,-80,8,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,22,-48,-80,40,16,-80,37,-48,-80,26,-48,-80,13,16,-80,29,-48,48,49,51,17,35,53,51,53,52,54,51,50,23,7,38,35,34,6,21,21,33,53,54,54,51,50,5,17,35,17,38,35,34,21,21,51,21,35,17,35,17,33,17,-25,-85,-85,-70,-86,
		64,63,10,47,53,90,98,1,-112,1,-64,-79,101,1,43,-71,124,76,-56,-25,-25,-71,-2,112,3,-85,-113,111,-82,-66,17,-106,9,105,98,114,118,-83,-72,61,-6,40,5,99,20,-46,107,-113,-4,85,3,-85,-4,85,0,1,0,60,-1,-20,4,-101,6,21,0,38,0,118,0,-80,0,69,88,-80,33,47,27,-79,33,32,62,89,-80,0,69,88,-80,29,47,27,
		-79,29,26,62,89,-80,0,69,88,-80,24,47,27,-79,24,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,29,16,-80,16,-48,-80,37,-48,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,14,-48,-80,33,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,14,
		16,-80,26,-48,48,49,1,35,17,20,22,51,50,55,23,6,35,34,38,53,17,35,53,51,17,38,39,39,34,21,17,35,17,35,53,51,53,52,54,51,50,22,23,17,51,4,-106,-54,54,65,35,52,1,73,70,124,126,-59,-59,61,102,24,-73,-71,-85,-85,-77,-96,93,-37,90,-54,3,-85,-3,97,65,65,12,-106,20,-106,-118,2,-97,-113,1,31,28,7,
		1,-35,-5,96,3,-85,-113,112,-83,-66,57,44,-2,-118,0,1,0,95,-1,-20,6,84,6,17,0,76,0,-51,-78,22,77,78,17,18,57,0,-80,0,69,88,-80,71,47,27,-79,71,32,62,89,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-80,0,69,88,-80,75,47,27,-79,75,26,62,89,-80,0,69,88,-80,64,47,27,-79,64,26,62,89,-80,0,
		69,88,-80,9,47,27,-79,9,18,62,89,-80,0,69,88,-80,44,47,27,-79,44,18,62,89,-80,75,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,13,-48,-80,71,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-78,29,64,44,17,18,57,-80,64,16,-79,32,1,-80,10,
		43,88,33,-40,27,-12,89,-78,58,44,64,17,18,57,-80,58,16,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,49,44,64,17,18,57,-80,44,16,-79,52,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,20,51,50,55,23,6,35,34,38,53,17,35,53,51,53,52,38,35,34,6,21,20,30,2,21,35,52,38,35,34,6,21,20,22,
		4,22,22,21,20,6,35,34,38,38,53,51,22,22,51,50,54,53,52,38,36,38,38,53,52,54,51,50,23,38,53,52,54,51,50,22,21,21,51,6,79,-54,119,35,52,1,77,66,118,-124,-68,-68,102,98,88,92,31,37,30,-70,-127,98,101,114,106,1,21,-84,83,-24,-71,-126,-56,113,-71,5,-117,114,105,127,113,-2,-25,-91,79,-31,-81,96,86,44,
		-54,-101,-71,-55,-54,3,-85,-3,126,-97,12,-106,20,-90,-105,2,-126,-113,85,114,117,88,70,59,105,112,124,76,76,110,88,71,67,68,62,86,121,87,-111,-81,92,-91,96,93,109,85,71,75,83,60,84,116,80,-123,-72,30,110,82,124,-91,-57,-61,77,0,0,22,0,91,-2,114,7,-18,5,-82,0,13,0,26,0,40,0,55,0,61,0,67,0,73,0,79,
		0,86,0,90,0,94,0,98,0,102,0,106,0,110,0,118,0,122,0,126,0,-126,0,-122,0,-118,0,-114,1,-58,-78,16,-113,-112,17,18,57,-80,16,16,-80,0,-48,-80,16,16,-80,27,-48,-80,16,16,-80,48,-48,-80,16,16,-80,60,-48,-80,16,16,-80,62,-48,-80,16,16,-80,70,-48,-80,16,16,-80,74,-48,-80,16,16,-80,80,-48,-80,16,16,-80,87,
		-48,-80,16,16,-80,91,-48,-80,16,16,-80,97,-48,-80,16,16,-80,99,-48,-80,16,16,-80,103,-48,-80,16,16,-80,109,-48,-80,16,16,-80,112,-48,-80,16,16,-80,119,-48,-80,16,16,-80,123,-48,-80,16,16,-80,127,-48,-80,16,16,-80,-124,-48,-80,16,16,-80,-120,-48,-80,16,16,-80,-116,-48,0,-80,61,47,-80,0,69,88,-80,70,47,27,-79,70,30,62,89,
		-78,126,73,3,43,-78,122,123,3,43,-78,-126,119,3,43,-78,127,58,3,43,-78,10,61,70,17,18,57,-80,10,47,-80,3,-48,-80,3,47,-80,14,-48,-80,14,47,-80,10,16,-80,15,-48,-80,15,47,-78,80,14,15,17,18,57,-80,80,47,-79,111,7,-80,10,43,88,33,-40,27,-12,89,-78,21,80,111,17,18,57,-80,10,16,-79,30,7,-80,10,43,88,
		33,-40,27,-12,89,-80,3,16,-79,37,7,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-80,41,-48,-80,41,47,-80,14,16,-80,46,-48,-80,46,47,-79,52,7,-80,10,43,88,33,-40,27,-12,89,-80,61,16,-79,60,10,-80,10,43,88,33,-40,27,-12,89,-80,61,16,-80,107,-48,-80,103,-48,-80,99,-48,-80,62,-48,-80,60,16,-80,108,-48,-80,104,-48,-80,
		100,-48,-80,63,-48,-80,58,16,-80,65,-48,-80,70,16,-80,96,-48,-80,92,-48,-80,88,-48,-80,75,-48,-79,74,10,-80,10,43,88,33,-40,27,-12,89,-80,90,-48,-80,94,-48,-80,98,-48,-80,71,-48,-80,73,16,-80,78,-48,-80,14,16,-79,81,7,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,118,7,-80,10,43,88,33,-40,27,-12,89,-80,119,16,-80,
		-124,-48,-80,122,16,-80,-123,-48,-80,123,16,-80,-120,-48,-80,126,16,-80,-119,-48,-80,127,16,-80,-116,-48,-80,-126,16,-80,-115,-48,48,49,1,20,6,35,34,38,39,53,52,54,51,50,22,23,19,17,51,50,22,21,20,7,22,22,21,20,35,1,52,38,35,34,6,21,21,20,22,51,50,54,53,1,51,17,20,6,35,34,38,53,51,20,51,50,54,53,
		1,17,51,21,51,21,33,53,51,53,51,17,1,17,33,21,35,21,37,53,33,17,35,53,1,21,51,50,53,52,39,19,53,33,21,33,53,33,21,33,53,33,21,1,53,33,21,33,53,33,21,33,53,33,21,19,51,50,53,52,38,35,35,1,35,53,51,53,35,53,51,17,35,53,51,37,35,53,51,53,35,53,51,17,35,53,51,3,57,-127,
		100,102,-128,2,126,104,101,-128,2,67,-68,98,114,84,50,52,-48,-2,-113,74,65,64,74,74,66,64,73,3,-70,92,105,82,88,109,93,104,41,54,-7,-60,113,-60,5,40,-57,111,-8,109,1,53,-60,5,-20,1,54,111,-4,92,126,103,98,-53,1,22,-3,91,1,21,-3,92,1,20,2,10,1,22,-3,91,1,21,-3,92,1,20,-68,93,118,58,60,93,
		-4,-15,113,113,113,113,113,113,7,34,111,111,111,111,111,111,1,-44,98,121,120,94,117,95,124,120,94,-2,-77,2,37,73,77,84,32,13,70,45,-101,1,72,69,78,78,69,112,69,78,78,69,1,79,-2,-122,78,93,81,83,91,54,44,-4,-55,1,59,-54,113,113,-54,-2,-59,6,31,1,29,116,-87,-87,116,-2,-29,-87,-4,-74,-87,83,82,4,3,74,
		116,116,116,116,116,116,-7,56,113,113,113,113,113,113,3,-60,80,41,30,-2,-45,-4,126,-6,-4,21,-7,126,-4,126,-6,-4,21,-7,0,5,0,92,-3,-43,7,-41,8,115,0,3,0,28,0,32,0,36,0,40,0,82,-77,17,17,16,4,43,-77,4,17,28,4,43,-77,10,17,23,4,43,-80,4,16,-80,29,-48,-80,28,16,-80,30,-48,0,-80,33,47,
		-80,37,47,-78,28,30,3,43,-80,37,16,-80,0,-48,-80,0,47,-80,33,16,-80,2,-48,-80,2,47,-78,13,0,2,17,18,57,-80,13,47,-78,31,30,2,17,18,57,-80,31,47,48,49,9,3,5,52,54,55,54,54,53,52,38,35,34,6,7,51,54,54,51,50,22,21,20,7,6,6,21,23,35,21,51,3,51,21,35,3,51,21,35,4,24,3,
		-65,-4,65,-4,68,4,15,30,36,74,92,-89,-107,-112,-96,2,-53,2,58,43,57,56,93,91,47,-54,-54,-54,75,4,4,2,4,4,6,82,-4,49,-4,49,3,-49,-15,58,58,24,39,-121,74,-128,-105,-117,127,51,52,64,52,95,60,65,92,76,91,-86,-3,76,4,10,-98,4,0,1,0,66,0,0,2,-85,3,32,0,22,0,86,-78,8,23,24,17,18,
		57,0,-80,0,69,88,-80,14,47,27,-79,14,24,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-79,21,2,-80,10,43,88,33,-40,27,-12,89,-80,2,-48,-78,20,21,14,17,18,57,-78,3,14,20,17,18,57,-80,14,16,-79,8,2,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-80,11,-48,48,49,33,33,53,1,54,53,52,38,35,34,
		6,21,35,52,54,32,22,21,20,15,2,33,2,-85,-3,-87,1,44,109,64,60,75,71,-99,-89,1,8,-102,107,84,-80,1,-113,108,1,26,102,69,49,61,76,57,114,-108,127,110,104,107,79,-111,0,1,0,122,0,0,1,-17,3,21,0,6,0,54,0,-80,0,69,88,-80,5,47,27,-79,5,24,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,
		89,-78,4,5,1,17,18,57,-80,4,47,-79,3,2,-80,10,43,88,33,-40,27,-12,89,-80,2,-48,48,49,33,35,17,7,53,37,51,1,-17,-99,-40,1,99,18,2,89,57,-128,117,0,0,2,0,80,-1,-11,2,-99,3,32,0,13,0,23,0,72,-78,3,24,25,17,18,57,-80,3,16,-80,16,-48,0,-80,0,69,88,-80,10,47,27,-79,10,24,62,
		89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,10,16,-79,16,2,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,21,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,35,34,38,39,53,52,54,51,50,22,23,39,52,35,34,7,21,20,51,50,55,2,-99,-104,-115,-117,-100,1,-101,-117,-115,-104,2,-99,-118,-123,4,-117,-124,4,1,
		69,-94,-82,-84,-96,-114,-93,-82,-84,-99,7,-64,-76,-77,-62,-75,0,2,0,85,-1,-6,3,-102,4,-99,0,19,0,32,0,84,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,2,16,8,17,18,57,-80,2,47,-80,16,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,20,1,
		-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,6,35,34,38,53,52,54,51,50,22,21,21,16,0,5,35,53,51,36,3,50,54,55,53,52,38,35,34,6,21,20,22,2,-33,101,-85,-82,-52,-27,-70,-58,-32,-2,-52,-2,-44,41,35,1,-108,-41,79,-125,30,-124,105,104,127,124,1,-20,110,-41,
		-80,-76,-28,-2,-30,63,-2,-63,-2,-64,5,-104,7,1,120,79,64,66,-124,-98,-113,108,109,-117,0,3,0,96,-1,-16,3,-83,4,-99,0,21,0,33,0,44,0,101,0,-80,0,69,88,-80,19,47,27,-79,19,28,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,42,-48,-80,42,47,-78,-33,42,1,93,-78,31,42,1,93,-79,25,1,-80,10,
		43,88,33,-40,27,-12,89,-78,3,42,25,17,18,57,-78,14,25,42,17,18,57,-80,9,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,37,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,22,22,21,20,6,32,38,53,52,54,55,38,38,53,52,54,32,22,3,52,38,35,34,6,21,20,22,51,50,54,3,52,38,
		35,34,6,21,20,22,50,54,3,-112,99,85,98,115,-24,-2,-124,-23,113,98,85,96,-42,1,98,-38,-100,-125,108,107,-128,127,110,109,-128,30,116,93,94,110,111,-66,112,3,90,86,-121,38,38,-109,98,-105,-75,-77,-103,99,-110,39,38,-122,86,-108,-81,-81,-3,88,86,110,108,88,91,100,103,2,101,78,100,97,81,80,98,99,0,1,0,66,0,0,3,-64,
		4,-115,0,6,0,58,-78,1,7,8,17,18,57,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,5,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,0,5,3,17,18,57,48,49,1,1,35,1,33,53,33,3,-64,-3,-24,-61,2,23,-3,70,3,126,4,36,-5,-36,3,-12,-103,0,
		0,2,0,114,-1,-16,3,-69,4,-109,0,21,0,32,0,101,-78,7,33,34,17,18,57,-80,7,16,-80,22,-48,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-80,0,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,8,14,0,17,18,57,-80,8,47,-78,5,8,14,17,18,57,-79,22,
		1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,21,35,6,6,7,54,54,51,50,22,21,20,6,35,34,38,53,53,16,0,33,3,34,6,7,21,20,22,50,54,52,38,3,0,30,-56,-32,14,52,-106,78,-82,-55,-33,-66,-62,-22,1,64,1,60,-48,80,-125,32,-119,-46,126,123,4,-109,-100,
		3,-72,-79,57,63,-41,-82,-80,-34,-5,-44,75,1,63,1,74,-3,-40,77,64,40,-118,-92,-123,-40,-122,0,1,0,-128,-1,-16,3,-59,4,-115,0,29,0,107,-78,26,30,31,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,1,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,
		7,1,13,17,18,57,-80,7,47,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,5,7,26,17,18,57,-80,13,16,-80,17,-48,-80,13,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,29,-48,48,49,19,19,33,21,33,3,54,51,50,22,21,20,6,35,34,38,39,51,22,22,51,50,54,53,52,38,35,34,7,7,-92,69,2,
		-88,-3,-12,37,99,115,-72,-41,-33,-60,-85,-22,13,-78,14,-128,98,112,121,-116,115,105,66,41,2,67,2,74,-94,-2,-33,48,-46,-76,-78,-46,-79,-105,91,86,-126,113,106,127,42,27,0,2,0,48,0,0,3,-28,4,-115,0,10,0,14,0,80,-78,14,15,16,17,18,57,-80,14,16,-80,9,-48,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,
		0,69,88,-80,4,47,27,-79,4,18,62,89,-78,1,9,4,17,18,57,-80,1,47,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,1,16,-80,11,-48,-78,13,9,4,17,18,57,48,49,1,51,21,35,17,35,17,33,39,1,51,1,33,17,7,3,53,-81,-81,-70,-3,-72,3,2,66,-61,-3,-63,1,-123,26,1,-99,-105,-2,-6,1,6,
		115,3,20,-3,16,1,-4,47,0,1,0,78,-1,-16,3,-97,4,-99,0,38,0,-113,-78,32,39,40,17,18,57,0,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-78,1,14,25,17,18,57,-80,1,47,-78,-65,1,1,93,-76,-81,1,-65,1,2,113,-76,-33,1,-17,1,2,93,-76,31,1,47,1,
		2,93,-76,111,1,127,1,2,114,-80,14,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-80,10,-48,-80,1,16,-79,37,1,-80,10,43,88,33,-40,27,-12,89,-78,20,37,1,17,18,57,-80,25,16,-80,29,-48,-80,25,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,50,54,53,52,38,35,34,6,21,35,52,54,51,
		50,22,21,20,6,7,22,21,20,6,35,34,38,53,51,20,22,51,50,54,53,52,33,35,1,96,122,118,-127,108,112,98,127,-71,-26,-77,-68,-38,101,91,-43,-23,-63,-67,-22,-71,-125,108,112,127,-2,-20,113,2,-101,99,84,83,96,91,77,-116,-76,-81,-100,79,-119,37,64,-47,-102,-70,-77,-106,79,99,98,91,-61,0,0,1,0,78,0,0,3,-54,4,-99,
		0,24,0,86,-78,9,25,26,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,2,-48,-78,3,16,0,17,18,57,-80,16,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-80,12,-48,-78,22,0,16,17,18,57,48,49,
		33,33,53,1,54,54,53,52,38,35,34,6,21,35,52,54,51,50,22,21,20,6,7,1,33,3,-54,-4,-97,1,-85,103,93,116,94,121,-123,-70,-11,-61,-74,-42,99,-101,-2,-72,2,126,-125,1,-99,94,-117,65,82,105,112,107,-91,-50,-70,-107,81,-82,-95,-2,-23,0,0,1,0,-104,0,0,2,-99,4,-112,0,6,0,65,-78,1,7,8,17,18,57,0,
		-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,5,17,18,57,-80,4,47,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,2,3,5,17,18,57,48,49,33,35,17,5,53,37,51,2,-99,-70,-2,-75,1,-21,26,3,-81,99,-97,-91,0,0,2,0,99,-1,-16,3,-85,4,-99,0,13,
		0,24,0,72,-78,3,25,26,17,18,57,-80,3,16,-80,16,-48,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,10,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,35,34,2,39,53,52,18,51,50,18,23,
		39,16,35,34,17,21,20,22,51,50,17,3,-85,-40,-53,-55,-38,2,-39,-54,-53,-41,3,-70,-21,-22,122,114,-23,1,-15,-8,-2,-9,1,5,-12,-74,-7,1,5,-2,-2,-17,15,1,73,-2,-77,-31,-89,-88,1,83,0,1,0,71,0,0,3,-32,4,-115,0,9,0,70,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,0,69,88,-80,2,47,27,
		-79,2,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,0,2,17,18,57,-80,7,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-78,9,5,7,17,18,57,48,49,37,33,21,33,53,1,33,53,33,21,1,47,2,-79,-4,103,2,-104,-3,113,3,120,-105,-105,124,3,120,-103,121,0,0,1,0,13,0,0,4,28,4,-115,0,8,
		0,49,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,0,1,4,17,18,57,48,49,1,1,51,1,17,35,17,1,51,2,20,1,56,-48,-2,82,-71,-2,88,-48,2,74,2,67,-3,10,-2,105,1,-94,2,-21,0,0,1,0,38,0,0,
		4,49,4,-115,0,11,0,83,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,0,1,4,17,18,57,-78,6,1,4,17,18,57,-78,3,0,6,17,18,57,-78,9,6,0,17,18,57,48,
		49,1,1,51,1,1,35,1,1,35,1,1,51,2,40,1,31,-36,-2,117,1,-103,-36,-2,-43,-2,-40,-36,1,-106,-2,115,-37,2,-38,1,-77,-3,-66,-3,-75,1,-69,-2,69,2,75,2,66,0,0,1,0,49,0,0,5,-15,4,-115,0,18,0,96,-78,14,19,20,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-80,0,69,88,-80,
		8,47,27,-79,8,28,62,89,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,1,3,10,17,18,57,-78,6,3,10,17,18,57,-78,13,3,10,17,18,57,48,49,1,23,55,19,51,19,23,55,19,51,1,35,1,39,7,1,35,1,51,1,
		-81,11,15,-8,-91,-12,13,12,-58,-72,-2,-42,-82,-2,-4,1,1,-2,-12,-83,-2,-41,-73,1,38,80,64,3,119,-4,-122,59,80,3,101,-5,115,3,-107,5,5,-4,107,4,-115,0,0,1,0,20,0,0,4,83,4,-115,0,8,0,49,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,0,69,
		88,-80,5,47,27,-79,5,18,62,89,-78,1,3,5,17,18,57,48,49,1,23,55,1,51,1,35,1,51,2,26,25,26,1,64,-58,-2,55,-83,-2,55,-57,1,36,94,92,3,107,-5,115,4,-115,0,0,1,0,116,-1,-16,4,10,4,-115,0,17,0,61,-78,4,18,19,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,
		88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,13,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,20,6,35,34,38,39,17,51,17,20,22,51,50,54,53,17,4,10,-6,-47,-46,-10,3,-73,-113,-123,-125,-113,4,-115,-4,-12,-74,-37,-45,-74,3,20,-4,-12,121,-127,127,123,3,12,0,0,1,0,40,
		0,0,3,-3,4,-115,0,7,0,47,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,6,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,48,49,1,33,17,35,17,33,53,33,3,-3,-2,113,-71,-2,115,3,-43,3,-12,-4,12,3,-12,-103,0,0,1,0,67,-1,-16,3,-35,
		4,-99,0,37,0,93,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,28,47,27,-79,28,18,62,89,-78,2,28,9,17,18,57,-80,9,16,-80,13,-48,-80,9,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-80,28,16,-80,32,-48,-80,28,16,-79,35,1,-80,10,
		43,88,33,-40,27,-12,89,48,49,1,52,38,36,39,38,53,52,54,51,50,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,6,35,34,36,53,51,20,22,51,50,54,3,35,121,-2,-38,86,-61,-13,-65,-60,-7,-71,-115,121,113,-122,123,1,56,-80,86,-13,-57,-49,-2,-17,-70,-102,-116,126,-126,1,42,80,88,74,43,98,-77,-113,-78,-56,-100,
		98,107,89,80,65,88,80,101,-120,91,-109,-87,-53,-94,102,114,91,0,0,2,0,-118,0,0,4,37,4,-115,0,13,0,22,0,99,-78,21,23,24,17,18,57,-80,21,16,-80,5,-48,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,15,4,
		2,17,18,57,-80,15,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,10,0,4,17,18,57,-80,4,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,50,22,21,20,7,1,21,35,1,51,50,54,53,52,38,35,35,2,90,-2,-23,-71,1,-86,-43,-25,-21,1,32,-58,-3,-28,-10,117,-119,-122,126,-16,1,-63,-2,
		63,4,-115,-70,-86,-28,89,-2,30,10,2,88,109,93,100,110,0,2,0,89,-1,54,4,87,4,-99,0,19,0,33,0,79,-78,8,34,35,17,18,57,-80,8,16,-80,30,-48,0,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,3,8,16,17,18,57,-80,16,16,-79,23,1,-80,10,43,88,33,
		-40,27,-12,89,-80,8,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,23,7,37,6,35,34,0,17,53,52,18,54,51,50,0,17,39,52,38,35,34,6,7,21,20,22,51,50,54,53,4,85,112,102,-40,124,-2,-7,54,70,-28,-2,-27,127,-24,-106,-22,1,21,-73,-84,-100,-108,-84,4,-82,-104,-100,-86,2,36,-90,-13,70,-96,
		111,-57,13,1,49,1,8,62,-87,1,3,-118,-2,-51,-2,-7,6,-58,-46,-49,-71,85,-62,-40,-45,-57,0,2,0,-118,0,0,4,27,4,-115,0,10,0,19,0,79,-78,10,20,21,17,18,57,-80,10,16,-80,12,-48,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-78,11,3,1,17,18,57,-80,
		11,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,50,22,21,20,6,35,37,33,50,54,53,52,38,39,33,1,67,-71,1,-45,-52,-14,-22,-42,-2,-24,1,26,124,-120,-120,119,-2,-31,1,-74,-2,74,4,-115,-57,-88,-86,-66,-104,106,100,96,119,1,0,2,0,96,
		-1,-16,4,90,4,-99,0,13,0,27,0,72,-78,3,28,29,17,18,57,-80,3,16,-80,17,-48,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,10,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,0,35,34,0,
		17,53,16,0,51,50,0,23,7,52,38,35,34,6,21,21,20,22,51,50,54,53,4,90,-2,-20,-24,-27,-2,-25,1,23,-27,-23,1,19,2,-73,-84,-101,-106,-81,-80,-105,-100,-87,2,36,-2,-5,-2,-47,1,50,1,7,62,1,2,1,52,-2,-48,-1,5,-58,-46,-42,-59,66,-61,-41,-45,-57,0,1,0,-118,0,0,4,88,4,-115,0,9,0,69,0,-80,
		0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,2,5,0,17,18,57,-78,7,5,0,17,18,57,48,49,33,35,1,17,35,17,51,1,17,51,4,88,-72,-3,-93,-71,-71,2,93,-72,3,108,-4,
		-108,4,-115,-4,-109,3,109,0,0,1,0,-118,0,0,5,119,4,-115,0,14,0,96,-78,1,15,16,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,12,47,27,
		-79,12,18,62,89,-78,1,0,4,17,18,57,-78,7,0,4,17,18,57,-78,10,0,4,17,18,57,48,49,9,2,51,17,35,17,19,1,35,1,19,17,35,17,1,122,1,-121,1,-123,-15,-72,19,-2,114,-120,-2,115,19,-72,4,-115,-4,113,3,-113,-5,115,1,-111,2,21,-4,90,3,-94,-3,-17,-2,111,4,-115,0,1,0,-118,0,0,3,-117,4,-115,
		0,5,0,41,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,33,21,33,17,51,1,67,2,72,-4,-1,-71,-105,-105,4,-115,0,1,0,-118,0,0,4,87,4,-115,0,12,0,76,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,
		-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,2,8,17,18,57,-78,6,2,4,17,18,57,-78,10,2,8,17,18,57,48,49,1,7,17,35,17,51,17,55,1,51,1,1,35,1,-42,-109,-71,-71,-126,1,-115,-29,-2,33,2,1,-31,2,
		7,-114,-2,-121,4,-115,-3,-43,-112,1,-101,-3,-7,-3,122,0,0,1,0,43,-1,-16,3,77,4,-115,0,15,0,54,-78,5,16,17,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-80,9,-48,-80,5,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,17,20,6,35,
		34,38,53,51,20,22,51,50,54,53,2,-110,-69,-44,-79,-62,-37,-70,113,114,92,110,4,-115,-4,-59,-99,-59,-73,-92,94,102,109,95,0,1,0,-105,0,0,1,81,4,-115,0,3,0,29,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,48,49,33,35,17,51,1,81,-70,-70,4,-115,0,0,1,
		0,-118,0,0,4,88,4,-115,0,11,0,84,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,9,0,10,17,18,57,124,-80,9,47,24,-78,-93,9,1,93,-79,2,1,-80,10,43,88,33,
		-40,27,-12,89,48,49,33,35,17,33,17,35,17,51,17,33,17,51,4,88,-71,-3,-92,-71,-71,2,92,-71,1,-14,-2,14,4,-115,-3,-3,2,3,0,1,0,99,-1,-16,4,53,4,-99,0,29,0,98,-78,10,30,31,17,18,57,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,29,10,3,
		17,18,57,-80,29,47,-78,13,29,10,17,18,57,-80,10,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,29,16,-79,26,3,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,6,35,34,0,39,53,16,0,51,50,22,23,35,38,35,34,6,21,21,20,22,51,50,55,53,33,53,33,
		4,53,66,-23,-105,-18,-2,-32,2,1,11,-14,-56,-14,27,-72,38,-11,-97,-90,-71,-96,-74,81,-2,-25,1,-47,-106,83,83,1,42,-4,90,1,6,1,39,-68,-75,-39,-50,-57,84,-66,-41,74,-18,-112,0,1,0,-118,0,0,3,-101,4,-115,0,9,0,67,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,
		89,-80,9,-48,-80,9,47,-78,31,9,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,21,33,17,33,3,75,-3,-8,-71,3,17,-3,-88,2,8,1,-13,-2,13,4,-115,-103,-2,-104,0,0,1,0,67,-1,19,3,-35,5,115,0,43,0,105,0,-80,0,69,
		88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,34,47,27,-79,34,18,62,89,-78,2,34,9,17,18,57,-80,9,16,-80,12,-48,-80,9,16,-80,16,-48,-80,9,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,34,16,-80,31,-48,-80,34,16,-80,38,-48,-80,34,16,-79,41,1,
		-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,36,39,38,53,52,54,55,53,51,21,22,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,6,7,21,35,53,38,38,53,51,20,22,51,50,54,3,35,121,-2,-38,86,-61,-53,-90,-107,-93,-58,-71,-115,121,113,-122,123,1,56,-80,86,-61,-87,-107,-70,-33,-70,-102,-116,126,-126,1,42,80,
		88,74,43,98,-77,-126,-84,16,-39,-37,21,-62,-120,98,107,89,80,65,88,80,101,-120,91,-126,-90,16,-31,-31,19,-62,-108,102,114,91,0,1,0,48,0,0,3,-17,4,-99,0,32,0,99,0,-80,0,69,88,-80,20,47,27,-79,20,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,15,7,20,17,18,57,-80,15,47,-79,14,4,-80,10,
		43,88,33,-40,27,-12,89,-80,1,-48,-80,7,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,-80,20,16,-80,24,-48,-80,20,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-80,31,-48,48,49,1,33,23,22,7,33,7,33,53,51,54,55,55,39,35,53,51,39,38,54,51,50,22,21,35,52,38,35,34,6,23,23,33,
		3,29,-2,112,1,5,56,2,-108,1,-4,-124,10,79,9,1,1,-92,-96,4,6,-53,-75,-73,-54,-71,104,96,93,104,4,4,1,-108,1,-12,34,-53,111,-104,-104,23,-35,70,34,121,123,-55,-20,-52,-73,112,119,-113,-118,123,0,1,0,13,0,0,3,-110,4,-115,0,23,0,109,-78,0,24,25,17,18,57,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,
		89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,0,12,1,17,18,57,-78,8,1,12,17,18,57,-80,8,47,-80,3,-48,-80,3,47,-80,5,-80,10,43,88,-40,27,-36,89,-80,8,16,-80,10,-80,10,43,88,-40,27,-36,89,-80,14,-48,-80,8,16,-80,16,-48,-80,5,16,-80,18,-48,-80,3,16,-80,20,-48,-80,1,16,-80,22,-48,48,49,1,
		19,51,1,51,21,33,7,21,33,21,33,21,35,53,33,53,33,53,33,53,51,1,51,1,-47,-3,-60,-2,-44,-43,-2,-15,3,1,18,-2,-18,-71,-2,-18,1,18,-2,-18,-37,-2,-44,-57,2,77,2,64,-3,-116,121,7,68,120,-35,-35,120,75,121,2,116,0,0,1,0,-118,0,0,3,-123,4,-115,0,5,0,51,-78,1,6,7,17,18,57,0,-80,0,
		69,88,-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,4,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,33,3,-123,-3,-66,-71,2,-5,3,-12,-4,12,4,-115,0,2,0,20,0,0,4,83,4,-115,0,3,0,8,0,61,-78,5,9,10,17,18,57,-80,5,16,-80,2,-48,0,
		-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,5,2,0,17,18,57,-79,7,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,33,1,51,3,39,7,1,33,4,83,-5,-63,1,-55,-83,61,26,25,-2,-8,2,67,4,-115,-2,-35,92,94,-3,48,0,3,0,96,-1,-16,4,90,4,-99,0,3,
		0,17,0,31,0,97,0,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,2,7,14,17,18,57,124,-80,2,47,24,-76,96,2,112,2,2,113,-76,96,2,112,2,2,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,
		28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,5,16,0,35,34,0,17,53,16,0,51,50,0,23,7,52,38,35,34,6,21,21,20,22,51,50,54,53,3,85,-2,31,1,-31,1,5,-2,-20,-24,-27,-2,-25,1,23,-27,-23,1,19,2,-73,-84,-101,-106,-81,-80,-105,-100,-87,1,-7,-103,110,-2,-5,-2,-47,1,50,1,7,62,1,2,
		1,52,-2,-48,-1,5,-58,-46,-42,-59,66,-61,-41,-45,-57,0,0,1,0,20,0,0,4,83,4,-115,0,8,0,56,-78,7,9,10,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,7,2,0,17,18,57,48,49,51,35,1,51,
		1,35,1,39,7,-37,-57,1,-55,-83,1,-55,-58,-2,-64,26,25,4,-115,-5,115,3,106,92,94,0,0,3,0,62,0,0,3,75,4,-115,0,3,0,7,0,11,0,102,-78,4,12,13,17,18,57,-80,4,16,-80,1,-48,-80,4,16,-80,9,-48,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,
		-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,7,10,0,17,18,57,-80,7,47,-78,-65,7,1,93,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,33,53,33,3,33,53,33,19,33,53,33,3,75,-4,-13,3,13,67,-3,119,2,-119,67,-4,-13,3,13,-104,1,123,-104,1,73,
		-103,0,0,1,0,-118,0,0,4,68,4,-115,0,7,0,64,-78,1,8,9,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,6,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,33,17,35,17,33,4,68,
		-70,-3,-71,-71,3,-70,3,-12,-4,12,4,-115,0,1,0,63,0,0,3,-56,4,-115,0,12,0,69,-78,6,13,14,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,5,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,
		-80,7,-48,48,49,1,1,33,21,33,53,1,1,53,33,21,33,1,2,111,-2,-74,2,-93,-4,119,1,81,-2,-81,3,87,-3,-113,1,74,2,58,-2,95,-103,-112,1,-73,1,-74,-112,-103,-2,95,0,3,0,96,0,0,5,6,4,-115,0,17,0,23,0,30,0,94,0,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,8,47,27,
		-79,8,18,62,89,-78,15,16,8,17,18,57,-80,15,47,-80,0,-48,-78,9,8,16,17,18,57,-80,9,47,-80,6,-48,-80,9,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,27,-48,-80,20,16,-80,28,-48,48,49,1,22,4,21,20,4,7,21,35,53,38,36,53,52,36,55,53,51,
		1,16,5,17,6,6,5,52,38,39,17,54,54,3,16,-26,1,16,-2,-19,-29,-71,-22,-2,-13,1,16,-25,-71,-2,8,1,63,-102,-91,3,54,-90,-104,-104,-90,4,22,13,-6,-53,-51,-4,13,110,110,13,-3,-54,-52,-4,13,118,-3,-75,-2,-40,17,2,114,9,-106,-104,-103,-107,9,-3,-114,10,-106,0,0,1,0,96,0,0,4,-74,4,-115,0,21,0,93,
		-78,0,22,23,17,18,57,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-80,0,69,88,-80,15,47,27,-79,15,28,62,89,-80,0,69,88,-80,20,47,27,-79,20,28,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,19,3,9,17,18,57,-80,19,47,-80,0,-48,-80,19,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,8,
		-48,48,49,1,36,17,17,51,17,6,2,7,17,35,17,38,2,39,17,51,17,16,5,17,51,2,-24,1,21,-71,3,-14,-39,-70,-39,-16,5,-70,1,20,-70,1,-69,51,1,107,1,52,-2,-67,-13,-2,-30,24,-2,-33,1,31,20,1,29,-14,1,75,-2,-53,-2,-114,45,2,-44,0,0,1,0,117,0,0,4,126,4,-99,0,33,0,94,-78,7,34,35,
		17,18,57,0,-80,0,69,88,-80,24,47,27,-79,24,28,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-80,0,69,88,-80,32,47,27,-79,32,18,62,89,-80,15,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,14,-48,-80,0,-48,-80,24,16,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-80,30,-48,-80,31,-48,48,49,
		37,54,54,53,53,52,38,35,34,6,21,21,20,22,23,21,33,53,51,38,17,53,52,0,51,50,0,21,21,16,7,51,21,33,2,-69,-120,127,-82,-99,-100,-84,-115,127,-2,62,-81,-77,1,27,-25,-24,1,28,-78,-75,-2,61,-99,31,-33,-51,38,-77,-64,-63,-73,33,-52,-33,32,-99,-105,-99,1,58,30,-18,1,35,-2,-36,-11,28,-2,-53,-100,-105,0,1,
		0,38,-1,-20,5,44,4,-115,0,25,0,110,-78,22,26,27,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,14,47,27,-79,14,18,62,89,-80,0,69,88,-80,24,47,27,-79,24,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,-78,8,2,14,17,18,57,-80,8,47,
		-80,14,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,21,33,17,54,51,50,22,21,20,6,35,53,50,54,53,52,38,35,34,7,17,35,1,-118,-2,-100,3,-119,-2,-108,-105,-100,-44,-30,-27,-32,-115,127,125,-128,-99,-106,-71,3,-12,-103,-103,-2,-41,49,-48,-60,-66,-66,
		-105,109,120,-125,121,50,-3,-50,0,1,0,96,-1,-16,4,48,4,-99,0,30,0,-128,-78,3,31,32,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,15,11,3,17,18,57,-80,11,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-78,22,11,3,17,18,57,124,-80,22,47,24,
		-78,-96,22,1,93,-76,96,22,112,22,2,93,-78,48,22,1,113,-76,96,22,112,22,2,113,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,30,3,11,17,18,57,48,49,1,6,6,35,34,0,17,53,52,54,54,51,50,22,23,35,38,38,35,34,6,7,33,21,33,22,22,51,50,54,
		55,4,48,20,-4,-47,-32,-2,-15,123,-25,-104,-52,-9,19,-71,18,-115,126,-103,-94,6,1,-65,-2,65,4,-95,-111,-121,-115,20,1,121,-69,-50,1,39,1,3,94,-92,-7,-120,-45,-69,-126,116,-61,-81,-104,-78,-62,111,-125,0,0,2,0,39,0,0,6,-5,4,-115,0,23,0,32,0,122,-78,4,33,34,17,18,57,-80,4,16,-80,24,-48,0,-80,0,69,88,
		-80,18,47,27,-79,18,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,18,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-78,20,18,3,17,18,57,-80,20,47,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,
		-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,33,17,33,3,14,2,7,35,55,55,54,54,19,19,33,17,33,22,22,37,17,33,50,54,53,52,38,35,6,-5,-26,-61,-2,43,-2,94,15,11,77,-105,123,59,4,46,96,81,10,20,3,14,1,36,-63,-32,-3,59,1,21,114,-124,-125,115,1,110,-91,-57,2,3,-12,-2,101,-19,
		-10,117,1,-91,1,4,-66,1,9,2,28,-2,74,4,-63,45,-2,89,117,99,95,112,0,2,0,-118,0,0,7,9,4,-115,0,18,0,27,0,-116,-78,1,28,29,17,18,57,-80,1,16,-80,19,-48,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,
		89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,1,2,11,17,18,57,124,-80,1,47,24,-78,-96,1,1,93,-78,4,2,11,17,18,57,-80,4,47,-80,1,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,
		17,51,17,33,22,22,21,20,6,7,33,17,33,17,35,17,51,1,17,33,50,54,53,52,38,39,1,67,2,72,-71,1,36,-63,-32,-26,-61,-2,43,-3,-72,-71,-71,3,1,1,21,115,-124,125,110,2,-118,2,3,-2,74,4,-63,-92,-91,-57,2,1,-14,-2,14,4,-115,-3,-78,-2,89,119,97,91,113,3,0,1,0,40,0,0,5,46,4,-115,0,21,
		0,92,-78,7,22,23,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,5,-48,-78,8,2,12,17,18,57,-80,8,47,-79,17,1,-80,10,43,88,33,-40,27,
		-12,89,48,49,1,33,53,33,21,33,17,54,51,50,22,23,17,35,17,52,38,35,34,7,17,35,1,-117,-2,-99,3,-119,-2,-108,-109,-96,-44,-34,4,-70,125,127,-99,-106,-70,3,-12,-103,-103,-2,-41,49,-54,-63,-2,-113,1,100,-121,121,50,-3,-50,0,0,1,0,-118,-2,-101,4,67,4,-115,0,11,0,80,-78,3,12,13,17,18,57,0,-80,2,47,-80,
		0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,9,-48,48,49,33,33,17,35,17,33,17,51,17,33,17,51,4,67,-2,-127,-71,-2,127,-71,2,71,
		-71,-2,-101,1,101,4,-115,-4,11,3,-11,0,0,2,0,-118,0,0,4,8,4,-115,0,12,0,21,0,97,-78,3,22,23,17,18,57,-80,3,16,-80,13,-48,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,11,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,3,11,9,17,18,57,
		-80,3,47,-80,9,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,50,22,21,20,6,7,33,17,33,1,50,54,53,52,38,39,33,17,3,-107,-3,-82,1,17,-50,-26,-28,-59,-2,43,3,11,-2,-61,115,-124,125,110,-2,-33,3,-9,-2,-32,-60,-91,-92,-56,2,4,-115,
		-4,11,119,97,91,113,3,-2,89,0,0,2,0,46,-2,-84,4,-25,4,-115,0,15,0,21,0,93,-78,19,22,23,17,18,57,-80,19,16,-80,5,-48,0,-80,9,47,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,7,-48,-80,8,-48,-80,9,16,
		-80,13,-48,-80,8,16,-80,16,-48,-80,17,-48,-80,5,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,48,49,55,55,54,54,55,19,33,17,51,17,35,17,33,17,35,19,33,33,17,33,3,2,-123,41,71,71,7,14,3,7,-113,-71,-4,-70,-70,1,1,46,2,66,-2,100,12,17,-104,49,86,-3,-40,1,-103,-4,11,-2,20,1,84,-2,-83,1,-21,
		3,92,-2,-56,-2,-103,0,1,0,31,0,0,5,-21,4,-115,0,21,0,-110,-78,1,22,23,17,18,57,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,13,47,27,-79,13,28,62,89,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,
		18,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-78,16,9,2,17,18,57,124,-80,16,47,24,-78,-96,16,1,93,-76,96,16,112,16,2,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,19,16,0,17,18,57,-80,19,16,-80,8,-48,-80,16,16,-80,11,-48,48,49,1,35,17,35,17,35,1,35,1,1,51,1,51,17,
		51,17,51,1,51,1,1,35,3,-59,99,-70,100,-2,-59,-22,1,-122,-2,-98,-32,1,44,89,-70,89,1,44,-32,-2,-100,1,-120,-22,1,-10,-2,10,1,-10,-2,10,2,81,2,60,-2,3,1,-3,-2,3,1,-3,-3,-51,-3,-90,0,1,0,71,-1,-16,3,-44,4,-99,0,40,0,-128,-78,36,41,42,17,18,57,0,-80,0,69,88,-80,10,47,27,-79,10,
		28,62,89,-80,0,69,88,-80,22,47,27,-79,22,18,62,89,-80,10,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,6,10,22,17,18,57,-78,39,10,22,17,18,57,-80,39,47,-76,31,39,47,39,2,93,-78,-65,39,1,93,-76,-33,39,-17,39,2,93,-79,36,1,-80,10,43,88,33,-40,27,-12,89,-78,16,36,39,17,18,57,-78,28,22,10,
		17,18,57,-80,22,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,35,34,6,21,35,52,54,51,50,22,21,20,6,7,22,22,21,20,6,35,34,38,39,38,53,51,22,22,51,50,54,53,52,37,35,53,51,54,3,8,-118,125,110,-127,-70,-19,-68,-45,-18,110,103,118,113,-2,-43,91,-87,61,121,-71,5,-125,121,-120,-110,-2,-1,
		-99,-100,-17,3,80,84,93,88,79,-114,-75,-88,-106,86,-115,41,36,-110,91,-98,-76,44,46,89,-99,86,96,96,88,-63,5,-104,5,0,0,1,0,-118,0,0,4,97,4,-115,0,9,0,76,-78,0,10,11,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,0,69,88,-80,2,47,27,
		-79,2,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,4,0,2,17,18,57,-78,9,0,2,17,18,57,48,49,1,51,17,35,17,1,35,17,51,17,3,-88,-71,-71,-3,-101,-71,-71,4,-115,-5,115,3,116,-4,-116,4,-115,-4,-116,0,1,0,-117,0,0,4,44,4,-115,0,12,0,105,-78,10,13,14,17,18,57,0,-80,0,69,88,
		-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,6,2,4,17,18,57,124,-80,6,47,24,-78,-96,6,1,93,-76,96,6,112,6,2,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,10,1,6,17,18,
		57,48,49,1,35,17,35,17,51,17,51,1,51,1,1,35,1,-82,106,-71,-71,100,1,-123,-33,-2,53,1,-21,-17,1,-10,-2,10,4,-115,-2,3,1,-3,-3,-59,-3,-82,0,1,0,39,0,0,4,54,4,-115,0,15,0,79,-78,4,16,17,17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,
		62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,3,2,2,7,35,55,55,54,54,55,19,4,54,-71,-2,94,15,13,-92,-80,68,4,41,94,80,13,25,4,-115,-5,115,3,-12,-2,-126,-2,-86,-2,
		-27,5,-91,3,7,-98,-30,2,94,0,0,1,0,34,-1,-20,4,11,4,-115,0,17,0,68,-78,1,18,19,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,1,8,2,17,18,57,-79,12,1,-80,10,43,88,33,-40,27,-12,89,
		48,49,1,23,1,51,1,7,6,7,7,34,39,55,23,50,54,55,1,51,1,-11,45,1,20,-43,-2,94,37,80,-86,38,80,20,6,92,49,73,32,-2,102,-42,2,48,120,2,-43,-4,69,73,-111,11,1,8,-109,5,49,59,3,-97,0,1,0,-118,-2,-84,4,-15,4,-115,0,11,0,70,-78,9,12,13,17,18,57,0,-80,2,47,-80,0,69,88,-80,
		6,47,27,-79,6,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,-80,9,-48,48,49,37,51,3,35,17,33,17,51,17,33,17,51,4,68,-83,18,-91,-4,80,-71,2,71,-70,-104,-2,20,1,84,4,-115,-4,11,3,-11,0,1,
		0,61,0,0,3,-33,4,-115,0,17,0,71,-78,4,18,19,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,13,8,0,17,18,57,-80,13,47,-79,4,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,35,17,6,35,34,38,
		39,17,51,17,20,22,51,50,55,17,51,3,-33,-71,-112,-93,-44,-34,4,-71,126,127,-99,-106,-71,1,-62,48,-54,-63,1,112,-2,-99,-121,121,50,2,49,0,0,1,0,-118,0,0,5,-58,4,-115,0,11,0,80,-78,5,12,13,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,
		69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,-80,9,-48,48,49,33,33,17,51,17,33,17,51,17,33,17,51,5,-58,-6,-60,-71,1,-120,-70,1,-120,-71,4,-115,-4,11,3,-11,-4,11,3,-11,0,0,1,0,-118,-2,-84,6,117,4,-115,0,15,
		0,89,-78,11,16,17,17,18,57,0,-80,2,47,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,-48,-80,9,-48,-80,12,-48,-80,13,-48,48,
		49,37,51,3,35,17,33,17,51,17,33,17,51,17,33,17,51,5,-57,-82,18,-90,-6,-51,-71,1,-120,-70,1,-120,-70,-104,-2,20,1,84,4,-115,-4,11,3,-11,-4,11,3,-11,0,2,0,8,0,0,4,-42,4,-115,0,13,0,22,0,97,-78,8,23,24,17,18,57,-80,8,16,-80,21,-48,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,
		0,69,88,-80,3,47,27,-79,3,18,62,89,-80,7,16,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-78,10,7,3,17,18,57,-80,10,47,-80,3,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,33,17,33,53,33,17,33,50,22,22,1,50,54,53,52,38,35,
		33,17,4,-42,-28,-60,-2,42,-2,-80,2,10,1,22,-124,-62,104,-2,81,114,-124,-125,115,-2,-21,1,110,-92,-56,2,3,-12,-103,-2,74,88,-93,-2,-68,117,99,95,112,-2,89,0,-1,-1,0,-118,0,0,5,103,4,-115,0,38,2,8,0,0,0,7,1,-29,4,22,0,0,0,2,0,-118,0,0,4,8,4,-115,0,10,0,19,0,82,-78,8,20,21,
		17,18,57,-80,8,16,-80,11,-48,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,8,5,3,17,18,57,-80,8,47,-80,3,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,7,33,17,51,17,33,50,22,1,
		50,54,53,52,38,39,33,17,4,8,-28,-59,-2,43,-71,1,17,-50,-26,-2,80,115,-124,125,110,-2,-33,1,110,-92,-56,2,4,-115,-2,74,-60,-2,-123,119,97,91,113,3,-2,89,0,1,0,75,-1,-16,4,27,4,-99,0,30,0,125,-78,3,31,32,17,18,57,0,-80,0,69,88,-80,19,47,27,-79,19,28,62,89,-80,0,69,88,-80,27,47,27,-79,
		27,18,62,89,-78,0,27,19,17,18,57,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,9,19,27,17,18,57,124,-80,9,47,24,-78,-96,9,1,93,-76,96,9,112,9,2,93,-78,48,9,1,113,-76,96,9,112,9,2,113,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-78,15,19,27,
		17,18,57,48,49,1,22,22,51,50,54,55,33,53,33,38,38,35,34,6,7,35,54,54,51,50,0,23,21,20,6,6,35,34,38,39,1,4,20,-115,-121,-115,-94,7,-2,65,1,-66,5,-93,-104,126,-115,18,-71,19,-9,-52,-28,1,17,5,120,-30,-107,-49,-2,20,1,121,-125,111,-69,-71,-104,-81,-61,116,-126,-69,-45,-2,-33,-12,117,-93,-7,-121,-50,-69,
		0,2,0,-118,-1,-16,6,21,4,-99,0,19,0,33,0,-115,-78,4,34,35,17,18,57,-80,4,16,-80,24,-48,0,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,13,8,11,17,18,57,124,
		-80,13,47,24,-76,96,13,112,13,2,113,-78,-96,13,1,93,-76,96,13,112,13,2,93,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,0,35,34,0,39,35,17,35,17,51,17,51,54,0,51,50,0,23,7,52,38,
		35,34,6,21,21,20,22,51,50,54,53,6,21,-2,-20,-24,-35,-2,-21,12,-40,-71,-71,-40,14,1,20,-38,-23,1,19,2,-73,-84,-101,-106,-81,-80,-105,-100,-87,2,36,-2,-5,-2,-47,1,28,-14,-2,2,4,-115,-2,9,-15,1,22,-2,-48,-1,5,-58,-46,-42,-59,66,-61,-41,-45,-57,0,2,0,80,0,0,3,-4,4,-115,0,13,0,20,0,99,-78,19,
		21,22,17,18,57,-80,19,16,-80,7,-48,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,17,7,0,17,18,57,-80,17,47,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-78,1,11,7,17,18,57,-80,7,16,-79,18,1,-80,10,43,88,
		33,-40,27,-12,89,48,49,51,1,38,38,53,52,54,55,33,17,35,17,33,3,19,20,23,33,17,33,34,80,1,34,122,113,-36,-56,1,-47,-71,-2,-48,-1,46,-26,1,27,-2,-17,-16,2,13,38,-99,104,-95,-78,2,-5,115,1,-33,-2,33,3,48,-76,4,1,124,0,1,0,11,0,0,3,-25,4,-115,0,13,0,82,-78,1,14,15,17,18,57,0,
		-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,13,8,2,17,18,57,-80,13,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,13,16,-80,6,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,17,35,17,35,53,51,17,33,21,33,17,51,2,-121,
		-30,-71,-31,-31,2,-5,-3,-66,-30,1,-3,-2,3,1,-3,-105,1,-7,-103,-2,-96,0,0,1,0,31,-2,-84,6,34,4,-115,0,25,0,-84,-78,8,26,27,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,20,47,27,-79,20,28,62,89,-80,0,69,88,-80,24,47,27,-79,24,28,62,89,-80,0,69,88,-80,13,47,
		27,-79,13,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,23,10,24,17,18,57,124,-80,23,47,24,-78,-96,23,1,93,-76,96,23,112,23,2,93,-76,96,23,112,23,2,113,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-78,0,7,23,17,18,57,-80,5,16,-79,1,1,-80,10,
		43,88,33,-40,27,-12,89,-80,7,16,-80,11,-48,-78,15,23,7,17,18,57,-80,23,16,-80,18,-48,48,49,1,1,51,17,35,17,35,1,35,17,35,17,35,1,35,1,1,51,1,51,17,51,17,51,1,51,4,99,1,38,-103,-89,122,-2,-60,99,-70,100,-2,-59,-22,1,-122,-2,-98,-32,1,44,89,-70,89,1,44,-32,2,90,-2,60,-2,22,1,84,
		1,-10,-2,10,1,-10,-2,10,2,81,2,60,-2,3,1,-3,-2,3,1,-3,0,1,0,-117,-2,-84,4,78,4,-115,0,16,0,-126,-78,0,17,18,17,18,57,0,-80,3,47,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,15,47,27,-79,15,28,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-80,0,69,88,-80,5,
		47,27,-79,5,18,62,89,-78,13,9,11,17,18,57,124,-80,13,47,24,-76,96,13,112,13,2,113,-78,-96,13,1,93,-76,96,13,112,13,2,93,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-78,0,8,13,17,18,57,-80,5,16,-79,1,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,1,51,17,35,17,35,1,35,17,35,17,51,17,51,1,
		51,2,65,1,111,-98,-88,105,-2,113,106,-71,-71,100,1,-123,-33,2,82,-2,68,-2,22,1,84,1,-10,-2,10,4,-115,-2,3,1,-3,0,0,1,0,-117,0,0,4,-25,4,-115,0,20,0,121,-78,11,21,22,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,19,47,27,-79,19,28,62,89,-80,0,69,88,-80,9,
		47,27,-79,9,18,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-78,0,17,19,17,18,57,124,-80,0,47,24,-78,-96,0,1,93,-76,96,0,112,0,2,93,-76,96,0,112,0,2,113,-80,4,-48,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-78,8,16,0,17,18,57,-80,12,-48,48,49,1,51,53,51,21,51,1,51,1,
		1,35,1,35,21,35,53,35,17,35,17,51,1,68,80,-108,60,1,-124,-32,-2,52,1,-21,-17,-2,113,65,-108,80,-71,-71,2,-112,-28,-28,1,-3,-3,-59,-3,-82,1,-10,-50,-50,-2,10,4,-115,0,1,0,35,0,0,5,21,4,-115,0,14,0,127,-78,0,15,16,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,69,88,-80,
		10,47,27,-79,10,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-78,8,2,6,17,18,57,124,-80,8,47,24,-78,-96,8,1,93,-76,96,8,112,8,2,93,-76,96,8,112,8,2,113,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,4,1,-80,10,43,88,33,-40,27,
		-12,89,-78,12,1,8,17,18,57,48,49,1,35,17,35,17,33,53,33,17,51,1,51,1,1,35,2,-105,105,-70,-2,-81,2,11,99,1,-123,-32,-2,52,1,-21,-17,1,-10,-2,10,3,-11,-104,-2,3,1,-3,-3,-59,-3,-82,0,2,0,96,-1,-21,5,91,4,-97,0,35,0,46,0,-104,-78,20,47,48,17,18,57,-80,20,16,-80,36,-48,0,-80,0,
		69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,27,47,27,-79,27,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,2,4,27,17,18,57,-80,2,47,-80,11,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,
		-80,2,16,-79,38,1,-80,10,43,88,33,-40,27,-12,89,-78,21,19,38,17,18,57,-78,33,2,38,17,18,57,-80,27,16,-79,44,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,39,6,35,32,0,17,53,16,18,51,23,34,6,21,21,20,22,51,50,55,38,3,53,52,18,51,50,18,21,21,16,7,22,51,1,16,23,54,17,53,52,38,
		35,34,3,5,91,-39,-90,-119,-93,-2,-22,-2,-58,-12,-46,1,126,-112,-48,-57,54,50,-29,1,-49,-75,-72,-51,-74,94,118,-3,-110,-31,-74,98,106,-58,5,20,59,60,1,69,1,42,26,1,3,1,40,-98,-61,-56,33,-24,-27,8,-78,1,69,39,-21,1,4,-2,-1,-15,56,-2,-38,-78,18,1,-3,-2,-52,121,-127,1,30,56,-84,-93,-2,-61,-1,-1,0,13,
		0,0,4,28,4,-115,0,38,1,-45,0,0,1,7,2,38,0,68,-2,-34,0,8,0,-78,0,10,1,93,48,49,0,1,0,38,-2,-84,4,113,4,-115,0,16,0,108,-78,11,17,18,17,18,57,0,-80,7,47,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,15,47,27,-79,15,28,62,89,-80,0,69,88,-80,9,47,27,-79,
		9,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,0,1,12,17,18,57,-78,11,12,1,17,18,57,-78,3,11,0,17,18,57,-80,9,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,14,0,11,17,18,57,48,49,1,1,51,1,1,53,51,17,35,17,35,1,1,35,1,1,51,2,40,1,31,-36,-2,117,1,49,-88,-88,
		116,-2,-43,-2,-40,-36,1,-106,-2,115,-37,2,-38,1,-77,-3,-66,-2,74,1,-2,22,1,84,1,-69,-2,69,2,75,2,66,0,1,0,38,-2,-84,5,-14,4,-115,0,15,0,94,-78,9,16,17,17,18,57,0,-80,2,47,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,4,47,
		27,-79,4,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,10,-48,-80,11,-48,-80,0,16,-80,12,-48,-80,13,-48,48,49,37,51,3,35,17,33,17,33,53,33,21,33,17,33,17,51,5,68,-82,18,-91,-4,80,-2,-101,3,-119,-2,-107,2,70,-70,-104,-2,20,1,84,3,-12,-103,
		-103,-4,-92,3,-11,0,0,1,0,61,0,0,3,-33,4,-115,0,23,0,80,-78,4,24,25,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,22,47,27,-79,22,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,16,11,0,17,18,57,-80,16,47,-79,7,1,-80,10,43,88,33,-40,27,-12,89,-80,
		4,-48,-80,16,16,-80,19,-48,48,49,33,35,17,6,7,21,35,53,38,38,39,17,51,17,20,22,23,53,51,21,54,55,17,51,3,-33,-71,99,105,-107,-68,-55,3,-71,103,104,-107,103,101,-71,1,-62,33,11,-58,-61,10,-55,-70,1,109,-2,-99,123,120,11,-16,-19,11,34,2,49,0,1,0,-118,0,0,4,44,4,-115,0,17,0,71,-78,4,18,19,
		17,18,57,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,4,0,8,17,18,57,-80,4,47,-79,13,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,51,17,54,51,50,22,23,17,35,17,52,38,35,34,7,17,35,-118,-71,-102,-103,-44,
		-34,4,-71,126,127,-104,-101,-71,4,-115,-2,62,49,-54,-63,-2,-113,1,100,-121,121,51,-3,-49,0,2,0,2,-1,-16,5,107,4,-99,0,28,0,36,0,108,-78,21,37,38,17,18,57,-80,21,16,-80,30,-48,0,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,33,14,0,17,18,57,-80,33,47,
		-78,-65,33,1,93,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,3,-48,-80,33,16,-80,10,-48,-80,0,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-79,29,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,0,53,38,38,53,51,20,22,23,62,2,51,50,0,17,21,33,20,22,51,50,54,55,23,6,6,3,34,6,7,
		33,53,52,38,3,-111,-1,-2,-50,-90,-72,-103,95,102,5,-121,-23,-114,-8,1,16,-4,-82,-63,-73,76,-121,80,57,60,-72,-106,-113,-75,6,2,-103,-82,16,1,34,-13,11,-58,-88,94,119,12,-109,-20,-127,-2,-21,-2,-3,-126,-79,-64,31,40,-110,40,47,4,17,-62,-92,27,-95,-86,0,2,0,94,-1,-16,4,105,4,-99,0,22,0,30,0,97,-78,8,31,32,
		17,18,57,-80,8,16,-80,23,-48,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,13,0,8,17,18,57,-80,13,47,-80,0,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,26,1,-80,10,43,88,33,-40,27,-12,
		89,48,49,1,50,0,23,21,20,6,6,35,34,0,17,53,33,53,52,38,35,34,7,39,54,54,19,50,54,55,33,21,20,22,2,71,-9,1,41,2,-124,-20,-109,-8,-2,-16,3,82,-63,-73,-109,-112,57,65,-64,-119,-111,-77,6,-3,103,-83,4,-99,-2,-32,-17,-120,-103,-12,-119,1,21,1,1,-126,1,-79,-63,72,-110,41,47,-5,-19,-58,-95,27,-96,-84,
		0,1,0,71,-1,-19,3,-44,4,-115,0,28,0,112,-78,26,29,30,17,18,57,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-80,2,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,0,2,17,18,57,-78,5,11,2,17,18,57,-80,5,47,-78,17,11,2,17,18,57,-80,11,16,
		-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-78,28,5,26,17,18,57,48,49,1,33,53,33,23,1,22,22,21,20,6,35,34,38,39,38,53,51,22,22,51,50,54,53,52,38,35,35,53,2,-77,-3,-68,3,56,2,-2,-87,-79,-47,-4,-41,89,-85,60,122,-71,5,-119,115,-120,-110,-118,-122,
		-128,3,-12,-103,118,-2,-101,16,-59,-117,-89,-66,45,46,90,-98,89,100,104,106,95,106,-91,0,0,3,0,96,-1,-16,4,90,4,-99,0,13,0,20,0,27,0,118,-78,3,28,29,17,18,57,-80,3,16,-80,14,-48,-80,3,16,-80,21,-48,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-79,14,
		1,-80,10,43,88,33,-40,27,-12,89,-78,25,10,3,17,18,57,124,-80,25,47,24,-78,-96,25,1,93,-76,96,25,112,25,2,93,-76,96,25,112,25,2,113,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,0,35,34,0,17,53,16,0,51,50,0,23,1,50,54,55,33,22,
		22,19,34,6,7,33,38,38,4,90,-2,-20,-24,-27,-2,-25,1,23,-27,-23,1,19,2,-2,4,-109,-88,9,-3,118,10,-83,-115,-111,-85,8,2,-118,9,-86,2,36,-2,-5,-2,-47,1,50,1,7,62,1,2,1,52,-2,-48,-1,-2,28,-68,-76,-80,-64,3,119,-61,-84,-77,-68,0,1,0,48,0,0,3,-17,4,-99,0,39,0,-78,-78,29,40,41,17,18,
		57,0,-80,0,69,88,-80,29,47,27,-79,29,28,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,6,29,12,17,18,57,-80,6,47,-78,15,6,1,113,-78,15,6,1,93,-78,79,6,1,113,-80,1,-48,-80,1,47,64,9,31,1,47,1,63,1,79,1,4,93,-78,0,1,1,93,-79,2,4,-80,10,43,88,33,-40,27,-12,89,-80,6,
		16,-79,7,4,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,-80,14,-48,-80,15,-48,-80,7,16,-80,17,-48,-80,6,16,-80,19,-48,-80,2,16,-80,22,-48,-80,1,16,-80,24,-48,-78,33,1,29,17,18,57,-80,29,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,21,33,23,21,33,21,
		33,6,7,33,7,33,53,51,54,55,35,53,51,53,39,35,53,51,39,38,54,51,50,22,21,35,52,38,35,34,6,23,1,-121,1,-106,-2,110,3,1,-113,-2,108,10,36,2,-108,1,-4,-124,10,63,20,-97,-91,3,-94,-98,2,6,-53,-75,-73,-54,-71,104,96,93,104,4,2,-88,121,93,16,121,106,71,-104,-104,18,-97,121,16,93,121,64,-55,-20,-52,
		-73,112,119,-113,-118,0,0,1,0,66,-1,-16,3,-98,4,-99,0,33,0,-94,-78,20,34,35,17,18,57,0,-80,0,69,88,-80,21,47,27,-79,21,28,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,33,21,8,17,18,57,-80,33,47,-78,15,33,1,93,-76,16,33,32,33,2,93,-79,0,4,-80,10,43,88,33,-40,27,-12,89,-80,8,
		16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,11,-48,-80,33,16,-80,13,-48,-80,33,16,-80,18,-48,-80,18,47,64,9,31,18,47,18,63,18,79,18,4,93,-78,0,18,1,93,-79,15,4,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-80,18,16,-80,28,-48,-80,15,16,-80,30,-48,
		48,49,1,33,18,33,50,55,23,6,35,34,38,39,35,53,51,53,35,53,51,54,54,51,50,23,7,38,35,32,3,33,21,33,21,33,3,47,-2,104,32,1,2,98,104,27,118,111,-45,-11,20,-101,-105,-105,-101,22,-11,-49,96,-121,21,89,121,-1,0,32,1,-104,-2,100,1,-100,1,-106,-2,-15,28,-107,30,-38,-52,121,109,121,-52,-36,31,-107,28,-2,
		-16,121,109,0,0,4,0,-118,0,0,7,-83,4,-99,0,3,0,16,0,30,0,40,0,-85,-78,31,41,42,17,18,57,-80,31,16,-80,1,-48,-80,31,16,-80,4,-48,-80,31,16,-80,17,-48,0,-80,0,69,88,-80,39,47,27,-79,39,28,62,89,-80,0,69,88,-80,37,47,27,-79,37,28,62,89,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-80,
		0,69,88,-80,34,47,27,-79,34,18,62,89,-80,0,69,88,-80,32,47,27,-79,32,18,62,89,-80,7,16,-80,13,-48,-80,13,47,-80,2,-48,-80,2,47,-76,0,2,16,2,2,93,-79,1,3,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-79,20,3,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,27,3,-80,10,43,88,33,-40,27,-12,89,-78,
		33,39,32,17,18,57,-78,38,32,39,17,18,57,48,49,37,33,53,33,1,52,54,32,22,21,21,20,6,35,34,38,53,23,20,22,51,50,54,53,53,52,38,35,34,6,21,1,35,1,17,35,17,51,1,17,51,7,110,-3,-45,2,45,-3,-110,-68,1,52,-67,-66,-105,-103,-65,-93,94,87,84,94,97,83,82,97,-2,-75,-72,-3,-93,-71,-71,2,93,
		-72,-67,-114,2,3,-107,-70,-72,-101,80,-104,-74,-73,-100,5,89,106,105,92,82,90,104,103,94,-4,-75,3,108,-4,-108,4,-115,-4,-109,3,109,0,2,0,40,0,0,4,102,4,-115,0,22,0,31,0,-122,-78,0,32,33,17,18,57,-80,24,-48,0,-80,0,69,88,-80,12,47,27,-79,12,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,
		22,12,2,17,18,57,-80,22,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,22,16,-80,6,-48,-80,22,16,-80,11,-48,-80,11,47,64,9,15,11,31,11,47,11,63,11,4,93,-76,-65,11,-49,11,2,93,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,19,-48,-80,11,16,-80,23,-48,-80,12,16,-79,30,1,-80,10,43,88,33,
		-40,27,-12,89,48,49,37,33,21,35,53,35,53,51,53,35,53,51,17,33,50,22,21,20,6,7,33,21,33,37,33,50,54,53,52,38,35,33,2,-92,-2,-2,-70,-64,-64,-64,-64,1,-49,-59,-22,-29,-66,-2,-35,1,2,-2,-2,1,21,114,-125,-124,112,-2,-22,-76,-76,-76,-104,89,-104,2,80,-52,-88,-91,-53,4,89,-15,120,98,100,122,0,1,0,62,
		-1,-11,2,-102,3,32,0,38,0,116,0,-80,0,69,88,-80,14,47,27,-79,14,24,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-78,0,25,14,17,18,57,124,-80,0,47,24,-74,-128,0,-112,0,-96,0,3,93,-80,14,16,-79,7,2,-80,10,43,88,33,-40,27,-12,89,-78,10,0,7,17,18,57,-80,0,16,-79,38,2,-80,10,43,88,
		33,-40,27,-12,89,-78,20,38,0,17,18,57,-80,25,16,-79,32,2,-80,10,43,88,33,-40,27,-12,89,-78,29,38,32,17,18,57,48,49,1,51,50,54,53,52,38,35,34,6,21,35,52,54,51,50,22,21,20,6,7,22,21,20,6,35,34,38,53,51,20,22,51,50,54,53,52,39,35,1,9,84,74,72,63,70,57,75,-99,-93,124,-119,-100,70,
		66,-107,-86,-120,-124,-90,-98,79,67,70,73,-100,88,1,-53,61,48,45,58,51,41,98,123,121,104,55,91,25,41,-113,106,125,126,107,45,60,60,51,113,2,0,2,0,54,0,0,2,-69,3,21,0,10,0,14,0,74,0,-80,0,69,88,-80,9,47,27,-79,9,24,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,1,9,4,17,18,57,
		-80,1,47,-79,2,2,-80,10,43,88,33,-40,27,-12,89,-80,6,-48,-80,1,16,-80,11,-48,-78,8,11,6,17,18,57,-78,13,9,4,17,18,57,48,49,1,51,21,35,21,35,53,33,39,1,51,1,51,17,7,2,80,107,107,-99,-2,-119,6,1,121,-95,-2,-124,-33,17,1,43,-126,-87,-87,102,2,6,-2,22,1,33,28,0,0,1,0,91,-1,-11,
		2,-89,3,21,0,27,0,100,0,-80,0,69,88,-80,1,47,27,-79,1,24,62,89,-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,1,16,-79,4,9,-80,10,43,88,33,-40,27,-12,89,-78,7,13,1,17,18,57,-80,7,47,-79,25,2,-80,10,43,88,33,-40,27,-12,89,-78,5,7,25,17,18,57,-80,13,16,-80,17,-48,-80,13,16,-79,19,
		2,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,27,-48,48,49,19,19,33,21,33,7,54,51,50,22,21,20,6,35,34,38,39,51,22,51,50,54,53,52,38,35,34,7,112,50,1,-34,-2,-93,22,65,74,-128,-113,-96,-122,121,-89,6,-101,10,-127,65,72,78,74,73,59,1,-125,1,-110,-124,-86,29,-119,121,124,-111,126,101,99,75,68,62,77,43,
		0,2,0,86,-1,-11,2,-85,3,30,0,19,0,31,0,81,0,-80,0,69,88,-80,0,47,27,-79,0,24,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-80,0,16,-79,1,2,-80,10,43,88,33,-40,27,-12,89,-78,6,12,0,17,18,57,-80,6,47,-79,20,2,-80,10,43,88,33,-40,27,-12,89,-80,12,16,-79,27,2,-80,10,43,88,
		33,-40,27,-12,89,48,49,1,21,35,4,7,54,51,50,22,21,20,6,35,34,38,53,53,52,54,55,3,34,6,7,21,20,22,51,50,54,52,38,2,40,17,-2,-12,23,72,114,118,-121,-97,-124,-117,-89,-34,-51,126,51,77,17,83,63,61,78,71,3,30,-125,2,-37,77,-111,119,116,-102,-90,-105,51,-48,-28,5,-2,110,44,32,34,84,85,79,124,76,
		0,1,0,58,0,0,2,-91,3,21,0,6,0,51,0,-80,0,69,88,-80,5,47,27,-79,5,24,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,5,16,-79,4,2,-80,10,43,88,33,-40,27,-12,89,-78,0,5,4,17,18,57,48,49,1,1,35,1,33,53,33,2,-91,-2,-93,-90,1,93,-2,59,2,107,2,-69,-3,69,2,-109,-126,
		0,3,0,79,-1,-11,2,-97,3,32,0,19,0,30,0,40,0,125,0,-80,0,69,88,-80,17,47,27,-79,17,24,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,36,6,17,17,18,57,-80,36,47,-74,-33,36,-17,36,-1,36,3,93,-74,15,36,31,36,47,36,3,93,-78,-1,36,1,113,-76,15,36,31,36,2,114,-79,23,2,-80,10,
		43,88,33,-40,27,-12,89,-78,2,36,23,17,18,57,-78,12,23,36,17,18,57,-80,6,16,-79,29,2,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-79,31,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,7,22,21,20,6,32,38,53,52,54,55,38,53,52,54,51,50,22,3,52,38,35,34,6,21,20,22,50,54,3,34,6,21,20,22,
		50,54,52,38,2,-117,119,-117,-96,-2,-16,-96,74,64,119,-105,125,126,-105,-119,78,62,63,75,76,126,76,-116,55,63,63,112,63,64,2,67,118,55,59,-125,106,121,121,106,66,97,27,55,118,103,118,118,-2,58,52,58,58,52,53,58,58,1,-16,53,48,46,56,56,92,55,0,2,0,73,-1,-7,2,-107,3,32,0,18,0,30,0,93,0,-80,0,69,
		88,-80,8,47,27,-79,8,24,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,2,15,8,17,18,57,-80,2,47,-74,15,2,31,2,47,2,3,93,-80,15,16,-79,16,2,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,19,2,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,25,2,-80,10,43,88,33,-40,27,-12,89,48,49,1,
		6,35,34,38,53,52,54,51,50,22,23,21,16,5,7,53,50,54,39,50,55,53,52,38,35,34,6,21,20,22,1,-10,69,101,118,-115,-93,-127,-119,-100,3,-2,115,55,-106,-124,123,94,42,79,60,59,76,74,1,64,65,-118,126,121,-96,-91,-108,61,-2,100,20,1,127,98,-98,71,60,83,80,84,67,65,78,0,0,1,0,-113,2,-117,3,11,3,34,
		0,3,0,18,0,-80,2,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,53,33,3,11,-3,-124,2,124,2,-117,-105,0,0,3,0,-98,4,64,2,110,6,114,0,3,0,15,0,27,0,116,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,7,-48,-80,7,47,64,9,63,7,79,7,95,7,111,7,4,93,-80,2,-48,-80,
		2,47,-74,63,2,79,2,95,2,3,93,-80,0,-48,-80,0,47,64,17,15,0,31,0,47,0,63,0,79,0,95,0,111,0,127,0,8,93,-80,2,16,-80,3,-48,25,-80,3,47,24,-80,13,16,-79,19,7,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-79,25,7,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,7,35,7,52,54,51,50,22,
		21,20,6,35,34,38,55,20,22,51,50,54,53,52,38,35,34,6,1,-79,-67,-36,114,-126,100,72,68,99,97,70,72,100,85,51,36,35,48,48,35,37,50,6,114,-72,-41,70,97,94,73,71,92,94,69,35,50,49,36,38,50,52,0,1,0,-118,0,0,3,-82,4,-115,0,11,0,87,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,0,
		69,88,-80,4,47,27,-79,4,18,62,89,-80,11,-48,-80,11,47,-78,-33,11,1,93,-78,31,11,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,33,17,33,21,33,17,33,3,87,-3,-20,2,107,-4,
		-36,3,30,-3,-101,2,20,2,14,-2,-119,-105,4,-115,-103,-2,-78,0,0,3,0,30,-2,74,4,17,4,78,0,41,0,55,0,68,0,-108,0,-80,0,69,88,-80,38,47,27,-79,38,26,62,89,-80,0,69,88,-80,22,47,27,-79,22,20,62,89,-80,38,16,-80,41,-48,-80,41,47,-79,0,3,-80,10,43,88,33,-40,27,-12,89,-78,8,22,38,17,18,
		57,-80,8,47,-78,14,8,22,17,18,57,-80,14,47,-76,-112,14,-96,14,2,93,-79,55,1,-80,10,43,88,33,-40,27,-12,89,-78,28,55,14,17,18,57,-78,32,8,38,17,18,57,-80,22,16,-79,48,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,59,1,-80,10,43,88,33,-40,27,-12,89,-80,38,16,-79,66,1,-80,10,43,88,33,-40,27,
		-12,89,48,49,1,35,22,23,21,20,6,6,35,34,39,6,21,20,23,51,22,22,21,20,6,6,35,34,38,53,52,54,55,38,53,52,55,38,53,53,52,54,51,50,23,33,1,6,6,21,20,22,51,50,54,53,52,38,39,35,3,20,22,51,50,54,53,53,52,38,34,6,21,4,17,-105,58,1,111,-61,120,79,73,52,122,-73,-56,-50,-115,-12,
		-105,-47,-1,94,84,56,115,-82,-15,-69,80,71,1,111,-3,60,56,60,-108,-125,-110,-51,104,108,-17,116,-116,105,103,-118,-118,-46,-118,3,-89,84,105,25,98,-90,94,21,42,64,80,2,1,-107,-113,84,-95,96,-101,122,83,-118,42,47,74,124,82,106,-59,11,-99,-54,20,-5,-8,26,93,55,74,89,114,76,74,65,2,2,-91,83,123,122,88,18,87,120,120,90,
		0,2,0,100,-1,-21,4,88,4,78,0,16,0,28,0,99,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-78,0,2,9,17,18,57,-78,11,9,2,17,18,57,-80,2,16,-79,20,1,-80,
		10,43,88,33,-40,27,-12,89,-80,9,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,2,33,34,2,53,53,16,18,51,32,19,55,51,3,19,35,1,20,22,51,50,19,53,38,38,35,34,6,3,-126,108,-2,-14,-64,-28,-30,-60,1,9,108,34,-80,106,113,-80,-3,117,-110,-121,-45,72,28,-110,107,-122,-107,-15,-2,-6,1,27,-12,15,1,
		8,1,61,-2,-1,-19,-3,-30,-3,-28,1,-12,-81,-61,1,-121,36,-66,-53,-29,0,2,0,-79,0,0,4,-29,5,-81,0,22,0,30,0,99,-78,24,31,32,17,18,57,-80,24,16,-80,4,-48,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,18,62,89,-78,
		23,3,1,17,18,57,-80,23,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,9,23,0,17,18,57,-80,3,16,-79,29,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,35,17,33,50,22,21,20,7,22,19,21,22,23,21,35,38,39,53,52,38,35,37,33,50,54,53,16,33,33,1,114,-63,2,14,-16,-5,-19,-34,5,2,65,-58,59,
		3,-116,127,-2,-98,1,57,-94,-99,-2,-49,-2,-71,2,116,-3,-116,5,-81,-46,-52,-27,99,69,-2,-6,-100,-115,61,24,54,-84,-117,120,-113,-99,124,-124,1,0,0,1,0,-78,0,0,5,29,5,-80,0,12,0,105,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,2,47,27,-79,
		2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,6,2,4,17,18,57,124,-80,6,47,24,-76,99,6,115,6,2,93,-76,51,6,67,6,2,93,-78,-109,6,1,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,10,1,6,17,18,57,48,49,1,35,17,35,17,51,17,51,1,51,1,1,35,2,35,-79,-64,-64,-106,1,-3,
		-17,-3,-44,2,85,-21,2,-114,-3,114,5,-80,-3,126,2,-126,-3,62,-3,18,0,1,0,-110,0,0,4,20,6,0,0,12,0,84,0,-80,0,69,88,-80,4,47,27,-79,4,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,7,8,
		2,17,18,57,-80,7,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,10,0,7,17,18,57,48,49,1,35,17,35,17,51,17,51,1,51,1,1,35,1,-52,-128,-70,-70,126,1,59,-37,-2,-122,1,-82,-37,1,-11,-2,11,6,0,-4,-114,1,-84,-2,19,-3,-77,0,0,1,0,-78,0,0,4,-6,5,-80,0,11,0,76,0,-80,0,69,88,-80,
		3,47,27,-79,3,30,62,89,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-78,0,3,1,17,18,57,-78,5,3,1,17,18,57,-78,9,0,5,17,18,57,48,49,1,17,35,17,51,17,51,1,51,1,1,35,1,114,-64,-64,12,2,99,-15,
		-3,107,2,-67,-19,2,-75,-3,75,5,-80,-3,121,2,-121,-3,59,-3,21,0,0,1,0,-110,0,0,3,-15,6,24,0,12,0,76,0,-80,0,69,88,-80,4,47,27,-79,4,32,62,89,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,0,8,
		2,17,18,57,-78,6,8,2,17,18,57,-78,10,6,0,17,18,57,48,49,1,35,17,35,17,51,17,51,1,51,1,1,35,1,80,4,-70,-70,1,1,-118,-16,-2,43,1,-1,-28,1,-13,-2,13,6,24,-4,117,1,-83,-2,13,-3,-71,0,0,2,0,-118,0,0,4,31,4,-115,0,10,0,20,0,72,-78,2,21,22,17,18,57,-80,2,16,-80,20,
		-48,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,1,16,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,22,22,23,21,20,0,33,3,17,51,50,54,53,53,52,38,35,-118,1,105,-94,-5,-116,3,-2,-55,
		-2,-7,-98,-92,-70,-58,-67,-73,4,-115,-123,-10,-97,77,-4,-2,-42,3,-12,-4,-93,-48,-64,64,-64,-51,0,1,0,96,-1,-16,4,48,4,-99,0,28,0,78,-78,3,29,30,17,18,57,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,11,16,-80,15,-48,-80,11,16,-79,18,1,-80,10,43,88,
		33,-40,27,-12,89,-80,3,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,28,-48,48,49,1,6,6,35,34,0,17,53,52,54,54,51,50,22,23,35,38,38,35,34,6,7,21,20,22,51,50,54,55,4,48,20,-4,-47,-32,-2,-15,123,-25,-104,-52,-9,19,-71,18,-115,126,-103,-89,1,-97,-105,-121,-115,20,1,121,-69,-50,1,39,1,
		3,94,-92,-7,-120,-45,-69,-126,116,-53,-67,106,-67,-49,111,-125,0,3,0,-118,0,0,3,-17,4,-115,0,14,0,22,0,30,0,107,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,23,0,1,17,18,57,-80,23,47,-78,-65,23,1,93,-76,31,23,47,23,2,93,-76,-33,23,-17,23,2,93,
		-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,8,15,23,17,18,57,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,22,21,20,6,7,22,22,21,20,6,7,1,17,33,50,54,53,52,35,37,51,50,54,53,52,39,35,-118,1,-106,-47,-34,95,88,99,
		116,-38,-55,-2,-9,1,6,115,122,-21,-2,-8,-22,108,124,-27,-19,4,-115,-93,-101,81,126,33,24,-107,101,-98,-82,1,2,18,-2,-123,98,85,-60,-115,85,83,-88,5,0,2,0,19,0,0,4,112,4,-115,0,7,0,10,0,71,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,
		6,47,27,-79,6,18,62,89,-78,9,4,2,17,18,57,-80,9,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,10,4,2,17,18,57,48,49,1,33,3,35,1,51,1,35,1,33,3,3,70,-3,-8,110,-67,1,-33,-90,1,-40,-68,-3,-58,1,-111,-57,1,23,-2,-23,4,-115,-5,115,1,-82,1,-3,0,0,1,0,-97,4,-114,1,-106,6,59,
		0,8,0,12,0,-80,0,47,-80,4,-48,-80,4,47,48,49,1,23,6,7,21,35,53,52,54,1,43,107,59,3,-71,84,6,59,83,99,111,-120,-126,77,-83,0,0,2,0,-127,4,-33,2,-32,6,-118,0,13,0,17,0,96,0,-80,3,47,-80,7,-48,-80,7,47,64,13,15,7,31,7,47,7,63,7,79,7,95,7,6,93,-80,3,16,-79,10,4,
		-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,13,-48,-80,13,47,-80,7,16,-80,17,-48,-80,17,47,-80,15,-48,-80,15,47,64,15,15,15,31,15,47,15,63,15,79,15,95,15,111,15,7,93,-80,17,16,-80,16,-48,25,-80,16,47,24,48,49,1,20,6,35,34,38,53,51,20,22,51,50,54,53,37,51,23,35,2,-32,-88,-121,-120,-88,-104,79,
		73,71,79,-2,-90,-102,112,101,5,-80,95,114,114,95,55,61,63,53,-38,-58,0,2,-4,-92,4,-68,-2,-52,6,-109,0,20,0,24,0,-102,0,-80,3,47,-78,15,3,1,93,-78,-1,3,1,93,-78,112,3,1,93,-80,7,-48,-80,7,47,64,11,15,7,31,7,47,7,63,7,79,7,5,93,-80,3,16,-80,10,-48,-80,10,47,-80,7,16,-79,14,3,
		-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,17,3,-80,10,43,88,33,-40,27,-12,89,-80,14,16,-80,20,-48,-80,14,16,-80,23,-48,-80,23,47,64,25,63,23,79,23,95,23,111,23,127,23,-113,23,-97,23,-81,23,-65,23,-49,23,-33,23,-17,23,12,93,-80,21,-48,-80,21,47,64,11,15,21,31,21,47,21,63,21,79,21,5,93,-80,23,16,
		-80,24,-48,25,-80,24,47,24,48,49,1,20,6,35,34,38,38,35,34,6,21,39,52,54,51,50,22,51,50,54,53,39,51,7,35,-2,-52,96,70,53,113,34,20,35,47,84,96,70,47,-127,44,35,48,-115,-85,-74,120,5,125,74,105,66,9,51,38,21,75,107,75,51,38,-2,-31,0,0,2,0,110,4,-31,4,88,6,-107,0,6,0,10,0,93,
		0,-80,3,47,-78,15,3,1,93,-80,5,-48,-80,5,47,-80,0,-48,-80,0,47,-74,15,0,31,0,47,0,3,93,-80,3,16,-80,2,-48,25,-80,2,47,24,-78,4,3,0,17,18,57,-80,6,-48,25,-80,6,47,24,-80,3,16,-80,9,-48,-80,9,47,-80,7,-48,-80,7,47,-74,15,7,31,7,47,7,3,93,-80,9,16,-80,10,-48,25,-80,10,47,
		24,48,49,1,51,1,35,39,7,35,1,51,3,35,1,-110,-104,1,34,-59,-87,-86,-58,3,34,-56,-55,-115,5,-24,-2,-7,-97,-97,1,-76,-2,-3,0,2,-1,94,4,-49,3,70,6,-126,0,6,0,10,0,93,0,-80,3,47,-78,15,3,1,93,-80,4,-48,25,-80,4,47,24,-80,0,-48,25,-80,0,47,24,-80,3,16,-80,1,-48,-80,1,47,-80,6,
		-48,-80,6,47,-74,15,6,31,6,47,6,3,93,-78,2,3,6,17,18,57,-80,3,16,-80,8,-48,-80,8,47,-80,7,-48,25,-80,7,47,24,-80,8,16,-80,10,-48,-80,10,47,-74,15,10,31,10,47,10,3,93,48,49,1,35,39,7,35,1,51,5,35,3,51,3,70,-59,-86,-86,-60,1,34,-104,-2,-113,-116,-56,-57,4,-49,-98,-98,1,6,85,1,
		2,0,0,2,0,105,4,-28,3,-20,6,-49,0,6,0,21,0,115,0,-80,3,47,-80,5,-48,-80,5,47,-74,15,5,31,5,47,5,3,93,-78,4,3,5,17,18,57,25,-80,4,47,24,-80,0,-48,-80,3,16,-80,1,-48,-80,1,47,-78,2,5,3,17,18,57,-80,7,-48,124,-80,7,47,24,64,13,15,7,31,7,47,7,63,7,79,7,95,7,
		6,93,-80,14,-48,-80,14,47,64,13,15,14,31,14,47,14,63,14,79,14,95,14,6,93,-80,13,-48,-78,8,7,13,17,18,57,-78,20,14,7,17,18,57,48,49,1,35,39,7,35,1,51,23,39,54,54,53,52,35,55,50,22,21,20,6,7,7,3,70,-86,-59,-59,-87,1,16,-68,-66,1,65,59,-115,5,-128,-122,74,60,1,4,-28,-70,-70,1,
		6,124,-125,4,26,33,67,92,88,73,59,66,7,60,0,2,0,105,4,-28,3,70,6,-44,0,6,0,26,0,-121,0,-80,3,47,-80,1,-48,-80,1,47,-80,6,-48,-80,6,47,64,9,15,6,31,6,47,6,63,6,4,93,-78,4,3,6,17,18,57,25,-80,4,47,24,-80,0,-48,-78,2,6,1,17,18,57,-80,6,16,-80,10,-48,-80,10,47,-76,
		63,10,79,10,2,93,-80,13,-48,-80,13,47,64,13,15,13,31,13,47,13,63,13,79,13,95,13,6,93,-80,10,16,-80,16,-48,-80,16,47,-80,13,16,-79,20,4,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,23,4,-80,10,43,88,33,-40,27,-12,89,-80,20,16,-80,26,-48,48,49,1,35,39,7,35,37,51,55,20,6,35,34,38,35,34,
		6,21,39,52,54,51,50,22,51,50,54,53,3,70,-86,-59,-59,-87,1,45,-125,-61,96,65,54,110,40,29,54,77,96,64,42,124,38,31,52,4,-28,-98,-98,-12,-27,62,94,71,46,29,19,63,98,70,45,28,0,1,0,-118,0,0,3,-123,5,-60,0,7,0,51,-78,3,8,9,17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,
		0,69,88,-80,4,47,27,-79,4,18,62,89,-80,6,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,51,17,33,17,35,17,33,2,-52,-71,-3,-66,-71,2,66,5,-60,-2,48,-4,12,4,-115,0,0,2,0,-127,4,-33,2,-32,6,-118,0,13,0,17,0,96,0,-80,3,47,-80,7,-48,-80,7,47,64,13,15,7,31,7,47,7,63,7,
		79,7,95,7,6,93,-80,3,16,-79,10,4,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,13,-48,-80,13,47,-80,7,16,-80,16,-48,-80,16,47,-80,15,-48,-80,15,47,64,15,15,15,31,15,47,15,63,15,79,15,95,15,111,15,7,93,-80,16,16,-80,17,-48,25,-80,17,47,24,48,49,1,20,6,35,34,38,53,51,20,22,51,50,54,53,
		39,51,7,35,2,-32,-88,-121,-120,-88,-104,79,73,71,79,96,-103,-92,102,5,-80,95,114,114,95,55,61,63,53,-38,-58,0,0,2,0,-127,4,-32,2,-54,7,3,0,13,0,28,0,102,0,-80,3,47,-80,7,-48,-80,7,47,64,13,15,7,31,7,47,7,63,7,79,7,95,7,6,93,-80,3,16,-79,10,4,-80,10,43,88,33,-40,27,-12,89,-80,
		7,16,-80,13,-48,-80,13,47,-80,7,16,-80,14,-48,-80,14,47,-80,21,-48,-80,21,47,64,15,15,21,31,21,47,21,63,21,79,21,95,21,111,21,7,93,-80,20,-48,-78,15,20,14,17,18,57,-78,27,14,21,17,18,57,48,49,1,20,6,35,34,38,53,51,20,22,51,50,54,53,39,39,54,54,53,52,35,55,50,22,21,20,6,7,7,2,
		-54,-95,-125,-124,-95,-110,74,73,69,76,-55,1,74,66,-96,7,-112,-108,81,68,1,5,-80,94,114,115,93,53,62,61,54,17,124,4,24,29,59,82,78,66,50,59,7,62,-1,-1,0,80,2,-115,2,-99,5,-72,3,7,1,-57,0,0,2,-104,0,19,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,16,-48,48,49,0,-1,-1,0,54,2,-104,
		2,-69,5,-83,3,7,2,32,0,0,2,-104,0,19,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-80,13,-48,48,49,0,-1,-1,0,91,2,-115,2,-89,5,-83,3,7,2,33,0,0,2,-104,0,16,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,48,49,-1,-1,0,86,2,-115,2,-85,5,-74,3,7,2,34,0,0,2,-104,0,19,
		0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-80,20,-48,48,49,0,-1,-1,0,58,2,-104,2,-91,5,-83,3,7,2,35,0,0,2,-104,0,16,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,48,49,-1,-1,0,79,2,-115,2,-97,5,-72,3,7,2,36,0,0,2,-104,0,25,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,
		-80,23,-48,-80,17,16,-80,31,-48,48,49,0,-1,-1,0,73,2,-111,2,-107,5,-72,3,7,2,37,0,0,2,-104,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,25,-48,48,49,0,0,1,0,126,-1,-21,5,29,5,-59,0,30,0,78,-78,12,31,32,17,18,57,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,89,-80,0,69,
		88,-80,3,47,27,-79,3,18,62,89,-80,12,16,-80,16,-48,-80,12,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,30,-48,48,49,1,6,0,35,34,36,2,39,53,52,18,36,51,50,0,23,35,38,38,35,34,2,17,21,20,18,22,51,50,54,55,5,28,24,-2,-37,
		-18,-79,-2,-31,-94,1,-99,1,27,-78,-19,1,47,25,-63,24,-65,-99,-64,-22,110,-56,125,-95,-80,26,1,-50,-33,-2,-4,-76,1,71,-53,68,-45,1,74,-77,-2,-6,-29,-93,-88,-2,-53,-2,-2,55,-95,-1,0,-112,-99,-87,0,1,0,126,-1,-21,5,30,5,-60,0,34,0,112,-78,12,35,36,17,18,57,0,-80,0,69,88,-80,12,47,27,-79,12,30,62,
		89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-78,16,3,12,17,18,57,-80,16,47,-80,12,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,34,12,3,17,18,57,-80,34,47,-76,63,34,79,34,2,93,-76,15,34,31,34,2,93,-79,31,1,-80,10,43,88,33,-40,27,-12,89,
		48,49,37,6,4,35,34,36,2,39,53,52,18,36,51,50,4,23,35,38,38,35,34,2,7,7,20,18,22,51,50,54,55,17,33,53,33,5,30,67,-2,-29,-80,-69,-2,-42,-88,3,-101,1,28,-75,-15,1,33,34,-64,30,-70,-100,-75,-20,10,1,120,-45,-123,114,-75,42,-2,-80,2,15,-66,97,114,-76,1,71,-46,45,-37,1,78,-74,-27,-38,-107,-116,
		-2,-36,-14,70,-84,-2,-10,-116,58,48,1,70,-101,0,0,2,0,-78,0,0,5,17,5,-80,0,11,0,21,0,72,-78,3,22,23,17,18,57,-80,3,16,-80,21,-48,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,1,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,13,
		1,-80,10,43,88,33,-40,27,-12,89,48,49,51,17,33,50,4,18,23,21,20,2,4,7,3,17,51,50,0,17,53,52,0,35,-78,1,-79,-63,1,56,-79,4,-83,-2,-62,-53,-23,-33,-22,1,19,-2,-9,-24,5,-80,-84,-2,-60,-56,62,-48,-2,-63,-79,2,5,18,-5,-117,1,42,1,3,36,-4,1,40,0,2,0,126,-1,-21,5,95,5,-59,0,17,
		0,34,0,72,-78,4,35,36,17,18,57,-80,4,16,-80,31,-48,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,13,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,31,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,2,4,35,34,36,2,39,53,52,18,36,51,
		50,4,18,23,7,52,2,38,35,34,6,6,7,21,20,18,22,51,50,18,53,5,95,-94,-2,-30,-81,-85,-2,-31,-90,2,-92,1,33,-85,-83,1,32,-93,1,-65,110,-57,125,120,-58,114,1,113,-55,121,-63,-17,2,-62,-50,-2,-80,-71,-71,1,74,-56,55,-51,1,79,-68,-71,-2,-76,-52,5,-94,1,0,-113,-113,-2,-100,53,-96,-2,-2,-110,1,59,-1,0,
		0,2,0,126,-1,4,5,95,5,-59,0,21,0,38,0,79,-78,8,39,40,17,18,57,-80,8,16,-80,35,-48,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,3,8,17,17,18,57,-80,17,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,35,1,-80,10,43,88,33,-40,
		27,-12,89,48,49,1,20,2,7,23,7,37,6,35,34,36,2,39,53,52,18,36,51,50,4,18,21,39,52,2,38,35,34,6,6,7,21,20,18,22,51,50,18,53,5,95,-87,-108,-6,-125,-2,-52,57,60,-85,-2,-32,-92,3,-94,1,34,-84,-82,1,33,-94,-65,110,-57,125,120,-57,113,1,113,-55,121,-63,-17,2,-62,-44,-2,-84,90,-61,121,-13,12,
		-70,1,70,-58,58,-52,1,80,-66,-69,-2,-80,-50,1,-93,1,1,-113,-112,-1,-100,51,-96,-2,-2,-110,1,59,-1,0,0,1,0,-96,0,0,2,-55,4,-115,0,6,0,51,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,5,17,18,57,-80,4,47,-79,3,1,-80,10,43,88,33,-40,
		27,-12,89,48,49,33,35,17,5,53,37,51,2,-55,-71,-2,-112,2,10,31,3,-90,-117,-88,-54,0,0,1,0,-125,0,0,4,32,4,-96,0,24,0,86,-78,9,25,26,17,18,57,0,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-79,23,1,-80,10,43,88,33,-40,27,-12,89,-80,2,-48,-78,
		22,23,17,17,18,57,-78,3,17,22,17,18,57,-80,17,16,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-80,12,-48,48,49,33,33,53,1,54,55,55,52,38,35,34,6,21,35,52,54,54,51,50,22,21,20,7,1,33,4,32,-4,-121,1,-3,125,10,3,125,102,122,-107,-71,120,-46,126,-69,-31,-59,-2,-122,2,120,-125,1,-55,115,84,
		53,84,108,-114,117,112,-65,108,-72,-104,-79,-76,-2,-84,0,1,0,15,-2,-93,3,-34,4,-115,0,24,0,81,0,-80,11,47,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-78,5,11,2,17,18,57,-80,5,47,-80,11,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,23,
		1,-80,10,43,88,33,-40,27,-12,89,-78,24,23,5,17,18,57,48,49,1,33,53,33,21,1,22,22,21,20,0,35,34,39,55,22,51,50,54,53,52,38,35,35,53,2,-28,-3,116,3,114,-2,-128,-78,-30,-2,-52,-1,-54,-46,52,-91,-79,-76,-41,-71,-64,60,3,-12,-103,118,-2,108,24,-10,-77,-7,-2,-38,103,-117,88,-54,-91,-85,-91,103,0,0,2,
		0,62,-2,-74,4,-96,4,-115,0,10,0,14,0,76,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,6,16,-80,5,-48,-80,5,47,-80,0,16,-80,12,-48,-78,13,9,2,17,18,57,48,49,
		37,51,21,35,17,35,17,33,53,1,51,1,33,17,7,3,-37,-59,-59,-70,-3,29,2,-42,-57,-3,60,2,10,28,-106,-105,-2,-73,1,73,109,4,33,-4,9,2,-4,53,0,1,0,101,-2,-96,4,5,4,-116,0,27,0,81,0,-80,13,47,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-78,7,13,
		1,17,18,57,-80,7,47,-79,24,1,-80,10,43,88,33,-40,27,-12,89,-78,5,7,24,17,18,57,-80,13,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,27,-48,48,49,19,19,33,21,33,3,54,55,54,18,21,20,0,35,34,39,55,22,51,50,54,53,52,38,35,34,6,7,-122,102,3,20,-3,126,54,111,-107,-56,-15,-2,-32,
		-15,-32,-81,58,-126,-45,-103,-65,-91,-121,106,117,34,1,116,3,24,-85,-2,116,64,2,2,-2,-11,-31,-17,-2,-30,114,-117,101,-49,-92,-113,-74,58,83,0,1,0,74,-2,-74,3,-14,4,-115,0,6,0,38,0,-80,1,47,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,0,3,5,17,18,57,48,49,
		1,1,35,1,33,53,33,3,-14,-3,-96,-70,2,87,-3,27,3,-88,4,35,-6,-109,5,63,-104,0,0,2,0,-125,4,-39,2,-46,6,-48,0,13,0,33,0,126,0,-80,3,47,-80,7,-48,-80,7,47,64,13,15,7,31,7,47,7,63,7,79,7,95,7,6,93,-80,3,16,-79,10,4,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,13,-48,-80,
		13,47,-80,7,16,-80,17,-48,-80,17,47,-80,20,-48,-80,20,47,64,11,15,20,31,20,47,20,63,20,79,20,5,93,-80,17,16,-80,23,-48,-80,23,47,-80,20,16,-79,27,4,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-79,30,4,-80,10,43,88,33,-40,27,-12,89,-80,27,16,-80,33,-48,48,49,1,20,6,35,34,38,53,51,20,22,51,50,
		54,53,19,20,6,35,34,38,35,34,6,21,39,52,54,51,50,22,51,50,54,53,2,-46,-95,-122,-121,-95,-106,74,72,71,74,-115,96,70,58,119,44,34,48,83,96,69,48,-127,44,35,48,5,-82,95,118,118,95,54,64,64,54,1,10,74,105,75,51,38,21,75,107,75,51,38,0,1,0,103,-2,-103,1,33,0,-103,0,3,0,18,0,-80,4,47,
		-80,2,-48,-80,2,47,-80,1,-48,-80,1,47,48,49,1,35,17,51,1,33,-70,-70,-2,-103,2,0,0,2,0,96,-1,-16,6,109,4,-99,0,19,0,29,0,-97,-78,21,30,31,17,18,57,-80,21,16,-80,10,-48,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-80,0,69,88,-80,2,47,27,
		-79,2,18,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,11,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-80,15,-48,-80,15,47,-78,31,15,1,93,-78,-33,15,1,93,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,2,16,-79,20,1,-80,10,43,88,33,
		-40,27,-12,89,-80,9,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,33,5,34,0,17,53,16,0,51,5,33,21,33,17,33,21,33,17,33,5,55,17,39,34,6,21,21,20,22,6,109,-3,99,-2,-114,-27,-2,-25,1,23,-27,1,91,2,-81,-3,-101,2,20,-3,-20,2,108,-5,-15,-22,-20,-106,-81,-80,16,1,50,1,7,62,1,2,
		1,52,16,-103,-2,-78,-104,-2,-119,13,7,3,103,9,-42,-59,66,-61,-41,0,0,2,0,-126,-2,-87,4,63,4,-95,0,24,0,37,0,78,0,-80,20,47,-80,0,69,88,-80,12,47,27,-79,12,28,62,89,-80,20,16,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,5,20,12,17,18,57,-80,5,47,-78,3,5,12,17,18,57,-79,26,1,-80,10,
		43,88,33,-40,27,-12,89,-80,12,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,50,54,55,6,35,34,2,53,52,54,54,51,50,0,19,21,20,2,4,35,34,39,55,22,19,50,54,55,53,52,38,35,34,6,21,20,22,1,-33,-79,-36,21,119,-73,-46,-1,117,-46,-124,-21,1,5,2,-110,-2,-13,-81,-97,118,38,122,-32,105,-97,34,
		-95,-110,127,-104,-93,-65,-12,-39,105,1,20,-30,-100,-20,126,-2,-36,-2,-10,-6,-36,-2,-70,-82,60,-114,50,1,-4,92,82,-108,-59,-59,-61,-85,-107,-55,0,1,-1,-74,-2,75,1,103,0,-104,0,12,0,40,0,-80,13,47,-80,0,69,88,-80,4,47,27,-79,4,20,62,89,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-80,13,16,-80,12,-48,-80,12,47,
		48,49,37,21,6,6,35,34,39,55,22,51,50,53,53,1,103,1,-86,-105,59,52,14,30,67,-119,-104,-11,-88,-80,18,-99,13,-62,-23,0,-1,-1,0,59,-2,-93,4,10,4,-115,1,6,2,76,44,0,0,16,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,48,49,-1,-1,0,115,-2,-96,4,19,4,-116,1,6,2,78,14,0,0,16,0,-80,
		0,69,88,-80,1,47,27,-79,1,28,62,89,48,49,-1,-1,0,35,-2,-74,4,-123,4,-115,1,6,2,77,-27,0,0,19,0,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-80,12,-48,48,49,0,-1,-1,0,119,0,0,4,20,4,-96,1,6,2,75,-12,0,0,16,0,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,48,49,-1,-1,0,118,
		-2,-74,4,30,4,-115,1,6,2,79,44,0,0,16,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,48,49,-1,-1,0,55,-1,-21,4,72,4,-95,1,6,2,101,-65,0,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,15,-48,48,49,0,-1,-1,0,126,-1,-20,4,22,5,-79,1,6,0,26,-6,0,0,19,0,-80,0,69,
		88,-80,0,47,27,-79,0,30,62,89,-80,21,-48,48,49,0,-1,-1,0,95,-2,-87,4,28,4,-95,1,6,2,83,-35,0,0,19,0,-80,0,69,88,-80,12,47,27,-79,12,28,62,89,-80,32,-48,48,49,0,-1,-1,0,112,-1,-20,4,14,5,-60,1,6,0,28,0,0,0,25,0,-80,0,69,88,-80,21,47,27,-79,21,30,62,89,-80,27,-48,-80,
		21,16,-80,34,-48,48,49,0,-1,-1,0,-12,0,0,3,29,4,-115,0,6,2,74,84,0,-1,-1,-1,-76,-2,75,1,101,4,58,0,6,0,-100,0,0,-1,-1,-1,-76,-2,75,1,101,4,58,0,6,0,-100,0,0,-1,-1,0,-101,0,0,1,85,4,58,1,6,0,-115,0,0,0,16,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,48,49,
		-1,-1,-1,-6,-2,89,1,90,4,58,0,38,0,-115,0,0,0,6,0,-92,-56,10,-1,-1,0,-101,0,0,1,85,4,58,0,6,0,-115,0,0,0,1,0,-118,-1,-20,3,-7,4,-99,0,33,0,102,0,-80,0,69,88,-80,21,47,27,-79,21,28,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,62,89,-80,0,69,88,-80,31,47,27,-79,31,18,
		62,89,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,25,31,21,17,18,57,-80,25,47,-76,31,25,47,25,2,93,-80,8,-80,10,43,88,-40,27,-36,89,-80,25,16,-80,10,-48,-80,21,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,22,51,50,54,53,52,38,35,35,53,19,38,35,34,3,17,35,17,54,54,51,50,22,23,1,
		22,22,21,20,6,35,34,39,1,-61,82,88,97,114,-120,-121,84,-19,78,99,-45,4,-72,1,-59,-55,107,-61,101,-2,-18,-87,-74,-41,-75,119,104,-75,51,123,99,98,85,-119,1,39,62,-2,-11,-3,6,2,-11,-46,-42,85,98,-2,-74,15,-93,-122,-84,-52,49,0,0,2,0,120,-1,-21,4,-119,4,-95,0,11,0,25,0,59,0,-80,0,69,88,-80,8,47,
		27,-79,8,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,8,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,22,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,16,0,32,0,3,53,16,0,32,0,19,39,52,38,35,34,6,7,21,20,22,51,50,54,55,4,-119,-2,-24,-2,34,-2,-26,1,1,25,1,-34,
		1,25,1,-70,-78,-99,-101,-78,2,-74,-101,-102,-79,2,2,60,-2,-22,-2,-59,1,60,1,20,20,1,20,1,62,-2,-60,-2,-21,13,-54,-30,-32,-59,52,-55,-27,-35,-54,0,0,1,0,59,0,0,3,-46,5,-80,0,6,0,51,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,5,16,-79,3,
		1,-80,10,43,88,33,-40,27,-12,89,-78,0,3,5,17,18,57,48,49,1,1,35,1,33,53,33,3,-46,-3,-66,-70,2,64,-3,37,3,-105,5,72,-6,-72,5,24,-104,0,2,0,-116,-1,-20,4,52,6,0,0,16,0,27,0,102,-78,20,28,29,17,18,57,-80,20,16,-80,13,-48,0,-80,9,47,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,
		-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,7,47,27,-79,7,18,62,89,-78,6,13,4,17,18,57,-78,11,13,4,17,18,57,-80,13,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,6,35,34,39,7,35,17,51,17,54,51,50,18,17,39,
		52,38,35,34,7,17,22,51,50,54,4,52,111,-55,-128,-47,112,15,-96,-71,112,-59,-55,-15,-71,-93,-116,-73,80,85,-76,-118,-93,2,18,-97,-4,-117,-107,-127,6,0,-3,-61,-117,-2,-45,-2,-1,7,-76,-42,-86,-2,44,-85,-40,0,0,1,0,92,-1,-20,3,-17,4,78,0,29,0,75,-78,0,30,31,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,
		26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,3,-48,-80,16,16,-80,20,-48,-80,16,16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,50,54,55,51,14,2,35,34,0,53,53,52,54,54,51,50,22,23,35,38,38,35,34,6,21,21,20,22,2,64,99,-108,
		8,-80,5,120,-60,110,-33,-2,-5,118,-37,-109,-74,-15,8,-80,8,-113,104,-113,-101,-99,-125,120,90,94,-88,99,1,42,-4,32,-99,-7,-122,-38,-82,105,-121,-50,-65,33,-68,-55,0,2,0,91,-1,-20,4,0,6,0,0,17,0,28,0,102,-78,26,29,30,17,18,57,-80,26,16,-80,4,-48,0,-80,7,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,
		-80,0,69,88,-80,13,47,27,-79,13,18,62,89,-80,0,69,88,-80,9,47,27,-79,9,18,62,89,-78,6,4,13,17,18,57,-78,11,4,13,17,18,57,-80,13,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,54,51,50,23,17,51,17,35,39,6,35,34,38,38,39,
		55,20,22,51,50,55,17,38,35,34,6,91,113,-50,-128,-66,111,-71,-95,14,111,-54,124,-53,117,1,-71,-88,-118,-81,82,83,-84,-115,-89,2,38,-97,-4,-115,-126,2,52,-6,0,120,-116,-116,-5,-104,6,-79,-40,-97,1,-15,-103,-42,0,2,0,91,-2,86,4,0,4,78,0,27,0,38,0,127,-78,31,39,40,17,18,57,-80,31,16,-80,11,-48,0,-80,0,
		69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,20,62,89,-80,0,69,88,-80,24,47,27,-79,24,18,62,89,-78,5,3,24,17,18,57,-80,11,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-78,22,3,24,17,18,57,-80,24,16,-79,31,1,-80,10,43,88,33,
		-40,27,-12,89,-80,3,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,55,51,17,6,2,35,34,38,39,55,22,22,51,50,54,53,53,6,35,34,2,53,23,20,22,51,50,55,17,38,35,34,6,91,-8,-58,-52,111,15,-99,2,-12,-32,86,-56,72,55,63,-97,79,-107,-118,111,-63,-62,-6,-71,-90,-117,-81,83,83,-83,
		-114,-91,2,38,-10,1,50,-108,-128,-4,14,-17,-2,-3,55,50,-118,42,50,-80,-88,40,-127,1,56,-12,7,-80,-39,-95,1,-21,-99,-41,0,2,0,90,-1,-20,4,68,4,78,0,16,0,28,0,56,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,
		4,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,54,54,51,50,0,21,21,20,6,6,35,34,38,38,39,55,20,22,51,50,54,53,52,38,35,34,6,90,-128,-29,-112,-35,1,26,126,-27,-110,-113,-29,-127,2,-71,-81,-115,-114,-82,-79,-115,-117,-81,2,39,-100,-1,-116,-2,-52,-5,14,-99,-4,-116,-120,-7,-102,10,-80,-34,-32,-60,-81,-32,
		-34,0,0,2,0,-116,-2,96,4,50,4,78,0,16,0,27,0,112,-78,25,28,29,17,18,57,-80,25,16,-80,13,-48,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-80,0,69,88,-80,7,47,27,-79,7,20,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,6,13,4,17,18,
		57,-78,11,13,4,17,18,57,-80,13,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,20,6,6,35,34,39,17,35,17,51,23,54,51,50,18,23,7,52,38,35,34,7,17,22,51,50,54,4,50,110,-56,-127,-59,113,-71,-97,15,116,-54,-63,-18,10,-72,-87,-113,-88,84,83,-85,
		-116,-86,2,17,-98,-4,-117,125,-3,-9,5,-38,125,-111,-2,-23,-22,39,-80,-37,-107,-3,-5,-108,-33,0,0,2,0,91,-2,96,3,-1,4,78,0,15,0,26,0,109,-78,24,27,28,17,18,57,-80,24,16,-80,3,-48,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,8,47,27,
		-79,8,20,62,89,-80,0,69,88,-80,12,47,27,-79,12,18,62,89,-78,5,3,12,17,18,57,-78,10,3,12,17,18,57,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,24,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,55,51,17,35,17,6,35,34,2,53,23,20,22,51,50,55,17,38,35,34,6,91,-9,
		-52,-60,111,14,-96,-71,112,-70,-57,-6,-71,-86,-116,-90,86,88,-94,-114,-86,2,37,-11,1,52,-122,114,-6,38,2,4,120,1,53,-10,7,-82,-33,-109,2,17,-113,-33,0,2,0,93,-1,-20,3,-13,4,78,0,20,0,28,0,101,-78,8,29,30,17,18,57,-80,8,16,-80,21,-48,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,
		0,47,27,-79,0,18,62,89,-78,25,8,0,17,18,57,-80,25,47,-76,-65,25,-49,25,2,93,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,0,16,-79,16,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,0,39,39,52,54,54,51,50,18,21,21,33,22,22,51,50,55,23,6,
		1,34,6,7,33,53,52,38,2,113,-27,-2,-35,11,1,124,-35,-128,-43,-24,-3,36,8,-62,-103,-96,120,57,-125,-2,-18,115,-104,17,2,32,-119,20,1,23,-29,78,-101,-11,-118,-2,-2,-16,116,-99,-56,90,127,114,3,-54,-96,-106,25,-125,-102,0,0,2,0,96,-2,86,3,-14,4,78,0,26,0,37,0,127,-78,35,38,39,17,18,57,-80,35,16,-80,11,
		-48,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,0,69,88,-80,6,47,27,-79,6,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,20,62,89,-80,0,69,88,-80,23,47,27,-79,23,18,62,89,-78,5,3,23,17,18,57,-80,11,16,-79,17,1,-80,10,43,88,33,-40,27,-12,89,-78,21,3,23,17,18,57,-80,23,16,-79,30,1,-80,
		10,43,88,33,-40,27,-12,89,-80,3,16,-79,35,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,52,18,51,50,23,55,51,17,20,6,35,34,38,39,55,22,51,50,54,53,53,6,35,34,2,53,23,20,22,51,50,55,17,38,35,34,6,96,-24,-61,-54,112,16,-99,-11,-31,82,-81,65,55,122,-113,-107,-119,111,-64,-66,-21,-70,-107,-120,-81,82,85,
		-86,-119,-106,2,37,-6,1,47,-109,127,-4,5,-22,-1,45,41,-118,73,-89,-98,58,-128,1,50,-6,8,-75,-45,-96,1,-18,-101,-48,0,-1,-1,0,87,0,0,2,-122,5,-73,0,6,0,21,-83,0,0,3,0,103,-1,-16,4,-111,4,-99,0,29,0,38,0,50,0,-102,-78,44,51,52,17,18,57,-80,44,16,-80,14,-48,-80,44,16,-80,31,-48,0,-80,0,
		69,88,-80,13,47,27,-79,13,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,89,-78,42,13,26,17,18,57,-78,33,13,26,17,18,57,-78,7,42,33,17,18,57,-78,19,33,42,17,18,57,-80,0,16,-79,30,1,-80,10,43,88,33,-40,27,-12,89,-78,20,30,13,17,18,57,-78,22,13,
		0,17,18,57,-78,28,0,13,17,18,57,-78,25,20,28,17,18,57,-78,32,30,20,17,18,57,-80,13,16,-79,48,1,-80,10,43,88,33,-40,27,-12,89,48,49,5,34,38,53,52,54,55,55,39,38,53,52,54,51,50,22,21,20,7,7,1,54,53,51,20,7,23,35,39,6,39,50,55,1,7,6,21,20,22,3,20,23,23,55,54,53,52,38,
		35,34,6,1,-24,-85,-42,78,104,75,75,93,-83,-112,-122,-79,-101,73,1,12,69,-88,127,-57,-46,94,-105,-47,-111,106,-2,-37,100,76,107,21,63,54,66,83,72,66,56,72,16,-91,-127,86,-122,75,54,79,104,108,115,-108,-106,112,-112,111,52,-2,-29,116,-99,-32,-90,-46,97,113,-103,75,1,51,73,59,84,73,93,3,0,58,70,57,48,60,77,52,69,70,
		0,1,0,0,0,0,3,-117,4,-115,0,13,0,97,-78,0,14,15,17,18,57,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,13,4,10,17,18,57,-80,13,47,-79,0,2,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-80,4,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,1,
		16,-80,6,-48,-80,7,-48,-80,13,16,-80,12,-48,-80,9,-48,-80,8,-48,48,49,1,5,17,33,21,33,17,7,53,55,17,51,17,37,2,77,-2,-10,2,72,-4,-1,-118,-118,-71,1,10,2,-111,85,-2,91,-105,2,2,44,125,44,2,14,-2,44,85,0,2,0,9,0,0,5,-15,4,-115,0,15,0,18,0,-120,-78,5,19,20,17,18,57,-80,5,16,
		-80,17,-48,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,15,10,4,17,18,57,-80,15,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-78,17,10,4,17,18,57,-80,17,47,
		-79,6,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-78,18,10,4,17,18,57,48,49,1,33,19,33,21,33,3,33,3,35,1,33,21,33,19,33,5,33,3,5,-120,-2,53,14,2,38,-3,38,11,-2,102,-93,-58,2,-106,3,41,-3,-28,12,1,-48,-4,59,1,68,19,2,21,-2,-128,-107,1,45,
		-2,-45,4,-115,-106,-2,-76,-25,2,50,0,2,0,-118,0,0,3,-73,4,-115,0,12,0,21,0,89,-78,21,22,23,17,18,57,-80,21,16,-80,9,-48,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,11,47,27,-79,11,18,62,89,-78,2,0,11,17,18,57,-80,2,47,-78,15,0,11,17,18,57,-80,15,47,-79,9,1,-80,
		10,43,88,33,-40,27,-12,89,-80,2,16,-79,13,1,-80,10,43,88,33,-40,27,-12,89,48,49,19,51,21,51,22,22,21,20,6,35,35,21,35,19,17,51,50,54,53,52,38,39,-118,-71,-59,-60,-21,-22,-42,-76,-71,-71,-74,-128,-124,-120,119,4,-115,-53,4,-59,-90,-87,-66,-20,3,42,-2,90,108,98,96,119,1,0,3,0,96,-1,-57,4,90,4,-74,
		0,21,0,30,0,39,0,106,-78,6,40,41,17,18,57,-80,6,16,-80,27,-48,-80,6,16,-80,36,-48,0,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,24,17,6,17,18,57,-78,25,17,6,17,18,57,-80,17,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-78,33,17,6,17,18,57,
		-78,34,6,17,17,18,57,-80,6,16,-79,36,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,22,17,21,16,0,35,34,39,7,35,55,38,17,53,16,0,51,50,23,55,51,1,20,23,1,38,35,34,6,21,37,52,39,1,22,51,50,54,53,3,-42,-124,-2,-20,-24,-102,116,75,-107,127,-113,1,23,-27,-95,123,69,-107,-4,-59,61,1,-55,79,114,
		-106,-81,2,-116,52,-2,59,74,106,-100,-87,3,-4,-103,-2,-1,62,-2,-5,-2,-47,71,112,-66,-102,1,9,63,1,2,1,52,78,103,-3,110,-97,105,2,-86,59,-42,-59,3,-105,98,-3,92,52,-45,-57,0,0,2,0,48,0,0,4,-77,4,-115,0,19,0,23,0,-115,-78,3,24,25,17,18,57,-80,3,16,-80,20,-48,0,-80,0,69,88,-80,12,47,27,
		-79,12,28,62,89,-80,0,69,88,-80,16,47,27,-79,16,28,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,19,12,2,17,18,57,-80,19,47,-78,15,19,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,21,12,2,17,18,57,-80,21,47,-79,4,1,-80,10,43,88,33,-40,
		27,-12,89,-80,0,16,-80,8,-48,-80,19,16,-80,10,-48,-80,19,16,-80,14,-48,-80,0,16,-80,22,-48,48,49,1,35,17,35,17,33,17,35,17,35,53,51,53,51,21,33,53,51,21,51,1,33,53,33,4,-77,91,-71,-3,-92,-71,90,90,-71,2,92,-71,91,-4,-112,2,92,-3,-92,3,79,-4,-79,1,-14,-2,14,3,79,-105,-89,-89,-89,-89,-2,-92,
		-59,0,0,1,0,-118,-2,75,4,88,4,-115,0,19,0,91,-78,2,20,21,17,18,57,0,-80,0,69,88,-80,12,47,27,-79,12,28,62,89,-80,0,69,88,-80,15,47,27,-79,15,28,62,89,-80,0,69,88,-80,0,47,27,-79,0,20,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-80,0,16,-79,5,1,-80,10,43,88,33,-40,27,-12,
		89,-78,9,12,10,17,18,57,-78,14,10,12,17,18,57,48,49,1,34,39,55,22,51,50,53,53,1,17,35,17,51,1,17,51,17,20,6,3,23,60,52,13,35,64,-120,-3,-92,-71,-71,2,93,-72,-86,-2,75,18,-99,13,-61,81,3,107,-4,-108,4,-115,-4,-109,3,109,-5,26,-87,-77,-1,-1,0,37,2,31,2,13,2,-74,2,6,0,17,0,0,
		0,2,0,7,0,0,4,-28,5,-80,0,15,0,29,0,105,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,5,17,18,57,-80,4,47,-78,-49,4,1,93,-78,47,4,1,93,-78,-97,4,1,113,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,17,-48,-80,0,16,-79,18,1,-80,
		10,43,88,33,-40,27,-12,89,-80,5,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-80,28,-48,48,49,51,17,35,53,51,17,33,50,4,18,23,21,20,2,4,7,19,35,17,51,50,18,55,53,52,2,39,35,17,51,-57,-64,-64,1,-101,-66,1,36,-97,1,-97,-2,-39,-60,41,-4,-55,-34,-9,1,-23,-42,-32,-4,2,-102,-105,2,127,
		-88,-2,-54,-55,93,-50,-2,-54,-90,2,2,-102,-2,3,1,18,-7,93,-8,1,19,2,-2,31,0,2,0,7,0,0,4,-28,5,-80,0,15,0,29,0,105,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-78,4,0,5,17,18,57,-80,4,47,-78,-49,4,1,93,-78,47,4,1,93,-78,-97,4,
		1,113,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,17,-48,-80,0,16,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,5,16,-79,27,1,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-80,28,-48,48,49,51,17,35,53,51,17,33,50,4,18,23,21,20,2,4,7,19,35,17,51,50,18,55,53,52,2,39,35,17,51,-57,-64,-64,1,-101,
		-66,1,36,-97,1,-97,-2,-39,-60,41,-4,-55,-34,-9,1,-23,-42,-32,-4,2,-102,-105,2,127,-88,-2,-54,-55,93,-50,-2,-54,-90,2,2,-102,-2,3,1,18,-7,93,-8,1,19,2,-2,31,0,1,-1,-30,0,0,3,-3,6,0,0,25,0,108,0,-80,23,47,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-80,0,69,88,-80,16,47,27,-79,16,18,
		62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-78,47,23,1,93,-78,15,23,1,93,-78,21,16,23,17,18,57,-80,21,47,-79,18,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-78,2,16,4,17,18,57,-80,4,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-80,24,-48,48,49,1,35,17,54,51,32,19,17,35,17,
		38,38,35,34,6,7,17,35,17,35,53,51,53,51,21,51,2,94,-5,123,-59,1,87,3,-71,1,105,111,90,-120,38,-71,-56,-56,-71,-5,4,-46,-2,-27,-105,-2,125,-3,53,2,-52,117,112,96,78,-4,-3,4,-46,-105,-105,-105,0,1,0,49,0,0,4,-105,5,-80,0,15,0,78,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,0,69,88,
		-80,2,47,27,-79,2,18,62,89,-78,15,10,2,17,18,57,-80,15,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,15,16,-80,6,-48,-80,10,16,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,12,-48,48,49,1,35,17,35,17,35,53,51,17,33,53,33,21,33,17,51,3,-86,-25,-65,-42,-42,-2,45,4,102,-2,44,-25,3,
		55,-4,-55,3,55,-105,1,68,-98,-98,-2,-68,0,1,-1,-12,-1,-20,2,112,5,64,0,29,0,118,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-80,0,69,88,-80,17,47,27,-79,17,18,62,89,-80,1,16,-80,0,-48,-80,0,47,-80,1,16,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,5,-48,-80,5,47,-78,0,5,1,
		93,-79,8,1,-80,10,43,88,33,-40,27,-12,89,-80,17,16,-79,12,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,21,-48,-80,5,16,-80,24,-48,-80,4,16,-80,25,-48,-80,1,16,-80,28,-48,48,49,1,17,51,21,35,21,51,21,35,17,20,22,51,50,55,21,6,35,34,38,53,17,35,53,51,53,35,53,51,17,1,-121,-54,-54,-23,-23,
		54,65,32,56,73,69,124,126,-38,-38,-59,-59,5,64,-2,-6,-113,-70,-105,-2,-78,65,65,12,-106,20,-106,-118,1,78,-105,-70,-113,1,6,0,-1,-1,0,28,0,0,5,29,7,54,2,38,0,37,0,0,1,7,0,68,1,48,1,54,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,12,8,-12,48,49,-1,-1,0,28,0,0,5,29,
		7,54,2,38,0,37,0,0,1,7,0,117,1,-65,1,54,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,13,8,-12,48,49,-1,-1,0,28,0,0,5,29,7,54,2,38,0,37,0,0,1,7,0,-98,0,-55,1,54,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,15,6,-12,48,49,-1,-1,0,28,0,0,
		5,29,7,34,2,38,0,37,0,0,1,7,0,-91,0,-59,1,58,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,14,4,-12,48,49,-1,-1,0,28,0,0,5,29,6,-5,2,38,0,37,0,0,1,7,0,106,0,-7,1,54,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,17,4,-12,-80,27,-48,48,49,0,
		-1,-1,0,28,0,0,5,29,7,-111,2,38,0,37,0,0,1,7,0,-93,1,80,1,65,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,14,6,-12,-80,24,-48,48,49,0,-1,-1,0,28,0,0,5,29,7,-108,2,38,0,37,0,0,0,7,2,39,1,90,1,34,-1,-1,0,119,-2,68,4,-40,5,-60,2,38,0,39,0,0,
		0,7,0,121,1,-46,-1,-9,-1,-1,0,-87,0,0,4,70,7,66,2,38,0,41,0,0,1,7,0,68,0,-5,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,13,8,-12,48,49,-1,-1,0,-87,0,0,4,70,7,66,2,38,0,41,0,0,1,7,0,117,1,-118,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,
		6,30,62,89,-79,14,8,-12,48,49,-1,-1,0,-87,0,0,4,70,7,66,2,38,0,41,0,0,1,7,0,-98,0,-108,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,16,6,-12,48,49,-1,-1,0,-87,0,0,4,70,7,7,2,38,0,41,0,0,1,7,0,106,0,-60,1,66,0,23,0,-80,0,69,88,-80,6,47,
		27,-79,6,30,62,89,-79,18,4,-12,-80,27,-48,48,49,0,-1,-1,-1,-32,0,0,1,-127,7,66,2,38,0,45,0,0,1,7,0,68,-1,-89,1,66,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,5,8,-12,48,49,-1,-1,0,-80,0,0,2,81,7,66,2,38,0,45,0,0,1,7,0,117,0,53,1,66,0,20,0,-80,
		0,69,88,-80,3,47,27,-79,3,30,62,89,-79,6,8,-12,48,49,-1,-1,-1,-23,0,0,2,70,7,66,2,38,0,45,0,0,1,7,0,-98,-1,64,1,66,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,8,6,-12,48,49,-1,-1,-1,-43,0,0,2,94,7,7,2,38,0,45,0,0,1,7,0,106,-1,112,1,66,0,23,
		0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,10,4,-12,-80,20,-48,48,49,0,-1,-1,0,-87,0,0,5,8,7,34,2,38,0,50,0,0,1,7,0,-91,0,-5,1,58,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,13,4,-12,48,49,-1,-1,0,118,-1,-20,5,9,7,56,2,38,0,51,0,0,1,7,0,68,
		1,82,1,56,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,33,8,-12,48,49,-1,-1,0,118,-1,-20,5,9,7,56,2,38,0,51,0,0,1,7,0,117,1,-31,1,56,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,34,8,-12,48,49,-1,-1,0,118,-1,-20,5,9,7,56,2,38,0,51,0,0,1,7,
		0,-98,0,-21,1,56,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,34,6,-12,48,49,-1,-1,0,118,-1,-20,5,9,7,36,2,38,0,51,0,0,1,7,0,-91,0,-25,1,60,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,35,4,-12,48,49,-1,-1,0,118,-1,-20,5,9,6,-3,2,38,0,51,0,0,
		1,7,0,106,1,27,1,56,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,39,4,-12,-80,48,-48,48,49,0,-1,-1,0,-116,-1,-20,4,-86,7,54,2,38,0,57,0,0,1,7,0,68,1,43,1,54,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-79,20,8,-12,48,49,-1,-1,0,-116,-1,-20,4,-86,7,54,
		2,38,0,57,0,0,1,7,0,117,1,-70,1,54,0,20,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-79,21,8,-12,48,49,-1,-1,0,-116,-1,-20,4,-86,7,54,2,38,0,57,0,0,1,7,0,-98,0,-60,1,54,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-79,23,6,-12,48,49,-1,-1,0,-116,-1,-20,4,-86,
		6,-5,2,38,0,57,0,0,1,7,0,106,0,-12,1,54,0,23,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-79,25,4,-12,-80,35,-48,48,49,0,-1,-1,0,15,0,0,4,-69,7,54,2,38,0,61,0,0,1,7,0,117,1,-120,1,54,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-79,11,8,-12,48,49,-1,-1,
		0,109,-1,-20,3,-22,6,0,2,38,0,69,0,0,1,7,0,68,0,-43,0,0,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,42,9,-12,48,49,-1,-1,0,109,-1,-20,3,-22,6,0,2,38,0,69,0,0,1,7,0,117,1,100,0,0,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,43,9,-12,48,49,
		-1,-1,0,109,-1,-20,3,-22,6,0,2,38,0,69,0,0,1,6,0,-98,110,0,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,43,1,-12,48,49,-1,-1,0,109,-1,-20,3,-22,5,-20,2,38,0,69,0,0,1,6,0,-91,106,4,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,1,-12,48,49,-1,-1,
		0,109,-1,-20,3,-22,5,-59,2,38,0,69,0,0,1,7,0,106,0,-98,0,0,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,48,1,-12,-80,57,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,6,91,2,38,0,69,0,0,1,7,0,-93,0,-11,0,11,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,
		4,-12,-80,54,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,6,95,2,38,0,69,0,0,0,7,2,39,0,-1,-1,-19,-1,-1,0,92,-2,68,3,-20,4,78,2,38,0,71,0,0,0,7,0,121,1,63,-1,-9,-1,-1,0,93,-1,-20,3,-13,6,0,2,38,0,73,0,0,1,7,0,68,0,-59,0,0,0,20,0,-80,0,69,88,-80,8,47,
		27,-79,8,26,62,89,-79,31,9,-12,48,49,-1,-1,0,93,-1,-20,3,-13,6,0,2,38,0,73,0,0,1,7,0,117,1,84,0,0,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,32,9,-12,48,49,-1,-1,0,93,-1,-20,3,-13,6,0,2,38,0,73,0,0,1,6,0,-98,94,0,0,20,0,-80,0,69,88,-80,8,47,
		27,-79,8,26,62,89,-79,32,1,-12,48,49,-1,-1,0,93,-1,-20,3,-13,5,-59,2,38,0,73,0,0,1,7,0,106,0,-114,0,0,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,37,1,-12,-80,46,-48,48,49,0,-1,-1,-1,-58,0,0,1,103,5,-1,2,38,0,-115,0,0,1,6,0,68,-115,-1,0,20,0,-80,0,69,
		88,-80,2,47,27,-79,2,26,62,89,-79,5,9,-12,48,49,-1,-1,0,-106,0,0,2,55,5,-1,2,38,0,-115,0,0,1,6,0,117,27,-1,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,6,9,-12,48,49,-1,-1,-1,-49,0,0,2,44,5,-1,2,38,0,-115,0,0,1,7,0,-98,-1,38,-1,-1,0,20,0,-80,0,69,
		88,-80,2,47,27,-79,2,26,62,89,-79,8,1,-12,48,49,-1,-1,-1,-69,0,0,2,68,5,-60,2,38,0,-115,0,0,1,7,0,106,-1,86,-1,-1,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-79,11,1,-12,-80,20,-48,48,49,0,-1,-1,0,-116,0,0,3,-33,5,-20,2,38,0,82,0,0,1,6,0,-91,97,4,0,20,
		0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,21,1,-12,48,49,-1,-1,0,91,-1,-20,4,52,6,0,2,38,0,83,0,0,1,7,0,68,0,-49,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,29,9,-12,48,49,-1,-1,0,91,-1,-20,4,52,6,0,2,38,0,83,0,0,1,7,0,117,1,94,0,0,
		0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,30,9,-12,48,49,-1,-1,0,91,-1,-20,4,52,6,0,2,38,0,83,0,0,1,6,0,-98,104,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,30,1,-12,48,49,-1,-1,0,91,-1,-20,4,52,5,-20,2,38,0,83,0,0,1,6,0,-91,100,4,0,20,
		0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,31,1,-12,48,49,-1,-1,0,91,-1,-20,4,52,5,-59,2,38,0,83,0,0,1,7,0,106,0,-104,0,0,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,35,1,-12,-80,44,-48,48,49,0,-1,-1,0,-120,-1,-20,3,-36,6,0,2,38,0,89,0,0,1,7,0,68,
		0,-57,0,0,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,18,9,-12,48,49,-1,-1,0,-120,-1,-20,3,-36,6,0,2,38,0,89,0,0,1,7,0,117,1,86,0,0,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,19,9,-12,48,49,-1,-1,0,-120,-1,-20,3,-36,6,0,2,38,0,89,0,0,1,6,
		0,-98,96,0,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,21,1,-12,48,49,-1,-1,0,-120,-1,-20,3,-36,5,-59,2,38,0,89,0,0,1,7,0,106,0,-112,0,0,0,23,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,24,1,-12,-80,33,-48,48,49,0,-1,-1,0,22,-2,75,3,-80,6,0,2,38,0,93,
		0,0,1,7,0,117,1,27,0,0,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-79,18,9,-12,48,49,-1,-1,0,22,-2,75,3,-80,5,-59,2,38,0,93,0,0,1,6,0,106,85,0,0,23,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,23,1,-12,-80,32,-48,48,49,0,-1,-1,0,28,0,0,5,29,6,-29,
		2,38,0,37,0,0,1,7,0,112,0,-57,1,62,0,19,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,12,-36,48,49,0,-1,-1,0,109,-1,-20,3,-22,5,-83,2,38,0,69,0,0,1,6,0,112,108,8,0,19,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,42,-36,48,49,0,-1,-1,0,28,0,0,5,29,7,14,
		2,38,0,37,0,0,1,7,0,-95,0,-12,1,55,0,19,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,13,-36,48,49,0,-1,-1,0,109,-1,-20,3,-22,5,-40,2,38,0,69,0,0,1,7,0,-95,0,-103,0,1,0,19,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,43,-36,48,49,0,0,2,0,28,-2,79,5,29,
		5,-80,0,22,0,25,0,105,0,-80,0,69,88,-80,22,47,27,-79,22,30,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,20,62,89,-79,7,3,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,17,-48,-80,17,47,-78,23,20,22,17,18,57,-80,
		23,47,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-78,25,22,20,17,18,57,48,49,1,1,35,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,3,33,3,35,1,3,33,3,2,-16,2,45,38,58,113,78,48,52,13,70,90,89,103,-87,-121,-3,-98,-119,-58,2,44,-93,1,-17,-8,5,-80,-6,80,45,91,86,72,26,121,44,104,86,-112,
		108,1,115,-2,-124,5,-80,-4,106,2,-87,0,0,2,0,109,-2,79,3,-22,4,78,0,45,0,55,0,-108,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-80,0,69,88,-80,30,47,27,-79,30,18,62,89,-80,0,69,88,-80,41,47,27,-79,41,20,62,89,-80,30,16,-80,0,-48,-80,0,47,
		-78,2,4,23,17,18,57,-78,11,23,4,17,18,57,-80,11,47,-80,23,16,-79,15,1,-80,10,43,88,33,-40,27,-12,89,-78,18,11,23,17,18,57,-80,41,16,-79,36,3,-80,10,43,88,33,-40,27,-12,89,-80,4,16,-79,46,1,-80,10,43,88,33,-40,27,-12,89,-80,11,16,-79,51,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,38,39,6,
		35,34,38,53,52,36,51,51,53,52,38,35,34,6,21,35,52,54,54,51,50,22,23,17,20,23,21,35,7,6,21,20,51,50,55,23,6,35,34,38,53,52,39,50,54,55,53,35,32,21,20,22,3,36,15,7,-127,-77,-96,-51,1,1,-23,-76,116,113,99,-122,-70,115,-59,118,-69,-44,4,38,33,58,113,78,48,52,13,70,90,89,103,-120,87,-100,
		35,-111,-2,-84,116,7,38,69,-122,-75,-117,-87,-69,85,97,115,100,71,81,-105,88,-69,-92,-2,14,-107,88,16,45,91,86,72,26,121,44,104,86,-112,-16,90,72,-34,-57,87,98,0,-1,-1,0,119,-1,-20,4,-40,7,87,2,38,0,39,0,0,1,7,0,117,1,-58,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,31,8,-12,
		48,49,-1,-1,0,92,-1,-20,3,-20,6,0,2,38,0,71,0,0,1,7,0,117,1,51,0,0,0,20,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-79,32,9,-12,48,49,-1,-1,0,119,-1,-20,4,-40,7,87,2,38,0,39,0,0,1,7,0,-98,0,-48,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,31,
		6,-12,48,49,-1,-1,0,92,-1,-20,3,-20,6,0,2,38,0,71,0,0,1,6,0,-98,61,0,0,20,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-79,32,1,-12,48,49,-1,-1,0,119,-1,-20,4,-40,7,25,2,38,0,39,0,0,1,7,0,-94,1,-83,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,35,
		4,-12,48,49,-1,-1,0,92,-1,-20,3,-20,5,-62,2,38,0,71,0,0,1,7,0,-94,1,26,0,0,0,20,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-79,36,1,-12,48,49,-1,-1,0,119,-1,-20,4,-40,7,87,2,38,0,39,0,0,1,7,0,-97,0,-27,1,88,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,
		-79,33,6,-12,48,49,-1,-1,0,92,-1,-20,3,-20,6,0,2,38,0,71,0,0,1,6,0,-97,82,1,0,20,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-79,34,1,-12,48,49,-1,-1,0,-87,0,0,4,-58,7,66,2,38,0,40,0,0,1,7,0,-97,0,-98,1,67,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,
		-79,27,6,-12,48,49,-1,-1,0,95,-1,-20,5,43,6,2,0,38,0,72,0,0,1,7,1,-70,3,-44,5,19,0,72,0,-78,-16,31,1,114,-78,31,31,1,93,-78,-97,31,1,93,-78,31,31,1,113,-76,-49,31,-33,31,2,113,-78,-33,31,1,114,-78,95,31,1,114,-78,79,31,1,113,-78,-49,31,1,93,-76,79,31,95,31,2,93,-78,96,31,
		1,93,-78,-32,31,1,113,-78,-32,31,1,93,48,49,-1,-1,0,-87,0,0,4,70,6,-17,2,38,0,41,0,0,1,7,0,112,0,-110,1,74,0,19,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,13,-36,48,49,0,-1,-1,0,93,-1,-20,3,-13,5,-83,2,38,0,73,0,0,1,6,0,112,92,8,0,19,0,-80,0,69,88,-80,
		8,47,27,-79,8,26,62,89,-80,31,-36,48,49,0,-1,-1,0,-87,0,0,4,70,7,26,2,38,0,41,0,0,1,7,0,-95,0,-65,1,67,0,19,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,15,-36,48,49,0,-1,-1,0,93,-1,-20,3,-13,5,-40,2,38,0,73,0,0,1,7,0,-95,0,-119,0,1,0,19,0,-80,0,69,
		88,-80,8,47,27,-79,8,26,62,89,-80,33,-36,48,49,0,-1,-1,0,-87,0,0,4,70,7,4,2,38,0,41,0,0,1,7,0,-94,1,113,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,19,4,-12,48,49,-1,-1,0,93,-1,-20,3,-13,5,-62,2,38,0,73,0,0,1,7,0,-94,1,59,0,0,0,20,0,-80,
		0,69,88,-80,8,47,27,-79,8,26,62,89,-79,37,1,-12,48,49,0,1,0,-87,-2,79,4,70,5,-80,0,27,0,122,0,-80,0,69,88,-80,22,47,27,-79,22,30,62,89,-80,0,69,88,-80,21,47,27,-79,21,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,20,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-78,26,21,22,17,
		18,57,-80,26,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,21,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,15,16,-79,10,3,-80,10,43,88,33,-40,27,-12,89,-80,22,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,35,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,33,17,33,21,33,
		17,33,3,-32,-3,-119,2,-35,73,58,113,78,48,52,13,70,90,89,103,-101,-3,93,3,-109,-3,45,2,119,2,-95,-3,-4,-99,45,91,86,72,26,121,44,104,86,-118,105,5,-80,-98,-2,44,0,0,2,0,93,-2,104,3,-13,4,78,0,37,0,45,0,126,0,-80,0,69,88,-80,26,47,27,-79,26,26,62,89,-80,0,69,88,-80,13,47,27,-79,13,
		20,62,89,-80,0,69,88,-80,18,47,27,-79,18,18,62,89,-80,4,-48,-80,13,16,-79,8,3,-80,10,43,88,33,-40,27,-12,89,-78,42,18,26,17,18,57,-80,42,47,-76,-65,42,-49,42,2,93,-79,30,1,-80,10,43,88,33,-40,27,-12,89,-80,18,16,-79,34,1,-80,10,43,88,33,-40,27,-12,89,-78,37,18,26,17,18,57,-80,26,16,-79,38,
		1,-80,10,43,88,33,-40,27,-12,89,48,49,37,6,7,51,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,38,0,53,53,52,54,54,51,50,18,17,21,33,22,22,51,50,54,55,1,34,6,7,33,53,38,38,3,-27,71,115,1,58,113,78,48,52,13,70,90,89,103,98,-38,-2,-11,123,-35,-127,-45,-22,-3,35,4,-77,-118,98,-120,51,
		-2,-62,112,-104,18,2,30,8,-120,-67,110,54,45,91,86,72,26,121,44,104,86,108,90,4,1,33,-17,33,-95,-3,-113,-2,-22,-2,-3,77,-96,-59,80,66,2,-95,-93,-109,14,-115,-101,0,-1,-1,0,-87,0,0,4,70,7,66,2,38,0,41,0,0,1,7,0,-97,0,-87,1,67,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,17,
		6,-12,48,49,-1,-1,0,93,-1,-20,3,-13,6,0,2,38,0,73,0,0,1,6,0,-97,115,1,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,34,1,-12,48,49,-1,-1,0,122,-1,-20,4,-36,7,87,2,38,0,43,0,0,1,7,0,-98,0,-56,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,34,
		6,-12,48,49,-1,-1,0,96,-2,86,3,-14,6,0,2,38,0,75,0,0,1,6,0,-98,85,0,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,39,1,-12,48,49,-1,-1,0,122,-1,-20,4,-36,7,47,2,38,0,43,0,0,1,7,0,-95,0,-13,1,88,0,19,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,34,
		-36,48,49,0,-1,-1,0,96,-2,86,3,-14,5,-40,2,38,0,75,0,0,1,7,0,-95,0,-128,0,1,0,19,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,39,-36,48,49,0,-1,-1,0,122,-1,-20,4,-36,7,25,2,38,0,43,0,0,1,7,0,-94,1,-91,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,
		-79,39,4,-12,48,49,-1,-1,0,96,-2,86,3,-14,5,-62,2,38,0,75,0,0,1,7,0,-94,1,50,0,0,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,44,1,-12,48,49,-1,-1,0,122,-3,-10,4,-36,5,-60,2,38,0,43,0,0,0,7,1,-70,1,-38,-2,-105,-1,-1,0,96,-2,86,3,-14,6,-109,2,38,0,75,
		0,0,1,7,2,52,1,43,0,88,0,19,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-80,42,-36,48,49,0,-1,-1,0,-87,0,0,5,8,7,66,2,38,0,44,0,0,1,7,0,-98,0,-15,1,66,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-79,16,6,-12,48,49,-1,-1,0,-116,0,0,3,-33,7,65,2,38,
		0,76,0,0,1,7,0,-98,0,29,1,65,0,9,0,-80,17,47,-80,20,-36,48,49,0,-1,-1,-1,-73,0,0,2,122,7,46,2,38,0,45,0,0,1,7,0,-91,-1,60,1,70,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-79,7,4,-12,48,49,-1,-1,-1,-99,0,0,2,96,5,-22,2,38,0,-115,0,0,1,7,0,-91,
		-1,34,0,2,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,7,1,-12,48,49,-1,-1,-1,-52,0,0,2,108,6,-17,2,38,0,45,0,0,1,7,0,112,-1,62,1,74,0,19,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,5,-36,48,49,0,-1,-1,-1,-78,0,0,2,82,5,-85,2,38,0,-115,0,0,1,7,
		0,112,-1,36,0,6,0,19,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,5,-36,48,49,0,-1,-1,-1,-20,0,0,2,67,7,26,2,38,0,45,0,0,1,7,0,-95,-1,107,1,67,0,19,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-80,7,-36,48,49,0,-1,-1,-1,-46,0,0,2,41,5,-41,2,38,0,-115,0,0,
		1,7,0,-95,-1,81,0,0,0,19,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-80,7,-36,48,49,0,-1,-1,0,24,-2,88,1,120,5,-80,2,38,0,45,0,0,0,6,0,-92,-26,9,-1,-1,-1,-5,-2,79,1,104,5,-60,2,38,0,77,0,0,0,6,0,-92,-55,0,-1,-1,0,-87,0,0,1,-124,7,4,2,38,0,45,0,0,
		1,7,0,-94,0,28,1,66,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,11,4,-12,48,49,-1,-1,0,-73,-1,-20,5,-7,5,-80,0,38,0,45,0,0,0,7,0,46,2,45,0,0,-1,-1,0,-115,-2,75,3,74,5,-60,0,38,0,77,0,0,0,7,0,78,1,-15,0,0,-1,-1,0,53,-1,-20,4,-126,7,53,2,38,
		0,46,0,0,1,7,0,-98,1,124,1,53,0,20,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-79,20,6,-12,48,49,-1,-1,-1,-76,-2,75,2,57,5,-40,2,38,0,-100,0,0,1,7,0,-98,-1,51,-1,-40,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,18,4,-12,48,49,-1,-1,0,-87,-2,88,5,5,5,-80,
		2,38,0,47,0,0,0,7,1,-70,1,-108,-2,-7,-1,-1,0,-115,-2,69,4,12,6,0,2,38,0,79,0,0,0,7,1,-70,1,17,-2,-26,-1,-1,0,-95,0,0,4,28,7,49,2,38,0,48,0,0,1,7,0,117,0,38,1,49,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,8,8,-12,48,49,-1,-1,0,-109,0,0,
		2,52,7,-106,2,38,0,80,0,0,1,7,0,117,0,24,1,-106,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,32,62,89,-79,6,9,-12,48,49,-1,-1,0,-87,-2,9,4,28,5,-80,2,38,0,48,0,0,0,7,1,-70,1,108,-2,-86,-1,-1,0,87,-2,9,1,85,6,0,2,38,0,80,0,0,0,7,1,-70,-1,-5,-2,-86,-1,-1,
		0,-87,0,0,4,28,5,-79,2,38,0,48,0,0,1,7,1,-70,1,-43,4,-62,0,16,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,48,49,-1,-1,0,-100,0,0,2,-83,6,2,0,38,0,80,0,0,1,7,1,-70,1,86,5,19,0,80,0,-78,31,8,1,93,-78,-97,8,1,93,-76,31,8,47,8,2,113,-78,-81,8,1,113,-76,
		47,8,63,8,2,114,-78,-33,8,1,114,-74,95,8,111,8,127,8,3,114,-76,-49,8,-33,8,2,113,-78,79,8,1,113,-78,-49,8,1,93,-76,79,8,95,8,2,93,-78,96,8,1,93,-78,-16,8,1,114,48,49,-1,-1,0,-87,0,0,4,28,5,-80,2,38,0,48,0,0,0,7,0,-94,1,-68,-3,-59,-1,-1,0,-100,0,0,2,-96,6,0,
		0,38,0,80,0,0,0,7,0,-94,1,56,-3,-74,-1,-1,0,-87,0,0,5,8,7,54,2,38,0,50,0,0,1,7,0,117,1,-11,1,54,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,12,8,-12,48,49,-1,-1,0,-116,0,0,3,-33,6,0,2,38,0,82,0,0,1,7,0,117,1,91,0,0,0,20,0,-80,0,69,
		88,-80,3,47,27,-79,3,26,62,89,-79,20,9,-12,48,49,-1,-1,0,-87,-2,9,5,8,5,-80,2,38,0,50,0,0,0,7,1,-70,1,-48,-2,-86,-1,-1,0,-116,-2,9,3,-33,4,78,2,38,0,82,0,0,0,7,1,-70,1,51,-2,-86,-1,-1,0,-87,0,0,5,8,7,54,2,38,0,50,0,0,1,7,0,-97,1,20,1,55,0,20,
		0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,15,6,-12,48,49,-1,-1,0,-116,0,0,3,-33,6,0,2,38,0,82,0,0,1,6,0,-97,122,1,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,22,1,-12,48,49,-1,-1,-1,-68,0,0,3,-33,6,4,2,38,0,82,0,0,1,7,1,-70,-1,96,5,21,0,16,
		0,-80,23,47,-78,79,23,1,93,-78,-97,23,1,93,48,49,-1,-1,0,118,-1,-20,5,9,6,-27,2,38,0,51,0,0,1,7,0,112,0,-23,1,64,0,19,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,33,-36,48,49,0,-1,-1,0,91,-1,-20,4,52,5,-83,2,38,0,83,0,0,1,6,0,112,102,8,0,19,0,-80,0,69,
		88,-80,4,47,27,-79,4,26,62,89,-80,29,-36,48,49,0,-1,-1,0,118,-1,-20,5,9,7,16,2,38,0,51,0,0,1,7,0,-95,1,22,1,57,0,19,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,34,-36,48,49,0,-1,-1,0,91,-1,-20,4,52,5,-40,2,38,0,83,0,0,1,7,0,-95,0,-109,0,1,0,19,0,-80,
		0,69,88,-80,4,47,27,-79,4,26,62,89,-80,31,-36,48,49,0,-1,-1,0,118,-1,-20,5,9,7,55,2,38,0,51,0,0,1,7,0,-90,1,107,1,56,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,38,8,-12,-80,34,-48,48,49,0,-1,-1,0,91,-1,-20,4,52,5,-1,2,38,0,83,0,0,1,7,0,-90,0,-24,
		0,0,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,34,9,-12,-80,30,-48,48,49,0,-1,-1,0,-88,0,0,4,-55,7,54,2,38,0,54,0,0,1,7,0,117,1,-128,1,54,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,26,8,-12,48,49,-1,-1,0,-116,0,0,2,-46,6,0,2,38,0,86,0,0,
		1,7,0,117,0,-74,0,0,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,16,9,-12,48,49,-1,-1,0,-88,-2,9,4,-55,5,-80,2,38,0,54,0,0,0,7,1,-70,1,99,-2,-86,-1,-1,0,83,-2,9,2,-105,4,78,2,38,0,86,0,0,0,7,1,-70,-1,-9,-2,-86,-1,-1,0,-88,0,0,4,-55,7,54,2,38,
		0,54,0,0,1,7,0,-97,0,-97,1,55,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,29,6,-12,48,49,-1,-1,0,99,0,0,2,-51,6,0,2,38,0,86,0,0,1,6,0,-97,-42,1,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,18,1,-12,48,49,-1,-1,0,80,-1,-20,4,114,7,56,2,38,
		0,55,0,0,1,7,0,117,1,-115,1,56,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,41,8,-12,48,49,-1,-1,0,95,-1,-20,3,-69,6,0,2,38,0,87,0,0,1,7,0,117,1,81,0,0,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-79,41,9,-12,48,49,-1,-1,0,80,-1,-20,4,114,7,56,
		2,38,0,55,0,0,1,7,0,-98,0,-105,1,56,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,41,6,-12,48,49,-1,-1,0,95,-1,-20,3,-69,6,0,2,38,0,87,0,0,1,6,0,-98,91,0,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-79,41,1,-12,48,49,-1,-1,0,80,-2,77,4,114,5,-60,
		2,38,0,55,0,0,0,7,0,121,1,-97,0,0,-1,-1,0,95,-2,69,3,-69,4,78,2,38,0,87,0,0,0,7,0,121,1,93,-1,-8,-1,-1,0,80,-3,-1,4,114,5,-60,2,38,0,55,0,0,0,7,1,-70,1,117,-2,-96,-1,-1,0,95,-3,-10,3,-69,4,78,2,38,0,87,0,0,0,7,1,-70,1,51,-2,-105,-1,-1,0,80,
		-1,-20,4,114,7,56,2,38,0,55,0,0,1,7,0,-97,0,-84,1,57,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,43,6,-12,48,49,-1,-1,0,95,-1,-20,3,-69,6,0,2,38,0,87,0,0,1,6,0,-97,112,1,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-79,43,1,-12,48,49,-1,-1,0,49,
		-3,-1,4,-105,5,-80,2,38,0,56,0,0,0,7,1,-70,1,102,-2,-96,-1,-1,0,9,-3,-1,2,86,5,64,2,38,0,88,0,0,0,7,1,-70,0,-59,-2,-96,-1,-1,0,49,-2,77,4,-105,5,-80,2,38,0,56,0,0,0,7,0,121,1,-112,0,0,-1,-1,0,9,-2,77,2,-103,5,64,2,38,0,88,0,0,0,7,0,121,0,-17,
		0,0,-1,-1,0,49,0,0,4,-105,7,54,2,38,0,56,0,0,1,7,0,-97,0,-95,1,55,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,13,6,-12,48,49,-1,-1,0,9,-1,-20,2,-20,6,121,0,38,0,88,0,0,1,7,1,-70,1,-107,5,-118,0,18,0,-78,15,26,1,93,-78,-97,26,1,93,-78,79,26,1,93,
		48,49,-1,-1,0,-116,-1,-20,4,-86,7,34,2,38,0,57,0,0,1,7,0,-91,0,-64,1,58,0,20,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-79,22,4,-12,48,49,-1,-1,0,-120,-1,-20,3,-36,5,-20,2,38,0,89,0,0,1,6,0,-91,92,4,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,20,1,-12,
		48,49,-1,-1,0,-116,-1,-20,4,-86,6,-29,2,38,0,57,0,0,1,7,0,112,0,-62,1,62,0,19,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-80,19,-36,48,49,0,-1,-1,0,-120,-1,-20,3,-36,5,-83,2,38,0,89,0,0,1,6,0,112,94,8,0,19,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,18,-36,48,
		49,0,-1,-1,0,-116,-1,-20,4,-86,7,14,2,38,0,57,0,0,1,7,0,-95,0,-17,1,55,0,19,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-80,22,-36,48,49,0,-1,-1,0,-120,-1,-20,3,-36,5,-40,2,38,0,89,0,0,1,7,0,-95,0,-117,0,1,0,19,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,20,
		-36,48,49,0,-1,-1,0,-116,-1,-20,4,-86,7,-111,2,38,0,57,0,0,1,7,0,-93,1,75,1,65,0,23,0,-80,0,69,88,-80,10,47,27,-79,10,30,62,89,-79,22,6,-12,-80,32,-48,48,49,0,-1,-1,0,-120,-1,-20,3,-36,6,91,2,38,0,89,0,0,1,7,0,-93,0,-25,0,11,0,23,0,-80,0,69,88,-80,7,47,27,-79,
		7,26,62,89,-79,20,4,-12,-80,30,-48,48,49,0,-1,-1,0,-116,-1,-20,4,-86,7,53,2,38,0,57,0,0,1,7,0,-90,1,68,1,54,0,23,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-79,21,8,-12,-80,25,-48,48,49,0,-1,-1,0,-120,-1,-20,4,12,5,-1,2,38,0,89,0,0,1,7,0,-90,0,-32,0,0,0,23,
		0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,19,9,-12,-80,23,-48,48,49,0,0,1,0,-116,-2,123,4,-86,5,-80,0,32,0,85,0,-80,0,69,88,-80,24,47,27,-79,24,30,62,89,-80,0,69,88,-80,13,47,27,-79,13,20,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-80,24,16,-80,32,-48,-78,4,19,32,17,18,
		57,-80,13,16,-79,8,3,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,6,6,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,7,34,0,39,17,51,17,20,22,51,50,54,53,17,4,-86,1,-118,-125,-101,78,48,52,13,70,90,89,103,79,22,-17,-2,-28,2,-66,-82,-95,-93,
		-83,5,-80,-4,33,-108,-30,59,114,96,72,26,121,44,104,86,97,83,1,1,2,-30,3,-32,-4,38,-98,-81,-82,-98,3,-37,0,1,0,-120,-2,79,3,-26,4,58,0,31,0,111,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,0,69,88,-80,29,47,27,-79,29,26,62,89,-80,0,69,88,-80,31,47,27,-79,31,18,62,89,-80,0,69,88,
		-80,18,47,27,-79,18,18,62,89,-80,0,69,88,-80,10,47,27,-79,10,20,62,89,-79,5,3,-80,10,43,88,33,-40,27,-12,89,-80,31,16,-80,15,-48,-80,15,47,-78,16,18,29,17,18,57,-80,18,16,-79,26,1,-80,10,43,88,33,-40,27,-12,89,48,49,33,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,39,6,35,34,38,39,17,
		51,17,20,51,50,55,17,51,17,3,-46,58,113,78,48,52,13,70,90,89,103,-90,4,108,-47,-83,-75,1,-71,-56,-44,70,-71,45,91,86,72,26,121,44,104,86,-113,106,101,127,-55,-59,2,-64,-3,69,-10,-98,3,19,-5,-58,-1,-1,0,61,0,0,6,-19,7,54,2,38,0,59,0,0,1,7,0,-98,1,-59,1,54,0,20,0,-80,0,69,88,-80,
		3,47,27,-79,3,30,62,89,-79,23,6,-12,48,49,-1,-1,0,43,0,0,5,-45,6,0,2,38,0,91,0,0,1,7,0,-98,1,36,0,0,0,20,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,15,1,-12,48,49,-1,-1,0,15,0,0,4,-69,7,54,2,38,0,61,0,0,1,7,0,-98,0,-110,1,54,0,20,0,-80,0,69,
		88,-80,1,47,27,-79,1,30,62,89,-79,11,6,-12,48,49,-1,-1,0,22,-2,75,3,-80,6,0,2,38,0,93,0,0,1,6,0,-98,37,0,0,20,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,20,1,-12,48,49,-1,-1,0,15,0,0,4,-69,6,-5,2,38,0,61,0,0,1,7,0,106,0,-62,1,54,0,23,0,-80,0,69,
		88,-80,8,47,27,-79,8,30,62,89,-79,16,4,-12,-80,25,-48,48,49,0,-1,-1,0,86,0,0,4,122,7,54,2,38,0,62,0,0,1,7,0,117,1,-121,1,54,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-79,12,8,-12,48,49,-1,-1,0,88,0,0,3,-77,6,0,2,38,0,94,0,0,1,7,0,117,1,33,0,0,
		0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,12,9,-12,48,49,-1,-1,0,86,0,0,4,122,6,-8,2,38,0,62,0,0,1,7,0,-94,1,110,1,54,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-79,17,4,-12,48,49,-1,-1,0,88,0,0,3,-77,5,-62,2,38,0,94,0,0,1,7,0,-94,1,8,
		0,0,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,17,1,-12,48,49,-1,-1,0,86,0,0,4,122,7,54,2,38,0,62,0,0,1,7,0,-97,0,-90,1,55,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-79,15,6,-12,48,49,-1,-1,0,88,0,0,3,-77,6,0,2,38,0,94,0,0,1,6,0,-97,
		64,1,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,15,1,-12,48,49,-1,-1,-1,-14,0,0,7,87,7,66,2,38,0,-127,0,0,1,7,0,117,2,-55,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,21,8,-12,48,49,-1,-1,0,78,-1,-20,6,124,6,1,2,38,0,-122,0,0,1,7,0,117,
		2,122,0,1,0,20,0,-80,0,69,88,-80,29,47,27,-79,29,26,62,89,-79,64,9,-12,48,49,-1,-1,0,118,-1,-93,5,29,7,-128,2,38,0,-125,0,0,1,7,0,117,1,-23,1,-128,0,20,0,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-79,44,8,-12,48,49,-1,-1,0,91,-1,122,4,52,6,0,2,38,0,-119,0,0,1,7,
		0,117,1,55,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,41,9,-12,48,49,-1,-1,-1,-66,0,0,4,31,4,-115,2,38,2,48,0,0,1,7,2,38,-1,47,-1,120,0,44,0,-78,31,24,1,113,-76,-33,24,-17,24,2,113,-76,31,24,47,24,2,93,-78,31,24,1,114,-78,79,24,1,113,-76,-17,24,-1,24,2,
		93,-78,95,24,1,93,48,49,-1,-1,-1,-66,0,0,4,31,4,-115,2,38,2,48,0,0,1,7,2,38,-1,47,-1,120,0,54,0,-76,-17,23,-1,23,2,93,-78,79,23,1,113,-78,31,23,1,114,-78,-33,23,1,114,-78,111,23,1,114,-76,-33,23,-17,23,2,113,-78,31,23,1,113,-78,95,23,1,93,-76,31,23,47,23,2,93,48,49,-1,-1,
		0,40,0,0,3,-3,4,-115,2,38,1,-40,0,0,1,6,2,38,69,-32,0,13,0,-78,3,10,1,93,-78,-80,10,1,93,48,49,0,-1,-1,0,19,0,0,4,112,6,30,2,38,2,51,0,0,1,7,0,68,0,-43,0,30,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,12,6,-12,48,49,-1,-1,0,19,0,0,4,112,
		6,30,2,38,2,51,0,0,1,7,0,117,1,100,0,30,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-79,13,6,-12,48,49,-1,-1,0,19,0,0,4,112,6,30,2,38,2,51,0,0,1,6,0,-98,110,30,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,15,4,-12,48,49,-1,-1,0,19,0,0,4,112,
		6,10,2,38,2,51,0,0,1,6,0,-91,106,34,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-79,14,2,-12,48,49,-1,-1,0,19,0,0,4,112,5,-29,2,38,2,51,0,0,1,7,0,106,0,-98,0,30,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,18,2,-12,-80,27,-48,48,49,0,-1,-1,0,19,
		0,0,4,112,6,121,2,38,2,51,0,0,1,7,0,-93,0,-11,0,41,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,14,6,-12,-80,24,-48,48,49,0,-1,-1,0,19,0,0,4,112,6,124,2,38,2,51,0,0,0,7,2,39,0,-1,0,10,-1,-1,0,96,-2,74,4,48,4,-99,2,38,2,49,0,0,0,7,0,121,
		1,116,-1,-3,-1,-1,0,-118,0,0,3,-82,6,30,2,38,2,40,0,0,1,7,0,68,0,-88,0,30,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,13,6,-12,48,49,-1,-1,0,-118,0,0,3,-82,6,30,2,38,2,40,0,0,1,7,0,117,1,55,0,30,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,
		-79,14,6,-12,48,49,-1,-1,0,-118,0,0,3,-82,6,30,2,38,2,40,0,0,1,6,0,-98,65,30,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,16,4,-12,48,49,-1,-1,0,-118,0,0,3,-82,5,-29,2,38,2,40,0,0,1,6,0,106,113,30,0,23,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,19,
		2,-12,-80,28,-48,48,49,0,-1,-1,-1,-66,0,0,1,95,6,30,2,38,1,-29,0,0,1,6,0,68,-123,30,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,5,6,-12,48,49,-1,-1,0,-114,0,0,2,47,6,30,2,38,1,-29,0,0,1,6,0,117,19,30,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,
		-79,6,6,-12,48,49,-1,-1,-1,-57,0,0,2,36,6,30,2,38,1,-29,0,0,1,7,0,-98,-1,30,0,30,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,8,4,-12,48,49,-1,-1,-1,-77,0,0,2,60,5,-29,2,38,1,-29,0,0,1,7,0,106,-1,78,0,30,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,28,
		62,89,-79,11,2,-12,-80,20,-48,48,49,0,-1,-1,0,-118,0,0,4,88,6,10,2,38,1,-34,0,0,1,7,0,-91,0,-107,0,34,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,13,2,-12,48,49,-1,-1,0,96,-1,-16,4,90,6,30,2,38,1,-35,0,0,1,7,0,68,0,-18,0,30,0,20,0,-80,0,69,88,-80,
		10,47,27,-79,10,28,62,89,-79,29,6,-12,48,49,-1,-1,0,96,-1,-16,4,90,6,30,2,38,1,-35,0,0,1,7,0,117,1,125,0,30,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,30,6,-12,48,49,-1,-1,0,96,-1,-16,4,90,6,30,2,38,1,-35,0,0,1,7,0,-98,0,-121,0,30,0,20,0,-80,0,69,
		88,-80,10,47,27,-79,10,28,62,89,-79,32,4,-12,48,49,-1,-1,0,96,-1,-16,4,90,6,10,2,38,1,-35,0,0,1,7,0,-91,0,-125,0,34,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,31,2,-12,48,49,-1,-1,0,96,-1,-16,4,90,5,-29,2,38,1,-35,0,0,1,7,0,106,0,-73,0,30,0,23,0,-80,
		0,69,88,-80,10,47,27,-79,10,28,62,89,-79,35,2,-12,-80,44,-48,48,49,0,-1,-1,0,116,-1,-16,4,10,6,30,2,38,1,-41,0,0,1,7,0,68,0,-49,0,30,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,19,6,-12,48,49,-1,-1,0,116,-1,-16,4,10,6,30,2,38,1,-41,0,0,1,7,0,117,1,94,
		0,30,0,20,0,-80,0,69,88,-80,17,47,27,-79,17,28,62,89,-79,20,6,-12,48,49,-1,-1,0,116,-1,-16,4,10,6,30,2,38,1,-41,0,0,1,6,0,-98,104,30,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,22,4,-12,48,49,-1,-1,0,116,-1,-16,4,10,5,-29,2,38,1,-41,0,0,1,7,0,106,0,-104,
		0,30,0,23,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,25,2,-12,-80,34,-48,48,49,0,-1,-1,0,13,0,0,4,28,6,30,2,38,1,-45,0,0,1,7,0,117,1,51,0,30,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-79,11,6,-12,48,49,-1,-1,0,19,0,0,4,112,5,-53,2,38,2,51,0,0,
		1,6,0,112,108,38,0,19,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-80,12,-36,48,49,0,-1,-1,0,19,0,0,4,112,5,-10,2,38,2,51,0,0,1,7,0,-95,0,-103,0,31,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,14,8,-12,48,49,0,2,0,19,-2,79,4,112,4,-115,0,22,0,25,0,105,
		0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,1,47,27,-79,1,18,62,89,-80,0,69,88,-80,12,47,27,-79,12,20,62,89,-79,7,3,-80,10,43,88,33,-40,27,-12,89,-80,1,16,-80,17,-48,-80,17,47,-78,23,20,0,17,18,57,-80,23,47,-79,19,1,-80,10,43,
		88,33,-40,27,-12,89,-78,25,0,20,17,18,57,48,49,1,1,35,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,3,33,3,35,1,3,33,3,2,-104,1,-40,38,58,113,78,48,52,13,70,90,89,103,-80,104,-3,-8,110,-67,1,-33,120,1,-111,-57,4,-115,-5,115,45,91,86,72,26,121,44,104,86,-108,108,1,10,-2,-23,4,-115,-3,
		33,1,-3,0,-1,-1,0,96,-1,-16,4,48,6,30,2,38,2,49,0,0,1,7,0,117,1,105,0,30,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-79,31,6,-12,48,49,-1,-1,0,96,-1,-16,4,48,6,30,2,38,2,49,0,0,1,6,0,-98,115,30,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-79,33,
		4,-12,48,49,-1,-1,0,96,-1,-16,4,48,5,-32,2,38,2,49,0,0,1,7,0,-94,1,80,0,30,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,-79,35,2,-12,48,49,-1,-1,0,96,-1,-16,4,48,6,30,2,38,2,49,0,0,1,7,0,-97,0,-120,0,31,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,28,62,89,
		-79,33,6,-12,48,49,-1,-1,0,-118,0,0,4,31,6,30,2,38,2,48,0,0,1,6,0,-97,49,31,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,28,62,89,-79,26,6,-12,48,49,-1,-1,0,-118,0,0,3,-82,5,-53,2,38,2,40,0,0,1,6,0,112,63,38,0,19,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-80,13,
		-36,48,49,0,-1,-1,0,-118,0,0,3,-82,5,-10,2,38,2,40,0,0,1,6,0,-95,108,31,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,15,8,-12,48,49,-1,-1,0,-118,0,0,3,-82,5,-32,2,38,2,40,0,0,1,7,0,-94,1,30,0,30,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,19,
		2,-12,48,49,0,1,0,-118,-2,79,3,-82,4,-115,0,27,0,124,0,-80,0,69,88,-80,22,47,27,-79,22,28,62,89,-80,0,69,88,-80,20,47,27,-79,20,18,62,89,-80,0,69,88,-80,15,47,27,-79,15,20,62,89,-80,20,16,-80,27,-48,-80,27,47,-78,31,27,1,93,-78,-33,27,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,
		20,16,-79,2,1,-80,10,43,88,33,-40,27,-12,89,-80,20,16,-80,5,-48,-80,15,16,-79,10,3,-80,10,43,88,33,-40,27,-12,89,-80,22,16,-79,25,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,33,21,35,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,33,17,33,21,33,17,33,3,87,-3,-20,2,107,61,58,113,78,
		48,52,13,70,90,89,103,-101,-3,-54,3,30,-3,-101,2,20,2,14,-2,-119,-105,45,91,86,72,26,121,44,104,86,-118,105,4,-115,-103,-2,-78,0,-1,-1,0,-118,0,0,3,-82,6,30,2,38,2,40,0,0,1,6,0,-97,86,31,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,17,6,-12,48,49,-1,-1,0,99,-1,-16,4,53,
		6,30,2,38,1,-27,0,0,1,6,0,-98,113,30,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,32,4,-12,48,49,-1,-1,0,99,-1,-16,4,53,5,-10,2,38,1,-27,0,0,1,7,0,-95,0,-100,0,31,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,32,8,-12,48,49,-1,-1,0,99,-1,-16,4,53,
		5,-32,2,38,1,-27,0,0,1,7,0,-94,1,78,0,30,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,37,2,-12,48,49,-1,-1,0,99,-3,-4,4,53,4,-99,2,38,1,-27,0,0,0,7,1,-70,1,79,-2,-99,-1,-1,0,-118,0,0,4,88,6,30,2,38,1,-28,0,0,1,7,0,-98,0,-112,0,30,0,20,0,-80,
		0,69,88,-80,7,47,27,-79,7,28,62,89,-79,16,4,-12,48,49,-1,-1,-1,-107,0,0,2,88,6,10,2,38,1,-29,0,0,1,7,0,-91,-1,26,0,34,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-79,7,2,-12,48,49,-1,-1,-1,-86,0,0,2,74,5,-53,2,38,1,-29,0,0,1,7,0,112,-1,28,0,38,0,19,
		0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-80,5,-36,48,49,0,-1,-1,-1,-54,0,0,2,33,5,-10,2,38,1,-29,0,0,1,7,0,-95,-1,73,0,31,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,7,8,-12,48,49,-1,-1,0,6,-2,79,1,102,4,-115,2,38,1,-29,0,0,0,6,0,-92,-44,0,-1,-1,
		0,-120,0,0,1,99,5,-32,2,38,1,-29,0,0,1,6,0,-94,-5,30,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,11,2,-12,48,49,-1,-1,0,43,-1,-16,4,13,6,30,2,38,1,-30,0,0,1,7,0,-98,1,7,0,30,0,20,0,-80,0,69,88,-80,0,47,27,-79,0,28,62,89,-79,20,4,-12,48,49,-1,-1,
		0,-118,-2,5,4,87,4,-115,2,38,1,-31,0,0,0,7,1,-70,1,20,-2,-90,-1,-1,0,-126,0,0,3,-117,6,30,2,38,1,-32,0,0,1,6,0,117,7,30,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-79,8,6,-12,48,49,-1,-1,0,-118,-2,7,3,-117,4,-115,2,38,1,-32,0,0,0,7,1,-70,1,16,-2,-88,
		-1,-1,0,-118,0,0,3,-117,4,-114,2,38,1,-32,0,0,1,7,1,-70,1,126,3,-97,0,16,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,48,49,-1,-1,0,-118,0,0,3,-117,4,-115,2,38,1,-32,0,0,0,7,0,-94,1,102,-3,55,-1,-1,0,-118,0,0,4,88,6,30,2,38,1,-34,0,0,1,7,0,117,1,-113,0,30,
		0,20,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,12,6,-12,48,49,-1,-1,0,-118,-2,3,4,88,4,-115,2,38,1,-34,0,0,0,7,1,-70,1,108,-2,-92,-1,-1,0,-118,0,0,4,88,6,30,2,38,1,-34,0,0,1,7,0,-97,0,-82,0,31,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,15,6,-12,
		48,49,-1,-1,0,96,-1,-16,4,90,5,-53,2,38,1,-35,0,0,1,7,0,112,0,-123,0,38,0,19,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-80,29,-36,48,49,0,-1,-1,0,96,-1,-16,4,90,5,-10,2,38,1,-35,0,0,1,7,0,-95,0,-78,0,31,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,31,
		8,-12,48,49,-1,-1,0,96,-1,-16,4,90,6,29,2,38,1,-35,0,0,1,7,0,-90,1,7,0,30,0,23,0,-80,0,69,88,-80,10,47,27,-79,10,28,62,89,-79,30,6,-12,-80,34,-48,48,49,0,-1,-1,0,-118,0,0,4,37,6,30,2,38,1,-38,0,0,1,7,0,117,1,39,0,30,0,20,0,-80,0,69,88,-80,5,47,27,-79,
		5,28,62,89,-79,25,6,-12,48,49,-1,-1,0,-118,-2,7,4,37,4,-115,2,38,1,-38,0,0,0,7,1,-70,1,13,-2,-88,-1,-1,0,-118,0,0,4,37,6,30,2,38,1,-38,0,0,1,6,0,-97,70,31,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,28,6,-12,48,49,-1,-1,0,67,-1,-16,3,-35,6,30,2,38,
		1,-39,0,0,1,7,0,117,1,62,0,30,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,40,6,-12,48,49,-1,-1,0,67,-1,-16,3,-35,6,30,2,38,1,-39,0,0,1,6,0,-98,72,30,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,42,4,-12,48,49,-1,-1,0,67,-2,77,3,-35,4,-99,2,38,
		1,-39,0,0,0,7,0,121,1,83,0,0,-1,-1,0,67,-1,-16,3,-35,6,30,2,38,1,-39,0,0,1,6,0,-97,93,31,0,20,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,42,6,-12,48,49,-1,-1,0,40,-2,1,3,-3,4,-115,2,38,1,-40,0,0,0,7,1,-70,1,20,-2,-94,-1,-1,0,40,0,0,3,-3,6,30,
		2,38,1,-40,0,0,1,6,0,-97,80,31,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,13,6,-12,48,49,-1,-1,0,40,-2,79,3,-3,4,-115,2,38,1,-40,0,0,0,7,0,121,1,62,0,2,-1,-1,0,116,-1,-16,4,10,6,10,2,38,1,-41,0,0,1,6,0,-91,100,34,0,20,0,-80,0,69,88,-80,17,47,
		27,-79,17,28,62,89,-79,21,2,-12,48,49,-1,-1,0,116,-1,-16,4,10,5,-53,2,38,1,-41,0,0,1,6,0,112,102,38,0,19,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,19,-36,48,49,0,-1,-1,0,116,-1,-16,4,10,5,-10,2,38,1,-41,0,0,1,7,0,-95,0,-109,0,31,0,20,0,-80,0,69,88,-80,9,47,
		27,-79,9,28,62,89,-79,21,8,-12,48,49,-1,-1,0,116,-1,-16,4,10,6,121,2,38,1,-41,0,0,1,7,0,-93,0,-17,0,41,0,23,0,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-79,21,6,-12,-80,31,-48,48,49,0,-1,-1,0,116,-1,-16,4,20,6,29,2,38,1,-41,0,0,1,7,0,-90,0,-24,0,30,0,23,0,-80,
		0,69,88,-80,17,47,27,-79,17,28,62,89,-79,20,6,-12,-80,24,-48,48,49,0,0,1,0,116,-2,116,4,10,4,-115,0,32,0,85,0,-80,0,69,88,-80,24,47,27,-79,24,28,62,89,-80,0,69,88,-80,14,47,27,-79,14,20,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-80,24,16,-80,32,-48,-78,5,19,32,17,18,57,-80,
		14,16,-79,9,3,-80,10,43,88,33,-40,27,-12,89,-80,19,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,17,20,6,7,7,6,21,20,51,50,55,23,6,35,34,38,53,52,55,34,38,39,17,51,17,20,22,51,50,54,53,17,4,10,120,111,50,108,78,48,52,13,70,90,89,103,90,-51,-7,4,-73,-113,-123,-125,-113,4,-115,-4,
		-13,122,-70,48,40,91,82,72,26,121,44,104,86,104,86,-50,-72,3,23,-4,-12,121,-127,127,123,3,12,0,-1,-1,0,49,0,0,5,-15,6,30,2,38,1,-43,0,0,1,7,0,-98,1,59,0,30,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,28,62,89,-79,23,4,-12,48,49,-1,-1,0,13,0,0,4,28,6,30,2,38,1,-45,0,0,
		1,6,0,-98,61,30,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,13,4,-12,48,49,-1,-1,0,13,0,0,4,28,5,-29,2,38,1,-45,0,0,1,6,0,106,109,30,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,16,2,-12,-80,25,-48,48,49,0,-1,-1,0,71,0,0,3,-32,6,30,2,38,1,-46,
		0,0,1,7,0,117,1,51,0,30,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,12,6,-12,48,49,-1,-1,0,71,0,0,3,-32,5,-32,2,38,1,-46,0,0,1,7,0,-94,1,26,0,30,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-79,17,2,-12,48,49,-1,-1,0,71,0,0,3,-32,6,30,2,38,
		1,-46,0,0,1,6,0,-97,82,31,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,28,62,89,-79,15,6,-12,48,49,-1,-1,0,28,0,0,5,29,6,63,2,38,0,37,0,0,0,6,0,-82,4,0,-1,-1,-1,41,0,0,4,70,6,63,2,38,0,41,0,0,0,7,0,-82,-2,114,0,0,-1,-1,-1,55,0,0,5,8,6,65,2,38,
		0,44,0,0,0,7,0,-82,-2,-128,0,2,-1,-1,-1,61,0,0,1,119,6,64,2,38,0,45,0,0,0,7,0,-82,-2,-122,0,1,-1,-1,-1,-26,-1,-20,5,29,6,63,0,38,0,51,20,0,0,7,0,-82,-1,47,0,0,-1,-1,-1,20,0,0,5,31,6,63,0,38,0,61,100,0,0,7,0,-82,-2,93,0,0,-1,-1,-1,-23,0,0,
		4,-33,6,63,0,38,0,-70,20,0,0,7,0,-82,-1,50,0,0,-1,-1,-1,-101,-1,-12,2,-83,6,116,2,38,0,-61,0,0,1,7,0,-81,-1,42,-1,-20,0,29,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,24,1,-12,-80,15,-48,-80,24,16,-80,33,-48,48,49,0,-1,-1,0,28,0,0,5,29,5,-80,2,6,0,37,0,0,
		-1,-1,0,-87,0,0,4,-120,5,-80,2,6,0,38,0,0,-1,-1,0,-87,0,0,4,70,5,-80,2,6,0,41,0,0,-1,-1,0,86,0,0,4,122,5,-80,2,6,0,62,0,0,-1,-1,0,-87,0,0,5,8,5,-80,2,6,0,44,0,0,-1,-1,0,-73,0,0,1,119,5,-80,2,6,0,45,0,0,-1,-1,0,-87,0,0,5,5,5,-80,
		2,6,0,47,0,0,-1,-1,0,-87,0,0,6,82,5,-80,2,6,0,49,0,0,-1,-1,0,-87,0,0,5,8,5,-80,2,6,0,50,0,0,-1,-1,0,118,-1,-20,5,9,5,-60,2,6,0,51,0,0,-1,-1,0,-87,0,0,4,-64,5,-80,2,6,0,52,0,0,-1,-1,0,49,0,0,4,-105,5,-80,2,6,0,56,0,0,-1,-1,0,15,
		0,0,4,-69,5,-80,2,6,0,61,0,0,-1,-1,0,57,0,0,4,-50,5,-80,2,6,0,60,0,0,-1,-1,-1,-43,0,0,2,94,7,7,2,38,0,45,0,0,1,7,0,106,-1,112,1,66,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,11,4,-12,-80,20,-48,48,49,0,-1,-1,0,15,0,0,4,-69,6,-5,2,38,
		0,61,0,0,1,7,0,106,0,-62,1,54,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,16,4,-12,-80,25,-48,48,49,0,-1,-1,0,100,-1,-21,4,119,6,58,2,38,0,-69,0,0,1,7,0,-82,1,117,-1,-5,0,20,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-79,36,1,-12,48,49,-1,-1,0,99,-1,-20,
		3,-20,6,57,2,38,0,-65,0,0,1,7,0,-82,1,43,-1,-6,0,20,0,-80,0,69,88,-80,21,47,27,-79,21,26,62,89,-79,40,1,-12,48,49,-1,-1,0,-111,-2,97,3,-16,6,58,2,38,0,-63,0,0,1,7,0,-82,1,70,-1,-5,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,21,1,-12,48,49,-1,-1,0,-61,
		-1,-12,2,75,6,37,2,38,0,-61,0,0,1,6,0,-82,42,-26,0,20,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,15,1,-12,48,49,-1,-1,0,-113,-1,-20,3,-10,6,116,2,38,0,-53,0,0,1,6,0,-81,33,-20,0,29,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-79,29,1,-12,-80,21,-48,-80,29,16,-80,39,
		-48,48,49,0,-1,-1,0,-102,0,0,4,63,4,58,2,6,0,-114,0,0,-1,-1,0,91,-1,-20,4,52,4,78,2,6,0,83,0,0,-1,-1,0,-102,-2,96,3,-18,4,58,2,6,0,118,0,0,-1,-1,0,33,0,0,3,-70,4,58,2,6,0,90,0,0,0,1,0,90,-2,76,4,116,4,73,0,27,0,110,0,-80,0,69,88,-80,4,47,
		27,-79,4,26,62,89,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-80,0,69,88,-80,19,47,27,-79,19,20,62,89,-80,0,69,88,-80,14,47,27,-79,14,20,62,89,-78,3,4,19,17,18,57,-78,18,19,4,17,18,57,-78,6,3,18,17,18,57,-79,9,1,-80,10,43,88,33,-40,27,-12,89,-78,21,18,3,17,18,57,-80,0,16,-79,24,
		1,-80,10,43,88,33,-40,27,-12,89,48,49,19,50,23,19,19,51,1,19,22,23,51,55,7,6,35,34,38,39,3,1,35,1,3,38,35,7,39,54,-62,-82,88,-107,-1,-69,-2,-96,-38,61,68,26,72,47,24,37,91,120,62,-94,-2,-25,-60,1,-125,-88,73,107,68,1,68,4,73,-64,-2,-83,2,4,-3,47,-2,14,-128,3,5,-98,15,94,-122,1,
		114,-3,-65,3,16,1,-125,-73,5,-108,15,0,-1,-1,-1,-27,-1,-12,2,110,5,-79,2,38,0,-61,0,0,1,6,0,106,-128,-20,0,23,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,20,1,-12,-80,29,-48,48,49,0,-1,-1,0,-113,-1,-20,3,-10,5,-79,2,38,0,-53,0,0,1,6,0,106,119,-20,0,23,0,-80,0,69,88,-80,
		0,47,27,-79,0,26,62,89,-79,26,1,-12,-80,35,-48,48,49,0,-1,-1,0,91,-1,-20,4,52,6,58,2,38,0,83,0,0,1,7,0,-82,1,67,-1,-5,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,30,1,-12,48,49,-1,-1,0,-113,-1,-20,3,-10,6,37,2,38,0,-53,0,0,1,7,0,-82,1,34,-1,-26,0,20,
		0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-79,21,1,-12,48,49,-1,-1,0,122,-1,-20,6,25,6,34,2,38,0,-50,0,0,1,7,0,-82,2,83,-1,-29,0,20,0,-80,0,69,88,-80,0,47,27,-79,0,26,62,89,-79,38,1,-12,48,49,-1,-1,0,-87,0,0,4,70,7,7,2,38,0,41,0,0,1,7,0,106,0,-60,1,66,
		0,23,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,19,4,-12,-80,28,-48,48,49,0,-1,-1,0,-79,0,0,4,48,7,66,2,38,0,-79,0,0,1,7,0,117,1,-112,1,66,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,8,8,-12,48,49,0,1,0,80,-1,-20,4,114,5,-60,0,38,0,100,-78,0,39,40,
		17,18,57,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,0,69,88,-80,26,47,27,-79,26,18,62,89,-80,6,16,-80,11,-48,-80,6,16,-79,14,1,-80,10,43,88,33,-40,27,-12,89,-78,38,26,6,17,18,57,-80,38,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,-80,26,16,-80,31,-48,-80,26,16,-79,34,1,-80,10,43,88,33,
		-40,27,-12,89,48,49,1,38,38,53,52,36,51,50,22,22,21,35,52,38,35,34,6,21,20,22,4,22,22,21,20,4,35,34,36,38,53,51,20,22,51,50,54,52,38,2,86,-9,-31,1,19,-36,-106,-21,-127,-63,-88,-103,-114,-97,-105,1,107,-51,99,-2,-20,-25,-106,-2,-4,-115,-63,-61,-93,-104,-94,-106,2,-119,71,-49,-104,-84,-31,116,-52,121,-124,-105,
		125,111,89,123,102,123,-92,111,-79,-43,115,-56,127,-124,-103,124,-42,117,-1,-1,0,-73,0,0,1,119,5,-80,2,6,0,45,0,0,-1,-1,-1,-43,0,0,2,94,7,7,2,38,0,45,0,0,1,7,0,106,-1,112,1,66,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,11,4,-12,-80,20,-48,48,49,0,-1,-1,0,53,-1,-20,
		3,-52,5,-80,2,6,0,46,0,0,-1,-1,0,-78,0,0,5,29,5,-80,2,6,2,44,0,0,-1,-1,0,-87,0,0,5,5,7,48,2,38,0,47,0,0,1,7,0,117,1,123,1,48,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,14,8,-12,48,49,-1,-1,0,77,-1,-21,4,-53,7,26,2,38,0,-34,0,0,1,7,
		0,-95,0,-38,1,67,0,19,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,21,-36,48,49,0,-1,-1,0,28,0,0,5,29,5,-80,2,6,0,37,0,0,-1,-1,0,-87,0,0,4,-120,5,-80,2,6,0,38,0,0,-1,-1,0,-79,0,0,4,48,5,-80,2,6,0,-79,0,0,-1,-1,0,-87,0,0,4,70,5,-80,2,6,0,41,
		0,0,-1,-1,0,-79,0,0,4,-1,7,26,2,38,0,-36,0,0,1,7,0,-95,1,49,1,67,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,13,-36,48,49,0,-1,-1,0,-87,0,0,6,82,5,-80,2,6,0,49,0,0,-1,-1,0,-87,0,0,5,8,5,-80,2,6,0,44,0,0,-1,-1,0,118,-1,-20,5,9,5,-60,
		2,6,0,51,0,0,-1,-1,0,-78,0,0,5,1,5,-80,2,6,0,-74,0,0,-1,-1,0,-87,0,0,4,-64,5,-80,2,6,0,52,0,0,-1,-1,0,119,-1,-20,4,-40,5,-60,2,6,0,39,0,0,-1,-1,0,49,0,0,4,-105,5,-80,2,6,0,56,0,0,-1,-1,0,57,0,0,4,-50,5,-80,2,6,0,60,0,0,-1,-1,0,109,
		-1,-20,3,-22,4,78,2,6,0,69,0,0,-1,-1,0,93,-1,-20,3,-13,4,78,2,6,0,73,0,0,-1,-1,0,-100,0,0,4,1,5,-60,2,38,0,-16,0,0,1,7,0,-95,0,-94,-1,-19,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,13,-36,48,49,0,-1,-1,0,91,-1,-20,4,52,4,78,2,6,0,83,0,0,
		-1,-1,0,-116,-2,96,4,30,4,78,2,6,0,84,0,0,0,1,0,92,-1,-20,3,-20,4,78,0,29,0,75,-78,16,30,31,17,18,57,0,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,8,47,27,-79,8,18,62,89,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,8,16,-80,3,-48,-80,16,16,-80,20,-48,-80,16,
		16,-79,23,1,-80,10,43,88,33,-40,27,-12,89,48,49,37,50,54,55,51,14,2,35,34,0,17,53,52,54,54,51,50,22,23,35,38,38,35,34,6,21,21,20,22,2,62,99,-108,8,-81,5,118,-59,110,-35,-2,-5,116,-39,-108,-74,-15,8,-81,8,-113,105,-115,-101,-102,-125,120,90,93,-88,100,1,39,1,0,31,-98,-10,-120,-38,-82,105,-121,-53,-64,
		35,-69,-54,0,-1,-1,0,22,-2,75,3,-80,4,58,2,6,0,93,0,0,-1,-1,0,41,0,0,3,-54,4,58,2,6,0,92,0,0,-1,-1,0,93,-1,-20,3,-13,5,-59,2,38,0,73,0,0,1,7,0,106,0,-114,0,0,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,37,1,-12,-80,46,-48,48,49,0,-1,-1,0,-102,
		0,0,3,71,5,-20,2,38,0,-20,0,0,1,7,0,117,0,-51,-1,-20,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,8,9,-12,48,49,-1,-1,0,95,-1,-20,3,-69,4,78,2,6,0,87,0,0,-1,-1,0,-115,0,0,1,104,5,-60,2,6,0,77,0,0,-1,-1,-1,-69,0,0,2,68,5,-60,2,38,0,-115,0,0,
		1,7,0,106,-1,86,-1,-1,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-79,11,1,-12,-80,20,-48,48,49,0,-1,-1,-1,-65,-2,75,1,89,5,-60,2,6,0,78,0,0,-1,-1,0,-100,0,0,4,63,5,-21,2,38,0,-15,0,0,1,7,0,117,1,59,-1,-21,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,
		-79,15,9,-12,48,49,-1,-1,0,22,-2,75,3,-80,5,-40,2,38,0,93,0,0,1,6,0,-95,80,1,0,19,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-80,19,-36,48,49,0,-1,-1,0,61,0,0,6,-19,7,54,2,38,0,59,0,0,1,7,0,68,2,44,1,54,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,
		-79,20,8,-12,48,49,-1,-1,0,43,0,0,5,-45,6,0,2,38,0,91,0,0,1,7,0,68,1,-117,0,0,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,14,9,-12,48,49,-1,-1,0,61,0,0,6,-19,7,54,2,38,0,59,0,0,1,7,0,117,2,-69,1,54,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,
		62,89,-79,21,8,-12,48,49,-1,-1,0,43,0,0,5,-45,6,0,2,38,0,91,0,0,1,7,0,117,2,26,0,0,0,20,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,15,9,-12,48,49,-1,-1,0,61,0,0,6,-19,6,-5,2,38,0,59,0,0,1,7,0,106,1,-11,1,54,0,23,0,-80,0,69,88,-80,3,47,27,-79,
		3,30,62,89,-79,26,4,-12,-80,35,-48,48,49,0,-1,-1,0,43,0,0,5,-45,5,-59,2,38,0,91,0,0,1,7,0,106,1,84,0,0,0,23,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,20,1,-12,-80,29,-48,48,49,0,-1,-1,0,15,0,0,4,-69,7,54,2,38,0,61,0,0,1,7,0,68,0,-7,1,54,0,20,
		0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,10,8,-12,48,49,-1,-1,0,22,-2,75,3,-80,6,0,2,38,0,93,0,0,1,7,0,68,0,-116,0,0,0,20,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,17,9,-12,48,49,-1,-1,0,103,4,33,0,-3,6,0,2,6,0,11,0,0,-1,-1,0,-120,4,18,2,35,
		6,0,2,6,0,6,0,0,-1,-1,0,-96,-1,-11,3,-118,5,-80,0,38,0,5,0,0,0,7,0,5,2,15,0,0,-1,-1,-1,-76,-2,75,2,63,5,-40,2,38,0,-100,0,0,1,7,0,-97,-1,72,-1,-39,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,19,1,-12,48,49,-1,-1,0,48,4,22,1,71,6,0,2,6,
		1,-123,0,0,-1,-1,0,-87,0,0,6,82,7,54,2,38,0,49,0,0,1,7,0,117,2,-103,1,54,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,17,8,-12,48,49,-1,-1,0,-117,0,0,6,120,6,0,2,38,0,81,0,0,1,7,0,117,2,-83,0,0,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,
		-79,32,9,-12,48,49,-1,-1,0,28,-2,107,5,29,5,-80,2,38,0,37,0,0,0,7,0,-89,1,127,0,0,-1,-1,0,109,-2,107,3,-22,4,78,2,38,0,69,0,0,0,7,0,-89,0,-57,0,0,-1,-1,0,-87,0,0,4,70,7,66,2,38,0,41,0,0,1,7,0,68,0,-5,1,66,0,20,0,-80,0,69,88,-80,6,47,27,-79,
		6,30,62,89,-79,13,8,-12,48,49,-1,-1,0,-79,0,0,4,-1,7,66,2,38,0,-36,0,0,1,7,0,68,1,109,1,66,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,11,8,-12,48,49,-1,-1,0,93,-1,-20,3,-13,6,0,2,38,0,73,0,0,1,7,0,68,0,-59,0,0,0,20,0,-80,0,69,88,-80,8,47,
		27,-79,8,26,62,89,-79,31,9,-12,48,49,-1,-1,0,-100,0,0,4,1,5,-20,2,38,0,-16,0,0,1,7,0,68,0,-34,-1,-20,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,11,9,-12,48,49,-1,-1,0,90,0,0,5,33,5,-80,2,6,0,-71,0,0,-1,-1,0,95,-2,40,5,67,4,58,2,6,0,-51,0,0,
		-1,-1,0,22,0,0,4,-35,6,-24,2,38,1,25,0,0,1,7,0,-84,4,57,0,-6,0,23,0,-80,0,69,88,-80,15,47,27,-79,15,30,62,89,-79,17,8,-12,-80,21,-48,48,49,0,-1,-1,-1,-5,0,0,4,11,5,-63,2,38,1,26,0,0,1,7,0,-84,3,-44,-1,-45,0,23,0,-80,0,69,88,-80,17,47,27,-79,17,26,62,89,
		-79,19,9,-12,-80,23,-48,48,49,0,-1,-1,0,91,-2,75,8,64,4,78,0,38,0,83,0,0,0,7,0,93,4,-112,0,0,-1,-1,0,118,-2,75,9,48,5,-60,0,38,0,51,0,0,0,7,0,93,5,-128,0,0,-1,-1,0,80,-2,81,4,106,5,-60,2,38,0,-37,0,0,0,7,2,81,1,-100,-1,-72,-1,-1,0,88,-2,82,3,-84,
		4,77,2,38,0,-17,0,0,0,7,2,81,1,67,-1,-71,-1,-1,0,119,-2,81,4,-40,5,-60,2,38,0,39,0,0,0,7,2,81,1,-27,-1,-72,-1,-1,0,92,-2,81,3,-20,4,78,2,38,0,71,0,0,0,7,2,81,1,82,-1,-72,-1,-1,0,15,0,0,4,-69,5,-80,2,6,0,61,0,0,-1,-1,0,46,-2,96,3,-33,4,58,
		2,6,0,-67,0,0,-1,-1,0,-73,0,0,1,119,5,-80,2,6,0,45,0,0,-1,-1,0,27,0,0,7,53,7,26,2,38,0,-38,0,0,1,7,0,-95,1,-8,1,67,0,19,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,25,-36,48,49,0,-1,-1,0,21,0,0,6,4,5,-60,2,38,0,-18,0,0,1,7,0,-95,1,95,
		-1,-19,0,19,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-80,25,-36,48,49,0,-1,-1,0,-73,0,0,1,119,5,-80,2,6,0,45,0,0,-1,-1,0,28,0,0,5,29,7,14,2,38,0,37,0,0,1,7,0,-95,0,-12,1,55,0,19,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-80,14,-36,48,49,0,-1,-1,0,109,
		-1,-20,3,-22,5,-40,2,38,0,69,0,0,1,7,0,-95,0,-103,0,1,0,19,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,44,-36,48,49,0,-1,-1,0,28,0,0,5,29,6,-5,2,38,0,37,0,0,1,7,0,106,0,-7,1,54,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,18,4,-12,-80,27,-48,48,
		49,0,-1,-1,0,109,-1,-20,3,-22,5,-59,2,38,0,69,0,0,1,7,0,106,0,-98,0,0,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,48,1,-12,-80,57,-48,48,49,0,-1,-1,-1,-14,0,0,7,87,5,-80,2,6,0,-127,0,0,-1,-1,0,78,-1,-20,6,124,4,78,2,6,0,-122,0,0,-1,-1,0,-87,0,0,
		4,70,7,26,2,38,0,41,0,0,1,7,0,-95,0,-65,1,67,0,19,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-80,15,-36,48,49,0,-1,-1,0,93,-1,-20,3,-13,5,-40,2,38,0,73,0,0,1,7,0,-95,0,-119,0,1,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,33,-36,48,49,0,-1,-1,0,93,
		-1,-20,5,18,6,-39,2,38,1,88,0,0,1,7,0,106,0,-45,1,20,0,23,0,-80,0,69,88,-80,0,47,27,-79,0,30,62,89,-79,39,4,-12,-80,48,-48,48,49,0,-1,-1,0,98,-1,-20,3,-23,4,79,2,6,0,-99,0,0,-1,-1,0,98,-1,-20,3,-23,5,-58,2,38,0,-99,0,0,1,7,0,106,0,-121,0,1,0,23,0,-80,
		0,69,88,-80,0,47,27,-79,0,26,62,89,-79,36,1,-12,-80,45,-48,48,49,0,-1,-1,0,27,0,0,7,53,7,7,2,38,0,-38,0,0,1,7,0,106,1,-3,1,66,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,29,4,-12,-80,38,-48,48,49,0,-1,-1,0,21,0,0,6,4,5,-79,2,38,0,-18,0,0,1,7,
		0,106,1,100,-1,-20,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,29,1,-12,-80,38,-48,48,49,0,-1,-1,0,80,-1,-20,4,106,7,28,2,38,0,-37,0,0,1,7,0,106,0,-73,1,87,0,23,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,48,4,-12,-80,57,-48,48,49,0,-1,-1,0,88,-1,-19,3,-84,
		5,-59,2,38,0,-17,0,0,1,6,0,106,94,0,0,23,0,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-79,46,1,-12,-80,55,-48,48,49,0,-1,-1,0,-79,0,0,4,-1,6,-17,2,38,0,-36,0,0,1,7,0,112,1,4,1,74,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,11,-36,48,49,0,-1,-1,0,-100,
		0,0,4,1,5,-103,2,38,0,-16,0,0,1,6,0,112,117,-12,0,19,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-80,11,-36,48,49,0,-1,-1,0,-79,0,0,4,-1,7,7,2,38,0,-36,0,0,1,7,0,106,1,54,1,66,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,17,4,-12,-80,26,-48,48,49,0,
		-1,-1,0,-100,0,0,4,1,5,-79,2,38,0,-16,0,0,1,7,0,106,0,-89,-1,-20,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,17,1,-12,-80,26,-48,48,49,0,-1,-1,0,118,-1,-20,5,9,6,-3,2,38,0,51,0,0,1,7,0,106,1,27,1,56,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,
		-79,39,4,-12,-80,48,-48,48,49,0,-1,-1,0,91,-1,-20,4,52,5,-59,2,38,0,83,0,0,1,7,0,106,0,-104,0,0,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,35,1,-12,-80,44,-48,48,49,0,-1,-1,0,103,-1,-20,4,-6,5,-60,2,6,1,23,0,0,-1,-1,0,91,-1,-20,4,52,4,78,2,6,1,24,
		0,0,-1,-1,0,103,-1,-20,4,-6,7,2,2,38,1,23,0,0,1,7,0,106,1,39,1,61,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,39,4,-12,-80,48,-48,48,49,0,-1,-1,0,91,-1,-20,4,52,5,-57,2,38,1,24,0,0,1,7,0,106,0,-120,0,2,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,
		62,89,-79,36,1,-12,-80,45,-48,48,49,0,-1,-1,0,-109,-1,-20,4,-12,7,29,2,38,0,-25,0,0,1,7,0,106,1,13,1,88,0,23,0,-80,0,69,88,-80,19,47,27,-79,19,30,62,89,-79,39,4,-12,-80,48,-48,48,49,0,-1,-1,0,100,-1,-20,3,-32,5,-59,2,38,0,-1,0,0,1,6,0,106,124,0,0,23,0,-80,0,69,
		88,-80,8,47,27,-79,8,26,62,89,-79,39,1,-12,-80,48,-48,48,49,0,-1,-1,0,77,-1,-21,4,-53,6,-17,2,38,0,-34,0,0,1,7,0,112,0,-83,1,74,0,19,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-80,19,-36,48,49,0,-1,-1,0,22,-2,75,3,-80,5,-83,2,38,0,93,0,0,1,6,0,112,35,8,0,19,
		0,-80,0,69,88,-80,14,47,27,-79,14,26,62,89,-80,17,-36,48,49,0,-1,-1,0,77,-1,-21,4,-53,7,7,2,38,0,-34,0,0,1,7,0,106,0,-33,1,66,0,23,0,-80,0,69,88,-80,17,47,27,-79,17,30,62,89,-79,25,4,-12,-80,34,-48,48,49,0,-1,-1,0,22,-2,75,3,-80,5,-59,2,38,0,93,0,0,1,6,0,106,
		85,0,0,23,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,23,1,-12,-80,32,-48,48,49,0,-1,-1,0,77,-1,-21,4,-53,7,65,2,38,0,-34,0,0,1,7,0,-90,1,47,1,66,0,23,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-79,20,8,-12,-80,24,-48,48,49,0,-1,-1,0,22,-2,75,3,-47,5,-1,2,38,
		0,93,0,0,1,7,0,-90,0,-91,0,0,0,23,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,22,9,-12,-80,18,-48,48,49,0,-1,-1,0,-106,0,0,4,-56,7,7,2,38,0,-31,0,0,1,7,0,106,1,9,1,66,0,23,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,26,4,-12,-80,35,-48,48,49,0,-1,-1,
		0,103,0,0,3,-67,5,-79,2,38,0,-7,0,0,1,6,0,106,100,-20,0,23,0,-80,0,69,88,-80,9,47,27,-79,9,26,62,89,-79,24,1,-12,-80,33,-48,48,49,0,-1,-1,0,-78,0,0,6,48,7,7,0,38,0,-26,15,0,0,39,0,45,4,-71,0,0,1,7,0,106,1,-45,1,66,0,23,0,-80,0,69,88,-80,10,47,27,-79,
		10,30,62,89,-79,31,4,-12,-80,40,-48,48,49,0,-1,-1,0,-99,0,0,5,127,5,-79,0,38,0,-2,0,0,0,39,0,-115,4,42,0,0,1,7,0,106,1,109,-1,-20,0,23,0,-80,0,69,88,-80,10,47,27,-79,10,26,62,89,-79,31,1,-12,-80,40,-48,48,49,0,-1,-1,0,95,-1,-20,3,-16,6,0,2,6,0,72,0,0,-1,-1,
		0,28,-2,-94,5,29,5,-80,2,38,0,37,0,0,0,7,0,-83,5,2,0,0,-1,-1,0,109,-2,-94,3,-22,4,78,2,38,0,69,0,0,0,7,0,-83,4,74,0,0,-1,-1,0,28,0,0,5,29,7,-70,2,38,0,37,0,0,1,7,0,-85,4,-18,1,70,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,11,8,-12,
		48,49,-1,-1,0,109,-1,-20,3,-22,6,-124,2,38,0,69,0,0,1,7,0,-85,4,-109,0,16,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,41,1,-12,48,49,-1,-1,0,28,0,0,5,29,7,-61,2,38,0,37,0,0,1,7,2,55,0,-61,1,46,0,23,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,14,
		12,-12,-80,20,-48,48,49,0,-1,-1,0,109,-1,-20,4,-64,6,-114,2,38,0,69,0,0,1,6,2,55,104,-7,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,8,-12,-80,50,-48,48,49,0,-1,-1,0,28,0,0,5,29,7,-65,2,38,0,37,0,0,1,7,2,56,0,-57,1,61,0,23,0,-80,0,69,88,-80,4,47,
		27,-79,4,30,62,89,-79,14,12,-12,-80,19,-48,48,49,0,-1,-1,-1,-54,-1,-20,3,-22,6,-119,2,38,0,69,0,0,1,6,2,56,108,7,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,8,-12,-80,49,-48,48,49,0,-1,-1,0,28,0,0,5,29,7,-22,2,38,0,37,0,0,1,7,2,57,0,-56,1,27,0,23,
		0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,12,12,-12,-80,32,-48,48,49,0,-1,-1,0,109,-1,-20,4,89,6,-75,2,38,0,69,0,0,1,6,2,57,109,-26,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,42,8,-12,-80,48,-48,48,49,0,-1,-1,0,28,0,0,5,29,7,-38,2,38,0,37,0,0,1,7,
		2,58,0,-57,1,6,0,23,0,-80,0,69,88,-80,5,47,27,-79,5,30,62,89,-79,12,12,-12,-80,21,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,6,-91,2,38,0,69,0,0,1,6,2,58,108,-47,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,42,8,-12,-80,51,-48,48,49,0,-1,-1,0,28,-2,-94,5,29,7,54,
		2,38,0,37,0,0,0,39,0,-98,0,-55,1,54,1,7,0,-83,5,2,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,15,6,-12,48,49,-1,-1,0,109,-2,-94,3,-22,6,0,2,38,0,69,0,0,0,38,0,-98,110,0,1,7,0,-83,4,74,0,0,0,20,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,
		-79,45,1,-12,48,49,-1,-1,0,28,0,0,5,29,7,-73,2,38,0,37,0,0,1,7,2,60,0,-22,1,45,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,14,7,-12,-80,27,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,6,-126,2,38,0,69,0,0,1,7,2,60,0,-113,-1,-8,0,23,0,-80,0,69,88,-80,23,47,
		27,-79,23,26,62,89,-79,44,4,-12,-80,57,-48,48,49,0,-1,-1,0,28,0,0,5,29,7,-73,2,38,0,37,0,0,1,7,2,53,0,-22,1,45,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,14,7,-12,-80,28,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,6,-126,2,38,0,69,0,0,1,7,2,53,0,-113,-1,-8,
		0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,4,-12,-80,58,-48,48,49,0,-1,-1,0,28,0,0,5,29,8,64,2,38,0,37,0,0,1,7,2,61,0,-18,1,61,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,14,7,-12,-80,39,-48,48,49,0,-1,-1,0,109,-1,-20,3,-22,7,10,2,38,0,69,
		0,0,1,7,2,61,0,-109,0,7,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,4,-12,-80,69,-48,48,49,0,-1,-1,0,28,0,0,5,29,8,21,2,38,0,37,0,0,1,7,2,80,0,-18,1,69,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,14,7,-12,-80,28,-48,48,49,0,-1,-1,0,109,
		-1,-20,3,-22,6,-33,2,38,0,69,0,0,1,7,2,80,0,-109,0,15,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,44,4,-12,-80,58,-48,48,49,0,-1,-1,0,28,-2,-94,5,29,7,14,2,38,0,37,0,0,0,39,0,-95,0,-12,1,55,1,7,0,-83,5,2,0,0,0,19,0,-80,0,69,88,-80,4,47,27,-79,
		4,30,62,89,-80,14,-36,48,49,0,-1,-1,0,109,-2,-94,3,-22,5,-40,2,38,0,69,0,0,0,39,0,-95,0,-103,0,1,1,7,0,-83,4,74,0,0,0,19,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-80,44,-36,48,49,0,-1,-1,0,-87,-2,-84,4,70,5,-80,2,38,0,41,0,0,0,7,0,-83,4,-64,0,10,-1,-1,
		0,93,-2,-94,3,-13,4,78,2,38,0,73,0,0,0,7,0,-83,4,-116,0,0,-1,-1,0,-87,0,0,4,70,7,-58,2,38,0,41,0,0,1,7,0,-85,4,-71,1,82,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,12,8,-12,48,49,-1,-1,0,93,-1,-20,3,-13,6,-124,2,38,0,73,0,0,1,7,0,-85,4,-125,
		0,16,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,30,1,-12,48,49,-1,-1,0,-87,0,0,4,70,7,46,2,38,0,41,0,0,1,7,0,-91,0,-112,1,70,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,15,4,-12,48,49,-1,-1,0,93,-1,-20,3,-13,5,-20,2,38,0,73,0,0,1,6,0,-91,
		90,4,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,33,1,-12,48,49,-1,-1,0,-87,0,0,4,-26,7,-49,2,38,0,41,0,0,1,7,2,55,0,-114,1,58,0,23,0,-80,0,69,88,-80,7,47,27,-79,7,30,62,89,-79,15,12,-12,-80,21,-48,48,49,0,-1,-1,0,93,-1,-20,4,-80,6,-114,2,38,0,73,0,0,
		1,6,2,55,88,-7,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,33,8,-12,-80,39,-48,48,49,0,-1,-1,-1,-16,0,0,4,70,7,-53,2,38,0,41,0,0,1,7,2,56,0,-110,1,73,0,23,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,15,12,-12,-80,20,-48,48,49,0,-1,-1,-1,-70,-1,-20,3,-13,
		6,-119,2,38,0,73,0,0,1,6,2,56,92,7,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,33,8,-12,-80,38,-48,48,49,0,-1,-1,0,-87,0,0,4,127,7,-10,2,38,0,41,0,0,1,7,2,57,0,-109,1,39,0,23,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,15,12,-12,-80,19,-48,48,49,0,
		-1,-1,0,93,-1,-20,4,73,6,-75,2,38,0,73,0,0,1,6,2,57,93,-26,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,31,8,-12,-80,37,-48,48,49,0,-1,-1,0,-87,0,0,4,70,7,-26,2,38,0,41,0,0,1,7,2,58,0,-110,1,18,0,23,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,15,
		12,-12,-80,22,-48,48,49,0,-1,-1,0,93,-1,-20,3,-13,6,-91,2,38,0,73,0,0,1,6,2,58,92,-47,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,33,8,-12,-80,40,-48,48,49,0,-1,-1,0,-87,-2,-84,4,70,7,66,2,38,0,41,0,0,0,39,0,-98,0,-108,1,66,1,7,0,-83,4,-64,0,10,0,20,
		0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,16,6,-12,48,49,-1,-1,0,93,-2,-94,3,-13,6,0,2,38,0,73,0,0,0,38,0,-98,94,0,1,7,0,-83,4,-116,0,0,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,32,1,-12,48,49,-1,-1,0,-73,0,0,1,-8,7,-58,2,38,0,45,0,0,1,7,
		0,-85,3,100,1,82,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,4,8,-12,48,49,-1,-1,0,-101,0,0,1,-34,6,-126,2,38,0,-115,0,0,1,7,0,-85,3,74,0,14,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-79,4,1,-12,48,49,-1,-1,0,-93,-2,-85,1,126,5,-80,2,38,0,45,0,0,
		0,7,0,-83,3,107,0,9,-1,-1,0,-123,-2,-84,1,104,5,-60,2,38,0,77,0,0,0,7,0,-83,3,77,0,10,-1,-1,0,118,-2,-94,5,9,5,-60,2,38,0,51,0,0,0,7,0,-83,5,24,0,0,-1,-1,0,91,-2,-94,4,52,4,78,2,38,0,83,0,0,0,7,0,-83,4,-99,0,0,-1,-1,0,118,-1,-20,5,9,7,-68,
		2,38,0,51,0,0,1,7,0,-85,5,16,1,72,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,46,8,-12,48,49,-1,-1,0,91,-1,-20,4,52,6,-124,2,38,0,83,0,0,1,7,0,-85,4,-115,0,16,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,42,1,-12,48,49,-1,-1,0,118,-1,-20,5,61,
		7,-59,2,38,0,51,0,0,1,7,2,55,0,-27,1,48,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,35,12,-12,-80,41,-48,48,49,0,-1,-1,0,91,-1,-20,4,-70,6,-114,2,38,0,83,0,0,1,6,2,55,98,-7,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,31,8,-12,-80,37,-48,48,49,0,
		-1,-1,0,71,-1,-20,5,9,7,-63,2,38,0,51,0,0,1,7,2,56,0,-23,1,63,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,33,12,-12,-80,40,-48,48,49,0,-1,-1,-1,-60,-1,-20,4,52,6,-119,2,38,0,83,0,0,1,6,2,56,102,7,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,29,
		8,-12,-80,36,-48,48,49,0,-1,-1,0,118,-1,-20,5,9,7,-20,2,38,0,51,0,0,1,7,2,57,0,-22,1,29,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,33,12,-12,-80,39,-48,48,49,0,-1,-1,0,91,-1,-20,4,83,6,-75,2,38,0,83,0,0,1,6,2,57,103,-26,0,23,0,-80,0,69,88,-80,4,47,
		27,-79,4,26,62,89,-79,29,8,-12,-80,35,-48,48,49,0,-1,-1,0,118,-1,-20,5,9,7,-36,2,38,0,51,0,0,1,7,2,58,0,-23,1,8,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,33,12,-12,-80,42,-48,48,49,0,-1,-1,0,91,-1,-20,4,52,6,-91,2,38,0,83,0,0,1,6,2,58,102,-47,0,23,
		0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,29,8,-12,-80,38,-48,48,49,0,-1,-1,0,118,-2,-94,5,9,7,56,2,38,0,51,0,0,0,39,0,-98,0,-21,1,56,1,7,0,-83,5,24,0,0,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,34,6,-12,48,49,-1,-1,0,91,-2,-94,4,52,6,0,2,38,
		0,83,0,0,0,38,0,-98,104,0,1,7,0,-83,4,-99,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,30,1,-12,48,49,-1,-1,0,101,-1,-20,5,-99,7,49,2,38,0,-104,0,0,1,7,0,117,1,-35,1,49,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,40,8,-12,48,49,-1,-1,0,91,
		-1,-20,4,-70,6,0,2,38,0,-103,0,0,1,7,0,117,1,101,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,38,9,-12,48,49,-1,-1,0,101,-1,-20,5,-99,7,49,2,38,0,-104,0,0,1,7,0,68,1,78,1,49,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,39,8,-12,48,49,-1,-1,
		0,91,-1,-20,4,-70,6,0,2,38,0,-103,0,0,1,7,0,68,0,-42,0,0,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,37,9,-12,48,49,-1,-1,0,101,-1,-20,5,-99,7,-75,2,38,0,-104,0,0,1,7,0,-85,5,12,1,65,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,52,8,-12,48,49,
		-1,-1,0,91,-1,-20,4,-70,6,-124,2,38,0,-103,0,0,1,7,0,-85,4,-108,0,16,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,50,1,-12,48,49,-1,-1,0,101,-1,-20,5,-99,7,29,2,38,0,-104,0,0,1,7,0,-91,0,-29,1,53,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,41,4,-12,
		48,49,-1,-1,0,91,-1,-20,4,-70,5,-20,2,38,0,-103,0,0,1,6,0,-91,107,4,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,39,1,-12,48,49,-1,-1,0,101,-2,-94,5,-99,6,55,2,38,0,-104,0,0,0,7,0,-83,5,9,0,0,-1,-1,0,91,-2,-103,4,-70,4,-80,2,38,0,-103,0,0,0,7,0,-83,
		4,-101,-1,-9,-1,-1,0,-116,-2,-94,4,-86,5,-80,2,38,0,57,0,0,0,7,0,-83,4,-18,0,0,-1,-1,0,-120,-2,-94,3,-36,4,58,2,38,0,89,0,0,0,7,0,-83,4,81,0,0,-1,-1,0,-116,-1,-20,4,-86,7,-70,2,38,0,57,0,0,1,7,0,-85,4,-23,1,70,0,20,0,-80,0,69,88,-80,10,47,27,-79,10,30,
		62,89,-79,19,8,-12,48,49,-1,-1,0,-120,-1,-20,3,-36,6,-124,2,38,0,89,0,0,1,7,0,-85,4,-123,0,16,0,20,0,-80,0,69,88,-80,7,47,27,-79,7,26,62,89,-79,17,1,-12,48,49,-1,-1,0,-116,-1,-20,6,29,7,66,2,38,0,-102,0,0,1,7,0,117,1,-44,1,66,0,20,0,-80,0,69,88,-80,26,47,27,-79,
		26,30,62,89,-79,29,8,-12,48,49,-1,-1,0,-120,-1,-20,5,15,5,-20,2,38,0,-101,0,0,1,7,0,117,1,99,-1,-20,0,20,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-79,28,9,-12,48,49,-1,-1,0,-116,-1,-20,6,29,7,66,2,38,0,-102,0,0,1,7,0,68,1,69,1,66,0,20,0,-80,0,69,88,-80,18,47,
		27,-79,18,30,62,89,-79,28,8,-12,48,49,-1,-1,0,-120,-1,-20,5,15,5,-20,2,38,0,-101,0,0,1,7,0,68,0,-44,-1,-20,0,20,0,-80,0,69,88,-80,13,47,27,-79,13,26,62,89,-79,27,9,-12,48,49,-1,-1,0,-116,-1,-20,6,29,7,-58,2,38,0,-102,0,0,1,7,0,-85,5,3,1,82,0,20,0,-80,0,69,88,-80,
		26,47,27,-79,26,30,62,89,-79,41,8,-12,48,49,-1,-1,0,-120,-1,-20,5,15,6,112,2,38,0,-101,0,0,1,7,0,-85,4,-110,-1,-4,0,20,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-79,40,1,-12,48,49,-1,-1,0,-116,-1,-20,6,29,7,46,2,38,0,-102,0,0,1,7,0,-91,0,-38,1,70,0,20,0,-80,0,69,
		88,-80,18,47,27,-79,18,30,62,89,-79,30,4,-12,48,49,-1,-1,0,-120,-1,-20,5,15,5,-40,2,38,0,-101,0,0,1,6,0,-91,105,-16,0,20,0,-80,0,69,88,-80,19,47,27,-79,19,26,62,89,-79,29,1,-12,48,49,-1,-1,0,-116,-2,-102,6,29,6,2,2,38,0,-102,0,0,0,7,0,-83,5,9,-1,-8,-1,-1,0,-120,-2,-94,
		5,15,4,-112,2,38,0,-101,0,0,0,7,0,-83,4,-121,0,0,-1,-1,0,15,-2,-94,4,-69,5,-80,2,38,0,61,0,0,0,7,0,-83,4,-69,0,0,-1,-1,0,22,-2,5,3,-80,4,58,2,38,0,93,0,0,0,7,0,-83,5,28,-1,99,-1,-1,0,15,0,0,4,-69,7,-70,2,38,0,61,0,0,1,7,0,-85,4,-73,1,70,
		0,20,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-79,9,8,-12,48,49,-1,-1,0,22,-2,75,3,-80,6,-124,2,38,0,93,0,0,1,7,0,-85,4,74,0,16,0,20,0,-80,0,69,88,-80,15,47,27,-79,15,26,62,89,-79,16,1,-12,48,49,-1,-1,0,15,0,0,4,-69,7,34,2,38,0,61,0,0,1,7,0,-91,0,-114,
		1,58,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,30,62,89,-79,12,4,-12,48,49,-1,-1,0,22,-2,75,3,-80,5,-20,2,38,0,93,0,0,1,6,0,-91,33,4,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-79,19,1,-12,48,49,-1,-1,0,95,-2,-51,4,-84,6,0,0,38,0,72,0,0,0,39,2,38,1,-95,
		2,71,1,7,0,67,0,-97,-1,100,0,8,0,-78,47,30,1,93,48,49,-1,-1,0,49,-2,-103,4,-105,5,-80,2,38,0,56,0,0,0,7,2,81,2,63,0,0,-1,-1,0,40,-2,-103,3,-80,4,58,2,38,0,-10,0,0,0,7,2,81,1,-58,0,0,-1,-1,0,-106,-2,-103,4,-56,5,-80,2,38,0,-31,0,0,0,7,2,81,2,-2,
		0,0,-1,-1,0,103,-2,-103,3,-67,4,59,2,38,0,-7,0,0,0,7,2,81,1,-11,0,0,-1,-1,0,-79,-2,-103,4,48,5,-80,2,38,0,-79,0,0,0,7,2,81,0,-17,0,0,-1,-1,0,-102,-2,-103,3,71,4,58,2,38,0,-20,0,0,0,7,2,81,0,-43,0,0,-1,-1,0,63,-2,85,5,-67,5,-61,2,38,1,76,0,0,
		0,7,2,81,3,6,-1,-68,-1,-1,-1,-34,-2,89,4,99,4,78,2,38,1,77,0,0,0,7,2,81,2,1,-1,-64,-1,-1,0,-116,0,0,3,-33,6,0,2,6,0,76,0,0,0,2,-1,-44,0,0,4,-79,5,-80,0,18,0,27,0,100,0,-80,0,69,88,-80,15,47,27,-79,15,30,62,89,-80,0,69,88,-80,10,47,27,-79,10,18,62,
		89,-78,2,10,15,17,18,57,-80,2,47,-78,14,15,2,17,18,57,-80,14,47,-79,11,1,-80,10,43,88,33,-40,27,-12,89,-80,1,-48,-80,14,16,-80,17,-48,-80,2,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,21,33,22,4,21,20,4,7,33,17,35,53,51,53,
		51,21,51,3,17,33,50,54,53,52,38,39,2,80,-19,1,106,-28,1,0,-2,-2,-33,-3,-45,-49,-49,-64,-19,-19,1,95,-113,-97,-103,-115,4,80,-14,3,-28,-60,-59,-22,4,4,80,-105,-55,-55,-3,-39,-3,-35,-104,-128,123,-114,2,0,0,2,-1,-44,0,0,4,-79,5,-80,0,18,0,27,0,100,0,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,
		-80,0,69,88,-80,10,47,27,-79,10,18,62,89,-78,2,10,16,17,18,57,-80,2,47,-78,17,2,16,17,18,57,-80,17,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-80,11,-48,-80,17,16,-80,14,-48,-80,2,16,-79,19,1,-80,10,43,88,33,-40,27,-12,89,-80,10,16,-79,20,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,35,21,33,
		22,4,21,20,4,7,33,17,35,53,51,53,51,21,51,3,17,33,50,54,53,52,38,39,2,80,-19,1,106,-28,1,0,-2,-2,-33,-3,-45,-49,-49,-64,-19,-19,1,95,-113,-97,-103,-115,4,80,-14,3,-28,-60,-59,-22,4,4,80,-105,-55,-55,-3,-39,-3,-35,-104,-128,123,-114,2,0,0,1,0,3,0,0,4,48,5,-80,0,13,0,80,0,-80,0,69,
		88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,13,8,2,17,18,57,-80,13,47,-78,122,13,1,93,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,4,-48,-80,13,16,-80,6,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,35,53,51,17,33,21,33,17,33,
		2,127,-2,-13,-63,-82,-82,3,127,-3,66,1,13,2,-84,-3,84,2,-84,-105,2,109,-98,-2,49,0,0,1,-1,-4,0,0,3,71,4,58,0,13,0,75,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,13,8,2,17,18,57,-80,13,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-80,
		4,-48,-80,13,16,-80,6,-48,-80,8,16,-79,10,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,33,17,35,17,35,53,51,17,33,21,33,17,33,2,120,-2,-36,-70,-98,-98,2,-83,-2,13,1,36,1,-33,-2,33,1,-33,-105,1,-60,-103,-2,-43,0,1,-1,-9,0,0,5,49,5,-80,0,20,0,-128,0,-80,0,69,88,-80,8,47,27,-79,8,30,
		62,89,-80,0,69,88,-80,16,47,27,-79,16,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-78,14,8,2,17,18,57,-80,14,47,-78,47,14,1,93,-78,-49,14,1,93,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,7,8,2,17,18,57,-80,7,47,-79,4,1,-80,10,43,88,
		33,-40,27,-12,89,-80,7,16,-80,10,-48,-80,4,16,-80,12,-48,-78,18,1,14,17,18,57,48,49,1,35,17,35,17,35,53,51,53,51,21,51,21,35,17,51,1,51,1,1,35,2,55,-79,-64,-49,-49,-64,-19,-19,-106,1,-3,-17,-3,-44,2,85,-21,2,-114,-3,114,4,55,-105,-30,-30,-105,-2,-9,2,-126,-3,62,-3,18,0,0,1,-1,-65,0,0,
		4,40,6,0,0,20,0,118,0,-80,0,69,88,-80,8,47,27,-79,8,32,62,89,-80,0,69,88,-80,16,47,27,-79,16,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-80,0,69,88,-80,19,47,27,-79,19,18,62,89,-78,14,16,2,17,18,57,-80,14,47,-79,1,1,-80,10,43,88,33,-40,27,-12,89,-78,7,8,16,17,18,57,
		-80,7,47,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,7,16,-80,10,-48,-80,4,16,-80,12,-48,-78,18,1,14,17,18,57,48,49,1,35,17,35,17,35,53,51,53,51,21,51,21,35,17,51,1,51,1,1,35,1,-32,-128,-70,-25,-25,-70,-37,-37,126,1,59,-37,-2,-122,1,-82,-37,1,-11,-2,11,4,-63,-105,-88,-88,-105,-3,-51,1,-84,-2,
		19,-3,-77,0,0,1,0,15,0,0,4,-69,5,-80,0,14,0,87,-78,10,15,16,17,18,57,0,-80,0,69,88,-80,8,47,27,-79,8,30,62,89,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,2,47,27,-79,2,18,62,89,-78,6,8,2,17,18,57,-80,6,47,-79,5,1,-80,10,43,88,33,-40,27,-12,89,-80,0,-48,
		-78,10,8,2,17,18,57,-80,6,16,-80,14,-48,48,49,1,35,17,35,17,35,53,51,1,51,1,1,51,1,51,3,-90,-31,-64,-37,-108,-2,81,-36,1,122,1,124,-38,-2,81,-102,2,9,-3,-9,2,9,-105,3,16,-3,37,2,-37,-4,-16,0,1,0,46,-2,96,3,-33,4,58,0,14,0,100,-78,10,15,16,17,18,57,0,-80,0,69,88,-80,8,
		47,27,-79,8,26,62,89,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,2,47,27,-79,2,20,62,89,-80,0,69,88,-80,0,47,27,-79,0,18,62,89,-80,0,69,88,-80,4,47,27,-79,4,18,62,89,-79,6,1,-80,10,43,88,33,-40,27,-12,89,-78,10,11,0,17,18,57,-80,13,-48,-80,14,-48,48,49,5,35,17,35,
		17,35,53,51,1,51,1,1,51,1,51,3,74,-26,-70,-36,-65,-2,-95,-67,1,31,1,24,-67,-2,-93,-56,11,-2,107,1,-107,-105,3,-82,-4,-38,3,38,-4,82,0,1,0,57,0,0,4,-50,5,-80,0,17,0,100,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-80,0,69,88,-80,14,47,27,-79,14,30,62,89,-80,0,69,88,-80,2,47,
		27,-79,2,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,17,11,2,17,18,57,-80,17,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,11,2,17,18,57,-80,7,-48,-80,17,16,-80,9,-48,-78,13,11,2,17,18,57,48,49,1,35,1,35,1,1,35,1,35,53,51,1,51,1,1,51,1,51,3,-60,-92,1,-82,-28,
		-2,-102,-2,-104,-29,1,-81,-96,-111,-2,107,-31,1,95,1,93,-30,-2,107,-106,2,-98,-3,98,2,56,-3,-56,2,-98,-105,2,123,-3,-46,2,46,-3,-123,0,0,1,0,41,0,0,3,-54,4,58,0,17,0,100,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-80,0,69,88,-80,14,47,27,-79,14,26,62,89,-80,0,69,88,-80,2,47,27,-79,
		2,18,62,89,-80,0,69,88,-80,5,47,27,-79,5,18,62,89,-78,17,14,2,17,18,57,-80,17,47,-79,0,1,-80,10,43,88,33,-40,27,-12,89,-78,4,14,2,17,18,57,-80,7,-48,-80,17,16,-80,9,-48,-78,13,14,2,17,18,57,48,49,1,35,1,35,3,3,35,1,35,53,51,1,51,19,19,51,1,51,3,60,-77,1,65,-42,-6,-6,
		-41,1,65,-86,-98,-2,-42,-42,-19,-16,-40,-2,-42,-89,1,-31,-2,31,1,-107,-2,107,1,-31,-105,1,-62,-2,117,1,-117,-2,62,0,-1,-1,0,99,-1,-20,3,-20,4,77,2,6,0,-65,0,0,-1,-1,0,18,0,0,4,47,5,-80,2,38,0,42,0,0,0,7,2,38,-1,-125,-2,127,-1,-1,0,-111,2,-117,5,-55,3,34,0,70,1,-81,-124,0,
		102,102,64,0,-1,-1,0,93,0,0,4,51,5,-60,2,6,0,22,0,0,-1,-1,0,94,-1,-20,3,-7,5,-60,2,6,0,23,0,0,-1,-1,0,53,0,0,4,80,5,-80,2,6,0,24,0,0,-1,-1,0,-102,-1,-20,4,45,5,-80,2,6,0,25,0,0,-1,-1,0,-104,-1,-20,4,48,5,-79,0,6,0,26,20,0,-1,-1,0,-124,-1,-20,
		4,34,5,-60,0,6,0,28,20,0,-1,-1,0,100,-1,-1,3,-8,5,-60,0,6,0,29,0,0,-1,-1,0,-121,-1,-20,4,30,5,-60,0,6,0,20,20,0,-1,-1,0,122,-1,-20,4,-36,7,87,2,38,0,43,0,0,1,7,0,117,1,-66,1,87,0,20,0,-80,0,69,88,-80,11,47,27,-79,11,30,62,89,-79,34,8,-12,48,49,-1,-1,
		0,96,-2,86,3,-14,6,0,2,38,0,75,0,0,1,7,0,117,1,75,0,0,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,39,9,-12,48,49,-1,-1,0,-87,0,0,5,8,7,54,2,38,0,50,0,0,1,7,0,68,1,102,1,54,0,20,0,-80,0,69,88,-80,6,47,27,-79,6,30,62,89,-79,11,8,-12,48,49,
		-1,-1,0,-116,0,0,3,-33,6,0,2,38,0,82,0,0,1,7,0,68,0,-52,0,0,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,26,62,89,-79,19,9,-12,48,49,-1,-1,0,28,0,0,5,29,7,32,2,38,0,37,0,0,1,7,0,-84,4,109,1,50,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,12,8,-12,
		-80,16,-48,48,49,0,-1,-1,0,57,-1,-20,3,-22,5,-21,2,38,0,69,0,0,1,7,0,-84,4,18,-1,-3,0,23,0,-80,0,69,88,-80,23,47,27,-79,23,26,62,89,-79,42,9,-12,-80,46,-48,48,49,0,-1,-1,0,95,0,0,4,70,7,44,2,38,0,41,0,0,1,7,0,-84,4,56,1,62,0,23,0,-80,0,69,88,-80,6,47,
		27,-79,6,30,62,89,-79,13,8,-12,-80,17,-48,48,49,0,-1,-1,0,41,-1,-20,3,-13,5,-21,2,38,0,73,0,0,1,7,0,-84,4,2,-1,-3,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,26,62,89,-79,31,9,-12,-80,35,-48,48,49,0,-1,-1,-1,10,0,0,1,-22,7,44,2,38,0,45,0,0,1,7,0,-84,2,-29,1,62,
		0,23,0,-80,0,69,88,-80,2,47,27,-79,2,30,62,89,-79,5,8,-12,-80,9,-48,48,49,0,-1,-1,-2,-16,0,0,1,-48,5,-23,2,38,0,-115,0,0,1,7,0,-84,2,-55,-1,-5,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,26,62,89,-79,5,9,-12,-80,9,-48,48,49,0,-1,-1,0,118,-1,-20,5,9,7,34,2,38,0,51,
		0,0,1,7,0,-84,4,-113,1,52,0,23,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-79,33,8,-12,-80,37,-48,48,49,0,-1,-1,0,51,-1,-20,4,52,5,-21,2,38,0,83,0,0,1,7,0,-84,4,12,-1,-3,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,26,62,89,-79,29,9,-12,-80,33,-48,48,49,0,-1,-1,0,85,
		0,0,4,-55,7,32,2,38,0,54,0,0,1,7,0,-84,4,46,1,50,0,23,0,-80,0,69,88,-80,4,47,27,-79,4,30,62,89,-79,25,8,-12,-80,29,-48,48,49,0,-1,-1,-1,-117,0,0,2,-105,5,-21,2,38,0,86,0,0,1,7,0,-84,3,100,-1,-3,0,23,0,-80,0,69,88,-80,11,47,27,-79,11,26,62,89,-79,15,9,-12,
		-80,19,-48,48,49,0,-1,-1,0,-116,-1,-20,4,-86,7,32,2,38,0,57,0,0,1,7,0,-84,4,104,1,50,0,23,0,-80,0,69,88,-80,9,47,27,-79,9,30,62,89,-79,20,8,-12,-80,24,-48,48,49,0,-1,-1,0,43,-1,-20,3,-36,5,-21,2,38,0,89,0,0,1,7,0,-84,4,4,-1,-3,0,23,0,-80,0,69,88,-80,7,47,
		27,-79,7,26,62,89,-79,18,9,-12,-80,22,-48,48,49,0,-1,-1,-2,-42,0,0,4,-46,6,63,0,38,0,-48,100,0,0,7,0,-82,-2,31,0,0,-1,-1,0,-87,-2,-84,4,-120,5,-80,2,38,0,38,0,0,0,7,0,-83,4,-70,0,10,-1,-1,0,-116,-2,-103,4,32,6,0,2,38,0,70,0,0,0,7,0,-83,4,-85,-1,-9,-1,-1,
		0,-87,-2,-84,4,-58,5,-80,2,38,0,40,0,0,0,7,0,-83,4,-71,0,10,-1,-1,0,95,-2,-94,3,-16,6,0,2,38,0,72,0,0,0,7,0,-83,4,-67,0,0,-1,-1,0,-87,-2,9,4,-58,5,-80,2,38,0,40,0,0,1,7,1,-70,1,101,-2,-86,0,8,0,-78,0,26,1,93,48,49,-1,-1,0,95,-3,-1,3,-16,6,0,
		2,38,0,72,0,0,0,7,1,-70,1,105,-2,-96,-1,-1,0,-87,-2,-84,5,8,5,-80,2,38,0,44,0,0,0,7,0,-83,5,31,0,10,-1,-1,0,-116,-2,-84,3,-33,6,0,2,38,0,76,0,0,0,7,0,-83,4,-95,0,10,-1,-1,0,-87,0,0,5,5,7,48,2,38,0,47,0,0,1,7,0,117,1,123,1,48,0,20,0,-80,
		0,69,88,-80,5,47,27,-79,5,30,62,89,-79,14,8,-12,48,49,-1,-1,0,-115,0,0,4,12,7,65,2,38,0,79,0,0,1,7,0,117,1,68,1,65,0,9,0,-80,5,47,-80,15,-36,48,49,0,-1,-1,0,-87,-2,-5,5,5,5,-80,2,38,0,47,0,0,0,7,0,-83,4,-24,0,89,-1,-1,0,-115,-2,-24,4,12,6,0,2,38,
		0,79,0,0,0,7,0,-83,4,101,0,70,-1,-1,0,-87,-2,-84,4,28,5,-80,2,38,0,48,0,0,0,7,0,-83,4,-64,0,10,-1,-1,0,-122,-2,-84,1,97,6,0,2,38,0,80,0,0,0,7,0,-83,3,78,0,10,-1,-1,0,-87,-2,-84,6,82,5,-80,2,38,0,49,0,0,0,7,0,-83,5,-46,0,10,-1,-1,0,-117,-2,-84,
		6,120,4,78,2,38,0,81,0,0,0,7,0,-83,5,-42,0,10,-1,-1,0,-87,-2,-84,5,8,5,-80,2,38,0,50,0,0,0,7,0,-83,5,36,0,10,-1,-1,0,-116,-2,-84,3,-33,4,78,2,38,0,82,0,0,0,7,0,-83,4,-121,0,10,-1,-1,0,118,-1,-20,5,9,7,-26,2,38,0,51,0,0,1,7,2,54,5,11,1,83,
		0,42,0,-80,0,69,88,-80,13,47,27,-79,13,30,62,89,-80,35,-36,-78,127,35,1,113,-78,-17,35,1,113,-78,79,35,1,113,-78,47,35,1,113,-80,55,-48,48,49,-1,-1,0,-87,0,0,4,-64,7,66,2,38,0,52,0,0,1,7,0,117,1,124,1,66,0,20,0,-80,0,69,88,-80,3,47,27,-79,3,30,62,89,-79,22,8,-12,48,49,
		-1,-1,0,-116,-2,96,4,30,5,-9,2,38,0,84,0,0,1,7,0,117,1,-109,-1,-9,0,20,0,-80,0,69,88,-80,12,47,27,-79,12,26,62,89,-79,29,9,-12,48,49,-1,-1,0,-88,-2,-84,4,-55,5,-80,2,38,0,54,0,0,0,7,0,-83,4,-73,0,10,-1,-1,0,-126,-2,-84,2,-105,4,78,2,38,0,86,0,0,0,7,0,-83,
		3,74,0,10,-1,-1,0,80,-2,-94,4,114,5,-60,2,38,0,55,0,0,0,7,0,-83,4,-55,0,0,-1,-1,0,95,-2,-102,3,-69,4,78,2,38,0,87,0,0,0,7,0,-83,4,-121,-1,-8,-1,-1,0,49,-2,-94,4,-105,5,-80,2,38,0,56,0,0,0,7,0,-83,4,-70,0,0,-1,-1,0,9,-2,-94,2,86,5,64,2,38,0,88,
		0,0,0,7,0,-83,4,25,0,0,-1,-1,0,-116,-1,-20,4,-86,7,-28,2,38,0,57,0,0,1,7,2,54,4,-28,1,81,0,22,0,-80,0,69,88,-80,18,47,27,-79,18,30,62,89,-80,22,-36,-80,42,-48,48,49,-1,-1,0,28,0,0,4,-3,7,46,2,38,0,58,0,0,1,7,0,-91,0,-76,1,70,0,20,0,-80,0,69,88,-80,
		6,47,27,-79,6,30,62,89,-79,10,4,-12,48,49,-1,-1,0,33,0,0,3,-70,5,-29,2,38,0,90,0,0,1,6,0,-91,29,-5,0,20,0,-80,0,69,88,-80,1,47,27,-79,1,26,62,89,-79,10,1,-12,48,49,-1,-1,0,28,-2,-84,4,-3,5,-80,2,38,0,58,0,0,0,7,0,-83,4,-28,0,10,-1,-1,0,33,-2,-84,3,-70,
		4,58,2,38,0,90,0,0,0,7,0,-83,4,77,0,10,-1,-1,0,61,-2,-84,6,-19,5,-80,2,38,0,59,0,0,0,7,0,-83,5,-17,0,10,-1,-1,0,43,-2,-84,5,-45,4,58,2,38,0,91,0,0,0,7,0,-83,5,83,0,10,-1,-1,0,86,-2,-84,4,122,5,-80,2,38,0,62,0,0,0,7,0,-83,4,-70,0,10,-1,-1,
		0,88,-2,-84,3,-77,4,58,2,38,0,94,0,0,0,7,0,-83,4,98,0,10,-1,-1,-2,50,-1,-20,5,79,5,-42,0,38,0,51,70,0,0,7,1,113,-3,-61,0,0,-1,-1,0,19,0,0,4,112,5,28,2,38,2,51,0,0,0,7,0,-82,-1,-36,-2,-35,-1,-1,-1,99,0,0,3,-22,5,31,0,38,2,40,60,0,0,7,0,-82,
		-2,-84,-2,-32,-1,-1,-1,-128,0,0,4,-108,5,28,0,38,1,-28,60,0,0,7,0,-82,-2,-55,-2,-35,-1,-1,-1,-124,0,0,1,-115,5,30,0,38,1,-29,60,0,0,7,0,-82,-2,-51,-2,-33,-1,-1,-1,-43,-1,-16,4,100,5,28,0,38,1,-35,10,0,0,7,0,-82,-1,30,-2,-35,-1,-1,-1,27,0,0,4,88,5,28,0,38,1,-45,
		60,0,0,7,0,-82,-2,100,-2,-35,-1,-1,-1,-18,0,0,4,-120,5,27,0,38,1,-13,10,0,0,7,0,-82,-1,55,-2,-36,-1,-1,0,19,0,0,4,112,4,-115,2,6,2,51,0,0,-1,-1,0,-118,0,0,3,-17,4,-115,2,6,2,50,0,0,-1,-1,0,-118,0,0,3,-82,4,-115,2,6,2,40,0,0,-1,-1,0,71,0,0,3,-32,
		4,-115,2,6,1,-46,0,0,-1,-1,0,-118,0,0,4,88,4,-115,2,6,1,-28,0,0,-1,-1,0,-105,0,0,1,81,4,-115,2,6,1,-29,0,0,-1,-1,0,-118,0,0,4,87,4,-115,2,6,1,-31,0,0,-1,-1,0,-118,0,0,5,119,4,-115,2,6,1,-33,0,0,-1,-1,0,-118,0,0,4,88,4,-115,2,6,1,-34,0,0,-1,-1,
		0,96,-1,-16,4,90,4,-99,2,6,1,-35,0,0,-1,-1,0,-118,0,0,4,27,4,-115,2,6,1,-36,0,0,-1,-1,0,40,0,0,3,-3,4,-115,2,6,1,-40,0,0,-1,-1,0,13,0,0,4,28,4,-115,2,6,1,-45,0,0,-1,-1,0,38,0,0,4,49,4,-115,2,6,1,-44,0,0,-1,-1,-1,-77,0,0,2,60,5,-29,2,38,
		1,-29,0,0,1,7,0,106,-1,78,0,30,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,11,2,-12,-80,20,-48,48,49,0,-1,-1,0,13,0,0,4,28,5,-29,2,38,1,-45,0,0,1,6,0,106,109,30,0,23,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,16,2,-12,-80,25,-48,48,49,0,-1,-1,0,-118,
		0,0,3,-82,5,-29,2,38,2,40,0,0,1,6,0,106,113,30,0,23,0,-80,0,69,88,-80,6,47,27,-79,6,28,62,89,-79,19,2,-12,-80,28,-48,48,49,0,-1,-1,0,-118,0,0,3,-123,6,30,2,38,1,-22,0,0,1,7,0,117,1,52,0,30,0,20,0,-80,0,69,88,-80,4,47,27,-79,4,28,62,89,-79,8,6,-12,48,49,
		-1,-1,0,67,-1,-16,3,-35,4,-99,2,6,1,-39,0,0,-1,-1,0,-105,0,0,1,81,4,-115,2,6,1,-29,0,0,-1,-1,-1,-77,0,0,2,60,5,-29,2,38,1,-29,0,0,1,7,0,106,-1,78,0,30,0,23,0,-80,0,69,88,-80,2,47,27,-79,2,28,62,89,-79,11,2,-12,-80,20,-48,48,49,0,-1,-1,0,43,-1,-16,3,77,
		4,-115,2,6,1,-30,0,0,-1,-1,0,-118,0,0,4,87,6,30,2,38,1,-31,0,0,1,7,0,117,1,37,0,30,0,20,0,-80,0,69,88,-80,5,47,27,-79,5,28,62,89,-79,15,6,-12,48,49,-1,-1,0,34,-1,-20,4,11,5,-10,2,38,2,1,0,0,1,6,0,-95,103,31,0,20,0,-80,0,69,88,-80,2,47,27,-79,2,28,
		62,89,-79,20,8,-12,48,49,-1,-1,0,19,0,0,4,112,4,-115,2,6,2,51,0,0,-1,-1,0,-118,0,0,3,-17,4,-115,2,6,2,50,0,0,-1,-1,0,-118,0,0,3,-123,4,-115,2,6,1,-22,0,0,-1,-1,0,-118,0,0,3,-82,4,-115,2,6,2,40,0,0,-1,-1,0,-118,0,0,4,97,5,-10,2,38,1,-2,0,0,1,7,
		0,-95,0,-55,0,31,0,20,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-79,13,8,-12,48,49,-1,-1,0,-118,0,0,5,119,4,-115,2,6,1,-33,0,0,-1,-1,0,-118,0,0,4,88,4,-115,2,6,1,-28,0,0,-1,-1,0,96,-1,-16,4,90,4,-99,2,6,1,-35,0,0,-1,-1,0,-118,0,0,4,68,4,-115,2,6,1,-17,
		0,0,-1,-1,0,-118,0,0,4,27,4,-115,2,6,1,-36,0,0,-1,-1,0,96,-1,-16,4,48,4,-99,2,6,2,49,0,0,-1,-1,0,40,0,0,3,-3,4,-115,2,6,1,-40,0,0,-1,-1,0,38,0,0,4,49,4,-115,2,6,1,-44,0,0,0,1,0,71,-2,80,3,-44,4,-99,0,41,0,-99,0,-80,0,69,88,-80,10,47,27,-79,
		10,28,62,89,-80,0,69,88,-80,25,47,27,-79,25,18,62,89,-80,0,69,88,-80,24,47,27,-79,24,20,62,89,-80,10,16,-79,3,1,-80,10,43,88,33,-40,27,-12,89,-78,6,10,25,17,18,57,-78,39,25,10,17,18,57,124,-80,39,47,24,-78,-16,39,1,93,-78,0,39,1,113,-78,-96,39,1,93,-76,96,39,112,39,2,93,-78,48,39,1,
		113,-76,96,39,112,39,2,113,-79,38,1,-80,10,43,88,33,-40,27,-12,89,-78,16,38,39,17,18,57,-80,25,16,-80,22,-48,-78,29,25,10,17,18,57,-80,25,16,-79,32,1,-80,10,43,88,33,-40,27,-12,89,48,49,1,52,38,35,34,6,21,35,52,54,51,50,22,21,20,6,7,22,22,21,20,6,7,17,35,17,38,38,53,51,22,22,51,
		50,54,53,52,37,35,53,51,54,3,8,-118,125,110,-127,-70,-19,-68,-45,-18,110,103,118,113,-53,-81,-70,-93,-74,-71,5,-125,121,-120,-110,-2,-1,-99,-100,-17,3,80,84,93,88,79,-114,-75,-88,-106,86,-115,41,36,-110,91,-116,-81,18,-2,91,1,-89,20,-83,-120,86,96,96,88,-63,5,-104,5,0,1,0,-118,-2,-103,4,-6,4,-115,0,15,0,95,0,-80,
		1,47,-80,0,69,88,-80,9,47,27,-79,9,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,0,69,88,-80,6,47,27,-79,6,18,62,89,-78,11,3,9,17,18,57,124,-80,11,47,24,-78,-96,11,1,93,-79,4,1,-80,10,43,88,33,-40,27,-12,89,-80,9,16,-80,12,-48,-80,3,16,-79,14,1,-80,10,43,88,33,-40,27,-12,
		89,48,49,1,35,17,35,17,33,17,35,17,51,17,33,17,51,17,51,4,-6,-70,-95,-3,-92,-71,-71,2,92,-71,-94,-2,-103,1,103,1,-14,-2,14,4,-115,-3,-3,2,3,-4,12,0,0,1,0,96,-2,86,4,48,4,-99,0,31,0,90,0,-80,0,69,88,-80,14,47,27,-79,14,28,62,89,-80,0,69,88,-80,3,47,27,-79,3,18,62,89,-80,
		0,69,88,-80,5,47,27,-79,5,20,62,89,-80,3,16,-80,6,-48,-80,14,16,-80,18,-48,-80,14,16,-79,21,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-79,28,1,-80,10,43,88,33,-40,27,-12,89,-80,3,16,-80,31,-48,48,49,1,6,6,7,17,35,17,38,2,53,53,52,54,54,51,50,22,23,35,38,38,35,34,6,7,21,20,22,
		51,50,54,55,4,48,20,-53,-87,-70,-73,-41,123,-25,-104,-52,-9,19,-71,18,-115,126,-103,-89,1,-97,-105,-121,-115,20,1,121,-88,-57,20,-2,96,1,-94,30,1,30,-29,97,-92,-7,-120,-45,-69,-126,116,-53,-67,106,-67,-49,111,-125,-1,-1,0,13,0,0,4,28,4,-115,2,6,1,-45,0,0,-1,-1,0,2,-2,81,5,107,4,-99,2,38,2,23,0,0,
		0,7,2,81,2,-68,-1,-72,-1,-1,0,-118,0,0,4,97,5,-53,2,38,1,-2,0,0,1,7,0,112,0,-100,0,38,0,19,0,-80,0,69,88,-80,8,47,27,-79,8,28,62,89,-80,11,-36,48,49,0,-1,-1,0,34,-1,-20,4,11,5,-53,2,38,2,1,0,0,1,6,0,112,58,38,0,19,0,-80,0,69,88,-80,17,47,27,-79,17,28,
		62,89,-80,19,-36,48,49,0,-1,-1,0,96,0,0,5,6,4,-115,2,6,1,-15,0,0,-1,-1,0,-105,-1,-16,5,53,4,-115,0,38,1,-29,0,0,0,7,1,-30,1,-24,0,0,-1,-1,0,9,0,0,5,-15,6,0,2,38,2,115,0,0,0,7,0,117,2,-98,0,0,-1,-1,0,96,-1,-57,4,90,6,30,2,38,2,117,0,0,0,7,
		0,117,1,125,0,30,-1,-1,0,67,-3,-1,3,-35,4,-99,2,38,1,-39,0,0,0,7,1,-70,1,41,-2,-96,-1,-1,0,49,0,0,5,-15,6,30,2,38,1,-43,0,0,0,7,0,68,1,-94,0,30,-1,-1,0,49,0,0,5,-15,6,30,2,38,1,-43,0,0,0,7,0,117,2,49,0,30,-1,-1,0,49,0,0,5,-15,5,-29,2,38,
		1,-43,0,0,0,7,0,106,1,107,0,30,-1,-1,0,13,0,0,4,28,6,30,2,38,1,-45,0,0,0,7,0,68,0,-92,0,30,-1,-1,0,28,-2,79,5,29,5,-80,2,38,0,37,0,0,0,7,0,-92,1,124,0,0,-1,-1,0,109,-2,79,3,-22,4,78,2,38,0,69,0,0,0,7,0,-92,0,-60,0,0,-1,-1,0,-87,-2,89,
		4,70,5,-80,2,38,0,41,0,0,0,7,0,-92,1,58,0,10,-1,-1,0,93,-2,79,3,-13,4,78,2,38,0,73,0,0,0,7,0,-92,1,6,0,0,-1,-1,0,19,-2,79,4,112,4,-115,2,38,2,51,0,0,0,7,0,-92,1,30,0,0,-1,-1,0,-118,-2,87,3,-82,4,-115,2,38,2,40,0,0,0,7,0,-92,0,-25,0,8,
		-1,-1,0,-123,-2,-84,1,96,4,58,2,38,0,-115,0,0,0,7,0,-83,3,77,0,10,0,0,0,26,1,62,0,1,0,0,0,0,0,0,0,47,0,0,0,1,0,0,0,0,0,1,0,6,0,47,0,1,0,0,0,0,0,2,0,7,0,53,0,1,0,0,0,0,0,3,0,6,0,47,0,1,0,0,0,0,0,4,0,6,0,47,
		0,1,0,0,0,0,0,5,0,19,0,60,0,1,0,0,0,0,0,6,0,14,0,79,0,1,0,0,0,0,0,7,0,32,0,93,0,1,0,0,0,0,0,9,0,6,0,125,0,1,0,0,0,0,0,11,0,10,0,-125,0,1,0,0,0,0,0,12,0,19,0,-115,0,1,0,0,0,0,0,13,0,46,0,-96,0,1,0,0,0,0,
		0,14,0,42,0,-50,0,3,0,1,4,9,0,0,0,94,0,-8,0,3,0,1,4,9,0,1,0,12,1,86,0,3,0,1,4,9,0,2,0,14,1,98,0,3,0,1,4,9,0,3,0,12,1,