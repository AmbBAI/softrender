#ifndef _BASE_TEXTURE_H_
#define _BASE_TEXTURE_H_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Texture
{
	static void Initialize();
	static void Finalize();

	static bool LoadTexture(Texture& texture, const char* file);
	static void UnloadTexture(Texture& texture);

	enum AddressMode
	{
		Warp,
		Clamp,
		Mirror,
	};

	int GetWidth();
	int GetHeight();

	void UnparkColor();
	void UnparkBump(float strength = 2.0f);

	const Color32 GetColor(int x, int y) const;
	const Color Sample(float u, float v) const;

	//const u8 GetHeight(int x, int y) const;
	//const Vector3 SampleNormal(float u, float v, AddressMode mode) const;


	AddressMode addressMode = Warp;

protected:
	int width = 0;
	int height = 0;
	void* imageHandle = nullptr;
	std::vector<Color32> colors;
	std::vector<Vector3> normals;
};

}

#endif //! _BASE_TEXTURE_H_