#ifndef _BASE_TEXTURE_H_
#define _BASE_TEXTURE_H_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"

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
    
	typedef std::vector<Color32> Bitmap;
	typedef Color(*SampleFunc)(const Texture::Bitmap& bitmap, u32 width, u32 height, float u, float v);
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
	u32 GetWidth();
	u32 GetHeight();
	u32 GetBPP();

	void ConvertBumpToNormal(float strength = 2.0f);
	bool GenerateMipmaps();

	const Color GetColor(u32 x, u32 y) const;
	const Color Sample(float u, float v) const;

protected:
    bool UnparkColor(u8* bytes, u32 width, u32 height, u32 pitch, u32 bpp);
    
public:
    AddressMode xAddressMode = AddressMode_Warp;
	AddressMode yAddressMode = AddressMode_Warp;
    FilterMode filterMode = FilterMode_Bilinear;
    
protected:
	u32 width = 0;
	u32 height = 0;
	u32 bpp = 3;

	Bitmap bitmap;
    
    bool isMipmapCreated = false;
	std::vector<Bitmap> mipmaps;

};

}

#endif //! _BASE_TEXTURE_H_