#ifndef _BASE_TEXTURE_H_
#define _BASE_TEXTURE_H_

#include "base/header.h"
#include "rasterizer/bitmap.h"
#include "base/color.h"

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
    u32 GetWidth() { return mainTex ? mainTex->GetWidth() : 0; }
    u32 GetHeight() { return mainTex ? mainTex->GetHeight() : 0; }

	void ConvertBumpToNormal(float strength = 10.f);
	bool GenerateMipmaps();

	const Color Sample(float u, float v, float lod = 0.f) const;

protected:
	static BitmapPtr UnparkColor(u8* bytes, u32 width, u32 height, u32 pitch, u32 bpp);
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