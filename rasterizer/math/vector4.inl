#ifndef _MATH_VECTOR4_INLINE_
#define _MATH_VECTOR4_INLINE_

#include "math/mathf.h"

namespace rasterizer
{

const Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float t)
{
	return Vector4(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t),
		Mathf::Lerp(a.w, b.w, t));
}

}

#endif // !_MATH_VECTOR4_INLINE_