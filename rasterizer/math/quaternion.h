#ifndef _BASE_QUATERNION_H_
#define _BASE_QUATERNION_H_

#include "base/header.h"
#include "math/vector3.h"

namespace rasterizer
{
struct Quaternion
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;

	Quaternion() = default;
	Quaternion(const Vector3& eulerAngle);
	Quaternion(float _x, float _y, float _z, float _w);

	const Quaternion Inverse() const;
	const Quaternion Multiply(const Quaternion& q) const;
	const Vector3 Rotate(const Vector3& v) const;

	void SetEulerAngle(float _x, float _y, float _z);
	void SetEulerAngle(const Vector3& euler_angle);
	const Vector3 GetEulerAngle();

	static const Quaternion identity;
};
}

#endif //_BASE_QUATERNION_H_
