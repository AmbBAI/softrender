#ifndef _MATH_TRANSFORM_H_
#define _MATH_TRANSFORM_H_

#include "math/mathf.h"
#include "math/matrix4x4.h"
#include "math/vector3.h"
#include "math/quaternion.h"

namespace rasterizer {

struct Transform
{
	Vector3 position = Vector3(0.f, 0.f, 0.f);
	Quaternion rotation = Quaternion(0.f, 0.f, 0.f, 1.f);
    Vector3 scale = Vector3(1.f,1.f,1.f);

    const Matrix4x4 GetMatrix() const;
	// TODO
	void Translate(const Vector3& translate);
	void Rotate(const Vector3& eulerAngle, bool isLocal = true);

	void GetAxis(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
};

} // namespace rasterizer

#endif //!_MATH_TRANSFORM_H_
