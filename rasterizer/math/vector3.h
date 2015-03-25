#ifndef _BASE_VECTOR3_H_
#define _BASE_VECTOR3_H_

#include "base/header.h"

namespace rasterizer
{

struct Vector3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	Vector3() = default;
	Vector3(float _x, float _y, float _z);

	float Length() const;
	float SqrLength() const;
	const Vector3 Normalize() const;
	const Vector3 Negate() const;
	const Vector3 Add(const Vector3& v) const;
	const Vector3 Subtract(const Vector3& v) const;
	const Vector3 Multiply(float f) const;
	const Vector3 Divide(float f) const;
	float Dot(const Vector3& v) const;
	const Vector3 Cross(const Vector3& v) const;

	const Vector3 operator +(const Vector3& v) const { return Add(v); }
	const Vector3 operator -(const Vector3& v) const { return Subtract(v); }
	const Vector3 operator *(float f) const { return Multiply(f); }
	const Vector3 operator /(float f) const { return Divide(f); }

	std::string ToString() const;
	
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 front;
	static const Vector3 up;
	static const Vector3 right;
	static const Vector3 back;
	static const Vector3 down;
	static const Vector3 left;
};

}

#endif //!_BASE_VECTOR3_H_