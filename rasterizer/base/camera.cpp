#include "camera.h"
#include "math/mathf.h"

namespace rasterizer
{

//void Camera::SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
//{
//    position = eye;
//    direction = (target - eye).Normalize();
//    
//	viewMatrix = Matrix4x4::LookAt(eye, target, up);
//}

Matrix4x4 Camera::viewMatrix() const
{
	return transform.worldToLocalMatrix();
}

Matrix4x4 Camera::projectionMatrix() const
{
	return projectionMatrix_;
}

bool Camera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	projectionMode_ = ProjectionMode_Perspective;
	projectionMatrix_ = Matrix4x4::PerspectiveFov(fov, aspect, zNear, zFar);
	return true;
}

bool Camera::SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	projectionMode_ = ProjectionMode_Orthographic;
	projectionMatrix_ = Matrix4x4::Orthographic(left, right, bottom, top, zNear, zFar);
	return true;
}

}