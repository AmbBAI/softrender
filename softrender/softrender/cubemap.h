#ifndef _SOFTRENDER_CUBEMAP_H_
#define _SOFTRENDER_CUBEMAP_H_

#include "base/header.h"
#include "softrender/texture2d.h"
#include "math/vector2.h"
#include "math/vector3.h"

namespace sr
{

class Cubemap;
typedef std::shared_ptr<Cubemap> CubemapPtr;

class Cubemap
{
public:
	enum CubemapFace
	{
		CubemapFace_PositiveX = 0,
		CubemapFace_NegativeX = 1,
		CubemapFace_PositiveY = 2,
		CubemapFace_NegativeY = 3,
		CubemapFace_PositiveZ = 4,
		CubemapFace_NegativeZ = 5
	};

	void SetTexture(CubemapFace face, Texture2DPtr tex);

	Color Sample(const Vector3& s);
	//Color Sample(const Vector3& s, const Vector3& ddx, const Vector3& ddy);

protected:
	void CalcTexCoord(const Vector3& s, CubemapFace& face, Vector2& texcoord);

private:
	Texture2DPtr maps[6];
};

}

#endif //! _SOFTRENDER_CUBEMAP_H_
