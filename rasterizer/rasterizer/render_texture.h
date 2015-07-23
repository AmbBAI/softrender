#ifndef _RASTERIZER_RENDER_TEXTURE_H_
#define _RASTERIZER_RENDER_TEXTURE_H_

#include "base/header.h"
#include "rasterizer/bitmap.h"
#include "base/color.h"
#include "math/vector2.h"

namespace rasterizer
{

class RenderTexture;
typedef std::shared_ptr<RenderTexture> RenderTexturePtr;

class RenderTexture
{
public:
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

protected:
	RenderTexture() = default;
	~RenderTexture() = default;

protected:
	BitmapPtr colorBuffer;
	BitmapPtr depthBuffer;
	int width;
	int height;
};

}

#endif //! _RASTERIZER_RENDER_TEXTURE_H_