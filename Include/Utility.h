#pragma once
#include <math.h>

#ifndef MY_UTILITY
#define MY_UTILITY

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;



class Vector2 final {
public:
	Vector2() noexcept = default;
	Vector2(float value) noexcept 
		:	x(value), y(value)
	{
	}
	Vector2(float x, float y) noexcept
		: x(x), y(y)
	{
	}

	Vector2(const Vector2& other) = default;
	Vector2& operator=(const Vector2& other) noexcept = default;

	Vector2(Vector2&& other) noexcept = default;
	Vector2& operator=(Vector2&& other) noexcept = default;


	Vector2 operator-(const Vector2& other) noexcept {
		return Vector2(this->x - other.x, this->y - other.y);
	}


	bool operator==(const Vector2& other) const noexcept {
		if (this->x != other.x)
			return false;
		if (this->y != other.y)
			return false;

		return true;
	}
	bool operator!=(const Vector2& other) const noexcept {
		return !(*this == other);
	}


public:
	float x{ 0.0f };
	float y{ 0.0f };
};


namespace {
	Vector2 operator-(const Vector2& lhs, const Vector2& rhs) noexcept {
		return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
	}


	static inline float Distance(const Vector2& lhs, const Vector2& rhs) noexcept {
		Vector2 ResultingVector = rhs - lhs;
		return sqrtf(((ResultingVector.x * ResultingVector.x) + (ResultingVector.y * ResultingVector.y)) * 1.0f);
	}


}





#endif // !MY_UTILITY
