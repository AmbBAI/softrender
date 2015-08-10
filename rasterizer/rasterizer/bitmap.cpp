#include "bitmap.h"
#include "../thirdpart/freeimage/FreeImage.h"
using namespace sr;

Bitmap::Bitmap(int width, int height, BitmapType type)
{
	this->width = width;
	this->height = height;
	this->type = type;

	int size = width * height;
	switch (type)
	{
	case Bitmap::BitmapType_Alpha8:
		break;
	case Bitmap::BitmapType_RGB24:
		size *= 3;
		break;
	case Bitmap::BitmapType_RGBA32:
		size <<= 2;
		break;
	case Bitmap::BitmapType_AlphaFloat:
		size <<= 2;
		break;
		//case Bitmap::BitmapType_DXT1:
		//	if (width & 3) return nullptr;
		//	if (height & 3) return nullptr;
		//	size = (size >> 4) << 3;
		//	break;
		//case Bitmap::BitmapType_Normal:
		//	size <<= 1;
		//	break;
	case Bitmap::BitmapType_Unknown:
		assert(false);
		return;
	}

	bytes = new uint8_t[size];
}

Bitmap::~Bitmap()
{
	if (bytes != nullptr)
	{
		delete bytes;
		bytes = nullptr;
	}
}

uint8_t Bitmap::GetPixel_Alpha8(int x, int y) const
{
	return (uint8_t)*(bytes + (y * width + x));
}

Color32 Bitmap::GetPixel_RGB24(int x, int y) const
{
	rawptr_t byte = bytes + (y * width + x) * 3;
	uint8_t r = *byte;
	uint8_t g = *(byte + 1);
	uint8_t b = *(byte + 2);
	return Color32(255, r, g, b);
}

Color32 Bitmap::GetPixel_RGBA32(int x, int y) const
{
	rawptr_t byte = bytes + (y * width + x) * 4;
	uint8_t r = *byte;
	uint8_t g = *(byte + 1);
	uint8_t b = *(byte + 2);
	uint8_t a = *(byte + 3);
	return Color32(a, r, g, b);
}

float Bitmap::GetPixel_AlphaFloat(int x, int y) const
{
	return *(float*)(bytes + (y * width + x) * 4);
}

void Bitmap::SetPixel_Alpha8(int x, int y, uint8_t val)
{
	*(uint8_t*)(bytes + (y * width + x)) = val;
}

void Bitmap::SetPixel_RGB24(int x, int y, Color32 color)
{
	rawptr_t byte = bytes + (y * width + x) * 3;
	*byte = color.r;
	*(byte + 1) = color.g;
	*(byte + 2) = color.b;
}

void Bitmap::SetPixel_RGBA32(int x, int y, Color32 color)
{
	rawptr_t byte = bytes + (y * width + x) * 4;
	*byte = color.r;
	*(byte + 1) = color.g;
	*(byte + 2) = color.b;
	*(byte + 3) = color.a;
}

void Bitmap::SetPixel_AlphaFloat(int x, int y, float val)
{
	*(float*)(bytes + (y * width + x) * 4) = val;
}

float Bitmap::GetAlpha(int x, int y) const
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	switch (type)
	{
	case sr::Bitmap::BitmapType_Alpha8:
		return GetPixel_Alpha8(x, y) / 255.f;
	case sr::Bitmap::BitmapType_RGB24:
		return 1.f;
	case sr::Bitmap::BitmapType_RGBA32:
		return *(uint8_t*)(bytes + (y * width + x) * 4 + 3) / 255.f;
	case sr::Bitmap::BitmapType_AlphaFloat:
		return GetPixel_AlphaFloat(x, y);
	default:
		return 1.f;
	}
}

Color Bitmap::GetPixel(int x, int y) const
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	switch (type)
	{
	case BitmapType_Alpha8:
		return Color(GetPixel_Alpha8(x, y) / 255.f, 1.f, 1.f, 1.f);
	case BitmapType_RGB24:
		return GetPixel_RGB24(x, y);
	case BitmapType_RGBA32:
		return GetPixel_RGBA32(x, y);
	case BitmapType_AlphaFloat:
		return Color(GetPixel_AlphaFloat(x, y), 1.f, 1.f, 1.f);
	}
	return Color::black;
}

void Bitmap::SetPixel(int x, int y, const Color& color)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);
	switch (type)
	{
	case sr::Bitmap::BitmapType_Alpha8:
		SetPixel_Alpha8(x, y, Color32(color).a);
		break;
	case sr::Bitmap::BitmapType_RGB24:
		SetPixel_RGB24(x, y, color);
		break;
	case sr::Bitmap::BitmapType_RGBA32:
		SetPixel_RGBA32(x, y, color);
		break;
	case sr::Bitmap::BitmapType_AlphaFloat:
		SetPixel_AlphaFloat(x, y, color.a);
		break;
	default:
		break;
	}
}

void Bitmap::SetAlpha(int x, int y, float alpha)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);
	switch (type)
	{
	case sr::Bitmap::BitmapType_Alpha8:
		SetPixel_Alpha8(x, y, (uint8_t)(Mathf::Clamp01(alpha) * 255.f));
		break;
	case sr::Bitmap::BitmapType_RGB24:
		break;
	case sr::Bitmap::BitmapType_RGBA32:
		*(uint8_t*)(bytes + (y * width + x) * 4 + 3) = (uint8_t)(Mathf::Clamp01(alpha) * 255.f);
		break;
	case sr::Bitmap::BitmapType_AlphaFloat:
		SetPixel_AlphaFloat(x, y, alpha);
		break;
	default:
		break;
	}
}

