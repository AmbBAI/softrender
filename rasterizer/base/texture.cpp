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
    this->colors.assign(width * height, Color32::black);
    
    u8* line = bytes;
    for (u32 y = 0; y < height; ++y)
    {
        u8* byte = line;
        for (u32 x = 0; x < width; ++x)
        {
            Color32& color = this->colors[y * width + x];
            if (bpp == 1)
            {
				color.a = color.r = color.g = color.b = byte[0];
            }
            else
            {
                color.b = byte[0];
                color.g = byte[1];
                color.r = byte[2];
                color.a = (bpp == 4) ? byte[3] : 255 ;
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
            
            float ph = bump[offset];
        	float px1 = bump[y * width + x1];
        	float px2 = bump[y * width + x2];
        	float py1 = bump[y1 * width + x];
        	float py2 = bump[y2 * width + x];
            Vector3 xOff = Vector3(strength, 0.f, px1 - px2).Normalize();
            Vector3 yOff = Vector3(0.f, strength, py1 - py2).Normalize();
        	Vector3 normal = xOff.Cross(yOff);
        
            Color color;
        	color.r = (normal.x + 1.0f) / 2.0f;
        	color.g = (normal.y + 1.0f) / 2.0f;
        	color.b = (normal.z + 1.0f) / 2.0f;
        	color.a = ph;
			colors[offset] = color;
        }
    }
}
    
const Color Texture::GetColor(u32 x, u32 y) const
{
    assert(x < width);
    assert(y < height);

    u32 offset = y * width + x;
    assert(offset < (int)colors.size());
    
	return Color32(colors[offset]);
}

const Color Texture::Sample(float u, float v) const
{
    switch (filterMode) {
        case FilterMode_Point:
            return PointSample(u, v);
        case FilterMode_Bilinear:
            //TODO bilinear with mipmap
            //if (isMipmapCreated) ; else
            return LinearSample(u, v);
        case FilterMode_Trilinear:
            //TODO trilinear
            return LinearSample(u, v);
        default:
            return LinearSample(u, v);
    }
    
	return PointSample(u, v);
}
    
const Color Texture::LinearSample(float u, float v) const
{
	assert(height > 0 && width > 0);

    int x = -1, y = -1, x2 = -1, y2 = -1;
    float xFrac = 0.f, yFrac;
    switch (addressMode)
    {
            // to template interface
        case Texture::AddressMode_Warp:
        {
            float fx = WarpTexcoord(u, width);
            x = Mathf::FloorToInt(fx);
            float fy = WarpTexcoord(v, height);
            y = Mathf::FloorToInt(fy);
            xFrac = fx - x;
            yFrac = fy - y;
            x = WarpTexcoordFix(x, width);
            y = WarpTexcoordFix(y, height);
            x2 = WarpTexcoordFix(x + 1, width);
            y2 = WarpTexcoordFix(y + 1, height);
        }
            break;
        case Texture::AddressMode_Clamp:
        {
            float fx = ClampTexcoord(u, width);
            x = Mathf::FloorToInt(fx);
            float fy = ClampTexcoord(v, height);
            y = Mathf::FloorToInt(fy);
            xFrac = fx - x;
            yFrac = fy - y;
            x = ClampTexcoordFix(x, width);
            y = ClampTexcoordFix(y, height);
            x2 = ClampTexcoordFix(x + 1, width);
            y2 = ClampTexcoordFix(y + 1, height);
        }
            break;
        case Texture::AddressMode_Mirror:
        {
            float fx = MirrorTexcoord(u, width);
            x = Mathf::FloorToInt(fx);
            float fy = MirrorTexcoord(v, height);
            y = Mathf::FloorToInt(fy);
            xFrac = fx - x;
            yFrac = fy - y;
            x = MirrorTexcoordFix(x, width);
            y = MirrorTexcoordFix(y, height);
            x2 = MirrorTexcoordFix(x + 1, width);
            y2 = MirrorTexcoordFix(y + 1, height);
        }
            break;
    }

	Color c0 = GetColor(x, y);
	Color c1 = GetColor(x2, y);
    Color c2 = GetColor(x, y2);
    Color c3 = GetColor(x2, y2);

	return Color::Lerp(Color::Lerp(c0, c1, xFrac), Color::Lerp(c2, c3, xFrac), yFrac);
}

