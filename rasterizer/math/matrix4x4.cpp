#include "matrix4x4.h"
#include "math/mathf.h"

namespace rasterizer
{

const Matrix4x4 Matrix4x4::identity = Matrix4x4(Vector3(0.f, 0.f, 0.f), Quaternion(0.f, 0.f, 0.f, 1.f), Vector3(1.f, 1.f, 1.f));

Matrix4x4::Matrix4x4(float* _m)
{
	memcpy(m, _m, sizeof(float) * 16);
}

Matrix4x4::Matrix4x4(const Vector3& p, const Quaternion& q, const Vector3& s)
{
	Matrix4x4 pMat, rMat, sMat;
	sMat.AssignScaling(s);
	rMat.AssignRotation(q);
	pMat.AssignTranslation(p);

	(*this) = pMat * rMat * sMat;
}

void Matrix4x4::Fill(float val)
{
	std::fill(m, m+16, val);
	//memcpy(m, &val, sizeof(float) * 16);
}

void Matrix4x4::Identity()
{
	Matrix4x4::Fill(0.f);
	m[0] = m[5] = m[10] = m[15] = 1.f;
}

bool Matrix4x4::IsIdentity()
{
	return (memcmp(Matrix4x4::identity.m, m, sizeof(float) * 16) == 0);
}

const Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& mat) const
{
	Matrix4x4 ret;
	ret.m[0] = m[0] * mat.m[0] + m[4] * mat.m[1] + m[8] * mat.m[2] + m[12] * mat.m[3];
	ret.m[1] = m[1] * mat.m[0] + m[5] * mat.m[1] + m[9] * mat.m[2] + m[13] * mat.m[3];
	ret.m[2] = m[2] * mat.m[0] + m[6] * mat.m[1] + m[10] * mat.m[2] + m[14] * mat.m[3];
	ret.m[3] = m[3] * mat.m[0] + m[7] * mat.m[1] + m[11] * mat.m[2] + m[15] * mat.m[3];

	ret.m[4] = m[0] * mat.m[4] + m[4] * mat.m[5] + m[8] * mat.m[6] + m[12] * mat.m[7];
	ret.m[5] = m[1] * mat.m[4] + m[5] * mat.m[5] + m[9] * mat.m[6] + m[13] * mat.m[7];
	ret.m[6] = m[2] * mat.m[4] + m[6] * mat.m[5] + m[10] * mat.m[6] + m[14] * mat.m[7];
	ret.m[7] = m[3] * mat.m[4] + m[7] * mat.m[5] + m[11] * mat.m[6] + m[15] * mat.m[7];

	ret.m[8] = m[0] * mat.m[8] + m[4] * mat.m[9] + m[8] * mat.m[10] + m[12] * mat.m[11];
	ret.m[9] = m[1] * mat.m[8] + m[5] * mat.m[9] + m[9] * mat.m[10] + m[13] * mat.m[11];
	ret.m[10] = m[2] * mat.m[8] + m[6] * mat.m[9] + m[10] * mat.m[10] + m[14] * mat.m[11];
	ret.m[11] = m[3] * mat.m[8] + m[7] * mat.m[9] + m[11] * mat.m[10] + m[15] * mat.m[11];

	ret.m[12] = m[0] * mat.m[12] + m[4] * mat.m[13] + m[8] * mat.m[14] + m[12] * mat.m[15];
	ret.m[13] = m[1] * mat.m[12] + m[5] * mat.m[13] + m[9] * mat.m[14] + m[13] * mat.m[15];
	ret.m[14] = m[2] * mat.m[12] + m[6] * mat.m[13] + m[10] * mat.m[14] + m[14] * mat.m[15];
	ret.m[15] = m[3] * mat.m[12] + m[7] * mat.m[13] + m[11] * mat.m[14] + m[15] * mat.m[15];

	return ret;
}

const Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const
{
	return Matrix4x4::Multiply(mat);
}