void Bitmap::Fill(const Color& color)
{
	assert(bytes != nullptr);

	switch (type)
	{
	case sr::Bitmap::BitmapType_Alpha8:
		std::memset(bytes, Color32(color).a, width * height);
		//std::fill_n((uint8_t*)bytes, width * height, Color32(color).a);
		break;
	case sr::Bitmap::BitmapType_RGB24:
		{
			Color32 c32 = color;
			for (int i = 0; i < width * height; ++i)
			{
				int offset = i * 3;
				*(uint8_t*)(bytes + offset) = c32.r;
				*(uint8_t*)(bytes + offset + 1) = c32.g;
				*(uint8_t*)(bytes + offset + 2) = c32.b;
			}
		}
		break;
	case sr::Bitmap::BitmapType_RGBA32:
		std::fill_n((uint32_t*)bytes, width * height, Color32(color).rgba);
		break;
	case sr::Bitmap::BitmapType_AlphaFloat:
		std::fill_n((float*)bytes, width * height, color.a);
		break;
	default:
		break;
	}
}

BitmapPtr Bitmap::LoadFromFile(const char* file)
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
	rawptr_t imageBytes = (rawptr_t)FreeImage_GetBits(fiBitmap);

	if (imageType != FIT_BITMAP)
	{
		printf("%s (imageType %d)\n", file, imageType);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return nullptr;
	}

	Bitmap::BitmapType pixelType = Bitmap::BitmapType_Unknown;
	switch (bpp)
	{
	case 1:
		pixelType = Bitmap::BitmapType_Alpha8;
		break;
	case 3:
		pixelType = Bitmap::BitmapType_RGB24;
		break;
	case 4:
		pixelType = Bitmap::BitmapType_RGBA32;
		break;
	default:
		return nullptr;
	}

	BitmapPtr bitmap = std::make_shared<Bitmap>(width, height, pixelType);
	assert(pitch == bpp * width);
	memcpy(bitmap->bytes, imageBytes, bpp * width * height);
	FreeImage_Unload(fiBitmap);

	switch (bitmap->type)
	{
	case sr::Bitmap::BitmapType_RGB24:
	{
		rawptr_t bitmapPtr = bitmap->bytes;
		for (int i = 0; i < width * height; ++i)
		{
			FlipRGB(bitmapPtr);
			bitmapPtr += 3;
		}
	}
	break;
	case sr::Bitmap::BitmapType_RGBA32:
	{
		rawptr_t bitmapPtr = bitmap->bytes;
		for (int i = 0; i < width * height; ++i)
		{
			FlipRGB(bitmapPtr);
			bitmapPtr += 4;
		}
	}
	break;
	default:
		break;
	}
	return bitmap;
}

bool Bitmap::SaveToFile(const char* file)
{
	switch (type)
	{
	case sr::Bitmap::BitmapType_Unknown:
		return false;
	case sr::Bitmap::BitmapType_Alpha8:
	{
		FIBITMAP* fiBitmap = FreeImage_AllocateT(FIT_BITMAP, width, height, 8);
		if (fiBitmap == nullptr) return false;
		rawptr_t ptr = FreeImage_GetBits(fiBitmap);
		memcpy(ptr, bytes, width * height);
		bool ret = !!FreeImage_Save(FIF_PNG, fiBitmap, file);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return ret;
	}
	case sr::Bitmap::BitmapType_RGB24:
	{
		FIBITMAP* fiBitmap = FreeImage_AllocateT(FIT_BITMAP, width, height, 24);
		if (fiBitmap == nullptr) return false;
		rawptr_t imagePtr = FreeImage_GetBits(fiBitmap);
		memcpy(imagePtr, bytes, width * height * 3);
		for (int i = 0; i < width * height; ++i)
		{
			FlipRGB(imagePtr);
			imagePtr += 3;
		}
		bool ret = !!FreeImage_Save(FIF_PNG, fiBitmap, file);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return ret;
	}
	case sr::Bitmap::BitmapType_RGBA32:
	{
		FIBITMAP* fiBitmap = FreeImage_AllocateT(FIT_BITMAP, width, height, 32);
		if (fiBitmap == nullptr) return false;
		rawptr_t imagePtr = FreeImage_GetBits(fiBitmap);
		memcpy(imagePtr, bytes, width * height * 4);
		for (int i = 0; i < width * height; ++i)
		{
			FlipRGB(imagePtr);
			imagePtr += 4;
		}
		bool ret = !!FreeImage_Save(FIF_PNG, fiBitmap, file);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return ret;
	}
	case sr::Bitmap::BitmapType_AlphaFloat:
	{
		FIBITMAP* fiBitmap = FreeImage_AllocateT(FIT_FLOAT, width, height, 32);
		if (fiBitmap == nullptr) return false;
		rawptr_t ptr = FreeImage_GetBits(fiBitmap);
		memcpy(ptr, bytes, width * height * sizeof(float));
		bool ret = !!FreeImage_Save(FIF_TIFF, fiBitmap, file);
		FreeImage_Unload(fiBitmap);
		fiBitmap = nullptr;
		return ret;
	}
	default:
		break;
	}
	return false;
}



