#ifndef _BASE_BITMAP_H_
#define _BASE_BITMAP_H_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"

namespace rasterizer
{

class Bitmap;
typedef std::shared_ptr<Bitmap> BitmapPtr;

class Bitmap
{
public:
	enum BitmapType
	{
		BitmapType_Unknown = 0,
		BitmapType_L8 = 1,
		BitmapType_RGB888 = 3,
		BitmapType_RGBA8888 = 4,
		//BitmapType_Normal = 9,
		BitmapType_DXT1 = 10
	};

	static BitmapPtr Create(int width, int height, BitmapType type);

	Bitmap() = default;
	~Bitmap();

	Color GetColor(int x, int y)
	{
		switch (type)
		{
		case BitmapType_L8:
			return GetPixel_L8(x, y);
		case BitmapType_RGB888:
			return GetPixel_RGB888(x, y);
		case BitmapType_RGBA8888:
			return GetPixel_RGBA8888(x, y);
		//case BitmapType_Normal:
		//	return GetPixel_Normal(x, y);
		case BitmapType_DXT1:
			return GetPixel_DXT1(x, y);
		}
		return Color::black;
	}

	rawptr_t GetBytes() { return bytes; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	BitmapType GetType() const { return type; }

	BitmapPtr CompressToDXT1();

protected:
	Color GetPixel_L8(int x, int y);
	Color GetPixel_RGB888(int x, int y);
	Color GetPixel_RGBA8888(int x, int y);
	Color GetPixel_DXT1(int x, int y);

private:
	BitmapType type = BitmapType_Unknown;
	int width = 0;
	int height = 0;

	rawptr_t bytes = nullptr;
};


}

#endif //! _BASE_BITMAP_H_