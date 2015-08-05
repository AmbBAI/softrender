#ifndef _MATH_VECTOR2_INLINE_
#define _MATH_VECTOR2_INLINE_

#include "math/mathf.h"

namespace rasterizer
{

float Vector2::Length() const { return Mathf::Sqrt(x * x + y * y); }
float Vector2::SqrLength() const { return (x * x + y * y); }
Vector2 Vector2::Normalize() const { return (*this) * Mathf::InvSqrt(Length()); }
Vector2 Vector2::Negate() const { return Vector2(-x, -y); }

float Vector2::Dot(const Vector2& v) const { return x * v.x + y * v.y; }
float Vector2::Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }

Vector2 Vector2::LinearInterp(const Vector2& a, const Vector2& b, float t)
{
	return Vector2(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t));
}

Vector2 Vector2::TriangleInterp(const Vector2& a, const Vector2& b, const Vector2& c, float t0, float t1, float t2)
{
	return Vector2(
		Mathf::Terp(a.x, b.x, c.x, t0, t1, t2),
		Mathf::Terp(a.y, b.y, c.y, t0, t1, t2));
}

}

#endif // !_MATH_VECTOR2_INLINE_