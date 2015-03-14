#include "camera.h"
#include "math/mathf.h"

namespace rasterizer
{

void Camera::SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	viewMatrix = Matrix4x4::LookAt(eye, target, up);
}

const Matrix4x4* Camera::GetViewMatrix() const
{
	return &viewMatrix;
}

const Matrix4x4* Camera::GetProjectionMatrix() const
{
	return &projectionMatrix;
}

bool Camera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	projectionMatrix = Matrix4x4::PerspectiveFov(fov, aspect, zNear, zFar);
	return true;
}

bool Camera::SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	projectionMatrix = Matrix4x4::Orthographic(left, right, bottom, top, zFar, zFar);
	return true;
}

float Camera::GetLinearDepth(float viewZ) const
{
	return (-viewZ - zNear) / (zFar - zNear);
}


}