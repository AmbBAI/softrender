#ifndef _SOFTRENDER_STENCIL_H_
#define _SOFTRENDER_STENCIL_H_

#include "base/header.h"
#include "bitmap.h"

namespace sr
{

class StencilBuffer;
typedef std::shared_ptr<StencilBuffer> StencilBufferPtr;

class StencilBuffer : protected Bitmap
{
public:
	StencilBuffer(int width, int height) : Bitmap(width, height, Bitmap::BitmapType_Alpha8)
	{
	}

	void Fill(uint8_t stencil)
	{
		assert(bytes != nullptr);
		std::memset(bytes, stencil, width * height);
	}

	uint8_t GetStencil(int x, int y) const
	{
		return GetPixel_Alpha8(x, y);
	}

	void SetStencil(int x, int y, uint8_t stencil)
	{
		SetPixel_Alpha8(x, y, stencil);
	}

	bool SaveToFile(const char* file) { return Bitmap::SaveToFile(file); }
	bool SaveToFile(const std::string& file) { return Bitmap::SaveToFile(file); }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	rawptr_t GetBytes() { return bytes; }
};


} // namespace sr

#endif // !_SOFTRENDER_STENCIL_H_
