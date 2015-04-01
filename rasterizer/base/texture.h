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
	enum AddressMode
	{
		AddressMode_Warp,
		AddressMode_Clamp,
		AddressMode_Mirror,
	};

	enum TextureType
	{
		TextureType_Texture,
		TextureType_NormalMap,
	};

	typedef std::vector<Color32> Bitmap;

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

	const Color GetColor(int x, int y) const;
	const Color Sample(float u, float v) const;
    const Color PointSample(float u, float v) const;
    const Color LinearSample(float u, float v) const;

protected:
	bool UnparkColor(u8* bytes, u32 width, u32 height, u32 pitch, u32 bpp);

protected:
	u32 width = 0;
	u32 height = 0;
	u32 bpp = 3;
	AddressMode addressMode = AddressMode_Warp;

	Bitmap colors;
	std::vector<Bitmap> mipmaps;
};

}

#endif //! _BASE_TEXTURE_H_