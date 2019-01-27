/*

	GUI library that follows Google's Material design
	guidelines, by Björn Sundin aka Avocado Boy.
	It is very freely themable, and easy to use.

	   (me)
		v
		--
   \  --  --    /
	\|      \ /
	|   /\   \
	\   \/   /
	 \      /
	  ------
	   | |

*/

#pragma once

//------------------------------

#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <map>

// For debugging.
#include <iostream>

//------------------------------

namespace AvoGUI
{
	/// <summary>
	/// Returns the square root of a float using a fast but less accurate algorithm.
	/// </summary>
	inline float fastSqrt(float p_x)
	{
		int32_t bits = (*(int32_t*)&p_x - (1 << 23) >> 1) + (1 << 29);
		float approximation = *(float*)&bits;
		return (p_x / approximation + approximation) * 0.5f;
	}
	/// <summary>
	/// Returns the inverse square root of a float using a fast but less accurate algorithm.
	/// </summary>
	inline float fastInverseSqrt(float p_x)
	{
		int32_t bits = 0x5f375a86 - (*(int32_t*)&p_x >> 1);
		float approximation = *(float*)&bits;
		return approximation * (1.5f - 0.5f*p_x*approximation*approximation);
	}

	/// <summary>
	/// Returns the biggest of two numbers.
	/// </summary>
	template<typename Type>
	Type max(Type p_a, Type p_b)
	{
		return p_a > p_b ? p_a : p_b;
	}
	/// <summary>
	/// Returns the biggest of three numbers.
	/// </summary>
	template<typename Type>
	Type max(Type p_a, Type p_b, Type p_c)
	{
		return p_a > p_b ? (p_a > p_c ? p_a : p_c) : (p_b > p_c ? p_b : p_c);
	}

	/// <summary>
	/// Returns the smallest of two numbers.
	/// </summary>
	template<typename Type>
	Type min(Type p_a, Type p_b)
	{
		return p_a < p_b ? p_a : p_b;
	}
	/// <summary>
	/// Returns the smallest of three numbers.
	/// </summary>
	template<typename Type>
	Type min(Type p_a, Type p_b, Type p_c)
	{
		return p_a < p_b ? (p_a < p_c ? p_a : p_c) : (p_b < p_c ? p_b : p_c);
	}

	//------------------------------

	/// <summary>
	/// Returns a value between p_start and p_end depending on p_progress. This is linear interpolation.
	/// <param name="p_progress">If this is below 0 or above 1, the returned value will not be within the start and end position.</param>
	/// </summary>
	template<typename Type>
	Type interpolate(Type p_start, Type p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * p_progress;
	}
	/// <summary>
	/// Clips p_value so that the returned value is never below p_min or above p_max. If p_min <= p_value <= p_max, then the returned value is equal to p_value.
	/// </summary>
	template<typename Type>
	Type constrain(Type p_value, Type p_min = 0, Type p_max = 1)
	{
		return p_value < p_min ? p_min : (p_value > p_max ? p_max : p_value);
	}

	//------------------------------

	/// <summary>
	/// Removes an element from a vector.
	/// </summary>
	template<typename Type>
	void removeVectorElement(std::vector<Type>& p_vector, Type p_value)
	{
		for (uint32_t a = 0; a < p_vector.size(); a++)
		{
			if (p_vector[a] == p_value)
			{
				p_vector.erase(p_vector.begin() + a);
				return;
			}
		}
	}

	//------------------------------

	/// <summary>
	/// <para>Converts a const char* string to a wchar* string.</para>
	/// <para>Remember to delete[] the result when you're done!</para>
	/// </summary>
	wchar_t* widenString(const char* p_string);

	//------------------------------

	/// <summary>
	/// A 2D point where x is the horizontal component and y is the vertical component.
	/// Or just a list of two numbers.
	/// </summary>
	template<typename PointType = float>
	struct Point
	{
		PointType x, y;

		inline Point()
		{
			x = y = (PointType)0;
		}
		inline Point(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
		}
		/// <summary>
		/// Initializes the point with the same x and y coordinates.
		/// </summary>
		/// <param name="p_coordinate">x and y coordinate</param>
		inline Point(PointType p_coordinate)
		{
			x = p_coordinate;
			y = p_coordinate;
		}
		template<typename T>
		inline Point(const Point<T>& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
		}
		template<typename T>
		inline Point(Point<T>&& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
		}

		//------------------------------

		inline Point<PointType> operator-() const
		{
			return Point<PointType>(-x, -y);
		}

		//------------------------------

		template<typename T>
		inline Point<PointType>& operator=(const Point<T>& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}
		template<typename T>
		inline Point<PointType>& operator=(Point<T>&& p_point)
		{
			x = (PointType)p_point.x;
			y = (PointType)p_point.y;
			return *this;
		}

		/// <summary>
		/// Sets the same value for the x and the y coordinates.
		/// </summary>
		/// <param name="p_coordinate">x and y coordinate</param>
		inline Point<PointType>& operator=(PointType p_coordinate)
		{
			x = y = p_coordinate;
			return *this;
		}

		//------------------------------

		inline void set(PointType p_x, PointType p_y)
		{
			x = p_x;
			y = p_y;
		}

		//------------------------------

		inline bool operator==(const Point<PointType>& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		inline bool operator==(Point<PointType>&& p_point) const
		{
			return x == p_point.x && y == p_point.y;
		}
		inline bool operator==(PointType p_coordinate) const
		{
			return x == p_coordinate && y == p_coordinate;
		}

		//------------------------------

		inline bool operator!=(const Point<PointType>& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		inline bool operator!=(Point<PointType>&& p_point) const
		{
			return x != p_point.x || y != p_point.y;
		}
		inline bool operator!=(PointType p_coordinate) const
		{
			return x != p_coordinate || y != p_coordinate;
		}

		//------------------------------

		template<typename T>
		inline Point<PointType> operator+(const Point<T>& p_point) const
		{
			return Point<PointType>(x + (PointType)p_point.x, y + (PointType)p_point.y);
		}
		template<typename T>
		inline Point<PointType> operator+(Point<T>&& p_point) const
		{
			return Point<PointType>(x + (PointType)p_point.x, y + (PointType)p_point.y);
		}
		/// <summary>
		/// Returns this point offset by the same amount on the x and y axis.
		/// </summary>
		template<typename OffsetType>
		inline Point<PointType> operator+(OffsetType p_offset) const
		{
			return Point<PointType>(x + (PointType)p_offset, y + (PointType)p_offset);
		}

		template<typename OffsetType>
		inline Point<PointType>& operator+=(const Point<OffsetType>& p_offset)
		{
			x += (PointType)p_offset.x;
			y += (PointType)p_offset.y;
			return *this;
		}
		inline Point<PointType>& operator+=(PointType p_offset)
		{
			x += p_offset;
			y += p_offset;
			return *this;
		}

		//------------------------------

		template<typename T>
		inline Point<PointType> operator-(const Point<T>& p_point) const
		{
			return Point<PointType>(x - (PointType)p_point.x, y - (PointType)p_point.y);
		}
		template<typename T>
		inline Point<PointType> operator-(Point<T>&& p_point) const
		{
			return Point<PointType>(x - (PointType)p_point.x, y - (PointType)p_point.y);
		}
		inline Point<PointType> operator-(PointType p_offset) const
		{
			return Point<PointType>(x - p_offset, y - p_offset);
		}

		template<typename OffsetType>
		inline Point<PointType>& operator-=(const Point<OffsetType>& p_offset)
		{
			x -= (PointType)p_offset.x;
			y -= (PointType)p_offset.y;
			return *this;
		}
		inline Point<PointType>& operator-=(PointType p_offset)
		{
			x -= p_offset;
			y -= p_offset;
			return *this;
		}

		//------------------------------

		template<typename T>
		inline Point<PointType> operator*(const Point<T>& p_point) const
		{
			return Point<PointType>(x*(PointType)p_point.x, y*(PointType)p_point.y);
		}
		template<typename T>
		inline Point<PointType> operator*(Point<T>&& p_point) const
		{
			return Point<PointType>(x*(PointType)p_point.x, y*(PointType)p_point.y);
		}
		inline Point<PointType> operator*(double p_factor) const
		{
			return Point<PointType>(x*p_factor, y*p_factor);
		}

		template<typename T>
		inline Point<PointType>& operator*=(const Point<T>& p_point)
		{
			x *= (PointType)p_point.x;
			x *= (PointType)p_point.y;
			return *this;
		}
		inline Point<PointType>& operator*=(double p_factor)
		{
			x *= p_factor;
			x *= p_factor;
			return *this;
		}

		//------------------------------

		template<typename T>
		inline Point<PointType> operator/(const Point<T>& p_point) const
		{
			return Point<PointType>(x / (PointType)p_point.x, y / (PointType)p_point.y);
		}
		template<typename T>
		inline Point<PointType> operator/(Point<T>&& p_point) const
		{
			return Point<PointType>(x / (PointType)p_point.x, y / (PointType)p_point.y);
		}
		inline Point<PointType> operator/(double p_divisor) const
		{
			return Point<PointType>(x / p_divisor, y / p_divisor);
		}

		template<typename T>
		inline Point<PointType>& operator/=(const Point<T>& p_point)
		{
			x /= (PointType)p_point.x;
			x /= (PointType)p_point.y;
			return *this;
		}
		inline Point<PointType>& operator/=(double p_divisor)
		{
			x /= p_divisor;
			x /= p_divisor;
			return *this;
		}

		//------------------------------

		template<typename T>
		inline double getDotProduct(const Point<T>& p_point) const
		{
			return x * p_point.x + y * p_point.y;
		}
		inline double getDotProduct(double p_x, double p_y) const
		{
			return x * p_x + y * p_y;
		}

		template<typename T>
		inline double getCrossProduct(const Point<T>& p_point) const
		{
			return x * p_point.x - y * p_point.x;
		}
		inline double getCrossProduct(double p_x, double p_y) const
		{
			return x * p_y - y * p_x;
		}

		//------------------------------

		inline PointType getLengthSquared() const
		{
			return x * x + y * y;
		}
		inline double getLength() const
		{
			return sqrt(x*x + y * y);
		}
		inline double getLengthFast() const
		{
			return fastSqrt(x*x + y * y);
		}

		static inline double getLengthSquared(float p_x, float p_y)
		{
			return p_x * p_x + p_y * p_y;
		}
		static inline double getLength(float p_x, float p_y)
		{
			return sqrt(p_x * p_x + p_y * p_y);
		}
		static inline double getLengthFast(float p_x, float p_y)
		{
			return fastSqrt(p_x * p_x + p_y * p_y);
		}

		template<typename T0, typename T1>
		static inline double getDistanceSquared(const Point<T0>& p_point_0, const Point<T1>& p_point_1)
		{
			return (p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y);
		}
		static inline double getDistanceSquared(double p_x0, double p_y0, double p_x1, double p_y1)
		{
			return (p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0);
		}
		template<typename T0, typename T1>
		static inline double getDistance(const Point<T0>& p_point_0, const Point<T1>& p_point_1)
		{
			return sqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		static inline double getDistance(double p_x0, double p_y0, double p_x1, double p_y1)
		{
			return sqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}
		template<typename T0, typename T1>
		static inline double getDistanceFast(const Point<T0>& p_point_0, const Point<T1>& p_point_1)
		{
			return fastSqrt((p_point_1.x - p_point_0.x)*(p_point_1.x - p_point_0.x) + (p_point_1.y - p_point_0.y)*(p_point_1.y - p_point_0.y));
		}
		static inline double getDistanceFast(double p_x0, double p_y0, double p_x1, double p_y1)
		{
			return fastSqrt((p_x1 - p_x0)*(p_x1 - p_x0) + (p_y1 - p_y0)*(p_y1 - p_y0));
		}

		//------------------------------

		inline Point<PointType>& normalize()
		{
			float length = sqrt(x*x + y * y);
			x /= length;
			y /= length;
			return *this;
		}
		inline Point<PointType>& normalizeFast()
		{
			float inverseLength = fastInverseSqrt(x*x + y * y);
			x *= inverseLength;
			y *= inverseLength;
			return *this;
		}
	};

	template<typename Type>
	Point<Type> interpolate(const Point<Type>& p_start, const Point<Type>& p_end, double p_progress)
	{
		return p_start * (1.0 - p_progress) + p_end * (p_progress);
	}

	//------------------------------

	/// <summary>
	/// A 2D axis-aligned rectangle. right > left and bottom > top. 
	/// </summary>
	template<typename RectangleType = float>
	struct Rectangle
	{
		RectangleType left, top, right, bottom;

		inline Rectangle()
		{
			left = top = right = bottom = (RectangleType)0;
		}
		inline Rectangle(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			left = p_left;
			top = p_top;
			right = p_right;
			bottom = p_bottom;
		}
		template<typename PositionType, typename SizeType>
		inline Rectangle(const Point<PositionType>& p_position, const Point<SizeType>& p_size)
		{
			left = (RectangleType)p_position.x;
			top = (RectangleType)p_position.y;
			right = left + (RectangleType)p_size.x;
			bottom = top + (RectangleType)p_size.y;
		}
		template<typename ParameterRectangleType>
		inline Rectangle(const Rectangle<ParameterRectangleType>& p_rectangle)
		{
			*this = p_rectangle;
		}
		template<typename ParameterRectangleType>
		inline Rectangle(Rectangle<ParameterRectangleType>&& p_rectangle)
		{
			*this = p_rectangle;
		}

		//------------------------------

		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType>& operator=(const Rectangle<ParameterRectangleType>& p_rectangle)
		{
			left = (RectangleType)p_rectangle.left;
			top = (RectangleType)p_rectangle.top;
			right = (RectangleType)p_rectangle.right;
			bottom = (RectangleType)p_rectangle.bottom;
			return *this;
		}
		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType>& operator=(Rectangle<ParameterRectangleType>&& p_rectangle)
		{
			left = (RectangleType)p_rectangle.left;
			top = (RectangleType)p_rectangle.top;
			right = (RectangleType)p_rectangle.right;
			bottom = (RectangleType)p_rectangle.bottom;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// Offsets the position of the rectangle.
		/// </summary>
		/// <param name="p_point">Offset to use</param>
		template<typename OffsetType>
		inline Rectangle<RectangleType>& operator+=(const Point<OffsetType>& p_offset)
		{
			left += (RectangleType)p_offset.x;
			top += (RectangleType)p_offset.y;
			right += (RectangleType)p_offset.x;
			bottom += (RectangleType)p_offset.y;
			return *this;
		}
		/// <summary>
		/// Offsets the position of the rectangle negatively.
		/// </summary>
		/// <param name="p_point">Negative offset to use</param>
		template<typename OffsetType>
		inline Rectangle<RectangleType>& operator-=(const Point<OffsetType>& p_offset)
		{
			left -= (RectangleType)p_offset.x;
			top -= (RectangleType)p_offset.y;
			right -= (RectangleType)p_offset.x;
			bottom -= (RectangleType)p_offset.y;
			return *this;
		}

		//------------------------------

		template<typename ParameterRectangleType>
		inline bool operator==(const Rectangle<ParameterRectangleType>& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}
		template<typename ParameterRectangleType>
		inline bool operator==(Rectangle<ParameterRectangleType>&& p_rectangle) const
		{
			return left == p_rectangle.left && right == p_rectangle.right
				&& top == p_rectangle.top && bottom == p_rectangle.bottom;
		}

		template<typename ParameterRectangleType>
		inline bool operator!=(const Rectangle<ParameterRectangleType>& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}
		template<typename ParameterRectangleType>
		inline bool operator!=(Rectangle<ParameterRectangleType>&& p_rectangle) const
		{
			return left != p_rectangle.left || right != p_rectangle.right
				|| top != p_rectangle.top || bottom != p_rectangle.bottom;
		}

		//------------------------------

		inline Rectangle<RectangleType> createCopyAtOrigin() const
		{
			return Rectangle<RectangleType>(0, 0, right - left, bottom - top);
		}

		//------------------------------

		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new top-left corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType> createCopyWithTopLeft(const Point<PositionType>& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_position.x, p_position.y, p_willKeepSize*(p_position.x - left) + right, p_willKeepSize*(p_position.y - top) + bottom);
		}
		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new top-left corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType> createCopyWithTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_left, p_top, p_willKeepSize*(p_left - left) + right, p_willKeepSize*(p_top - top) + bottom);
		}
		/// <summary>
		/// <para>Sets the top left coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType>& setTopLeft(const Point<PositionType>& p_position, bool p_willKeepSize = true)
		{
			return setTopLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/// <summary>
		/// <para>Sets the top left coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setTopLeft(RectangleType p_left, RectangleType p_top, bool p_willKeepSize = true)
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
		/// <summary>
		/// Returns the top left coordinates of the rectangle.
		/// </summary>
		inline Point<RectangleType> getTopLeft() const
		{
			return Point<RectangleType>(left, top);
		}

		//------------------------------

		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new top-right corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType> createCopyWithTopRight(const Point<PositionType>& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_position.x - right) + left, p_position.y, p_position.x, p_willKeepSize*(p_position.y - top) + bottom);
		}
		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new top-right corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType> createCopyWithTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_right - right) + left, p_top, p_right, p_willKeepSize*(p_top - top) + bottom);
		}
		/// <summary>
		/// <para>Sets the top right coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType>& setTopRight(const Point<PositionType>& p_position, bool p_willKeepSize = true)
		{
			return setTopRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/// <summary>
		/// <para>Sets the top right coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setTopRight(RectangleType p_right, RectangleType p_top, bool p_willKeepSize = true)
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
		/// <summary>
		/// Returns the top right coordinates of the rectangle.
		/// </summary>
		inline Point<RectangleType> getTopRight() const
		{
			return Point<RectangleType>(right, top);
		}

		//------------------------------

		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new bottom-left corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType> createCopyWithBottomLeft(const Point<PositionType>& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_position.x, p_willKeepSize*(p_position.y - bottom) + top, (p_position.x - left) + right, p_position.y);
		}
		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new bottom-left corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType> createCopyWithBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_left, p_willKeepSize*(p_bottom - bottom) + top, (p_left - left) + right, p_bottom);
		}
		/// <summary>
		/// <para>Sets the bottom left coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType>& setBottomLeft(const Point<PositionType>& p_position, bool p_willKeepSize = true)
		{
			return setBottomLeft(p_position.x, p_position.y, p_willKeepSize);
		}
		/// <summary>
		/// <para>Sets the bottom left coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setBottomLeft(RectangleType p_left, RectangleType p_bottom, bool p_willKeepSize = true)
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
		/// <summary>
		/// Returns the bottom left coordinates of the rectangle.
		/// </summary>
		inline Point<RectangleType> getBottomLeft() const
		{
			return Point<RectangleType>(left, bottom);
		}

		//------------------------------

		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new bottom-right corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType> createCopyWithBottomRight(const Point<PositionType>& p_position, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_position.x - right) + left, p_willKeepSize*(p_position.y - bottom) + top, p_position.x, p_position.y);
		}
		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new bottom-right corner.</para>
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true) const
		{
			return Rectangle<RectangleType>(p_willKeepSize*(p_right - right) + left, p_willKeepSize*(p_bottom - bottom) + top, p_right, p_bottom);
		}
		/// <summary>
		/// <para>Sets the bottom right coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType>& setBottomRight(const Point<PositionType>& p_position, bool p_willKeepSize = true)
		{
			return setBottomRight(p_position.x, p_position.y, p_willKeepSize);
		}
		/// <summary>
		/// <para>Sets the bottom right coordinates of the rectangle.</para> 
		/// <para>If p_willKeepSize is true, the rectangle will only get moved, keeping its size.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setBottomRight(RectangleType p_right, RectangleType p_bottom, bool p_willKeepSize = true)
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
		/// <summary>
		/// Returns the bottom right coordinates of the rectangle.
		/// </summary>
		inline Point<RectangleType> getBottomRight() const
		{
			return Point<RectangleType>(right, bottom);
		}

		//------------------------------

		/// <summary>
		/// <para>Sets the left coordinate of the rectangle.</para>
		/// <para>If p_willKeepWidth is true, the right coordinate will be changed</para>
		/// <para>so that the width stays the same.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setLeft(RectangleType p_left, bool p_willKeepWidth = true)
		{
			if (p_willKeepWidth)
			{
				right += p_left - left;
			}
			left = p_left;
			return *this;
		}
		/// <summary>
		/// <para>Sets the top coordinate of the rectangle.</para>
		/// <para>If p_willKeepHeight is true, the bottom coordinate will be changed</para>
		/// <para>so that the height stays the same.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setTop(RectangleType p_top, bool p_willKeepHeight = true)
		{
			if (p_willKeepHeight)
			{
				bottom += p_top - top;
			}
			top = p_top;
			return *this;
		}
		/// <summary>
		/// <para>Sets the right coordinate of the rectangle.</para>
		/// <para>If p_willKeepWidth is true, the left coordinate will be changed</para>
		/// <para>so that the width stays the same.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setRight(RectangleType p_right, bool p_willKeepWidth = true)
		{
			if (p_willKeepWidth)
			{
				left += p_right - right;
			}
			right = p_right;
			return *this;
		}
		/// <summary>
		/// <para>Sets the bottom coordinate of the rectangle.</para>
		/// <para>If p_willKeepWidth is true, the top coordinate will be changed</para>
		/// <para>so that the height stays the same.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setBottom(RectangleType p_bottom, bool p_willKeepHeight = true)
		{
			if (p_willKeepHeight)
			{
				top += p_bottom - bottom;
			}
			bottom = p_bottom;
			return *this;
		}

		//------------------------------

		inline Rectangle<RectangleType> createCopyWithOutwardsRoundedCoordinates() const
		{
			Rectangle<RectangleType> rounded;
			rounded.left = floor(left);
			rounded.top = floor(top);
			rounded.right = ceil(right);
			rounded.bottom = ceil(bottom);
			return rounded;
		}
		inline Rectangle<RectangleType>& roundCoordinatesOutwards()
		{
			left = floor(left);
			top = floor(top);
			right = ceil(right);
			bottom = ceil(bottom);
			return *this;
		}

		//------------------------------

		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new center position.</para>
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType> createCopyWithCenter(const Point<PositionType>& p_position) const
		{
			RectangleType offsetX = (RectangleType)p_position.x - (left + right)*0.5;
			RectangleType offsetY = (RectangleType)p_position.y - (top + bottom)*0.5;
			return Rectangle<RectangleType>(offsetX + left, offsetY + top, offsetX + right, offsetY + bottom);
		}
		/// <summary>
		/// <para>Creates a copy of this rectangle, with a new center position.</para>
		/// </summary>
		inline Rectangle<RectangleType> createCopyWithBottomRight(RectangleType p_centerX, RectangleType p_centerY) const
		{
			RectangleType offsetX = p_centerX - (left + right)*0.5;
			RectangleType offsetY = p_centerY - (top + bottom)*0.5;
			return Rectangle<RectangleType>(offsetX + left, offsetY + top, offsetX + right, offsetY + bottom);
		}
		/// <summary>
		/// <para>Sets the center coordinates of the rectangle.</para> 
		/// </summary>
		template<typename PositionType>
		inline Rectangle<RectangleType>& setCenter(const Point<PositionType>& p_position)
		{
			return setCenter(p_position.x, p_position.y);
		}
		/// <summary>
		/// <para>Sets the center coordinates of the rectangle.</para> 
		/// </summary>
		inline Rectangle<RectangleType>& setCenter(RectangleType p_x, RectangleType p_y)
		{
			RectangleType halfWidth = (right - left) / 2;
			RectangleType halfHeight = (bottom - top) / 2;
			left = p_x - halfWidth;
			top = p_y - halfHeight;
			right = p_x + halfWidth;
			bottom = p_y + halfHeight;
			return *this;
		}
		/// <summary>
		/// <para>Sets the horizontal center coordinate of the rectangle.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setCenterX(RectangleType p_x)
		{
			RectangleType halfWidth = (right - left) / 2;
			left = p_x - halfWidth;
			right = p_x + halfWidth;
			return *this;
		}
		/// <summary>
		/// <para>Sets the vertical center coordinate of the rectangle.</para>
		/// </summary>
		inline Rectangle<RectangleType>& setCenterY(RectangleType p_y)
		{
			RectangleType halfHeight = (bottom - top) / 2;
			top = p_y - halfHeight;
			bottom = p_y + halfHeight;
			return *this;
		}
		/// <summary>
		/// Returns the center coordinates of the rectangle.
		/// </summary>
		inline Point<RectangleType> getCenter() const
		{
			return Point<RectangleType>((left + right) / 2, (top + bottom) / 2);
		}
		/// <summary>
		/// Returns the x-axis center coordinate of the rectangle.
		/// </summary>
		inline RectangleType getCenterX() const
		{
			return (left + right) / 2;
		}
		/// <summary>
		/// Returns the y-axis center coordinate of the rectangle.
		/// </summary>
		inline RectangleType getCenterY() const
		{
			return (top + bottom) / 2;
		}

		//------------------------------

		template<typename OffsetType>
		inline Rectangle<RectangleType>& moveTopLeft(const Point<OffsetType>& p_offset)
		{
			left += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		inline Rectangle<RectangleType>& moveTopLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		template<typename OffsetType>
		inline Rectangle<RectangleType>& moveTopRight(const Point<OffsetType>& p_offset)
		{
			right += p_offset.x;
			top += p_offset.y;
			return *this;
		}
		inline Rectangle<RectangleType>& moveTopRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			top += p_offsetY;
			return *this;
		}

		template<typename OffsetType>
		inline Rectangle<RectangleType>& moveBottomLeft(const Point<OffsetType>& p_offset)
		{
			left += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		inline Rectangle<RectangleType>& moveBottomLeft(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		template<typename OffsetType>
		inline Rectangle<RectangleType>& moveBottomRight(const Point<OffsetType>& p_offset)
		{
			right += p_offset.x;
			bottom += p_offset.y;
			return *this;
		}
		inline Rectangle<RectangleType>& moveBottomRight(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			right += p_offsetX;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// Creates a copy of this rectangle, offseted by an amount.
		/// </summary>
		template<typename OffsetType>
		inline Rectangle<RectangleType> createMovedCopy(const Point<OffsetType>& p_offset) const
		{
			return Rectangle<RectangleType>(left + p_offset.x, top + p_offset.y, right + p_offset.x, bottom + p_offset.y);
		}
		/// <summary>
		/// Creates a copy of this rectangle, offseted by an amount.
		/// </summary>
		inline Rectangle<RectangleType> createMovedCopy(RectangleType p_offsetX, RectangleType p_offsetY) const
		{
			return Rectangle<RectangleType>(left + p_offsetX, top + p_offsetY, right + p_offsetX, bottom + p_offsetY);
		}
		/// <summary>
		/// Does the same as the += operator, offsets the whole rectangle.
		/// </summary>
		template<typename OffsetType>
		inline Rectangle<RectangleType>& move(const Point<OffsetType>& p_offset)
		{
			left += p_offset.x;
			right += p_offset.x;
			top += p_offset.y;
			bottom += p_offset.y;
			return *this;
		}
		/// <summary>
		/// Does the same as the += operator, offsets the whole rectangle.
		/// </summary>
		inline Rectangle<RectangleType>& move(RectangleType p_offsetX, RectangleType p_offsetY)
		{
			left += p_offsetX;
			right += p_offsetX;
			top += p_offsetY;
			bottom += p_offsetY;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// Sets the width and height of the rectangle, changing the right and bottom properties.
		/// </summary>
		template<typename SizeType>
		inline Rectangle<RectangleType>& setSize(const Point<SizeType>& p_size)
		{
			return setSize(p_size.x, p_size.y);
		}
		/// <summary>
		/// Sets the width and height of the rectangle, changing the right and bottom variables.
		/// </summary>
		inline Rectangle<RectangleType>& setSize(RectangleType p_width, RectangleType p_height)
		{
			right = left + p_width;
			bottom = top + p_height;
			return *this;
		}
		/// <summary>
		/// Returns a point representing the size of the rectangle, where the x component is width and y is height.
		/// </summary>
		inline Point<RectangleType> getSize() const
		{
			return Point<RectangleType>(right - left, bottom - top);
		}

		/// <summary>
		/// Sets the width of the rectangle, changing the right property.
		/// </summary>
		inline Rectangle<RectangleType>& setWidth(RectangleType p_width)
		{
			right = left + p_width;
			return *this;
		}
		/// <summary>
		/// Returns the width of the rectangle.
		/// </summary>
		inline RectangleType getWidth() const
		{
			return right - left;
		}

		/// <summary>
		/// Sets the height of the rectangle, changing the bottom property.
		/// </summary>
		inline Rectangle<RectangleType>& setHeight(RectangleType p_height)
		{
			bottom = top + p_height;
			return *this;
		}
		/// <summary>
		/// Returns the height of the rectangle.
		/// </summary>
		inline RectangleType getHeight() const
		{
			return bottom - top;
		}

		//------------------------------

		/// <summary>
		/// Returns a new copy of this rectangle which is clipped to fit into the parameter rectangle.
		/// </summary>
		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType> createBoundedCopy(const Rectangle<ParameterRectangleType>& p_bounds) const
		{
			Rectangle<RectangleType> bounded;
			bounded.left = max(left, min(right, p_bounds.left));
			bounded.top = max(top, min(bottom, p_bounds.top));
			bounded.right = min(right, max(left, p_bounds.right));
			bounded.bottom = min(bottom, max(top, p_bounds.bottom));
			return bounded;
		}
		/// <summary>
		/// Returns a new copy of this rectangle which is clipped to fit into the parameter rectangle.
		/// </summary>
		inline Rectangle<RectangleType> createBoundedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			Rectangle<RectangleType> bounded;
			bounded.left = max(left, min(right, p_left));
			bounded.top = max(top, min(bottom, p_top));
			bounded.right = min(right, max(left, p_right));
			bounded.bottom = min(bottom, max(top, p_bottom));
			return bounded;
		}

		/// <summary>
		/// Clips this rectangle to fit into the parameter rectangle.
		/// </summary>
		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType>& bound(const Rectangle<ParameterRectangleType>& p_bounds)
		{
			if (left < p_bounds.left) left = p_bounds.left;
			if (top < p_bounds.top) top = p_bounds.top;
			if (right > p_bounds.right) right = p_bounds.right;
			if (bottom > p_bounds.bottom) bottom = p_bounds.bottom;
			return *this;
		}
		/// <summary>
		/// Clips this rectangle to fit into the parameter rectangle.
		/// </summary>
		inline Rectangle<RectangleType>& bound(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			if (left < p_left) left = p_left;
			if (top < p_top) top = p_top;
			if (right > p_right) right = p_right;
			if (bottom > p_bottom) bottom = p_bottom;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// Returns a new copy of this rectangle which is extended so that it contains another rectangle.
		/// </summary>
		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType> createContainedCopy(const Rectangle<ParameterRectangleType>& p_rectangle) const
		{
			Rectangle<RectangleType> contained;
			contained.left = min(left, p_rectangle.left);
			contained.top = min(top, p_rectangle.top);
			contained.right = max(right, p_rectangle.right);
			contained.bottom = max(bottom, p_rectangle.bottom);
			return contained;
		}
		/// <summary>
		/// Returns a new copy of this rectangle which is extended so that it contains another rectangle.
		/// </summary>
		inline Rectangle<RectangleType> createContainedCopy(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom) const
		{
			Rectangle<RectangleType> contained;
			contained.left = min(left, p_left);
			contained.top = min(top, p_top);
			contained.right = max(right, p_right);
			contained.bottom = max(bottom, p_bottom);
			return contained;
		}

		/// <summary>
		/// Extends the rectangle so that it contains another rectangle.
		/// </summary>
		template<typename ParameterRectangleType>
		inline Rectangle<RectangleType>& contain(const Rectangle<ParameterRectangleType>& p_rectangle)
		{
			if (p_rectangle.left < left) left = p_rectangle.left;
			if (p_rectangle.top < top) top = p_rectangle.top;
			if (p_rectangle.right > right) right = p_rectangle.right;
			if (p_rectangle.bottom > bottom) bottom = p_rectangle.bottom;
			return *this;
		}

		/// <summary>
		/// Extends the rectangle so that it contains another rectangle.
		/// </summary>
		inline Rectangle<RectangleType>& contain(RectangleType p_left, RectangleType p_top, RectangleType p_right, RectangleType p_bottom)
		{
			if (p_left < left) left = p_left;
			if (p_top < top) top = p_top;
			if (p_right > right) right = p_right;
			if (p_bottom > bottom) bottom = p_bottom;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// <para>Returns if a point lies within this rectangle.</para>
		/// </summary>
		template<typename PointType>
		inline bool getIsContaining(const Point<PointType>& p_point) const
		{
			return p_point.x >= left && p_point.x <= right
				&& p_point.y >= top && p_point.y <= bottom;
		}
		/// <summary>
		/// <para>Returns if a point lies within this rectangle.</para>
		/// </summary>
		inline bool getIsContaining(RectangleType p_x, RectangleType p_y) const
		{
			return p_x >= left && p_x <= right
				&& p_y >= top && p_y <= bottom;
		}

		/// <summary>
		/// <para>Returns if another rectangle is fully inside this rectangle.</para>
		/// </summary>
		template<typename ParameterRectangleType>
		inline bool getIsContaining(const Rectangle<ParameterRectangleType>& p_rectangle) const
		{
			return p_rectangle.left >= left && p_rectangle.right <= right
				&& p_rectangle.top >= top && p_rectangle.bottom <= bottom;
		}

		/// <summary>
		/// <para>Returns if this rectangle intersects/overlaps/touches another rectangle.</para>
		/// </summary>
		template<typename ParameterRectangleType>
		inline bool getIsIntersecting(const Rectangle<ParameterRectangleType>& p_rectangle) const
		{
			return p_rectangle.right >= left && p_rectangle.bottom >= top
				&& p_rectangle.left <= right && p_rectangle.top <= bottom;
		}
	};

	// would be useless and too much job to make templated... floats are pretty nice anyways

	/// <summary>
	/// An abstract class for objects that have 2d bounds which can be changed by the user.
	/// </summary>
	class ProtectedRectangle
	{
	protected:
		Rectangle<float> m_bounds;

	public:
		ProtectedRectangle() { }
		ProtectedRectangle(const Rectangle<float>& p_bounds) : m_bounds(p_bounds) { }
		ProtectedRectangle(Rectangle<float>&& p_bounds) : m_bounds(p_bounds) { }

		virtual void setBounds(const Rectangle<float>& p_rectangle) = 0;
		virtual void setBounds(float p_left, float p_top, float p_right, float p_bottom) = 0;
		virtual void setBounds(const Point<float>& p_position, const Point<float>& p_size) = 0;
		virtual const Rectangle<float>& getBounds() const = 0;

		//------------------------------

		virtual void move(const Point<float>& p_offset) = 0;
		virtual void move(float p_offsetX, float p_offsetY) = 0;

		//------------------------------

		virtual void setTopLeft(const Point<float>& p_position, bool p_willKeepSize = true) = 0;
		virtual void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) = 0;
		virtual Point<float> getTopLeft() const = 0;

		virtual void setTopRight(const Point<float>& p_position, bool p_willKeepSize = true) = 0;
		virtual void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) = 0;
		virtual Point<float> getTopRight() const = 0;

		virtual void setBottomLeft(const Point<float>& p_position, bool p_willKeepSize = true) = 0;
		virtual void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) = 0;
		virtual Point<float> getBottomLeft() const = 0;

		virtual void setBottomRight(const Point<float>& p_position, bool p_willKeepSize = true) = 0;
		virtual void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) = 0;
		virtual Point<float> getBottomRight() const = 0;

		//------------------------------

		virtual void setCenter(const Point<float>& p_position) = 0;
		virtual void setCenter(float p_x, float p_y) = 0;
		virtual void setCenterX(float p_x) = 0;
		virtual void setCenterY(float p_y) = 0;
		virtual Point<float> getCenter() const = 0;
		virtual float getCenterX() const = 0;
		virtual float getCenterY() const = 0;

		//------------------------------

		virtual void setLeft(float p_left, bool p_willKeepWidth = false) = 0;
		virtual float getLeft() const = 0;

		virtual void setTop(float p_top, bool p_willKeepHeight = false) = 0;
		virtual float getTop() const = 0;

		virtual void setRight(float p_right, bool p_willKeepWidth = false) = 0;
		virtual float getRight() const = 0;

		virtual void setBottom(float p_bottom, bool p_willKeepHeight = false) = 0;
		virtual float getBottom() const = 0;

		//------------------------------

		virtual void setWidth(float p_width) = 0;
		virtual float getWidth() const = 0;

		virtual void setHeight(float p_height) = 0;
		virtual float getHeight() const = 0;

		virtual void setSize(const Point<float>& p_size) = 0;
		virtual void setSize(float p_width, float p_height) = 0;
		virtual Point<float> getSize() const = 0;

		//------------------------------

		virtual bool getIsIntersecting(const Rectangle<float>& p_rectangle) const = 0;
		virtual bool getIsIntersecting(ProtectedRectangle* p_view) const = 0;

		virtual bool getIsContaining(const Rectangle<float>& p_rectangle) const = 0;
		virtual bool getIsContaining(ProtectedRectangle* p_rectangle) const = 0;
		virtual bool getIsContaining(float p_x, float p_y) const = 0;
		virtual bool getIsContaining(const Point<float>& p_point) const = 0;
	};

	//------------------------------

	/// <summary>
	/// <para>Cubic bezier animation easing.</para>
	/// <para>Try out this, can be useful for making your easing curves:</para>
	/// <para>http://bjornsundin.com/projects/cubic-bezier-easing</para>
	/// <para>-</para>
	/// <para>Ease in example:</para>
	/// <para>Easing(0.7, 0, 1, 1).easeValue(x)</para>
	/// <para>Ease out example:</para>
	/// <para>Easing(0, 0, 0.3, 1).easeValue(x)</para>
	/// <para>See the constructors and easeValue() for more info.</para>
	/// </summary>
	struct Easing
	{
		float x0, y0, x1, y1;

		inline Easing() : x0(0.f), y0(0.f), x1(1.f), y1(1.f) { }
		inline Easing(const Easing& p_easing)
		{
			*this = p_easing;
		}
		inline Easing(Easing&& p_easing)
		{
			*this = p_easing;
		}
		/// <summary>
		/// Initializes the control points of the bezier curve easing.
		/// </summary>
		/// <param name="p_x0">X value of the first control point</param>
		/// <param name="p_y0">Y value of the first control point</param>
		/// <param name="p_x1">X value of the second control point</param>
		/// <param name="p_y1">Y value of the second control point</param>
		Easing(float p_x0, float p_y0, float p_x1, float p_y1) :
			x0(p_x0), y0(p_y0), x1(p_x1), y1(p_y1)
		{ }
		/// <summary>
		/// Initializes the control points of the bezier curve easing.
		/// </summary>
		/// <param name="p_x0">X value of the first control point</param>
		/// <param name="p_y0">Y value of the first control point</param>
		/// <param name="p_x1">X value of the second control point</param>
		/// <param name="p_y1">Y value of the second control point</param>
		Easing(double p_x0, double p_y0, double p_x1, double p_y1) :
			x0((float)p_x0), y0((float)p_y0), x1((float)p_x1), y1((float)p_y1)
		{ }

		inline Easing& operator=(const Easing& p_easing)
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}
		inline Easing& operator=(Easing&& p_easing)
		{
			x0 = p_easing.x0;
			y0 = p_easing.y0;
			x1 = p_easing.x1;
			y1 = p_easing.y1;
			return *this;
		}

		/// <summary>
		/// <para>Transforms a normalized value according to a cubic bezier curve.</para>
		/// </summary>
		/// <param name="p_value">A normalized value which will be eased</param>
		/// <param name="p_precision">Max amount of error in the output.</param>
		/// <returns></returns>
		float easeValue(float p_value, float p_precision = 0.005f) const;
	};

	//------------------------------

	/// <summary>
	/// <para>This is very useful when using a pointer in multiple places.</para>
	/// <para>It doesn't get deleted until every remember() has a forget().</para>
	/// <para>The constructor is the first remember(), meaning m_referenceCount</para>
	/// <para>is initialized with 1. Don't use the delete operator with objects</para>
	/// <para>that are ReferenceCounted, use forget() instead.</para>
	/// <para>-</para>
	/// <para>I just like this a lot more than using std::shared_ptr etc.</para>
	/// </summary>
	class ReferenceCounted
	{
	private:
		int32_t m_referenceCount;

	public:
		inline ReferenceCounted() : m_referenceCount(1) { }
		virtual ~ReferenceCounted() { };

		/// <summary>
		/// Increments the reference count.
		/// </summary>
		inline void remember()
		{
			m_referenceCount++;
		}

		/// <summary>
		/// <para>Decrements the reference count and deletes the</para>
		/// <para>object if the reference count has reached 0.</para>
		/// </summary>
		inline void forget()
		{
			m_referenceCount--;
			if (!m_referenceCount)
			{
				delete this;
			}
		}
	};

	//------------------------------
	// Color stuff

