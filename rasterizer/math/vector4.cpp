#include "vector4.h"
#include "math/mathf.h"

namespace rasterizer
{

Vector4::Vector4(float _x, float _y, float _z, float _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

Vector4::Vector4(const Vector4& v)
	: Vector4(v.x, v.y, v.z, v.w)
{
}

Vector4::Vector4(const Vector3& v)
	: Vector4(v.x, v.y, v.z, 1.0f)
{
}

Vector4::operator Vector3() const
{
	return Vector3(x, y, z);
}

}