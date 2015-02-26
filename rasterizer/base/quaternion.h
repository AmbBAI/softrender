#ifndef _BASE_QUATERNION_H_
#define _BASE_QUATERNION_H_
#include "header.h"
#include "vector3.h"

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

	void SetEulerAngle(float _x, float _y, float _z);
	void SetEulerAngle(const Vector3& euler_angle);
	const Vector3 GetEulerAngle();

	static const Quaternion identity;
};
}

#endif //_BASE_QUATERNION_H_
