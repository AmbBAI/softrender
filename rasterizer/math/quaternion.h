#ifndef _BASE_QUATERNION_H_
#define _BASE_QUATERNION_H_

#include "base/header.h"
#include "math/vector3.h"

namespace rasterizer
{
struct Quaternion
{
	union
	{
		struct
		{
			float x, y, z;
		};
		Vector3 xyz;
	};
	float w;

	Quaternion() = default;
	Quaternion(const Vector3& eulerAngle);
	Quaternion(const Vector3& _xyz, float _w) : xyz(_xyz), w(_w) {}
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	Quaternion Inverse() const;
	Quaternion Conjugate() const;
	Quaternion Multiply(const Quaternion& q) const;
	Vector3 Rotate(const Vector3& v) const;

	void SetEulerAngle(float _x, float _y, float _z);
	void SetEulerAngle(const Vector3& eulerAngle);
	//Vector3 GetEulerAngle();

	static const Quaternion identity;
};
}

#endif //_BASE_QUATERNION_H_
