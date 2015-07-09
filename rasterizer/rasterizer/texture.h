#ifndef _BASE_TEXTURE_H_
#define _BASE_TEXTURE_H_

#include "base/header.h"
#include "rasterizer/bitmap.h"
#include "base/color.h"
#include "math/vector2.h"

namespace rasterizer
{

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class Texture
{
public:
	enum TextureType
	{
		TextureType_Texture,
		TextureType_NormalMap,
	};

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

	typedef Color(*SampleFunc)(const BitmapPtr bitmap, float u, float v);
	static SampleFunc sampleFunc[2][AddressModeCount][AddressModeCount];

public:
	static void Initialize();
	static void Finalize();

	static TexturePtr CreateTexture(const char* file);
    static TexturePtr LoadTexture(const char* file);
	static std::map<std::string, TexturePtr> texturePool;

public:
	Texture() = default;
	virtual ~Texture() = default;

public:
	int GetWidth() const { return mainTex ? mainTex->GetWidth() : 0; }
	int GetHeight() const { return mainTex ? mainTex->GetHeight() : 0; }

	void ConvertBumpToNormal(float strength = 10.f);
	bool GenerateMipmaps();
	void CompressTexture();

	float CalcLOD(const Vector2& ddx, const Vector2& ddy) const;
	const Color Sample(const Vector2& uv, float lod = 0.f) const;
	const Color Sample(const Vector2& uv, const Vector2& ddx, const Vector2& ddy) const { return Sample(uv, CalcLOD(ddx, ddy)); }

protected:
	static BitmapPtr UnparkColor(rawptr_t bytes, int width, int height, int pitch, int bpp);
	const BitmapPtr GetBitmap(int miplv) const;
    
public:
    AddressMode xAddressMode = AddressMode_Warp;
	AddressMode yAddressMode = AddressMode_Warp;
    FilterMode filterMode = FilterMode_Bilinear;
    
protected:
	std::string file;

	BitmapPtr mainTex;
	std::vector<BitmapPtr> mipmaps;

};

}

#endif //! _BASE_TEXTURE_H_