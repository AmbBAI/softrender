#ifndef _BASE_VECTOR2_H_
#define _BASE_VECTOR2_H_

#include "base/header.h"

namespace rasterizer
{

struct Vector2
{
	float x = 0.f;
	float y = 0.f;

	Vector2() = default;
	Vector2(float _x, float _y);

	float Length() const;
	float SqrLength() const;
	const Vector2 Normalize() const;
	const Vector2 Negate() const;
	const Vector2 Add(const Vector2& v) const;
	const Vector2 Subtract(const Vector2& v) const;
	const Vector2 Multiply(float f) const;
	const Vector2 Divide(float f) const;
	const float Dot(const Vector2& v) const;

    const Vector2 operator +(const Vector2& v) const { return Add(v); }
    const Vector2 operator -(const Vector2& v) const { return Subtract(v); }
    const Vector2 operator *(float f) const { return Multiply(f); }
    const Vector2 operator /(float f) const { return Divide(f); }
    
	static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

	static const Vector2 zero;
	static const Vector2 one;
};

}

#endif //!_BASE_VECTOR2_H_