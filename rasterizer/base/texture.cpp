#include "texture.h"
#include "math/mathf.h"
#include "thirdpart/freeimage/FreeImage.h"

namespace rasterizer
{

void Texture::Initialize()
{
	FreeImage_Initialise();
}

void Texture::Finalize()
{
	FreeImage_DeInitialise();
}

bool Texture::LoadTexture(Texture& texture, const char* file)
{
	FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(file);
	FIBITMAP* fiBitmap = FreeImage_Load(imageFormat, file);
	if (fiBitmap == nullptr) return false;

	texture.width = FreeImage_GetWidth(fiBitmap);
	texture.height = FreeImage_GetHeight(fiBitmap);
	texture.imageHandle = fiBitmap;
	return true;
}

void Texture::UnloadTexture(Texture& texture)
{
	FIBITMAP* fiBitmap = (FIBITMAP*)texture.imageHandle;
	if (fiBitmap == nullptr) return;
	FreeImage_Unload(fiBitmap);
	texture.imageHandle = nullptr;
}

int Texture::GetWidth()
{
	return width;
}

int Texture::GetHeight()
{
	return height;
}

const Color32 Texture::GetColor(int x, int y) const
{
	FIBITMAP* fiBitmap = (FIBITMAP*)imageHandle;
	if (fiBitmap == nullptr) return Color32::black;
	if (x < 0 || x >= width) return Color32::black;
	if (y < 0 || y >= height) return Color32::black;

	RGBQUAD rgbQuad;
	BOOL ret = FreeImage_GetPixelColor(fiBitmap, (u32)x, (u32)y, &rgbQuad);
	if (ret == FALSE) return Color32::black;
	
	Color32 color;
	color.r = rgbQuad.rgbRed;
	color.g = rgbQuad.rgbGreen;
	color.b = rgbQuad.rgbBlue;
	color.a = 255; // ..
	return color;
}

const Color32 Texture::Sample(float u, float v, AddressMode mode) const
{
	assert(imageHandle != nullptr);
	assert(height > 0 && width > 0);

	switch (mode)
	{
	case rasterizer::Texture::Warp:
		u = Mathf::Repeat(u, 1.0f);
		v = Mathf::Repeat(v, 1.0f);
		break;
	case rasterizer::Texture::Clamp:
		u = Mathf::Clamp01(u);
		v = Mathf::Clamp01(v);
		break;
	case rasterizer::Texture::Mirror:
		u = Mathf::PingPong(u, 1.0f);
		v = Mathf::PingPong(v, 1.0f);
		break;
	}

	float fx = u * (width - 1);
	float fy = v * (height - 1);
	int x = Mathf::FloorToInt(fx);
	int y = Mathf::FloorToInt(fy);
	float fpartX = fx - x;
	float fpartY = fx - y;
	int x2 = x + 1;
	int y2 = y + 1;

	switch (mode)
	{
	case rasterizer::Texture::Warp:
		if (x2 >= width) x2 = 0;
		if (y2 >= height) y2 = 0;
		break;
	case rasterizer::Texture::Mirror:
		if (x2 >= width) x2 = width - 1;
		if (y2 >= height) y2 = height - 1;
		break;
	}

	Color32 c0 = GetColor(x, y);
	Color32 c1 = GetColor(x2, y);
	Color32 c2 = GetColor(x, y2);
	Color32 c3 = GetColor(x2, y2);

	return Color32::Lerp(Color32::Lerp(c0, c1, fpartX), Color32::Lerp(c2, c3, fpartX), fpartY);
}

}
