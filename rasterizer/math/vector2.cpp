#include "vector2.h"
#include "math/mathf.h"

namespace rasterizer
{


const Vector2 Vector2::zero = Vector2(0.f, 0.f);
const Vector2 Vector2::one = Vector2(1.f, 1.f);

Vector2::Vector2(float _x, float _y)
	: x(_x), y(_y)
{
}

float Vector2::Length() const
{
	return Mathf::Sqrt(x * x + y * y);
}

float Vector2::SqrLength() const
{
	return (x * x + y * y);
}

const Vector2 Vector2::Normalize() const
{
	float inv = 1.f / Length();
	return Vector2(x * inv, y * inv);
}

const Vector2 Vector2::Negate() const
{
	return Vector2(-x, -y);
}

const Vector2 Vector2::Add(const Vector2& v) const
{
	return Vector2(x + v.x, y + v.y);
}

const Vector2 Vector2::Subtract(const Vector2& v) const
{
	return Vector2(x - v.x, y - v.y);
}

const Vector2 Vector2::Multiply(float f) const
{
	return Vector2(x * f, y * f);
}

const Vector2 Vector2::Divide(float f) const
{
	float invf = 1.f / f;
	return Vector2(x * invf, y * invf);
}

const float Vector2::Dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

rasterizer::Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t)
{
	return Vector2(Mathf::Lerp(a.x, b.x, t), Mathf::Lerp(a.y, b.y, t));
}

}