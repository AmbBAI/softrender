#include "texture.h"
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

}
