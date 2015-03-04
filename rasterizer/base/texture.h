#ifndef _BASE_TEXTURE_H_
#define _BASE_TEXTURE_H_

#include "base/header.h"
#include "base/color.h"

namespace rasterizer
{

struct Texture
{
	static void Initialize();
	static void Finalize();

	static bool LoadTexture(Texture& texture, const char* file);
	static void UnloadTexture(Texture& texture);

	enum WarpMode
	{
		Warp,
		Clamp,
		Mirror,
	};

	int GetWidth();
	int GetHeight();
	const Color32 GetColor(int x, int y) const;
	const Color32 Sample(float u, float v, WarpMode mode) const;

protected:
	int width = 0;
	int height = 0;
	void* imageHandle = nullptr;
};

}

#endif //! _BASE_TEXTURE_H_