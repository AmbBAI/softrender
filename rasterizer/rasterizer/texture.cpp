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

	int width = (int)FreeImage_GetWidth(fiBitmap);
	int height = (int)FreeImage_GetHeight(fiBitmap);
	int pitch = (int)FreeImage_GetPitch(fiBitmap);
	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
	int bpp = (int)(FreeImage_GetBPP(fiBitmap) >> 3);
	rawptr_t bytes = (rawptr_t)FreeImage_GetBits(fiBitmap);

	if (imageType != FIT_BITMAP)
	{
		printf("%s (imageType %d)\n", file, imageType);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return nullptr;
	}

	//TexturePtr tex = std::make_shared<Texture>();
	TexturePtr tex = TexturePtr(new Texture());
	tex->file = file;
	tex->mainTex = UnparkColor(bytes, width, height, pitch, bpp);
	if (tex->mainTex == nullptr) tex = nullptr;

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

BitmapPtr Texture::UnparkColor(rawptr_t bytes, int width, int height, int pitch, int bpp)
{
	if (bytes == nullptr) return nullptr;
	if (width <= 0 || height <= 0) return nullptr;

	Bitmap::BitmapType pixelType = Bitmap::BitmapType_Unknown;
	switch (bpp)
	{
	case 1:
		pixelType = Bitmap::BitmapType_L8;
		break;
	case 3:
		pixelType = Bitmap::BitmapType_RGB888;
		break;
	case 4:
		pixelType = Bitmap::BitmapType_RGBA8888;
		break;
	default:
		return nullptr;
	}

	BitmapPtr bitmap = Bitmap::Create(width, height, pixelType);
	rawptr_t bmpLine = bitmap->GetBytes();
	int bmpPitch = width * bpp;

	rawptr_t imgLine = bytes;
	for (int y = 0; y < height; ++y)
	{
		if (bpp == 1)
		{
			memcpy(bmpLine, imgLine, sizeof(uint8_t) * bmpPitch);
		}
		else
		{
			for (int x = 0; x < width; ++x)
			{
				rawptr_t imgByte = imgLine + x * bpp;
				rawptr_t bmpByte = bmpLine + x * bpp;
				*(bmpByte + 2) = *(imgByte + 0);
				*(bmpByte + 1) = *(imgByte + 1);
				*(bmpByte + 0) = *(imgByte + 2);
				if (bpp == 4) *(bmpByte + 3) = *(imgByte + 3);
			}
		}

		bmpLine += bmpPitch;
		imgLine += pitch;
	}
	return bitmap;
}

void Texture::ConvertBumpToNormal(float strength/* = 10.f*/)
{
	int width = mainTex->GetWidth();
	int height = mainTex->GetHeight();
	std::vector<float> bump(width * height, 0.f);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			bump[y * width + x] = mainTex->GetColor(x, y).a;
		}
	}

	mainTex = Bitmap::Create(width, height, Bitmap::BitmapType_RGBA8888);
	rawptr_t bytes = mainTex->GetBytes();
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

			*((uint32_t*)bytes + offset) = Color32(color).rgba;

			//float invZ = 1.f / (normal.z + 1.);
			//float px = normal.x * invZ;
			//float py = normal.y * invZ;
			//// float d = 2. / (1 + px * px + py * py);
			//// normal.x = px * d;
			//// normal.y = py * d;
			//// normal.z = d - 1;

			//*(bytes + (offset << 1)) = (u8)(px * 255.f);
			//*(bytes + (offset << 1 | 1)) = (u8)(py * 255.f);
		}
	}
}

const Color Texture::Sample(float u, float v, float lod/* = 0.f*/) const
{
	switch (filterMode) {
	case FilterMode_Point:
        {
            int miplv = Mathf::RoundToInt(lod);
            
            const BitmapPtr bmp = GetBitmap(miplv);
            assert(bmp != nullptr);
            return sampleFunc[0][xAddressMode][xAddressMode](bmp, u, v);
        }
	case FilterMode_Bilinear:
        {
            int miplv = Mathf::RoundToInt(lod);
            
			const BitmapPtr bmp = GetBitmap(miplv);
			assert(bmp != nullptr);
            return sampleFunc[1][xAddressMode][xAddressMode](bmp, u, v);
        }
	case FilterMode_Trilinear:
        {
            int miplv1 = Mathf::FloorToInt(lod);
            int miplv2 = miplv1 + 1;
            float frac = lod - miplv1;
            
			const BitmapPtr bmp1 = GetBitmap(miplv1);
            assert(bmp1 != nullptr);
            Color color1 = sampleFunc[1][xAddressMode][xAddressMode](bmp1, u, v);
            
			const BitmapPtr bmp2 = GetBitmap(miplv2);
            if (bmp2 == bmp1) return color1;
            Color color2 = sampleFunc[1][xAddressMode][xAddressMode](bmp2, u, v);
            return Color::Lerp(color1, color2, frac);
        }
	}
    return Color::black;
}


bool Texture::GenerateMipmaps()
{
	int width = mainTex->GetWidth();
	int height = mainTex->GetHeight();

	if (width != height) return false;
	if (!Mathf::IsPowerOfTwo(width)) return false;
	
	mipmaps.clear();

	BitmapPtr source = mainTex;
	int s = (width >> 1);
	int ss = width;
	for (int l = 0;; ++l)
	{
		BitmapPtr mipmap = Bitmap::Create(s, s, Bitmap::BitmapType_RGBA8888);
		rawptr_t bytes = mipmap->GetBytes();
		for (int y = 0; y < s; ++y)
		{
			int y0 = y * 2;
			int y1 = y0 + 1;
			for (int x = 0; x < s; ++x)
			{
				int x0 = x * 2;
				int x1 = x0 + 1;
				Color c0 = source->GetColor(x0, y0);
				Color c1 = source->GetColor(x1, y0);
				Color c2 = source->GetColor(x0, y1);
				Color c3 = source->GetColor(x1, y1);
				*((uint32_t*)bytes + y * s + x) = Color32(Color::Lerp(c0, c1, c2, c3, 0.5f, 0.5f)).rgba;
			}
		}

		mipmaps.push_back(mipmap);
		source = mipmaps[l];
		ss = s;
		s >>= 1;
		if (s <= 0) break;
	}
	return true;
}

const BitmapPtr Texture::GetBitmap(int miplv) const
{
    if (miplv < 0) miplv = 0;
	if (mipmaps.size() <= 0) miplv = 0;
	if (miplv == 0) return mainTex;
	else
	{
		miplv = Mathf::Clamp(miplv, 0, (int)mipmaps.size());
		return mipmaps[miplv - 1];
	}
}

void Texture::CompressTexture()
{
	if (mainTex->GetType() == Bitmap::BitmapType_RGB888)
	{
		mainTex = mainTex->CompressToDXT1();
	}
}


}
