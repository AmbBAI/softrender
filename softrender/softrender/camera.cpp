#include "camera.h"
#include "math/mathf.h"

namespace sr
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

Matrix4x4 Camera::GetFullScreenQuadMatrix() const
{
	Matrix4x4 mat = projectionMatrix_.Inverse();
	Vector4 con = mat.MultiplyPoint(Vector3(1.f, 1.f, 0.1f));
	con.xyz /= con.w;
	Vector3 pos = Vector3::front * con.z;
	Quaternion rot = Quaternion::identity;
	Vector3 scale = Vector3(con.x, con.y, -1.f);
	return transform.localToWorldMatrix() * Matrix4x4::TRS(pos, rot, scale);
}

bool Camera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	this->zFar_ = zFar;
	this->zNear_ = zNear;
	projectionMode_ = ProjectionMode_Perspective;
	projectionMatrix_ = Matrix4x4::PerspectiveFov(fov, aspect, zNear, zFar);
	return true;
}

bool Camera::SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	this->zFar_ = zFar;
	this->zNear_ = zNear;
	projectionMode_ = ProjectionMode_Orthographic;
	projectionMatrix_ = Matrix4x4::Orthographic(left, right, bottom, top, zNear, zFar);
	return true;
}

}