#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/matrix4x4.h"

namespace rasterizer
{

struct Camera
{
	void SetLookAt(const Vector3& pos, const Vector3& at, const Vector3& up);
	void SetViewMatrix(const Matrix4x4& _viewMatrix );
	const Matrix4x4* GetViewMatrix() const;

	bool SetPerspective(float fov, float aspect, float zNear, float zFar);
	bool SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	void SetProjectionMatrix(const Matrix4x4& _projectionMatrix);
	const Matrix4x4* GetProjectionMatrix() const;

protected:
	void SetView(const Vector3& pos, const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

protected:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};

}

#endif //!_BASE_CAMERA_H_