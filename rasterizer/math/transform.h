#ifndef _MATH_TRANSFORM_H_
#define _MATH_TRANSFORM_H_

#include "math/mathf.h"
#include "math/matrix4x4.h"
#include "math/vector3.h"
#include "math/quaternion.h"

namespace sr {

struct Transform
{
	Vector3 position = Vector3(0.f, 0.f, 0.f);
	Quaternion rotation = Quaternion(0.f, 0.f, 0.f, 1.f);
    Vector3 scale = Vector3(1.f,1.f,1.f);

	Transform() = default;
	Transform(const Vector3& p, const Quaternion& r, const Vector3& s)
		: position(p), rotation(r), scale(s) {}

    Matrix4x4 localToWorldMatrix() const;
	Matrix4x4 worldToLocalMatrix() const;
	Vector3 forward() const { return rotation.Rotate(Vector3::front).Normalize(); }
	Vector3 right() const { return rotation.Rotate(Vector3::right).Normalize(); }
	Vector3 up() const { return rotation.Rotate(Vector3::up).Normalize(); }

	// TODO
	void Translate(const Vector3& translate);
	void Rotate(const Vector3& eulerAngle, bool isLocal = true);

	void GetAxis(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
};

} // namespace rasterizer

#endif //!_MATH_TRANSFORM_H_