#pragma region Color stuff
	/// <summary>
	/// ARGB formatted 32-bit color
	/// </summary>
	typedef uint32_t colorInt;

	inline unsigned char getRed(colorInt p_color)
	{
		return (p_color >> 16) & 0xff;
	}
	inline unsigned char getGreen(colorInt p_color)
	{
		return (p_color >> 8) & 0xff;
	}
	inline unsigned char getBlue(colorInt p_color)
	{
		return p_color & 0xff;
	}
	inline unsigned char getAlpha(colorInt p_color)
	{
		return (p_color >> 24) & 0xff;
	}

	/// <summary>
	/// <para>Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].</para>
	/// <para>This means that a Color object is 4 times as big as a packed 32-bit color.</para>
	/// </summary>
	class Color
	{
	public:
		float red;
		float green;
		float blue;
		float alpha;

		//------------------------------

		Color() :
			red(0.f), green(0.f), blue(0.f), alpha(0.f)
		{ }
		/// <summary>
		/// The channels are floats in the range [0, 1].
		/// </summary>
		Color(float p_red, float p_green, float p_blue, float p_alpha = 1.f) :
			red(p_red), green(p_green), blue(p_blue), alpha(p_alpha)
		{ }
		Color(unsigned char p_red, unsigned char p_green, unsigned char p_blue, unsigned char p_alpha = 255) :
			red(float(p_red) / 255.f), green(float(p_green) / 255.f), blue(float(p_blue) / 255.f), alpha(float(p_alpha) / 255.f)
		{ }
		/// <summary>
		/// Initializes the color with a grayscale value. The values are floats in the range [0, 1].
		/// </summary>
		Color(float p_lightness, float p_alpha = 1.f)
		{
			red = constrain(p_lightness);
			green = red;
			blue = red;
			alpha = constrain(p_alpha);
		}
		Color(unsigned char p_lightness, unsigned char p_alpha = 255)
		{
			red = float(p_lightness) / 255.f;
			green = red;
			blue = red;
			alpha = float(p_alpha) / 255.f;
		}
		Color(const Color& p_color, float p_alpha)
		{
			red = p_color.red;
			green = p_color.green;
			blue = p_color.blue;
			alpha = p_alpha;
		}
		Color(const colorInt& p_color)
		{
			operator=(p_color);
		}
		Color(const Color& p_color)
		{
			operator=(p_color);
		}
		Color(Color&& p_color)
		{
			operator=(p_color);
		}

		Color& operator=(const colorInt& p_color)
		{
			alpha = float(p_color >> 24) / 255.f;
			red = float(p_color >> 16 & 0xff) / 255.f;
			green = float((p_color >> 8 & 0xff)) / 255.f;
			blue = float(p_color & 0xff) / 255.f;
			return *this;
		}
		Color& operator=(const Color& p_color)
		{
			red = p_color.red;
			green = p_color.green;
			blue = p_color.blue;
			alpha = p_color.alpha;
			return *this;
		}
		Color& operator=(Color&& p_color)
		{
			red = p_color.red;
			green = p_color.green;
			blue = p_color.blue;
			alpha = p_color.alpha;
			return *this;
		}

		//------------------------------

		/// <summary>
		/// Sets the values for the red, green, blue and alpha channels. They are all floats in the range of [0, 1].
		/// </summary>
		inline void setRGBA(float p_red, float p_green, float p_blue, float p_alpha = 1.f)
		{
			red = constrain(p_red);
			green = constrain(p_green);
			blue = constrain(p_blue);
			alpha = constrain(p_alpha);
		}
		/// <summary>
		/// Sets the values for the red, green, blue and alpha channels. The parameters are bytes, in the range of [0, 255].
		/// </summary>
		inline void setRGBA(unsigned char p_red, unsigned char p_green, unsigned char p_blue, unsigned char p_alpha = 255)
		{
			red = float(p_red) / 255.f;
			green = float(p_green) / 255.f;
			blue = float(p_blue) / 255.f;
			alpha = float(p_alpha) / 255.f;
		}

		//------------------------------

		/// <summary>
		/// Sets the hue, saturation and brightness values of the color. They are all floats in the range [0, 1].
		/// </summary>
		inline Color& setHSB(float p_hue, float p_saturation, float p_brightness)
		{
			p_hue -= floor(p_hue);
			p_brightness = constrain(p_brightness);
			float factor = p_brightness * constrain(p_saturation);
			red = p_brightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 1.f);
			green = p_brightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 1.f);
			blue = p_brightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 1.f);
			return *this;
		}
		/// <summary>
		/// Sets the hue, saturation, brightness and alpha values of the color. They are all floats in the range [0, 1].
		/// </summary>
		inline Color& setHSBA(float p_hue, float p_saturation, float p_brightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSB(p_hue, p_saturation, p_brightness);
		}
		/// <summary>
		/// Sets the hue, saturation and lightness values of the color. They are all floats in the range [0, 1]
		/// </summary>
		inline Color& setHSL(float p_hue, float p_saturation, float p_lightness)
		{
			p_hue -= floor(p_hue);
			p_lightness = constrain(p_lightness);
			float factor = 2.f*constrain(p_saturation)*(p_lightness < 0.5f ? p_lightness : (1.f - p_lightness));
			red = p_lightness + factor * (constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f) - 0.5);
			green = p_lightness + factor * (min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f) - 0.5f);
			blue = p_lightness + factor * (constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f) - 0.5f);
			return *this;
		}
		/// <summary>
		/// Sets the hue, saturation, lightness and alpha values of the color. They are all floats in the range [0, 1]
		/// </summary>
		inline Color& setHSLA(float p_hue, float p_saturation, float p_lightness, float p_alpha = 1.f)
		{
			alpha = p_alpha;
			return setHSL(p_hue, p_saturation, p_lightness);
		}

		/// <summary>
		/// Changes the hue of the color. The hue is a float in the range [0, 1].
		/// </summary>
		inline Color& setHue(float p_hue)
		{
			p_hue -= floor(p_hue);
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			red = minColor + (maxColor - minColor)*(constrain(1.f - (p_hue - 1.f / 6.f)*6.f) + constrain((p_hue - 4.f / 6.f)*6.f));
			green = minColor + (maxColor - minColor)*(min(1.f, p_hue*6.f) - constrain((p_hue - 3.f / 6.f)*6.f));
			blue = minColor + (maxColor - minColor)*(constrain((p_hue - 2.f / 6.f)*6.f) - constrain((p_hue - 5.f / 6.f)*6.f));
			return *this;
		}
		/// <summary>
		/// Returns the hue of the color. The hue is a float in the range [0, 1].
		/// </summary>
		inline float getHue() const
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

		/// <summary>
		/// <para>Sets the HSB saturation of the color. The saturation is a float in the range [0, 1].</para>
		/// <para>HSB saturation can change lightness, and HSL saturation can change brightness.</para>
		/// <para>Keep in mind that you can't change the saturation if the color is black, because only RGBA values are stored.</para>
		/// </summary>
		inline Color& setSaturationHSB(float p_saturation)
		{
			float brightness = max(red, green, blue);

			float saturation = getSaturationHSB();
			if (saturation == 0.f)
			{
				return *this;
			}

			p_saturation = constrain(p_saturation);
			float factor = p_saturation / saturation;

			red = brightness + factor * (red - brightness);
			green = brightness + factor * (green - brightness);
			blue = brightness + factor * (blue - brightness);

			return *this;
		}
		/// <summary>
		/// Returns the HSB saturation of the color. The saturation is a float in the range [0, 1].
		/// </summary>
		inline float getSaturationHSB() const
		{
			float brightness = getBrightness();
			if (brightness)
			{
				return 1.f - min(red, green, blue) / brightness;
			}
			return 0.f;
		}

		/// <summary>
		/// <para>Sets the HSL saturation of the color. The saturation is a float in the range [0, 1].</para>
		/// <para>HSB saturation can change lightness, and HSL saturation can change brightness.</para>
		/// <para>Keep in mind that you can't change the saturation if the color is black, since only RGBA values are stored.</para>
		/// </summary>
		inline Color& setSaturationHSL(float p_saturation)
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
		/// <summary>
		/// Returns the HSL saturation of the color. The saturation is a float in the range [0, 1].
		/// </summary>
		inline float getSaturationHSL() const
		{
			float minColor = min(red, green, blue);
			float maxColor = max(red, green, blue);
			if (minColor == maxColor)
			{
				return 0.f;
			}
			return max((maxColor - minColor) / (maxColor + minColor), (maxColor - minColor) / (2.f - maxColor - minColor));
		}

		/// <summary>
		/// Sets the brightness of the color. The brightness is a float in the range [0, 1]. A brightness of 0 makes the
		/// color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
		/// </summary>
		inline Color& setBrightness(float p_brightness)
		{
			p_brightness = constrain(p_brightness);

			float brightness = max(red, green, blue);
			if (brightness == 0.f)
			{
				red = p_brightness;
				green = p_brightness;
				blue = p_brightness;
				return *this;
			}

			red *= p_brightness / brightness;
			green *= p_brightness / brightness;
			blue *= p_brightness / brightness;

			return *this;
		}
		/// <summary>
		/// Returns the brightness of the color. The brightness is a float in the range [0, 1].
		/// </summary>
		inline float getBrightness() const
		{
			return max(red, green, blue);
		}

		/// <summary>
		/// Changes the lightness of the color. The lightness a float in the range [0, 1]. A lightness of 0 makes the
		/// color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
		/// </summary>
		inline Color& setLightness(float p_lightness)
		{
			p_lightness = constrain(p_lightness);

			float lightness = getLightness();

			if (lightness == 0.f || lightness == 1.f)
			{
				red = p_lightness;
				green = p_lightness;
				blue = p_lightness;
				return *this;
			}

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
		/// <summary>
		/// Returns the lightness of the color. The lightness is a float in the range [0, 1].
		/// </summary>
		inline float getLightness() const
		{
			return 0.5f*(min(red, green, blue) + max(red, green, blue));
		}

		//------------------------------

		/// <summary>
		/// Packs the color into a 32-bit integer. ARGB format.
		/// </summary>
		colorInt getPacked() const
		{
			return (colorInt(alpha * 0xff) << 24) | (colorInt(red * 0xff) << 16) | (colorInt(green * 0xff) << 8) | (colorInt(blue * 0xff));
		}
	};

	//
	// Material design 2014 colors
	//

	colorInt const
		COLOR_RED_50 = 0xFFEBEE,
		COLOR_RED_100 = 0xFFCDD2,
		COLOR_RED_200 = 0xEF9A9A,
		COLOR_RED_300 = 0xE57373,
		COLOR_RED_400 = 0xEF5350,
		COLOR_RED_500 = 0xF44336,
		COLOR_RED_600 = 0xE53935,
		COLOR_RED_700 = 0xD32F2F,
		COLOR_RED_800 = 0xC62828,
		COLOR_RED_900 = 0xB71C1C,
		COLOR_RED_A100 = 0xFF8A80,
		COLOR_RED_A200 = 0xFF5252,
		COLOR_RED_A400 = 0xFF1744,
		COLOR_RED_A700 = 0xD50000;

	colorInt const
		COLOR_PINK_50 = 0xFCE4EC,
		COLOR_PINK_100 = 0xF8BBD0,
		COLOR_PINK_200 = 0xF48FB1,
		COLOR_PINK_300 = 0xF06292,
		COLOR_PINK_400 = 0xEC407A,
		COLOR_PINK_500 = 0xE91E63,
		COLOR_PINK_600 = 0xD81B60,
		COLOR_PINK_700 = 0xC2185B,
		COLOR_PINK_800 = 0xAD1457,
		COLOR_PINK_900 = 0x880E4F,
		COLOR_PINK_A100 = 0xFF80AB,
		COLOR_PINK_A200 = 0xFF4081,
		COLOR_PINK_A400 = 0xF50057,
		COLOR_PINK_A700 = 0xC51162;

	colorInt const
		COLOR_PURPLE_50 = 0xF3E5F5,
		COLOR_PURPLE_100 = 0xE1BEE7,
		COLOR_PURPLE_200 = 0xCE93D8,
		COLOR_PURPLE_300 = 0xBA68C8,
		COLOR_PURPLE_400 = 0xAB47BC,
		COLOR_PURPLE_500 = 0x9C27B0,
		COLOR_PURPLE_600 = 0x8E24AA,
		COLOR_PURPLE_700 = 0x7B1FA2,
		COLOR_PURPLE_800 = 0x6A1B9A,
		COLOR_PURPLE_900 = 0x4A148C,
		COLOR_PURPLE_A100 = 0xEA80FC,
		COLOR_PURPLE_A200 = 0xE040FB,
		COLOR_PURPLE_A400 = 0xD500F9,
		COLOR_PURPLE_A700 = 0xAA00FF;

	colorInt const
		COLOR_DEEP_PURPLE_50 = 0xEDE7F6,
		COLOR_DEEP_PURPLE_100 = 0xD1C4E9,
		COLOR_DEEP_PURPLE_200 = 0xB39DDB,
		COLOR_DEEP_PURPLE_300 = 0x9579CD,
		COLOR_DEEP_PURPLE_400 = 0x7E57C2,
		COLOR_DEEP_PURPLE_500 = 0x673AB7,
		COLOR_DEEP_PURPLE_600 = 0x5E35B1,
		COLOR_DEEP_PURPLE_700 = 0x512DA8,
		COLOR_DEEP_PURPLE_800 = 0x4527A0,
		COLOR_DEEP_PURPLE_900 = 0x311B92,
		COLOR_DEEP_PURPLE_A100 = 0xB388FF,
		COLOR_DEEP_PURPLE_A200 = 0x7C4DFF,
		COLOR_DEEP_PURPLE_A400 = 0x651FFF,
		COLOR_DEEP_PURPLE_A700 = 0x6200EA;

	colorInt const
		COLOR_INDIGO_50 = 0xE8EAF6,
		COLOR_INDIGO_100 = 0xC5CAE9,
		COLOR_INDIGO_200 = 0x9FA8DA,
		COLOR_INDIGO_300 = 0x7986CB,
		COLOR_INDIGO_400 = 0x5C6BC0,
		COLOR_INDIGO_500 = 0x3F51B5,
		COLOR_INDIGO_600 = 0x3949AB,
		COLOR_INDIGO_700 = 0x303F9F,
		COLOR_INDIGO_800 = 0x283593,
		COLOR_INDIGO_900 = 0x1A237E,
		COLOR_INDIGO_A100 = 0x8C9EFF,
		COLOR_INDIGO_A200 = 0x536DFE,
		COLOR_INDIGO_A400 = 0x3D5AFE,
		COLOR_INDIGO_A700 = 0x304FFE;

	colorInt const
		COLOR_BLUE_50 = 0xE3F2FD,
		COLOR_BLUE_100 = 0xBBDEFB,
		COLOR_BLUE_200 = 0x90CAF9,
		COLOR_BLUE_300 = 0x64B5F6,
		COLOR_BLUE_400 = 0x42A5F5,
		COLOR_BLUE_500 = 0x2196F3,
		COLOR_BLUE_600 = 0x1E88E5,
		COLOR_BLUE_700 = 0x1976D2,
		COLOR_BLUE_800 = 0x1565C0,
		COLOR_BLUE_900 = 0x0D47A1,
		COLOR_BLUE_A100 = 0x82B1FF,
		COLOR_BLUE_A200 = 0x448AFF,
		COLOR_BLUE_A400 = 0x2979FF,
		COLOR_BLUE_A700 = 0x2962FF;

	colorInt const
		COLOR_LIGHT_BLUE_50 = 0xE1F5FE,
		COLOR_LIGHT_BLUE_100 = 0xB3E5FC,
		COLOR_LIGHT_BLUE_200 = 0x81D4FA,
		COLOR_LIGHT_BLUE_300 = 0x4FC3F7,
		COLOR_LIGHT_BLUE_400 = 0x29B6F6,
		COLOR_LIGHT_BLUE_500 = 0x03A9F4,
		COLOR_LIGHT_BLUE_600 = 0x039BE5,
		COLOR_LIGHT_BLUE_700 = 0x0288D1,
		COLOR_LIGHT_BLUE_800 = 0x0277BD,
		COLOR_LIGHT_BLUE_900 = 0x01579B,
		COLOR_LIGHT_BLUE_A100 = 0x80D8FF,
		COLOR_LIGHT_BLUE_A200 = 0x40C4FF,
		COLOR_LIGHT_BLUE_A400 = 0x00B0FF,
		COLOR_LIGHT_BLUE_A700 = 0x0091EA;

	colorInt const
		COLOR_CYAN_50 = 0xE0F7FA,
		COLOR_CYAN_100 = 0xB2EBF2,
		COLOR_CYAN_200 = 0x80DEEA,
		COLOR_CYAN_300 = 0x4DD0E1,
		COLOR_CYAN_400 = 0x26C6DA,
		COLOR_CYAN_500 = 0x00BCD4,
		COLOR_CYAN_600 = 0x00ACC1,
		COLOR_CYAN_700 = 0x0097A7,
		COLOR_CYAN_800 = 0x00838F,
		COLOR_CYAN_900 = 0x006064,
		COLOR_CYAN_A100 = 0x84FFFF,
		COLOR_CYAN_A200 = 0x18FFFF,
		COLOR_CYAN_A400 = 0x00E5FF,
		COLOR_CYAN_A700 = 0x00B8D4;

	colorInt const
		COLOR_TEAL_50 = 0xE0F2F1,
		COLOR_TEAL_100 = 0xB2DFDB,
		COLOR_TEAL_200 = 0x80CBC4,
		COLOR_TEAL_300 = 0x4DB6AC,
		COLOR_TEAL_400 = 0x26A69A,
		COLOR_TEAL_500 = 0x009688,
		COLOR_TEAL_600 = 0x00897B,
		COLOR_TEAL_700 = 0x00796B,
		COLOR_TEAL_800 = 0x00695C,
		COLOR_TEAL_900 = 0x004D40,
		COLOR_TEAL_A100 = 0xA7FFEB,
		COLOR_TEAL_A200 = 0x64FFDA,
		COLOR_TEAL_A400 = 0x1DE9B6,
		COLOR_TEAL_A700 = 0x00BFA5;

	colorInt const
		COLOR_GREEN_50 = 0xE8F5E9,
		COLOR_GREEN_100 = 0xC8E6C9,
		COLOR_GREEN_200 = 0xA5D6A7,
		COLOR_GREEN_300 = 0x81C784,
		COLOR_GREEN_400 = 0x66BB6A,
		COLOR_GREEN_500 = 0x4CAF50,
		COLOR_GREEN_600 = 0x43A047,
		COLOR_GREEN_700 = 0xE88E3C,
		COLOR_GREEN_800 = 0x2E7D32,
		COLOR_GREEN_900 = 0x1B5E20,
		COLOR_GREEN_A100 = 0xB9F6CA,
		COLOR_GREEN_A200 = 0x69F0AE,
		COLOR_GREEN_A400 = 0x00E676,
		COLOR_GREEN_A700 = 0x00C853;

	colorInt const
		COLOR_LIGHT_GREEN_50 = 0xF1F8E9,
		COLOR_LIGHT_GREEN_100 = 0xDCEDC8,
		COLOR_LIGHT_GREEN_200 = 0xC5E1A5,
		COLOR_LIGHT_GREEN_300 = 0xAED581,
		COLOR_LIGHT_GREEN_400 = 0x9CCC65,
		COLOR_LIGHT_GREEN_500 = 0x8BC34A,
		COLOR_LIGHT_GREEN_600 = 0x7CB342,
		COLOR_LIGHT_GREEN_700 = 0x689F38,
		COLOR_LIGHT_GREEN_800 = 0x558B2F,
		COLOR_LIGHT_GREEN_900 = 0x33691E,
		COLOR_LIGHT_GREEN_A100 = 0xCCFF90,
		COLOR_LIGHT_GREEN_A200 = 0xB2FF59,
		COLOR_LIGHT_GREEN_A400 = 0x76FF03,
		COLOR_LIGHT_GREEN_A700 = 0x64DD17;

	colorInt const
		COLOR_LIME_50 = 0xF9FBE7,
		COLOR_LIME_100 = 0xF0F4C3,
		COLOR_LIME_200 = 0xE6EE9C,
		COLOR_LIME_300 = 0xDCE775,
		COLOR_LIME_400 = 0xD4E157,
		COLOR_LIME_500 = 0xCDDC39,
		COLOR_LIME_600 = 0xC0CA33,
		COLOR_LIME_700 = 0xAFB42B,
		COLOR_LIME_800 = 0x9E9D24,
		COLOR_LIME_900 = 0x827717,
		COLOR_LIME_A100 = 0xF4FF81,
		COLOR_LIME_A200 = 0xEEFF41,
		COLOR_LIME_A400 = 0xC6FF00,
		COLOR_LIME_A700 = 0xAEEA00;

	colorInt const
		COLOR_YELLOW_50 = 0xFFFDE7,
		COLOR_YELLOW_100 = 0xFFF9C4,
		COLOR_YELLOW_200 = 0xFFF59D,
		COLOR_YELLOW_300 = 0xFFF176,
		COLOR_YELLOW_400 = 0xFFEE58,
		COLOR_YELLOW_500 = 0xFFEB3B,
		COLOR_YELLOW_600 = 0xFDD835,
		COLOR_YELLOW_700 = 0xFBC02D,
		COLOR_YELLOW_800 = 0xF9A825,
		COLOR_YELLOW_900 = 0xF57F17,
		COLOR_YELLOW_A100 = 0xFFFF8D,
		COLOR_YELLOW_A200 = 0xFFFF00,
		COLOR_YELLOW_A400 = 0xFFEA00,
		COLOR_YELLOW_A700 = 0xFFD600;

	colorInt const
		COLOR_AMBER_50 = 0xFFF8E1,
		COLOR_AMBER_100 = 0xFFECB3,
		COLOR_AMBER_200 = 0xFFE082,
		COLOR_AMBER_300 = 0xFFD54F,
		COLOR_AMBER_400 = 0xFFCA28,
		COLOR_AMBER_500 = 0xFFC107,
		COLOR_AMBER_600 = 0xFFB300,
		COLOR_AMBER_700 = 0xFFA000,
		COLOR_AMBER_800 = 0xFF8F00,
		COLOR_AMBER_900 = 0xFF7F00,
		COLOR_AMBER_A100 = 0xFFE57F,
		COLOR_AMBER_A200 = 0xFFD740,
		COLOR_AMBER_A400 = 0xFFC400,
		COLOR_AMBER_A700 = 0xFFAB00;

	colorInt const
		COLOR_ORANGE_50 = 0xFFF3E0,
		COLOR_ORANGE_100 = 0xFFE0B2,
		COLOR_ORANGE_200 = 0xFFCC80,
		COLOR_ORANGE_300 = 0xFFB74D,
		COLOR_ORANGE_400 = 0xFFA726,
		COLOR_ORANGE_500 = 0xFF9800,
		COLOR_ORANGE_600 = 0xFB8C00,
		COLOR_ORANGE_700 = 0xF57C00,
		COLOR_ORANGE_800 = 0xEF6C00,
		COLOR_ORANGE_900 = 0xE65100,
		COLOR_ORANGE_A100 = 0xFFD180,
		COLOR_ORANGE_A200 = 0xFFAB40,
		COLOR_ORANGE_A400 = 0xFF9100,
		COLOR_ORANGE_A700 = 0xFF6D00;

	colorInt const
		COLOR_DEEP_ORANGE_50 = 0xFBE9E7,
		COLOR_DEEP_ORANGE_100 = 0xFFCCBC,
		COLOR_DEEP_ORANGE_200 = 0xFFAB91,
		COLOR_DEEP_ORANGE_300 = 0xFF8A65,
		COLOR_DEEP_ORANGE_400 = 0xFF7043,
		COLOR_DEEP_ORANGE_500 = 0xFF5722,
		COLOR_DEEP_ORANGE_600 = 0xF4511E,
		COLOR_DEEP_ORANGE_700 = 0xE64A19,
		COLOR_DEEP_ORANGE_800 = 0xD84315,
		COLOR_DEEP_ORANGE_900 = 0xBF360C,
		COLOR_DEEP_ORANGE_A100 = 0xFF9E80,
		COLOR_DEEP_ORANGE_A200 = 0xFF6E40,
		COLOR_DEEP_ORANGE_A400 = 0xFF3D00,
		COLOR_DEEP_ORANGE_A700 = 0xDD2C00;

	colorInt const
		COLOR_BROWN_50 = 0xEFEBE9,
		COLOR_BROWN_100 = 0xD7CCC8,
		COLOR_BROWN_200 = 0xBCAAA4,
		COLOR_BROWN_300 = 0xA1887F,
		COLOR_BROWN_400 = 0x8D6E63,
		COLOR_BROWN_500 = 0x795548,
		COLOR_BROWN_600 = 0x6D4C41,
		COLOR_BROWN_700 = 0x5D4037,
		COLOR_BROWN_800 = 0x4E342E,
		COLOR_BROWN_900 = 0x3E2723;

	colorInt const
		COLOR_GRAY_50 = 0xFAFAFA,
		COLOR_GRAY_100 = 0xF5F5F5,
		COLOR_GRAY_200 = 0xEEEEEE,
		COLOR_GRAY_300 = 0xE0E0E0,
		COLOR_GRAY_400 = 0xBDBDBD,
		COLOR_GRAY_500 = 0x9E9E9E,
		COLOR_GRAY_600 = 0x757575,
		COLOR_GRAY_700 = 0x616161,
		COLOR_GRAY_800 = 0x424242,
		COLOR_GRAY_900 = 0x212121;

	colorInt const
		COLOR_BLUE_GRAY_50 = 0xECEFF1,
		COLOR_BLUE_GRAY_100 = 0xCFD8DC,
		COLOR_BLUE_GRAY_200 = 0xB0BEC5,
		COLOR_BLUE_GRAY_300 = 0x90A4AE,
		COLOR_BLUE_GRAY_400 = 0x78909C,
		COLOR_BLUE_GRAY_500 = 0x607D8B,
		COLOR_BLUE_GRAY_600 = 0x546E7A,
		COLOR_BLUE_GRAY_700 = 0x455A64,
		COLOR_BLUE_GRAY_800 = 0x37474F,
		COLOR_BLUE_GRAY_900 = 0x263238;

	class Theme : public ReferenceCounted
	{
	public:
		/// <summary>
		/// <para>These are the colors that are used by all default views.</para>
		/// <para>You can change them and add your own.</para>
		/// <para>Built-in colors:</para>
		/// <para>"background"</para>
		/// <para>"on background"</para>
		/// <para>"primary"</para>
		/// <para>"primary on background"</para>
		/// <para>"on primary"</para>
		/// <para>"secondary"</para>
		/// <para>"secondary on background"</para>
		/// <para>"on secondary"</para>
		/// <para>"shadow"</para>
		/// </summary>
		std::map<const char*, Color> colors;

		/// <summary>
		/// <para>These are the animation easings used by the default views.</para>
		/// <para>You can change them and add your own.</para>
		/// <para>Built-in easings:</para>
		/// <para>"in"</para>
		/// <para>"out"</para>
		/// <para>"in out"</para>
		/// <para>"ripple"</para>
		/// </summary>
		std::map<const char*, Easing> easings;

		Theme()
		{
			colors["background"] = 0xfffefefe;
			colors["on background"] = 0xff070707;

			colors["primary"] = 0xff630eec;
			colors["primary on background"] = 0xff630eec;
			colors["on primary"] = 0xfffefefe;

			colors["secondary"] = COLOR_PINK_A200;
			colors["secondary on background"] = COLOR_PINK_A200;
			colors["on secondary"] = 0xfffefefe;

			colors["shadow"] = 0x70000000;

			//------------------------------

			easings["in"] = Easing(0.6, 0.0, 0.8, 0.2);
			easings["out"] = Easing(0.1, 0.9, 0.2, 1.0);
			easings["in out"] = Easing(0.4, 0.0, 0.0, 1.0);
			easings["ripple"] = Easing(0.1, 0.8, 0.2, 0.95);
		}
	};

