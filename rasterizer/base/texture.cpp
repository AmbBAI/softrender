#include "texture.h"
#include "math/mathf.h"
#include "freeimage/FreeImage.h"

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

std::map<std::string, TexturePtr> Texture::texturePool;
TexturePtr Texture::CreateTexture(const char* file)
{
	FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(file);
	FIBITMAP* fiBitmap = FreeImage_Load(imageFormat, file);
	if (fiBitmap == nullptr) return nullptr;

	u32 width = FreeImage_GetWidth(fiBitmap);
	u32 height = FreeImage_GetHeight(fiBitmap);
	u32 pitch = FreeImage_GetPitch(fiBitmap);
	u32 bpp = (FreeImage_GetBPP(fiBitmap) >> 3);
	BYTE* bytes = FreeImage_GetBits(fiBitmap);

	TexturePtr tex(new Texture());
    tex->UnparkColor(bytes, width, height, pitch, bpp);
    
	FreeImage_Unload(fiBitmap);
	return tex;
}

TexturePtr Texture::LoadTexture(const char* file)
{
    std::map<std::string, TexturePtr>::iterator itor;
    itor = texturePool.find(file);
    if (itor != texturePool.end())
    {
        return itor->second;
    }
    else
    {
        TexturePtr tex = CreateTexture(file);
        if (tex != nullptr) texturePool[file] = tex;
        return tex;
    }
}
    
u32 Texture::GetWidth()
{
	return width;
}

u32 Texture::GetHeight()
{
	return height;
}

u32 Texture::GetBPP()
{
	return bpp;
}

bool Texture::UnparkColor(u8* bytes, u32 width, u32 height, u32 pitch, u32 bpp)
{
    if(bytes == nullptr) return false;
    if(width <= 0 || height <= 0) return false;
    if (bpp != 1 && bpp != 3 && bpp != 4) return false;
    
    this->width = width;
    this->height = height;
	this->bpp = bpp;
    this->colors.assign(width * height, Color::black);
    
    u8* line = bytes;
    for (u32 y = 0; y < height; ++y)
    {
        u8* byte = line;
        for (u32 x = 0; x < width; ++x)
        {
            Color& color = this->colors[y * width + x];
            if (bpp == 1)
            {
                color.r = color.g = color.b = byte[0] / 255.f;
                color.a = 1.f;
            }
            else
            {
                color.b = byte[0] / 255.f;
                color.g = byte[1] / 255.f;
                color.r = byte[2] / 255.f;
                color.a = (bpp == 4) ? byte[3] / 255.f : 1.f ;
            }
            byte += bpp;
        }
        line += pitch;
    }
    return true;
}

void Texture::ConvertBumpToNormal(float strength/* = 2.0f*/)
{
    std::vector<float> bump(width * height, 0.f);
    for (int i=0; i<(int)colors.size(); ++i)
    {
        bump[i] = colors[i].b;
    }
    
    for (int y = 0; y < (int)height; ++y)
    {
    	for (int x = 0; x < (int)width; ++x)
        {
        	int offset = y * width + x;
        	int x1 = x - 1;
        	int x2 = x + 1;
        	int y1 = y - 1;
        	int y2 = y + 1;

            if (x1 < 0) x1 = 0;
        	if (x2 >= (int)width) x2 = width - 1;
        	if (y1 < 0) y1 = 0;
        	if (y2 >= (int)height) y2 = height - 1;
            
        	float px1 = bump[y * width + x1];
        	float px2 = bump[y * width + x2];
        	float py1 = bump[y1 * width + x];
        	float py2 = bump[y2 * width + x];
        	Vector3 normal = Vector3(px1 - px2, py1 - py2, 0.25f / strength).Normalize();
        
            Color& color = colors[offset];
        	color.r = (normal.x + 1.0f) / 2.0f;
        	color.g = (normal.y + 1.0f) / 2.0f;
        	color.b = (normal.z + 1.0f) / 2.0f;
        	color.a = 1.f;
        }
    }
}
    
const Color Texture::GetColor(int x, int y) const
{
	if (x < 0 || x >= (int)width) return Color::black;
	if (y < 0 || y >= (int)height) return Color::black;

	return colors[y * width + x];
}

const Color Texture::Sample(float u, float v) const
{
    return LinearSample(u, v);
}
    
const Color Texture::LinearSample(float u, float v) const
{
	assert(height > 0 && width > 0);

	switch (addressMode)
	{
	case Texture::AddressMode_Warp:
		if (u < 0.f || u >= 1.f) u = Mathf::Repeat(u, 1.0f);
		if (v < 0.f || v >= 1.f) v = Mathf::Repeat(v, 1.0f);
		break;
	case Texture::AddressMode_Clamp:
		u = Mathf::Clamp01(u);
		v = Mathf::Clamp01(v);
		break;
	case Texture::AddressMode_Mirror:
		if (u < 0.f || u > 1.f) u = Mathf::PingPong(u, 1.0f);
		if (v < 0.f || v > 1.f) v = Mathf::PingPong(v, 1.0f);
		break;
	}

	float fx = u * (width - 1);
	float fy = v * (height - 1);
	int x = Mathf::FloorToInt(fx);
	int y = Mathf::FloorToInt(fy);
	float fpartX = fx - x;
	float fpartY = fy - y;
	int x2 = x + 1;
	int y2 = y + 1;

	switch (addressMode)
	{
	case Texture::AddressMode_Warp:
		if (x2 >= (int)width) x2 = 0;
		if (y2 >= (int)height) y2 = 0;
		break;
    case Texture::AddressMode_Clamp:
        break;
    case Texture::AddressMode_Mirror:
		if (x2 >= (int)width) x2 = width - 1;
		if (y2 >= (int)height) y2 = height - 1;
		break;
	}

	Color c0 = GetColor(x, y);
	Color c2 = GetColor(x, y2);
	Color c1 = GetColor(x2, y);
	Color c3 = GetColor(x2, y2);

	return Color::Lerp(Color::Lerp(c0, c1, fpartX), Color::Lerp(c2, c3, fpartX), fpartY);
}

const Color Texture::PointSample(float u, float v) const
{
    assert(height > 0 && width > 0);
    
    switch (addressMode)
    {
        case Texture::AddressMode_Warp:
            if (u < 0.f || u >= 1.f) u = Mathf::Repeat(u, 1.0f);
            if (v < 0.f || v >= 1.f) v = Mathf::Repeat(v, 1.0f);
            break;
        case Texture::AddressMode_Clamp:
            u = Mathf::Clamp01(u);
            v = Mathf::Clamp01(v);
            break;
        case Texture::AddressMode_Mirror:
            if (u < 0.f || u > 1.f) u = Mathf::PingPong(u, 1.0f);
            if (v < 0.f || v > 1.f) v = Mathf::PingPong(v, 1.0f);
            break;
    }
        
    float fx = u * (width - 1);
    float fy = v * (height - 1);
    int x = Mathf::FloorToInt(fx);
    int y = Mathf::FloorToInt(fy);
        
    return GetColor(x, y);
}

}
