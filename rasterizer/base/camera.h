#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/matrix4x4.h"

namespace rasterizer
{

struct Camera;
typedef std::shared_ptr<Camera> CameraPtr;

struct Camera
{
public:
	void SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
	const Matrix4x4* GetViewMatrix() const;

	bool SetPerspective(float fov, float aspect, float zNear, float zFar);
	bool SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	const Matrix4x4* GetProjectionMatrix() const;
	float GetLinearDepth(float viewZ) const
	{
		return (viewZ - zNear) / (zFar - zNear);
	}

protected:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	float zNear = 0;
	float zFar = 0;
};

}

#endif //!_BASE_CAMERA_H_