const Color Texture::PointSample(float u, float v) const
{
    assert(height > 0 && width > 0);
    
    int x = -1;
    int y = -1;
    switch (addressMode)
    {
        case Texture::AddressMode_Warp:
        {
            float fx = WarpTexcoord(u, width);
            x = WarpTexcoordFix(Mathf::RoundToInt(fx), width);
            float fy = WarpTexcoord(v, height);
            y = WarpTexcoordFix(Mathf::RoundToInt(fy), height);
        }
            break;
        case Texture::AddressMode_Clamp:
        {
            float fx = ClampTexcoord(u, width);
            x = ClampTexcoordFix(Mathf::RoundToInt(fx), width);
            float fy = ClampTexcoord(v, height);
            y = ClampTexcoordFix(Mathf::RoundToInt(fy), height);
        }
            break;
        case Texture::AddressMode_Mirror:
        {
            float fx = MirrorTexcoord(u, width);
            x = MirrorTexcoordFix(Mathf::RoundToInt(fx), width);
            float fy = MirrorTexcoord(v, height);
            y = MirrorTexcoordFix(Mathf::RoundToInt(fy), height);
        }
            break;
    }

    return GetColor(x, y);
}

bool Texture::GenerateMipmaps()
{
	if (width != height) return false;
	if (!Mathf::IsPowerOfTwo(width)) return false;
	if (colors.size() != width * height) return false;

	mipmaps.clear();

	Bitmap* source = &colors;
	u32 s = (width >> 1);
	for (int l = 0;; ++l)
	{
		Bitmap mipmap(s * s, Color32::black);
		for (u32 y = 0; y < s; ++y)
		{
			u32 y0 = y * 2;
			u32 y1 = y0 + 1;
			for (u32 x = 0; x < s; ++x)
			{
				u32 x0 = x * 2;
				u32 x1 = x0 + 1;
				Color c0 = (*source)[y0 * s + x0];
				Color c1 = (*source)[y0 * s + x1];
				Color c2 = (*source)[y1 * s + x0];
				Color c3 = (*source)[y1 * s + x1];

				mipmap[y * s + x] = Color::Lerp(
					Color::Lerp(c0, c1, 0.5f),
					Color::Lerp(c2, c3, 0.5f), 0.5f);
			}
		}
		
		mipmaps.push_back(mipmap);
		source = &mipmaps[l];
		s >>= 1;
		if (s <= 0) break;
	}
	return true;
}

float Texture::WarpTexcoord(float coord, u32 length)
{
    return (coord - Mathf::Floor(coord)) * length - 0.5f;
}

float Texture::MirrorTexcoord(float coord, u32 length)
{
    int round = Mathf::FloorToInt(coord);
    float tmpCoord = (round & 1) ? (1 + round - coord) : (coord - round);
    return tmpCoord * length - 0.5f;
}

float Texture::ClampTexcoord(float coord, u32 length)
{
    return Mathf::Clamp(coord * length, 0.5f, length - 0.5f) - 0.5f;
}

int Texture::WarpTexcoordFix(int coord, u32 length)
{
    if (coord < 0) return length - 1;
    if (coord >= length) return 0;
    return coord;
}

int Texture::MirrorTexcoordFix(int coord, u32 length)
{
    if (coord < 0) return 0;
    if (coord >= length) return length - 1;
    return coord;
}
    
int Texture::ClampTexcoordFix(int coord, u32 length)
{
    if (coord < 0) return 0;
    if (coord >= length) return length - 1;
    return coord;
}
    
}
