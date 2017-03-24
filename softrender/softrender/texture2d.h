#ifndef _SOFTRENDER_TEXTURE2D_H_
#define _SOFTRENDER_TEXTURE2D_H_

#include "base/header.h"
#include "softrender/bitmap.h"
#include "math/color.h"
#include "math/vector2.h"

namespace sr
{

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;

class Texture2D
{
public:
	enum AddressMode
	{
		AddressMode_Warp = 0,
		AddressMode_Mirror = 1,
		AddressMode_Clamp = 2,
		AddressModeCount = 3
	};

	enum FilterMode
	{
		FilterMode_Point = 0,
		FilterMode_Bilinear,
		FilterMode_Trilinear
	};

public:
	static void Initialize();
	static void Finalize();

	static Texture2DPtr CreateWithBitmap(BitmapPtr& bitmap);
    static Texture2DPtr LoadTexture(const char* file);
	static std::map<std::string, Texture2DPtr> texturePool;

protected:
	typedef Color(*SampleFunc)(const Bitmap& bitmap, float u, float v);
	static SampleFunc sampleFunc[2][AddressModeCount][AddressModeCount];

	Texture2D() = default;

public:
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	void ConvertBumpToNormal(float strength = 10.f);
	bool GenerateMipmaps();
	//void CompressTexture();

	float CalcLOD(const Vector2& ddx, const Vector2& ddy) const;
	const Color Sample(const Vector2& uv, float lod = 0.f) const;
	const Color Sample(const Vector2& uv, const Vector2& ddx, const Vector2& ddy) const { return Sample(uv, CalcLOD(ddx, ddy)); }

	float SampleProj(const Vector2& uv, float value, float bias/*=0*/) const;

	int GetMipmapsCount() const;
	void SetMipmaps(std::vector<BitmapPtr>& bitmaps);
	const BitmapPtr GetBitmap(int miplv) const;
    
public:
    AddressMode xAddressMode = AddressMode_Warp;
	AddressMode yAddressMode = AddressMode_Warp;
    FilterMode filterMode = FilterMode_Bilinear;
    
protected:
	std::string file;

	int width;
	int height;
	BitmapPtr mainTex;
	std::vector<BitmapPtr> mipmaps;
};

}

#endif //! _SOFTRENDER_TEXTURE2D_H_