#pragma endregion

	//------------------------------

	class View;

	class ViewEventListener
	{
	public:
		virtual void handleViewSizeChange(View* p_view) = 0;
	};

	//------------------------------

	class GUI;
	class ViewContainer;
	class DrawingContext;
	class Image;

	/// <summary>
	/// <para>A rectangle which can draw itself and receive events.</para>
	/// <para>Used for GUI components and stuff.</para>
	/// </summary>
	class View : public ReferenceCounted, public ProtectedRectangle
	{
	private:
		bool m_isVisible;
		float m_cornerRadius;

		Rectangle<float> m_lastBounds;
		Rectangle<float> m_lastShadowBounds;

		Image* m_shadowImage;
		bool m_hasShadow;
		float m_elevation;

		uint32_t m_layerIndex;
		uint32_t m_index;

		bool m_isInAnimationUpdateQueue;

		//------------------------------

		std::vector<ViewEventListener*> m_viewEventListeners;

		void* m_userData;

		//------------------------------

		void sendSizeChangedEvents()
		{
			handleSizeChanged();
			for (uint32_t a = 0; a < m_viewEventListeners.size(); a++)
			{
				m_viewEventListeners[a]->handleViewSizeChange(this);
			}
		}

		Point<float> calculateAbsolutePositionRelativeTo(Point<float> p_position) const;

	protected:
		GUI* m_GUI;
		ViewContainer* m_parent;
		Theme* m_theme;

	public:
		View(ViewContainer* p_parent, const Rectangle<float>& p_bounds);
		virtual ~View();

		//------------------------------

		/// <summary>
		/// Returns a pointer to the highest view in the hierarchy, the GUI.
		/// </summary>
		inline GUI* getGUI() const
		{
			return m_GUI;
		}

		//------------------------------

		/// <summary>
		/// Attaches this view to a new parent.
		/// </summary>
		inline void setParent(ViewContainer* p_container);
		/// <summary>
		/// Returns a pointer to the parent of this view.
		/// </summary>
		inline ViewContainer* getParent() const
		{
			return m_parent;
		}

		//------------------------------

		/// <summary>
		/// <para>Sets a new theme to be used by this view and upcoming children of this view.</para>
		/// <para>-</para>
		/// <para>Child views inherit their parent's theme, unless they make a new one.</para>
		/// <para>In that way, different sections of the GUI can have different themes.</para>
		/// <para>Only children created after this function is called will inherit the new theme.</para>
		/// </summary>
		/// <param name="p_newTheme">A pointer to a newly allocated theme on the heap.</param>
		inline void setTheme(Theme* p_newTheme)
		{
			if (m_theme)
			{
				m_theme->forget();
			}
			m_theme = p_newTheme;
		}
		/// <summary>
		/// <para>Returns a pointer to the theme which is used by this view.</para>
		/// </summary>
		inline Theme* getTheme() const
		{
			return m_theme;
		}

		//------------------------------

		/// <summary>
		/// Calculates the bounds of the view shadow relative to the top left corner of the GUI.
		/// </summary>
		Rectangle<float> calculateAbsoluteShadowBounds() const
		{
			Rectangle<float> shadowBounds(getShadowBounds());
			return Rectangle<float>(calculateAbsolutePositionRelativeTo(shadowBounds.getTopLeft()), shadowBounds.getSize());
		}
		/// <summary>
		/// Calculates the bounds of the view relative to the top left corner of the GUI.
		/// </summary>
		inline Rectangle<float> calculateAbsoluteBounds() const
		{
			return Rectangle<float>(calculateAbsoluteTopLeft(), getSize());
		}
		/// <summary>
		/// Calculates the top left position of the view relative to the top left corner of the GUI.
		/// </summary>
		Point<float> calculateAbsoluteTopLeft() const
		{
			return calculateAbsolutePositionRelativeTo(getTopLeft());
		}
		/// <summary>
		/// Calculates the top right position of the view relative to the top left corner of the GUI.
		/// </summary>
		Point<float> calculateAbsoluteTopRight() const
		{
			return calculateAbsolutePositionRelativeTo(getTopRight());
		}
		/// <summary>
		/// Calculates the bottom left position of the view relative to the top left corner of the GUI.
		/// </summary>
		Point<float> calculateAbsoluteBottomLeft() const
		{
			return calculateAbsolutePositionRelativeTo(getBottomLeft());
		}
		/// <summary>
		/// Calculates the bottom right position of the view relative to the top left corner of the GUI.
		/// </summary>
		Point<float> calculateAbsoluteBottomRight() const
		{
			return calculateAbsolutePositionRelativeTo(getBottomRight());
		}
		/// <summary>
		/// Calculates the center position of the view relative to the top left corner of the GUI.
		/// </summary>
		Point<float> calculateAbsoluteCenter() const
		{
			return calculateAbsolutePositionRelativeTo(getCenter());
		}

		//------------------------------

		/// <summary>
		/// Sets the rectangle representing the bounds of this view.
		/// </summary>
		inline void setBounds(const Rectangle<float>& p_rectangle) override
		{
			m_bounds = p_rectangle;
			if (p_rectangle.right - p_rectangle.left != m_bounds.right - m_bounds.left ||
				p_rectangle.bottom - p_rectangle.top != m_bounds.bottom - m_bounds.top)
			{
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Sets the rectangle representing the bounds of this view.
		/// </summary>
		inline void setBounds(float p_left, float p_top, float p_right, float p_bottom) override
		{
			m_bounds.left = p_left;
			m_bounds.top = p_top;
			m_bounds.right = p_right;
			m_bounds.bottom = p_bottom;
			if (p_right - p_left != m_bounds.right - m_bounds.left ||
				p_bottom - p_top != m_bounds.bottom - m_bounds.top)
			{
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Sets the rectangle representing the bounds of this view.
		/// </summary>
		inline void setBounds(const Point<float>& p_position, const Point<float>& p_size) override
		{
			m_bounds.left = p_position.x;
			m_bounds.top = p_position.y;
			m_bounds.right = p_position.x + p_size.x;
			m_bounds.bottom = p_position.y + p_size.y;
			if (p_size.x != m_bounds.right - m_bounds.left ||
				p_size.y != m_bounds.bottom - m_bounds.top)
			{
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Returns a rectangle representing the bounds of this view.
		/// </summary>
		inline const Rectangle<float>& getBounds() const override
		{
			return m_bounds;
		}

		//------------------------------

		/// <summary>
		/// Moves the bounds of the view.
		/// </summary>
		inline void move(const Point<float>& p_offset) override
		{
			m_bounds += p_offset;
		}
		/// <summary>
		/// Moves the bounds of the view.
		/// </summary>
		inline void move(float p_offsetX, float p_offsetY) override
		{
			m_bounds.move(p_offsetX, p_offsetY);
		}

		//------------------------------

		/// <summary>
		/// <para>Sets the top left coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setTopLeft(const Point<float>& p_position, bool p_willKeepSize = true) override
		{
			if (p_position.x != m_bounds.left || p_position.y != m_bounds.top)
			{
				m_bounds.setTopLeft(p_position, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// <para>Sets the top left coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setTopLeft(float p_left, float p_top, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_top != m_bounds.top)
			{
				m_bounds.setTopLeft(p_left, p_top, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinates of the top left corner of the view.
		/// </summary>
		inline Point<float> getTopLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.top);
		}

		/// <summary>
		/// <para>Sets the top right coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setTopRight(const Point<float>& p_position, bool p_willKeepSize = true) override
		{
			if (p_position.x != m_bounds.right || p_position.y != m_bounds.top)
			{
				m_bounds.setTopRight(p_position, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// <para>Sets the top right coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setTopRight(float p_right, float p_top, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_top != m_bounds.top)
			{
				m_bounds.setTopRight(p_right, p_top, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinates of the top right corner of the view.
		/// </summary>
		inline Point<float> getTopRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.top);
		}

		/// <summary>
		/// <para>Sets the bottom left coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setBottomLeft(const Point<float>& p_position, bool p_willKeepSize = true) override
		{
			if (p_position.x != m_bounds.left || p_position.y != m_bounds.bottom)
			{
				m_bounds.setBottomLeft(p_position, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// <para>Sets the bottom left coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setBottomLeft(float p_left, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_left != m_bounds.left || p_bottom != m_bounds.bottom)
			{
				m_bounds.setBottomLeft(p_left, p_bottom, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinates of the bottom left corner of the view.
		/// </summary>
		inline Point<float> getBottomLeft() const override
		{
			return Point<float>(m_bounds.left, m_bounds.bottom);
		}

		/// <summary>
		/// <para>Sets the bottom right coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setBottomRight(const Point<float>& p_position, bool p_willKeepSize = true) override
		{
			if (p_position.x != m_bounds.right || p_position.y != m_bounds.bottom)
			{
				m_bounds.setBottomRight(p_position, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// <para>Sets the bottom right coordinates of the view.</para> 
		/// <para>If p_willKeepSize is true, the view will only get moved, keeping its size.</para>
		/// </summary>
		inline void setBottomRight(float p_right, float p_bottom, bool p_willKeepSize = true) override
		{
			if (p_right != m_bounds.right || p_bottom != m_bounds.bottom)
			{
				m_bounds.setBottomRight(p_right, p_bottom, p_willKeepSize);
				if (!p_willKeepSize)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinates of the bottom right corner of the view.
		/// </summary>
		inline Point<float> getBottomRight() const override
		{
			return Point<float>(m_bounds.right, m_bounds.bottom);
		}

		//------------------------------

		/// <summary>
		/// <para>Sets the center coordinates of the view.</para> 
		/// </summary>
		inline void setCenter(const Point<float>& p_position) override
		{
			m_bounds.setCenter(p_position.x, p_position.y);
		}
		/// <summary>
		/// <para>Sets the center coordinates of the view.</para> 
		/// </summary>
		inline void setCenter(float p_x, float p_y) override
		{
			m_bounds.setCenter(p_x, p_y);
		}
		/// <summary>
		/// Sets the horizontal center coordinate of the view.
		/// </summary>
		inline void setCenterX(float p_x) override
		{
			m_bounds.setCenterX(p_x);
		}
		/// <summary>
		/// Sets the vertical center coordinate of the view.
		/// </summary>
		inline void setCenterY(float p_y) override
		{
			m_bounds.setCenterY(p_y);
		}
		/// <summary>
		/// Returns the center coordinates of the view.
		/// </summary>
		inline Point<float> getCenter() const override
		{
			return m_bounds.getCenter();
		}
		/// <summary>
		/// Returns the x-axis center coordinate of the view.
		/// </summary>
		inline float getCenterX() const override
		{
			return m_bounds.getCenterX();
		}
		/// <summary>
		/// Returns the y-axis center coordinate of the rectangle.
		/// </summary>
		inline float getCenterY() const override
		{
			return m_bounds.getCenterY();
		}

		//------------------------------

		/// <summary>
		/// <para>Sets the left coordinate of this view and updates the layout. If p_willKeepWidth is</para>
		/// <para>true, the right coordinate will be changed so that the width of the view stays the same.</para>
		/// </summary>
		inline void setLeft(float p_left, bool p_willKeepWidth = false) override
		{
			if (p_left != m_bounds.left)
			{
				m_bounds.setLeft(p_left, p_willKeepWidth);
				if (!p_willKeepWidth)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the left coordinate of this view.
		/// </summary>
		inline float getLeft() const override
		{
			return m_bounds.left;
		}

		/// <summary>
		/// <para>Sets the top coordinate of this view. If p_willKeepHeight is true, the bottom </para>
		/// <para>coordinate will be changed so that the height of the view stays the same.</para>
		/// </summary>
		inline void setTop(float p_top, bool p_willKeepHeight = false) override
		{
			if (p_top != m_bounds.top)
			{
				m_bounds.setTop(p_top, p_willKeepHeight);
				if (!p_willKeepHeight)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the top coordinate of this view.
		/// </summary>
		inline float getTop() const override
		{
			return m_bounds.top;
		}

		/// <summary>
		/// <para>Sets the right coordinate of this view. If p_willKeepWidth is true, the left</para>
		/// <para>coordinate will be changed so that the width of the view stays the same.</para>
		/// </summary>
		inline void setRight(float p_right, bool p_willKeepWidth = false) override
		{
			if (p_right != m_bounds.right)
			{
				m_bounds.setRight(p_right, p_willKeepWidth);
				if (!p_willKeepWidth)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinate of the right edge of this view.
		/// </summary>
		inline float getRight() const override
		{
			return m_bounds.right;
		}

		/// <summary>
		/// <para>Sets the bottom coordinate of this view and updates the layout. If p_willKeepHeight is true, the top</para>
		/// <para>coordinate will be changed so that the height of the view stays the same.</para>
		/// </summary>
		inline void setBottom(float p_bottom, bool p_willKeepHeight = false) override
		{
			if (p_bottom != m_bounds.bottom)
			{
				m_bounds.setBottom(p_bottom, p_willKeepHeight);
				if (!p_willKeepHeight)
				{
					handleSizeChanged();
				}
			}
		}
		/// <summary>
		/// Returns the coordinate of the bottom edge of this view.
		/// </summary>
		inline float getBottom() const override
		{
			return m_bounds.bottom;
		}

		//------------------------------

		/// <summary>
		/// Sets the width of this view in pixels and updates the layout.
		/// </summary>
		inline void setWidth(float p_width) override
		{
			if (p_width != m_bounds.right - m_bounds.left)
			{
				m_bounds.setWidth(p_width);
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Returns the height of this view in pixels.
		/// </summary>
		inline float getWidth() const override
		{
			return m_bounds.right - m_bounds.left;
		}

		/// <summary>
		/// Sets the height of this view in pixels and updates the layout.
		/// </summary>
		inline void setHeight(float p_height) override
		{
			if (p_height != m_bounds.bottom - m_bounds.top)
			{
				m_bounds.setHeight(p_height);
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Returns the height of this view in pixels.
		/// </summary>
		inline float getHeight() const override
		{
			return m_bounds.bottom - m_bounds.top;
		}

		/// <summary>
		/// Sets the size of this view in pixels and updates the layout.
		/// </summary>
		inline void setSize(const Point<float>& p_size) override
		{
			if (p_size.x != m_bounds.right - m_bounds.left || p_size.y != m_bounds.bottom - m_bounds.top)
			{
				m_bounds.setSize(p_size);
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Sets the size of this view in pixels and updates the layout.
		/// </summary>
		inline void setSize(float p_width, float p_height) override
		{
			if (p_width != m_bounds.right - m_bounds.left || p_height != m_bounds.bottom - m_bounds.top)
			{
				m_bounds.setSize(p_width, p_height);
				handleSizeChanged();
			}
		}
		/// <summary>
		/// Returns the size of this view in pixels.
		/// </summary>
		inline Point<float> getSize() const override
		{
			return Point<float>(m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top);
		}

		//------------------------------

		/// <summary>
		/// Returns if this view intersects/overlaps a rectangle.
		/// </summary>
		inline bool getIsIntersecting(const Rectangle<float>& p_rectangle) const override
		{
			if (m_cornerRadius > 0.f)
			{
				if (m_bounds.getIsIntersecting(p_rectangle))
				{
					if (p_rectangle.right < m_bounds.left + m_cornerRadius)
					{
						if (p_rectangle.bottom < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.right, p_rectangle.bottom, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_rectangle.top > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.right, p_rectangle.top, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					else if (p_rectangle.left > m_bounds.right - m_cornerRadius)
					{
						if (p_rectangle.bottom < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.left, p_rectangle.bottom, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_rectangle.top > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.left, p_rectangle.top, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsIntersecting(p_rectangle);
		}
		/// <summary>
		/// Returns if this view intersects/overlaps another protected rectangle.
		/// </summary>
		inline bool getIsIntersecting(ProtectedRectangle* p_protectedRectangle) const override
		{
			return getIsIntersecting(p_protectedRectangle->getBounds());
		}
		/// <summary>
		/// Returns if this view intersects/overlaps another view. Takes rounded corners of both views into account.
		/// </summary>
		bool getIsIntersecting(View* p_view) const;

		//------------------------------

		/// <summary>
		/// Returns if this view contains a rectangle. The rectangle is relative to the parent of the view.
		/// </summary>
		inline bool getIsContaining(const Rectangle<float>& p_rectangle) const override
		{
			if (m_cornerRadius > 0.f)
			{
				if (m_bounds.getIsContaining(p_rectangle))
				{
					if (p_rectangle.left < m_bounds.left + m_cornerRadius)
					{
						if (p_rectangle.top < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.left, p_rectangle.top, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_rectangle.bottom > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.left, p_rectangle.bottom, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					else if (p_rectangle.right > m_bounds.right - m_cornerRadius)
					{
						if (p_rectangle.top < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.right, p_rectangle.top, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_rectangle.bottom > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_rectangle.right, p_rectangle.bottom, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_rectangle);
		}
		/// <summary>
		/// Returns if this view contains another protected rectangle. The rectangle is relative to the parent of this view.
		/// </summary>
		inline bool getIsContaining(ProtectedRectangle* p_rectangle) const override
		{
			return getIsContaining(p_rectangle->getBounds());
		}
		/// <summary>
		/// Returns if this view contains another view. Takes rounded corners of both views into account.
		/// </summary>
		bool getIsContaining(View* p_view) const;

		/// <summary>
		/// Returns if this view contains a point. The point is relative to the parent of the view.
		/// </summary>
		inline bool getIsContaining(float p_x, float p_y) const override
		{
			if (m_cornerRadius > 0.f)
			{
				if (m_bounds.getIsContaining(p_x, p_y))
				{
					if (p_x < m_bounds.left + m_cornerRadius)
					{
						if (p_y < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_x, p_y, m_bounds.left + m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_y > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_x, p_y, m_bounds.left + m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					else if (p_x > m_bounds.right - m_cornerRadius)
					{
						if (p_y < m_bounds.top + m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_x, p_y, m_bounds.right - m_cornerRadius, m_bounds.top + m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
						if (p_y > m_bounds.bottom - m_cornerRadius)
						{
							return Point<>::getDistanceSquared(p_x, p_y, m_bounds.right - m_cornerRadius, m_bounds.bottom - m_cornerRadius) < m_cornerRadius*m_cornerRadius;
						}
					}
					return true;
				}
				return false;
			}
			return m_bounds.getIsContaining(p_x, p_y);
		}
		/// <summary>
		/// Returns if this view contains a point. The point is relative to the parent of the view.
		/// </summary>
		inline bool getIsContaining(const Point<float>& p_point) const override
		{
			return getIsContaining(p_point.x, p_point.y);
		}

		//------------------------------

		/// <summary>
		/// <para>Sets if the view is visible and can receive events.</para>
		/// </summary>
		inline void setIsVisible(bool p_isVisible)
		{
			m_isVisible = p_isVisible;
		}
		/// <summary>
		/// <para>Returns if the view is visible and can receive events.</para>
		/// </summary>
		inline bool getIsVisible() const
		{
			return m_isVisible;
		}

		//------------------------------

		/// <summary>
		/// Sets the roundness of the corners of the view.
		/// </summary>
		/// <param name="p_radius">Radius of the corner circles.</param>
		inline void setCornerRadius(float p_radius)
		{
			m_cornerRadius = p_radius;
		}
		/// <summary>
		/// Returns the roundness of the corners of the view, as a radius. 
		/// </summary>
		inline float getCornerRadius() const
		{
			return m_cornerRadius;
		}

		//------------------------------

		/// <summary>
		/// <para>Sets the elevation of the view. This both changes its shadow (if the view has shadow) and drawing order.</para>
		/// <para>The higher the elevation is, the later it will get drawn.</para>
		/// </summary>
		/// <param name="p_elevation">If this is negative, it is set from the top of the elevation space.</param>
		void setElevation(float p_elevation);
		/// <summary>
		/// <para>Returns the elevation of the view. See the setElevation method.</para>
		/// </summary>
		inline float getElevation() const
		{
			return m_elevation;
		}

		/// <summary>
		/// Sets whether the elevation is shown with a shadow or not.
		/// </summary>
		void setHasShadow(bool p_hasShadow);
		/// <summary>
		/// Returns whether the elevation is shown with a shadow or not.
		/// </summary>
		bool getHasShadow() const
		{
			return m_hasShadow;
		}

		/// <summary>
		/// <para>Returns the rectangle which represents the area where the shadow is drawn.</para>
		/// <para>This rectangle is bigger than the view or equal to the size of the view.</para>
		/// </summary>
		Rectangle<float> getShadowBounds() const;

		/// <summary>
		/// This is only used by view containers.
		/// </summary>
		inline void setIndex(uint32_t p_index)
		{
			m_index = p_index;
		}
		/// <summary>
		/// <para>Returns the index of this view relative to its siblings.</para>
		/// </summary>
		inline uint32_t getIndex() const
		{
			return m_index;
		}

		/// <summary>
		/// Returns the layer index of the view, how deep down the view hierarchy it is.
		/// </summary>
		inline uint32_t getLayerIndex() const
		{
			return m_layerIndex;
		}

		//------------------------------

		/// <summary>
		/// <para>Sets some arbitrary data you can use yourself to keep track of the view, or not.</para>
		/// </summary>
		/// <param name="p_userData"></param>
		inline void setUserData(void* p_userData)
		{
			m_userData = p_userData;
		}
		/// <summary>
		/// Returns your data that you for some reason wanted to associate with this view.
		/// </summary>
		inline void* getUserData()
		{
			return m_userData;
		}

		//------------------------------

		/// <summary>
		/// Queues an animation update for the next frame.
		/// </summary>
		void queueAnimationUpdate();
		/// <summary>
		/// Don't do anything with this.
		/// </summary>
		inline void informAboutAnimationUpdateQueueRemoval()
		{
			m_isInAnimationUpdateQueue = false;
		}

		/// <summary>
		/// <para>Updates things like animations and does other stuff that you never want to happen</para>
		/// <para>more than once every frame. Call queueAnimationUpdate() when you want this</para>
		/// <para>method to be called in the next frame. This system allows for animations</para>
		/// <para>to only get updated when they have to.</para>
		/// </summary>
		virtual void updateAnimations() { }

		//------------------------------

		/// <summary>
		/// Adds an event listener to the view that recieves events about when the view has changed size.
		/// </summary>
		void addEventListener(ViewEventListener* p_eventListener)
		{
			m_viewEventListeners.push_back(p_eventListener);
		}
		/// <summary>
		/// Removes an event listener from the view.
		/// </summary>
		void removeEventListener(ViewEventListener* p_eventListener)
		{
			removeVectorElement(m_viewEventListeners, p_eventListener);
		}

		//------------------------------

		/// <summary>
		/// <para>Implement this function in your view if you want to update things when the size of</para>
		/// <para>the view has been changed.</para>
		/// </summary>
		virtual void handleSizeChanged() { }

		/// <summary>
		/// <para>Call this if you want the view to get redrawn. Adds an invalid rectangle to the window.</para>
		/// </summary>
		void invalidate();

		/// <summary>
		/// <para>Draws the shadow of the view. This gets called by the parent ViewContainer before the</para>
		/// <para>content of the view is drawn.</para>
		/// </summary>
		void drawShadow(DrawingContext* p_drawingContext);
		/// <summary>
		/// <para>Draws the content of the view. This is a method that is called by default from the other draw method</para>
		/// <para>that also takes the target rectangle as input. You do not often care about that parameter.</para>
		/// </summary>
		/// <param name="p_drawingContext">Object used to draw graphics to the window.</param>
		virtual inline void draw(DrawingContext* p_drawingContext) { }
		/// <summary>
		/// <para>Draws the content of the view. Override this method if you want the target rectangle, override the overloaded</para>
		/// <para>method that only takes the drawing context otherwise.</para>
		/// </summary>
		/// <param name="p_drawingContext">Object used to draw graphics to the window.</param>
		/// <param name="p_targetRectangle">
		/// The rectangle that needs to be drawn. To optimize your application, you can make sure to only draw stuff in this region.
		/// </param>
		virtual inline void draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle)
		{
			draw(p_drawingContext);
		}

		/// <summary>
		/// <para>Draws on top of the view, without any view clipping applied. This is a method that is called by default from the other</para>
		/// <para>drawUnclipped method that also takes the target rectangle as input. You do not often care about that parameter.</para>
		/// </summary>
		/// <param name="p_drawingContext"></param>
		virtual inline void drawUnclipped(DrawingContext* p_drawingContext) {}

		/// <summary>
		/// <para>Draws on top of the view, without any view clipping applied. Override this method if you want the target rectangle, override</para>
		/// <para>the overloaded method that only takes the drawing context otherwise.</para>
		/// </summary>
		/// <param name="p_drawingContext">Object used to draw graphics to the window.</param>
		/// <param name="p_targetRectangle">
		/// The rectangle that needs to be drawn. To optimize your application, you can make sure to only draw stuff in this region.
		/// </param>
		virtual inline void drawUnclipped(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle)
		{
			drawUnclipped(p_drawingContext);
		}
	};

	//------------------------------

	/// <summary>
	/// <para>A view that has other views inside it. It reacts to mouse events as if it were behind the child views.</para>
	/// </summary>
	class ViewContainer : public View
	{
	protected:
		std::vector<View*> m_views;

	public:
		ViewContainer(ViewContainer* p_parent, const Rectangle<float>& p_bounds);
		virtual ~ViewContainer();

		//------------------------------

		/// <summary>
		/// <para>Adds a child view to the container. Do not call this method yourself, it is called automatically</para>
		/// <para>when you create a view with a parent.</para>
		/// </summary>
		void addView(View* p_view);
		/// <summary>
		/// <para>Removes a child view from the container. This forgets the view. If you haven't remembered</para>
		/// <para>it yourself, it will get deleted.</para>
		/// </summary>
		/// <param name="p_view">Pointer to view to remove from the container</param>
		void removeView(View* p_view);
		/// <summary>
		/// <para>Removes a child view from the container. This forgets the view. If you haven't remembered</para>
		/// <para>it yourself, it will get deleted.</para>
		/// </summary>
		/// <param name="p_viewIndex">Index of the view to remove from the container</param>
		void removeView(uint32_t p_viewIndex);
		/// <summary>
		/// Deletes the children views and empties the view container.
		/// </summary>
		void removeAllViews();

		/// <summary>
		/// Returns the child view at an index.
		/// </summary>
		inline View* getView(uint32_t p_viewIndex)
		{
			return m_views[p_viewIndex];
		}
		/// <summary>
		/// Returns the number of child views.
		/// </summary>
		inline uint32_t getNumberOfViews()
		{
			return m_views.size();
		}

		/// <summary>
		/// <para>Makes sure the view is drawn at the correct time, according to elevation. You shouldn't</para>
		/// <para>need to use this method, it is used internally.</para>
		/// </summary>
		void updateViewDrawingIndex(View* p_view);

		//------------------------------

		inline Rectangle<float> calculateContentBounds() const
		{
			if (!m_views.size())
			{
				return Rectangle<float>();
			}

			float left = m_views[0]->getLeft();
			float right = m_views[0]->getRight();
			float top = m_views[0]->getTop();
			float bottom = m_views[0]->getBottom();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getLeft() < left)
				{
					left = m_views[a]->getLeft();
				}
				if (m_views[a]->getTop() < top)
				{
					top = m_views[a]->getTop();
				}
				if (m_views[a]->getRight() > right)
				{
					right = m_views[a]->getRight();
				}
				if (m_views[a]->getBottom() > bottom)
				{
					bottom = m_views[a]->getBottom();
				}
			}

			return Rectangle<float>(left, top, right, bottom);
		}

		inline float calculateContentWidth() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float left = m_views[0]->getLeft();
			float right = m_views[0]->getRight();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getLeft() < left)
				{
					left = m_views[a]->getLeft();
				}
				if (m_views[a]->getRight() > right)
				{
					right = m_views[a]->getRight();
				}
			}
			return right - left;
		}
		inline float calculateContentHeight() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float top = m_views[0]->getTop();
			float bottom = m_views[0]->getBottom();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getTop() < top)
				{
					top = m_views[a]->getTop();
				}
				if (m_views[a]->getBottom() > bottom)
				{
					bottom = m_views[a]->getBottom();
				}
			}
			return bottom - top;
		}
		inline Point<float> calculateContentSize() const
		{
			return calculateContentBounds().getSize();
		}

		inline float calculateContentLeft() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float left = m_views[0]->getLeft();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getLeft() < left)
				{
					left = m_views[a]->getLeft();
				}
			}
			return left;
		}
		inline float calculateContentRight() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float right = m_views[0]->getRight();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getRight() > right)
				{
					right = m_views[a]->getRight();
				}
			}
			return right;
		}
		inline float calculateContentTop() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float top = m_views[0]->getTop();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getTop() < top)
				{
					top = m_views[a]->getTop();
				}
			}
			return top;
		}
		inline float calculateContentBottom() const
		{
			if (!m_views.size())
			{
				return 0.f;
			}

			float bottom = m_views[0]->getBottom();
			for (uint32_t a = 1; a < m_views.size(); a++)
			{
				if (m_views[a]->getBottom() > bottom)
				{
					bottom = m_views[a]->getBottom();
				}
			}
			return bottom;
		}

		//------------------------------

		inline void setPadding(float p_padding) 
		{
			setPadding(p_padding, p_padding, p_padding, p_padding);
		}
		inline void setPadding(float p_horizontalPadding, float p_verticalPadding)
		{
			setPadding(p_horizontalPadding, p_horizontalPadding, p_verticalPadding, p_verticalPadding);
		}
		inline void setPadding(float p_leftPadding, float p_rightPadding, float p_topPadding, float p_bottomPadding)
		{
			Rectangle<float> contentBounds(calculateContentBounds());
			Point<float> offset(p_leftPadding - contentBounds.left, p_topPadding - contentBounds.top);
			for (uint32_t a = 0; a < m_views.size(); a++)
			{
				m_views[a]->move(offset);
			}
			setSize(contentBounds.getWidth() + p_leftPadding + p_rightPadding, contentBounds.getHeight() + p_topPadding + p_bottomPadding);
		}

		inline void setLeftPadding(float p_leftPadding)
		{
			float left = calculateContentLeft();
			float offset = p_leftPadding - left;
			for (uint32_t a = 0; a < m_views.size(); a++)
			{
				m_views[a]->move(offset, 0.f);
			}
			setWidth(getWidth() + offset);
		}
		inline void setRightPadding(float p_rightPadding)
		{
			setWidth(calculateContentRight() + p_rightPadding);
		}
		inline void setTopPadding(float p_topPadding)
		{
			float top = calculateContentTop();
			float offset = p_topPadding - top;
			for (uint32_t a = 0; a < m_views.size(); a++)
			{
				m_views[a]->move(offset, 0.f);
			}
			setHeight(getHeight() + offset);
		}
		inline void setBottomPadding(float p_bottomPadding)
		{
			setHeight(calculateContentRight() + p_bottomPadding);
		}
	};

	//------------------------------

#pragma region Window stuff
	class Window;

	class WindowEvent
	{
	public:
		/// <summary>
		/// The window that has experienced the event.
		/// </summary>
		Window* window = 0;
		/// <summary>
		/// The new width of the window if it has been maximized, restored or changed size.
		/// </summary>
		uint32_t width = 0U;
		/// <summary>
		/// The new height of the window if it has been maximized, restored or changed size.
		/// </summary>
		uint32_t height = 0U;
	};

	class WindowEventListener
	{
	public:
		virtual void handleWindowCreated(const WindowEvent& p_event) { }
		virtual void handleWindowDestroyed(const WindowEvent& p_event) { }

		virtual void handleWindowMinimized(const WindowEvent& p_event) { }
		virtual void handleWindowMaximized(const WindowEvent& p_event) { }
		virtual void handleWindowRestored(const WindowEvent& p_event) { }

		/// <summary>
		/// <para>Gets called when the size of the window has changed. This includes if it has been maximized,</para>
		/// <para>restored or if the border has been dragged to resize it. The width and height properties of</para>
		/// <para>the event tell you the new size of the window.</para>
		/// </summary>
		/// <param name="p_event"></param>
		virtual void handleWindowSizeChanged(const WindowEvent& p_event) { }

		virtual void handleWindowFocused(const WindowEvent& p_event) { }
		virtual void handleWindowUnfocused(const WindowEvent& p_event) { }
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
		return (uint32_t)p_left & (uint32_t)p_right;
	}
	constexpr ModifierKeyFlags operator|(ModifierKeyFlags p_left, ModifierKeyFlags p_right)
	{
		return ModifierKeyFlags((uint32_t)p_left | (uint32_t)p_right);
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
		/// <summary>
		/// X coordinate of the mouse pointer.
		/// </summary>
		int32_t x = 0;
		/// <summary>
		/// Y coordinate of the mouse pointer.
		/// </summary>
		int32_t y = 0;
		/// <summary>
		/// <para>The movement of the mouse pointer in the X axis that has moved the pointer from where it was</para>
		/// <para>before to where it is now.</para>
		/// </summary>
		int32_t movementX = 0;
		/// <summary>
		/// <para>The movement of the mouse pointer in the Y axis that has moved the pointer from where it was</para>
		/// <para>before to where it is now.</para>
		/// </summary>
		int32_t movementY = 0;
		/// <summary>
		/// <para>How much the mouse wheel has been moved. If it is positive, the wheel has been moved away</para>
		/// <para>from the user, if it negative it has moved towards the user. It represents the number of ticks</para>
		/// <para>the wheel has been moved, but can be a fraction if the mouse has smooth scrolling.</para>
		/// </summary>
		float scrollDelta = 0.f;
		/// <summary>
		/// The mouse button that has been pressed, released or double clicked (depending on the mouse event).
		/// </summary>
		MouseButton mouseButton = MouseButton::None;
		/// <summary>
		/// The modifier keys and mouse buttons that were down when the event ocurred.
		/// </summary>
		ModifierKeyFlags modifierKeys = ModifierKeyFlags::None;
		/// <summary>
		/// <para>If this is true, the event is directed to the view that gets the event. A mouse event is directed</para>
		/// <para>to a view if the mouse either moved from, moved to, or hovered over the view when the event</para>
		/// <para>ocurred. If it is a mouse up event, this is only true if the last mouse down was above the view.</para>
		/// <para>You usually only want to do anything if this is true. But if indirect mouse events are disabled</para>
		/// <para>in the GUI, isTarget is always true because events are not sent to the untargeted views.</para>
		/// <para>Multiple views can be targeted in one event since mouse event listeners can be overlaid.</para>
		/// </summary>
		bool isTarget = false;
	};

	class MouseEventListener
	{
	private:
		Cursor m_cursor;
		bool m_isOverlay;

	public:
		MouseEventListener() :
			m_isOverlay(false), m_cursor(Cursor::Arrow)
		{
		}

		//------------------------------------

		/// <summary>
		/// Sets the cursor that will by default be shown when the mouse enters the view (if this mouse listener is a view).
		/// </summary>
		inline void setCursor(Cursor p_cursor)
		{
			m_cursor = p_cursor;
		}
		/// <summary>
		/// Returns the cursor that will by default be shown when the mouse enters the view (if this mouse listener is a view).
		/// </summary>
		inline Cursor getCursor()
		{
			return m_cursor;
		}

		//------------------------------------

		/// <summary>
		/// <para>If you set this to true, any mouse events that are directed to this view are also sent to the view below.</para>
		/// </summary>
		void setIsOverlay(bool p_isOverlay)
		{
			m_isOverlay = p_isOverlay;
		}
		/// <summary>
		/// <para>Returns if this view is a mouse event overlay. If it is true, it means that any mouse events directed to this</para>
		/// <para>view are also directed to the view below.</para>
		/// </summary>
		/// <returns></returns>
		bool getIsOverlay()
		{
			return m_isOverlay;
		}

		//------------------------------------

		/// <summary>
		/// Gets called when a mouse button has been pressed down.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseDown(const MouseEvent& p_event) { }
		/// <summary>
		/// Gets called when a mouse button has been released.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseUp(const MouseEvent& p_event) { }
		/// <summary>
		/// Gets called when a mouse button has been double clicked.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseDoubleClick(const MouseEvent& p_event) { }

		/// <summary>
		/// <para>Gets called when the mouse pointer has been moved. The isTarget property of the event is only true</para>
		/// <para>if the mouse has been moved inside of the view. If it has entered the view, a mouse enter event is</para>
		/// <para>sent, and if it has left the view, a mouse leave event is sent.</para>
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseMove(const MouseEvent& p_event) { }
		/// <summary>
		/// Gets called when the mouse pointer has entered the view, if this mouse listener is a view, that is.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseEnter(const MouseEvent& p_event);
		/// <summary>
		/// Gets called when the mouse pointer has left the view, if this mouse listener is a view, that is.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseLeave(const MouseEvent& p_event) { }
		/// <summary>
		/// Gets called when the mouse wheel has been moved/scrolled.
		/// </summary>
		/// <param name="p_event">An object that contains information about the mouse event.</param>
		virtual void handleMouseScrolled(const MouseEvent& p_event) { }
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
		/// <summary>
		/// The character that was pressed. This is only valid for character press events.
		/// </summary>
		char character = 0;
		/// <summary>
		/// The keyboard key that was pressed or released. This is not valid for character press events.
		/// </summary>
		KeyboardKey key = KeyboardKey::None;
		/// <summary>
		/// <para>If this is true, this character/key press event is generated after the initial attack because</para>
		/// <para>the key is being held down.</para>
		/// </summary>
		bool isRepeated = false;
	};

	class KeyboardEventListener
	{
	public:
		/// <summary>
		/// <para>This method is called when a character key has been pressed. The event contains the character</para>
		/// <para>that was pressed, and whether it's a repeated character or not.</para>
		/// </summary>
		/// <param name="p_event">Object containing information about the event.</param>
		virtual void handleCharacterPressed(const KeyboardEvent& p_event) { }
		/// <summary>
		/// <para>This method is called when a keyboard key has been pressed. The event contains the virtual key</para>
		/// <para>that was pressed, and whether it's a repeated key event or not.</para>
		/// </summary>
		/// <param name="p_event">Object containing information about the event.</param>
		virtual void handleKeyboardKeyPressed(const KeyboardEvent& p_event) { }
		/// <summary>
		/// <para>This method is called when a keyboard key has been released. The event only contains the virtual</para>
		/// <para>key that was pressed.</para>
		/// </summary>
		/// <param name="p_event">Object containing information about the event.</param>
		virtual void handleKeyboardKeyReleased(const KeyboardEvent& p_event) { }
	};

	//------------------------------

	enum class WindowStyleFlags
	{
		Border = 0x1UL,
		Visible = 0x2UL,
		Child = 0x4UL,
		Minimized = 0x8UL,
		Maximized = 0x10UL,
		MinimizeBox = 0x20UL,
		MaximizeBox = 0x40UL,
		ResizeBorder = 0x80UL,
		Default = Border | Visible | MinimizeBox | MaximizeBox | ResizeBorder
	};

	constexpr WindowStyleFlags operator&(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32_t)p_left & (uint32_t)p_right);
	}
	constexpr WindowStyleFlags operator|(WindowStyleFlags p_left, WindowStyleFlags p_right)
	{
		return WindowStyleFlags((uint32_t)p_left | (uint32_t)p_right);
	}
	constexpr WindowStyleFlags& operator|=(WindowStyleFlags& p_left, WindowStyleFlags p_right)
	{
		p_left = p_left | p_right;
		return p_left;
	}

	class GUI;

	/// <summary>
	/// An abstract window.
	/// Only intended to be created by a GUI.
	/// </summary>
	class Window : public ReferenceCounted
	{
	protected:
		GUI* m_GUI;
		std::deque<View*> m_animationUpdateQueue;

		Point<int32_t> m_position;
		Point<uint32_t> m_size;
		bool m_isFullscreen;

		Point<int32_t> m_mousePosition;

	public:
		virtual void create(const char* p_title, int32_t p_x, int32_t p_y, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, Window* p_parent = 0) = 0;
		virtual void create(const char* p_title, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_styleFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, Window* p_parent = 0) = 0;

		virtual void close() = 0;

		//------------------------------

		virtual void* getWindowHandle() = 0;

		//------------------------------

		virtual void setIsFullscreen(bool p_isFullscreen) = 0;
		bool getIsFullscreen()
		{
			return m_isFullscreen;
		}

		//------------------------------

		virtual void hide() = 0;
		virtual void show() = 0;

		virtual void maximize() = 0;
		virtual void minimize() = 0;
		virtual void restore() = 0;

		//------------------------------

		virtual void setPosition(const Point<int32_t>& p_position) = 0;
		virtual void setPosition(int32_t p_x, int32_t p_y) = 0;
		inline const Point<int32_t>& getPosition() const
		{
			return m_position;
		}
		inline int32_t getX() const
		{
			return m_position.x;
		}
		inline int32_t getY() const
		{
			return m_position.y;
		}

		virtual void setSize(const Point<uint32_t>& p_size) = 0;
		virtual void setSize(uint32_t p_width, uint32_t p_height) = 0;
		inline const Point<uint32_t>& getSize() const
		{
			return m_size;
		}
		inline uint32_t getWidth() const
		{
			return m_size.x;
		}
		inline uint32_t getHeight() const
		{
			return m_size.y;
		}

		//------------------------------

		/// <summary>
		/// <para>Returns the bounds of the current monitor used</para>
		/// <para>by the window</para>
		/// </summary>
		virtual Rectangle<uint32_t> getMonitorBounds() = 0;
		/// <summary>
		/// <para>Returns the size of the current monitor used</para>
		/// <para>by the window.</para>
		/// </summary>
		virtual Point<uint32_t> getMonitorPosition() = 0;
		/// <summary>
		/// <para>Returns the size of the current monitor used</para>
		/// <para>by the window.</para>
		/// </summary>
		virtual Point<uint32_t> getMonitorSize() = 0;

		//------------------------------

		virtual bool getIsKeyDown(KeyboardKey p_key) = 0;
		virtual bool getIsMouseButtonDown(MouseButton p_button) = 0;
		inline const Point<int32_t>& getMousePosition()
		{
			return m_mousePosition;
		}

		//------------------------------

		virtual void setAnimationTimerInterval(uint32_t p_interval) = 0;
		virtual void queueAnimationUpdateForView(View* p_view) = 0;

		virtual void invalidateRect(const Rectangle<float>& p_rectangle) = 0;

		//------------------------------

		virtual void setCursor(Cursor p_cursor) = 0;
		virtual Cursor getCursor() = 0;
	};
#pragma endregion

	//------------------------------

	class Image : public ReferenceCounted
	{
	public:
		virtual void* getHandle() = 0;

		virtual Point<uint32_t> getSize() = 0;
		virtual uint32_t getWidth() = 0;
		virtual uint32_t getHeight() = 0;
	};

	//------------------------------

	enum class WordWrapping
	{
		Emergency, // Keeps words whole unless a word is wider than the maximum width.
		WholeWord, // Only wraps between words to prevent overflow.
		Always, // Always wraps to the next line to prevent overflow.
		Never // Allows overflow, never wraps.
	};

	enum class FontWeight
	{
		Thin100 = 100,
		Thin200 = 200,
		Thin300 = 300,
		Thin350 = 350,
		Medium = 400,
		Thick500 = 500,
		Thick600 = 600,
		Thick700 = 700,
		Thick800 = 800,
		Thick900 = 900,
		Thick950 = 950
	};

	enum class FontStyle
	{
		Normal,
		Oblique,
		Italic
	};

	enum class FontStretch
	{
		Undefined = 0,
		UltraCondensed = 1, // Most condensed
		ExtraCondensed = 2,
		Condensed = 3,
		SemiCondensed = 4,
		Medium = 5,
		SemiStretched = 6,
		Stretched = 7,
		ExtraStretched = 8,
		UltraStretched = 9 // Most stretched
	};

	class Text : public ProtectedRectangle, public ReferenceCounted
	{
	public:
		virtual void setWordWrapping(WordWrapping p_wordWrapping) = 0;
		virtual WordWrapping getWordWrapping() = 0;

		virtual void minimizeSize() = 0;

		//------------------------------

		/// <summary>
		/// Sets the font in a section of the text.
		/// </summary>
		/// <param name="p_name">The name of the font to be used.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this font.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this font.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setFont(const char* p_name, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;

		//------------------------------

		/// <summary>
		/// Sets the character spacing in a section of the text.
		/// </summary>
		/// <param name="p_characterSpacing">The spacing between the characters of the text.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this spacing.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this spacing.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setCharacterSpacing(float p_characterSpacing, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Sets the leading and trailing spacing of the characters in a section of the text.
		/// </summary>
		/// <param name="p_leading">The spacing before the characters of the text.</param>
		/// <param name="p_trailing">The spacing after the characters of the text.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this spacing.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this spacing.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setCharacterSpacing(float p_leading, float p_trailing, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Returns the spacing before one of the characters.
		/// </summary>
		virtual float getLeadingCharacterSpacing(int32_t p_characterIndex = 0) = 0;
		/// <summary>
		/// Returns the spacing after one of the characters.
		/// </summary>
		virtual float getTrailingCharacterSpacing(int32_t p_characterIndex = 0) = 0;

		//------------------------------

		/// <summary>
		/// Sets the font weight in a section of the text.
		/// </summary>
		/// <param name="p_fontWeight">The thickness of the text.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this font weight.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this font weight.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setFontWeight(FontWeight p_fontWeight, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Returns the weight/thickness of a character in the text.
		/// </summary>
		virtual FontWeight getFontWeight(uint32_t p_characterPosition = 0) = 0;

		//------------------------------

		/// <summary>
		/// Sets the font style in a section of the text.
		/// </summary>
		/// <param name="p_fontStyle">The style of the text.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this font style.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this font style.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setFontStyle(FontStyle p_fontStyle, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Returns the style of a character in the text.
		/// </summary>
		virtual FontStyle getFontStyle(uint32_t p_characterPosition = 0) = 0;

		//------------------------------

		/// <summary>
		/// Sets the font stretch in a section of the text.
		/// </summary>
		/// <param name="p_fontStretch">How stretched/wide the characters of the text are.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this font stretch.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this font stretch.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setFontStretch(FontStretch p_fontStretch, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Returns the font stretch of a character in the text.
		/// </summary>
		virtual FontStretch getFontStretch(uint32_t p_characterPosition = 0) = 0;

		//------------------------------

		/// <summary>
		/// Sets the font size in a section of the text.
		/// </summary>
		/// <param name="p_fontSize">The size (height) of the text.</param>
		/// <param name="p_startPosition">
		/// <para>The position of the first character to use this font size.</para>
		/// <para>If this is negative, it is relative to the end of the text.</para>
		/// </param>
		/// <param name="p_length">
		/// <para>The number of characters to use this font size.</para>
		/// <para>If this is negative, it goes to the left of the start position.</para>
		/// <para>If it is 0, everything after the starting position will be affected.</para>
		/// </param>
		virtual void setFontSize(float p_fontSize, int32_t p_startPosition = 0, int32_t p_length = 0) = 0;
		/// <summary>
		/// Returns the size (height) of a character in the text.
		/// </summary>
		virtual float getFontSize(uint32_t p_characterPosition = 0) = 0;

		//------------------------------

		virtual const std::string& getString() = 0;

		//------------------------------

		virtual void* getHandle() = 0;
	};

	//------------------------------

#pragma region Drawing context stuff
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

	struct TextProperties : public ReferenceCounted
	{
	public:
		const char* fontFamilyName = "arial";
		const char* fontLocaleName = "";
		FontWeight fontWeight = FontWeight::Medium;
		FontStyle fontStyle = FontStyle::Normal;
		FontStretch fontStretch = FontStretch::Medium;
		float fontSize = 11.f;
	};

	/// <summary>
	/// An abstract drawing context, created by a GUI to be used 
	/// when drawing shapes and stuff in your (and my) views.
	/// </summary>
	class DrawingContext : public ReferenceCounted
	{
	protected:
		TextProperties m_textProperties;

	public:
		/// <summary>
		/// <para>Initializes drawing.</para>
		/// <para>The GUI calls this for you.</para>
		/// </summary>
		virtual void beginDrawing() = 0;
		/// <summary>
		/// <para>Finishes the drawing and shows it.</para>
		/// <para>The GUI calls this for you.</para>
		/// </summary>
		virtual void finishDrawing(const Rectangle<float>& p_updatedRectangle) = 0;

		//------------------------------

		/// <summary>
		/// <para>Moves the screen position of the coordinate (0, 0).</para>
		/// </summary>
		virtual void moveOrigin(const Point<float>& p_offset) = 0;
		/// <summary>
		/// <para>Moves the screen position of the coordinate (0, 0).</para>
		/// </summary>
		virtual void moveOrigin(float p_offsetX, float p_offsetY) = 0;
		/// <summary>
		/// <para>Sets the screen position of the coordinate (0, 0).</para>
		/// </summary>
		virtual void setOrigin(const Point<float>& p_origin) = 0;
		/// <summary>
		/// <para>Sets the screen position of the coordinate (0, 0).</para>
		/// </summary>
		virtual void setOrigin(float p_x, float p_y) = 0;
		/// <summary>
		/// <para>Returns the screen position of the coordinate (0, 0).</para>
		/// </summary>
		virtual Point<float> getOrigin() = 0;

		//------------------------------

		/// <summary>
		/// <para>Resizes the drawing buffers.</para>
		/// <para>The GUI calls this for you when it is being resized.</para>
		/// </summary>
		virtual void setSize(const Point<uint32_t>& p_size) = 0;
		/// <summary>
		/// <para>Resizes the drawing buffers.</para>
		/// <para>The GUI calls this for you when it is being resized.</para>
		/// </summary>
		virtual void setSize(uint32_t p_width, uint32_t p_height) = 0;

		//------------------------------

		/// <summary>
		/// Clears the whole buffer with the specified color.
		/// </summary>
		virtual void clear(const Color& p_color) = 0;
		/// <summary>
		/// Clears the whole buffer with a transparent background.
		/// </summary>
		virtual void clear() = 0;

		//------------------------------

		/// <summary>
		/// Draws a filled rectangle onto the screen using the current color. Change color with the method setColor.
		/// </summary>
		virtual void fillRectangle(const Rectangle<float>& p_rectangle) = 0;
		/// <summary>
		/// Draws a filled rectangle onto the screen using the current color. Change color with the method setColor.
		/// </summary>
		virtual void fillRectangle(const Point<float>& p_position, const Point<float>& p_size) = 0;
		/// <summary>
		/// Draws a filled rectangle onto the screen using the current color. Change color with the method setColor.
		/// </summary>
		virtual void fillRectangle(float p_left, float p_top, float p_right, float p_bottom) = 0;

		/// <summary>
		/// Draws a filled rectangle at the origin using the current color. Change color with the method setColor.
		/// </summary>
		virtual void fillRectangle(const Point<float>& p_size) = 0;
		/// <summary>
		/// Draws a filled rectangle at the origin using the current color. Change color with the method setColor.
		/// </summary>
		virtual void fillRectangle(float p_width, float p_height) = 0;

		//------------------------------

		/// <summary>
		/// Draws a rectangle outline using the current color. Change the color being used with the method setColor.
		/// </summary>
		/// <param name="p_strokeWidth">Width of the line</param>
		virtual void strokeRectangle(const Rectangle<float>& p_rectangle, float p_strokeWidth = 1.f) = 0;
		/// <summary>
		/// Draws a rectangle outline using the current color. Change the color being used with the method setColor.
		/// </summary>
		/// <param name="p_strokeWidth">Width of the line</param>
		virtual void strokeRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_strokeWidth = 1.f) = 0;
		/// <summary>
		/// Draws a rectangle outline using the current color. Change the color being used with the method setColor.
		/// </summary>
		/// <param name="p_strokeWidth">Width of the line</param>
		virtual void strokeRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_strokeWidth = 1.f) = 0;

		/// <summary>
		/// Draws a rectangle outline at the origin using the current color. Change the color being used with the method setColor.
		/// </summary>
		/// <param name="p_strokeWidth">Width of the line</param>
		virtual void strokeRectangle(const Point<float>& p_size, float p_strokeWidth = 1.f) = 0;
		/// <summary>
		/// Draws a rectangle outline at the origin using the current color. Change the color being used with the method setColor.
		/// </summary>
		/// <param name="p_strokeWidth">Width of the line</param>
		virtual void strokeRectangle(float p_width, float p_height, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		virtual void fillRoundedRectangle(const Rectangle<float>& p_rectangle, float p_radius) = 0;
		virtual void fillRoundedRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_radius) = 0;
		virtual void fillRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius) = 0;

		virtual void fillRoundedRectangle(const Point<float>& p_size, float p_radius) = 0;
		virtual void fillRoundedRectangle(float p_width, float p_height, float p_radius) = 0;

		//------------------------------

		virtual void strokeRoundedRectangle(const Rectangle<float>& p_rectangle, float p_radius, float p_strokeWidth = 1.f) = 0;
		virtual void strokeRoundedRectangle(const Point<float>& p_position, const Point<float>& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		virtual void strokeRoundedRectangle(float p_left, float p_top, float p_right, float p_bottom, float p_radius, float p_strokeWidth = 1.f) = 0;

		virtual void strokeRoundedRectangle(const Point<float>& p_size, float p_radius, float p_strokeWidth = 1.f) = 0;
		virtual void strokeRoundedRectangle(float p_width, float p_height, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		virtual void fillCircle(const Point<float>& p_position, float p_radius) = 0;
		virtual void fillCircle(float p_x, float p_y, float p_radius) = 0;

		virtual void strokeCircle(const Point<float>& p_position, float p_radius, float p_strokeWidth = 1.f) = 0;
		virtual void strokeCircle(float p_x, float p_y, float p_radius, float p_strokeWidth = 1.f) = 0;

		//------------------------------

		virtual void drawLine(const Point<float>& p_point_0, const Point<float>& p_point_1, float p_thickness = 1.f) = 0;
		virtual void drawLine(float p_x0, float p_y0, float p_x1, float p_y1, float p_thickness = 1.f) = 0;

		//------------------------------

		virtual void setLineCap(LineCap p_lineCap) = 0;
		virtual void setStartLineCap(LineCap p_lineCap) = 0;
		virtual void setEndLineCap(LineCap p_lineCap) = 0;
		virtual LineCap getStartLineCap() = 0;
		virtual LineCap getEndLineCap() = 0;

		//------------------------------

		virtual void setLineJoin(LineJoin p_lineJoin) = 0;
		virtual LineJoin getLineJoin() = 0;

		virtual void setLineJoinMiterLimit(float p_miterLimit) = 0;
		virtual float getLineJoinMiterLimit() = 0;

		//------------------------------

		virtual void pushClipRectangle(Rectangle<float> p_rectangle) = 0;
		virtual void popClipRectangle() = 0;

		//------------------------------

		virtual void pushRoundedClipRectangle(const Rectangle<float>& p_rectangle, float p_radius) = 0;
		virtual void popRoundedClipRectangle() = 0;

		//------------------------------

		virtual Image* createRectangleShadowImage(const Point<uint32_t>& p_size, float p_blur, const Color& p_color) = 0;
		virtual Image* createRectangleShadowImage(uint32_t p_width, uint32_t p_height, float p_blur, const Color& p_color) = 0;

		//------------------------------

		virtual Image* createRoundedRectangleShadowImage(const Point<uint32_t>& p_size, float p_radius, float p_blur, const Color& p_color) = 0;
		virtual Image* createRoundedRectangleShadowImage(uint32_t p_width, uint32_t p_height, float p_radius, float p_blur, const Color& p_color) = 0;

		//------------------------------

		virtual void drawImage(Image* p_image, const Point<float>& p_position, const Rectangle<float>& sourceRectangle, float p_scale = 1.f, float p_opacity = 1.f) = 0;
		virtual void drawImage(Image* p_image, const Point<float>& p_position, float p_scale = 1.f, float p_opacity = 1.f) = 0;

		//------------------------------

		/// <summary>
		/// Sets the color being used when drawing shapes.
		/// </summary>
		virtual void setColor(const Color& p_color) = 0;

		//------------------------------

		/// <summary>
		/// Adds a new font family that can be used by text.
		/// </summary>
		/// <param name="p_data">Font data, the data that is in a font file.</param>
		/// <param name="p_dataSize">The length/size of the data in bytes</param>
		virtual void addFont(const void* p_data, uint32_t p_dataSize) = 0;

		//------------------------------

		/// <summary>
		/// Sets the default properties of text created with this drawing context. These properties can be overridden by changing the properties of a text object.
		/// </summary>
		virtual void setDefaultTextProperties(const TextProperties& p_textProperties) = 0;
		/// <summary>
		/// Returns the default properties of text created with this drawing context. These properties can be overridden by changing the properties of a text object.
		/// </summary>
		virtual TextProperties getDefaultTextProperties() = 0;

		//------------------------------

		/// <summary>
		/// Creates a new Text object which represents a pre-calculated text layout.
		/// </summary>
		/// <param name="p_string">The content of the text.</param>
		/// <param name="p_fontSize">The font size of the text.</param>
		/// <param name="p_bounds">The maximum bounds of the text.</param>
		virtual Text* createText(const char* p_string, float p_fontSize, const Rectangle<float>& p_bounds = Rectangle<float>()) = 0;
		/// <summary>
		/// Draws pre-calculated text created with the createText method.
		/// </summary>
		virtual void drawText(Text* p_text) = 0;

		/// <summary>
		/// <para>Lays out and draws a string in a rectangle.</para>
		/// <para>If you're drawing the same text repeatedly, use a Text object (created with createText()).</para>
		/// </summary>
		virtual void drawText(const char* p_string, const Rectangle<float>& p_rectangle) = 0;
		/// <summary>
		/// <para>Lays out and draws a string in a rectangle.</para>
		/// <para>If you're drawing the same text repeatedly, use a Text object (created with createText()).</para>
		/// </summary>
		virtual void drawText(const char* p_string, float p_left, float p_top, float p_right, float p_bottom) = 0;
		/// <summary>
		/// <para>Lays out and draws a string in a rectangle.</para>
		/// <para>If you're drawing the same text repeatedly, use a Text object (created with createText()).</para>
		/// </summary>
		virtual void drawText(const char* p_string, const Point<float>& p_position, const Point<float>& p_size) = 0;
		/// <summary>
		/// <para>Lays out and draws a string in a rectangle.</para>
		/// <para>If you're drawing the same text repeatedly, use a Text object (created with createText()).</para>
		/// </summary>
		virtual void drawText(const char* p_string, float p_x, float p_y) = 0;
		/// <summary>
		/// <para>Lays out and draws a string in a rectangle.</para>
		/// <para>If you're drawing the same text repeatedly, use a Text object (created with createText()).</para>
		/// </summary>
		virtual void drawText(const char* p_string, const Point<float>& p_position) = 0;
	};
#pragma endregion

	//------------------------------

	/// <summary>
	/// <para>The highest view in the view hierarchy.</para>
	/// <para>Holds a window and recieves events from it.</para>
	/// </summary>
	class GUI : public ViewContainer, public WindowEventListener
	{
	private:
		Window* m_window;
		DrawingContext* m_drawingContext;

		std::vector<WindowEventListener*> m_windowEventListeners;

		//------------------------------

		std::vector<MouseEventListener*> m_mouseEventListeners;
		std::vector<MouseEventListener*> m_pressedMouseEventListeners;

		std::vector<MouseEventListener*> getTopMouseListenersAt(const Point<float>& p_coordinates);
		std::vector<MouseEventListener*> getTopMouseListenersAt(float p_x, float p_y);

		//------------------------------

		std::vector<KeyboardEventListener*> m_keyboardEventListeners;
		KeyboardEventListener* m_keyboardFocus;

		//------------------------------

		bool m_areIndirectMouseEventsEnabled;
		bool m_areIndirectKeyboardEventsEnabled;

	public:
		GUI();
		~GUI();

		/// <summary>
		/// <para>This method creates the window and drawing context as well as</para>
		/// <para>creates the content of the GUI and lays it out.</para>
		/// </summary>
		/// <param name="p_title">The name of the window.</param>
		/// <param name="p_x">X coordinate of the window in desktop coordinates.</param>
		/// <param name="p_y">Y coordinate of the window in desktop coordinates.</param>
		/// <param name="p_width">Width of the window.</param>
		/// <param name="p_height">Height of the window.</param>
		/// <param name="p_windowFlags">Flags which change how the window appears.</param>
		/// <param name="p_isFullscreen">If this is true, no window will be shown and graphics will be drawn in fullscreen.</param>
		/// <param name="p_parent">Parent GUI, is only used if the Child flag is turned on in p_windowFlags.</param>
		void create(const char* p_title, uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, GUI* p_parent = 0);
		/// <summary>
		/// <para>This method creates the window and drawing context as well as</para>
		/// <para>creates the content of the GUI and lays it out.</para>
		/// </summary>
		/// <param name="p_title">The name of the window.</param>
		/// <param name="p_width">Width of the window.</param>
		/// <param name="p_height">Height of the window.</param>
		/// <param name="p_windowFlags">Flags which change how the window appears.</param>
		/// <param name="p_isFullscreen">If this is true, no window will be shown and graphics will be drawn in fullscreen.</param>
		/// <param name="p_parent">Parent GUI, is only used if the Child flag is turned on in p_windowFlags.</param>
		void create(const char* p_title, uint32_t p_width, uint32_t p_height, WindowStyleFlags p_windowFlags = WindowStyleFlags::Default, bool p_isFullscreen = false, GUI* p_parent = 0);

		//------------------------------

		/// <summary>
		/// Returns the topmost view which contains the coordinates given.
		/// </summary>
		View* getViewAt(const Point<float>& p_coordinates);
		/// <summary>
		/// Returns the topmost view which contains the coordinates given.
		/// </summary>
		View* getViewAt(float p_x, float p_y);

		//------------------------------

		void handleWindowCreated(const WindowEvent& p_event) override;
		void handleWindowSizeChanged(const WindowEvent& p_event) override;

		//------------------------------

		/// <summary>
		/// <para>Mouse down event that has been sent directly from the window to the GUI.</para>
		/// <para>This sends the event down to all event listeners and lets them know if the event is directed to them.</para>
		/// <para>Make the GUI an event listener and ignore the p_isMine parameter in the handler if you want to</para>
		/// <para>handle all mouse events.</para>
		/// </summary>
		void handleMouseDown(const MouseEvent& p_event);
		/// <summary>
		/// <para>Mouse up event that has been sent directly from the window to the GUI.</para>
		/// <para>This sends the event down to all event listeners and lets them know if the event is directed to them.</para>
		/// <para>Make the GUI an event listener and ignore the p_isMine parameter in the handler if you want to</para>
		/// <para>handle all mouse events.</para>
		/// </summary>
		void handleMouseUp(const MouseEvent& p_event);
		/// <summary>
		/// <para>Double click event that has been sent directly from the window to the GUI.</para>
		/// <para>This sends the event down to all event listeners and lets them know if the event is directed to them.</para>
		/// <para>Make the GUI an event listener and ignore the p_isMine parameter in the handler if you want to</para>
		/// <para>handle all mouse events.</para>
		/// </summary>
		void handleMouseDoubleClick(const MouseEvent& p_event);

		/// <summary>
		/// <para>Mouse move event that has been sent directly from the window to the GUI.</para>
		/// <para>This sends the event down to all event listeners and lets them know if the event is directed to them.</para>
		/// <para>Make the GUI an event listener and ignore the p_isMine parameter in the handler if you want to</para>
		/// <para>handle all mouse events.</para>
		/// </summary>
		void handleMouseMove(const MouseEvent& p_event);
		/// <summary>
		/// <para>Mouse scroll event that has been sent directly from the window to the GUI.</para>
		/// <para>This sends the event down to all event listeners and lets them know if the event is directed to them.</para>
		/// <para>Make the GUI an event listener and ignore the p_isMine parameter in the handler if you want to</para>
		/// <para>handle all mouse events.</para>
		/// </summary>
		void handleMouseScrolled(const MouseEvent& p_event);

		//------------------------------

		/// <summary>
		/// <para>Sets the keyboard event listener that keyboard events are directed to.</para>
		/// <para>If indirect keyboard events are enabled, listeners that do not have keyboard focus will also receive</para>
		/// <para>the events, but the isTarget property of the keyboard event will tell you if it does have keyboard focus.</para>
		/// </summary>
		inline void setKeyboardFocus(KeyboardEventListener* p_keyboardFocus)
		{
			m_keyboardFocus = p_keyboardFocus;
		}

		void handleCharacterPressed(const KeyboardEvent& p_event) { }
		void handleKeyboardKeyPressed(const KeyboardEvent& p_event) { }
		void handleKeyboardKeyReleased(const KeyboardEvent& p_event) { }

		//------------------------------

		/// <summary>
		/// Adds an event listener that will recieve events about the window.
		/// </summary>
		void addWindowEventListener(WindowEventListener* p_listener);
		/// <summary>
		/// Adds an event listener that will recieve mouse events.
		/// </summary>
		void addMouseEventListener(MouseEventListener* p_listener);
		/// <summary>
		/// Adds an event listener that will recieve keyboard events.
		/// </summary>
		void addKeyboardEventListener(KeyboardEventListener* p_listener);

		//------------------------------

		/// <summary>
		/// <para>After this method has been called, mouse events will not only be sent to the view</para>
		/// <para>that has been clicked or scrolled over, but to all of the registered mouse event</para>
		/// <para>listeners. The isTarget property in the mouse event tells you if the event is </para>
		/// <para>directed to that view.</para>
		/// </summary>
		inline void enableIndirectMouseEvents()
		{
			m_areIndirectMouseEventsEnabled = true;
		}
		/// <summary>
		/// <para>After this method has been called, mouse events will only be sent to the view</para>
		/// <para>that has been clicked or scrolled over, and no other mouse event listeners.</para>
		/// </summary>
		inline void disableIndirectMouseEvents()
		{
			m_areIndirectMouseEventsEnabled = false;
		}

		/// <summary>
		/// <para>After this method has been called, keyboard events will not only be sent to the view</para>
		/// <para>with keyboard focus, but to all of the registered keyboard listeners. The isTarget property</para>
		/// <para>in the keyboard event tells you if the event is directed to that view.</para>
		/// </summary>
		inline void enableIndirectKeyboardEvents()
		{
			m_areIndirectKeyboardEventsEnabled = true;
		}
		/// <summary>
		/// <para>After this method has been called, keyboard events will only be sent to the view</para>
		/// <para>with keyboard focus, and no other keyboard event listeners.</para>
		/// </summary>
		inline void disableIndirectKeyboardEvents()
		{
			m_areIndirectKeyboardEventsEnabled = false;
		}

		//------------------------------

		/// <summary>
		/// Returns a pointer to the window used by this GUI.
		/// </summary>
		inline Window* getWindow()
		{
			return m_window;
		}
		/// <summary>
		/// Returns a pointer to the drawing context used by this GUI.
		/// </summary>
		inline DrawingContext* getDrawingContext()
		{
			return m_drawingContext;
		}

		//------------------------------

		/// <summary>
		/// <para>This is called after the window and drawing context have been created.</para>
		/// <para>You should initialize your GUI in this method.</para>
		/// </summary>
		virtual void createContent() { };

		//------------------------------

		void queueAnimationUpdateForView(View* p_view);

		//------------------------------

		/// <summary>
		/// Invalidates a region of the GUI that has been changed, and therefore needs to be redrawn.
		/// </summary>
		void invalidateRect(const Rectangle<float>& p_rectangle);

		/// <summary>
		/// <para>Draws a portion of the GUI.</para>
		/// <para>This method should only be called by a window, you don't need to care about it.</para>
		/// </summary>
		/// <param name="p_drawingContext">Object used for drawing to the window.</param>
		/// <param name="p_rectangle">Region that needs to be redrawn.</param>
		void draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_rectangle);

		//------------------------------

		/// <summary>
		/// Runs the global OS window event loop.
		/// </summary>
		static void run();
	};

	//------------------------------

	class Ripple : 
		public View, public ViewEventListener, 
		public MouseEventListener
	{
	private:
		Easing m_alphaEasing;
		Color m_color;

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
		Ripple(ViewContainer* p_parent, const Color& p_color = Color(1.f, 0.45f));
		virtual ~Ripple();

		//------------------------------

		/// <summary>
		/// Sets the color that is used by the ripple and hover effects.
		/// </summary>
		inline void setColor(const Color& p_color)
		{
			m_color = p_color;
		}
		/// <summary>
		/// Returns the color that is used by the ripple and hover effects.
		/// </summary>
		inline const Color& getColor()
		{
			return m_color;
		}

		//------------------------------

		/// <summary>
		/// <para>Sets whether the view will be lightly highlighted when the mouse hovers over it or not.</para>
		/// <para>This is true by default and is recommended since it indicates that the view can be pressed.</para>
		/// </summary>
		inline void setHasHoverEffect(bool p_hasHoverEffect)
		{
			m_hasHoverEffect = p_hasHoverEffect;
		}
		/// <summary>
		/// Returns whether the view will be lightly highlighted when the mouse hovers over it or not.
		/// </summary>
		inline bool getHasHoverEffect()
		{
			return m_hasHoverEffect;
		}

		//------------------------------

		void handleViewSizeChange(View* p_view) override;

		void handleMouseDown(const MouseEvent& p_event) override;
		void handleMouseUp(const MouseEvent& p_event) override;
		void handleMouseEnter(const MouseEvent& p_event) override;
		void handleMouseLeave(const MouseEvent& p_event) override;

		void updateAnimations() override;

		//------------------------------

		void draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override;
	};

	//------------------------------

	class Button;

	class ButtonListener
	{
	public:
		virtual void handleButtonClick(Button* p_button) { };
	};

	//------------------------------

	class Button : public ViewContainer, public MouseEventListener
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
		float m_fontSize;

		float m_animationTime;
		bool m_isPressed;

		bool m_isRaising;

		Emphasis m_emphasis;

		std::vector<ButtonListener*> m_buttonListeners;

	public:
		Button(ViewContainer* p_parent, const char* p_text = "", Emphasis p_emphasis = Emphasis::High, float p_x = 0.f, float p_y = 0.f);
		virtual ~Button();

		//------------------------------

		void addButtonListener(ButtonListener* p_buttonListener);

		//------------------------------

		void handleMouseDown(const MouseEvent& p_event) override;
		void handleMouseUp(const MouseEvent& p_event) override;

		//------------------------------

		void updateAnimations() override;

		//------------------------------

		/// <summary>
		/// Sets the text that the button displays.
		/// </summary>
		void setText(const char* p_text);
		/// <summary>
		/// Returns the text that the button displays.
		/// </summary>
		const char* getText();

		//------------------------------

		void drawUnclipped(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override;

		void draw(DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override;
	};

	//------------------------------
	// Font data

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
		0,14,0,42,0,-50,0,3,0,1,4,9,0,0,0,94,0,-8,0,3,0,1,4,9,0,1,0,12,1,86,0,3,0,1,4,9,0,2,0,14,1,98,0,3,0,1,4,9,0,3,0,12,1,86,0,3,0,1,4,9,0,4,0,12,1,86,0,3,0,1,4,9,0,5,0,38,1,112,0,3,0,1,4,9,0,6,0,28,1,-106,
		0,3,0,1,4,9,0,7,0,64,1,-78,0,3,0,1,4,9,0,9,0,12,1,-14,0,3,0,1,4,9,0,11,0,20,1,-2,0,3,0,1,4,9,0,12,0,38,2,18,0,3,0,1,4,9,0,13,0,92,2,56,0,3,0,1,4,9,0,14,0,84,2,-108,67,111,112,121,114,105,103,104,116,32,50,48,49,49,32,71,111,111,
		103,108,101,32,73,110,99,46,32,65,108,108,32,82,105,103,104,116,115,32,82,101,115,101,114,118,101,100,46,82,111,98,111,116,111,82,101,103,117,108,97,114,86,101,114,115,105,111,110,32,50,46,49,51,55,59,32,50,48,49,55,82,111,98,111,116,111,45,82,101,103,117,108,97,114,82,111,98,111,116,111,32,105,115,32,97,32,116,114,97,
		100,101,109,97,114,107,32,111,102,32,71,111,111,103,108,101,46,71,111,111,103,108,101,71,111,111,103,108,101,46,99,111,109,67,104,114,105,115,116,105,97,110,32,82,111,98,101,114,116,115,111,110,76,105,99,101,110,115,101,100,32,117,110,100,101,114,32,116,104,101,32,65,112,97,99,104,101,32,76,105,99,101,110,115,101,44,32,86,101,114,
		115,105,111,110,32,50,46,48,104,116,116,112,58,47,47,119,119,119,46,97,112,97,99,104,101,46,111,114,103,47,108,105,99,101,110,115,101,115,47,76,73,67,69,78,83,69,45,50,46,48,0,67,0,111,0,112,0,121,0,114,0,105,0,103,0,104,0,116,0,32,0,50,0,48,0,49,0,49,0,32,0,71,0,111,0,111,0,103,0,108,
		0,101,0,32,0,73,0,110,0,99,0,46,0,32,0,65,0,108,0,108,0,32,0,82,0,105,0,103,0,104,0,116,0,115,0,32,0,82,0,101,0,115,0,101,0,114,0,118,0,101,0,100,0,46,0,82,0,111,0,98,0,111,0,116,0,111,0,82,0,101,0,103,0,117,0,108,0,97,0,114,0,86,0,101,0,114,0,115,0,105,
		0,111,0,110,0,32,0,50,0,46,0,49,0,51,0,55,0,59,0,32,0,50,0,48,0,49,0,55,0,82,0,111,0,98,0,111,0,116,0,111,0,45,0,82,0,101,0,103,0,117,0,108,0,97,0,114,0,82,0,111,0,98,0,111,0,116,0,111,0,32,0,105,0,115,0,32,0,97,0,32,0,116,0,114,0,97,0,100,0,101,
		0,109,0,97,0,114,0,107,0,32,0,111,0,102,0,32,0,71,0,111,0,111,0,103,0,108,0,101,0,46,0,71,0,111,0,111,0,103,0,108,0,101,0,71,0,111,0,111,0,103,0,108,0,101,0,46,0,99,0,111,0,109,0,67,0,104,0,114,0,105,0,115,0,116,0,105,0,97,0,110,0,32,0,82,0,111,0,98,0,101,
		0,114,0,116,0,115,0,111,0,110,0,76,0,105,0,99,0,101,0,110,0,115,0,101,0,100,0,32,0,117,0,110,0,100,0,101,0,114,0,32,0,116,0,104,0,101,0,32,0,65,0,112,0,97,0,99,0,104,0,101,0,32,0,76,0,105,0,99,0,101,0,110,0,115,0,101,0,44,0,32,0,86,0,101,0,114,0,115,0,105,
		0,111,0,110,0,32,0,50,0,46,0,48,0,104,0,116,0,116,0,112,0,58,0,47,0,47,0,119,0,119,0,119,0,46,0,97,0,112,0,97,0,99,0,104,0,101,0,46,0,111,0,114,0,103,0,47,0,108,0,105,0,99,0,101,0,110,0,115,0,101,0,115,0,47,0,76,0,73,0,67,0,69,0,78,0,83,0,69,0,45,
		0,50,0,46,0,48,0,0,0,3,0,0,0,0,0,0,-1,106,0,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,8,0,2,-1,-1,0,15,0,1,0,2,0,14,0,0,0,0,0,0,2,40,0,2,0,89,0,37,0,62,0,1,0,69,0,94,0,1,0,121,0,121,0,1,0,-127,
		0,-127,0,1,0,-125,0,-125,0,1,0,-122,0,-122,0,1,0,-119,0,-119,0,1,0,-117,0,-106,0,1,0,-104,0,-99,0,1,0,-92,0,-92,0,1,0,-88,0,-83,0,3,0,-79,0,-79,0,1,0,-70,0,-69,0,1,0,-65,0,-65,0,1,0,-63,0,-63,0,1,0,-61,0,-61,0,1,0,-57,0,-57,0,1,0,-53,0,-53,0,1,0,-51,
		0,-50,0,1,0,-48,0,-47,0,1,0,-45,0,-45,0,1,0,-38,0,-34,0,1,0,-31,0,-31,0,1,0,-27,0,-27,0,1,0,-25,0,-23,0,1,0,-21,0,-5,0,1,0,-3,0,-3,0,1,0,-1,1,1,0,1,1,3,1,3,0,1,1,8,1,9,0,1,1,22,1,26,0,1,1,28,1,28,0,1,1,32,1,34,0,1,1,36,
		1,37,0,3,1,42,1,43,0,1,1,51,1,52,0,1,1,54,1,54,0,1,1,59,1,60,0,1,1,65,1,68,0,1,1,71,1,72,0,1,1,75,1,77,0,1,1,81,1,81,0,1,1,84,1,88,0,1,1,93,1,94,0,1,1,98,1,98,0,1,1,100,1,100,0,1,1,104,1,104,0,1,1,106,1,108,0,1,1,110,
		1,110,0,1,1,112,1,112,0,1,1,-70,1,-70,0,3,1,-69,1,-63,0,2,1,-46,1,-26,0,1,1,-22,1,-22,0,1,1,-13,1,-13,0,1,1,-11,1,-11,0,1,1,-4,1,-2,0,1,2,0,2,1,0,1,2,3,2,3,0,1,2,7,2,7,0,1,2,9,2,11,0,1,2,17,2,17,0,1,2,22,2,24,0,1,2,26,
		2,26,0,1,2,40,2,40,0,1,2,43,2,43,0,1,2,45,2,45,0,1,2,48,2,51,0,1,2,95,2,99,0,1,2,122,2,-30,0,1,2,-27,3,-117,0,1,3,-115,3,-92,0,1,3,-90,3,-78,0,1,3,-76,3,-67,0,1,3,-65,3,-38,0,1,3,-34,3,-34,0,1,3,-32,3,-25,0,1,3,-23,3,-21,0,1,3,-18,
		3,-14,0,1,3,-12,4,124,0,1,4,127,4,127,0,1,4,-126,4,-125,0,1,4,-123,4,-122,0,1,4,-120,4,-117,0,1,4,-107,4,-48,0,1,4,-46,4,-15,0,1,4,-13,4,-6,0,1,4,-4,4,-3,0,1,5,7,5,13,0,1,0,1,0,2,0,0,0,12,0,0,0,44,0,1,0,14,0,-88,0,-88,0,-87,0,-87,0,-86,
		0,-86,0,-85,0,-85,0,-84,0,-84,1,36,1,37,1,38,1,39,0,1,0,5,0,121,0,-92,0,-83,0,-83,1,-70,0,0,0,1,0,0,0,10,0,50,0,76,0,4,68,70,76,84,0,26,99,121,114,108,0,26,103,114,101,107,0,26,108,97,116,110,0,26,0,4,0,0,0,0,-1,-1,0,2,0,0,0,1,0,2,99,112,115,112,
		0,14,107,101,114,110,0,20,0,0,0,1,0,0,0,0,0,1,0,1,0,2,0,6,2,16,0,1,0,0,0,1,0,8,0,1,0,10,0,5,0,36,0,72,0,1,0,-6,0,8,0,10,0,20,0,21,0,22,0,23,0,24,0,25,0,26,0,27,0,28,0,29,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,
		0,46,0,47,0,48,0,49,0,50,0,51,0,52,0,53,0,54,0,55,0,56,0,57,0,58,0,59,0,60,0,61,0,62,0,101,0,103,0,-127,0,-125,0,-124,0,-116,0,-113,0,-111,0,-109,0,-79,0,-78,0,-77,0,-76,0,-75,0,-74,0,-73,0,-72,0,-71,0,-70,0,-46,0,-45,0,-44,0,-43,0,-42,0,-41,0,-40,0,-39,0,-38,
		0,-37,0,-36,0,-35,0,-34,0,-33,0,-32,0,-31,0,-30,0,-29,0,-28,0,-27,0,-26,0,-25,0,-24,0,-23,1,47,1,51,1,53,1,55,1,57,1,59,1,65,1,67,1,69,1,73,1,75,1,76,1,88,1,89,1,-105,1,-99,1,-94,1,-91,2,122,2,123,2,125,2,127,2,-128,2,-127,2,-126,2,-125,2,-124,2,-123,2,-122,2,-121,
		2,-120,2,-119,2,-118,2,-117,2,-116,2,-115,2,-114,2,-113,2,-112,2,-111,2,-110,2,-109,2,-108,2,-107,2,-106,2,-105,2,-104,2,-103,2,-74,2,-72,2,-70,2,-68,2,-66,2,-64,2,-62,2,-60,2,-58,2,-56,2,-54,2,-52,2,-50,2,-48,2,-46,2,-44,2,-42,2,-40,2,-38,2,-36,2,-34,2,-32,2,-30,2,-29,2,-27,2,-25,2,-23,
		2,-21,2,-19,2,-17,2,-15,2,-13,2,-11,2,-8,2,-6,2,-4,2,-2,3,0,3,2,3,4,3,6,3,8,3,10,3,12,3,14,3,16,3,18,3,20,3,22,3,24,3,26,3,28,3,30,3,32,3,34,3,36,3,37,3,39,3,41,3,43,3,45,3,-122,3,-121,3,-120,3,-119,3,-118,3,-117,3,-116,3,-114,3,-113,3,-112,3,-111,
		3,-110,3,-109,3,-108,3,-107,3,-106,3,-105,3,-104,3,-103,3,-102,3,-101,3,-100,3,-99,3,-83,3,-82,3,-81,3,-80,3,-79,3,-78,3,-77,3,-76,3,-75,3,-74,3,-73,3,-72,3,-71,3,-70,3,-69,3,-68,3,-67,3,-66,3,-65,3,-64,3,-63,3,-62,3,-45,3,-43,3,-41,3,-39,3,-18,3,-16,3,-14,4,7,4,13,4,19,4,125,
		4,-126,4,-122,5,7,5,9,0,2,0,0,0,2,0,10,58,24,0,1,3,-14,0,4,0,0,1,-12,7,-50,52,-58,52,-58,7,-4,8,94,54,-2,55,-82,52,-52,57,-52,55,122,8,100,56,24,56,24,55,-72,56,2,56,24,56,24,57,-52,56,68,12,2,12,-48,56,-118,57,88,57,-108,52,-34,54,-124,57,-78,13,70,55,92,56,102,53,-116,
		13,-116,56,58,14,-62,56,58,56,58,55,-120,56,102,56,124,15,-60,57,118,16,38,53,60,57,118,16,64,56,102,57,-52,16,-122,53,-58,54,-2,57,-52,54,-2,17,8,18,6,19,8,19,-22,20,-116,57,118,20,-110,20,-100,56,58,23,-122,25,120,26,106,27,112,27,-122,27,-116,27,-110,30,-116,30,-110,30,-52,31,2,31,-116,53,-96,53,-96,33,-66,
		56,24,34,96,35,94,52,-34,37,-64,56,24,56,24,53,66,56,24,56,24,56,24,38,-106,53,-96,56,24,53,-96,40,64,41,6,41,-104,41,-6,42,-32,53,-106,43,110,53,60,51,70,43,-104,45,114,56,102,49,0,49,58,51,36,51,36,56,102,50,112,50,-6,51,36,51,36,51,36,54,-2,55,-120,57,88,57,118,51,70,56,102,53,-58,53,-106,
		52,-34,53,60,55,-72,55,-72,55,-72,56,24,52,-34,53,60,56,24,56,24,57,-52,53,-106,52,-34,53,60,52,-58,51,112,52,-58,52,-58,52,-58,58,8,52,18,52,96,58,2,52,-68,57,-22,57,-16,58,2,57,-16,57,-22,57,-22,57,-22,57,-22,52,-82,57,-16,52,-52,57,-52,57,-52,57,-52,57,-52,56,-118,54,-2,54,-2,54,-2,54,-2,54,-2,
		54,-2,54,-2,52,-52,55,122,55,122,55,122,55,122,56,24,56,24,56,24,56,24,56,24,57,-52,57,-52,57,-52,57,-52,57,-52,54,-124,55,92,55,92,55,92,55,92,55,92,55,92,55,92,53,-116,53,-116,53,-116,53,-116,56,58,55,-120,55,-120,55,-120,55,-120,55,-120,57,118,57,118,54,-2,55,92,54,-2,55,92,54,-2,55,92,52,-52,52,-52,
		52,-52,52,-52,57,-52,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,56,24,56,58,56,24,56,24,56,24,56,24,56,24,55,-72,56,2,56,2,56,2,56,2,56,24,56,58,56,24,56,58,56,24,56,58,56,58,57,-52,55,-120,57,-52,55,-120,57,-52,55,-120,56,124,56,124,56,124,56,-118,56,-118,56,-118,57,-108,
		54,-124,57,118,54,-124,57,-78,57,-78,57,-78,58,2,58,2,58,8,57,-16,57,-16,57,-16,57,-16,57,-16,57,-16,57,-16,58,2,58,2,58,2,58,2,58,2,57,-16,57,-16,57,-16,58,2,57,-22,52,-68,52,-68,52,-68,52,-68,58,2,58,2,58,2,58,8,54,-2,55,122,56,24,56,24,57,-52,54,-124,54,-2,55,-82,55,122,57,-78,56,24,
		56,24,55,-72,56,24,56,24,57,-52,56,68,56,-118,54,-124,52,-34,56,24,54,-124,56,58,55,-120,57,118,55,-120,55,122,53,-58,56,24,56,24,55,-72,55,-72,53,66,54,-2,55,-82,53,-58,55,122,56,24,56,24,57,-52,56,68,52,-52,56,-118,52,-34,55,92,53,-116,55,-120,56,102,57,118,53,60,53,-116,53,-106,57,118,57,-108,57,-108,57,-108,
		54,-124,57,118,52,-58,52,-58,52,-58,56,24,56,58,54,-2,55,92,55,122,53,-116,57,88,57,118,52,-52,54,-124,57,118,56,24,52,-34,53,60,56,24,54,-2,55,92,54,-2,55,92,55,122,53,-116,53,-116,53,-116,52,-34,53,60,57,-52,55,-120,55,-120,56,102,53,66,57,118,53,66,57,118,53,66,57,118,54,-2,55,92,54,-2,55,92,54,-2,
		55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,54,-2,55,92,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,55,122,53,-116,56,24,56,24,57,-52,55,-120,57,-52,55,-120,57,-52,55,-120,57,-52,55,-120,
		57,-52,55,-120,57,-52,55,-120,57,-52,55,-120,55,-120,54,-124,57,118,54,-124,57,118,54,-124,57,118,56,-118,53,-58,53,-106,56,58,53,-96,53,-58,55,-72,54,-124,56,24,56,58,54,-2,55,92,55,122,56,24,57,-52,55,-120,56,124,55,-82,56,102,57,-52,57,-52,56,24,56,58,55,-72,55,-72,56,2,56,24,56,58,56,24,56,58,57,-52,56,68,
		56,102,56,124,56,-118,57,88,57,118,57,88,57,118,57,-108,57,-78,57,-52,57,-16,58,2,57,-16,57,-22,58,8,57,-22,57,-16,58,2,58,8,0,2,0,-92,0,4,0,4,0,0,0,6,0,6,0,1,0,11,0,12,0,2,0,19,0,19,0,4,0,37,0,42,0,5,0,44,0,45,0,11,0,47,0,54,0,13,0,56,0,56,0,21,
		0,58,0,63,0,22,0,69,0,70,0,28,0,73,0,74,0,30,0,76,0,76,0,32,0,79,0,79,0,33,0,81,0,84,0,34,0,86,0,86,0,38,0,88,0,88,0,39,0,90,0,93,0,40,0,95,0,95,0,44,0,-118,0,-118,0,45,0,-106,0,-106,0,46,0,-99,0,-99,0,47,0,-79,0,-75,0,48,0,-73,0,-71,0,53,
		0,-69,0,-69,0,56,0,-67,0,-66,0,57,0,-64,0,-63,0,59,0,-61,0,-59,0,61,0,-57,0,-50,0,64,0,-46,0,-46,0,72,0,-44,0,-34,0,73,0,-32,0,-17,0,84,0,-15,0,-15,0,100,0,-10,0,-8,0,101,0,-5,0,-4,0,104,0,-2,1,0,0,106,1,3,1,5,0,109,1,10,1,10,0,112,1,13,1,13,0,113,
		1,24,1,26,0,114,1,34,1,34,0,117,1,46,1,48,0,118,1,51,1,53,0,121,1,55,1,55,0,124,1,57,1,57,0,125,1,59,1,59,0,126,1,67,1,68,0,127,1,84,1,84,0,-127,1,86,1,86,0,-126,1,88,1,88,0,-125,1,92,1,94,0,-124,1,-124,1,-123,0,-121,1,-121,1,-119,0,-119,1,-40,1,-40,0,-116,
		1,-38,1,-37,0,-115,1,-35,1,-35,0,-113,1,-32,1,-31,0,-112,1,-21,1,-19,0,-110,1,-1,1,-1,0,-107,2,14,2,16,0,-106,2,48,2,48,0,-103,2,51,2,51,0,-102,2,69,2,69,0,-101,2,71,2,72,0,-100,2,122,2,123,0,-98,2,125,2,125,0,-96,2,127,2,-108,0,-95,2,-103,2,-96,0,-73,2,-94,2,-91,0,-65,
		2,-86,2,-81,0,-61,2,-76,2,-68,0,-55,2,-66,2,-66,0,-46,2,-64,2,-64,0,-45,2,-62,2,-62,0,-44,2,-60,2,-60,0,-43,2,-58,2,-49,0,-42,2,-40,2,-38,0,-32,2,-36,2,-36,0,-29,2,-34,2,-34,0,-28,2,-32,2,-32,0,-27,2,-30,2,-30,0,-26,2,-25,2,-25,0,-25,2,-23,2,-23,0,-24,2,-21,2,-21,0,-23,
		2,-19,2,-19,0,-22,2,-17,2,-17,0,-21,2,-15,2,-3,0,-20,2,-1,2,-1,0,-7,3,1,3,1,0,-6,3,3,3,3,0,-5,3,14,3,14,0,-4,3,16,3,16,0,-3,3,18,3,18,0,-2,3,32,3,32,0,-1,3,34,3,37,1,0,3,39,3,39,1,4,3,41,3,41,1,5,3,47,3,56,1,6,3,67,3,71,1,16,
		3,77,3,79,1,21,3,84,3,84,1,24,3,101,3,105,1,25,3,109,3,111,1,30,3,120,3,120,1,33,3,-122,3,-117,1,34,3,-114,3,-99,1,40,3,-96,3,-96,1,56,3,-92,3,-92,1,57,3,-90,3,-90,1,58,3,-86,3,-86,1,59,3,-83,3,-82,1,60,3,-80,3,-79,1,62,3,-77,3,-71,1,64,3,-69,3,-67,1,71,
		3,-65,3,-60,1,74,3,-58,3,-57,1,80,3,-55,3,-52,1,82,3,-46,3,-45,1,86,3,-43,3,-43,1,88,3,-41,3,-41,1,89,3,-39,3,-36,1,90,3,-33,3,-28,1,94,3,-26,3,-26,1,100,3,-22,3,-21,1,101,3,-16,3,-16,1,103,3,-14,3,-5,1,104,3,-2,3,-1,1,114,4,1,4,4,1,116,4,11,4,12,1,120,
		4,16,4,16,1,122,4,18,4,24,1,123,4,30,4,70,1,-126,4,72,4,72,1,-85,4,74,4,87,1,-84,4,95,4,95,1,-70,4,112,4,117,1,-69,4,119,4,119,1,-63,4,123,4,124,1,-62,4,127,4,127,1,-60,4,-127,4,-126,1,-59,4,-124,4,-124,1,-57,4,-122,4,-122,1,-56,4,-105,4,-101,1,-55,4,-99,4,-99,1,-50,
		4,-97,4,-96,1,-49,4,-94,4,-94,1,-47,4,-90,4,-88,1,-46,4,-86,4,-86,1,-43,4,-84,4,-82,1,-42,4,-80,4,-80,1,-39,4,-78,4,-78,1,-38,4,-76,4,-70,1,-37,4,-68,4,-68,1,-30,4,-65,4,-65,1,-29,4,-62,4,-58,1,-28,4,-56,4,-56,1,-23,4,-54,4,-53,1,-22,4,-49,4,-49,1,-20,4,-46,4,-46,1,-19,
		4,-40,4,-40,1,-18,4,-35,4,-35,1,-17,4,-24,4,-24,1,-16,4,-22,4,-22,1,-15,4,-15,4,-15,1,-14,4,-11,4,-11,1,-13,0,11,0,56,-1,-40,0,-46,-1,-40,0,-42,-1,-40,1,57,-1,-40,1,69,-1,-40,3,14,-1,-40,3,16,-1,-40,3,18,-1,-40,3,-63,-1,-40,4,119,-1,-40,4,-65,-1,-40,0,24,0,58,0,20,0,59,
		0,18,0,61,0,22,1,25,0,20,2,-103,0,22,3,32,0,18,3,34,0,22,3,36,0,22,3,-117,0,22,3,-102,0,22,3,-99,0,22,3,-45,0,18,3,-43,0,18,3,-41,0,18,3,-39,0,22,3,-22,0,20,3,-14,0,22,4,112,0,22,4,114,0,22,4,116,0,22,4,-122,0,22,4,-62,0,20,4,-60,0,20,4,-58,0,18,
		0,1,0,19,-1,32,0,-25,0,16,-1,22,0,18,-1,22,0,37,-1,86,0,46,-2,-8,0,56,0,20,0,69,-1,-34,0,71,-1,-21,0,72,-1,-21,0,73,-1,-21,0,75,-1,-21,0,83,-1,-21,0,85,-1,-21,0,86,-1,-26,0,89,-1,-22,0,90,-1,-24,0,93,-1,-24,0,-108,-1,-21,0,-103,-1,-21,0,-101,-1,-22,0,-78,-1,86,0,-76,
		-1,86,0,-69,-1,-21,0,-67,-1,-24,0,-56,-1,-21,0,-55,-1,-21,0,-53,-1,-22,0,-46,0,20,0,-42,0,20,0,-9,-1,-21,1,3,-1,-21,1,13,-1,86,1,24,-1,-21,1,26,-1,-24,1,30,-1,-21,1,34,-1,-21,1,57,0,20,1,66,-1,-21,1,69,0,20,1,96,-1,-21,1,97,-1,-21,1,107,-1,-21,1,-122,-1,22,1,-118,-1,22,
		1,-114,-1,22,1,-113,-1,22,1,-21,-1,-64,1,-19,-1,-64,2,51,-1,-64,2,127,-1,86,2,-128,-1,86,2,-127,-1,86,2,-126,-1,86,2,-125,-1,86,2,-124,-1,86,2,-123,-1,86,2,-102,-1,-34,2,-101,-1,-34,2,-100,-1,-34,2,-99,-1,-34,2,-98,-1,-34,2,-97,-1,-34,2,-96,-1,-34,2,-95,-1,-21,2,-94,-1,-21,2,-93,-1,-21,2,-92,
		-1,-21,2,-91,-1,-21,2,-85,-1,-21,2,-84,-1,-21,2,-83,-1,-21,2,-82,-1,-21,2,-81,-1,-21,2,-80,-1,-22,2,-79,-1,-22,2,-78,-1,-22,2,-77,-1,-22,2,-76,-1,-24,2,-75,-1,-24,2,-74,-1,86,2,-73,-1,-34,2,-72,-1,86,2,-71,-1,-34,2,-70,-1,86,2,-69,-1,-34,2,-67,-1,-21,2,-65,-1,-21,2,-63,-1,-21,2,-61,-1,-21,
		2,-59,-1,-21,2,-57,-1,-21,2,-55,-1,-21,2,-53,-1,-21,2,-51,-1,-21,2,-49,-1,-21,2,-47,-1,-21,2,-45,-1,-21,2,-43,-1,-21,2,-41,-1,-21,2,-27,-2,-8,2,-7,-1,-21,2,-5,-1,-21,2,-3,-1,-21,3,14,0,20,3,16,0,20,3,18,0,20,3,21,-1,-22,3,23,-1,-22,3,25,-1,-22,3,27,-1,-22,3,29,-1,-22,3,31,
		-1,-22,3,35,-1,-24,3,50,-1,-64,3,51,-1,-64,3,52,-1,-64,3,53,-1,-64,3,54,-1,-64,3,55,-1,-64,3,56,-1,-64,3,77,-1,-64,3,78,-1,-64,3,79,-1,-64,3,-122,-1,86,3,-114,-1,86,3,-98,-1,-21,3,-94,-1,-22,3,-92,-1,-21,3,-90,-1,-24,3,-87,-1,-22,3,-86,-1,-21,3,-85,-1,-22,3,-78,-2,-8,3,-74,-1,86,
		3,-63,0,20,3,-61,-1,-34,3,-60,-1,-21,3,-58,-1,-21,3,-56,-1,-21,3,-55,-1,-24,3,-53,-1,-21,3,-46,-1,-24,3,-38,-1,-24,3,-30,-1,86,3,-29,-1,-34,3,-26,-1,-21,3,-21,-1,-24,3,-20,-1,-21,3,-15,-1,-21,3,-13,-1,-24,3,-8,-1,86,3,-7,-1,-34,3,-6,-1,86,3,-5,-1,-34,3,-1,-1,-21,4,1,-1,-21,4,2,
		-1,-21,4,12,-1,-21,4,14,-1,-21,4,16,-1,-21,4,20,-1,-24,4,22,-1,-24,4,24,-1,-24,4,29,-1,-21,4,30,-1,86,4,31,-1,-34,4,32,-1,86,4,33,-1,-34,4,34,-1,86,4,35,-1,-34,4,36,-1,86,4,37,-1,-34,4,38,-1,86,4,39,-1,-34,4,40,-1,86,4,41,-1,-34,4,42,-1,86,4,43,-1,-34,4,44,-1,86,
		4,45,-1,-34,4,46,-1,86,4,47,-1,-34,4,48,-1,86,4,49,-1,-34,4,50,-1,86,4,51,-1,-34,4,52,-1,86,4,53,-1,-34,4,55,-1,-21,4,57,-1,-21,4,59,-1,-21,4,61,-1,-21,4,63,-1,-21,4,65,-1,-21,4,67,-1,-21,4,69,-1,-21,4,75,-1,-21,4,77,-1,-21,4,79,-1,-21,4,81,-1,-21,4,83,-1,-21,4,85,
		-1,-21,4,87,-1,-21,4,89,-1,-21,4,91,-1,-21,4,93,-1,-21,4,95,-1,-21,4,97,-1,-21,4,99,-1,-22,4,101,-1,-22,4,103,-1,-22,4,105,-1,-22,4,107,-1,-22,4,109,-1,-22,4,111,-1,-22,4,113,-1,-24,4,115,-1,-24,4,117,-1,-24,4,119,0,20,4,-103,-1,86,4,-102,-1,-34,4,-100,-1,-21,4,-96,-1,-21,4,-92,-1,-22,
		4,-87,-1,-21,4,-85,-1,-21,4,-65,0,20,4,-61,-1,-24,4,-59,-1,-24,4,-53,-1,-64,4,-46,-1,-64,4,-22,-1,-64,0,51,0,56,-1,-43,0,58,-1,-28,0,59,-1,-20,0,61,-1,-35,0,-46,-1,-43,0,-42,-1,-43,1,25,-1,-28,1,57,-1,-43,1,69,-1,-43,1,-21,0,14,1,-19,0,14,2,51,0,14,2,-103,-1,-35,3,14,-1,-43,
		3,16,-1,-43,3,18,-1,-43,3,32,-1,-20,3,34,-1,-35,3,36,-1,-35,3,50,0,14,3,51,0,14,3,52,0,14,3,53,0,14,3,54,0,14,3,55,0,14,3,56,0,14,3,77,0,14,3,78,0,14,3,79,0,14,3,-117,-1,-35,3,-102,-1,-35,3,-99,-1,-35,3,-63,-1,-43,3,-45,-1,-20,3,-43,-1,-20,3,-41,-1,-20,3,-39,
		-1,-35,3,-22,-1,-28,3,-14,-1,-35,4,112,-1,-35,4,114,-1,-35,4,116,-1,-35,4,119,-1,-43,4,-122,-1,-35,4,-65,-1,-43,4,-62,-1,-28,4,-60,-1,-28,4,-58,-1,-20,4,-53,0,14,4,-46,0,14,4,-22,0,14,0,29,0,56,-1,-80,0,58,-1,-19,0,61,-1,-48,0,-46,-1,-80,0,-42,-1,-80,1,25,-1,-19,1,57,-1,-80,1,69,
		-1,-80,2,-103,-1,-48,3,14,-1,-80,3,16,-1,-80,3,18,-1,-80,3,34,-1,-48,3,36,-1,-48,3,-117,-1,-48,3,-102,-1,-48,3,-99,-1,-48,3,-63,-1,-80,3,-39,-1,-48,3,-22,-1,-19,3,-14,-1,-48,4,112,-1,-48,4,114,-1,-48,4,116,-1,-48,4,119,-1,-80,4,-122,-1,-48,4,-65,-1,-80,4,-62,-1,-19,4,-60,-1,-19,0,17,0,46,
		-1,-18,0,57,-1,-18,2,-107,-1,-18,2,-106,-1,-18,2,-105,-1,-18,2,-104,-1,-18,2,-27,-1,-18,3,20,-1,-18,3,22,-1,-18,3,24,-1,-18,3,26,-1,-18,3,28,-1,-18,3,30,-1,-18,3,-78,-1,-18,4,98,-1,-18,4,100,-1,-18,4,-63,-1,-18,0,77,0,6,0,16,0,11,0,16,0,13,0,20,0,65,0,18,0,71,-1,-24,0,72,
		-1,-24,0,73,-1,-24,0,75,-1,-24,0,85,-1,-24,0,97,0,19,0,-108,-1,-24,0,-103,-1,-24,0,-69,-1,-24,0,-56,-1,-24,0,-55,-1,-24,0,-9,-1,-24,1,3,-1,-24,1,30,-1,-24,1,34,-1,-24,1,66,-1,-24,1,96,-1,-24,1,97,-1,-24,1,107,-1,-24,1,-124,0,16,1,-123,0,16,1,-121,0,16,1,-120,0,16,1,-119,0,16,
		2,-95,-1,-24,2,-94,-1,-24,2,-93,-1,-24,2,-92,-1,-24,2,-91,-1,-24,2,-67,-1,-24,2,-65,-1,-24,2,-63,-1,-24,2,-61,-1,-24,2,-59,-1,-24,2,-57,-1,-24,2,-55,-1,-24,2,-53,-1,-24,2,-51,-1,-24,2,-49,-1,-24,2,-47,-1,-24,2,-45,-1,-24,2,-43,-1,-24,2,-41,-1,-24,3,-98,-1,-24,3,-60,-1,-24,3,-56,-1,-24,3,-53,
		-1,-24,3,-37,0,16,3,-36,0,16,3,-33,0,16,3,-26,-1,-24,3,-20,-1,-24,3,-15,-1,-24,3,-1,-1,-24,4,1,-1,-24,4,2,-1,-24,4,14,-1,-24,4,29,-1,-24,4,55,-1,-24,4,57,-1,-24,4,59,-1,-24,4,61,-1,-24,4,63,-1,-24,4,65,-1,-24,4,67,-1,-24,4,69,-1,-24,4,89,-1,-24,4,91,-1,-24,4,93,-1,-24,
		4,97,-1,-24,4,-100,-1,-24,4,-87,-1,-24,4,-85,-1,-24,0,64,0,71,-1,-20,0,72,-1,-20,0,73,-1,-20,0,75,-1,-20,0,85,-1,-20,0,-108,-1,-20,0,-103,-1,-20,0,-69,-1,-20,0,-56,-1,-20,0,-55,-1,-20,0,-9,-1,-20,1,3,-1,-20,1,30,-1,-20,1,34,-1,-20,1,66,-1,-20,1,96,-1,-20,1,97,-1,-20,1,107,-1,-20,
		2,-95,-1,-20,2,-94,-1,-20,2,-93,-1,-20,2,-92,-1,-20,2,-91,-1,-20,2,-67,-1,-20,2,-65,-1,-20,2,-63,-1,-20,2,-61,-1,-20,2,-59,-1,-20,2,-57,-1,-20,2,-55,-1,-20,2,-53,-1,-20,2,-51,-1,-20,2,-49,-1,-20,2,-47,-1,-20,2,-45,-1,-20,2,-43,-1,-20,2,-41,-1,-20,3,-98,-1,-20,3,-60,-1,-20,3,-56,-1,-20,3,-53,
		-1,-20,3,-26,-1,-20,3,-20,-1,-20,3,-15,-1,-20,3,-1,-1,-20,4,1,-1,-20,4,2,-1,-20,4,14,-1,-20,4,29,-1,-20,4,55,-1,-20,4,57,-1,-20,4,59,-1,-20,4,61,-1,-20,4,63,-1,-20,4,65,-1,-20,4,67,-1,-20,4,69,-1,-20,4,89,-1,-20,4,91,-1,-20,4,93,-1,-20,4,97,-1,-20,4,-100,-1,-20,4,-87,-1,-20,
		4,-85,-1,-20,0,24,0,83,-1,-20,1,24,-1,-20,2,-85,-1,-20,2,-84,-1,-20,2,-83,-1,-20,2,-82,-1,-20,2,-81,-1,-20,2,-7,-1,-20,2,-5,-1,-20,2,-3,-1,-20,3,-92,-1,-20,3,-86,-1,-20,3,-58,-1,-20,4,12,-1,-20,4,16,-1,-20,4,75,-1,-20,4,77,-1,-20,4,79,-1,-20,4,81,-1,-20,4,83,-1,-20,4,85,-1,-20,
		4,87,-1,-20,4,95,-1,-20,4,-96,-1,-20,0,6,0,16,-1,-124,0,18,-1,-124,1,-122,-1,-124,1,-118,-1,-124,1,-114,-1,-124,1,-113,-1,-124,0,17,0,46,-1,-20,0,57,-1,-20,2,-107,-1,-20,2,-106,-1,-20,2,-105,-1,-20,2,-104,-1,-20,2,-27,-1,-20,3,20,-1,-20,3,22,-1,-20,3,24,-1,-20,3,26,-1,-20,3,28,-1,-20,3,30,
		-1,-20,3,-78,-1,-20,4,98,-1,-20,4,100,-1,-20,4,-63,-1,-20,0,32,0,6,-1,-14,0,11,-1,-14,0,90,-1,-13,0,93,-1,-13,0,-67,-1,-13,0,-10,-1,-11,1,26,-1,-13,1,-124,-1,-14,1,-123,-1,-14,1,-121,-1,-14,1,-120,-1,-14,1,-119,-1,-14,2,-76,-1,-13,2,-75,-1,-13,3,35,-1,-13,3,-90,-1,-13,3,-55,-1,-13,3,-46,
		-1,-13,3,-38,-1,-13,3,-37,-1,-14,3,-36,-1,-14,3,-33,-1,-14,3,-21,-1,-13,3,-13,-1,-13,4,20,-1,-13,4,22,-1,-13,4,24,-1,-13,4,113,-1,-13,4,115,-1,-13,4,117,-1,-13,4,-61,-1,-13,4,-59,-1,-13,0,63,0,39,-1,-13,0,43,-1,-13,0,51,-1,-13,0,53,-1,-13,0,-125,-1,-13,0,-109,-1,-13,0,-104,-1,-13,0,-77,
		-1,-13,0,-60,0,13,0,-45,-1,-13,1,8,-1,-13,1,23,-1,-13,1,27,-1,-13,1,29,-1,-13,1,31,-1,-13,1,33,-1,-13,1,65,-1,-13,1,106,-1,-13,2,69,-1,-13,2,70,-1,-13,2,72,-1,-13,2,73,-1,-13,2,-122,-1,-13,2,-112,-1,-13,2,-111,-1,-13,2,-110,-1,-13,2,-109,-1,-13,2,-108,-1,-13,2,-68,-1,-13,2,-66,-1,-13,
		2,-64,-1,-13,2,-62,-1,-13,2,-48,-1,-13,2,-46,-1,-13,2,-44,-1,-13,2,-42,-1,-13,2,-8,-1,-13,2,-6,-1,-13,2,-4,-1,-13,3,45,-1,-13,3,-118,-1,-13,3,-105,-1,-13,3,-67,-1,-13,3,-64,-1,-13,3,-19,-1,-13,3,-16,-1,-13,4,11,-1,-13,4,13,-1,-13,4,15,-1,-13,4,74,-1,-13,4,76,-1,-13,4,78,-1,-13,4,80,
		-1,-13,4,82,-1,-13,4,84,-1,-13,4,86,-1,-13,4,88,-1,-13,4,90,-1,-13,4,92,-1,-13,4,94,-1,-13,4,96,-1,-13,4,-97,-1,-13,4,-72,-1,-13,0,64,0,39,-1,-26,0,43,-1,-26,0,51,-1,-26,0,53,-1,-26,0,-125,-1,-26,0,-109,-1,-26,0,-104,-1,-26,0,-77,-1,-26,0,-72,-1,-62,0,-60,0,16,0,-45,-1,-26,1,8,
		-1,-26,1,23,-1,-26,1,27,-1,-26,1,29,-1,-26,1,31,-1,-26,1,33,-1,-26,1,65,-1,-26,1,106,-1,-26,2,69,-1,-26,2,70,-1,-26,2,72,-1,-26,2,73,-1,-26,2,-122,-1,-26,2,-112,-1,-26,2,-111,-1,-26,2,-110,-1,-26,2,-109,-1,-26,2,-108,-1,-26,2,-68,-1,-26,2,-66,-1,-26,2,-64,-1,-26,2,-62,-1,-26,2,-48,-1,-26,
		2,-46,-1,-26,2,-44,-1,-26,2,-42,-1,-26,2,-8,-1,-26,2,-6,-1,-26,2,-4,-1,-26,3,45,-1,-26,3,-118,-1,-26,3,-105,-1,-26,3,-67,-1,-26,3,-64,-1,-26,3,-19,-1,-26,3,-16,-1,-26,4,11,-1,-26,4,13,-1,-26,4,15,-1,-26,4,74,-1,-26,4,76,-1,-26,4,78,-1,-26,4,80,-1,-26,4,82,-1,-26,4,84,-1,-26,4,86,
		-1,-26,4,88,-1,-26,4,90,-1,-26,4,92,-1,-26,4,94,-1,-26,4,96,-1,-26,4,-97,-1,-26,4,-72,-1,-26,0,56,0,37,-1,-28,0,60,-1,-46,0,61,-1,-45,0,-78,-1,-28,0,-76,-1,-28,0,-60,-1,-30,0,-38,-1,-46,1,13,-1,-28,1,51,-1,-46,1,67,-1,-46,1,93,-1,-46,2,127,-1,-28,2,-128,-1,-28,2,-127,-1,-28,2,-126,
		-1,-28,2,-125,-1,-28,2,-124,-1,-28,2,-123,-1,-28,2,-103,-1,-45,2,-74,-1,-28,2,-72,-1,-28,2,-70,-1,-28,3,34,-1,-45,3,36,-1,-45,3,-122,-1,-28,3,-117,-1,-45,3,-114,-1,-28,3,-102,-1,-45,3,-101,-1,-46,3,-99,-1,-45,3,-74,-1,-28,3,-62,-1,-46,3,-39,-1,-45,3,-30,-1,-28,3,-14,-1,-45,3,-11,-1,-46,3,-8,-1,-28,
		3,-6,-1,-28,4,3,-1,-46,4,30,-1,-28,4,32,-1,-28,4,34,-1,-28,4,36,-1,-28,4,38,-1,-28,4,40,-1,-28,4,42,-1,-28,4,44,-1,-28,4,46,-1,-28,4,48,-1,-28,4,50,-1,-28,4,52,-1,-28,4,112,-1,-45,4,114,-1,-45,4,116,-1,-45,4,-122,-1,-45,4,-103,-1,-28,0,40,0,16,-1,30,0,18,-1,30,0,37,-1,-51,
		0,-78,-1,-51,0,-76,-1,-51,0,-57,-1,-14,1,13,-1,-51,1,-122,-1,30,1,-118,-1,30,1,-114,-1,30,1,-113,-1,30,2,127,-1,-51,2,-128,-1,-51,2,-127,-1,-51,2,-126,-1,-51,2,-125,-1,-51,2,-124,-1,-51,2,-123,-1,-51,2,-74,-1,-51,2,-72,-1,-51,2,-70,-1,-51,3,-122,-1,-51,3,-114,-1,-51,3,-74,-1,-51,3,-30,-1,-51,3,-8,
		-1,-51,3,-6,-1,-51,4,30,-1,-51,4,32,-1,-51,4,34,-1,-51,4,36,-1,-51,4,38,-1,-51,4,40,-1,-51,4,42,-1,-51,4,44,-1,-51,4,46,-1,-51,4,48,-1,-51,4,50,-1,-51,4,52,-1,-51,4,-103,-1,-51,0,1,0,-60,0,14,0,2,0,-54,-1,-19,0,-10,-1,-64,0,-70,0,71,-1,-36,0,72,-1,-36,0,73,-1,-36,0,75,
		-1,-36,0,81,-1,-13,0,82,-1,-13,0,83,-1,-42,0,84,-1,-13,0,85,-1,-36,0,89,-1,-35,0,90,-1,-31,0,93,-1,-31,0,-108,-1,-36,0,-103,-1,-36,0,-101,-1,-35,0,-69,-1,-36,0,-67,-1,-31,0,-66,-1,-18,0,-65,-1,-26,0,-63,-1,-13,0,-62,-1,-21,0,-61,-1,-23,0,-59,-1,-16,0,-58,-1,-25,0,-56,-1,-36,0,-55,-1,-36,
		0,-54,-1,-29,0,-53,-1,-35,0,-52,-1,-50,0,-51,-1,-44,0,-50,-1,-37,0,-20,-1,-13,0,-16,-1,-13,0,-15,-1,-13,0,-13,-1,-13,0,-12,-1,-13,0,-11,-1,-13,0,-9,-1,-36,0,-8,-1,-13,0,-6,-1,-13,0,-5,-1,-13,0,-2,-1,-13,1,0,-1,-13,1,3,-1,-36,1,5,-1,-13,1,24,-1,-42,1,26,-1,-31,1,30,-1,-36,1,34,
		-1,-36,1,43,-1,-13,1,54,-1,-13,1,60,-1,-13,1,62,-1,-13,1,66,-1,-36,1,83,-1,-13,1,85,-1,-13,1,87,-1,-13,1,92,-1,-13,1,96,-1,-36,1,97,-1,-36,1,107,-1,-36,2,-95,-1,-36,2,-94,-1,-36,2,-93,-1,-36,2,-92,-1,-36,2,-91,-1,-36,2,-86,-1,-13,2,-85,-1,-42,2,-84,-1,-42,2,-83,-1,-42,2,-82,-1,-42,
		2,-81,-1,-42,2,-80,-1,-35,2,-79,-1,-35,2,-78,-1,-35,2,-77,-1,-35,2,-76,-1,-31,2,-75,-1,-31,2,-67,-1,-36,2,-65,-1,-36,2,-63,-1,-36,2,-61,-1,-36,2,-59,-1,-36,2,-57,-1,-36,2,-55,-1,-36,2,-53,-1,-36,2,-51,-1,-36,2,-49,-1,-36,2,-47,-1,-36,2,-45,-1,-36,2,-43,-1,-36,2,-41,-1,-36,2,-14,-1,-13,2,-12,
		-1,-13,2,-10,-1,-13,2,-9,-1,-13,2,-7,-1,-42,2,-5,-1,-42,2,-3,-1,-42,3,21,-1,-35,3,23,-1,-35,3,25,-1,-35,3,27,-1,-35,3,29,-1,-35,3,31,-1,-35,3,35,-1,-31,3,-98,-1,-36,3,-96,-1,-13,3,-94,-1,-35,3,-92,-1,-42,3,-90,-1,-31,3,-87,-1,-35,3,-86,-1,-42,3,-85,-1,-35,3,-60,-1,-36,3,-59,-1,-13,
		3,-58,-1,-42,3,-57,-1,-13,3,-56,-1,-36,3,-55,-1,-31,3,-53,-1,-36,3,-52,-1,-13,3,-47,-1,-13,3,-46,-1,-31,3,-38,-1,-31,3,-31,-1,-13,3,-26,-1,-36,3,-25,-1,-13,3,-21,-1,-31,3,-20,-1,-36,3,-15,-1,-36,3,-13,-1,-31,3,-1,-1,-36,4,1,-1,-36,4,2,-1,-36,4,8,-1,-13,4,10,-1,-13,4,12,-1,-42,4,14,
		-1,-36,4,16,-1,-42,4,20,-1,-31,4,22,-1,-31,4,24,-1,-31,4,28,-1,-13,4,29,-1,-36,4,55,-1,-36,4,57,-1,-36,4,59,-1,-36,4,61,-1,-36,4,63,-1,-36,4,65,-1,-36,4,67,-1,-36,4,69,-1,-36,4,75,-1,-42,4,77,-1,-42,4,79,-1,-42,4,81,-1,-42,4,83,-1,-42,4,85,-1,-42,4,87,-1,-42,4,89,-1,-36,
		4,91,-1,-36,4,93,-1,-36,4,95,-1,-42,4,97,-1,-36,4,99,-1,-35,4,101,-1,-35,4,103,-1,-35,4,105,-1,-35,4,107,-1,-35,4,109,-1,-35,4,111,-1,-35,4,113,-1,-31,4,115,-1,-31,4,117,-1,-31,4,124,-1,-13,4,-104,-1,-13,4,-100,-1,-36,4,-96,-1,-42,4,-92,-1,-35,4,-87,-1,-36,4,-85,-1,-36,4,-75,-1,-13,4,-73,
		-1,-13,4,-61,-1,-31,4,-59,-1,-31,0,124,0,6,-1,-38,0,11,-1,-38,0,71,-1,-16,0,72,-1,-16,0,73,-1,-16,0,75,-1,-16,0,85,-1,-16,0,89,-1,-17,0,90,-1,-36,0,93,-1,-36,0,-108,-1,-16,0,-103,-1,-16,0,-101,-1,-17,0,-69,-1,-16,0,-67,-1,-36,0,-62,-1,-20,0,-60,0,15,0,-58,-1,-22,0,-56,-1,-16,0,-55,
		-1,-16,0,-54,-1,-60,0,-53,-1,-17,0,-52,-1,-25,0,-9,-1,-16,1,3,-1,-16,1,26,-1,-36,1,30,-1,-16,1,34,-1,-16,1,66,-1,-16,1,96,-1,-16,1,97,-1,-16,1,107,-1,-16,1,-124,-1,-38,1,-123,-1,-38,1,-121,-1,-38,1,-120,-1,-38,1,-119,-1,-38,2,-95,-1,-16,2,-94,-1,-16,2,-93,-1,-16,2,-92,-1,-16,2,-91,-1,-16,
		2,-80,-1,-17,2,-79,-1,-17,2,-78,-1,-17,2,-77,-1,-17,2,-76,-1,-36,2,-75,-1,-36,2,-67,-1,-16,2,-65,-1,-16,2,-63,-1,-16,2,-61,-1,-16,2,-59,-1,-16,2,-57,-1,-16,2,-55,-1,-16,2,-53,-1,-16,2,-51,-1,-16,2,-49,-1,-16,2,-47,-1,-16,2,-45,-1,-16,2,-43,-1,-16,2,-41,-1,-16,3,21,-1,-17,3,23,-1,-17,3,25,
		-1,-17,3,27,-1,-17,3,29,-1,-17,3,31,-1,-17,3,35,-1,-36,3,-98,-1,-16,3,-94,-1,-17,3,-90,-1,-36,3,-87,-1,-17,3,-85,-1,-17,3,-60,-1,-16,3,-56,-1,-16,3,-55,-1,-36,3,-53,-1,-16,3,-46,-1,-36,3,-38,-1,-36,3,-37,-1,-38,3,-36,-1,-38,3,-33,-1,-38,3,-26,-1,-16,3,-21,-1,-36,3,-20,-1,-16,3,-15,-1,-16,
		3,-13,-1,-36,3,-1,-1,-16,4,1,-1,-16,4,2,-1,-16,4,14,-1,-16,4,20,-1,-36,4,22,-1,-36,4,24,-1,-36,4,29,-1,-16,4,55,-1,-16,4,57,-1,-16,4,59,-1,-16,4,61,-1,-16,4,63,-1,-16,4,65,-1,-16,4,67,-1,-16,4,69,-1,-16,4,89,-1,-16,4,91,-1,-16,4,93,-1,-16,4,97,-1,-16,4,99,-1,-17,4,101,
		-1,-17,4,103,-1,-17,4,105,-1,-17,4,107,-1,-17,4,109,-1,-17,4,111,-1,-17,4,113,-1,-36,4,115,-1,-36,4,117,-1,-36,4,-100,-1,-16,4,-92,-1,-17,4,-87,-1,-16,4,-85,-1,-16,4,-61,-1,-36,4,-59,-1,-36,0,60,0,6,-1,-96,0,11,-1,-96,0,74,-1,-23,0,89,-1,-15,0,90,-1,-59,0,93,-1,-59,0,-101,-1,-15,0,-67,
		-1,-59,0,-62,-1,-18,0,-60,0,16,0,-58,-1,-20,0,-54,-1,32,0,-53,-1,-15,1,26,-1,-59,1,-124,-1,-96,1,-123,-1,-96,1,-121,-1,-96,1,-120,-1,-96,1,-119,-1,-96,2,-80,-1,-15,2,-79,-1,-15,2,-78,-1,-15,2,-77,-1,-15,2,-76,-1,-59,2,-75,-1,-59,3,21,-1,-15,3,23,-1,-15,3,25,-1,-15,3,27,-1,-15,3,29,-1,-15,
		3,31,-1,-15,3,35,-1,-59,3,-94,-1,-15,3,-90,-1,-59,3,-87,-1,-15,3,-85,-1,-15,3,-55,-1,-59,3,-46,-1,-59,3,-38,-1,-59,3,-37,-1,-96,3,-36,-1,-96,3,-33,-1,-96,3,-21,-1,-59,3,-13,-1,-59,4,20,-1,-59,4,22,-1,-59,4,24,-1,-59,4,99,-1,-15,4,101,-1,-15,4,103,-1,-15,4,105,-1,-15,4,107,-1,-15,4,109,
		-1,-15,4,111,-1,-15,4,113,-1,-59,4,115,-1,-59,4,117,-1,-59,4,-92,-1,-15,4,-61,-1,-59,4,-59,-1,-59,0,65,0,71,-1,-25,0,72,-1,-25,0,73,-1,-25,0,75,-1,-25,0,85,-1,-25,0,-108,-1,-25,0,-103,-1,-25,0,-69,-1,-25,0,-60,0,15,0,-56,-1,-25,0,-55,-1,-25,0,-9,-1,-25,1,3,-1,-25,1,30,-1,-25,1,34,
		-1,-25,1,66,-1,-25,1,96,-1,-25,1,97,-1,-25,1,107,-1,-25,2,-95,-1,-25,2,-94,-1,-25,2,-93,-1,-25,2,-92,-1,-25,2,-91,-1,-25,2,-67,-1,-25,2,-65,-1,-25,2,-63,-1,-25,2,-61,-1,-25,2,-59,-1,-25,2,-57,-1,-25,2,-55,-1,-25,2,-53,-1,-25,2,-51,-1,-25,2,-49,-1,-25,2,-47,-1,-25,2,-45,-1,-25,2,-43,-1,-25,
		2,-41,-1,-25,3,-98,-1,-25,3,-60,-1,-25,3,-56,-1,-25,3,-53,-1,-25,3,-26,-1,-25,3,-20,-1,-25,3,-15,-1,-25,3,-1,-1,-25,4,1,-1,-25,4,2,-1,-25,4,14,-1,-25,4,29,-1,-25,4,55,-1,-25,4,57,-1,-25,4,59,-1,-25,4,61,-1,-25,4,63,-1,-25,4,65,-1,-25,4,67,-1,-25,4,69,-1,-25,4,89,-1,-25,4,91,
		-1,-25,4,93,-1,-25,4,97,-1,-25,4,-100,-1,-25,4,-87,-1,-25,4,-85,-1,-25,0,5,0,-54,-1,-22,0,-19,-1,-18,0,-10,-1,-85,1,58,-1,-20,1,109,-1,-20,0,1,0,-10,-1,-43,0,1,0,-54,0,11,0,-66,0,6,0,12,0,11,0,12,0,71,-1,-24,0,72,-1,-24,0,73,-1,-24,0,74,0,12,0,75,-1,-24,0,83,-1,-22,
		0,85,-1,-24,0,90,0,11,0,93,0,11,0,-108,-1,-24,0,-103,-1,-24,0,-69,-1,-24,0,-67,0,11,0,-66,-1,-19,0,-58,0,11,0,-56,-1,-24,0,-55,-1,-24,0,-54,0,12,0,-9,-1,-24,1,3,-1,-24,1,24,-1,-22,1,26,0,11,1,30,-1,-24,1,34,-1,-24,1,66,-1,-24,1,96,-1,-24,1,97,-1,-24,1,107,-1,-24,1,-124,
		0,12,1,-123,0,12,1,-121,0,12,1,-120,0,12,1,-119,0,12,1,-45,0,13,1,-42,0,13,1,-40,0,14,1,-39,-1,-11,1,-37,-1,-20,1,-35,-1,-19,1,-27,-1,-20,1,-21,-1,-65,1,-20,-1,-19,1,-19,-1,-65,1,-12,0,14,1,-11,-1,-19,1,-8,0,14,2,16,0,14,2,17,-1,-19,2,18,0,13,2,20,0,14,2,26,-1,-19,
		2,49,-1,-18,2,51,-1,-65,2,-95,-1,-24,2,-94,-1,-24,2,-93,-1,-24,2,-92,-1,-24,2,-91,-1,-24,2,-85,-1,-22,2,-84,-1,-22,2,-83,-1,-22,2,-82,-1,-22,2,-81,-1,-22,2,-76,0,11,2,-75,0,11,2,-67,-1,-24,2,-65,-1,-24,2,-63,-1,-24,2,-61,-1,-24,2,-59,-1,-24,2,-57,-1,-24,2,-55,-1,-24,2,-53,-1,-24,2,-51,
		-1,-24,2,-49,-1,-24,2,-47,-1,-24,2,-45,-1,-24,2,-43,-1,-24,2,-41,-1,-24,2,-7,-1,-22,2,-5,-1,-22,2,-3,-1,-22,3,35,0,11,3,50,-1,-65,3,51,-1,-65,3,52,-1,-65,3,53,-1,-65,3,54,-1,-65,3,55,-1,-65,3,56,-1,-65,3,57,-1,-19,3,67,-1,-19,3,68,-1,-19,3,69,-1,-19,3,70,-1,-19,3,71,-1,-19,
		3,76,0,13,3,77,-1,-65,3,78,-1,-65,3,79,-1,-65,3,80,-1,-19,3,81,-1,-19,3,82,-1,-19,3,83,-1,-19,3,90,-1,-19,3,91,-1,-19,3,92,-1,-19,3,93,-1,-19,3,109,-1,-19,3,110,-1,-19,3,111,-1,-19,3,115,-1,-11,3,116,-1,-11,3,117,-1,-11,3,118,-1,-11,3,120,0,14,3,-127,0,13,3,-126,0,13,3,-98,
		-1,-24,3,-92,-1,-22,3,-90,0,11,3,-86,-1,-22,3,-60,-1,-24,3,-58,-1,-22,3,-56,-1,-24,3,-55,0,11,3,-53,-1,-24,3,-46,0,11,3,-38,0,11,3,-37,0,12,3,-36,0,12,3,-33,0,12,3,-26,-1,-24,3,-21,0,11,3,-20,-1,-24,3,-15,-1,-24,3,-13,0,11,3,-1,-1,-24,4,1,-1,-24,4,2,-1,-24,4,12,-1,-22,
		4,14,-1,-24,4,16,-1,-22,4,20,0,11,4,22,0,11,4,24,0,11,4,29,-1,-24,4,55,-1,-24,4,57,-1,-24,4,59,-1,-24,4,61,-1,-24,4,63,-1,-24,4,65,-1,-24,4,67,-1,-24,4,69,-1,-24,4,75,-1,-22,4,77,-1,-22,4,79,-1,-22,4,81,-1,-22,4,83,-1,-22,4,85,-1,-22,4,87,-1,-22,4,89,-1,-24,4,91,
		-1,-24,4,93,-1,-24,4,95,-1,-22,4,97,-1,-24,4,113,0,11,4,115,0,11,4,117,0,11,4,-100,-1,-24,4,-96,-1,-22,4,-87,-1,-24,4,-85,-1,-24,4,-61,0,11,4,-59,0,11,4,-53,-1,-65,4,-49,-1,-19,4,-48,0,13,4,-46,-1,-65,4,-34,0,13,4,-31,0,13,4,-22,-1,-65,4,-15,-1,-19,4,-12,-1,-19,4,-11,0,14,
		4,-7,-1,-19,4,-6,0,13,0,1,0,-10,-1,-40,0,14,0,92,-1,-19,0,94,-1,-19,0,-18,-1,-19,0,-10,-1,-86,1,52,-1,-19,1,68,-1,-19,1,94,-1,-19,3,38,-1,-19,3,40,-1,-19,3,42,-1,-19,3,-54,-1,-19,3,-10,-1,-19,4,4,-1,-19,4,-55,-1,-19,0,13,0,92,-1,-14,0,94,-1,-14,0,-18,-1,-14,1,52,-1,-14,
		1,68,-1,-14,1,94,-1,-14,3,38,-1,-14,3,40,-1,-14,3,42,-1,-14,3,-54,-1,-14,3,-10,-1,-14,4,4,-1,-14,4,-55,-1,-14,0,34,0,90,-1,-12,0,92,-1,-14,0,93,-1,-12,0,94,-1,-13,0,-67,-1,-12,0,-18,-1,-14,1,26,-1,-12,1,52,-1,-14,1,68,-1,-14,1,94,-1,-14,2,-76,-1,-12,2,-75,-1,-12,3,35,-1,-12,
		3,38,-1,-13,3,40,-1,-13,3,42,-1,-13,3,-90,-1,-12,3,-55,-1,-12,3,-54,-1,-14,3,-46,-1,-12,3,-38,-1,-12,3,-21,-1,-12,3,-13,-1,-12,3,-10,-1,-14,4,4,-1,-14,4,20,-1,-12,4,22,-1,-12,4,24,-1,-12,4,113,-1,-12,4,115,-1,-12,4,117,-1,-12,4,-61,-1,-12,4,-59,-1,-12,4,-55,-1,-13,0,-116,0,6,-1,-54,
		0,11,-1,-54,0,56,-1,-46,0,58,-1,-44,0,60,-1,-12,0,61,-1,-45,0,81,-1,-47,0,82,-1,-47,0,84,-1,-47,0,90,-1,-26,0,92,-1,-17,0,93,-1,-26,0,-67,-1,-26,0,-63,-1,-47,0,-46,-1,-46,0,-42,-1,-46,0,-38,-1,-12,0,-34,-1,-19,0,-31,-1,-31,0,-26,-1,-44,0,-20,-1,-47,0,-18,-1,-17,0,-16,-1,-47,0,-15,
		-1,-47,0,-13,-1,-47,0,-12,-1,-47,0,-11,-1,-47,0,-10,-1,-55,0,-8,-1,-47,0,-6,-1,-47,0,-5,-1,-47,0,-2,-1,-47,1,0,-1,-47,1,5,-1,-47,1,9,-1,-27,1,25,-1,-44,1,26,-1,-26,1,32,-1,-29,1,43,-1,-47,1,51,-1,-12,1,52,-1,-17,1,54,-1,-47,1,57,-1,-46,1,58,-1,-60,1,60,-1,-47,1,62,-1,-47,
		1,67,-1,-12,1,68,-1,-17,1,69,-1,-46,1,71,-1,-31,1,73,-1,-31,1,83,-1,-47,1,85,-1,-47,1,87,-1,-47,1,92,-1,-47,1,93,-1,-12,1,94,-1,-17,1,98,-1,-44,1,99,-1,-11,1,100,-1,-25,1,108,-1,-46,1,109,-1,-55,1,-124,-1,-54,1,-123,-1,-54,1,-121,-1,-54,1,-120,-1,-54,1,-119,-1,-54,2,-103,-1,-45,2,-86,
		-1,-47,2,-76,-1,-26,2,-75,-1,-26,2,-14,-1,-47,2,-12,-1,-47,2,-10,-1,-47,2,-9,-1,-47,3,14,-1,-46,3,16,-1,-46,3,18,-1,-46,3,34,-1,-45,3,35,-1,-26,3,36,-1,-45,3,-117,-1,-45,3,-102,-1,-45,3,-101,-1,-12,3,-99,-1,-45,3,-96,-1,-47,3,-90,-1,-26,3,-75,-1,-19,3,-63,-1,-46,3,-62,-1,-12,3,-59,-1,-47,
		3,-57,-1,-47,3,-55,-1,-26,3,-54,-1,-17,3,-52,-1,-47,3,-47,-1,-47,3,-46,-1,-26,3,-39,-1,-45,3,-38,-1,-26,3,-37,-1,-54,3,-36,-1,-54,3,-33,-1,-54,3,-31,-1,-47,3,-25,-1,-47,3,-22,-1,-44,3,-21,-1,-26,3,-14,-1,-45,3,-13,-1,-26,3,-11,-1,-12,3,-10,-1,-17,4,3,-1,-12,4,4,-1,-17,4,8,-1,-47,4,10,
		-1,-47,4,19,-1,-19,4,20,-1,-26,4,21,-1,-19,4,22,-1,-26,4,23,-1,-19,4,24,-1,-26,4,25,-1,-31,4,28,-1,-47,4,112,-1,-45,4,113,-1,-26,4,114,-1,-45,4,115,-1,-26,4,116,-1,-45,4,117,-1,-26,4,119,-1,-46,4,121,-1,-31,4,124,-1,-47,4,-122,-1,-45,4,-104,-1,-47,4,-75,-1,-47,4,-73,-1,-47,4,-65,-1,-46,
		4,-62,-1,-44,4,-61,-1,-26,4,-60,-1,-44,4,-59,-1,-26,0,40,0,56,-1,-66,0,90,-1,-17,0,93,-1,-17,0,-67,-1,-17,0,-46,-1,-66,0,-42,-1,-66,0,-26,-1,-55,0,-10,-1,-33,1,9,-1,-19,1,26,-1,-17,1,32,-1,-21,1,57,-1,-66,1,58,-1,-33,1,69,-1,-66,1,76,-1,-23,1,99,-1,-11,1,109,-1,-32,2,-76,-1,-17,
		2,-75,-1,-17,3,14,-1,-66,3,16,-1,-66,3,18,-1,-66,3,35,-1,-17,3,-90,-1,-17,3,-63,-1,-66,3,-55,-1,-17,3,-46,-1,-17,3,-38,-1,-17,3,-21,-1,-17,3,-13,-1,-17,4,20,-1,-17,4,22,-1,-17,4,24,-1,-17,4,113,-1,-17,4,115,-1,-17,4,117,-1,-17,4,119,-1,-66,4,-65,-1,-66,4,-61,-1,-17,4,-59,-1,-17,0,63,
		0,56,-1,-26,0,58,-1,-25,0,60,-1,-14,0,61,-1,-25,0,92,-1,-15,0,-46,-1,-26,0,-42,-1,-26,0,-38,-1,-14,0,-34,-1,-18,0,-31,-1,-24,0,-26,-1,-26,0,-18,-1,-15,0,-10,-1,-48,1,25,-1,-25,1,51,-1,-14,1,52,-1,-15,1,57,-1,-26,1,58,-1,-50,1,67,-1,-14,1,68,-1,-15,1,69,-1,-26,1,71,-1,-24,1,73,
		-1,-24,1,93,-1,-14,1,94,-1,-15,1,98,-1,-25,1,100,-1,-19,1,108,-1,-26,1,109,-1,-48,2,-103,-1,-25,3,14,-1,-26,3,16,-1,-26,3,18,-1,-26,3,34,-1,-25,3,36,-1,-25,3,-117,-1,-25,3,-102,-1,-25,3,-101,-1,-14,3,-99,-1,-25,3,-75,-1,-18,3,-63,-1,-26,3,-62,-1,-14,3,-54,-1,-15,3,-39,-1,-25,3,-22,-1,-25,
		3,-14,-1,-25,3,-11,-1,-14,3,-10,-1,-15,4,3,-1,-14,4,4,-1,-15,4,19,-1,-18,4,21,-1,-18,4,23,-1,-18,4,25,-1,-24,4,112,-1,-25,4,114,-1,-25,4,116,-1,-25,4,119,-1,-26,4,121,-1,-24,4,-122,-1,-25,4,-65,-1,-26,4,-62,-1,-25,4,-60,-1,-25,0,-104,0,37,0,16,0,39,-1,-24,0,43,-1,-24,0,51,-1,-24,
		0,53,-1,-24,0,56,-1,-32,0,58,-1,-32,0,61,-1,-33,0,-125,-1,-24,0,-109,-1,-24,0,-104,-1,-24,0,-78,0,16,0,-77,-1,-24,0,-76,0,16,0,-46,-1,-32,0,-45,-1,-24,0,-44,0,16,0,-42,-1,-32,0,-39,0,20,0,-35,0,16,0,-31,-1,-31,0,-26,-1,-32,0,-19,0,19,0,-14,0,16,0,-7,-1,-32,1,4,0,16,1,8,
		-1,-24,1,13,0,16,1,23,-1,-24,1,25,-1,-32,1,27,-1,-24,1,29,-1,-24,1,31,-1,-24,1,33,-1,-24,1,57,-1,-32,1,65,-1,-24,1,69,-1,-32,1,71,-1,-31,1,72,-1,-32,1,73,-1,-31,1,74,-1,-32,1,77,-1,-31,1,80,0,16,1,81,0,16,1,88,-1,-23,1,98,-1,-33,1,100,-1,-34,1,102,0,16,1,106,-1,-24,
		1,108,-1,-33,1,110,-1,-14,1,111,0,16,1,112,0,16,2,69,-1,-24,2,70,-1,-24,2,72,-1,-24,2,73,-1,-24,2,127,0,16,2,-128,0,16,2,-127,0,16,2,-126,0,16,2,-125,0,16,2,-124,0,16,2,-123,0,16,2,-122,-1,-24,2,-112,-1,-24,2,-111,-1,-24,2,-110,-1,-24,2,-109,-1,-24,2,-108,-1,-24,2,-103,-1,-33,2,-74,
		0,16,2,-72,0,16,2,-70,0,16,2,-68,-1,-24,2,-66,-1,-24,2,-64,-1,-24,2,-62,-1,-24,2,-48,-1,-24,2,-46,-1,-24,2,-44,-1,-24,2,-42,-1,-24,2,-8,-1,-24,2,-6,-1,-24,2,-4,-1,-24,3,14,-1,-32,3,16,-1,-32,3,18,-1,-32,3,34,-1,-33,3,36,-1,-33,3,45,-1,-24,3,-122,0,16,3,-118,-1,-24,3,-117,-1,-33,
		3,-114,0,16,3,-105,-1,-24,3,-102,-1,-33,3,-99,-1,-33,3,-74,0,16,3,-67,-1,-24,3,-64,-1,-24,3,-63,-1,-32,3,-39,-1,-33,3,-30,0,16,3,-22,-1,-32,3,-19,-1,-24,3,-16,-1,-24,3,-14,-1,-33,3,-8,0,16,3,-6,0,16,4,11,-1,-24,4,13,-1,-24,4,15,-1,-24,4,25,-1,-31,4,26,-1,-32,4,30,0,16,4,32,
		0,16,4,34,0,16,4,36,0,16,4,38,0,16,4,40,0,16,4,42,0,16,4,44,0,16,4,46,0,16,4,48,0,16,4,50,0,16,4,52,0,16,4,74,-1,-24,4,76,-1,-24,4,78,-1,-24,4,80,-1,-24,4,82,-1,-24,4,84,-1,-24,4,86,-1,-24,4,88,-1,-24,4,90,-1,-24,4,92,-1,-24,4,94,-1,-24,4,96,-1,-24,
		4,112,-1,-33,4,114,-1,-33,4,116,-1,-33,4,119,-1,-32,4,121,-1,-31,4,122,-1,-32,4,-122,-1,-33,4,-103,0,16,4,-97,-1,-24,4,-72,-1,-24,4,-65,-1,-32,4,-62,-1,-32,4,-60,-1,-32,0,53,0,27,-1,-14,0,56,-1,-15,0,58,-1,-12,0,60,-1,-12,0,61,-1,-16,0,-46,-1,-15,0,-44,-1,-11,0,-42,-1,-15,0,-38,-1,-12,
		0,-35,-1,-11,0,-34,-1,-13,0,-26,-1,-15,1,25,-1,-12,1,51,-1,-12,1,57,-1,-15,1,67,-1,-12,1,69,-1,-15,1,80,-1,-11,1,93,-1,-12,1,98,-1,-14,1,100,-1,-14,1,102,-1,-11,1,108,-1,-14,1,111,-1,-11,2,-103,-1,-16,3,14,-1,-15,3,16,-1,-15,3,18,-1,-15,3,34,-1,-16,3,36,-1,-16,3,-117,-1,-16,3,-102,
		-1,-16,3,-101,-1,-12,3,-99,-1,-16,3,-75,-1,-13,3,-63,-1,-15,3,-62,-1,-12,3,-39,-1,-16,3,-22,-1,-12,3,-14,-1,-16,3,-11,-1,-12,4,3,-1,-12,4,19,-1,-13,4,21,-1,-13,4,23,-1,-13,4,112,-1,-16,4,114,-1,-16,4,116,-1,-16,4,119,-1,-15,4,-122,-1,-16,4,-65,-1,-15,4,-62,-1,-12,4,-60,-1,-12,0,106,0,37,
		0,15,0,56,-1,-26,0,58,-1,-26,0,60,0,14,0,61,-1,-26,0,-78,0,15,0,-76,0,15,0,-46,-1,-26,0,-44,0,14,0,-42,-1,-26,0,-39,0,19,0,-38,0,14,0,-35,0,14,0,-34,0,11,0,-31,-1,-27,0,-26,-1,-26,0,-25,-1,-12,0,-19,0,18,0,-14,0,15,0,-10,-1,-25,0,-7,-1,-24,1,4,0,15,1,13,0,15,
		1,25,-1,-26,1,51,0,14,1,57,-1,-26,1,58,-1,-25,1,67,0,14,1,69,-1,-26,1,71,-1,-27,1,72,-1,-24,1,73,-1,-27,1,74,-1,-24,1,76,-1,-28,1,80,0,14,1,81,0,15,1,93,0,14,1,98,-1,-26,1,100,-1,-26,1,102,0,14,1,108,-1,-26,1,109,-1,-25,1,111,0,14,1,112,0,15,2,127,0,15,2,-128,
		0,15,2,-127,0,15,2,-126,0,15,2,-125,0,15,2,-124,0,15,2,-123,0,15,2,-103,-1,-26,2,-74,0,15,2,-72,0,15,2,-70,0,15,3,14,-1,-26,3,16,-1,-26,3,18,-1,-26,3,34,-1,-26,3,36,-1,-26,3,-122,0,15,3,-117,-1,-26,3,-114,0,15,3,-102,-1,-26,3,-101,0,14,3,-99,-1,-26,3,-75,0,11,3,-74,0,15,
		3,-63,-1,-26,3,-62,0,14,3,-39,-1,-26,3,-30,0,15,3,-22,-1,-26,3,-14,-1,-26,3,-11,0,14,3,-8,0,15,3,-6,0,15,4,3,0,14,4,19,0,11,4,21,0,11,4,23,0,11,4,25,-1,-27,4,26,-1,-24,4,30,0,15,4,32,0,15,4,34,0,15,4,36,0,15,4,38,0,15,4,40,0,15,4,42,0,15,4,44,
		0,15,4,46,0,15,4,48,0,15,4,50,0,15,4,52,0,15,4,112,-1,-26,4,114,-1,-26,4,116,-1,-26,4,119,-1,-26,4,121,-1,-27,4,122,-1,-24,4,-122,-1,-26,4,-103,0,15,4,-65,-1,-26,4,-62,-1,-26,4,-60,-1,-26,0,49,0,56,-1,-29,0,60,-1,-27,0,61,-1,-28,0,-46,-1,-29,0,-44,-1,-27,0,-42,-1,-29,0,-39,
		-1,-30,0,-38,-1,-27,0,-35,-1,-27,0,-34,-1,-23,0,-14,-1,-22,1,4,-1,-22,1,51,-1,-27,1,57,-1,-29,1,67,-1,-27,1,69,-1,-29,1,80,-1,-27,1,81,-1,-22,1,93,-1,-27,1,102,-1,-27,1,108,-1,-28,1,111,-1,-27,1,112,-1,-22,2,-103,-1,-28,3,14,-1,-29,3,16,-1,-29,3,18,-1,-29,3,34,-1,-28,3,36,-1,-28,
		3,-117,-1,-28,3,-102,-1,-28,3,-101,-1,-27,3,-99,-1,-28,3,-75,-1,-23,3,-63,-1,-29,3,-62,-1,-27,3,-39,-1,-28,3,-14,-1,-28,3,-11,-1,-27,4,3,-1,-27,4,19,-1,-23,4,21,-1,-23,4,23,-1,-23,4,112,-1,-28,4,114,-1,-28,4,116,-1,-28,4,119,-1,-29,4,-122,-1,-28,4,-65,-1,-29,0,36,0,56,-1,-30,0,60,-1,-28,
		0,-46,-1,-30,0,-44,-1,-28,0,-42,-1,-30,0,-39,-1,-31,0,-38,-1,-28,0,-35,-1,-28,0,-34,-1,-23,0,-19,-1,-28,0,-14,-1,-21,1,4,-1,-21,1,51,-1,-28,1,57,-1,-30,1,67,-1,-28,1,69,-1,-30,1,80,-1,-28,1,81,-1,-21,1,93,-1,-28,1,102,-1,-28,1,111,-1,-28,1,112,-1,-21,3,14,-1,-30,3,16,-1,-30,3,18,
		-1,-30,3,-101,-1,-28,3,-75,-1,-23,3,-63,-1,-30,3,-62,-1,-28,3,-11,-1,-28,4,3,-1,-28,4,19,-1,-23,4,21,-1,-23,4,23,-1,-23,4,119,-1,-30,4,-65,-1,-30,0,24,0,56,-1,-21,0,61,-1,-13,0,-46,-1,-21,0,-42,-1,-21,1,57,-1,-21,1,69,-1,-21,2,-103,-1,-13,3,14,-1,-21,3,16,-1,-21,3,18,-1,-21,3,34,
		-1,-13,3,36,-1,-13,3,-117,-1,-13,3,-102,-1,-13,3,-99,-1,-13,3,-63,-1,-21,3,-39,-1,-13,3,-14,-1,-13,4,112,-1,-13,4,114,-1,-13,4,116,-1,-13,4,119,-1,-21,4,-122,-1,-13,4,-65,-1,-21,0,57,0,81,-1,-17,0,82,-1,-17,0,84,-1,-17,0,92,-1,-16,0,-63,-1,-17,0,-20,-1,-17,0,-19,-1,-18,0,-18,-1,-16,0,-16,
		-1,-17,0,-15,-1,-17,0,-13,-1,-17,0,-12,-1,-17,0,-11,-1,-17,0,-10,-1,-18,0,-8,-1,-17,0,-6,-1,-17,0,-5,-1,-17,0,-2,-1,-17,1,0,-1,-17,1,5,-1,-17,1,9,-1,-12,1,32,-1,-15,1,43,-1,-17,1,52,-1,-16,1,54,-1,-17,1,58,-1,-17,1,60,-1,-17,1,62,-1,-17,1,68,-1,-16,1,83,-1,-17,1,85,-1,-17,
		1,87,-1,-17,1,92,-1,-17,1,94,-1,-16,1,109,-1,-17,2,-86,-1,-17,2,-14,-1,-17,2,-12,-1,-17,2,-10,-1,-17,2,-9,-1,-17,3,-96,-1,-17,3,-59,-1,-17,3,-57,-1,-17,3,-54,-1,-16,3,-52,-1,-17,3,-47,-1,-17,3,-31,-1,-17,3,-25,-1,-17,3,-10,-1,-16,4,4,-1,-16,4,8,-1,-17,4,10,-1,-17,4,28,-1,-17,4,124,
		-1,-17,4,-104,-1,-17,4,-75,-1,-17,4,-73,-1,-17,0,35,0,6,-1,-14,0,11,-1,-14,0,90,-1,-11,0,93,-1,-11,0,-67,-1,-11,0,-10,-1,-12,1,9,-1,-11,1,26,-1,-11,1,58,-1,-11,1,109,-1,-11,1,-124,-1,-14,1,-123,-1,-14,1,-121,-1,-14,1,-120,-1,-14,1,-119,-1,-14,2,-76,-1,-11,2,-75,-1,-11,3,35,-1,-11,3,-90,
		-1,-11,3,-55,-1,-11,3,-46,-1,-11,3,-38,-1,-11,3,-37,-1,-14,3,-36,-1,-14,3,-33,-1,-14,3,-21,-1,-11,3,-13,-1,-11,4,20,-1,-11,4,22,-1,-11,4,24,-1,-11,4,113,-1,-11,4,115,-1,-11,4,117,-1,-11,4,-61,-1,-11,4,-59,-1,-11,0,10,0,-19,0,20,0,-10,-1,-19,0,-7,-1,-19,0,-4,-1,-30,1,58,-1,-19,1,72,
		-1,-19,1,74,-1,-19,1,109,-1,-19,4,26,-1,-19,4,122,-1,-19,0,118,0,71,-1,-16,0,72,-1,-16,0,73,-1,-16,0,75,-1,-16,0,83,-1,-21,0,85,-1,-16,0,-108,-1,-16,0,-103,-1,-16,0,-69,-1,-16,0,-56,-1,-16,0,-55,-1,-16,0,-9,-1,-16,1,3,-1,-16,1,24,-1,-21,1,28,-1,-21,1,30,-1,-16,1,34,-1,-16,1,66,
		-1,-16,1,96,-1,-16,1,97,-1,-16,1,107,-1,-16,1,-37,-1,-21,1,-35,-1,-21,1,-27,-1,-23,1,-20,-1,-21,1,-11,-1,-21,2,17,-1,-21,2,26,-1,-21,2,49,-1,-21,2,-95,-1,-16,2,-94,-1,-16,2,-93,-1,-16,2,-92,-1,-16,2,-91,-1,-16,2,-85,-1,-21,2,-84,-1,-21,2,-83,-1,-21,2,-82,-1,-21,2,-81,-1,-21,2,-67,-1,-16,
		2,-65,-1,-16,2,-63,-1,-16,2,-61,-1,-16,2,-59,-1,-16,2,-57,-1,-16,2,-55,-1,-16,2,-53,-1,-16,2,-51,-1,-16,2,-49,-1,-16,2,-47,-1,-16,2,-45,-1,-16,2,-43,-1,-16,2,-41,-1,-16,2,-7,-1,-21,2,-5,-1,-21,2,-3,-1,-21,3,57,-1,-21,3,67,-1,-21,3,68,-1,-21,3,69,-1,-21,3,70,-1,-21,3,71,-1,-21,3,80,
		-1,-21,3,81,-1,-21,3,82,-1,-21,3,83,-1,-21,3,90,-1,-21,3,91,-1,-21,3,92,-1,-21,3,93,-1,-21,3,109,-1,-21,3,110,-1,-21,3,111,-1,-21,3,-98,-1,-16,3,-92,-1,-21,3,-86,-1,-21,3,-60,-1,-16,3,-58,-1,-21,3,-56,-1,-16,3,-53,-1,-16,3,-26,-1,-16,3,-20,-1,-16,3,-15,-1,-16,3,-1,-1,-16,4,1,-1,-16,
		4,2,-1,-16,4,12,-1,-21,4,14,-1,-16,4,16,-1,-21,4,29,-1,-16,4,55,-1,-16,4,57,-1,-16,4,59,-1,-16,4,61,-1,-16,4,63,-1,-16,4,65,-1,-16,4,67,-1,-16,4,69,-1,-16,4,75,-1,-21,4,77,-1,-21,4,79,-1,-21,4,81,-1,-21,4,83,-1,-21,4,85,-1,-21,4,87,-1,-21,4,89,-1,-16,4,91,-1,-16,4,93,
		-1,-16,4,95,-1,-21,4,97,-1,-16,4,-100,-1,-16,4,-96,-1,-21,4,-87,-1,-16,4,-85,-1,-16,4,-49,-1,-21,4,-15,-1,-21,4,-12,-1,-21,4,-7,-1,-21,0,-29,0,6,0,13,0,11,0,13,0,69,-1,-16,0,71,-1,-80,0,72,-1,-80,0,73,-1,-80,0,74,0,13,0,75,-1,-80,0,83,-1,-42,0,85,-1,-80,0,90,0,11,0,93,
		0,11,0,-108,-1,-80,0,-103,-1,-80,0,-69,-1,-80,0,-67,0,11,0,-66,-1,-80,0,-57,-1,-85,0,-56,-1,-64,0,-55,-1,-80,0,-52,-1,-43,0,-19,-1,-86,0,-14,-1,-81,0,-9,-1,-80,1,3,-1,-80,1,4,-1,-81,1,24,-1,-42,1,26,0,11,1,28,-1,-30,1,30,-1,-80,1,32,0,12,1,34,-1,-80,1,66,-1,-80,1,81,-1,-81,
		1,96,-1,-80,1,97,-1,-80,1,99,0,11,1,101,0,11,1,107,-1,-80,1,112,-1,-81,1,-124,0,13,1,-123,0,13,1,-121,0,13,1,-120,0,13,1,-119,0,13,1,-45,0,13,1,-42,0,13,1,-40,0,14,1,-39,-1,-11,1,-37,-1,-20,1,-35,-1,-19,1,-27,-1,-20,1,-21,-1,-65,1,-20,-1,-19,1,-19,-1,-65,1,-12,0,14,1,-11,
		-1,-19,1,-8,0,14,2,16,0,14,2,17,-1,-19,2,18,0,13,2,20,0,14,2,26,-1,-19,2,49,-1,-18,2,51,-1,-65,2,-102,-1,-16,2,-101,-1,-16,2,-100,-1,-16,2,-99,-1,-16,2,-98,-1,-16,2,-97,-1,-16,2,-96,-1,-16,2,-95,-1,-80,2,-94,-1,-80,2,-93,-1,-80,2,-92,-1,-80,2,-91,-1,-80,2,-85,-1,-42,2,-84,-1,-42,
		2,-83,-1,-42,2,-82,-1,-42,2,-81,-1,-42,2,-76,0,11,2,-75,0,11,2,-73,-1,-16,2,-71,-1,-16,2,-69,-1,-16,2,-67,-1,-80,2,-65,-1,-80,2,-63,-1,-80,2,-61,-1,-80,2,-59,-1,-80,2,-57,-1,-80,2,-55,-1,-80,2,-53,-1,-80,2,-51,-1,-80,2,-49,-1,-80,2,-47,-1,-80,2,-45,-1,-80,2,-43,-1,-80,2,-41,-1,-80,2,-7,
		-1,-42,2,-5,-1,-42,2,-3,-1,-42,3,35,0,11,3,50,-1,-65,3,51,-1,-65,3,52,-1,-65,3,53,-1,-65,3,54,-1,-65,3,55,-1,-65,3,56,-1,-65,3,57,-1,-19,3,67,-1,-19,3,68,-1,-19,3,69,-1,-19,3,70,-1,-19,3,71,-1,-19,3,76,0,13,3,77,-1,-65,3,78,-1,-65,3,79,-1,-65,3,80,-1,-19,3,81,-1,-19,
		3,82,-1,-19,3,83,-1,-19,3,90,-1,-19,3,91,-1,-19,3,92,-1,-19,3,93,-1,-19,3,109,-1,-19,3,110,-1,-19,3,111,-1,-19,3,115,-1,-11,3,116,-1,-11,3,117,-1,-11,3,118,-1,-11,3,120,0,14,3,-127,0,13,3,-126,0,13,3,-98,-1,-80,3,-92,-1,-42,3,-90,0,11,3,-86,-1,-42,3,-61,-1,-16,3,-60,-1,-80,3,-58,
		-1,-42,3,-56,-1,-80,3,-55,0,11,3,-53,-1,-80,3,-46,0,11,3,-38,0,11,3,-37,0,13,3,-36,0,13,3,-33,0,13,3,-29,-1,-16,3,-26,-1,-80,3,-21,0,11,3,-20,-1,-80,3,-15,-1,-80,3,-13,0,11,3,-7,-1,-16,3,-5,-1,-16,3,-1,-1,-80,4,1,-1,-80,4,2,-1,-80,4,12,-1,-42,4,14,-1,-80,4,16,-1,-42,
		4,20,0,11,4,22,0,11,4,24,0,11,4,29,-1,-80,4,31,-1,-16,4,33,-1,-16,4,35,-1,-16,4,37,-1,-16,4,39,-1,-16,4,41,-1,-16,4,43,-1,-16,4,45,-1,-16,4,47,-1,-16,4,49,-1,-16,4,51,-1,-16,4,53,-1,-16,4,55,-1,-80,4,57,-1,-80,4,59,-1,-80,4,61,-1,-80,4,63,-1,-80,4,65,-1,-80,4,67,
		-1,-80,4,69,-1,-80,4,75,-1,-42,4,77,-1,-42,4,79,-1,-42,4,81,-1,-42,4,83,-1,-42,4,85,-1,-42,4,87,-1,-42,4,89,-1,-80,4,91,-1,-80,4,93,-1,-80,4,95,-1,-42,4,97,-1,-80,4,113,0,11,4,115,0,11,4,117,0,11,4,-102,-1,-16,4,-100,-1,-80,4,-96,-1,-42,4,-87,-1,-80,4,-85,-1,-80,4,-61,0,11,
		4,-59,0,11,4,-53,-1,-65,4,-49,-1,-19,4,-48,0,13,4,-46,-1,-65,4,-34,0,13,4,-31,0,13,4,-22,-1,-65,4,-15,-1,-19,4,-12,-1,-19,4,-11,0,14,4,-7,-1,-19,4,-6,0,13,0,14,0,-19,0,20,0,-14,0,16,0,-10,-1,-16,0,-7,-1,-16,1,1,0,12,1,4,0,16,1,58,-1,-16,1,72,-1,-16,1,74,-1,-26,
		1,81,0,16,1,109,-1,-16,1,112,0,16,4,26,-1,-16,4,122,-1,-16,0,77,0,71,0,12,0,72,0,12,0,73,0,12,0,75,0,12,0,85,0,12,0,-108,0,12,0,-103,0,12,0,-69,0,12,0,-56,0,12,0,-55,0,12,0,-19,0,58,0,-14,0,24,0,-10,-1,-29,0,-9,0,12,0,-7,-1,-9,1,3,0,12,1,4,0,24,
		1,30,0,12,1,34,0,12,1,58,-1,-30,1,66,0,12,1,72,-1,-9,1,74,-1,-29,1,81,0,24,1,96,0,12,1,97,0,12,1,107,0,12,1,109,-1,-29,1,112,0,24,2,-95,0,12,2,-94,0,12,2,-93,0,12,2,-92,0,12,2,-91,0,12,2,-67,0,12,2,-65,0,12,2,-63,0,12,2,-61,0,12,2,-59,0,12,2,-57,
		0,12,2,-55,0,12,2,-53,0,12,2,-51,0,12,2,-49,0,12,2,-47,0,12,2,-45,0,12,2,-43,0,12,2,-41,0,12,3,-98,0,12,3,-60,0,12,3,-56,0,12,3,-53,0,12,3,-26,0,12,3,-20,0,12,3,-15,0,12,3,-1,0,12,4,1,0,12,4,2,0,12,4,14,0,12,4,26,-1,-9,4,29,0,12,4,55,0,12,
		4,57,0,12,4,59,0,12,4,61,0,12,4,63,0,12,4,65,0,12,4,67,0,12,4,69,0,12,4,89,0,12,4,91,0,12,4,93,0,12,4,97,0,12,4,122,-1,-9,4,-100,0,12,4,-87,0,12,4,-85,0,12,0,34,0,90,-1,-12,0,92,-1,-16,0,93,-1,-12,0,-67,-1,-12,0,-19,-1,-17,0,-18,-1,-16,0,-14,-1,-13,
		1,4,-1,-13,1,26,-1,-12,1,52,-1,-16,1,68,-1,-16,1,81,-1,-13,1,94,-1,-16,1,112,-1,-13,2,-76,-1,-12,2,-75,-1,-12,3,35,-1,-12,3,-90,-1,-12,3,-55,-1,-12,3,-54,-1,-16,3,-46,-1,-12,3,-38,-1,-12,3,-21,-1,-12,3,-13,-1,-12,3,-10,-1,-16,4,4,-1,-16,4,20,-1,-12,4,22,-1,-12,4,24,-1,-12,4,113,
		-1,-12,4,115,-1,-12,4,117,-1,-12,4,-61,-1,-12,4,-59,-1,-12,0,10,0,6,-1,-42,0,11,-1,-42,1,-124,-1,-42,1,-123,-1,-42,1,-121,-1,-42,1,-120,-1,-42,1,-119,-1,-42,3,-37,-1,-42,3,-36,-1,-42,3,-33,-1,-42,0,8,0,-10,-1,-70,1,9,-1,-49,1,32,-1,-37,1,58,-1,80,1,74,-1,-99,1,99,-1,-16,1,101,-1,-14,
		1,109,-1,76,0,10,0,6,-1,-11,0,11,-1,-11,1,-124,-1,-11,1,-123,-1,-11,1,-121,-1,-11,1,-120,-1,-11,1,-119,-1,-11,3,-37,-1,-11,3,-36,-1,-11,3,-33,-1,-11,0,40,0,76,0,32,0,79,0,32,0,80,0,32,0,83,-1,-128,0,87,-1,-112,0,91,0,11,1,24,-1,-128,1,-63,-1,-112,2,-85,-1,-128,2,-84,-1,-128,2,-83,
		-1,-128,2,-82,-1,-128,2,-81,-1,-128,2,-7,-1,-128,2,-5,-1,-128,2,-3,-1,-128,3,5,-1,-112,3,7,-1,-112,3,9,-1,-112,3,11,-1,-112,3,13,-1,-112,3,-92,-1,-128,3,-86,-1,-128,3,-58,-1,-128,3,-51,-1,-112,4,12,-1,-128,4,16,-1,-128,4,75,-1,-128,4,77,-1,-128,4,79,-1,-128,4,81,-1,-128,4,83,-1,-128,4,85,-1,-128,
		4,87,-1,-128,4,95,-1,-128,4,-96,-1,-128,4,-83,0,32,4,-81,0,32,4,-79,0,32,4,-66,-1,-112,0,19,1,-45,-1,-18,1,-43,-1,-11,1,-42,-1,-15,1,-40,-1,-14,1,-12,-1,-14,1,-8,-1,-14,2,16,-1,-14,2,18,-1,-18,2,20,-1,-14,3,76,-1,-18,3,120,-1,-14,3,-128,-1,-11,3,-127,-1,-18,3,-126,-1,-18,4,-48,-1,-18,
		4,-34,-1,-18,4,-31,-1,-18,4,-11,-1,-14,4,-6,-1,-18,0,19,1,-45,-1,-27,1,-43,-1,-15,1,-42,-1,-21,1,-40,-1,-23,1,-12,-1,-23,1,-8,-1,-23,2,16,-1,-23,2,18,-1,-27,2,20,-1,-23,3,76,-1,-27,3,120,-1,-23,3,-128,-1,-15,3,-127,-1,-27,3,-126,-1,-27,4,-48,-1,-27,4,-34,-1,-27,4,-31,-1,-27,4,-11,-1,-23,
		4,-6,-1,-27,0,3,1,-43,-1,-11,1,-42,-1,-18,3,-128,-1,-11,0,2,1,-42,-1,-73,1,-37,-1,-16,0,1,0,91,0,11,0,4,0,13,-1,-26,0,65,-1,-12,0,97,-1,-17,1,77,-1,-19,0,23,0,-72,-1,-44,0,-66,-1,-16,0,-62,-1,-19,0,-60,0,17,0,-54,-1,-32,0,-52,-1,-25,0,-51,-1,-27,0,-50,-1,-18,0,-39,0,18,
		0,-22,-1,-23,0,-10,-1,-41,1,58,-1,-41,1,74,-1,-45,1,76,-1,-42,1,77,-1,-59,1,88,-1,-25,1,98,0,13,1,100,0,12,1,109,-1,-42,1,110,-1,-14,1,-37,-1,-23,1,-27,-1,-25,2,49,-1,-23,0,1,1,28,-1,-15,0,18,0,-39,-1,-82,0,-26,0,18,0,-21,-1,-32,0,-19,-1,-83,0,-17,-1,-42,0,-3,-1,-33,1,1,
		-1,-46,1,7,-1,-32,1,28,-1,-50,1,46,-1,-35,1,48,-1,-30,1,56,-1,-32,1,64,-1,-32,1,74,-1,-23,1,77,-1,-38,1,95,-1,-67,1,105,-1,-33,1,108,0,17,0,2,0,-10,-1,-11,1,-123,-1,-80,0,2,0,-19,-1,-55,1,28,-1,-18,0,9,0,-26,-1,-61,0,-10,-1,-49,1,58,-1,-50,1,73,-1,-25,1,76,-1,-33,1,98,
		-1,-47,1,100,-1,-20,1,108,-1,-96,1,109,-1,-47,0,47,0,86,-1,109,0,91,-1,-116,0,109,-3,-65,0,124,-2,125,0,-127,-2,-68,0,-122,-1,43,0,-119,-1,75,0,-72,-1,97,0,-66,-1,-113,0,-65,-1,15,0,-61,-2,-24,0,-58,-1,31,0,-57,-2,-27,0,-54,-1,70,0,-52,-2,-19,0,-51,-2,-3,0,-50,-2,-39,0,-39,-1,82,0,-26,
		0,5,0,-22,-1,-67,0,-21,-1,73,0,-19,-2,-2,0,-17,-1,19,0,-10,-1,104,0,-3,-1,14,0,-1,-1,19,1,1,-1,7,1,7,-1,14,1,9,-1,17,1,28,-1,60,1,32,-1,-84,1,46,-1,21,1,48,-1,60,1,56,-1,14,1,58,-1,106,1,64,-1,73,1,74,-1,12,1,76,-1,63,1,77,-2,-15,1,88,-1,-64,1,95,-2,-17,
		1,99,-1,49,1,101,-1,95,1,105,-1,10,1,108,0,5,1,109,-1,48,1,110,-1,-43,0,30,0,10,-1,-30,0,13,0,20,0,14,-1,-49,0,65,0,18,0,74,-1,-22,0,86,-1,-40,0,88,-1,-22,0,97,0,19,0,109,-1,-82,0,124,-1,-51,0,-127,-1,-96,0,-122,-1,-63,0,-119,-1,-64,0,-72,-1,-48,0,-68,-1,-22,0,-66,-1,-18,
		0,-65,-1,-58,0,-64,0,13,0,-62,-1,-23,0,-61,-1,-42,0,-58,-1,-24,0,-57,-1,-70,0,-54,-1,-23,0,-52,-1,-53,0,-51,-1,-38,0,-50,-1,-57,1,-115,-1,-45,1,-37,-1,-53,1,-27,-1,-53,2,49,-1,-51,0,23,0,35,-1,-61,0,88,-1,-17,0,91,-1,-33,0,-102,-1,-18,0,-72,-1,-27,0,-71,-1,-47,0,-60,0,17,0,-54,-1,-56,
		0,-39,0,19,0,-26,-1,-59,0,-10,-1,-54,1,58,-1,-97,1,73,-1,81,1,74,-1,123,1,76,-1,-54,1,77,-1,-35,1,88,-1,-14,1,98,-1,117,1,100,-1,-54,1,108,-1,79,1,109,-1,-116,1,-42,-1,-51,1,-27,-1,-11,0,7,0,-10,-1,-16,1,9,-1,-15,1,32,-1,-13,1,58,-1,-15,1,99,-1,-13,1,101,-1,-23,1,109,-1,-45,
		0,3,0,74,-1,-18,0,91,-1,-22,1,-42,-1,-16,0,9,0,-54,-1,-22,0,-19,-1,-72,0,-10,-1,-22,1,9,-1,-16,1,32,-1,-15,1,58,-1,-21,1,99,-1,-11,1,109,-1,-20,1,-123,-1,-80,0,2,1,17,0,11,1,108,-1,-26,0,18,0,91,-1,-63,0,-72,-1,-59,0,-54,-1,-76,0,-22,-1,-41,0,-10,-1,-71,1,9,-1,-78,1,28,
		-1,-46,1,32,-1,-56,1,58,-1,-96,1,74,-1,-59,1,88,-1,-28,1,99,-1,-52,1,101,-1,-52,1,109,-1,-53,1,110,-1,-17,1,-37,-1,-25,1,-27,-1,-26,2,49,-1,-24,0,5,0,91,-1,-92,1,-42,-1,84,1,-37,-1,-15,1,-27,-1,-15,2,49,-1,-13,0,8,0,-39,0,21,0,-19,0,21,1,73,-1,-28,1,74,-1,-27,1,76,-1,-28,
		1,98,-1,-29,1,100,-1,-30,1,108,-1,-28,0,2,0,-10,-1,-64,1,-123,-1,-80,0,8,0,88,0,14,0,-127,-1,-97,0,-66,-1,-11,0,-60,-1,-34,0,-57,-1,-27,0,-39,-1,-88,0,-19,-1,-54,1,95,-1,-29,0,5,0,-54,-1,-22,0,-19,-1,-18,0,-10,-1,-80,1,58,-1,-20,1,109,-1,-20,0,3,0,74,0,15,0,88,0,50,0,91,
		0,17,0,51,0,4,-1,-40,0,86,-1,-75,0,91,-1,-57,0,109,-2,-72,0,124,-1,40,0,-127,-1,77,0,-122,-1,-114,0,-119,-1,-95,0,-72,-1,-82,0,-66,-1,-55,0,-65,-1,126,0,-61,-1,103,0,-58,-1,-121,0,-57,-1,101,0,-54,-1,-98,0,-52,-1,106,0,-51,-1,115,0,-50,-1,94,0,-39,-1,-91,0,-26,0,15,0,-22,-1,-28,0,-21,
		-1,-96,0,-19,-1,116,0,-17,-1,-128,0,-10,-1,-78,0,-3,-1,125,0,-1,-1,-128,1,1,-1,121,1,7,-1,125,1,9,-1,127,1,28,-1,-104,1,32,-1,-38,1,46,-1,-127,1,48,-1,-104,1,56,-1,125,1,58,-1,-77,1,64,-1,-96,1,74,-1,124,1,76,-1,-102,1,77,-1,108,1,88,-1,-26,1,95,-1,107,1,99,-1,-110,1,101,-1,-83,
		1,105,-1,123,1,108,0,15,1,109,-1,-111,1,110,-1,-14,1,-37,-1,-71,1,-27,-1,-71,2,49,-1,-71,0,7,0,13,0,20,0,65,0,17,0,86,-1,-30,0,97,0,19,1,-37,-1,-39,1,-27,-1,-39,2,49,-1,-39,0,7,0,74,0,13,0,-66,-1,-11,0,-58,0,11,0,-57,-1,-22,0,-54,0,12,0,-19,-1,-56,1,28,-1,-15,0,7,
		0,13,0,15,0,65,0,12,0,86,-1,-21,0,97,0,14,1,-37,-1,-25,1,-27,-1,-25,2,49,-1,-23,0,6,0,91,-1,-27,0,-72,-1,-53,0,-51,-1,-28,1,-37,-1,-20,1,-27,-1,-21,2,49,-1,-19,0,7,0,-127,-1,-33,0,-75,-1,-13,0,-73,-1,-16,0,-60,-1,-22,0,-39,-1,-33,0,-26,-1,-32,1,108,-1,-32,0,1,1,-37,-1,-21,
		0,4,1,-42,-1,-57,1,-37,-1,-14,1,-27,-1,-14,2,49,-1,-14,0,1,1,-42,-1,-15,0,1,1,-42,0,13,0,2,11,12,0,4,0,0,14,-84,23,104,0,38,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,0,-1,-29,-1,-28,0,0,0,0,0,0,0,0,0,17,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,17,0,0,0,17,0,0,0,0,0,0,0,0,-1,-28,-1,-27,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-21,0,0,0,0,0,0,0,0,-1,-27,-1,-43,-1,-19,0,0,0,0,0,0,-1,-22,0,0,-1,-23,0,0,0,0,0,0,0,0,0,0,-1,-31,-1,-102,0,0,-1,-11,-1,-22,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-11,0,0,-1,-12,-1,-11,0,0,0,0,-1,-11,-1,-50,-1,-17,-1,127,-1,-94,0,0,0,0,0,12,0,0,0,0,-1,-15,0,0,-1,-120,0,0,-1,-69,-1,-60,-1,-57,0,17,0,0,0,18,0,0,-1,-87,0,0,0,0,-1,-55,-1,-113,0,0,0,0,-1,-35,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-15,0,0,0,0,0,0,0,0,0,0,-1,-16,0,0,0,0,0,0,0,0,-1,120,-1,-21,0,0,0,0,0,0,0,0,0,0,-1,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-19,0,0,0,0,-1,-19,-1,-17,0,0,0,0,0,0,-1,-26,0,0,0,20,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,-1,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-67,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-11,0,0,0,0,0,0,0,0,0,0,-1,-15,0,0,0,0,0,0,0,0,-1,-29,-1,-15,0,0,0,0,0,0,0,0,0,0,-1,-14,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-13,0,0,0,0,
		-1,-15,0,0,0,0,-1,-15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,-1,-107,-1,-41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-22,0,0,0,0,0,0,0,0,0,0,
		0,0,-1,-21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-26,-1,-31,-1,-23,-1,-27,-1,-23,0,0,0,0,-1,-25,-1,-40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-64,0,0,-1,-93,0,0,0,0,0,0,0,0,-1,-65,-1,-29,-1,-40,-1,-65,-1,-39,-1,-94,-1,-73,-1,-53,-1,-20,-1,-96,0,17,0,18,-1,-85,-1,-58,-1,-30,-1,-16,0,13,0,0,0,0,0,0,-1,-23,0,17,0,0,-1,-13,0,0,-1,45,0,0,
		-1,-17,0,18,0,0,-1,-52,0,0,0,0,0,0,-1,-96,-1,-13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-22,-1,-18,0,0,0,0,0,0,-1,-20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,-1,-99,-1,-28,-1,-109,-1,-99,-1,-95,-1,-79,-1,-113,-1,-71,-1,-72,0,0,0,16,0,16,-1,-81,-1,-116,-1,-60,-1,-16,0,0,0,0,0,0,0,0,-1,-77,0,15,0,0,-1,-15,-1,-53,-1,38,-1,126,-1,-19,0,16,-1,-68,-1,24,0,0,-1,124,0,0,-1,16,-1,-15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,-1,-14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-20,0,0,0,0,0,0,0,0,-1,-65,-1,-64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-40,0,0,-1,-16,0,0,0,0,-1,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-21,-1,-26,0,0,-1,-21,-1,-19,0,13,0,0,-1,-20,-1,-27,0,0,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-26,-1,-25,0,0,-1,-21,-1,-21,0,0,0,0,-1,-25,-1,-31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,0,0,0,17,0,0,0,14,0,0,
		-1,-46,0,0,-1,-47,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-20,
		0,0,0,0,-1,-20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-19,0,0,0,0,-1,-20,0,0,0,0,-1,-40,0,0,0,18,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,-1,-123,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-13,0,0,0,0,-1,-13,0,0,-1,118,-1,-11,0,0,0,15,0,0,0,0,0,0,-1,-58,0,0,0,0,0,0,-1,-31,
		0,0,-1,-26,0,0,0,0,0,0,0,0,0,0,-1,-55,-2,-68,-1,-39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-11,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-65,0,0,0,0,-1,-44,0,19,0,0,-1,-14,-1,123,-1,-54,-2,-19,-1,17,0,19,0,0,0,0,0,0,0,0,-1,-38,0,0,-2,-80,0,0,-1,113,-1,63,-1,59,0,0,0,0,0,0,0,0,-1,81,0,0,0,0,0,0,0,0,
		0,0,0,0,-1,-111,0,0,-1,-59,0,0,-1,-20,-1,-61,0,0,-1,-120,-1,-50,0,0,0,0,0,0,0,0,0,0,0,0,-1,-80,0,0,0,0,0,0,0,0,0,0,-1,-107,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-20,0,0,0,0,-1,-20,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-31,0,0,0,0,-1,-31,-1,-19,-1,-43,-1,-33,-1,-25,0,0,0,0,0,14,0,0,
		-1,-53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-123,0,0,0,0,0,0,0,0,-1,-60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-27,-1,-55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-24,0,0,0,0,0,0,0,0,-1,-13,0,0,0,0,0,0,-1,-44,-1,-13,0,0,-1,-46,-1,-28,-1,-75,-1,-46,-1,-39,-1,-11,0,0,0,0,0,0,-1,-76,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,31,0,0,0,0,
		0,0,0,0,-1,-37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,121,-1,-11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,-2,-11,-1,-83,0,0,0,0,0,0,0,0,-1,-16,0,0,0,0,-1,-64,-1,-55,0,0,0,0,0,0,-1,-11,0,0,0,0,0,0,-1,-56,0,0,0,0,-1,-25,0,0,-1,-21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,-1,86,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,68,-1,-67,-1,51,-1,68,-1,75,-1,62,-1,44,0,0,-1,114,0,0,0,7,0,7,0,0,-1,39,-1,-122,-1,-47,0,0,0,0,0,0,0,0,-1,106,0,5,0,0,
		0,0,-1,-110,-2,122,-1,15,0,0,0,7,0,0,-2,98,0,0,-1,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-20,0,0,0,0,0,0,0,0,-1,-76,-1,-69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-43,0,0,-1,-67,-1,-23,
		-1,-102,-1,-67,0,0,-1,-91,-1,-111,0,0,0,0,0,0,0,18,0,18,0,0,-1,-46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-54,-2,109,-1,-69,0,0,0,0,0,0,-1,-119,0,0,-1,-23,0,0,0,0,0,0,0,2,0,-102,0,6,0,6,0,0,0,11,0,11,0,1,0,16,0,16,0,2,
		0,18,0,18,0,3,0,37,0,41,0,4,0,44,0,52,0,9,0,56,0,62,0,18,0,69,0,71,0,25,0,73,0,73,0,28,0,76,0,76,0,29,0,81,0,84,0,30,0,86,0,86,0,34,0,90,0,90,0,35,0,92,0,94,0,36,0,-118,0,-118,0,39,0,-106,0,-106,0,40,0,-79,0,-76,0,41,0,-67,0,-67,0,45,
		0,-63,0,-63,0,46,0,-57,0,-57,0,47,0,-44,0,-43,0,48,0,-41,0,-41,0,50,0,-38,0,-38,0,51,0,-36,0,-34,0,52,0,-32,0,-26,0,55,0,-20,0,-20,0,62,0,-18,0,-18,0,63,0,-9,0,-9,0,64,0,-4,0,-4,0,65,0,-2,0,-1,0,66,1,4,1,5,0,68,1,10,1,10,0,70,1,13,1,13,0,71,
		1,24,1,26,0,72,1,46,1,48,0,75,1,51,1,53,0,78,1,55,1,55,0,81,1,57,1,57,0,82,1,59,1,59,0,83,1,67,1,68,0,84,1,84,1,84,0,86,1,86,1,86,0,87,1,88,1,88,0,88,1,92,1,94,0,89,1,-124,1,-118,0,92,1,-114,1,-113,0,99,1,-40,1,-40,0,101,1,-35,1,-35,0,102,
		1,-32,1,-31,0,103,1,-21,1,-19,0,105,1,-1,1,-1,0,108,2,14,2,16,0,109,2,48,2,48,0,112,2,51,2,51,0,113,2,69,2,69,0,114,2,71,2,72,0,115,2,122,2,123,0,117,2,125,2,125,0,119,2,127,2,-91,0,120,2,-86,2,-81,0,-97,2,-76,2,-60,0,-91,2,-58,2,-49,0,-74,2,-40,2,-38,0,-64,
		2,-36,2,-36,0,-61,2,-34,2,-34,0,-60,2,-32,2,-32,0,-59,2,-30,2,-30,0,-58,2,-27,2,-27,0,-57,2,-25,2,-25,0,-56,2,-23,2,-23,0,-55,2,-21,2,-21,0,-54,2,-19,2,-19,0,-53,2,-17,2,-17,0,-52,2,-15,2,-3,0,-51,2,-1,2,-1,0,-38,3,1,3,1,0,-37,3,3,3,3,0,-36,3,14,3,14,0,-35,
		3,16,3,16,0,-34,3,18,3,18,0,-33,3,20,3,20,0,-32,3,22,3,22,0,-31,3,24,3,24,0,-30,3,26,3,26,0,-29,3,28,3,28,0,-28,3,30,3,30,0,-27,3,32,3,32,0,-26,3,34,3,42,0,-25,3,47,3,56,0,-16,3,67,3,71,0,-6,3,77,3,79,0,-1,3,84,3,84,1,2,3,101,3,105,1,3,
		3,109,3,111,1,8,3,120,3,120,1,11,3,-122,3,-117,1,12,3,-114,3,-99,1,18,3,-96,3,-96,1,34,3,-92,3,-92,1,35,3,-90,3,-90,1,36,3,-86,3,-86,1,37,3,-83,3,-82,1,38,3,-80,3,-71,1,40,3,-69,3,-67,1,50,3,-65,3,-60,1,53,3,-58,3,-52,1,59,3,-46,3,-45,1,66,3,-43,3,-43,1,68,
		3,-41,3,-41,1,69,3,-39,3,-36,1,70,3,-33,3,-28,1,74,3,-26,3,-26,1,80,3,-22,3,-21,1,81,3,-16,3,-5,1,83,3,-2,3,-1,1,95,4,1,4,4,1,97,4,11,4,12,1,101,4,16,4,16,1,103,4,18,4,24,1,104,4,30,4,70,1,111,4,72,4,72,1,-104,4,74,4,87,1,-103,4,95,4,95,1,-89,
		4,98,4,98,1,-88,4,100,4,100,1,-87,4,112,4,117,1,-86,4,119,4,119,1,-80,4,123,4,124,1,-79,4,127,4,127,1,-77,4,-127,4,-126,1,-76,4,-124,4,-124,1,-74,4,-122,4,-122,1,-73,4,-105,4,-101,1,-72,4,-99,4,-99,1,-67,4,-97,4,-96,1,-66,4,-94,4,-94,1,-64,4,-90,4,-88,1,-63,4,-86,4,-86,1,-60,
		4,-84,4,-82,1,-59,4,-80,4,-80,1,-56,4,-78,4,-78,1,-55,4,-76,4,-70,1,-54,4,-68,4,-68,1,-47,4,-65,4,-65,1,-46,4,-63,4,-58,1,-45,4,-56,4,-53,1,-39,4,-49,4,-49,1,-35,4,-46,4,-46,1,-34,4,-40,4,-40,1,-33,4,-35,4,-35,1,-32,4,-24,4,-24,1,-31,4,-22,4,-22,1,-30,4,-15,4,-15,1,-29,
		4,-11,4,-11,1,-28,0,2,1,116,0,6,0,6,0,25,0,11,0,11,0,25,0,16,0,16,0,33,0,18,0,18,0,33,0,37,0,37,0,2,0,38,0,38,0,28,0,39,0,39,0,19,0,40,0,40,0,1,0,41,0,41,0,5,0,46,0,46,0,10,0,47,0,47,0,11,0,48,0,48,0,24,0,51,0,51,0,1,0,52,
		0,52,0,22,0,56,0,56,0,14,0,57,0,57,0,10,0,58,0,58,0,29,0,59,0,59,0,27,0,60,0,60,0,18,0,61,0,61,0,12,0,62,0,62,0,17,0,69,0,69,0,6,0,70,0,70,0,7,0,71,0,71,0,23,0,73,0,73,0,8,0,76,0,76,0,4,0,81,0,82,0,4,0,83,0,83,0,3,0,84,
		0,84,0,7,0,86,0,86,0,21,0,90,0,90,0,9,0,92,0,92,0,20,0,93,0,93,0,9,0,94,0,94,0,16,0,-118,0,-118,0,7,0,-106,0,-106,0,1,0,-79,0,-79,0,34,0,-78,0,-78,0,2,0,-77,0,-77,0,1,0,-76,0,-76,0,2,0,-67,0,-67,0,9,0,-63,0,-63,0,4,0,-57,0,-57,0,7,0,-44,
		0,-43,0,32,0,-38,0,-38,0,18,0,-34,0,-34,0,37,0,-28,0,-28,0,32,0,-26,0,-26,0,32,0,-20,0,-20,0,26,0,-18,0,-18,0,20,0,-9,0,-9,0,7,0,-4,0,-4,0,31,0,-2,0,-2,0,31,0,-1,0,-1,0,7,1,4,1,5,0,31,1,10,1,10,0,31,1,13,1,13,0,2,1,24,1,24,0,3,1,25,
		1,25,0,29,1,26,1,26,0,9,1,46,1,46,0,7,1,47,1,47,0,34,1,48,1,48,0,26,1,51,1,51,0,18,1,52,1,52,0,20,1,53,1,53,0,11,1,55,1,55,0,11,1,57,1,57,0,11,1,67,1,67,0,18,1,68,1,68,0,20,1,88,1,88,0,1,1,92,1,92,0,26,1,93,1,93,0,18,1,94,
		1,94,0,20,1,-124,1,-123,0,25,1,-122,1,-122,0,33,1,-121,1,-119,0,25,1,-118,1,-118,0,33,1,-114,1,-113,0,33,1,-40,1,-40,0,35,1,-35,1,-35,0,13,1,-32,1,-32,0,36,1,-31,1,-31,0,30,1,-21,1,-21,0,15,1,-20,1,-20,0,13,1,-19,1,-19,0,15,1,-1,1,-1,0,30,2,14,2,16,0,30,2,48,
		2,48,0,13,2,51,2,51,0,15,2,69,2,69,0,19,2,71,2,72,0,1,2,122,2,123,0,1,2,125,2,125,0,14,2,127,2,-123,0,2,2,-122,2,-122,0,19,2,-121,2,-118,0,5,2,-112,2,-108,0,1,2,-107,2,-104,0,10,2,-103,2,-103,0,12,2,-102,2,-96,0,6,2,-95,2,-95,0,23,2,-94,2,-91,0,8,2,-86,
		2,-86,0,4,2,-85,2,-81,0,3,2,-76,2,-75,0,9,2,-74,2,-74,0,2,2,-73,2,-73,0,6,2,-72,2,-72,0,2,2,-71,2,-71,0,6,2,-70,2,-70,0,2,2,-69,2,-69,0,6,2,-68,2,-68,0,19,2,-67,2,-67,0,23,2,-66,2,-66,0,19,2,-65,2,-65,0,23,2,-64,2,-64,0,19,2,-63,2,-63,0,23,2,-62,
		2,-62,0,19,2,-61,2,-61,0,23,2,-60,2,-60,0,1,2,-58,2,-58,0,5,2,-57,2,-57,0,8,2,-56,2,-56,0,5,2,-55,2,-55,0,8,2,-54,2,-54,0,5,2,-53,2,-53,0,8,2,-52,2,-52,0,5,2,-51,2,-51,0,8,2,-50,2,-50,0,5,2,-49,2,-49,0,8,2,-39,2,-39,0,4,2,-27,2,-27,0,10,2,-25,
		2,-25,0,11,2,-23,2,-23,0,24,2,-21,2,-21,0,24,2,-19,2,-19,0,24,2,-17,2,-17,0,24,2,-14,2,-14,0,4,2,-12,2,-12,0,4,2,-10,2,-9,0,4,2,-8,2,-8,0,1,2,-7,2,-7,0,3,2,-6,2,-6,0,1,2,-5,2,-5,0,3,2,-4,2,-4,0,1,2,-3,2,-3,0,3,2,-1,2,-1,0,21,3,1,
		3,1,0,21,3,3,3,3,0,21,3,14,3,14,0,14,3,16,3,16,0,14,3,18,3,18,0,14,3,20,3,20,0,10,3,22,3,22,0,10,3,24,3,24,0,10,3,26,3,26,0,10,3,28,3,28,0,10,3,30,3,30,0,10,3,32,3,32,0,27,3,34,3,34,0,12,3,35,3,35,0,9,3,36,3,36,0,12,3,37,
		3,37,0,17,3,38,3,38,0,16,3,39,3,39,0,17,3,40,3,40,0,16,3,41,3,41,0,17,3,42,3,42,0,16,3,47,3,48,0,13,3,49,3,49,0,35,3,50,3,56,0,15,3,67,3,71,0,13,3,77,3,79,0,15,3,84,3,84,0,13,3,101,3,101,0,30,3,102,3,105,0,36,3,109,3,111,0,13,3,120,
		3,120,0,35,3,-122,3,-122,0,2,3,-121,3,-121,0,5,3,-118,3,-118,0,1,3,-117,3,-117,0,12,3,-114,3,-114,0,2,3,-113,3,-113,0,28,3,-112,3,-112,0,5,3,-111,3,-111,0,17,3,-108,3,-108,0,11,3,-105,3,-105,0,1,3,-104,3,-104,0,22,3,-103,3,-103,0,14,3,-102,3,-102,0,12,3,-101,3,-101,0,18,3,-99,
		3,-99,0,12,3,-96,3,-96,0,4,3,-92,3,-92,0,3,3,-90,3,-90,0,9,3,-86,3,-86,0,3,3,-83,3,-83,0,5,3,-82,3,-82,0,34,3,-78,3,-78,0,10,3,-77,3,-76,0,11,3,-75,3,-75,0,37,3,-74,3,-74,0,2,3,-73,3,-73,0,28,3,-72,3,-72,0,34,3,-71,3,-71,0,5,3,-67,3,-67,0,1,3,-65,
		3,-65,0,22,3,-64,3,-64,0,19,3,-63,3,-63,0,14,3,-62,3,-62,0,18,3,-61,3,-61,0,6,3,-60,3,-60,0,8,3,-58,3,-58,0,3,3,-57,3,-57,0,7,3,-56,3,-56,0,23,3,-55,3,-55,0,9,3,-54,3,-54,0,20,3,-53,3,-53,0,8,3,-52,3,-52,0,26,3,-46,3,-46,0,9,3,-45,3,-45,0,27,3,-43,
		3,-43,0,27,3,-41,3,-41,0,27,3,-39,3,-39,0,12,3,-38,3,-38,0,9,3,-37,3,-36,0,25,3,-33,3,-33,0,25,3,-31,3,-31,0,4,3,-30,3,-30,0,2,3,-29,3,-29,0,6,3,-28,3,-28,0,5,3,-26,3,-26,0,8,3,-22,3,-22,0,29,3,-21,3,-21,0,9,3,-16,3,-16,0,19,3,-15,3,-15,0,23,3,-14,
		3,-14,0,12,3,-13,3,-13,0,9,3,-11,3,-11,0,18,3,-10,3,-10,0,20,3,-8,3,-8,0,2,3,-7,3,-7,0,6,3,-6,3,-6,0,2,3,-5,3,-5,0,6,3,-2,3,-2,0,5,3,-1,3,-1,0,8,4,1,4,2,0,8,4,3,4,3,0,18,4,4,4,4,0,20,4,11,4,11,0,1,4,12,4,12,0,3,4,16,
		4,16,0,3,4,18,4,18,0,7,4,19,4,19,0,37,4,20,4,20,0,9,4,21,4,21,0,37,4,22,4,22,0,9,4,23,4,23,0,37,4,24,4,24,0,9,4,30,4,30,0,2,4,31,4,31,0,6,4,32,4,32,0,2,4,33,4,33,0,6,4,34,4,34,0,2,4,35,4,35,0,6,4,36,4,36,0,2,4,37,
		4,37,0,6,4,38,4,38,0,2,4,39,4,39,0,6,4,40,4,40,0,2,4,41,4,41,0,6,4,42,4,42,0,2,4,43,4,43,0,6,4,44,4,44,0,2,4,45,4,45,0,6,4,46,4,46,0,2,4,47,4,47,0,6,4,48,4,48,0,2,4,49,4,49,0,6,4,50,4,50,0,2,4,51,4,51,0,6,4,52,
		4,52,0,2,4,53,4,53,0,6,4,54,4,54,0,5,4,55,4,55,0,8,4,56,4,56,0,5,4,57,4,57,0,8,4,58,4,58,0,5,4,59,4,59,0,8,4,60,4,60,0,5,4,61,4,61,0,8,4,62,4,62,0,5,4,63,4,63,0,8,4,64,4,64,0,5,4,65,4,65,0,8,4,66,4,66,0,5,4,67,
		4,67,0,8,4,68,4,68,0,5,4,69,4,69,0,8,4,74,4,74,0,1,4,75,4,75,0,3,4,76,4,76,0,1,4,77,4,77,0,3,4,78,4,78,0,1,4,79,4,79,0,3,4,80,4,80,0,1,4,81,4,81,0,3,4,82,4,82,0,1,4,83,4,83,0,3,4,84,4,84,0,1,4,85,4,85,0,3,4,86,
		4,86,0,1,4,87,4,87,0,3,4,95,4,95,0,3,4,98,4,98,0,10,4,100,4,100,0,10,4,112,4,112,0,12,4,113,4,113,0,9,4,114,4,114,0,12,4,115,4,115,0,9,4,116,4,116,0,12,4,117,4,117,0,9,4,119,4,119,0,14,4,123,4,123,0,34,4,124,4,124,0,26,4,127,4,127,0,4,4,-127,
		4,-127,0,32,4,-126,4,-126,0,34,4,-124,4,-124,0,11,4,-122,4,-122,0,12,4,-104,4,-104,0,4,4,-103,4,-103,0,2,4,-102,4,-102,0,6,4,-101,4,-101,0,5,4,-97,4,-97,0,1,4,-96,4,-96,0,3,4,-94,4,-94,0,21,4,-90,4,-90,0,28,4,-89,4,-89,0,7,4,-88,4,-88,0,1,4,-86,4,-86,0,1,4,-83,
		4,-83,0,4,4,-82,4,-82,0,11,4,-80,4,-80,0,11,4,-78,4,-78,0,24,4,-75,4,-75,0,4,4,-73,4,-73,0,4,4,-72,4,-72,0,1,4,-71,4,-71,0,22,4,-70,4,-70,0,7,4,-68,4,-68,0,21,4,-65,4,-65,0,14,4,-63,4,-63,0,10,4,-62,4,-62,0,29,4,-61,4,-61,0,9,4,-60,4,-60,0,29,4,-59,
		4,-59,0,9,4,-58,4,-58,0,27,4,-56,4,-56,0,17,4,-55,4,-55,0,16,4,-54,4,-54,0,1,4,-53,4,-53,0,15,4,-49,4,-49,0,13,4,-46,4,-46,0,15,4,-40,4,-40,0,30,4,-35,4,-35,0,35,4,-24,4,-24,0,30,4,-22,4,-22,0,15,4,-15,4,-15,0,13,4,-11,4,-11,0,35,0,1,0,6,4,-11,0,20,
		0,0,0,0,0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,26,0,31,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,35,0,0,0,0,0,0,0,0,0,2,
		0,0,0,2,0,0,0,16,0,11,0,10,0,29,0,22,0,17,0,12,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,4,0,3,0,1,0,0,0,14,0,0,0,5,0,9,0,0,0,21,0,9,0,15,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,2,0,1,0,0,0,0,0,0,0,2,0,1,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,2,0,6,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,0,0,0,9,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,2,0,25,0,0,0,11,0,0,0,0,0,0,0,17,0,0,0,0,0,25,0,34,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,3,0,0,0,21,0,0,0,3,0,3,0,27,0,3,0,3,0,3,0,0,0,1,0,3,0,33,0,3,0,3,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,1,0,27,0,3,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,2,0,4,0,29,0,9,0,2,0,0,0,2,0,1,0,2,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,0,21,0,0,0,3,0,0,0,0,0,11,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,2,
		0,1,0,17,0,21,0,11,0,0,0,32,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,27,0,0,0,3,0,0,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,3,0,17,0,21,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,0,2,0,1,0,0,0,0,0,0,
		0,25,0,27,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,31,0,0,0,20,0,20,0,26,0,20,0,20,0,20,0,26,0,0,0,0,0,0,0,26,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,28,0,36,0,0,0,18,0,24,0,30,0,0,0,8,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,13,0,8,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,24,0,8,
		0,0,0,0,0,24,0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,0,0,0,0,24,0,8,0,23,0,28,0,24,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,
		0,0,0,0,0,6,0,6,0,6,0,6,0,6,0,6,0,6,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,2,0,2,0,2,0,10,0,10,0,10,0,10,0,12,0,7,0,7,0,7,0,7,0,7,0,7,0,7,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,
		0,3,0,4,0,4,0,4,0,4,0,4,0,5,0,5,0,5,0,5,0,9,0,9,0,6,0,7,0,6,0,7,0,6,0,7,0,2,0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,2,0,1,0,2,0,1,0,2,0,1,0,2,
		0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,3,0,0,0,3,0,3,0,2,0,4,0,2,0,4,0,2,0,4,0,0,0,0,0,0,0,0,0,0,0,0,
		0,16,0,14,0,16,0,14,0,16,0,14,0,16,0,14,0,16,0,14,0,11,0,0,0,11,0,0,0,11,0,0,0,10,0,5,0,10,0,5,0,10,0,5,0,10,0,5,0,10,0,5,0,10,0,5,0,22,0,0,0,12,0,9,0,12,0,19,0,15,0,19,0,15,0,19,0,15,0,0,0,0,0,2,0,0,0,0,0,0,
		0,0,0,13,0,13,0,13,0,13,0,13,0,13,0,13,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8,0,8,0,8,0,8,0,18,0,18,0,18,0,18,0,23,0,13,0,13,0,13,0,8,0,8,0,8,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8,0,8,0,8,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,8,0,8,0,0,0,0,0,0,0,30,0,30,0,30,0,30,0,0,0,24,0,0,0,18,0,18,0,18,0,18,0,18,0,18,0,36,0,23,0,23,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,2,
		0,12,0,0,0,0,0,6,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,12,0,17,0,0,0,12,0,1,0,0,0,3,0,0,0,5,0,0,0,4,0,0,0,9,0,0,0,0,0,5,0,4,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,0,0,34,0,6,0,0,
		0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,11,0,17,0,7,0,1,0,3,0,4,0,3,0,1,0,9,0,21,0,1,0,3,0,14,0,0,0,0,0,0,0,3,0,9,0,22,0,0,0,22,0,0,0,22,0,0,0,12,0,9,0,20,0,20,0,0,0,0,0,20,0,0,0,3,0,6,0,7,0,0,
		0,0,0,1,0,3,0,0,0,0,0,29,0,9,0,1,0,2,0,0,0,0,0,2,0,1,0,12,0,9,0,0,0,17,0,21,0,0,0,6,0,7,0,6,0,7,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,17,0,21,0,0,0,0,0,0,0,3,0,0,0,3,0,2,0,4,0,2,0,1,0,2,0,4,0,0,
		0,0,0,34,0,9,0,34,0,9,0,34,0,9,0,32,0,33,0,0,0,3,0,1,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,6,0,7,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,
		0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,2,0,4,0,2,0,4,0,2,0,4,0,2,0,4,0,2,0,4,0,2,0,4,0,2,0,4,0,2,0,1,0,2,0,1,0,2,0,1,0,2,0,4,0,2,0,1,0,10,0,5,0,10,0,5,0,0,0,5,0,0,0,5,0,0,0,5,
		0,0,0,5,0,0,0,5,0,12,0,9,0,12,0,9,0,12,0,9,0,0,0,11,0,0,0,32,0,33,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		0,6,0,7,0,0,0,1,0,0,0,0,0,2,0,4,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,3,0,2,0,0,0,0,0,0,0,0,0,16,0,14,0,11,0,0,0,10,0,29,0,9,0,29,0,9,
		0,22,0,0,0,19,0,15,0,0,0,13,0,0,0,0,0,0,0,8,0,23,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,28,0,0,0,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,
		0,0,0,8,0,24,0,28,0,0,0,0,0,8,0,23,0,1,0,0,0,10,1,98,2,-110,0,4,68,70,76,84,0,26,99,121,114,108,0,26,103,114,101,107,0,26,108,97,116,110,0,72,0,4,0,0,0,0,-1,-1,0,18,0,0,0,1,0,2,0,3,0,4,0,8,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,
		0,20,0,21,0,22,0,23,0,46,0,7,65,90,69,32,0,-28,67,82,84,32,0,-28,70,82,65,32,0,90,77,79,76,32,0,-74,78,65,86,32,0,-120,82,79,77,32,0,-74,84,82,75,32,0,-28,0,0,-1,-1,0,19,0,0,0,1,0,2,0,3,0,4,0,7,0,8,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,
		0,20,0,21,0,22,0,23,0,0,-1,-1,0,20,0,0,0,1,0,2,0,3,0,4,0,6,0,8,0,9,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,0,20,0,21,0,22,0,23,0,0,-1,-1,0,20,0,0,0,1,0,2,0,3,0,4,0,6,0,8,0,11,0,12,0,13,0,14,0,15,0,16,0,17,0,18,
		0,19,0,20,0,21,0,22,0,23,0,0,-1,-1,0,20,0,0,0,1,0,2,0,3,0,4,0,6,0,8,0,10,0,12,0,13,0,14,0,15,0,16,0,17,0,18,0,19,0,20,0,21,0,22,0,23,0,0,-1,-1,0,19,0,0,0,1,0,2,0,3,0,4,0,5,0,8,0,12,0,13,0,14,0,15,0,16,0,17,0,18,
		0,19,0,20,0,21,0,22,0,23,0,24,99,50,115,99,0,-110,99,99,109,112,0,-104,100,108,105,103,0,-96,100,110,111,109,0,-90,102,114,97,99,0,-84,108,105,103,97,0,-74,108,105,103,97,0,-68,108,105,103,97,0,-56,108,110,117,109,0,-48,108,111,99,108,0,-42,108,111,99,108,0,-36,108,111,99,108,0,-30,110,117,109,114,0,-24,
		111,110,117,109,0,-18,112,110,117,109,0,-12,115,109,99,112,0,-6,115,115,48,49,1,0,115,115,48,50,1,6,115,115,48,51,1,12,115,115,48,52,1,18,115,115,48,53,1,24,115,115,48,54,1,30,115,115,48,55,1,36,116,110,117,109,1,42,0,0,0,1,0,0,0,0,0,2,0,2,0,4,0,0,0,1,0,10,0,0,0,1,
		0,24,0,0,0,3,0,22,0,23,0,25,0,0,0,1,0,9,0,0,0,4,0,8,0,9,0,8,0,9,0,0,0,2,0,8,0,9,0,0,0,1,0,21,0,0,0,1,0,7,0,0,0,1,0,5,0,0,0,1,0,6,0,0,0,1,0,25,0,0,0,1,0,18,0,0,0,1,0,19,0,0,0,1,0,1,0,0,0,1,
		0,11,0,0,0,1,0,12,0,0,0,1,0,13,0,0,0,1,0,14,0,0,0,1,0,15,0,0,0,1,0,16,0,0,0,1,0,17,0,0,0,1,0,20,0,26,0,54,4,48,7,-18,8,-96,8,-54,15,110,15,-124,15,-82,15,-62,15,-26,16,16,16,76,16,96,16,116,16,-120,16,-102,16,-76,16,-10,17,20,17,102,17,-84,18,14,
		18,108,18,-128,18,-80,18,-46,0,1,0,0,0,1,0,8,0,2,1,-6,0,-6,1,-25,2,113,1,-47,1,-48,1,-49,1,-50,1,-51,1,-52,1,-53,1,-54,1,-55,1,-56,2,51,2,50,2,49,2,48,2,40,1,-26,1,-27,1,-28,1,-29,1,-30,1,-31,1,-32,1,-33,1,-34,1,-35,1,-36,1,-37,1,-38,1,-39,1,-40,1,-41,1,-42,
		1,-43,1,-44,1,-45,1,-46,1,-24,1,-23,2,115,2,117,2,116,2,118,2,114,2,119,2,82,1,-22,1,-21,1,-20,1,-19,1,-18,1,-17,1,-16,1,-15,1,-14,1,-13,1,-12,1,-11,1,-10,1,-9,1,-8,1,-7,1,-6,1,-5,1,-4,1,-3,1,-2,2,0,2,1,4,-2,2,2,2,3,2,4,2,5,2,6,2,7,2,8,2,9,
		2,10,2,11,2,59,2,13,2,14,2,15,2,16,4,-8,2,17,2,19,2,20,2,21,2,22,2,23,2,24,2,25,2,27,2,28,2,30,2,29,3,47,3,48,3,49,3,50,3,51,3,52,3,53,3,54,3,55,3,56,3,57,3,58,3,59,3,60,3,61,3,62,3,63,3,64,3,65,3,66,3,67,3,68,3,69,3,70,3,71,
		3,72,3,73,3,74,3,75,3,76,3,77,3,78,3,79,3,80,3,81,3,82,3,83,3,84,3,85,3,86,3,87,3,88,3,89,3,90,3,91,3,92,3,93,3,94,3,95,3,96,3,97,3,98,3,99,4,-1,3,100,3,101,3,102,3,103,3,104,3,105,3,106,3,107,3,108,3,109,3,110,3,111,3,112,3,113,3,114,3,115,
		3,116,3,117,5,2,3,118,3,119,3,121,3,120,3,122,3,123,3,124,3,125,3,126,3,127,3,-128,3,-127,3,-126,3,-125,3,-124,3,-123,5,0,5,1,4,-53,4,-52,4,-51,4,-50,4,-49,4,-48,4,-47,4,-46,4,-45,4,-44,4,-43,4,-42,4,-41,4,-40,4,-39,4,-38,4,-37,4,-36,4,-35,4,-34,4,-33,4,-32,4,-31,4,-30,
		4,-29,4,-28,4,-27,4,-26,4,-25,1,-1,4,-24,4,-23,4,-22,4,-21,4,-20,4,-19,4,-18,4,-17,4,-16,4,-15,4,-14,4,-13,4,-12,4,-11,4,-10,5,3,5,4,5,5,5,6,4,-9,4,-7,4,-6,4,-4,2,26,4,-3,4,-5,2,12,2,18,5,11,5,12,0,1,0,-6,0,8,0,10,0,20,0,21,0,22,0,23,0,24,
		0,25,0,26,0,27,0,28,0,29,0,37,0,38,0,39,0,40,0,41,0,42,0,43,0,44,0,45,0,46,0,47,0,48,0,49,0,50,0,51,0,52,0,53,0,54,0,55,0,56,0,57,0,58,0,59,0,60,0,61,0,62,0,101,0,103,0,-127,0,-125,0,-124,0,-116,0,-113,0,-111,0,-109,0,-79,0,-78,0,-77,0,-76,0,-75,
		0,-74,0,-73,0,-72,0,-71,0,-70,0,-46,0,-45,0,-44,0,-43,0,-42,0,-41,0,-40,0,-39,0,-38,0,-37,0,-36,0,-35,0,-34,0,-33,0,-32,0,-31,0,-30,0,-29,0,-28,0,-27,0,-26,0,-25,0,-24,0,-23,1,47,1,51,1,53,1,55,1,57,1,59,1,65,1,67,1,69,1,73,1,75,1,76,1,88,1,89,1,-105,1,-99,
		1,-94,1,-91,2,122,2,123,2,125,2,127,2,-128,2,-127,2,-126,2,-125,2,-124,2,-123,2,-122,2,-121,2,-120,2,-119,2,-118,2,-117,2,-116,2,-115,2,-114,2,-113,2,-112,2,-111,2,-110,2,-109,2,-108,2,-107,2,-106,2,-105,2,-104,2,-103,2,-74,2,-72,2,-70,2,-68,2,-66,2,-64,2,-62,2,-60,2,-58,2,-56,2,-54,2,-52,2,-50,
		2,-48,2,-46,2,-44,2,-42,2,-40,2,-38,2,-36,2,-34,2,-32,2,-30,2,-29,2,-27,2,-25,2,-23,2,-21,2,-19,2,-17,2,-15,2,-13,2,-11,2,-8,2,-6,2,-4,2,-2,3,0,3,2,3,4,3,6,3,8,3,10,3,12,3,14,3,16,3,18,3,20,3,22,3,24,3,26,3,28,3,30,3,32,3,34,3,36,3,37,3,39,
		3,41,3,43,3,45,3,-122,3,-121,3,-120,3,-119,3,-118,3,-117,3,-116,3,-114,3,-113,3,-112,3,-111,3,-110,3,-109,3,-108,3,-107,3,-106,3,-105,3,-104,3,-103,3,-102,3,-101,3,-100,3,-99,3,-83,3,-82,3,-81,3,-80,3,-79,3,-78,3,-77,3,-76,3,-75,3,-74,3,-73,3,-72,3,-71,3,-70,3,-69,3,-68,3,-67,3,-66,3,-65,
		3,-64,3,-63,3,-62,3,-45,3,-43,3,-41,3,-39,3,-18,3,-16,3,-14,4,7,4,13,4,19,4,125,4,-126,4,-122,5,7,5,9,0,1,0,0,0,1,0,8,0,2,1,-36,0,-21,2,113,2,51,2,50,2,49,2,48,2,40,1,-26,1,-27,1,-28,1,-29,1,-30,1,-31,1,-32,1,-33,1,-34,1,-35,1,-36,1,-37,1,-38,1,-39,
		1,-40,1,-41,1,-42,1,-43,1,-44,1,-45,1,-46,2,100,2,115,3,48,2,117,2,116,3,47,1,-29,2,114,2,119,2,82,4,-46,4,-45,1,-22,1,-21,4,-44,4,-43,4,-42,1,-20,4,-41,1,-19,1,-18,1,-17,4,-36,1,-16,1,-16,4,-35,4,-34,1,-15,1,-14,1,-13,1,-6,4,-21,4,-20,1,-5,1,-4,1,-3,1,-2,1,-1,
		2,0,4,-17,4,-16,4,-14,4,-11,4,-2,2,2,2,3,2,4,2,5,2,6,2,7,2,8,2,9,2,10,2,11,1,-12,1,-11,1,-10,1,-9,1,-8,1,-7,2,59,2,13,2,14,2,15,2,16,4,-8,2,17,2,19,2,20,2,21,2,23,2,25,2,118,3,49,3,50,3,51,3,52,3,53,3,54,3,55,3,56,3,57,3,58,
		3,59,3,60,3,61,3,62,3,63,3,64,3,65,3,66,3,67,3,68,3,69,3,70,3,71,3,72,3,73,3,74,3,75,3,76,3,-126,3,77,3,78,3,79,3,80,3,81,3,82,3,83,3,84,3,85,3,86,3,87,3,88,3,89,3,90,3,91,3,92,3,93,3,94,3,95,3,96,3,97,3,98,4,-1,3,100,3,101,3,102,
		3,103,3,104,3,105,3,106,3,107,3,108,3,109,3,110,3,111,3,112,3,113,3,114,3,115,3,116,3,117,5,2,3,118,3,119,3,121,3,120,3,122,3,123,3,124,3,125,3,126,3,127,3,-128,3,-127,3,-125,3,-124,3,-123,5,0,5,1,4,-53,4,-52,4,-51,4,-50,4,-40,4,-37,4,-39,4,-38,4,-33,4,-32,4,-31,4,-49,
		4,-48,4,-47,4,-22,4,-19,4,-18,4,-15,4,-13,4,-12,2,1,4,-10,4,-30,4,-29,4,-28,4,-27,4,-26,4,-25,4,-24,4,-23,5,3,5,4,5,5,5,6,4,-9,4,-7,4,-6,2,24,4,-4,2,26,4,-3,4,-5,2,22,2,12,2,18,5,11,5,12,0,1,0,-21,0,10,0,69,0,70,0,71,0,72,0,73,0,74,0,75,
		0,76,0,77,0,78,0,79,0,80,0,81,0,82,0,83,0,84,0,85,0,86,0,87,0,88,0,89,0,90,0,91,0,92,0,93,0,94,0,-123,0,-122,0,-121,0,-119,0,-118,0,-117,0,-115,0,-112,0,-110,0,-108,0,-69,0,-68,0,-67,0,-66,0,-65,0,-64,0,-63,0,-62,0,-61,0,-60,0,-59,0,-58,0,-57,0,-56,0,-55,0,-54,
		0,-53,0,-52,0,-51,0,-50,0,-22,0,-21,0,-20,0,-19,0,-18,0,-17,0,-16,0,-15,0,-14,0,-13,0,-12,0,-11,0,-10,0,-9,0,-8,0,-7,0,-6,0,-5,0,-4,0,-3,0,-2,0,-1,1,0,1,1,1,2,1,3,1,4,1,5,1,6,1,7,1,48,1,52,1,54,1,56,1,58,1,60,1,66,1,68,1,70,1,74,1,77,
		1,90,2,124,2,126,2,-102,2,-101,2,-100,2,-99,2,-98,2,-97,2,-96,2,-95,2,-94,2,-93,2,-92,2,-91,2,-90,2,-89,2,-88,2,-87,2,-86,2,-85,2,-84,2,-83,2,-82,2,-81,2,-80,2,-79,2,-78,2,-77,2,-76,2,-75,2,-73,2,-71,2,-69,2,-67,2,-65,2,-63,2,-61,2,-59,2,-57,2,-55,2,-53,2,-51,2,-49,2,-47,
		2,-45,2,-43,2,-41,2,-39,2,-37,2,-35,2,-33,2,-31,2,-28,2,-26,2,-24,2,-22,2,-20,2,-18,2,-16,2,-14,2,-12,2,-10,2,-7,2,-5,2,-3,2,-1,3,1,3,3,3,5,3,7,3,9,3,11,3,13,3,15,3,17,3,19,3,21,3,23,3,25,3,27,3,29,3,31,3,33,3,35,3,38,3,40,3,42,3,44,3,46,
		3,-98,3,-97,3,-96,3,-95,3,-93,3,-92,3,-91,3,-90,3,-89,3,-88,3,-87,3,-86,3,-85,3,-84,3,-61,3,-60,3,-59,3,-58,3,-57,3,-56,3,-55,3,-54,3,-53,3,-52,3,-51,3,-50,3,-49,3,-48,3,-47,3,-46,3,-44,3,-42,3,-40,3,-38,3,-17,3,-15,3,-13,4,1,4,8,4,14,4,20,4,126,4,127,4,-125,4,-121,
		5,8,5,10,0,6,0,0,0,6,0,18,0,42,0,66,0,90,0,114,0,-118,0,3,0,0,0,1,0,18,0,1,0,-112,0,1,0,0,0,3,0,1,0,1,0,77,0,3,0,0,0,1,0,18,0,1,0,120,0,1,0,0,0,3,0,1,0,1,0,78,0,3,0,0,0,1,0,18,0,1,0,96,0,1,0,0,0,3,0,1,
		0,1,2,-31,0,3,0,0,0,1,0,18,0,1,0,72,0,1,0,0,0,3,0,1,0,1,3,-50,0,3,0,0,0,1,0,18,0,1,0,48,0,1,0,0,0,3,0,1,0,1,3,-48,0,3,0,0,0,1,0,18,0,1,0,24,0,1,0,0,0,3,0,1,0,1,4,73,0,2,0,2,0,-88,0,-84,0,0,1,36,1,39,
		0,5,0,1,0,0,0,1,0,8,0,2,0,18,0,6,2,97,2,95,2,98,2,99,2,96,5,13,0,1,0,6,0,77,0,78,2,-31,3,-50,3,-48,4,73,0,4,0,0,0,1,0,8,0,1,6,50,0,54,0,114,0,-92,0,-82,0,-72,0,-54,0,-4,1,14,1,24,1,74,1,100,1,126,1,-112,1,-70,1,-10,2,0,2,34,
		2,60,2,78,2,-118,2,-100,2,-74,2,-32,2,-14,3,36,3,46,3,56,3,74,3,124,3,-122,3,-112,3,-102,3,-76,3,-50,3,-32,4,10,4,60,4,70,4,104,4,-126,4,-108,4,-58,4,-40,4,-14,5,28,5,46,5,56,5,66,5,76,5,86,5,-128,5,-86,5,-44,5,-2,6,40,0,6,0,14,0,20,0,26,0,32,0,38,0,44,
		2,-128,0,2,0,-87,4,30,0,2,0,-83,2,127,0,2,0,-88,4,32,0,2,0,-85,2,-126,0,2,0,-86,4,-103,0,2,0,-84,0,1,0,4,4,-90,0,2,0,-83,0,1,0,4,2,-68,0,2,0,-87,0,2,0,6,0,12,4,-86,0,2,1,-70,4,-88,0,2,0,-83,0,6,0,14,0,20,0,26,0,32,0,38,0,44,2,-120,
		0,2,0,-87,4,54,0,2,0,-83,2,-121,0,2,0,-88,4,56,0,2,0,-85,4,58,0,2,0,-86,4,-101,0,2,0,-84,0,2,0,6,0,12,4,-107,0,2,0,-87,2,-42,0,2,1,-70,0,1,0,4,4,-84,0,2,0,-83,0,6,0,14,0,20,0,26,0,32,0,38,0,44,2,-116,0,2,0,-87,4,72,0,2,0,-83,2,-117,
		0,2,0,-88,4,70,0,2,0,-85,2,-38,0,2,0,-86,4,-99,0,2,0,-84,0,3,0,8,0,14,0,20,4,-82,0,2,0,-87,2,-25,0,2,1,-70,4,-80,0,2,0,-83,0,3,0,8,0,14,0,20,2,-23,0,2,0,-87,2,-21,0,2,1,-70,4,-78,0,2,0,-83,0,2,0,6,0,12,3,-32,0,2,0,-87,4,-76,0,2,
		0,-83,0,5,0,12,0,18,0,24,0,30,0,36,2,-15,0,2,0,-87,2,-13,0,2,1,-70,4,-74,0,2,0,-83,4,-105,0,2,0,-88,2,-113,0,2,0,-86,0,7,0,16,0,24,0,30,0,36,0,42,0,48,0,54,4,-72,0,3,0,-86,0,-87,2,-111,0,2,0,-87,4,74,0,2,0,-83,2,-112,0,2,0,-88,4,76,0,2,
		0,-85,2,-109,0,2,0,-86,4,-97,0,2,0,-84,0,1,0,4,4,-71,0,2,0,-87,0,4,0,10,0,16,0,22,0,28,2,-2,0,2,0,-87,3,0,0,2,1,-70,4,-69,0,2,0,-83,4,-95,0,2,0,-84,0,3,0,8,0,14,0,20,3,4,0,2,0,-87,3,10,0,2,1,-70,4,-67,0,2,0,-83,0,2,0,6,0,12,
		3,14,0,2,1,-70,4,-65,0,2,0,-83,0,7,0,16,0,24,0,30,0,36,0,42,0,48,0,54,4,-63,0,3,0,-86,0,-87,2,-106,0,2,0,-87,4,98,0,2,0,-83,2,-107,0,2,0,-88,4,100,0,2,0,-85,3,20,0,2,0,-86,4,-93,0,2,0,-84,0,2,0,6,0,12,4,-60,0,2,0,-83,4,-62,0,2,0,-86,
		0,3,0,8,0,14,0,20,3,-43,0,2,0,-87,4,-58,0,2,0,-83,3,-45,0,2,0,-88,0,5,0,12,0,18,0,24,0,30,0,36,2,-103,0,2,0,-87,4,112,0,2,0,-83,3,-39,0,2,0,-88,4,114,0,2,0,-85,4,116,0,2,0,-86,0,2,0,6,0,12,3,37,0,2,0,-87,4,-56,0,2,0,-83,0,6,0,14,
		0,20,0,26,0,32,0,38,0,44,2,-101,0,2,0,-87,4,31,0,2,0,-83,2,-102,0,2,0,-88,4,33,0,2,0,-85,2,-99,0,2,0,-86,4,-102,0,2,0,-84,0,1,0,4,4,-89,0,2,0,-83,0,1,0,4,2,-67,0,2,0,-87,0,2,0,6,0,12,4,-85,0,2,1,-70,4,-87,0,2,0,-83,0,6,0,14,0,20,
		0,26,0,32,0,38,0,44,2,-93,0,2,0,-87,4,55,0,2,0,-83,2,-94,0,2,0,-88,4,57,0,2,0,-85,4,59,0,2,0,-86,4,-100,0,2,0,-84,0,1,0,4,4,-106,0,2,0,-87,0,1,0,4,4,-83,0,2,0,-83,0,1,0,4,4,73,0,2,0,-83,0,3,0,8,0,14,0,20,4,-81,0,2,0,-87,2,-24,
		0,2,1,-70,4,-79,0,2,0,-83,0,3,0,8,0,14,0,20,2,-22,0,2,0,-87,2,-20,0,2,1,-70,4,-77,0,2,0,-83,0,2,0,6,0,12,3,-31,0,2,0,-87,4,-75,0,2,0,-83,0,5,0,12,0,18,0,24,0,30,0,36,2,-14,0,2,0,-87,2,-12,0,2,1,-70,4,-73,0,2,0,-83,4,-104,0,2,0,-88,
		2,-86,0,2,0,-86,0,6,0,14,0,20,0,26,0,32,0,38,0,44,2,-84,0,2,0,-87,4,75,0,2,0,-83,2,-85,0,2,0,-88,4,77,0,2,0,-85,2,-82,0,2,0,-86,4,-96,0,2,0,-84,0,1,0,4,4,-70,0,2,0,-87,0,4,0,10,0,16,0,22,0,28,2,-1,0,2,0,-87,3,1,0,2,1,-70,4,-68,
		0,2,0,-83,4,-94,0,2,0,-84,0,3,0,8,0,14,0,20,3,5,0,2,0,-87,3,11,0,2,1,-70,4,-66,0,2,0,-83,0,2,0,6,0,12,3,15,0,2,1,-70,4,-64,0,2,0,-83,0,6,0,14,0,20,0,26,0,32,0,38,0,44,2,-79,0,2,0,-87,4,99,0,2,0,-83,2,-80,0,2,0,-88,4,101,0,2,
		0,-85,3,21,0,2,0,-86,4,-92,0,2,0,-84,0,2,0,6,0,12,4,-59,0,2,0,-83,4,-61,0,2,0,-86,0,3,0,8,0,14,0,20,3,-42,0,2,0,-87,4,-57,0,2,0,-83,3,-44,0,2,0,-88,0,5,0,12,0,18,0,24,0,30,0,36,2,-76,0,2,0,-87,4,113,0,2,0,-83,3,-38,0,2,0,-88,4,115,
		0,2,0,-85,4,117,0,2,0,-86,0,2,0,6,0,12,3,38,0,2,0,-87,4,-55,0,2,0,-83,0,1,0,4,3,43,0,2,0,-87,0,1,0,4,3,45,0,2,0,-87,0,1,0,4,3,44,0,2,0,-87,0,1,0,4,3,46,0,2,0,-87,0,5,0,12,0,18,0,24,0,30,0,36,2,-89,0,2,0,-87,2,-90,0,2,
		0,-88,4,71,0,2,0,-85,2,-37,0,2,0,-86,4,-98,0,2,0,-84,0,5,0,12,0,18,0,24,0,30,0,36,4,88,0,2,0,-87,4,96,0,2,0,-83,4,90,0,2,0,-88,4,92,0,2,0,-85,4,94,0,2,0,-86,0,5,0,12,0,18,0,24,0,30,0,36,4,89,0,2,0,-87,4,97,0,2,0,-83,4,91,0,2,
		0,-88,4,93,0,2,0,-85,4,95,0,2,0,-86,0,5,0,12,0,18,0,24,0,30,0,36,4,102,0,2,0,-87,4,110,0,2,0,-83,4,104,0,2,0,-88,4,106,0,2,0,-85,4,108,0,2,0,-86,0,5,0,12,0,18,0,24,0,30,0,36,4,103,0,2,0,-87,4,111,0,2,0,-83,4,105,0,2,0,-88,4,107,0,2,
		0,-85,4,109,0,2,0,-86,0,1,0,4,4,-91,0,2,0,-87,0,2,0,17,0,37,0,41,0,0,0,43,0,45,0,5,0,47,0,52,0,8,0,54,0,59,0,14,0,61,0,62,0,20,0,69,0,73,0,22,0,75,0,77,0,27,0,79,0,84,0,30,0,86,0,91,0,36,0,93,0,94,0,42,0,-127,0,-127,0,44,0,-125,
		0,-125,0,45,0,-122,0,-122,0,46,0,-119,0,-119,0,47,0,-115,0,-115,0,48,0,-104,0,-101,0,49,0,-48,0,-48,0,53,0,1,0,0,0,1,0,8,0,1,0,6,0,2,0,1,0,2,3,8,3,9,0,1,0,0,0,1,0,8,0,2,0,18,0,6,5,7,5,8,5,9,5,10,5,11,5,12,0,1,0,6,2,-70,2,-69,
		2,-52,2,-51,3,79,3,88,0,1,0,0,0,1,0,8,0,1,0,6,0,1,0,1,0,1,1,123,0,4,0,0,0,1,0,8,0,1,0,64,0,1,0,8,0,2,0,6,0,14,1,-66,0,3,0,74,0,77,1,-68,0,2,0,77,0,4,0,0,0,1,0,8,0,1,0,28,0,1,0,8,0,2,0,6,0,14,1,-65,0,3,
		0,74,0,80,1,-67,0,2,0,80,0,1,0,1,0,74,0,4,0,0,0,1,0,8,0,1,0,42,0,3,0,12,0,22,0,32,0,1,0,4,1,-69,0,2,0,74,0,1,0,4,1,-63,0,2,0,88,0,1,0,4,1,-64,0,2,0,88,0,1,0,3,0,74,0,87,0,-107,0,1,0,0,0,1,0,8,0,1,0,6,1,-34,
		0,1,0,1,0,75,0,1,0,0,0,1,0,8,0,1,0,6,1,111,0,1,0,1,0,-69,0,1,0,0,0,1,0,8,0,1,0,6,1,-11,0,1,0,1,0,54,0,1,0,0,0,1,0,8,0,2,0,28,0,2,2,44,2,45,0,1,0,0,0,1,0,8,0,2,0,10,0,2,2,46,2,47,0,1,0,2,0,47,0,79,
		0,1,0,0,0,1,0,8,0,2,0,30,0,12,2,69,2,71,2,70,2,72,2,73,2,103,2,104,2,105,2,106,2,107,2,108,2,109,0,1,0,12,0,39,0,40,0,43,0,51,0,53,0,70,0,71,0,72,0,75,0,83,0,84,0,85,0,1,0,0,0,1,0,8,0,2,0,12,0,3,2,110,2,111,2,111,0,1,0,3,
		0,73,0,75,2,106,0,1,0,0,0,1,0,8,0,2,0,46,0,20,2,90,2,94,2,88,2,85,2,87,2,86,2,91,2,89,2,93,2,92,2,79,2,74,2,75,2,76,2,77,2,78,0,26,0,28,2,83,2,101,0,2,0,4,0,20,0,29,0,0,2,102,2,102,0,10,2,112,2,112,0,11,4,-115,4,-108,0,12,0,1,
		0,0,0,1,0,8,0,2,0,46,0,20,4,-108,2,112,4,-115,4,-114,4,-113,4,-112,4,-111,2,102,4,-110,4,-109,2,76,2,78,2,77,2,75,2,79,2,101,0,26,2,83,0,28,2,74,0,2,0,2,0,20,0,29,0,0,2,85,2,94,0,10,0,1,0,0,0,1,0,8,0,2,0,46,0,20,2,91,2,93,2,94,2,88,
		2,85,2,87,2,86,2,89,2,92,2,90,0,27,0,21,0,22,0,23,0,24,0,25,0,26,0,28,0,29,0,20,0,1,0,20,0,26,0,28,2,74,2,75,2,76,2,77,2,78,2,79,2,83,2,101,2,102,2,112,4,-115,4,-114,4,-113,4,-112,4,-111,4,-110,4,-109,4,-108,0,1,0,0,0,1,0,8,0,2,0,46,0,20,
		4,-111,4,-110,2,112,4,-115,4,-114,4,-113,4,-112,2,102,4,-109,0,23,0,25,0,24,0,22,0,27,0,20,0,26,0,29,0,28,0,21,4,-108,0,2,0,6,0,26,0,26,0,0,0,28,0,28,0,1,2,74,2,79,0,2,2,83,2,83,0,8,2,85,2,94,0,9,2,101,2,101,0,19,0,1,0,0,0,1,0,8,0,1,
		0,6,1,-127,0,1,0,1,0,19,0,6,0,0,0,1,0,8,0,3,0,1,0,18,0,1,0,108,0,0,0,1,0,0,0,24,0,2,0,3,1,-108,1,-108,0,0,1,-59,1,-57,0,1,2,31,2,37,0,4,0,1,0,0,0,1,0,8,0,2,0,60,0,10,1,-57,1,-58,1,-59,2,31,2,32,2,33,2,34,2,35,2,36,
		2,37,0,1,0,0,0,1,0,8,0,2,0,26,0,10,2,62,0,122,0,115,0,116,2,63,2,64,2,65,2,66,2,67,2,68,0,2,0,1,0,20,0,29,0,0,0
	};
	uint32_t const FONT_SIZE_ROBOTO_REGULAR = 171676;
	const char* const FONT_NAME_ROBOTO_REGULAR = "Roboto";
}
