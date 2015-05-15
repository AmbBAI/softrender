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
		BitmapType_DXT1 = 10
	};

	static BitmapPtr Create(u32 width, u32 height, BitmapType type);

	Bitmap() = default;
	~Bitmap();

	Color GetColor(u32 x, u32 y)
	{
		switch (type)
		{
		case BitmapType_L8:
			return GetPixel_L8(x, y);
		case BitmapType_RGB888:
			return GetPixel_RGB888(x, y);
		case BitmapType_RGBA8888:
			return GetPixel_RGBA8888(x, y);
		case BitmapType_DXT1:
			return GetPixel_DXT1(x, y);
		}
		return Color::black;
	}

	u8* GetBytes() { return bytes; }
	u32 GetWidth() const { return width; }
	u32 GetHeight() const { return height; }
	BitmapType GetType() const { return type; }

protected:
	Color GetPixel_L8(u32 x, u32 y);
	Color GetPixel_RGB888(u32 x, u32 y);
	Color GetPixel_RGBA8888(u32 x, u32 y);
	Color GetPixel_DXT1(u32 x, u32 y);

private:
	BitmapType type = BitmapType_Unknown;
	u32 width = 0;
	u32 height = 0;

	u8* bytes = nullptr;
};


}

#endif //! _BASE_BITMAP_H_