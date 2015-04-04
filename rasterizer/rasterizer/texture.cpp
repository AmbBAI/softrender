#include "texture.h"
#include "math/mathf.h"
#include "freeimage/FreeImage.h"
#include "sampler.hpp"

namespace rasterizer
{

Texture::SampleFunc Texture::sampleFunc[2][AddressModeCount][AddressModeCount] = {
	{
		{
			PointSampler::Sample < WarpAddresser, WarpAddresser >,
			PointSampler::Sample < WarpAddresser, MirrorAddresser >,
			PointSampler::Sample < WarpAddresser, ClampAddresser >
		},
		{
			PointSampler::Sample < MirrorAddresser, WarpAddresser >,
			PointSampler::Sample < MirrorAddresser, MirrorAddresser >,
			PointSampler::Sample < MirrorAddresser, ClampAddresser >
		},
		{
			PointSampler::Sample < ClampAddresser, WarpAddresser >,
			PointSampler::Sample < ClampAddresser, MirrorAddresser >,
			PointSampler::Sample < ClampAddresser, ClampAddresser >
		},
	},
	{
		{
			LinearSampler::Sample < WarpAddresser, WarpAddresser >,
			LinearSampler::Sample < WarpAddresser, MirrorAddresser >,
			LinearSampler::Sample < WarpAddresser, ClampAddresser >
		},
		{
			LinearSampler::Sample < MirrorAddresser, WarpAddresser >,
			LinearSampler::Sample < MirrorAddresser, MirrorAddresser >,
			LinearSampler::Sample < MirrorAddresser, ClampAddresser >
		},
		{
			LinearSampler::Sample < ClampAddresser, WarpAddresser >,
			LinearSampler::Sample < ClampAddresser, MirrorAddresser >,
			LinearSampler::Sample < ClampAddresser, ClampAddresser >
		},
	},
};



void Texture::Initialize()
{
	FreeImage_Initialise();
}

void Texture::Finalize()
{
	FreeImage_DeInitialise();
}

std::map<std::string, TexturePtr> Texture::texturePool;
TexturePtr Texture::CreateTexture(const char* file)
{
	FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(file);
	FIBITMAP* fiBitmap = FreeImage_Load(imageFormat, file);
	if (fiBitmap == nullptr) return nullptr;

	u32 width = FreeImage_GetWidth(fiBitmap);
	u32 height = FreeImage_GetHeight(fiBitmap);
	u32 pitch = FreeImage_GetPitch(fiBitmap);
	u32 bpp = (FreeImage_GetBPP(fiBitmap) >> 3);
	BYTE* bytes = FreeImage_GetBits(fiBitmap);

	TexturePtr tex(new Texture());
	tex->UnparkColor(bytes, width, height, pitch, bpp);

	FreeImage_Unload(fiBitmap);
	return tex;
}

TexturePtr Texture::LoadTexture(const char* file)
{
	std::map<std::string, TexturePtr>::iterator itor;
	itor = texturePool.find(file);
	if (itor != texturePool.end())
	{
		return itor->second;
	}
	else
	{
		TexturePtr tex = CreateTexture(file);
		if (tex != nullptr) texturePool[file] = tex;
		return tex;
	}
}

u32 Texture::GetWidth()
{
	return width;
}

u32 Texture::GetHeight()
{
	return height;
}

u32 Texture::GetBPP()
{
	return bpp;
}

bool Texture::UnparkColor(u8* bytes, u32 width, u32 height, u32 pitch, u32 bpp)
{
	if (bytes == nullptr) return false;
	if (width <= 0 || height <= 0) return false;
	if (bpp != 1 && bpp != 3 && bpp != 4) return false;

	this->width = width;
	this->height = height;
	this->bpp = bpp;
	this->bitmap.assign(width * height, Color32::black);

	u8* line = bytes;
	for (u32 y = 0; y < height; ++y)
	{
		u8* byte = line;
		for (u32 x = 0; x < width; ++x)
		{
			Color32& color = this->bitmap[y * width + x];
			if (bpp == 1)
			{
				color.a = color.r = color.g = color.b = byte[0];
			}
			else
			{
				color.b = byte[0];
				color.g = byte[1];
				color.r = byte[2];
				color.a = (bpp == 4) ? byte[3] : 255;
			}
			byte += bpp;
		}
		line += pitch;
	}
	return true;
}

void Texture::ConvertBumpToNormal(float strength/* = 2.0f*/)
{
	std::vector<float> bump(width * height, 0.f);
	for (int i = 0; i < (int)bitmap.size(); ++i)
	{
		bump[i] = bitmap[i].b;
	}

	for (int y = 0; y < (int)height; ++y)
	{
		for (int x = 0; x < (int)width; ++x)
		{
			int offset = y * width + x;
			int x1 = x - 1;
			int x2 = x + 1;
			int y1 = y - 1;
			int y2 = y + 1;

			if (x1 < 0) x1 = 0;
			if (x2 >= (int)width) x2 = width - 1;
			if (y1 < 0) y1 = 0;
			if (y2 >= (int)height) y2 = height - 1;

			float ph = bump[offset];
			float px1 = bump[y * width + x1];
			float px2 = bump[y * width + x2];
			float py1 = bump[y1 * width + x];
			float py2 = bump[y2 * width + x];
			Vector3 xOff = Vector3(strength, 0.f, px1 - px2).Normalize();
			Vector3 yOff = Vector3(0.f, strength, py1 - py2).Normalize();
			Vector3 normal = xOff.Cross(yOff);

			Color color;
			color.r = (normal.x + 1.0f) / 2.0f;
			color.g = (normal.y + 1.0f) / 2.0f;
			color.b = (normal.z + 1.0f) / 2.0f;
			color.a = ph;
			bitmap[offset] = color;
		}
	}
}

const Color Texture::GetColor(u32 x, u32 y) const
{
	assert(x < width);
	assert(y < height);

	u32 offset = y * width + x;
	assert(offset < (int)bitmap.size());

	return Color32(bitmap[offset]);
}

const Color Texture::Sample(float u, float v) const
{
	switch (filterMode) {
	case FilterMode_Point:
		return sampleFunc[0][xAddressMode][xAddressMode](bitmap, width, height, u, v);
	case FilterMode_Bilinear:
		//TODO bilinear with mipmap
		//if (isMipmapCreated) ; else
		return sampleFunc[1][xAddressMode][xAddressMode](bitmap, width, height, u, v);
	case FilterMode_Trilinear:
		//TODO trilinear
		return sampleFunc[1][xAddressMode][xAddressMode](bitmap, width, height, u, v);
	default:
		return sampleFunc[1][xAddressMode][xAddressMode](bitmap, width, height, u, v);
	}
}

bool Texture::GenerateMipmaps()
{
	if (width != height) return false;
	if (!Mathf::IsPowerOfTwo(width)) return false;
	if (bitmap.size() != width * height) return false;

	mipmaps.clear();

	Bitmap* source = &bitmap;
	u32 s = (width >> 1);
	for (int l = 0;; ++l)
	{
		Bitmap mipmap(s * s, Color32::black);
		for (u32 y = 0; y < s; ++y)
		{
			u32 y0 = y * 2;
			u32 y1 = y0 + 1;
			for (u32 x = 0; x < s; ++x)
			{
				u32 x0 = x * 2;
				u32 x1 = x0 + 1;
				Color c0 = (*source)[y0 * s + x0];
				Color c1 = (*source)[y0 * s + x1];
				Color c2 = (*source)[y1 * s + x0];
				Color c3 = (*source)[y1 * s + x1];

				mipmap[y * s + x] = Color::Lerp(c0, c1,c2, c3, 0.5f, 0.5f);
			}
		}

		mipmaps.push_back(mipmap);
		source = &mipmaps[l];
		s >>= 1;
		if (s <= 0) break;
	}
	return true;
}
}
