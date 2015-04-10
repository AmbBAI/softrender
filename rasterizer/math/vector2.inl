#ifndef _MATH_VECTOR2_INLINE_
#define _MATH_VECTOR2_INLINE_

#include "math/mathf.h"

namespace rasterizer
{

float Vector2::Length() const { return Mathf::Sqrt(x * x + y * y); }
float Vector2::SqrLength() const { return (x * x + y * y); }
const Vector2 Vector2::Normalize() const { return (*this) * Mathf::InvSqrt(Length()); }
const Vector2 Vector2::Negate() const { return Vector2(-x, -y); }
const Vector2 Vector2::Add(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
const Vector2 Vector2::Subtract(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
const Vector2 Vector2::Multiply(float f) const { return Vector2(x * f, y * f); }
const Vector2 Vector2::Divide(float f) const { return Multiply(1.f / f); }

float Vector2::Dot(const Vector2& v) const { return x * v.x + y * v.y; }
    
const Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t)
{
	return Vector2(Mathf::Lerp(a.x, b.x, t), Mathf::Lerp(a.y, b.y, t));
}

}

#endif // !_MATH_VECTOR2_INLINE_