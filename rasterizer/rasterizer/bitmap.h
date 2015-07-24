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
		BitmapType_Alpha8,
		BitmapType_RGB24,
		BitmapType_RGBA32,

		BitmapType_AlphaFloat,
		
		//BitmapType_DXT1,
		//BitmapType_Normal,

	};

	Bitmap(int width, int height, BitmapType type);
	virtual ~Bitmap();

	Color GetPixel(int x, int y) const;
	void SetPixel(int x, int y, const Color& color);
	float GetAlpha(int x, int y) const;
	void SetAlpha(int x, int y, float alpha);

	rawptr_t GetBytes() { return bytes; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	BitmapType GetType() const { return type; }

protected:
	uint8_t GetPixel_Alpha8(int x, int y) const;
	void SetPixel_Alpha8(int x, int y, uint8_t val);
	Color32 GetPixel_RGB24(int x, int y) const;
	void SetPixel_RGB24(int x, int y, Color32 color);
	Color32 GetPixel_RGBA32(int x, int y) const;
	void SetPixel_RGBA32(int x, int y, Color32 color);
	float GetPixel_AlphaFloat(int x, int y) const;
	void SetPixel_AlphaFloat(int x, int y, float val);

private:
	BitmapType type = BitmapType_Unknown;
	int width = 0;
	int height = 0;

	rawptr_t bytes = nullptr;
};


}

#endif //! _BASE_BITMAP_H_