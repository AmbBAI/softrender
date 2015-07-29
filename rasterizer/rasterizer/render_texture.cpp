#include "render_texture.h"
#include "math/mathf.h"
#include "freeimage/FreeImage.h"
using namespace rasterizer;


RenderTexture::RenderTexture(int width, int height)
{
	this->width = width;
	this->height = height;
	colorBuffer = std::make_shared<Bitmap>(width, height, Bitmap::BitmapType_RGBA32);
	depthBuffer = std::make_shared<Bitmap>(width, height, Bitmap::BitmapType_AlphaFloat);
	assert(colorBuffer != nullptr);
	assert(depthBuffer != nullptr);
}
