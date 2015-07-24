#include "bitmap.h"
using namespace rasterizer;

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
	case rasterizer::Bitmap::BitmapType_Alpha8:
		return GetPixel_Alpha8(x, y) / 255.f;
	case rasterizer::Bitmap::BitmapType_RGB24:
		return 1.f;
	case rasterizer::Bitmap::BitmapType_RGBA32:
		return *(uint8_t*)(bytes + (y * width + x) * 4 + 3) / 255.f;
	case rasterizer::Bitmap::BitmapType_AlphaFloat:
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
	case rasterizer::Bitmap::BitmapType_Alpha8:
		SetPixel_Alpha8(x, y, Color32(color).a);
		break;
	case rasterizer::Bitmap::BitmapType_RGB24:
		SetPixel_RGB24(x, y, color);
		break;
	case rasterizer::Bitmap::BitmapType_RGBA32:
		SetPixel_RGBA32(x, y, color);
		break;
	case rasterizer::Bitmap::BitmapType_AlphaFloat:
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
	case rasterizer::Bitmap::BitmapType_Alpha8:
		SetPixel_Alpha8(x, y, (uint8_t)(Mathf::Clamp01(alpha) * 255.f));
		break;
	case rasterizer::Bitmap::BitmapType_RGB24:
		break;
	case rasterizer::Bitmap::BitmapType_RGBA32:
		*(uint8_t*)(bytes + (y * width + x) * 4 + 3) = (uint8_t)(Mathf::Clamp01(alpha) * 255.f);
		break;
	case rasterizer::Bitmap::BitmapType_AlphaFloat:
		SetPixel_AlphaFloat(x, y, alpha);
		break;
	default:
		break;
	}
}