void Matrix4x4::AssignRotation(const Quaternion& quaternion)
{
	float xx = quaternion.x * quaternion.x;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float xw = quaternion.x * quaternion.w;

	float yy = quaternion.y * quaternion.y;
	float yz = quaternion.y * quaternion.z;
	float yw = quaternion.y * quaternion.w;

	float zz = quaternion.z * quaternion.z;
	float zw = quaternion.z * quaternion.w;

	float ww = quaternion.w * quaternion.w;

	//m[0] = 1.f - 2.f * (yy + zz);
	m[0] = ww + xx - yy - zz;
	m[1] = 2.f * (xy + zw);
	m[2] = 2.f * (xz - yw);
	m[3] = 0.f;

	m[4] = 2.f * (xy - zw);
	//m[5] = 1.f - 2.f * (xx + zz);
	m[5] = ww - xx + yy - zz;
	m[6] = 2.f * (yz + xw);
	m[7] = 0.f;

	m[8] = 2.f * (xz + yw);
	m[9] = 2.f * (yz - xw);
	//m[10] = 1.f - 2.f * (xx + yy);
	m[10] = ww - xx - yy + zz;
	m[11] = 0.f;

	m[12] = 0.f;
	m[13] = 0.f;
	m[14] = 0.f;
	m[15] = 1.f;
}
//
//void Matrix4x4::AssignRotationX(float radians)
//{
//	/*
//		|  1  0       0       0 |
//	M = |  0  cos(A) -sin(A)  0 |
//		|  0  sin(A)  cos(A)  0 |
//		|  0  0       0       1 |
//
//	*/
//
//	m[0] = 1.0f;
//	m[1] = 0.0f;
//	m[2] = 0.0f;
//	m[3] = 0.0f;
//
//	m[4] = 0.0f;
//	m[5] = Mathf::Cos(radians);
//	m[6] = Mathf::Sin(radians);
//	m[7] = 0.0f;
//
//	m[8] = 0.0f;
//	m[9] = -Mathf::Sin(radians);
//	m[10] = Mathf::Cos(radians);
//	m[11] = 0.0f;
//
//	m[12] = 0.0f;
//	m[13] = 0.0f;
//	m[14] = 0.0f;
//	m[15] = 1.0f;
//}
//
//void Matrix4x4::AssignRotationY(float radians)
//{
//	/*
//		|  cos(A)  0   sin(A)  0 |
//	M = |  0       1   0       0 |
//		| -sin(A)  0   cos(A)  0 |
//		|  0       0   0       1 |
//	*/
//
//	m[0] = Mathf::Cos(radians);
//	m[1] = 0.0f;
//	m[2] = -Mathf::Sin(radians);
//	m[3] = 0.0f;
//
//	m[4] = 0.0f;
//	m[5] = 1.0f;
//	m[6] = 0.0f;
//	m[7] = 0.0f;
//
//	m[8] = Mathf::Sin(radians);
//	m[9] = 0.0f;
//	m[10] = Mathf::Cos(radians);
//	m[11] = 0.0f;
//
//	m[12] = 0.0f;
//	m[13] = 0.0f;
//	m[14] = 0.0f;
//	m[15] = 1.0f;
//}
//
//void Matrix4x4::AssignRotationZ(float radians)
//{
//	/*
//		|  cos(A)  -sin(A)   0   0 |
//	M = |  sin(A)   cos(A)   0   0 |
//		|  0        0        1   0 |
//		|  0        0        0   1 |
//	*/
//
//	m[0] = Mathf::Cos(radians);
//	m[1] = Mathf::Sin(radians);
//	m[2] = 0.0f;
//	m[3] = 0.0f;
//
//	m[4] = -Mathf::Sin(radians);
//	m[5] = Mathf::Cos(radians);
//	m[6] = 0.0f;
//	m[7] = 0.0f;
//
//	m[8] = 0.0f;
//	m[9] = 0.0f;
//	m[10] = 1.0f;
//	m[11] = 0.0f;
//
//	m[12] = 0.0f;
//	m[13] = 0.0f;
//	m[14] = 0.0f;
//	m[15] = 1.0f;
//}

void Matrix4x4::AssignScaling(const Vector3& scaling)
{
	Matrix4x4::Fill(0.f);

	m[0] = scaling.x;
	m[5] = scaling.y;
	m[10] = scaling.z;
	m[15] = 1.0f;
}

void Matrix4x4::AssignTranslation(const Vector3& translation)
{
	Matrix4x4::Fill(0.f);

	m[0] = 1.0f;
	m[5] = 1.0f;
	m[10] = 1.0f;

	m[12] = translation.x;
	m[13] = translation.y;
	m[14] = translation.z;
	m[15] = 1.0f;
}

