#include "clipper.hpp"

namespace rasterizer
{

Clipper::Plane Clipper::viewFrustumPlanes[6] = {
	Clipper::Plane(0x01, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.x, -v0.w, v1.x, -v1.w); }),
	Clipper::Plane(0x02, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.x, v0.w, v1.x, v1.w); }),
	Clipper::Plane(0x04, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.y, -v0.w, v1.y, -v1.w); }),
	Clipper::Plane(0x08, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.y, v0.w, v1.y, v1.w); }),
	Clipper::Plane(0x10, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.z, -v0.w, v1.z, -v1.w); }),
	Clipper::Plane(0x20, [](const Vector4& v0, const Vector4& v1) { return Clipper::Clip(v0.z, v0.w, v1.z, v1.w); })
};


}