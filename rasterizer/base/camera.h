#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include "base/header.h"
#include "base/vector3.h"
#include "base/matrix4x4.h"

namespace rasterizer
{

struct Camera
{
	void SetLookAtLH(const Vector3& pos, const Vector3& at, const Vector3& up);
	void SetViewMatrix(const Matrix4x4& _viewMatrix );
	const Matrix4x4* GetViewMatrix();

	void SetProjectionMatrix(const Matrix4x4& _projectionMatrix);
	const Matrix4x4* GetProjectionMatrix();

protected:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};

}

#endif //!_BASE_CAMERA_H_