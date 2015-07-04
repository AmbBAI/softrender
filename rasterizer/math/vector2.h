#ifndef _BASE_VECTOR2_H_
#define _BASE_VECTOR2_H_

#include "base/header.h"

namespace rasterizer
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
	inline const Vector2 Normalize() const;
	inline const Vector2 Negate() const;
	inline float Dot(const Vector2& v) const;

	inline const Vector2 operator +(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
	inline const Vector2 operator -(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
	inline const Vector2 operator *(float f) const { return Vector2(x * f, y * f); }
	inline const Vector2 operator /(float f) const { return this->operator*(1.f / f); }

	static inline const float Dot(const Vector2& a, const Vector2& b);
	//static inline const Vector2 Cross(const Vector2& a, const Vector2& b);

	static inline const Vector2 LinearInterp(const Vector2& a, const Vector2& b, float t);
	static inline const Vector2 TriangleInterp(const Vector2& a, const Vector2& b, const Vector2& c, float t0, float t1, float t2);

	static const Vector2 zero;
	static const Vector2 one;
};

}

#include "vector2.inl"

#endif //!_BASE_VECTOR2_H_