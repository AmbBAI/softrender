#include "ray3.h"

namespace rasterizer
{


Ray3::Ray3(const Vector3& _origin, const Vector3& _direction)
	:origin(_origin), direction(_direction)
{
}

Vector3 Ray3::GetPoint(float t) const
{
	return origin + direction * t;
}

}