#ifndef _BASE_TEXTURE_CUBE_H_
#define _BASE_TEXTURE_CUBE_H_

#include "base/header.h"
#include "rasterizer/texture.h"
#include "math/vector2.h"
#include "math/vector3.h"

namespace rasterizer
{

class TextureCUBE;
typedef std::shared_ptr<TextureCUBE> TextureCUBEPtr;

class TextureCUBE
{
public:
	enum CUBEAxis
	{
		CUBEAxis_Right = 0,
		CUBEAxis_Left = 1,
		CUBEAxis_Top = 2,
		CUBEAxis_Bottom = 3,
		CUBEAxis_Front = 4,
		CUBEAxis_Back = 5
	};

	TexturePtr cubeMap[6];

	Color Sample(const Vector3& s);
	//Color Sample(const Vector3& s, const Vector3& ddx, const Vector3& ddy);

	void CalcTexCoord(const Vector3& s, CUBEAxis& axis, Vector2& texcoord);
};

}

#endif //! _BASE_TEXTURE_CUBE_H_