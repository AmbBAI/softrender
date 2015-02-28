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

	static const Vector2 zero;
	static const Vector2 one;
};

}

#endif //!_BASE_VECTOR2_H_