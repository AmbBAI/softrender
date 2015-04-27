
#include "transform.h"

namespace rasterizer {

const Matrix4x4 Transform::GetMatrix() const
{
    return Matrix4x4::TRS(position, Quaternion(rotation), scale);
}


}