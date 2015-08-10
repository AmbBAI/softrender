#ifndef _MATH_VECTOR2_H_
#define _MATH_VECTOR2_H_

#include "base/header.h"

namespace sr
{

struct Vector2
{
	union
	{
		float f[2];
        struct { float x, y; };
	};

	Vector2() = default;
    Vector2(float _x, float _y): x(_x), y(_y) {}

	inline float Length() const;
	inline float SqrLength() const;
	inline Vector2 Normalize() const;
	inline float Dot(const Vector2& v) const;

	inline Vector2 operator +() const { return *this; }
	inline Vector2 operator -() const { return Vector2(-x, -y); }
	inline Vector2 operator +(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
	inline Vector2 operator -(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
	inline Vector2 operator *(float f) const { return Vector2(x * f, y * f); }
	inline Vector2 operator *(const Vector2& v) const { return Vector2(x * v.x, y * v.y); }
	inline Vector2 operator /(float f) const { return this->operator*(1.f / f); }

	static inline float Dot(const Vector2& a, const Vector2& b);

	static inline Vector2 LinearInterp(const Vector2& a, const Vector2& b, float t);
	static inline Vector2 TriangleInterp(const Vector2& a, const Vector2& b, const Vector2& c, float t0, float t1, float t2);

	static const Vector2 zero;
	static const Vector2 one;
};

}

#include "vector2.inl"

#endif //!_MATH_VECTOR2_H_