#ifndef _BASE_RAY3_H_
#define _BASE_RAY3_H_

#include "base/header.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Ray3
{
	Vector3 origin = Vector3::zero;
	Vector3 direction = Vector3::front;

	Ray3() = default;
	Ray3(const Vector3& _origin, const Vector3& _direction);

	const Vector3 GetPoint(float t) const;
};


}

#endif //!_BASE_RAY3_H_
