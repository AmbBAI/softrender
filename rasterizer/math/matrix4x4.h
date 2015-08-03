#ifndef _BASE_MATRIX4X4_H_
#define _BASE_MATRIX4X4_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/quaternion.h"

namespace rasterizer
{
struct Matrix4x4
{
	float m[16];

	Matrix4x4() = default;
	Matrix4x4(float* _m);
	Matrix4x4(const Vector3& p, const Quaternion& q, const Vector3& s);

	float Get(int r, int c);

	void Identity();
	bool IsIdentity();

	const Matrix4x4 Inverse() const;
	const Matrix4x4 Transpose() const;

	const Matrix4x4 Multiply(const Matrix4x4& mat) const;
	const Matrix4x4 operator*(const Matrix4x4& mat) const;

	const Vector4 MultiplyPoint(const Vector3& p) const;
	const Vector3 MultiplyPoint3x4(const Vector3& p) const;
	const Vector3 MultiplyVector(const Vector3& p) const;

	static Matrix4x4 TRS(const Vector3& p, const Quaternion& q, const Vector3& s);
	static const Matrix4x4 TBN(const Vector3& tangent, const Vector3& bitangent, const Vector3& normal);
	static const Matrix4x4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	static const Matrix4x4 PerspectiveFov(float fov, float aspect, float zNear, float zFar);
	static const Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

	static const Matrix4x4 identity;

protected:
	void Fill(float val);

	void AssignTranslation(const Vector3& translation);

	void AssignRotation(const Quaternion& quaternion);
	//void AssignRotationX(float radians);
	//void AssignRotationY(float radians);
	//void AssignRotationZ(float radians);

	void AssignScaling(const Vector3& scaling);
};

}

#endif //_BASE_MATRIX4X4_H_
