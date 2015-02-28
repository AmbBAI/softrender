#include "vector3.h"
#include "math/mathf.h"

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
	//printf("Vector3:ctor(x,y,z)\n");
}

Vector3::Vector3(const Vector3& v)
	: x(v.x)
	, y(v.y)
	, z(v.z)
{
	//printf("Vector3:copy\n");
}

float Vector3::Length() const
{
	return Mathf::Sqrt(x * x + y * y + z * z);
}

float Vector3::SqrLength() const
{
	return (x * x + y * y + z * z);
}

const Vector3 Vector3::Normalize() const
{
	float inv = 1.f / Length();
	return Vector3(x * inv, y * inv, z * inv);
}

const Vector3 Vector3::Negate() const
{
	return Vector3(-x, -y, -z);
}

const Vector3 Vector3::Add(const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

const Vector3 Vector3::Subtract(const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

const Vector3 Vector3::Multiply(float f) const
{
	return Vector3(x * f, y * f, z * f);
}

const Vector3 Vector3::Divide(float f) const
{
	float invf = 1.f / f;
	return Vector3(x * invf, y * invf, z * invf);
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

}