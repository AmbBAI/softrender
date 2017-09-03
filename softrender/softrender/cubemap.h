#ifndef _SOFTRENDER_CUBEMAP_H_
#define _SOFTRENDER_CUBEMAP_H_

#include "base/header.h"
#include "softrender/texture2d.h"
#include "math/vector2.h"
#include "math/vector3.h"

namespace sr
{

class Cubemap;
typedef std::shared_ptr<Cubemap> CubemapPtr;

class Cubemap
{
public:
	void InitWith6Images(Texture2DPtr img[6]);
	void InitWithLatlong(Texture2DPtr tex);

	Color Sample(const Vector3& s, float rougness = 0.f) const;

	bool Mapping6ImagesToLatlong(int height, Bitmap::BitmapType type); // weight = height * 2
	bool PrefilterEnvMap(uint32_t mapCount, uint32_t sampleCount) const;

	bool Get6Images(Texture2DPtr img[6]);
	bool GetLagLong(Texture2DPtr& latlong);

protected:
	Color SampleLatlong(const Vector3& s, float lod) const;
	Color Sample6Images(const Vector3& s) const;

	void DirectionToLatlongTexcoord(const Vector3& s, Vector2& texcoord) const;
	void Direction6ImagesTexcoord(const Vector3& s, int& face, Vector2& texcoord) const;
	float RoughnessToLod(float rougness) const;

private:
	enum MappingType
	{
		MappingType_None,
		MappingType_6Images,
		MappingType_LatLong,
	};

	enum CubemapFace
	{
		CubemapFace_PositiveX = 0,
		CubemapFace_NegativeX = 1,
		CubemapFace_PositiveY = 2,
		CubemapFace_NegativeY = 3,
		CubemapFace_PositiveZ = 4,
		CubemapFace_NegativeZ = 5
	};

	MappingType mappingType = MappingType_None;
	Texture2DPtr images[6];
	Texture2DPtr latlong;
};

}

#endif //! _SOFTRENDER_CUBEMAP_H_
