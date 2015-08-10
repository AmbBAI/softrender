#ifndef _SOFTRENDER_CAMERA_H_
#define _SOFTRENDER_CAMERA_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/transform.h"
#include "math/matrix4x4.h"

namespace sr
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

	Transform transform;

	Matrix4x4 viewMatrix() const;

	bool SetPerspective(float fov, float aspect, float zNear, float zFar);
	bool SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);
	Matrix4x4 projectionMatrix() const;
        
	float GetLinearDepth(float projectionZ) const { return zNear / (projectionZ * (zNear - zFar) + zFar); }

	ProjectionMode projectionMode() { return projectionMode_; }

protected:
	ProjectionMode projectionMode_;
	Matrix4x4 projectionMatrix_;

	float zNear = 0;
	float zFar = 0;
};

}

#endif //!_SOFTRENDER_CAMERA_H_
