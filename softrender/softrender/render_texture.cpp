#include "render_texture.h"
#include "math/mathf.h"
#include "freeimage/FreeImage.h"
using namespace sr;


RenderTexture::RenderTexture(int width, int height)
{
	this->width = width;
	this->height = height;
	colorBuffer = std::make_shared<Bitmap>(width, height, Bitmap::BitmapType_RGBA32);
	depthBuffer = std::make_shared<Bitmap>(width, height, Bitmap::BitmapType_AlphaFloat);
	assert(colorBuffer != nullptr);
	assert(depthBuffer != nullptr);
}

sr::RenderTexture::RenderTexture(BitmapPtr colorBuffer, BitmapPtr depthBuffer)
{
	assert(colorBuffer != nullptr);
	assert(depthBuffer != nullptr);
	this->width = colorBuffer->GetWidth();
	this->height = colorBuffer->GetHeight();
	assert(this->width == depthBuffer->GetWidth());
	assert(this->height == depthBuffer->GetHeight());
	this->colorBuffer = colorBuffer;
	this->depthBuffer = depthBuffer;
}

BitmapPtr RenderTexture::CreateGBuffer(int index, Bitmap::BitmapType format)
{
	BitmapPtr bitmap = std::make_shared<Bitmap>(width, height, format);
	SetGBuffer(index, bitmap);
	return bitmap;
}

void RenderTexture::SetGBuffer(int index, BitmapPtr bitmap)
{
	assert(bitmap == nullptr || bitmap->GetWidth() == width);
	assert(bitmap == nullptr || bitmap->GetHeight() == height);

	switch (index)
	{
	case 0:
		gbuffer0 = bitmap;
		break;
	case 1:
		gbuffer1 = bitmap;
		break;
	case 2:
		gbuffer2 = bitmap;
		break;
	default:
		throw std::out_of_range("g-buffer index out of range");
	}
}

void RenderTexture::ClearGBuffer(int index)
{
	SetGBuffer(index, nullptr);
}

BitmapPtr RenderTexture::GetGBuffer(int index)
{
	switch (index)
	{
	case 0:
		return gbuffer0;
	case 1:
		return gbuffer1;
	case 2:
		return gbuffer2;
	default:
		throw std::out_of_range("g-buffer index out of range");
	}
}
