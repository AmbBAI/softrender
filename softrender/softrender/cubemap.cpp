#include "cubemap.h"
#include "pbsf.hpp"

using namespace sr;

Color Cubemap::Sample(const Vector3& s, float rougness/* = 0.f*/) const
{
	switch (mappingType)
	{
	case Cubemap::MappingType_6Images:
		return Sample6Images(s);
	case Cubemap::MappingType_LatLong:
		return SampleLatlong(s, RoughnessToLod(rougness));
	default:
		return Color::black;
	}
}

void Cubemap::InitWith6Images(Texture2DPtr imgs[6])
{
	for (int i = 0; i < 6; ++i)
	{
		images[i] = imgs[i];
	}
	mappingType = MappingType_6Images;
}

void Cubemap::InitWithLatlong(Texture2DPtr tex)
{
	latlong = tex;
	latlong->xAddressMode = Texture2D::AddressMode_Warp;
	latlong->yAddressMode = Texture2D::AddressMode_Warp;
	mappingType = MappingType_LatLong;
}

bool Cubemap::Mapping6ImagesToLatlong(int height, Bitmap::BitmapType type)
{
	BitmapPtr bitmap = BitmapPtr(new Bitmap(height * 2, height, type));
	int weight = height * 2;
	for (int x = 0; x < weight; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			float u = float(x) * Mathf::PI * 2.f / weight;
			float v = float(y) * Mathf::PI / height;
			Vector3 dir = Vector3::zero;
			dir.x = -Mathf::Sin(u) * Mathf::Sin(v);
			dir.y = -Mathf::Cos(v);
			dir.z = -Mathf::Cos(u) * Mathf::Sin(v);
			bitmap->SetPixel(x, y, Sample6Images(dir));
		}
	}
	latlong = Texture2D::CreateWithBitmap(bitmap);
	latlong->xAddressMode = Texture2D::AddressMode_Warp;
	latlong->yAddressMode = Texture2D::AddressMode_Warp;

	for (int i = 0; i < 6; ++i)
	{
		images[i] = nullptr;
	}
	mappingType = MappingType_LatLong;
	return true;
}

Color Cubemap::Sample6Images(const Vector3& s) const
{
	int face;
	Vector2 texcoord;
	Direction6ImagesTexcoord(s, face, texcoord);

	Texture2DPtr tex = images[face];
	if (tex == nullptr) return Color::black;
	return tex->Sample(texcoord);
}

Color Cubemap::SampleLatlong(const Vector3& s, float lod) const
{
	if (latlong == nullptr) return Color::black;

	Vector2 texcoord;
	DirectionToLatlongTexcoord(s, texcoord);
	return latlong->Sample(texcoord, lod);
}

void Cubemap::DirectionToLatlongTexcoord(const Vector3& s, Vector2& texcoord) const
{
	Vector3 ns = s.Normalize();
	texcoord.x = Mathf::Atan2(ns.x, ns.z) * Mathf::invPI * 0.5f + 0.5f;
	texcoord.y = -Mathf::Acos(Mathf::Clamp(ns.y, -1.f, 1.f)) * Mathf::invPI;
}

void Cubemap::Direction6ImagesTexcoord(const Vector3& s, int& face, Vector2& texcoord) const
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

bool Cubemap::PrefilterEnvMap(uint32_t mapCount, uint32_t sampleCount) const
{
	if (mappingType != MappingType_LatLong)
	{
		return false;
	}

	int height = latlong->GetHeight();
	int width = latlong->GetWidth();
	Bitmap::BitmapType type = latlong->GetBitmap(0)->GetType();
	std::vector<BitmapPtr> bitmaps;
	for (uint32_t i = 0; i < mapCount; ++i)
	{
		height >>= 1;
		if (height < 1) height = 1;
		width = height * 2;

		BitmapPtr bitmap = BitmapPtr(new Bitmap(width, height, type));
		float roughness = float(i + 1) / mapCount;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				float u = float(x) * Mathf::PI * 2.f / width;
				float v = float(y) * Mathf::PI / height;
				Vector3 dir = Vector3::zero;
				dir.x = -Mathf::Sin(u) * Mathf::Sin(v);
				dir.y = -Mathf::Cos(v);
				dir.z = -Mathf::Cos(u) * Mathf::Sin(v);
				Vector3 rgb = PBSF::PrefilterEnvMap(*this, roughness, dir, sampleCount);
				bitmap->SetPixel(x, y, Color(rgb, 1.f));
			}
		}
		bitmaps.push_back(bitmap);
	}
	latlong->SetMipmaps(bitmaps);
	return true;
}

float Cubemap::RoughnessToLod(float rougness) const
{
	if (mappingType != MappingType_LatLong)
	{
		return 0.f;
	}

	return rougness * latlong->GetMipmapsCount();
}

bool Cubemap::Get6Images(Texture2DPtr img[6])
{
	if (mappingType != MappingType_6Images)
	{
		return false;
	}

	for (int i = 0; i < 6; ++i)
	{
		img[i] = images[i];
	}
	return true;
}

bool Cubemap::GetLagLong(Texture2DPtr& latlong)
{
	if (mappingType != Cubemap::MappingType_LatLong)
	{
		return false;
	}

	latlong = this->latlong;
	return true;
}
