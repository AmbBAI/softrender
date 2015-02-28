#include "quaternion.h"
#include "math/mathf.h"

namespace rasterizer
{

const Quaternion Quaternion::identity = Quaternion();

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
	: x(_x)
	, y(_y)
	, z(_z)
	, w(_w)
{

}

Quaternion::Quaternion(const Vector3& eulerAngle)
{
	SetEulerAngle(eulerAngle.x, eulerAngle.y, eulerAngle.z);
}

void Quaternion::SetEulerAngle(float _x, float _y, float _z)
{
	float radx = (_x * Mathf::deg2rad);
	float rady = (_y * Mathf::deg2rad);
	float radz = (_z * Mathf::deg2rad);
	float cos_x_2 = Mathf::Cos(radx / 2.f);
	float cos_y_2 = Mathf::Cos(rady / 2.f);
	float cos_z_2 = Mathf::Cos(radz / 2.f);
	float sin_x_2 = Mathf::Sin(radx / 2.f);
	float sin_y_2 = Mathf::Sin(rady / 2.f);
	float sin_z_2 = Mathf::Sin(radz / 2.f);

	x = sin_x_2 * cos_y_2 * cos_z_2 + cos_x_2 * sin_y_2 * sin_z_2;
	y = cos_x_2 * sin_y_2 * cos_z_2 + sin_x_2 * cos_y_2 * sin_z_2;
	z = cos_x_2 * cos_y_2 * sin_z_2 + sin_x_2 * sin_y_2 * cos_z_2;
	w = cos_x_2 * cos_y_2 * cos_z_2 + sin_x_2 * sin_y_2 * sin_z_2;
}

void Quaternion::SetEulerAngle(const Vector3& euler_angle)
{
	SetEulerAngle(euler_angle.x, euler_angle.y, euler_angle.z);
}

const Vector3 Quaternion::GetEulerAngle()
{
	float radx = Mathf::Atan2(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
	float rady = Mathf::Asin(2.f * (w * y + x * z));
	float radz = Mathf::Atan2(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));

	return Vector3(
		radx * Mathf::rad2deg,
		rady * Mathf::rad2deg,
		radz * Mathf::rad2deg);
}



}
