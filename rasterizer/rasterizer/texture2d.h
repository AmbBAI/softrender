#ifndef _RASTERIZER_TEXTURE2D_H_
#define _RASTERIZER_TEXTURE2D_H_

#include "base/header.h"
#include "rasterizer/bitmap.h"
#include "base/color.h"
#include "math/vector2.h"

namespace rasterizer
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

	static Texture2DPtr CreateWithBitmap(BitmapPtr bitmap);
    static Texture2DPtr LoadTexture(const char* file);
	static std::map<std::string, Texture2DPtr> texturePool;

protected:
	typedef Color(*SampleFunc)(const BitmapPtr bitmap, float u, float v);
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

	const Vector4 SampleProj(const Vector2& uv, float bias = 0.f) const;

protected:
	static BitmapPtr UnparkColor(rawptr_t bytes, int width, int height, int pitch, int bpp);
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

#endif //! _RASTERIZER_TEXTURE2D_H_