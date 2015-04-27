#ifndef _MATH_TRANSFORM_H_
#define _MATH_TRANSFORM_H_

#include "math/mathf.h"
#include "math/matrix4x4.h"
#include "math/vector3.h"

namespace rasterizer {

struct Transform
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    
    const Matrix4x4 GetMatrix() const;
};

} // namespace rasterizer

#endif //!_MATH_TRANSFORM_H_
