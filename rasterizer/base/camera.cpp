#include "camera.h"
#include "math/mathf.h"

namespace rasterizer
{

void Camera::SetViewMatrix(const Matrix4x4& _viewMatrix)
{
	viewMatrix = _viewMatrix;
}

void Camera::SetLookAt(const Vector3& pos, const Vector3& at, const Vector3& up)
{
	Vector3 zAxis = pos.Subtract(at).Normalize();
	Vector3 xAxis = up.Cross(zAxis).Normalize();
	Vector3 yAxis = zAxis.Cross(xAxis);

	SetView(pos, xAxis, yAxis, zAxis);
}

void Camera::SetView(const Vector3& pos, const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = xAxis.x; mat.m[1] = yAxis.x; mat.m[2] = zAxis.x;
	mat.m[4] = xAxis.y; mat.m[5] = yAxis.y; mat.m[6] = zAxis.y;
	mat.m[8] = xAxis.z; mat.m[9] = yAxis.z; mat.m[10] = zAxis.z;
	mat.m[12] = -xAxis.Dot(pos);
	mat.m[13] = -yAxis.Dot(pos);
	mat.m[14] = -zAxis.Dot(pos);
	viewMatrix = mat;
}

const Matrix4x4* Camera::GetViewMatrix() const
{
	return &viewMatrix;
}

void Camera::SetProjectionMatrix(const Matrix4x4& _projectionMatrix)
{
	projectionMatrix = _projectionMatrix;
}

const Matrix4x4* Camera::GetProjectionMatrix() const
{
	return &projectionMatrix;
}

bool Camera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	float r = (fov / 2) * Mathf::deg2rad;
	float zDelta = zFar - zNear;
	float s = Mathf::Sin(r);
	float cotangent = 0;

	if (zDelta == 0 || s == 0 || aspect == 0) {
		return false;
	}

	Matrix4x4 mat = Matrix4x4::identity;

	//cos(r) / sin(r) = cot(r)
	cotangent = Mathf::Cos(r) / s;

	mat.m[0] = cotangent / aspect;
	mat.m[5] = cotangent;
	mat.m[10] = -zFar / zDelta;
	mat.m[11] = -1.0f;
	mat.m[14] = -2.0f * zNear * zFar / zDelta;
	mat.m[15] = 0;

	projectionMatrix = mat;
	return true;
}

bool Camera::SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	float deltaX = right - left;
	float deltaY = top - bottom;
	float deltaZ = zFar - zNear;

	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = 2 / deltaX;
	mat.m[5] = 2 / deltaY;
	mat.m[10] = -2 / deltaZ;
	mat.m[12] = -(right + left) / deltaX;
	mat.m[13] = -(top + bottom) / deltaY;
	mat.m[14] = -(zFar + zNear) / deltaZ;

	projectionMatrix = mat;
	return true;
}


}