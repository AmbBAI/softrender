#include "vector3.h"

namespace rasterizer
{


const Vector3 Vector3::zero = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::one = Vector3(1.f, 1.f, 1.f);
const Vector3 Vector3::front = Vector3(0.f, 0.f, -1.f);
const Vector3 Vector3::up = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::right = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::back = Vector3(0.f, 0.f, 1.f);
const Vector3 Vector3::down = Vector3(0.f, -1.f, 0.f);
const Vector3 Vector3::left = Vector3(-1.f, 0.f, 0.f);

Vector3::Vector3(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
}

float Vector3::Dot(const Vector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

const Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
		);
}

std::string Vector3::ToString() const
{
	std::string str;
	char tmp[1024];
	sprintf(tmp, "(%.2f, %.2f, %.2f)", x, y, z);
	str = tmp;
	return str;
}

const Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
	return Vector3(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t));
}

}