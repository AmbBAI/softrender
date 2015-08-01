#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/transform.h"
#include "math/matrix4x4.h"

namespace rasterizer
{

struct Camera;
typedef std::shared_ptr<Camera> CameraPtr;

struct Camera
{
public:
	enum ProjectionMode
	{
		ProjectionMode_Perspective,
		ProjectionMode_Orthographic
	};

	void SetLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
	void SetLookAt(const Transform& trans);
	const Matrix4x4* GetViewMatrix() const;

	bool SetPerspective(float fov, float aspect, float zNear, float zFar);
	bool SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	const Matrix4x4* GetProjectionMatrix() const;
    
    
    const Vector3 GetPosition() const { return position; }
    const Vector3 GetDirection() const { return direction; }
	float GetLinearDepth(float projectionZ) const { return zNear / (projectionZ * (zNear - zFar) + zFar); }

	ProjectionMode projectionMode() { return projectionMode_; }

protected:
	ProjectionMode projectionMode_;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	float zNear = 0;
	float zFar = 0;
    
    Vector3 position;
    Vector3 direction;
};

}

#endif //!_BASE_CAMERA_H_