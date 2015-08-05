#ifndef _RASTERIZER_RENDER_TEXTURE_H_
#define _RASTERIZER_RENDER_TEXTURE_H_

#include "base/header.h"
#include "rasterizer/bitmap.h"
#include "math/color.h"
#include "math/vector2.h"

namespace rasterizer
{

class RenderTexture;
typedef std::shared_ptr<RenderTexture> RenderTexturePtr;

class RenderTexture
{
public:
	RenderTexture(int width, int height);
	
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	BitmapPtr GetColorBuffer() { return colorBuffer; }
	BitmapPtr GetDepthBuffer() { return depthBuffer; }

protected:
	BitmapPtr colorBuffer = nullptr;
	BitmapPtr depthBuffer = nullptr;
	int width = 0;
	int height = 0;
};

}

#endif //! _RASTERIZER_RENDER_TEXTURE_H_