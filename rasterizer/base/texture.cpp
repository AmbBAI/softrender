#include "texture.h"
#include "math/mathf.h"
#include "thirdpart/freeimage/FreeImage.h"

namespace rasterizer
{

void _stdcall FreeImagePrint(FREE_IMAGE_FORMAT fmt, const char* msg)
{
	puts(msg);
}

void Texture::Initialize()
{
	FreeImage_Initialise();
	FreeImage_SetOutputMessageStdCall(FreeImagePrint);
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
	//FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(fiBitmap);
	//FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
	//u32 pitch = FreeImage_GetPitch(fiBitmap);

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

void Texture::UnparkColor()
{
	FIBITMAP* fiBitmap = (FIBITMAP*)imageHandle;
	if (fiBitmap == nullptr) return;
	colors.assign(width * height, Color());
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			RGBQUAD rgbQuad;
			BOOL ret = FreeImage_GetPixelColor(fiBitmap, (u32)x, (u32)y, &rgbQuad);
			if (ret)
			{
				Color32& color = colors[y * width + x];
				color.r = rgbQuad.rgbRed;
				color.g = rgbQuad.rgbGreen;
				color.b = rgbQuad.rgbBlue;
				color.a = 255; // ..
			}
		}
	}
}

const Color32 Texture::GetColor(int x, int y) const
{
	FIBITMAP* fiBitmap = (FIBITMAP*)imageHandle;
	if (fiBitmap == nullptr) return Color32::black;
	if (x < 0 || x >= width) return Color32::black;
	if (y < 0 || y >= height) return Color32::black;

	return colors[y * width + x];

	//RGBQUAD rgbQuad;
	//BOOL ret = FreeImage_GetPixelColor(fiBitmap, (u32)x, (u32)y, &rgbQuad);
	//if (ret == FALSE) return Color32::black;
	//
	//Color32 color;
	//color.r = rgbQuad.rgbRed;
	//color.g = rgbQuad.rgbGreen;
	//color.b = rgbQuad.rgbBlue;
	//color.a = 255; // ..
	//return color;
}

const Color Texture::Sample(float u, float v) const
{
	assert(imageHandle != nullptr);
	assert(height > 0 && width > 0);

	switch (addressMode)
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

	switch (addressMode)
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

	Color c0 = GetColor(x, y);
	Color c1 = GetColor(x2, y);
	Color c2 = GetColor(x, y2);
	Color c3 = GetColor(x2, y2);

	return Color::Lerp(Color::Lerp(c0, c1, fpartX), Color::Lerp(c2, c3, fpartX), fpartY);
}

void Texture::UnparkBump(float strength/* = 2.0f*/)
{
	FIBITMAP* fiBitmap = (FIBITMAP*)imageHandle;
	if (fiBitmap == nullptr) return;
	colors.assign(width * height, Color());
	BYTE* bytes = FreeImage_GetBits(fiBitmap);

	//for (int y = 0; y < height; ++y)
	//{
	//	for (int x = 0; x < width; ++x)
	//	{
	//		int offset = y * width + x;
	//		u8 gray = bytes[offset];
	//		Color32& color = colors[offset];
	//		color.r = gray;
	//		color.g = gray;
	//		color.b = gray;
	//		color.a = 255; // ..
	//	}
	//}

	normals.assign(width * height, Vector3());
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int offset = y * width + x;
			int x1 = x - 1;
			int x2 = x + 1;
			int y1 = y - 1;
			int y2 = y + 1;
			switch (addressMode)
			{
			case rasterizer::Texture::Warp:
				if (x1 < 0) x1 = width - 1;
				if (x2 >= width) x2 = 0;
				if (y1 < 0) y1 = height - 1;
				if (y2 >= height) y2 = 0;
				break;
			case rasterizer::Texture::Clamp:
			case rasterizer::Texture::Mirror:
				if (x1 < 0) x1 = 0;
				if (x2 >= width) x2 = width - 1;
				if (y1 < 0) y1 = 0;
				if (y2 >= height) y2 = height - 1;
				break;
			default:
				break;
			}

			int px1 = bytes[y * width + x1];
			int px2 = bytes[y * width + x2];
			int py1 = bytes[y1 * width + x];
			int py2 = bytes[y2 * width + x];
			//Vector3 u = Vector3(1.0f, 0.f, (px2 - px1) / 255.f).Normalize();
			//Vector3 v = Vector3(0.0f, 1.0f, (py2 - py1) / 255.f).Normalize();
			Vector3& normal = normals[offset];
			normals[offset] = Vector3((px2 - px1) / 255.f, (py2 - py1) / 255.f, 0.25f / strength).Normalize();

			Color32& color = colors[offset];
			color.r = u8((normal.x + 1.0f) / 2.0 * 255);
			color.g = u8((normal.y + 1.0f) / 2.0 * 255);
			color.b = u8((normal.z + 1.0f) / 2.0 * 255);
			color.a = 255; // ..
		}
	}
}

}
