#include "cubemap.h"

using namespace rasterizer;

Color Cubemap::Sample(const Vector3& s)
{
	CubemapFace axis;
	Vector2 texcoord;
	CalcTexCoord(s, axis, texcoord);

	TexturePtr tex = maps[axis];
	if (tex == nullptr) return Color::black;
	return tex->Sample(texcoord);
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

void Cubemap::CalcTexCoord(const Vector3& s, CubemapFace& face, Vector2& texcoord)
{
	Vector3 ns = s.Normalize();

	float absx = Mathf::Abs(ns.x);
	float absy = Mathf::Abs(ns.y);
	float absz = Mathf::Abs(ns.z);

	if (absx > absy && absx > absz)
	{
		if (ns.x > 0)
		{
			face = CubemapFace_PositiveX;
			texcoord = (Vector2(ns.z, ns.y) / absx + Vector2::one) / 2.f;
		}
		else
		{
			face = CubemapFace_NegativeX;
			texcoord = (Vector2(-ns.z, ns.y) / absx + Vector2::one) / 2.f;
		}
	}
	else if (absy > absz)
	{
		if (ns.y > 0)
		{
			face = CubemapFace_PositiveY;
			texcoord = (Vector2(ns.x, ns.z) / absy + Vector2::one) / 2.f;
		}
		else
		{
			face = CubemapFace_NegativeY;
			texcoord = (Vector2(ns.x, -ns.z) / absy + Vector2::one) / 2.f;
		}
	}
	else
	{
		if (ns.z > 0)
		{
			face = CubemapFace_PositiveZ;
			texcoord = (Vector2(-ns.x, ns.y) / absz + Vector2::one) / 2.f;
		}
		else
		{
			face = CubemapFace_NegativeZ;
			texcoord = (Vector2(ns.x, ns.y) / absz + Vector2::one) / 2.f;
		}
	}
}

void Cubemap::SetTexture(CubemapFace face, TexturePtr tex)
{
	maps[face] = tex;
}

