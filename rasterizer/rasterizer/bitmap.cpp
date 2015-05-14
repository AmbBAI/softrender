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
	case rasterizer::Bitmap::BitmapType_8Bit:
		break;
	case rasterizer::Bitmap::BitmapType_RGB:
		size *= 3;
		break;
	case rasterizer::Bitmap::BitmapType_RGBA:
		size *= 4;
		break;
	case rasterizer::Bitmap::BitmapType_DXT1:
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
