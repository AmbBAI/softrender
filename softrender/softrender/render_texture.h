#ifndef _SOFTRENDER_RENDER_TEXTURE_H_
#define _SOFTRENDER_RENDER_TEXTURE_H_

#include "base/header.h"
#include "softrender/bitmap.h"
#include "math/color.h"
#include "math/vector2.h"

namespace sr
{

class RenderTexture;
typedef std::shared_ptr<RenderTexture> RenderTexturePtr;

class RenderTexture
{
public:
	RenderTexture(int width, int height);
	RenderTexture(BitmapPtr colorBuffer, BitmapPtr depthBuffer);
	
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	BitmapPtr CreateGBuffer(int index, Bitmap::BitmapType format);
	void SetGBuffer(int index, BitmapPtr bitmap);
	void ClearGBuffer(int index);

	BitmapPtr GetColorBuffer() { return colorBuffer; }
	BitmapPtr GetDepthBuffer() { return depthBuffer; }
	BitmapPtr GetGBuffer(int index);

protected:
	BitmapPtr colorBuffer = nullptr;
	BitmapPtr depthBuffer = nullptr;
	BitmapPtr gbuffer0 = nullptr;
	BitmapPtr gbuffer1 = nullptr;
	BitmapPtr gbuffer2 = nullptr;

	int width = 0;
	int height = 0;
};

}

#endif //! _SOFTRENDER_RENDER_TEXTURE_H_
