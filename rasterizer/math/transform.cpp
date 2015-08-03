#include "transform.h"

using namespace rasterizer;

Matrix4x4 Transform::localToWorldMatrix() const
{
    return Matrix4x4::TRS(position, Quaternion(rotation), scale);
}

Matrix4x4 Transform::worldToLocalMatrix() const
{
	return localToWorldMatrix().Inverse();
}

void Transform::GetAxis(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
	zAxis = rotation.Rotate(Vector3::front).Normalize();
	xAxis = Vector3::down.Cross(zAxis).Normalize();
	yAxis = xAxis.Cross(zAxis);
}

void Transform::Translate(const Vector3& translate)
{
	this->position += translate;
}

void Transform::Rotate(const Vector3& eulerAngle, bool isLocal/* = true*/)
{
	if (isLocal)
	{
		rotation = rotation.Multiply(eulerAngle);
	}
	else
	{
		Rotate(rotation.Inverse().Rotate(eulerAngle), true);
	}
}