const Matrix4x4 Matrix4x4::Inverse() const
{
	Matrix4x4 mat;
	float det;
	int i;

	mat.m[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	mat.m[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	mat.m[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	mat.m[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	mat.m[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	mat.m[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	mat.m[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	mat.m[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	mat.m[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	mat.m[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	mat.m[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	mat.m[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	mat.m[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	mat.m[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	mat.m[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	mat.m[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * mat.m[0] + m[1] * mat.m[4] + m[2] * mat.m[8] + m[3] * mat.m[12];

	det = 1.0f / det;
	for (i = 0; i < 16; i++) {
		mat.m[i] *= det;
	}

	return mat;
}

const Matrix4x4 Matrix4x4::Transpose() const
{
	int x, z;
	Matrix4x4 mat;

	for (z = 0; z < 4; ++z) {
		for (x = 0; x < 4; ++x) {
			mat.m[(z * 4) + x] = m[(x * 4) + z];
		}
	}

	return mat;
}

const Vector4 Matrix4x4::MultiplyPoint(const Vector3& p) const
{
	Vector4 ret = Vector4(
		m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12],
		m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13],
		m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14],
        m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15]);
	return ret;
}

const Vector3 Matrix4x4::MultiplyPoint3x4(const Vector3& p) const
{
	return Vector3(
		m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12],
		m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13],
		m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14]);
}

const Vector3 Matrix4x4::MultiplyVector(const Vector3& p) const
{
	return Vector3(
		m[0] * p.x + m[4] * p.y + m[8] * p.z,
		m[1] * p.x + m[5] * p.y + m[9] * p.z,
		m[2] * p.x + m[6] * p.y + m[10] * p.z);
}

const Matrix4x4 Matrix4x4::TBN(const Vector3& tangent, const Vector3& bitangent, const Vector3& normal)
{
	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = tangent.x; mat.m[4] = bitangent.x; mat.m[8] = normal.x;
	mat.m[1] = tangent.y; mat.m[5] = bitangent.y; mat.m[9] = normal.y;
	mat.m[2] = tangent.z; mat.m[6] = bitangent.z; mat.m[10] = normal.z;
	return mat;
}

const Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float deltaX = right - left;
	float deltaY = top - bottom;
	float deltaZ = zFar - zNear;

	assert(deltaX != 0.f);
	assert(deltaY != 0.f);
	assert(deltaZ != 0.f);

	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = 2.f / deltaX;
	mat.m[5] = 2.f / deltaY;
	mat.m[10] = 1.f / deltaZ;
	mat.m[12] = -(right + left) / deltaX;
	mat.m[13] = -(top + bottom) / deltaY;
	mat.m[14] = -zNear / deltaZ;
	mat.m[15] = 1;

	return mat;
}

const Matrix4x4 Matrix4x4::PerspectiveFov(float fov, float aspect, float zNear, float zFar)
{
	float r = (fov / 2.f) * Mathf::deg2rad;
	float zDelta = zFar - zNear;
	float sinR = Mathf::Sin(r);

	assert(zDelta != 0.f);
	assert(sinR != 0.f);
	assert(aspect != 0.f);

	Matrix4x4 mat = Matrix4x4::identity;

	//cos(r) / sin(r) = cot(r)
	float cotR = Mathf::Cos(r) / sinR;

	mat.m[0] = cotR / aspect;
	mat.m[5] = cotR;
	mat.m[10] = zFar / zDelta;
	mat.m[11] = 1.0f;
	mat.m[14] = -zNear * zFar / zDelta;
	mat.m[15] = 0.f;
	return mat;
}

const Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Vector3 zAxis = (target - eye).Normalize();
	Vector3 xAxis = up.Cross(zAxis).Normalize();
	Vector3 yAxis = zAxis.Cross(xAxis);

	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = xAxis.x; mat.m[1] = yAxis.x; mat.m[2] = zAxis.x;
	mat.m[4] = xAxis.y; mat.m[5] = yAxis.y; mat.m[6] = zAxis.y;
	mat.m[8] = xAxis.z; mat.m[9] = yAxis.z; mat.m[10] = zAxis.z;
	mat.m[12] = -xAxis.Dot(eye);
	mat.m[13] = -yAxis.Dot(eye);
	mat.m[14] = -zAxis.Dot(eye);
	return mat;
}

Matrix4x4 Matrix4x4::TRS(const Vector3& p, const Quaternion& q, const Vector3& s)
{
	return Matrix4x4(p, q, s);
}

















}
