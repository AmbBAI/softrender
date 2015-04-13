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
	if (fiBitmap == nullptr)
    {
        printf("%s FAILED!\n", file);
        return nullptr;
    }
    
	u32 width = FreeImage_GetWidth(fiBitmap);
	u32 height = FreeImage_GetHeight(fiBitmap);
	u32 pitch = FreeImage_GetPitch(fiBitmap);
	u32 bpp = (FreeImage_GetBPP(fiBitmap) >> 3);
	BYTE* bytes = FreeImage_GetBits(fiBitmap);

	TexturePtr tex(new Texture());
	tex->file = file;
	tex->UnparkColor(bytes, width, height, pitch, bpp);

	FreeImage_Unload(fiBitmap);
    printf("%s SUCCEED!\n", file);
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

	//TODO S3TC
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

void Texture::ConvertBumpToNormal(float strength/* = 0.04f*/)
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
            Vector3 normal = Vector3(px1 - px2, py1 - py2, 1.f / strength).Normalize();

			Color color;
			color.r = (normal.x + 1.0f) / 2.0f;
			color.g = (normal.y + 1.0f) / 2.0f;
			color.b = (normal.z + 1.0f) / 2.0f;
			color.a = ph;
			bitmap[offset] = color;
		}
	}
}

const Color Texture::GetColor(u32 x, u32 y, int miplv/* = 0*/) const
{
	u32 bmp_width = width;
	u32 bmp_height = height;
	const Bitmap* bmp = CheckMipmap(bmp_width, bmp_height, miplv);
	assert(bmp != nullptr);

	assert(x < bmp_width);
	assert(y < bmp_height);

	u32 offset = y * bmp_width + x;
	assert(offset < (int)bitmap.size());

	return (*bmp)[offset];
}

const Color Texture::Sample(float u, float v, float lod/* = 0.f*/) const
{
	switch (filterMode) {
	case FilterMode_Point:
        {
            int miplv = Mathf::RoundToInt(lod);
            
            u32 bmp_width = width;
            u32 bmp_height = height;
            const Bitmap* bmp = CheckMipmap(bmp_width, bmp_height, miplv);
            assert(bmp != nullptr);
            return sampleFunc[0][xAddressMode][xAddressMode](*bmp, bmp_width, bmp_height, u, v);
        }
	case FilterMode_Bilinear:
        {
            int miplv = Mathf::RoundToInt(lod);
            
            u32 bmp_width = width;
            u32 bmp_height = height;
            const Bitmap* bmp = CheckMipmap(bmp_width, bmp_height, miplv);
            assert(bmp != nullptr);
            return sampleFunc[1][xAddressMode][xAddressMode](*bmp, bmp_width, bmp_height, u, v);
        }
	case FilterMode_Trilinear:
        {
            int miplv1 = Mathf::FloorToInt(lod);
            int miplv2 = miplv1 + 1;
            float frac = lod - miplv1;
            
            u32 bmp_width = width;
            u32 bmp_height = height;
            const Bitmap* bmp1 = CheckMipmap(bmp_width, bmp_height, miplv1);
            assert(bmp1 != nullptr);
            Color color1 = sampleFunc[1][xAddressMode][xAddressMode](*bmp1, bmp_width, bmp_height, u, v);
            
            const Bitmap* bmp2 = CheckMipmap(bmp_width, bmp_height, miplv2);
            if (bmp2 == bmp1) return color1;
            Color color2 = sampleFunc[1][xAddressMode][xAddressMode](*bmp2, bmp_width, bmp_height, u, v);
            return Color::Lerp(color1, color2, frac);
        }
	}
    return Color::black;
}


bool Texture::GenerateMipmaps()
{
	if (width != height) return false;
	if (!Mathf::IsPowerOfTwo(width)) return false;
	if (bitmap.size() != width * height) return false;

	mipmaps.clear();

	Bitmap* source = &bitmap;
	u32 s = (width >> 1);
	u32 ss = width;
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
				Color c0 = (*source)[y0 * ss + x0];
				Color c1 = (*source)[y0 * ss + x1];
				Color c2 = (*source)[y1 * ss + x0];
				Color c3 = (*source)[y1 * ss + x1];
				mipmap[y * s + x] = Color::Lerp(c0, c1, c2, c3, 0.5f, 0.5f);
			}
		}

		mipmaps.push_back(mipmap);
		source = &mipmaps[l];
		ss = s;
		s >>= 1;
		if (s <= 0) break;
	}
	return true;
}

const Texture::Bitmap* Texture::CheckMipmap(u32& bmp_width, u32& bmp_height, int miplv) const
{
    if (miplv < 0) miplv = 0;
	if (mipmaps.size() <= 0) miplv = 0;
	const Bitmap* bmp = nullptr;
	bmp_width = width;
	bmp_height = height;
	if (miplv == 0) bmp = &bitmap;
	else
	{
		miplv = Mathf::Clamp(miplv, 0, (int)mipmaps.size());
		bmp = &mipmaps[miplv - 1];
		bmp_width >>= miplv;
		bmp_height >>= miplv;
	}
	assert(bmp != nullptr);
	return bmp;
}

}
