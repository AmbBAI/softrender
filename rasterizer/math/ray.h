#ifndef _MATH_RAY3_H_
#define _MATH_RAY3_H_

#include "base/header.h"
#include "math/vector3.h"

namespace sr
{

struct Ray
{
	Vector3 origin = Vector3::zero;
	Vector3 direction = Vector3::front;

	Ray() = default;
	Ray(const Vector3& _origin, const Vector3& _direction)
		:origin(_origin), direction(_direction) { }


	Vector3 GetPoint(float t) const { return origin + direction * t; }
};


}

#endif //!_MATH_RAY3_H_
