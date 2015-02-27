#include "camera.h"
#include "base/mathf.h"

namespace rasterizer
{

void Camera::SetViewMatrix(const Matrix4x4& _viewMatrix)
{
	viewMatrix = _viewMatrix;
}

void Camera::SetLookAtLH(const Vector3& pos, const Vector3& at, const Vector3& up)
{
	Vector3 zAxis = at.Subtract(pos).Normalize();
	Vector3 xAxis = up.Cross(zAxis).Normalize();
	Vector3 yAxis = zAxis.Cross(xAxis);

	Matrix4x4 mat = Matrix4x4::identity;
	mat.m[0] = xAxis.x; mat.m[1] = yAxis.x; mat.m[2] = zAxis.x;
	mat.m[4] = xAxis.y; mat.m[5] = yAxis.y; mat.m[6] = zAxis.y;
	mat.m[8] = xAxis.z; mat.m[9] = yAxis.z; mat.m[10] = zAxis.z;
	mat.m[12] = -xAxis.Dot(pos);
	mat.m[13] = -yAxis.Dot(pos);
	mat.m[14] = -zAxis.Dot(pos);
	viewMatrix = mat;
}

const Matrix4x4* Camera::GetViewMatrix()
{
	return &viewMatrix;
}

void Camera::SetProjectionMatrix(const Matrix4x4& _projectionMatrix)
{
	projectionMatrix = _projectionMatrix;
}

const Matrix4x4* Camera::GetProjectionMatrix()
{
	return &projectionMatrix;
}


}