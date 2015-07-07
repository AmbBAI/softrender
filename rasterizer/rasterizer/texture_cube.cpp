#include "texture_cube.h"

using namespace rasterizer;

Color TextureCUBE::Sample(const Vector3& s)
{
	CUBEAxis axis;
	Vector2 texcoord;
	CalcTexCoord(s, axis, texcoord);

	TexturePtr tex = cubeMap[axis];
	if (tex == nullptr) return Color::black;
	return tex->Sample(texcoord.x, texcoord.y);
}

//Color TextureCUBE::Sample(const Vector3& s, const Vector3& ddx, const Vector3& ddy)
//{
//	CUBEAxis axis;
//	Vector2 texcoord;
//	CalcTexCoord(s, axis, texcoord);
//
//	TexturePtr tex = cubeMap[axis];
//	if (tex == nullptr) return Color::black;
//	return tex->Sample(texcoord.x, texcoord.y);
//}

void TextureCUBE::CalcTexCoord(const Vector3& s, CUBEAxis& axis, Vector2& texcoord)
{
	Vector3 ns = s.Normalize();

	float absx = Mathf::Abs(ns.x);
	float absy = Mathf::Abs(ns.y);
	float absz = Mathf::Abs(ns.z);

	if (absx > absy && absx > absz)
	{
		if (ns.x > 0)
		{
			axis = CUBEAxis_Right;
			texcoord = (Vector2(ns.z, ns.y) / absx + Vector2::one) / 2.f;
		}
		else
		{
			axis = CUBEAxis_Left;
			texcoord = (Vector2(-ns.z, ns.y) / absx + Vector2::one) / 2.f;
		}
	}
	else if (absy > absz)
	{
		if (ns.y > 0)
		{
			axis = CUBEAxis_Top;
			texcoord = (Vector2(ns.x, ns.z) / absy + Vector2::one) / 2.f;
		}
		else
		{
			axis = CUBEAxis_Bottom;
			texcoord = (Vector2(ns.x, -ns.z) / absy + Vector2::one) / 2.f;
		}
	}
	else
	{
		if (ns.z > 0)
		{
			axis = CUBEAxis_Front;
			texcoord = (Vector2(-ns.x, ns.y) / absz + Vector2::one) / 2.f;
		}
		else
		{
			axis = CUBEAxis_Back;
			texcoord = (Vector2(ns.x, ns.y) / absz + Vector2::one) / 2.f;
		}
	}
}

