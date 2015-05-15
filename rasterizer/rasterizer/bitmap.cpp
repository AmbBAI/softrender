#include "bitmap.h"
using namespace rasterizer;


BitmapPtr Bitmap::Create(u32 width, u32 height, BitmapType type)
{
	BitmapPtr bitmap(new Bitmap());
	bitmap->width = width;
	bitmap->height = height;
	bitmap->type = type;
	if (bitmap->type == BitmapType_Unknown) return bitmap;

	u32 size = width * height;
	switch (type)
	{
	case Bitmap::BitmapType_L8:
		break;
	case Bitmap::BitmapType_RGB888:
		size *= 3;
		break;
	case Bitmap::BitmapType_RGBA8888:
		size = size << 2;
		break;
	case Bitmap::BitmapType_DXT1:
		if (width & 3) return nullptr;
		if (height & 3) return nullptr;
		size = (size >> 4) << 6;
		break;
	}

	bitmap->bytes = new u8[size];
	return bitmap;
}

Bitmap::~Bitmap()
{
	if (bytes != nullptr)
	{
		delete bytes;
		bytes = nullptr;
	}
}

Color Bitmap::GetPixel_L8(u32 x, u32 y)
{
	u32 grey = (u32)*(bytes + (y * width + x));
	return Color32(grey, grey, grey, grey);
}

Color Bitmap::GetPixel_RGB888(u32 x, u32 y)
{
	u8* byte = bytes + (y * width + x) * 3;
	u8 r = *byte;
	u8 g = *(byte + 1);
	u8 b = *(byte + 2);
	return Color32(255, r, g, b);
}

Color Bitmap::GetPixel_RGBA8888(u32 x, u32 y)
{
	u8* byte = bytes + (y * width + x) * 4;
	u8 r = *byte;
	u8 g = *(byte + 1);
	u8 b = *(byte + 2);
	u8 a = *(byte + 3);
	return Color32(a, r, g, b);
}

Color Bitmap::GetPixel_DXT1(u32 x, u32 y)
{
	int xBlock = x >> 2;
	int yBlock = y >> 2;
	int block = yBlock * (width >> 2) + xBlock;
	u8* byte = bytes + (block * 64);

	u32 indexBlock = *((u32*)byte + 1);
	u8 index = indexBlock & (3 << ((((y | 3) << 2) | (x | 3)) << 1));

	u32 colorByte = *(u32*)byte;
	Color c0, c1;
	if (index == 1 || (index & 2))
	{
		c0.a = 1.;
		c0.r = (float)((colorByte >> 27) & 0xf8) / 0xf8;
		c0.g = (float)((colorByte >> 21) & 0xfc) / 0xfc;
		c0.b = (float)((colorByte >> 16) & 0xf8) / 0xf8;
		if (index == 1) return c0;
	}
	if (index == 2 || (index & 2))
	{
		c1.a = 1.;
		c1.r = (float)((colorByte >> 11) & 0xf8) / 0xf8;
		c1.g = (float)((colorByte >> 5) & 0xfc) / 0xfc;
		c1.b = (float)((colorByte >> 0) & 0xf8) / 0xf8;
		if (index == 2) return c1;
	}

	if (index == 3)
	{
		return Color::Lerp(c0, c1, 0.33333333f);
	}
	else
	{
		return Color::Lerp(c0, c1, 0.66666667f);
	}
}
