#include "ray3.h"

namespace rasterizer
{


Ray3::Ray3(const Vector3& _origin, const Vector3& _direction)
	:origin(_origin), direction(_direction)
{
}

const Vector3 Ray3::GetPoint(float t) const
{
	Vector3 dis = direction.Multiply(t);
	return origin.Add(dis);
}

}