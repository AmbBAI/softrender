#include "quaternion.h"
#include "math/mathf.h"

namespace rasterizer
{

const Quaternion Quaternion::identity = Quaternion();

Quaternion::Quaternion(const Vector3& eulerAngle)
{
	SetEulerAngle(eulerAngle.x, eulerAngle.y, eulerAngle.z);
}

void Quaternion::SetEulerAngle(float _x, float _y, float _z)
{
	float rad_x_2 = _x / 2.f * Mathf::deg2rad;
	float rad_y_2 = _y / 2.f * Mathf::deg2rad;
	float rad_z_2 = _z / 2.f * Mathf::deg2rad;
	float cos_x_2 = Mathf::Cos(rad_x_2);
	float cos_y_2 = Mathf::Cos(rad_y_2);
	float cos_z_2 = Mathf::Cos(rad_z_2);
	float sin_x_2 = Mathf::Sin(rad_x_2);
	float sin_y_2 = Mathf::Sin(rad_y_2);
	float sin_z_2 = Mathf::Sin(rad_z_2);

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

const Quaternion Quaternion::Multiply(const Quaternion& q) const
{;
	return Quaternion(q.xyz * w + xyz * q.w + xyz.Cross(q.xyz), w * q.w - xyz.Dot(q.xyz));
}

const Vector3 Quaternion::Rotate(const Vector3& v) const
{
	return v + xyz.Cross(xyz.Cross(v) + v * w) * 2.f;
}

const Quaternion Quaternion::Inverse() const
{
	float invSqrtLen = 1.f / (x * x + y * y + z * z + w * w);
	Quaternion ret = Conjugate();
	ret.xyz *= invSqrtLen;
	ret.w *= invSqrtLen;
	return ret;
}

const Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-xyz, w);
}



}
